############################################################
#	  (c)2003-2012 Broadcom Corporation
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
#  1.	  This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile: Makefile.inc $
# $brcm_Revision: 17 $
# $brcm_Date: 8/20/12 3:37p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/examples/Makefile.inc $
# 
# 17   8/20/12 3:37p erickson
# SW7425-3260: rely on NEXUS_MODE=client building client and server in
#  usermode
# 
# 16   6/20/12 5:33p erickson
# SWANDROID-57: android mods
# 
# 15   10/28/11 10:38a erickson
# SW7420-1123: rework to build client and server in two-pass build for
#  linuxuser
# 
# 14   5/13/11 9:47a erickson
# SW7340-265: prevent unnecessary rebuilds
# 
# 13   5/13/11 9:11a erickson
# SW7420-1819: move linuxkernel_example into subdir and reimplement with
#  kbuild
# 
# 12   1/28/11 2:36p jtna
# SW7422-232: merge Android changes
# 
# 11   11/11/10 1:51p erickson
# SW7420-1228: add B_REFSW_ANDROID logic
# 
# 10   9/29/10 9:34a erickson
# SW7420-1123: add linux usermode IPC
# 
# 9   3/22/10 5:30p ttrammel
# SW7405-3833: Modify for pre-built binary build.
# 
# 8   3/19/10 10:07a erickson
# SW7405-3832: add clean varieties
# 
# 7   9/16/09 1:42p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 6   3/31/09 7:39p katrep
# PR53733: Fixed MSDRM_PD_SUPPORT compilation which was broken after the
#  move dvr dir
#
# 5   3/13/09 2:45p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 4   3/2/09 10:43a rjlewis
# PR51841: fix vxworks builds.
#
# 3   2/5/09 1:02p erickson
# PR51841: update
#
# 2   2/4/09 3:51p erickson
# PR51841: refactor
#
# 1   2/4/09 2:42p erickson
# PR51841: reorganize examples
#
############################################################
# Nexus examples

ifeq ($(B_REFSW_OS),vxworks)
# VxWorks needs a wrapper function to call main.
EXTRA_OBJS += ../vxworks_cmd.o
# We like to use .out for loadable objects.
APPSUFFIX = .out
APPS = $(addsuffix .out, $(THEAPPS))
else
APPS = $(THEAPPS)
endif
APPS ?= $(APP)

.PHONY: api clean_api apps clean_apps
.NOTPARALLEL: api apps

all: api apps

apps: $(APPS)

# For linux builds, link to the correct libraries
ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS := -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} -lpthread -lm
endif
ifeq ($(B_REFSW_OS),vxworks)
LDFLAGS := -lnexus -L${NEXUS_BIN_DIR}
endif

# For Android builds
ifeq ($(B_REFSW_ANDROID),y)
ANDROID_LIB = $(ANDROID)/out/target/product/bcm$(BCHP_CHIP)/obj/lib
LDFLAGS := -EL \
	-lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} \
	-Bdynamic \
	-Wl,-T,$(ANDROID)/build/core/mipself.x \
	-Wl,-dynamic-linker,/system/bin/linker \
	-Wl,--gc-sections \
	-Wl,-z,nocopyreloc \
	-Wl,--no-undefined \
	-Wl,-rpath-link=$(ANDROID_LIB) \
	-L$(ANDROID_LIB) \
	-nostdlib \
	$(ANDROID_LIB)/crtbegin_dynamic.o \
	$(ANDROID_LIB)/crtend_android.o \
	-lc -lm -lgcc
endif

# This cleans nexus and local apps
clean: clean_api clean_apps

# Clean api except on binary version
ifeq ($(NEXUS_PREBUILT_BINARY),y)
api:
clean_api:
else
api:
	$(MAKE) $(NEXUS_BUILD_OPTIONS) -C $(NEXUS_TOP)/build
clean_api:
	$(MAKE) $(NEXUS_BUILD_OPTIONS) -C $(NEXUS_TOP)/build clean
endif

clean_apps:  $(OTHER_APP_CLEANS)
	-$(RM) $(APPS) *.d *.o *.out

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

# Always build with debug
CFLAGS += -g

# append to EXTRA_OBJS for any extra source file needs, including VxWorks, DRM, etc.

# Implicit rule for building local apps
%$(APPSUFFIX): %.c $(patsubst %.o,%.c,$(EXTRA_OBJS))
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS)
	cp -f $@ ${NEXUS_BIN_DIR}

install:
	cp -f ${APPS} ${NEXUS_BIN_DIR}
