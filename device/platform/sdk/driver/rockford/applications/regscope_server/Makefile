#############################################################################
#
#       Copyright (c) 2003-2010, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/8 $
# $brcm_Date: 9/29/10 1:27p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/applications/regscope_server/Makefile $
# 
# Hydra_Software_Devel/8   9/29/10 1:27p hongtaoz
# SW7425-10: added stub_server compile option for user customizable reg
# read value;
# 
# Hydra_Software_Devel/7   9/9/10 2:37p hongtaoz
# SW7425-10: renamed stub server app for emulation test;
# 
# Hydra_Software_Devel/6   9/8/10 10:53a hongtaoz
# SW7425-10, SW7425-28: added faked ikos server capability;
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

# Target O/S (can be one of the following: linuxuser vxworks)
ifeq ($(SYSTEM),vxworks)
    TARGET_OS = vxworks
	APP = regscope_server.out
else
ifeq ($(SYSTEM), linuxemu)
    TARGET_OS = linuxemu
	APP = stub_server
else
    TARGET_OS = linuxuser
	APP = regscope_server
endif
endif

# Directories relative to location of makefile
MAGNUM_DIR = ../../../magnum
ROCKFORD_DIR = ../../../rockford
FRAMEWORK_DIR = $(ROCKFORD_DIR)/appframework

TIPSIM ?= n
ifeq (${TIPSIM}, y)
	CFLAGS += -DTIPSIM
else
	CFLAGS += -DSETTOPBOX
endif

ifeq (${SYSTEM},vxworks)
	CFLAGS += -DVXWORKS
else
	ifeq ($(TARGET_OS),linuxemu)
		CFLAGS += -DEMULATION
		ifeq ($(STUB_SERVER), y)
			CFLAGS += -DSTUB_SERVER=1
		endif
	endif
	CFLAGS += -DLINUX
endif

# Application object file list, include path, and C flags
APP_SOURCES = ./regscope_server.c 

APP_INCLUDES = ./
APP_CFLAGS =
ifneq ($(SYSTEM), linuxemu)
APP_LDFLAGS= -static
endif

# Define other chips to use. If not defined, files will not be included.
#BCHP_QAM_CHIP = 3250
#BCHP_SDS_CHIP = 4500
#BCHP_VSB_CHIP = 3510

# Show build commands (y/n)?
VERBOSE?=n

# Debug (y/n)?
ifeq ($(origin DEBUG),undefined)
DEBUG = y
endif

# Must define ENDIAN to be either big or little
ifeq ($(SYSTEM),vxworks)
ENDIAN ?= big
else
ENDIAN ?= little
endif

# Include object file list, include path, and C flags for common utilities and base modules 
# (sets MAGNUM_SOURCES, MAGNUM_INCLUDES, MAGNUM_CFLAGS, and MAGNUM_MODULE_DIR)
include $(FRAMEWORK_DIR)/build/magnum.inc

# Include object file list, include path, and C flags for other magnum modules required
# by this application (adds to MAGNUM_SOURCES, MAGNUM_INCLUDES, and MAGNUM_CFLAGS).
# Here is where you also define module specific makefile defines

# At this point, user can add additional files, include paths, and C flags to 
# MAGNUM_OBJ, MAGNUM_INC, and MAGNUM_CFLAGS (optional).

# Include other common modules (adds to COMMON_SOURCES, COMMON_INCLUDES, and
# COMMON_CFLAGS)
#include $(FRAMEWORK_DIR)/build/modules/common/fpga.inc

# Specifies how magnum files and application files are built.
include $(FRAMEWORK_DIR)/build/$(TARGET_OS)/build.inc

# End of file
