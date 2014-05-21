/*
 * PRULATest.c
 *
 * Testbed for PRU application
 *
 * This file is a part of the BeagleLogic Project
 * Copyright (C) 2014 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 *
 * This file is free software under GPL v3+
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include <sys/mman.h>

#include <time.h>

void *extmem, *pru0ram, *pru1ram;

#include "PRUTestFirmware_bin.h"

void load_firmware(char *name) {
	/* Look in normal directory first, else use */
}

int main(void) {

	int ret, i;
	uint32_t *ptr;

	struct timespec ts;

	tpruss_intc_initdata initdata = PRUSS_INTC_INITDATA;

	puts("PRU Application test\n"); /* print Hello, world */

	/* Set up the PRU */
	prussdrv_init();

	/* Set up EVTOUT0 and EVTOUT1 */
	if ((ret = prussdrv_open(PRU_EVTOUT_0)) != 0) {
		printf("prussdrv_open failed! Error code=%d", ret);
		return ret;
	}

	if ((ret = prussdrv_open(PRU_EVTOUT_1)) != 0) {
		printf("prussdrv_open failed! Error code=%d", ret);
		return ret;
	}

	/* Initialize INTC */
	prussdrv_pruintc_init(&initdata);

	/* Map external (DDR) memory to the PRU. We have already configured
	 * UIO driver with extram_pool_sz
	 */
	prussdrv_map_extmem(&extmem);
	prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pru0ram);
	prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &pru1ram);

	// Write 16 random bytes into PRU1's memory
	// If fw is compiled with TEST on, These will be
	// transferred via the broadside to PRU0 and then
	// the implementation can be verified
	clock_gettime(CLOCK_REALTIME, &ts);
	srandom(ts.tv_nsec);
	for (i = 0; i < 8; i++) {
		((uint32_t *)(pru1ram + 12))[i] = random();
	}

	// Load the firmware into the PRUs
	if (prussdrv_exec_program(0, "./pru0fw.bin") == 0) {
		puts("Loaded PRU firmware for PRU0");
	}

	if (prussdrv_exec_program(1, "./pru1fw.bin") == 0) {
		puts("Loaded PRU firmware for PRU1");
	}

	// Wait for interrupt and receive data
	ret = prussdrv_pru_wait_event(PRU_EVTOUT_0);
	prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
	printf("Received Interrupt from PRU0 \n");

//	ret = prussdrv_pru_wait_event(PRU_EVTOUT_1);
//	prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU1_ARM_INTERRUPT);
//	printf("PRU0 Received Interrupt from PRU1 \n");

	// Print the received data
	if (pru0ram != 0 && pru1ram != 0) {
		ptr = pru0ram;
		printf("PRU0 Cycle count starts at: %d\n", *ptr++);
		printf("Cycle count after data reception:%d \n", *ptr++);
		printf("Stall count after data reception:%d\n", *ptr++);

		ptr = pru1ram;
		printf("PRU1 Cycle count starts at: %d\n", *ptr++);
		printf("Cycle count after data reception:%d \n", *ptr++);
		printf("Stall count after data reception:%d \nData received:\n\n", *ptr++);

		uint16_t *data = pru0ram + 12;
		uint16_t *data1 = pru1ram + 12;
		for (i = 0; i < 16; i++) {
			printf("%04X\n", data[i]);

//			// If testing, match PRU0 and PRU1 RAM Contents
//			if (data[i] != data1[i]) {
//				printf("Fail at here %d, expected %04X\n", i, data1[i]);
//			}
		}

		printf("Done.\n\n");
	}

	return EXIT_SUCCESS;
}
