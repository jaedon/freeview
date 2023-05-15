############################################################
#     (c)2007-2011 Broadcom Corporation
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
# $brcm_Workfile: record.inc $
# $brcm_Revision: 8 $
# $brcm_Date: 8/10/11 4:55p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/record/record.inc $
# 
# 8   8/10/11 4:55p erickson
# SWDTV-4755: reduce DEPENDENCY list, depend on recursion
# 
# 7   4/27/11 6:20p vsilyaev
# SW7425-232: Promote ATOM module to separate component
# 
# 6   10/19/10 1:08p vsilyaev
# SW7422-14: use build specifier for bcmplayer/bcmindexer
# 
# 5   9/22/10 12:40p jtna
# SW7405-4865: moved bvlc.c and .h to magnum/commonutils/vlc
# 
# 4   9/17/09 10:11a erickson
# SW7405-3026: allow Record module to build and run without Playback
#  module
#
# 3   5/7/08 4:21p erickson
# PR42329: use NEXUS_StopCallbacks instead of sleeps and loops inside
#  NEXUS_Record_Stop
#
# 2   4/18/08 4:03p vsilyaev
# PR 41868: Added security API to playpump and recpump
#
# 1   1/18/08 2:22p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/1   11/7/07 5:11p vsilyaev
# PR 36788: PVR record module
#
############################################################

######################
# Define this module #
######################
NEXUS_MODULES += RECORD

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_RECORD_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/record/include
NEXUS_RECORD_PRIVATE_INCLUDES =  \
    $(NEXUS_TOP)/modules/record/src \
    ${BSEAV}/lib/bcmplayer/include 

##################
# Module Sources #
##################
NEXUS_RECORD_SOURCES = \
    $(NEXUS_TOP)/modules/record/src/nexus_record.c  \
    $(NEXUS_TOP)/modules/record/src/nexus_record_module.c

NEXUS_RECORD_MAGNUM_MODULES :=  \
    ${BSEAV}/lib/bcmplayer/bcmplayer.inc 

# thunk layer
NEXUS_RECORD_PRIVATE_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)
NEXUS_RECORD_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_record_thunks.c

##################
# Module Defines #
##################

#######################
# Module Dependencies #
#######################
NEXUS_RECORD_DEPENDENCIES := FILE TRANSPORT PLAYBACK VIDEO_DECODER AUDIO DMA

# if nexus is compiled without playback module, record needs to include bcmindexer code
NEXUS_EXTRA_MODULES += B_NAV_INDEXER

