@ write.S: write sycall wrapper
@
@ Authors: Soumyaroop <soumyard@andrew.cmu.edu>
@ Date:    1.31 a.m 11th october.
#include <bits/swi.h>
#include <errno.h>

        .file   "write.S"
        .text

        .global write

@call the Write SWI and check if the return
@is valid.
@if not valid, return error.
write:
        stmfd   sp!, {lr}
        swi WRITE_SWI
        cmp r0, #0     
        blt negative
        mov r1, #0    
@return 1 in case of success.
@fallback to 'negative' otherwise.
 
err:
        ldr r2, =errno
        str r1, [r2]
        ldmfd sp!, {lr}
        mov pc, lr

@in case of error, return the absolute
@value to the error handler.
@and return a -1 to caller.
negative:
        mvn r0, r0
        add r1, r0, #1
        mov r0, #-1
        b   err
