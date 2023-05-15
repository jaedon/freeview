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
# $brcm_Workfile: input_capture.inc $
# $brcm_Revision: 7 $
# $brcm_Date: 11/11/12 10:52a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/input_capture/input_capture.inc $
# 
# 7   11/11/12 10:52a mward
# SWNOOS-518: SW7435-433: Disable bcp on 7358 7552.
# 
# 6   11/11/12 10:02a mward
# SWNOOS-518: SW7435-433: Disable bcp on 7468.
# 
# 5   11/10/12 4:25p mward
# SWNOOS-518: SW7125-1321: Disable bcp on 7125, 7400.
# 
# 4   10/8/12 2:56p randyjew
# SW7435-235: update bcp path
# 
# 3   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# SWNOOS-518/4   3/23/12 12:55p agin
# SWNOOS-518:  Adapt to other chips who may not support bicap.
# 
# SWNOOS-518/3   3/15/12 12:10p agin
# SWNOOS-518:  Merge BICAP.
# 
# SWNOOS-518/2   3/4/12 9:52p agin
# SWNOOS-518:  Support Nexus.
# 
# SWNOOS-518/1   2/29/12 12:42p agin
# SWNOOS-518:  Create BICAP PI.
# 
# 1   4/24/08 12:59p erickson
# PR42113: add InputCapture
#
# 1   1/18/08 2:17p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/1   11/20/07 5:55p erickson
# PR37423: added module for input_capture, keypad and uhf_input
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += INPUT_CAPTURE

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_INPUT_CAPTURE_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/input_capture/include

##################
# Module Sources #
##################
NEXUS_INPUT_CAPTURE_SOURCES := \
	$(NEXUS_TOP)/modules/input_capture/src/nexus_input_capture.c

# thunk layer
NEXUS_INPUT_CAPTURE_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/input_capture/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_INPUT_CAPTURE_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_input_capture_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_INPUT_CAPTURE_DEPENDENCIES := BASE CORE

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
NEXUS_INPUT_CAPTURE_MAGNUM_MODULES = \
	$(MAGNUM)/portinginterface/icp/bicp.inc

BBCP_EXISTS = $(shell test -e ${MAGNUM}/portinginterface/bcp && echo y)
ifneq ($(BBCP_EXISTS),)
ifneq ($(findstring $(BCHP_CHIP), 7125 7400 7468 7358 7552), $(BCHP_CHIP))
NEXUS_INPUT_CAPTURE_DEFINES += BBCP_SUPPORT
NEXUS_INPUT_CAPTURE_MAGNUM_MODULES += $(MAGNUM)/portinginterface/bcp/bbcp.inc
endif
endif
