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
# $brcm_Workfile: Makefile $
# $brcm_Revision: 65 $
# $brcm_Date: 10/25/12 11:58a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/examples/Makefile $
# 
# 65   10/25/12 11:58a erickson
# SW7425-4137: merge
# 
# SW7425-4137/1   10/25/12 11:32a mward
# SW7425-4137:  add cec to SUBDIRS.
# 
# 64   4/5/12 10:30a erickson
# SW7425-1969: allow NEXUS_BIN_DIR
# 
# 63   2/16/12 2:48p erickson
# SW7425-2300: remove hdmi directory
# 
# 62   5/25/11 4:48p erickson
# SW7422-430: build security examples
# 
# 61   5/25/11 10:25a erickson
# SWDEPRECATED-2425: add two apps
# 
# 60   5/25/11 10:10a erickson
# SW7340-265: use SUBDIRS variable
# 
# 59   5/13/11 9:47a erickson
# SW7340-265: prevent unnecessary rebuilds
# 
# 58   5/12/11 1:16p erickson
# SW7420-1819: move linuxkernel_example into subdir and reimplement with
#  kbuild
# 
# 57   9/29/10 9:33a erickson
# SW7420-1123: add linux usermode IPC
# 
# 56   3/22/10 5:20p ttrammel
# SW7405-3833: Modify for pre-built binary build.
# 
# 55   2/23/10 10:11a erickson
# SW7405-3833: make api a dependency of APP or APPS
#
# 54   2/23/10 9:43a vsilyaev
# SW7405-3833: Fixed building top-level examples
#
# 53   2/12/10 10:22a erickson
# SW7405-3833: add NEXUS_PREBUILT_BINARY test using nexus_headers build
#  option
#
# 52   12/11/09 2:56p mphillip
# SW7550-112: Merge 7550 changes to main branch
#
# 51   11/5/09 4:16p erickson
# SW7400-2511: add nexus_print_options passthrough
#
# 50   9/16/09 1:42p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 49   3/13/09 2:44p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 48   2/19/09 3:30p erickson
# PR51841: remove examples_lib
#
# 47   2/5/09 1:02p erickson
# PR51841: update
#
# 46   2/4/09 4:29p erickson
# PR51841: reorganize
#
# 45   2/4/09 3:50p erickson
# PR51841: refactor
#
############################################################
# Nexus examples

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd .. && cd)
else
NEXUS_TOP := $(shell cd ../; pwd)
endif

# include cross-compiler definitions
ifeq ($(NEXUS_PREBUILT_BINARY),y)
# do "make api" and "make nexus_headers" first, then "make NEXUS_PREBUILT_BINARY=y"
NEXUS_BIN_DIR ?= $(NEXUS_TOP)/bin
include $(NEXUS_BIN_DIR)/include/platform_app.inc
Q_ ?= @
else
NEXUS_PLATFORM ?= $(PLATFORM)
ifeq ($(NEXUS_PLATFORM),)
$(error NEXUS_PLATFORM is not defined)
endif
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc
endif

THEAPPS = \
	boot \
	decode \
	power_down \
	heaps

ifeq ($(SOFT_GFX_SUPPORT),y)
#include $(NEXUS_TOP)/lib/softgfx/Makefile
CFLAGS += -DB_HAS_SOFT_GFX=1
CFLAGS += -I$(NEXUS_TOP)/lib/softgfx/include
EXTRA_SOFT_GFX_OBJS := b_softgfx
THEAPPS += \
	softgfx_fill \
	softgfx_blit \
	softgfx_blended_fill \
	softgfx_framebuffer \
	softgfx_moving_blend \
	softgfx_moving_blit

endif #SOFT_GFX_SUPPORT

ifeq ($(B_REFSW_OS),vxworks)
# VxWorks needs a wrapper function to call main.
EXTRA_OBJS += vxworks_cmd.o
# We like to use .out for loadable objects.
APPSUFFIX = .out
APPS = $(addsuffix .out, $(THEAPPS))
else
APPS = $(THEAPPS)
endif
APPS ?= $(APP)

.PHONY: api clean_api apps clean_apps subdirs clean_subdirs

.NOTPARALLEL: api apps

all: api apps

apps: $(APPS) subdirs

# For linux builds, link to the correct libraries
ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS := -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} -lpthread -lm
endif

ifeq ($(SOFT_GFX_SUPPORT),y)
LDFLAGS += -l$(EXTRA_SOFT_GFX_OBJS)
endif

# For VxWorks builds, link in the nexus library.
ifeq ($(B_REFSW_OS),vxworks)
LDFLAGS := -lnexus -L${NEXUS_BIN_DIR}
endif

nexus_print_options nexus_headers:
	$(MAKE) -C $(NEXUS_TOP)/build $@

SUBDIRS = io frontend graphics video audio dvr hdmi_input hdmi_output cec lipsync multiprocess transport security

subdirs:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i apps; done

clean_subdirs:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i clean_apps; done

# This cleans nexus and local apps
clean: clean_api clean_apps

# Clean api except on binary version
ifeq ($(NEXUS_PREBUILT_BINARY),y)
api:
clean_api:
else
api:
	$(MAKE) -C $(NEXUS_TOP)/build
clean_api:
	$(MAKE) -C $(NEXUS_TOP)/build clean
endif

clean_apps: clean_subdirs
	-$(RM) $(APPS) *.d *.o *.out

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

# Always build with debug
CFLAGS += -g

# Implicit rule for building local apps
%$(APPSUFFIX): %.c $(patsubst %.o,%.c,$(EXTRA_OBJS))
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS)
	cp -f $@ ${NEXUS_BIN_DIR}


ifeq ($(B_REFSW_OS),vxworks)
# Explicit rule for building vxworks wrapper app
vxworks_cmd.o: vxworks_cmd.c
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $< $(CFLAGS)
endif

install: all
	cp -f ${APPS} ${NEXUS_BIN_DIR}
