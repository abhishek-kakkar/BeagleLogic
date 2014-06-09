/*
 * pru_syscalls.h
 *
 * Syscall utilities for the PRU
 */

#ifndef PRU_SYSCALLS_H_
#define PRU_SYSCALLS_H_

#include "stdlib.h"

#include <stdint.h>

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

static inline void *pa_to_da(u32 pa)
{
	/* we don't support physical addresses in GPMC */
	if (pa < 0x00080000)
		return NULL;

	return (void *)pa;
}

#define SC_HALT                     0	/* halt system */
#define SC_PUTC                     1	/* output a single char */
#define SC_EXIT                     2	/* exit (with a value) */
#define SC_PUTS			            3	/* output a null terminated string */

#define SC_DOWNCALL_READY           254	/* host requested a downcall, ack it, and execute */
#define SC_DOWNCALL_DONE	        255	/* call is performed, inform the host */

/* in pru_syscalls.asm */
extern int syscall(u32 nr);
extern int syscall1(u32 nr, u32 arg0);
extern int syscall2(u32 nr, u32 arg0, u32 arg1);
extern int syscall3(u32 nr, u32 arg0, u32 arg1, u32 arg2);

static inline void sc_halt(void)
{
	syscall(SC_HALT);
}

static inline void sc_putc(char c)
{
	syscall1(SC_PUTC, (u32)c);
}

static inline void sc_exit(int code)
{
	syscall1(SC_EXIT, (u32)code);
}

static inline void sc_puts(const char *str)
{
	syscall1(SC_PUTS, (u32)str);
}

/* downcall is handled by assembly */

extern void sc_downcall(int (*handler)(u32 nr, u32 arg0, u32 arg1,
			u32 arg2, u32 arg3, u32 arg4));


#endif /* PRU_SYSCALLS_H_ */
