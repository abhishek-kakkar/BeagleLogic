;*
;* Syscall handlers
;*

	;* must be included by syscall0.asm or syscall1.asm

        .text

        .global syscall
syscall:
        .asg r14, nr

	LDI R31, SYSCALL_VALUE
	HALT
        JMP R3.w2

        .global syscall1
syscall1:
        .asg r14, nr
        .asg r15, arg0

	LDI R31, SYSCALL_VALUE
	HALT
        JMP R3.w2

        .global syscall2
syscall2:
        .asg r14, nr
        .asg r15, arg0
        .asg r16, arg1

	LDI R31, SYSCALL_VALUE
	HALT
        JMP R3.w2

        .global syscall3
syscall3:
        .asg r14, nr
        .asg r15, arg0
        .asg r16, arg1
        .asg r17, arg2

	LDI R31, SYSCALL_VALUE
	HALT
        JMP R3.w2

	.asg 254, DOWNCALL_READY
	.asg 255, DOWNCALL_DONE

        .global sc_downcall
sc_downcall:
	MOV R0.w0, R14.w0		;* save the pointer to the function
	;* first issue the downcall ready
	LDI R14, DOWNCALL_READY
	LDI R31, SYSCALL_VALUE
	HALT				;* host must save R3.w2 locally
	;* the host will manipulate our state so that the arguments are correct
	JAL R3.w2, R0.w0		;* call
	MOV R0, R14			;* save the return code
	;* when we return here, we will inform the host of the result
	LDI R14, DOWNCALL_DONE		;
	LDI R31, SYSCALL_VALUE
	HALT				;* host must return to save R3.w2
