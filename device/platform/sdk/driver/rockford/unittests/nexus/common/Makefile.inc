############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile.inc $
# $brcm_Revision: Hydra_Software_Devel/39 $
# $brcm_Date: 9/13/12 4:16p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/common/Makefile.inc $
# 
# Hydra_Software_Devel/39   9/13/12 4:16p gskerl
# SW7346-853: Added seting of DEBUG_SUFFIX (because its needed for
# linking to mlibs libraries).
# 
# Hydra_Software_Devel/38   9/12/12 7:16p gskerl
# SW7346-853: Added support for building and linking to BSEAV/lib/mlibs
# when B_PLAYBACK_IP_XML_SUPPORT is defined (like for MPEG-DASH).
# 
# Hydra_Software_Devel/37   1/11/12 3:06p mward
# SW7125-1205:  Fix build with NETACCEL_SUPPORT=n.  Copy .chn files.
# 
# Hydra_Software_Devel/36   1/10/12 5:22p mward
# SW7125-1205: Add option BUILD_CHANNELS_FILE_UNITTESTS.
# 
# Hydra_Software_Devel/35   12/19/11 6:52p hongtaoz
# SW7425-1110: added user data callback to log the source and loopback
# decoders user data and parsed closed caption user data to verify the
# cc data transcoder;
# 
# Hydra_Software_Devel/34   11/18/11 12:28p erickson
# SW7420-1148: build client for usermode or kernelmode multiprocess
# 
# Hydra_Software_Devel/33   10/19/11 2:52p mward
# SW7125-1119:  Change DEBUG to B_REFSW_DEBUG.
# 
# Hydra_Software_Devel/32   6/3/11 10:48a erickson
# SW7425-42: encoder unittests require oslib and tshdrbuilder. rework
# Makefile so that it's explicit and local.
# 
# Hydra_Software_Devel/31   5/19/11 1:21p hongtaoz
# SW7425-42: fixed a typo causing link error for transcode_ts;
# 
# Hydra_Software_Devel/30   5/18/11 2:02p hongtaoz
# SW7425-42, SW7405-4957: add TS PSI builder library for stream_mux test;
# 
# Hydra_Software_Devel/29   3/24/11 5:11p jrubio
# SW7400-2511: use B_REFSW_ARCH instead of ARCH
# 
# Hydra_Software_Devel/28   3/23/11 4:32p erickson
# SW7420-1671: support libnexus_client.so
# 
# Hydra_Software_Devel/27   3/14/11 12:11p jtna
# SW7422-170: define ARCH and DEBUG for UNITTEST_BWIN_SUPPORT
# 
# Hydra_Software_Devel/26   3/2/11 12:28p jtna
# SW7405-4653: copy bcmnetaccel.ko to nexus/bin when building IP
# unittests
# 
# Hydra_Software_Devel/25   2/25/11 7:04p jtna
# SW7405-4653: fix broken IP playback sub unittest
# 
# Hydra_Software_Devel/24   1/5/11 3:29p jtna
# SW7422-170: add playback_svc_mvc_demo.c
# 
# Hydra_Software_Devel/23   6/23/10 3:08p ajutras
# SW35230-370: Support DVB-CI on 35230.
# 
# Hydra_Software_Devel/22   5/25/10 5:00p mward
# SW7125-253: Fixed attempted link to phony targets.  Fixed build with
# PLAYBACK_IP_SUPPORT=y LIVEMEDIA_SUPPORT=n.
# 
# Hydra_Software_Devel/21   5/24/10 12:26p katrep
# SW7125-253:Fixed link issues as EXTRA_SRC not getting compiled
# 
# Hydra_Software_Devel/20   5/12/10 11:01a erickson
# SW7125-253: do implicit rules for separate compile and link
#
# Hydra_Software_Devel/19   5/11/10 5:03p mward
# SW7125-253:  Allow build with LIVEMEDIA=n and PLAYBACK_IP_SUPPORT=y.
# Allow build with GL_SUPPORT=y.
#
# Hydra_Software_Devel/18   5/10/10 12:14p erickson
# SW7125-253: fix non-IP builds
#
# Hydra_Software_Devel/17   5/7/10 6:44p mward
# SW7125-253:  stbgcc-4.4.3-1.0 must do final link using c++ not gcc if
# LiveMedia library built with c++ is used.
#
# Hydra_Software_Devel/17   5/7/10 6:42p mward
# SW7125-253:  stbgcc-4.4.3-1.0 must do final link using c++ not gcc if
# LiveMedia library built with c++ is used.
#
# Hydra_Software_Devel/16   5/4/10 12:39p jtna
# SW7405-4289: explicitly build other library dependencies
#
# Hydra_Software_Devel/15   5/3/10 6:02p jtna
# SW7405-4289: added support for building IP playback example app
#
# Hydra_Software_Devel/14   3/5/10 12:32p erickson
# SW3556-1043: add cp
#
# Hydra_Software_Devel/13   2/4/10 2:25p mward
# SW7125-195: Eliminate extra copy of unittests in nexus/bin.
#
# Hydra_Software_Devel/12   9/21/09 2:58p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# Hydra_Software_Devel/11   4/13/09 9:38a erickson
# PR42679: copy .str files if they exist
#
# Hydra_Software_Devel/10   4/1/09 3:37p vsilyaev
# PR 53225: Use NEXUS_BIN_DIR as the binary directory
#
# Hydra_Software_Devel/9   3/31/09 7:16p vsilyaev
# PR 50887: Fixed parallel build
#
# Hydra_Software_Devel/8   3/30/09 2:34p erickson
# PR42679: cp to nexus/bin
#
# Hydra_Software_Devel/7   3/13/09 3:55p vsilyaev
# PR 53225: Added NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# Hydra_Software_Devel/6   10/23/08 3:59p erickson
# PR42679: update
#
# Hydra_Software_Devel/5   9/3/08 10:30a erickson
# PR46473: fix EXTRA_SRC
#
# Hydra_Software_Devel/4   8/27/08 7:21a erickson
# PR42789: add bluetooth unittest
#
# Hydra_Software_Devel/3   7/23/08 2:35p vsilyaev
# PR 42679: Fixed install target
#
# Hydra_Software_Devel/2   5/14/08 12:29p erickson
# PR42679: make test cruisecontrol friendly
#
# Hydra_Software_Devel/1   5/14/08 10:07a erickson
# PR42679: eliminate duplicate code in unittest Makefiles
#
############################################################

NEXUS_PLATFORM ?= $(PLATFORM)
ifeq ($(NEXUS_PLATFORM),)
$(error NEXUS_PLATFORM is not defined)
endif

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc

# include IPSTB-related variables if PLAYBACK_IP_SUPPORT is explicitly on
ifeq ($(PLAYBACK_IP_SUPPORT),y)

#needed for blive_ext.mak
ifeq ($(B_REFSW_ARCH),)
B_REFSW_ARCH=mipsel-linux
export B_REFSW_ARCH
endif
ifneq ($(B_REFSW_DEBUG),n)
B_REFSW_DEBUG=y
export B_REFSW_DEBUG
DEBUG_SUFFIX = debug
else
DEBUG_SUFFIX = release
endif

# ip_playback applib definitions
include $(NEXUS_TOP)/lib/playback_ip/b_playback_ip_lib.inc

ifneq ($(NETACCEL_SUPPORT),n)
NETACCEL_SUPPORT=y
export NETACCEL_SUPPORT
endif
ifneq ($(LIVEMEDIA_SUPPORT),n)
LIVEMEDIA_SUPPORT=y
export LIVEMEDIA_SUPPORT
endif

endif #PLAYBACK_IP_SUPPORT


ifeq ($(APPS),)
$(error Must define APPS)
endif

EXTRA_OBJS += $(patsubst %.c,%.o,$(EXTRA_SRC))

ifeq ($(BUILD_IP_UNITTESTS),y)
ifneq ($(PLAYBACK_IP_SUPPORT),y)
$(error You must set PLAYBACK_IP_SUPPORT=y to use option BUILD_IP_UNITTESTS)
endif

ifneq ($(NETACCEL_SUPPORT),n)
# copy bcmnetaccel.ko to nexus/bin
.PHONY: netaccel
all: netaccel
netaccel:
	$(MAKE) -C $(BSEAV)/lib/netaccel install INSTALL_DIR=$(NEXUS_TOP)/bin
endif

IP_STREAMER_DIR := ../ip_streamer
include $(IP_STREAMER_DIR)/Makefile.inc
vpath %c $(IP_STREAMER_DIR)
vpath %h $(IP_STREAMER_DIR)
IP_STREAMER_CFLAGS := -I$(IP_STREAMER_DIR) -DIP_STREAMER_SUBST
endif #BUILD_IP_UNITTESTS

ifeq ($(BUILD_CHANNELS_FILE_UNITTESTS),y)

CHANNELS_FILE_DIR := ../channels_file
include $(CHANNELS_FILE_DIR)/Makefile.inc
vpath %c $(CHANNELS_FILE_DIR)
vpath %h $(CHANNELS_FILE_DIR)
CHANNELS_FILE_CFLAGS := -I$(CHANNELS_FILE_DIR) -DCHANNELS_FILE_SUPPORT
endif #BUILD_CHANNELS_FILE_UNITTESTS

ifeq ($(B_REFSW_OS),vxworks)
# VxWorks needs a wrapper function to call main.
EXTRA_OBJS += vxworks_cmd.o
# We like to use .out for loadable objects.
APPSUFFIX = .out
APP_BINS = $(addsuffix .out, $(APPS))
else
APP_BINS = $(APPS)
endif

.PHONY: nexus apps clean clean_apps

all: nexus apps

.NOTPARALLEL: nexus apps

apps: $(APP_BINS)

ifeq ($(PLAYBACK_IP_SUPPORT),y)
ifeq ($(LIVEMEDIA_SUPPORT),y)
$(APP_BINS): ippblibs livemedialibs
else
$(APP_BINS): ippblibs
endif
endif

# For linux builds, link to the correct libraries
ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS := -lnexus${NEXUS_LIB_SUFFIX} -L$(NEXUS_BIN_DIR) -lpthread
endif

LDFLAGS += -lm

ifeq ($(PLAYBACK_IP_SUPPORT),y)
OS_LIB_UNITTESTS=y
LDFLAGS += -lb_playback_ip -lm

ifeq ($(B_PLAYBACK_IP_XML_SUPPORT), y)
LDFLAGS += -L$(BSEAV)/lib/mlibs/$(B_REFSW_ARCH).$(DEBUG_SUFFIX)/ -lmnet -lmcommon
endif

NEXUS_APP_INCLUDE_PATHS += \
$(NEXUS_TOP)/lib/playback_ip/include

ifeq ($(LIVEMEDIA_SUPPORT),y)
# livemedia library definitions
include $(BSEAV)/lib/livemedia/blive_ext/include/blive_ext.mak
LIVEMEDIA_DIR = $(BSEAV)/lib/livemedia/live
BLIVE_EXT_DIR = $(BSEAV)/lib/livemedia/blive_ext
LDFLAGS += -L$(LIVEMEDIA_DIR)/liveMedia -lliveMedia
LDFLAGS += -L$(LIVEMEDIA_DIR)/groupsock -lgroupsock
LDFLAGS += -L$(LIVEMEDIA_DIR)/UsageEnvironment -lUsageEnvironment
LDFLAGS += -L$(LIVEMEDIA_DIR)/BasicUsageEnvironment -lBasicUsageEnvironment
LDFLAGS += $(BLIVE_EXT_LDFLAGS)
B_LINKER := $(CXX)
# This is the minimum needed to compile and link with Nexus
C_ONLY_FLAGS := -std=c89 -std=c99 -Wstrict-prototypes
CFLAGS += $(filter-out $(C_ONLY_FLAGS),$(NEXUS_CFLAGS)) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
else
B_LINKER := $(CC)
# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
endif

else
B_LINKER := $(CC)
# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
endif # PLAYBACK_IP_SUPPORT


ifeq ($(UNITTEST_BWIN_SUPPORT),y)
ifeq ($(B_REFSW_ARCH),)
export B_REFSW_ARCH=mipsel-linux
endif
ifneq ($(DEBUG),n)
export DEBUG=y
endif

include $(BSEAV)/lib/bwin/include/bwin.mak
CFLAGS += $(BWIN_CFLAGS)
LDFLAGS += ${BWIN_LDFLAGS}

$(APP_BINS): bwin

.PHONY: bwin clean_bwin
.NOTPARALLEL: bwin

bwin:
	$(MAKE) -C $(BSEAV)/lib/bwin/build -j 1

clean_bwin:
	$(MAKE) -C $(BSEAV)/lib/bwin/build clean

clean: clean_bwin
endif


# This builds nexus
nexus:
	$(MAKE) -C $(NEXUS_TOP)/build

# This cleans nexus and local apps
clean: clean_apps
	$(MAKE) -C $(NEXUS_TOP)/build clean

# This builds playback_ip lib
ifeq ($(PLAYBACK_IP_SUPPORT),y)
.PHONY: ippblibs
ippblibs:
	$(MAKE) -C $(NEXUS_TOP)/lib/playback_ip
ifeq ($(B_PLAYBACK_IP_XML_SUPPORT), y)
	$(MAKE) -C $(BSEAV)/lib/mlibs
endif

ifeq ($(LIVEMEDIA_SUPPORT),y)
clean: clean_ippblibs clean_livemedialibs

.PHONY: livemedialibs
livemedialibs:
	$(MAKE) -C $(LIVEMEDIA_DIR) &> /dev/null
	$(MAKE) -C $(BLIVE_EXT_DIR)/build

.PHONY: clean_livemedialibs
clean_livemedialibs:
	$(MAKE) -C $(LIVEMEDIA_DIR) clean &> /dev/null
	$(MAKE) -C $(BLIVE_EXT_DIR)/build clean
else
clean: clean_ippblibs
endif # LIVEMEDIA_SUPPORT

.PHONY: clean_ippblibs
clean_ippblibs:
	$(MAKE) -C $(NEXUS_TOP)/lib/playback_ip clean
endif # PLAYBACK_IP_SUPPORT

ifeq ($(BUILD_IP_UNITTESTS),y)
ifneq ($(NETACCEL_SUPPORT),n)
clean: clean_netaccel
.PHONY: clean_netaccel
clean_netaccel:
	$(Q_)$(MAKE) -C $(BSEAV)/lib/netaccel clean
endif
endif

#
# OS lib option for unittests
#
ifeq ($(OS_LIB_UNITTESTS),y)
B_LIB_TOP := $(NEXUS_TOP)/lib
include $(NEXUS_TOP)/lib/os/b_os_lib.inc
CFLAGS += $(addprefix -I,$(B_OS_LIB_PUBLIC_INCLUDES))
LDFLAGS += -lb_os
$(APP_BINS): oslib
clean: clean_oslib
oslib:
	$(MAKE) -C $(NEXUS_TOP)/lib/os
clean_oslib:
	$(MAKE) -C $(NEXUS_TOP)/lib/os clean
endif

clean_apps: $(OTHER_APP_CLEANS)
	-$(RM) $(APP_BINS) *.d *.o *.out

# Always build with debug
CFLAGS += -g

#
# Installation: copying app to DESTDIR
#
ifeq ($(DESTDIR),)
DESTDIR = $(NEXUS_BIN_DIR)
endif

# cancel this implicit rule so we can separate compile & link stages
%: %.c

ifeq ($(BUILD_IP_UNITTESTS),y)
CFLAGS += $(IP_STREAMER_CFLAGS)
EXTRA_SRC += $(IP_STREAMER_SRC)
EXTRA_OBJ += $(patsubst %.c,%.o,$(IP_STREAMER_SRC)
endif

#
# kludge: UDP parser is needed to parse the raw user data from nexus API for cc test purpose.
# This part is to compile UDP of magnum and link with transcode unittests.
ifeq ($(BUILD_TRANSCODE_USERDATA),y)
include $(MAGNUM)/commonutils/udp/budp.inc
include $(MAGNUM)/commonutils/avc/bavc.inc
include $(MAGNUM)/commonutils/sur/bsur.inc
include $(MAGNUM)/commonutils/pxl/bpxl.inc
include $(MAGNUM)/commonutils/fmt/bfmt.inc
include $(MAGNUM)/basemodules/mem/bmem.inc
CFLAGS += $(addprefix -I,$(BUDP_INCLUDES))
CFLAGS += $(addprefix -I,$(BAVC_INCLUDES))
CFLAGS += $(addprefix -I,$(BSUR_INCLUDES))
CFLAGS += $(addprefix -I,$(BPXL_INCLUDES))
CFLAGS += $(addprefix -I,$(BFMT_INCLUDES))
CFLAGS += $(addprefix -I,$(BMEM_INCLUDES))
EXTRA_SRC += $(BUDP_SOURCES)
endif

ifeq ($(BUILD_CHANNELS_FILE_UNITTESTS),y)
CFLAGS += $(CHANNELS_FILE_CFLAGS)
EXTRA_SRC += $(CHANNELS_FILE_SRC)
EXTRA_OBJ += $(patsubst %.c,%.o,$(CHANNELS_FILE_SRC)
endif

# Implicit rule for building local apps
%$(APPSUFFIX) : %.o $(patsubst %.c,%.o,$(EXTRA_SRC))
	@echo [Link... $<]
	$(Q_)$(B_LINKER) -o $@ $@.o $(LDFLAGS) $(EXTRA_OBJS)
	$(CP) $@ $(DESTDIR)

%.o: %.c 
	@echo [Compile... $<]
	$(Q_)$(CC) -c -o $@ $^ $(CFLAGS)


ifeq ($(SYSTEM),vxworks)
# Explicit rule for building vxworks wrapper app
vxworks_cmd.o: vxworks_cmd.c
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
endif

.PHONY: install install_only
install: all install_only

APP_STREAMS = $(addsuffix .str,$(APP_BINS))

APP_CHANNELS = $(addsuffix .chn,$(APP_BINS))

install_only:
	$(CP) $(APP_BINS) $(wildcard $(APP_STREAMS)) $(wildcard $(APP_CHANNELS)) $(DESTDIR)

print_files:
	@echo $(APP_BINS)

