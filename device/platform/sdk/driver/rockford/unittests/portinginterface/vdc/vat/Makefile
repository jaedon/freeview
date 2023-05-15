############################################################
#     Copyright (c) 2009-2009, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 5/13/09 5:25p $
#
# Module Description:
#
# Revision History:
#
# Created: 03/28/2009 by Philip Truong
#
# $brcm_Log: W:/pntruong/views/pntruong_93548_devel_sview/rockford/unittests/portinginterface/vdc/vat/Makefile $
# 
# Hydra_Software_Devel/5   5/13/09 5:25p pntruong
# PR51926:  Added vbi.
# 
# Hydra_Software_Devel/4   4/24/09 2:53p pntruong
# PR51926: Fixed link error.
#
# Hydra_Software_Devel/3   4/24/09 11:25a pntruong
# PR51926: apapt to new change.
#
# Hydra_Software_Devel/2   4/23/09 11:33a pntruong
# PR51926: More stub work to make we only need bare minimum in nexus code
# to get by board initialization.
#
# Hydra_Software_Devel/1   4/16/09 9:23p pntruong
# PR51926: Using nexus_platform init for vdc unittest.  Initial version
# from nexus example.  Currently only build for 93548; need to make this
# build for all platforms supported by Nexus.
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../../nexus; pwd)
endif

APPS = vat

OBJDIR=${PLATFORM}

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

NEXUS_BASE_ONLY=y

# include these before platform_app.inc and they will get sucked in
MAGNUM= $(NEXUS_TOP)/../magnum
include $(MAGNUM)/portinginterface/vdc/bvdc.inc
include $(MAGNUM)/portinginterface/vbi/bvbi.inc
include $(MAGNUM)/commonutils/rdc/brdc.inc
include $(MAGNUM)/commonutils/fmt/bfmt.inc
include $(MAGNUM)/commonutils/sur/bsur.inc
include $(MAGNUM)/commonutils/avc/bavc.inc
include $(MAGNUM)/commonutils/pxl/bpxl.inc
include $(MAGNUM)/commonutils/mth/bmth.inc
include $(MAGNUM)/basemodules/mem/bmem.inc
include $(MAGNUM)/basemodules/int/bint.inc
include $(MAGNUM)/basemodules/tmr/btmr.inc
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

# we need to build these magnum modules
BUILD_MODULES=BVDC BRDC BVBI
MAGNUM_DEFINES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
MAGNUM_CFLAGS := $(addprefix -D,$(MAGNUM_DEFINES))
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

$(APPS): bvat_main.c $(EXTRA_OBJ)
	@echo [Compile... $<]
	$(Q_)$(CC) -o $(OBJDIR)/$@ bvat_main.c $(EXTRA_OBJ) $(CFLAGS) $(LDFLAGS)

# End of file
