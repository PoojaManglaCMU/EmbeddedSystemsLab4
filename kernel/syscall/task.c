 /* @file: task.c
  * 
  * Contains task_create and event_wait syscalls 
  * and utility functions required by them.
  * 
  * @author: Group Member 1: Soumyaroop Dutta <soumyard>
  *          Group Member 2: Pooja Mangla <pmangla>
  *          Group Number: 18
  *          Date: Dec 3, 2015
  */


#include "CswiHandler.h"

static void sort_tasks(task_t* arr_task, int length)
{
	int i,j;
	for(i = 0 ;i < length ; i ++)
            for( j = i+1; j < length ; j++)
		if( arr_task[i].T > arr_task[j].T){
	            task_t t = arr_task[i];
                    arr_task[i] = arr_task[j];
		    arr_task[j] = t;
		}
}


unsigned task_create(unsigned* reg)
{
     task_t* tasks = (task_t *) reg[0];
     size_t num_tasks = reg[1];

     size_t i, j;


/* Total number of tasks should not be greater than 64. Return error if > 64 */
     if(num_tasks > 63)
     	return -EINVAL;
     
/* Task pointer should not lie outside the valid address space, 
 * valid_addr function can be found in memcheck.c, already provided */
     if(!(valid_addr((void *)tasks, sizeof(task_t)*num_tasks, \
     				USR_START_ADDR, USR_END_ADDR)))
     	return -EFAULT;
     
     for(i = 0; i < num_tasks; i++)
     {
     	/* Function pointer should not be NULL */
     	if(tasks[i].lambda == NULL)
     	  return -EINVAL;
     
     	/* Stack pointers of two tasks should not be same */
     	for(j = 0; j < num_tasks; j++)
           if((tasks[i].stack_pos == tasks[j].stack_pos) && (i != j))
     			return -EINVAL;
     
     }
     /*Inititalize the devices */
     dev_init();
     /* Initialize the run queues */
     runqueue_init();
     /* Re-Initialize the mutex arrays 
      *	just clear off the sleep queues
      * and locks, but keep the mutex alive*/
     mutex_reinit();
     /*sort the tasks provided */
     sort_tasks(tasks, num_tasks);    
     /* initialize the system tcbs and idle tasks*/
     allocate_tasks(&tasks, num_tasks);
     /*dispatch the first function */
     dispatch_nosave();
     return 1;
}

int event_wait(unsigned int dev)
{
   /* Return invalid if the device does not exist */
   if(dev >= NUM_DEVICES)
     return -EINVAL;

   /* Add the task to the sleep queue of the device and context switch */
   dev_wait(dev);
   dispatch_sleep();	
   return 1; 	
}

