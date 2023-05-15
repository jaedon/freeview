#

obj-y :=

ifneq ($(CONFIG_PLATFORM_DB7346),)
obj-y += db7346/
endif

ifneq ($(CONFIG_PLATFORM_DB7358),)
obj-y += db7358/
endif

ifneq ($(CONFIG_PLATFORM_DB7362),)
obj-y += db7362/
endif

ifneq ($(CONFIG_PLATFORM_DB4506),)
obj-y += db4506/
endif

ifneq ($(CONFIG_PLATFORM_DB4538),)
obj-y += db4538/
endif

ifneq ($(CONFIG_PLATFORM_DB45216),)
obj-y += db45216/
endif

ifneq ($(CONFIG_PLATFORM_LA8286),)
obj-y += la8286/
endif

ifneq ($(CONFIG_PLATFORM_LA8296),)
obj-y += la8296/
endif

ifneq ($(CONFIG_PLATFORM_LA8297),)
obj-y += la8297/
endif

ifneq ($(CONFIG_PLATFORM_LDT1803),)
obj-y += ldt1803/
endif

ifneq ($(CONFIG_PLATFORM_LA8305),)
obj-y += la8305/
endif

ifneq ($(CONFIG_PLATFORM_LA8304),)
obj-y += la8304/
endif

obj-y += di_channel_s.o
obj-y += drv_channel_lnb.o
obj-y += drv_channel_s.o

# Additional include path
ifneq ($(CONFIG_PLATFORM_LA8286),)
inccflag-y += -I$(HUMAX_BUILD_DIR)la8286
endif

ifneq ($(CONFIG_PLATFORM_LA8296),)
inccflag-y += -I$(HUMAX_BUILD_DIR)la8296
endif

ifneq ($(CONFIG_PLATFORM_LA8297),)
inccflag-y += -I$(HUMAX_BUILD_DIR)la8297
endif

ifneq ($(CONFIG_PLATFORM_LDT1803),)
inccflag-y += -I$(HUMAX_BUILD_DIR)ldt1803
endif

ifneq ($(CONFIG_PLATFORM_LA8305),)
inccflag-y += -I$(HUMAX_BUILD_DIR)la8305
endif

ifneq ($(CONFIG_PLATFORM_LA8304),)
inccflag-y += -I$(HUMAX_BUILD_DIR)la8304
endif

ifneq ($(CONFIG_PLATFORM_DB7346),)
inccflag-y += -I$(HUMAX_BUILD_DIR)db7346
endif

ifneq ($(CONFIG_PLATFORM_DB7358),)
inccflag-y += -I$(HUMAX_BUILD_DIR)db7358
endif

ifneq ($(CONFIG_PLATFORM_DB4506),)
inccflag-y += -I$(HUMAX_BUILD_DIR)db4506
endif

ifneq ($(CONFIG_PLATFORM_DB4538),)
inccflag-y += -I$(HUMAX_BUILD_DIR)db4538
endif
