
/*
 * bq3060-test.c
 * TI BQ3060 Gas Gauge (Rev. A)
 */

#include <limits.h>
#include <stdint.h>

#include "lpc23xx.h"

#include "lpc23xx-pll.h"
#include "lpc23xx-binsem.h"
#include "lpc23xx-i2c.h"
#include "lpc23xx-uart.h"
#include "lpc23xx-vic.h"
#include "lpc23xx-util.h"
#include "printf-lpc.h"

#include "olimex2378-util.h"
#include "bq3060-test.h"

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

    for(i=0; i<I2C_MAX_BUFFER; ++i) {
        caller->i2c_tx_buffer[i] = i2c_s->i2c_tx_buffer[i];
        caller->i2c_rd_buffer[i] = i2c_s->i2c_rd_buffer[i];
    }
    /* maybe trigger an interrupt here */
}

void poll_wait() {
    uint32_t i, on;

    STAT_LED_OFF;
    i  = 0;
    on = 0;

    while(is_binsem_locked(&i2c1_binsem_g)== 1) {
        i++;
        if(i % BQ3060_POLL_WAITTICKS == 0) {
            if(on==0) {
                STAT_LED_ON;
                on = 1;
            } else {
                on = 0;
                STAT_LED_OFF;
            }
            i = 0;
        }
    }
}


/*
 * bq3060_task
 */
void bq3060_task() {

    uint32_t i;
    uint16_t device_type;

    /**************************************/
    i2c_init_state( &xact_s) ;

    uart0_putstring("Read Device Type...\n");

    xact_s.i2c_tx_buffer[0] =  i2c_create_write_address(BQ3060_ADDR);
    xact_s.i2c_tx_buffer[1] =  0x0;
    xact_s.i2c_tx_buffer[2] =  0x1;
    xact_s.i2c_tx_buffer[3] =  i2c_create_read_address(BQ3060_ADDR);
    xact_s.write_length     =  0x3;
    xact_s.read_length      =  0x2;
    xact_s.xact_active      =  0x1;
    xact_s.xact_success     =  0x0;

    start_i2c1_master_xact(&xact_s, &xact_callback);

    poll_wait();

    if(xact_s.xact_success == 1) {
        uart0_putstring("bq3060 i2c read device type xaction success.\n");
    } else {
        uart0_putstring("eeprom i2c read device type xaction fail.\n");
    }

    device_type = 0;
    for (i=0; i<xact_s.read_length; ++i) {
        device_type = device_type | (xact_s.i2c_rd_buffer[i] << 8*i);
    }

    printf_lpc(UART0,"Device type is: 0x%x\n",device_type);
    if(device_type != 0x0900) printf_lpc(UART0,"Error, wrong value, should be 0x0900\n"); 

}


int main (void) {

    int32_t cycles = 3;

    pllstart_seventytwomhz() ;
    //pllstart_sixtymhz() ;
    //pllstart_fourtyeightmhz() ;

    MAMCR  = 0x0;
    MAMTIM = 0x4;
    MAMCR  = 0x2;

    uart0_init_115200() ;

    vic_enableIRQ();
    vic_enableFIQ();

    uart0_putstring("\n***Starting olimex2378 bq3060 test***\n\n");

    i2c_init(I2C1);

    stat_led_flash_fast(cycles); // initial visual check

    bq3060_task() ;

    stat_led_flash_fast(4);

    uart0_putstring("\n\n***Done***\n\n");

    return(0);
}

