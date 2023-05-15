############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmuxlib_common.inc $
# $brcm_Revision: Hydra_Software_Devel/9 $
# $brcm_Date: 9/6/12 11:46p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/syslib/muxlib/bmuxlib_common.inc $
# 
# Hydra_Software_Devel/9   9/6/12 11:46p nilesh
# SW7425-3784: Removed symlinks
# 
# Hydra_Software_Devel/SW7425-3784/1   8/30/12 3:05p nilesh
# SW7425-3784: Removed symlinks
# 
# Hydra_Software_Devel/8   8/13/12 12:28p nilesh
# SW7425-3362: Merged FNRT support
# 
# Hydra_Software_Devel/SW7425-3362/4   7/24/12 12:03p delkert
# SW7425-3370: Add Test-mode defines for FNRT
#
# Hydra_Software_Devel/SW7425-3362/3   7/5/12 1:16p nilesh
# SW7425-3362: Added BMUXLIB_FNRT_DUMP_DESC support
#
# Hydra_Software_Devel/SW7425-3362/2   7/3/12 5:27p nilesh
# SW7425-3362: Added file element "bmuxlib_fnrt_priv.c"
#
# Hydra_Software_Devel/SW7425-3362/1   7/2/12 2:57p nilesh
# SW7425-3362: Added file element "bmuxlib_fnrt.c"
#
############################################################

# Define this module
MAGNUM_MODULES += BMUXLIB_FILE_COMMON

# Module Includes
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE

BMUXLIB_FILE_COMMON_INCLUDES += \
	$(MAGNUM)/syslib/muxlib_v1243/include


BMUXLIB_FILE_COMMON_SOURCES += \
        $(MAGNUM)/syslib/muxlib_v1243/src/common/bmuxlib_list.c \
        $(MAGNUM)/syslib/muxlib_v1243/src/common/bmuxlib_output.c \
        $(MAGNUM)/syslib/muxlib_v1243/src/common/bmuxlib_input.c \
        $(MAGNUM)/syslib/muxlib_v1243/src/common/bmuxlib_fnrt.c \
        $(MAGNUM)/syslib/muxlib_v1243/src/common/bmuxlib_fnrt_priv.c

else

BMUXLIB_FILE_COMMON_INCLUDES += \
	$(MAGNUM)/syslib/muxlib/include


BMUXLIB_FILE_COMMON_SOURCES += \
        $(MAGNUM)/syslib/muxlib/src/common/bmuxlib_list.c \
        $(MAGNUM)/syslib/muxlib/src/common/bmuxlib_output.c \
        $(MAGNUM)/syslib/muxlib/src/common/bmuxlib_input.c \
        $(MAGNUM)/syslib/muxlib/src/common/bmuxlib_fnrt.c \
        $(MAGNUM)/syslib/muxlib/src/common/bmuxlib_fnrt_priv.c

endif

# DEBUG Build Flags
ifeq ($(BMUXLIB_INPUT_DUMP_DESC),y)
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_INPUT_P_DUMP_DESC=1
endif
ifeq ($(BMUXLIB_INPUT_DUMP_FRAME_DESC),y)
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_INPUT_P_DUMP_DESC=1
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_INPUT_P_DUMP_FRAME_DESC=1
endif
ifeq ($(BMUXLIB_INPUT_DUMP_METADATA_DESC),y)
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_INPUT_P_DUMP_DESC=1
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_INPUT_P_DUMP_METADATA_DESC=1
endif
ifeq ($(BMUXLIB_FNRT_DUMP_DESC),y)
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_FNRT_P_DUMP_DESC=1
endif
ifeq ($(BMUXLIB_TEST_MODE),y)
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_INPUT_P_TEST_MODE=1
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_INPUT_P_DUMP_DESC=1
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_INPUT_P_DUMP_METADATA_DESC=1
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_FNRT_P_TEST_MODE=1
BMUXLIB_FILE_COMMON_DEFINES += BMUXLIB_FNRT_P_DUMP_DESC=1
endif

