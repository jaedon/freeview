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
# $brcm_Workfile: playback.inc $
# $brcm_Revision: 25 $
# $brcm_Date: 7/9/12 3:48p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/playback/playback.inc $
# 
# 25   7/9/12 3:48p vsilyaev
# SW7435-256: Use BLST_AAT implementation of AA Tree
# 
# 24   6/21/12 2:53p vsilyaev
# SW7420-2315: Promoted AA tree as separate modules
# 
# 23   5/17/12 3:21p cdisc
# SW7125-1289: adding NEXUS prefix to common crypto variable
# 
# 22   5/15/12 9:33a cdisc
# SW7125-1289: adding Common Crypto support
# 
# 21   4/3/12 9:21a erickson
# SW7425-2719: move media framework build variables to nexus_defs.inc
# 
# 20   3/26/12 1:58p erickson
# SW7425-2719: add source-detection to media framework build variables
# 
# 19   9/22/11 4:44p erickson
# SW7420-1965: add simple decoder support to nexus playback module
# 
# 18   9/22/11 12:26p jtna
# SW7425-1333: use MEDIA_RMFF_SUPPORT flag
# 
# 17   8/10/11 4:55p erickson
# SWDTV-4755: reduce DEPENDENCY list, depend on recursion
# 
# 16   4/27/11 6:20p vsilyaev
# SW7425-232: Promote ATOM module to separate component
# 
# 15   2/22/11 7:53p vsilyaev
# SW7422-107: Added FLV mapping
# 
# 14   10/20/10 11:07a vsilyaev
# SW7422-14: Moved B_MEDIA_PLAYER_NAV to bmedia.inc
# 
# 13   10/20/10 11:00a vsilyaev
# SW7422-14: Fixed typo
# 
# 12   10/19/10 1:36p vsilyaev
# SW7422-14: use build specifier for bcmplayer/bcmindexer
# 
# 11   10/19/10 1:08p vsilyaev
# SW7422-14: use build specifier for bcmplayer/bcmindexer
# 
# 10   9/22/10 12:39p jtna
# SW7405-4865: moved bvlc.c and .h to magnum/commonutils/vlc
# 
# 9   9/8/10 12:06p vsilyaev
# SW7468-129: Added video decoder on ZSP
# 
# 8   4/2/10 2:26p erickson
# SW7405-4162: add BMEDIA_PROBE and BFILE_STDIO for ease of use
#
# 7   9/10/09 2:55p erickson
# SW7335-529: remove unneeded variable
#
# 6   9/1/09 3:52p jgarrett
# SW7335-529: Replacing direct access to MAGNUM_MODULES with
#  NEXUS_EXTRA_MODULES to avoid magnum paths leaking into application
#  include paths
#
# 5   8/29/08 5:48p vsilyaev
# PR 33812: Added support for navigation in the MPEG-2 TS file without an
#  index
#
# 4   4/18/08 4:03p vsilyaev
# PR 41868: Added security API to playpump and recpump
#
# 3   4/17/08 4:26p vsilyaev
# PR 41845: Hold playpump callbacks if not playing data
#
# 2   1/24/08 5:18p vsilyaev
# PR 38682: Compile bmedia framework into the kernel
#
# 1   1/18/08 2:22p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/9   12/20/07 10:29a vsilyaev
# PR 38073: Updated to work with new bmedia_player
#
# Nexus_Devel/PR38073/1   12/18/07 4:59p vsilyaev
# PR 38073: Updated playback module to work exclusively with media player
#
# Nexus_Devel/8   12/5/07 12:45p vsilyaev
# PR 37574: Added ASF and AVI modules
#
# Nexus_Devel/7   11/13/07 3:29p vsilyaev
# PR 37015: Fixed dependency on sys/types.h
#
# Nexus_Devel/6   11/12/07 1:05p vsilyaev
# PR 35824: Added more media modules
#
# Nexus_Devel/5   10/16/07 12:59p vsilyaev
# PR 35824: Splitting playback into managable piecies
#
# Nexus_Devel/4   10/15/07 5:12p vsilyaev
# PR 35824: Added module initialization
#
# Nexus_Devel/3   10/15/07 2:55p vsilyaev
# PR 35824: Added synchronization thunk layer
#
# Nexus_Devel/2   10/11/07 6:25p vsilyaev
# PR 35824: Added more playback code
#
# Nexus_Devel/1   10/10/07 4:56p vsilyaev
# PR 35824: Playback module
#
############################################################

#
# Conditional Options for this Makefile
#
# MEDIA_ASF_SUPPORT - If y, ASF  is enabled
# MEDIA_AVI_SUPPORT - If y, AVI/DIVX is enabled

######################
# Define this module #
######################
NEXUS_MODULES += PLAYBACK

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_PLAYBACK_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/playback/include
NEXUS_PLAYBACK_PRIVATE_INCLUDES = $(NEXUS_TOP)/modules/playback/src

##################
# Module Sources #
##################
NEXUS_PLAYBACK_SOURCES = \
    $(NEXUS_TOP)/modules/playback/src/nexus_playback.c \
    $(NEXUS_TOP)/modules/playback/src/nexus_playback_flow.c \
    $(NEXUS_TOP)/modules/playback/src/nexus_playback_media.c \
    $(NEXUS_TOP)/modules/playback/src/nexus_playback_module.c \
    $(NEXUS_TOP)/modules/playback/src/nexus_playback_trick.c

ifeq ($(HUMAX_PLATFORM_BASE), y)
NEXUS_PLAYBACK_SOURCES += ${BSEAV}/lib/thumbnail/bthumbnail_extractor.c 
endif

# thunk layer
NEXUS_PLAYBACK_PRIVATE_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)
NEXUS_PLAYBACK_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_playback_thunks.c

##################
# Module Defines #
##################

#######################
# Module Dependencies #
#######################
NEXUS_PLAYBACK_DEPENDENCIES := FILE VIDEO_DECODER AUDIO TRANSPORT DMA SIMPLE_DECODER

NEXUS_PLAYBACK_MAGNUM_MODULES =  \
    ${BSEAV}/lib/utils/batom.inc \
    ${BSEAV}/lib/media/bmedia.inc \
    ${BSEAV}/lib/bfile/bfile.inc

ifeq ($(HUMAX_PLATFORM_BASE), y)
NEXUS_PLAYBACK_MAGNUM_MODULES += ${BSEAV}/lib/thumbnail/bthumbnail.inc
endif

ifeq ($(MEDIA_ASF_SUPPORT), y)
NEXUS_PLAYBACK_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/asf/basf.inc
endif

ifeq ($(MEDIA_AVI_SUPPORT), y)
NEXUS_PLAYBACK_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/avi/bavi.inc
endif

ifeq ($(MEDIA_FLV_SUPPORT), y)
NEXUS_PLAYBACK_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/media/flv/bflv.inc
endif

ifeq ($(MEDIA_RMFF_SUPPORT), y)
NEXUS_PLAYBACK_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/media/rmff/brmff.inc
endif

NEXUS_PLAYBACK_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/bcmplayer/bcmplayer.inc 

# BSEAV makefiles don't include their module names into MAGNUM_MODULES

ifeq ($(NEXUS_COMMON_CRYPTO_SUPPORT), y)
NEXUS_EXTRA_MODULES += COMMON_CRYPTO
NEXUS_PLAYBACK_MAGNUM_MODULES += $(BSEAV)/lib/security/common_crypto/common_crypto.inc
endif

# modules that are also used in the trasport/playpump
NEXUS_EXTRA_MODULES += BMEDIA BMEDIA_FILTER BATOM

# playback only modules
NEXUS_EXTRA_MODULES += BMEDIA_PLAYER BFILE BFILE_CACHE BFILE_MEDIA B_NAV_PLAYER B_MEDIA_NAV_PLAYER BAA_TREE 

ifeq ($(HUMAX_PLATFORM_BASE), y)
NEXUS_EXTRA_MODULES += BFILE_ASYNC BTHUMBNAIL
endif

# These modules are not needed by nexus itself, but are part of the media framework and are
# called directly by many apps.
NEXUS_EXTRA_MODULES += BMEDIA_PROBE BFILE_STDIO

