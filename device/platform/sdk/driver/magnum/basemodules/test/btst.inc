############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: btst.inc $
# $brcm_Revision: Hydra_Software_Devel/7 $
# $brcm_Date: 9/7/12 1:19p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/test/btst.inc $
# 
# Hydra_Software_Devel/7   9/7/12 1:19p pntruong
# SW7435-331: Fixed src path.
#
# Hydra_Software_Devel/6   9/6/12 8:08p jasonh
# SW7435-331: Updating test directory structure.
#
# Hydra_Software_Devel/5   1/21/10 11:02a kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/SW35230-56/1   1/18/10 6:34p kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/4   9/15/09 1:37p erickson
# SW7400-2511: use B_REFSW_OS
#
# Hydra_Software_Devel/3   6/12/09 11:55a jessem
# PR 54881: Added linuxemu.
#
# Hydra_Software_Devel/2   10/16/07 1:56p jessem
# PR 36129: Added linuxclient for use with Linux-based IKOS emulation
# platform.
#
# Hydra_Software_Devel/1   1/18/07 2:24p jasonh
# PR 27117: Initial version.
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
MAGNUM_MODULES += BTST

# Backward-compatibility
B_REFSW_OS ?= $(OS)

# Sanity Check OS provided
BTST_LINUX_OS := linuxuser linuxclient linuxemu linuxpal
BTST_SUPPORTED_OS := $(BTST_LINUX_OS) vxworks
ifeq ($(findstring $(B_REFSW_OS), $(BTST_SUPPORTED_OS)),)
$(error [tst] OS must be one of the following: $(BTST_SUPPORTED_OS))
endif

# Determine source directory
BTST_LINUX_OS := linuxuser linuxclient linuxemu linuxpal
ifeq ($(filter $(B_REFSW_OS), $(BTST_LINUX_OS)), $(B_REFSW_OS))
	BTST_OS = linux
else
	BTST_OS = $(B_REFSW_OS)
endif

# Module Includes
BTST_INCLUDES += $(MAGNUM)/basemodules/test/include/

BTST_SOURCES += $(MAGNUM)/basemodules/test/src/$(BTST_OS)/btst_kni.c