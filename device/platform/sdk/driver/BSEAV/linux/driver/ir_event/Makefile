############################################################
#     (c)2010 Broadcom Corporation
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
# $brcm_Revision: 3 $
# $brcm_Date: 4/24/12 5:13p $
#
# Module Description:
#   This file is the makefile for the IR remote NEXUS to Linux input device driver
#
# Revision History:
#
# $brcm_Log: /BSEAV/linux/driver/ir_event/Makefile $
# 
# 3   4/24/12 5:13p ttrammel
# SWANDROID-70: Add support for 7435.
# 
# 2   2/18/11 11:01a ttrammel
# SW7420-1506: Remove hard coded platform and chip number.
# 
############################################################
NEXUS_TOP=../../../../nexus


LINUX_DIR?=${LINUX}


ifndef PLATFORM
$(error PLATFORM is not defined)
endif

OS = linuxkernel

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc


TARGET = nexus_ir_input_event
DRIVERS = $(TARGET).ko 


all: ${DRIVERS}
	cp ${DRIVERS} $(NEXUS_TOP)/bin 

LDFLAGS := ${NEXUS_LDFLAGS}
CFLAGS += -g -Os $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

OBJS += nexus_ir_input_event.o \
	nexus_ir_input_event.mod.o



%.ko: $(OBJS)
	@echo [Linking... $(notdir $@)]
	${Q_}$(LD) ${LDFLAGS} --strip-debug -r $(patsubst %.c, %.o, $(filter %.c %.s %.o, $^)) -o $@

%.o: %.c
	@echo [Compiling... $(notdir $@)]
	$(Q_)$(CC) $(CFLAGS) -c $(filter %.c %.s %.o, $^) -o $@

clean:
	-$(RM) $(DRIVERS) *.d *.o
