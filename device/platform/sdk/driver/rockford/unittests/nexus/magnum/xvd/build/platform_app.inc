############################################################
#     (c)2003-2008 Broadcom Corporation
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
# $brcm_Workfile: platform_app.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 5/14/09 11:42a $
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/magnum/xvd/build/platform_app.inc $
# 
# Hydra_Software_Devel/2   5/14/09 11:42a jtna
# PR54515: added header
# 
############################################################

ifndef NEXUS_TOP
$(error NEXUS_TOP is not defined)
endif

# Define platform specifics
# Supporting only 7405 for now
ifeq ($(findstring $(PLATFORM),97405 97459 97205 97466), $(PLATFORM))
BCHP_CHIP := 7405
ifndef BCHP_VER
$(warning Defaulting to BCHP_VER=B0)
BCHP_VER := B0
endif
endif

# Sanity check that we received a valid platform
ifndef BCHP_CHIP
$(error Unsupported platform $(PLATFORM))
endif

# Include nexus definitions
include $(NEXUS_TOP)/build/nexus_defs.inc

# Include a reduced set of nexus modules
include ./build/platform_modules.inc

# Convert include paths into single variable
NEXUS_APP_INCLUDE_PATHS := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_PUBLIC_INCLUDES))
NEXUS_APP_DEFINES := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_DEFINES))
NEXUS_APP_DEFINES += $(foreach module, $(NEXUS_MODULES),NEXUS_HAS_$(module))

# Include specific magnum modules
# bxvd.inc requires BCHP_CHIP to be defined
include $(MAGNUM)/portinginterface/xvd/bxvd.inc

# Add in magnum base module paths also
include $(MAGNUM)/basemodules/kni/bkni.inc
include $(MAGNUM)/basemodules/std/bstd.inc
include $(MAGNUM)/basemodules/dbg/bdbg.inc
include $(MAGNUM)/basemodules/chp/bchp.inc
include $(MAGNUM)/basemodules/reg/breg.inc
include $(MAGNUM)/basemodules/err/berr.inc
include $(MAGNUM)/commonutils/lst/blst.inc

# Convert magnum includes into the same variable
NEXUS_APP_INCLUDE_PATHS += $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES))
NEXUS_APP_DEFINES += $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES))

