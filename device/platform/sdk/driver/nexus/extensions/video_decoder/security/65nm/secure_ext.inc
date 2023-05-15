#############################################################################
#    (c)2011-2012 Broadcom Corporation
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
# $brcm_Workfile: secure_ext.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 8/27/12 2:08p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /nexus/extensions/video_decoder/security/65nm/secure_ext.inc $
# 
# 2   8/27/12 2:08p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 1   7/20/11 11:58a yili
# SW7125-136:Relocate security
# 
#############################################################################
###################
# Module Includes #
###################
NEXUS_VIDEO_DECODER_PRIVATE_INCLUDES += $(NEXUS_TOP)/extensions/video_decoder/security/65nm/src

ifeq ($(VIDEO_REGION_VERIFICATION_SUPPORT),y)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_VIDEO_VERIFICATION
NEXUS_VIDEO_DECODER_DEFINES += BXVD_USE_CUSTOM_IMAGE=1
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_VIDEO_VERIFICATION_LEGACY_65NM
# Hard coded XVD FW address.  Make it a build parameter later
ifeq ($(DUAL_VIDEO_DECODER_SUPPORT), y)
NEXUS_VIDEO_DECODER_DEFINES += XVD_PHYSICAL_ADDRESS=0x0fde0000
else
NEXUS_VIDEO_DECODER_DEFINES += XVD_PHYSICAL_ADDRESS=0x0fef0000
endif
endif

##################
# Module Sources #
##################
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/extensions/video_decoder/security/65nm/src/nexus_video_decoder_security.c \
    $(NEXUS_TOP)/extensions/video_decoder/security/65nm/src/nexus_avd0_image.c  \

ifeq ($(DUAL_VIDEO_DECODER_SUPPORT), y)
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/extensions/video_decoder/security/65nm/src/nexus_avd1_image.c 
endif
