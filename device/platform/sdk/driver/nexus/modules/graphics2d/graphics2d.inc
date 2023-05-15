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
# $brcm_Workfile: graphics2d.inc $
# $brcm_Revision: 4 $
# $brcm_Date: 7/16/12 1:20p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/graphics2d/graphics2d.inc $
# 
# 4   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 3   8/16/10 11:08a erickson
# SW7405-3671: add packet blit API. switch to packet-based GRC
#  implementation.
#
# 2   2/25/08 2:59p erickson
# PR39786: added NEXUS_Graphics2D_Destripe
#
# 1   1/18/08 2:16p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/1   10/19/07 9:38a erickson
# PR36275: initial
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += GRAPHICS2D

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_GRAPHICS2D_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/graphics2d/include

##################
# Module Sources #
##################
NEXUS_GRAPHICS2D_SOURCES := \
	$(NEXUS_TOP)/modules/graphics2d/src/nexus_graphics2d.c \
	$(NEXUS_TOP)/modules/graphics2d/src/nexus_graphics2d_destripe.c

# thunk layer
NEXUS_GRAPHICS2D_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/graphics2d/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_GRAPHICS2D_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_graphics2d_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_GRAPHICS2D_DEPENDENCIES := BASE CORE SURFACE

##################
# Magnum Modules #
##################

# Nexus only uses the packet-blit implementation of GRC.
# If you need the non-packet GRC, get an older version of nexus source.
GRCPACKET_SUPPORT=y

# These modules will be excluded from app-only builds
NEXUS_GRAPHICS2D_MAGNUM_MODULES =  \
    $(MAGNUM)/portinginterface/grc/bgrc.inc \
    $(MAGNUM)/syslib/grclib/bgrclib.inc