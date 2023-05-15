############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bscd.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 10/4/12 3:14p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/scd/bscd.inc $
# 
# Hydra_Software_Devel/3   10/4/12 3:14p katrep
# SW7445-33:add 7445
# 
# Hydra_Software_Devel/2   9/7/12 10:55a qxie
# SW7429-283: Smartcard: remove symlink in PI code
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
MAGNUM_MODULES += BSCD

# Module Includes
BSCD_INCLUDES += $(MAGNUM)/portinginterface/scd/include
BSCD_INCLUDES += $(MAGNUM)/portinginterface/scd/src/common

BSCD_SOURCES += $(MAGNUM)/portinginterface/scd/src/common/bscd_emvpriv.c

# Module Sources
ifeq ($(findstring $(BCHP_CHIP), 7425 7422 7358 7552 7344 7640 7360 7435 7231 7346 7429 7584 7563 7445), $(BCHP_CHIP))
	BSCD_INCLUDES += $(MAGNUM)/portinginterface/scd/src/core40nm

	BSCD_SOURCES += $(MAGNUM)/portinginterface/scd/src/core40nm/bscd.c
	BSCD_SOURCES += $(MAGNUM)/portinginterface/scd/src/core40nm/bscd_priv.c
else
	BSCD_INCLUDES += $(MAGNUM)/portinginterface/scd/src/core65nm

	BSCD_SOURCES += $(MAGNUM)/portinginterface/scd/src/core65nm/bscd.c
	BSCD_SOURCES += $(MAGNUM)/portinginterface/scd/src/core65nm/bscd_priv.c
endif

