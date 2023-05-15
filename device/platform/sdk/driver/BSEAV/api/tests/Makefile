############################################################
#	  (c)2012 Broadcom Corporation
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
# $brcm_Revision: 18 $
# $brcm_Date: 9/13/12 10:54p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/tests/Makefile $
# 
# 18   9/13/12 10:54p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/13/12 12:38p mphillip
# SW7425-3739: Remove symlinks for linux/driver/build subdirectories and
# dependencies
# 
# 17   9/6/12 5:19p erickson
# SW7435-349: add standard header
# 
############################################################
# Settop API tests

# include cross-compiler definitions
include ../build/tools.mak

# Must define BSEAV before doing any Settop API includes
BSEAV = $(shell cd ../.. && ${PWD})

# Option for quiet builds
ifeq ($(VERBOSE),y)
Q_ := @
else
Q_ := @
endif


# This forces the Settop API to build in static mode only.
ifeq ($(STATIC_SETTOPAPI),y)
SHAREDLIB=no
STATICLIB=yes
LDFLAGS += -static
else
ifeq ($(SYSTEM),linux)
SHAREDLIB=yes
STATICLIB=no
endif
endif

THEAPPS = \
	allpassrec\
	alpha_per_surface\
	audio_vol\
	audio_video\
	blitanddecode\
	boot \
	bstream_set_mpeg_parameters\
	capture_avd\
	colorkey \
	decode_qam_with_graphics\
	decode_still_picture\
	dualdisplay-hd \
	dualdisplay \
	dualgraphics \
	dualpvr \
	duplicate_open\
	encode_93740 \
	format_change \
	gfx_blit1\
	graphics_async_fill\
	graphics_bandwidth\
	graphics_doublebuffering\
	graphics_fill\
	graphics_test1 \
	graphics_test2 \
	hdsdsimul\
	iodecode\
	message_groups\
	message_overflow\
	palette\
	palette_blit\
	perfomance_pvr \
	pip \
	playpump_btp\
	sds_retune\
	soft_filter_test \
	spdif \
	transport_test1\
	transport_test2\
	transport_test3\
	updown\
	vbi_cgms_write\
	vbi_read_write\
	vbi_tt_write\
	video_transition \
	vsync\

DEPRECATED_APPS := \
	decode_analog_with_graphics\
	decode_audio_downmix\
	record_then_pause\
	record_new_pid\
	vbi_atsc53\
	

ifeq ($(SYSTEM),vxworks)
# VxWorks needs a wrapper function to call main.
VXOBJS = vxworks_cmd.o
# We like to use .out for loadable objects.
APPSUFFIX = .out
APPS = $(addsuffix .out, ${THEAPPS})
else
APPS = ${THEAPPS}
endif

ifeq ($(SYSTEM),linux)
DRIVER := bcmdriver
DRIVER_CLEAN := bcmdriver-clean
DRIVER_INSTALL := bcmdriver-install
else
DRIVER :=
DRIVER_CLEAN := 
DRIVER_INSTALL :=
endif

.PHONY: api clean 

ifeq ($(APP),)
all: ${APPS}
${APPS}: api
else
all: $(APP)
$(APP): api
endif

include ../build/tools.mak

# This builds the settop api
api: $(DRIVER)
	@echo "[Build..... libsettop]"
	@${MAKE} STATICLIB=$(STATICLIB) SHAREDLIB=$(SHAREDLIB) -C ../build

# This cleans the settop api and local apps
clean: $(DRIVER_CLEAN)
	@${MAKE} -C ../build clean
	$(Q_)-${RM} $(APPS) *.d *.o *.out

# Rule to build the usermode driver
bcmdriver:
	@echo "[Build..... bcmdriver]"
	@${MAKE} -C ../../linux/driver/build

# Rule to clean the usermode driver
bcmdriver-clean:
	@${MAKE} -C ../../linux/driver/build clean

# Rule to install the usermode driver
bcmdriver-install:
	@${MAKE} -C ../../linux/driver/build install INSTALL_DIR=$(shell cd $(DESTDIR);pwd)

# This is the minimum needed to compile and link with the Settop API
include ${BSEAV}/api/include/api.mak
CFLAGS += ${BSETTOP_CFLAGS} ${B_REFSW_MAGNUM_INCLUDE_DIRS}
LDFLAGS += ${BSETTOP_LDFLAGS} $(BSETTOP_LIB)

#
# Installation: copying library and app to DESTDIR (must specify DESTDIR)
#
ifndef DESTDIR
.PHONY: install
install:
	$(error Must define DESTDIR to install)
else
.PHONY: install install_only
install: all install_only

ifeq ($(APP),)
install_only: $(DRIVER_INSTALL)
	$(Q_)${CP} ${BSETTOP_LIBS} ${APPS} ${DESTDIR}
else
install_only: $(DRIVER_INSTALL)
	$(Q_)${CP} ${BSETTOP_LIBS} $(APP) ${DESTDIR}
endif
endif

capture_avd:
	@echo "[Compile... $@]"
	$(Q_)$(CC) -o capture_avd capture_avd.c argenv.c $(CFLAGS) $(LDFLAGS) -O

# Implicit rule for building local apps
%${APPSUFFIX}: %.c $(VXOBJS)
	@echo "[Compile... $<]"
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS) -O

ifeq ($(SYSTEM),vxworks)
# Explicit rule for building vxworks wrapper app
vxworks_cmd.o: vxworks_cmd.c
	@echo "[Compile... $<]"
	$(Q_)$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) -O
endif
