############################################################
#     (c)2003-2009 Broadcom Corporation
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
# $brcm_Workfile: os_flags_nexus.inc $
# $brcm_Revision: 7 $
# $brcm_Date: 10/16/09 4:23p $
#
# Module Description:
#   This file defines the nexus OS-specific flags for building in ucos.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/ucos/os_flags_nexus.inc $
# 
# 7   10/16/09 4:23p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 6   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 5   4/27/09 11:25a cnovak
# PR 43697: Allow compiler optimization level to be configurable.
#
# 4   3/13/09 2:44p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 3   8/7/08 5:50p cnovak
# PR43697: Add path info for the bsp and kernel libraries.
#
# 2   8/4/08 1:54p cnovak
# PR43697: Change path for OBJ directory.
#
# 1   6/17/08 3:18p cnovak
# PR43697: Add uCOS to Nexus.
#
############################################################

#
# Default compile optimization to -Os by default
#
B_REFSW_OPTIMIZER_LEVEL ?= s

#
# Install binaries to nexus/platform/$(NEXUS_PLATFORM)/bin$(NEXUS_BIN_DIR_SUFFIX)/hdi4.$(B_REFSW_ARCH).debug or .release.
#
ifeq ($(B_REFSW_DEBUG),n)
NEXUS_OBJ_DIR_SUFFIX := release
NEXUS_CFLAGS += -O$(B_REFSW_OPTIMIZER_LEVEL)
else
NEXUS_OBJ_DIR_SUFFIX := debug
NEXUS_CFLAGS += -g -O$(B_REFSW_OPTIMIZER_LEVEL)
endif

NEXUS_OBJ_DIR := $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/bin$(NEXUS_BIN_DIR_SUFFIX)/$(B_REFSW_OS).$(B_REFSW_ARCH).$(NEXUS_OBJ_DIR_SUFFIX)

#
#
#
NEXUS_OBJ_SUFFIX := o
NEXUS_STATIC_LIB := $(NEXUS_OBJ_DIR)/libnexus.a
MAGNUM_LIB := $(NEXUS_OBJ_DIR)/libmagnum.a
BSP_LIB := $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/src/$(B_REFSW_OS)/libbsp.a
KERNEL_LIB := $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/src/$(B_REFSW_OS)/kernel2.a


