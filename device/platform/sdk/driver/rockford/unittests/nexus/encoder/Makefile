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
# $brcm_Revision: Hydra_Software_Devel/8 $
# $brcm_Date: 8/31/12 1:49p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /rockford/unittests/nexus/encoder/Makefile $
# 
# Hydra_Software_Devel/8   8/31/12 1:49p hongtaoz
# SW7425-3425, SW7425-3349: merged FNRT to mainline; added FNRT unit
# test;
# 
# Hydra_Software_Devel/FNRT_Demo/1   7/5/12 1:25p hongtaoz
# SW7425-3349: added transcode_fnrt example;
# 
# Hydra_Software_Devel/7   5/17/12 3:41p gmohile
# SW7425-1473 : Merge to mainline
# 
# Hydra_Software_Devel/6   3/12/12 4:41p gkamosa
# SW7425-2543: Merge support for 3383 SMS platform
# 
# Hydra_Software_Devel/Centaurus_Phase_1.0/1   2/10/12 5:36p jmiddelk
# SW7425-2337: Add support for 3383SMS platform
# 
# Hydra_Software_Devel/5   12/19/11 6:53p hongtaoz
# SW7425-1110: added user data callback to log the source and loopback
# decoders user data and parsed closed caption user data to verify the
# cc data transcoding function;
# 
# Hydra_Software_Devel/4   6/3/11 10:48a erickson
# SW7425-42: encoder unittests require oslib and tshdrbuilder. rework
# Makefile so that it's explicit and local.
# 
# Hydra_Software_Devel/3   6/1/11 1:58p nilesh
# SW7425-447: merge the asf muxer test to mainline;
# 
# Hydra_Software_Devel/SW7425-447/1   5/18/11 3:16p nilesh
# SW7425-447: Fixed ASF Support
# 
# Hydra_Software_Devel/2   5/18/11 2:22p hongtaoz
# SW7425-42: use ts header builder library to construct PAT/PMT for
# stream mux;
# 
# Hydra_Software_Devel/1   4/8/11 6:49p hongtaoz
# SW7425-165: move configurable nexus transcode unittests to rockford;
# 
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
    encode_video_es \
    encode_audio_es \
    transcode_ts \
    transcode_mp4 \
    transcode_asf \
    transcode_fnrt

# include tshdrbuilder lib
VIDEO_ENCODER_SUPPORT?=y

# transcode_ts required oslib
OS_LIB_UNITTESTS=y

# transcode_ts required udp to parse user data 
BUILD_TRANSCODE_USERDATA = y

include ../common/Makefile.inc

ifeq ($(SSL_SUPPORT), y)
LDFLAGS += -L$(OPENSSL_DIR) -lssl -lcrypto -ldl
endif

# This builds tshdrbuilder lib for PAT/PMT muxing
ifeq ($(VIDEO_ENCODER_SUPPORT),y)
.PHONY: tshdrbuilderlib clean_tshdrbuilderlib 
clean: clean_tshdrbuilderlib 
$(APP_BINS): tshdrbuilderlib 

clean_tshdrbuilderlib:
	$(MAKE) -C $(BSEAV)/lib/tshdrbuilder clean

tshdrbuilderlib:
	$(MAKE) -C $(BSEAV)/lib/tshdrbuilder

# add flags for tshdrbuilder lib
include $(BSEAV)/lib/tshdrbuilder/tshdrbuilder.inc
CFLAGS += $(addprefix -I,$(TSHDRBUILDER_INCLUDES))
LDFLAGS += -ltshdrbuilder

# add flags for bcmplayer lib
include $(BSEAV)/lib/bcmplayer/bcmplayer.inc
CFLAGS += $(addprefix -I,$(B_NAV_PLAYER_INCLUDES))
vpath %c $(BSEAV)/lib/bcmplayer/src
vpath %h $(BSEAV)/lib/bcmplayer/include
endif



.PHONY: psiplib clean_psiplib
clean: clean_psiplib
$(APP_BINS): psiplib

B_PSIP_TOP := $(NEXUS_TOP)/../rockford/lib/psip

clean_psiplib:
	$(MAKE) -C $(B_PSIP_TOP) clean

psiplib:
	$(MAKE) -C $(B_PSIP_TOP)

# add flags for psiplib 
include $(B_PSIP_TOP)/b_psip_lib.inc
CFLAGS += $(addprefix -I,$(B_PSIP_LIB_PUBLIC_INCLUDES)) -I$(BSEAV)/api/include
LDFLAGS += -lb_psip


include $(BSEAV)/lib/bfile/bfile.inc
ifeq ($(MEDIA_AVI_SUPPORT),y)
include $(BSEAV)/lib/avi/bavi.inc
endif
ifeq ($(MEDIA_ASF_SUPPORT),y)
include $(BSEAV)/lib/asf/basf.inc
endif
ifeq ($(MEDIA_RMFF_SUPPORT),y)
include $(BSEAV)/lib/media/rmff/brmff.inc
endif
include $(BSEAV)/lib/utils/batom.inc
include $(BSEAV)/lib/media/bmedia.inc
CFLAGS += $(addprefix -I,$(BMEDIA_INCLUDES) $(BFILE_MEDIA_INCLUDES)) -I$(BSEAV)/lib/mpeg2_ts_parse -I$(BSEAV)/lib/tspsi
CFLAGS += $(addprefix -D,$(BMEDIA_PROBE_DEFINES))

vpath %.c $(foreach file, $(PROBE_SRCS), $(dir $(patsubst %.c, %.o, $(file))))



