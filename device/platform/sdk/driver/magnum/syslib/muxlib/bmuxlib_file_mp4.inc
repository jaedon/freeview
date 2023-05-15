############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmuxlib_file_mp4.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 9/6/12 11:46p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/syslib/muxlib/bmuxlib_file_mp4.inc $
# 
# Hydra_Software_Devel/5   9/6/12 11:46p nilesh
# SW7425-3784: Removed symlinks
# 
# Hydra_Software_Devel/SW7425-3784/2   9/4/12 2:24p nilesh
# SW7425-3784: Fixed check test compilation
# 
# Hydra_Software_Devel/SW7425-3784/1   8/30/12 3:05p nilesh
# SW7425-3784: Removed symlinks
# 
# Hydra_Software_Devel/4   5/29/12 1:32p delkert
# SW7425-2567: Add test mode compile option
#
# Hydra_Software_Devel/3   5/26/11 11:54a nilesh
# SW7425-647: Moved MP4 Muxer to file/mp4 subdirectory
#
# Hydra_Software_Devel/2   5/13/11 11:57a delkert
# SW7425-446:
# Added file element "bmuxlib_file_mp4_boxes.c".
# Added file element "bmuxlib_file_mp4_boxes.h".
# Added file element "bmuxlib_file_mp4_metadata.c".
# Added file element "bmuxlib_file_mp4_metadata.h".
#
# Hydra_Software_Devel/1   4/8/11 10:02a nilesh
# SW7425-295: Split bmuxlib.inc into bmuxlib_file_mp4.inc and
# bmuxlib_stream_ts.inc
#
############################################################

# Define this module
MAGNUM_MODULES += BMUXLIB_FILE_MP4

# Module Includes
BMUXLIB_FILE_MP4_INCLUDES += \
	$(MAGNUM)/syslib/muxlib/include \
	$(MAGNUM)/syslib/muxlib/src/file/mp4


BMUXLIB_FILE_MP4_SOURCES += \
        $(MAGNUM)/syslib/muxlib/src/file/mp4/bmuxlib_file_mp4.c \
        $(MAGNUM)/syslib/muxlib/src/file/mp4/bmuxlib_file_mp4_priv.c \
        $(MAGNUM)/syslib/muxlib/src/file/mp4/bmuxlib_file_mp4_boxes.c \
        $(MAGNUM)/syslib/muxlib/src/file/mp4/bmuxlib_file_mp4_metadata.c

ifeq ($(BMUXLIB_TEST_MODE),y)
BMUXLIB_FILE_MP4_DEFINES += BMUXLIB_MP4_P_TEST_MODE=1
endif