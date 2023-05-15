#############################################################################
#    (c)2009-2011 Broadcom Corporation
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
# $brcm_Revision: 9 $
# $brcm_Date: 5/25/11 4:48p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /nexus/examples/security/Makefile $
# 
# 9   5/25/11 4:48p erickson
# SW7422-430: build security examples
# 
# 8   3/22/10 6:14p ttrammel
# SW7405-3833: Modify for pre-built binary build.
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
NEXUS_TOP := $(shell cd ../.. && cd)
else
NEXUS_TOP := $(shell cd ../../; pwd)
endif

# include cross-compiler definitions
ifeq ($(NEXUS_PREBUILT_BINARY),y)
# do "make api" and "make nexus_headers" first, then "make NEXUS_PREBUILT_BINARY=y"
include $(NEXUS_TOP)/bin/include/platform_app.inc
Q_ ?= @
else
NEXUS_PLATFORM ?= $(PLATFORM)
ifndef NEXUS_PLATFORM
$(error NEXUS_PLATFORM is not defined)
endif

include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc
endif

THEAPPS = \
    decode_dvb \
    decode_aescbc \
    decode_3desecb \
    decode_dvb_samekey \
    recpump_dvb_ca_m2m_encrypt \
    decode_dvb_cps_even_odd_key \
    decode_3desecb_different_keys \
    decode_aescbc_diff_iv \
    m2m_aesecb \
    m2m_aesecb_dskey \
	m2m_aescbc_ext_key_iv \
	m2m_aescbc_scatter_gather \
	m2m_aesecb_timestamp 

ifeq ($(KEYLADDER_SUPPORT),y)
THEAPPS += \
    decode_dvb_keyladder \
    decode_aescbc_keyladder_otp_key \
    m2m_3desecb_keyladder \
    m2m_3desecb_keyladder_otp_key \
    m2m_aesecb_keyladder_otp_key \
    m2m_aescbc_keyladder

endif


include ../Makefile.inc
