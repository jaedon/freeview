#############################################################################
#    (c)2009-2010 Broadcom Corporation
# 
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
#  
# Except as expressly set forth in the Authorized License,
#  
# 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#  
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
# USE OR PERFORMANCE OF THE SOFTWARE.
# 
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
# ANY LIMITED REMEDY.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 3 $
# $brcm_Date: 6/11/10 6:53p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /nexus/examples/security/noBSP/Makefile $
# 
# 3   6/11/10 6:53p qcheng
# SW7550-102 : Add Security support for 7550
# 
# 1   4/22/10 7:13p qcheng
# SW7550-102 : Add Security support for 7550
# 
# 7   12/28/09 6:20p qcheng
# SW7550-102 : Add Security support for 7550
# 
# 6   12/10/09 10:39a yili
# SW7125-136:New sample code
# 
# 5   10/16/09 4:41p erickson
# SW7400-2511: switch to NEXUS_PLATFORM
# 
# 4   4/22/09 3:31p yili
# PR53745: Add nexus sampel code
# 
# 3   4/15/09 3:20p yili
# PR53745:Add more nexus secuirty sample code
# 
# 2   4/6/09 5:30p yili
# PR53745:Add keyladder sample code
# 
# 1   4/6/09 11:26a yili
# PR53745:Adding security sample code to Nexus
# 
#############################################################################


ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../ && cd)
else
NEXUS_TOP := $(shell cd ../../../ ; pwd)
endif

NEXUS_PLATFORM ?= $(PLATFORM)
ifndef NEXUS_PLATFORM
$(error NEXUS_PLATFORM is not defined)
endif

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc

THEAPPS = \
    decode_multi2 \
    decode_3desecb \
    decode_dvb \
    decode_3desecb_cps \
    decode_3desecb_cpd \
    decode_dvb_cps_even_odd_key 

ifeq ($(KEYLADDER_SUPPORT),y)
THEAPPS += \
    decode_dvb_keyladder \
    m2m_3desecb_keyladder \
    m2m_aescbc_keyladder
endif


include ../../Makefile.inc
