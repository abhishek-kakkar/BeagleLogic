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

/* Maximum number of SG entries; each entry is 8 bytes */
#define MAX_BUFLIST_ENTRIES	128

/* Downcall API. To be kept in sync with the kernel driver */
#define DC_GET_VERSION  0   /* Firmware */
#define DC_GET_MAX_SG	1   /* Get the Max number of bufferlist entries */
#define DC_GET_CXT_PTR	2   /* Get the context pointer */
#define DC_SM_RATE	3   /* Get/set rate = (200 / n) MHz, n = 2... */
#define DC_SM_TRIGGER	4   /* Write configuration (samplerate/unit) */
#define DC_SM_ARM	7   /* Arm the LA (start sampling) */

/* Define magic bytes for the structure. This "looks like" BEAGLELO */
#define FW_MAGIC	0xBEA61E10

/* Just a basic structure describing the start and end buffer addresses */
typedef struct buflist {
	u32 dma_start_addr;
	u32 dma_end_addr;
} bufferlist;

/* Structure describing the context */
struct capture_context {
	u32 magic;
	u32 errorCode;

	u32 interrupt1count;

	bufferlist list[MAX_BUFLIST_ENTRIES];
} cxt = {0};

u16 state_run = 0;
short trigger_flags = -1;

extern void sc_downcall(int (*handler)(u32 nr, u32 arg0, u32 arg1, 
	u32 arg2, u32 arg3, u32 arg4));

static inline void resume_other_pru(void) {
	u32 i;

	i = (u16)PCTRL_OTHER(0x0000);
	i |= (((u16)PCTRL_OTHER(0x0004) + 1) << 16) | CONTROL_ENABLE;
	i &= ~CONTROL_SOFT_RST_N;
	PCTRL_OTHER(0x0000) = i;
}

static inline int wait_other_pru_timeout(u32 timeout) {
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

	/* Verify if PRU1 is indeed halted and waiting for us */
	if (wait_other_pru_timeout(200))
		return -1;

	/* Verify magic bytes */
	i = pru_other_read_reg(0);
	if (i != FW_MAGIC)
		return -1;

	/* All clear, now write the configuration bits */
	pru_other_write_reg(14, samplerate);
	pru_other_write_reg(15, sampleunit);
	trigger_flags = triggerflags;

	/* Resume over the HALT instruction, give it some time to configure */
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
			return MAX_BUFLIST_ENTRIES;

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

extern void run(struct capture_context *ctx, u32 trigger_flags);

int main(void)
{
	/* Enable OCP Master Port */
	PRUCFG_SYSCFG &= ~SYSCFG_STANDBY_INIT;
	cxt.magic = FW_MAGIC;
	while (1) {

		/* Poll for downcalls */
		if(PINTC_SRSR0 & BIT(SYSEV_ARM_TO_PRU0)) {
			PINTC_SICR = SYSEV_ARM_TO_PRU0;
			sc_downcall(handle_downcall);
		}
		
		/* Run triggered */
		if (state_run == 1) {
			/* Clear all pending interrupts */
			PINTC_SECR0 = 0xFFFFFFFF;

			resume_other_pru();
			run(&cxt, trigger_flags);

			/* Wait for the previous interrupts to be handled */
			while (!(PINTC_SRSR0 & BIT(SYSEV_VR_PRU0_TO_ARM)));
			while ((PINTC_SRSR0 & BIT(SYSEV_VR_PRU0_TO_ARM)));

			/* Signal completion */
			SIGNAL_EVENT(SYSEV_VR_PRU1_TO_ARM);

			/* Reset PRU1 and our state */
			PCTRL_OTHER(0x0000) &= (u16)~CONTROL_SOFT_RST_N;
			state_run = 0;
			trigger_flags = -1;
		}
	}
}

