
/*
 * blinkm-test.c
 */

#include <limits.h>
#include <stdint.h>

#include "lpc23xx.h"

#include "lpc23xx-pll.h"
#include "lpc23xx-binsem.h"
#include "lpc23xx-i2c.h"
#include "lpc23xx-uart.h"
#include "lpc23xx-util.h"

#include "olimex2378-util.h"
#include "blinkm-test.h"

i2c_master_xact_t       xact_s;


/*
 * xact_callback
 * callback function for i2c
 */
void xact_callback(i2c_master_xact_t* caller, i2c_master_xact_t* i2c_s) {
    uint16_t i;

    caller->i2c_ext_slave_address = i2c_s->i2c_ext_slave_address;
    caller->xact_success          = i2c_s->xact_success;
    caller->write_length          = i2c_s->write_length;
    caller->read_length           = i2c_s->read_length;
    caller->xact_active           = i2c_s->xact_active;
 
    uart0_putstring("In callback\n");
    for(i=0; i<I2C_MAX_BUFFER; ++i) {
        caller->i2c_tx_buffer[i] = i2c_s->i2c_tx_buffer[i];
        caller->i2c_rd_buffer[i] = i2c_s->i2c_rd_buffer[i];
    }

   uart0_putstring("xact_success is: ");
    uart0_putstring(util_uitoa(caller->xact_success, 10));
    uart0_putchar('\n');
    /* maybe trigger an interrupt here */
}

/*
 * blinkm_task
 * 
 */
void blinkm_task() {

    xact_s.i2c_tx_buffer[0] =  i2c_create_write_address(BLINKM_ADDR);
    xact_s.i2c_tx_buffer[1] =  'o';
    xact_s.i2c_tx_buffer[2] =  'n';
    xact_s.i2c_tx_buffer[3] =  0xbd;
    xact_s.i2c_tx_buffer[4] =  0x10;
    xact_s.i2c_tx_buffer[5] =  0x03;
    xact_s.write_length     =  0x06;
    xact_s.read_length      =  0x0;
    start_i2c0_master_xact(&xact_s, &xact_callback);

    uart0_putstring("past start xaction write.\n\n");

    if(xact_s.xact_success == 1) {
        uart0_putstring("i2c write xaction success.\n");
    } else {
        uart0_putstring("i2c write xaction fail.\n");
    }

    uart0_putstring("i2c Read Color Task...\n");

    // xact_s.I2C_TX_buffer[1] =  'Z';
    //
    xact_s.i2c_tx_buffer[0] =  i2c_create_write_address(BLINKM_ADDR);
    xact_s.i2c_tx_buffer[1] =  'g';
    xact_s.write_length     =  0x02;
    xact_s.i2c_tx_buffer[2] =  i2c_create_read_address(BLINKM_ADDR);
    xact_s.read_length      =  0x02;
    start_i2c0_master_xact(&xact_s, &xact_callback);

//    uart0_putstring("... ");
//    asm volatile ("nop\n");
 
    if(xact_s.xact_success == 1) {
        uart0_putstring("i2c read xaction success.\n");
    } else {
        uart0_putstring("i2c read xaction fail.\n");
    }

    uart0_putstring("Read data 0 is 0x");
    uart0_putstring(util_uitoa(xact_s.i2c_rd_buffer[0],16));
    uart0_putstring("\n");
    uart0_putstring("Read data 1 is 0x");
    uart0_putstring(util_uitoa(xact_s.i2c_rd_buffer[1],16));
    uart0_putstring("\n");
    uart0_putstring("Read data 2 is 0x");
    uart0_putstring(util_uitoa(xact_s.i2c_rd_buffer[2],16));
    uart0_putstring("\n");
}


int main (void) {

    int32_t cycles = 5;

    pllstart_seventytwomhz() ;
    //   pllstart_sixtymhz() ;
    //   pllstart_fourtyeightmhz() ;

    MAMCR  = 0x0;
    MAMTIM = 0x4;
    MAMCR  = 0x2;

    uart0_init_115200() ;

    uart0_putstring("\n***Starting olimex blinkm test***\n\n");

    i2c_init(I2C0);

    uart0_putstring("cpsr is:\t0b");
    uart0_putstring(util_uitoa(__get_cpsr(),2));
    uart0_putstring("\n");

    stat_led_flash(cycles); // initial visual check

    blinkm_task() ;

    stat_led_flash_slow(2);

    uart0_putstring("\n\n***Done***\n\n");

    return(0);

}

