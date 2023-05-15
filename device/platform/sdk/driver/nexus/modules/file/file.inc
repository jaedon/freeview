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
# $brcm_Workfile: file.inc $
# $brcm_Revision: 12 $
# $brcm_Date: 7/16/12 5:06p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/file/file.inc $
# 
# 12   7/16/12 5:06p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 11   9/6/11 10:10a erickson
# SWNOOS-482: correct internal spelling
# 
# 10   4/27/11 6:20p vsilyaev
# SW7425-232: Promote ATOM module to separate component
# 
# 9   2/3/11 2:31p vsilyaev
# SW7420-1457: Compile media related file modules
# 
# 8   1/19/11 2:02p erickson
# SW7420-1123: socket-based usermode IPC
#
# 7   9/22/10 6:59p vsilyaev
# SW7405-4419: Added API for file editing
#
# 6   9/22/10 12:39p jtna
# SW7405-4865: moved bvlc.c and .h to magnum/commonutils/vlc
#
# 5   4/22/10 6:57p mphillip
# SW7405-4251: Merge chunked PVR support from branch to main
#
# SW7405-3587/1   2/4/10 10:10a mphillip
# SW7405-3587: Add fifo chunk build support
#
# 4   9/18/09 5:04p erickson
# SW7405-3025: allow File module to build without Playback module
#
# 3   9/16/09 1:40p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 2   9/1/09 3:52p jgarrett
# SW7335-529: Replacing direct access to MAGNUM_MODULES with
#  NEXUS_EXTRA_MODULES to avoid magnum paths leaking into application
#  include paths
#
# 1   1/18/08 2:14p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/2   11/13/07 11:12a vsilyaev
# PR 37015: Initial checking for timeshifting/FIFO file
#
# Nexus_Devel/1   10/10/07 4:02p vsilyaev
# PR 35824: File I/O module
#
#
############################################################


#
# Conditional Options for this Makefile
#
# NEXUS_OS - OS to be used
#


######################
# Define this module #
######################
NEXUS_MODULES += FILE

##################
# Module Options #
##################

# alias certain OS values
ifeq ($(B_REFSW_OS),linuxemu)
NEXUS_FILE_OS=linuxuser
else
NEXUS_FILE_OS=$(B_REFSW_OS)
endif

###################
# Module Includes #
###################
NEXUS_FILE_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/file/include $(NEXUS_TOP)/modules/file/include/$(NEXUS_FILE_OS)
NEXUS_FILE_PRIVATE_INCLUDES = \
    $(NEXUS_TOP)/modules/file/src \
    $(NEXUS_TOP)/modules/file/src/$(NEXUS_FILE_OS) \
    ${BSEAV}/lib/bcmplayer/include 

##################
# Module Sources #
##################
NEXUS_FILE_SOURCES = \
    $(NEXUS_TOP)/modules/file/src/nexus_file.c \
    $(NEXUS_TOP)/modules/file/src/nexus_file_module.c \
    $(NEXUS_TOP)/modules/file/src/nexus_file_scheduler.c \
    $(NEXUS_TOP)/modules/file/src/nexus_file_chunk.c \
    $(NEXUS_TOP)/modules/file/src/nexus_file_fifo.c \
    $(NEXUS_TOP)/modules/file/src/nexus_file_fifo_chunk.c \
    $(NEXUS_TOP)/modules/file/src/$(NEXUS_FILE_OS)/nexus_file_posix.c

# thunk layer
NEXUS_FILE_PRIVATE_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)
NEXUS_FILE_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_file_thunks.c


##################
# Module Defines #
##################

#######################
# Module Dependencies #
#######################
NEXUS_FILE_DEPENDENCIES := CORE

NEXUS_FILE_MAGNUM_MODULES :=  \
    ${BSEAV}/lib/utils/batom.inc \
    ${BSEAV}/lib/bfile/bfile.inc

B_NAV_PLAYER_SOURCES = \
    ${BSEAV}/lib/bcmplayer/src/bcmplayer.c \
    ${BSEAV}/lib/bcmplayer/src/bcmindexer_vc1.c \
    ${BSEAV}/lib/bcmplayer/src/bcmindexer.c

B_NAV_PLAYER_INCLUDES = \
    ${BSEAV}/lib/bcmplayer/include

# BSEAV makefiles don't include their module names into MAGNUM_MODULES
NEXUS_EXTRA_MODULES += BATOM BFILE BFILE_MEDIA

# if nexus is compiled without playback module, file needs to include bcmplayer code
NEXUS_EXTRA_MODULES += B_NAV_PLAYER
