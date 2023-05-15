############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: brsp.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 1/11/07 2:39p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/commonutils/rsp/brsp.inc $
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
MAGNUM_MODULES += BRSP

# Module Includes
BRSP_INCLUDES += $(MAGNUM)/commonutils/rsp/

# Module Sources
BRSP_SOURCES += $(MAGNUM)/commonutils/rsp/brsp.c
BRSP_SOURCES += $(MAGNUM)/commonutils/rsp/brsp_mbyte.c
