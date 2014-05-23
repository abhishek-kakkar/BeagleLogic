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

.macro NOP
.mstart
	AND R0, R0, R0
.endm

//#define TEST

main:
	// Zero all registers, otherwise we might see residual values
	ZERO &R0, 4*30

	// OCP already enabled via PRU0 firmware

	// Set C28 in this PRU's bank =0x24000
	MOV    r0, CTPPR_0+0x2000  // Add 0x2000
	MOV    r1, 0x00000240      // C28 = 00_0240_00h = PRU1 CFG Registers
	SBBO   r1, r0, 0, 4

	// Configure R2 = 0x0000 - ptr to PRU1 RAM
	MOV    R2, 0

	// Enable the cycle counter
	LBCO   r0, C28, 0, 4
	SET    r0, 3
	SBCO   r0, C28, 0, 4

	// Load Cycle count reading to registers [LBCO=4 cycles, SBCO=2 cycles]
	LBCO   R0, C28, 0x0C, 4
	SBBO   R0, R2, 0, 4

	LDI    R31, PRU0_PRU1_INTERRUPT + 16    // Signal PRU0, incoming data

	MOV    R13, 0
#ifdef TEST
	LBBO   R4, R2, 12, 32					// Pull data from PRU1 SRAM
	XOUT   10, R4, 32                       // Push data to Bank0
	LDI    R31, PRU1_PRU0_INTERRUPT + 16	// Signal PRU0, incoming data
#else
sample0:
	// Sample into registers R14-R21
	// Also increment global sample counter R13 that is passed
	// as a synchronization mechanism along with the PRU registers
	// for free. If between two interrupts PRU0 finds that the
	// counter is more than 32, then it fires an interrupt and
    // aborts the operation (buffer overrrun condition).
	MOV    R14.w0, R31.w0
	NOP
sample1:
	MOV    R14.w2, R31.w0
	NOP
sample2:
	MOV    R15.w0, R31.w0
	NOP
sample3:
	MOV    R15.w2, R31.w0
	NOP
	MOV    R16.w0, R31.w0
	NOP
	MOV    R16.w2, R31.w0
	NOP
	MOV    R17.w0, R31.w0
	NOP
	MOV    R17.w2, R31.w0
	NOP
	MOV    R18.w0, R31.w0
	NOP
	MOV    R18.w2, R31.w0
	NOP
	MOV    R19.w0, R31.w0
	NOP
	MOV    R19.w2, R31.w0
	NOP
	MOV    R20.w0, R31.w0
	NOP
	MOV    R20.w2, R31.w0
	ADD    R13, R13, 32                     // Maintain global sample counter
	MOV    R21.w0, R31.w0
	LDI    R31, PRU1_PRU0_INTERRUPT + 16    // Jab PRU0
	MOV    R21.w2, R31.w0
	XOUT   10, R13, 36                      // Move data across the broadside
	MOV    R14.w0, R31.w0
	NOP
	MOV    R14.w2, R31.w0
	JMP    sample2
#endif

	// We never reach here, this is for debugging purposes only
	LBCO   R3, C28, 0x0C, 8
	SBBO   R3, R2, 4, 8                    // Store PRU1 total cycles & stall cycles in PRU1 RAM

	HALT
