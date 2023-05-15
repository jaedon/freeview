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
# $brcm_Workfile: wmdrmpd.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 8/23/12 3:57p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/wmdrmpd/wmdrmpd.inc $
# 
# 2   8/23/12 3:57p jgarrett
# SW7425-3757: Merge to main branch
# 
# SW7425-3757/1   8/15/12 4:55p hugost
# Pick up DRM piff parsing source code.
# 
# 1   7/27/12 2:26p jgarrett
# SW7425-3281: Merge to main branch
# 
# SW7425-3281/3   7/8/12 4:54p piyushg
# SW7425-3281: Added support for playready licensce acquisition and Nexus
#  fileio
# 
# SW7425-3281/2   6/21/12 5:05p jgarrett
# SW7425-3281: Initial IO implementation
# 
# SW7425-3281/1   6/20/12 2:35p piyushg
# SW7425-3281: Initial checkin of Nexus directory structure and header
#  files.
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += WMDRMPD


###################
# Module Includes #
###################
NEXUS_WMDRMPD_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/wmdrmpd/include


##################
# Module Sources #
##################
NEXUS_WMDRMPD_SOURCES := \
        $(NEXUS_TOP)/modules/wmdrmpd/src/nexus_wmdrmpd.c \
        $(NEXUS_TOP)/modules/wmdrmpd/src/nexus_wmdrmpd_module.c \
        $(NEXUS_TOP)/modules/wmdrmpd/src/$(B_REFSW_OS)/nexus_wmdrmpd_io_handler.c

ifeq ($(NEXUS_MODE),proxy)
NEXUS_WMDRMPD_SOURCES += $(BSEAV)/lib/playready/prdy_http.c
NEXUS_WMDRMPD_SOURCES += $(BSEAV)/lib/asf/asfpars.c
endif

# Pick up DRM Piff parsing module 
NEXUS_WMDRMPD_SOURCES += $(BSEAV)/lib/piff/bpiff.c

# thunk layer
NEXUS_WMDRMPD_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/wmdrmpd/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_WMDRMPD_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_wmdrmpd_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_WMDRMPD_DEPENDENCIES := BASE CORE WMDRMPD_CORE WMDRMPD_IO DMA TRANSPORT

