/*
 * Cirqhandler.c: IRQ handler main (entry) function
 *
 * This module is the IRQ handler.
 * 
 * It handles the timer interrupts.
 *
 * Author: Soumyaroop <soumyard@andrew.cmu.edu>
 *          Pooja Mangla <pmangla@andrew.cmu.edu>
 * Date:   8th November 2015
 */
#include <stdarg.h>
#include <types.h> 
#include <arm/reg.h>
#include <arm/timer.h>
#include <arm/interrupt.h>
#include <exports.h>
#include <device.h>

#define MS_PER_CYCLE 10

extern volatile unsigned long global_timer;
extern unsigned setcycles;

void c_irq_handler(void) {
    unsigned  timerInterrupt = 
		reg_read(INT_ICPR_ADDR) & (1 << (INT_OSTMR_0));
    unsigned  matchcycles;
    /* If timer interrupt is generated */ 
    if (timerInterrupt) {
        //add no of cycles desired to current oscr address.
        matchcycles = reg_read(OSTMR_OSCR_ADDR) + setcycles;
        reg_write(OSTMR_OSMR_ADDR(0), matchcycles);
        
	reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
    /* Increment the global timer variable */
        global_timer++;
    }

    /*call device update */
    dev_update(global_timer*MS_PER_CYCLE);
}

