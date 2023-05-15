############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: blst.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 7/6/12 4:16p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/commonutils/lst/blst.inc $
# 
# Hydra_Software_Devel/2   7/6/12 4:16p vsilyaev
# SW7435-256: Updated name space and added documentation
# 
# Hydra_Software_Devel/1   1/11/07 2:38p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BLST

# Module Includes
BLST_INCLUDES := $(MAGNUM)/commonutils/lst

# Module Sources
BLST_SOURCES := $(MAGNUM)/commonutils/lst/blst_aa_tree.c


