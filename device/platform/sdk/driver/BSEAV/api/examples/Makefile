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
# $brcm_Revision: 33 $
# $brcm_Date: 9/6/12 5:19p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/examples/Makefile $
# 
# 33   9/6/12 5:19p erickson
# SW7435-349: add standard header
# 
############################################################
# Settop API examples

# include cross-compiler definitions
include ../build/tools.mak

# Must define BSEAV before doing any Settop API includes
BSEAV = $(shell cd ../.. && ${PWD})

# Option for quiet builds
Q_=@

# This forces the Settop API to build in static mode only.
ifeq ($(STATIC_SETTOPAPI),y)
SHAREDLIB=no
STATICLIB=yes
#LDFLAGS += -static
endif

THEAPPS = \
	tune \
	oob \
	decode \
	pip \
	streamer \
	streamer_dvb \
	playback \
	playback_aes \
	playback_dvb \
	playback_ip \
	playback_pid_parser_ip \
	playpump1 \
	playpump2 \
	record \
	record_allpass \
	record_allpass_from_playback \
	record_from_playback \
	recpump1 \
	set_outputs \
	decode_switch \
	dual_decode \
	dual_decode_qam \
	dual_audio_pip \
	encode \
	message \
	pcmplay \
	window_clipping \
	vbi \
	clone \
	graphics \
	blit \
	graphics2 \
	userio \
	config \
	graphics3 \
	hdsd_single_interface \
	graphics_multires \
	crypto_kats \
	mosaic_dualsd \
	record_ip_new \
	recpump_streamer \
	hdsdsingle_pip

THEAPPS += \
	async_tune \
	decode_acquire_capture_buffer \
	graphics_bitmap \
	message_pes \
	playback_pid_parser \
	tsdma_example \
	tune656input

ifneq ($(SYSTEM),vxworks)
THEAPPS += \
	playback_rtp

ifeq ($(NETACCEL_SUPPORT),y)
THEAPPS += \
	netrecv \
	http_decode
CFLAGS += -I${BSEAV}/lib/netaccel/include/
endif
endif
# smartcard and smartcard_7038 have dependencies outside of Settop API; therefore they don't build by default.

ifeq ($(SYSTEM),vxworks)
# VxWorks needs a wrapper function to call main.
VXOBJS = vxworks_cmd.o
# We like to use .out for loadable objects.
APPSUFFIX = .out
APPS = $(addsuffix .out, ${THEAPPS})
else
APPS = ${THEAPPS}
endif

.PHONY: api clean

ifeq ($(APP),)
all: ${APPS}
${APPS}: api
else
all: $(APP)
$(APP): api
endif

# This builds the settop api
api:
	${MAKE}  STATICLIB=$(STATICLIB) SHAREDLIB=$(SHAREDLIB) -C ../build

# This cleans the settop api and local apps
clean:
	${MAKE} -C ../build clean
	-${RM} $(APPS) *.d *.o *.out;

# This is the minimum needed to compile and link with the Settop API
include ${BSEAV}/api/include/api.mak
CFLAGS += ${BSETTOP_CFLAGS}
LDFLAGS += ${BSETTOP_LDFLAGS}

# This allows the apps to use magnum basemodules like bkni.h.
# See BSEAV/build/refsw_inc.mak for this define.
CFLAGS += ${B_REFSW_MAGNUM_INCLUDE_DIRS}

# Options for smartcard app
SMARTCARD = ../../../magnum/portinginterface/scd/$(CHIP)
CFLAGS += -I${SMARTCARD}

ifneq ($(SYSTEM),vxworks)
CFLAGS += \
	-I$(SETTOP)/deviceinterface \
	-I$(BSEAV)/linux/driver/common \
	-I$(BSEAV)/linux/driver/973xx
endif

# Implicit rule for building local apps
%${APPSUFFIX}: %.c $(VXOBJS)
	@echo Compiling $<
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS)

# jpeg_slideshow has a dependency outside of Settop API; therefore it cannot build by default.
jpeg_slideshow${APPSUFFIX}: jpeg_slideshow.c $(VXOBJS) ${BSEAV}/lib/jpeg-6b/libjpeg.a
	@echo Compiling $<
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) -I${BSEAV}/lib/jpeg-6b $(LDFLAGS) ${BSEAV}/lib/jpeg-6b/libjpeg.a

ifeq ($(SYSTEM),vxworks)
# Explicit rule for building vxworks wrapper app
vxworks_cmd.o: vxworks_cmd.c
	@echo Compiling $<
	$(Q_)$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
endif

#
# Installation: copying library and app to DESTDIR
#
ifeq ($(DESTDIR),)
DESTDIR = ${BSEAV}/bin
endif

.PHONY: install install_only
install: all install_only

ifeq ($(APP),)
install_only:
	${CP} ${BSETTOP_LIBS} ${APPS} ${DESTDIR}
else
install_only:
	${CP} ${BSETTOP_LIBS} $(APP) ${DESTDIR}
endif
