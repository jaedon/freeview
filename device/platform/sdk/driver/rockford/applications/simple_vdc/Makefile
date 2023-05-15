#############################################################################
#
#       Copyright (c) 2005-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/21 $
# $brcm_Date: 4/5/12 7:01p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/applications/simple_vdc/Makefile $
# 
# Hydra_Software_Devel/21   4/5/12 7:01p darnstein
# SW7405-5639: Support DCS analog copy protection.
# 
# Hydra_Software_Devel/20   3/15/11 3:57p darnstein
# SW7425-164: Adapt to changes in the "plat" script.
# - Change DEBUG to B_REFSW_DEBUG.
# - Change ARCH to B_REFSW_ARCH.
# - Change OS to B_REFSW_OS.
# - Change mips-linux-uclibc to mips-linux.
# - Change mipsel-linux-uclibc to mipsel-linux.
# 
# Hydra_Software_Devel/19   9/2/10 4:47p hongtaoz
# SW7425-11: added emulation support for simple_vdc;
# 
# Hydra_Software_Devel/18   8/4/09 9:33a pntruong
# PR38929: fixed debug/release build.
#
# Hydra_Software_Devel/17   2/20/08 3:05p rpan
# PR38929: Got simple_vdc to run on 3548 IKOS.
#
# Hydra_Software_Devel/16   2/7/08 5:57p jessem
# PR 39142: Removed -DIKOS compile flag and moved to framework.inc.
#
# Hydra_Software_Devel/15   2/7/08 5:02p jessem
# PR 39142: Added IKOS support.
#
# Hydra_Software_Devel/14   2/15/07 7:55p pntruong
# PR15284, PR27951: Graphics shimmering on HD path when video is scaled
# down (PIG).  HW fixed.  Removed software work-around to avoid bvb
# error interrupts from window surface.
#
# Hydra_Software_Devel/13   2/7/07 11:47p pntruong
# PR 27117: Updating to use module specific .inc files.
#
# Hydra_Software_Devel/12   8/18/06 10:34p pntruong
# PR22577: Initial bringup of VDC.
#
# Hydra_Software_Devel/11   8/8/06 11:54a pntruong
# PR22577: Initial bringup of VDC.  Removed the gfx test, as it's already
# available with mvd_vdc_test/interactive.
#
# Hydra_Software_Devel/10   8/2/06 11:55a syang
# PR 22212: separete LINUX and VXWORKS
#
# Hydra_Software_Devel/9   4/4/06 5:48p jasonh
# PR 19565: Removed ARC and IMG include since that is not part of the
# application framework for the 97398.
#
# Hydra_Software_Devel/8   2/24/06 2:33p maivu
# PR 19565: Added support for 97398 built
#
# Hydra_Software_Devel/7   2/16/06 5:46p jasonh
# PR 19565: Cleaned up makefile
#
# Hydra_Software_Devel/6   2/14/06 1:38p jasonh
# PR 19565: Updated with new 7411 version and BCHP_VER comment.
#
# Hydra_Software_Devel/5   9/22/05 10:47a jasonh
# PR 16967: Changed sources from .o to .c files
#
# Hydra_Software_Devel/4   9/8/05 4:23p syang
# PR 16943: add sophistical gfx test
#
# Hydra_Software_Devel/3   9/2/05 11:16a jasonh
# PR 16967: Added debugging option
#
# Hydra_Software_Devel/2   8/31/05 5:27p jasonh
# PR 16943: Updated to use MAGNUM_MODULE_DIR
#
# Hydra_Software_Devel/1   7/27/05 5:03p jasonh
# PR 16397: Initial version
#
#############################################################################

# Requires the use of the 'plat' script which defines the following:
#   PLATFORM (97038, 97398, ...)
#   BCHP_VER (B0, B1, C0, ...)
#   BCHP_7411_VER (C0, D0, ...) plus versions for other secondary chipsets
#   DEBUG
#   LINUX
#   ARCH
#   TOOLCHAIN

# Target O/S (can be one of the following: linuxuser vxworks)
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
MAGNUM_DIR = ../../../magnum
ROCKFORD_DIR = ../../../rockford
FRAMEWORK_DIR = $(ROCKFORD_DIR)/appframework

# Name of application
ifeq ($(SYSTEM),vxworks)
APP = simple_vdc.out
else
APP = simple_vdc
endif

# Application object file list, include path, C flags, and link
# flags
APP_SOURCES= \
	./simple_vdc.c

# Initial application settings
APP_INCLUDES =
APP_CFLAGS   =
APP_LDFLAGS  =

# Must define ENDIAN to be either big or little
ENDIAN = little

# Want to see details (y/n)?
VERBOSE = n

# Debug (y/n)?
B_REFSW_DEBUG ?= y
ifeq ($(B_REFSW_DEBUG),y)
APP_CFLAGS += -DBDBG_DEBUG_BUILD
endif

ifeq ($(TARGET_OS),vxworks)
CFLAGS += -DVXWORKS
else
CFLAGS += -DLINUX
	ifeq ($(TARGET_OS),linuxclient)
	CFLAGS += -DIKOS_EMULATION
	endif
endif

BVDC_BVN_DEBUG ?= y
ifeq ($(BVDC_BVN_DEBUG), y)
	CFLAGS += -DBVDC_SUPPORT_BVN_DEBUG=1
endif

# Can have Macrovision or DCS analog copy protection. But not both.
BVDC_MACROVISION ?= y
BVDC_DCS ?= n
ifeq ($(BVDC_MACROVISION), y)
        CFLAGS += -DBVDC_MACROVISION=1
        CFLAGS += -DMACROVISION_SUPPORT=1
else
        CFLAGS += -DBVDC_MACROVISION=0
endif

ifeq ($(BVDC_DCS), y)
        ifeq ($(BVDC_MACROVISION),y)
                error Macrovision plus DCS not allowed
        endif
        CFLAGS += -DDCS_SUPPORT=1
endif

# Include object file list, include path, and C flags for common utilities and base modules
# (sets MAGNUM_SOURCES, MAGNUM_INCLUDES, MAGNUM_CFLAGS, and MAGNUM_MODULE_DIR)
include $(FRAMEWORK_DIR)/build/magnum.inc

# Include object file list, include path, and C flags for other magnum modules required
# by this application (adds to MAGNUM_SOURCES, MAGNUM_INCLUDES, and MAGNUM_CFLAGS).
# Here is where you also define module specific makefile defines
include $(MAGNUM_DIR)/portinginterface/vdc/bvdc.inc

# At this point, user can add additional files, include paths, and C flags to
# MAGNUM_SOURCES, MAGNUM_INCLUDES, and MAGNUM_CFLAGS (optional).

# Include other common modules (adds to COMMON_SOURCES, COMMON_INCLUDES, and
# COMMON_CFLAGS)

# Specifies how magnum files and application files are built.
include $(FRAMEWORK_DIR)/build/$(TARGET_OS)/build.inc

# End of file
