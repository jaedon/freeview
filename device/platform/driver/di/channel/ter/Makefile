#
obj-y :=

ifneq ($(CONFIG_PLATFORM_DP10048),)
obj-y += dp10048/
endif

ifneq ($(CONFIG_PLATFORM_DS2820),)
obj-y += ds2820/
endif

ifneq ($(CONFIG_PLATFORM_DT90507),)
obj-y += dt90507/
endif

ifneq ($(CONFIG_PLATFORM_DT90517),)
obj-y += dt90517/
endif

# JAEMS
ifneq ($(CONFIG_PLATFORM_MXL683),)
obj-y += mxl683/
endif

ifneq ($(CONFIG_PLATFORM_DB3461),)
obj-y += db3461/
endif

ifneq ($(CONFIG_PLATFORM_DB7563),)
obj-y += db7563/
endif

ifneq ($(CONFIG_PLATFORM_TN2117),)
obj-y += tn2117/
endif

ifneq ($(CONFIG_PLATFORM_TN3114),)
obj-y += tn3114/
endif

ifneq ($(CONFIG_PLATFORM_TN7320X),)
obj-y += tn7320X/
endif

ifneq ($(CONFIG_PLATFORM_TN18273),)
obj-y += tn18273/
endif

ifneq ($(CONFIG_PLATFORM_TN18275),)
obj-y += tn18275/
endif

obj-y += di_channel_t.o
obj-y += drv_channel_t.o

# Additional include path
ifneq ($(CONFIG_PLATFORM_DS2820),)
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/channel/ter/ds2820
endif

ifneq ($(CONFIG_PLATFORM_TN18273),)
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/channel/ter/tn18273
endif
