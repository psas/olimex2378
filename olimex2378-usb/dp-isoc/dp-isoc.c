
/*
 * dp-isoc.c
 *
 * Test the datapath from a sensor to
 * the LPC to USB(isochronous) to the FC
 * and from the FC to the LPC.
 * 
 */

#include <stdio.h>                      // EOF
#include <string.h>                     // memcpy

#include "lpc23xx.h"
#include "lpc23xx-debug.h"
#include "printf-lpc.h"

#include "lpc23xx-mam.h"
#include "lpc23xx-pll.h"
#include "lpc23xx-uart.h"
#include "lpc23xx-util.h"
#include "lpc23xx-vic.h"

#include "usbapi.h"
#include "usbhw_lpc.h"

#include "dp-isoc.h"


static const U8 abDescriptors[] = {

// device descriptor
	0x12,
	DESC_DEVICE,
	LE_WORD(0x0101),			// bcdUSB
	0x02,						// bDeviceClass
	0x00,						// bDeviceSubClass
	0x00,						// bDeviceProtocol
	MAX_PACKET_SIZE0,			// bMaxPacketSize
	LE_WORD(0xFFFF),			// idVendor
	LE_WORD(0x0005),			// idProduct
	LE_WORD(0x0100),			// bcdDevice
	0x01,						// iManufacturer
	0x02,						// iProduct
	0x03,						// iSerialNumber
	0x01,						// bNumConfigurations

// configuration descriptor
	0x09,
	DESC_CONFIGURATION,
	LE_WORD(32),                // sizeof(this configuration descriptor) + sizeof(all interfaces & endpoints defined)
								// 0x9+0x9+0x7+0x7= 0x20 = 32
	0x01, //0x02,				// bNumInterfaces
	0x01,						// bConfigurationValue
	0x00,						// iConfiguration
	0xC0,						// bmAttributes
	0x32,						// bMaxPower

// data class interface descriptor
	0x09,
	DESC_INTERFACE,
	0x00,						// bInterfaceNumber
	0x00,						// bAlternateSetting
	0x02,//DC				    // bNumEndPoints
	0xFF,// 0x0A,				// bInterfaceClass = data
	0x00,						// bInterfaceSubClass
	0x00,						// bInterfaceProtocol
	0x00,						// iInterface

    // data EP OUT
	0x07,
	DESC_ENDPOINT,
	ISOC_OUT_EP,				// bEndpointAddress
	0x0D,					    // bmAttributes = isoc, syncronous, data endpoint
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x01,						// bInterval

	// data EP IN
	0x07,
	DESC_ENDPOINT,
	ISOC_IN_EP,				    // bEndpointAddress
	0x0D,					    // bmAttributes = isoc, syncronous, data endpoint
	LE_WORD(MAX_PACKET_SIZE),	// wMaxPacketSize
	0x01,						// bInterval

	// string descriptors
	0x04,
	DESC_STRING,
	LE_WORD(0x0409),            // English united states

	0x0A,
	DESC_STRING,
	'P', 0, 'S', 0, 'A', 0, 'S', 0,                                                 // PSAS

	0x14,
	DESC_STRING,
	'U', 0, 'S', 0, 'B', 0, 'I', 0, 's', 0, 'o', 0, 'c', 0, '.', 0, '.', 0,         // USBIsoc..

	0x16,
	DESC_STRING,
	'l', 0, 'i', 0, 'b', 0, 'l', 0, 'p', 0, 'c', 0, '2', 0, '3', 0, 'x', 0, 'x', 0, // liblpc23xx

// terminating zero
	0                            // no more string descriptors
};

/*
 * stream_task
 */
void stream_task() {

    DBG(UART0,"In stream task.\n");
}


/*
 * main
 */
int main(void) {

    FIO_ENABLE;
    pllstart_seventytwomhz() ;
    mam_enable();
    uart0_init_115200() ;

    DBG(UART0,"Initialising USB stack\n");

    // initialise stack
    USBInit();

    // register descriptors
    USBRegisterDescriptors(abDescriptors);

    // register class request handler
    USBRegisterRequestHandler(REQTYPE_TYPE_CLASS, HandleClassRequest, abClassReqData);

    // register endpoint handlers
    USBHwRegisterEPIntHandler(INT_IN_EP, NULL);

    // register frame handler
    USBHwRegisterFrameHandler(USBFrameHandler);

    // register device event handler
    USBHwRegisterDevIntHandler(USBDevIntHandler);

    inputIsocDataBuffer[0] = 0;

    DBG(UART0,"Starting USB isoc datatpath test\n");

    VICVectPriority22 = 0x01;
    VICVectAddr22     = (int) USBIntHandler;

    // set up USB interrupt
    VICIntSelect      &= ~(1<<22);     // select IRQ for USB
    VICIntEnable      |= (1<<22);

    vic_enableIRQ();

    // connect to bus
    USBHwConnect(TRUE);

    stream_task();

    return 0;
}

