obj-y += memory_debug.o
obj-y += memory_hlib_debug.o
obj-y += memory_backtrace.o

ifeq ($(CONFIG_PROD_CPU_BCM_ARM),y)
obj-y += memory_backtrace_arm.o
else
obj-y += memory_backtrace_mips.o
endif


ccflags-y += -D_GNU_SOURCE