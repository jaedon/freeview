############################################################
#	  (c)2003-2012 Broadcom Corporation
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
# $brcm_Date: 5/10/12 4:46p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/examples/linuxkernel/Makefile $
# 
# 2   5/10/12 4:46p erickson
# SW7231-800: update
# 
# 1   5/12/11 1:16p erickson
# SW7420-1819: move linuxkernel_example into subdir and reimplement with
#  kbuild
# 
############################################################

# linux kbuild makefile

CWD := $(shell pwd)
# NEXUS_TOP must be absolute path for kbuild
NEXUS_TOP=$(CWD)/../..

# default
B_REFSW_OS ?= linuxkernel
ifneq ($(B_REFSW_OS),linuxkernel)
$(error This makefile builds a non-proxy linuxkernal example)
endif

# NOTE: $LINUX/Makefile defaults ARCH=mips. It cannot be set to mipsel-linux here.
CROSS_COMPILE ?= mipsel-linux
OBJCOPY ?= $(CROSS_COMPILE)-objcopy
CP = cp -f

NEXUS_PLATFORM ?= $(PLATFORM)
ifeq ($(NEXUS_PLATFORM),)
$(error You must define NEXUS_PLATFORM)
endif

# linuxkernel_example.c requires NEXUS_CFLAGS.
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc
NEXUS_CFLAGS := $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
# linuxkernel_example.c calls some nexus/base code, as if it was in nexus platform code.
NEXUS_CFLAGS += -I$(NEXUS_TOP)/base/include -I$(NEXUS_TOP)/base/include/$(B_REFSW_OS) -DNEXUS_BASE_OS_$(B_REFSW_OS)
NEXUS_CFLAGS += -I$(NEXUS_TOP)/platforms/$(PLATFORM)/src/$(B_REFSW_OS)

ifeq ($(LINUX),)
LINUX = /opt/brcm/linux
endif

.PHONY: api

# cp files for easier kbuild with NEXUS_BIN_DIR
all install: api
	$(CP) $(NEXUS_BIN_DIR)/libnexus.a $(NEXUS_BIN_DIR)/libmagnum.a .
	make -C $(LINUX) M=$(CWD) modules PLATFORM=$(PLATFORM) NEXUS_CFLAGS="\$(NEXUS_CFLAGS)"

clean:
	$(RM) libnexus.a libnexus.a
	make -C $(LINUX) M=$(CWD) clean

# this builds the Nexus api
# must build without server support (objdb)
api:
	$(MAKE) -C $(NEXUS_TOP)/build NEXUS_SERVER_SUPPORT=n
