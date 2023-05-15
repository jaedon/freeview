############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmem.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 1/11/07 2:37p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/mem/bmem.inc $
# 
# Hydra_Software_Devel/1   1/11/07 2:37p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BMEM

# Module Includes
BMEM_INCLUDES += $(MAGNUM)/basemodules/mem 

# Module Sources
BMEM_SOURCES += $(MAGNUM)/basemodules/mem/bmem.c
BMEM_SOURCES += $(MAGNUM)/basemodules/mem/bmem_debug.c
BMEM_SOURCES += $(MAGNUM)/basemodules/mem/bmem_local.c
BMEM_SOURCES += $(MAGNUM)/basemodules/mem/bmem_system.c
