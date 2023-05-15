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
# $brcm_Revision: 4 $
# $brcm_Date: 10/16/09 4:23p $
#
# Module Description:
#   This file defines the nexus OS-specific flags for building in vxworks.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/vxworks/os_flags_nexus.inc $
# 
# 4   10/16/09 4:23p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 3   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 2   3/13/09 2:44p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 1   5/21/08 4:44p rjlewis
# PR40352: Initial version.
#
#
############################################################

#
# Default to loadable module
#
ifndef B_REFSW_SHAREABLE
B_REFSW_SHAREABLE := y
endif

#
# Install binaries to nexus/platform/$(NEXUS_PLATFORM)/bin$(NEXUS_BIN_DIR_SUFFIX)/vxworks.$(B_REFSW_ARCH).debug or .release.
#
ifeq ($(B_REFSW_DEBUG),n)
NEXUS_OBJ_DIR_SUFFIX := release
NEXUS_CFLAGS += -Os
else
NEXUS_OBJ_DIR_SUFFIX := debug
NEXUS_CFLAGS += -g -Os
endif

NEXUS_OBJ_DIR := $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/bin$(NEXUS_BIN_DIR_SUFFIX)/vxworks.$(B_REFSW_ARCH).$(NEXUS_OBJ_DIR_SUFFIX)

#
# Allow for shared and non-shared builds.  Shared will be relocatable, non-shared will not.
#
ifeq ($(B_REFSW_SHAREABLE),y)
NEXUS_OBJ_SUFFIX := so
#NEXUS_CFLAGS += -DPIC -fpic
NEXUS_SHARED_LIB := $(NEXUS_OBJ_DIR)/libnexus.out
NEXUS_STATIC_LIB := $(NEXUS_OBJ_DIR)/libnexus.a
MAGNUM_LIB := $(NEXUS_OBJ_DIR)/libmagnum.a
else
NEXUS_OBJ_SUFFIX := o
NEXUS_STATIC_LIB := $(NEXUS_OBJ_DIR)/libnexus.a
MAGNUM_LIB := $(NEXUS_OBJ_DIR)/libmagnum.a
endif

