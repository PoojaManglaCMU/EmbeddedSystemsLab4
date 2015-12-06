#include <types.h>
#include <exports.h>
#include <config.h>
#include <bits/fileno.h>
#include <bits/errno.h>
#include <bits/swi.h>
#include <task.h>
#include <device.h>
#include <sched.h>
#include <lock.h>
#include "../sched/sched_i.h"
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <arm/reg.h>
#include <arm/physmem.h>
#include <sched.h>

#define DELETE 127
#define ENDOFTRANS 4
#define SDRAM_UP 0xa3FFFFFF
#define SDRAM_DOWN 0xa0000000
#define STRATA_FLASH 0x00FFFFFF
#define START_ADDR 0xa0000000
#define END_ADDR   0xa3000000

int read_swi_handler(unsigned* reg);
int write_swi_handler(unsigned* reg);
void sleep_swi_handler(int duration);
unsigned long time_swi_handler(void);
unsigned task_create(unsigned* reg);
int event_wait(unsigned int dev);

extern unsigned stack_Pointer; //SP
extern unsigned saved_Cpsr;
extern unsigned SHandlerAddr;
extern volatile unsigned long global_timer;

extern int Load_user_program(int argc, char** argv);
extern int S_Handler(void);
extern void enable_irqs(void);
extern void disable_irqs(void);

extern int valid_addr(const void* start, unsigned count, uintptr_t base, uintptr_t bound);
 
#define NO_OF_MS 10
#define CLOCKS_PER_MS 3250
