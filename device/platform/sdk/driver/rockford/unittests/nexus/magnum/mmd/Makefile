############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 8/30/11 5:47p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/magnum/mmd/Makefile $
# 
# Hydra_Software_Devel/1   8/30/11 5:47p jtna
# SW7405-5473: added mmd magnum/nexus hybrid unittest
# 
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../../nexus; pwd)
endif

# NEXUS_BASE_ONLY tells nexus to only build nexus/base and nexus/modules/core.
NEXUS_BASE_ONLY=y

# include cross-compiler definitions
ifeq ($(NEXUS_PREBUILT_BINARY),y)
# do "make api" and "make nexus_headers" first, then "make NEXUS_PREBUILT_BINARY=y"
include $(NEXUS_TOP)/bin/include/platform_app.inc
Q_ ?= @
else
NEXUS_PLATFORM ?= $(PLATFORM)
ifeq ($(NEXUS_PLATFORM),)
$(error NEXUS_PLATFORM is not defined)
endif
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc
endif

APPS = mmd_test
APPS ?= $(APP)

OBJDIR = obj

# The app must include or build these manually because NEXUS_BASE_ONLY does not include them
MAGNUM = $(NEXUS_TOP)/../magnum
include $(MAGNUM)/basemodules/mem/bmem.inc
include $(MAGNUM)/basemodules/int/bint.inc
include $(MAGNUM)/basemodules/tmr/btmr.inc
include $(MAGNUM)/commonutils/avc/bavc.inc
include $(MAGNUM)/commonutils/pxl/bpxl.inc
include $(MAGNUM)/commonutils/mrc/bmrc.inc
include $(MAGNUM)/commonutils/sur/bsur.inc
include $(MAGNUM)/commonutils/fmt/bfmt.inc

NEXUS_MAGNUM_MODULES := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_MAGNUM_MODULES))
include $(sort $(NEXUS_MAGNUM_MODULES))

# The following magnum modules must be built by the application
BUILD_MODULES=BMMD
include $(MAGNUM)/portinginterface/mmd/bmmd.inc
MAGNUM_INCLUDES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES)))
MAGNUM_DEFINES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
MAGNUM_CFLAGS := $(addprefix -D,$(MAGNUM_DEFINES)) $(addprefix -I,$(MAGNUM_INCLUDES))
CFLAGS += $(MAGNUM_CFLAGS)
EXTRA_SRC += $(foreach module, $(BUILD_MODULES), $($(module)_SOURCES))
VPATH = $(foreach module,$(BUILD_MODULES),$(dir $($(module)_SOURCES)))
vpath %.c $(VPATH)
EXTRA_OBJ_ = $(foreach file, $(patsubst %.c,%.o,$(EXTRA_SRC)), $(notdir $(file)))
EXTRA_OBJ = $(addprefix $(OBJDIR)/,$(EXTRA_OBJ_))

.PHONY: api clean_api apps clean_apps clean 
.NOTPARALLEL: api apps

all: api apps

apps: $(APPS)

$(OBJDIR):
	mkdir $(OBJDIR)

$(EXTRA_OBJ): $(OBJDIR)

# This cleans nexus and local apps
clean: clean_api clean_apps

# Clean api except on binary version
ifeq ($(NEXUS_PREBUILT_BINARY),y)
api:
clean_api:
else
api:
	$(MAKE) -C $(NEXUS_TOP)/build NEXUS_BASE_ONLY=y
clean_api:
	$(MAKE) -C $(NEXUS_TOP)/build clean
endif

clean_apps:
	-$(RM) $(APPS) *.d *.o *.out

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
LDFLAGS += $(NEXUS_LDFLAGS) -L$(NEXUS_TOP)/bin -lnexus -lpthread

# Always build with debug
CFLAGS += -g

# Implicit rule for building local apps
$(OBJDIR)/%.o: %.c
	@echo [Compile... $<]
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS)

mmd_test: mmd_test.c $(EXTRA_OBJ)
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ mmd_test.c $(EXTRA_OBJ) $(CFLAGS) $(LDFLAGS)
	cp -f $@ ${NEXUS_BIN_DIR}

install: all
	cp -f ${APPS} ${NEXUS_BIN_DIR}
