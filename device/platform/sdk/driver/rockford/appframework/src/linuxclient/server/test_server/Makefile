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
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 4/28/09 2:47p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/src/linuxclient/server/test_server/Makefile $
# 
# Hydra_Software_Devel/1   4/28/09 2:47p jessem
# PR 54380: Initial version.
# 
# Hydra_Software_Devel/5   1/4/07 3:44p hongtaoz
# PR22568: updated regscope server to be appframework compliant;
# 
# Hydra_Software_Devel/4   12/5/06 6:45p pntruong
# PR21065: fixed build error.
#
# Hydra_Software_Devel/3   5/10/06 12:27p hongtaoz
# PR21065: added kernel interface dependency to the makefile;
#
# Hydra_Software_Devel/2   2/2/06 3:33p hongtaoz
# PR19082: bring up 7400;
#
# Hydra_Software_Devel/1   9/7/05 8:37p hongtaoz
# PR17038: move regscope_server into rockford using appframework;
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
ifeq ($(SYSTEM), linuxclient)
    TARGET_OS = linuxclient
else
    TARGET_OS = linuxuser
endif
endif


# Directories relative to location of makefile
MAGNUM_DIR = ../../../../../../magnum
ROCKFORD_DIR = ../../../../../../rockford
FRAMEWORK_DIR = $(ROCKFORD_DIR)/appframework
EMU_SERVER_DIR = $(ROCKFORD_DIR)/appframework/src/linuxclient/server

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
	ifeq ($(TARGET_OS),linuxclient)
	CFLAGS += -DEMULATION
	endif
CFLAGS += -DLINUX
endif

# Application object file list, include path, and C flags
APP_SOURCES = \
	test_server_app.c

# The following an be modified accordingly
APP_SOURCES += ikos_server.c

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

