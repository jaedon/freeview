############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bavc.inc $
# $brcm_Revision: Hydra_Software_Devel/10 $
# $brcm_Date: 9/6/12 6:10p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/commonutils/avc/bavc.inc $
# 
# Hydra_Software_Devel/10   9/6/12 6:10p pntruong
# SW7435-324: Remove symbolic links and flatten directories.
#
# Hydra_Software_Devel/9   4/21/10 2:50p vle
# SW7405-3994,SW7405-3997: Add support for all platforms.
#
# Hydra_Software_Devel/8   4/20/10 2:36p mward
# SW7405-3994,SW7405-3997:  add 7125 support
#
# Hydra_Software_Devel/7   4/13/10 3:21p jrubio
# SW7405-3994,SW7405-3997:  add 7340/7342/7335/7325 support
#
# Hydra_Software_Devel/6   4/13/10 1:46p erickson
# SW3548-2013: add 7420
#
# Hydra_Software_Devel/5   4/5/10 6:44p rgreen
# SW7405-3994,SW7405-3997: Add support for 7400, 7405
#
# Hydra_Software_Devel/4   11/25/09 11:17a rgreen
# SW35230-15: Add support for 35230
#
# Hydra_Software_Devel/3   5/12/09 7:02p rgreen
# PR54000: Add debug BAVC_HDMI enum to string conversion functions
#
# Hydra_Software_Devel/2   2/8/07 11:31a pntruong
# PR 27117: Updating to use module specific .inc files.
#
# Hydra_Software_Devel/2   2/8/07 11:29a pntruong
# PR 27117: Updating to use module specific .inc files.
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
MAGNUM_MODULES += BAVC

BAVC_INCLUDES += $(MAGNUM)/commonutils/avc/include
BAVC_SOURCES  += $(MAGNUM)/commonutils/avc/src/bavc_hdmi.c

#
# End of file
#
