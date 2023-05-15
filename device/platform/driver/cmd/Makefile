#
ifeq ($(CONFIG_OS_UCOS),y)
obj-y += cmdtool.o
else

obj-y := cmd_hwtest_parser.o
obj-y += cmd_hwtest_parser_util.o
obj-${CONFIG_SCART} += cmd_hwtest_parser_scart.o
obj-${CONFIG_VIDEO_DECODER} += cmd_hwtest_parser_av.o
obj-${CONFIG_VIDEO_DECODER} += cmd_hwtest_parser_av_decode.o
obj-${CONFIG_VIDEO_DECODER} += cmd_hwtest_parser_av_data.o
obj-${CONFIG_AUDIO_DECODER} += cmd_hwtest_parser_audio.o
obj-y += cmd_hwtest_parser_hdmi.o

ifeq (${CONFIG_DIRECTFB},y)
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
obj-y += cmd_hwtest_parser_osd.o
endif
else
obj-y += cmd_hwtest_parser_osd.o
endif

obj-y += cmd_hwtest_parser_demux.o
obj-y += cmd_hwtest_parser_system.o
obj-y += cmd_hwtest_parser_ext.o
obj-$(CONFIG_CHANNEL) += cmd_hwtest_parser_channel.o
obj-y += cmd_hwtest_parser_eeprom.o
obj-y += cmd_hwtest_parser_nvram.o
obj-y += cmd_hwtest_parser_flash.o
obj-$(CONFIG_NAND_FLASH) += cmd_hwtest_parser_nandflash.o
obj-${CONFIG_EMMC_FLASH} += cmd_hwtest_parser_emmcflash.o

ifeq ($(CONFIG_DI_SHARED_LIB),y)
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
obj-$(CONFIG_USB) += cmd_hwtest_parser_usb.o
endif
else
obj-$(CONFIG_USB) += cmd_hwtest_parser_usb.o
endif

obj-y += cmd_hwtest_parser_vkernel.o
obj-${CONFIG_DSC} += cmd_hwtest_parser_dsc.o
obj-${CONFIG_SMARTCARD} += cmd_hwtest_parser_sc.o
obj-${CONFIG_MODEM} += cmd_hwtest_parser_modem.o
obj-${CONFIG_CABLE_MODEM} += cmd_hwtest_parser_cm.o
obj-${CONFIG_PVR} += cmd_hwtest_parser_pvr.o
obj-$(CONFIG_PRODUCT_IMAGE_HWTEST) += cmd_hwtest_interface.o
obj-${CONFIG_MEDIA20_MULTICAST} += cmd_hwtest_parser_media20_multicast.o
obj-${CONFIG_MEDIA20} += cmd_hwtest_parser_demo.o
obj-${CONFIG_MEDIA20} += cmd_hwtest_parser_media.o
obj-${CONFIG_MEDIA20} += cmd_hwtest_parser_media_scriptengine.o
#obj-${CONFIG_MEDIA20} += util_memtool.o

ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
ifeq (${CONFIG_CALYPSO},y)
HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../packages/calypso/debug/include
obj-y += cmd_hwtest_parser_calypso.o
obj-y += cmd_hwtest_parser_media_scriptengine.o
#obj-y += util_memtool.o
endif
endif

ifeq ($(CONFIG_DI_SHARED_LIB),y)
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
obj-$(CONFIG_FS) += cmd_hwtest_parser_fs.o
endif
else
obj-$(CONFIG_FS) += cmd_hwtest_parser_fs.o
endif

obj-$(CONFIG_RF4CE) += cmd_hwtest_parser_rf4ce.o
obj-$(CONFIG_TRANSCODER) += cmd_hwtest_parser_transcoder.o
obj-y += cmd_crash_test.o
obj-y += cmd_crash_test2.o
obj-y += cmdtool.o
obj-$(CONFIG_DINETWORK) += cmd_hwtest_parser_network.o
obj-$(CONFIG_CRYPTO) += cmd_hwtest_parser_crypto.o

ifeq ($(CONFIG_CAS), y)
ifeq ($(CONFIG_CAS_NA), y)
obj-y += cmd_hwtest_parser_ota.o
endif
endif

ifeq ($(CONFIG_DI_MODULE_MMC_SHARED_LIB),y)
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
obj-$(CONFIG_MMC) += cmd_hwtest_parser_mmc.o
endif
else
obj-$(CONFIG_MMC) += cmd_hwtest_parser_mmc.o
endif

endif

obj-y += unittest/
