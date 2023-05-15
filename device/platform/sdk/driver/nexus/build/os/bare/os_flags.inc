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
# $brcm_Workfile: os_flags.inc $
# $brcm_Revision: 1 $
# $brcm_Date: 5/3/11 5:10p $
#
# Module Description:
#   This file defines the OS-specific flags for building in linuxkernel mode.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/bare/os_flags.inc $
# 
# 1   5/3/11 5:10p erickson
# SW7420-1819: repartition so NFE is a standard feature
#
############################################################

noop: all

ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
B_REFSW_TOOLCHAIN_ARCH=mips-linux-uclibc
else
B_REFSW_TOOLCHAIN_ARCH=mipsel-linux-uclibc
endif

ifeq ($(B_REFSW_OPTIMIZER_LEVEL),)
B_REFSW_OPTIMIZER_LEVEL = 2
endif

NEXUS_CFLAGS += -fno-common
NEXUS_CFLAGS += -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -Wa,--trap -mlong-calls
NEXUS_CFLAGS += -msoft-float -pipe
STD_INC ?= $(B_REFSW_TOOLCHAIN_DIR)/$(B_REFSW_TOOLCHAIN_ARCH)/include
GCC_BASE ?= $(shell (test -d $(B_REFSW_TOOLCHAIN_DIR)/lib/gcc-lib && echo $(B_REFSW_TOOLCHAIN_DIR)/lib/gcc-lib/$(B_REFSW_TOOLCHAIN_ARCH)) || echo $(B_REFSW_TOOLCHAIN_DIR)/lib/gcc/$(B_REFSW_TOOLCHAIN_ARCH))
GCC_VER ?= $(shell (ls $(GCC_BASE)))
GCC_INC ?= $(GCC_BASE)/$(GCC_VER)/include

NEXUS_CFLAGS += -nostdinc -I$(STD_INC) -I$(GCC_INC)
NEXUS_CFLAGS += -DBSTD_CPU_ENDIAN=$(NEXUS_ENDIAN)
NEXUS_CFLAGS += -ffreestanding -finline-limit=100000  -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32

# Use memcpy to assign structures
NEXUS_CFLAGS += -mmemcpy

NEXUS_CFLAGS += -Wall

# Release builds generate unused values. We suppress this warning.
NEXUS_CFLAGS += -Wno-unused-value

NEXUS_LDFLAGS += -G 0 --warn-common

# Flag to generate dependencies
CDEP_FLAG = -MMD
