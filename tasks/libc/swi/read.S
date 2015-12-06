@ read.S: read sycall wrapper
@
@ Authors: Soumyaroop <Soumyard@andrew.cmu.edu>
@ Date:    9th October 2015 (11:11 AM)
#include <bits/swi.h>
#include <errno.h>

        .file   "read.S"
        .text

        .global read
@call the Read SWI and check if the return
@is valid.
@if not valid, return error.
read:
        stmfd   sp!, {lr}
        swi READ_SWI
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

