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
# $brcm_Revision: 29 $
# $brcm_Date: 10/23/12 3:07p $
#
# Module Description:
#   This file defines a list of nexus modules that will be included in the build.
#
# Revision History:
#
# $brcm_Log: /nexus/platforms/97429/build/platform_modules.inc $
# 
# 29   10/23/12 3:07p jessem
# SW7429-335: Added evaluation of VIDEO_ENCODER_SUPPORT.
# 
# 28   9/13/12 4:40p erickson
# SW7435-354: rearrange simple_decoder.inc so it automatically cleans up
#  first
# 
# 27   9/6/12 3:54p erickson
# SW7435-235: move frontend.inc to nexus/modules/frontend/build
# 
# 26   8/23/12 1:24p katrep
# SW7429-254:add NEXUS_USERMODE flag
# 
# 25   7/31/12 9:32a jgarrett
# SW7435-266: Removing BCHP_CHIP from audio.inc
# 
# 24   7/30/12 2:32p katrep
# SW7429-1:add playready
# 
# 23   7/30/12 9:24a katrep
# SW7429-1:enabled security
# 
# 22   7/24/12 10:08a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 21   7/20/12 12:27p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 20   7/17/12 1:19p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 19   7/17/12 7:58a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 18   7/16/12 4:52p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 17   7/3/12 3:18p katrep
# SW7429-1:disable nexus securtiy compilation for 7429 b0
# 
# 16   7/2/12 10:28a katrep
# SW7429-183:Cleaned up platform define and build issue with no frontend
# 
# 15   6/26/12 3:57p vishk
# SW7429-183: Add support for 7241DCSFBTSFF board
# 
# 14   5/11/12 9:09a katrep
# SW7429-1:add encoder
# 
# 13   5/7/12 6:14p vishk
# SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
# 
# 12   4/25/12 10:05a katrep
# SW7429-1:add cec & image image decoder
# 
# 11   2/6/12 4:54p katrep
# SW7429-1:enabled security
# 
# 10   1/17/12 5:37p katrep
# SW7429-44:disabled frontend for all the chips other than 7429
# 
# 9   1/17/12 4:33p katrep
# SW7429-44:compile time enable option for 3255
# 
# 8   1/16/12 12:42p ayxiang
# SW7429-44: add 7429 frontend support
# 
# 7   11/2/11 4:48p katrep
# SW7429-1:build error
# 
# 6   11/2/11 11:35a katrep
# SW7429-1:add nexus multiprocess
# 
# 5   10/26/11 11:06a katrep
# SW7429-1:add rfm
# 
# 4   10/25/11 5:28p katrep
# SW7429-1:updated for 7429 bringup
# 
# 3   10/3/11 11:57a katrep
# SW7429-1:updated for 7429 bringup
# 
# 2   9/22/11 5:14p katrep
# SW7429-1:add support for 7429 & bondouts
# 
# 1   8/9/11 8:01p katrep
# SW7429-1:initial checkin
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


ifeq ($(NEXUS_PLATFORM),97241)
    ifeq ($(NEXUS_PLATFORM_7241_C2SFF),y)
            NEXUS_FRONTEND_3462 := y
            ifeq ($(NEXUS_FRONTEND_3462_VER),)
                NEXUS_FRONTEND_3462_VER := A0
            endif
    else        
        ifeq ($(NEXUS_PLATFORM_7241_DCSFBTSFF),y)
                NEXUS_FRONTEND_3128 := y
                ifeq ($(NEXUS_FRONTEND_312x_VER),)
                    NEXUS_FRONTEND_312x_VER := C0
                endif
        else   
            ifeq ($(NEXUS_USE_FRONTEND_DAUGHTER_CARD),y)
                NEXUS_USE_FRONTEND_DAUGHTER_CARD := y
                NEXUS_FRONTEND_3128 := y
            endif
        endif
    endif
endif

ifeq ($(NEXUS_FRONTEND_3255),y)
NEXUS_FRONTEND_3255 :=y
endif

ifeq ($(NEXUS_FRONTEND_3461),y)
NEXUS_HAS_FRONTEND := y
NEXUS_FRONTEND_3461 :=y
endif

ifeq ($(USE_SPI_FRONTEND),y)
USE_SPI_FRONTEND := y
endif

# Include Modules

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

#ifeq ($(filter $(NEXUS_PLATFORM), 97429 97241), $(NEXUS_PLATFORM))
#include $(NEXUS_TOP)/modules/frontend/build/frontend.inc
#endif

ifeq ($(findstring y,$(NEXUS_FRONTEND_3461)),y)
include $(NEXUS_TOP)/modules/frontend/build/frontend.inc
endif

ifeq ($(findstring y,$(NEXUS_FRONTEND_3255) $(NEXUS_FRONTEND_3128) $(NEXUS_FRONTEND_3462)),y)
include $(NEXUS_TOP)/modules/frontend/build/frontend.inc
endif
 
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
ifneq ($(NEXUS_PLATFORM),97241)
include $(NEXUS_TOP)/modules/rfm/rfm.inc
include $(NEXUS_TOP)/modules/hdmi_input/hdmi_input.inc
endif
include $(NEXUS_TOP)/modules/picture_decoder/picture_decoder.inc
ifeq ($(BCEC_SUPPORT),y)
include $(NEXUS_TOP)/modules/cec/cec.inc
endif

include $(NEXUS_TOP)/modules/smartcard/smartcard.inc

ifeq ($(NEXUS_DVB_CI_SUPPORT),y)
include $(NEXUS_TOP)/modules/dvb_ci/dvb_ci.inc
endif

include $(NEXUS_TOP)/modules/surface_compositor/surface_compositor.inc
include $(NEXUS_TOP)/modules/input_router/input_router.inc
include $(NEXUS_TOP)/modules/simple_decoder/simple_decoder.inc

ifneq ($(filter $(B_REFSW_OS),linuxkernel bare),$(B_REFSW_OS))
NEXUS_USERMODE=y
endif

ifeq ($(HUMAX_PLATFORM_BASE),y)
ifeq ($(MSDRM_PRDY_SUPPORT),y)
#include $(NEXUS_TOP)/modules/wmdrmpd_core/wmdrmpd_core.inc
#include $(NEXUS_TOP)/modules/wmdrmpd_io/wmdrmpd_io.inc
ifeq ($(NEXUS_USERMODE),y)
#include $(NEXUS_TOP)/modules/wmdrmpd/wmdrmpd.inc
endif
endif
else
ifeq ($(MSDRM_PRDY_SUPPORT),y)
include $(NEXUS_TOP)/modules/wmdrmpd_core/wmdrmpd_core.inc
include $(NEXUS_TOP)/modules/wmdrmpd_io/wmdrmpd_io.inc
ifeq ($(NEXUS_USERMODE),y)
include $(NEXUS_TOP)/modules/wmdrmpd/wmdrmpd.inc
endif
endif
endif

ifeq ($(filter $(B_REFSW_OS),linuxuser linuxkernel bare),)
# Include uart for non-linux systems only.
include $(NEXUS_TOP)/modules/uart/uart.inc
endif

ifeq ($(NEXUS_TOUCHPAD_SUPPORT),y)
include $(NEXUS_TOP)/modules/touchpad/touchpad.inc
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

ifeq ($(NEXUS_MPOD_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_HAS_MPOD
endif


OTFPVR_SUPPORT:=y

endif #NEXUS_BASE_ONLY
