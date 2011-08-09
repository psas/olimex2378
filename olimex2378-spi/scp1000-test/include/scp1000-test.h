
/*
 * scp1000-test.h
 */

#ifndef _SCP1000_TEST_H
#define _SCP1000_TEST_H

/* gpio pinsel */
#define GPIO_1_21     10

// connect scp1000D01 breakout board power to GPIO so software
// can reset. There is no other way to reset the device.
//

// Choose GPIO P1.21 for POWER
// Pinned to EXT2-15 on olimex
#define SCP1000_PWR_PINSEL        (PINSEL3  = ((PINSEL3  & ~(0x3 << GPIO_1_21)) | (0x0 << GPIO_1_21)))

#define SCP1000_PWR_NOPULL        (PINMODE3 = ((PINMODE3 & ~(0x3 << GPIO_1_21)) | (0x2 << GPIO_1_21)))

#define SCP1000_PWR_EN            (FIO1DIR = (1<<21))
#define SCP1000_PWR_ON            (FIO1SET = (1<<21))
#define SCP1000_PWR_OFF           (FIO1CLR = (1<<21))

// scp1000 registers
typedef enum { REVID      =0x0,
               DATAWR     =0x1 ,
               ADDPTR     =0x2 ,
               OPERATION  =0x3 ,
               OPSTATUS   =0x4 ,
               RSTR       =0x6 ,
               STATUS     =0x7 ,
               DATARD8    =0x1F,
               DATARD16   =0x20,
               TEMPOUT    =0x21 } scp_regaddr;

#endif

