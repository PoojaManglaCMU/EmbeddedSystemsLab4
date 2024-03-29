/** @file ctx_switch.c
 * 
 * @brief C wrappers around assembly context switch routines.
 * 
 * @author: Group Member 1: Soumyaroop Dutta <soumyard>
 *          Group Member 2: Pooja Mangla <pmangla>
 *          Group Number: 18
 *          Date: Dec 3, 2015
 */
 

#include <types.h>
#include <assert.h>

#include <config.h>
#include <kernel.h>
#include "sched_i.h"
#include <arm/psr.h>
#include <arm/exception.h>
#include <device.h>

#ifdef DEBUG_MUTEX
#include <exports.h>
#endif

static tcb_t* cur_tcb = NULL;

/**
 * @brief Initialize the current TCB and priority.
 *
 * Set the initialization thread's priority to IDLE so that anything
 * will preempt it when dispatching the first task.
 */
void dispatch_init(tcb_t* idle)
{
     cur_tcb = idle;	
}


/**
 * @brief Context switch to the highest priority task while saving off the 
 * current task state.
 *
 * This function needs to be externally synchronized.
 * We could be switching from the idle task.  The priority searcher has been tuned
 * to return IDLE_PRIO for a completely empty run_queue case.
 */
void dispatch_save(void)
{
     unsigned highest_task;
     tcb_t* old_tcb;
     disable_interrupts();     	

     highest_task = highest_prio();
  
     if(highest_task < cur_tcb->cur_prio)
     {
        runqueue_add(cur_tcb, cur_tcb->cur_prio);
	old_tcb = cur_tcb;
	cur_tcb = runqueue_remove(highest_task);
	ctx_switch_full(&(cur_tcb->context), &(old_tcb->context));
     } 
     
     enable_interrupts();
}

/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */
void dispatch_nosave(void)
{
     unsigned highest_task;

     disable_interrupts();     	

     highest_task = highest_prio();
  
     if(highest_task < cur_tcb->cur_prio)
     {
	cur_tcb = runqueue_remove(highest_task);
	ctx_switch_half(&(cur_tcb->context));
     } 
     
     enable_interrupts();
}


/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * and save the current task but don't mark is runnable.
 *
 * There is always an idle task to switch to.
 */
void dispatch_sleep(void)
{
     unsigned highest_task;
     unsigned old_prio;
     tcb_t* old_tcb;

     disable_interrupts();
    
     old_tcb = cur_tcb;	
     old_prio = old_tcb->cur_prio;

     highest_task = highest_prio();
     cur_tcb = runqueue_remove(highest_task);
     ctx_switch_full(&(cur_tcb->context), &(old_tcb->context));

     enable_interrupts();
}

/**
 * @brief Returns the priority value of the current task.
 */
uint8_t get_cur_prio(void)
{
    return cur_tcb->cur_prio;
}

/**
 * @brief Returns the TCB of the current task.
 */
tcb_t* get_cur_tcb(void)
{
    return cur_tcb;
}
