############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: baus.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 7/19/12 5:55p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/aus/baus.inc $
# 
# Hydra_Software_Devel/5   7/19/12 5:55p jasonh
# SW7435-263: Updating with new CHP directory structure.
# 
# Hydra_Software_Devel/4   3/3/09 4:11p vishk
# PR 46431: DS PI: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/2   1/17/07 3:53p mward
# PR27004: Missing $(MAGNUM)/.
# 
# Hydra_Software_Devel/1   1/11/07 2:40p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BAUS

# Module Includes
BAUS_INCLUDES += $(MAGNUM)/portinginterface/aus/$(BAUS_CHIP)
BAUS_INCLUDES += $(MAGNUM)/basemodules/chp/include/$(BAUS_CHIP)/common     

# Module Sources
BAUS_SOURCES  += \
     $(MAGNUM)/portinginterface/aus/$(BAUS_CHIP)/baus.c

