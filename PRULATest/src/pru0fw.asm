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
	// Zero all registers, otherwise we might see residual values
	ZERO &R0, 4*30

	// Configuration: Enable OCP, Set C28 table entry to PRUCTRL registers, enable cycle counter
	LBCO   r0, CONST_PRUCFG, 4, 4
	CLR    r0, r0, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
	SBCO   r0, CONST_PRUCFG, 4, 4

	MOV    r0, CTPPR_0
	MOV    r1, 0x00000220    // C28 = 00_0220_00h = PRU0 CFG Registers
	SBBO   r1, r0, 0, 4

	// Configure R2 = 0x0000 - PRU0 RAM pointer
	MOV    R2, 0

	// Enable the cycle counter
	LBCO   r0, C28, 0, 4
	SET    r0, 3
	SBCO   r0, C28, 0, 4

	MOV    R1, PRU1_PRU0_INTERRUPT

waitforpru1:
	// Wait for green signal from PRU1
	WBS    R31, #30

	// Clear interrupt flag
	// 2 Cycle
	SBCO   R1, CONST_PRUSSINTC, SICR_OFFSET, 4

	// Load Cycle count reading to registers [LBCO=4 cycles, SBCO=2 cycles]
	LBCO   R0, C28, 0x0C, 4
	SBBO   R0, R2, 0, 4

	// Get data in from the register bank
	// 1 Cycle
	XIN    10, R4, 32

	// The captured signals are waiting for us from
	// PRU1 via the broadside interface. Put it
	// into the PRU0 RAM (1+no. of 32-bit transfers) = 9 cycles
	SBBO   R4, R2, 12, 32

	// Store CYCLE and STALL registers for examination
	// 4+2 each = 12 cycles
	LBCO   r3, C28, 0x0C, 4   // 0x2200C : CYCLE
	SBBO   r3, r2, 4, 4

	LBCO   r3, C28, 0x10, 4   // 0x22010 : STALL
	SBBO   r3, r2, 8, 4

	// Send notification to ARM that transfer completed = 1 cycle
	LDI    r31, 16 + PRU0_ARM_INTERRUPT

	// Goto wait again (if continuous sampling)
	//JMP    waitforpru1

	// Unreachable for continuous sampling or till host decides to stop
	HALT
