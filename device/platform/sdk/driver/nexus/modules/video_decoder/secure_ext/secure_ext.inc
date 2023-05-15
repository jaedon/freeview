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
# $brcm_Workfile: secure_ext.inc $
# $brcm_Revision: 4 $
# $brcm_Date: 7/16/12 1:40p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/video_decoder/secure_ext/secure_ext.inc $
# 
# 4   7/16/12 1:40p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 3   11/24/09 12:59p erickson
# SW7420-457: merge security changes
# 
# 2   11/3/09 11:50a yili
# SW7340-42:Add region verification - Minor change to meet Nexus design
#  guideline.
# 
# 1   3/9/09 4:50p yili
# PR52990:Add region verification
# 
# 
############################################################

###################
# Module Includes #
###################
NEXUS_VIDEO_DECODER_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/video_decoder/secure_ext/src

##################
# Module Sources #
##################
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/modules/video_decoder/secure_ext/src/nexus_avd0_image.c \
    $(NEXUS_TOP)/modules/video_decoder/secure_ext/src/nexus_video_decoder_security.c 

ifeq ($(DUAL_VIDEO_DECODER_SUPPORT), y)
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/modules/video_decoder/secure_ext/src/nexus_avd1_image.c 
endif

##################
# Module Defines #
##################
#NEXUS_VIDEO_DECODER_DEFINES += NEXUS_STILL_DECODER=1
