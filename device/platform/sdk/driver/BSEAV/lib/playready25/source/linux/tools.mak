#
# Default toolchain
#
#TOOLCHAIN_DIR=$(shell dirname $(shell dirname $(shell which mipsel-uclibc-gcc)))

#ARCH ?= mipsel-linux
#ARCH = i386-linux
#SYSTEM ?= linux
CROSS_COMPILE=$(CROSS_COMPILE_PREFIX)

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
