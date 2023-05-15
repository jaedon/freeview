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
# $brcm_Workfile: fpga.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 4/10/06 3:32p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/modules/common/fpga.inc $
# 
# Hydra_Software_Devel/2   4/10/06 3:32p darnstein
# PR20748: rearrange fpga includes.
# 
# Hydra_Software_Devel/1   9/1/05 9:47a jasonh
# PR 16943:
# 
#############################################################################

ifneq "$(PLATFORM)" "97398"
COMMON_INCLUDES += $(ROCKFORD_DIR)/commondrivers/fpga/$(PLATFORM)/

COMMON_SOURCES += \
	$(ROCKFORD_DIR)/commondrivers/fpga/$(PLATFORM)/bfpga.c \
	$(ROCKFORD_DIR)/commondrivers/fpga/$(PLATFORM)/bfpga_name.c
endif
