
# FreeType 2 configuration rules for a `normal' ANSI system
#


# Copyright 1996-2000, 2003 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.


ifndef TOP_DIR
  TOP_DIR := .
endif

DELETE    := rm -f
SEP       := /
BUILD_DIR := $(TOP_DIR)/builds/refsw

# The directory where all object files are placed.
#
# This lets you build the library in your own directory with something like
#
#   set TOP_DIR=.../path/to/freetype2/top/dir...
#   set OBJ_DIR=.../path/to/obj/dir
#   make -f $TOP_DIR/Makefile setup [options]
#   make -f $TOP_DIR/Makefile
#
include ../../api/build/tools.mak
include ../../build/refsw_inc.mak
ifeq ($(B_REFSW_DEBUG),y)
DEBUG_SUFFIX=debug
else
DEBUG_SUFFIX=release
endif

ifndef OBJ_DIR
OBJ_DIR := $(TOP_DIR)/objs/$(B_REFSW_ARCH).$(DEBUG_SUFFIX)
endif


# The directory where all library files are placed.
#
# By default, this is the same as $(OBJ_DIR); however, this can be changed
# to suit particular needs.
#
ifeq ($(B_REFSW_DEBUG),y)
LIB_DIR := $(TOP_DIR)/lib/$(B_REFSW_ARCH).$(DEBUG_SUFFIX)
endif


# The name of the final library file.  Note that the DOS-specific Makefile
# uses a shorter (8.3) name.
#
LIBRARY := lib$(PROJECT)


# Path inclusion flag.  Some compilers use a different flag than `-I' to
# specify an additional include path.  Examples are `/i=' or `-J'.
#
I := -I


# C flag used to define a macro before the compilation of a given source
# object.  Usually it is `-D' like in `-DDEBUG'.
#
D := -D


# The link flag used to specify a given library file on link.  Note that
# this is only used to compile the demo programs, not the library itself.
#
L := -l


# Target flag.
#
T := -o$(space)


# C flags
#
#   These should concern: debug output, optimization & warnings.
#
#   Use the ANSIFLAGS variable to define the compiler flags used to enfore
#   ANSI compliance.
#

ifndef CFLAGS
  CFLAGS := $(SYSTEM_FLAGS)
  CFLAGS += -c -g -DDARWIN_NO_CARBON
endif

# ANSIFLAGS: Put there the flags used to make your compiler ANSI-compliant.
#
ANSIFLAGS :=


# EOF
