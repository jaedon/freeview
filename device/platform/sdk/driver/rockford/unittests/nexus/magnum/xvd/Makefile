############################################################
#     Copyright (c) 2003-2009, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 5/14/09 10:51a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/magnum/xvd/Makefile $
# 
# Hydra_Software_Devel/1   5/14/09 10:51a jtna
# PR54515: added xvd_app
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../../nexus; pwd)
endif

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

APPS = xvd_app

OBJDIR=obj

# We do not use platform_app.inc and platform_modules.inc in nexus/platforms/$(PLATFORM)/build
# Instead, we define our own, including only the modules we need
include ./build/platform_app.inc

# we need to build these magnum modules
BUILD_MODULES=BXVD
MAGNUM_DEFINES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
MAGNUM_CFLAGS := $(addprefix -D,$(MAGNUM_DEFINES))
CFLAGS += $(MAGNUM_CFLAGS)
EXTRA_SRC += $(foreach module, $(BUILD_MODULES), $($(module)_SOURCES))
VPATH = $(foreach module,$(BUILD_MODULES),$(dir $($(module)_SOURCES)))
vpath %.c $(VPATH)
EXTRA_OBJ = $(foreach file, $(patsubst %.c,%.o,$(EXTRA_SRC)), $(OBJDIR)/$(notdir $(file)))

.PHONY: apps clean clean_apps $(APPS)


all: nexus_install apps

apps: $(APPS)

$(OBJDIR):
	mkdir $(OBJDIR)

$(EXTRA_OBJ): $(OBJDIR)

print:
	@echo $(EXTRA_OBJ)
	@echo $(VPATH)


# This cleans nexus and local apps
clean: clean_apps
	$(MAKE) -C $(NEXUS_TOP)/build clean

clean_apps:
	-$(RM) $(APPS) $(OBJDIR)/*.d $(OBJDIR)/*.o *.out

# This is the minimum needed to compile and link with Nexus
# Note: LDFLAGS is used in os_rules.inc, and should be empty. Use APP_LDFLAGS instead
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
APP_LDFLAGS += $(NEXUS_LDFLAGS) -L$(NEXUS_TOP)/bin -lnexus -lpthread

# Always build with debug
CFLAGS += -g

# Put our own include directory first to override nexus_platform.h and nexus_platform_features.h
NEXUS_CFLAGS += -I./include/$(PLATFORM)
# Display module needs header files for video decoder
NEXUS_CFLAGS += -I$(NEXUS_TOP)/modules/video_decoder/${BCHP_CHIP}/include

# Build nexus by including nexus.inc, not by recursive invocation of Make, so we can use our own platform_modules.inc file. 
include $(NEXUS_TOP)/build/nexus.inc

# Implicit rule for building local apps
$(OBJDIR)/%.o: %.c
	@echo [Compile... $<]
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS)

xvd_app: xvd_app.c $(EXTRA_OBJ) nexus_install
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ xvd_app.c $(EXTRA_OBJ) $(CFLAGS) $(APP_LDFLAGS)

install: xvd_app
	cp xvd_app $(DESTDIR)
