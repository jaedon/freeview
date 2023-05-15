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
# $brcm_Workfile: os_tools.inc $
# $brcm_Revision: 1 $
# $brcm_Date: 7/26/12 3:17p $
#
# Module Description:
#   This file defines the OS-specific tools for building in linuxuser mode.
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/build/os/bare/os_tools.inc $
# 
# 1   7/26/12 3:17p erickson
# SW7435-235: remove symlink
# 
# 18   11/3/11 12:05p erickson
# SW7346-534: allow B_REFSW_TOOLCHAIN_DIR to be set for absolute path if
#  B_REFSW_CROSS_COMPILE is not also set
# 
# 17   3/26/11 8:18a jrubio
# SW7400-2511: use mipsel-linux instead of mipsel-uclibc
# 
# 16   12/23/10 3:57p ttrammel
# SW7420-1331: Update Android build to use Broadcom toolchain.
# 
# 15   12/1/10 3:25p ttrammel
# SW7420-1284: Remove unused statements from file.
# 
# 14   11/10/10 7:01p ttrammel
# SW7420-1228: Initial Support for Android.
# 
# 13   10/12/10 11:48a ttrammel
# SW7420-1132: Add readelf tool.
# 
# 12   7/27/10 2:20p erickson
# SW7125-523: extend B_REFSW_ARCH for other variants of big-endian uclibc
#
# 11   4/9/10 4:51p jrubio
# SW7125-253: fix for Big Endian
#
# 10   4/2/10 6:07p mward
# SW7125-253:  stbgcc-4.4.3-1.0 has mipsel-linux-uclibc-gcc not  mipsel-
#  uclibc-gcc.
#
# 9   3/24/10 11:15a erickson
# SW7405-4104: remove comment about g++ GPL contamination
#
# 8   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 7   9/10/09 11:30a erickson
# SW7400-2511: add -p option to cp. this prevents external build systems
#  from detecting false dependency changes. this is a better solution
#  than the -u that was removed earlier.
#
# 6   9/4/09 12:11p erickson
# SW7400-2502: allow TOOLCHAIN_DIR override, use CC to find toolchain
#  path
#
# 5   4/8/09 12:41p erickson
# PR53611: backing off the -u change. it causes problems for copying
#  unmodified items to nexus/bin when switching between linuxkernel and
#  linuxuser modes.
#
# 4   3/26/09 11:02a vsilyaev
# PR 53611: Use simply expanded variables everywhere where $(shell ...)
#  is used. It prevents evaluation of $(shell ) every where variable is
#  used
#
# 3   3/26/09 10:43a vsilyaev
# PR 53611: Use update copy (it wouldn't copy data unless necessary)
#
# 2   6/27/08 8:58a erickson
# PR43846: convert to $(PERL)
#
# 1   1/18/08 2:15p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/5   1/18/08 1:10p vsilyaev
# PR 38682: Repartition os_tools.inc and os_rules.inc, to os_tools,
# os_flags, os_flags_nexus and os_rules.inc
#
# Nexus_Devel/4   10/2/07 11:54a jgarrett
# PR 34416: Defaulting to mips32
#
# Nexus_Devel/3   9/25/07 12:50p vsilyaev
# PR 34662: Enabled warnings
#
# Nexus_Devel/2   9/4/07 11:40a jgarrett
# PR 34416: Adding NEXUS_ENDIAN
#
# Nexus_Devel/1   8/31/07 4:15p jgarrett
# PR 34416: Splitting os.inc into rules and tools
#
############################################################

############################################################
# Overview:
#
# This file should only be included by nexus_defs.inc
#
############################################################

#
# Default to little-endian
#
ifndef B_REFSW_ARCH
B_REFSW_ARCH = mipsel-linux
endif

#
# Set variables based on the toolchain
#
ifeq ($(B_REFSW_CROSS_COMPILE),)
    # set default B_REFSW_CROSS_COMPILE based on B_REFSW_ARCH
    ifdef NEXUS_ANDROID_SUPPORT
    B_REFSW_CROSS_COMPILE=mipsel-linux-uclibc-
    else
    ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
    B_REFSW_CROSS_COMPILE = mips-linux-
    else
    B_REFSW_CROSS_COMPILE = mipsel-linux-
    endif
    endif
    
    # if you don't set B_REFSW_CROSS_COMPILE, you can set B_REFSW_TOOLCHAIN_DIR to give the default B_REFSW_CROSS_COMPILE an absolute path
    ifneq ($(B_REFSW_TOOLCHAIN_DIR),)
    B_REFSW_CROSS_COMPILE := $(B_REFSW_TOOLCHAIN_DIR)/$(B_REFSW_CROSS_COMPILE)
    endif
else
    # user has set B_REFSW_CROSS_COMPILE.
    # you can choose to set it with or without an absolute path, but you can't also set B_REFSW_TOOLCHAIN_DIR.
endif

# Define make variables
AS      = $(B_REFSW_CROSS_COMPILE)as
LD      = $(B_REFSW_CROSS_COMPILE)ld
CC      = $(B_REFSW_CROSS_COMPILE)gcc
CXX     = $(B_REFSW_CROSS_COMPILE)c++
AR      = $(B_REFSW_CROSS_COMPILE)ar
NM      = $(B_REFSW_CROSS_COMPILE)nm
STRIP   = $(B_REFSW_CROSS_COMPILE)strip
OBJCOPY = $(B_REFSW_CROSS_COMPILE)objcopy
OBJDUMP = $(B_REFSW_CROSS_COMPILE)objdump
RANLIB  = $(B_REFSW_CROSS_COMPILE)ranlib
READELF  = $(B_REFSW_CROSS_COMPILE)readelf
MKDIR   = mkdir -p
PWD     = pwd
MV      = mv
PERL    = perl

CPP     = $(CC) -E
CP      = cp -fp
RM      = rm -f
SORT    = sort
SED     = sed
TOUCH   = touch
AWK     = awk

#
# Select toolchain for glibc toolchains
#
B_REFSW_TOOLCHAIN_DIR ?= $(shell dirname $(shell dirname $(shell which $(CC))))

# Define BSTD_CPU_ENDIAN correctly
ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
NEXUS_ENDIAN = BSTD_ENDIAN_BIG
else
NEXUS_ENDIAN = BSTD_ENDIAN_LITTLE
endif

