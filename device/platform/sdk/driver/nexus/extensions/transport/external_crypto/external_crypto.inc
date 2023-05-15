############################################################
#     (c)2007-2008 Broadcom Corporation
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
# $brcm_Workfile: external_crypto.inc $
# $brcm_Revision: 1 $
# $brcm_Date: 11/17/08 11:28a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/extensions/transport/external_crypto/external_crypto.inc $
# 
# 1   11/17/08 11:28a erickson
# PR48845: added NEXUS_PLAYPUMP_EXTERNAL_CRYPTO extension
#
# 1   8/21/08 3:03p erickson
# PR45959: add sample extension
#
############################################################

#
# Sample extension for display module
#

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_TRANSPORT_PRIVATE_INCLUDES += $(NEXUS_TOP)/extensions/transport/external_crypto/src

# NEXUS_TRANSPORT_PUBLIC_INCLUDES not needed. no public API.

##################
# Module Sources #
##################
NEXUS_TRANSPORT_SOURCES += \
    $(NEXUS_TOP)/extensions/transport/external_crypto/src/nexus_playpump_external_crypto.c

NEXUS_TRANSPORT_DEFINES += NEXUS_PLAYPUMP_EXTERNAL_CRYPTO=1

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