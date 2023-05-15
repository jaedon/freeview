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
# $brcm_Revision: 34 $
# $brcm_Date: 9/20/12 1:30p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/examples/multiprocess/Makefile $
# 
# 34   9/20/12 1:30p erickson
# SW7435-27: link app to -lnexus_webcpu.so if NEXUS_WEBCPU=core1_server
# 
# 33   8/20/12 3:37p erickson
# SW7425-3260: rely on NEXUS_MODE=client building client and server in
#  usermode
# 
# 32   6/13/12 3:07p erickson
# SW7425-2981: refactor into b_simpledecoder wrapper library
# 
# 31   6/12/12 4:54p erickson
# SW7425-2981: use media probe
# 
# 30   6/6/12 10:12a erickson
# SW7420-1123: ensure client apps are built with NEXUS_MODE=client if
#  needed
# 
############################################################

############################################################
#
# Nexus multiprocess examples
#
# The multiprocess directory contains both client and server apps.
# For kernel mode (i.e. NEXUS_MODE=proxy), all apps link to libnexus.so.
# for user mode (i.e. NEXUS_MODE=client), server apps link to libnexus.so and client apps link to libnexus_client.so.
# In both cases, nexus will perform an internal two-pass build to generate both driver/proxy or server/client.
# The following are typical build instructions:
# 
# # make nexus server, client and all apps
# make 
# 
# # make nexus client and all client apps
# make client
# 
# # make nexus server and all server apps
# make server
# 
# # clean nexus server, client and all apps
# make clean
# 
# # clean just the apps
# make clean_apps
#
# # make a specific client or server app
# make <appname>
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../.. && cd)
else
NEXUS_TOP := $(shell cd ../../; pwd)
endif

WITH_APP_IPC ?=y

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

SERVER_APPS = decode_server blit_server mosaic_decode_server input_router register_server
CLIENT_APPS = \
	animation_client \
	audio_client \
	blit_client \
	cmd_client \
	decode_client \
	input_client \
	live_client \
	performance_test \
	picture_decoder_client \
	playpump_encrypted_client \
	playback_encrypted_client \
	published_tunneled_client \
	register_client \
	smooth_pig \
	tune_qam_client \
	tunneled_client

ifeq ($(WITH_APP_IPC),y)
IPC_STUBS := stubs/ipc_stubs_server.h stubs/ipc_stubs_client.h 
IPC :=  ${NEXUS_TOP}/lib/ipc
CFLAGS += -I${IPC} -Istubs
EXTRA_OBJS :=  bipc_util.o bipc_server.o bipc_client.o
IPC_STUBS := stubs/ipc_stubs_server.h stubs/ipc_stubs_client.h 
vpath %.c ${IPC}
ifneq ($(NEXUS_MODE),client)
SERVER_APPS += refsw_server
endif
.PHONY: clean_stubs
OTHER_APP_CLEANS += clean_stubs
endif

THEAPPS = $(CLIENT_APPS) $(SERVER_APPS)
ifeq ($(NEXUS_MODE),)
# if not proxy, then force usermode multiprocess
NEXUS_BUILD_OPTIONS = NEXUS_MODE=client
endif
include ../Makefile.inc

# media probe
BSEAV = $(NEXUS_TOP)/../BSEAV
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

ifeq ($(WITH_APP_IPC),y)
${SERVER_APPS} ${CLIENT_APPS}: ${IPC_STUBS}

${IPC_STUBS} : brc_client.h
	${Q_}${MKDIR} -p stubs
	${Q_}${PERL} -I ${NEXUS_TOP}/build/tools/common ${IPC}/build/bipc_build.pl ${IPC_STUBS} $^

clean_stubs:
	-$(RM) -rf stubs
endif

server: api $(SERVER_APPS)

client: api $(CLIENT_APPS)

NEXUS_LDFLAGS := -L${NEXUS_BIN_DIR} -lpthread -lm
ifeq ($(NEXUS_MODE),proxy)
NEXUS_CLIENT_LDFLAGS := -lnexus $(NEXUS_LDFLAGS)
else
ifeq ($(NEXUS_WEBCPU),core1_server)
NEXUS_CLIENT_LDFLAGS := -lnexus_webcpu $(NEXUS_LDFLAGS)
else
NEXUS_CLIENT_LDFLAGS := -lnexus_client $(NEXUS_LDFLAGS)
endif
endif
NEXUS_LDFLAGS += -lnexus

refsw_server: refsw_server.c b_simpledecoder.c $(patsubst %.o,%.c,$(EXTRA_OBJS))
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(NEXUS_LDFLAGS)
	cp -f $@ ${NEXUS_BIN_DIR}

$(filter-out refsw_server, $(SERVER_APPS)): %: %.c $(patsubst %.o,%.c,$(EXTRA_OBJS))
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(NEXUS_LDFLAGS)
	cp -f $@ ${NEXUS_BIN_DIR}

# use make's static pattern rules to give clients special LDFLAGS
$(CLIENT_APPS): %: %.c $(patsubst %.o,%.c,$(EXTRA_OBJS))
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(NEXUS_CLIENT_LDFLAGS)
	cp -f $@ ${NEXUS_BIN_DIR}
