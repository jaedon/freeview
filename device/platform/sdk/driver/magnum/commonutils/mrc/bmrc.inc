############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmrc.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 9/7/12 1:13p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/commonutils/mrc/bmrc.inc $
# 
# Hydra_Software_Devel/5   9/7/12 1:13p albertl
# SW7435-326: Flatten MRC directory structure.
# 
# Hydra_Software_Devel/4   6/30/10 5:05p albertl
# SW35230-498: Changed to use BMRC_ALLOW_PCU_TO_ACCESS_KERNEL flag.
# 
# Hydra_Software_Devel/3   3/18/10 11:55a erickson
# SW7405-3892: change default list of non-OS HW clients to prevent
# GFD/M2MC access to kernel memory. this is rarely done. added export
# BMRC_ALLOW_GFX_TO_ACCESS_KERNEL=y compile time option to allow this
# access.
#
# Hydra_Software_Devel/2   2/25/08 7:19p albertl
# PR36876:  Added bmrc_clienttable.c.
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
MAGNUM_MODULES += BMRC

# Module Includes
BMRC_INCLUDES += $(MAGNUM)/commonutils/mrc/include

# Module Sources
BMRC_SOURCES += $(MAGNUM)/commonutils/mrc/src/bmrc.c
BMRC_SOURCES += $(MAGNUM)/commonutils/mrc/src/bmrc_clienttable_priv.c
BMRC_SOURCES += $(MAGNUM)/commonutils/mrc/src/bmrc_monitor_clients.c
BMRC_SOURCES += $(MAGNUM)/commonutils/mrc/src/bmrc_monitor.c

# By default, GFD and M2MC cannot access OS-allocated memory. They access MEM-allocated memory.
# If you have done the necessary work to provide contiguous physical memory with physical
# offsets, then you can allow GFD and M2MC to access OS-allocated memory. You must
# export BMRC_ALLOW_GFX_TO_ACCESS_KERNEL=y to prevent MRC from blocking this access.
#
ifeq ($(BMRC_ALLOW_GFX_TO_ACCESS_KERNEL),y)
BMRC_DEFINES += BMRC_ALLOW_GFX_TO_ACCESS_KERNEL
endif

ifeq ($(BMRC_ALLOW_PCU_TO_ACCESS_KERNEL),y)
BMRC_DEFINES += BMRC_ALLOW_PCU_TO_ACCESS_KERNEL
endif
