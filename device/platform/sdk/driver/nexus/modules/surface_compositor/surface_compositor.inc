############################################################
#     (c)2011 Broadcom Corporation
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
# $brcm_Workfile: surface_compositor.inc $
# $brcm_Revision: 5 $
# $brcm_Date: 11/11/11 4:43p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/surface_compositor/surface_compositor.inc $
# 
# 5   11/11/11 4:43p erickson
# SW7420-1992: refactor video client code to not require compositor
#  shutdown
# 
# 4   10/24/11 5:43p vsilyaev
# SW7420-1992: Updated cursor support
# 
# 3   10/5/11 3:50p vsilyaev
# SW7420-1992: Split compositor(frontend) and renderer(backend) portions
# 
# 2   8/23/11 4:13p erickson
# SW7420-1992: rename to surface_compositor
# 
# 1   7/26/11 1:34p erickson
# SW7420-1992: add surface_compositor module and examples
# 
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += SURFACE_COMPOSITOR

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_SURFACE_COMPOSITOR_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/surface_compositor/include

##################
# Module Sources #
##################
NEXUS_SURFACE_COMPOSITOR_SOURCES := \
    $(NEXUS_TOP)/modules/surface_compositor/src/nexus_surface_compositor_module.c \
    $(NEXUS_TOP)/modules/surface_compositor/src/nexus_surface_compositor.c \
    $(NEXUS_TOP)/modules/surface_compositor/src/nexus_surface_client.c \
    $(NEXUS_TOP)/modules/surface_compositor/src/nexus_surface_client_video.c \
    $(NEXUS_TOP)/modules/surface_compositor/src/nexus_surface_cursor.c \
    $(NEXUS_TOP)/modules/surface_compositor/src/nexus_surface_renderer.c

# thunk layer
NEXUS_SURFACE_COMPOSITOR_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/surface_compositor/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_SURFACE_COMPOSITOR_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_surface_compositor_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_SURFACE_COMPOSITOR_DEPENDENCIES := SURFACE DISPLAY GRAPHICS2D

##################
# Magnum Modules #
##################

#####################
# Module Extensions #
#####################