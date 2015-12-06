/** @file kernel.h
 *
 * @brief Main kernel -- primary interface.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-17
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <types.h>

int valid_addr(const void* start, size_t count, uintptr_t base, uintptr_t bound);
extern int Load_user_program(int argc, char** argv);
extern int S_Handler(void);
extern void IRQ_Handler(void);
extern void exit_swi_handler_S(int result);
extern int C_SWI_Handler(int swi_num, unsigned *reg);
extern void C_IRQ_Handler(void);
extern void load_irqs(void);
extern void enable_irqs(void);
#endif /* KERNEL_H */
