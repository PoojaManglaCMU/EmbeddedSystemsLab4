SYSCALL_OBJS := io.o time.o task.o CswiHandler.o swiHandler.o
SYSCALL_OBJS := $(SYSCALL_OBJS:%=$(KDIR)/syscall/%)

KOBJS += $(SYSCALL_OBJS)
