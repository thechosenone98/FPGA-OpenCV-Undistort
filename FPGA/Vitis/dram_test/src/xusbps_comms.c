#include "xusbps_comms.h"

#include "xusbps.h"
#include "xusbps_ch9.h"

#include "xusbps_ringbuffer.h"
#include "xil_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************** Constant Definitions *****************************/
#define MEMORY_SIZE (512 * 1024)
#ifdef __ICCARM__
#pragma data_alignment = 32
u8 Buffer[MEMORY_SIZE];
#pragma data_alignment = 4
#else
u8 Buffer[MEMORY_SIZE] ALIGNMENT_CACHELINE;
#endif



static volatile int NumIrqs = 0;
static volatile int NumReceivedFrames = 0;



RingBuffer xusb_rx_buffer;

XUsbPs_Local usb_local_data;

int xusb_COMMS_init(XScuGic *IntcInstancePtr, XUsbPs *UsbInstancePtr,	u16 UsbDeviceId, u16 UsbIntrId, u32 rx_buf_size)
{
	int	Status;
	u8	*MemPtr = NULL;
	int	ReturnStatus = XST_FAILURE;

	/* For this example we only configure 2 endpoints:
	 *   Endpoint 0 (default control endpoint)
	 *   Endpoint 1 (BULK data endpoint)
	 */
	const u8 NumEndpoints = 3;

	XUsbPs_Config		*UsbConfigPtr;
	XUsbPs_DeviceConfig	deviceConfig;

	/* Initialize the USB driver so that it's ready to use,
	 * specify the controller ID that is generated in xparameters.h
	 */
	UsbConfigPtr = XUsbPs_LookupConfig(UsbDeviceId);
	if (NULL == UsbConfigPtr) {
		goto out;
	}


	/* We are passing the physical base address as the third argument
	 * because the physical and virtual base address are the same in our
	 * example.  For systems that support virtual memory, the third
	 * argument needs to be the virtual base address.
	 */
	Status = XUsbPs_CfgInitialize(UsbInstancePtr,
			UsbConfigPtr,
			UsbConfigPtr->BaseAddress);
	if (XST_SUCCESS != Status) {
		goto out;
	}

	/* Set up the interrupt subsystem.
	 */
	Status = UsbSetupIntrSystem(IntcInstancePtr,
			UsbInstancePtr,
			UsbIntrId);
	if (XST_SUCCESS != Status)
	{
		goto out;
	}

	/* Configuration of the DEVICE side of the controller happens in
	 * multiple stages.
	 *
	 * 1) The user configures the desired endpoint configuration using the
	 * XUsbPs_DeviceConfig data structure. This includes the number of
	 * endpoints, the number of Transfer Descriptors for each endpoint
	 * (each endpoint can have a different number of Transfer Descriptors)
	 * and the buffer size for the OUT (receive) endpoints.  Each endpoint
	 * can have different buffer sizes.
	 *
	 * 2) Request the required size of DMAable memory from the driver using
	 * the XUsbPs_DeviceMemRequired() call.
	 *
	 * 3) Allocate the DMAable memory and set up the DMAMemVirt and
	 * DMAMemPhys members in the XUsbPs_DeviceConfig data structure.
	 *
	 * 4) Configure the DEVICE side of the controller by calling the
	 * XUsbPs_ConfigureDevice() function.
	 */

	/*
	 * For this example we only configure Endpoint 0 and Endpoint 1.
	 *
	 * Bufsize = 0 indicates that there is no buffer allocated for OUT
	 * (receive) endpoint 0. Endpoint 0 is a control endpoint and we only
	 * receive control packets on that endpoint. Control packets are 8
	 * bytes in size and are received into the Queue Head's Setup Buffer.
	 * Therefore, no additional buffer space is needed.
	 */
	deviceConfig.EpCfg[0].Out.Type		= XUSBPS_EP_TYPE_CONTROL;
	deviceConfig.EpCfg[0].Out.NumBufs	= 2;
	deviceConfig.EpCfg[0].Out.BufSize	= 64;
	deviceConfig.EpCfg[0].Out.MaxPacketSize	= 64;
	deviceConfig.EpCfg[0].In.Type		= XUSBPS_EP_TYPE_CONTROL;
	deviceConfig.EpCfg[0].In.NumBufs	= 2;
	deviceConfig.EpCfg[0].In.MaxPacketSize	= 64;

	deviceConfig.EpCfg[1].Out.Type		= XUSBPS_EP_TYPE_BULK;
	deviceConfig.EpCfg[1].Out.NumBufs	= 512;
	deviceConfig.EpCfg[1].Out.BufSize	= 512;
	deviceConfig.EpCfg[1].Out.MaxPacketSize	= 512;
	deviceConfig.EpCfg[1].In.Type		= XUSBPS_EP_TYPE_BULK;
	deviceConfig.EpCfg[1].In.NumBufs	= 16;
	deviceConfig.EpCfg[1].In.MaxPacketSize	= 512;

    deviceConfig.EpCfg[2].Out.Type		= XUSBPS_EP_TYPE_BULK;
	deviceConfig.EpCfg[2].Out.NumBufs	= 16;
	deviceConfig.EpCfg[2].Out.BufSize	= 512;
	deviceConfig.EpCfg[2].Out.MaxPacketSize	= 512;
	deviceConfig.EpCfg[2].In.Type		= XUSBPS_EP_TYPE_BULK;
	deviceConfig.EpCfg[2].In.NumBufs	= 16;
	deviceConfig.EpCfg[2].In.MaxPacketSize	= 512;

	deviceConfig.NumEndpoints = NumEndpoints;

	MemPtr = (u8 *)&Buffer[0];
	memset(MemPtr,0,MEMORY_SIZE);
	Xil_DCacheFlushRange((unsigned int)MemPtr, MEMORY_SIZE);

	/* Finish the configuration of the DeviceConfig structure and configure
	 * the DEVICE side of the controller.
	 */
	deviceConfig.DMAMemPhys = (u32) MemPtr;

	Status = XUsbPs_ConfigureDevice(UsbInstancePtr, &deviceConfig);
	if (XST_SUCCESS != Status) {
		goto out;
	}

	/* Set the handler for receiving frames. */
	Status = XUsbPs_IntrSetHandler(UsbInstancePtr, UsbIntrHandler, NULL,
			XUSBPS_IXR_UE_MASK);
	if (XST_SUCCESS != Status) {
		goto out;
	}

	/* Set the handler for handling endpoint 0 events. This is where we
	 * will receive and handle the Setup packet from the host.
	 */
	Status = XUsbPs_EpSetHandler(UsbInstancePtr, 0,
			XUSBPS_EP_DIRECTION_OUT,
			XUsbPs_Ep0EventHandler, UsbInstancePtr);

	/* Set the handler for handling endpoint 1 events.
	 *
	 * Note that for this example we do not need to register a handler for
	 * TX complete events as we only send data using static data buffers
	 * that do not need to be free()d or returned to the OS after they have
	 * been sent.
	 */
	Status = XUsbPs_EpSetHandler(UsbInstancePtr, 1,
			XUSBPS_EP_DIRECTION_OUT,
			XUsbPs_Ep1EventHandler, UsbInstancePtr);

	/* Enable the interrupts. */
	XUsbPs_IntrEnable(UsbInstancePtr, XUSBPS_IXR_UR_MASK |
			XUSBPS_IXR_UI_MASK);

	Status = xusb_Ringbuffer_init(&xusb_rx_buffer, rx_buf_size);
    if (Status != XST_SUCCESS) {
        goto out;
    }
	/* Start the USB engine */
	XUsbPs_Start(UsbInstancePtr);


	xil_printf("Cleanup\r\n");

	/* Set return code to indicate success and fall through to clean-up
	 * code.
	 */
	return XST_SUCCESS;

	out:
	/* Clean up. It's always safe to disable interrupts and clear the
	 * handlers, even if they have not been enabled/set. The same is true
	 * for disabling the interrupt subsystem.
	 */
	XUsbPs_Stop(UsbInstancePtr);
	XUsbPs_IntrDisable(UsbInstancePtr, XUSBPS_IXR_ALL);
	(int) XUsbPs_IntrSetHandler(UsbInstancePtr, NULL, NULL, 0);

	UsbDisableIntrSystem(IntcInstancePtr, UsbIntrId);

	/* Free allocated memory.
	 */
	if (NULL != UsbInstancePtr->UserDataPtr) {
		free(UsbInstancePtr->UserDataPtr);
	}
	return ReturnStatus;
}


/*****************************************************************************/
/**
 *
 * This function is the handler which performs processing for the USB driver.
 * It is called from an interrupt context such that the amount of processing
 * performed should be minimized.
 *
 * This handler provides an example of how to handle USB interrupts and
 * is application specific.
 *
 * @param	CallBackRef is the Upper layer callback reference passed back
 *		when the callback function is invoked.
 * @param 	Mask is the Interrupt Mask.
 * @param	CallBackRef is the User data reference.
 *
 * @return
 * 		- XST_SUCCESS if successful
 * 		- XST_FAILURE on error
 *
 * @note	None.
 *
 ******************************************************************************/
static void UsbIntrHandler(void *CallBackRef, u32 Mask)
{
	NumIrqs++;
}


/*****************************************************************************/
/**
 * This funtion is registered to handle callbacks for endpoint 0 (Control).
 *
 * It is called from an interrupt context such that the amount of processing
 * performed should be minimized.
 *
 *
 * @param	CallBackRef is the reference passed in when the function
 *		was registered.
 * @param	EpNum is the Number of the endpoint on which the event occured.
 * @param	EventType is type of the event that occured.
 *
 * @return	None.
 *
 ******************************************************************************/
static void XUsbPs_Ep0EventHandler(void *CallBackRef, u8 EpNum,
		u8 EventType, void *Data)
{
	XUsbPs			*InstancePtr;
	int			Status;
	XUsbPs_SetupData	SetupData;
	u8	*BufferPtr;
	u32	BufferLen;
	u32	Handle;


	Xil_AssertVoid(NULL != CallBackRef);

	InstancePtr = (XUsbPs *) CallBackRef;

	switch (EventType) {

	/* Handle the Setup Packets received on Endpoint 0. */
	case XUSBPS_EP_EVENT_SETUP_DATA_RECEIVED:
		Status = XUsbPs_EpGetSetupData(InstancePtr, EpNum, &SetupData);
		if (XST_SUCCESS == Status) {
			/* Handle the setup packet. */
			(int) XUsbPs_Ch9HandleSetupPacket(InstancePtr,
					&SetupData);
		}
		break;

		/* We get data RX events for 0 length packets on endpoint 0. We receive
		 * and immediately release them again here, but there's no action to be
		 * taken.
		 */
	case XUSBPS_EP_EVENT_DATA_RX:
		/* Get the data buffer. */
		Status = XUsbPs_EpBufferReceive(InstancePtr, EpNum,
				&BufferPtr, &BufferLen, &Handle);
		if (XST_SUCCESS == Status) {
			/* Return the buffer. */
			XUsbPs_EpBufferRelease(Handle);
		}
		break;

	default:
		/* Unhandled event. Ignore. */
		break;
	}
#ifdef COMMS_DBG
	xil_printf("EP0\r\n");
#endif

}

/*****************************************************************************/
/**
 * This funtion is registered to handle callbacks for endpoint 1 (Bulk data).
 *
 * It is called from an interrupt context such that the amount of processing
 * performed should be minimized.
 *
 *
 * @param	CallBackRef is the reference passed in when the function was
 *		registered.
 * @param	EpNum is the Number of the endpoint on which the event occured.
 * @param	EventType is type of the event that occured.
 *
 * @return	None.
 *
 * @note 	None.
 *
 ******************************************************************************/
static void XUsbPs_Ep1EventHandler(void *CallBackRef, u8 EpNum,
		u8 EventType, void *Data)
{
	XUsbPs *usb;
	int status;
	u8 *buffer;
	u32 invalidate_length;
	u32 buffer_length;
	u32 handle;

	Xil_AssertVoid(NULL != CallBackRef);

	usb = (XUsbPs *)CallBackRef;

	switch(EventType) {
        case XUSBPS_EP_EVENT_DATA_RX:
            /* Get the data buffer */
            status = XUsbPs_EpBufferReceive(usb, EpNum, &buffer, &buffer_length, &handle);
            /* Invalidate the buffer pointer */
            invalidate_length = buffer_length;
            /* Ensure alignment for invalidation */
            if (buffer_length % 32) {
                invalidate_length = (buffer_length / 32) * 32 + 32;
            }

            /* Invalidate the cache for the range of the received data buffer */
            Xil_DCacheInvalidateRange((unsigned int)buffer, invalidate_length);

            if (status == XST_SUCCESS) {
                /* Handle the bulk request */
                status = xusb_COMMS_handle_bulk_request(usb, EpNum, buffer, buffer_length);
            }
            XUsbPs_EpBufferRelease(handle);
            break;
        default:
            /* Unhandled event */
            break;
	}
}

static int UsbSetupIntrSystem(XScuGic *IntcInstancePtr,
		XUsbPs *UsbInstancePtr, u16 UsbIntrId)
{
	int Status;
	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}
	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
			IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Xil_ExceptionInit();
	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler,
			IntcInstancePtr);
	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, UsbIntrId,
			(Xil_ExceptionHandler)XUsbPs_IntrHandler,
			(void *)UsbInstancePtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}
	/*
	 * Enable the interrupt for the device.
	 */
	XScuGic_Enable(IntcInstancePtr, UsbIntrId);

	/*
	 * Enable interrupts in the Processor.
	 */
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	return XST_SUCCESS;
}

static void UsbDisableIntrSystem(XScuGic *IntcInstancePtr, u16 UsbIntrId)
{
	/* Disconnect and disable the interrupt for the USB controller. */
	XScuGic_Disconnect(IntcInstancePtr, UsbIntrId);
}

void dump_buffer(u8 *buffer, u32 length) {
	int loop;

	for (loop = 0; loop < length; loop++) {
		xil_printf("%02X", buffer[loop]);
		if (loop < (length - 1)) {
			xil_printf("-");
		}
		else {
			xil_printf("\n\r");
		}
	}
}

int xusb_COMMS_handle_bulk_request(XUsbPs *usb, u8 endpoint, u8 *buffer, u32 buffer_length) {
    // xil_printf("EP1 RX BULK REQUEST\r\n");
#ifdef COMMS_DBG
	xil_printf("EP%d received %d bytes\r\n ", endpoint, buffer_length);
	dump_buffer(buffer, buffer_length);
		//xil_printf("\n\r");
#endif
	if (endpoint == 1) {
        u32 num_written;
		num_written = xusb_Ringbuffer_write(&xusb_rx_buffer, buffer, buffer_length);
        if (num_written != buffer_length) {
            xil_printf("ERROR: Ringbuffer full\r\n");
            return XST_FAILURE;
        }
	}

	return XST_SUCCESS;
}

int xusb_COMMS_send_data(XUsbPs *usb, u8 *buffer, u32 length) {
	int status;
#ifdef COMMS_DBG
	xil_printf("EP%d Sent %d bytes \r\n ", 1, length);
#endif

	status = XUsbPs_EpBufferSend(usb, 1, buffer, length);
	if (status == XST_SUCCESS) {
		return length;
	}
	else {
		return 0;
	}
}

int xusb_COMMS_rx_bytes_available(void) {
	return xusb_Ringbuffer_available_bytes(&xusb_rx_buffer);
}

int xusb_COMMS_rx_space_available(void) {
    return xusb_Ringbuffer_available_space(&xusb_rx_buffer);
}

int xusb_COMMS_clear_rx_buffer(void)
{
    xusb_Ringbuffer_clear_rx_buffer(&xusb_rx_buffer);
}

int xusb_COMMS_receive_data(u8 *buffer, u32 length) {
	return xusb_Ringbuffer_read(&xusb_rx_buffer, buffer, length);
}
