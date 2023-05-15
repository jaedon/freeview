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
# $brcm_Revision: 34 $
# $brcm_Date: 9/6/12 5:19p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/utils/userio/Makefile $
# 
# 34   9/6/12 5:19p erickson
# SW7435-349: add standard header
# 
############################################################
APP = userio

ifeq ($(filter y,$(STATIC_SETTOPAPI) $(STATIC_BRUTUS)),y)
SHAREDLIB=no
endif

NO_THREAD = yes

ifeq ($(SYSTEM),vxworks)
BSEAV = ../../..
else
BSEAV = $(shell cd ../../..; pwd)
endif

include ../Rules.make

LDFLAGS += ${BSETTOP_PVR_LDFLAGS}

OBJS += userio.o

#
# WINDOWS MEDIA (WMA, WMV, ASF) SUPPORT
#
ifeq ($(ASF_SUPPORT), y)
# fail for non-ASF platforms
ifeq ($(findstring $(PLATFORM),97398 97400 97401 97403 97405 97205 97466 97325 97335 97336 97018 97018RNG 97118 97118RNG 97458 97455 97456 97459 93549 93556 97420 97410 97409 93380vms 97340 97342 97125 97025 97119 97019 97116 97408 97468 97208 97550 97422 97425 97435 97344 97346 97231 97230 97418 97358 97552 97429 97428 97241),)
$(error ASF_SUPPORT not supported on this platform)
ifeq ($(PLATFORM), 97398)
ifeq ($(findstring $(BCHP_7411_VER),D0 E0),)
$(error ASF_SUPPORT not supported on this revision of 7411)
endif
endif
endif

#
# WM DRM
#
ifeq ($(DRM_SUPPORT), y)
ifneq ($(shell test -d $(BSEAV)/lib/drm/wmdrmpd10 && echo y), y)
$(error Error, no DRM libraries found! please contact Broadcom for DRM licensing details)
endif
ifeq (${SYSTEM},linuxkernel)
$(error Error, DRM not supported in kernel mode settop API)
endif

include $(BSEAV)/build/refsw_inc.mak

CFLAGS += -I$(BSEAV)/lib/utils
CFLAGS += -I$(BSEAV)/lib/asf -DB_HAS_ASF=1
CFLAGS += -I$(BSEAV)/lib/drm -DB_HAS_DRM=1
CFLAGS += -I$(BSEAV)/lib/media

vpath %.c $(BSEAV)/lib/drm
vpath %.c $(BSEAV)/lib/asf

DRM_BASE_DIR    = $(BSEAV)/lib/drm/wmdrmpd10
DRM_BIN_DIR     = $(DRM_BASE_DIR)/bin
DRM_REFCODE_INC = $(DRM_BASE_DIR)/inc

CFLAGS  += -I$(DRM_REFCODE_INC) -UDRM_NO_NATIVE_64_TYPES
CFLAGS  += -W -DDRM_SUPPORT_DLA=1 -D_DEBUG -DDEBUG -O3 -std=c99 -fshort-wchar

SRCS +=                         \
        drmcore.c               \
        drmplay.c               \
        drmchallenge.c          \
        drmresponse.c           \
        drmenv.c                \
        drmdecryptor.c          \
        drmhttp.c               \
        asfpars.c
endif
#
#WM DRM ND
#
ifeq ($(DRMND_SUPPORT), y)

ifneq ($(shell test -d $(BSEAV)/lib/drmnd/wmdrmnd10 && echo y), y)
$(error Error, no DRM libraries found! please contact Broadcom for DRM licensing details)
endif
ifeq (${SYSTEM},linuxkernel)
$(error Error, DRM-ND not supported in kernel mode settop API)
endif
LDFLAGS += $(BSEAV)/lib/drmnd/bin/avbrowse.a
endif

endif

ifeq ($(PLAYBACK_IP_SUPPORT),y)
	CFLAGS += -DB_HAS_IP=1
	LDFLAGS += -L$(BSEAV)/../nexus/bin -lb_playback_ip
ifeq ($(DTCP_IP_SUPPORT),y)
	CFLAGS += -DB_HAS_DTCP_IP=1
	LDFLAGS += -L$(BSEAV)/lib/security/bcrypt/lib -lbcrypt
	LDFLAGS += -L$(BSEAV)/../nexus/bin -lnexus
	LDFLAGS += -L$(BSEAV)/../nexus/bin -lb_dtcp_ip
ifeq ($(DTCP_IP_COMMON_DRM_SUPPORT),y)	
	LDFLAGS += -L$(BSEAV)/lib/security/common_drm/lib/$(CHIP) -lcmndrm
	LDFLAGS += -L$(BSEAV)/lib/drmrootfs -ldrmrootfs
endif
ifeq ($(SSL_SUPPORT),y)
	LDFLAGS +=  -L$(BSEAV)/lib/openssl/lib -lssl -lcrypto
endif 	
endif
endif

example:
	@echo 'userio'

include ${BSETTOP}/build/rules.mak

