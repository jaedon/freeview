#############################################################################
#    (c)2010-2011 Broadcom Corporation
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
# $brcm_Revision: 6 $
# $brcm_Date: 11/21/12 3:43p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /BSEAV/lib/security/common_drm/examples/common/Makefile $
# 
# 6   11/21/12 3:43p cdisc
# SWSECURITY-200: memory heap management issue
# 
# 5   9/6/12 11:28p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/5/12 5:16p mphillip
# SW7425-3739: Remove BSEAV/lib symlinks
# 
# 4   6/1/12 10:25a cdisc
# SW7425-3138: adjusting Makefile after changes to bcrypt and openssl
# 
# 3   5/23/12 2:22p celysee
# SW7425-2894: Adding common drm adn dtcp-ip
# 
# 2   5/15/12 3:51p cdisc
# SW7425-3067: linking statically with opennsl
# 
# 1   4/19/12 9:58a celysee
# SW7425-2894: Support for DTCP-IP with robustness rules
# 
#############################################################################

NEXUS_TOP := $(shell cd ../../../../../../nexus; pwd)
BSEAV_TOP := $(shell cd ../../../../../../BSEAV; pwd)


APPS = test_bcrypt test_multi_thread_partial_sha1 test_multi_thread_capps
#APPS = test_rng test_bcrypt test_multi_thread_partial_sha1 test_multi_thread_capps
SRCS = test_bcrypt.c test_multi_thread_partial_sha1.c test_multi_thread_capps.c
#SRCS = test_rng.c test_bcrypt.c test_multi_thread_partial_sha1.c test_multi_thread_capps.c
OBJS = ${SRCS:.c=.o}


# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

ifeq ($(APPS),)
$(error Must define APPS)
endif

include $(BSEAV_TOP)/lib/security/common_drm/lib/cmndrm_dir.inc

#$(error OBJS = ${OBJS} xxx APPS = ${APPS})

APP_BINS = $(APPS)

.PHONY: api clean

all: $(APP_BINS) copy


# For linux builds, link to the correct libraries
LDFLAGS := -L$(BSEAV_TOP)/lib/security/common_drm/lib/${CMNDRM_DIR}/ -lcmndrm 
LDFLAGS += -L$(BSEAV_TOP)/lib/security/bcrypt/lib/ -lbcrypt
LDFLAGS += -L$(NEXUS_TOP)/bin -lpthread -lm -lnexus
LDFLAGS += -L$(BSEAV_TOP)/lib/openssl/$(PLATFORM)/lib -lcrypto
LDFLAGS += -L$(BSEAV_TOP)/lib/drmrootfs/ -ldrmrootfs


# Always build with debug
CFLAGS += -g

# This cleans nexus and local apps
clean:
	#$(MAKE) -C $(NEXUS_TOP)/build clean
	-$(RM) $(APP_BINS) *.d *.o *.out;

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
#$(warning "NEXUS_APP_INCLUDE_PATHS is ${NEXUS_APP_INCLUDE_PATHS}")

# Always build with debug
CFLAGS += -g 
CFLAGS += -I$(BSEAV_TOP)/lib/security/common_drm/include 
CFLAGS += -I$(BSEAV_TOP)/lib/security/common_drm/include/priv
CFLAGS += -I$(NEXUS_TOP)/modules/security/${BCHP_CHIP}/include/priv
CFLAGS += -I$(BSEAV_TOP)/lib/openssl/$(PLATFORM)/include

# common crypto include path
CFLAGS += -I${BSEAV_TOP}/lib/security/common_crypto/include

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
#$(warning "NEXUS_APP_INCLUDE_PATHS is ${NEXUS_APP_INCLUDE_PATHS}")

# Implicit rule for building local apps
$(APPS): $(OBJS)
	@echo [Link... $@]
	@echo $(NEXUS_TOP)
	@echo $(LDFLAGS)
	${Q_} $(CC) -o $@ $@.o $(LDFLAGS)

.c.o: 
	@echo [Compile... $<]
	${Q_}$(CC) -c $< $(CFLAGS) 

copy:
	@echo [Copying executables...]
	cp $(APPS) $(NEXUS_TOP)/bin	
	@echo [Copying proper lib version...]
	cp -f $(BSEAV_TOP)/lib/security/common_drm/lib/${CMNDRM_DIR}/libcmndrm.so $(NEXUS_TOP)/bin
	@echo [Copying dependent libs drmrootfs, openssl...]
	cp -f -P $(BSEAV_TOP)/lib/openssl/$(PLATFORM)/lib/libcrypto.* $(NEXUS_TOP)/bin
	cp -f $(BSEAV_TOP)/lib/drmrootfs/libdrmrootfs.so $(NEXUS_TOP)/bin
	cp -f $(BSEAV)/lib/security/bcrypt/lib/libbcrypt.so $(NEXUS_TOP)/bin

