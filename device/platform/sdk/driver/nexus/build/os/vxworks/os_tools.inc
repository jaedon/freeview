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
# $brcm_Workfile: os_tools.inc $
# $brcm_Revision: 3 $
# $brcm_Date: 9/16/09 1:38p $
#
# Module Description:
#   This file defines the OS-specific tools for building in vxworks.
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/build/os/vxworks/os_tools.inc $
# 
# 3   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 2   6/27/08 8:59a erickson
# PR43846: convert to $(PERL)
#
# 1   5/21/08 4:44p rjlewis
# PR40352: Initial version.
#
#
############################################################

B_REFSW_ARCH = mips-vxworks

ifeq ($(B_REFSW_ARCH),mips-vxworks)

B_REFSW_CROSS_COMPILE ?= mips

# Define make variables
AS      = as$(B_REFSW_CROSS_COMPILE)
LD      = ld$(B_REFSW_CROSS_COMPILE)
CC      = cc$(B_REFSW_CROSS_COMPILE)
CXX     = c++$(B_REFSW_CROSS_COMPILE)
AR      = ar$(B_REFSW_CROSS_COMPILE)
NM      = nm$(B_REFSW_CROSS_COMPILE)
STRIP   = strip$(B_REFSW_CROSS_COMPILE)
OBJCOPY = objcopy$(B_REFSW_CROSS_COMPILE)
OBJDUMP = opbdump$(B_REFSW_CROSS_COMPILE)
RANLIB  = ranlib$(B_REFSW_CROSS_COMPILE)

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

endif

# Note: we only support BE -- don't change this.
NEXUS_ENDIAN = BSTD_ENDIAN_BIG
