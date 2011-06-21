


/*
 * olimex2378-util.h
 */

#ifndef _OLIMEX2378_UTIL
#define _OLIMEX2378_UTIL

#include "lpc23xx.h"

#define MAX_I2ASTRING    33

#define FIO_ENABLE       ( SCS |= 1 )
#define STAT_LED_ENABLE  ( FIO1DIR |= (1<<19) )
#define STAT_LED_DISABLE ( FIO1DIR = FIO1DIR & (~(1<<19)) )

#define STAT_LED_ON      ( FIO1CLR = (1 << 19) )
#define STAT_LED_OFF     ( FIO1SET = (1 << 19) )

void init_stat_led() ;
void stat_led_flash(uint32_t cycles) ;

#endif
