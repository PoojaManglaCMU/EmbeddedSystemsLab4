/** @file sched.c
 * 
 * @brief Top level implementation of the scheduler.
 * @author: Group Member 1: Soumyaroop Dutta <soumyard>
 *          Group Member 2: Pooja Mangla <pmangla>
 *          Group Number: 18
 *          Date: Dec 3, 2015
 */

#include <types.h>
#include <assert.h>
#include <task.h>
#include <kernel.h>
#include <config.h>
#include "sched_i.h"

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>

tcb_t system_tcb[OS_MAX_TASKS]; /*allocate memory for system TCBs */


/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */
 
static void idle(void)
{
	 enable_interrupts();
	 while(1);
}

void sched_init(task_t* main_task)
{
    main_task->lambda = (task_fun_t)idle;
    main_task->data = NULL;
    main_task->stack_pos = system_tcb[IDLE_PRIO].kstack_high;
    main_task->C = 0;
    main_task->T = 0;	
}

/**
 * @brief Allocate user-stacks and initializes the kernel contexts of the
 * given threads.
 *
 * This function assumes that:
 * - num_tasks < number of tasks allowed on the system.
 * - the tasks have already been deemed schedulable and have been appropriately
 *   scheduled.  In particular, this means that the task list is sorted in order
 *   of priority -- higher priority tasks come first.
 *
 * @param tasks  A list of scheduled task descriptors.
 * @param size   The number of tasks is the list.
 */
void allocate_tasks(task_t** tasks, size_t num_tasks)
{
	unsigned i;
        task_t idle_task;

      /* Initilize the run_queue and add the allocated tasks to the runqueue */

	for(i = 0; i < num_tasks; i++)
	{
           system_tcb[i].native_prio = i;
	   system_tcb[i].cur_prio = i;
	   system_tcb[i].holds_lock = 0;
	   system_tcb[i].context.r4 = (unsigned)(((*tasks)[i]).lambda);
	   system_tcb[i].context.r5 = (unsigned)(((*tasks)[i]).data);
	   system_tcb[i].context.r6 = (unsigned)(((*tasks)[i]).stack_pos);
	   system_tcb[i].context.sp = (void*)system_tcb[i].kstack_high;
	   system_tcb[i].sleep_queue = NULL;
	   /* first task would launch from here */
	   system_tcb[i].context.lr = launch_task;

	   runqueue_add(&system_tcb[i], i);
	}


	/* Initializing the idle task */
        sched_init(&idle_task);
	system_tcb[IDLE_PRIO].native_prio = IDLE_PRIO;
	system_tcb[IDLE_PRIO].cur_prio = IDLE_PRIO;
	system_tcb[IDLE_PRIO].holds_lock = 0;
	system_tcb[IDLE_PRIO].context.r4 = (unsigned)(idle_task.lambda);
	system_tcb[IDLE_PRIO].context.r5 = NULL;
	system_tcb[IDLE_PRIO].context.r6 = (unsigned)(idle_task.stack_pos);
	system_tcb[IDLE_PRIO].context.sp = (void*)system_tcb[IDLE_PRIO].kstack_high;
	system_tcb[IDLE_PRIO].sleep_queue = NULL;
	system_tcb[IDLE_PRIO].context.lr = launch_task;
	/* Adding idle task to the run queue */
	runqueue_add(&system_tcb[IDLE_PRIO], IDLE_PRIO);
        /* Make idle the current task */
        dispatch_init(&system_tcb[IDLE_PRIO]);
}
