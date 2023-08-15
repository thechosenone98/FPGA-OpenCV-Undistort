#include "xusbps_ringbuffer.h"

#include "xusbps_ch9.h"
#include <stdlib.h>
#include <string.h>
#include "xstatus.h"

int xusb_Ringbuffer_init(RingBuffer *rbuf, u32 size) {
	if (rbuf == NULL) {
		return XST_FAILURE;
	}

	rbuf->length = size + 1;
	rbuf->start = 0;
	rbuf->end = 0;
	rbuf->buffer = (u8*) malloc(rbuf->length);

	rbuf->initialized = XUSB_RINGBUFFER_INITIALIZED;

	return XST_SUCCESS;
}

int xusb_Ringbuffer_read(RingBuffer *rbuf, u8 *buffer, u32 length) {
	u32 bytes_read;
	u32 available_bytes;
	u32 remaining;

	if (rbuf == NULL) {
		return 0;
	}

	if (rbuf->initialized != XUSB_RINGBUFFER_INITIALIZED) {
		return 0;
	}

	remaining = length;
	available_bytes = xusb_Ringbuffer_available_bytes(rbuf);
	if (length > available_bytes) {
		remaining = available_bytes;
	}

	if ((rbuf->start + remaining) > rbuf->length) {
		bytes_read = rbuf->length - rbuf->start;
		remaining -= bytes_read;
		memcpy(buffer,
			   rbuf->buffer + rbuf->start,
			   bytes_read);
		memcpy(buffer + bytes_read,
			   rbuf->buffer,
			   remaining);
		bytes_read += remaining;
	}
	else {
		memcpy(buffer,
			   rbuf->buffer + rbuf->start,
			   length);
		bytes_read = length;
	}

	/* Update the pointers in the buffer */
	if (bytes_read == available_bytes) {
		rbuf->start = 0;
		rbuf->end = 0;
	}
	else if((rbuf->start + bytes_read) > rbuf->length) {
		rbuf->start = rbuf->length % (rbuf->start + bytes_read);
	}
	else {
		rbuf->start += bytes_read;
	}

	return bytes_read;
}

int xusb_Ringbuffer_write(RingBuffer *rbuf, u8 *data, u32 length) {
	u32 bytes_written;
	u32 available_bytes;
	u32 remaining;

	if (rbuf == NULL) {
		return 0;
	}

	if (rbuf->initialized != XUSB_RINGBUFFER_INITIALIZED) {
		return 0;
	}

	remaining = length;
	available_bytes = xusb_Ringbuffer_available_space(rbuf);
	if (length > available_bytes) {
		remaining = available_bytes;
	}

	if ((rbuf->end + remaining) > rbuf->length) {
		bytes_written = rbuf->length - rbuf->end;
		remaining -= bytes_written;
		memcpy(rbuf->buffer + rbuf->end,
			   data,
			   bytes_written);
		memcpy(rbuf->buffer,
			   data + bytes_written,
			   remaining);
		bytes_written += remaining;
	}
	else {
		memcpy(rbuf->buffer + rbuf->end,
			   data,
			   length);
		bytes_written = length;
	}

	/* Update the pointers in the buffer */
	if((rbuf->end + bytes_written) > rbuf->length) {
		rbuf->end = rbuf->length % (rbuf->end + bytes_written);
	}
	else {
		rbuf->end += bytes_written;
	}

	// xil_printf("\r\nREM SPACE: %d\r\n", xusb_Ringbuffer_available_space(rbuf));

	return bytes_written;
}

void xusb_Ringbuffer_clear_rx_buffer(RingBuffer *rbuf)
{
	if (rbuf == NULL) {
		return;
	}

	if (rbuf->initialized != XUSB_RINGBUFFER_INITIALIZED) {
		return;
	}

	rbuf->start = 0;
	rbuf->end = 0;
}

int xusb_Ringbuffer_available_bytes(RingBuffer *rbuf) {
	if (rbuf == NULL) {
		xil_printf("rbuf is null\n\r");
		return 0;
	}

	if (rbuf->initialized != XUSB_RINGBUFFER_INITIALIZED) {
		xil_printf("rbuf is not initialized\n\r");
		return 0;
	}

	if (rbuf->end < rbuf->start) {
		return rbuf->length - rbuf->start + rbuf->end;
	}
	else {
		return rbuf->end - rbuf->start;
	}
}

int xusb_Ringbuffer_available_space(RingBuffer *rbuf) {
	if (rbuf == NULL) {
		return 0;
	}

	if (rbuf->initialized != XUSB_RINGBUFFER_INITIALIZED) {
		return 0;
	}

	if (rbuf->end < rbuf->start) {
		return rbuf->start - rbuf->end;
	}
	else {
		return rbuf->length - rbuf->end + rbuf->start;
	}

}