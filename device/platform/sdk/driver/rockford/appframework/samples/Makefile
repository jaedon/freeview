#############################################################################
#
#       Copyright (c) 2005-2011, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/6 $
# $brcm_Date: 3/16/11 5:02p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/samples/Makefile $
# 
# Hydra_Software_Devel/6   3/16/11 5:02p pntruong
# SW7425-164: Default B_REFSW_DEBUG=y if unset in framework build system
# so not every single app needs to specify.
#
# Hydra_Software_Devel/5   6/22/10 11:30a vanessah
# SW7422-12:  To support appframework. Missing files added:
# magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
# do list: 1. in framework_board.c, more initialization to be done.  2.
# More registers mapping, like clock generation as well as
# BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
#
# Hydra_Software_Devel/5   6/22/10 11:30a vanessah
# SW7422-12:  To support appframework. Missing files added:
# magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
# do list: 1. in framework_board.c, more initialization to be done.  2.
# More registers mapping, like clock generation as well as
# BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
#
# Hydra_Software_Devel/5   6/22/10 10:59a vanessah
# SW7422-12:  To support appframework. Missing files added:
# magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
# do list: 1. in framework_board.c, more initialization to be done.  2.
# More registers mapping, like clock generation as well as
# BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
#
# Hydra_Software_Devel/5   6/22/10 10:56a vanessah
# SW7422-12:  To support appframework. Missing files added:
# magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
# do list: 1. in framework_board.c, more initialization to be done.  2.
# More registers mapping, like clock generation as well as
# BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
#
# Hydra_Software_Devel/4   2/16/06 5:46p jasonh
# PR 19565: Cleaned up makefile
#
# Hydra_Software_Devel/3   8/31/05 5:24p jasonh
# PR 16943: Changes for MAGNUM_MODULE_DIR and COMMON files.
#
# Hydra_Software_Devel/2   7/15/05 1:46p jasonh
# PR 13863: Fixed comments
#
# Hydra_Software_Devel/1   1/19/05 5:27p jasonh
# PR 13863: Adding updated application framework.
#
#############################################################################

# Requires the use of the 'plat' script which defines the following:
#   PLATFORM (97038, 97398, ...)
#   BCHP_VER (B0, B1, C0, ...)
#   BCHP_7411_VER (C0, D0, ...) plus versions for other secondary chipsets
#   B_REFSW_DEBUG
#   B_REFSW_ARCH
#   LINUX
#   TOOLCHAIN

# Target O/S (can be one of the following: linuxuser)
TARGET_OS = linuxuser

# Directories relative to location of makefile
MAGNUM_DIR = ../../../magnum
ROCKFORD_DIR = ../../../rockford
FRAMEWORK_DIR = $(ROCKFORD_DIR)/appframework

# Name of application
APP = test

# Application source file list, include path, C flags, and link
# flags
APP_SOURCES= ./test.c
APP_INCLUDES =
APP_CFLAGS=
APP_LDFLAGS=

# Want to see details (y/n)?
VERBOSE = n

# Include object file list, include path, and C flags for common utilities and base modules
# (sets MAGNUM_SOURCES, MAGNUM_INCLUDES, MAGNUM_CFLAGS, and MAGNUM_MODULE_DIR)
include $(FRAMEWORK_DIR)/build/magnum.inc

# Include object file list, include path, and C flags for other magnum modules required
# by this application (adds to MAGNUM_SOURCES, MAGNUM_INCLUDES, and MAGNUM_CFLAGS).
# Here is where you also define module specific makefile defines
BVDC_MACROVISION ?= y
#include $(MAGNUM_MODULE_DIR)/vdc.inc

# At this point, user can add additional files, include paths, and C flags to
# MAGNUM_SOURCES, MAGNUM_INCLUDES, and MAGNUM_CFLAGS (optional).

# Include other common modules (adds to COMMON_SOURCES, COMMON_INCLUDES, and
# COMMON_CFLAGS)
#include $(FRAMEWORK_DIR)/build/modules/common/fpga.inc

# Specifies how magnum files and application files are built.
include $(FRAMEWORK_DIR)/build/$(TARGET_OS)/build.inc
