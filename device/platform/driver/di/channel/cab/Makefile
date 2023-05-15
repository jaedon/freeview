#
obj-y :=

ifneq ($(CONFIG_PLATFORM_DP10023),)
obj-y += dp10023/
endif

ifneq ($(CONFIG_PLATFORM_DP10024),) 
obj-y += dp10024/
endif

ifneq ($(CONFIG_PLATFORM_DP10025),) 
obj-y += dp10025/
endif

ifneq ($(CONFIG_PLATFORM_DB3383),)
obj-y += db3383/
endif

ifneq ($(CONFIG_PLATFORM_DB3128),)
obj-y += db3128/
endif

ifneq ($(CONFIG_PLATFORM_DB7584),)
obj-y += db7584/
endif

ifneq ($(CONFIG_PLATFORM_DB7581),)
obj-y += db7581/
endif

ifneq ($(CONFIG_PLATFORM_TM2063),)
obj-y += tm2063/
endif

ifneq ($(CONFIG_PLATFORM_TN182I2),)
obj-y += tn182I2/
endif

ifneq ($(CONFIG_PLATFORM_TN18260),)
obj-y += tn18260/
endif

ifneq ($(CONFIG_PLATFORM_TM241SF),)
obj-y += tm241sf/
endif

obj-y += di_channel_c.o
obj-y += drv_channel_c.o

# Additional include path
ifneq ($(CONFIG_PLATFORM_DP10024),)
inccflag-y += -I$(HUMAX_BUILD_DIR)dp10024
endif

ifneq ($(CONFIG_PLATFORM_DP10023),)
inccflag-y += -I$(HUMAX_BUILD_DIR)dp10023
endif

ifneq ($(CONFIG_PLATFORM_DP10025),)
inccflag-y += -I$(HUMAX_BUILD_DIR)dp10025
endif

ifneq ($(CONFIG_PLATFORM_MT2063),)
inccflag-y += -I$(HUMAX_BUILD_DIR)mt2063
endif

ifneq ($(CONFIG_PLATFORM_TN182I2),)
inccflag-y += -I$(HUMAX_BUILD_DIR)tn182I2
endif

ifneq ($(CONFIG_PLATFORM_TN18260),)
inccflag-y += -I$(HUMAX_BUILD_DIR)tn18260
endif

ifneq ($(CONFIG_PLATFORM_DB3383),)
inccflag-y += -I$(HUMAX_BUILD_DIR)db3383
endif

ifneq ($(CONFIG_PLATFORM_DB3128),)
inccflag-y += -I$(HUMAX_BUILD_DIR)db3128
endif

ifneq ($(CONFIG_PLATFORM_DB7584),)
inccflag-y += -I$(HUMAX_BUILD_DIR)db7584
endif

ifneq ($(CONFIG_PLATFORM_DB7581),)
inccflag-y += -I$(HUMAX_BUILD_DIR)db7581
endif
