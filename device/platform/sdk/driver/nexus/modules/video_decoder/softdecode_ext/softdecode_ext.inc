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
# $brcm_Workfile: softdecode_ext.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 7/16/12 1:40p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/video_decoder/softdecode_ext/softdecode_ext.inc $
# 
# 2   7/16/12 1:40p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 1   9/8/10 11:46a vsilyaev
# SW7468-129: Added video decoder on ZSP
# 
# SW7468-129/1   3/5/10 7:30p vsilyaev
# SW7468-129: Added hooks for soft video decoder
# 
# 
############################################################

NEXUS_VIDEO_DECODER_P_SOFTDECODE := ${NEXUS_TOP}/../BSEAV/lib/ffmpeg/
NEXUS_VIDEO_DECODER_P_SOFTDECODE_LIB := $(wildcard ${NEXUS_VIDEO_DECODER_P_SOFTDECODE}/mipsel-uclibc.debug/lib*.a)

ifneq (${NEXUS_VIDEO_DECODER_P_SOFTDECODE_LIB},)
B_SOFTDECODE_ROOT := ${NEXUS_VIDEO_DECODER_P_SOFTDECODE}

###################
# Module Includes #
###################
NEXUS_VIDEO_DECODER_PRIVATE_INCLUDES += \
    ${NEXUS_VIDEO_DECODER_P_SOFTDECODE} \
    $(NEXUS_TOP)/modules/video_decoder/softdecode_ext/src

##################
# Module Sources #
##################
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/modules/video_decoder/softdecode_ext/src/nexus_video_decoder_soft.c 

##################
# Module Defines #
##################
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_NUM_SOFT_VIDEO_DECODERS=1

NEXUS_EXTRALIBS += ${NEXUS_VIDEO_DECODER_P_SOFTDECODE_LIB}
endif
