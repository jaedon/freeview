############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bsynclib.inc $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 9/6/12 8:52p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/syslib/synclib/bsynclib.inc $
# 
# Hydra_Software_Devel/4   9/6/12 8:52p bandrews
# SW7425-3850: merge to main
# 
# Hydra_Software_Devel/SW7425-3850/1   9/4/12 5:48p bandrews
# SW7425-3850: remove symlinks
# 
# Hydra_Software_Devel/3   1/26/10 9:01p bandrews
# SW7405-3774: added mute control support
# 
# Hydra_Software_Devel/2   12/10/09 9:18p bandrews
# SW7401-3634: adding PWC (now JTI) support to synclib
# 
# Hydra_Software_Devel/1   3/24/08 3:09p bandrews
# PR40865: Fixed
# 
# Hydra_Software_Devel/6   1/31/08 3:19p bandrews
# PR37951: Fixed build error
# 
# Hydra_Software_Devel/5   1/31/08 3:17p bandrews
# PR37951: Updated to default to real impl, not stubs
# 
# Hydra_Software_Devel/4   1/30/08 4:32p erickson
# PR37951: added syslib include path
# 
# Hydra_Software_Devel/3   1/3/08 5:17p bandrews
# PR37951: Updated based on initial feedback
# 
# Hydra_Software_Devel/2   12/12/07 2:53p bandrews
# PR37951: Initial check-in
# 
# Hydra_Software_Devel/1   12/5/07 10:59p bandrews
# PR37951: Check in stubs for external work
############################################################

# Define this module
MAGNUM_MODULES += BSYNCLIB

# Module Includes
BSYNCLIB_INCLUDES += $(MAGNUM)/syslib/synclib/include

ifeq ($(BSYNCLIB_STUBS),y) 
BSYNCLIB_SOURCES += \
	$(MAGNUM)/syslib/synclib/src/bsynclib_stubs.c
else # BSYNCLIB_STUBS=n or not defined
BSYNCLIB_SOURCES += \
	$(MAGNUM)/syslib/synclib/src/bsynclib.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_priv.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_audio_sink.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_audio_source.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_video_format.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_video_sink.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_video_source.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_delay_element.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_resource_pool.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_rate_mismatch_detector.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_mute_control.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_state_machine.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_timer.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_algo.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_channel.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_channel_priv.c \
	$(MAGNUM)/syslib/synclib/src/bsynclib_channel_config.c 
endif

ifeq ($(BSYNCLIB_JTI_SUPPORT),y)
BSYNCLIB_DEFINES += BSYNCLIB_JITTER_TOLERANCE_IMPROVEMENT_SUPPORT
endif

