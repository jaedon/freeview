#
inccflag-y := -I$(HUMAX_DRIVER_DIR)/cmd/unittest

obj-y += unittest_vk.o
obj-$(CONFIG_CHANNEL) += unittest_di_channel.o
obj-y += unittest_di_demux.o
obj-${CONFIG_CI} += unittest_di_ci.o
obj-y += unittest_di_flash.o
obj-y += unittest_di_hdmi.o
obj-$(CONFIG_FS) += unittest_di_fs.o
ifeq (${CONFIG_ETHERNET},y)
obj-y += unittest_di_eth.o
endif
obj-y += unittest_di_scart.o
ifeq (${CONFIG_VIDEO_DECODER},y)
obj-y += unittest_di_video.o
endif
ifeq (${CONFIG_AUDIO_DECODER},y)
obj-y += unittest_di_audio.o
endif
ifeq (${CONFIG_DIRECTFB},y)
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
endif
else
obj-y += unittest_di_osd.o
endif
obj-${CONFIG_EEPROM_EMU} += unittest_di_eeprom.o
obj-y += unittest_di_key.o
obj-${CONFIG_CRYPTO} += unittest_di_crypt.o
obj-y += unittest_di_frontPanel.o
obj-y += unittest_di_uart.o
obj-y += unittest_di_led.o
obj-$(CONFIG_USB) += unittest_di_usb.o
ifeq (${CONFIG_RECORD},y)
ifeq (${CONFIG_PLAYBACK},y)
obj-y += unittest_di_pvr.o
endif
endif