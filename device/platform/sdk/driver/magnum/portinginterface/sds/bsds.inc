############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bsds.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 7/19/12 6:05p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/sds/bsds.inc $
# 
# Hydra_Software_Devel/2   7/19/12 6:05p jasonh
# SW7435-263: Updating with new CHP directory structure.
# 
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BSDS

# Module Includes
BSDS_INCLUDES += $(MAGNUM)/basemodules/chp/include/4500/common
BSDS_INCLUDES += $(MAGNUM)/portinginterface/sds/4500

# Module Sources
BSDS_SOURCES += \
	$(MAGNUM)/portinginterface/sds/4500/bsds.c	 \
	$(MAGNUM)/portinginterface/sds/4500/bsds_priv.c

