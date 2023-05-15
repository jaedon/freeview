#
obj-y := gpio/
ifeq ($(CONFIG_OS_LINUX), y)
ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-y += hotplug/
else
obj-y += hotplug/
endif
endif
obj-y += i2c/
obj-y += sync/
obj-y += uart/
obj-y += tspath/

ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-y += pm/
else
obj-y += pm/
endif


ifeq (${CONFIG_CI},y)
ifeq ($(findstring DI_CI_CTL_TYPE_NONE, $(CONFIG_DI_CI_CTL_TYPE)),)
obj-y += ci/
endif
endif

ifeq ($(findstring DI_FAN_CTL_TYPE_NONE, $(CONFIG_DI_FAN_CTL_TYPE)),)
ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-y += fan/
else
obj-y += fan/
endif
endif

ifeq ($(findstring DI_MICOM_CTL_TYPE_NONE, $(CONFIG_DI_MICOM_CTL_TYPE)),)
obj-y += micom/
endif

ifeq ($(findstring DI_PWM_CTL_TYPE_NONE, $(CONFIG_DI_PWM_CTL_TYPE)),)
obj-y += pwm/
endif

ifeq ($(findstring DI_IR_RECEIVER_CTL_TYPE_NONE, $(CONFIG_DI_IR_RECEIVER_CTL_TYPE)),)
obj-y += remocon/
endif

ifeq ($(findstring DI_HDD_SMART_TYPE_NONE, $(CONFIG_DI_HDD_SMART_TYPE)),)
ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-${CONFIG_HDD_SMART} += smart/
else
obj-${CONFIG_HDD_SMART} += smart/
endif
endif

ifeq ($(findstring DI_SPI_CTL_TYPE_NONE, $(CONFIG_DI_SPI_CTL_TYPE)),)
obj-y += spi/
endif

obj-y += sharedmem/

obj-${CONFIG_STLED} += stled/
obj-${CONFIG_DRM} += drm/
obj-${CONFIG_RF4CE_EXTERNAL} += rf4ce/
obj-$(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST) += hilcrest/
obj-${CONFIG_TRANSCODER} += mux/
obj-${CONFIG_TRANSCODER} += encoder/
obj-${CONFIG_DINETWORK} += wireless/
obj-${CONFIG_WATCHDOG} += watchdog/
obj-${CONFIG_IPSTREAM} += ipstream/
obj-$(CONFIG_AUDIO_HIGH_RESOLUTION_AUDIO_EXT_DAC) += hra/
obj-$(CONFIG_OS_UCOS) += bootloader/
