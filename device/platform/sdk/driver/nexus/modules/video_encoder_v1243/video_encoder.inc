############################################################
#     (c)2010-2012 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile: video_encoder.inc $
# $brcm_Revision: 5 $
# $brcm_Date: 11/9/12 4:53p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/video_encoder/video_encoder.inc $
# 
# 5   11/9/12 4:53p mphillip
# SW7425-4156: Update VICE security support
# 
# 4   10/1/12 3:49p katrep
# SW7445-1:add 7445 support
# 
# 3   7/17/12 1:19p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 2   12/13/10 7:02p vsilyaev
# SW7425-40: Added system initialization for the StreamMux and
#  VideoEncoder modules
# 
# 1   12/9/10 8:11p vsilyaev
# SW7425-40: Video Encoder module
# 
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += VIDEO_ENCODER

##################
# Module Options #
##################

###################
# Module Includes #
###################
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE
NEXUS_VIDEO_ENCODER_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/video_encoder_v1243/include
else
NEXUS_VIDEO_ENCODER_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/video_encoder/include
endif
##########################
# Common Module Settings #
##########################
NEXUS_VIDEO_ENCODER_DEPENDENCIES += BASE CORE SECURITY DISPLAY SURFACE TRANSPORT

# thunk layer
NEXUS_VIDEO_ENCODER_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_video_encoder_thunks.c
NEXUS_VIDEO_ENCODER_PRIVATE_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)

ifeq ($(BDSP_ENCODER_SUPPORT),y)

#######################
# DSP video encoder   #
#######################
NEXUS_VIDEO_ENCODER_DEPENDENCIES += AUDIO
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE
NEXUS_VIDEO_ENCODER_SOURCES += $(NEXUS_TOP)/modules/video_encoder_v1243/src/dsp/nexus_video_encoder.c 
NEXUS_VIDEO_ENCODER_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/video_encoder_v1243/src/dsp
else
NEXUS_VIDEO_ENCODER_SOURCES += $(NEXUS_TOP)/modules/video_encoder/src/dsp/nexus_video_encoder.c 
NEXUS_VIDEO_ENCODER_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/video_encoder/src/dsp
endif
else

#######################
# VCE video encoder   #
#######################
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE
NEXUS_VIDEO_ENCODER_SOURCES += $(NEXUS_TOP)/modules/video_encoder_v1243/src/nexus_video_encoder.c 
NEXUS_VIDEO_ENCODER_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/video_encoder_v1243/src 
else
NEXUS_VIDEO_ENCODER_SOURCES += $(NEXUS_TOP)/modules/video_encoder/src/nexus_video_encoder.c 
NEXUS_VIDEO_ENCODER_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/video_encoder/src 
endif

ifeq ($(NEXUS_CONFIG_IMAGE),y)
ifeq ($(NEXUS_MODE),proxy)
BVCE_IMG_ONLY := y
else
BVCE_IMG_EXTERNAL := y
endif
endif

# Allow NEXUS_POWER_MANAGEMENT to be a master switch
ifeq ($(NEXUS_POWER_MANAGEMENT),n)
BVCE_POWER_MANAGEMENT := n
else
# VCE defaults off for settops, so force it on
BVCE_POWER_MANAGEMENT := y
endif

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
# Note that we have to include vbilib and vbi just to get dccparse.
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE
NEXUS_VIDEO_ENCODER_MAGNUM_MODULES :=  \
    $(MAGNUM)/portinginterface/vce_v1243/bvce.inc \
    $(MAGNUM)/commonutils/img/bimg.inc 
else
NEXUS_VIDEO_ENCODER_MAGNUM_MODULES :=  \
    $(MAGNUM)/portinginterface/vce/bvce.inc \
    $(MAGNUM)/commonutils/img/bimg.inc 
endif

# Used to load the new ARC600 based encoder
ifeq ($(filter $(BCHP_CHIP), 7422 7425 7445), $(BCHP_CHIP))
NEXUS_VIDEO_ENCODER_MAGNUM_MODULES +=  \
    $(MAGNUM)/commonutils/afl/bafl.inc
endif
ENCODER_CALLBACK := n
ifeq ($(VICE_REGION_VERIFICATION_SUPPORT),y)
ENCODER_CALLBACK := y
endif
ifeq ($(VICH_SUPPORT),y)
ENCODER_CALLBACK := y
endif

ifeq ($(ENCODER_CALLBACK),y)
include $(NEXUS_VIDEO_ENCODER_SECURITY_EXTENSION_INC)
endif

endif