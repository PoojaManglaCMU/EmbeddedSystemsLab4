/** @file time.S
 *
 * @brief time sycall wrapper
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-31
 */

#include <asm.h>
#include <bits/swi.h>
#include <errno.h>

	.file "task_create.S"

FUNC(task_create)
        stmfd   sp!, {lr}
        swi CREATE_SWI
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
