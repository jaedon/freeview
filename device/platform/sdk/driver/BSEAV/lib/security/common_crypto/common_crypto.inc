############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: common_crypto.inc $
# $brcm_Revision: 3 $
# $brcm_Date: 7/27/12 4:48p $
#
# Module Description:
#   RMFF media library module descriptor
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/security/common_crypto/common_crypto.inc $
# 
# 3   7/27/12 4:48p cdisc
# SW7429-211: refactoring to work with flat nexus directory structure
# 
# 2   5/10/12 6:06p ajutras
# SW7125-1288: Add support for Comcast key and certificate protection
# 
# 1   4/24/12 11:10a cdisc
# SW7425-2923: adding common_crypto dir and sources
# 
# SW7425-2317/2   4/19/12 3:06p cdisc
# SW7425-2901: replacing NEXUS with NEXUS_TOP
# 
# SW7425-2317/1   4/12/12 9:37a hugost
# Created inc file for the build system.
# 
# 2   12/7/11 4:00p vsilyaev
# SW7425-1917: Added player for RMFF content
# 
# 1   2/2/10 3:50p vsilyaev
# SW7468-69: RMFF parser
# 
# 
############################################################


# COMMON CRYPTO module
COMMON_CRYPTO_INCLUDES := ${BSEAV}/lib/security/common_crypto/include \
	${NEXUS_TOP}/base/include/public \
    ${NEXUS_TOP}/modules/dma/include \
    ${NEXUS_TOP}/modules/core/include \
    ${NEXUS_TOP}/modules/security/include

    
ifeq ($(findstring $(NEXUS_PLATFORM), 97125), $(NEXUS_PLATFORM))
COMMON_CRYPTO_INCLUDES += ${NEXUS_TOP}/modules/security/keyladder_ext/include
else
COMMON_CRYPTO_INCLUDES += ${NEXUS_TOP}/extensions/security/keyladder/include
endif

COMMON_CRYPTO_SOURCES :=  \
	${BSEAV}/lib/security/common_crypto/src/common_crypto.c
