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
# $brcm_Revision: Hydra_Software_Devel/7 $
# $brcm_Date: 9/29/11 2:47p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/magnum/Makefile $
# 
# Hydra_Software_Devel/7   9/29/11 2:47p erickson
# SW7425-1343: backout last change
# 
# Hydra_Software_Devel/5   4/6/10 2:47p erickson
# SWDEPRECATED-3716: get NEXUS_BASE_ONLY working again
#
# Hydra_Software_Devel/4   4/8/09 2:10p shyam
# PR53323 : Add support for defines and cleaup rules
#
# Hydra_Software_Devel/3   4/2/09 4:20p shyam
# PR53323 : Make Base build part of the make file
#
# Hydra_Software_Devel/2   3/26/09 11:26a erickson
# PR53323: rename app, add obj subdir
#
# Hydra_Software_Devel/1   3/19/09 4:58p erickson
# PR53323: add test app for calling magnum directly from app
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = vdc_app

OBJDIR=obj

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

# NEXUS_BASE_ONLY tells nexus to only build nexus/base and nexus/modules/core.
NEXUS_BASE_ONLY=y
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

# The app must include or build these manually because NEXUS_BASE_ONLY does not include them
MAGNUM= $(NEXUS_TOP)/../magnum
include $(MAGNUM)/portinginterface/vdc/bvdc.inc
include $(MAGNUM)/commonutils/rdc/brdc.inc
include $(MAGNUM)/portinginterface/pwm/bpwm.inc
include $(MAGNUM)/portinginterface/vbi/bvbi.inc
include $(MAGNUM)/syslib/vbilib/bvbilib.inc

NEXUS_MAGNUM_MODULES := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_MAGNUM_MODULES))
include $(sort $(NEXUS_MAGNUM_MODULES))

# The following magnum modules must be built by the application
BUILD_MODULES=BVDC BRDC BAVC
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
	mkdir $(OBJDIR)

print:
	@echo $(EXTRA_OBJ)
	@echo $(VPATH)

# This builds nexus
nexus:
	$(MAKE) -C $(NEXUS_TOP)/build NEXUS_BASE_ONLY=y

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
	@echo [Compile... $<]
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS)

vdc_app: vdc_app.c $(EXTRA_OBJ)
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ vdc_app.c $(EXTRA_OBJ) $(CFLAGS) $(LDFLAGS)

