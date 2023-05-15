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
# $brcm_Revision: 8 $
# $brcm_Date: 9/17/12 4:37p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97405/Makefile $
# 
# 8   9/17/12 4:37p mphillip
# SW7425-3739: Update legacy driver target directories to match new
# hierarchy
# 
# 7   4/7/11 7:50p katrep
# SW7405-5229:add support multipe heaps
# 
# 4   2/23/09 11:15a katrep
# PR52401: Fixed the build issue without precompiled header files for
# 7405 C0.
# 
# 1   2/14/07 4:35p katrep
# PR27643: Added support 97405
# 
# 8   1/31/07 4:08p jgarrett
# PR 20139: Converting to new driver build
# 
# 7   11/16/06 5:08p jgarrett
# PR 25230: Relocating usermode driver to BSEAV
# 
# 6   9/20/06 3:54p jackli
# PR24025: restore DEFCONFIG for 7400
# 
# 5   9/1/06 1:41p jackli
# PR24025: enable ip streaming in kernelmode settop api
# 
# 5   9/1/06 1:31p jackli
# PR24025: enable ip streaming in kernelmode settop api
# 
# 5   8/31/06 5:23p jackli
# PR24025: add ip support in kernel mode api in 97456
# 
# 5   8/31/06 5:20p jackli
# PR24025: add ip support in kernel mode api in 97456
# 
# 4   7/19/06 5:39p jgarrett
# PR 19909: Fixing make -j
# 
# 3   7/14/06 11:32a jgarrett
# PR 19909: Merging to latest baseline
# 
# PR19909/2   7/6/06 4:44p jgarrett
# PR 19909: Merging to latest baseline
# 
# 2   6/14/06 5:39p jgarrett
# PR 20139: Allowing configuration of 2.6.x kernels
# $brcm_Log: /BSEAV/linux/driver/build/97405/Makefile $
# 
# 8   9/17/12 4:37p mphillip
# SW7425-3739: Update legacy driver target directories to match new
# hierarchy
# 
# 7   4/7/11 7:50p katrep
# SW7405-5229:add support multipe heaps
# 
# 4   2/23/09 11:15a katrep
# PR52401: Fixed the build issue without precompiled header files for
# 7405 C0.
# 
# 1   2/14/07 4:35p katrep
# PR27643: Added support 97405
# 
# 8   1/31/07 4:08p jgarrett
# PR 20139: Converting to new driver build
# 
# 7   11/16/06 5:08p jgarrett
# PR 25230: Relocating usermode driver to BSEAV
# 
# 6   9/20/06 3:54p jackli
# PR24025: restore DEFCONFIG for 7400
# 
# 5   9/1/06 1:41p jackli
# PR24025: enable ip streaming in kernelmode settop api
# 
# 5   9/1/06 1:31p jackli
# PR24025: enable ip streaming in kernelmode settop api
# 
# 5   8/31/06 5:23p jackli
# PR24025: add ip support in kernel mode api in 97456
# 
# 5   8/31/06 5:20p jackli
# PR24025: add ip support in kernel mode api in 97456
# 
# 4   7/19/06 5:39p jgarrett
# PR 19909: Fixing make -j
# 
# 3   7/14/06 11:32a jgarrett
# PR 19909: Merging to latest baseline
# 
# PR19909/2   7/6/06 4:44p jgarrett
# PR 19909: Merging to latest baseline
# 
# PR19909/1   7/6/06 11:04a jgarrett
# PR 19909: Reducing build output
# 
# 5   1/9/06 4:26p erickson
# PR18945: default LINUX so that it's not required
# 
# 4   10/17/05 1:58p vsilyaev
# PR17108: build driver from the source code
# 
############################################################

BSEAV=$(shell cd ../../../..;pwd)

LINUX ?= /opt/brcm/linux
LINUX_VER_GE_2_6 ?= $(shell (grep 'PATCHLEVEL = 6' ${LINUX}/Makefile >/dev/null && echo 'y'))
ifeq ($(LINUX_VER_GE_2_6),y)
LINUX_VER_EQ_2_6_31 := $(shell (grep 'SUBLEVEL = 31' ${LINUX}/Makefile >/dev/null && echo 'y'))
endif

ifeq ($(LINUX_VER_EQ_2_6_31),y)

.PHONY: checkdirs driver $(DRIVERS)
all: checkdirs driver
include Common.2.6.31.make
include ../usermode/usermode.mak
driver: checkdirs $(DRIVERS)

else

# Check for SMP vs. non-SMP kernel configuration
ifeq ($(SMP),y)
DEFCONFIG = arch/mips/configs/bcm97405b0-smp_defconfig
else
DEFCONFIG = arch/mips/configs/bcm97405b0_defconfig
endif

include ../97400/Makefile

endif
