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
# $brcm_Date: 10/9/09 5:47p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Roy Lewis
#
# $brcm_Log: /rockford/unittests/nexus/magnum/1394/Makefile $
# 
# Hydra_Software_Devel/1   10/9/09 5:47p rjlewis
# SW7420-380: Initial release.
# 
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../../nexus; pwd)
endif

APPS = 1394_t \
       1394_r 

APPSUFFIX = 

OBJDIR=obj

ifndef PLATFORM
$(error PLATFORM is not defined)
endif


include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

MAGNUM= $(NEXUS_TOP)/../magnum

include $(MAGNUM)/commonutils/fmt/bfmt.inc
include $(MAGNUM)/commonutils/sur/bsur.inc
include $(MAGNUM)/commonutils/avc/bavc.inc
include $(MAGNUM)/commonutils/pxl/bpxl.inc
include $(MAGNUM)/commonutils/mth/bmth.inc
include $(MAGNUM)/commonutils/mrc/bmrc.inc
include $(MAGNUM)/basemodules/mem/bmem.inc
include $(MAGNUM)/basemodules/int/bint.inc
include $(MAGNUM)/basemodules/tmr/btmr.inc
include tnf/btnf.inc

# we need to build these magnum modules
BUILD_MODULES=BTNF
MAGNUM_DEFINES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
MAGNUM_INCLUDES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES)))
MAGNUM_CFLAGS := $(addprefix -D,$(MAGNUM_DEFINES)) $(addprefix -I,$(MAGNUM_INCLUDES))

CFLAGS += $(MAGNUM_CFLAGS)

EXTRA_SRC += $(foreach module, $(BUILD_MODULES), $($(module)_SOURCES))
VPATH = $(foreach module,$(BUILD_MODULES),$(dir $($(module)_SOURCES)))
vpath %.c $(VPATH)
EXTRA_OBJ = $(foreach file, $(patsubst %.c,%.o,$(EXTRA_SRC)), $(OBJDIR)/$(notdir $(file)))

.PHONY: nexus apps clean clean_apps $(APPS)

all: nexus apps

apps: $(OBJDIR) $(APPS)

$(OBJDIR):
	mkdir -p $@

print:
	@echo $(EXTRA_OBJ)
	@echo $(VPATH)

# This builds nexus
nexus:
	$(MAKE) -C $(NEXUS_TOP)/build 

# This cleans nexus and local apps
clean: clean_apps
	$(MAKE) -C $(NEXUS_TOP)/build clean

clean_apps:
	-$(RM) $(APPS) $(OBJDIR)/*.d $(OBJDIR)/*.o *.out

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
LDFLAGS += $(NEXUS_LDFLAGS) -L$(NEXUS_TOP)/bin -lnexus -lpthread

# Always build with debug
CFLAGS += -g
Q_=@

# Implicit rule for building local apps
$(OBJDIR)/%.o: %.c
	@echo [Compile... $(notdir $<)]
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS)

1394_t: 1394_t.c $(EXTRA_OBJ)
	@echo [Compile... $(notdir $<)]
	$(Q_)$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

1394_r: 1394_r.c $(EXTRA_OBJ)
	@echo [Compile... $(notdir $<)]
	$(Q_)$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

