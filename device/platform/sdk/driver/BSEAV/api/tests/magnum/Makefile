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
# $brcm_Revision: 9 $
# $brcm_Date: 9/6/12 5:19p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/tests/magnum/Makefile $
# 
# 9   9/6/12 5:19p erickson
# SW7435-349: add standard header
# 
############################################################
# Must define BSEAV before doing any Settop API includes
include ../../build/tools.mak
BSEAV=$(shell cd ../../.. && ${PWD})
B_REFSW_DEBUG ?= y

# These are the default apps
# Because of the implicit build rules, you can build any single-file application you
# like without modifying this Makefile.
# For instance, "make mytest" will build "mytest.c" into an app call "mytest".
APPS = test1 test3 test_bsp task_priority

.PHONY: api
all: ${APPS} 

${APPS}: api

clean:
	${MAKE} -C ${BSEAV}/api/build clean
	${RM} $(APPS) *.d *.o

# Build Settop API with static libs only.
# If you've already built with shared libs, you'll need to make clean, then make.
# tools.mak is required for cross-compiler
include ${BSEAV}/api/build/tools.mak
# api.mak is required for public Settop API
include ${BSEAV}/api/include/api.mak
# system-specific Makefile.core is required for internal Settop API
include ${BSEAV}/api/build/magnum/Makefile.inc

include $(ROCKFORD)/inc/compose.mak
CFLAGS += ${MAGNUM_INC} 
CFLAGS += $(addprefix -I, $(sort ${R_INC}))

LDFLAGS +=${BSETTOP_LDFLAGS} $(BSETTOP_LIB)

Q_=@

%: %.c
	@echo $(CC) -o $@ $(filter %.c %.s %.o, $^) 
	${Q_}$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS) 

