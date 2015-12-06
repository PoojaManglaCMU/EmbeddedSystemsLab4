@userAddess.S: User mode startup routine
@
@ loads the user stack and then switches to the usermode
@
@ Author: Soumyaroop <soumyard@andrew.cmu.edu>
@         Pooja Mangla <pmangla@andrew.cmu.edu>
@         Date:   Tue, 26 Oct 2015

        .file   "UserAddress.S"
        .text

        .global _start
        .global Load_user_program

Load_user_program:
        mrs r4, cpsr
        bic r4, r4, #0xFF 
        orr r4, r4, #0x50 @set irq and usermode
        msr cpsr, r4   @switch to usermode

        ldr sp, =0xa3000000 @top of the user stack

        mov r7, r0 @load the user stack
LC0:
        @store the Argv from the kernel stack
        @to the user stack

        add r5, r1, r7, lsl #2  
        ldr r6, [r5]
        str r6, [sp, #-4]! 
        sub r7, r7, #1
        cmp r7, #0
        bge LC0
        stmfd sp!, {r0}

        ldr pc, =0xa0000000 @switch to user program.
