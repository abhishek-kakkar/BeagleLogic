;* PRU1 Firmware for BeagleLogic (customized for PRUDAQ)
;*
;* Copyright (C) 2014 Kumar Abhishek <abhishek@theembeddedkitchen.net>
;*
;* Mar '16: Modified by Kumar Abhishek for supporting the PRUDAQ board
;* Adapted by Jason Holt to follow an external clock signal
;*
;* This modified firmware captures interleaved channels A & B into
;* /dev/beaglelogic
;*
;* This program is free software; you can redistribute it and/or modify
;* it under the terms of the GNU General Public License version 2 as
;* published by the Free Software Foundation.

	.include "beaglelogic-pru-defs.inc"

NOP	.macro
	 ADD R0.b0, R0.b0, R0.b0
	.endm

; Generic delay loop macro
; Also includes a post-finish op
DELAY	.macro Rx, op
	SUB	R0, Rx, 2
	QBEQ	$E?, R0, 0
$M?:	SUB	R0, R0, 1
	QBNE	$M?, R0, 0
$E?:	op
	.endm

	.sect ".text:main"
	.global main
main:
	; Set C28 in this PRU's bank =0x24000
	LDI32  R0, CTPPR_0+0x2000               ; Add 0x2000
	LDI    R1, 0x00000240                   ; C28 = 00_0240_00h = PRU1 CFG Registers
	SBBO   &R1, R0, 0, 4

	; Configure R2 = 0x0000 - ptr to PRU1 RAM
	LDI    R2, 0

	; Enable the cycle counter
	LBCO   &R0, C28, 0, 4
	SET    R0, R0, 3
	SBCO   &R0, C28, 0, 4

	; Load Cycle count reading to registers [LBCO=4 cycles, SBCO=2 cycles]
	LBCO   &R0, C28, 0x0C, 4
	SBCO   &R0, C24, 0, 4

	; Load magic bytes into R2
	LDI32  R0, 0xBEA61E10

	; Wait for PRU0 to load configuration into R14[samplerate] and R15[unit]
	; This will occur from an downcall issued to us by PRU0
	HALT

	; Jump to the appropriate sample loop
	; TODO

	LDI    R31, 27 + 16                     ; Signal VRING1 to kernel driver
	HALT

	; Sample starts here
	; Maintain global bytes transferred counter (8 byte bursts)
	LDI    R29, 0
	LDI    R20.w0, 0x03FF                ; For masking unused bits
	LDI    R20.w2, 0x03FF

sampleAD9201:
	; Changed code for AD9201 sampling
	WBC    R31, 11                       ; Wait for falling edge

	WBS    R31, 11                       ; Wait for rising edge
	NOP                                  ; 3 cycles ~15ns delay before readout
	NOP
	NOP
	MOV    R21.w0, R31.w0                ; Read I0
	WBC    R31, 11                       ; falling edge
	NOP
$sampleAD9201$2:
	NOP
	NOP
	MOV    R21.w2, R31.w0                ; Read Q0

	WBS    R31, 11
	AND    R21, R21, R20                 ; Mask unused bits
	NOP
	NOP

	MOV    R22.w0, R31.w0                ; I1
	WBC    R31, 11
	NOP
	NOP
	NOP
	MOV    R22.w2, R31.w0                ; Q1

	WBS    R31, 11
	AND    R22, R22, R20
	NOP
	NOP
	MOV    R23.w0, R31.w0                ; I2
	WBC    R31, 11
	NOP
	NOP
	NOP
	MOV    R23.w2, R31.w0                ; Q2

	WBS    R31, 11
	AND    R23, R23, R20
	NOP
	NOP
	MOV    R24.w0, R31.w0                ; I3
	WBC    R31, 11
	NOP
	NOP
	NOP
	MOV    R24.w2, R31.w0                ; Q3

	WBS    R31, 11
	AND    R24, R24, R20
	NOP
	NOP
	MOV    R25.w0, R31.w0                ; I4
	WBC    R31, 11
	NOP
	NOP
	NOP
	MOV    R25.w2, R31.w0                ; Q4

	WBS    R31, 11
	AND    R25, R25, R20
	NOP
	NOP
	MOV    R26.w0, R31.w0                ; I5
	WBC    R31, 11
	NOP
	NOP
	NOP
	MOV    R26.w2, R31.w0                ; Q5

	WBS    R31, 11
	AND    R26, R26, R20
	NOP
	NOP
	MOV    R27.w0, R31.w0                ; I6
	WBC    R31, 11
	NOP
	NOP
	NOP
	MOV    R27.w2, R31.w0                ; Q6

	WBS    R31, 11
	AND    R27, R27, R20
	NOP
	NOP
	MOV    R28.w0, R31.w0                ; I7
	WBC    R31, 11
	NOP
	NOP
	ADD    R29, R29, 32                  ; Maintain global byte counter
	MOV    R28.w2, R31.w0                ; Q7

	WBS    R31, 11
	AND    R28, R28, R20
	XOUT   10, &R21, 36                     ; Move data across the broadside
	LDI    R31, PRU1_PRU0_INTERRUPT + 16    ; Jab PRU0
	MOV    R21.w0, R31.w0
	WBC    R31, 11
	JMP    $sampleAD9201$2

; End-of-firmware
	HALT
