#
obj-y :=

ifneq ($(CONFIG_FRONTEND_SAT),)
obj-y += sat/
endif

ifneq ($(CONFIG_FRONTEND_CAB),)
obj-y += cab/
endif

ifneq ($(CONFIG_FRONTEND_TER),)
obj-y += ter/
endif

obj-y += di_channel.o
obj-y += di_channel_attr.o
obj-y += di_channel_util.o
obj-y += di_channel_mon.o
obj-y += drv_channel.o
obj-y += drv_channel_config.o

ifneq ($(CONFIG_DEMOD_TSMF),)
obj-y += drv_channel_tsmf.o
endif
