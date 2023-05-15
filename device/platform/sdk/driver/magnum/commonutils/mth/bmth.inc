############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmth.inc $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 9/6/12 6:12p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/commonutils/mth/bmth.inc $
# 
# Hydra_Software_Devel/4   9/6/12 6:12p pntruong
# SW7435-324: Remove symbolic links and flatten directories.
#
# Hydra_Software_Devel/3   8/26/10 7:46p albertl
# SW7405-4556, SW7405-4515: Changed multiplication to use 64-bit
# intermediate.  Added fix to float macro.
#
# Hydra_Software_Devel/2   6/11/07 6:56p albertl
# PR31093:  Added bmth_fix
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
MAGNUM_MODULES += BMTH

# Module Includes
BMTH_INCLUDES += $(MAGNUM)/commonutils/mth/include

# Module Sources
BMTH_SOURCES += $(MAGNUM)/commonutils/mth/src/bmth.c
BMTH_SOURCES += $(MAGNUM)/commonutils/mth/src/bmth_fix.c
BMTH_SOURCES += $(MAGNUM)/commonutils/mth/src/bmth_fix_matrix.c

#
# End of file
#
