############################################################
#     Copyright (c) 2003-2006, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Linux.make $
# $brcm_Revision: 4 $
# $brcm_Date: 11/16/06 5:08p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/common/Linux.make $
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
# BCM_CHIP - chip number (e.g. 7115)
#
# Output parameters:
# LINUX_INC - path to the linux includes

ifndef PLATFORM
$(error PLATFORM must be defined)
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
ARCH ?= mipsel-uclibc

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
SHARED_LINUX_INC := $(LINUX)/include.$(ARCH).$(PLATFORM)

# Check for SMP-aware kernel option
ifeq ($(SMP),y)
SHARED_LINUX_INC := $(SHARED_LINUX_INC)-smp
endif

ifeq ($(shell test -d $(SHARED_LINUX_INC) && echo "y"),y)
LINUX_INC = $(SHARED_LINUX_INC)
else
LINUX_INC := $(BUILDDIR)/linux_include

# Check for SMP-aware kernel option
ifeq ($(SMP),y)
LINUX_INC := $(LINUX_INC)-smp
endif

# Only clean up a local LINUX_INC, not a shared one.
OTHER_CLEANS += linuxinc_clean
endif

#
# Having this rule allows us to automate all-platform linux include builds
#
build_linux_include: ${LINUX_INC}

#
# If the defconfig is something other than the standard name,
# it must be defined in the platform-specific Makefile.
#
ifeq ($(ARCH),mipsel-linux)
# glibc
DEFCONFIG ?= arch/mips/defconfig-brcm-$(BCM_CHIP)
else
# uclibc, default
DEFCONFIG ?= arch/mips/defconfig-brcm-bb-$(BCM_CHIP)
endif

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
	echo "Configuring Linux:" $(LINUX_INC) "using" $(LINUX)/$(DEFCONFIG)
	if [ ! -w $(LINUX) ]; then \
		echo; \
		echo "ERROR: Configuring kernel source requires write access to $(LINUX)."; \
		echo; \
		test -w $(LINUX); \
	fi
	(\
	cd ${LINUX};${CP} -p ${DEFCONFIG} .config &&\
	make silentoldconfig ARCH=mips  && \
	make include/linux/version.h ARCH=mips &&\
	make include/asm ARCH=mips && make include/config/MARKER ARCH=mips)  && \
	echo "Copying linux include directory..." && \
	${RM} -rf ${LINUX_INC}.tmp.$$$$ && \
	${CP} -a ${LINUX}/include ${LINUX_INC}.tmp.$$$$ &&\
	mv ${LINUX_INC}.tmp.$$$$ ${LINUX_INC}

else
# 2.4 kernel include creation 
${LINUX_INC}: ${LINUX}
	@echo "Configuring Linux:" $(LINUX_INC) "using" $(LINUX)/$(DEFCONFIG)
	@if [ ! -w $(LINUX) ]; then \
		echo; \
		echo "ERROR: Configuring kernel source requires write access to $(LINUX)."; \
		echo; \
		test -w $(LINUX); \
	fi
	@(\
	cd ${LINUX};${CP} -p ${DEFCONFIG} .config &&\
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

