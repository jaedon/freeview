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
# $brcm_Revision: 23 $
# $brcm_Date: 10/9/12 12:13a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/examples/audio/Makefile $
# 
# 23   10/9/12 12:13a jgarrett
# SW7346-914: Adding audio ancillary data support
# 
# 22   7/27/12 3:21p jgarrett
# SW7425-3443: Removing DTV-only interfaces
# 
# 21   2/9/12 3:06p jgarrett
# SW7346-672: Adding audio_playback_loop.c
# 
# 20   12/20/11 5:06p jgarrett
# SW7425-1018:Adding ATSC A/85 example
# 
# 19   9/2/11 5:53p jgarrett
# SW7425-724: Fixing application name for btsc_encode
# 
# 18   8/29/11 5:50p jgarrett
# SW7425-724: Adding BTSC encode example
# 
# 17   8/26/11 4:40p jgarrett
# SWDTV-8371: Adding example to convert AC3Plus to AC3
# 
# 16   7/13/11 4:16p jgarrett
# SW7325-854: Adding AVL and 3D Surround examples
# 
# 15   6/21/11 5:03p jgarrett
# SW7425-654: Adding Audio NRT APIs
# 
# 14   4/27/11 6:52p jgarrett
# SW7420-1826: Adding AudioOutputGroup
# 
# 13   2/18/11 6:51p jgarrett
# SW7425-94: Merge to main branch
# 
# NEXUS_AACENC_INTEGRATION/1   2/15/11 7:33p jgarrett
# SW7425-94: Adding initial mux output files for AAC encode
# 
# 12   2/9/11 2:13p vsilyaev
# SW7425-41: Merged updated examples
# 
# SW7425_bringup/1   1/17/11 11:23p hongtaoz
# SW7425-41: add encode_audio example to transcode audio es output;
# 
# 11   12/30/10 12:10p jgarrett
# SW7425-41: Adding mux output example
# 
# 10   8/26/10 4:21p jgarrett
# SW7420-790: Merging MS11 examples to main
# 
# SW7420-790/3   8/16/10 5:07p jgarrett
# SW7420-790: Merge to latest baseline
# 
# SW7420-790/2   6/14/10 5:47p jgarrett
# SW7420-790: Adding single decode MS11 example
# 
# SW7420-790/1   6/11/10 5:00p jgarrett
# SW7420-790: Adding MS11 Support
# 
# 9   6/14/10 1:30p jgarrett
# SW7405-3993: Fixing audyssey example build
# 
# 8   5/13/10 6:52p jgarrett
# SW7405-4212: Adding multistream decode example with DSP mixing
# 
# 7   5/6/10 5:11p jgarrett
# SW7405-3993: Adding audyssey example
# 
# SW7405-4212/1   4/28/10 3:10p jgarrett
# SW7405-4212: Adding multistream decode example
# 
# 6   3/23/10 10:43a ttrammel
# SW7405-3833: Modify for pre-built binary build.
# 
# 6   3/23/10 10:40a ttrammel
# SW7405-3833: Modify for pre-built binary build.
# 
# 6   3/22/10 6:03p ttrammel
# SW7405-3833: Modify for pre-built binary build.
# 
# 5   10/16/09 4:41p erickson
# SW7400-2511: switch to NEXUS_PLATFORM
# 
# 4   9/1/09 12:15p vsilyaev
#   SWDEPRECATED-3830:  Adding example to switch between audio tracks
# 
# 3   4/24/09 3:51p erickson
# PR35457: move play_mp3 to nexus/examples/audio
# 
# 2   4/6/09 5:22p jgarrett
# PR 53920: Adding TruVolume example
# 
# 1   2/4/09 4:01p erickson
# PR51841: add files
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
	ac3_encode \
	audio_capture \
	audio_descriptors \
	audio_mixer \
	audio_playback \
	audio_playback_loop \
	audio_track_change \
	dts_encode \
	i2s_input \
	play_mp3 \
	tru_volume \
	audio_multistream_decode \
	audio_output_group \
	auto_volume_level \
	3d_surround \
        convert_ac3plus_to_ac3 \
        btsc_encode \
        atsc_a85 \
        ms11_ac3plus \
        ms11_dual_ac3plus \
        audio_mux_playback encode_audio \
        audio_transcode_mux_playback \
        audio_nonrealtime_transcode \
        ancillary_data


include ../Makefile.inc
