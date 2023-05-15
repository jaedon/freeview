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
# $brcm_Workfile: hdmi_input.inc $
# $brcm_Revision: 25 $
# $brcm_Date: 10/8/12 3:54p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/hdmi_input/hdmi_input.inc $
# 
# 25   10/8/12 3:54p rgreen
# SW7445-24: add support for 7445
# 
# 24   8/23/12 10:49a erickson
# SW7425-1747: remove unused code
# 
# 23   7/16/12 2:39p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 22   11/22/11 12:51p mward
# SW7435-20:  Add 7435.
# 
# 21   10/25/11 5:28p katrep
# SW7429-1:updated for 7429 bringup
# 
# 20   8/26/11 3:03p rgreen
# SW7425-1200,SWDTV-7644:  Remove depracated power management 1.0 scheme
# 
# 19   5/10/11 10:51a rgreen
# SWDTV-4664,SW3556-1233,SW3556-1236: Update HDMI Rx to build with new
#  HDCP Key Verification support.
# 
# 18   4/19/11 11:16a rgreen
# SW7422-186: Remove nexus_hdmi_repeater.c
# 
# 17   4/11/11 6:46p jhaberf
# SWDTV-6551: Added 35233 DTV chip support
# 
# 16   1/13/11 3:49p jhaberf
# SW35125-1: Added 35125 support
# 
# 15   12/23/10 9:18p hongtaoz
# SW35230-2374: add hdcp support to 7425;
# 
# 14   12/23/10 2:44p rgreen
# SW35230-2374:  Add support for 7422 HDCP OTP Key Checking
# 
# 13   12/23/10 2:02p rgreen
# SW35230-2374: Add support for OTP HDCP key check
# 
# 12   10/29/10 6:46p rgreen
# SW7425-21,SW7422-69:Temporarily disable CEC for 742x platforms
# 
# 11   10/27/10 10:00a rgreen
# SW7422-69,SW7425-21: include nexus Hdmi Input CEC support  for 742x
# 
# 10   7/14/10 11:51a adtran
# SW35230-517: added nexus_hdmi_input_tvm.c
# 
# 9   11/30/09 10:53a rgreen
# SW35230-15: Compile CEC only for supported chips; fix
#  NEXUS_HDMI_INPUT_SOURCES variable
# 
# 8   11/25/09 11:19a rgreen
# SW35230-15: Compile CEC only for supported chips
# 
# 7   4/9/09 4:53p rgreen
# PR52879: Add stub for EDID RAM Support
# 
# 6   4/9/09 4:22p rgreen
# PR52879: Add EDID RAM Support
# 
# PR52879/2   3/23/09 3:48p rgreen
# PR52879: Add EDID RAM Support
# 
# PR52879/1   3/23/09 2:03p rgreen
# PR52879: Add EDID RAM Support
# 
# 5   3/6/09 11:08a rgreen
# PR51307: Add HDMI HDCP Repeater Support
# 
# 4   1/29/09 11:01a erickson
# PR50787: Allow NEXUS_POWER_MANAGEMENT to be a master switch
# 
# 3   8/7/08 8:29a erickson
# PR45358: intial checkin
# 
# 2   6/18/08 12:39p erickson
# PR43781: remove unused hdrlib
# 
# 1   1/18/08 2:16p jgarrett
# PR 38808: Merging to main branch
# 
# Nexus_Devel/3   11/16/07 3:33p erickson
# PR36814: added SECURITY
# 
# Nexus_Devel/2   11/9/07 3:17p erickson
# PR36814: config Timebase
# 
# Nexus_Devel/1   11/8/07 2:41p erickson
# PR36814: add hdmi_input
# 
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += HDMI_INPUT

##################
# Module Options #
##################
# Allow NEXUS_POWER_MANAGEMENT to be a master switch

###################
# Module Includes #
###################
NEXUS_HDMI_INPUT_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/hdmi_input/include

##################
# Module Sources #
##################
NEXUS_HDMI_INPUT_SOURCES := \
    $(NEXUS_TOP)/modules/hdmi_input/src/nexus_hdmi_input.c \
    $(NEXUS_TOP)/modules/hdmi_input/src/nexus_hdmi_input_edid_ram.c


ifneq ($(findstring $(BCHP_CHIP), 7422 7425 7429 7435 7445),)
NEXUS_HDMI_INPUT_SOURCES += \
    $(NEXUS_TOP)/modules/hdmi_input/src/nexus_hdmi_input_hdcp.c
endif 
 
# thunk layer
NEXUS_HDMI_INPUT_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/hdmi_input/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_HDMI_INPUT_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_hdmi_input_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_HDMI_INPUT_DEPENDENCIES := BASE CORE TRANSPORT SECURITY

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
NEXUS_HDMI_INPUT_MAGNUM_MODULES =  \
    $(MAGNUM)/portinginterface/hdr/bhdr.inc \
    $(MAGNUM)/commonutils/acm/bacm.inc 
