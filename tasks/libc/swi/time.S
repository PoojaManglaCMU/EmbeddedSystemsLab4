/** @file time.S
 *
 * @brief time sycall wrapper
 */

#include <asm.h>
#include <bits/swi.h>
	.file "time.S"

FUNC(time)
        .text

        .global time

time:
        stmfd   sp!, {lr}
        swi TIME_SWI                                                      
        ldmfd sp!, {lr}
        mov pc, lr

