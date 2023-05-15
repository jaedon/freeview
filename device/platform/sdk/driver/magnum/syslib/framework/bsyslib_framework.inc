############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bsyslib_framework.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/6/12 8:52p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/syslib/framework/bsyslib_framework.inc $
# 
# Hydra_Software_Devel/2   9/6/12 8:52p bandrews
# SW7425-3850: merge to main
# 
# Hydra_Software_Devel/SW7425-3850/1   9/4/12 5:48p bandrews
# SW7425-3850: remove symlinks
# 
# Hydra_Software_Devel/1   3/24/08 3:08p bandrews
# PR40865: Fixed
# 
# Hydra_Software_Devel/2   1/31/08 3:12p bandrews
# PR36149: Added source files
# 
# Hydra_Software_Devel/1   10/16/07 2:13p bandrews
# PR36149: Initial implementation
############################################################

# Define this module
MAGNUM_MODULES += BSYSLIB

# Module Includes
BSYSLIB_INCLUDES += $(MAGNUM)/syslib/framework/include

# Module Sources
BSYSLIB_SOURCES = \
	$(MAGNUM)/syslib/framework/src/bsyslib_callback.c \
	$(MAGNUM)/syslib/framework/src/bsyslib_list.c \
	$(MAGNUM)/syslib/framework/src/bsyslib_resource_pool.c

