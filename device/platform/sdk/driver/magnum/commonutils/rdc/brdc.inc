############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: brdc.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 9/6/12 6:13p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/commonutils/rdc/brdc.inc $
# 
# Hydra_Software_Devel/5   9/6/12 6:13p pntruong
# SW7435-324: Remove symbolic links and flatten directories.
#
# Hydra_Software_Devel/4   3/26/09 11:27a shyam
# PR50711 : Add make option for splash
#
# Hydra_Software_Devel/3   2/28/08 5:03p jessem
# PR 38623: Added RDMA blockout support.
#
# Hydra_Software_Devel/2   2/8/07 11:32a pntruong
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
MAGNUM_MODULES += BRDC

# Module Includes
BRDC_INCLUDES += $(MAGNUM)/commonutils/rdc/include

ifeq ($(BRDC_DISABLE_TRACK_EXECUTION),y)
BRDC_DEFINES += BRDC_DISABLE_TRACK_EXECUTION
endif

# Module Sources
BRDC_SOURCES += \
	$(MAGNUM)/commonutils/rdc/src/brdc.c \
	$(MAGNUM)/commonutils/rdc/src/brdc_dbg.c \
	$(MAGNUM)/commonutils/rdc/src/brdc_private.c \
	$(MAGNUM)/commonutils/rdc/src/brdc_blockout_priv.c

#
# End of file
#
