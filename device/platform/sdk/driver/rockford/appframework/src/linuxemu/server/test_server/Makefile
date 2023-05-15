#############################################################################
#
#       Copyright (c) 2003-2009, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 5/8/09 12:31p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/src/linuxemu/server/test_server/Makefile $
# 
# Hydra_Software_Devel/2   5/8/09 12:31p jessem
# PR 54380: Changed ikos_server.c to test_server.c
# 
# Hydra_Software_Devel/1   5/5/09 4:58p jessem
# PR 54380: Initial version.
# 
# Hydra_Software_Devel/1   4/28/09 2:47p jessem
# PR 54380: Initial version.
#
#############################################################################

# Requires the use of the 'plat' script which defines the following:
#   PLATFORM (97038, 97395, ...)
#   BCHP_REV (B0, B1, C0, ...)
#   DEBUG
#   LINUX
#   ARCH
#   TOOLCHAIN

# Target O/S (can be one of the following: linuxuser linuxclient vxworks)
ifeq ($(SYSTEM),vxworks)
    TARGET_OS = vxworks
else
ifeq ($(SYSTEM), linuxemu)
    TARGET_OS = linuxemu
else
    TARGET_OS = linuxuser
endif
endif


# Directories relative to location of makefile
MAGNUM_DIR = ../../../../../../magnum
ROCKFORD_DIR = ../../../../../../rockford
FRAMEWORK_DIR = $(ROCKFORD_DIR)/appframework
EMU_SERVER_DIR = $(ROCKFORD_DIR)/appframework/src/linuxemu/server

# Name of application
ifeq (${SYSTEM},vxworks)
APP = test_server.out
else
APP = test_server
endif

TIPSIM ?= n
ifeq (${TIPSIM}, y)
	CFLAGS += -DTIPSIM
else
	CFLAGS += -DSETTOPBOX
endif

ifeq ($(TARGET_OS),vxworks)
CFLAGS += -DVXWORKS
else
	ifeq ($(TARGET_OS),linuxemu)
	CFLAGS += -DEMULATION
	endif
CFLAGS += -DLINUX
endif

# Application object file list, include path, and C flags
APP_SOURCES = \
	test_server_app.c

# The following an be modified accordingly
APP_SOURCES += test_server.c

APP_INCLUDES = ./
APP_CFLAGS =
APP_LDFLAGS=

# Show build commands (y/n)?
VERBOSE=n

# Must define ENDIAN to be either big or little
ifeq ($(SYSTEM),vxworks)
ENDIAN ?= big
else
ENDIAN ?= little
endif

include ${FRAMEWORK_DIR}/src/linuxclient/server/server.inc

# Include object file list, include path, and C flags for common utilities and base modules
# (sets MAGNUM_SOURCES, MAGNUM_INCLUDES, MAGNUM_CFLAGS, and MAGNUM_MODULE_DIR)
include $(FRAMEWORK_DIR)/build/magnum.inc

# Specifies how magnum files and application files are built.
include $(FRAMEWORK_DIR)/build/$(TARGET_OS)/build.inc

