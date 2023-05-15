############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/17 $
# $brcm_Date: 2/20/12 1:23p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/Makefile $
# 
# Hydra_Software_Devel/17   2/20/12 1:23p jessem
# SW7425-2211: Added back security support for 7425B2.
#
# Hydra_Software_Devel/16   2/17/12 4:24p jessem
# SW7425-2211: Reverted back to previous version due to missing
# HSM/SECURITY  labels.
#
# Hydra_Software_Devel/15   2/17/12 11:59a jessem
# SW7425-2211: Added back security support for 7425B2.
#
# Hydra_Software_Devel/14   2/6/12 3:37p jessem
# SW7425-2211: Temporarily security from the 7425 build until the 7425B2
# HSM symlinks are created.
#
# Hydra_Software_Devel/13   1/13/12 8:15p erickson
# SW7125-1205: merge
#
# Hydra_Software_Devel/SW7125-1205/1   1/11/12 2:53p mward
# SW7125-1205:  Include platform_app.inc to set NEXUS_BIN_DIR.
#
# Hydra_Software_Devel/12   11/15/11 11:31a erickson
# SWDEPRECATED-3044: use NEXUS_BIN_DIR, readd dma, don't re-make nexus
# for each subdir
#
# Hydra_Software_Devel/11   4/16/10 2:13p erickson
# SWDEPRECATED-3044: fail build of 'all' if any fail
#
# Hydra_Software_Devel/10   2/8/10 5:02p mward
# SW7125-195: make sure run_autotests is executable.
#
# Hydra_Software_Devel/9   2/4/10 2:33p mward
# SW7125-195: Allow DESTDIR to be nexus/bin, and default to that if it's
# not given.
#
# Hydra_Software_Devel/8   5/6/09 10:40a erickson
# PR42679: remove debug dir from automated test
#
# Hydra_Software_Devel/7   5/5/09 3:04p erickson
# PR42679: explicit subdir list
#
# Hydra_Software_Devel/6   4/6/09 2:20p erickson
# PR42679: update
#
# Hydra_Software_Devel/5   3/13/09 3:55p vsilyaev
# PR 53225: Added NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# Hydra_Software_Devel/4   10/24/08 12:48p erickson
# PR42679: exclude README
#
# Hydra_Software_Devel/3   10/23/08 3:59p erickson
# PR42679: update
#
# Hydra_Software_Devel/2   5/14/08 10:07a erickson
# PR42679: eliminate duplicate code in unittest Makefiles
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../nexus; pwd)
endif

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

DESTDIR ?= $(NEXUS_BIN_DIR)

# apps in debug are designed to assert, so they are not candidates for test automation
DIRLIST = \
	audio \
	display \
	dma \
	dvb_ci \
	file \
	frontend \
	general \
	graphics2d \
	hdmi_input \
	io \
	open_tests \
	playback \
	pwm \
	record \
	security \
	surface \
	transport \
	video_decoder

ifneq ($(PLATFORM),97425)
DIRLIST += security
endif

UNITTEST_DESTDIR = $(DESTDIR)/unittests

all:
	# build nexus once
	make -C $(NEXUS_TOP)/build
	# then build the apps without rebuilding nexus every time
	@for dir in ${DIRLIST}; do \
	    echo [cd $$dir]; \
		mkdir -p $(UNITTEST_DESTDIR)/$$dir; \
	    make DESTDIR=$(UNITTEST_DESTDIR)/$$dir -sC $$dir apps || exit; \
	done

clean clean_apps apps:
	@for dir in ${DIRLIST}; do \
	    echo [$$dir] $@; \
	    make -sC $$dir $@; \
	done

.PHONY: build_script

install install_only: build_script
	@for dir in ${DIRLIST}; do \
	    echo [$$dir] $@; \
		mkdir -p $(UNITTEST_DESTDIR)/$$dir; \
	    make DESTDIR=$(UNITTEST_DESTDIR)/$$dir -sC $$dir $@; \
	done
ifneq ($(DESTDIR),$(NEXUS_BIN_DIR))
	cp -f $(NEXUS_BIN_DIR)/* $(DESTDIR)
endif
	cp -f common/unittests.txt common/run_unittests common/auto_unittests.txt $(DESTDIR)
	@sed 's/unittests.txt/auto_unittests.txt/g' $(DESTDIR)/run_unittests > $(DESTDIR)/run_autotests
	@chmod +x $(DESTDIR)/run_autotests

build_script:
	@for dir in ${DIRLIST}; do \
	    for file in `make -sC $$dir print_files`; do \
	        echo unittests/$$dir/$$file; \
	    done \
	done >common/unittests.txt
