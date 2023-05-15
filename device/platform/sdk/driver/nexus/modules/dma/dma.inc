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
# $brcm_Workfile: dma.inc $
# $brcm_Revision: 7 $
# $brcm_Date: 10/1/12 3:32p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/dma/dma.inc $
# 
# 7   10/1/12 3:32p jtna
# SW7445-18: add stubs for nexus xpt dma
# 
# 6   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 5   4/22/11 6:49p jtna
# SW7420-420: merge BMMD-based Nexus DMA module
# 
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += DMA

###################
# Module Includes #
###################
NEXUS_DMA_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/dma/include

##################
# Module Sources #
##################
ifeq ($(findstring $(BCHP_CHIP), 7445), $(BCHP_CHIP)) #TODO: key off of something else
NEXUS_DMA_DEFINES += NEXUS_HAS_XPT_DMA=1
NEXUS_DMA_SOURCES := $(NEXUS_TOP)/modules/dma/src/nexus_xpt_dma_module.c 
NEXUS_DMA_SOURCES += $(NEXUS_TOP)/modules/dma/src/nexus_xpt_dma.c 
else
NEXUS_DMA_SOURCES := $(NEXUS_TOP)/modules/dma/src/nexus_dma_module.c 
NEXUS_DMA_SOURCES += $(NEXUS_TOP)/modules/dma/src/nexus_dma.c 
endif

# thunk layer
NEXUS_DMA_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/dma/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_DMA_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_dma_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_DMA_DEPENDENCIES := BASE CORE

##################
# Magnum Modules #
##################
ifeq ($(findstring $(BCHP_CHIP), 7445), $(BCHP_CHIP)) #TODO: key off of something else
else
NEXUS_DMA_MAGNUM_MODULES =  \
    $(MAGNUM)/portinginterface/mmd/bmmd.inc
endif

