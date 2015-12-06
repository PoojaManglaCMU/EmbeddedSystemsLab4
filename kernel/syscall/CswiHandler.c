/*
 * CswiHandler.h: Swi handler main (entry) function
 *
 * This module is the SWI handler.
 * 
 * We handle the read, write and exit swi calls.
 *
 * Author: Soumyaroop <soumyard@andrew.cmu.edu>
 *		   Pooja Mangla <pmangla@andrew.cmu.edu>
 * Date:   26th october 2015
 */

#include"CswiHandler.h"


/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);
        disable_irqs();
	while(1);
}

unsigned long C_SWI_Handler(int swi_num, unsigned* reg) {

unsigned long result = 0;

/* Handle different syscalls using switch case */
switch(swi_num) {

/* Read syscall */
case READ_SWI:
	result = read_swi_handler(reg);	
	break;
/* Write syscall */
case WRITE_SWI:
	result = write_swi_handler(reg);
	break;
/* Time syscall */
case TIME_SWI:
	result = time_swi_handler();
	break;
/* Sleep syscall */
case SLEEP_SWI:
	sleep_swi_handler(reg[0]);
	break;
/* Task create syscall */
case CREATE_SWI:
	result = task_create(reg);
	break;
/* Event wait syscall */
case EVENT_WAIT:
	result = event_wait(reg[0]);
	break;
/* Mutex create syscall */
case MUTEX_CREATE:
	result = mutex_create();
	break;
/* Mutex lock syscall */
case MUTEX_LOCK:
	result = mutex_lock(reg[0]);
	break;
/* Mutex unlock syscall*/
case MUTEX_UNLOCK:
	result = mutex_unlock(reg[0]);
	break;
default:
/*Invalid syscall*/
        invalid_syscall(swi_num);
	break; 
}

return result;
}

