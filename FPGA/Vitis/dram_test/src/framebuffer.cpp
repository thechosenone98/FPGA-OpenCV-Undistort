
#include <stdlib.h>
#include <iostream>

#include "xstatus.h"
#include "xscugic.h"
#include "xparameters.h"

#include "xv_frmbufrd_l2.h"
#include "xv_frmbufwr_l2.h"

#include "xv_frmbufrd.h"
#include "xv_frmbufwr.h"

#include "framebuffer.h"

#include "timers.h"

#define THEORETICAL_PROFILING

static XVidC_VideoStream VideoStream;

XV_frmbufrd frmbufrd_instance;
XV_frmbufwr frmbufwr_instance;

bool transfer_complete = false;
volatile int num_image_to_read = 0;
volatile int num_image_to_write = 0;
int image_size = 0;
volatile int start_address_read = 0;
volatile int start_address_write = 0;

//Profiling variables
volatile int num_frames = 0;
volatile u64 start_rollover_counter[1000];
volatile u64 end_rollover_counter[1000];
volatile u16 start[1000];
volatile u16 end[1000];
bool stop_timer = false;
volatile int frame_counter;

int initialize_framebuffers(uint32_t axi_data_width,
                            uint32_t video_in_mem_addr,
                            uint32_t video_out_mem_addr,
                            uint32_t image_width,
                            uint32_t image_height,
                            uint32_t max_data_width,
                            uint32_t video_format,
                            XVidC_PixelsPerClock pixel_per_clock,
                            XVidC_ColorFormat Cfmt)
{
    int Status;

    // Initialize the frame buffer read IP core
    u16 DeviceId = XPAR_XV_FRMBUFRD_0_DEVICE_ID;
    XV_frmbufrd_Config *config_read = XV_frmbufrd_LookupConfig(DeviceId);
    if (config_read == nullptr)
    {
        std::cerr << "Failed to find the configuration for the device Framebuffer Read." << std::endl;
        return -1;
    }

    if (XV_frmbufrd_CfgInitialize(&frmbufrd_instance, config_read, config_read->BaseAddress) != XST_SUCCESS)
    {
        std::cerr << "Failed to initialize the frame buffer read IP core." << std::endl;
        return -1;
    }

    u32 width = image_width;
    u32 height = image_height;
    u32 stride = CalcStride(Cfmt, axi_data_width, image_width);
    start_address_read = video_in_mem_addr;

    XV_frmbufrd_Set_HwReg_width(&frmbufrd_instance, width);
    XV_frmbufrd_Set_HwReg_height(&frmbufrd_instance, height);
    XV_frmbufrd_Set_HwReg_stride(&frmbufrd_instance, stride);
    XV_frmbufrd_Set_HwReg_video_format(&frmbufrd_instance, video_format);
    XV_frmbufrd_Set_HwReg_frm_buffer_V(&frmbufrd_instance, start_address_read);

    // Do the same for the frame buffer write IP core
    DeviceId = XPAR_XV_FRMBUFWR_0_DEVICE_ID;
    XV_frmbufwr_Config *config_write = XV_frmbufwr_LookupConfig(DeviceId);
    if (config_write == nullptr)
    {
        std::cerr << "Failed to find the configuration for the device Framebuffer Write." << std::endl;
        return -1;
    }

    if (XV_frmbufwr_CfgInitialize(&frmbufwr_instance, config_write, config_write->BaseAddress) != XST_SUCCESS)
    {
        std::cerr << "Failed to initialize the frame buffer write IP core." << std::endl;
        return -1;
    }

    width = image_width;
    height = image_height;
    stride = CalcStride(Cfmt, axi_data_width, image_width);
    start_address_write = video_out_mem_addr;

    XV_frmbufwr_Set_HwReg_width(&frmbufwr_instance, width);
    XV_frmbufwr_Set_HwReg_height(&frmbufwr_instance, height);
    XV_frmbufwr_Set_HwReg_stride(&frmbufwr_instance, stride);
    XV_frmbufwr_Set_HwReg_video_format(&frmbufwr_instance, video_format);
    XV_frmbufwr_Set_HwReg_frm_buffer_V(&frmbufwr_instance, start_address_write);

    return XST_SUCCESS;
}

void start_image_transfer(int nb_bytes_in_image, int num_image, uint32_t video_in_mem_addr, uint32_t video_out_mem_addr)
{
    image_size = nb_bytes_in_image;
    transfer_complete = false;
    num_image_to_read = num_image;
    num_image_to_write = num_image;
    start_address_read = video_in_mem_addr;
    start_address_write = video_out_mem_addr;

    // For profiling we count the frame in the batch (also serves as a check that all frames have been processed)
    num_frames = 0;

    // Reset the start address of the frame buffer read/write IP core
    XV_frmbufrd_Set_HwReg_frm_buffer_V(&frmbufrd_instance, start_address_read);
    XV_frmbufwr_Set_HwReg_frm_buffer_V(&frmbufwr_instance, start_address_write);

    // Start the frame buffer read IP core
    XV_frmbufrd_Start(&frmbufrd_instance);

    // Start the frame buffer write IP core
    XV_frmbufwr_Start(&frmbufwr_instance);
}

void wait_for_transfer_complete()
{
    // Wait until all the frames have been processed
    while (!transfer_complete);
}

// FrameBuffer Write interrupt handler
void FrameBufferWriteInterruptHandler(void *CallbackRef)
{
    num_image_to_write--;
    if (num_image_to_write == 0)
    {
        transfer_complete = true;
    } else {
        // Restart itself with next address since there is still an image to write
        start_address_write += image_size;
        XV_frmbufwr_Set_HwReg_frm_buffer_V(&frmbufwr_instance, start_address_write);
        XV_frmbufwr_Start(&frmbufwr_instance);
    }

    // Clear the interrupt
    XV_frmbufwr_InterruptClear(&frmbufwr_instance, XVFRMBUFWR_IRQ_DONE_MASK);
}

// Can_Write interrupt handler
void CanWriteInterruptHandler(void *InstancePtr)
{
    // Profiling (This way of measuring time works for batched images, but not for single images since the CanWrite signal is off for the first image)
    #ifndef THEORETICAL_PROFILING
    if (!stop_timer){
        measure_time_start();
        stop_timer = true;
    } else {
        measure_time_stop();
        get_timer_data(&(start_rollover_counter[num_frames]), &(end_rollover_counter[num_frames]), &(start[num_frames]), &(end[num_frames]));
        num_frames += 1;
        stop_timer = false;
    }
    frame_counter++;
    #endif

    // If there are still image in the batch, keep going
    if (num_image_to_read > 0){
        num_image_to_read--;
    }

    if (num_image_to_read > 0)
    {
        // Update the reading address of the frame buffer read IP core
        start_address_read += image_size;
        XV_frmbufrd_Set_HwReg_frm_buffer_V(&frmbufrd_instance, start_address_read);

        // Wait for the frame buffer read IP core to be done (it should be done already)
        while(!XV_frmbufrd_IsDone(&frmbufrd_instance));

        // Start the frame buffer read IP core
        XV_frmbufrd_Start(&frmbufrd_instance);
    }
}

#ifdef THEORETICAL_PROFILING
void FrameStartInterruptHandler(void *InstancePtr){
    // Start the timer for profiling
    measure_time_start();
    frame_counter++;
}


void FrameEndInterruptHandler(void *InstancePtr){
    // Stop the timer for profiling
    measure_time_stop();
    get_timer_data(&(start_rollover_counter[num_frames]), &(end_rollover_counter[num_frames]), &(start[num_frames]), &(end[num_frames]));
    num_frames += 1;
}
#endif

int enable_framebuffer_interrupts(XScuGic *IntcPtr)
{
    int Status;


    // Connect the FrameBuffer Write interrupt handler that will be called when an interrupt occurs.
    Status = XScuGic_Connect(IntcPtr, XPAR_FABRIC_V_FRMBUF_WR_0_INTERRUPT_INTR, (Xil_InterruptHandler)FrameBufferWriteInterruptHandler, &frmbufwr_instance);
    if (Status != XST_SUCCESS)
    {
        return Status;
    }

    Status = XScuGic_Connect(IntcPtr, XPAR_FABRIC_AXI_IMAGE_DISSECTOR_0_INTERRUPT_INTR, (Xil_InterruptHandler)CanWriteInterruptHandler, 0);
    // Set the priority of this interrupt high and trigger it on rising edge and falling edge of the signal
    XScuGic_SetPriorityTriggerType(IntcPtr, XPAR_FABRIC_AXI_IMAGE_DISSECTOR_0_INTERRUPT_INTR, 0xA8, 0x3);

    #ifdef THEORETICAL_PROFILING
    Status = XScuGic_Connect(IntcPtr, XPAR_FABRIC_CONTROL_FSM_PIPELINED_0_START_BRAM_WRITER_IRQ_INTR, (Xil_InterruptHandler)FrameStartInterruptHandler, 0);
    // Set the priority of this interrupt high and trigger it on rising edge and falling edge of the signal
    XScuGic_SetPriorityTriggerType(IntcPtr, XPAR_FABRIC_CONTROL_FSM_PIPELINED_0_START_BRAM_WRITER_IRQ_INTR, 0xB0, 0x3);

    Status = XScuGic_Connect(IntcPtr, XPAR_FABRIC_BRAM_WRITER_AND_NORM_0_DONE_IRQ_INTR, (Xil_InterruptHandler)FrameEndInterruptHandler, 0);
    // Set the priority of this interrupt high and trigger it on rising edge and falling edge of the signal
    XScuGic_SetPriorityTriggerType(IntcPtr, XPAR_FABRIC_BRAM_WRITER_AND_NORM_0_DONE_IRQ_INTR, 0xB0, 0x3);
    #endif

    // Enable all interrupts (wrongly explained in the doc, this line does not completely enables interrupts, it just activates them and then you still need to select which one you want to trigger)
    // You can verify this by commenting out the line below and testing the code with either IRQ_DONE_MASK or IRQ_READY_MASK and then running the program. You will see that the interrupts are not triggered.
    XV_frmbufwr_InterruptGlobalEnable(&frmbufwr_instance);

    // Enable the "done" interrupt for the FrameBuffer Write IP core
    XV_frmbufwr_InterruptEnable(&frmbufwr_instance, XVFRMBUFWR_IRQ_DONE_MASK);

    // Enable the interrupts
    XScuGic_Enable(IntcPtr, XPAR_FABRIC_V_FRMBUF_WR_0_INTERRUPT_INTR);
    XScuGic_Enable(IntcPtr, XPAR_FABRIC_AXI_IMAGE_DISSECTOR_0_INTERRUPT_INTR);
    #ifdef THEORETICAL_PROFILING
    XScuGic_Enable(IntcPtr, XPAR_FABRIC_CONTROL_FSM_PIPELINED_0_START_BRAM_WRITER_IRQ_INTR);
    XScuGic_Enable(IntcPtr, XPAR_FABRIC_BRAM_WRITER_AND_NORM_0_DONE_IRQ_INTR);
    #endif

    return XST_SUCCESS;
}

static u32 CalcStride(XVidC_ColorFormat Cfmt,
                      u16 AXIMMDataWidth,
                      u32 width)
{
    u32 stride;
    u16 MMWidthBytes = AXIMMDataWidth / 8;
    u8 bpp_numerator;
    u8 bpp_denominator = 1;

    switch (Cfmt)
    {
    case XVIDC_CSF_MEM_Y_UV10:
    case XVIDC_CSF_MEM_Y_UV10_420:
    case XVIDC_CSF_MEM_Y10:
        /* 4 bytes per 3 pixels (Y_UV10, Y_UV10_420, Y10) */
        bpp_numerator = 4;
        bpp_denominator = 3;
        break;
    case XVIDC_CSF_MEM_Y_UV8:
    case XVIDC_CSF_MEM_Y_UV8_420:
    case XVIDC_CSF_MEM_Y8:
    case XVIDC_CSF_MEM_Y_U_V8:
    case XVIDC_CSF_MEM_Y_U_V10:
        /* 1 byte per pixel (Y_UV8, Y_UV8_420, Y8, Y_U_V10) */
        bpp_numerator = 1;
        break;
    case XVIDC_CSF_MEM_RGB8:
    case XVIDC_CSF_MEM_YUV8:
    case XVIDC_CSF_MEM_BGR8:
        /* 3 bytes per pixel (RGB8, YUV8, BGR8) */
        bpp_numerator = 3;
        break;
    case XVIDC_CSF_MEM_RGBX12:
    case XVIDC_CSF_MEM_YUVX12:
        /* 5 bytes per pixel (RGBX12, YUVX12) */
        bpp_numerator = 5;
        break;
    case XVIDC_CSF_MEM_Y_UV12:
    case XVIDC_CSF_MEM_Y_UV12_420:
    case XVIDC_CSF_MEM_Y12:
        /* 3 bytes per 2 pixels (Y_UV12, Y_UV12_420, Y12) */
        bpp_numerator = 3;
        bpp_denominator = 2;
        break;
    case XVIDC_CSF_MEM_RGB16:
    case XVIDC_CSF_MEM_YUV16:
        /* 6 bytes per pixel (RGB16, YUV16) */
        bpp_numerator = 6;
        break;
    case XVIDC_CSF_MEM_YUYV8:
    case XVIDC_CSF_MEM_UYVY8:
    case XVIDC_CSF_MEM_Y_UV16:
    case XVIDC_CSF_MEM_Y_UV16_420:
    case XVIDC_CSF_MEM_Y16:
        /* 2 bytes per pixel (YUYV8, UYVY8, Y_UV16, Y_UV16_420, Y16) */
        bpp_numerator = 2;
        break;
    default:
        /* 4 bytes per pixel */
        bpp_numerator = 4;
    }
    stride = ((((width * bpp_numerator) / bpp_denominator) +
               MMWidthBytes - 1) /
              MMWidthBytes) *
             MMWidthBytes;

    return (stride);
}

double get_average_framerate()
{
    double total_elapsed_time = 0;
    double elapsed_time = 0;

    for (int i = 0; i < num_frames; i++)
    {
        elapsed_time = compute_elapsed_from_timer_data(start_rollover_counter[i], end_rollover_counter[i], start[i], end[i], TIMER_PRESCALER);
        total_elapsed_time += elapsed_time;
    }
    std::cout << "Total frames: " << frame_counter << std::endl;
    if(total_elapsed_time <= 0.0)
    	return 0;

    return (double)1.0/(total_elapsed_time / (double)num_frames);
}

void reset_frame_counter(){
    frame_counter = 0;
}

int get_frame_counter(){
    return frame_counter;
}
