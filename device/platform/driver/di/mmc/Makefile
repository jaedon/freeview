#
ifneq ($(CONFIG_DI_MODULE_MMC_SHARED_LIB),y)
ifeq ($(CONFIG_MMC),y)
ifeq ($(CONFIG_DI_SHARED_LIB),y)
so-y := di_mmc.o
else
obj-y += di_mmc.o
endif
endif
endif
