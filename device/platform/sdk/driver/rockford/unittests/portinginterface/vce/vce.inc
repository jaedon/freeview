#############################################################################
#
#       Copyright (c) 2003-2011, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: vce.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 12/7/11 2:21p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/portinginterface/vce/vce.inc $
# 
# Hydra_Software_Devel/2   12/7/11 2:21p nilesh
# SW7425-1877: Added XUDlib support
# 
# Hydra_Software_Devel/1   4/7/11 5:07p nilesh
# SW7425-297: Added vce.inc
# 
#############################################################################


include $(MAGNUM_DIR)/portinginterface/vce/bvce.inc
include $(MAGNUM_DIR)/syslib/xudlib/bxudlib.inc
        
COMMON_INCLUDES += $(ROCKFORD_DIR)/unittests/portinginterface/vce

COMMON_SOURCES += \
	$(ROCKFORD_DIR)/unittests/portinginterface/vce/vce_encoder.c