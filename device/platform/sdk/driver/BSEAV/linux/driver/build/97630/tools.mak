############################################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: tools.mak $
# $brcm_Revision: 1 $
# $brcm_Date: 12/10/10 5:41p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/linux/driver/build/97630/tools.mak $
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
###########################################################################

# This make include defines macros for cross-development tools.
# It is used in a wide range of environments, so don't add anything
# that doesn't belong here.

# ARCH must indentify the CPU and operating system being compiled for.
# If you chose a value of ARCH which doesn't uniquely identify this, you
# will have an unstable or hacked-up build system elsewhere.

# Valid values include:
#  i386-linux = Intel Linux systems(e.g. unix host compiler)
#  i386-mingw = host compiler in Windows Mingw environment
#  mipsel-linux = glibc, little endian Linux on MIPS
#  mips-linux = glibc, big endian Linux on MIPS
#  mipsel-uclibc = uclibc (busybox), little endian Linux on MIPS
#  mips-uclibc = uclibc (busybox), big endian Linux on MIPS
#  mips-vxworks = vxworks, big endian on MIPS

# Handle vxworks from either the SYSTEM or ARCH variable.
ifeq ($(ARCH),mips-vxworks)
SYSTEM=vxworks
endif

ifeq ($(SYSTEM),vxworks)

ARCH = mips-vxworks
CROSS_COMPILE = mips
CC      = cc${CROSS_COMPILE}
CXX     = c++${CROSS_COMPILE}
LD      = ld${CROSS_COMPILE}
AR      = ar${CROSS_COMPILE}
NM      = nm${CROSS_COMPILE}
STRIP   = strip${CROSS_COMPILE}
OBJCOPY = objcopy${CROSS_COMPILE}
OBJDUMP = objdump${CROSS_COMPILE}
RANLIB  = ranlib${CROSS_COMPILE}

# MKDIR must make recursive dirs, and not fail if already existing
ifeq ($(OSTYPE),linux)
MKDIR   = mkdir -p
PWD     = pwd
MV      = mv
else
ifeq ($(vxWorksVersion),6)
MKDIR   = mkdir -p
PWD     = pwd
MV      = mv
else
# These are really DOS options:
MKDIR   = -mkdir
PWD     = chdir
MV      = move
endif
endif

else

#
# Default toolchain
#
ARCH ?= mipsel-uclibc
SYSTEM ?= linux

#
# Set variables based on the toolchain
#
ifeq ($(findstring ${ARCH}, i386-linux i386-mingw), ${ARCH})
CROSS_COMPILE ?=
TOOLCHAIN_DIR=/usr/bin
else
CROSS_COMPILE ?= $(ARCH)-
ifeq ($(ARCH),mipsel-linux)
TOOLCHAIN_DIR=/opt/toolchains/mipsel
else
#
# Discover the uclibc toolchain directory assuming the compiler exists in bin subdir
# Use which and dirname bash shell commands.
#
TOOLCHAIN_DIR=$(shell dirname $(shell dirname $(shell which mipsel-uclibc-gcc)))
endif
endif

# Define make variables
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)gcc
# NOTE: We must use c++ and not g++. g++ uses libraries with GPL code which we don't want to link with.
CXX     = $(CROSS_COMPILE)c++
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
STRIP   = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB  = $(CROSS_COMPILE)ranlib
MKDIR   = mkdir -p
PWD     = pwd
MV      = mv

endif

# These are operations common to all environments.
CPP     = $(CC) -E
CP      = cp -f
RM      = rm -f
SORT    = sort
SED     = sed
TOUCH   = touch
AWK 	= awk

# Define options for quiet builds
export VERBOSE
ifneq ($(VERBOSE),)
Q_:=
else
# This was Q_?=@, but that caused vxWorks problems.  VERBOSE=y must be used now to get verbose msgs.
Q_:=@
MAKEFLAGS += --no-print-directory
endif

