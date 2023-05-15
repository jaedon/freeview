############################################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: globals.mak $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 10/19/11 2:51p $
#
# Module Description:
# 
# Global definitions 
#
# Revision History:
#
# $brcm_Log: /rockford/inc/globals.mak $
# 
# Hydra_Software_Devel/3   10/19/11 2:51p mward
# SW7125-1119:  Change DEBUG to B_REFSW_DEBUG.
# 
# Hydra_Software_Devel/2   10/24/03 10:00a vsilyaev
# Use := to assign ODIR
# 
# Hydra_Software_Devel/1   4/1/03 8:01p vsilyaev
# Framework make include files - initial version.
# 
# 
###########################################################################


ifdef B_REFSW_DEBUG
# apply  ODIR conversion for debug/release build
ifdef  ODIR
ifeq    ($(B_REFSW_DEBUG),1)
ODIR	:= $(ODIR).debug
MAGNUM_CFLAGS += -DBDBG_DEBUG_BUILD=1 
else
ODIR 	:= $(ODIR).release
endif
endif
else
# if B_REFSW_DEBUG not defined use debug build
MAGNUM_CFLAGS += -DBDBG_DEBUG_BUILD=1 
endif

ifndef ODIR
ODIR=.
endif

_ODIR := $(ODIR)/.dir
APP := $(ODIR)/$(APP)


