############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bkir.inc $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 9/6/12 8:14p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/kir/bkir.inc $
# 
# Hydra_Software_Devel/4   9/6/12 8:14p agin
# SWNOOS-563:  Implement new directory structure.
# 
# Hydra_Software_Devel/2   6/18/10 11:14a jerrylim
# SW35230-260: Add 35230 IR support to Nexus/Magnum
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
MAGNUM_MODULES += BKIR

#
# need export BKIR_SUPPORT_TYPE_P=y
#
ifeq ($(BKIR_SUPPORT_TYPE_P), y)
BKIR_DEFINES += BKIR_SUPPORT_TYPE_P=1
endif


# Module Includes
BKIR_INCLUDES += $(MAGNUM)/portinginterface/kir/include

# Module Sources
BKIR_SOURCES += $(MAGNUM)/portinginterface/kir/src/bkir.c
