

#include <string.h>                    // memcpy
#include <stdio.h>                     // EOF

#include "lpc23xx-types.h"
#include "lpc23xx-debug.h"

#include "lpc23xx-pll.h"
#include "lpc23xx-uart.h"
#include "lpc23xx-util.h"
#include "printf-lpc.h"
#include "lpc23xx-vic.h"
#include "lpc23xx-mam.h"

#include "olimex2378-util.h"

#include "lpc23xx.h"


#include "usbapi.h"
#include "usbhw_lpc.h"
#include "usbstruct.h"

#include "serial_fifo.h"


#define INT_IN_EP               0x81
#define BULK_OUT_EP             0x05
#define BULK_IN_EP              0x82

#define MAX_PACKET_SIZE         64

#define LE_WORD(x)              ((x)&0xFF),((x)>>8)

// CDC definitions
#define CS_INTERFACE            0x24
#define CS_ENDPOINT             0x25

#define SET_LINE_CODING         0x20
#define GET_LINE_CODING         0x21
#define SET_CONTROL_LINE_STATE  0x22

#define INT_VECT_NUM            0


// data structure for GET_LINE_CODING / SET_LINE_CODING class requests
typedef struct {
        uint32_t             dwDTERate;
        uint8_t              bCharFormat;
        uint8_t              bParityType;
        uint8_t              bDataBits;
} TLineCoding;

static TLineCoding LineCoding = {115200, 0, 0, 8};
static uint8_t abBulkBuf[64];
static uint8_t abClassReqData[8];
static volatile BOOL fBulkInBusy;
static volatile BOOL fChainDone;

static uint8_t txdata[VCOM_FIFO_SIZE];
static uint8_t rxdata[VCOM_FIFO_SIZE];

static fifo_t txfifo;
static fifo_t rxfifo;

// forward declaration of interrupt handler
static void USBIntHandler(void) __attribute__ ((interrupt("IRQ")));

static const uint8_t abDescriptors[] = {

// device descriptor
        0x12,
        DESC_DEVICE,
        LE_WORD(0x0101),                        // bcdUSB
        0x02,                                           // bDeviceClass
        0x00,                                           // bDeviceSubClass
        0x00,                                           // bDeviceProtocol
        MAX_PACKET_SIZE0,                       // bMaxPacketSize
        LE_WORD(0xFFFF),                        // idVendor
        LE_WORD(0x0005),                        // idProduct
        LE_WORD(0x0100),                        // bcdDevice
        0x01,                                           // iManufacturer
        0x02,                                           // iProduct
        0x03,                                           // iSerialNumber
        0x01,                                           // bNumConfigurations

// configuration descriptor
        0x09,
        DESC_CONFIGURATION,
        LE_WORD(67),                            // wTotalLength
        0x02,                                           // bNumInterfaces
        0x01,                                           // bConfigurationValue
        0x00,                                           // iConfiguration
        0xC0,                                           // bmAttributes
        0x32,                                           // bMaxPower
// control class interface
        0x09,
        DESC_INTERFACE,
        0x00,                                           // bInterfaceNumber
        0x00,                                           // bAlternateSetting
        0x01,                                           // bNumEndPoints
        0x02,                                           // bInterfaceClass
        0x02,                                           // bInterfaceSubClass
        0x01,                                           // bInterfaceProtocol, linux requires value of 1 for the cdc_acm module
        0x00,                                           // iInterface
// header functional descriptor
        0x05,
        CS_INTERFACE,
        0x00,
        LE_WORD(0x0110),
// call management functional descriptor
        0x05,
        CS_INTERFACE,
        0x01,
        0x01,                                           // bmCapabilities = device handles call management
        0x01,                                           // bDataInterface
// ACM functional descriptor
        0x04,
        CS_INTERFACE,
        0x02,
        0x02,                                           // bmCapabilities
// union functional descriptor
        0x05,
        CS_INTERFACE,
        0x06,
        0x00,                                           // bMasterInterface
        0x01,                                           // bSlaveInterface0
// notification EP
        0x07,
        DESC_ENDPOINT,
        INT_IN_EP,                                      // bEndpointAddress
        0x03,                                           // bmAttributes = intr
        LE_WORD(8),                                     // wMaxPacketSize
        0x0A,                                           // bInterval
// data class interface descriptor
        0x09,
        DESC_INTERFACE,
        0x01,                                           // bInterfaceNumber
        0x00,                                           // bAlternateSetting
        0x02,                                           // bNumEndPoints
        0x0A,                                           // bInterfaceClass = data
        0x00,                                           // bInterfaceSubClass
        0x00,                                           // bInterfaceProtocol
        0x00,                                           // iInterface
// data EP OUT
        0x07,
        DESC_ENDPOINT,
        BULK_OUT_EP,                            // bEndpointAddress
        0x02,                                           // bmAttributes = bulk
        LE_WORD(MAX_PACKET_SIZE),       // wMaxPacketSize
        0x00,                                           // bInterval
// data EP in
        0x07,
        DESC_ENDPOINT,
        BULK_IN_EP,                                     // bEndpointAddress
        0x02,                                           // bmAttributes = bulk
        LE_WORD(MAX_PACKET_SIZE),       // wMaxPacketSize
        0x00,                                           // bInterval
        
        // string descriptors
        0x04,
        DESC_STRING,
        LE_WORD(0x0409),

        0x0E,
        DESC_STRING,
        'L', 0, 'P', 0, 'C', 0, 'U', 0, 'S', 0, 'B', 0,

        0x14,
        DESC_STRING,
        'U', 0, 'S', 0, 'B', 0, 'S', 0, 'e', 0, 'r', 0, 'i', 0, 'a', 0, 'l', 0,

        0x12,
        DESC_STRING,
        'D', 0, 'E', 0, 'A', 0, 'D', 0, 'C', 0, '0', 0, 'D', 0, 'E', 0,

// terminating zero
        0
};


/**
        Local function to handle incoming bulk data
                
        @param [in] bEP
        @param [in] bEPStatus
 */
static void BulkOut(uint8_t bEP, uint8_t bEPStatus)
{
        int i, iLen;

        if (fifo_free(&rxfifo) < MAX_PACKET_SIZE) {
                // may not fit into fifo
                return;
        }

        // get data from USB into intermediate buffer
        iLen = USBHwEPRead(bEP, abBulkBuf, sizeof(abBulkBuf));
        for (i = 0; i < iLen; i++) {
                // put into FIFO
                if (!fifo_put(&rxfifo, abBulkBuf[i])) {
                        // overflow... :(
                        ASSERT(FALSE);
                        break;
                }
        }
}


/**
        Sends the next packet in chain of packets to the host

        @param [in] bEP
        @param [in] bEPStatus
 */
static void SendNextBulkIn(uint8_t bEP, BOOL fFirstPacket)
{
        int iLen;

        // this transfer is done
        fBulkInBusy = FALSE;

        // first packet?
        if (fFirstPacket) {
                fChainDone = FALSE;
        }

        // last packet?
        if (fChainDone) {
                return;
        }

        // get up to MAX_PACKET_SIZE bytes from transmit FIFO into intermediate buffer
        for (iLen = 0; iLen < MAX_PACKET_SIZE; iLen++) {
                if (!fifo_get(&txfifo, &abBulkBuf[iLen])) {
                        break;
                }
        }

        // send over USB
        USBHwEPWrite(bEP, abBulkBuf, iLen);
        fBulkInBusy = TRUE;

        // was this a short packet?
        if (iLen < MAX_PACKET_SIZE) {
                fChainDone = TRUE;
        }
}


/**
        Local function to handle outgoing bulk data

        @param [in] bEP
        @param [in] bEPStatus
 */
static void BulkIn(uint8_t bEP, uint8_t bEPStatus)
{
        SendNextBulkIn(bEP, FALSE);
}


/**
        Local function to handle the USB-CDC class requests
                
        @param [in] pSetup
        @param [out] piLen
        @param [out] ppbData
 */
static BOOL HandleClassRequest(TSetupPacket *pSetup, int *piLen, uint8_t **ppbData)
{
        switch (pSetup->bRequest) {

        // set line coding
        case SET_LINE_CODING:
DBG(UART0,"SET_LINE_CODING\n");
                memcpy((uint8_t *)&LineCoding, *ppbData, 7);
                *piLen = 7;
DBG(UART0,"dwDTERate=%u, bCharFormat=%u, bParityType=%u, bDataBits=%u\n",
        LineCoding.dwDTERate,
        LineCoding.bCharFormat,
        LineCoding.bParityType,
        LineCoding.bDataBits);
                break;

        // get line coding
        case GET_LINE_CODING:
DBG(UART0,"GET_LINE_CODING\n");
                *ppbData = (uint8_t *)&LineCoding;
                *piLen = 7;
                break;

        // set control line state
        case SET_CONTROL_LINE_STATE:
                // bit0 = DTR, bit = RTS
DBG(UART0,"SET_CONTROL_LINE_STATE %X\n", pSetup->wValue);
                break;

        default:
                return FALSE;
        }
        return TRUE;
}


/**
        Initialises the VCOM port.
        Call this function before using VCOM_putchar or VCOM_getchar
 */
void VCOM_init(void) {
        fifo_init(&txfifo, txdata);
        fifo_init(&rxfifo, rxdata);
        fBulkInBusy = FALSE;
        fChainDone  = TRUE;
}


/**
        Writes one character to VCOM port
        
        @param [in] c character to write
        @returns character written, or EOF if character could not be written
 */
int VCOM_putchar(int c)
{
        return fifo_put(&txfifo, c) ? c : EOF;
}


/**
        Reads one character from VCOM port
        
        @returns character read, or EOF if character could not be read
 */
int VCOM_getchar(void)
{
        uint8_t c;
        
        return fifo_get(&rxfifo, &c) ? c : EOF;
}


/**
        Interrupt handler
        
        Simply calls the USB ISR, then signals end of interrupt to VIC
 */
static void USBIntHandler(void)
{
        USBHwISR();
        VICAddress = 0x00;    // dummy write to VIC to signal end of ISR       
}

/**
        USB frame interrupt handler
        
        Called every milisecond by the hardware driver.
        
        This function is responsible for sending the first of a chain of packets
        to the host. A chain is always terminated by a short packet, either a
        packet shorter than the maximum packet size or a zero-length packet
        (as required by the windows usbser.sys driver).

 */
static void USBFrameHandler(uint16_t wFrame) {
        if (!fBulkInBusy && (fifo_avail(&txfifo) != 0)) {
                // send first packet
                SendNextBulkIn(BULK_IN_EP, TRUE);
        }
}


/**
        USB device status handler
        
        Resets state machine when a USB reset is received.
 */
static void USBDevIntHandler(uint8_t bDevStatus)
{
        if ((bDevStatus & DEV_STATUS_RESET) != 0) {
                fBulkInBusy = FALSE;
        }
}
static void W4DevInt(uint32_t dwIntr)
{
    // wait for specific interrupt
    while ((USBDevIntSt & dwIntr) != dwIntr);
    // clear the interrupt bits
    USBDevIntClr = dwIntr;
}
void usb_readdeverror(){
	// write command code
	USBDevIntClr = DEV_STAT;
	// clear CDFULL/CCEMTY
	USBDevIntClr = CDFULL | CCEMTY;
	// write command code
	USBCmdCode = 0x00000500 | (0xFF << 16);
	W4DevInt(CCEMTY);
	// get data
	USBCmdCode = 0x00000200 | (0xFF << 16);
	W4DevInt(CDFULL);

	printf_lpc(UART0,"Error(FF) USBCmdData:\t0x%x\n",USBCmdData);
}

void usb_readdevstatus(){
	// write command code
	USBDevIntClr = DEV_STAT;
	// clear CDFULL/CCEMTY
	USBDevIntClr = CDFULL | CCEMTY;
	// write command code
	USBCmdCode = 0x00000500 | (0xFE << 16);
	W4DevInt(CCEMTY);
	// get data
	USBCmdCode = 0x00000200 | (0xFE << 16);
	W4DevInt(CDFULL);

	printf_lpc(UART0,"Status(FE) USBCmdData:\t0x%x\n",USBCmdData);
}



void usb_init (void) {
	/* clock is configured in lpc23xx-pll.c/.h */
	PCONP |= 0x80000000;                    /* USB PCLK -> enable USB Per.  */
#ifdef LPC2378_PORTB
	USBClkCtrl = (1 << 1) | (1 << 3) | (1 << 4); /* Enable the clocks */
	while (!(USBClkSt & ((1 << 1) | (1 << 3) | (1 << 4))));

	USBPortSelect = 0x3; /* Set LPC to use USB Port B pins */

	USBClkCtrl = (USBClkCtrl & ~(1<<3)); /* p 359 of user manual */
#else
	USBClkCtrl = (1 << 1) | (1 << 4); /* Enable the clocks */
	while (!(USBClkSt & ((1 << 1) | (1 << 4))));
#endif

	/* LPC23xx user manual v3 p 321 */
#ifdef LPC2378_PORTB
	PINSEL1  = (PINSEL1 & ~(3 << 30))   | (1 << 30);   // USB_D+2
	PINSEL3  = (PINSEL3 & ~(3 << 28))   | (2 << 28);   // V_bus
	PINMODE3 = (PINMODE3 & ~(3 << 28))  | (2 << 28);   // disable pullup on V_bus p359 user manual
			/*
			 * Due to a bug in the LPC23xx chips, the connection functionality must be
			 * simulated using GPIO. This is only true for rev '-'
			 * NXP ES_LPC2378 Errata sheet Rev. 10 20 April 2011
			 */
	PINSEL0 = (PINSEL0 & ~((3 << 26) | (3 << 28))) | (1 << 26) | (0b10 << 28); // USB_UP_LED2, USB_CONNECT_LED2
	//printf_lpc(UART0,"pinsel0: 0x%x\n",PINSEL0);
#else

	PINSEL1 &= ~0x3C000000;                 /* P0.29 USB1_D+, P0.30 USB1_D- */
	PINSEL1 |=  0x14000000;                 /* PINSEL1 26.27, 28.29         */

	PINSEL3 &= ~0x30000030;                 /* P1.18 GoodLink, P1.30 VBus   */
	PINSEL3 |=  0x20000010;                 /* PINSEL3 4.5, 28.29           */
	PINSEL4 &= ~0x000C0000;
	FIO2DIR |= (1 << 9);
	FIO2CLR  = (1 << 9);

#endif



 // OTG_CLK_CTRL = 0x12;	                  /* Dev clock, AHB clock enable  */
 // while ((OTG_CLK_STAT & 0x12) != 0x12);

  VICVectAddr22 = (unsigned long)USBDevIntHandler; /* USB Interrupt -> Vector 22   */
  VICVectPriority22 = 0x01;
  VICIntEnable = 1 << 22;                 /* Enable USB Interrupt         */

  USBHwConnect(TRUE);

}

void usb_task() {
        char c;

        usb_init();

        // register descriptors
    /*    USBRegisterDescriptors(abDescriptors);

     // register class request handler
        USBRegisterRequestHandler(REQTYPE_TYPE_CLASS, HandleClassRequest, abClassReqData);

        // register endpoint handlers
        USBHwRegisterEPIntHandler(INT_IN_EP, NULL);
        USBHwRegisterEPIntHandler(BULK_IN_EP, BulkIn);
        USBHwRegisterEPIntHandler(BULK_OUT_EP, BulkOut);

        // register frame handler
        USBHwRegisterFrameHandler(USBFrameHandler);

        // register device event handler
        USBHwRegisterDevIntHandler(USBDevIntHandler);
*/
        // initialise VCOM
        VCOM_init();

        USBHwConnect(TRUE);

        vic_enableIRQ();

        printf_lpc(UART0,"Starting loop\n");
        // echo any character received (do USB stuff in interrupt)
        while (1) {
        	usb_readdevstatus();
        	 usb_readdeverror();
                c = VCOM_getchar();
                if (c != EOF) {
                        // show on console
                        if ((c == 9) || (c == 10) || (c == 13) || ((c >= 32) && (c <= 126))) {
                                DBG(UART0,"%c", c);
                        }
                        else {

                        	stat_led_flash_fast(2);
                           //     DBG(UART0,".");
                        }
                        VCOM_putchar(c);
                }
        }
}


/*
 * main
 */
int main() {

//    pllstart_seventytwomhz() ;
    pllstart_fourtyeightmhz() ;
    uart0_init_115200() ;
    mam_enable();
    SCS |= 1;
    printf_lpc(UART0,"\n***Starting olimex usb-echo test***\n\n");

    stat_led_flash(3); // initial visual check

    usb_task();

    stat_led_flash(3);
    printf_lpc(UART0,"\n\n***Done***\n\n");
    return 0;
}

