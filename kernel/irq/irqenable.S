@irqenable.S: irq mode enable routine
@
@ loads the irq stack and enables irq
@
@ Author: Soumyaroop <soumyard@andrew.cmu.edu>
@          Pooja Mangla <pmangla@andrew.cmu.edu>
@ Date:   Tue, 8th Nov 2015

        .file   "irqenable.S"
        .text

        .global load_irqs
        .global enable_irqs
	.global disable_irqs

load_irqs:
        mrs r12, cpsr
	mov r11, r12
        bic r12, r12, #0xFF 
        orr r12, r12, #0xD2
        msr cpsr, r12  @switch to irq mode

        ldr sp, =0xa376f6f9 @top of the irq stack

	msr cpsr, r11 @switch back to svc mode
	mov pc, lr

enable_irqs:
        mrs r12, cpsr
	bic r12, r12, #0x80			@Unmask IRQs
	msr cpsr, r12
        mov pc,lr

disable_irqs:
	mrs r12, cpsr
	orr r12, r12, #0xC0			@Mask IRQs
	msr cpsr, r12
	mov pc,lr
