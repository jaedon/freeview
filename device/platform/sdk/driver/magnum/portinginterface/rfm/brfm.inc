############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: brfm.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 6/20/12 4:34p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/rfm/brfm.inc $
# 
# Hydra_Software_Devel/5   6/20/12 4:34p mward
# SW7420-2298: Fixed filter expression.
# 
# Hydra_Software_Devel/4   6/15/12 6:39p jtna
# SW7420-2298: flatten RFM directory structure
# 
# Hydra_Software_Devel/3   8/13/08 2:06p jtna
# PR45633: Add 7420 support
# 
# Hydra_Software_Devel/2   1/21/08 9:08p jtna
# PR34667: Add support for dual BTSC encoders
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
MAGNUM_MODULES += BRFM

# Module Includes
BRFM_INCLUDES += $(MAGNUM)/portinginterface/rfm/include

# Module Sources
BRFM_SOURCES += $(MAGNUM)/portinginterface/rfm/src/brfm.c
BRFM_SOURCES += $(MAGNUM)/portinginterface/rfm/src/brfm_scripts.c

ifeq ($(filter $(BCHP_CHIP), 7400 7420), $(BCHP_CHIP))
BRFM_SOURCES += $(MAGNUM)/portinginterface/rfm/src/bbe/brfm_scripts_bbe.c
endif