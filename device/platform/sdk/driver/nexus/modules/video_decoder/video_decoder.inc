############################################################
#     (c)2007-2012 Broadcom Corporation
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
# $brcm_Workfile: video_decoder.inc $
# $brcm_Revision: 51 $
# $brcm_Date: 10/1/12 3:49p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/video_decoder/video_decoder.inc $
# 
# 51   10/1/12 3:49p katrep
# SW7445-1:add 7445 support
# 
# 50   9/21/12 6:03p randyjew
# SW7563-2: Add 7563 support.
# 
# 49   9/6/12 8:38p bandrews
# SW7231-967: disable garbage fifo watchdog by default
# 
# 48   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 47   6/8/12 2:29p erickson
# SW7584-35: add 7584, remove unused chip
# 
# 46   5/15/12 7:11p bselva
# SW7360-19: Add support for 7360 platform
# 
# 45   1/24/12 3:20p vsilyaev
# SW7425-2258: Use common file for open/init time decoder dispatch
# 
# 44   1/24/12 2:44p vsilyaev
# SW7425-2258: Added Nexus video decoder extension to decode MJPEG
# 
# 43   11/3/11 5:41p mward
# SW7435-7:  Add 7435.
# 
# 42   10/25/11 5:28p katrep
# SW7429-1:updated for 7429 bringup
# 
# 41   7/22/11 11:17a erickson
# SW7552-82: fix logic for OTFPVR_SUPPORT
# 
# 40   7/11/11 2:27p nickh
# SW7422-404: Security System test
# 
# 39   6/20/11 5:41p jgarrett
# SW7425-409: Merging VP6 support to main branch
# 
# SW7425-409/1   6/6/11 7:16p jgarrett
# SW7425-409: Adding DSP video decoder extensions for VDE
# 
# 38   5/16/11 10:24a erickson
# SWDEPRECATED-3571: fix OTFPVR_SUPPORT with kernel mode proxy
# 
# 37   3/30/11 3:52p jhaberf
# SW35330-13: enabled afl for 35233 DTV chip
# 
# 36   2/25/11 10:39a gmohile
# SW7408-210 : Check for NEXUS_POWER_MANAGEMENT=y
# 
# 35   1/18/11 9:29a erickson
# SW7358-26: add 7358
# 
# SW7358-26/1   12/28/10 3:04p xhuang
# SW7358-26: Add 7358/7552 support
# 
# 34   12/15/10 12:03p erickson
# SW35230-2585: reduce DEPENDENCY list, depend on recursion
# 
# 33   12/2/10 11:45a katrep
# SW7231-4:enabled afl for 7231,7346,7344
# 
# 32   11/2/10 6:06p hongtaoz
# SW7425-9: add 7425 support;
# 
# 31   10/13/10 11:13a nickh
# SW7422-73: Add commonutils/afl directory needed for loading new ARC600
#  on 7422
# 
# 30   9/22/10 12:39p jtna
# SW7405-4865: moved bvlc.c and .h to magnum/commonutils/vlc
# 
# 29   9/8/10 4:15p vsilyaev
# SW7468-129: Postmerge fixes
# 
# 28   9/8/10 12:25p vsilyaev
# SW7468-129: Added video decoder on ZSP
# 
# 27   8/10/10 1:12p erickson
# SW3548-3022: switch to UDP (userdata parser) for userdata parsing
#
# 26   6/23/10 12:11p erickson
# SW7550-453: refactor NEXUS_StripedSurface to use
#  nexus_striped_surface.h API
#
# 25   2/25/10 6:06p gmohile
# SW7405-2993 : Merge xdm support
# 
# SW7405-2993/3   2/19/10 2:38p nilesh
# SW7405-2993: Fixed duplicate clearcase keyword warning
# 
# 24   11/24/09 1:06p erickson
# SW7420-457: add REGION_VERIFICATION_SUPPORT compile time options
# 
# SW7405-2993/1   11/20/09 4:47p nilesh
# SW7405-2993: Added XDM commonutil
# 
# 23   9/29/09 11:25a jtna
# SWDEPRECATED-4001: add bvlc.c for kernelmode
# 
# 22   9/16/09 1:41p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 21   9/1/09 3:52p jgarrett
# SW7335-529: Replacing direct access to MAGNUM_MODULES with
#  NEXUS_EXTRA_MODULES to avoid magnum paths leaking into application
#  include paths
#
# 20   7/15/09 1:55p erickson
# PR56522: add nexus_video_decoder_primer.c
#
# 19   6/25/09 2:28p erickson
# PR50407: default BXVD_POWER_MANAGEMENT on (XVD defaults it off)
#
# 17   4/8/09 4:54p vsilyaev
# PR 54015: Don't include bkni_multi.h into the bdbg.h. All thread-aware
#  modules should include explicitly bkni_multi.h
#
# 16   3/31/09 7:29p vsilyaev
# PR 50887: OTFPVR support for the Nexus video decoder
#
# 15   3/16/09 11:27a yili
# PR52990:Add region verification - Minor change to meet Nexus design
#  guideline.
#
# 14   3/9/09 4:46p yili
# PR52990:Adding region verification
#
# 13   12/31/08 1:27p erickson
# PR50787: Allow NEXUS_POWER_MANAGEMENT to be a master switch
#
# 12   8/21/08 3:33p erickson
# PR45959: flatten still_ext and trick_ext into normal code
#
# 11   8/21/08 3:16p katrep
# PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
#
# 10   5/20/08 3:42p erickson
# PR42128: added dataReadyCallback to ExtendedSettings
#
# 9   5/8/08 2:48p erickson
# PR42438: derive NEXUS_VideoInputStatus.format for digital decode using
#  common function
#
# 8   5/5/08 11:45a jrubio
# PR40169: add still decode to 7335/7325
#
# 7   5/2/08 1:18p jrubio
# PR42336: exclude 7335/7325 for the time being
#
# 6   4/29/08 6:02p jrubio
# PR42336: make sure that if num of decodes is 0 then still is not
#  compiled in
#
# 5   4/17/08 4:38p erickson
# PR41533: rework NEXUS_VideoDecoder_GetUserDataBuffer to use
#  NEXUS_UserDataHeader before payload
#
# 4   4/7/08 4:10p erickson
# PR32888: added NEXUS_CRC_CAPTURE
#
# 3   3/25/08 12:54p erickson
# PR40850: add more status
#
# 2   2/20/08 4:57p erickson
# PR39786: initial impl
#
# 1   1/18/08 2:16p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/7   12/3/07 10:09a erickson
# PR34926: reorg video_decoder extensions
#
# Nexus_Devel/6   10/15/07 3:23p vsilyaev
# PR 35824: Added video_decoder module trick_ext exension
#
# Nexus_Devel/5   10/3/07 1:06p erickson
# PR34926: impl userdata
#
# Nexus_Devel/4   9/28/07 1:20p erickson
# PR35395: added video_decoder thunk
#
# Nexus_Devel/3   9/28/07 11:32a erickson
# PR34926: rename videodecoder to video_decoder
#
# Nexus_Devel/2   9/26/07 11:01a erickson
# PR34926: refactor implementation into _priv source file
#
# Nexus_Devel/1   9/14/07 4:14p erickson
# PR34926: initial compile
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += VIDEO_DECODER

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_VIDEO_DECODER_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/video_decoder/include
NEXUS_VIDEO_DECODER_PRECOMPILED_HEADER := $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_module.h

##################
# Module Sources #
##################
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE
NEXUS_VIDEO_DECODER_SOURCES := \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_avd_interface.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_common.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_extra.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_userdata_v1243.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_priv.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_primer.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_still_decoder.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_still_decoder_priv.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_trick.c
else
NEXUS_VIDEO_DECODER_SOURCES := \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_avd_interface.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_common.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_extra.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_userdata.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_priv.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_primer.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_still_decoder.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_still_decoder_priv.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_trick.c
endif

# Look for DSP video decoder first 
ifneq ($(findstring NEXUS_NUM_DSP_VIDEO_DECODERS, $(NEXUS_PLATFORM_DEFINES)),)
include ${NEXUS_TOP}/extensions/video_decoder/dsp_video_decoder/dsp_video_decoder.inc
NEXUS_VIDEO_DECODER_DYNAMIC_DISPATCH:=y
else
# No DSP video decoder, Look for ZSP video decoder
ifneq ($(findstring NEXUS_NUM_ZSP_VIDEO_DECODERS, $(NEXUS_PLATFORM_DEFINES)),)
include ${NEXUS_TOP}/modules/video_decoder/zspdecode_ext/zspdecode_ext.inc
NEXUS_VIDEO_DECODER_DYNAMIC_DISPATCH:=y
endif
endif

ifneq ($(findstring NEXUS_NUM_SID_VIDEO_DECODERS, $(NEXUS_PLATFORM_DEFINES)),)
NEXUS_VIDEO_DECODER_DYNAMIC_DISPATCH:=y
include ${NEXUS_TOP}/extensions/video_decoder/sid_video_decoder/sid_video_decoder.inc
endif

ifdef NEXUS_VIDEO_DECODER_DYNAMIC_DISPATCH
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_dispatch.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_dispatch_init.c \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_xdm.c 
else
#NO SID/ZSP/DSP video decoder
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_avd.c 
endif


# thunk layer
NEXUS_VIDEO_DECODER_PRIVATE_INCLUDES += \
    $(NEXUS_TOP)/modules/video_decoder/src $(NEXUS_SYNCTHUNK_DIR)

NEXUS_VIDEO_DECODER_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_video_decoder_thunks.c

ifeq ($(NEXUS_CRC_CAPTURE),y)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_CRC_CAPTURE=1
endif

ifeq ($(NEXUS_VIDEO_DECODER_GARBAGE_FIFO_WATCHDOG_SUPPORT),y)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_VIDEO_DECODER_GARBAGE_FIFO_WATCHDOG_SUPPORT=1
endif

#######################
# Module Dependencies #
#######################
NEXUS_VIDEO_DECODER_DEPENDENCIES += TRANSPORT SURFACE

######################
# Module Definitions #
######################

ifeq ($(NEXUS_CONFIG_IMAGE),y)
ifeq ($(NEXUS_MODE),proxy)
BXVD_IMG_ONLY := y
else
BXVD_IMG_EXTERNAL := y
endif
endif

# Allow NEXUS_POWER_MANAGEMENT to be a master switch
ifneq ($(NEXUS_POWER_MANAGEMENT),y)
BXVD_POWER_MANAGEMENT := n
else
# XVD defaults off for settops, so force it on
BXVD_POWER_MANAGEMENT := y
endif

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
# Note that we have to include vbilib and vbi just to get dccparse.
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE

NEXUS_VIDEO_DECODER_MAGNUM_MODULES :=  \
    $(MAGNUM)/portinginterface/xvd/bxvd.inc \
    $(MAGNUM)/portinginterface/vbi/bvbi.inc \
    $(MAGNUM)/commonutils/img/bimg.inc \
    $(MAGNUM)/commonutils/xdm/bxdm.inc \
    $(MAGNUM)/commonutils/udp_v1243/budp.inc \
    $(MAGNUM)/commonutils/vlc/bvlc.inc 
    
else # HUMAX_PLATFORM_BASE

NEXUS_VIDEO_DECODER_MAGNUM_MODULES :=  \
    $(MAGNUM)/portinginterface/xvd/bxvd.inc \
    $(MAGNUM)/portinginterface/vbi/bvbi.inc \
    $(MAGNUM)/commonutils/img/bimg.inc \
    $(MAGNUM)/commonutils/xdm/bxdm.inc \
    $(MAGNUM)/commonutils/udp/budp.inc \
    $(MAGNUM)/commonutils/vlc/bvlc.inc 

endif # HUMAX_PLATFORM_BASE

# Used to load the new ARC600 based decoder
ifeq ($(filter $(BCHP_CHIP), 7422 7425 7429 7435 7445 7231 7346 7344 7358 7360 7563 7552 7584), $(BCHP_CHIP))
NEXUS_VIDEO_DECODER_MAGNUM_MODULES +=  \
    $(MAGNUM)/commonutils/afl/bafl.inc
endif


ifeq ($(OTFPVR_SUPPORT),y)
NEXUS_VIDEO_DECODER_MAGNUM_MODULES += ${BSEAV}/lib/otfpvr/botfpvr.inc
# BSEAV makefiles don't include their module names into MAGNUM_MODULES. must conditionally include into the driver, not proxy.
ifneq ($(findstring $(NEXUS_MODE),proxy client),)
else
NEXUS_EXTRA_MODULES += BOTFPVR
endif
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_OTFPVR=1
NEXUS_VIDEO_DECODER_SOURCES += $(NEXUS_TOP)/modules/video_decoder/src/nexus_video_decoder_otf_trick.c
endif

VIDEO_CALLBACK := n
ifeq ($(VIDEO_REGION_VERIFICATION_SUPPORT),y)
VIDEO_CALLBACK := y
endif
ifeq ($(VICH_SUPPORT),y)
VIDEO_CALLBACK := y
endif


ifeq ($(VIDEO_CALLBACK),y)
include $(NEXUS_VIDEO_DECODER_SECURITY_EXTENSION_INC)
endif

#####################
# Module Extensions #
#####################
ifneq ($(NEXUS_VIDEO_DECODER_EXTENSION_INC),)
include $(NEXUS_VIDEO_DECODER_EXTENSION_INC)
endif

