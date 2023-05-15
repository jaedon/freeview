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
# $brcm_Workfile: b_lib_system_defs.inc $
# $brcm_Revision: 5 $
# $brcm_Date: 9/16/09 1:43p $
#
# Module Description:
#   This file defines basic library definitions for nexus-based systems
#
# Revision History:
#
# Created: 03/05/2007 by Jon Garrett
#
# $brcm_Log: /nexus/lib/build/nexus/b_lib_system_defs.inc $
# 
# 5   9/16/09 1:43p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 4   8/5/08 10:19p ssood
# PR45400: excluding c89 compiler option for C++ files
#
# 3   8/5/08 7:59a ssood
# PR45400: make -Wstrict-prototypes as C only option to fix the warnings
#  for C++ compilation
#
# 2   7/15/08 5:05p jgarrett
# PR 44825: Adding C++ support
#
# 1   3/10/08 1:57p jgarrett
# PR 40306: Adding build framework
#
############################################################

# Define NEXUS_TOP
NEXUS_TOP := $(B_LIB_TOP)/..

NEXUS_PLATFORM ?= $(PLATFORM)
B_REFSW_OS ?= $(OS)

# Sanity checks
ifeq ($(NEXUS_PLATFORM),)
$(error NEXUS_PLATFORM has not been defined)
endif
ifeq ($(B_REFSW_OS),linuxkernel)
$(error Can not build app libraries for linuxkernel mode)
endif

# Include Nexus Defs
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc

# Add C++ flags
B_LIB_CPPFLAGS := -fno-rtti -fno-exceptions
B_LIB_C_ONLY_FLAGS := -std=c99 -std=c89 -Wstrict-prototypes
