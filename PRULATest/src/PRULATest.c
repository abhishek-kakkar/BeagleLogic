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

/* Handles to all PRU related memory areas*/
void *extram, *pru0ram, *pru1ram, *prusharedram;

int main(void) {

	int ret, i;
	uint32_t *ptr;
	size_t extram_sz;

	struct timespec ts1, ts2;

	tpruss_intc_initdata initdata = PRUSS_INTC_INITDATA;

	puts("PRU Application test\n"); /* print Hello, world */

	/* Set up the PRU */
	prussdrv_init();
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

	/* Clear any residual interrupts from previous runs */
	prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
	prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU1_ARM_INTERRUPT);

	/* Get a pointer to all accessible PRU memories */
	prussdrv_map_extmem(&extram);
	prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pru0ram);
	prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &pru1ram);
	prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &prusharedram);

	/* Report and store the PRU buffer size */
	extram_sz = prussdrv_extmem_size();
	printf("DDR Buffer Size:%d at physical address 0x%08X\n\n", extram_sz, prussdrv_get_phys_addr(extram));

	/*
	 * PRU0 Firmware ABI
	 *
	 * To be loaded into PRU0 SRAM before loading FW
	 *
	 * PRU0 Call format:
	 *   Offset  0: Sample Count (0 for continuous streaming)
	 *   Offset  4: Reserved
	 *   Offset  8: Half ring size (Usually 4 MB) <= Note!!
	 *              Please note that a low ring size
	 *              may lead to missing interrupts!
	 *   Offset 12: Physical address of DDR where to write
	 *
	 * PRU0 Returns:
	 *   Offset 16: PRU0 interrupt count
	 *   Offset 20: PRU1 interrupt count
	 *   Offset 24: PRU0 cycle count (only when exiting)
	 *   Offset 28: PRU0 Stall count
	 *   Offset 32: No. of samples collected, padded to
	 *              next multiple of 32 [Global sample counter]
	 *   Offset 36: Starting cycle for PRU0.
	 */
	ptr = pru0ram;
	ptr[0] = 0;
	ptr[1] = 0;
	ptr[2] = extram_sz / 2;
	ptr[3] = prussdrv_get_phys_addr(extram);
	ptr[4] = 0;
	ptr[5] = 0;
	ptr[6] = 0;
	ptr[7] = 0;
	ptr[8] = 0;
	ptr[9] = 0;

	/* Load the firmware into the PRUs, PRU0 first and then PRU1 */
	if (prussdrv_exec_program(0, "./pru0fw.bin") == 0) {
		puts("Loaded PRU firmware for PRU0");
	}
	if (prussdrv_exec_program(1, "./pru1fw.bin") == 0) {
		puts("Loaded PRU firmware for PRU1\n");
	}

	for (i = 0; i < 1; i++) {
		/* Wait for interrupt and receive data */
		uint32_t *p = pru0ram;
		uint32_t t1, t2, t3;

		ret = prussdrv_pru_wait_event(PRU_EVTOUT_0);
		prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
		t1 = p[4];
		printf("PRU_INT0(%d, %d) ", i+1, t1);

		/* TODO Process data as fast as we receive. We have only ~15ms of time in between*/

		ret = prussdrv_pru_wait_event(PRU_EVTOUT_1);
		prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU1_ARM_INTERRUPT);

		/* Retrieve global sample count from the PRU mem */
		t2 = p[5];
		printf("PRU_INT1(%d, %d), ", i+1, t2, 1);

		printf ("Bytes collected: %u\n", p[8]);

		/* TODO Process data as fast as we receive. We have only ~15ms of time in between*/
	}

	printf("Total: %08X (%u samples)\n\nFirst 16 samples:", ptr[8], ptr[8] / 2);

	uint16_t *data = extram;
	for (i = 0; i < 16; i++) {
		printf("%c%04X", i % 4 == 0 ? '\n' : ' ', data[i]);
	}

	puts("\nDone.");

	/* Stop the PRUs and flush all interrupts */
	prussdrv_pru_disable(1);
	prussdrv_pru_disable(0);

	prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
	prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU1_ARM_INTERRUPT);

	/* Close the drivers */
	prussdrv_exit();

	return EXIT_SUCCESS;
}
