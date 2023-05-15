#############################################################################
#
#       Copyright (c) 2007-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 12 $
# $brcm_Date: 6/6/12 1:35p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/app/standby/Makefile $
# 
# 12   6/6/12 1:35p gmohile
# SW7344-308 : Enable S3 warm boot
# 
# 11   7/25/11 2:31p nickh
# SW7420-2006: renamed pmlib-263x files
# 
# 10   3/10/11 5:34p gmohile
# SW7408-210 : Add active standby app
# 
# 9   3/7/11 11:03a gmohile
# SW7408-210 : Update standby app for PM2.0
# 
# 8   11/23/10 4:00p mward
# SW7125-737:  Add LDFLAGS -lm so it can build with GL_SUPPORT turned on.
# 
# 7   11/9/10 4:37p gskerl
# SW7125-237: Improved kernel version checking algorithm
# 
# 6   4/15/10 1:28p erickson
# SW3548-2714: add driver and clean_driver rules for 2.6.18
#
# 5   4/14/10 2:27p erickson
# SW3548-2714: build brcmpmdrv and bcmkni for 2.6.18
#
# 4   1/28/10 4:38p erickson
# SW7420-532: refactor nexus platform standby for linux 2.6.31
#
# 3   12/10/09 4:29p erickson
# SW7325-250: refactor app to use NEXUS_Platform_Standby,
# brcm_pm_standby. allow app to loop. faster wakeup to audio playback.
#
# 2   10/6/09 3:22p erickson
# SW7400-2511: add B_REFSW_ preix
#
# 1   10/29/08 4:07p maivu
# PR 48036: initial revision for 93549B0
#
#############################################################################
# Nexus examples

#
# Work out Kernel Version from Kernel Source's Top Level Makefile
#
K_VERSION       := $(shell grep -m1 "^VERSION" $(LINUX)/Makefile | cut -d\  -f3)
K_PATCHLEVEL    := $(shell grep -m1 "^PATCHLEVEL" $(LINUX)/Makefile | cut -d\  -f3)
K_SUBLEVEL      := $(shell grep -m1 "^SUBLEVEL" $(LINUX)/Makefile | cut -d\  -f3)
K_EXTRAVERSION  := $(shell grep -m1 "^EXTRAVERSION" $(LINUX)/Makefile | cut -d-  -f2-)
KERNEL_VERSION  := $(K_VERSION).$(K_PATCHLEVEL).$(K_SUBLEVEL)-$(K_EXTRAVERSION)

check-version = $(shell expr \( $(K_VERSION) \* 65536 + $(K_PATCHLEVEL) \* 256 + $(K_SUBLEVEL) \) \>= \( $(1) \* 65536 + $(2) \* 256 + $(3) \))


ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../nexus; pwd)
endif

NEXUS_PLATFORM ?= $(PLATFORM)
ifeq ($(NEXUS_PLATFORM),)
$(error NEXUS_PLATFORM is not defined)
endif
# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc

THEAPPS = \
	standby \
	active_standby

ifeq ($(B_REFSW_OS),vxworks)
# VxWorks needs a wrapper function to call main.
VXOBJS = vxworks_cmd.o
# We like to use .out for loadable objects.
APPSUFFIX = .out
APPS = $(addsuffix .out, $(THEAPPS))
else
APPS = $(THEAPPS)
endif

.PHONY: api clean

ifeq ($(APP),)
all: $(APPS)
$(APPS): api
else
all: $(APP)
$(APP): api
endif

# For linux builds, link to the correct libraries
ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS := -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} -lpthread -lm
endif

# For VxWorks builds, link in the nexus library.
ifeq ($(B_REFSW_OS),vxworks)
LDFLAGS := -lnexus -L${NEXUS_BIN_DIR}
endif

# This builds the Nexus api
api:
	$(MAKE) -C $(NEXUS_TOP)/build

# needed to link with nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

# needed to link with pmlib
PMLIB_PATH=$(NEXUS_TOP)/../BSEAV/lib/power_standby
CFLAGS += -I$(PMLIB_PATH)

vpath %.c $(PMLIB_PATH)

ifeq ($(call check-version,2,6,30),1)   # if kernel >= 2.6.30
EXTRA_SRCS += pmlib-263x.c
else                                    # if kernel < 2.6.30
$(error This app is not supported for kernel version below 2.6.30)
endif

# Implicit rule for building local apps
%$(APPSUFFIX): %.c $(EXTRA_SRCS) $(VXOBJS)
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS)
	cp -f $@ ethtool $(NEXUS_TOP)/bin

ifeq ($(SYSTEM),vxworks)
# Explicit rule for building vxworks wrapper app
vxworks_cmd.o: vxworks_cmd.c
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
endif

# This cleans nexus and local apps
clean: clean_apps 
	$(MAKE) -C $(NEXUS_TOP)/build clean

clean_apps:
	-$(RM) $(APPS) *.d *.o *.out;

install: all

