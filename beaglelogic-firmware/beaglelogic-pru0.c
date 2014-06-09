/*
 * PRU0 Firmware for BeagleLogic
 * 
 * Copyright (C) 2014 Kumar Abhishek
 *
 * Licensed under the GNU GPL and provided "as-is" without any expressed 
 * or implied warranties / liabilities.
 */

#include "pru_defs.h"
#include "pru_syscalls.h"

/* We are compiling for PRU0 */
#define PRU0

/*
 * Define firmware version
 * This is version 0.2 [v0.1 was the PASM firmware]
 */
#define MAJORVER	0
#define MINORVER	2

/*
 * Maximum number of SG entries; each entry contains
 * start address and end address.
 */
#define MAX_SG_ENTRIES	32

/* Define the downcall API */
#define DC_GET_VERSION	0	/* Firmware */
#define DC_GET_MAX_SG   1   /* Get the Max number of SG entries */
#define DC_GET_CXT_PTR  2   /* Get the context pointer */
#define DC_SM_RATE      3   /* Get/set rate = (200 / n) MHz, n = 2, ... */
#define DC_SM_TRIGGER	4   /* RFU */
#define DC_SM_ARM       5	/* Arm the LA (start sampling) */
/* To abort sampling, just halt both the PRUs and
 * read R29 for total byte count */

/* Just a basic structure describing the start and end buffer addresses */
typedef struct sglist {
	u32 dma_start_addr;
	u32 dma_end_addr;
} scatterlist;

/* Structure describing the context */
struct capture_context {
	u32 sampleRate;
	u32 errorCode;

	u32 interrupt1count;

	scatterlist list[MAX_SG_ENTRIES];
} cxt;

u32 state_run = 0;

extern void sc_downcall(int (*handler)(u32 nr, u32 arg0, u32 arg1, 
	u32 arg2, u32 arg3, u32 arg4));

/* Handle downcalls */
static int handle_downcall(u32 id, u32 arg0, u32 arg1, u32 arg2,
		u32 arg3, u32 arg4) {
	switch (id)
	{
		case DC_GET_VERSION:
			return (MINORVER | (MAJORVER << 8));

		case DC_GET_MAX_SG:
			return MAX_SG_ENTRIES;

		case DC_GET_CXT_PTR:
			return (int)&cxt;

		case DC_SM_RATE:
			if (arg0 != 0)
				cxt.sampleRate = arg0;
			return cxt.sampleRate;

		case DC_SM_TRIGGER:
			return -1;	// RFU

		case DC_SM_ARM:
			state_run = 1;
			return 0;
	}

	return -1;
}

static int poll_downcall(void)
{
	/* Check if a downcall request is received */
	if(PINTC_SRSR0 & BIT(SYSEV_ARM_TO_PRU0)){
		PINTC_SICR = SYSEV_ARM_TO_PRU0;

		sc_downcall(handle_downcall);
		if (state_run == 1)
			return 0;
	}
	return 1;
}

extern void run(struct capture_context *ctx);

int main(void)
{
	/* Enable OCP Master Port */
	PRUCFG_SYSCFG &= ~SYSCFG_STANDBY_INIT;

	state_run = 0;

	while (1) {

		poll_downcall();
		
		if (state_run == 1) {
			__delay_cycles(16777216);
			SIGNAL_EVENT(SYSEV_VR_PRU0_TO_ARM);
			cxt.interrupt1count++;

			__delay_cycles(16777216);
			SIGNAL_EVENT(SYSEV_VR_PRU1_TO_ARM);
			cxt.interrupt1count++;
		}
	}
}

