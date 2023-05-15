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
# $brcm_Workfile: Makefile $
# $brcm_Revision: 20 $
# $brcm_Date: 7/23/12 10:20a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/examples/video/Makefile $
# 
# 20   7/23/12 10:20a erickson
# SW7425-3545: add display_vbi
# 
# 19   5/9/12 4:37p erickson
# SW7425-1747: remove old code
# 
# 18   1/16/12 11:15a erickson
# SW7346-629: add decode_crc example
# 
# 17   6/30/11 3:19p erickson
# SW7425-466: add triple_decode example for 7425
# 
# 16   5/13/11 9:39a erickson
# SW7340-265: remove duplicate rule
# 
# 15   4/20/11 9:16a erickson
# SW7405-5224: add prefersynclock
# 
# 14   3/17/11 12:48p erickson
# SW7422-333: always build mvc/svc examples, but have print err on
#  runtime failure
#
# 13   3/11/11 4:13p erickson
# SW7346-103: add support for CMP CRC capture
#
# 12   12/15/10 11:10a erickson
# SW7420-1177: add check for NEXUS_DVO_DVI_LOOPBACK_SUPPORT
#
# 11   11/29/10 3:47p erickson
# SW7420-1177: add hddvo_hdvi_loopback.c
#
# SW7405-4230/1   4/30/10 11:57a yuxiaz
# SW7405-4230: Added example code to do vec swap at display create.
#
# 9   3/22/10 6:15p ttrammel
# SW7405-3833: Modify for pre-built binary build.
#
# 8   10/16/09 4:41p erickson
# SW7400-2511: switch to NEXUS_PLATFORM
#
# 7   7/15/09 1:19p erickson
# PR56821: add analog_pip to DTV builds
#
# 6   4/27/09 10:04a erickson
# PR54512: add capture_buffer example
#
# 5   3/26/09 4:55p erickson
# PR48963: add hdsd_simul_pip
#
# 4   2/26/09 9:28a erickson
# PR35457: added pal_panel example
#
# 3   2/23/09 11:45a erickson
# PR52270: added example to test VDEC->VEC bypass for composite output
#
# 2   2/4/09 4:06p erickson
# PR51841: move app to DTV
#
# 1   2/4/09 3:51p erickson
# PR51841: refactor
#
# 1   2/4/09 2:42p erickson
# PR51841: reorganize examples
#
############################################################
# Nexus examples

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../.. && cd)
else
NEXUS_TOP := $(shell cd ../../; pwd)
endif

# include cross-compiler definitions
ifeq ($(NEXUS_PREBUILT_BINARY),y)
# do "make api" and "make nexus_headers" first, then "make NEXUS_PREBUILT_BINARY=y"
include $(NEXUS_TOP)/bin/include/platform_app.inc
Q_ ?= @
else
NEXUS_PLATFORM ?= $(PLATFORM)
ifndef NEXUS_PLATFORM
$(error NEXUS_PLATFORM is not defined)
endif

include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc
endif

THEAPPS = \
    capture_buffer \
    ccir656_video \
    decode_crc \
    decode_video \
    decode_video_dvb \
    decode_video_via_graphics \
    display_crc \
    display_vbi \
    dual_decode \
    pig \
    pip \
    playback_mvc \
    playback_svc \
    prefersynclock \
    source_pending \
    still_decoder \
    triple_decode \
    userdata \
    window_clipping

THEAPPS += \
    hdsd_simul \
    hdsd_simul_pip \
    hdsd_simul_vecswap \
    mosaic_decode 
    
ifdef NEXUS_DVO_DVI_LOOPBACK_SUPPORT
THEAPPS += hddvo_dvi_loopback
endif

include ../Makefile.inc
