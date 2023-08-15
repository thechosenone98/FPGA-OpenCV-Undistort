#ifndef XUSBPS_CDC_H_
#define XUSBPS_CDC_H_

#include "xusbps.h"
#include "xscugic.h"

//#define XUSB_CDC_DEBUG

#define XUSB_CDC_DRIVER_UNCONNECTED			0
#define XUSB_CDC_DRIVER_CONFIG 				1
#define XUSB_CDC_DRIVER_CONNECTED 			2

/* Function prototypes */
int xusb_COMMS_init(XScuGic *IntcInstancePtr, XUsbPs *UsbInstancePtr,	u16 UsbDeviceId, u16 UsbIntrId, u32 rx_buf_size);
int xusb_COMMS_handle_bulk_request(XUsbPs *usb, u8 endpoint, u8 *buffer, u32 buffer_length);
int xusb_COMMS_send_data(XUsbPs *usb, u8 *buffer, u32 length);
int xusb_COMMS_rx_bytes_available(void);
int xusb_COMMS_rx_space_available(void);
int xusb_COMMS_clear_rx_buffer(void);
int xusb_COMMS_receive_data(u8 *buffer, u32 length);


static void UsbIntrHandler(void *CallBackRef, u32 Mask);
static void XUsbPs_Ep0EventHandler(void *CallBackRef, u8 EpNum,
					u8 EventType, void *Data);
static void XUsbPs_Ep1EventHandler(void *CallBackRef, u8 EpNum,
					u8 EventType, void *Data);
static int UsbSetupIntrSystem(XScuGic *IntcInstancePtr,
			      XUsbPs *UsbInstancePtr, u16 UsbIntrId);
static void UsbDisableIntrSystem(XScuGic *IntcInstancePtr, u16 UsbIntrId);



#endif /* XUSBPS_CDC_H_ */