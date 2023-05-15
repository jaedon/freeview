############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bkpd.inc $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 9/6/12 8:46p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/kpd/bkpd.inc $
# 
# Hydra_Software_Devel/4   9/6/12 8:46p agin
# SWNOOS-563:  Implement new directory structure.
# 
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# BKPD_3250_SUPPORT - If defined, build for the 3250 keypad as opposed to the chip specified by BCHP_CHIP.
#

# Define this module
MAGNUM_MODULES += BKPD

# Module Includes
ifneq ($(BKPD_3250_SUPPORT),)
	BKPD_INCLUDES += $(MAGNUM)/portinginterface/kpd/3250
else
	BKPD_INCLUDES += $(MAGNUM)/portinginterface/kpd/include
endif

# Module Sources
ifneq ($(BKPD_3250_SUPPORT),)
	BKPD_SOURCES += $(MAGNUM)/portinginterface/kpd/3250/bkpd.c
else
	BKPD_SOURCES += $(MAGNUM)/portinginterface/kpd/src/bkpd.c
endif

