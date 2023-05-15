############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# 
############################################################

DEFCONFIG = arch/mips/configs/bcm97630_defconfig

BUILDDIR=$(shell pwd)

.PHONY: checkdirs driver $(DRIVERS)
all: checkdirs driver

SYSTEM := linuxkernel
include Common.make
include tools.mak
include ../../../../build/refsw_inc.mak
include ../usermode/usermode.mak

driver: checkdirs ${DRIVERS}

