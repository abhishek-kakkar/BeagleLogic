#ifndef PRU_DEFS_H
#define PRU_DEFS_H

#include <stdint.h>

#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_intc.h>

volatile register uint32_t __R31;
volatile register uint32_t __R30;

#if defined(PRU0) || defined(PRU1)

#ifdef PRU0
#define PCTRL(_reg) \
	(*(volatile uint32_t *)((uint8_t *)0x22000 + (_reg)))
#define PCTRL_OTHER(_reg) \
	(*(volatile uint32_t *)((uint8_t *)0x24000 + (_reg)))
#else
#define PCTRL(_reg) \
	(*(volatile uint32_t *)((uint8_t *)0x24000 + (_reg)))
#define PCTRL_OTHER(_reg) \
	(*(volatile uint32_t *)((uint8_t *)0x22000 + (_reg)))
#endif

#define PCTRL_CONTROL		PCTRL(0x0000)
#define  CONTROL_SOFT_RST_N	(1 << 0)
#define  CONTROL_ENABLE		(1 << 1)
#define  CONTROL_SLEEPING	(1 << 2)
#define  CONTROL_COUNTER_ENABLE	(1 << 3)
#define  CONTROL_SINGLE_STEP	(1 << 8)
#define  CONTROL_RUNSTATE	(1 << 15)
#define PCTRL_STATUS		PCTRL(0x0004)
#define PCTRL_WAKEUP_EN		PCTRL(0x0008)
#define PCTRL_CYCLE		PCTRL(0x000C)
#define PCTRL_STALL		PCTRL(0x0010)
#define PCTRL_CTBIR0		PCTRL(0x0020)
#define PCTRL_CTBIR1		PCTRL(0x0024)
#define PCTRL_CTPPR0		PCTRL(0x0028)
#define PCTRL_CTPPR1		PCTRL(0x002C)

/* we can't access our debug registers (since we have to be stopped) */
#ifdef PRU0
#define PDBG_OTHER(_reg) \
	(*(volatile uint32_t *)((uint8_t *)0x24400 + (_reg)))
#else
#define PDBG_OTHER(_reg) \
	(*(volatile uint32_t *)((uint8_t *)0x22400 + (_reg)))
#endif

#endif

#define SIGNAL_EVENT(x) \
	__R31 = (1 << 5) | ((x) - 16); \


#ifndef PRU_CLK
/* default PRU clock (200MHz) */
#define PRU_CLK	200000000
#endif

/* NOTE: Do no use it for larger than 5 secs */
#define PRU_200MHz_sec(x)	((uint32_t)(((x) * 200000000)))
#define PRU_200MHz_ms(x)	((uint32_t)(((x) * 200000)))
#define PRU_200MHz_ms_err(x)	0
#define PRU_200MHz_us(x)	((uint32_t)(((x) * 200)))
#define PRU_200MHz_us_err(x)	0
#define PRU_200MHz_ns(x)	((uint32_t)(((x) * 2) / 10))
#define PRU_200MHz_ns_err(x)	((uint32_t)(((x) * 2) % 10))

#if PRU_CLK != 200000000
/* NOTE: Do no use it for larger than 5 secs */
#define PRU_sec(x)	((uint32_t)(((uint64_t)(x) * PRU_CLK)))
#define PRU_ms(x)	((uint32_t)(((uint64_t)(x) * PRU_CLK) / 1000))
#define PRU_ms_err(x)	((uint32_t)(((uint64_t)(x) * PRU_CLK) % 1000))
#define PRU_us(x)	((uint32_t)(((uint64_t)(x) * PRU_CLK) / 1000000))
#define PRU_us_err(x)	((uint32_t)(((uint64_t)(x) * PRU_CLK) % 1000000))
#define PRU_ns(x)	((uint32_t)(((uint64_t)(x) * PRU_CLK) / 1000000000))
#define PRU_ns_err(x)	((uint32_t)(((uint64_t)(x) * PRU_CLK) % 1000000000))
#else
/* NOTE: Do no use it for larger than 5 secs */
#define PRU_sec(x)	PRU_200MHz_sec(x)
#define PRU_ms(x)	PRU_200MHz_ms(x)
#define PRU_ms_err(x)	PRU_200MHz_ms_err(x)
#define PRU_us(x)	PRU_200MHz_us(x)
#define PRU_us_err(x)	PRU_200MHz_us_err(x)
#define PRU_ns(x)	PRU_200MHz_ns(x)
#define PRU_ns_err(x)	PRU_200MHz_ns_err(x)
#endif

#define DPRAM_SHARED	0x00010000

/* event definitions */
#define SYSEV_PRU0_TO_ARM	16
#define SYSEV_ARM_TO_PRU0	17

#define SYSEV_PRU1_TO_ARM	18
#define SYSEV_ARM_TO_PRU1	19

#define SYSEV_PRU0_TO_PRU1	21
#define SYSEV_PRU1_TO_PRU0	20

#define SYSEV_PRU0_TO_ARM_A	22
#define SYSEV_ARM_TO_PRU0_A	23
#define SYSEV_PRU0_TO_ARM_B	24

#define pru0_signal() (__R31 & (1U << 30))
#define pru1_signal() (__R31 & (1U << 31))

#ifdef PRU0
#define pru_signal()	pru0_signal()
#define SYSEV_OTHER_PRU_TO_THIS_PRU	SYSEV_PRU1_TO_PRU0
#define SYSEV_ARM_TO_THIS_PRU		SYSEV_ARM_TO_PRU0
#define SYSEV_THIS_PRU_TO_OTHER_PRU	SYSEV_PRU0_TO_PRU1
#define SYSEV_THIS_PRU_TO_ARM		SYSEV_PRU0_TO_ARM
#endif

#ifdef PRU1
#define pru_signal()	pru1_signal()
#define SYSEV_OTHER_PRU_TO_THIS_PRU	SYSEV_PRU0_TO_PRU1
#define SYSEV_ARM_TO_THIS_PRU		SYSEV_ARM_TO_PRU1
#define SYSEV_THIS_PRU_TO_OTHER_PRU	SYSEV_PRU1_TO_PRU0
#define SYSEV_THIS_PRU_TO_ARM		SYSEV_PRU1_TO_ARM
#endif

/* all events < 32 */
#define SYSEV_THIS_PRU_INCOMING_MASK	\
	(BIT(SYSEV_ARM_TO_THIS_PRU) | \
	 BIT(SYSEV_OTHER_PRU_TO_THIS_PRU) | \

#define DELAY_CYCLES(x) \
	do { \
		uint32_t t = (x) >> 1; \
		do { \
			__asm(" "); \
		} while (--t); \
	} while(0)

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

/* access to the resources of the other PRU (halt it and have your way) */
#if defined(PRU0) || defined(PRU1)

static inline void pru_other_halt(void)
{
	PCTRL_OTHER(0x0000) &= ~CONTROL_ENABLE;	/* clear enable */
	/* loop until RUNSTATE clears */
	while ((PCTRL_OTHER(0x0000) & CONTROL_RUNSTATE) != 0)
		;
}

static inline void pru_other_resume(void)
{
	PCTRL_OTHER(0x0000) |= CONTROL_ENABLE;	/* set enable */
}

static inline uint32_t pru_other_read_reg(uint16_t reg)
{
	uint32_t val;

	reg <<= 2;	/* multiply by 4 */
	pru_other_halt();
	val = PDBG_OTHER(reg);
	pru_other_resume();
	return val;
}

static inline void pru_other_write_reg(uint16_t reg, uint32_t val)
{
	reg <<= 2;	/* multiply by 4 */
	pru_other_halt();
	PDBG_OTHER(reg) = val;
	pru_other_resume();
}

static inline void pru_other_and_or_reg(uint16_t reg, uint32_t andmsk, uint32_t ormsk)
{
	reg <<= 2;	/* multiply by 4 */
	pru_other_halt();
	PDBG_OTHER(reg) = (PDBG_OTHER(reg) & andmsk) | ormsk;
	pru_other_resume();
}

#endif

#endif
