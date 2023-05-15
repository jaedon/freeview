############################################################
#     (c)2008-2011 Broadcom Corporation
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
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
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
# $brcm_Date: 2/8/12 1:05p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/playbackdevice/Makefile $
# 
# 2   2/8/12 1:05p gayatriv
# SW7425-2243: updating playbackdevice after verifying in BSEAV directory
# 
# 2   8/16/11 4:46p mstoutch
# SWDTV-8265: Initial porting to the AppLibs@@/main the playbackdevice
#  library and example application. The DEBUG option is not quite
#  supported yet and macrovision support is not implemented in the
#  playbackdevice library
# 
# 1   8/16/11 2:24p cdisc
# SW7425-1114: adding playback device
# 
# 2   3/17/09 4:01p ajagmag
# Modify nexus manager for interaction with new flashrunner lib.
# 
# 1   3/10/09 4:02p ajagmag
# Add FlashLite to CARD vob
#
#
############################################################


# Directory Paths
ifeq ($(PLAYBACKDEVICE_STAND_ALONE_APPLICATION),y)
NEXUSMGR_SRC_PATH:= ${PWD}/nexusMgr
endif 
NEXUSMGR_LIBDIR:= ${PWD}/bin
EDK_LIBDIR:= ${PWD}/bin
#include $(NEXUS)/platforms/$(PLATFORM)/build/platform_app.inc
#CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))   -DPIC -fpic
CFLAGS += -I $(NEXUS)/lib/os/include
CFLAGS += -I $(NEXUS_MGR_DIR)/nexusMgr.h


all: nexusMgr playbackdevice

clean: lib-clean

export NEXUSMGR_SRC_PATH
export NEXUSMGR_LIBDIR
export EDK_LIBDIR

lib-environment:
	@if [ -z "${PLATFORM}"  ] || \
#	   [ -z "${BCHP_CHIP}"  ] || \
	   [ -z "${BCHP_VER}"  ] || \
#	   [ -z "${DEBUG}"  ] || \
	   [ -z "${TOOLCHAIN_ROOT}"  ] || \
	   [ -z "${SC_PLATFORM}"  ] || \
	   [ -z "${NEXUS}"  ]; then \
		echo ; echo ;\
		echo "I detected that one of the following environment variables is missing:"; \
		echo "PLATFORM"=${PLATFORM}; \
#		echo "BCHP_CHIP"=${BCHP_CHIP}; \
		echo "BCHP_VER"=${BCHP_VER}; \
#		echo "DEBUG"=${DEBUG}; \
		echo "NEXUS"=${NEXUS}; \
		echo "TOOLCHAIN_ROOT"=${TOOLCHAIN_ROOT}; \
		echo "SC_PLATFORM"=${SC_PLATFORM}; \
		echo "You should define ALL of these if you expect this build to work"; \
		exit 1; \
	fi
	@if ! which mipsel-linux-gcc ; then \
		echo "path ="${PATH};\
		echo ; echo ;\
		echo "I detected your path variable does not have a mipsel-linux-gcc available"; \
		echo "You should add the toolchain bin path to your PATH variable"; \
		echo "start compiling.........";\
		exit 1; \
	fi;

.PHONY: nexusMgr playbackdevice

ifeq ($(PLAYBACKDEVICE_STAND_ALONE_APPLICATION),y)
nexusMgr: lib-environment
	${MAKE} -C ${PWD}/nexusMgr
endif 
playbackdevice: lib-environment
	${MAKE} ${CFLAGS} -C ${PWD}/src

lib-clean: clean_playbackdevice clean_nexusmgr
	@echo "lib-clean done ..."

clean_playbackdevice:
	@echo "clean_playbackdevice: in progress"


	${MAKE} ${CFLAGS} -C ${PWD}/src clean

clean_nexusmgr:
	@echo "clean_nexusmgr: in progress"
	${MAKE} -C ${PWD}/nexusMgr clean

