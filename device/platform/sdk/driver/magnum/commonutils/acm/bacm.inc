############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bacm.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 3/15/07 3:18p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/commonutils/acm/bacm.inc $
# 
# Hydra_Software_Devel/1   3/15/07 3:18p rgreen
# PR27181: Initial audio common support for SPDIF Channel Status
# types/functions
# 
#
############################################################


#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BACM


BACM_INCLUDES += $(MAGNUM)/commonutils/acm

# Module Sources
BACM_SOURCES += $(MAGNUM)/commonutils/acm/bacm_spdif.c

