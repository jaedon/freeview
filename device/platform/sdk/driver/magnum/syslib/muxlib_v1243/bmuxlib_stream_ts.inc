############################################################
#     Copyright (c) 2003-2013, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmuxlib_stream_ts.inc $
# $brcm_Revision: Hydra_Software_Devel/11 $
# $brcm_Date: 4/17/13 2:54p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/syslib/muxlib/bmuxlib_stream_ts.inc $
# 
# Hydra_Software_Devel/11   4/17/13 2:54p nilesh
# SW7425-4721: Disable MTU BPP by default
# 
# Hydra_Software_Devel/10   3/26/13 2:56p nilesh
# SW7425-659: Added BMUXLIB_ENABLE_MTU_RT build option
# 
# Hydra_Software_Devel/9   3/21/13 4:06p nilesh
# SW7425-659: Enabled MTU BPP by default.
# 
# Hydra_Software_Devel/8   3/6/13 9:49a nilesh
# SW7425-659: Merge MTU BPP support. Disabled by default until XPT FW
# update.
# 
# Hydra_Software_Devel/SW7425-659/1   2/11/13 1:36p nilesh
# SW7425-659: Added BMUXLIB_ENABLE_MTU compile time option
# 
# Hydra_Software_Devel/7   1/4/13 9:48a nilesh
# SW7425-4388: Added BMUXLIB_DUMP_PCR debug
# 
# Hydra_Software_Devel/6   9/6/12 11:46p nilesh
# SW7425-3784: Removed symlinks
# 
# Hydra_Software_Devel/SW7425-3784/2   9/4/12 2:24p nilesh
# SW7425-3784: Fixed check test compilation
# 
# Hydra_Software_Devel/SW7425-3784/1   8/30/12 3:05p nilesh
# SW7425-3784: Removed symlinks
# 
# Hydra_Software_Devel/5   4/5/12 9:26a delkert
# SW7425-2567: Add TEST_MODE build option
#
# Hydra_Software_Devel/4   2/16/12 4:31p nilesh
# SW7425-2217: Merged TS VBI Userdata insertion support
# Added file element "bmuxlib_ts_userdata.c".
# Added file element "bmuxlib_ts_userdata.h".
#
# Hydra_Software_Devel/SW7425-2217/1   2/14/12 1:16p delkert
# SW7425-2217: Add new userdata files
#
# Hydra_Software_Devel/3   10/6/11 5:14p nilesh
# SW7425-721: Updated to use BMUXlib_Input routines
#
# Hydra_Software_Devel/2   5/26/11 11:54a nilesh
# SW7425-647: Moved TS Muxer to stream/ts subdirectory
#
# Hydra_Software_Devel/1   4/8/11 10:02a nilesh
# SW7425-295: Split bmuxlib.inc into bmuxlib_file_mp4.inc and
# bmuxlib_stream_ts.inc
#
# Hydra_Software_Devel/4   4/4/11 3:46p delkert
# SW7425-73: Merge mp4_mux_devel to mainline
#
# Hydra_Software_Devel/mp4_mux_devel/1   3/30/11 3:08p nilesh
# SW7425-73: Added MP4 mux
#
# Hydra_Software_Devel/3   3/2/11 10:29a nilesh
# SW7425-38: Added compile time dbg build flags
#
# Hydra_Software_Devel/2   9/21/10 5:28p nilesh
# SW7425-38: Added bmuxlib_ts.c and bmuxlib_ts_priv.c
#
# Hydra_Software_Devel/1   9/20/10 4:43p nilesh
# SW7425-38:
# Added file element "bmuxlib_ts.c".
# Added file element "bmuxlib_ts.h".
# SW7425-38: Merged muxlib to mainline
#
# Hydra_Software_Devel/SW7425-38/1   9/17/10 12:33p nilesh
# SW7425-38: Initial Muxer include
#
############################################################

# Define this module
MAGNUM_MODULES += BMUXLIB_STREAM_TS

# Module Includes
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE

BMUXLIB_STREAM_TS_INCLUDES += \
	$(MAGNUM)/syslib/muxlib_v1243/include \
	$(MAGNUM)/syslib/muxlib_v1243/src/stream/ts/


BMUXLIB_STREAM_TS_SOURCES += \
        $(MAGNUM)/syslib/muxlib_v1243/src/stream/ts/bmuxlib_ts.c \
        $(MAGNUM)/syslib/muxlib_v1243/src/stream/ts/bmuxlib_ts_priv.c \
        $(MAGNUM)/syslib/muxlib_v1243/src/stream/ts/bmuxlib_ts_userdata.c
        
else

BMUXLIB_STREAM_TS_INCLUDES += \
	$(MAGNUM)/syslib/muxlib/include \
	$(MAGNUM)/syslib/muxlib/src/stream/ts/


BMUXLIB_STREAM_TS_SOURCES += \
        $(MAGNUM)/syslib/muxlib/src/stream/ts/bmuxlib_ts.c \
        $(MAGNUM)/syslib/muxlib/src/stream/ts/bmuxlib_ts_priv.c \
        $(MAGNUM)/syslib/muxlib/src/stream/ts/bmuxlib_ts_userdata.c

endif

# DEBUG Build Flags
ifeq ($(BMUXLIB_DUMP_TRANSPORT_DESC),y)
BMUXLIB_STREAM_TS_DEFINES += BMUXLIB_TS_P_DUMP_TRANSPORT_DESC=1
endif
ifeq ($(BMUXLIB_DUMP_TRANSPORT_PES),y)
BMUXLIB_STREAM_TS_DEFINES += BMUXLIB_TS_P_DUMP_TRANSPORT_PES=1
endif
ifeq ($(BMUXLIB_DUMP_PCR),y)
BMUXLIB_STREAM_TS_DEFINES += BMUXLIB_TS_P_DUMP_PCR=1
endif
ifeq ($(BMUXLIB_TEST_MODE),y)
BMUXLIB_STREAM_TS_DEFINES += BMUXLIB_TS_P_TEST_MODE=1
endif
# ifeq ($(BMUXLIB_ENABLE_MTU),y)
BMUXLIB_STREAM_TS_DEFINES += BMUXLIB_TS_P_ENABLE_MTU=1
# ifeq ($(BMUXLIB_ENABLE_MTU_RT),y)
BMUXLIB_STREAM_TS_DEFINES += BMUXLIB_TS_P_ENABLE_MTU_RT=1
# endif
# endif
