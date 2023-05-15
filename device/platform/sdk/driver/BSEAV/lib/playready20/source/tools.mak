#
# Default toolchain
#
#ARCH ?= mipsel-linux-uclibc
ARCH = i386-linux
SYSTEM ?= linux

#
# Set variables based on the toolchain
#
ifeq ($(ARCH),i386-linux)
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

CROSS_COMPILE = mipsel-linux-
#CROSS_COMPILE =

# Define make variables
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)gcc
# NOTE: We must use c++ and not g++ to avoid GPL contamination
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

# These are operations common to all environments.
CPP     = $(CC) -E
CP      = cp -f
RM      = rm -f
SORT    = sort
SED     = sed
TOUCH   = touch
MAKE	= make
