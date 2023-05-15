############################################################
#     (c)2010-2012 Broadcom Corporation
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
# $brcm_Workfile: graphics3d.inc $
# $brcm_Revision: 3 $
# $brcm_Date: 7/17/12 7:59a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/graphics3d/graphics3d.inc $
# 
# 3   7/17/12 7:59a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 2   2/8/11 12:57p hauxwell
# SWVC4-118: refactor directory
# 
# 1   1/24/11 7:09p hauxwell
# SWVC4-111: merge to mainline
# 
# hauxwell_35230_v3d/6   1/14/11 1:29p gsweet
# Fixes for kernel mode build of graphics3d
# 
# hauxwell_35230_v3d/5   12/6/10 11:51a gsweet
# SWVC4-101: Allow abstract platform support for V3D driver.
# 
# hauxwell_35230_v3d/4   11/3/10 12:59p hauxwell
# cant use a hardcoded chip version, as this file is shared with 7422A0
# 
# hauxwell_35230_v3d/3   10/4/10 11:12a hauxwell
# proxy mode
# 
# hauxwell_35230_v3d/2   8/19/10 12:19p hauxwell
# force C0 build for moment
# 
# hauxwell_35230_v3d/1   8/12/10 10:15a hauxwell
# add 35230 C0 build
# 
# 
############################################################

######################
# Define this module #
######################
NEXUS_MODULES += GRAPHICS3D

ifeq ($(V3D_SUPPORT),y)
###################
# V3D - 40nm 3D   #
###################
NEXUS_GRAPHICS3D_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/graphics3d/include
NEXUS_GRAPHICS3D_SOURCES := $(NEXUS_TOP)/modules/graphics3d/src/nexus_graphics3d.c
NEXUS_GRAPHICS3D_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/graphics3d/src
NEXUS_GRAPHICS3D_MAGNUM_MODULES += $(MAGNUM)/../rockford/middleware/v3d/khronos.inc
else
###################
# P3D - 65nm 3D   #
###################
NEXUS_GRAPHICS3D_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/graphics3d/include/p3d
NEXUS_GRAPHICS3D_SOURCES := $(NEXUS_TOP)/modules/graphics3d/src/p3d/nexus_graphics3d.c
NEXUS_GRAPHICS3D_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/graphics3d/src/p3d
NEXUS_GRAPHICS3D_MAGNUM_MODULES += \
    $(MAGNUM)/portinginterface/p3d/bp3d.inc \
    $(MAGNUM)/../rockford/middleware/opengles/opengles.inc
endif

#######################
# Generic Rules       #
#######################
NEXUS_GRAPHICS3D_DEPENDENCIES := BASE CORE
NEXUS_GRAPHICS3D_PRIVATE_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)
NEXUS_GRAPHICS3D_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_graphics3d_thunks.c
