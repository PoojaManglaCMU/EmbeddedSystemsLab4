IRQ_OBJS := CirqHandler.o  irqenable.o  irqHandler.o 
IRQ_OBJS := $(IRQ_OBJS:%=$(KDIR)/irq/%)

KOBJS += $(IRQ_OBJS)
