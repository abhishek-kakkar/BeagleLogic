/* PRU0 Firmware for BeagleLogic
 * 
 * Copyright (C) 2014 Kumar Abhishek
 *
 * Licensed under the GNU GPL and provided "as-is" without any expressed 
 * or implied warranties / liabilities.
 */


#include "pru_defs.h"

typedef unsigned int u32;

volatile register unsigned int __R30;
volatile register unsigned int __R31;

extern void sc_downcall(int (*handler)(u32 nr, u32 arg0, u32 arg1, 
	u32 arg2, u32 arg3, u32 arg4));

static int handle_downcall(u32 id, u32 arg0, u32 arg1, u32 arg2,
		u32 arg3, u32 arg4)
{
	return 0;
}

static int poll_downcall(void)
{
	if( PINTC_SRSR0 & (1 << 21) ){
		PINTC_SICR = 21;
		sc_downcall(handle_downcall);
	}
	return 0;
}

struct samplePacket {
	char s[32];
	u32 sampleCount;
};

int main()
{
	struct samplePacket samples;
	
	while(1)
	{
		if (poll_downcall())
			break;
	}
	
	while (1) {
	
		/* WIP */
		
	}
	return 0;
}

