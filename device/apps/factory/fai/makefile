###############################################################################
#                                                                             #
#                             Humax Templte Makefile                          #
#                                                                             #
# Author:                                                                     #
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
# Rule for build for each vkernel
###############################################################################
OBJS_LIST = list.o 				\
			fai_font.o 			\
			fai_widget.o 		\
			fai_window.o 		\
			fai_text.o 			\
			fai_testmenu.o 		\
			fai_button.o 		\
			fai_msgbox.o 		\
			fai_testparams.o 	\
			fai_app.o 			\
			fai_main.o 			\
			fai_init.o 			\
			fai_system.o 		\
			fai_tmd.o			\
			fai_channel.o 		\
			fai_video.o 		\
			fai_audio.o 		\
			fai_fkey.o			\
			fai_rcu.o			\
			fai_av.o 			\
			fai_panel.o			\
			fai_util.o			\
			decompress.o
			
ifeq ($(FACTORY_IMAGE_TYPE), IMAGE_TYPE_STANDALONE)
OBJS_LIST  +=	fai_entry.o
endif

ifeq ($(FACTORY_USE_CHANNEL_SAT), YES)
OBJS_LIST  +=	fai_channel_s.o
endif

ifeq ($(FACTORY_USE_CHANNEL_TER), YES)
OBJS_LIST  +=	fai_channel_t.o
endif

ifeq ($(FACTORY_USE_CHANNEL_CAB), YES)
OBJS_LIST  +=	fai_channel_c.o
endif

ifeq ($(FACTORY_USE_SCART), YES)
OBJS_LIST  +=	fai_scart.o
endif

ifeq ($(FACTORY_USE_RTC), YES)
OBJS_LIST  +=	fai_clock.o
endif

ifeq ($(FACTORY_USE_MES), YES)
OBJS_LIST  +=	fai_mes.o
endif

ifeq ($(FACTORY_USE_EEPROM), YES)
OBJS_LIST  +=	fai_eeprom.o
endif

ifeq ($(FACTORY_USE_ETHERNET), YES)
OBJS_LIST  +=	fai_ethernet.o
endif

ifeq ($(FACTORY_USE_WIFI), YES)
OBJS_LIST  +=	fai_wifi.o
endif

ifeq ($(DI10_WIFI_ENABLE), YES)
OBJS_LIST  +=	fai_wifi.o
endif

ifeq ($(FACTORY_USE_BT), YES)
OBJS_LIST  +=	fai_bluetooth.o
endif

ifeq ($(FACTORY_USE_FLASH), YES)
OBJS_LIST  +=	fai_flash.o
endif

ifeq ($(FACTORY_USE_HDD), YES)
OBJS_LIST  +=	fai_hdd.o
endif

ifeq ($(FACTORY_USE_MODEM), YES)
OBJS_LIST  +=	fai_modem.o
endif

ifeq ($(FACTORY_USE_RF), YES)
OBJS_LIST +=	fai_rf.o
endif

ifeq ($(FACTORY_USE_SMARTCARD), YES)
OBJS_LIST  +=	fai_sc.o
endif

ifeq ($(FACTORY_USE_USB), YES)
OBJS_LIST  +=	fai_usb.o
endif

ifeq ($(FACTORY_USE_CI), YES)
OBJS_LIST  +=	fai_ci.o
endif

ifeq ($(FACTORY_USE_HDMI), YES)
OBJS_LIST  +=	fai_hdmi.o
endif

ifeq ($(FACTORY_USE_FAN), YES)
OBJS_LIST  +=	fai_fan.o
endif

ifeq ($(FACTORY_USE_IRB), YES)
OBJS_LIST  +=	fai_irb.o
endif

ifeq ($(FACTORY_USE_CABLEMODEM), YES)
OBJS_LIST  +=	fai_cablemodem.o
endif

ifeq ($(FACTORY_USE_MMC), YES)
OBJS_LIST  +=  fai_mmc.o
endif

###############################################################################
# List up all directories of header files whih are need to build
###############################################################################
INCLUDES_LIST :=	-I$(FACTORYSW_DIR)/fdi/include	\
					-I$(FACTORYSW_DIR)/fvk/include	\
					-I$(FACTORYSW_DIR)/include		\
					-I./include						\
					$(FACTORYSW_INCLUDE)	                



###############################################################################
# Export some macros for build
###############################################################################
include	$(FACTORYSW_MAKE_DIR)/rules.mak
