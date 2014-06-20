;* PRU1 Firmware for BeagleLogic
;* Copyright (C) 2014 Kumar Abhishek
;*
;* Licensed under the GNU GPL and provided "as-is" without any expressed 
;* or implied warranties / liabilities.

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
	QBEQ   sampleincnumberstest, R14, 0
	QBNE   samplexm, R14, 1
sample100m:
	QBEQ   sample100m8, R15, 1
sample100m16:
	MOV    R21.w0, R31.w0
	NOP
	MOV    R21.w2, R31.w0
	NOP
$sample100m16$2:
	MOV    R22.w0, R31.w0
	NOP
	MOV    R22.w2, R31.w0
	NOP
	MOV    R23.w0, R31.w0
	NOP
	MOV    R23.w2, R31.w0
	NOP
	MOV    R24.w0, R31.w0
	NOP
	MOV    R24.w2, R31.w0
	NOP
	MOV    R25.w0, R31.w0
	NOP
	MOV    R25.w2, R31.w0
	NOP
	MOV    R26.w0, R31.w0
	NOP
	MOV    R26.w2, R31.w0
	NOP
	MOV    R27.w0, R31.w0
	NOP
	MOV    R27.w2, R31.w0
	NOP
	MOV    R28.w0, R31.w0
	ADD    R29, R29, 32                     ; Maintain global byte counter
	MOV    R28.w2, R31.w0
	XOUT   10, &R21, 36                     ; Move data across the broadside
	MOV    R21.w0, R31.w0
	LDI    R31, PRU1_PRU0_INTERRUPT + 16    ; Jab PRU0
	MOV    R21.w2, R31.w0
	JMP    $sample100m16$2

sample100m8:
	MOV    R21.b0, R31.b0
	NOP
	MOV    R21.b1, R31.b0
	NOP
$sample100m8$2:
	MOV    R21.b2, R31.b0
	NOP
	MOV    R21.b3, R31.b0
	NOP
	MOV    R22.b0, R31.b0
	NOP
	MOV    R22.b1, R31.b0
	NOP
	MOV    R22.b2, R31.b0
	NOP
	MOV    R22.b3, R31.b0
	NOP
	MOV    R23.b0, R31.b0
	NOP
	MOV    R23.b1, R31.b0
	NOP
	MOV    R23.b2, R31.b0
	NOP
	MOV    R23.b3, R31.b0
	NOP
	MOV    R24.b0, R31.b0
	NOP
	MOV    R24.b1, R31.b0
	NOP
	MOV    R24.b2, R31.b0
	NOP
	MOV    R24.b3, R31.b0
	NOP
	MOV    R25.b0, R31.b0
	NOP
	MOV    R25.b1, R31.b0
	NOP
	MOV    R25.b2, R31.b0
	NOP
	MOV    R25.b3, R31.b0
	NOP
	MOV    R26.b0, R31.b0
	NOP
	MOV    R26.b1, R31.b0
	NOP
	MOV    R26.b2, R31.b0
	NOP
	MOV    R26.b3, R31.b0
	NOP
	MOV    R27.b0, R31.b0
	NOP
	MOV    R27.b1, R31.b0
	NOP
	MOV    R27.b2, R31.b0
	NOP
	MOV    R27.b3, R31.b0
	NOP
	MOV    R28.b0, R31.b0
	NOP
	MOV    R28.b1, R31.b0
	NOP
	MOV    R28.b2, R31.b0
	ADD    R29, R29, 32
	MOV    R28.b3, R31.b0
	XOUT   10, &R21, 36                     ; Move data across the broadside
	MOV    R21.b0, R31.b0
	LDI    R31, PRU1_PRU0_INTERRUPT + 16    ; Jab PRU0
	MOV    R21.b1, R31.b0
	JMP    $sample100m8$2

samplexm:
	QBEQ   samplexm8, R15, 1
samplexm16:
	MOV    R21.w0, R31.w0
	DELAY  R14, NOP
	MOV    R21.w2, R31.w0
	DELAY  R14, NOP
$samplexm16$2:
	MOV    R22.w0, R31.w0
	DELAY  R14, NOP
	MOV    R22.w2, R31.w0
	DELAY  R14, NOP
	MOV    R23.w0, R31.w0
	DELAY  R14, NOP
	MOV    R23.w2, R31.w0
	DELAY  R14, NOP
	MOV    R24.w0, R31.w0
	DELAY  R14, NOP
	MOV    R24.w2, R31.w0
	DELAY  R14, NOP
	MOV    R25.w0, R31.w0
	DELAY  R14, NOP
	MOV    R25.w2, R31.w0
	DELAY  R14, NOP
	MOV    R26.w0, R31.w0
	DELAY  R14, NOP
	MOV    R26.w2, R31.w0
	DELAY  R14, NOP
	MOV    R27.w0, R31.w0
	DELAY  R14, NOP
	MOV    R27.w2, R31.w0
	DELAY  R14, NOP
	MOV    R28.w0, R31.w0
	DELAY  R14, "ADD    R29, R29, 32"                     ; Maintain global byte counter
	MOV    R28.w2, R31.w0
	DELAY  R14, "XOUT   10, &R21, 36"                     ; Move data across the broadside
	MOV    R21.w0, R31.w0
	DELAY  R14, "LDI    R31, PRU1_PRU0_INTERRUPT + 16"    ; Jab PRU0
	MOV    R21.w2, R31.w0
	DELAY  R14, "JMP    $samplexm16$2"

samplexm8:
	MOV    R21.b0, R31.b0
	DELAY  R14, NOP
	MOV    R21.b1, R31.b0
	DELAY  R14, NOP
$samplexm8$2:
	MOV    R21.b2, R31.b0
	DELAY  R14, NOP
	MOV    R21.b3, R31.b0
	DELAY  R14, NOP
	MOV    R22.b0, R31.b0
	DELAY  R14, NOP
	MOV    R22.b1, R31.b0
	DELAY  R14, NOP
	MOV    R22.b2, R31.b0
	DELAY  R14, NOP
	MOV    R22.b3, R31.b0
	DELAY  R14, NOP
	MOV    R23.b0, R31.b0
	DELAY  R14, NOP
	MOV    R23.b1, R31.b0
	DELAY  R14, NOP
	MOV    R23.b2, R31.b0
	DELAY  R14, NOP
	MOV    R23.b3, R31.b0
	DELAY  R14, NOP
	MOV    R24.b0, R31.b0
	DELAY  R14, NOP
	MOV    R24.b1, R31.b0
	DELAY  R14, NOP
	MOV    R24.b2, R31.b0
	DELAY  R14, NOP
	MOV    R24.b3, R31.b0
	DELAY  R14, NOP
	MOV    R25.b0, R31.b0
	DELAY  R14, NOP
	MOV    R25.b1, R31.b0
	DELAY  R14, NOP
	MOV    R25.b2, R31.b0
	DELAY  R14, NOP
	MOV    R25.b3, R31.b0
	DELAY  R14, NOP
	MOV    R26.b0, R31.b0
	DELAY  R14, NOP
	MOV    R26.b1, R31.b0
	DELAY  R14, NOP
	MOV    R26.b2, R31.b0
	DELAY  R14, NOP
	MOV    R26.b3, R31.b0
	DELAY  R14, NOP
	MOV    R27.b0, R31.b0
	DELAY  R14, NOP
	MOV    R27.b1, R31.b0
	DELAY  R14, NOP
	MOV    R27.b2, R31.b0
	DELAY  R14, NOP
	MOV    R27.b3, R31.b0
	DELAY  R14, NOP
	MOV    R28.b0, R31.b0
	DELAY  R14, NOP
	MOV    R28.b1, R31.b0
	DELAY  R14, NOP
	MOV    R28.b2, R31.b0
	DELAY  R14, "ADD    R29, R29, 32"
	MOV    R28.b3, R31.b0
	DELAY  R14, "XOUT   10, &R21, 36"
	MOV    R21.b0, R31.b0
	DELAY  R14, "LDI    R31, PRU1_PRU0_INTERRUPT + 16"
	MOV    R21.b1, R31.b0
	DELAY  R14, "JMP    $samplexm8$2"

; Unit test to check for dropped frames
; Runs at 100 MHz
sampleincnumberstest:
	LDI    R21, 0
	NOP
	NOP
	NOP
$S1:	ADD    R22, R21, 1
	NOP
	NOP
	NOP
	ADD    R23, R22, 1
	NOP
	NOP
	NOP
	ADD    R24, R23, 1
	NOP
	NOP
	NOP
	ADD    R25, R24, 1
	NOP
	NOP
	NOP
	ADD    R26, R25, 1
	NOP
	NOP
	NOP
	ADD    R27, R26, 1
	NOP
	NOP
	NOP
	ADD    R28, R27, 1
	XOUT   10, &R21, 36
	LDI    R31, PRU1_PRU0_INTERRUPT + 16
	NOP
	ADD    R21, R28, 1
	NOP
	NOP
	JMP    $S1

; End-of-firmware
	HALT
