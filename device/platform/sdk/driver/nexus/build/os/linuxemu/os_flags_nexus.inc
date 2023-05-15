############################################################
#     (c)2003-2012 Broadcom Corporation
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
# $brcm_Revision: 1 $
# $brcm_Date: 8/20/12 10:02a $
#
# Module Description:
#   This file defines the NEXUS OS-specific flags for building in linuxuser mode.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/linuxemu/os_flags_nexus.inc $
# 
# 1   8/20/12 10:02a erickson
# SW7435-235: refactor symlinks by copying
# 
# 13   6/4/12 4:07p erickson
# SW7435-27: use separate NEXUS_OBJ_DIR for NEXUS_WEBCPU=core1_server
# 
# 12   12/14/10 5:01p erickson
# SW7420-1264: use NEXUS_BIN_DIR and NEXUS_BUILD_DIR for final and
#  intermediate binaries
# 
# 11   9/29/10 9:33a erickson
# SW7420-1123: add linux usermode IPC
# 
# 10   12/15/09 4:34p erickson
# SW7400-2640: wrap some variable defines with ifeq to allow for
#  customization
#
# 9   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 8   4/8/09 4:31p vsilyaev
# PR 54016: Added support for precompiled header
#
# 7   3/31/09 10:44a vsilyaev
# PR 53611: Don't link static libraries unless directed
#
# 6   3/27/09 3:08p vsilyaev
# PR 53611: Temporary returned back unconditional build of static
#  libraries
#
# 5   3/26/09 11:07a vsilyaev
# PR 53611: Don't link static libraries unless directed
#
# 4   3/13/09 2:44p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 3   1/23/08 9:21p vobadm
# PR35457: update docs
#
# 2   1/23/08 3:09p vsilyaev
# PR 38682: Compile PROXY code into the dedicated directory
#
# 1   1/18/08 12:47p vsilyaev
# PR 38682: OS dependent compiler flags
#
#
############################################################

############################################################
# Overview:
#
# this file should only be included by nexus.inc
# it is only used for internal nexus build
#
############################################################

#
# Default to -Os by default
#
OPTIMIZER_LEVEL ?= s

#
# Default to PIC code by default
#
ifndef B_REFSW_SHAREABLE
B_REFSW_SHAREABLE := y
endif

#
# Install binaries to $(NEXUS_BUILD_DIR)/$(B_REFSW_OS).$(B_REFSW_ARCH).debug or .release.
#
ifeq ($(B_REFSW_DEBUG),n)
NEXUS_OBJ_DIR_SUFFIX := release
NEXUS_CFLAGS += -O${OPTIMIZER_LEVEL}
else
NEXUS_OBJ_DIR_SUFFIX := debug
NEXUS_CFLAGS += -g -O${OPTIMIZER_LEVEL}
endif

ifeq ($(NEXUS_OBJ_DIR),)
ifeq ($(NEXUS_WEBCPU),core1_server)
NEXUS_OBJ_DIR := $(NEXUS_BUILD_DIR)/$(B_REFSW_OS).$(B_REFSW_ARCH).$(NEXUS_WEBCPU).$(NEXUS_OBJ_DIR_SUFFIX)
else
ifdef NEXUS_MODE
NEXUS_OBJ_DIR := $(NEXUS_BUILD_DIR)/$(B_REFSW_OS).$(B_REFSW_ARCH).$(NEXUS_MODE).$(NEXUS_OBJ_DIR_SUFFIX)
else
NEXUS_OBJ_DIR := $(NEXUS_BUILD_DIR)/$(B_REFSW_OS).$(B_REFSW_ARCH).$(NEXUS_OBJ_DIR_SUFFIX)
endif
endif
endif


#
# Allow for shared and non-shared builds.  Shared will be relocatable, non-shared will not.
#
ifeq ($(B_REFSW_SHAREABLE),y)
NEXUS_OBJ_SUFFIX := lo
NEXUS_CFLAGS += -DPIC -fpic
NEXUS_SHARED_LIB := $(NEXUS_OBJ_DIR)/libnexus${NEXUS_LIB_SUFFIX}.so
else
NEXUS_OBJ_SUFFIX := o
NEXUS_STATIC_LIB := $(NEXUS_OBJ_DIR)/libnexus${NEXUS_LIB_SUFFIX}.a
MAGNUM_LIB := $(NEXUS_OBJ_DIR)/libmagnum${NEXUS_LIB_SUFFIX}.a
endif

NEXUS_MAGNUM_PRECOMPILED_HEADER_H := ${NEXUS_OBJ_DIR}/MAGNUM/$(notdir ${NEXUS_MAGNUM_PRECOMPILED_HEADER})
NEXUS_PRECOMPILED_HEADER_H := ${NEXUS_OBJ_DIR}/MAGNUM/$(notdir ${NEXUS_PRECOMPILED_HEADER})


