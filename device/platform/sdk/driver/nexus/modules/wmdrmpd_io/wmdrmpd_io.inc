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
# $brcm_Workfile: wmdrmpd_io.inc $
# $brcm_Revision: 1 $
# $brcm_Date: 7/27/12 2:26p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/wmdrmpd_io/wmdrmpd_io.inc $
# 
# 1   7/27/12 2:26p jgarrett
# SW7425-3281: Merge to main branch
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
NEXUS_MODULES += WMDRMPD_IO


###################
# Module Includes #
###################
NEXUS_WMDRMPD_IO_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/wmdrmpd_io/include


##################
# Module Sources #
##################
NEXUS_WMDRMPD_IO_SOURCES := \
        $(NEXUS_TOP)/modules/wmdrmpd_io/src/nexus_wmdrmpd_io.c \
        $(NEXUS_TOP)/modules/wmdrmpd_io/src/nexus_wmdrmpd_io_module.c

# thunk layer
NEXUS_WMDRMPD_IO_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/wmdrmpd_io/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_WMDRMPD_IO_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_wmdrmpd_io_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_WMDRMPD_IO_DEPENDENCIES := BASE CORE
