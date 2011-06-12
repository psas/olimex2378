
/*
 * led-test.c
 */

#include <limits.h>
#include <stdint.h>

#include "lpc23xx.h"

#include "lpc23xx-pll.h"
#include "lpc23xx-uart.h"

#include "olimex2378-util.h"
#include "led-test.h"

int main (void) {

    int32_t cycles = 16;

    pllstart_seventytwomhz() ;
 //   pllstart_sixtymhz() ;
 //   pllstart_fourtyeightmhz() ;

    uart0_init_115200() ;

    uart0_putstring("\n***Starting olimex led test***\n\n");

    stat_led_flash(cycles);

    // negative numbers in itoa
    uart0_putstring("Print a negative number: ");
    uart0_putstring(itoa(-42, 10));
    uart0_putstring("\n");

    uart0_putstring("Print a negative number: ");
    uart0_putstring(itoa(-42, 16));
    uart0_putstring("\n");


    uart0_putstring("0b");
    uart0_putstring(itoa(cycles,2));
    uart0_putstring(" cycles.\n");

    uart0_putstring("0d");
    uart0_putstring(itoa(cycles,10));
    uart0_putstring(" cycles.\n");

    uart0_putstring("0x");
    uart0_putstring(itoa(cycles,16));
    uart0_putstring(" cycles.\n");

    uart0_putstring("\n\n***Done***\n\n");

    return(0);

}



