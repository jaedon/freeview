############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bfmt.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/6/12 6:11p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/commonutils/fmt/bfmt.inc $
# 
# Hydra_Software_Devel/2   9/6/12 6:11p pntruong
# SW7435-324: Remove symbolic links and flatten directories.
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
MAGNUM_MODULES += BFMT

# Module Includes
BFMT_INCLUDES += $(MAGNUM)/commonutils/fmt/include

# Module Sources
BFMT_SOURCES += $(MAGNUM)/commonutils/fmt/src/bfmt.c

#
# End of file
#

