############################################################
#     (c)2003-2010 Broadcom Corporation
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
# $brcm_Date: 3/25/10 2:35p $
#
# Module Description:
#   This file defines the OS-specific tools for building in linuxemu mode.
#
# Revision History:
#
# Created:
#
# $brcm_Log: /nexus/build/os/linuxpal/os_tools.inc $
# 
# 1   3/25/10 2:35p adtran
# SW35230-126: Add support for Palladium testing
# 
# 3   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 2   6/5/09 2:55p jessem
# PR 54881: Changed ARCH to host and CROSS_COMPILE to /tools/bin/.
#
# 1   5/11/09 2:03p jessem
# PR 54881: Added support for emulation under linux.
#
#
############################################################

# Running on the build server use the native compiler.
# B_REFSW_ARCH = i386-linux 
# B_REFSW_CROSS_COMPILE = /tools/bin/

B_REFSW_CROSS_COMPILE ?=
B_REFSW_TOOLCHAIN_DIR =/tools/oss/packages/i686-rhel5/gcc/4.2.1/bin/ 


# Define make variables
AS      = $(B_REFSW_CROSS_COMPILE)as
LD      = $(B_REFSW_CROSS_COMPILE)ld
CC      = /tools/oss/packages/i686-rhel5/gcc/4.2.1/bin/gcc
# NOTE: We must use c++ and not g++ to avoid GPL contamination
CXX     = $(B_REFSW_CROSS_COMPILE)c++
AR      = $(B_REFSW_CROSS_COMPILE)ar
NM      = $(B_REFSW_CROSS_COMPILE)nm
STRIP   = $(B_REFSW_CROSS_COMPILE)strip
OBJCOPY = $(B_REFSW_CROSS_COMPILE)objcopy
OBJDUMP = $(B_REFSW_CROSS_COMPILE)objdump
RANLIB  = $(B_REFSW_CROSS_COMPILE)ranlib
MKDIR   = mkdir -p
PWD     = pwd
MV      = mv
PERL    = perl

CPP     = $(CC) -E
CP      = cp -f
RM      = rm -f
SORT    = sort
SED     = sed
TOUCH   = touch
AWK 	= awk


# Big endian only
# NEXUS_ENDIAN = BSTD_ENDIAN_BIG
NEXUS_ENDIAN = BSTD_ENDIAN_LITTLE

