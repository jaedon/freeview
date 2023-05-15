############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bdsp.inc $
# $brcm_Revision: Hydra_Software_Devel/11 $
# $brcm_Date: 12/13/12 8:47p $
#
# Module Description:
#
# Revision History:
#
# Created: 12/15/2010 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/dsp/bdsp.inc $
# 
# Hydra_Software_Devel/11   12/13/12 8:47p ananthan
# SW7231-1092: Correcting build failures because of incorrect include
# path.
# 
# Hydra_Software_Devel/10   11/26/12 1:11p ananthan
# SW7425-4242: Add SCM3 interface and merge SCM to mainline.
# 
# Hydra_Software_Devel/9   10/1/12 3:29p katrep
# SW7445-42:add 7445
# 
# Hydra_Software_Devel/8   9/26/12 10:46a randyjew
# SW7563-17:Add 7563 support
# 
# Hydra_Software_Devel/7   6/20/12 2:16p gprasad
# SW7425-2952: DSP directory structure flattening
# 
# Hydra_Software_Devel/6   6/18/12 12:20p gprasad
# SW7425-2952: DSP directory structure flattening
# 
# Hydra_Software_Devel/5   1/27/12 12:58p ashoky
# FWRAA-377: Merging h264 soft dsp video encoder to mainline.
# 
# Hydra_Software_Devel/4   6/8/11 4:46p gautamk
# SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
# 
# Hydra_Software_Devel/3   4/6/11 2:49p srajapur
# SW7425-291: [7425]BDSP directory structure changes
# 
# Hydra_Software_Devel/3   4/6/11 2:47p srajapur
# SW7425-291: [7425]BDSP directory structure changes
# 
# Hydra_Software_Devel/2   12/15/10 6:47p jgarrett
# SW7422-146: Initial compileable prototype
# 
# Hydra_Software_Devel/1   12/15/10 1:59p jgarrett
# SW7422-146: Adding initial BDSP files
# 
############################################################

#
# Conditional Options for this Makefile
#
# BDSP_AUDIO_SUPPORT (default=y) - Include audio support
# BDSP_VIDEO_SUPPORT (default=n) - Include video support
# BDSP_IMG_ONLY (default=n) - Include firmware only
# BDSP_IMG_EXTERNAL (default=n) - Include non-firmware files only
#

# Define this module
MAGNUM_MODULES += BDSP

# Set Default conditional options
BDSP_AUDIO_SUPPORT ?= y
BDSP_VIDEO_SUPPORT ?=y
BDSP_VIDEO_ENCODE_SUPPORT ?= y
BDSP_SCM_SUPPORT ?=y

# Include standard files
BDSP_INCLUDES := $(MAGNUM)/basemodules/dsp/include

ifneq ($(BDSP_IMG_ONLY),y)
BDSP_SOURCES := \
        $(MAGNUM)/basemodules/dsp/src/bdsp.c \
        $(MAGNUM)/basemodules/dsp/src/bdsp_context.c \
        $(MAGNUM)/basemodules/dsp/src/bdsp_task.c \
        $(MAGNUM)/basemodules/dsp/src/bdsp_priv.c
        
# Include conditional files
ifeq ($(BDSP_AUDIO_SUPPORT),y)
BDSP_SOURCES += $(MAGNUM)/basemodules/dsp/src/bdsp_audio_task.c
BDSP_DEFINES += BDSP_AUDIO_SUPPORT=1
endif
ifeq ($(BDSP_VIDEO_SUPPORT),y)
BDSP_SOURCES += $(MAGNUM)/basemodules/dsp/src/bdsp_video_task.c
BDSP_DEFINES += BDSP_VIDEO_SUPPORT=1
endif

ifeq ($(BDSP_VIDEO_ENCODE_SUPPORT),y)
BDSP_SOURCES += $(MAGNUM)/basemodules/dsp/src/bdsp_video_encode_task.c
BDSP_DEFINES += BDSP_VIDEO_ENCODE_SUPPORT=1
endif
ifeq ($(BDSP_SCM_SUPPORT),y)
BDSP_SOURCES += $(MAGNUM)/basemodules/dsp/src/bdsp_scm_task.c
BDSP_DEFINES += BDSP_SCM_SUPPORT=1
endif

endif #BDSP_IMG_ONLY != y

# DSP specifics
# Define the DSP type(s) used on this chipset
BDSP_RAAGA_SUPPORT := y

# Define Fp version for all supported chips
ifeq ($(BCHP_CHIP),7358)
BDSP_RAAGA_FP_VER := fp2008
endif

ifeq ($(BCHP_CHIP),7552)
ifeq ($(BCHP_VER_LOWER),a0)
BDSP_RAAGA_FP_VER := fp2008
else
BDSP_RAAGA_FP_VER := fp2011
endif
endif

### A0 build is not supported for the below chips ###
### A0 versions for the below chips are fp2008 ver ##
ifeq ($(BCHP_CHIP),7231)
BDSP_RAAGA_FP_VER := fp2011
endif

ifeq ($(BCHP_CHIP),7425)
BDSP_RAAGA_FP_VER := fp2011
endif

ifeq ($(BCHP_CHIP),7344)
BDSP_RAAGA_FP_VER := fp2011
endif

ifeq ($(BCHP_CHIP),7346)
BDSP_RAAGA_FP_VER := fp2011
endif

######################################################

ifeq ($(BCHP_CHIP),7360)
BDSP_RAAGA_FP_VER := fp2011
endif

ifeq ($(BCHP_CHIP),7429)
BDSP_RAAGA_FP_VER := fp2011
endif

ifeq ($(BCHP_CHIP),7584)
BDSP_RAAGA_FP_VER := fp2011
endif

ifeq ($(BCHP_CHIP),7435)
BDSP_RAAGA_FP_VER := fp2011
endif


ifeq ($(BCHP_CHIP),7445)
BDSP_RAAGA_FP_VER := fp2011
endif

ifeq ($(BCHP_CHIP),7563)
BDSP_RAAGA_FP_VER := fp2011
endif

# Include Raaga files
include $(MAGNUM)/basemodules/dsp/build/bdsp_raaga.inc

