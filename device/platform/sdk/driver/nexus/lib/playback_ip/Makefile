############################################################
#     (c)2008-2011 Broadcom Corporation
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
# $brcm_Workfile: Makefile $
# $brcm_Revision: 7 $
# $brcm_Date: 2/18/11 5:25p $
#
# Module Description: Playback IP Applib Makefile
#
# Revision History:
#
# Created:
#
# $brcm_Log: /nexus/lib/playback_ip/Makefile $
# 
# 7   2/18/11 5:25p zhangjq
# SW7420-1419:Clean up Android Media Player code
# 
# 6   11/17/09 3:52p ssood
# SW7420-454: Enhance IP library to stream files from local disk
# 
# 5   8/14/09 4:37p ssood
# PR53773: Adding support for AVI playback over HTTP
# 
# 4   4/3/09 3:08p lwhite
# PR53773: Added SSL support
# 
# 3   11/5/08 4:12p ssood
# PR48146: more work on support needed for MP4/ASF playback
# 
# 2   10/27/08 11:45a ssood
# PR45431: added support to compile probe interface of bmedia
# 
# 
############################################################

ifdef COMSPEC
# Any DOS environment
B_LIB_TOP := $(shell cd .. && cd)
else
B_LIB_TOP := $(shell cd ..; pwd)
endif
BSEAV = $(B_LIB_TOP)/../../BSEAV

#if used in android, libnexus.so has included bellow files

# Include library definitions
include $(B_LIB_TOP)/build/b_lib_defs.inc
ifneq ($(NEXUS_ANDROID_SUPPORT),y)
include $(BSEAV)/lib/bfile/bfile.inc
include ${BSEAV}/lib/media/bmedia.inc
endif

# Include module sources
# Note: You must also include any dependent libraries here also.
include b_playback_ip_lib.inc

ifneq ($(NEXUS_ANDROID_SUPPORT),y)
include $(BSEAV)/lib/media/bmedia.inc
ifeq ($(MEDIA_ASF_SUPPORT), y)
include $(BSEAV)/lib/asf/basf.inc
endif
ifeq ($(MEDIA_AVI_SUPPORT), y)
include $(BSEAV)/lib/avi/bavi.inc
endif
endif

# Instruct build system to build only this library and name it libb_playback_ip
B_BUILD_LIBS := B_PLAYBACK_IP_LIB
B_LIB_OUTPUT_NAME := b_playback_ip

# Include library rules
include $(B_LIB_TOP)/build/b_lib_rules.inc

 B_LIB_CFLAGS += -std=c99
