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
# $brcm_Workfile: surface.inc $
# $brcm_Revision: 4 $
# $brcm_Date: 7/16/12 1:19p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/surface/surface.inc $
# 
# 4   7/16/12 1:19p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 3   2/3/11 6:37p vsilyaev
# SW7420-1441: Use local (non proxied) implementation for function that
#  flush CPU cache
# 
# 2   11/7/08 9:42a erickson
# PR46300: added general purpose NEXUS_SURFACE_EXTENSION_INC hook
#
# 1   1/18/08 2:13p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/3   9/28/07 10:01a erickson
# PR34419: 0.1 prerel review
#
# Nexus_Devel/2   9/13/07 2:23p vsilyaev
# PR 34419: Updated include path
#
# Nexus_Devel/1   9/12/07 4:11p vsilyaev
# PR 34419: Surface build script
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += SURFACE

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_SURFACE_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/surface/include

##################
# Module Sources #
##################
NEXUS_SURFACE_SOURCES := $(NEXUS_TOP)/modules/surface/src/nexus_surface.c
NEXUS_SURFACE_LOCAL_SOURCES := $(NEXUS_TOP)/modules/surface/src/nexus_surface_local.c

# thunk layer
NEXUS_SURFACE_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/surface/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_SURFACE_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_surface_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_SURFACE_DEPENDENCIES := BASE CORE

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
NEXUS_SURFACE_MAGNUM_MODULES =  \
    $(MAGNUM)/commonutils/sur/bsur.inc

#####################
# Module Extensions #
#####################
ifneq ($(NEXUS_SURFACE_EXTENSION_INC),)
include $(NEXUS_SURFACE_EXTENSION_INC)
endif
