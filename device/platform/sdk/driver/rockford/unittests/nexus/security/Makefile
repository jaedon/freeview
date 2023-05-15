############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 10/12/10 6:42p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/security/Makefile $
# 
# Hydra_Software_Devel/2   10/12/10 6:42p mphillip
# SW7405-4668: Activate crypto unit tests
# 
# Hydra_Software_Devel/1   3/18/09 11:34a erickson
# PR53355: add test app
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
    security \
    encrypt_m2m_aes \
    encrypt_m2m_3des

include ../common/Makefile.inc
