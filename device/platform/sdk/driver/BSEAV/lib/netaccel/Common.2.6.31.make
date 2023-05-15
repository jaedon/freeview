############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Common.2.6.31.make $
# $brcm_Revision: 6 $
# $brcm_Date: 3/29/12 5:22p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/lib/netaccel/Common.2.6.31.make $
# 
# 6   3/29/12 5:22p jrubio
# SW7346-751: add 3.0 kernel support
# 
# 5   1/30/12 3:51p jrubio
# SW7346-647: fix NETACCEL compiling for 2631
# 
# 4   1/20/12 1:43p jrubio
# SW7346-647: add Kbuild for 2631(37)
# 
# 3   10/19/11 4:34p mward
# SW7125-1119: Change DEBUG to B_REFSW_DEBUG.
# 
# 2   12/2/09 6:35p ssood
# SW7405-3154: disable gcc option to use its soft float library as gcc's
# float->int expression doesn't truncate properly (MIPS FPU instead
# should be used)
# 
# 1   10/26/09 4:41p lwhite
# SW7405-3154: Add Support for linux 2.6.31
#
############################################################


.PHONY: checkdirs driver $(DRIVERS)
all: checkdirs driver

LINUX_INC = $(LINUX)/include

#
# The reference software does not automatically configure the kernel. That must be
# done by the developer.
#
ifeq ($(LINUX_VER_GE_2_6),y)
LINUX_VER_EQ_2_6_37 := $(shell (grep 'SUBLEVEL = 37' ${LINUX}/Makefile >/dev/null && echo 'y'))
endif

#ifneq ($(LINUX_VER_EQ_2_6_37),y)
#LINUX_CONFIGURED = $(shell test -f $(LINUX_INC)/linux/autoconf.h && echo "y")
#ifneq ($(LINUX_CONFIGURED),y)
#$(error $(LINUX) must be configured before using it to build a driver.)
#endif
#endif

ifdef PLATFORM
NEXUS_PLATFORM ?= $(PLATFORM)
endif

# set defaults
ifeq ($(B_REFSW_DEBUG),)
B_REFSW_DEBUG=y
endif
ifeq ($(B_REFSW_ARCH),)
B_REFSW_ARCH=mipsel-linux
endif

# reverse backward-compatibility for tools.mak and refsw_inc.mak
ifdef B_REFSW_DEBUG
DEBUG ?= $(B_REFSW_DEBUG)
endif
#ifdef B_REFSW_ARCH
#ARCH ?= $(B_REFSW_ARCH)
#endif
ifdef NEXUS_PLATFORM
PLATFORM ?= $(NEXUS_PLATFORM)
endif

# defines toolchain
#include tools.mak
#include ${BSEAV}/api/build/tools.mak

# destination for binary
ifeq ($(B_REFSW_DEBUG),y)
BCM_OBJ_DIR=$(B_REFSW_ARCH).debug
else
BCM_OBJ_DIR=$(B_REFSW_ARCH).release
endif

$(BCM_OBJ_DIR):
	$(Q_)mkdir -p $(BCM_OBJ_DIR)

checkdirs: $(BCM_OBJ_DIR) 


ifeq ($(B_REFSW_DEBUG),y)
CFLAGS += -DCONFIG_PROC_FS
endif

