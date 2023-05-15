############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: common.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 9/10/12 12:46p $
#
# Module Description:
#
# Revision History:
#
# Created: 
#
# $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/common.inc $
# 
# Hydra_Software_Devel/3   9/10/12 12:46p jgarrett
# SW7425-3856: Using new bvlc location
# 
# Hydra_Software_Devel/2   7/11/12 2:18p jgarrett
# SW7425-3457: baa_tree moved to magnum/commonutils/lst
# 
# Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
# SW7425-3457: Adding ape_decode application
# 
# 
############################################################


# Directories relative to location of makefile
BSEAV_DIR = $(MAGNUM_DIR)/../BSEAV

ifneq ($(APETEST_COMMON_ENABLED),n)
# media probe (only needed for playback and record)
BSEAV = $(BSEAV_DIR)
include $(BSEAV)/lib/bfile/bfile.inc
ifeq ($(MEDIA_AVI_SUPPORT),y)
include $(BSEAV)/lib/avi/bavi.inc
endif
ifeq ($(MEDIA_ASF_SUPPORT),y)
include $(BSEAV)/lib/asf/basf.inc
endif
ifeq ($(MEDIA_RMFF_SUPPORT),y)
include $(BSEAV)/lib/media/rmff/brmff.inc
endif
include $(BSEAV)/lib/utils/batom.inc
include $(BSEAV)/lib/media/bmedia.inc
include $(MAGNUM)/commonutils/vlc/bvlc.inc


COMMON_CFLAGS += $(BMEDIA_PROBE_DEFINES)
COMMON_SOURCES += $(BMEDIA_SOURCES)
COMMON_SOURCES += $(BMEDIA_PROBE_SOURCES) $(BMEDIA_FILTER_SOURCES) 

COMMON_SOURCES += $(BFILE_SOURCES) $(BFILE_MEDIA_SOURCES) $(BFILE_STDIO_SOURCES)
COMMON_SOURCES += $(BATOM_SOURCES) 
#COMMON_SOURCES += $(BMEDIA_PLAYER_SOURCES)
COMMON_SOURCES += ${BSEAV}/lib/media/bmpeg2ts_parser.c \
	${BSEAV}/lib/media/bmpeg2ts_player.c \
	${BSEAV}/lib/media/bmpeg2pes_player.c \
	${BSEAV}/lib/media/btime_indexer.c \
	${BSEAV}/lib/otfpvr/botf_marker.c

COMMON_SOURCES += ${B_MEDIA_PLAYER_CORE_SOURCES} ${B_MEDIA_PLAYER_MP4_SOURCES} ${B_MEDIA_PLAYER_MKV_SOURCES}
COMMON_SOURCES += ${BASF_PLAYER_SOURCES} ${BAVI_PLAYER_SOURCES} ${BRMFF_PLAYER_SOURCES}
    
COMMON_INCLUDES += $(BMEDIA_PLAYER_INCLUDES) $(BMEDIA_INCLUDES) $(BFILE_MEDIA_INCLUDES)
COMMON_INCLUDES += $(BFILE_INCLUDES) $(BFILE_MEDIA_INCLUDES) $(BFILE_STDIO_INCLUDES)
#COMMON_INCLUDES += $(BSEAV)/lib/utils 

APP_SOURCES += apetest_board.c apetest_cmdline.c apetest_utils.c apetest_pblib.c apetest_capture.c 
CFLAGS += -DAPETEST_COMMON_ENABLED=1

endif
#APETEST_COMMON_ENABLED


ifneq ($(APETEST_HDMITX_ENABLED),n)
CFLAGS += -DAPETEST_HDMITX_ENABLED=1
include $(MAGNUM_DIR)/portinginterface/hdm/bhdm.inc
include $(MAGNUM_DIR)/portinginterface/vdc/bvdc.inc
APP_SOURCES += apetest_hdmitx.c
endif

