/*
 * PRUIncludes.asm
 *
 * This file is a part of the BeagleLogic Project
 * Copyright (C) 2014 Kumar Abhishek <abhishek@theembeddedkitchen.net>
 *
 * This file is free software under GPL v3+
 *
 */

// Interrupt mappings, refer to pruss_intc_mapping.h
#define PRU0_PRU1_INTERRUPT     17
#define PRU1_PRU0_INTERRUPT     18
#define PRU0_ARM_INTERRUPT      19
#define PRU1_ARM_INTERRUPT      20
#define ARM_PRU0_INTERRUPT      21
#define ARM_PRU1_INTERRUPT      22

#define CONST_PRUSSINTC C0
#define CONST_PRUCFG    C4
#define CONST_PRUDRAM   C24
#define CONST_L3RAM     C30
#define CONST_DDR       C31

// Address for the PRUSS_PRU_CTRL registers
#define CONTROL			0x22000
#define STATUS			0x22004
#define WAKEUP_EN		0x22008
#define CYCLE			0x2200C
#define STALL			0x22010
#define CTBIR_0         0x22020
#define CTBIR_1         0x22024
#define CTPPR_0         0x22028
#define CTPPR_1         0x2202C

.macro LDI32
.mparam dst, src
	LDI dst.w0, (src) & 0xFFFF
	LDI dst.w2, (src) >> 16
.endm

.macro  LD32
.mparam dst,src
    LBBO    dst,src,#0x00,4
.endm

.macro  LD16
.mparam dst,src
    LBBO    dst,src,#0x00,2
.endm

.macro  LD8
.mparam dst,src
    LBBO    dst,src,#0x00,1
.endm

.macro ST32
.mparam src,dst
    SBBO    src,dst,#0x00,4
.endm

.macro ST16
.mparam src,dst
    SBBO    src,dst,#0x00,2
.endm

.macro ST8
.mparam src,dst
    SBBO    src,dst,#0x00,1
.endm
