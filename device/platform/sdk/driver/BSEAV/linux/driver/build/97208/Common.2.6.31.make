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
# $brcm_Revision: 2 $
# $brcm_Date: 9/17/12 4:37p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97208/Common.2.6.31.make $
# 
# 2   9/17/12 4:37p mphillip
# SW7425-3739: Update legacy driver target directories to match new
# hierarchy
# 
# 1   9/13/12 6:23p mphillip
# SW7425-3739: Merge symlink removal changes to main
# 
# SW7425-3739/1   9/13/12 12:38p mphillip
# SW7425-3739: Remove symlinks for linux/driver/build subdirectories and
# dependencies
# 
# 2   3/31/11 5:54p jrubio
# SW7400-2511: remove mips-uclibc
# 
# 1   10/20/09 11:36a erickson
# SW7405-3154: reorg 2.6.31 support to support power driver with same
# include
#
# 2   10/20/09 9:44a erickson
# SW7405-3154: fix reverse compatibility
#
# 1   10/19/09 3:25p erickson
# SW7405-3154: add 2.6.31 support
#
############################################################

BSEAV=$(shell cd ../../../..;pwd)

.PHONY: checkdirs driver $(DRIVERS)
all: checkdirs driver

SYSTEM = linuxkernel
LINUX_INC = $(LINUX)/include

#
# The reference software does not automatically configure the kernel. That must be
# done by the developer.
#
LINUX_CONFIGURED = $(shell test -f $(LINUX_INC)/linux/autoconf.h && echo "y")
ifneq ($(LINUX_CONFIGURED),y)
$(error $(LINUX) must be configured before using it to build a driver.)
endif

# backward-compatibility
ifdef ARCH
B_REFSW_ARCH ?= $(ARCH)
endif
ifdef DEBUG
B_REFSW_DEBUG ?= $(DEBUG)
endif
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
ifdef B_REFSW_ARCH
ARCH ?= $(B_REFSW_ARCH)
endif
ifdef NEXUS_PLATFORM
PLATFORM ?= $(NEXUS_PLATFORM)
endif

# defines toolchain
include ../../../../api/build/tools.mak

# destination for binary
ifeq ($(B_REFSW_DEBUG),y)
BCM_OBJ_DIR=../$(B_REFSW_ARCH).debug$(NEXUS_BIN_DIR_SUFFIX)
else
BCM_OBJ_DIR=../$(B_REFSW_ARCH).release$(NEXUS_BIN_DIR_SUFFIX)
endif

$(BCM_OBJ_DIR):
	$(Q_)mkdir -p $(BCM_OBJ_DIR)

checkdirs: $(BCM_OBJ_DIR)

ifeq ($(B_REFSW_DEBUG),y)
CFLAGS += -DCONFIG_PROC_FS
endif

# soft float for kernel mode code
CFLAGS += -msoft-float
FLOATLIB_OBJS += fp-bit-single.o fp-bit-double.o
FLOATLIB_DIR = $(BSEAV)/linux/driver/floatlib/src

#########################################################
#
# Driver installation
#
ifeq ($(INSTALL_DIR),)
install:
	$(error INSTALL_DIR is undefined)
else
install:
	@echo "[Install... $(sort $(notdir $(DRIVERS)))]"
	$(Q_)$(CP) $(DRIVERS) $(INSTALL_DIR)
endif

#
# Clean up drivers
#
clean: $(OTHER_CLEANS)
	$(Q_)$(RM) -r $(BCM_OBJ_DIR)

# defines CFLAGS and LDFLAGS
include ../../../../build/refsw_inc.mak

driver: checkdirs $(DRIVERS)

