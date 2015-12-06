 /* @file: time.c
  * 
  * Contains sleep and time syscall handlers 
  * and utility functions required by them.
  * 
  * @author: Group Member 1: Soumyaroop Dutta <soumyard>
  *          Group Member 2: Pooja Mangla <pmangla>
  *          Group Number: 18
  *          Date: Dec 3, 2015
  */


#include "CswiHandler.h"

/* Sleep syscall handler, invoked when user wants 
   the program to sleep for a specified duration */
void sleep_swi_handler(int duration){

     unsigned long ticks_to_sleep = global_timer + duration/10;

     if(duration <= 0)
	return;
	
     /* Wait till the time, rounding-up to the nearest multiple of 10 */
     while(global_timer < ticks_to_sleep);

     /* sleep would have a time drift of 0-9 ms*/
     return;
}

/* Time syscall handler, invoked when user wants to get the 
   current time */
unsigned long time_swi_handler(void){

	unsigned long time = (global_timer+1) * 10;
	unsigned long currentcycles;
	unsigned long targetcycles;
        unsigned long cyclesfornextupdate;
	
	/* we got the time till last update + 10 ms. check for 
	 * the time drift. i.e. the number of cycles spent,
	 * after the last update. we could have a drift of 
         * 0-9 ms. subtract the time left for next update*/

        currentcycles = reg_read(OSTMR_OSCR_ADDR);
	targetcycles = reg_read(OSTMR_OSMR_ADDR(0));

	cyclesfornextupdate = targetcycles - currentcycles;
	time = time - (cyclesfornextupdate/CLOCKS_PER_MS);
	return time;
}
