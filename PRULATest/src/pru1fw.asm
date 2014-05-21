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

main:
	// Enable OCP master port
	LBCO   R0, CONST_PRUCFG, 4, 4
	CLR    R0, R0, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
	SBCO   R0, CONST_PRUCFG, 4, 4

	// Configure C28 to point to 0x24000 (PRU1 Control)
	MOV    R0, CTPPR_0
	MOV    R1, 0x00000240
	SBBO   R1, R0, 0, 4

	// Enable the cycle counter
	MOV    R4, CONTROL
	LBBO   R0, R4, 0, 4
	SET    R0, 3
	SBBO   R0, R4, 0, 4


	// Load Cycle count Reading to Registers
	LBCO   R3, C28, 0x0C, 4
	SBBO   R3, R2, 0, 4

sample:
	// Each NOP is a window for doing 1 instruction magic
	MOV    R4.w0, R31.w0
	NOP
sample1:
	MOV    R4.w2, R31.w0
	NOP
	MOV    R5.w0, R31.w0
	NOP
	MOV    R5.w2, R31.w0
	NOP
	MOV    R6.w0, R31.w0
	NOP
	MOV    R6.w2, R31.w0
	NOP
	MOV    R7.w0, R31.w0
	NOP
	MOV    R7.w2, R31.w0
	NOP
	MOV    R8.w0, R31.w0
	NOP
	MOV    R8.w2, R31.w0
	NOP
	MOV    R9.w0, R31.w0
	NOP
	MOV    R9.w2, R31.w0
	NOP
	MOV    R10.w0, R31.w0
	NOP
	MOV    R10.w2, R31.w0
	NOP
	MOV    R11.w0, R31.w0
	MOV    R31, PRU1_PRU0_INTERRUPT + 16
	MOV    R11.w2, R31.w0
	XOUT   14, R4, 4*8
	//MOV    R4.w0, R31.w0
	//jmp sample

	LBCO   R3, C28, 0x0C, 4
	SBBO   R3, R2, 4, 4

	LBCO   R3, C28, 0x10, 4
	SBBO   R3, R2, 8, 4

	// Send notification to ARM
	//LDI    R31.b0, ARM_PRU0_INTERRUPT+16

	HALT
