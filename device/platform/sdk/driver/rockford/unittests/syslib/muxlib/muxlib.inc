#############################################################################
#
#       Copyright (c) 2003-2011, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: muxlib.inc $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 6/22/11 9:55a $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/syslib/muxlib/muxlib.inc $
# 
# Hydra_Software_Devel/4   6/22/11 9:55a delkert
# SW7425-447: Conditionally compile ASF support
#
# Hydra_Software_Devel/3   5/11/11 12:02p nilesh
# SW7425-447: Added ASF Mux support
#
# Hydra_Software_Devel/2   4/8/11 10:34a nilesh
# SW7425-295: Changed to use bmuxlib_stream_ts.inc and
# bmuxlib_file_mp4.inc
#
# Hydra_Software_Devel/1   4/7/11 5:07p nilesh
# SW7425-297: Added muxlib.inc
#
#############################################################################

include $(MAGNUM_DIR)/syslib/muxlib/bmuxlib_common.inc
include $(MAGNUM_DIR)/syslib/muxlib/bmuxlib_stream_ts.inc
include $(MAGNUM_DIR)/syslib/muxlib/bmuxlib_file_mp4.inc
ifeq ($(MEDIA_ASF_SUPPORT), y)
   include $(MAGNUM_DIR)/syslib/muxlib/bmuxlib_file_asf.inc
endif

COMMON_INCLUDES += $(ROCKFORD_DIR)/unittests/syslib/muxlib

COMMON_SOURCES += \
	$(ROCKFORD_DIR)/unittests/syslib/muxlib/muxer.c \
	$(ROCKFORD_DIR)/unittests/syslib/muxlib/muxer_ts.c \
	$(ROCKFORD_DIR)/unittests/syslib/muxlib/muxer_file.c \
	$(ROCKFORD_DIR)/unittests/syslib/muxlib/muxer_mp4.c

ifeq ($(MEDIA_ASF_SUPPORT), y)
COMMON_SOURCES += $(ROCKFORD_DIR)/unittests/syslib/muxlib/muxer_asf.c
CFLAGS += -DASF_SUPPORTED
endif

