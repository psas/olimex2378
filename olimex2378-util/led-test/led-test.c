
/*
 * led-test.c
 */

#include <limits.h>
#include <stdint.h>

#include "lpc23xx.h"

#include "lpc23xx-pll.h"
#include "lpc23xx-uart.h"
#include "lpc23xx-util.h"

#include "olimex2378-util.h"
#include "led-test.h"
/*
 * led_flash
 */
void led_flash(uint32_t cycles) {

    int x              = 0;
    int interval       = 1000000;

    Freq cclk;
    cclk  =  pllquery_cclk_mhz();

    switch(cclk) {
        case ZERO:
            interval = 100000;
            break;
        case FOURTY_EIGHT_MHZ:
            interval = 0.01 * 48000000;
            break;
        case SIXTY_MHZ:
            interval = 0.01 * 60000000;
            break;
        case SEVENTY_TWO_MHZ:
            interval = 0.01 * 72000000;
            break;
        default:
            interval = 100000;
            break;

    }

    FIO_ENABLE;
    P0_0_ENABLE;
    P0_1_ENABLE;
    P0_27_ENABLE;
    P0_28_ENABLE;

    if(cycles > 0) {
    	for(;;) {
    		if(cycles==0) break;
    		x++;
    		if (x == interval) {
    			--cycles;
    			P0_0_ON;
    			P0_1_ON;
    			P0_27_ON;
    			P0_28_ON;
    		} else if (x >= (interval * 2)) {
    			P0_0_OFF;
    			P0_1_OFF;
    			P0_27_OFF;
    			P0_28_OFF;
    			x = 0;
    		}
    	}
    }
}

int main (void) {

    int32_t cycles = 3;

    pllstart_seventytwomhz() ;
 //   pllstart_sixtymhz() ;
 //   pllstart_fourtyeightmhz() ;

    uart0_init_115200() ;

    uart0_putstring("\n***Starting olimex led test***\n\n");

    stat_led_flash_fast(cycles);
    led_flash(cycles);
    // negative numbers in itoa
    uart0_putstring("Print a negative number: ");
    uart0_putstring(util_itoa(-42, 10));
    uart0_putstring("\n");

    uart0_putstring("Print a negative number: ");
    uart0_putstring(util_itoa(-42, 16));
    uart0_putstring("\n");


    uart0_putstring("0b");
    uart0_putstring(util_itoa(cycles,2));
    uart0_putstring(" cycles.\n");

    uart0_putstring("0d");
    uart0_putstring(util_itoa(cycles,10));
    uart0_putstring(" cycles.\n");

    uart0_putstring("0x");
    uart0_putstring(util_itoa(cycles,16));
    uart0_putstring(" cycles.\n");

    uart0_putstring("\n\n***Done***\n\n");

    return(0);

}



