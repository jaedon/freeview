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
# $brcm_Workfile: nexus_defs.inc $
# $brcm_Revision: 25 $
# $brcm_Date: 10/22/12 3:10p $
#
# Module Description:
#   This file contains a set of definitions for building or using nexus
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/build/nexus_defs.inc $
# 
# 25   10/22/12 3:10p erickson
# SW7425-2130: redefine B_REFSW_DEBUG=n to still print ERR to console
# 
# 24   5/15/12 2:57p erickson
# SW7425-2719: use .inc files for media framework build variables, fix
#  flv and rmff
# 
# 23   4/3/12 9:21a erickson
# SW7425-2719: move media framework build variables to nexus_defs.inc
# 
# 22   10/3/11 10:21a erickson
# SW7425-1343: force NEXUS_SERVER_SUPPORT=n if NEXUS_BASE_ONLY=y
# 
# 21   9/2/11 12:39p erickson
# SW7420-1148: move NEXUS_SERVER_SUPPORT to nexus_defs.inc for general
#  use
# 
# 20   12/14/10 5:01p erickson
# SW7420-1264: use NEXUS_BIN_DIR and NEXUS_BUILD_DIR for final and
#  intermediate binaries
# 
# 19   11/10/10 6:55p ttrammel
# SW7420-1228: Initial Support for Android.
# 
# 18   8/21/10 2:08p ttrammel
# SW7420-973: Change FIRMWARE_SUPPORT to NEXUS_FIRMWARE_SUPPORT
# 
# 17   8/11/10 4:56p ttrammel
# SW7420-943: Merge NFE to main branch.
# 
# NFE/1   5/10/10 6:04p ttrammel
# SW7405-4315: Initial NFE check-in.
# 
# 16   12/15/09 4:34p erickson
# SW7400-2640: wrap some variable defines with ifeq to allow for
#  customization
#
# 15   9/16/09 5:30p erickson
# SW7400-2511: move NEXUS_POWER_MANAGEMENT to core.inc
#
# 14   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 13   8/17/09 10:45a jrubio
# PR55882: remove PowerManagement default for 7342/7340
#
# 12   8/16/09 6:37p nickh
# PR47760: Enable power management for 7420
#
# 11   6/25/09 2:28p erickson
# PR50407: enable power management for 7405
#
# 10   6/12/09 9:09a erickson
# PR55685: remove 7405 exception
#
# 9   6/3/09 3:15p erickson
# PR55685: default NEXUS_POWER_MANAGEMENT=n until 7405's BPWR_Open sets
#  initial state
#
# 8   5/29/09 5:57p jrubio
# PR55232: add 7342/7340
#
# 7   5/15/09 12:32p erickson
# PR54843: default NEXUS_POWER_MANAGEMENT for 7325/7335
#
# 6   3/13/09 2:42p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 5   11/21/08 4:42p vsilyaev
# PR 38808: Fixed definition of BDBG_DEBUG_BUILD
#
# 4   10/30/08 8:22p erickson
# PR40799: only turn on pm for DTV now
#
# 3   10/30/08 1:23p erickson
# PR40799: default NEXUS_POWER_MANAGEMENT on
#
# 2   10/5/08 11:50p erickson
# PR40799: added NEXUS_POWER_MANAGEMENT, defaults off
#
# 1   1/18/08 2:12p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/3   1/18/08 1:10p vsilyaev
# PR 38682: Repartition os_tools.inc and os_rules.inc, to os_tools,
# os_flags, os_flags_nexus and os_rules.inc
#
# Nexus_Devel/2   1/17/08 6:07p jgarrett
# PR 38808: Updating file locations
#
# Nexus_Devel/1   10/2/07 11:54a jgarrett
# PR 34416: Adding nexus_defs.inc
#
############################################################

############################################################
# Overview:
#
# This file is used by both nexus and application Makefiles
# It is included by the platform Makefile (for internal nexus builds)
# It is included by the platform_app.inc (for application builds)
#
############################################################

#
# Sanity check that NEXUS_TOP is defined
#
ifndef NEXUS_TOP
$(error NEXUS_TOP is not defined.)
endif

# backward-compatibility
ifdef PLATFORM
NEXUS_PLATFORM ?= $(PLATFORM)
endif
ifdef MODE
NEXUS_MODE ?= $(MODE)
endif
ifdef OS
B_REFSW_OS ?= $(OS)
endif
ifdef DEBUG
B_REFSW_DEBUG ?= $(DEBUG)
endif
ifdef VERBOSE
B_REFSW_VERBOSE ?= $(VERBOSE)
endif
ifdef ARCH
B_REFSW_ARCH ?= $(ARCH)
endif
ifdef SHAREABLE
B_REFSW_SHAREABLE ?= $(SHAREABLE)
endif

ifdef NEXUS_FIRMWARE_SUPPORT
B_REFSW_FIRMWARE ?= $(NEXUS_FIRMWARE_SUPPORT)
endif

ifdef NEXUS_ANDROID_SUPPORT
ifndef ANDROID
$(error ANDROID must point to the top of a previously built Android source directory.)
endif

B_REFSW_ANDROID ?= $(NEXUS_ANDROID_SUPPORT)
endif

#
# Builds default to linuxuser
#
ifeq ($(B_REFSW_OS),)
B_REFSW_OS = linuxuser
endif

#
# Define the bin directories
#
ifeq ($(NEXUS_BIN_DIR),)
# location of final binaries
NEXUS_BIN_DIR = ${NEXUS_TOP}/bin${NEXUS_BIN_DIR_SUFFIX}
endif
ifeq ($(NEXUS_BUILD_DIR),)
# location of intermediate binaries and build artifacts
NEXUS_BUILD_DIR = ${NEXUS_TOP}/platforms/$(NEXUS_PLATFORM)/bin${NEXUS_BIN_DIR_SUFFIX}
endif
ifeq ($(NEXUS_SYNCTHUNK_DIR),)
# subdir for auto-generated thunk files
NEXUS_SYNCTHUNK_DIR = $(NEXUS_BUILD_DIR)/syncthunk
endif

#
# Set OS-independent defaults
#
ifeq ($(B_REFSW_DEBUG),)
B_REFSW_DEBUG = y
endif

#
# Define root of other trees
#
MAGNUM := $(NEXUS_TOP)/../magnum
ROCKFORD := $(MAGNUM)/../rockford
BSEAV := $(MAGNUM)/../BSEAV

#
# Define options for quiet builds
#
ifneq ($(B_REFSW_VERBOSE),)
Q_:=
else
Q_:=@
MAKEFLAGS += --no-print-directory
endif

#
# Define debug options
# Always enable DBG, but allow B_REFSW_DEBUG=n to reduce size
#
NEXUS_CFLAGS += -DBDBG_DEBUG_BUILD=1
ifeq ($(B_REFSW_DEBUG),n)
B_REFSW_DEBUG_LEVEL=err
B_REFSW_DEBUG_COMPACT_ERR=y
B_REFSW_DEBUG_ASSERT_FAIL=n
endif

# multiprocess support
# enables class verification code
NEXUS_SERVER_SUPPORT ?= y
ifeq ($(NEXUS_BASE_ONLY),y)
NEXUS_SERVER_SUPPORT = n
endif
ifeq ($(NEXUS_SERVER_SUPPORT),y)
NEXUS_CFLAGS += -DNEXUS_SERVER_SUPPORT=1
endif


#
# Include os tools and flags
#
include $(NEXUS_TOP)/build/os/$(B_REFSW_OS)/os_tools.inc
include $(NEXUS_TOP)/build/os/$(B_REFSW_OS)/os_flags.inc

#
# Define a flag to indicate this file has been included
#
NEXUS_DEFS_INCLUDED := yes

# default media flags for transport.inc, playback.inc and others
ifeq ($(MEDIA_ASF_SUPPORT),)
MEDIA_ASF_SUPPORT = $(shell test -e ${BSEAV}/lib/asf/basf.inc && echo y)
endif
ifeq ($(MEDIA_AVI_SUPPORT),)
MEDIA_AVI_SUPPORT = $(shell test -e ${BSEAV}/lib/avi/bavi.inc && echo y)
endif
ifeq ($(MEDIA_FLV_SUPPORT),)
MEDIA_FLV_SUPPORT = $(shell test -e ${BSEAV}/lib/media/flv/bflv.inc && echo y)
endif
ifeq ($(MEDIA_RMFF_SUPPORT),)
MEDIA_RMFF_SUPPORT = $(shell test -e ${BSEAV}/lib/media/rmff/brmff.inc && echo y)
endif
