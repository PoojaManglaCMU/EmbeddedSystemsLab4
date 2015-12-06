/**
 * @file device.c
 *
 * @brief Implements simulated devices.
 *
 * @author: Group Member 1: Soumyaroop Dutta <soumyard>
 *          Group Member 2: Pooja Mangla <pmangla>
 *          Group Number: 18
 */

#include <types.h>
#include <assert.h>

#include <config.h>
#include <task.h>
#include <sched.h>
#include <device.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>

#define MS_PER_CYCLE 10
/**
 * @brief Fake device maintainence structure.
 * Since our tasks are periodic, we can represent 
 * tasks with logical devices. 
 * These logical devices should be signalled periodically 
 * so that you can instantiate a new job every time period.
 * Devices are signaled by calling dev_update 
 * on every timer interrupt. In dev_update check if it is 
 * time to create a tasks new job. If so, make the task runnable.
 * There is a wait queue for every device which contains the tcbs of
 * all tasks waiting on the device event to occur.
 */

extern volatile unsigned long global_timer;

struct dev
{
	tcb_t* sleep_queue;
	unsigned long   next_match;
};
typedef struct dev dev_t;

/* devices will be periodically signaled at the following frequencies */
const unsigned long dev_freq[NUM_DEVICES] = {100, 200, 500, 50};
static dev_t devices[NUM_DEVICES];

/**
 * @brief Initialize the sleep queues and match values for all devices.
 */
void dev_init(void)
{
   int i = 0;
   for(i = 0; i < NUM_DEVICES; i++)
   {
       devices[i].sleep_queue = NULL;
       devices[i].next_match = global_timer*MS_PER_CYCLE;
   } 
}


/**
 * @brief Puts a task to sleep on the sleep queue until the next
 * event is signalled for the device.
 *
 * @param dev  Device number.
 */
void dev_wait(unsigned int dev)
{
    tcb_t* current = get_cur_tcb();

    current->sleep_queue = devices[dev].sleep_queue;
    devices[dev].sleep_queue = current;
    devices[dev].next_match += dev_freq[dev];
}


/**
 * @brief Signals the occurrence of an event on all applicable devices. 
 * This function should be called on timer interrupts to determine that 
 * the interrupt corresponds to the event frequency of a device. If the 
 * interrupt corresponded to the interrupt frequency of a device, this 
 * function should ensure that the task is made ready to run 
 */
void dev_update(unsigned long millis)
{
    int i = 0;
    int count = 0;
    tcb_t* tcb = NULL;
    tcb_t* prev = NULL;


    /*
     * Match the current time with match value of all the devices
     * and add the tasks of the devices whose value matched to the run queue
     */
    for(i = 0; i < NUM_DEVICES; i++)
    {
    	if(devices[i].next_match <= millis)
    	{
    	   /* Add the task to the run queue according to its priority */
    	   for(tcb = devices[i].sleep_queue; tcb != NULL; 
	   				tcb = tcb->sleep_queue)
    	   {
    	       runqueue_add(tcb, tcb->cur_prio);
    	       if(prev != NULL)
    	       	  prev->sleep_queue = NULL;
    
    	       prev = tcb;
    	       count++;
    	   }
    
    	   /* Update the sleep queue and next_match value of the device */
    	   devices[i].sleep_queue = NULL;
    	   devices[i].next_match = global_timer*MS_PER_CYCLE;
    	}
    }

    if(count > 0)
    {
    	/* Context switch to the highest priority task in the run queue */
    	dispatch_save();
    }
}

