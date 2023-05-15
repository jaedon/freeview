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
# $brcm_Revision: 21 $
# $brcm_Date: 8/9/12 12:56p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/examples/dvr/Makefile $
# 
# 21   8/9/12 12:56p erickson
# SW7405-5681: add record_from_playback
# 
# 20   7/27/12 2:32p jgarrett
# SW7425-3281: Merge to main branch
# 
# SW7425-3281/1   7/8/12 4:54p piyushg
# SW7425-3281: Added support for playready licensce acquisition and Nexus
#  fileio
# 
# SW7425-2317/4   5/31/12 1:34p piyushg
# SW7425-2317: remove playback_encrypted_mp4 from the default make.
# 
# SW7425-2317/3   4/11/12 4:11p hugost
# Added piff include path to the cflags.
# 
# SW7425-2317/2   4/6/12 7:41p piyushg
# SW7425-2317: Initial support for MP4
# 
# SW7425-2317/1   2/28/12 10:03p piyushg
# SW7425-2317: Initial checkin for Nexus DRM usrmode and kernelmode
#  support
# 
# 19   2/7/12 12:53p vsilyaev
# SW7425-2324: Added example that shows how to probe codec meta data
#  (profile, samplerate, etc) for data captured by recpump
# 
# 18   8/9/11 11:46a vsilyaev
# SW7420-1849: Added BSEAV/lib/bfile path for applications
# 
# 17   5/25/11 10:31a erickson
# SW7425-466: build mosaic_playback
# 
# 16   4/27/11 10:45a erickson
# SW7405-4249: add playback_pcr_indexing
# 
# 15   4/19/11 2:37p jtna
# SW7125-812: moved playback_customfile target to end of Makefile; it was
#  overriding the default target
# 
# 14   4/14/11 11:55a vsilyaev
# SW7425-356: Moved encoder examples
# 
# 13   3/22/11 1:38p erickson
# SW7125-812: change to NEXUS_BIN_DIR
# 
# 12   3/10/11 12:13p erickson
# SW7125-812: provide example library and app for adding bof/eof trim to
#  nexus file
#
# 11   2/7/11 2:25p erickson
# SW7335-1036: add recpump_with_wrap
#
# 10   1/3/11 11:32a vsilyaev
# SW7425-39, SW7425-40: Merged from SW7425_bringup
#
# 9   12/16/10 12:10p vsilyaev
# SW7425-39, SW7425-40: added encode_video and transcode_playback to
#  example makefile;
#
# SW7425_bringup/3   1/2/11 6:37p hongtaoz
# SW7425-39, SW7425-40: added transcode_hdmi test that outputs TS with
#  PCR/PAT/PMT;
#
# SW7425_bringup/2   1/2/11 5:29p hongtaoz
# SW7425-39, SW7425-40: hardcoded call sequence for 24hz encode rate;
#  added hdmi video encode test;
#
# 9   12/16/10 12:10p vsilyaev
# SW7425-39, SW7425-40: added encode_video and transcode_playback to
#  example makefile;
#
# SW7425_bringup/1   12/15/10 9:27p hongtaoz
# SW7425-39, SW7425-40: added encode_video and transcode_playback to
#  example makefile;
#
# 8   3/22/10 6:06p ttrammel
# SW7405-3833: Modify for pre-built binary build.
#
# 7   10/16/09 4:41p erickson
# SW7400-2511: switch to NEXUS_PLATFORM
#
# 6   9/10/09 1:32p erickson
# SW7420-228: record_append needs bcmplayer/include
#
# 5   7/16/09 12:06p vsilyaev
# PR 55653: Added example to play LPCM audio using playpump and audio
#  decoder
#
# 4   4/22/09 11:24a jtna
# PR54389: added playpump_two_pes_files.c
#
# 3   2/19/09 3:18p erickson
# PR51841: remove examples_lib
#
# 2   2/19/09 1:32p erickson
# PR51841: refactor
#
# 1   2/4/09 4:01p erickson
# PR51841: add files
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

THEAPPS = \
	mosaic_playback \
	playback \
	playback_customfile \
	playpump \
	playpump_lpcm \
	playpump_scatter_gather \
	playpump_two_pes_files \
	recpump \
	recpump_with_wrap \
	record \
	record_allpass \
	record_allpass_from_playback \
	record_allpass_from_remux \
	record_from_playback \
	timeshift \
	index_audio \
	codec_probe \
	playback_pcr_indexing

# The following app can't be build in prebuilt binary mode
ifneq ($(NEXUS_PREBUILT_BINARY),y)
THEAPPS +=    record_append
endif


# This will be defined in platform_app.inc appropriately
ifdef NEXUS_DTV_PLATFORM
# DTV-specific apps
else
# STB-specific apps
endif

# IP Tuning Example: currently not enabled for VxWorks
ifneq ($(findstring linux,$(OS)),)
THEAPPS += \
	playpump_ip
endif

CFLAGS += -I$(NEXUS_TOP)/../BSEAV/lib/netaccel/include
CFLAGS += -I$(NEXUS_TOP)/../BSEAV/lib/bcmplayer/include

codec_probe: CFLAGS += -I$(NEXUS_TOP)/../BSEAV/lib/media -I$(NEXUS_TOP)/../BSEAV/lib/utils

# ASF with DRM example.
# Need to build and link the DRM library.
#
ifeq ($(MSDRM_PRDY_SUPPORT), y)

	CFLAGS += -I$(BSEAV)/lib/playready -I$(NEXUS_TOP)/../BSEAV/lib/media -I$(NEXUS_TOP)/../BSEAV/lib/utils -I$(NEXUS_TOP)/../BSEAV/lib/asf  -I$(NEXUS_TOP)/../BSEAV/lib/piff
	THEAPPS += \
		playback_encrypted_asf
		#playback_encrypted_mp4 

#    ifeq ($(B_DRM_OEMSECURESTORE), y)
#    EXTRA_DRM_OBJS += $(BSEAV)/lib/openssl/openssl-0.9.8h_mipsel-uclibc/libcrypto.a
#    endif

endif

include ../Makefile.inc

BCMPLAYER_DIR=$(NEXUS_TOP)/../BSEAV/lib/bcmplayer
CFLAGS += -I$(BCMPLAYER_DIR)/include -I$(BCMPLAYER_DIR)/utils
vpath %.c $(BCMPLAYER_DIR)/utils
playback_customfile: playback_customfile.c b_customfile.c b_playback.c
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(LDFLAGS) $(CFLAGS)
	$(CP) $@ $(NEXUS_BIN_DIR)
