// Main C++ entry program

#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include <cmath>
// #include <unistd.h>
#include "xusbps_comms.h"
#include "xusbps_ringbuffer.h"
#include "xscugic.h"
#include "xusbps.h"
#include "xusbps_ch9.h"
#include "xusbps_ch9_storage.h"
#include "xil_cache.h"

#include "sleep.h"


#include "xv_frmbufrd_l2.h"
#include "xv_frmbufwr_l2.h"

#include "framebuffer.h"
#include "timers.h"

#include "xundistort_coordinate_generator.h"

#define ALIGNMENT_CACHELINE  __attribute__ ((aligned (32)))

// Define Image related constants
// Don't use more than 220 MB to be on the safe side (224 would be the absolute max limit in this scenario)
#define VIDEO_IN_MEM_ADDR 0x02000000 // 32 MB from the start of the DRAM
#define VIDEO_OUT_MEM_ADDR 0x10000000 // 256 MB from the start of the DRAM
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240
#define NUMBER_OF_IMAGES 3000

// FPGA Framebuffer controllers
int transferred_images_counter = 0;


#define RINGBUFFER_SIZE 262144

// Define USB signaling constants
bool ACK_FLAG = false;

bool SEND_IMAGES_FLAG = false;
int NUMBER_OF_IMAGES_TO_SEND = 0;

bool RECEIVE_IMAGES_FLAG = false;
int NUMBER_OF_IMAGES_TO_RECEIVE = 0;

bool RECEIVE_PARAMETERS_FLAG = false;

bool TRANSFER_IMAGES_FLAG = false;

bool PRINT_FRAMERATE_FLAG = false;
u8 buffer[8];

// Interrupt definition
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

static int SetupInterruptSystem(u16 IntcDeviceID, XScuGic *IntcInstancePtr)
{
	int Status;
	XScuGic_Config *IntcConfig; /* The configuration parameters of the
					interrupt controller */

	/*
	 * Initialize the interrupt controller driver
	 */
	IntcConfig = XScuGic_LookupConfig(IntcDeviceID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			IntcInstancePtr);

	/*
	 * Enable interrupts in the ARM
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

// Define Image class
class Image {
public:
    static constexpr int kWidth = 320;
    static constexpr int kHeight = 240;
    static constexpr int kNumPixels = kWidth * kHeight;

    Image() = default;

    void copyBuffer(const u8* buffer) {
        std::copy(buffer, buffer + kNumPixels, pixels_.begin());
    }

    void setPixel(int x, int y, uint8_t value) {
        pixels_[y * kWidth + x] = value;
    }

    uint8_t getPixel(int x, int y) const {
        return pixels_[y * kWidth + x];
    }

    const u8* getBuffer() const {
        return pixels_.data();
    }

    u8* getBufferNotConst() {
        return pixels_.data();
    }

private:
    std::array<uint8_t, kNumPixels> pixels_;
};

template<class T, std::size_t N, std::size_t nb_bytes> class static_vector
{
    // properly aligned uninitialized storage for N T's
    // TODO: check alignof later on
    std::aligned_storage_t<sizeof(T), alignof(T)> data[N];
    std::size_t m_size = 0;
 
public:
    // Create an object in aligned storage
    template<typename ...Args> void emplace_back(Args&&... args) 
    {
        if( m_size >= N ) // possible error handling
            throw std::bad_alloc{};
 
        // construct value in memory of aligned storage
        // using inplace operator new
        ::new(&data[m_size]) T(std::forward<Args>(args)...);
        ++m_size;
    }
 
    // Access an object in aligned storage
    T& operator[](std::size_t pos) 
    {
        // Note: std::launder is needed after the change of object model in P0137R1
        return *std::launder(reinterpret_cast<T*>(&data[pos]));
    }
 
    // Destroy objects from aligned storage
    ~static_vector() 
    {
        for(std::size_t pos = 0; pos < m_size; ++pos) {
            // Note: std::launder is needed after the change of object model in P0137R1
            std::destroy_at(std::launder(reinterpret_cast<T*>(&data[pos])));
        }
    }
};

void fillCheckerboard(Image& image, uint8_t color1, uint8_t color2, int translateX, int translateY, float angleDegrees) {
    float angleRadians = angleDegrees * M_PI / 180.0f;
    float cosAngle = std::cos(angleRadians);
    float sinAngle = std::sin(angleRadians);

    for (int y = 0; y < Image::kHeight; y++) {
        for (int x = 0; x < Image::kWidth; x++) {
            int translatedX = x - translateX;
            int translatedY = y - translateY;
            int rotatedX = std::round(translatedX * cosAngle - translatedY * sinAngle);
            int rotatedY = std::round(translatedX * sinAngle + translatedY * cosAngle);
            uint8_t color = (((rotatedX / 20) % 2 == 0) ^ ((rotatedY / 20) % 2 == 0)) ? color1 : color2;
            image.setPixel(x, y, color);
        }
    }
}

int XUsbPs_IsDeviceConfigured(XUsbPs *InstancePtr)
{
    return (InstancePtr->IsConfigDone);
}

void process_ack(){
    if(xusb_COMMS_rx_bytes_available() < 4)
        return;

    // Get the data
    xusb_COMMS_receive_data(buffer, 4);
    if (buffer[0] == 'O' && buffer[1] == 'K' && buffer[2] == 'A' && buffer[3] == 'Y')
    {
        // Set ACK flag
        ACK_FLAG = true;
    }
}

void send_images(XUsbPs *InstancePtr, static_vector<Image, NUMBER_OF_IMAGES, Image::kNumPixels> *imageVector){
    if(!SEND_IMAGES_FLAG){
        return;
    }
    int	Status;
    std::cout << "Sending images..." << std::endl;
    // Send the images
    for (int i = 0; i < NUMBER_OF_IMAGES_TO_SEND; i++) {
        // Send the image
        Status = XUsbPs_EpBufferSend(InstancePtr, 1, imageVector->operator[](i).getBuffer(), Image::kNumPixels);
        if (Status != XST_SUCCESS) {
            std::cout << "Error: XUsbPs_EpBufferSend" << std::endl;
            return;
        }
        // Wait for aknowledgement of the image
        while(!ACK_FLAG) {
            process_ack();
        }
        // Reset flag
        ACK_FLAG = false;
    }

    std::cout << "All images sent" << std::endl;
    // Reset flag
    SEND_IMAGES_FLAG = false;
}

void receive_images(XUsbPs *InstancePtr, static_vector<Image, NUMBER_OF_IMAGES, Image::kNumPixels> *imageVector){
    if(!RECEIVE_IMAGES_FLAG){
        return;
    }
    
    int	Status;
    int bytes_read = 0;
    std::cout << "Receiving images..." << std::endl;

    // Receive the images
    for (int i = 0; i < NUMBER_OF_IMAGES_TO_RECEIVE; i++) {
        //Wait for the image
        while(xusb_COMMS_rx_bytes_available() < Image::kNumPixels);
        // Receive the image
        bytes_read = xusb_COMMS_receive_data(imageVector->operator[](i).getBufferNotConst(), Image::kNumPixels);

        if (bytes_read != Image::kNumPixels) {
            std::cout << "Error: XUsbPs_EpBufferReceive" << std::endl;
            RECEIVE_IMAGES_FLAG = false;
            return;
        }

        // std::cout << "Image transfered, sending ACK..." << std::endl;
        // Send ACK
        Status = XUsbPs_EpBufferSend(InstancePtr, 1, (u8 *)"ACK", 3);
        if (Status != XST_SUCCESS) {
            std::cout << "Error: XUsbPs_EpBufferSend" << std::endl;
            return;
        }
        // std::cout << "Image " << i << " received" << std::endl;
    }

    std::cout << "All images received" << std::endl;

    // Flush cache
    Xil_DCacheFlushRange((INTPTR)imageVector, NUMBER_OF_IMAGES_TO_RECEIVE * Image::kNumPixels);
    // Invalidate cache
    Xil_DCacheInvalidateRange((INTPTR)imageVector, NUMBER_OF_IMAGES_TO_RECEIVE * Image::kNumPixels);

    // Reset flag
    RECEIVE_IMAGES_FLAG = false;
    TRANSFER_IMAGES_FLAG = true;
}

int32_t float_to_fixed_point(double num, int N) {
    // This only works for signed fixed point numbers with 32 bit total or less
    int32_t fixed_point_num = (int32_t) (num * pow(2, N));
    return fixed_point_num;
}

double fixed_point_to_double(int32_t num, int N) {
    // This only works for signed fixed point numbers with 32 bit total or less
    double double_num = (double) num / pow(2, N);
    return double_num;
}

// List of parameters value and names
double parameters[18] = {320.0, 1.0/320.0, 240.0, 1.0/240.0, 160.0, 120.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
std::string parameter_names[18] = {"fx", "1/fx", "fy", "1/fy", "cx", "cy", "k1", "k2", "k3", "k4", "k5", "k6", "p1", "p2", "s1", "s2", "s3", "s4"};
void set_parameters(XUndistort_coordinate_generator *undistort_instance_ptr){
    XUndistort_coordinate_generator_Set_fx_in(undistort_instance_ptr, float_to_fixed_point(parameters[0], 21));
    XUndistort_coordinate_generator_Set_fx_inverse_in(undistort_instance_ptr, float_to_fixed_point(parameters[1], 32));
    XUndistort_coordinate_generator_Set_fy_in(undistort_instance_ptr, float_to_fixed_point(parameters[2], 21));
    XUndistort_coordinate_generator_Set_fy_inverse_in(undistort_instance_ptr, float_to_fixed_point(parameters[3], 32));
    XUndistort_coordinate_generator_Set_cx_in(undistort_instance_ptr, float_to_fixed_point(parameters[4], 21));
    XUndistort_coordinate_generator_Set_cy_in(undistort_instance_ptr, float_to_fixed_point(parameters[5], 21));
    XUndistort_coordinate_generator_Set_k1_in(undistort_instance_ptr, float_to_fixed_point(parameters[6], 27));
    XUndistort_coordinate_generator_Set_k2_in(undistort_instance_ptr, float_to_fixed_point(parameters[7], 27));
    XUndistort_coordinate_generator_Set_k3_in(undistort_instance_ptr, float_to_fixed_point(parameters[8], 27));
    XUndistort_coordinate_generator_Set_k4_in(undistort_instance_ptr, float_to_fixed_point(parameters[9], 27));
    XUndistort_coordinate_generator_Set_k5_in(undistort_instance_ptr, float_to_fixed_point(parameters[10], 27));
    XUndistort_coordinate_generator_Set_k6_in(undistort_instance_ptr, float_to_fixed_point(parameters[11], 27));
    XUndistort_coordinate_generator_Set_p1_in(undistort_instance_ptr, float_to_fixed_point(parameters[12], 27));
    XUndistort_coordinate_generator_Set_p2_in(undistort_instance_ptr, float_to_fixed_point(parameters[13], 27));
    XUndistort_coordinate_generator_Set_s1_in(undistort_instance_ptr, float_to_fixed_point(parameters[14], 27));
    XUndistort_coordinate_generator_Set_s2_in(undistort_instance_ptr, float_to_fixed_point(parameters[15], 27));
    XUndistort_coordinate_generator_Set_s3_in(undistort_instance_ptr, float_to_fixed_point(parameters[16], 27));
    XUndistort_coordinate_generator_Set_s4_in(undistort_instance_ptr, float_to_fixed_point(parameters[17], 27));
}

void get_parameters(XUndistort_coordinate_generator *undistort_instance_ptr){
    // Get parameters from PL
    parameters[0] = fixed_point_to_double(XUndistort_coordinate_generator_Get_fx_in(undistort_instance_ptr), 21);
    parameters[1] = fixed_point_to_double(XUndistort_coordinate_generator_Get_fx_inverse_in(undistort_instance_ptr), 32);
    parameters[2] = fixed_point_to_double(XUndistort_coordinate_generator_Get_fy_in(undistort_instance_ptr), 21);
    parameters[3] = fixed_point_to_double(XUndistort_coordinate_generator_Get_fy_inverse_in(undistort_instance_ptr), 32);
    parameters[4] = fixed_point_to_double(XUndistort_coordinate_generator_Get_cx_in(undistort_instance_ptr), 21);
    parameters[5] = fixed_point_to_double(XUndistort_coordinate_generator_Get_cy_in(undistort_instance_ptr), 21);
    parameters[6] = fixed_point_to_double(XUndistort_coordinate_generator_Get_k1_in(undistort_instance_ptr), 27);
    parameters[7] = fixed_point_to_double(XUndistort_coordinate_generator_Get_k2_in(undistort_instance_ptr), 27);
    parameters[8] = fixed_point_to_double(XUndistort_coordinate_generator_Get_k3_in(undistort_instance_ptr), 27);
    parameters[9] = fixed_point_to_double(XUndistort_coordinate_generator_Get_k4_in(undistort_instance_ptr), 27);
    parameters[10] = fixed_point_to_double(XUndistort_coordinate_generator_Get_k5_in(undistort_instance_ptr), 27);
    parameters[11] = fixed_point_to_double(XUndistort_coordinate_generator_Get_k6_in(undistort_instance_ptr), 27);
    parameters[12] = fixed_point_to_double(XUndistort_coordinate_generator_Get_p1_in(undistort_instance_ptr), 27);
    parameters[13] = fixed_point_to_double(XUndistort_coordinate_generator_Get_p2_in(undistort_instance_ptr), 27);
    parameters[14] = fixed_point_to_double(XUndistort_coordinate_generator_Get_s1_in(undistort_instance_ptr), 27);
    parameters[15] = fixed_point_to_double(XUndistort_coordinate_generator_Get_s2_in(undistort_instance_ptr), 27);
    parameters[16] = fixed_point_to_double(XUndistort_coordinate_generator_Get_s3_in(undistort_instance_ptr), 27);
    parameters[17] = fixed_point_to_double(XUndistort_coordinate_generator_Get_s4_in(undistort_instance_ptr), 27);
}

void print_all_parameters(){
    // Print all parameters
    std::cout << "Parameters" << std::endl;
    std::cout << "----------" << std::endl;
    for (int i = 0; i < 18; i++) {
        std::cout << parameter_names[i] << " received: " << parameters[i] << std::endl;
    }
}

void receive_parameters(XUsbPs *InstancePtr, XUndistort_coordinate_generator *undistort_instance_ptr){
    if(!RECEIVE_PARAMETERS_FLAG){
        return;
    }

    int	Status;
    int bytes_read = 0;
    // Wait and read all 12 parameters
    for (int i = 0; i < 18; i++) {
        while(xusb_COMMS_rx_bytes_available() < 8);
        bytes_read = xusb_COMMS_receive_data(reinterpret_cast<u8*>(&parameters[i]), 8);

        if (bytes_read != 8) {
            std::cout << "Error: XUsbPs_EpBufferReceive" << std::endl;
            RECEIVE_PARAMETERS_FLAG = false;
            return;
        } else {
            std::cout << "Parameter " << parameter_names[i] << " received: " << parameters[i] << std::endl;
        }
    }
    Status = XUsbPs_EpBufferSend(InstancePtr, 1, (u8 *)"ACK", 3);
    if (Status != XST_SUCCESS) {
        std::cout << "ERROR HERE!!!" << std::endl;
        std::cout << "Error: XUsbPs_EpBufferSend" << std::endl;
        return;
    }
    std::cout << "ACK sent" << std::endl;
    std::cout << "Parameters received" << std::endl;
    print_all_parameters();
    std::cout << "Setting parameters..." << std::endl;
    set_parameters(undistort_instance_ptr);
    std::cout << "Checking parameters..." << std::endl;
    get_parameters(undistort_instance_ptr);
    print_all_parameters();
    RECEIVE_PARAMETERS_FLAG = false;
}

void transfer_images(XUsbPs *InstancePtr){
    if(!TRANSFER_IMAGES_FLAG){
        return;
    }
    
    int byte_sent;
    transferred_images_counter = 0;
    std::cout << "Transferring images..." << std::endl;
    // Transfer the images
    // Start FrameBuffer Read and Write
    start_image_transfer(sizeof(Image), NUMBER_OF_IMAGES_TO_RECEIVE, VIDEO_IN_MEM_ADDR, VIDEO_OUT_MEM_ADDR);
    wait_for_transfer_complete();
    std::cout << "All images transferred" << std::endl;

    //Send signal to the host
    byte_sent = xusb_COMMS_send_data(InstancePtr, (u8 *)"DONE", 4);
    if (byte_sent != 4) {
        std::cout << "Error: XUsbPs_EpBufferSend" << std::endl;
        TRANSFER_IMAGES_FLAG = false;
        return;
    }

    PRINT_FRAMERATE_FLAG = true;
    // Reset flag
    TRANSFER_IMAGES_FLAG = false;
}

void process_usb_commands(){
    if(xusb_COMMS_rx_bytes_available() < 7){
        // Print the number of bytes available
        // std::cout << "Bytes available: " << xusb_COMMS_rx_bytes_available() << std::endl;
        return;
    }

    // Get the data
    xusb_COMMS_receive_data(buffer, 7);

    // Process the data
    if (buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T') {
        // Read the rest of the buffer to get the image number
        NUMBER_OF_IMAGES_TO_SEND = buffer[3] << 24 | buffer[4] << 16 | buffer[5] << 8 | buffer[6];
        std::cout << "Nb. Image to Send: " << NUMBER_OF_IMAGES_TO_SEND << std::endl;
        // Set flag to start sending images
        SEND_IMAGES_FLAG = true;
    }
    else if (buffer[0] == 'S' && buffer[1] == 'E' && buffer[2] == 'T')
    {
        // Read the rest of the buffer to get the image number
        NUMBER_OF_IMAGES_TO_RECEIVE = buffer[3] << 24 | buffer[4] << 16 | buffer[5] << 8 | buffer[6];
        std::cout << "Nb. Image to Receive: " << NUMBER_OF_IMAGES_TO_RECEIVE << std::endl;
        // Set flag to start sending images
        RECEIVE_IMAGES_FLAG = true;
        std::cout << "HELLO" << std::endl;
    }
    else if (buffer[0] == 'P' && buffer[1] == 'A' && buffer[2] == 'R')
    {
        // Receiving parameters
        std::cout << "Receiving parameters..." << std::endl;
        // Set flag to start sending images
        RECEIVE_PARAMETERS_FLAG = true;
    }
    // Print what was received
    std::cout << "Received: " << buffer[0] << buffer[1] << buffer[2] << buffer[3] << buffer[4] << buffer[5] << buffer[6] << std::endl;
}

void print_framerate(){
    // Only print framerate if print flag is the only one set
    if(!PRINT_FRAMERATE_FLAG || TRANSFER_IMAGES_FLAG || RECEIVE_IMAGES_FLAG || RECEIVE_PARAMETERS_FLAG || SEND_IMAGES_FLAG){
        return;
    }
    // Print average framerate
    double framerate = get_average_framerate();
    std::cout << "Average framerate: " << framerate << " fps" << std::endl;
    // Reset flag
    PRINT_FRAMERATE_FLAG = false;
}


static XScuGic IntcInstance;	/* The instance of the IRQ Controller */
static XUsbPs UsbInstance;	/* The instance of the USB Controller */
static XTtcPs TtcInstance;	/* The instance of the TTC Controller */
XUndistort_coordinate_generator_Config *undistort_config;
XUndistort_coordinate_generator undistort_instance;
static_vector<Image, NUMBER_OF_IMAGES, Image::kNumPixels> imageVectorIn __attribute__((section(".video_in"))) __attribute__((aligned(alignof(Image))));;
static_vector<Image, NUMBER_OF_IMAGES, Image::kNumPixels> imageVectorOut __attribute__((section(".video_out"))) __attribute__((aligned(alignof(Image))));;
int main(int argc, char *argv[])
{
    int Status;
    std::cout << "Program Started" << std::endl;

    // Allocate memory for 500 Image objects in contiguous memory
    // Fill in images
    std::cout << "Creating images..." << std::endl;
    for (int i = 0; i < NUMBER_OF_IMAGES; i++) {
        imageVectorIn.emplace_back();
        imageVectorOut.emplace_back();
    }

    // Set to High Speed
    UsbInstance.AppData->Speed = XUSBPS_SPEED_HIGH;

    //Initialize interrupt controller
    Status = SetupInterruptSystem(XPAR_SCUGIC_SINGLE_DEVICE_ID, &IntcInstance);
    if (Status != XST_SUCCESS) {
        std::cout << "Error: SetupInterruptSystem" << std::endl;
		return XST_FAILURE;
	}

    // Initialize the USB controller
    Status = xusb_COMMS_init(&IntcInstance, &UsbInstance, XPAR_XUSBPS_0_DEVICE_ID, XPAR_XUSBPS_0_INTR, RINGBUFFER_SIZE);
    if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

    // Wait for the USB to be connected and configured completely
    while (XUsbPs_IsDeviceConfigured(&UsbInstance) == 0) {
        xil_printf("Waiting for USB to be configured...\r\n");
        sleep(1);
    }

    std::cout << "USB Configured" << std::endl;

    // Framebuffer setup
    // Initialize framebuffers
    std::cout << "Initializing frame buffers..." << std::endl;
    initialize_framebuffers(64, VIDEO_IN_MEM_ADDR, VIDEO_OUT_MEM_ADDR, IMAGE_WIDTH, IMAGE_HEIGHT, 8, 24, XVIDC_PPC_8, XVIDC_CSF_MEM_Y8);
    // Enable framebuffer interrupts
    std::cout << "Enabling framebuffer interrupts..." << std::endl;
    enable_framebuffer_interrupts(&IntcInstance);

    //Enable the TTC timers
    std::cout << "Enabling TTC timers..." << std::endl;
    enable_ttc(&TtcInstance, &IntcInstance, TIMER_PRESCALER);

    // Testing timer functionnality
    measure_time_start();
    usleep(1000000);
    measure_time_stop();
    print_time("sleep(1)", TIMER_PRESCALER);

    sleep(2);
    // Clear ring buffer
    xusb_COMMS_clear_rx_buffer();

    // Initialze the Undistort HLS module
    std::cout << "Initializing the Undistort HLS module..." << std::endl;
    Status = XUndistort_coordinate_generator_Initialize(&undistort_instance, XPAR_XUNDISTORT_COORDINATE_GENERATOR_0_DEVICE_ID);
    if (Status != XST_SUCCESS) {
        std::cout << "Error initializing the Undistort HLS module" << std::endl;
        return XST_FAILURE;
    }
    // Send parameters to the Undistort HLS module
    std::cout << "Sending parameters to the Undistort HLS module..." << std::endl;
    set_parameters(&undistort_instance);

    //DEBUG: Reset frame counter
    reset_frame_counter();

    std::cout << "Waiting for data..." << std::endl;


    //Program loop
    while (1) {
        print_framerate();
        receive_images(&UsbInstance, &imageVectorIn);
        transfer_images(&UsbInstance);
        receive_parameters(&UsbInstance, &undistort_instance);
        send_images(&UsbInstance, &imageVectorOut);
        process_usb_commands();
    }
    return 0;
}
