;*
;* Syscall handlers for PRU0
;*

	.asg 35, SYSCALL_VALUE       ; 32 | (19 - 16) = 35

    .include "pru_syscalls.asm"
