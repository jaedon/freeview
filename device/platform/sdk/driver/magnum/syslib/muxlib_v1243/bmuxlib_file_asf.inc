############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmuxlib_file_asf.inc $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 9/6/12 11:46p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/syslib/muxlib/bmuxlib_file_asf.inc $
# 
# Hydra_Software_Devel/4   9/6/12 11:46p nilesh
# SW7425-3784: Removed symlinks
# 
# Hydra_Software_Devel/SW7425-3784/2   9/4/12 2:24p nilesh
# SW7425-3784: Fixed check test compilation
# 
# Hydra_Software_Devel/SW7425-3784/1   8/30/12 3:05p nilesh
# SW7425-3784: Removed symlinks
# 
############################################################

# Define this module
MAGNUM_MODULES += BMUXLIB_FILE_ASF

# Module Includes
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE

BMUXLIB_FILE_ASF_INCLUDES += \
	$(MAGNUM)/syslib/muxlib_v1243/include \
	$(MAGNUM)/syslib/muxlib_v1243/src/file/asf/


BMUXLIB_FILE_ASF_SOURCES += \
        $(MAGNUM)/syslib/muxlib_v1243/src/file/asf/bmuxlib_file_asf.c \
        $(MAGNUM)/syslib/muxlib_v1243/src/file/asf/bmuxlib_file_asf_priv.c \
        $(MAGNUM)/syslib/muxlib_v1243/src/file/asf/bmuxlib_file_asf_header.c \
        $(MAGNUM)/syslib/muxlib_v1243/src/file/asf/bmuxlib_file_asf_packetizer.c

else

BMUXLIB_FILE_ASF_INCLUDES += \
	$(MAGNUM)/syslib/muxlib/include \
	$(MAGNUM)/syslib/muxlib/src/file/asf/


BMUXLIB_FILE_ASF_SOURCES += \
        $(MAGNUM)/syslib/muxlib/src/file/asf/bmuxlib_file_asf.c \
        $(MAGNUM)/syslib/muxlib/src/file/asf/bmuxlib_file_asf_priv.c \
        $(MAGNUM)/syslib/muxlib/src/file/asf/bmuxlib_file_asf_header.c \
        $(MAGNUM)/syslib/muxlib/src/file/asf/bmuxlib_file_asf_packetizer.c

endif