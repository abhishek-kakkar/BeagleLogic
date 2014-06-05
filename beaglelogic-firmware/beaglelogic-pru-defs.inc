;
; Assembler Includes for both the PRUs
; 

	;*
	;* Constants
	;*

	.asg C0, CONST_PRUSSINTC
	.asg C4, CONST_PRUCFG
	.asg C24, PRUDRAM
	.asg C31, CONST_DDR
	
	.asg 0x10, GER_OFFSET
	.asg 0x24, SICR_OFFSET
	.asg 0x28, EISR_OFFSET
	.asg 0x34, HIESR_OFFSET

	.asg 0x02000, CONST_OTHERPRU_MEM
	.asg 0x10000, CONST_SHARED_MEM

	;* IRQ numbers
	.asg 17, PRU0_PRU1_INTERRUPT
	.asg 18, PRU1_PRU0_INTERRUPT
	.asg 19, PRU0_ARM_INTERRUPT
	.asg 20, PRU1_ARM_INTERRUPT
	.asg 21, ARM_PRU0_INTERRUPT
	.asg 22, ARM_PRU1_INTERRUPT
	
	.asg 0x22028, CTPPR_0
	; Address for the PRUSS_PRU_CTRL registers
	; CONTROL                 0x22000
	; STATUS                  0x22004
	; WAKEUP_EN               0x22008
	; CYCLE                   0x2200C
	; STALL                   0x22010
	; CTBIR_0                 0x22020
	; CTBIR_1                 0x22024
	; CTPPR_0                 0x22028
	; CTPPR_1                 0x2202C
