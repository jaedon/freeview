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
# $brcm_Revision: 16 $
# $brcm_Date: 9/13/12 4:40p $
#
# Module Description:
#   This file defines a list of nexus modules that will be included in the build.
#
# Revision History:
#
# $brcm_Log: /nexus/platforms/97552/build/platform_modules.inc $
# 
# 16   9/13/12 4:40p erickson
# SW7435-354: rearrange simple_decoder.inc so it automatically cleans up
#  first
# 
# 15   9/6/12 3:54p erickson
# SW7435-235: move frontend.inc to nexus/modules/frontend/build
# 
# 14   7/31/12 9:32a jgarrett
# SW7435-266: Removing BCHP_CHIP from audio.inc
# 
# 13   7/24/12 10:07a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 12   7/20/12 12:27p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 11   7/17/12 7:58a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 10   7/16/12 4:52p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 9   1/17/12 8:54a xhuang
# SW7552-193: Add HDMI CEC support for 7552
# 
# 8   12/28/11 4:08p vishk
# SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board.
# 
# 7   12/20/11 2:45p vishk
# SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board
# 
# 6   11/2/11 3:38p xhuang
# SW7552-135: add multi process support
# 
# 5   10/11/11 5:50p xhuang
# SW7552-135: add nexus modules required by applibs
# 
# 4   7/7/11 3:30p xhuang
# SW7552-39: Enable smart card support
# 
# 3   7/7/11 11:00a xhuang
# SW7552-41: Enable security for 7552
# 
# 2   5/20/11 4:30p erickson
# SW7420-1819: revise OS test for nfe
# 
# 1   2/14/11 4:13p xhuang
# SW7552-25: fork platform_modules for 7552
# 
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

ifneq ($(filter $(NEXUS_PLATFORM), 97344 97346 97358),)
NEXUS_FRONTEND_EXTENSION_INC := $(NEXUS_TOP)/extensions/frontend/$(NEXUS_PLATFORM)/frontend_ext.inc
NEXUS_FRONTEND_3440 := y
endif

# Include Modules

ifeq ($(filter $(NEXUS_PLATFORM), 97552), $(NEXUS_PLATFORM))
    ifeq ($(NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD),y)
        NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD := y
        NEXUS_FRONTEND_3128 := y
    endif
ifeq ($(NEXUS_FRONTEND_312x_VER),)
    NEXUS_FRONTEND_312x_VER := C0
endif
endif

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
include $(NEXUS_TOP)/modules/rfm/rfm.inc
include $(NEXUS_TOP)/modules/keypad/keypad.inc
include $(NEXUS_TOP)/modules/frontend/build/frontend.inc
include $(NEXUS_TOP)/modules/spi/spi.inc
include $(NEXUS_TOP)/modules/security/security.inc
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

include $(NEXUS_TOP)/base/base.inc

ifneq ($(filter $(B_REFSW_OS),linuxkernel bare),$(B_REFSW_OS))
NEXUS_USERMODE=y
endif

ifneq ($(PVR_SUPPORT),n)
ifeq ($(NEXUS_USERMODE),y)
    # Do not compile modules that do file I/O into the kernel
    include $(NEXUS_TOP)/modules/file/file.inc
    include $(NEXUS_TOP)/modules/playback/playback.inc
    include $(NEXUS_TOP)/modules/record/record.inc
endif
endif

ifeq ($(NEXUS_MPOD_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_HAS_MPOD
endif

ifeq ($(BCEC_SUPPORT),y)
    include $(NEXUS_TOP)/modules/cec/cec.inc
endif
endif #NEXUS_BASE_ONLY
