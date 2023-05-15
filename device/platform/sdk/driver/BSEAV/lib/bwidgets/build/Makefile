############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 4 $
# $brcm_Date: 9/6/12 11:28p $
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/bwidgets/build/Makefile $
# 
# 4   9/6/12 11:28p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/5/12 5:16p mphillip
# SW7425-3739: Remove BSEAV/lib symlinks
# 
# 3   10/22/10 3:21p erickson
# SWDEPRECATED-626: revive bwidgets on nexus
# 
############################################################
include ../../../api/build/tools.mak
BSEAV = $(shell cd ../../.. && ${PWD})

.PHONY: check_environment static
all: check_environment static

include ../include/bwidgets.mak
include $(BSEAV)/build/refsw_inc.mak

LIB = bwidgets
ODIR = $(BWIDGETS_LIBDIR)

CFLAGS += -ansi -W -Wall
ifneq ($(SYSTEM),vxworks)
CFLAGS += -O2
endif
CFLAGS += $(BWIDGETS_CFLAGS)
CFLAGS += $(B_REFSW_CFLAGS) $(B_REFSW_GENERIC_MAGNUM_CFLAGS) $(B_REFSW_MAGNUM_INCLUDE_DIRS)
ifeq ($(DEBUG),y)
CFLAGS += -g -DBDBG_DEBUG_BUILD=1
endif

vpath %.c $(BWIDGETS_DIR)/src

SRCS = \
	bwidgets.c \
	bwidget_button.c \
	bwidget_label.c \
	bwidget_base.c

check_environment:
	mkdir -p $(BWIDGETS_LIBDIR)

include ../../../api/build/rules.mak
