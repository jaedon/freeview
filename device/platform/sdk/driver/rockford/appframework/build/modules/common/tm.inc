#############################################################################
#
#       Copyright (c) 2005-2006, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: tm.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/27/06 4:58p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/modules/common/tm.inc $
# 
# Hydra_Software_Devel/2   9/27/06 4:58p jessem
# PR 23955: Modified tm.inc to support 97398.
# 
# Hydra_Software_Devel/1   4/10/06 2:13p syang
# PR 20748: init version for bring up I2C
# 
#############################################################################


ifeq ($(PLATFORM), 97398)
COMMON_INCLUDES += $(ROCKFORD_DIR)/bsp/bcm97038/common/
COMMON_SOURCES += \
	$(ROCKFORD_DIR)/bsp/bcm97038/common/bcmtm.c
else
COMMON_INCLUDES += $(ROCKFORD_DIR)/bsp/bcm$(PLATFORM)/common/
COMMON_SOURCES += \
	$(ROCKFORD_DIR)/bsp/bcm$(PLATFORM)/common/bcmtm.c
endif	
