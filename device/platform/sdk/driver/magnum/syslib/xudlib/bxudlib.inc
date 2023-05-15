#############################################################################
#
#       Copyright (c) 2003-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bxudlib.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 5/8/12 5:25p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /magnum/syslib/xudlib/bxudlib.inc $
# 
# Hydra_Software_Devel/2   5/8/12 5:25p nilesh
# SW7425-2469: Updated to more easily support 24fps CC source content
# 
# Hydra_Software_Devel/1   12/7/11 11:55a nilesh
# SW7425-1877: Initial XUDlib implementation
# 
#############################################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BXUDLIB

# Module Includes
BXUDLIB_INCLUDES += $(MAGNUM)/syslib/xudlib

BXUDLIB_SOURCES += \
	$(MAGNUM)/syslib/xudlib/bxudlib.c

ifeq ($(BXUDLIB_TEST_MODE),y)
BXUDLIB_DEFINES += BXUDLIB_P_TEST_MODE=1
endif

ifeq ($(BXUDLIB_DUMP_INPUT_CC),y)
BXUDLIB_DEFINES += BXUDLIB_P_DUMP_INPUT_CC=1
endif

ifeq ($(BXUDLIB_DUMP_OUTPUT_CC),y)
BXUDLIB_DEFINES += BXUDLIB_P_DUMP_OUTPUT_CC=1
endif