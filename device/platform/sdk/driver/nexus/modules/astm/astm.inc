############################################################
#     (c)2007-2012 Broadcom Corporation
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
# $brcm_Workfile: astm.inc $
# $brcm_Revision: 7 $
# $brcm_Date: 7/16/12 1:20p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/astm/astm.inc $
# 
# 7   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 6   8/10/11 4:55p erickson
# SWDTV-4755: reduce DEPENDENCY list, depend on recursion
# 
# 5   12/12/08 4:56p bandrews
# PR50079: fixed check for test framework
# 
# 4   12/8/08 4:07p bandrews
# PR50079: fixed for non-test build
# 
# 3   12/8/08 2:54p bandrews
# PR50079: added test framework
# 
# 2   3/24/08 10:32p bandrews
# PR40090: astm integration
# 
# 1   3/24/08 9:30p bandrews
# PR40090: astm impl
# 
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += ASTM

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_ASTM_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/astm/include

##################
# Module Sources #
##################
NEXUS_ASTM_SOURCES := \
	$(NEXUS_TOP)/modules/astm/src/nexus_astm.c

# thunk layer
NEXUS_ASTM_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/astm/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_ASTM_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_astm_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_ASTM_DEPENDENCIES := AUDIO VIDEO_DECODER

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
NEXUS_ASTM_MAGNUM_MODULES =  \
    $(MAGNUM)/syslib/astmlib/bastmlib.inc \
    $(MAGNUM)/syslib/framework/bsyslib_framework.inc

ifeq ($(NEXUS_ASTM_TEST_SUPPORT),y)
NEXUS_ASTM_PRIVATE_INCLUDES += $(ROCKFORD)/unittests/nexus/astm
NEXUS_ASTM_DEFINES += NEXUS_HAS_ASTM_TEST_SUPPORT=1
endif

