############################################################
#     (c)2011-2012 Broadcom Corporation
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
# $brcm_Date: 8/23/12 10:12a $
#
# Module Description:
#   This file defines the rules to build the 97400 platform
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/platforms/common/build/nfe_test/Makefile $
# 
# 2   8/23/12 10:12a erickson
# SW7435-235: cp instead of symlink
# 
# 1   6/1/11 4:28p erickson
# SW7420-1819: add nfe_test
#
############################################################

CWD := $(shell pwd)

# NFE driver has a separate Makefile which uses kbuild
# the driver has the minimal amount of source code to interface with the nexus firmware and be portable

# NOTE: $LINUX/Makefile defaults ARCH=mips. It cannot be set to mipsel-linux here.
CROSS_COMPILE ?= mipsel-linux
CP = cp -f

NEXUS_PLATFORM ?= $(PLATFORM)
LINUX ?= /opt/brcm/linux
NEXUS_TOP=$(CWD)/../../../..

all:
	$(CP) -p $(NEXUS_TOP)/bin/libmagnum.a .
	$(CP) -p $(NEXUS_TOP)/bin/libnexus.a .
	$(CP) -p $(NEXUS_TOP)/build/nfe_driver/b_bare_os.c .
	$(CP) -p $(NEXUS_TOP)/../magnum/basemodules/kni/bare/b_bare_os.h .
	$(CP) -p $(NEXUS_TOP)/build/nfe_driver/nexus_driver.c .
	$(CP) -p $(NEXUS_TOP)/build/nfe_driver/nexus_driver_firmware.h .
	$(CP) -p $(NEXUS_TOP)/platforms/common/src/linuxkernel/nexus_generic_driver.h .
	make -C $(LINUX) M=$(CWD) modules PLATFORM=$(NEXUS_PLATFORM)
	$(CP) nexus.ko $(NEXUS_TOP)/bin

clean:
	make -C $(LINUX) M=$(CWD) clean

ifeq ($(INSTALL_DIR),)
install:
	$(error INSTALL_DIR is undefined)
else
install: all
	@echo "[Install... nexus.ko]"
	${Q_}$(CP) nexus.ko $(INSTALL_DIR)
endif
