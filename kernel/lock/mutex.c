/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 * @author: Group Member 1: Soumyaroop Dutta <soumyard>
 *          Group Member 2: Pooja Mangla <pmangla>
 *          Group Number: 18
 *          Date: Dec 3, 2015
 * 
 */

//#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#ifdef DEBUG_MUTEX
#include <exports.h> // temp
#endif

mutex_t gtMutex[OS_NUM_MUTEX];

void mutex_init()
{
        int i = 0;
	for (i = 0; i < OS_NUM_MUTEX; i++)
	{
		gtMutex[i].bAvailable = 1;
		gtMutex[i].pHolding_Tcb = NULL;
		gtMutex[i].bLock = 0;
		gtMutex[i].pSleep_queue = NULL;
	}
	
}

void mutex_reinit()
{
        int i = 0;
	for (i = 0; i < OS_NUM_MUTEX; i++)
	{
		gtMutex[i].pHolding_Tcb = NULL;
		gtMutex[i].bLock = 0;
		gtMutex[i].pSleep_queue = NULL;
	}
	
}

int mutex_create(void)
{
	disable_interrupts();

	int i;

        for(i=0; i<OS_NUM_MUTEX; i++){
	   if(gtMutex[i].bAvailable == 1){
	      gtMutex[i].bAvailable = 0;
	      break;
	   } 
	}
	
	if(i== OS_NUM_MUTEX)
	  return -ENOMEM;	

	enable_interrupts();	
	return i;
}

int mutex_lock(int mutex)
{
	disable_interrupts();
	tcb_t *cur_tcb = get_cur_tcb();
	tcb_t *next = gtMutex[mutex].pSleep_queue;

	/* Check for invalid mutex identifier */
	if((gtMutex[mutex].bAvailable == TRUE) || (mutex >= OS_NUM_MUTEX) ||
			(mutex < 0))
		return -EINVAL;

	/* If you already own the mutex, it will cause a deadlock*/
	if(cur_tcb == gtMutex[mutex].pHolding_Tcb)
		return -EDEADLOCK;

	/* If someone is already holding the mutex and its not yourself */
	if(gtMutex[mutex].pHolding_Tcb!= NULL)
	{
		if(gtMutex[mutex].pSleep_queue == NULL)
			gtMutex[mutex].pSleep_queue = cur_tcb;

		else	/* Find the last element on the sleep queue.*/
		{
			while(next->sleep_queue != NULL)
			      next = next->sleep_queue;

			next->sleep_queue = cur_tcb;
		}

		/* Schedule the next task and put the current to sleep. */
		dispatch_sleep();
	}

	gtMutex[mutex].pHolding_Tcb = cur_tcb;
	gtMutex[mutex].bLock = 1;
	cur_tcb->holds_lock++;
	enable_interrupts();
        return 0;
}

int mutex_unlock(int mutex)
{
	disable_interrupts();
        tcb_t* current_tcb;

	if((mutex > OS_NUM_MUTEX) || (gtMutex[mutex].bAvailable == 1))
	   return -EINVAL;

	current_tcb = get_cur_tcb();

	if((gtMutex[mutex].bLock == 0) 
		|| (gtMutex[mutex].pHolding_Tcb != current_tcb))
	   return -EPERM;

        current_tcb->holds_lock--;
	gtMutex[mutex].pHolding_Tcb = NULL;
	gtMutex[mutex].bLock = 0;  
	
	if(gtMutex[mutex].pSleep_queue != NULL){
	   tcb_t* next_tcb;

	   next_tcb = gtMutex[mutex].pSleep_queue;

	   gtMutex[mutex].pSleep_queue = next_tcb->sleep_queue;

	   next_tcb->sleep_queue = NULL;

           runqueue_add(next_tcb, next_tcb->cur_prio);
	   dispatch_save();
	}

	return 0; 
}

