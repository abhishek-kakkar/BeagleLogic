/*
 * PRU0 Firmware for BeagleLogic
 * 
 * Copyright (C) 2014 Kumar Abhishek
 *
 * Licensed under the GNU GPL and provided "as-is" without any expressed 
 * or implied warranties / liabilities.
 */

/* We are compiling for PRU0 */
#define PRU0

#include "pru_syscalls.h"
#include "pru_defs.h"

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
#define MAX_SG_ENTRIES	128

/* Define the downcall API */
#define DC_GET_VERSION  0   /* Firmware */
#define DC_GET_MAX_SG	1   /* Get the Max number of SG entries */
#define DC_GET_CXT_PTR	2   /* Get the context pointer */
#define DC_SM_RATE	3   /* Get/set rate = (200 / n) MHz, n = 2... */
#define DC_SM_TRIGGER	4   /* RFU */
#define DC_SM_ARM	7   /* Arm the LA (start sampling) */
/* To abort sampling, just halt both the PRUs and
 * read R29 for total byte count */

/* Define magic bytes for the structure. This "looks like" BEAGLELO */
#define FW_MAGIC	0xBEA61E10

/* Just a basic structure describing the start and end buffer addresses */
typedef struct sglist {
	u32 dma_start_addr;
	u32 dma_end_addr;
} scatterlist;

/* Structure describing the context */
struct capture_context {
	u32 magic;
	u32 errorCode;

	u32 interrupt1count;

	scatterlist list[MAX_SG_ENTRIES];
} cxt = {0};

u32 state_run = 0;

extern void sc_downcall(int (*handler)(u32 nr, u32 arg0, u32 arg1, 
	u32 arg2, u32 arg3, u32 arg4));

static void resume_other_pru(void) {
	u32 i;

	i = (u16)PCTRL_OTHER(0x0000);
	i |= (((u16)PCTRL_OTHER(0x0004) + 1) << 16) | CONTROL_ENABLE;
	i &= ~CONTROL_SOFT_RST_N;
	PCTRL_OTHER(0x0000) = i;
}

static int wait_other_pru_timeout(u32 timeout) {
	do {
		if ((PCTRL_OTHER(0x0000) & CONTROL_RUNSTATE) == 0)
			return 0;
	} while (timeout--);
	return -1;
}

/* Write the registers of PRU1 (samplerate and sample and enable it */
int configure_capture(u32 samplerate, u32 sampleunit, u32 triggerflags,
		u32 reserved0, u32 reserved1) {
	u32 i;
	/* Soft-reset PRU1 and wait for it to halt */
//	PCTRL_OTHER(0x0000) &= ~;

	/* Verify if PRU1 is indeed halted and waiting for us */
	if (wait_other_pru_timeout(200))
		return -1;

	/* Verify that it is indeed our firmware */
	i = pru_other_read_reg(2);
	if (i != FW_MAGIC)
		return -1;

	/* All clear, now write the configuration bits */
	pru_other_write_reg(2, samplerate);
	pru_other_write_reg(3, sampleunit);
	//state_run = triggerflags;

	/* Resume over the HALT instruction */
	resume_other_pru();

	__delay_cycles(10);

	if (wait_other_pru_timeout(200))
		return -1;


	/* Now the other PRU should be ready to take instructions */
	return 0;
}



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
			return 0;

		case DC_SM_TRIGGER:
			/* Write configuration bits */
			return configure_capture(arg0, arg1, arg2, arg3, arg4);

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
	int i;

	/* Enable OCP Master Port */
	PRUCFG_SYSCFG &= ~SYSCFG_STANDBY_INIT;

	cxt.magic = FW_MAGIC;

	while (1) {

		poll_downcall();
		
		if (state_run == 1) {

			PINTC_SECR0 = 0xFFFFFFFF;
			resume_other_pru();
			run(&cxt);

			/* Clear pending interrupt */
			PINTC_SICR = SYSEV_ARM_TO_PRU1;

			while (!(PINTC_SRSR0 & BIT(SYSEV_VR_PRU0_TO_ARM)));
			while ((PINTC_SRSR0 & BIT(SYSEV_VR_PRU0_TO_ARM)));

			SIGNAL_EVENT(SYSEV_VR_PRU1_TO_ARM);

			/* Reset PRU1 so that it can register again correctly */
			PCTRL_OTHER(0x0000) &= (u16)~CONTROL_SOFT_RST_N;
			state_run = 0;
		}
	}
}

