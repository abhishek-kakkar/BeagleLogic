;*
;* Syscall handlers for PRU0
;*
;*
;* This file is a part of the BeagleLogic project
;*
;* This program is free software; you can redistribute it and/or modify
;* it under the terms of the GNU General Public License version 2 as
;* published by the Free Software Foundation.

	.asg 35, SYSCALL_VALUE       ; 32 | (19 - 16) = 35

    .include "pru_syscalls.asm"
