/** @file main.c
 *
 * @brief kernel main
 * 
 * Contains routines to initialize timer and wire in IRQ and
 * SWI handlers. 
 *
 * @author: Group Member 1: Soumyaroop Dutta <soumyard>
 *          Group Member 2: Pooja Mangla <pmangla>
 *          Group Number: 18	   
 *          Date: Dec 4, 2015	   
 *   
 */
 
#include <kernel.h>
#include <exports.h>
#include <lock.h>
#include <task.h>
#include <sched.h>
#include <device.h>
#include <assert.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <arm/reg.h>

#define SWI_VECTOR 0x8
#define IRQ_VECTOR 0x18
#define LDR_WITH_OFFSET 0xe51ff004
#define LDR_POSITIVE_OFFSET 0xe59ff000
#define LDR_NEGATIVE_OFFSET 0xe51ff000 
#define NO_OF_MS 10
#define CLOCKS_PER_MS 3250

uint32_t global_data;
unsigned stack_Pointer;
unsigned saved_R8;

unsigned SHandlerAddr;
unsigned IRQHandlerAddr;

volatile unsigned long global_timer;

unsigned setcycles;
void set_timer(void);
void wireinHandler(unsigned vectorAddress, unsigned HandlerAddr);

/* Kernel main function */
int kmain(int argc, char** argv, unsigned stackP, unsigned r8)
{
        app_startup(); /* bss is valid after this point */

        stack_Pointer = stackP;
        saved_R8 = r8;

        SHandlerAddr = (unsigned)&(S_Handler);
        IRQHandlerAddr = (unsigned)&(IRQ_Handler);

        /* Preparing the IRQ stack */
        load_irqs();

	//initiliaze the timer
        global_timer = 0;
	setcycles = CLOCKS_PER_MS * NO_OF_MS;
	set_timer();

        //wirein the IRQ and SWI handlers.
        wireinHandler(SWI_VECTOR, SHandlerAddr);
        wireinHandler(IRQ_VECTOR, IRQHandlerAddr);
	//Initialize the mutexes.
        mutex_init();
        //enable IRQ for svc mode.
        enable_irqs();

        //load the user program.
        Load_user_program(argc, argv);

		
	assert(0);        /* should never get here */
	return(0);
}

/* Common routine to wire in SWI handlers and IRQ handlers */
void wireinHandler(unsigned int vectorAddress, unsigned int HandlerAddr)
{
        unsigned int oldoffset = 0;
        unsigned int PC_Handler = LDR_WITH_OFFSET;
        unsigned int *vector;

        /*The location to the original  handler is 
         *fetched.
         *We achieve dereferencing the address stored
         *in the jump table, pointed by the vector.*/

        vector= (unsigned int*) vectorAddress;

        //gives you the offset stored in the LDR instruction
        oldoffset = *vector;
        oldoffset = 0xFFF & oldoffset;

        if((*vector & LDR_POSITIVE_OFFSET) == LDR_POSITIVE_OFFSET)
        {

          /*add 0x8 for next instruction at = PC + 8
           *add the offset as it is positive.*/
          oldoffset = vectorAddress + oldoffset + 0x8;
        }
        else if((*vector & LDR_NEGATIVE_OFFSET) == LDR_NEGATIVE_OFFSET){
          //substract the offset as it was negative 
          oldoffset = vectorAddress - oldoffset + 0x8;
        }

        /*gives you the pointer to the address on the 
          jump table*/
        vector = (unsigned int*) oldoffset;

        /*stores the address of the swi handler.*/
        oldoffset = *vector;
        /*points to the handler*/
        vector = (unsigned int*) (oldoffset);

        /*The new instructions are loaded.
         *The LDR instruction and 
         *the address to the handler.*/
        *vector = PC_Handler;
        *(vector + 1) = HandlerAddr;
}

/* Routine to setup the timer */
void set_timer(void){

     /*initialize timer*/
     reg_clear(INT_ICLR_ADDR, 1 << INT_OSTMR_0);
     reg_set(INT_ICMR_ADDR, 1 << INT_OSTMR_0);
     /*enable OIER*/
     reg_set(OSTMR_OIER_ADDR, OSTMR_OIER_E0);
     /*clear OSCR*/
     reg_write(OSTMR_OSCR_ADDR, 0);

     /*set no. of cycles*/
     reg_write(OSTMR_OSMR_ADDR(0), setcycles);

     /*reset OSSR */
     reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);
}
