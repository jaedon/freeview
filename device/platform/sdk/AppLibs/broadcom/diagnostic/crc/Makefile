#############################################################################
#    (c)2011 Broadcom Corporation
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
# $brcm_Revision: 9 $
# $brcm_Date: 11/14/11 6:11p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/broadcom/diagnostic/crc/Makefile $
# 
# 9   11/14/11 6:11p jliang
# SW7425-1582: Rollback changes as they broke crc build.
# 
# 8   11/4/11 6:53p ismailk
# SW7425-1582:Link with libs in /usr/local/lib 
# 
# 7   11/3/11 4:32p jliang
# SW7346-463: Removed libcrc-nexus.so dependency on .so files.
# 
# 6   11/1/11 10:20a jliang
# SW7346-463: Change how CRC is enabled.
# 
# 5   6/21/11 4:17p jliang
# SW7346-103: Added mvc app.
# 
# 4   5/24/11 1:04p jliang
# SW7346-103: adding frontend app.
# 
# 3   4/1/11 6:08p jliang
# SW7346-103: add back isCaptureDone.
# 
# 2   3/30/11 5:18p jliang
# SW7346-103: added cp command to copy crc to nexus/bin.
# 
# 1   3/29/11 12:35p jliang
# SW7346-103: adding CRC app source code.
# 
#############################################################################
ifeq ($(HUMAX_PLATFORM_BASE),y)
APPLIBS_TOP := $(NEXUS_TOP)/AppLibs
NEXUS_TOP := $(NEXUS_TOP)
BSEAV :=  $(BSEAV_TOP)
APPLIBS := $(APPLIBS_TOP)
NEXUS_PLATFORM = $(PLATFORM)
else
NEXUS_TOP := ../../../../nexus
BSEAV :=  ../../../../BSEAV
APPLIBS := ../../..
NEXUS_PLATFORM = $(PLATFORM)
APPLIBS_TOP := $(NEXUS_TOP)/../AppLibs
endif

ifneq ($(wildcard $(APPLIBS_TOP)/opensource/common/common.inc),)
include $(APPLIBS_TOP)/opensource/common/common.inc
endif

include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc

ifeq ($(B_REFSW_DEBUG),y)
OBJECTS_DIR = $(PLATFORM)$(BCHP_VER_LOWER).$(B_REFSW_ARCH).debug
CFLAGS += -g
else
OBJECTS_DIR = $(PLATFORM)$(BCHP_VER_LOWER).$(B_REFSW_ARCH).release
endif
TARGET_LIB_DIR = $(APPLIBS_TOP)/target/$(OBJECTS_DIR)/usr/local/lib

CXXFLAGS = -O2 -Wall -fmessage-length=0 -fPIC -std=c++0x

# This is the minimum needed to compile and link with Nexus
CXXFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -I$(NEXUS_TOP)/lib/playback_ip/include -I../include

ifeq (${GPIO_CRC_PIN},y)
	CXXFLAGS += -DGPIO_CRC_PIN
endif
# HUMAX_PLATFORM_BASE for HUMAX modification
ifeq ($(strip $(HUMAX_PLATFORM_BASE)),y)
CXXFLAGS += $(addprefix -include ,$(INC_AUTOCONF))
CXXFLAGS += $(addprefix -D,$(BCM_FIX))
endif

LDFLAGS := -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} -lpthread -lm  $(EXTRA_DRM_OBJS)
ifeq (${SSL_SUPPORT},y)
	LDFLAGS += -lssl -lcrypto -ldl
endif

LIVEMEDIA_DIR := $(BSEAV)/lib/livemedia/live
ifneq (${LIVEMEDIA_SUPPORT},n)
	include $(BSEAV)/lib/livemedia/blive_ext/include/blive_ext.mak
    #LDFLAGS += $(BLIVE_EXT_LDFLAGS) # -lblive_ext won't link because it was generated using ranlib. Link to .o files instead.
    LDFLAGS += $(patsubst $(BLIVE_EXT_DIR)/src/%.cpp,$(BLIVE_EXT_LIBDIR)/%.o,$(wildcard $(BLIVE_EXT_DIR)/src/*.cpp))
    
    LDFLAGS += -L$(LIVEMEDIA_DIR)/liveMedia -lliveMedia
    LDFLAGS += -L$(LIVEMEDIA_DIR)/groupsock -lgroupsock
    LDFLAGS += -L$(LIVEMEDIA_DIR)/UsageEnvironment -lUsageEnvironment
    LDFLAGS += -L$(LIVEMEDIA_DIR)/BasicUsageEnvironment -lBasicUsageEnvironment
endif

CRC_LDFLAGS := $(LDFLAGS) -lb_playback_ip -lb_os

OBJS = crc.o

FE_OBJS = crcfe.o

FE_CRC = crcfe

MVC_OBJS = crc_mvc.o

MVC_CRC = crc_mvc

LIB_OBJS = ReferenceCrc.o NexusCrc.o

LIB_CRC = libcrc-nexus.so

LIBS =

CRC = crc

ifeq ($(HUMAX_PLATFORM_BASE),y)
all: deps $(CRC) $(MVC_CRC)
else
all: $(CRC) $(MVC_CRC)
endif
	
# HUMAX_PLATFORM_BASE for HUMAX modification
# - add $(CXXFLAGS) in all targets for compile
ifeq ($(HUMAX_PLATFORM_BASE),y)
$(CRC):	$(OBJS) $(LIB_CRC)
	@echo [Building... $@]
	$(CXX) -o $(CRC) $(OBJS) $(CXXFLAGS) $(LIBS) $(CRC_LDFLAGS) -lcrc-nexus
	cp -f $(CRC) $(NEXUS_TOP)/bin

$(FE_CRC): $(FE_OBJS) $(LIB_CRC)
	@echo [Building... $@]
	$(CXX) -o $(FE_CRC) $(FE_OBJS) $(CXXFLAGS) $(LIBS) $(LDFLAGS) -lcrc-nexus
	cp -f $(FE_CRC) $(NEXUS_TOP)/bin
		
$(MVC_CRC): $(MVC_OBJS) $(LIB_CRC)
	@echo [Building... $@]
	$(CXX) -o $(MVC_CRC) $(MVC_OBJS) $(CXXFLAGS) $(LIBS) $(CRC_LDFLAGS) -lcrc-nexus
	cp -f $(MVC_CRC) $(NEXUS_TOP)/bin
	
$(LIB_CRC):	$(LIB_OBJS)
	@echo [Building... $@]
	$(CXX) -shared -o $(LIB_CRC) $(LIB_OBJS) $(CXXFLAGS) $(LIBS)
	cp -f $(LIB_CRC) $(NEXUS_TOP)/bin

else
$(CRC):	$(OBJS) $(LIB_CRC) playback_ip_deps
	@echo [Building... $@]
	$(CXX) -o $(CRC) $(OBJS) $(LIBS) $(CRC_LDFLAGS) -lcrc-nexus
	@cp -f $(CRC) $(NEXUS_TOP)/bin

$(FE_CRC): $(FE_OBJS) $(LIB_CRC)
	@echo [Building... $@]
	$(CXX) -o $(FE_CRC) $(FE_OBJS) $(LIBS) $(LDFLAGS) -lcrc-nexus
	@cp -f $(FE_CRC) $(NEXUS_TOP)/bin
		
$(MVC_CRC): $(MVC_OBJS) $(LIB_CRC) playback_ip_deps
	@echo [Building... $@]
	$(CXX) -o $(MVC_CRC) $(MVC_OBJS) $(LIBS) $(CRC_LDFLAGS) -lcrc-nexus
	@cp -f $(MVC_CRC) $(NEXUS_TOP)/bin
	
$(LIB_CRC):	$(LIB_OBJS)
	@echo [Building... $@]
	$(CXX) -shared -o $(LIB_CRC) $(LIB_OBJS) $(LIBS)
	cp -f $(LIB_CRC) $(NEXUS_TOP)/bin
ifneq ($(wildcard $(APPLIBS_TOP)/opensource/common/common.inc),)	
	cp -f $(LIB_CRC) $(TARGET_LIB_DIR)
endif
endif
	
clean:
	rm -f $(OBJS) $(CRC) $(FE_OBJS) $(FE_CRC) $(MVC_OBJS) $(MVC_CRC) $(LIB_OBJS) $(LIB_CRC)

ifeq ($(HUMAX_PLATFORM_BASE),y)
deps: playback_ip os
else
deps: nexus playback_ip os
endif

nexus:
	cd $(NEXUS_TOP)/build; \
	make clean; \
	make;

openssl:
ifeq (${SSL_SUPPORT}, $(if $(wildcard ${NEXUS_BIN_DIR}/libssl.so),don't_build,y))
ifneq ($(HUMAX_PLATFORM_BASE),y)
	export BSEAV_TARGET_LIB_DIR="../../../nexus/bin"; \
	make -C ${BSEAV}/lib/openssl; \
	make -C ${BSEAV}/lib/openssl install;
endif
endif

livemedia:
ifneq (${LIVEMEDIA_SUPPORT},n)
	make -C $(LIVEMEDIA_DIR)
	make -C $(BSEAV)/lib/livemedia/blive_ext/build
endif

playback_ip_deps: openssl livemedia

playback_ip: playback_ip_deps
	cd $(NEXUS_TOP)/lib/playback_ip; \
	make clean; \
	make;

os:	
	cd $(NEXUS_TOP)/lib/os; \
	make clean; \
	make

