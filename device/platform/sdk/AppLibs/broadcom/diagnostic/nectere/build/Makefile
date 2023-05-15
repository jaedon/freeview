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
# $brcm_Revision: 5 $
# $brcm_Date: 7/20/11 3:26p $
# 
# Module Description:
# 
# Revision History:
#  
#############################################################################
NEXUS_TOP := $(NEXUS)
APPLIBS_TOP := $(NEXUS_TOP)/../AppLibs

include $(APPLIBS_TOP)/opensource/directfb/build/directfb_common.inc
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc

ifeq ($(B_REFSW_ARCH),)
B_REFSW_ARCH = mipsel-linux
endif

ifeq ($(B_REFSW_DEBUG),n)
TARGET_SUFFIX:= release
CXXFLAGS = -O3
else
TARGET_SUFFIX:= debug
CXXFLAGS = -O0 -g3
endif

BUILD_DIR:= $(shell pwd)
APPLIBS_DIR:= $(BUILD_DIR)/../../../..
BCHP_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
TARGET_DIR:= $(APPLIBS_DIR)/target/$(PLATFORM)$(BCHP_VER_LOWER).$(B_REFSW_ARCH)
OBJ_DIR:= $(BUILD_DIR)/$(TARGET_SUFFIX)
TARGET_DIR:= $(TARGET_DIR).$(TARGET_SUFFIX)
LIB_DIR:= $(TARGET_DIR)/usr/local/lib

NEXUS_PLATFORM = $(PLATFORM)

SRC_DIR:=$(BUILD_DIR)/../src
INC_DIR:=$(BUILD_DIR)/../../include

DFB_DIR:= $(APPLIBS_DIR)/opensource/directfb/src/DirectFB-${DIRECTFB_VERSION}

CXXFLAGS += -Wall -fmessage-length=0 -fPIC

# This is the minimum needed to compile and link with Nexus
CXXFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -I$(INC_DIR)

CXXFLAGS += -I$(DFB_DIR) -I$(DFB_DIR)/include -I$(DFB_DIR)/lib

LDFLAGS := -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} -lpthread -lm  $(EXTRA_DRM_OBJS)

SRC = $(wildcard $(SRC_DIR)/*.cpp)

OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

NECTERE = libnectere.so

all: $(OBJ_DIR) $(NECTERE) install

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo [Compiling... $<]
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NECTERE): $(OBJS)
	@echo [Building... $@]
	$(CXX) -shared -o $(NECTERE) $(OBJS) $(LDFLAGS)

install:
	cp -f $(NECTERE) $(LIB_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(NECTERE)

deps: nexus playback_ip os

nexus:
	cd $(NEXUS_TOP)/build; \
	make clean; \
	make;

playback_ip:
	cd $(NEXUS_TOP)/lib/playback_ip; \
	make clean; \
	make; 

os:	
	cd $(NEXUS_TOP)/lib/os; \
	make clean; \
	make
