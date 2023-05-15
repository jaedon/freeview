############################################################
#    (c)2012 Broadcom Corporation
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
# $brcm_Revision: 1 $
# $brcm_Date: 11/30/12 4:11p $
# 
# Module Description:
# 
# Revision History:
# 
############################################################

NEXUS_TOP := $(shell cd ../../../../../../../../../nexus; pwd)
BSEAV_TOP := $(shell cd ../../../../../../../../../BSEAV; pwd)

APPS = test_oem_crypto_widevine
SRCS = test_oem_crypto_widevine.c 
OBJS = ${SRCS:.c=.o}

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

ifeq ($(APPS),)
$(error Must define APPS)
endif

#include $(BSEAV_TOP)/lib/security/common_drm/lib/cmndrm_dir.inc
CFLAGS += -EL
APP_BINS = $(APPS)

.PHONY: api clean

all: $(APP_BINS) copy

$(APP_BINS): 
LDFLAGS += -EL
LDFLAGS := -lpthread -lm
LDFLAGS += -L$(BSEAV_TOP)/lib/security/common_drm -lcmndrm 
LDFLAGS += -L$(BSEAV_TOP)/lib/security/bcrypt -lbcrypt
LDFLAGS += -L$(NEXUS_TOP)/bin -lnexus 
LDFLAGS += -L$(BSEAV_TOP)/lib/security/third_party/android/drm/widevine/OEMCrypto/ -loemcrypto
LDFLAGS += -L$(BSEAV_TOP)/lib/drmrootfs/ -ldrmrootfs
#LDFLAGS += -L$(BSEAV_TOP)/lib/openssl/${PLATFORM}/lib -lcrypto
LDFLAGS  += -L$(BSEAV_TOP)/../AppLibs/opensource/android/src/mips-ics/out/target/product/bcm7425/system/lib -lcrypto


# Always build with debug
CFLAGS += -g -I$(BSEAV_TOP)/lib/security/common_drm/include 
CFLAGS += -I$(BSEAV_TOP)/lib/security/bcrypt/include 
CFLAGS += -I$(BSEAV_TOP)/lib/security/third_party/android/drm/widevine/OEMCrypto/include
#CFLAGS += -I$(NEXUS_TOP)/modules/security/${BCHP_CHIP}/include/priv
CFLAGS += -I$(BSEAV_TOP)/lib/openssl/${PLATFORM}/include


# common crypto include path
CFLAGS += -I${BSEAV_TOP}/lib/security/common_crypto/include

#$(error XXX $(CFLAGS))

# This cleans nexus and local apps
clean:
	-$(RM) $(APP_BINS) *.d *.o *.out;

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
#$(warning "NEXUS_APP_INCLUDE_PATHS is ${NEXUS_APP_INCLUDE_PATHS}")

# Implicit rule for building local apps
#$(APPS): prep_openssl $(OBJS)
$(APPS): $(OBJS)
	@echo [Link... $@]
	@echo $(NEXUS_TOP)
	@echo $(LDFLAGS)
	${Q_} $(CC) -o $@ $@.o $(LDFLAGS)

prep_openssl:
	if [ ! -d "${BSEAV_TOP}/lib/openssl/${PLATFORM}" ]; then \
		echo "${BSEAV_TOP}/lib/openssl/${PLATFORM} not found, calling make in ${BSEAV_TOP}/lib/openssl/"; \
		make -C ${BSEAV_TOP}/lib/openssl/; \
	fi
	
.c.o: 
	@echo [Compile... $<]
	${Q_}$(CC) -c $< $(CFLAGS) 

copy:
	@echo [Copying executables...]
	cp $(APPS) $(NEXUS_TOP)/bin	
	@echo [Copying proper Common DRM lib version...]
	cp $(BSEAV_TOP)/lib/security/common_drm/lib/${CMNDRM_DIR}/libcmndrm.so $(NEXUS_TOP)/bin
	@echo [Copying dependent libs drmrootfs, openssl...]
	cp -f $(BSEAV_TOP)/lib/openssl/${PLATFORM}/lib/libcrypto.so.1.0.0 $(NEXUS_TOP)/bin
	cp -f $(BSEAV_TOP)/lib/drmrootfs/libdrmrootfs.so $(NEXUS_TOP)/bin
	cp -f $(BSEAV)/lib/security/bcrypt/lib/libbcrypt.so $(NEXUS_TOP)/bin
			
