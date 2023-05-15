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
# $brcm_Workfile: os_flags.inc $
# $brcm_Revision: 19 $
# $brcm_Date: 10/22/12 3:12p $
#
# Module Description:
#   This file defines the OS-specific flags for building in linuxuser mode.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/linuxuser/os_flags.inc $
# 
# 19   10/22/12 3:12p erickson
# SW7425-2130: suppress warnings for DEBUG=n
# 
# 18   9/19/12 10:42a erickson
# SW7445-45: revise 7445 support
# 
# 17   9/18/12 2:48p erickson
# SW7445-45: add 7445 support
# 
# 16   11/11/10 3:05p vsilyaev
# SW7405-4995: Use -mmemcpy option to force use of memcpy when assigning
#  compound types
# 
# 15   11/10/10 7:02p ttrammel
# SW7420-1228: Initial Support for Android.
# 
# 14   7/27/10 2:20p erickson
# SW7125-523: extend B_REFSW_ARCH for other variants of big-endian uclibc
#
# 13   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 12   9/4/09 12:11p erickson
# SW7400-2502: support big endian
#
# 11   3/26/09 2:20p vsilyaev
# PR 53611: Use -MMD files to create dependency file (this would exclude
#  #include <header.h> files)
#
# 10   3/26/09 10:57a vsilyaev
# PR 53611: Use simply expanded variables everywhere where $(shell ...)
#  is used. It prevents evaluation of $(shell ) every where variable is
#  used
#
# 9   3/18/09 10:06a vsilyaev
# PR 46190: Added NEXUS_C_WARNINGS to specify warning related flags
#
# 8   8/5/08 10:57a erickson
# PR45422: allow -std=c89 exceptions
#
# 7   8/4/08 2:27p erickson
# PR45361: use c89 for maximum compatibility
#
# 6   7/16/08 11:20a erickson
# PR44853: add -Wstrict-prototypes
#
# 5   6/18/08 1:07p erickson
# PR43730: re-add -W to debug builds
#
# 4   4/23/08 4:51p erickson
# PR41961: remove -W to fix DEBUG=n warnings
#
# 3   4/2/08 11:31a erickson
# PR40198: add -Wno-unused-value for release build warnings with
#  BERR_TRACE
#
# 2   1/30/08 4:25p vsilyaev
# PR 38682: Use C99 to help debug module to eject better code
#
# 1   1/18/08 12:47p vsilyaev
# PR 38682: OS dependent compiler flags
#
#
############################################################

############################################################
# Overview:
#
# This file should only be included by nexus_defs.inc
#
############################################################

NEXUS_CFLAGS += -D_GNU_SOURCE=1 -DLINUX -pipe
NEXUS_CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
NEXUS_CFLAGS += -DBSTD_CPU_ENDIAN=${NEXUS_ENDIAN}

ifeq ($(findstring mips, ${B_REFSW_ARCH}),mips)
#
# Default to mips32 in linuxuser
#
NEXUS_CFLAGS += -mips32
# Use memcpy to assign structures
NEXUS_CFLAGS += -mmemcpy
endif

ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mipsel-linux), ${B_REFSW_ARCH})
B_REFSW_TOOLCHAIN_ARCH=${B_REFSW_ARCH}
else
ifeq ($(filter ${B_REFSW_ARCH}, mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
B_REFSW_TOOLCHAIN_ARCH=mips-linux
else
B_REFSW_TOOLCHAIN_ARCH=mipsel-linux
endif
endif

# B_REFSW_TOOLCHAIN_DIR is defined in os_tools.inc
UCLIBC_VERSION := $(shell basename $(B_REFSW_TOOLCHAIN_DIR)/$(B_REFSW_TOOLCHAIN_ARCH)/lib/libuClibc*)
NEXUS_CFLAGS += -DUCLIBC_VERSION=\"$(UCLIBC_VERSION)\"

# We use c89 for maximum compatibility across OS's
# But, we allow overrides for integrating other code
ifeq ($(NEXUS_C_STD),)
NEXUS_C_STD=c89
endif
NEXUS_CFLAGS += -std=$(NEXUS_C_STD)

ifeq ($(NEXUS_C_WARNINGS),)
NEXUS_C_WARNINGS += -pedantic
ifeq ($(B_REFSW_DEBUG),y)
# -W is deprecated and is equivalent to -Wextra. We don't use it in release builds.
NEXUS_C_WARNINGS += -W -Wstrict-prototypes
# Release builds generate unused values because BDBG_MSG and WRN are compiled out. -Wno-unused-value does not suppress, so we must remove -Wall.
NEXUS_C_WARNINGS += -Wall
endif
endif

NEXUS_CFLAGS += ${NEXUS_C_WARNINGS}

ifeq ($(B_REFSW_ANDROID),y)
include $(NEXUS_TOP)/build/os/linuxuser/os_flags_android.inc
endif

# Flag to generate dependencies
CDEP_FLAG = -MMD

