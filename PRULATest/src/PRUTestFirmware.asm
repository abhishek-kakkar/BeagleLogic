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

	MOV		r4, CONTROL
	LBBO	r0, r4, 0, 4
	SET		r0, 3
	SBBO	r0, r4, 0, 4

	MOV	    r1, 0xDEADBEEF
	MOV		r4, 0xFEEBDAED
	MOV     r2, 0
	MOV		r3, 0x2000

	ST32	r1, r2
	ST32	r4, r3

	MOV		r4, CYCLE
	MOV		r3, STALL

	SBBO	r4, r2, 0, 4
	AND		r5, r5, r5
	AND		r5, r5, r5
	AND		r5, r5, r5
	SBBO	r4, r2, 4, 4
	SBBO	r3, r2, 8, 4

	// Send notification to ARM
	LDI		r31.b0, PRU0_ARM_INTERRUPT+16

	HALT
