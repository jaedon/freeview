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
# $brcm_Revision: 48 $
# $brcm_Date: 10/23/12 2:27a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/security/security.inc $
# 
# 48   10/23/12 2:27a atruong
# SW7435-439: To support Unified Refsw for 40-nm platforms in Nexus
#  Security/HSM PI
# 
# 47   10/3/12 11:46a atruong
# SW7346-1057: Use BCHP_CHIP instead of NEXUS_PLATFORM for option parsing
# 
# 46   9/21/12 10:47a erickson
# SW7435-246: add 93335
# 
# 45   9/21/12 10:22a cdisc
# SW7425-366: HSM reorg and libnexus_security update
# 
# 44   9/11/12 4:42p atruong
# SWSECURITY-175: Remove symbolic links in HSM PI
# 
# 43   7/20/12 12:27p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 42   7/16/12 11:13a erickson
# SWSECURITY-53: add 7358
# 
# 41   6/20/12 2:30p erickson
# SW7231-865: add 97231, add NEXUS_SECURITY_WEBCPU_EXTENSION_INC
# 
# 40   3/30/12 4:02a atruong
# SW7435-6: Add Nexus and HSM PI Security Support
# 
# 39   3/12/12 4:41p gkamosa
# SW7425-2543: Merge support for 3383 SMS platform
# 
# 38   3/5/12 3:04p atruong
# SWSECURITY-113: Secure RSA2 Support for 40-nm Platforms
# 
# 38   3/5/12 3:02p atruong
# SWSECURITY-113: Secure RSA2 Support for 40-nm Platforms
# 
# 37   2/28/12 11:00a mward
# SW7435-7: Add 7435.
# 
# 36   2/8/12 12:44p erickson
# SW7425-1932: use NEXUS_SECURITY_DEFINES instead of NEXUS_CFLAGS
# 
# 35   2/7/12 1:29p katrep
# SW7429-6:build issue for 7428,7241
# 
# 34   2/6/12 2:48p atruong
# SW7429-6: HSM PI/Nexus Security Support for 7429A0
# 
# Centaurus_Phase_1.0/1   2/16/12 1:11p jmiddelk
# SW7425-2337: Add support for 3383sms platform
# 
# 33   12/19/11 2:42p mphillip
# SW7425-1932: Add feature defines for Nexus Security platform features
# 
# 32   11/3/11 5:38p mward
# SW7435-7:  Add 7435.
# 
# 31   10/17/11 5:42p atruong
# SWSECURITY-69: Support for 40-nm B0 Zeus 2.0 platforms
# 
# 30   10/13/11 10:01a atruong
# SWSECURITY-69: Support BSP Zeus 2.0 FW
# 
# 29   9/21/11 3:49p atruong
# SW7346-495: Add NSK2AVK support
# 
# 28   8/5/11 12:06p yili
# SW7422-404:Changed to match source code change
# 
# 27   6/30/11 2:44a atruong
# SW7552-41: Nexus Security Support For BCM7552 A0
# 
# 26   6/21/11 4:07p bselva
# SW7358-50:Enable Nexus security support for 7358 platform
# 
# 25   3/30/11 10:24a jrubio
# SW7346-111: set CUST?=G
# 
# 24   3/11/11 4:51p katrep
# SW7231-47:fix support for 97230
# 
# 23   3/7/11 11:48a randyjew
# SW7344-9:Add 97418 support
# 
# 22   2/23/11 11:14a mphillip
# SW7422-253: Merge API changes to main
# 
# SW7422-253/1   2/15/11 7:53p mphillip
# SW7422-253: Add support for new extension conventions
# 
# 21   2/16/11 11:06p hongtaoz
# SW7425-103: add security support for 7425;
# 
# SW7422-253/1   2/15/11 7:53p mphillip
# SW7422-253: Add support for new extension conventions
# 
# 20   12/22/10 5:48p mphillip
# SW7422-104: Merge 40nm support to /main
# 
# SW7422-104/1   12/16/10 6:14p mphillip
# SW7422-104: Initial support for 7422
# 
# 19   8/2/10 5:47p atruong
# SW7405-4696: Adding Security IP Licensing module for all platforms
# 
# 18   3/15/10 9:57a atruong
# SW7400-2678: Nexus AV Keyladder to support DirecTV
# 
# 17   3/14/10 11:32p atruong
# SW7400-2678: Nexus AV Keyladder to support DirecTV
# 
# 16   11/24/09 1:06p erickson
# SW7420-457: add REGION_VERIFICATION_SUPPORT compile time options
#
# 15   10/1/09 1:46p yili
# SW7340-42:Folder rename for raw command EXT
#
# 14   9/30/09 3:21p yili
# SW7340-42:Add raw command interface
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
# Module Features #
###################


# Compile-time variables to cover specific features
ifeq ($(filter $(BCHP_CHIP), 7400 7401 7405 7325 7335 7125 7340 7342 7420), $(BCHP_CHIP))
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_CHIP_SIZE=65
endif
ifeq ($(filter $(BCHP_CHIP), 7231 7344 7346 7358 7360 7422 7425 7429 7428 7241 7435 7552), $(BCHP_CHIP))
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_CHIP_SIZE=40
endif
ifeq ($(filter $(BCHP_CHIP), 7125 7231 7340 7342 7344 7346 7358 7360 7420 7422 7425 7429 7428 7241 7435 7552), $(BCHP_CHIP))
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_HAS_ASKM=1
endif


###################
# Module Includes #
###################
NEXUS_SECURITY_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/security/include

##################
# Module Sources #
##################
NEXUS_SECURITY_SOURCES := \
		$(NEXUS_TOP)/modules/security/src/nexus_security.c \
		$(NEXUS_TOP)/modules/security/src/nexus_security_misc.c

# thunk layer
NEXUS_SECURITY_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/security/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_SECURITY_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_security_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_SECURITY_DEPENDENCIES := BASE CORE

##################
# Magnum Modules #
##################
BHSM_CUST ?= G
BHSM_BSECK ?=OFF

NEXUS_SECURITY_MAGNUM_MODULES =  \
    $(MAGNUM)/portinginterface/hsm/bhsm.inc

ifeq ($(filter $(BCHP_CHIP), 7422 7425 7435 7344 7346 7418 7231 7230 7358 7360 7552 7429 7428 7241), $(BCHP_CHIP))
# Make this a default for 40-nm chips
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_SC_VALUE
endif
#####################
# Module Extensions #
#####################
# Note: The user is responsible for setting any other environment variables, such as BHSM_KEYLADDER=ON
ifeq ($(HUMAX_PLATFORM_BASE), y)
BSP_M2M_EXT_KEY_IV_SUPPORT=ON
NEXUS_COMMON_CRYPTO_SUPPORT=y

ifeq ($(HSM_LIBCRC),y)
MAGNUM_PRECOMPILED_OBJECTS += $(MAGNUM)/portinginterface/hsm/src/core40nm/otpmsp/crc/libcrc.a
endif
endif

ifeq ($(KEYLADDER_SUPPORT),y)
include ${NEXUS_TOP}/modules/security/keyladder_ext/keyladder_ext.inc
NEXUS_SECURITY_DEFINES += NEXUS_HAS_KEYLADDER_SUPPORT=1
endif

ifeq ($(MSDRM_PD_SUPPORT),y)
NEXUS_SECURITY_DEFINES += MSDRM_PD_SUPPORT=1
endif

ifeq ($(OTPMSP_SUPPORT),y)
include ${NEXUS_TOP}/modules/security/otpmsp_ext/otpmsp_ext.inc
endif

ifeq ($(REGION_VERIFICATION_SUPPORT),y)
include ${NEXUS_TOP}/modules/security/regver_ext/regver_ext.inc
# NEXUS_SECURITY_REGION_VERIFICATION is for Nexus-level source code (including
# transport and extensions)
# BSP_REGION_VERIFICATION is passed to HSM for controls there
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_REGION_VERIFICATION=1
NEXUS_CFLAGS +=  -DBSP_REGION_VERIFICATION
endif

ifeq ($(BSP_DS2_KEY_ROUTING),ON)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_DS2_ROUTING
endif

ifeq ($(BSP_SC_VALUE_SUPPORT),ON)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_SC_VALUE
endif

ifeq ($(BSP_M2M_EXT_KEY_IV_SUPPORT),ON)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_EXT_KEY_IV
endif

ifeq ($(SECUREACCESS_SUPPORT),y)
include ${NEXUS_TOP}/modules/security/secureaccess_ext/secureaccess_ext.inc
endif

ifeq ($(USERCMD_SUPPORT),y)
include ${NEXUS_TOP}/modules/security/usercmd_ext/usercmd_ext.inc
endif

ifeq ($(SECURERSA_SUPPORT),y)
include ${NEXUS_TOP}/modules/security/securersa_ext/securersa_ext.inc
endif

ifeq ($(NEXUS_SECURITY_RAWCMD),ON)
include ${NEXUS_TOP}/modules/security/rawcommand_ext/nexus_security_rawcommand.inc
endif

ifeq ($(IPLICENSING_SUPPORT),y)
include ${NEXUS_TOP}/modules/security/iplicensing_ext/iplicensing_ext.inc
endif
ifneq ($(NEXUS_SECURITY_SECUREACCESS_EXTENSION_INC),)
include $(NEXUS_SECURITY_SECUREACCESS_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_SECUREACCESS=1
endif

ifneq ($(NEXUS_SECURITY_SECURERSA_EXTENSION_INC),)
include $(NEXUS_SECURITY_SECURERSA_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_SECURERSA=1
endif

ifneq ($(NEXUS_SECURITY_IPLICENSING_EXTENSION_INC),)
include $(NEXUS_SECURITY_IPLICENSING_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_IPLICENSING=1
endif

ifneq ($(NEXUS_SECURITY_AVKEYLADDER_EXTENSION_INC),)
include $(NEXUS_SECURITY_AVKEYLADDER_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_AVKEYLADDER=1
endif

ifneq ($(NEXUS_SECURITY_KEYLADDER_EXTENSION_INC),)
include $(NEXUS_SECURITY_KEYLADDER_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_KEYLADDER=1
NEXUS_SECURITY_DEFINES += NEXUS_HAS_KEYLADDER_SUPPORT=1
endif

ifneq ($(NEXUS_SECURITY_OTPMSP_EXTENSION_INC),)
include $(NEXUS_SECURITY_OTPMSP_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_OTPMSP=1
endif

ifneq ($(NEXUS_SECURITY_USERCMD_EXTENSION_INC),)
include $(NEXUS_SECURITY_USERCMD_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_USERCMD=1
endif

ifneq ($(NEXUS_SECURITY_HSMRAWCMD_EXTENSION_INC),)
include $(NEXUS_SECURITY_HSMRAWCMD_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_RAWCMD=1
NEXUS_CFLAGS +=  -DBHSM_AUTO_TEST
endif

ifneq ($(NEXUS_SECURITY_REGVERIFY_EXTENSION_INC),)
include $(NEXUS_SECURITY_REGVERIFY_EXTENSION_INC)
NEXUS_CFLAGS +=  -DBSP_REGION_VERIFICATION
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_REGION_VERIFICATION=1
endif

ifneq ($(NEXUS_SECURITY_WEBCPU_EXTENSION_INC),)
include $(NEXUS_SECURITY_WEBCPU_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_WEBCPU=1
endif

ifneq ($(NEXUS_SECURITY_GENROOTKEY_EXTENSION_INC),)
include $(NEXUS_SECURITY_GENROOTKEY_EXTENSION_INC)
NEXUS_SECURITY_DEFINES += NEXUS_SECURITY_GENROOTKEY=1
endif

# ericz - 12/14/2011 - CSP472766 - refactor for NEXUS_EXTRALIBS and HSM_SOURCE_AVAILABLE
# populate default settings for NEXUS_EXTRALIBS
# $(B_REFSW_OS) : linuxkernel   or   linuxuser
# $(ZEUSVER)    : defined in bhsm_zeus.inc
ifneq ($(NEXUS_EXTRA_SECLIBS_FILES),)

ifeq ($(DEBUG),n)
NEXUS_EXTRA_SECLIBS_DIR ?= ${NEXUS_TOP}/modules/security/lib/${ZEUSVER}/${B_REFSW_OS}/retail
else
NEXUS_EXTRA_SECLIBS_DIR ?= ${NEXUS_TOP}/modules/security/lib/${ZEUSVER}/${B_REFSW_OS}/debug
endif

NEXUS_EXTRALIBS += ${NEXUS_EXTRA_SECLIBS_DIR}/${NEXUS_EXTRA_SECLIBS_FILES}

endif

ifeq ($(HSM_SOURCE_AVAILABLE),n)
NEXUS_SECURITY_SOURCES := $(filter %_thunks.c, $(NEXUS_SECURITY_SOURCES))
endif
