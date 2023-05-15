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
# $brcm_Revision: 1 $
# $brcm_Date: 9/13/12 6:24p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97468/Makefile $
# 
# 1   9/13/12 6:24p mphillip
# SW7425-3739: Merge symlink removal changes to main
# 
# SW7425-3739/1   9/13/12 12:39p mphillip
# SW7425-3739: Remove symlinks for linux/driver/build subdirectories and
# dependencies
# 
# 21   3/31/11 5:54p jrubio
# SW7400-2511: remove mips-uclibc
# 
# 20   3/9/11 3:00p erickson
# SW7420-1576: avoid conflicts with $LINUX/Makefile over ARCH=mips
# 
# 19   3/7/11 4:39p erickson
# SW7420-1576: build bcmdriver with kbuild for linux 2.6.31 and following
#
# 18   10/20/09 2:17p erickson
# SW7405-3154: reorg 2.6.31 support to 97401 directory
#
# 17   1/29/07 12:32p erickson
# PR25902: rework linux drivers to remove duplication
#
# 16   1/12/07 7:12a erickson
# PR25902: rework kernelmode driver to use separate kni driver, no longer
# use ../common directory (which had legacy stuff)
#
# 14   11/16/06 5:08p jgarrett
# PR 25230: Moving usermode driver to BSEAV
#
# 13   11/10/06 9:19a erickson
# PR25397: if c1, config linux kernel for c0
#
# 12   9/20/06 4:28p haisongw
# PR24338: add 97455B0 Board support
#
# 11   9/20/06 10:37a erickson
# PR24374: added c0 support (and beyond)
#
# 10   8/17/06 12:41p erickson
# PR23574: use BE defconfig
#
# 9   7/19/06 5:40p jgarrett
# PR 19909: Fixing make -j
#
# 8   7/18/06 9:28a erickson
# PR21941: rework to support Common.make
#
# 7   7/14/06 11:32a jgarrett
# PR 19909: Merging to latest baseline
#
# PR19909/1   7/6/06 4:44p jgarrett
# PR 19909: Merging to latest baseline
#
# 6   1/19/06 2:42p erickson
# PR17108: for 2.6, do a simple check to see if the kernel has been
# built. it avoids some errors.
#
# 5   1/9/06 4:26p erickson
# PR18945: default LINUX so that it's not required
#
# 4   10/17/05 1:58p vsilyaev
# PR17108: build driver from the source code
#
############################################################

LINUX ?= /opt/brcm/linux

LINUX_PATCHLEVEL = $(shell grep 'PATCHLEVEL = ' ${LINUX}/Makefile | awk '{print $$3}')
LINUX_SUBLEVEL = $(shell grep 'SUBLEVEL = ' ${LINUX}/Makefile | awk '{print $$3}')
LINUX_VER_GE_2_6_31 = $(shell test $(LINUX_PATCHLEVEL) -eq 6 -a $(LINUX_SUBLEVEL) -ge 31 && echo y)

# linux 2.6.31 and greater uses kbuild for bcmdriver.ko
ifeq ($(LINUX_VER_GE_2_6_31),y)

ifeq ($(B_REFSW_ARCH),)
B_REFSW_ARCH=mipsel-linux
endif
ifdef DEBUG
B_REFSW_DEBUG ?= $(DEBUG)
endif
ifeq ($(B_REFSW_DEBUG),)
B_REFSW_DEBUG=y
endif
ifeq ($(B_REFSW_DEBUG),y)
BCM_OBJ_DIR=$(B_REFSW_ARCH).debug$(NEXUS_BIN_DIR_SUFFIX)
else
BCM_OBJ_DIR=$(B_REFSW_ARCH).release$(NEXUS_BIN_DIR_SUFFIX)
endif

.PHONY: $(BCM_OBJ_DIR)
CP = cp -f

# use Kbuild makefile in BSEAV/linux/driver/usermode for 2.6.31
all install: $(BCM_OBJ_DIR)
	$(MAKE) -C ../../usermode ARCH=mips $@
	$(CP) ../../usermode/bcmdriver.ko $(BCM_OBJ_DIR)

$(BCM_OBJ_DIR):
	$(Q_)mkdir -p $(BCM_OBJ_DIR)

clean:
	$(MAKE) -C ../../usermode ARCH=mips $@
	$(RM) -r $(BCM_OBJ_DIR)

else

# for 2.6.18 and others, use our own module Makefile
.PHONY: checkdirs driver $(DRIVERS)
all: checkdirs driver
SYSTEM := linuxkernel
include Common.make
include ../../../../build/refsw_inc.mak
include ../usermode/usermode.mak

driver: checkdirs ${DRIVERS}

endif
