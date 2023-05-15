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
# $brcm_Workfile: hdmi_output.inc $
# $brcm_Revision: 15 $
# $brcm_Date: 7/16/12 1:20p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/hdmi_output/hdmi_output.inc $
# 
# 15   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 14   3/26/12 3:20p vle
# SW7425-2515: Merged to mainline
# 
# SW7425-2515/1   3/23/12 6:19p vle
# SW7425-2515: Update Brutus to use new NEXUS_Cec module. Remove all CEC
#  related code out of hdmi_output module
# 
# 13   12/19/11 12:34p vle
# SW7425-1140: Fix build issue without CEC support. CEC functionalities
#  in NEXUS_HDMI_OUTPUT modules is enabled with build option
#  BHDM_CEC_SUPPORT=y
# 
# 12   11/23/11 2:03p vle
# SW7425-1140: include cec PI only when BCEC_SUPPORT is set.
# 
# 11   11/22/11 6:11p vle
# SW7425-1140: Update nexus hdmi_output cec to use new BCEC PI instead of
#  BHDM_CEC
# 
# SW7425-1140/1   11/16/11 12:18p vle
# SW7425-1140: Update to use new CEC PI instead of BHDM_CEC
# 
# 10   8/10/11 4:55p erickson
# SWDTV-4755: reduce DEPENDENCY list, depend on recursion
# 
# 9   4/29/11 12:06p erickson
# SW7125-798: revise NEXUS_HdmiOutput_SetPreEmphasisConfiguration
# 
# 8   4/22/11 11:09a erickson
# SW7125-798: merge
# 
# 7   9/15/10 6:51p spothana
# SW7420-1024: Use conditional NEXUS_HDCP_SUPPORT
# 
# 6   9/14/09 7:11p vle
# SW7405-2885: Add nexus_hdmi_output_extra files.
# 
# 5   4/8/09 4:31p vsilyaev
# PR 54016: Added support for precompiled header
# 
# 4   8/1/08 4:26p erickson
# PR39979: add CEC support
# 
# 3   2/26/08 5:42p jgarrett
# PR 39017: Enabling HDCP by default
# 
# 2   2/21/08 5:58p jgarrett
# PR 39017: Adding HDCP
# 
# 1   2/1/08 5:25p jgarrett
# PR 39017: Adding hdmi_output
# 
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += HDMI_OUTPUT

##################
# Module Options #
##################
NEXUS_HDMI_OUTPUT_OPTIONS := NEXUS_HDCP_SUPPORT

###################
# Module Includes #
###################
NEXUS_HDMI_OUTPUT_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/hdmi_output/include

NEXUS_HDMI_OUTPUT_PRECOMPILED_HEADER = $(NEXUS_TOP)/modules/hdmi_output/src/nexus_hdmi_output_module.h

##################
# Module Sources #
##################
NEXUS_HDMI_OUTPUT_SOURCES := $(NEXUS_TOP)/modules/hdmi_output/src/nexus_hdmi_output.c \
                             $(NEXUS_TOP)/modules/hdmi_output/src/nexus_hdmi_output_module.c \
                             $(NEXUS_TOP)/modules/hdmi_output/src/nexus_hdmi_output_hdcp.c \
                             $(NEXUS_TOP)/modules/hdmi_output/src/nexus_hdmi_output_extra.c


# thunk layer
NEXUS_HDMI_OUTPUT_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/hdmi_output/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_HDMI_OUTPUT_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_hdmi_output_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_HDMI_OUTPUT_DEPENDENCIES := SECURITY I2C

##################
# Magnum Modules #
##################
NEXUS_HDMI_OUTPUT_MAGNUM_MODULES =  \
    $(MAGNUM)/portinginterface/hdm/bhdm.inc 

ifeq ($(findstring SECURITY, $(NEXUS_MODULES)),SECURITY)
NEXUS_HDMI_OUTPUT_MAGNUM_MODULES += \
    $(MAGNUM)/syslib/hdcplib/bhdcplib.inc

ifeq ($(NEXUS_HDCP_SUPPORT),y)
BHSM_KEYLADDER=ON
BHDCPLIB_HDCP_SUPPORT=y
endif
endif

