###############################################################################
#                                                                             #
#                             Humax Templte Makefile                          #
#                                                                             #
# Author: dklee (Plat1, STB division)                                         #
#                                                                             #
# Copyright (c) Humax co, Ltd., 2007                                          #
#                                                                             #
###############################################################################
# $Header:
# Setup tool name for compile and make lib
# Port this file for each platfrom.
###############################################################################

###############################################################################
# [User Define] Include Project toolset
###############################################################################


###############################################################################
# [User Define] Define Product Name
###############################################################################
export FACTORYSW_NAME := $(PRODUCT_NAME)

###############################################################################
# [User Define] Define Output Library Name
###############################################################################
export FACTORYSW_OUTPUT_NAME := $(BUILD_LIB)

###############################################################################
# [User Define] Define Project Include directories
###############################################################################
FACTORYSW_INCLUDE := -I$(FACTORYSW_DIR)/../../platform/driver/di/drv/include \
					 -I$(FACTORYSW_DIR)/../../di_inc


###############################################################################
# [User Define] Define Compiler Include directory
###############################################################################
FACTORYSW_COMPILER_INC := -I/opt/toolchains/crosstools_hf-linux-2.6.18.0_gcc-4.2-11ts_uclibc-nptl-0.9.29-20070423_20090508/mipsel-linux-uclibc/include \
						  -I/opt/toolchains/crosstools_hf-linux-2.6.18.0_gcc-4.2-11ts_uclibc-nptl-0.9.29-20070423_20090508/lib/gcc/mipsel-linux-uclibc/4.2.0/include


###############################################################################
# [User Define] Define Compiler and Options
###############################################################################
FACTORYSW_CC 			:= @/opt/toolchains/crosstools_hf-linux-2.6.18.0_gcc-4.2-11ts_uclibc-nptl-0.9.29-20070423_20090508/bin/mipsel-linux-uclibc-gcc
FACTORYSW_CFLAGS 		:= -funwind-tables -O0 -g -c

ifeq ($(CONFIG_PLATFORM_ALI),y)
FACTORYSW_CFLAGS		+= -EL -mips32r2 -msoft-float -fsigned-char -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
FACTORYSW_CFLAGS		+= -fdata-sections -ffunction-sections -fno-strict-aliasing -fno-builtin
endif

ifeq ($(DEBUG),y)
FACTORYSW_CFLAGS		+= -DCONFIG_DEBUG
endif

FACTORYSW_AR			:= @/opt/toolchains/crosstools_hf-linux-2.6.18.0_gcc-4.2-11ts_uclibc-nptl-0.9.29-20070423_20090508/bin/mipsel-linux-uclibc-ar
FACTORYSW_ARFLAGS 		:= -crs


###############################################################################
# [User Define] Define Tool Macros
###############################################################################
FACTORYSW_MAKE 			:= make
FACTORYSW_RM			:= rm -rf
FACTORYSW_ECHO			:= echo
FACTORYSW_MKDIR			:= mkdir -p
FACTORYSW_CP    		:= cp

FACTORYSW_HOST_ENV=$(shell uname)

ifeq ($(FACTORYSW_HOST_ENV),Linux)
FACTORYSW_CFGTOH		:= cfgtoh
FACTORYSW_MAKEDEP		:= makedepend
else
FACTORYSW_CFGTOH		:= cfgtoh.exe
FACTORYSW_MAKEDEP		:= makedepend.exe
endif

###############################################################################
# [Reserved] Do not change this!!
###############################################################################
ifneq ($(windir),)  #For, Windows
FACTORYSW_OS      		:= WINDOWS
else
FACTORYSW_OS      		:= LINUX
endif

ifeq ($(FACTORYSW_OS), WINDOWS)
FACTORYSW_SED 			:= $(FACTORYSW_MAKE_DIR)/bin/sed
else
FACTORYSW_SED 			:= sed
endif

FACTORYSW_CFLAGS += -DFACTORY_BUILD_OS_$(FACTORYSW_OS)
