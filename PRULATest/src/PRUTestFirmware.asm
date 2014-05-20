/*
 * PRUTestFirmware.asm
 *
 * This file is a part of the BeagleLogic Project
 * Copyright (C) 2014 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 *
 * This file is free software under GPL v3+
 *
 */

.origin 0
.entrypoint main

#include "PRUIncludes.asm"

main:
	// Enable OCP master port
    LBCO    r0, CONST_PRUCFG, 4, 4
    CLR     r0, r0, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    SBCO	r0, CONST_PRUCFG, 4, 4

	// Configure C28 to point to 0x22000 (PRU0 Control)
	MOV		r0, 0x22028
	MOV 	r1, 0x0220
	SBBO	r1, r0, 0, 4

	// Enable the cycle counter
	MOV		r4, CONTROL
	LBBO	r0, r4, 0, 4
	SET		r0, 3
	SBBO	r0, r4, 0, 4

	// Cycle count starts here
	MOV	    r1, 0xDEADBEEF
	MOV		r4, 0xFEEBDAED
	MOV     r2, 0x0000
	MOV		r3, 0x2000

	ST32	r1, r2
	ST32	r4, r3

	// Load Cycle count reading to registers
	LBCO	r3, C28, 0x0C, 4
	SBBO	r3, r2, 0, 4

	MOV		R4.w0, R31.w0
	MOV		R4.w1, R31.w0
	MOV		R5.w0, R31.w0
	MOV		R5.w1, R31.w0
	MOV		R6.w0, R31.w0
	MOV		R6.w1, R31.w0
	MOV		R7.w0, R31.w0
	MOV		R7.w1, R31.w0
	MOV		R8.w0, R31.w0
	MOV		R8.w1, R31.w0
	MOV		R9.w0, R31.w0
	MOV		R9.w1, R31.w0
	MOV		R10.w0, R31.w0
	MOV		R10.w1, R31.w0
	MOV		R11.w0, R31.w0
	MOV		R11.w1, R31.w0

	SBBO	R4, R2, 12, 64

	LBCO	r3, C28, 0x0C, 4
	SBBO	r3, r2, 4, 4

	LBCO	r3, C28, 0x10, 4
	SBBO	r3, r2, 8, 4

	// Send notification to ARM
	LDI		r31.b0, PRU0_ARM_INTERRUPT+16

	HALT
