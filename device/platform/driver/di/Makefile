#
obj-${CONFIG_AUDIO_DECODER} := audio/
obj-$(CONFIG_CHANNEL) += channel/
obj-${CONFIG_CI} += ci/
obj-${CONFIG_CABLE_MODEM} += cm/
obj-${CONFIG_CABLE_MODEM} += snmp/
obj-y += nvram/
obj-y += demux/
ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-y += drv/
endif
obj-y += drv/
obj-${CONFIG_DSC} += dsc/
obj-${CONFIG_CRYPTO} += crypt/
obj-${CONFIG_EEPROM} += eeprom/
obj-${CONFIG_EEPROM_EMU} += eeprom_emu/
obj-${CONFIG_ETHERNET} += ethernet/

obj-y += flash/
obj-y += front_panel/
ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-$(CONFIG_FS) += fs/
else
obj-$(CONFIG_FS) += fs/
endif
ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-${CONFIG_HDD} += hdd/
else
obj-${CONFIG_HDD} += hdd/
endif
obj-y += hdmi/
obj-$(CONFIG_IRB) += irb/
obj-y += key/
obj-y += led/

ifeq (${CONFIG_MEDIA},y)
ifeq (${CONFIG_MEDIA20},y)
obj-y += media20/
endif
ifeq (${CONFIG_MEDIA10},y)
obj-y += media/
obj-$(CONFIG_IPTUNER) += iptuner/
endif
endif
obj-${CONFIG_DRM} += drm/
obj-${CONFIG_CALYPSO} += calypso/
obj-${CONFIG_MODEM} += modem/
obj-y += osd/
obj-y += power/
obj-y += pvr/
obj-y += rtc/
obj-y += scart/
obj-${CONFIG_SMARTCARD} += smartcard/
obj-y += system/
obj-y += uart/

ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-$(CONFIG_USB) += usb/
else
obj-$(CONFIG_USB) += usb/
endif

obj-y += video/
obj-$(CONFIG_WATCHDOG) += watchdog/
obj-$(CONFIG_TRANSCODER) += transcoder/
obj-$(CONFIG_PRODUCT_IMAGE_FACTORY) += factory/

obj-y += di_init.o
ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-y += di_config.o
endif
obj-y += di_config.o

obj-$(CONFIG_DINETWORK) += network/
obj-$(CONFIG_RF4CE) += rf4ce/

ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-$(CONFIG_MMC) += mmc/
else
obj-$(CONFIG_MMC) += mmc/
endif

ifeq ($(CONFIG_DI_SHARED_LIB), y)
so-$(CONFIG_FAN) += fan/
else
obj-$(CONFIG_FAN) += fan/
endif

obj-y += rfvod/
obj-y += loader/

obj-$(CONFIG_MIC) += mic/

obj-$(CONFIG_PUMP) += pump/
