############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bspi.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 9/6/12 8:23p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/spi/bspi.inc $
# 
# Hydra_Software_Devel/3   9/6/12 8:23p agin
# SWNOOS-563:  Implement new directory structure.
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
MAGNUM_MODULES += BSPI

	# Module Includes
	BSPI_INCLUDES += $(MAGNUM)/portinginterface/spi_fast/include

	# Module Sources
	BSPI_SOURCES += $(MAGNUM)/portinginterface/spi_fast/src/bspi.c

