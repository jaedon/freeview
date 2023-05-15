############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bwin.mak $
# $brcm_Revision: 8 $
# $brcm_Date: 10/19/11 2:46p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/app/brutus/build/bwin.mak $
# 
# 8   10/19/11 2:46p mward
# SW7125-1119:  Change DEBUG to B_REFSW_DEBUG.
# 
# 7   3/24/11 4:24p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 6   7/19/06 5:39p jgarrett
# PR 19909: Fixing make -j
# 
# 5   7/14/06 11:29a jgarrett
# PR 19909: Merging to latest baseline
# 
# PR19909/1   7/6/06 10:51a jgarrett
# PR 19909: Reducing build output for brutus
# 
# 3   2/23/05 10:50a erickson
# PR14180: fixed PHONY rule
# 
# 2   2/17/05 1:53p erickson
# PR9497: remove unneeded stuff
# 
# 1   2/7/05 7:40p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/4   9/9/04 1:59p erickson
# PR12608: converted JPEG_SUPPORT to PICTURES_SUPPORT
# 
# Irvine_BSEAVSW_Devel/3   5/12/04 10:24a erickson
# PR10967: added standard header
############################################################

#
# Partial Makefile for apps that use mlibs and bwin
#
# Input: BSEAV, B_REFSW_ARCH
# Output: LDFLAGS, CXXFLAGS are augmented
#

OTHER_CLEANS += clean-mlibs clean-bwin
.PHONY: mlibs bwin clean-mlibs clean-bwin
#clean: clean-mlibs clean-bwin

MLIBS=$(BSEAV)/lib/mlibs
CXXFLAGS += -I$(MLIBS)
ifeq ($(B_REFSW_DEBUG),y)
MLIBS_ODIR=$(B_REFSW_ARCH).debug
else
MLIBS_ODIR=$(B_REFSW_ARCH).release
endif
LDFLAGS += -L$(MLIBS)/$(MLIBS_ODIR) -lmwidgets -lmcommon -lmnet

BWIN=$(BSEAV)/lib/bwin
include $(BWIN)/include/bwin.mak
CXXFLAGS += $(BWIN_CFLAGS)
LDFLAGS += $(BWIN_LDFLAGS)

mlibs:
	${Q_}$(MAKE) -C $(MLIBS) 
bwin:
	${Q_}$(MAKE) -C $(BWIN)/build
clean-mlibs:
	${Q_}$(MAKE) -C $(MLIBS) clean
clean-bwin:
	${Q_}$(MAKE) -C $(BWIN)/build clean

