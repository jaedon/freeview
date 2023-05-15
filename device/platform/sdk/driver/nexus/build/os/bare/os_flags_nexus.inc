############################################################
#     (c)2003-2011 Broadcom Corporation
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
# $brcm_Date: 5/3/11 5:10p $
#
# Module Description:
#   This file defines the nexus OS-specific flags for building in linuxkernel mode.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/bare/os_flags_nexus.inc $
# 
# 1   5/3/11 5:10p erickson
# SW7420-1819: repartition so NFE is a standard feature
# 
# 10   12/14/10 5:01p erickson
# SW7420-1264: use NEXUS_BIN_DIR and NEXUS_BUILD_DIR for final and
#  intermediate binaries
# 
# 9   12/18/09 11:29a erickson
# SW7400-2640: wrap some variable defines with ifeq to allow for
#  customization
#
# 8   10/14/09 10:51a erickson
# SW7400-2511: add NEXUS_ namespace
#
# 7   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 6   9/1/09 4:26p erickson
# SWDEPRECATED-3858: allow NEXUS_STATIC_LIB and MAGNUM_LIB. needed for
#  non-proxy linuxkernel builds.
#
# 5   4/8/09 4:31p vsilyaev
# PR 54016: Added support for precompiled header
#
# 4   3/31/09 7:29p vsilyaev
# PR 50887: OTFPVR support for the Nexus video decoder
#
# 3   3/26/09 11:07a vsilyaev
# PR 53611: Don't link static libraries unless directed
#
# 2   3/13/09 2:44p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 1   1/18/08 12:47p vsilyaev
# PR 38682: OS dependent compiler flags
#
# Nexus_Devel/2   1/16/08 3:35p vsilyaev
# PR 38682: Cleanup of flags
#
#
############################################################


#
# Default to -Os by default
#
B_REFSW_OPTIMIZER_LEVEL ?= s


ifeq ($(B_REFSW_DEBUG),n)
NEXUS_OBJ_DIR_SUFFIX := release
NEXUS_CFLAGS += -O$(B_REFSW_OPTIMIZER_LEVEL)
else
NEXUS_OBJ_DIR_SUFFIX := debug
NEXUS_CFLAGS += -g -O$(B_REFSW_OPTIMIZER_LEVEL)
endif

#
# Install binaries to $(NEXUS_BUILD_DIR)/$(B_REFSW_OS).$(B_REFSW_ARCH).debug or .release.
#
ifeq ($(NEXUS_OBJ_DIR),)
NEXUS_OBJ_DIR := $(NEXUS_BUILD_DIR)/$(B_REFSW_OS).$(B_REFSW_ARCH).$(NEXUS_OBJ_DIR_SUFFIX)
endif

NEXUS_OBJ_SUFFIX := o
NEXUS_STATIC_LIB := $(NEXUS_OBJ_DIR)/libnexus.a
MAGNUM_LIB := $(NEXUS_OBJ_DIR)/libmagnum.a
NEXUS_DRIVER := $(NEXUS_OBJ_DIR)/nexus.ko
NEXUS_CFLAGS += -DKBUILD_MODNAME=nexus

NEXUS_MAGNUM_PRECOMPILED_HEADER_H := $(NEXUS_OBJ_DIR)/MAGNUM/$(notdir $(NEXUS_MAGNUM_PRECOMPILED_HEADER))
NEXUS_PRECOMPILED_HEADER_H := $(NEXUS_OBJ_DIR)/MAGNUM/$(notdir $(NEXUS_PRECOMPILED_HEADER))

