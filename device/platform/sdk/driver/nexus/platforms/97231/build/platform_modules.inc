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
# $brcm_Revision: 42 $
# $brcm_Date: 10/3/12 1:55p $
#
# Module Description:
#   This file defines a list of nexus modules that will be included in the build.
#
# Revision History:
#
# $brcm_Log: /nexus/platforms/97231/build/platform_modules.inc $
# 
# 42   10/3/12 1:55p randyjew
# SW7231-317: Add wmdrmpd modules
# 
# 41   9/13/12 4:40p erickson
# SW7435-354: rearrange simple_decoder.inc so it automatically cleans up
#  first
# 
# 40   9/6/12 3:54p erickson
# SW7435-235: move frontend.inc to nexus/modules/frontend/build
# 
# 39   8/21/12 5:05p vishk
# SW7231-959: Live channel could not be tuned in successfully due to
#  'Unable to open onboard 3109 tuner/demodulator'
# 
# 38   7/31/12 9:31a jgarrett
# SW7435-266: Removing BCHP_CHIP from audio.inc
# 
# 37   7/24/12 10:07a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 36   7/20/12 12:27p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 35   7/17/12 1:19p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 34   7/17/12 7:58a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 33   7/16/12 4:51p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 32   7/16/12 3:54p vishk
# SW7231-895: Add support for 97231B2 EUSFF_V20 board.
# 
# 31   6/25/12 5:05p gmohile
# SW7231-873 : Add mp4 mux support
# 
# 30   5/31/12 5:06p vishk
# SW7231-828: Merge to mainline
# 
# AlphaBoxee/1   5/31/12 4:09p vishk
# SW7231-828: Please provide ATSC(MX603+BCM3517) tuner driver.
# 
# 29   4/3/12 11:25a erickson
# SW7425-2690: default V3D_SUPPORT=y if source exists
# 
# 28   2/23/12 4:38p katrep
# SW7231-317:add hdmi cec
# 
# 27   2/20/12 4:19p katrep
# SW7231-317:add ability to disabled 3d graphics for 7229
# 
# 26   1/16/12 6:06p gmohile
# SW7425-1473 : Merge raaga encoder support
# 
# 25   1/6/12 5:00p vishk
# SW7231-76: Add support for FBTSFF 97231 + 3128 board.
# 
# 24   11/7/11 2:53p vishk
# SW3461-78: Add support for 3128 C0 on DCSFBTSFF board(7231B0+3128C0)
# 
# 23   10/31/11 10:23a katrep
# SW7231-317:add nexus multiprocess
# 
# 22   10/21/11 10:08a katrep
# SW7231-400:add compile time option NEXUS_FRONTEND_3461_VER to specify
#  3461 chip version
# 
# 21   10/20/11 2:51p katrep
# SW7231-317:enabled security
# 
# 20   10/12/11 3:31p katrep
# SW7231-317:enabled picture decoder
# 
# 19   10/4/11 4:09p katrep
# SW7231-317:disable still image decoder
# 
# 18   9/13/11 4:48p katrep
# SW7231-317:add still image decoder for 7231 B0
# 
# 17   8/31/11 11:16a katrep
# SW7231-317:add support for 7231 B0
# 
# 16   8/9/11 6:35p katrep
# SW7231-334:integrated svb-ci code from Ashwin
# 
# 15   6/8/11 9:54a katrep
# SW7231-32:add support 3461 card probing
# 
# 14   5/20/11 4:30p erickson
# SW7420-1819: revise OS test for nfe
# 
# 13   5/17/11 9:52a katrep
# SW7231-4:turn keypad input on
# 
# 12   5/16/11 2:04p katrep
# SW7231-4: removed keypad input
# 
# 11   3/17/11 1:07p vishk
# SW7231-32: Implement the Nexus code to support the 3461 frontend chip
# 
# 10   3/16/11 1:30p vishk
# SW7231-76: Add support for FBTSFF 97231 + 3128 board.
# 
# 9   3/15/11 11:45a katrep
# SWVC4-118:turn on graphics3d by default, V3D_SUPPORT is dprecated
# 
# 8   2/25/11 1:54p vishk
# SW7231-30: Add support for 3109,3406 tuners on 97231CSFF board
# 
# 7   2/14/11 12:14p hauxwell
# SWVC4-118: nexus v3d header files to mainline
# 
# 6   2/11/11 10:38a katrep
# SW7231-4:add simple_decoder to the build
# 
# 5   2/9/11 3:21p katrep
# SW7231-28:updated for 3d graphics
# 
# 4   2/7/11 4:04p katrep
# SW7231-34:enable security
# 
# 3   12/29/10 9:54p katrep
# SW7231-28:add V3D support
# 
# 2   12/2/10 10:08a katrep
# SW7231-4:intial version
# 
# 2   9/29/10 2:13p erickson
# SW7420-1123: consolidate 3D support using GL_SUPPORT variable
#
# 1   9/29/10 10:53a erickson
# SW7420-1123: refactor platform_modules.inc so that it can fork per
#  platform
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


# Include Modules
ifeq ($(NEXUS_PLATFORM),97231)
    ifeq ($(NEXUS_PLATFORM_7231_FBTSFF),y)
        NEXUS_FRONTEND_3128 := y
        ifeq ($(NEXUS_FRONTEND_312x_VER),)
            NEXUS_FRONTEND_312x_VER := B0
        endif
    else
        ifeq ($(NEXUS_PLATFORM_7231_CSFF),y)
            NEXUS_FRONTEND_3109 := y
            NEXUS_FRONTEND_31xx_VER := B0
        else         
            ifeq ($(NEXUS_PLATFORM_7231_3520),y)
            	NEXUS_FRONTEND_3520 := y
            else 
                #for NEXUS_PLATFORM_7231_EUSFF,  NEXUS_PLATFORM_7231_EUSFF_V20 and CVBS BOARDS
                 NEXUS_FRONTEND_3461 := y
                 ifeq ($(NEXUS_FRONTEND_3461_VER),)
                 NEXUS_FRONTEND_3461_VER := B0
                 endif 
	    endif
        endif
     endif
endif

ifeq ($(NEXUS_FRONTEND_3461),y)
NEXUS_HAS_FRONTEND := y
NEXUS_FRONTEND_3461 :=y
endif

# allow users to use a customized platform.inc for their board
# this must be placed before all other module .inc's for IMG loading support
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
include $(NEXUS_TOP)/modules/keypad/keypad.inc
include $(NEXUS_TOP)/modules/frontend/build/frontend.inc
include $(NEXUS_TOP)/modules/spi/spi.inc
include $(NEXUS_TOP)/modules/security/security.inc
ifneq ($(BCHP_VER),A0)
include $(NEXUS_TOP)/modules/picture_decoder/picture_decoder.inc
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

ifneq ($(filter $(B_REFSW_OS),linuxkernel bare),$(B_REFSW_OS))
NEXUS_USERMODE=y
endif

ifeq ($(VIDEO_ENCODER_SUPPORT),y)
include $(NEXUS_TOP)/modules/video_encoder/video_encoder.inc
include $(NEXUS_TOP)/modules/stream_mux/stream_mux.inc
ifeq ($(NEXUS_USERMODE),y)
    include $(NEXUS_TOP)/modules/file_mux/file_mux.inc
endif
endif

include $(NEXUS_TOP)/base/base.inc



ifneq ($(PVR_SUPPORT),n)
ifeq ($(NEXUS_USERMODE),y)
    # Don't compile modules that do file I/O into the kernel
    include $(NEXUS_TOP)/modules/file/file.inc
    include $(NEXUS_TOP)/modules/playback/playback.inc
    include $(NEXUS_TOP)/modules/record/record.inc
endif
endif

ifeq ($(V3D_SUPPORT),)
V3D_SUPPORT = $(shell test -e ${NEXUS_TOP}/../rockford/middleware/v3d/khronos.inc && echo y)
endif
ifeq ($(V3D_SUPPORT),y)
include $(NEXUS_TOP)/modules/graphics3d/graphics3d.inc
endif

ifeq ($(BCEC_SUPPORT),y)
include $(NEXUS_TOP)/modules/cec/cec.inc
endif

ifeq ($(NEXUS_MPOD_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_HAS_MPOD
endif

ifeq ($(NEXUS_DVB_CI_SUPPORT),y)
include $(NEXUS_TOP)/modules/dvb_ci/dvb_ci.inc
endif
OTFPVR_SUPPORT:=y

ifeq ($(MSDRM_PRDY_SUPPORT),y)
include $(NEXUS_TOP)/modules/wmdrmpd_core/wmdrmpd_core.inc
include $(NEXUS_TOP)/modules/wmdrmpd_io/wmdrmpd_io.inc
ifeq ($(NEXUS_USERMODE),y)
include $(NEXUS_TOP)/modules/wmdrmpd/wmdrmpd.inc
endif
endif

endif #NEXUS_BASE_ONLY
