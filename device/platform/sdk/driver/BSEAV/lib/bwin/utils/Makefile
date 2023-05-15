############################################################
#     (c)2003-2008 Broadcom Corporation
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
# $brcm_Revision: 8 $
# $brcm_Date: 5/25/12 2:06a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /BSEAV/lib/bwin/utils/Makefile $
# 
# 8   5/25/12 2:06a tokushig
# SW7231-749: update Makefile to build buildfont utility
# 
# 7   10/19/11 2:48p mward
# SW7125-1119:  Change DEBUG to B_REFSW_DEBUG.
# 
# 6   7/26/11 1:33p erickson
# SW7420-1992: add surfacecmp support
# 
# 5   3/24/11 4:25p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 4   2/18/10 9:36a erickson
# SW3548-2788: move settop api code to subdir
#
# 3   2/17/10 3:05p erickson
# SW3548-2788: add osddump
#
# 2   3/13/09 2:52p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 1   9/9/08 2:01p erickson
# PR46295: create nexus variety of showimage
#
############################################################
# Nexus examples

FREETYPE_SUPPORT=y

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc
OS ?= $(B_REFSW_OS)
B_REFSW_ARCH?=mipsel-linux

BSEAV=$(shell cd ../../.. && ${PWD})
CFLAGS += $(SYSTEM_FLAGS)

include $(BSEAV)/lib/bwin/include/bwin.mak
CFLAGS += $(BWIN_CFLAGS)
LDFLAGS += ${BWIN_LDFLAGS}

APPS = showimage osddump buildfont

.PHONY: bwin api clean_apps clean_api clean_bwin

ifeq ($(APP),)
all: $(APPS)
$(APPS): bwin api
else
all: $(APP)
$(APP): bwin api
endif

# For linux builds, link to the correct libraries
ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS += -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} -lpthread -lm
endif

# build bwin
bwin:
	$(MAKE) FREETYPE_SUPPORT=y -C ../build

# build nexus
api:
	$(MAKE) -C $(NEXUS_TOP)/build

# This cleans nexus and local apps
clean: clean_bwin clean_api clean_apps

clean_bwin:
	$(MAKE) -C ../build clean

clean_api:
	$(MAKE) -C $(NEXUS_TOP)/build clean

clean_apps:
	-$(RM) $(APPS) *.d *.o *.out;

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

# Always build with debug
CFLAGS += -g

# Implicit rule for building local apps
%: %.c $(EXTRA_OBJS)
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS)
	cp $@ ${NEXUS_BIN_DIR}

install: all

