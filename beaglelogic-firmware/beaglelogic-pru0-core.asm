;* PRU0 Firmware for BeagleLogic
;* Copyright (C) 2014 Kumar Abhishek
;*
;* Licensed under the GNU GPL and provided "as-is" without any expressed 
;* or implied warranties / liabilities.

;* Import all symbols from the C file
	.cdecls "beaglelogic-pru0.c"

;* C declaration:
;* void run(struct capture_context *ctx, u32 trigger_flags)
	.clink
	.global run
run:
	LDI	R17, (MAX_BUFLIST_ENTRIES * 8)
	LDI	R0, 18
	LDI	R1, 0
$run$0:
	; Load scatter/gather list entries
	ADD	R16, R14, 12
$run$1:
	; Load start and end address of mem chunk
	; If it's zero, we're done
	LBBO	&R18, R16, 0, 8
	QBEQ	$run$canexit, R18, 0
$run$2:
	; Wait for and clear the buffer ready signal from PRU1
	WBS	R31, 30
	SBCO	&R0, C0, 0x24, 4

	XIN	10, &R21, 36		; Get the logic data from PRU1
	SBBO	&R21, R18, 0, 32	; Write buffer
	ADD	R18, R18, 32
	QBLT	$run$2, R19, R18

	; Signal ARM that one buffer is now ready
	; Also check if we received the kill signal
	LDI	R31, SYSEV_VR_PRU0_TO_ARM + 16
	QBBS	$run$exit, R31, 31

	; Move to next buffer
	ADD	R16, R16, 8
	QBLT	$run$1, R17, R16

	LDI	R14, 0
	JMP	$run$exit
$run$canexit:
	QBBS	$run$0, R15, 0
$run$error:
	MOV	R14, R29
$run$exit:
	JMP	R3.w2
