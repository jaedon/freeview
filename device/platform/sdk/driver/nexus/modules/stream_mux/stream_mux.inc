############################################################
#     (c)2010-2012 Broadcom Corporation
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
# $brcm_Workfile: stream_mux.inc $
# $brcm_Revision: 7 $
# $brcm_Date: 7/16/12 2:39p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/stream_mux/stream_mux.inc $
# 
# 7   7/16/12 2:39p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 6   5/16/11 8:11p vsilyaev
# SW7425-482: Added support for the ASF file MUX
# 
# 5   4/8/11 1:04p vsilyaev
# SW7425-232: Updated to the latest mux code
# 
# 4   3/2/11 10:10p hongtaoz
# SW7425-38: use muxlib debug flags;
# 
# 3   3/1/11 6:38p hongtaoz
# SW7425-38: added compile option to file capture muxlib's debug data;
# 
# 2   12/13/10 7:02p vsilyaev
# SW7425-40: Added system initialization for the StreamMux and
#  VideoEncoder modules
# 
# 1   12/9/10 8:08p vsilyaev
# SW7425-39: Mux module
# 
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += STREAM_MUX

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_STREAM_MUX_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/stream_mux/include

##################
# Module Sources #
##################
NEXUS_STREAM_MUX_SOURCES := \
    $(NEXUS_TOP)/modules/stream_mux/src/nexus_stream_mux.c 



# thunk layer
NEXUS_STREAM_MUX_PRIVATE_INCLUDES += \
    $(NEXUS_TOP)/modules/stream_mux/src $(NEXUS_SYNCTHUNK_DIR)

NEXUS_STREAM_MUX_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_stream_mux_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_STREAM_MUX_DEPENDENCIES += BASE CORE DMA TRANSPORT VIDEO_ENCODER AUDIO

######################
# Module Definitions #
######################


##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
# Note that we have to include vbilib and vbi just to get dccparse.
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE

NEXUS_STREAM_MUX_MAGNUM_MODULES :=  \
    $(MAGNUM)/syslib/muxlib_v1243/bmuxlib_common.inc \
    $(MAGNUM)/syslib/muxlib_v1243/bmuxlib_stream_ts.inc
    
else # HUMAX_PLATFORM_BASE

NEXUS_STREAM_MUX_MAGNUM_MODULES :=  \
    $(MAGNUM)/syslib/muxlib/bmuxlib_common.inc \
    $(MAGNUM)/syslib/muxlib/bmuxlib_stream_ts.inc

endif