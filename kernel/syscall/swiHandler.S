@ swihandler.S: Swi Handler startup routine
@
@ Also calls handles the Exit_SWI
@
@ Author: Soumyaroop <soumyard@andrew.cmu.edu>
@         Pooja Mangla <pmangla@andrew.cmu.edu>
@ Date:   Tue, 26 Oct 2015

        .file   "swiHandler.S"
        .text

        .global S_Handler

S_Handler:
	@save user space lr and sp
	stmfd sp, {sp, lr}^
	sub   sp, sp, #8
	@leave space for spsr
	sub  sp, sp, #4
	@save registers and return address
        stmfd sp!, {r0-r12, lr}

	@extract spsr and store
	mrs r0, spsr
	mov r1, sp
	@jump back 13 registers
	add r1, r1, #56
	stm r1, {r0}

        mov r1, sp
        ldr r0, [lr, #-4]
        bic r0,r0,#0xff000000
        ldr r12, =saved_R8 @restore the r8
	ldr r8, [r12]
	bl enable_irqs
        bl C_SWI_Handler

	@skip past r0 and r1
        add sp, sp, #8
	@restore r2-r12 and lr
        ldmfd sp!, {r2-r12, lr}
	@restore spsr
	ldmfd sp!, {r1}
	msr   spsr, r1 
	@restore usermode
	ldmfd sp, {sp, lr}^
	add sp, sp, #8
        movs pc, lr
