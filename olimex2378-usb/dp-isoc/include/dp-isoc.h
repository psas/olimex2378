

/*
 * dp-isoc.h
 */

#ifndef _DP_ISOC_H
#define _DP_ISOC_H

#define 	INT_IN_EP				0x81

#define 	ISOC_OUT_EP     		0x06
#define 	ISOC_IN_EP      		0x83
#define		MAX_PACKET_SIZE			128

#define 	BYTES_PER_ISOC_FRAME 	4

#define 	LE_WORD(x)				((x)&0xFF),((x)>>8)


__attribute__ ((aligned(4))) U32 inputIsocDataBuffer[(BYTES_PER_ISOC_FRAME/4)];

#endif
