############################################################
#	  (c)2003-2010 Broadcom Corporation
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
# $brcm_Revision: 2 $
# $brcm_Date: 9/6/12 11:28p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /BSEAV/lib/bcmplayer/tests/Makefile $#
# 
# 2   9/6/12 11:28p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/5/12 5:16p mphillip
# SW7425-3739: Remove BSEAV/lib symlinks
# 
# 1   6/3/10 3:54p erickson
# SW7400-2788: add testse
#
############################################################

APPS=bcmplayer_test

all: $(APPS)

#
# SetTop tree information
#
BSEAV=$(shell cd ../../..; pwd)
MAGNUM=$(shell cd $(BSEAV)/../magnum; pwd)
OS=linuxuser
ODIR=.
Q_=@

# need this for CHIP definitions
include $(BSEAV)/api/include/api.mak
include $(BSEAV)/api/build/tools.mak

CFLAGS += $(BSETTOP_CFLAGS)
LFLAGS += -lpthread
CFLAGS += -DCMDLINE_TEST
CFLAGS += -I../include -I../src -Wall -I.

BCHP_CHIP = $(CHIP)
include $(MAGNUM)/basemodules/std/bstd.inc
include $(MAGNUM)/basemodules/kni/bkni.inc
include $(MAGNUM)/basemodules/dbg/bdbg.inc
include $(MAGNUM)/basemodules/err/berr.inc
include $(MAGNUM)/basemodules/chp/bchp.inc
include $(MAGNUM)/basemodules/reg/breg.inc
include $(MAGNUM)/commonutils/lst/blst.inc
include $(MAGNUM)/commonutils/vlc/bvlc.inc

MAGNUM_DEFINES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
MAGNUM_INCLUDES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES)))
CFLAGS += $(addprefix -D,$(MAGNUM_DEFINES))
CFLAGS += $(addprefix -I,$(MAGNUM_INCLUDES))
MAGNUM_SRC := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_SOURCES)))
MAGNUM_OBJ := $(patsubst %.c,%.o,$(MAGNUM_SRC))

vpath %.c ../src

BCMPLAYER_OBJ = \
	bcmindexer.o \
	bcmindexer_vc1.o \
	bcmplayer.o

# cancel this implicit rule so we can separate compile & link stages
%: %.c

%.o: %.c
	@echo [Compile $@]
	$(Q_)$(CC) -c -o $@ $^ $(CFLAGS)

bcmplayer_test: bcmplayer_test.o $(BCMPLAYER_OBJ) $(MAGNUM_OBJ)
	@echo [Link $@]
	$(Q_)$(CC) -o $@ $^ $(LFLAGS)

clean:
	$(RM) $(APPS) *.o $(MAGNUM_OBJ)

ifeq ($(DESTDIR),)
install:
	$(error You must define DESTDIR first)
else
install:
	cp -f $(APPS) $(DESTDIR)
endif

