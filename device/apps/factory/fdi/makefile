###############################################################################
#                                                                             #
#                             Humax Templte Makefile                          #
#                                                                             #
# Author: 								                                      #
#                                                                             #
# Copyright (c) Humax co, Ltd., 2007                                          #
#                                                                             #
###############################################################################
# $Header:
# List up sub-directories
###############################################################################
include $(FACTORYSW_MAKE_DIR)/toolset.mak

include $(FACTORYSW_PRODUCT_CONFIG_DIR)/factorysw_def.cfg
include $(FACTORYSW_DIR)/config/$(FACTORYSW_NAME)/factorysw.cfg
include $(FACTORYSW_PRODUCT_CONFIG_DIR)/factorysw_module.cfg

FACTORYSW_CFLAGS += $(addprefix -D,$(FACTORY_DEFINES))

SUBDIRS_LIST	=

###############################################################################
# Rule for build
###############################################################################
OBJS_LIST  = 	fdi_init.o 		\
				fdi_key.o 		\
				fdi_osd.o 		\
				fdi_system.o 	\
				fdi_video.o 	\
				fdi_audio.o 	\
				fdi_demux.o 	\
				fdi_channel.o 	\
				fdi_flash.o		\
				fdi_eeprom.o	\
				fdi_panel.o 	\
				fdi_uart.o 		\
				fdi_led.o		\
				fdi_crypt.o		\
				fdi_print.o	\
				fdi_nvram_unpack.o

ifeq ($(FACTORY_USE_CHANNEL_SAT), YES)
OBJS_LIST  +=	fdi_channel_s.o
endif

ifeq ($(FACTORY_USE_CHANNEL_TER), YES)
OBJS_LIST  +=	fdi_channel_t.o
endif

ifeq ($(FACTORY_USE_CHANNEL_CAB), YES)
OBJS_LIST  +=	fdi_channel_c.o
endif

ifeq ($(FACTORY_USE_SCART), YES)
OBJS_LIST  +=	fdi_scart.o
endif

ifeq ($(FACTORY_USE_RTC), YES)
OBJS_LIST  +=	fdi_clock.o
endif

ifeq ($(FACTORY_USE_ETHERNET), YES)
OBJS_LIST  +=	fdi_ethernet.o
endif

ifeq ($(FACTORY_USE_WIFI), YES)
OBJS_LIST  +=	fdi_wifi.o
endif

ifeq ($(DI10_WIFI_ENABLE), YES)
OBJS_LIST  +=	fdi_wifi.o
endif

ifeq ($(FACTORY_USE_BT), YES)
OBJS_LIST  +=	fdi_bluetooth.o
endif

ifeq ($(FACTORY_USE_HDD), YES)
OBJS_LIST  +=	fdi_hdd.o
endif

ifeq ($(FACTORY_USE_MODEM), YES)
OBJS_LIST  +=	fdi_modem.o
endif

ifeq ($(FACTORY_USE_RF), YES)
OBJS_LIST +=	fdi_rf.o
endif

ifeq ($(FACTORY_USE_SMARTCARD), YES)
OBJS_LIST  +=	fdi_sc.o
endif

ifeq ($(FACTORY_USE_USB), YES)
OBJS_LIST  +=	fdi_usb.o
endif

ifeq ($(FACTORY_USE_CI), YES)
OBJS_LIST  +=	fdi_ci.o
endif

ifeq ($(FACTORY_USE_HDMI), YES)
OBJS_LIST  +=	fdi_hdmi.o
endif

ifeq ($(FACTORY_USE_FAN), YES)
OBJS_LIST  +=	fdi_fan.o
endif

ifeq ($(FACTORY_USE_IRB), YES)
OBJS_LIST  +=	fdi_irb.o
endif

ifeq ($(FACTORY_USE_CABLEMODEM), YES)
OBJS_LIST  +=	fdi_cablemodem.o
endif

ifeq ($(MES_DOWNLOAD_COMM_TYPE), MES_COMM_SERIAL)
OBJS_LIST  +=	fdi_mes_serial.o
endif

ifeq ($(MES_DOWNLOAD_COMM_TYPE), MES_COMM_ETHERNET)
OBJS_LIST  +=	fdi_mes_ethernet.o
endif

ifeq ($(TMD_INPUT_TYPE), TMD_INPUT_NONE)
OBJS_LIST  +=	fdi_tmd_none.o
endif

ifeq ($(TMD_INPUT_TYPE), TMD_INPUT_SERIAL)
OBJS_LIST  +=	fdi_tmd_serial.o
endif

ifeq ($(TMD_INPUT_TYPE), TMD_INPUT_ETHERNET)
OBJS_LIST  +=	fdi_tmd_ethernet.o
endif

ifeq ($(FACTORY_USE_CAS_IR_UTIL), YES)
OBJS_LIST  +=	fdi_cas_ir_util.o
endif

ifeq ($(FACTORY_USE_MMC), YES)
OBJS_LIST  +=	fdi_mmc.o
endif

ifeq ($(VERIMATRIX_BETACRYPT_NSC_ENABLE), YES)
OBJS_LIST +=	fdi_cas_vmxbc_api.o
endif

###############################################################################
# List up all directories of header files whih are need to build
###############################################################################
INCLUDES_LIST :=	-I$(FACTORYSW_DIR)/fvk/include	\
					-I$(FACTORYSW_DIR)/include		\
	                -I./include						\
					$(FACTORYSW_INCLUDE)

###############################################################################
# Export some macros for build
###############################################################################
export	INCLUDES_LIST

include	$(FACTORYSW_MAKE_DIR)/rules.mak
