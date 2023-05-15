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
# $brcm_Revision: 16 $
# $brcm_Date: 9/6/12 11:28p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/bcmplayer/utils/Makefile $
# 
# 16   9/6/12 11:28p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# 15   9/6/12 5:22p erickson
# SW7435-349: add standard header
# 
############################################################
# Linux makefile
#
# The Win32/VisualC++ project can be found in win32 subdir:
#   Old Comment: SetTop\build\bcm97031rb0\le\visualc\bcmplayerutils
#

APPS:=createindex printindex printts filterts byteswap printsc rmscprevent \
	printpes mkbtpfile printavc printvc1 muxts btp_generator 

ifdef  ${CHIP}
APPS+= b_playback_capture
endif

all: $(APPS)

#
# SetTop tree information
#
BSEAV=$(shell cd ../../..; pwd)
MAGNUM=$(shell cd $(BSEAV)/../magnum; pwd)
ROCKFORD=$(shell cd $(BSEAV)/../rockford; pwd)
OS=linuxuser
ODIR=.
Q_=@

# need this for CHIP definitions
include $(BSEAV)/api/include/api.mak
include $(BSEAV)/api/build/tools.mak

CFLAGS += $(BSETTOP_CFLAGS) $(B_REFSW_MAGNUM_INCLUDE_DIRS)
LFLAGS += -lpthread

ifeq (${B_REFSW_ARCH},i386-linux)
LFLAGS += -lrt
endif
	
CFLAGS += -DCMDLINE_TEST

CFLAGS += -I../include -I../src -Wall -I. -std=c99
#CFLAGS += -DSTORE_SEQUENCE_END_CODES

include $(BSEAV)/../magnum/basemodules/std/bstd.inc
include $(BSEAV)/../magnum/basemodules/kni/bkni.inc
include $(BSEAV)/../magnum/basemodules/dbg/bdbg.inc
include $(BSEAV)/../magnum/commonutils/vlc/bvlc.inc
include ${BSEAV}/lib/bcmplayer/bcmplayer.inc

CFLAGS += $(addprefix -I,$(BSTD_INCLUDES) $(BKNI_INCLUDES) $(BDBG_INCLUDES) $(BVLC_INCLUDES))

BCMPLAYER_SRC += $(sort ${B_NAV_INDEXER_SOURCES} ${B_NAV_PLAYER_SOURCES})

MAGNUM_SRC = \
	$(BSTD_SOURCES) \
	$(BKNI_SOURCES) \
	$(BDBG_SOURCES) \
	$(BVLC_SOURCES)

createindex: createindex.c $(BCMPLAYER_SRC) ../src/tsindexer.c $(MAGNUM_SRC)
	@echo $(CC) -o $@
	$(Q_)$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

printindex: printindex.c $(BCMPLAYER_SRC) $(MAGNUM_SRC)
	@echo $(CC) -o $@
	$(Q_)$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

mkbtpfile: mkbtpfile.c $(BCMPLAYER_SRC) $(MAGNUM_SRC)
	@echo $(CC) -o $@
	$(Q_)$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

printavc: printavc.c ts_utils.c $(MAGNUM_SRC)
	@echo $(CC) -o $@
	$(Q_)$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

NEXUS_TYPES_PATH = $(BSEAV)/../nexus/modules/core/include
NEXUS_BASE_TYPES_PATH = $(BSEAV)/../nexus/base/include/public
b_playback_capture: b_playback.c b_playback_capture.c $(MAGNUM_SRC) $(BCMPLAYER_SRC)
	@echo $(CC) -o $@
	$(Q_)$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS) -I$(NEXUS_TYPES_PATH) -I$(NEXUS_BASE_TYPES_PATH)

# default is for single-file apps w/ ts_utils
%: %.c ts_utils.c b_playback.c $(MAGNUM_SRC) $(BCMPLAYER_SRC)
	@echo $(CC) -o $@
	$(Q_)$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS) -I$(NEXUS_TYPES_PATH) -I$(NEXUS_BASE_TYPES_PATH)

clean:
	$(RM) $(APPS) *.o

ifeq ($(DESTDIR),)
install:
	$(error You must define DESTDIR first)
else
install:
	cp -f $(APPS) $(DESTDIR)
endif

