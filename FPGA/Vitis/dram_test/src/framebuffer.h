#include "xv_frmbufrd_l2.h"
#include "xv_frmbufwr_l2.h"

int enable_framebuffer_interrupts(XScuGic *IntcPtr);
int initialize_framebuffers(uint32_t axi_data_width,
                            uint32_t video_in_mem_addr, 
                            uint32_t video_out_mem_addr, 
                            uint32_t image_width, 
                            uint32_t image_height,
                            uint32_t max_data_width,
                            uint32_t video_format,
                            XVidC_PixelsPerClock pixel_per_clock,
                            XVidC_ColorFormat Cfmt);
void start_image_transfer(int nb_bytes_in_image, int num_image, uint32_t video_in_mem_addr, uint32_t video_out_mem_addr);
void wait_for_transfer_complete();
static u32 CalcStride(XVidC_ColorFormat Cfmt,
                      u16 AXIMMDataWidth,
                      u32 width);

double get_average_framerate();

void reset_frame_counter();
int get_frame_counter();
