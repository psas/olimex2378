
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
 * uitoa - unsigned int to ascii?
 */
/*
 * itoa
 */
char* itoa(int32_t val, uint32_t base) {

    static char ibuf[MAX_I2ASTRING];

    int         i     = MAX_I2ASTRING-2;
    int         minus = 0;

    memset(ibuf,'\0',MAX_I2ASTRING);

    if(val < 0) {
        val   = -val;
        minus = 1;
    }

    if(val == 0) { 
        ibuf[0] = '0';
        return &ibuf[0];
    } else {
        for(; val && i ; --i, val /= base) {
            ibuf[i] = "0123456789abcdef"[val % base];
        }

        if(minus == 1 && base == 10) {
            ibuf[i] = '-';
            return &ibuf[i];
        } else {
            return &ibuf[i+1];
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

