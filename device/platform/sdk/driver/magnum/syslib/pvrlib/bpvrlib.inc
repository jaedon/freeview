############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bpvrlib.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 8/2/07 4:19p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/syslib/pvrlib/bpvrlib.inc $
# 
# Hydra_Software_Devel/2   8/2/07 4:19p vsilyaev
# PR 33751: Added bpvrlib_feed
# 
# Hydra_Software_Devel/1   1/11/07 2:41p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BPVRLIB

# Module Includes
BPVRLIB_INCLUDES += $(MAGNUM)/syslib/pvrlib

# Module Sources
BPVRLIB_SOURCES += $(MAGNUM)/syslib/pvrlib/bpvrlib_play.c
BPVRLIB_SOURCES += $(MAGNUM)/syslib/pvrlib/bpvrlib_p_ringmgr.c
BPVRLIB_SOURCES += $(MAGNUM)/syslib/pvrlib/bpvrlib_feed.c

# Chip Specifics
ifeq ($(BCHP_CHIP),7038)
BPVRLIB_SOURCES += $(MAGNUM)/syslib/pvrlib/bpvrlib_rec.c
endif

