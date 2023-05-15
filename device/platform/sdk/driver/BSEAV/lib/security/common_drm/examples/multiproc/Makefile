############################################################
#    (c)2003-2012 Broadcom Corporation
# 
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
#  
# Except as expressly set forth in the Authorized License,
#  
# 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#  
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
# USE OR PERFORMANCE OF THE SOFTWARE.
# 
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
# ANY LIMITED REMEDY.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 4 $
# $brcm_Date: 9/6/12 11:28p $
# 
# Module Description:
# 
# Revision History:
# 
############################################################

NEXUS_TOP := $(shell cd ../../../../../../nexus; pwd)
BSEAV_TOP := $(shell cd ../../../../../../BSEAV; pwd)
MAGNUM_TOP := $(shell cd ../../../../../../magnum; pwd)

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

include $(BSEAV_TOP)/lib/security/common_drm/lib/cmndrm_dir.inc

SRVR_APPS = test_drm_server
SRVR_SRCS = test_drm_server.c
SRVR_OBJS = ${SRVR_SRCS:.c=.o}
CLI_APPS = test_widevine_xpt_client   test_widevine_2_client
CLI_SRCS = test_widevine_xpt_client.c test_widevine_2_client.c
CLI_OBJS = ${CLI_SRCS:.c=.o}

WITH_APP_IPC ?=y

# include cross-compiler definitions
ifeq ($(NEXUS_PREBUILT_BINARY),y)
# do "make api" and "make nexus_headers" first, then "make NEXUS_PREBUILT_BINARY=y"
include $(NEXUS_TOP)/bin/include/platform_app.inc
#Q_ ?= @
else
NEXUS_PLATFORM ?= $(PLATFORM)
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc
endif

ifeq ($(WITH_APP_IPC),y)
CFLAGS += -I${IPC} -Istubs
# these next ones are for Makefile.inc - see below 
EXTRA_OBJS :=  bipc_util.o bipc_server.o bipc_client.o
IPC :=  ${NEXUS_TOP}/lib/ipc
IPC_STUBS := stubs/ipc_stubs_server.h stubs/ipc_stubs_client.h 
vpath %.c ${IPC}
.PHONY: stubs clean_stubs
endif

include $(NEXUS_TOP)/examples/Makefile.inc

.PHONY: client server clean clean_client clean_server clean_client_server

# common crypto include path
CFLAGS += -I${BSEAV_TOP}/lib/security/common_drm/include
CFLAGS += -I${NEXUS_TOP}/examples/multiprocess

LDFLAGS += -L$(BSEAV_TOP)/lib/security/common_drm/lib/${CMNDRM_DIR}/ -lcmndrm 
LDFLAGS += -L$(BSEAV_TOP)/lib/security/bcrypt/lib/ -lbcrypt
LDFLAGS += -L$(BSEAV_TOP)/lib/drmrootfs/ -ldrmrootfs
LDFLAGS += -L$(BSEAV_TOP)/lib/openssl/$(PLATFORM)/lib -lcrypto

ifneq ($(NEXUS_MODE),proxy)
# user mode
CLI_FLAGS = NEXUS_MODE=client
endif
ifeq ($(WITH_APP_IPC),y)

${SRVR_APPS} ${CLI_APPS}: ${IPC_STUBS}

server:
	@echo [Building... $@]
	$(MAKE) -C $(NEXUS_TOP)/build
	$(MAKE) $(SRVR_APPS)

client:
	@echo [Building... $@]
ifneq ($(NEXUS_MODE),proxy)
	$(MAKE) $(CLI_FLAGS) -C $(NEXUS_TOP)/build
endif
	$(MAKE) $(CLI_FLAGS) $(CLI_APPS)

clean_client_server:
	@echo [Clean... $@]
	-$(RM) $(SRVR_APPS) $(CLI_APPS) *.d *.o *.out

clean_client:
	@echo [Clean... $@]
	-$(RM) $(CLI_APPS) $(CLI_OBJS)

clean_server:
	@echo [Clean... $@]
	-$(RM) $(SRVR_APPS) $(SRVR_OBJS)

clean:
	$(MAKE) clean_client_server
	$(MAKE) clean_stubs
ifneq ($(NEXUS_MODE),proxy)
	# clean libnexus_client w/ apps
	$(MAKE) $(CLI_FLAGS) -C $(NEXUS_TOP)/build clean
endif

${IPC_STUBS} : ${NEXUS_TOP}/examples/multiprocess/brc_client.h
	${Q_}${MKDIR} -p stubs
	${Q_}${PERL} -I ${NEXUS_TOP}/build/tools/common ${IPC}/build/bipc_build.pl ${IPC_STUBS} $^

stubs:
	$(MAKE) $(IPC_STUBS)
	
clean_stubs:
	-$(RM) -rf stubs
endif
