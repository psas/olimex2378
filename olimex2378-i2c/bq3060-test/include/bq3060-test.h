
/*
 * bq3060-test.h
 */

#ifndef _BQ3060_TEST_H
#define _BQ3060_TEST_H

#include "lpc23xx-i2c.h"

#define BQ3060_ADDR            0x16  // 0b0001_0110
#define BQ3060_POLL_WAITTICKS  500000

extern i2c_master_xact_t       xact_s;

void xact_callback(i2c_master_xact_t* caller, i2c_master_xact_t* i2c_s) ;

#endif

