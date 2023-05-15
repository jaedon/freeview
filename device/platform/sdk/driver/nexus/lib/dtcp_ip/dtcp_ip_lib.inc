#############################################################################
#    (c)2009-2012 Broadcom Corporation
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
# $brcm_Workfile: dtcp_ip_lib.inc $
# $brcm_Revision: 27 $
# $brcm_Date: 10/4/12 11:29a $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /nexus/lib/dtcp_ip/dtcp_ip_lib.inc $
# 
# 27   10/4/12 11:29a cdisc
# SW7425-3979: DTCP-IP streamlining
# 
# SW7425-3979/1   9/28/12 2:35p cdisc
# SW7425-3979: making common DRM default for dtcp_ip
# 
# 26   9/11/12 2:56p leisun
# SWSECURITY-48: Auto-generating test keys, consolidate test/production
#  build, make common DRM default ON
# 
# 25   7/20/12 1:38p leisun
# SW7425-2894: Don't compile decrypto.c when building non-drm/testing key
#  mode
# 
# 24   5/31/12 7:20p ssood
# SW7425-2894: modify makefiles to use the latest OpenSSL lib
# 
# 23   5/23/12 11:54a celysee
# SW7425-2894: Added include file
# 
# 22   5/18/12 3:41p celysee
# SW7425-2894: dtcp-ip suppor (private)
# 
# 21   5/2/12 2:49p leisun
# SW7425-2894: remove DTCP_IP_TEST flag
# 
# 20   5/2/12 12:23p cdisc
# SWSECURITY-142: making bcrypt headers visible
# 
# 19   5/1/12 6:04p leisun
# SW7425-2894: Build is broken with default mode(Testing)
# 
# 18   4/26/12 1:34p leisun
# SW7425-2894: Fix makefile after DRM integration
# 
# 17   4/18/12 6:16p celysee
# SW7425-2894: Support for DTCP-IP with robustness rules
# 
# 16   2/11/11 5:39p leisun
# SWSECURITY-32: Updated lib to V1SE1.3 conformance.
# 
# 15   7/14/10 3:41p leisun
# SW7420-561: production key fix
# 
# 14   7/14/10 3:34p leisun
# SW7420-561: production key fix
# 
# 13   7/6/10 3:01p leisun
# SW7420-561: Added key encryption support, fix makefile issue
# 
# 12   7/2/10 2:29p leisun
# SW7420-561: Added key encryption support
# 
# 11   4/8/10 6:00p sgundime
# SW7420-689:CLOSE_SOCKET_ON_AKE_OK - made it always on. No need to set
#  DTCP_IP_CLOSE_SOCKET_ON_AKE_OK.
# 
# 10   12/30/09 9:27p dliu
# SW7405-3400: Make demo mode default off
# 
# 9   12/23/09 4:09p leisun
# SW7420-502: conditional compile for DTCP-IP
# 
# 8   12/23/09 3:10p leisun
# SW7420-502: conditional compile for DTCP-IP
# 
# 7   8/7/09 5:55p lwhite
# PR55693: PCP 14 byte fix
# 
# 6   7/31/09 4:04p leisun
# PR 55693: removed bcrypt dependency
# 
# 5   6/30/09 10:24p leisun
# PR 55693: Change DTCP_IP_HW_ENCRYPTION/DECRYPTION to
#  B_DTCP_IP_HW_ENCRYPTION/DECRYPTION
# 
# 4   6/26/09 11:28a ssood
# PR53773: Added include path for Nexus Security module
# 
# 3   6/25/09 6:46p ssood
# PR53773: Added include path for OpenSSL include files
# 
# 2   6/25/09 2:55a ssood
# PR53773: turning off DTCP debug logs
# 
# 1   6/5/09 4:06p leisun
# PR 55693: checking in DTCP-IP lib
# 
#############################################################################

#Identify this library
#
B_LIBS += B_DTCP_IP_LIB

#
#Define source files in this library
#
DTCP_TOP_DIR := $(B_LIB_TOP)/dtcp_ip

B_DTCP_IP_LIB_SOURCES = $(DTCP_TOP_DIR)/src/b_dtcp_ake.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_ake_msg.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_applib.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_applib_priv.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_content.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_exch_key.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_ip_ake.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_ip_ake_msg.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_ip_stack.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_ip_transport.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_ip_transport_priv.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_srm.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_stack.c \
						$(DTCP_TOP_DIR)/src/b_dtcp_stream.c \
						$(DTCP_TOP_DIR)/src/b_ecc_wrapper.c \
						$(DTCP_TOP_DIR)/src/b_socket_wrapper.c \

#
#Define public include path for this library
#
B_DTCP_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/os/include \
						$(B_LIB_TOP)/os/include/linuxuser \
						$(B_LIB_TOP)/../../BSEAV/lib/openssl/$(PLATFORM)/include \
						$(B_LIB_TOP)/../../BSEAV/lib/security/bcrypt/include \

#
#Define private include path(s) for this library
#
B_DTCP_IP_LIB_PRIVATE_INCLUDES += $(DTCP_TOP_DIR)/include

#
#Defines extra C definations, These are from original DTCP package.
#NOTE: the DTCP_IP_HARDWARE_DECRYPTION need to be defined, if you wan to use
#Broadcom's M2M hardware to decrypt DTCP content stream.
#
B_DTCP_IP_LIB_DEFINES += LINUX_BUILD _POSIX \
								__cdecl="" \
								__stdcall="" 


ifeq ($(DTCP_IP_HARDWARE_DECRYPTION), y)
B_DTCP_IP_LIB_DEFINES += B_DTCP_IP_HW_DECRYPTION
endif

ifeq ($(DTCP_IP_HARDWARE_ENCRYPTION), y)
B_DTCP_IP_LIB_DEFINES += B_DTCP_IP_HW_ENCRYPTION
endif

#ifeq ($(DTCP_IP_CLOSE_SOCKET_ON_AKE_OK), y)
# Make it default on!
B_DTCP_IP_LIB_DEFINES += CLOSE_SOCKET_ON_AKE_OK
#endif

ifneq ($(DTCP_IP_DATA_BRCM), n)
B_DTCP_IP_LIB_DEFINES += B_DTCP_IP_DATA_BRCM
#Using common DRM for content encryption/decryption
B_DTCP_IP_LIB_DEFINES += B_DTCP_IP_COMMON_DRM_CONTENT_SUPPORT
endif #DTCP_IP_DATA_BRCM

#build with common DRM key provisioning by default.
B_DTCP_IP_LIB_SOURCES += $(B_LIB_TOP)/../../BSEAV/lib/security/third_party/dtcp_ip/drm_dtcp_ip_vendor.c 

B_DTCP_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/security/common_drm/include \
                                 $(B_LIB_TOP)/../../BSEAV/lib/security/third_party/dtcp_ip \
                                 $(B_LIB_TOP)/../../BSEAV/lib/security/common_crypto/include 

#build with legacy production key provisioning if requested.
ifeq ($(DTCP_IP_LEGACY_PRODUCTION_KEY), y)
B_DTCP_IP_LIB_DEFINES += B_DTCP_IP_LEGACY_PRODUCTION_KEY
B_DTCP_IP_LIB_SOURCES += $(DTCP_TOP_DIR)/src/decrypto.c 
                         
B_DTCP_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../nexus/modules/security/$(BCHP_CHIP)/include \
								$(B_LIB_TOP)/../../nexus/extensions/security/keyladder/$(BCHP_CHIP)/include 
endif


#This is hack to get the C++ style comment in the lib to compile!
B_LIB_CFLAGS += -std=c99
