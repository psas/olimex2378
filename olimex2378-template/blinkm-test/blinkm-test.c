
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

int main (void) {

    int32_t cycles = 5;

    pllstart_seventytwomhz() ;
 //   pllstart_sixtymhz() ;
 //   pllstart_fourtyeightmhz() ;

    uart0_init_115200() ;

    uart0_putstring("\n***Starting olimex blinkm test***\n\n");

    stat_led_flash(cycles+10); // initial visual check








    stat_led_flash_slow(cycles);

    uart0_putstring("\n\n***Done***\n\n");

    return(0);

}

