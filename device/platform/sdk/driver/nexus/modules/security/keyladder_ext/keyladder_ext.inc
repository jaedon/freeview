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
# $brcm_Workfile: keyladder_ext.inc $
# $brcm_Revision: 5 $
# $brcm_Date: 7/20/12 12:43p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/security/keyladder_ext/keyladder_ext.inc $
# 
# 5   7/20/12 12:43p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 4   7/20/12 12:27p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 3   9/23/11 6:30p atruong
# SW7346-495: HSM_SOURCE_AVAILABLE option fix
# 
# 2   6/7/10 9:18a erickson
# SW35230-168: add HSM_SOURCE_AVAILABLE=y option to allow for binary
#  delivery of HSM extension
#
# 1   7/17/08 12:56p mphillip
# PR44912: Initial commit
#
#
############################################################

###################
# Module Includes #
###################
NEXUS_SECURITY_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/security/keyladder_ext/include

##################
# Module Sources #
##################

# if HSM_SOURCE_AVAILABLE=n is set, you must set NEXUS_EXTRALIBS to
# include the pre-built hsm binary for this extension
ifneq ($(HSM_SOURCE_AVAILABLE),n)
ifeq ($(filter $(BCHP_CHIP),7550 7408),$(BCHP_CHIP))
NEXUS_SECURITY_SOURCES += $(NEXUS_TOP)/modules/security/keyladder_ext/src/7550/nexus_keyladder.c
else
NEXUS_SECURITY_SOURCES += $(NEXUS_TOP)/modules/security/keyladder_ext/src/nexus_keyladder.c
endif
endif

##################
# Module Defines #
##################
NEXUS_SECURITY_DEFINES += NEXUS_KEYLADDER=1