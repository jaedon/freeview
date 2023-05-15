############################################################
#     Copyright (c) 2003-2009, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 18 $
# $brcm_Date: 10/20/09 2:17p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97401/Makefile $
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

BUILDDIR=$(shell pwd)
BSEAV=$(shell cd ../../../..;pwd)

LINUX ?= /opt/brcm/linux
LINUX_VER_GE_2_6 ?= $(shell (grep 'PATCHLEVEL = 6' ${LINUX}/Makefile >/dev/null && echo 'y'))
ifeq ($(LINUX_VER_GE_2_6),y)
LINUX_VER_EQ_2_6_31 := $(shell (grep 'SUBLEVEL = 31' ${LINUX}/Makefile >/dev/null && echo 'y'))
endif

.PHONY: checkdirs driver $(DRIVERS)
all: checkdirs driver

SYSTEM := linuxkernel

ifeq ($(LINUX_VER_EQ_2_6_31),y)
include Common.2.6.31.make
else
include Common.make
endif

include ../../../../build/refsw_inc.mak
include ../usermode/usermode.mak

driver: checkdirs ${DRIVERS}

