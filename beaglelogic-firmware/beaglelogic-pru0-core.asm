;* PRU0 Firmware for BeagleLogic
;* Copyright (C) 2014 Kumar Abhishek
;*
;* Licensed under the GNU GPL and provided "as-is" without any expressed 
;* or implied warranties / liabilities.

;* Import all symbols from the C file
	.cdecls "beaglelogic-pru0.c"

;* C declaration:
;* void run(struct capture_context *ctx)
	.clink
	.global run
run:
	LDI 	R17, (MAX_SG_ENTRIES * 8)
$run$0:
	; Load scatter/gather list entries
	ADD 	R15, R14, 12
$run$1:
	; Load start and end address of mem chunk
	; If it's zero, we're done
	LBBO	&R15, R0, 0, 8
	QBEQ	$run$exit, R0, 0
$run$2:
	; Fill a buffer
	WBS 	R31, 30

	XIN 	10, &R21, 4 * 9
	SBBO	&R21, R0, 0, 32

	ADD 	R0, R0, 32
	QBNE	$run$2, R1, R0

	; Signal ARM that one buffer is now ready
	ADD 	R16, R16, 1
	SBBO	&R16, R14, 12, 4
	LDI 	R31, SYSEV_VR_PRU0_TO_ARM + 16

	; Move to next SG chunk
	ADD 	R15, R15, 8
	QBLT	$run$1, R17, R15

	JMP 	$run$0

	LDI 	R14, 0
	JMP 	$run$exit
$run$error:
	MOV 	R14, R29
$run$exit:
	JMP R3.w2
