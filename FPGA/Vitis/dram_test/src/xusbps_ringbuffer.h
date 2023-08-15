#ifndef XUSBPS_RINGBUFFER_H_
#define XUSBPS_RINGBUFFER_H_

#include "xil_types.h"

typedef struct {
	u8 *buffer;
	u32 length;
	u32 start;
	u32 end;
	u32 initialized;
} RingBuffer;

#define XUSB_RINGBUFFER_INITIALIZED	0xaa995566

/* Function Prototypes */
int xusb_Ringbuffer_init(RingBuffer *rbuf, u32 size);
int xusb_Ringbuffer_read(RingBuffer *rbuf, u8 *buffer, u32 length);
int xusb_Ringbuffer_write(RingBuffer *rbuf, u8 *data, u32 length);
void xusb_Ringbuffer_clear_rx_buffer(RingBuffer *rbuf);
int xusb_Ringbuffer_available_bytes(RingBuffer *rbuf);
int xusb_Ringbuffer_available_space(RingBuffer *rbuf);

#endif /* XUSBPS_RINGBUFFER_H_ */