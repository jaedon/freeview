#############################################################################
#    (c)2009-2012 Broadcom Corporation
# 
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
#  
# Except as expressly set forth in the Authorized License,
#  
# 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#  
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
# USE OR PERFORMANCE OF THE SOFTWARE.
# 
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
# ANY LIMITED REMEDY.
#
# $brcm_Workfile: Makefile.am $
# $brcm_Revision: DirectFB_1_4_15_Port/2 $
# $brcm_Date: 3/7/12 5:25p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/inputdrivers/Makefile.am $
# 
# DirectFB_1_4_15_Port/2   3/7/12 5:25p christ
# SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
# 
# DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
# SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
# 
# DirectFB_1_4_5_Port/1   6/14/11 6:12p christ
# SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
# 
# DirectFB_1_4_Port/3   12/14/10 11:24a scchang
# SW7420-1313: Add bluetooth handset support
# 
# DirectFB_1_4_Port/2   2/24/10 6:39p robertwm
# SW7468-125: DirectFB: Provide ability to separately build IR and Keypad
#  input drivers.
# 
# DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
# PR56658: Add DirectFB 1.4.1 broadcom_files
# 
#############################################################################
## Makefile.am for DirectFB/inputdrivers
if BCMNEXUS_IR_INPUT
BCMNEXUS_IR_INPUT_DIR = bcmnexus
endif

if BCMNEXUS_KEY_INPUT
BCMNEXUS_KEY_INPUT_DIR = bcmnexus
endif

if BCMNEXUS_RF4CE_INPUT
BCMNEXUS_RF4CE_INPUT_DIR = bcmnexus
endif

SUBDIRS = \
	$(BCMNEXUS_IR_INPUT_DIR)	\
	$(BCMNEXUS_KEY_INPUT_DIR)	\
	$(BCMNEXUS_KEYBOARD_POINTER_INPUT_DIR) \
	$(BCMNEXUS_RF4CE_INPUT_DIR)
