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
# $brcm_Workfile: sw_rave.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 4/12/11 10:28a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/extensions/transport/sw_rave/sw_rave.inc $
# 
# 2   4/12/11 10:28a erickson
# SW7342-331: switch SW RAVE extension to use
#  BXPT_RaveSoftMode_ePointersOnly, fail on any codec that requires ITB
#  manipulation. rename to NEXUS_SW_RAVE_PEEK_EXTENSION.
# 
# 1   2/28/11 11:06a erickson
# SW7342-331: add template SW RAVE extension
#
############################################################

#
# Template for transport SW rave extension
#

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_TRANSPORT_PRIVATE_INCLUDES += $(NEXUS_TOP)/extensions/transport/sw_rave/src

# NEXUS_TRANSPORT_PUBLIC_INCLUDES not needed. no public API.

##################
# Module Sources #
##################
NEXUS_TRANSPORT_SOURCES += \
    $(NEXUS_TOP)/extensions/transport/sw_rave/src/nexus_sw_rave_extension.c

# ePointersOnly peeking at ITB
NEXUS_TRANSPORT_DEFINES += NEXUS_SW_RAVE_PEEK_EXTENSION=1

#######################
# Module Dependencies #
#######################
# add additional dependencies here
#NEXUS_TRANSPORT_DEPENDENCIES +=

##################
# Magnum Modules #
##################
# add additional magnum modules here
#NEXUS_TRANSPORT_MAGNUM_MODULES +=