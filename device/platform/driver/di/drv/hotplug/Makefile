#
ifneq ($(CONFIG_DRV_MODULE_HOTPLUG_SHARED_LIB),y)
ifeq ($(CONFIG_DI_SHARED_LIB),y)
so-y := drv_hotplug.o
else
obj-y := drv_hotplug.o
endif
endif
