#############################################################
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
# $brcm_Workfile: display.inc $
# $brcm_Revision: 28 $
# $brcm_Date: 10/10/12 10:32a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/display/display.inc $
# 
# 28   10/10/12 10:32a erickson
# SW7408-346: support graphics-only build
# 
# 27   10/1/12 3:49p katrep
# SW7445-1:add 7445 support
# 
# 26   7/16/12 1:19p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 25   1/16/12 6:04p gmohile
# SW7425-1473 : Merge raaga encoder support
# 
# 24   12/20/11 5:24p hongtaoz
# SW7425-1878: added default for encoder support;
# 
# 23   12/19/11 11:11a hongtaoz
# SW7425-1878: fixed VIDEO_ENCODER_SUPPORT=n compile;
# 
# 22   12/16/11 5:28p erickson
# SW7425-1878: merge XUDlib support for video encoder userdata
# 
# SW7425-1878/1   12/9/11 1:31p ajagmag
# SW7425-1878: Integrate XUDlib into nexus
# 
# 21   8/10/11 4:55p erickson
# SWDTV-4755: reduce DEPENDENCY list, depend on recursion
# 
# 20   6/13/11 5:41p hongtaoz
# SW7425-686: add BVN error debug message build;
# 
# 19   12/2/10 8:13p spothana
# SW7420-1177: Adding nexus hdmi dvo module.
# 
# SW7420-1177/1   11/1/10 8:44p spothana
# SW7420-1177: Adding nexus hdmi dvo module.
# 
# 18   10/22/10 2:23p erickson
# SW7422-69: add HDMI_INPUT for 7422 et al.
# 
# 17   10/22/10 12:09p erickson
# SW7422-69: move hdmi input connection to 7400 for 7422 support
# 
# 16   6/1/10 9:39p bandrews
# SW3548-2323: unlinked work on 7420 with 35230, input and display only
#  tuned on latter
# 
# 15   7/23/09 10:39a erickson
# PR56969: add hddvi input
#
# 14   4/8/09 4:54p vsilyaev
# PR 54015: Don't include bkni_multi.h into the bdbg.h. All thread-aware
#  modules should include explicitly bkni_multi.h
#
# 13   3/30/09 1:07p erickson
# PR43491: remove code that disables I2S outputs when enabling 656
#  inputs. if this is required in the future, it should be done in the
#  platform layer.
#
# 12   8/21/08 3:02p erickson
# PR45959: add sample extension
#
# 11   7/1/08 5:03p erickson
# PR44391: Add RUL Log Capability to Help VDC Debug
#
# 10   6/12/08 11:58a vsilyaev
# PR 43491: Disable I2S outputs when enabling 656 input
#
# 9   6/11/08 12:46p vsilyaev
# PR 43491: Added support for CCIR-656 input
#
# 8   6/3/08 9:39a ahulse
# PR43266: Add support for DCS in nexus
#
# 7   4/7/08 4:10p erickson
# PR32888: added NEXUS_CRC_CAPTURE
#
# 6   4/3/08 10:17a erickson
# PR41122: move picture quality code to 7400
#
# 5   3/21/08 12:16p vsilyaev
# PR 39986: Added VideoImageInput
#
# 4   2/20/08 1:33p erickson
# PR39405: added RFM output
#
# 3   2/7/08 2:31p erickson
# PR38701: added macrovision implementation with appropriate #deifne
#
# 2   2/1/08 5:25p jgarrett
# PR 39017: Adding HdmiOutput
#
# 1   1/18/08 2:14p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/13   1/11/08 1:51p erickson
# PR38679: add vbi read/write api's
#
# Nexus_Devel/12   1/3/08 8:43p erickson
# PR34662: refactor into smaller files
#
# Nexus_Devel/11   11/8/07 9:16a erickson
# PR36802: added mosaic mode
#
# Nexus_Devel/10   10/4/07 12:20p vsilyaev
# PR 34662: Coded VBI support
#
# Nexus_Devel/9   9/28/07 12:25p vsilyaev
# PR 34662: Added sync thunk layer
#
# Nexus_Devel/8   9/28/07 11:32a erickson
# PR34926: rename videodecoder to video_decoder
#
# Nexus_Devel/7   9/26/07 1:43p vsilyaev
# PR 34662: Implementation of display framebuffer
#
# Nexus_Devel/6   9/24/07 12:21p vsilyaev
# PR 34662: Added connection of window and source
#
# Nexus_Devel/5   9/21/07 4:11p vsilyaev
# PR 34662: Added VideoInput API
#
# Nexus_Devel/4   9/17/07 6:48p vsilyaev
# PR 34662: Coded more implementation
#
# Nexus_Devel/3   9/13/07 2:39p vsilyaev
# PR 34419: Updated for new directory layout
#
# Nexus_Devel/2   9/12/07 1:52p vsilyaev
# PR 34419: Updated display API
#
# Nexus_Devel/1   9/7/07 12:27p vsilyaev
# PR 34419: Added skeleton implementation
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += DISPLAY

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_DISPLAY_PRIVATE_INCLUDES = $(NEXUS_TOP)/modules/display/src
NEXUS_DISPLAY_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/display/include


NEXUS_DISPLAY_PRECOMPILED_HEADER = $(NEXUS_TOP)/modules/display/src/nexus_display_module.h

##################
# Module Sources #
##################
NEXUS_DISPLAY_SOURCES := \
    $(NEXUS_TOP)/modules/display/src/nexus_ccir656_input.c \
    $(NEXUS_TOP)/modules/display/src/nexus_hddvi_input.c \
    $(NEXUS_TOP)/modules/display/src/nexus_display.c \
    $(NEXUS_TOP)/modules/display/src/nexus_display_graphics.c \
    $(NEXUS_TOP)/modules/display/src/nexus_display_module.c \
    $(NEXUS_TOP)/modules/display/src/nexus_display_vbi.c \
    $(NEXUS_TOP)/modules/display/src/nexus_display_mosaic.c \
    $(NEXUS_TOP)/modules/display/src/nexus_hdmi_input_connection.c \
    $(NEXUS_TOP)/modules/display/src/nexus_picture_ctrl.c \
    $(NEXUS_TOP)/modules/display/src/nexus_video_adj.c \
    $(NEXUS_TOP)/modules/display/src/nexus_video_image_input.c \
    $(NEXUS_TOP)/modules/display/src/nexus_video_input.c \
    $(NEXUS_TOP)/modules/display/src/nexus_video_input_vbi.c \
    $(NEXUS_TOP)/modules/display/src/nexus_video_output.c \
    $(NEXUS_TOP)/modules/display/src/nexus_video_window.c \
    $(NEXUS_TOP)/modules/display/src/nexus_video_window_tune.c
    
# check the VDC's env variable
ifeq ($(BVDC_MACROVISION),y)
NEXUS_DISPLAY_DEFINES += MACROVISION_SUPPORT=1
else
# DCS and MACROVISION cannot be set at same time
ifeq ($(BVDC_DCS),y)
NEXUS_DISPLAY_DEFINES += DCS_SUPPORT=1
endif
endif

# capture VDC RUL's to disk for debug
# on the reference board, define capture_ruls to the filename
ifeq ($(BRDC_USE_CAPTURE_BUFFER),y)
NEXUS_DISPLAY_DEFINES += BRDC_USE_CAPTURE_BUFFER=1
endif

# turn on BVN error debug messages
ifeq ($(BVDC_SUPPORT_BVN_DEBUG),y)
NEXUS_DISPLAY_DEFINES += BVDC_SUPPORT_BVN_DEBUG=1
endif

ifeq ($(NEXUS_CRC_CAPTURE),y)
NEXUS_DISPLAY_DEFINES += NEXUS_CRC_CAPTURE=1
endif

#######################
# Module Dependencies #
#######################
NEXUS_DISPLAY_DEPENDENCIES := SURFACE VIDEO_DECODER HDMI_OUTPUT RFM HDMI_INPUT HDMI_DVO

# thunk layer
NEXUS_DISPLAY_PRIVATE_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)
NEXUS_DISPLAY_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_display_thunks.c

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE

NEXUS_DISPLAY_MAGNUM_MODULES =  \
    $(MAGNUM)/syslib/vbilib/bvbilib.inc \
    $(MAGNUM)/portinginterface/vbi/bvbi.inc \
    $(MAGNUM)/portinginterface/vdc/bvdc.inc \
    $(MAGNUM)/commonutils/udp_v1243/budp.inc \
    $(MAGNUM)/commonutils/rdc/brdc.inc

else # HUMAX_PLATFORM_BASE

NEXUS_DISPLAY_MAGNUM_MODULES =  \
    $(MAGNUM)/syslib/vbilib/bvbilib.inc \
    $(MAGNUM)/portinginterface/vbi/bvbi.inc \
    $(MAGNUM)/portinginterface/vdc/bvdc.inc \
    $(MAGNUM)/commonutils/udp/budp.inc \
    $(MAGNUM)/commonutils/rdc/brdc.inc

endif # HUMAX_PLATFORM_BASE

ifeq ($(filter $(BCHP_CHIP),7425 7435 7445), $(BCHP_CHIP))
VIDEO_ENCODER_SUPPORT ?= y
endif
ifeq ($(VIDEO_ENCODER_SUPPORT),y)
ifneq ($(BDSP_ENCODER_SUPPORT),y)
NEXUS_DISPLAY_MAGNUM_MODULES += $(MAGNUM)/syslib/xudlib/bxudlib.inc
endif
endif

ifneq ($(NEXUS_DISPLAY_EXTENSION_INC),)
include $(NEXUS_DISPLAY_EXTENSION_INC)
endif

