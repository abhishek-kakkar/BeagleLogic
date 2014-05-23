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

// Offsets (to be converted to structs in the next version
#define SMPL0   0
#define SMPL1   4
#define BUFLEN  8
#define RAMADDR 12

#define RETVAL	16
#define P0ICNT	16
#define P1ICNT	20
#define PCYCLE	24
#define PSTALL	28
#define RTCODE  32

main:
	// Zero all registers, otherwise we might see residual values
	ZERO &R0, 4*30

	// Configuration: Enable OCP, Set C28 table entry to PRUCTRL registers, enable cycle counter
	LBCO   R1, CONST_PRUCFG, 4, 4
	CLR    R1, R1, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
	SBCO   R1, CONST_PRUCFG, 4, 4

	MOV    R1, CTPPR_0
	MOV    R2, 0x00000220    // C28 = 00_0220_00h = PRU0 CFG Registers
	SBBO   R2, R1, 0, 4

	LBCO   R1, C28, 0, 4	 // Enable CYCLE counter
	SET    R1, 3
	SBCO   R1, C28, 0, 4

	// Load the params into the registers
	// R1-R4 not to be modified this point onwards
	LBBO   R1, R0, 0, 16     // Load Params into R1:R4

	// Generate the transfer limiting values
	// R9 = half buffer end
	// R10 = buffer end
	ADD    R9, R4, R3
	ADD    R10, R9, R3

	// Wait for an interrupt from PRU1 to begin
	MOV    R5, PRU0_PRU1_INTERRUPT
	WBS    R31, #31
	SBCO   R5, CONST_PRUSSINTC, SICR_OFFSET, 4
	MOV    R5, PRU1_PRU0_INTERRUPT

	// Run once: Time the first signal from PRU1 for debug
	WBS    R31, #30

	// Store time marker for debugging
	LBCO   R6, C28, 0x0C, 4
	SBBO   R6, R0, 36, 4

	// Create the loop counter
	MOV    R6, R4

	// Skip past the next statement
	JMP waitforpru111

	// Subsequent transfer loops will start from here
waitforpru11:
	// Wait for green signal from PRU1
	WBS    R31, #30

waitforpru111:
	// Clear interrupt flag
	// 2 Cycles
	SBCO   R5, CONST_PRUSSINTC, SICR_OFFSET, 4

	// Get data in from the register bank
	// 1 Cycle
	XIN    10, R13, 36

	// The captured signals are waiting for us from
	// PRU1 via the broadside interface. Put it
	// into the PRU0 RAM (1+no. of 32-bit transfers) = 9 cycles
	SBBO   R14, R6, 0, 32

	// Inc our offset counter
	ADD    R6, R6, 32

	// Check integrity of received data, we
	// should be trailing 32 bytes from the
	// received index.
	SUB    R12, R13, R12
	QBNE   xfererror1, R12, 32
	MOV    R12, R13

	QBLT   waitforpru11, R9, R6

	// Done with buffer 1, now send an interrupt to ARM (first ring ready)
	// Increment the interrupt counter
	ADD	   R7, R7, 1
	SBBO   R7, R0, P0ICNT, 4
	LDI    R31, 16 + PRU0_ARM_INTERRUPT


waitforpru12:
	// Wait for green signal from PRU1
	WBS    R31, #30

	// Clear interrupt flag
	// 2 Cycles
	SBCO   R5, CONST_PRUSSINTC, SICR_OFFSET, 4

	// Get data in from the register bank
	// 1 Cycle
	XIN    10, R13, 36

	// The captured signals are waiting for us from
	// PRU1 via the broadside interface. Put it
	// into the DDR RAM (1+no. of 32-bit transfers) = 9 cycles
	SBBO   R14, R6, 0, 32

	// Inc our offset counter
	ADD    R6, R6, 32

	// Check integrity of received data, we
	// should be trailing 32 bytes from the
	// received index.
	SUB    R12, R13, R12
	QBNE   xfererror1, R12, 32
	MOV    R12, R13

	// If not exited, then complete second buffer
	QBLT   waitforpru12, R10, R6

	// Done with filling, now send an interrupt to ARM (second ring ready)
	// Increment the interrupt counter
	ADD	   R8, R8, 1
	SBBO   R8, R0, P1ICNT, 4
	SBBO   R13, R0, 32, 4
	LDI    R31, 16 + PRU1_ARM_INTERRUPT

	// Reset pointer and restart
	MOV    R6, R4
	jmp waitforpru11

JMP exit

// Handle overruns
xfererror1:
	// Copy the sample index at which we failed integrity check
	MOV    R11, R13
	SBBO   R11, R0, RTCODE, 4

	// Fire both interrupts to release the code
	LDI    R31, 16 + PRU0_ARM_INTERRUPT
	LDI    R31, 16 + PRU1_ARM_INTERRUPT

// Never reach here (as of now)
exit:
	// Store CYCLE and STALL registers for examination (in one scoop)
	LBCO   R9, C28, 0x0C, 8   // 0x2200C : CYCLE, 0x22010 : STALL
	MOV    R11, R13
	SBBO   R9, R0, PCYCLE, 12

	// Unreachable for continuous sampling or till host decides to stop
	HALT
