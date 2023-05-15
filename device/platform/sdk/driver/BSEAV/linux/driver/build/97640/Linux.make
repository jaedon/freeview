############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Linux.make $
# $brcm_Revision: 3 $
# $brcm_Date: 2/12/11 4:06p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97640/Linux.make $
# 
# 3   2/12/11 4:06p etrudeau
# SWBLURAY-24485: remove appending chip number to linux include file
# 
# 2   2/11/11 9:26a etrudeau
# SWBLURAY-24485: Linux Makefile is in chip directory
# 
# 1   2/10/11 3:21p etrudeau
# SWBLURAY-24485: make Linux.make and tools.mak into real files not
# symlinks to 7630 since 7640 Linux include path has changed and
# toolchain uses mipsel-linux-uclibc- prefix
# 
# 2   2/3/11 2:50p etrudeau
# SWBLURAY-24485: provide more info when include dir does not exist and
# identify 7640 as a REV_SPECIFIC Linux include file platform
# 
# 1   12/10/10 5:41p etrudeau
# SWBLURAY-23579: merge 7630 support to Hydra to get ready for 7640 chip
# 
# bdvd_v3.0/1   9/11/09 4:38p jquinlan
# pr 16362
# 
# bdvd_v3.0/1   2/27/09 4:44p jquinlan
# new file
# 
# BCM7601_bringup_temp/1   7/10/08 4:47p fbasso
# Add support for 7601.
# 
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

include $(BSEAV)/build/platforms.mak

ifndef PLATFORM
$(error PLATFORM must be defined)
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
# Check for SMP-aware kernel option
ifeq ($(SMP),y)
#
# SMP
#
ifeq ($(findstring $(PLATFORM), $(LINUX_REV_SPECIFIC_SMP_PLATFORMS)),)
# Same kernel across all revisions
SHARED_LINUX_INC := $(LINUX)/include.$(ARCH).$(PLATFORM)-smp
DEFAULT_LINUX_INC := $(BUILDDIR)/linux_include_smp
else
# Rev-specific kernels
SHARED_LINUX_INC := $(LINUX)/include.$(ARCH).$(PLATFORM)$(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')-smp
DEFAULT_LINUX_INC := $(BUILDDIR)/linux_include_$(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')_smp
endif
else
#
# Non-SMP
#
ifeq ($(findstring $(PLATFORM), $(LINUX_REV_SPECIFIC_PLATFORMS)),)
# Same kernel across all revisions
SHARED_LINUX_INC := $(LINUX)/include
DEFAULT_LINUX_INC := $(BUILDDIR)/linux_include
else
# Rev-specific kernels
SHARED_LINUX_INC := $(LINUX)/include.$(ARCH).$(PLATFORM)$(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
DEFAULT_LINUX_INC := $(BUILDDIR)/linux_include_$(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
endif
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
PLATFORM_PATH = $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
# Use c0 for all cx platforms
ifeq ($(findstring $(PLATFORM_PATH), c1 c2 c3),$(PLATFORM_PATH))
PLATFORM_PATH := c0
endif

ifeq ($(PLATFORM),97403)
ifeq ($(PLATFORM_PATH),a1)
PLATFORM_PATH := a0
endif
endif

ifeq ($(PLATFORM),97458)
ifeq ($(PLATFORM_PATH),a1)
PLATFORM_PATH := a0
endif
endif

ifeq ($(shell (grep 'SUBLEVEL = 12' ${LINUX}/Makefile >/dev/null && echo 'y')),y)
ifeq ($(findstring $(PLATFORM),97400 97456),$(PLATFORM))
ifeq ($(findstring $(PLATFORM_PATH),c0 d0),$(PLATFORM_PATH))
PLATFORM_PATH := b0
endif
endif
else
ifeq ($(findstring $(PLATFORM),97400 97456),$(PLATFORM))
ifeq ($(findstring $(PLATFORM_PATH),b0 c0),$(PLATFORM_PATH))
PLATFORM_PATH := d0
endif
endif
endif

ifeq ($(SMP),y)
PLATFORM_PATH := ${PLATFORM_PATH}-smp
endif

# this follows the general rule. define DEFCONFIG if exceptions exist
ifeq ($(DEFCONFIG),)
ifeq ($(ARCH),mips-uclibc)
# big endian, 2.6.12-2.2a and beyond
DEFCONFIG := arch/mips/configs/bcm${PLATFORM}${PLATFORM_PATH}_be_defconfig
else
# little endian
DEFCONFIG := arch/mips/configs/bcm${PLATFORM}${PLATFORM_PATH}_defconfig
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
	@echo "The Linux include files were not found at ${LINUX_INC}"
	@echo "  or $(SHARED_LINUX_INC)"
	@echo "Configuring Linux:" $(LINUX_INC) "using" $(LINUX)/$(DEFCONFIG)
	if [ ! -w $(LINUX) ]; then \
		echo; \
		echo "ERROR: Configuring kernel source requires write access to $(LINUX)."; \
		echo; \
		test -w $(LINUX); \
	fi
	(\
	cd ${LINUX};${CP} -p ${DEFCONFIG} .config &&\
	make silentoldconfig ARCH=mips  && \
	make prepare ARCH=mips) && \
	@echo "Copying linux include directory..." && \
	${RM} -rf ${LINUX_INC}.tmp.$$$$ && \
	${CP} -a ${LINUX}/include ${LINUX_INC}.tmp.$$$$ &&\
	mv ${LINUX_INC}.tmp.$$$$ ${LINUX_INC}

else
# 2.4 kernel include creation 
${LINUX_INC}: ${LINUX}
	@echo "The Linux include files were not found at ${LINUX_INC}"
	@echo "  or $(SHARED_LINUX_INC)"
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
