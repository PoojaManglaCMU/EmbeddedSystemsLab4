/** @file sleep.S
 *
 * @brief sleep sycall wrapper
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-31
 */

#include <asm.h>
#include <bits/swi.h>
	.file "sleep.S"

FUNC(sleep)
        .text

        .global sleep

sleep:
        stmfd   sp!, {lr}
        swi SLEEP_SWI
        ldmfd sp!, {lr}
        mov pc, lr

