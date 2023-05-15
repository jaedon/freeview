#############################################################################
#    (c)2011-2012 Broadcom Corporation
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
# $brcm_Workfile: secure_ext.inc $
# $brcm_Revision: 5 $
# $brcm_Date: 8/27/12 2:08p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /nexus/extensions/video_decoder/security/40nm/secure_ext.inc $
# 
# 5   8/27/12 2:08p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 4   6/29/11 5:20p yili
# SW7422-404: Clean up
# 
# 3   4/20/11 6:20p yili
# SW7422-404:More tests
# 
# 2   4/20/11 1:44p yili
# SW7422-404:More test cases
# 
# 1   3/23/11 11:18a yili
# SW7422-404:Video decoder security
# 
#############################################################################
###################
# Module Includes #
###################
NEXUS_VIDEO_DECODER_PRIVATE_INCLUDES += $(NEXUS_TOP)/extensions/video_decoder/security/40nm/src

##################
# Module Sources #
##################
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/extensions/video_decoder/security/40nm/src/nexus_video_decoder_security.c 

ifeq ($(VIDEO_DECODER_NO_SECURE_HEAP),y)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_VIDEO_DECODER_NO_SECURE_HEAP
endif

ifeq ($(VICH_SUPPORT),y)
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/extensions/video_decoder/security/40nm/src/nexus_video_decoder_register_protection.c  
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_ENABLE_VICH
ifeq ($(VICH_0_NEGATIVE_HOST_WRITE),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_ENABLE_VICH_0_NEGATIVE_HOST_WRITE
endif
ifeq ($(VICH_1_NEGATIVE_HOST_WRITE),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_ENABLE_VICH_1_NEGATIVE_HOST_WRITE
endif
ifeq ($(VICH_0_NEGATIVE_BAD_SIGNATURE),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_ENABLE_VICH_0_NEGATIVE_BAD_SIGNATURE
endif
ifeq ($(VICH_1_NEGATIVE_BAD_SIGNATURE),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_ENABLE_VICH_1_NEGATIVE_BAD_SIGNATURE
endif
ifeq ($(filter $(BCHP_CHIP), 7358), $(BCHP_CHIP))
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_VIDEO_DECODER_SECURITY_NO_BLD
endif
endif

ifeq ($(filter $(BCHP_CHIP), 7358), $(BCHP_CHIP))
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_VIDEO_DECODER_SECURITY_NO_SVD
endif

ifeq ($(VIDEO_REGION_VERIFICATION_SUPPORT),y)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_VIDEO_VERIFICATION
NEXUS_VIDEO_DECODER_SOURCES += \
    $(NEXUS_TOP)/extensions/video_decoder/security/40nm/src/nexus_video_decoder_region_verification.c 
ifeq ($(NEXUS_DUMP_AVD_BIN),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_VIDEO_DECODER_DUMP_FW_FILE
endif
ifeq ($(NEXUS_DUMP_AVD_BIN),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_VIDEO_DECODER_DUMP_FW_FILE
endif

#For negative test
ifeq ($(REGION_VERIFICATION_BAD_SVD_OL_SIGNATURE_TEST),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_SVD_OL_BAD_SIGNATGURE_TEST
endif
ifeq ($(REGION_VERIFICATION_BAD_SVD_IL_SIGNATURE_TEST),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_SVD_IL_BAD_SIGNATGURE_TEST
endif
ifeq ($(REGION_VERIFICATION_BAD_SVD_BL_SIGNATURE_TEST),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_SVD_BL_BAD_SIGNATGURE_TEST
endif
ifeq ($(REGION_VERIFICATION_BAD_AVD_OL_SIGNATURE_TEST),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_AVD_OL_BAD_SIGNATGURE_TEST
endif
ifeq ($(REGION_VERIFICATION_BAD_AVD_IL_SIGNATURE_TEST),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_AVD_IL_BAD_SIGNATGURE_TEST
endif
ifeq ($(REGION_VERIFICATION_VIDEO_SVD_WRCH_TEST),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_VIDEO_DECODER_SVD_WRCH_TEST
endif
ifeq ($(REGION_VERIFICATION_VIDEO_AVD_WRCH_TEST),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_VIDEO_DECODER_AVD_WRCH_TEST
endif
ifeq ($(REGION_VERIFICATION_NO_SVD_REGION_TEST),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_VIDEO_NO_SVD_REGION_TEST
endif
ifeq ($(REGION_VERIFICATION_NO_AVD_REGION_TEST),ON)
NEXUS_VIDEO_DECODER_DEFINES += NEXUS_SECURITY_VIDEO_NO_AVD_REGION_TEST
endif

endif

