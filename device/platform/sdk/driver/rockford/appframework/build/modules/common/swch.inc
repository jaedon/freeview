#############################################################################
#
#       Copyright (c) 2005, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: swch.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 11/10/05 2:17p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/modules/common/swch.inc $
# 
# Hydra_Software_Devel/2   11/10/05 2:17p jasonh
# PR 18068: Added framework specific code to source list.
# 
# Hydra_Software_Devel/1   9/12/05 7:47p darnstein
# PR 1685: Causes the BSWCH files to compile.
# 
#############################################################################

COMMON_INCLUDES += \
	$(ROCKFORD_DIR)/commondrivers/swch/$(PLATFORM)/ \
	$(FRAMEWORK_DIR)/src/common/swch/$(PLATFORM)/

COMMON_SOURCES += \
	$(ROCKFORD_DIR)/commondrivers/swch/$(PLATFORM)/bswch.c \
	$(ROCKFORD_DIR)/commondrivers/swch/$(PLATFORM)/bswch_name.c \
	$(FRAMEWORK_DIR)/src/common/swch/$(PLATFORM)/bswchmgr.c
