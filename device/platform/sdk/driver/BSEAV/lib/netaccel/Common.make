############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Common.make $
# $brcm_Revision: 5 $
# $brcm_Date: 10/25/11 11:26a $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/lib/netaccel/Common.make $
# 
# 5   10/25/11 11:26a mward
# SW7125-1119: Correct a substitution error.
# 
# 4   10/19/11 4:35p mward
# SW7125-1119: Change DEBUG to B_REFSW_DEBUG.
# 
# 3   4/4/11 2:41p jrubio
# SW7400-2511: remove arch
# 
# 2   12/2/09 6:35p ssood
# SW7405-3154: disable gcc option to use its soft float library as gcc's
# float->int expression doesn't truncate properly (MIPS FPU instead
# should be used)
# 
# 1   10/26/09 4:40p lwhite
# SW7405-3154: Add Support for linux 2.6.31
# 
############################################################

#
# Common driver build for all platforms
# This should not be used to build usermode code
#
# Inputs: DRIVERS
# Outputs: Lots of stuff
#

#BSEAV=$(shell cd ../../../..;pwd)

# backward-compatibility
ifdef ARCH
B_REFSW_ARCH ?= $(ARCH)
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
#include Linux.make
include ${BSEAV}/linux/driver/build/${PLATFORM}/Linux.make

# defines TOOLCHAIN_DIR, B_REFSW_DEBUG, B_REFSW_ARCH
#include tools.mak

ifeq ($(B_REFSW_DEBUG),y)
BCM_OBJ_DIR=$(B_REFSW_ARCH).debug$(NEXUS_BIN_DIR_SUFFIX)
else
BCM_OBJ_DIR=$(B_REFSW_ARCH).release$(NEXUS_BIN_DIR_SUFFIX)
endif
#########################################################
#
# Standard include directories
# See tools.mak for TOOLCHAIN_DIR
#
STD_INC ?= $(TOOLCHAIN_DIR)/mipsel-linux/include
GCC_BASE ?= $(shell (test -d $(TOOLCHAIN_DIR)/lib/gcc-lib && echo $(TOOLCHAIN_DIR)/lib/gcc-lib/mipsel-linux) || echo $(TOOLCHAIN_DIR)/lib/gcc/mipsel-linux)

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

#$(BCM_OBJ_DIR):
#	${Q_}mkdir -p $(BCM_OBJ_DIR)

.PHONY: checkdirs
checkdirs: $(LINUX_INC) $(STD_INC) $(GCC_BASE) $(BSEAV) $(BCM_OBJ_DIR)

ifeq ($(B_REFSW_DEBUG),y)
CFLAGS += -DCONFIG_PROC_FS
endif

