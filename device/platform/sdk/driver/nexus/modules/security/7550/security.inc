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
# $brcm_Workfile: security.inc $
# $brcm_Revision: 6 $
# $brcm_Date: 7/20/12 12:45p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/security/7550/security.inc $
# 
# 6   7/20/12 12:45p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 5   7/20/12 12:27p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 4   9/16/10 12:00p cdisc
# SW35230-1387: decoupling 35230 security.inc from 7550 version
# 
# 3   9/14/10 12:30p cdisc
# SW35230-1352: adding support for nexus security library
# 
# 2   8/4/10 4:14p cdisc
# SW35230-581: enabling uesrcmd in security.inc
# 
# 1   12/7/09 11:32a qcheng
# SW7550-102 : Add Security support for 7550
# 
# 13   9/9/09 4:58p erickson
# SW7405-2964: move HSM build from transport.inc to security.inc
#
# 12   8/25/09 11:06a haoboy
# SW7405-2870: added support for secure RSA
#
# 11   8/25/09 1:09a atruong
# SW7420-223: 7420B0 ASKM Support
#
# 10   7/20/09 8:54a erickson
# PR56912: added secure access extension
#
# 9   5/6/09 3:53p yili
# PR53745:Add more nexus security sample code
#
# 8   4/2/09 1:59p yili
# PR53745:Add DS key routing
#
# 7   4/1/09 3:18p katrep
# PR53733: Fixed compilation issue when MSDRM_PD_SUPPORT is not enabled
#
# 6   3/9/09 4:31p yili
# PR52990:Adding region verification
#
# 5   12/24/08 11:56a mphillip
# PR49607: Add keyladder definition when building with keyladder support
#
# 4   8/11/08 9:41p atruong
# PR45612: Nexus Security extension for OTP/MSP
#
# 3   7/17/08 12:58p mphillip
# PR44912: Add the optional keyladder extension build hooks
#
# 2   5/28/08 2:32p mphillip
# PR38369: Refactor keyslot configuration to Security module from Crypto
#  module
#
# 1   1/18/08 2:16p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/1   11/15/07 3:54p erickson
# PR37137: added Security module
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += SECURITY

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_SECURITY_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/security/include

##################
# Module Sources #
##################
NEXUS_SECURITY_SOURCES := \
		$(NEXUS_TOP)/modules/security/7550/src/nexus_security.c \
		$(NEXUS_TOP)/modules/security/7550/src/nexus_security_misc.c

# thunk layer
NEXUS_SECURITY_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/security/7550/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_SECURITY_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_security_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_SECURITY_DEPENDENCIES := BASE CORE

##################
# Magnum Modules #
##################
#BHSM_CUST := G
#BHSM_BSECK ?=OFF
NEXUS_SECURITY_MAGNUM_MODULES =  \
    $(MAGNUM)/portinginterface/hsm/bhsm.inc

#####################
# Module Extensions #
#####################
# Note: The user is responsible for setting any other environment variables, such as BHSM_KEYLADDER=ON
ifeq ($(KEYLADDER_SUPPORT),y)
include ${NEXUS_TOP}/modules/security/keyladder_ext/keyladder_ext.inc
NEXUS_SECURITY_DEFINES += NEXUS_HAS_KEYLADDER_SUPPORT=1
endif

#ifeq ($(MSDRM_PD_SUPPORT),y)
#NEXUS_SECURITY_DEFINES += MSDRM_PD_SUPPORT=1
#endif

#ifeq ($(OTPMSP_SUPPORT),y)
#include ${NEXUS_TOP}/modules/security/7550/otpmsp_ext/otpmsp_ext.inc
#endif

#ifeq ($(REGION_VERIFICATION_SUPPORT),y)
#include ${NEXUS_TOP}/modules/security/regver_ext/regver_ext.inc
#endif

#ifeq ($(BSP_DS2_KEY_ROUTING),ON)
#NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_DS2_ROUTING
#endif

#ifeq ($(BSP_SC_VALUE_SUPPORT),ON)
#NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_SC_VALUE
#endif

#ifeq ($(BSP_M2M_EXT_KEY_IV_SUPPORT),ON)
#NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_EXT_KEY_IV
#endif

#ifeq ($(SECUREACCESS_SUPPORT),y)
#include ${NEXUS_TOP}/modules/security/secureaccess_ext/secureaccess_ext.inc
#endif

ifeq ($(USERCMD_SUPPORT),y)
include ${NEXUS_TOP}/modules/security/usercmd_ext/usercmd_ext.inc
endif

#ifeq ($(SECURERSA_SUPPORT),y)
#include ${NEXUS_TOP}/modules/security/securersa_ext/securersa_ext.inc
#endif
