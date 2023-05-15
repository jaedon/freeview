############################################################
#     Copyright (c) 2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bdgp.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 6/21/07 1:35p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/dgp/bdgp.inc $
# 
# Hydra_Software_Devel/1   6/21/07 1:35p nissen
# PR 29766: Source files for DGP.
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BDGP

# Module Includes
BDGP_INCLUDES += $(MAGNUM)/portinginterface/dgp/$(BCHP_CHIP)

# Module Sources
BDGP_SOURCES += $(MAGNUM)/portinginterface/dgp/$(BCHP_CHIP)/bdgp.c
BDGP_SOURCES += $(MAGNUM)/portinginterface/dgp/$(BCHP_CHIP)/bdgp_priv.c

