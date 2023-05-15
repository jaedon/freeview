############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bstd.inc $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 9/6/12 10:29a $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/std/bstd.inc $
# 
# Hydra_Software_Devel/4   9/6/12 10:29a vsilyaev
# SW7425-3760: Symbolic link removal
# 
# Hydra_Software_Devel/SW7425-3760/3   9/5/12 1:34p vsilyaev
# SW7425-3760: Added top level include path
# 
# Hydra_Software_Devel/SW7425-3760/2   9/4/12 4:51p vsilyaev
# SW7425-3760: Fixed typo
# 
# Hydra_Software_Devel/SW7425-3760/1   9/4/12 4:24p vsilyaev
# SW7425-3760: Removed symlinks
# 
# Hydra_Software_Devel/3   9/15/09 1:37p erickson
# SW7400-2511: use B_REFSW_OS
#
# Hydra_Software_Devel/2   1/18/07 1:56p jgarrett
# PR 27004: Adding BSTD_OMIT_DEFAULT_CONFIG
#
# Hydra_Software_Devel/1   1/11/07 2:37p jgarrett
# PR27004: Adding new .inc files
#
############################################################

#
# Conditional Options for this Makefile
#
# BTD_OMIT_DEFAULT_CONFIG - If defined, the path to basemodules/std/config/bstd_cfg.h will be removed from the build.  This allows the application to define a custom bstd_cfg.h file in their own directory.  If not defined (default), the standard path will be included.
#

# Define this module
MAGNUM_MODULES += BSTD

# Backward-compatibility
B_REFSW_OS ?= $(OS)

# Module Includes
BSTD_INCLUDES += $(MAGNUM)/basemodules/std
ifeq ($(BSTD_OMIT_DEFAULT_CONFIG),)
BSTD_INCLUDES += $(MAGNUM)/basemodules/std/config
endif

# Module Sources
ifeq ($(findstring $(B_REFSW_OS), linuxclient linuxemu linuxpal win32),${B_REFSW_OS})
BSTD_INCLUDES += $(MAGNUM)/basemodules/std/types/linuxuser
# Warning if types are not found
ifeq ($(wildcard $(MAGNUM)/basemodules/std/types/linuxuser),)
$(warning warning: Missing basemodules/std/types definition for OS=$(B_REFSW_OS))
endif
else
BSTD_INCLUDES += $(MAGNUM)/basemodules/std/types/$(B_REFSW_OS)
# Warning if types are not found
ifeq ($(wildcard $(MAGNUM)/basemodules/std/types/$(B_REFSW_OS)),)
$(warning warning: Missing basemodules/std/types definition for OS=$(B_REFSW_OS))
endif
endif


