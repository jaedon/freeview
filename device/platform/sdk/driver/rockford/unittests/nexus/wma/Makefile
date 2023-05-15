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
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/6/12 5:15p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/wma/Makefile $
# 
# Hydra_Software_Devel/2   9/6/12 5:15p erickson
# SW7435-349: add standard header
# 
############################################################


ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif


ifndef PLATFORM
$(error PLATFORM is not defined)
endif


THEAPPS = \
        wma_decode_capture \
	        

ifeq ($(B_REFSW_OS),linuxkernel)
APPSUFFIX = .ko
APPS = $(addsuffix ${APPSUFFIX}, $(THEAPPS))
else
APPS = $(THEAPPS)
endif

.PHONY: api clean
OBJDIR=./obj

all: $(APPS)

$(OBJDIR)/exists:
	mkdir -p $(OBJDIR)
	@touch $@

ifneq ($(NEXUS_PREBUILT_BINARY),y)
$(APPS): api
endif

ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS := -lnexus -L${NEXUS_BIN_DIR} -lpthread -lm
endif


# This builds the Nexus api
api:
	$(MAKE) -C $(NEXUS_TOP)/build

# This cleans nexus and local apps
clean: clean_apps
	$(MAKE) -C $(NEXUS_TOP)/build clean

clean_apps:
	-$(RM) -r $(APPS) $(OBJDIR)

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

# Always build with debug
CFLAGS += -g

# media probe (only needed for playback and record)
BSEAV = $(NEXUS_TOP)/../BSEAV
include $(BSEAV)/lib/bfile/bfile.inc
ifeq ($(MEDIA_AVI_SUPPORT),y)
include $(BSEAV)/lib/avi/bavi.inc
endif
ifeq ($(MEDIA_ASF_SUPPORT),y)
include $(BSEAV)/lib/asf/basf.inc
endif
include $(BSEAV)/lib/media/bmedia.inc
CFLAGS += $(addprefix -I,$(BMEDIA_INCLUDES) $(BFILE_MEDIA_INCLUDES)) -I$(BSEAV)/lib/mpeg2_ts_parse -I$(BSEAV)/lib/tspsi
CFLAGS += $(addprefix -D,$(BMEDIA_PROBE_DEFINES))

DECODE_SRCS = ./cmdline_args.c ./fileio_custom.c

DECODE_OBJS = $(foreach file, $(DECODE_SRCS), $(OBJDIR)/$(notdir $(patsubst %.c, %.o, $(file))))

vpath %.c $(foreach file, $(DECODE_SRCS) $(MESSAGE_SRCS), $(dir $(patsubst %.c, %.o, $(file))))

test:
	@echo $(EXTRA_OBJS)
	@echo $(foreach file, $(EXTRA_SRCS), $(dir $(patsubst %.c, %.o, $(file))))

$(OBJDIR)/%.o: %.c ${OBJDIR}/exists
	@echo [Compile... $<]
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS)

# cancel this implicit rule so we can do separate compile & link w/ implicit rules
%: %.c

%: $(OBJDIR)/%.o $(DECODE_OBJS) $(VXOBJS)
	@echo [Link... $@]
ifeq ($(B_REFSW_OS),linuxkernel)
	${Q_}$(LD) ${LDFLAGS} --strip-debug -r $(filter %.o, $^) ${NEXUS_BIN_DIR}/libnexus.a -o $@
else
	$(Q_)$(CC) -o $@ $(filter %.o, $^) $(LDFLAGS)
endif
	cp $@ ${NEXUS_BIN_DIR}


install: all

include ../common/Makefile.inc


