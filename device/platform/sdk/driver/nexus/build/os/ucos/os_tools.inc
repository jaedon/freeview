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
# $brcm_Workfile: os_tools.inc $
# $brcm_Revision: 5 $
# $brcm_Date: 8/17/11 4:00p $
#
# Module Description:
#   This file defines the OS-specific tools for building in ucos.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/ucos/os_tools.inc $
# 
# 5   8/17/11 4:00p bselva
# SW7358-92: Added the changes required for HDI port using ucos
# 
# 4   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 3   3/26/09 10:57a vsilyaev
# PR 53611: Use simply expanded variables everywhere where $(shell ...)
#  is used. It prevents evaluation of $(shell ) every where variable is
#  used
#
# 2   6/27/08 8:59a erickson
# PR43846: convert to $(PERL)
#
# 1   6/17/08 3:04p cnovak
# PR43697: Add uCOS to Nexus.
#
############################################################

B_REFSW_ARCH = sde

B_REFSW_CROSS_COMPILE ?= $(B_REFSW_ARCH)-

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

B_REFSW_TOOLCHAIN_DIR := $(shell dirname $(shell dirname $(shell which $(CC))))

# Note: This needs to be more flexible
NEXUS_ENDIAN ?= BSTD_ENDIAN_LITTLE

