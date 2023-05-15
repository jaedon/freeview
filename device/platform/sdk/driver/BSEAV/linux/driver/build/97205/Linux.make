############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Linux.make $
# $brcm_Revision: 1 $
# $brcm_Date: 9/13/12 6:31p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97205/Linux.make $
# 
# 1   9/13/12 6:31p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/13/12 12:38p mphillip
# SW7425-3739: Remove symlinks for linux/driver/build subdirectories and
# dependencies
# 
# 29   4/1/11 11:45a jrubio
# SW7400-2511: use mips-uclibc for mips include patch
# 
# 28   3/28/11 5:04p gskerl
# SW7400-2511: use B_REFSW_ARCH=mips-linux instead of mips-uclibc
# 
# 27   3/28/11 1:59p jrubio
# SW7400-2511: use B_REFSW_ARCH=mips-linux instead of mips-uclibc
# 
# 26   3/18/11 2:10p mward
# SW7420-1576: Existing shared Linux includes for 2.6.18 are named
# mips[el]-uclibc not mips[el]-linux.
# 
# 25   1/8/10 10:57a erickson
# SW7550-121: set SHELL variable for bash syntax
# 
# 24   11/17/09 2:03p erickson
# SW7405-3408: use $(filter) instead of $(findstring)
# 
# 23   10/19/09 3:22p erickson
# SW7405-3154: remove include of platforms.mak because the variables it
# defined are no longer used
#
# 22   9/17/09 11:38a erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 21   9/11/09 12:24p erickson
# SW7420-228: 2.6.30 support
#
# 20   5/7/09 3:01p anilmm
# PR54832:  Use B0 for 97459C0 build
#
# 19   3/26/09 11:20p vsilyaev
# PR 53611: Fixed location of SMP include directory
#
# 18   3/26/09 10:57a vsilyaev
# PR 53611: Use simply expanded variables everywhere where $(shell ...)
# is used. It prevents evaluation of $(shell ) every where variable is
# used
#
# 17   3/4/09 1:33p katrep
# PR52599: 7466,7205 now use 7405 defconfig for linux headers
#
# 16   3/2/09 4:20p katrep
# PR52599: Add support for 7205
#
# 15   2/23/09 11:15a katrep
# PR52401: Fixed the build issue without precompiled header files for
# 7405 C0.
#
# 14   9/23/08 1:54p mward
# PR39972: 7400e0 use 7400d0 kernel for 2.6.18.
#
# 13   3/4/08 11:20a mward
# PR37029: For 2.6.18, use 7400d0 defconfigs, for 2.6.12, use 7400b0.
#
# 12   2/21/08 10:34a mward
# PR37029: For 2.6.18-4.1 7400d0 does NOT share b0 kernel.
#
# 11   10/2/07 6:50p mward
# PR35355: Consolidate driver build settings in Linux.make which is
# shared with netaccel.
#
# 9   9/20/07 7:08p mward
# PR35179: 97400 c0 (and d0 for now) use b0 PLATFORM_PATH.
#
# 8   7/16/07 11:40a erickson
# PR33019: all Cx platforms should use Linux c0 config
#
# 7   6/29/07 3:46p ayxiang
# PR32664: adde support on A1 for 97458 3.0
#
# 6   6/12/07 10:21a gmohile
# PR 25109 : Add support for 7403 A1
#
# 5   6/8/07 2:43p jgarrett
# PR 31935: Updating for 2.6.18
#
# 4   5/18/07 3:48p jgarrett
# PR 30810: Supporting chip-rev-specific kernels
#
# 3   1/29/07 1:03p erickson
# PR25902: more generic DEFCONFIG, and override option
#
# 2   1/29/07 12:32p erickson
# PR25902: rework linux drivers to remove duplication
#
# 4   11/16/06 5:08p jgarrett
# PR 25230: Unconditionally defining 2.6 version
#
# 3   8/17/06 12:41p erickson
# PR23574: don't allow ARCH to break kernel build, and switch to
# silentoldconfig
#
# 2   6/12/06 1:45p jgarrett
# PR 20139: Adding SMP option for include path
#
# 1   2/8/05 12:17a dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/18   6/8/04 9:18a erickson
# PR11402: set non-standard DEFCONFIG in platform Makefile
#
# Irvine_BSEAVSW_Devel/18   6/8/04 9:13a erickson
# PR11402: set non-standard DEFCONFIG in platform Makefile
#
# Irvine_BSEAVSW_Devel/16   6/1/04 4:19p erickson
# PR9447: need to set ARCH here
#
# Irvine_BSEAVSW_Devel/15   6/1/04 3:22p erickson
# PR9447: changing default to uclibc
#
# Irvine_BSEAVSW_Devel/14   5/28/04 12:33p erickson
# PR11258: use correct defconfig for 7038b0
#
# Irvine_BSEAVSW_Devel/13   5/12/04 10:31a erickson
# PR10967: use std header
############################################################

# This file creates local copy of the linux include files
# using config file based on a chip number
#
# Input parameters:
# BUILDDIR - destination dir (optional, current directory used by default)
# LINUX - path to the LINUX source tree (optional, /opt/brcm/linux used by default)
# PLATFORM
#
# Output parameters:
# LINUX_INC - path to the linux includes
# OTHER_CLEANS - if linux source is configured, this allows it to be cleaned

SHELL=/bin/bash

# backward compatibility
ifdef DEFCONFIG
LINUX_DEFCONFIG ?= $(DEFCONFIG)
endif
ifdef PLATFORM
NEXUS_PLATFORM ?= $(PLATFORM)
endif
ifdef ARCH
B_REFSW_ARCH ?= $(ARCH)
endif

ifndef NEXUS_PLATFORM
$(error NEXUS_PLATFORM must be defined)
endif

ifndef BUILDDIR
BUILDDIR=.
endif

#
# If LINUX_INC is defined by the user, don't redefine it.
#
ifdef LINUX_INC

# We still want a rule. If it doesn't exist, then Common.make's checkdirs will catch it.
$(LINUX_INC):

else

#
# This is the default location of the linux kernel source.
# This can be a symlink to your latest version of kernel source.
#
LINUX ?= /opt/brcm/linux
# To keep compatible with the old way use another variable
ifeq  ($(B_REFSW_ARCH),mips-linux)
INC_NAME=mips-uclibc
else
INC_NAME=mipsel-uclibc
endif

#
# If you have a shared build environment, you need to set up pre-built
# LINUX_INC directories. This is because the 'make menuconfig' process is not
# reentrant and you'll stomp on each other. The SHARED_LINUX_INC
# variable is tested first. If it exists, it's used as LINUX_INC. Otherwise a local
# LINUX_INC is created.
#
# The easiest way to create SHARED_LINUX_INC directories for each platform is to
# make it locally, then copy it into the LINUX directory.
#
# Check for SMP-aware kernel option
BCHP_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')

ifeq ($(SMP),y)
#
# SMP
#
SHARED_LINUX_INC := $(LINUX)/include.$(INC_NAME).$(NEXUS_PLATFORM)-smp
DEFAULT_LINUX_INC := $(BUILDDIR)/linux_include_smp
else
#
# Non-SMP
#
SHARED_LINUX_INC := $(LINUX)/include.$(INC_NAME).$(NEXUS_PLATFORM)
DEFAULT_LINUX_INC := $(BUILDDIR)/linux_include
endif

ifeq ($(shell test -d $(SHARED_LINUX_INC) && echo "y"),y)
LINUX_INC = $(SHARED_LINUX_INC)
else
LINUX_INC := $(DEFAULT_LINUX_INC)

# Only clean up a local LINUX_INC, not a shared one.
OTHER_CLEANS += linuxinc_clean
endif

#
# Having this rule allows us to automate all-platform linux include builds
#
build_linux_include: ${LINUX_INC}

###########################################################

#
# Determine .config file for linux kernel
#
PLATFORM_PATH := ${BCHP_VER_LOWER}
# Use c0 for all cx platforms
ifeq ($(filter $(PLATFORM_PATH), c1 c2 c3),$(PLATFORM_PATH))
PLATFORM_PATH := c0
endif

ifeq ($(NEXUS_PLATFORM),97403)
ifeq ($(PLATFORM_PATH),a1)
PLATFORM_PATH := a0
endif
endif

ifeq ($(NEXUS_PLATFORM),97458)
ifeq ($(PLATFORM_PATH),a1)
PLATFORM_PATH := a0
endif
endif

ifeq ($(shell (grep 'SUBLEVEL = 12' ${LINUX}/Makefile >/dev/null && echo 'y')),y)
ifeq ($(filter $(NEXUS_PLATFORM),97400 97456),$(NEXUS_PLATFORM))
ifeq ($(filter $(PLATFORM_PATH),c0 d0 e0),$(PLATFORM_PATH))
PLATFORM_PATH := b0
endif
endif
else
ifeq ($(filter $(NEXUS_PLATFORM),97400 97456),$(NEXUS_PLATFORM))
ifeq ($(filter $(PLATFORM_PATH),b0 c0 e0),$(PLATFORM_PATH))
PLATFORM_PATH := d0
endif
endif
endif


ifeq ($(filter $(NEXUS_PLATFORM),97405 97466 97205 97459),$(NEXUS_PLATFORM))
ifeq ($(filter $(PLATFORM_PATH),b0 c0),$(PLATFORM_PATH))
PLATFORM_PATH := b0
endif
endif


ifeq ($(SMP),y)
PLATFORM_PATH := ${PLATFORM_PATH}-smp
endif

ifeq ($(CHIP),)
CHIP=$(patsubst 9%,%,$(NEXUS_PLATFORM))
endif

ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
# big endian
LINUX_DEFCONFIG_ENDIAN=_be
export CONFIG_CPU_BIG_ENDIAN=y
export CONFIG_CPU_LITTLE_ENDIAN=
else
# little endian
LINUX_DEFCONFIG_ENDIAN=
export CONFIG_CPU_LITTLE_ENDIAN=y
export CONFIG_CPU_BIG_ENDIAN=
endif

# Linux kernel doesn't support separate defconfigs for 7466 and 7205
ifeq ($(filter $(NEXUS_PLATFORM), 97466 97205),$(NEXUS_PLATFORM))
LINUX_DEFCONFIG := arch/mips/configs/bcm97405${PLATFORM_PATH}${LINUX_DEFCONFIG_ENDIAN}_defconfig
endif

# define LINUX_DEFCONFIG according to standard naming
ifeq ($(LINUX_DEFCONFIG),)
# this is the standard naming for 2.6.30 and later
LINUX_DEFCONFIG := arch/mips/configs/bcm${CHIP}${PLATFORM_PATH}_defconfig

LINUX_DEFCONFIG_EXISTS = $(shell test -e $(LINUX)/$(LINUX_DEFCONFIG) && echo "y")
ifneq ($(LINUX_DEFCONFIG_EXISTS),y)
# this is the standard naming for 2.6.18 and earlier
LINUX_DEFCONFIG := arch/mips/configs/bcm${NEXUS_PLATFORM}${PLATFORM_PATH}${LINUX_DEFCONFIG_ENDIAN}_defconfig
endif
endif


###########################################################
#
# Rule for configuring kernel source locally
#
LINUX_EXISTS = $(shell test -d $(LINUX) && echo "y")
ifneq ($(LINUX_EXISTS),y)
$(error The LINUX make variable points to a directory which does not exist. LINUX is currently defined as ${LINUX}. It should point to your kernel source.)
endif
LINUX_INC_EXISTS = $(shell test -d $(LINUX_INC) && echo "y")
LINUX_VER_GE_2_6 ?= $(shell (grep 'PATCHLEVEL = 6' ${LINUX}/Makefile >/dev/null && echo 'y'))
ifneq ($(LINUX_INC_EXISTS),y)
ifeq ($(LINUX_VER_GE_2_6),y)
# 2.6 kernel include creation
${LINUX_INC}: ${LINUX}
	echo Configuring Linux: $(LINUX_INC) using $(LINUX)/$(LINUX_DEFCONFIG)
	if [ ! -w $(LINUX) ]; then \
		echo; \
		echo ERROR: Configuring kernel source requires write access to $(LINUX).; \
		echo; \
		test -w $(LINUX); \
	fi
	(\
	cd ${LINUX};${CP} -p ${LINUX_DEFCONFIG} .config &&\
	make silentoldconfig ARCH=mips  && \
	make prepare ARCH=mips) && \
	echo "Copying linux include directory..." && \
	${RM} -rf ${LINUX_INC}.tmp.$$$$ && \
	${CP} -a ${LINUX}/include ${LINUX_INC}.tmp.$$$$ &&\
	mv ${LINUX_INC}.tmp.$$$$ ${LINUX_INC}

else
# 2.4 kernel include creation
${LINUX_INC}: ${LINUX}
	@echo "Configuring Linux:" $(LINUX_INC) "using" $(LINUX)/$(LINUX_DEFCONFIG)
	@if [ ! -w $(LINUX) ]; then \
		echo; \
		echo "ERROR: Configuring kernel source requires write access to $(LINUX)."; \
		echo; \
		test -w $(LINUX); \
	fi
	@(\
	cd ${LINUX};${CP} -p ${LINUX_DEFCONFIG} .config &&\
	echo 'x'|make menuconfig && make CROSS_COMPILE=$(CROSS_COMPILE) dep) && \
	echo "Copying linux include directory..." && \
	${RM} -rf ${LINUX_INC}.tmp.$$$$ && \
	${CP} -a ${LINUX}/include ${LINUX_INC}.tmp.$$$$ &&\
	mv ${LINUX_INC}.tmp.$$$$ ${LINUX_INC}

endif
endif

linuxinc_clean:
	${RM} -r ${LINUX_INC}

endif
