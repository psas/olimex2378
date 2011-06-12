
/*
 * led-test.h
 */


#ifndef _LED_TEST_H
#define _LED_TEST_H

#define FIO_ENABLE       ( SCS |= 1 )
#define STAT_LED_ENABLE  ( FIO1DIR |= (1<<19) )
#define STAT_LED_DISABLE ( FIO1DIR = FIO1DIR & (~(1<<19)) )

#define STAT_LED_ON      ( FIO1CLR = (1 << 19) )
#define STAT_LED_OFF     ( FIO1SET = (1 << 19) )

#endif

