/*
 * pru0fw.asm
 *
 * PRU 0 Firmware, for mem transfer
 *
 * This file is a part of the BeagleLogic Project
 * Copyright (C) 2014 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 *
 * This file is free software under GPL v3+
 *
 */

.origin 0
.entrypoint main

#include "prudefs.inc"

main:
	// Enable OCP master port
	LBCO   r0, CONST_PRUCFG, 4, 4
	CLR    r0, r0, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
	SBCO   r0, CONST_PRUCFG, 4, 4

	// Configure C28 to point to 0x22000 (PRU0 Control)
	MOV    r0, CTPPR_0
	MOV    r1, 0x00000220
	SBBO   r1, r0, 0, 4

	// Configure R2 = 0x0000
	MOV    r2, 0

	// Enable the cycle counter
	MOV    r4, CONTROL
	LBBO   r0, r4, 0, 4
	SET    r0, 3
	SBBO   r0, r4, 0, 4

	MOV    R1, PRU1_PRU0_INTERRUPT

	// Load Cycle count reading to registers
	LBCO   r3, C28, 0x0C, 4
	SBBO   r3, r2, 0, 4

	// Sampling with one instruction "spare"
	// Sample in sample bank 0

	// Repeat for sample Bank 2 [R4-R11]
	// Signal PRU0
	WBS    R31, 30
	SBCO   R1, CONST_PRUSSINTC, SICR_OFFSET, 4

	LDI    R31.b0, PRU0_ARM_INTERRUPT+16

	SBBO   R4, R2, 12, 32

	LBCO   r3, C28, 0x0C, 4
	SBBO   r3, r2, 4, 4

	LBCO   r3, C28, 0x10, 4
	SBBO   r3, r2, 8, 4

	// Send notification to ARM
	LDI    r31.b0, PRU1_ARM_INTERRUPT+16

	HALT
