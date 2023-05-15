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
# $brcm_Workfile: keypad.inc $
# $brcm_Revision: 4 $
# $brcm_Date: 7/16/12 1:20p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/keypad/keypad.inc $
# 
# 4   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 3   4/9/09 11:28a jrubio
# PR52188: add support for 2nd Receiver
# 
# 2   2/11/09 10:04a erickson
# PR48801: added NEXUS_USE_KPD option for keypad and led
#
# 1   1/18/08 2:17p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/1   11/20/07 5:55p erickson
# PR37423: added module for ir_blaster, keypad and uhf_input
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += KEYPAD

##################
# Module Options #
##################
ifeq ($(filter $(BCHP_CHIP), 7336), $(BCHP_CHIP))
ifneq ($(NEXUS_USE_KPD),y)
NEXUS_USE_KEYPAD_SPI=y
endif
endif

###################
# Module Includes #
###################
NEXUS_KEYPAD_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/keypad/include

##################
# Module Sources #
##################
ifeq ($(NEXUS_USE_KEYPAD_SPI),y)
NEXUS_KEYPAD_SOURCES := $(NEXUS_TOP)/modules/keypad/src/nexus_keypad_spi.c
else
NEXUS_KEYPAD_SOURCES := $(NEXUS_TOP)/modules/keypad/src/nexus_keypad.c
endif

# thunk layer
NEXUS_KEYPAD_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/keypad/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_KEYPAD_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_keypad_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_KEYPAD_DEPENDENCIES := SPI GPIO

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
NEXUS_KEYPAD_MAGNUM_MODULES =  \
    $(MAGNUM)/portinginterface/kpd/bkpd.inc