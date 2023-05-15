############################################################
#     (c)2003-2012 Broadcom Corporation
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
# $brcm_Workfile: platform_modules.inc $
# $brcm_Revision: 52 $
# $brcm_Date: 9/17/12 12:00p $
#
# Module Description:
#   This file defines a list of nexus modules that will be included in the build.
#
# Revision History:
#
# $brcm_Log: /nexus/platforms/97344/build/platform_modules.inc $
# 
# 52   9/17/12 12:00p erickson
# SW7435-354: restore VIDEO_ENCODER_SUPPORT
# 
# 51   9/13/12 4:40p erickson
# SW7435-354: rearrange simple_decoder.inc so it automatically cleans up
#  first
# 
# 50   9/12/12 2:40p jrubio
# SW7346-1004: add playready
# 
# 49   9/6/12 3:54p erickson
# SW7435-235: move frontend.inc to nexus/modules/frontend/build
# 
# 48   7/31/12 9:03a jrubio
# SW7435-266: remove BCHP_CHIP from audio line
# 
# 47   7/24/12 2:24p jrubio
# SW7346-848: merge in Vish Changes into mainline
# 
# 46   7/24/12 10:07a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 45   7/20/12 12:27p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 44   7/20/12 9:26a jrubio
# SW7346-702: V3D_SUPPORT should be on if 3D directory exists, PM failing
# 
# 43   7/17/12 1:19p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 42   7/17/12 7:58a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 41   7/16/12 4:51p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 40   6/14/12 4:27p jrubio
# SW7346-828: change SFF_V20 to SFF_V10
# 
# 39   2/6/12 9:09p gmohile
# SW7425-1473 : Merge 7346 raaga video encoder support
# 
# 38   1/12/12 3:30p randyjew
# SW7425-1140: Add CEC to 7344/7346
# 
# 37   12/16/11 5:16p vishk
# SW3128-92: Add FW auto detection support. Add support for 7418 platform
# 
# 36   12/13/11 5:14p randyjew
# SW7344-140: 3123 C0 in 7418SFF_H B0
# 
# 35   12/2/11 2:23p jrubio
# SW7346-582: add SFF V20 support
# 
# 34   11/9/11 5:56p randyjew
# SW7344-140: Modify 3123 support for 7418SFF_H
# 
# 33   11/3/11 4:44p randyjew
# SW7344-146: Add back hsm b0 support for 7344
# 
# 32   10/31/11 1:51p randyjew
# SW7344-204: Add Still Image decode support for 7344/7346 B0
# 
# 31   10/31/11 11:54a randyjew
# SW7344-149: Add Multiprocess includes
# 
# 30   10/26/11 11:01a jtna
# SW7420-887: wrap temp_monitor.inc around NEXUS_TEMP_MONITOR_SUPPORT
#  flag
# 
# 29   10/25/11 5:13p randyjew
# SW7344-146: Disable Security for 7344, until B0 hsm support is added.
# 
# 28   10/24/11 11:57a jrubio
# SW7346-470: enable security
# 
# 27   10/6/11 4:53p randyjew
# SW7344-198: Add NEXUS_PLATFORM_7344SV for 4506 support
# 
# 26   9/26/11 6:34p randyjew
# SW7344-140:Adding 3123 support for 7418SFF_H
# 
# 25   9/20/11 10:31a jrubio
# SW7346-470: disable security for B0
# 
# 24   9/2/11 5:48p vishk
# SW7346-301: Nexus Frontend 7346 needs to support both internal and
#  external (BCM3445) LNAs
# 
# 23   7/25/11 3:28p jrubio
# SW7344-117: enable 7346 g3 compilation
# 
# 22   7/22/11 12:07p jrubio
# SW7346-301: fix NEXUS_PLATFORM_VER=HR44
# 
# 21   6/30/11 2:53p jrubio
# SW7346-165: fix makefile issue
# 
# 20   6/27/11 10:40a jrubio
# SW7346-165: add default PLATFORM VERSION
# 
# 19   6/23/11 5:41p vishk
# SW7346-165: NEXUS Frontend changes for multi-tuner board are not
#  platform independent
# 
# 18   5/20/11 4:30p erickson
# SW7420-1819: revise OS test for nfe
# 
# 17   4/26/11 2:46p jrubio
# SW7346-158: add 7346 SPI frontpanel
# 
# 16   4/14/11 3:31p randyjew
# SW7346-137: re enable 7344 temp monitor
# 
# 15   4/14/11 12:56p randyjew
# SW7346-137: disable temp monitor support for 7344 until supported in
#  the tmn PI.
# 
# 14   4/14/11 9:00a jrubio
# SW7346-137: add temp monitor
# 
# 13   4/1/11 1:27p randyjew
# SW7344-9: Add simple decoder nexus module
# 
# 12   3/7/11 2:03p randyjew
# SW7344-9: Add 7418 support
# 
# 11   3/2/11 4:29p randyjew
# SW7418-1:Add 97418 support.
# 
# 10   2/25/11 11:52a jrubio
# SW7346-17: add HMC,SFF,SV platforms flags
# 
# 9   2/16/11 3:33p randyjew
# SW7344-25:Add Spi frontpanel support for 7344
# 
# 8   2/8/11 1:04p hauxwell
# SWVC4-118: refactor directories
# 
# 7   2/4/11 8:42a jrubio
# SW7344-9: add smartcard support
# 
# 6   2/2/11 4:13p jrubio
# SW7346-19: enable security hsm and kernelmode 3d graphics
# 
# 5   12/13/10 10:36p jrubio
# SW7344-9: update included modules
# 
# 4   12/8/10 2:48p jrubio
# SW7344-9: add uhf support back in
# 
# 3   12/8/10 2:33p jrubio
# SW7344-9: add frontend
# 
# 2   12/7/10 11:39a jrubio
# SW7344-9: include more modules
# 
# 1   11/29/10 11:08a jrubio
# SW7344-9: fork platform_modules.inc for 7344/7346
# 
# 8   11/12/10 10:39a rgreen
# SW7422-10: Merge changes
# 
# 1   10/11/10 2:54p nickh
# SW7422-10: Fork off platform_modules.inc file
# 
############################################################


ifeq ($(NEXUS_BASE_ONLY),y)
$(warning NEXUS_BASE_ONLY is defined)
# Minimum platform_modules.inc to run with just base and core
include $(NEXUS_TOP)/platforms/common/build/platform.inc
include $(NEXUS_TOP)/modules/core/core.inc
include $(NEXUS_TOP)/base/base.inc
NEXUS_PLATFORM_DEFINES+=NEXUS_BASE_ONLY

else
# Define Module Options


ifeq ($(filter $(NEXUS_PLATFORM), 97420 97410), $(NEXUS_PLATFORM))
NEXUS_MPOD_SUPPORT :=n
endif


#ifneq ($(filter $(NEXUS_PLATFORM), 97418 97344),)
#maybe needed for SWM
#NEXUS_FRONTEND_EXTENSION_INC := $(NEXUS_TOP)/extensions/frontend/$(NEXUS_PLATFORM)/frontend_ext.inc
#endif

# Include Modules

# allow users to use a customized platform.inc for their board
# this must be placed before all other module .inc files for IMG loading support
ifeq ($(PLATFORM_INC),)
PLATFORM_INC=$(NEXUS_TOP)/platforms/common/build/platform.inc
endif
include $(PLATFORM_INC)

#
# After PLATFORM, list all modules in order from least to most dependencies.
# This is required to handle automatic closing on abnormal application termination.

include $(NEXUS_TOP)/modules/core/core.inc
include $(NEXUS_TOP)/modules/pwm/pwm.inc
include $(NEXUS_TOP)/modules/i2c/i2c.inc
include $(NEXUS_TOP)/modules/gpio/gpio.inc
include $(NEXUS_TOP)/modules/led/led.inc

include $(NEXUS_TOP)/modules/ir_input/ir_input.inc
include $(NEXUS_TOP)/modules/ir_blaster/ir_blaster.inc
include $(NEXUS_TOP)/modules/input_capture/input_capture.inc
include $(NEXUS_TOP)/modules/uhf_input/uhf_input.inc
ifneq ($(filter $(NEXUS_PLATFORM), 97418 97344 97346),$(NEXUS_PLATFORM))
	include $(NEXUS_TOP)/modules/rfm/rfm.inc
endif
include $(NEXUS_TOP)/modules/keypad/keypad.inc
include $(NEXUS_TOP)/modules/spi/spi.inc


include $(NEXUS_TOP)/modules/security/security.inc

ifeq ($(filter $(NEXUS_PLATFORM), 97344 97346),$(NEXUS_PLATFORM))
ifneq ($(BCHP_VER),A0)
include $(NEXUS_TOP)/modules/picture_decoder/picture_decoder.inc
endif
endif
ifeq ($(filter $(NEXUS_PLATFORM), 97422 97425),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/picture_decoder/picture_decoder.inc
include $(NEXUS_TOP)/modules/hdmi_input/hdmi_input.inc
endif

ifeq ($(BCEC_SUPPORT),y)
    include $(NEXUS_TOP)/modules/cec/cec.inc
endif

include $(NEXUS_TOP)/modules/dma/dma.inc
include $(NEXUS_TOP)/modules/transport/transport.inc
include $(NEXUS_TOP)/modules/video_decoder/video_decoder.inc
include $(NEXUS_TOP)/modules/audio/audio.inc
include $(NEXUS_TOP)/modules/surface/surface.inc
include $(NEXUS_TOP)/modules/graphics2d/graphics2d.inc

include $(NEXUS_TOP)/modules/display/display.inc

include $(NEXUS_TOP)/modules/astm/astm.inc
include $(NEXUS_TOP)/modules/sync_channel/sync_channel.inc
include $(NEXUS_TOP)/modules/hdmi_output/hdmi_output.inc

include $(NEXUS_TOP)/modules/smartcard/smartcard.inc

ifeq ($(V3D_SUPPORT),)
V3D_SUPPORT = $(shell test -e ${NEXUS_TOP}/../rockford/middleware/v3d/khronos.inc && echo y)
endif

ifeq ($(V3D_SUPPORT),y)
include $(NEXUS_TOP)/modules/graphics3d/graphics3d.inc
endif

ifeq ($(NEXUS_DVB_CI_SUPPORT),y)
include $(NEXUS_TOP)/modules/dvb_ci/dvb_ci.inc
endif

include $(NEXUS_TOP)/modules/surface_compositor/surface_compositor.inc
include $(NEXUS_TOP)/modules/input_router/input_router.inc
include $(NEXUS_TOP)/modules/simple_decoder/simple_decoder.inc

ifeq ($(filter $(B_REFSW_OS),linuxuser linuxkernel bare),)
# Include uart for non-linux systems only.
include $(NEXUS_TOP)/modules/uart/uart.inc
endif

ifeq ($(NEXUS_TOUCHPAD_SUPPORT),y)
include $(NEXUS_TOP)/modules/touchpad/touchpad.inc
endif

ifeq ($(NEXUS_TEMP_MONITOR_SUPPORT),y)
include $(NEXUS_TOP)/modules/temp_monitor/temp_monitor.inc
endif

ifeq ($(MSDRM_PRDY_SUPPORT),y)
ifeq ($(HUMAX_PLATFORM_BASE),y)
#include $(NEXUS_TOP)/modules/wmdrmpd_core/wmdrmpd_core.inc
#include $(NEXUS_TOP)/modules/wmdrmpd_io/wmdrmpd_io.inc
else
include $(NEXUS_TOP)/modules/wmdrmpd_core/wmdrmpd_core.inc
include $(NEXUS_TOP)/modules/wmdrmpd_io/wmdrmpd_io.inc
endif
endif

ifeq ($(VIDEO_ENCODER_SUPPORT),y)
include $(NEXUS_TOP)/modules/video_encoder/video_encoder.inc
include $(NEXUS_TOP)/modules/stream_mux/stream_mux.inc
ifeq ($(NEXUS_USERMODE),y)
    include $(NEXUS_TOP)/modules/file_mux/file_mux.inc
endif
endif

include $(NEXUS_TOP)/base/base.inc

ifneq ($(filter $(B_REFSW_OS),linuxkernel bare),$(B_REFSW_OS))
NEXUS_USERMODE=y
endif

ifeq ($(SFF_V10),y)
NEXUS_PLATFORM_DEFINES += SFF_V10
endif

ifeq ($(NEXUS_USERMODE),y)
    include $(NEXUS_TOP)/modules/file/file.inc
ifneq ($(PVR_SUPPORT),n)
    # Don't compile modules that do file I/O into the kernel
        include $(NEXUS_TOP)/modules/playback/playback.inc
    endif
    include $(NEXUS_TOP)/modules/record/record.inc
endif

ifeq ($(MSDRM_PRDY_SUPPORT),y)
ifeq ($(NEXUS_USERMODE),y)
ifeq ($(HUMAX_PLATFORM_BASE),y)
#include $(NEXUS_TOP)/modules/wmdrmpd/wmdrmpd.inc
else
include $(NEXUS_TOP)/modules/wmdrmpd/wmdrmpd.inc
endif
endif
endif


ifeq ($(NEXUS_MPOD_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_HAS_MPOD
endif

#
# 7346 Platforms
#
#
ifeq ($(NEXUS_PLATFORM),97346)
ifeq ($(NEXUS_PLATFORM_VER),HMC)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_97346_HMC
endif
ifeq ($(NEXUS_PLATFORM_VER),SFF)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_97346_SFF
endif
ifeq ($(NEXUS_PLATFORM_VER),I2SFF)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_97346_I2SFF
NEXUS_FRONTEND_3472 := y
endif
ifeq ($(NEXUS_PLATFORM_VER),HR44)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_97346_HR44
NEXUS_FRONTEND_4506 := y
NEXUS_FRONTEND_4506_VER := B0
NEXUS_FRONTEND_3440 := y
endif
ifeq ($(NEXUS_PLATFORM_VER),SV)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_97346_SV
NEXUS_FRONTEND_4506 := y
NEXUS_FRONTEND_4506_VER := B0
NEXUS_FRONTEND_3440 := y
endif
endif

#
# 7344/7418 Platforms
#
#
ifeq ($(NEXUS_PLATFORM_7344SV),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_97344SV
NEXUS_FRONTEND_4506 := y
NEXUS_FRONTEND_4506_VER := B0
NEXUS_FRONTEND_3440 := y
endif

ifeq ($(NEXUS_PLATFORM_7418SFF_H),y)
    NEXUS_FRONTEND_3128 := y
	ifeq ($(NEXUS_FRONTEND_312x_VER),)
        NEXUS_FRONTEND_312x_VER := C0
    endif
endif
include $(NEXUS_TOP)/modules/frontend/build/frontend.inc

#
# SPI Front Panel
# Defaults on for Specified Platforms.
# Set NEXUS_SPI_FRONTPANEL=n to turn off.
#
ifneq ($(NEXUS_SPI_FRONTPANEL),n)
NEXUS_CFLAGS += -DNEXUS_HAS_SPI_FRONTPANEL=1
endif

endif #NEXUS_BASE_ONLY
