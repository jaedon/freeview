############################################################
#     (c)2003-2011 Broadcom Corporation
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
# $brcm_Revision: 13 $
# $brcm_Date: 12/10/11 12:13a $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /nexus/examples/encoder/Makefile $
# 
# 13   12/10/11 12:13a hongtaoz
# SW7425-1567, SW7425-1569: added 6xaudio app to the makefile;
# 
# 12   10/17/11 1:06p hongtaoz
# SW7425-1513: removed redundent transcode_mp4 example from nexus;
# 
# 11   10/10/11 5:05p hongtaoz
# SW7425-924: added dual audio-only transcodes example;
# 
# 10   9/30/11 3:44p hongtaoz
# SW7425-1278: add dual transcodes example;
# 
# 9   8/22/11 9:20a hongtaoz
# SW7425-165: moved encode_video_es test to rockford;
# 
# 8   6/7/11 3:53p vsilyaev
# SW7425-654: Added AFAP example
# 
# 7   4/17/11 4:14p hongtaoz
# SW7425-356: move transcode examples to encoder directory;
# 
# 6   4/16/11 8:34p hongtaoz
# SW7425-146, SW7425-165: add new examples for hdmi and qam input;
# 
# 5   4/14/11 11:55a vsilyaev
# SW7425-356: Moved encoder examples
# 
# 4   4/7/11 6:18p vsilyaev
# SW7425-232: Added File Mux example
# 
# 3   3/14/11 9:05p hongtaoz
# SW7425-165: fix compile issue;
# 
# SW7425-165/2   3/14/11 6:45p vanessah
# SW7425-165: fix makefile error and some UI issue
# 
# 2   3/14/11 9:53a hongtaoz
# SW7425-167: remove compile error for prerel label;
# 
# 1   3/12/11 9:15p hongtaoz
# SW7425-165: merge to mainline; add configurable transcode examples;
# 
# SW7425-165/1   3/12/11 5:52p vanessah
# SW7425-165: encode video and audio es
#
#
############################################################
# Nexus examples

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../.. && cd)
else
NEXUS_TOP := $(shell cd ../../; pwd)
endif

Q_ ?= @

# include cross-compiler definitions
ifeq ($(NEXUS_PREBUILT_BINARY),y)
# do "make api" and "make nexus_headers" first, then "make NEXUS_PREBUILT_BINARY=y"
include $(NEXUS_TOP)/bin/include/platform_app.inc
else
NEXUS_PLATFORM ?= $(PLATFORM)
ifndef NEXUS_PLATFORM
$(error NEXUS_PLATFORM is not defined)
endif

include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc
endif

THEAPPS := \
    audio_mux_playback \
    dual_audio_transcode \
    audio_transcode_mux_playback \
    encode_hdmi_audio \
    encode_video \
    encode_hdmi_video \
    encode_audio_es \
    transcode_qam_to_ts \
    transcode_hdmi_to_ts \
    transcode_playback_to_ts \
    transcode_playback_to_ts_dual \
    transcode_playback_to_ts_6xaudio \
    transcode_playback_to_ts_afap 


# This will be defined in platform_app.inc appropriately
ifdef NEXUS_DTV_PLATFORM
# DTV-specific apps
else
# STB-specific apps
endif

CFLAGS += -I$(NEXUS_TOP)/../BSEAV/lib/netaccel/include
CFLAGS += -I$(NEXUS_TOP)/../BSEAV/lib/bcmplayer/include

BFILE_DIR=$(NEXUS_TOP)/../BSEAV/lib/bfile
BCMPLAYER_DIR=$(NEXUS_TOP)/../BSEAV/lib/bcmplayer
CFLAGS += -I$(BFILE_DIR) -I$(BCMPLAYER_DIR)/include -I$(BCMPLAYER_DIR)/utils

include ../Makefile.inc

