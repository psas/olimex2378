
/*
 * olimex2378-util.c
 */

#include <stdint.h>
#include <string.h>

#include "lpc23xx.h"

#include "lpc23xx-pll.h"
#include "lpc23xx-uart.h"

#include "led-test.h"

#include "olimex2378-util.h"

/*
 * init_stat_led
 */
void init_stat_led() {
    FIO_ENABLE;
    STAT_LED_ENABLE;
}

/*
 * stat_led_flash_slow
 */
void stat_led_flash_slow(uint32_t cycles) {

    int x              = 0;
    int interval       = 100000;

    Freq cclk;
    cclk  =  pllquery_cclk_mhz();

    switch(cclk) {
        case ZERO:
            interval = 100000;
        case FOURTY_EIGHT_MHZ:
            interval = 0.01 * 48000000;
        case SIXTY_MHZ:
            interval = 0.01 * 60000000;
        case SEVENTY_TWO_MHZ:
            interval = 0.01 * 72000000;
    }

    FIO_ENABLE;
    STAT_LED_ENABLE;

    if(cycles > 0) {
        for(;;) {
            if(cycles==0) break;
            x++;
            if (x == interval) {
               //  uart0_putstring(".\n");
                --cycles;
                STAT_LED_ON;
            } else if (x >= (interval * 2)) {
                STAT_LED_OFF;
                x = 0;
            }
        }
    }
}

/*
 * stat_led_flash
 */
void stat_led_flash(uint32_t cycles) {

    int x              = 0;
    int interval       = 1000000;

    Freq cclk;
    cclk  =  pllquery_cclk_mhz();

    switch(cclk) {
        case ZERO:
            interval = 100000;
        case FOURTY_EIGHT_MHZ:
            interval = 0.001 * 48000000;
        case SIXTY_MHZ:
            interval = 0.001 * 60000000;
        case SEVENTY_TWO_MHZ:
            interval = 0.001 * 72000000;
    }

    FIO_ENABLE;
    STAT_LED_ENABLE;

    if(cycles > 0) {
        for(;;) {
            if(cycles==0) break;
            x++;
            if (x == interval) {
                --cycles;
                STAT_LED_ON;
            } else if (x >= (interval * 2)) {
                STAT_LED_OFF;
                x = 0;
            }
        }
    }
}

