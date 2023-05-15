############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmsglib.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/7/12 8:32a $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/syslib/msglib/bmsglib.inc $
# 
# Hydra_Software_Devel/2   9/7/12 8:32a marcusk
# SW7231-988: Removed symlinks
# 
# Hydra_Software_Devel/SW7231-988/1   9/6/12 8:41a marcusk
# SW7231-988: Removed symlinks
# 
# Hydra_Software_Devel/1   7/26/07 12:35p erickson
# PR32868: initial checkin of BMSGlib
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BMSGLIB

# Module Includes
BMSGLIB_INCLUDES += $(MAGNUM)/syslib/msglib/include

# Module Sources
BMSGLIB_SOURCES += $(MAGNUM)/syslib/msglib/src/bmsglib.c


