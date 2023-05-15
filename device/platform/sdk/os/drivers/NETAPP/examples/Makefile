############################################################################
#    (c)2006-2012 Broadcom Corporation
# 
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
#  
# Except as expressly set forth in the Authorized License,
#  
# 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#  
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
# USE OR PERFORMANCE OF THE SOFTWARE.
# 
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
# ANY LIMITED REMEDY.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 6 $
# $brcm_Date: 2/3/12 4:18p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /NetApp/examples/Makefile $
# 
# 6   2/3/12 4:18p steven
# SW7231-638: NetApp Version 6.2
# 
# 2   12/7/11 2:08p steven
# SW7231-514: Improvements to NetApp build to build standalone
# 
# 1   11/24/11 5:13p steven
# SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
# 
# SW7425-1404/4   11/24/11 4:02p steven
# SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
#
############################################################################
#
# DESCRIPTION:
#   Makefile for building NetApp test application
# 
# In order to make this test application work, you need to define a TARGET
# variable that points to the location of the built netapp so this makefile
# can extract the netapp.inc.
#
# IMPORTANT: NetApp depends on OSAPI that is built using g++ so you must link
# your app using g++, if this is a problem let me know.
############################################################################
ifndef NETAPP_OUTPUT_DIR
NETAPP_OUTPUT_DIR ?= $(CURDIR)/../stage/usr/local
endif
include ${NETAPP_OUTPUT_DIR}/include/netapp.inc

ifeq (${VERBOSE},y)
Q_=
else
Q_=@
endif

F_SRCS 		= $(wildcard *.c)
CFLAGS 		+= ${NETAPP_CFLAGS}
LDFLAGS 	+= ${NETAPP_LDFLAGS}

NEXUS_TOP = ${shell pwd}/../../../../nexus
NEXUS_INC:=$(wildcard ${NEXUS_TOP}/platforms/$(PLATFORM)/build/platform_app.inc)
ifneq (${NEXUS_INC},) 
include ${NEXUS_INC}
NEXUS_BIN_DIR 	?=$(NEXUS_TOP)/bin
NEXUS_C_STD=c99
NEXUS_C_WARNING=-wall
NEXUS_CFLAGS := $(subst -Wstrict-prototypes,,${NEXUS_CFLAGS})
NEXUS_CFLAGS := $(subst -pedantic,,${NEXUS_CFLAGS})
CFLAGS 			+= -DHAS_NEXUS $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
LDFLAGS 		+= -lnexus${NEXUS_LIB_SUFFIX} -L${NEXUS_BIN_DIR} -lpthread -lm
else
EXCLUDED_SRCS = NexusBTAvTester.c
endif

F_SRCS 		:= $(filter-out ${EXCLUDED_SRCS}, ${F_SRCS})
F_OUT  		= $(patsubst %.c,%,${F_SRCS})
F_OBJS 		= $(patsubst %.c,%.o, $(patsubst %.cpp,%.o, ${F_SRCS}))
	
install: ${F_OUT}
	install ${F_OUT} ${NETAPP_OUTPUT_DIR}/bin

%.o : %.c
	@printf "[${CC}] <------------- %s \n" $<
	${Q_} ${CC} ${CFLAGS} -c $<  -o $@

${F_OUT}: ${F_OBJS}
	@printf "[${LD}] <------------- %s \n" $@
	${Q_}${CXX} -o $@ -I. ${@}.o ${LDFLAGS} 

clean:
	${Q_}rm -rf ${F_OUT} *.o

.PHONY: install clean check
	
