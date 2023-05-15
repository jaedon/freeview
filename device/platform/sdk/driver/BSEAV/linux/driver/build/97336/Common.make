############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Common.make $
# $brcm_Revision: 1 $
# $brcm_Date: 9/13/12 6:23p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97336/Common.make $
# 
# 1   9/13/12 6:23p mphillip
# SW7425-3739: Merge symlink removal changes to main
# 
# SW7425-3739/1   9/13/12 12:39p mphillip
# SW7425-3739: Remove symlinks for linux/driver/build subdirectories and
# dependencies
# 
# 14   4/11/11 6:19p jhaberf
# SWDTV-6551: Explicity specify include path for DTV chips
# 
# 13   4/11/11 5:34p jhaberf
# SWDTV-6551: Added 4.5 toolchain for DTV chips
# 
# 12   3/31/11 5:54p jrubio
# SW7400-2511: remove mips-uclibc
# 
# 11   3/29/10 2:19p mananp
# SW7325-649:Missing bcmdriver.ko when DEBUG=n set for Nexus
# 
# 10   3/26/10 7:22p mananp
# SW7325-649:Missing bcmdriver.ko when DEBUG=n set for Nexus
# 
# 9   10/20/09 11:36a erickson
# SW7405-3154: reorg 2.6.31 support to support power driver with same
# include
#
# 8   10/20/09 9:44a erickson
# SW7405-3154: fix reverse compatibility
#
# 7   10/19/09 3:25p erickson
# SW7405-3154: fix reverse compatibility
#
# 6   10/16/09 4:55p erickson
# SW7400-2511: add B_REFSW_ compatibility
#
# 5   3/13/09 2:52p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 4   1/30/07 11:15a erickson
# PR25902: define BSEAV here
#
# 3   1/29/07 4:08p erickson
# PR25902: need to default DEBUG here
#
# 2   1/29/07 12:32p erickson
# PR25902: rework linux drivers to remove duplication
#
# 1   1/12/07 7:12a erickson
# PR25902: rework kernelmode driver to use separate kni driver, no longer
# use ../common directory (which had legacy stuff)
#
# 7   11/30/06 11:26a jgarrett
# PR 26266: Fixing unconditional mkdir (caused some things to rebuild
# always)
#
# 6   11/16/06 5:05p jgarrett
# PR 25230: Adding 2.4/2.6 distinction for proper build flags
#
# 5   9/19/06 5:18p gmohile
# PR 24129: Added support for 7313
#
# 4   8/21/06 1:04p erickson
# PR22882: convert soft-float to use source, not checked in binaries
#
# 3   7/14/06 11:31a jgarrett
# PR 19909: Merging to latest baseline
#
# PR19909/2   7/13/06 6:49p jgarrett
# PR 19909: Fixing install rule printout
#
# PR19909/1   7/13/06 5:36p jgarrett
# PR 19909: Quieting down build messages
#
# 2   6/12/06 1:45p jgarrett
# PR 20139: Changing GCC_BASE for 2.6.x
#
# 1   2/8/05 12:17a dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/31   12/7/04 6:35p bandrews
# PR13158: Initial 7318 support
#
# Irvine_BSEAVSW_Devel/30   9/16/04 6:15p bandrews
# PR12686: Added 7327 7312
#
# Irvine_BSEAVSW_Devel/29   8/26/04 4:12p erickson
# PR12476: removed deprecated compile option
#
# Irvine_BSEAVSW_Devel/28   8/19/04 1:14p erickson
# PR12101: added -DUSE_LEGACY_KERNAL_INTERFACE for code shared between
# legacy and magnum
#
# Irvine_BSEAVSW_Devel/27   8/19/04 10:29a erickson
# PR12101: use tspsi_validate to check both transport message filter
# correction and section length
#
# Irvine_BSEAVSW_Devel/26   8/12/04 6:25p bandrews
# PR11657: Added 2nd graphics device for 7329
#
# Irvine_BSEAVSW_Devel/25   8/12/04 4:32p bandrews
# PR11657: Added 7329 support
#
# Irvine_BSEAVSW_Devel/24   7/26/04 1:48p erickson
# PR11674: bcmdmadrv.c should always be included because of mem-to-mem
# DMA for settop api double buffering
#
# Irvine_BSEAVSW_Devel/23   5/25/04 10:10a bandrews
# PR11203: Removed EXPORT_SYMTAB
#
# Irvine_BSEAVSW_Devel/22   5/12/04 10:31a erickson
# PR10967: use std header
############################################################

#
# Common driver build for all platforms
# This should not be used to build usermode code
#
# Inputs: DRIVERS
# Outputs: Lots of stuff
#

BSEAV=$(shell cd ../../../..;pwd)

# backward-compatibility
ifdef ARCH
B_REFSW_ARCH ?= $(ARCH)
endif
ifdef DEBUG
B_REFSW_DEBUG ?= $(DEBUG)
endif
ifdef TOOLCHAIN_DIR
B_REFSW_TOOLCHAIN_DIR = $(TOOLCHAIN_DIR)
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

#
# Configure linux kernel source
# Output is LINUX_INC
#
include Linux.make

# defines TOOLCHAIN_DIR, DEBUG, ARCH
include ../../../../api/build/tools.mak

ifeq ($(B_REFSW_DEBUG),y)
BCM_OBJ_DIR=$(B_REFSW_ARCH).debug${NEXUS_BIN_DIR_SUFFIX}
else
BCM_OBJ_DIR=$(B_REFSW_ARCH).release${NEXUS_BIN_DIR_SUFFIX}
endif

#########################################################
#
# Standard include directories
# See tools.mak for TOOLCHAIN_DIR
#
ifeq ($(findstring $(PLATFORM), 935233 935331), $(PLATFORM))
STD_INC ?= $(TOOLCHAIN_DIR)/mipsel-linux-uclibc/sys-root/usr/include
GCC_BASE ?= $(TOOLCHAIN_DIR)/lib/gcc/mipsel-linux-uclibc
else
STD_INC ?= $(TOOLCHAIN_DIR)/mipsel-linux/include
GCC_BASE ?= $(shell (test -d $(TOOLCHAIN_DIR)/lib/gcc-lib && echo $(TOOLCHAIN_DIR)/lib/gcc-lib/mipsel-linux) || echo $(TOOLCHAIN_DIR)/lib/gcc/mipsel-linux)
endif

#
# BEWARE: If you have more than one version of gcc installed,
# the following line is going to have a problem.
#
GCC_VER = $(shell (ls $(GCC_BASE)))
GCC_INC = $(GCC_BASE)/$(GCC_VER)/include

$(STD_INC):
	@test -d $@ || (echo "STD_INC is incorrect"; test 0 == 1)

$(GCC_BASE):
	@test -d $@ || (echo "GCC_BASE is incorrect"; test 0 == 1)

$(BSEAV):
	@test -d $@ || (echo "BSEAV is incorrect"; test 0 == 1)

$(BCM_OBJ_DIR):
	${Q_}mkdir -p $(BCM_OBJ_DIR)

.PHONY: checkdirs
checkdirs: $(LINUX_INC) $(STD_INC) $(GCC_BASE) $(BSEAV) $(BCM_OBJ_DIR)

ifeq ($(B_REFSW_DEBUG),y)
CFLAGS += -DCONFIG_PROC_FS
endif

#########################################################
#
# soft float
#
# PR8054: We can't use the fpu in kernel mode unless it's
# in a critical section. The Linux exception handler doesn't
# store the fpu registers.

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
	${Q_}$(CP) $(DRIVERS) $(INSTALL_DIR)
endif

#
# Clean up drivers
#
clean: $(OTHER_CLEANS)
	${Q_}$(RM) -r $(BCM_OBJ_DIR)

