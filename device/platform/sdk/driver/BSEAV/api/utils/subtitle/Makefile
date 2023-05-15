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
# $brcm_Revision: 6 $
# $brcm_Date: 9/6/12 5:19p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/utils/subtitle/Makefile $
# 
# 6   9/6/12 5:19p erickson
# SW7435-349: add standard header
# 
############################################################
APP = subtitle

ifeq ($(STATIC_SETTOPAPI),y)
SHAREDLIB = no
LDFLAGS += -static
endif

ifeq ($(SYSTEM),vxworks)
BSEAV = ../../..
else
BSEAV = $(shell cd ../../..; pwd)
endif

MAGNUM = $(BSEAV)/../magnum
include ../Rules.make

CFLAGS += -I ${BSEAV}/api/src
CFLAGS += -I ${BSEAV}/api/src/$(CHIP)/board

OBJS += subtitles.o 
OBJS += subtitle_parse.o
OBJS += subtitle_init.o

include $(BSEAV)/build/refsw_inc.mak

include ${BSEAV}/lib/avi/bavi.inc
include ${BSEAV}/lib/bfile/bfile.inc
include ${BSEAV}/lib/media/bmedia.inc

ifeq ($(DIVX_DRM_SUPPORT),y)
        include $(BSEAV)/lib/divxdrm/Makefile.probe
else
TEST_MODULES :=  BMEDIA_PROBE BFILE_STDIO 
B_MODULES := $(sort ${TEST_MODULES} $(foreach module, $(TEST_MODULES), $($(module)_DEPENDS)))
B_TEST_INC := $(addprefix -I,$(sort $(foreach module, $(B_MODULES), $($(module)_INCLUDES))))
B_TEST_CFLAGS := $(addprefix -D,$(sort $(foreach module, $(B_MODULES), $($(module)_DEFINES))))
B_TEST_SOURCES = $(sort $(foreach module, $(B_MODULES), $($(module)_SOURCES)))
B_TEST_SRCS := $(notdir ${B_TEST_SOURCES})
vpath %c $(dir ${B_TEST_SOURCES})
CFLAGS += ${B_TEST_CFLAGS} ${B_TEST_INC}
SRCS += ${B_TEST_SRCS}

CFLAGS += -I ${BSEAV}/lib/tspsi
vpath %.c ${BSEAV}/lib/tspsi
SRCS += tspsi_validate.c

CFLAGS += -I ${BSEAV}/lib/mpeg2_ts_parse
vpath %.c ${BSEAV}/lib/mpeg2_ts_parse
SRCS += ts_pmt.c ts_pat.c ts_priv.c ts_psi.c
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
#$(error XXX LDFLAGS = ${LDFLAGS} XXX)
ifeq ($(SSL_SUPPORT),y)
	LDFLAGS +=  -L$(BSEAV)/lib/openssl/lib -lssl -lcrypto
endif 	
endif
endif

include ${BSETTOP}/build/rules.mak
