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
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 9/17/12 2:21p $
#
# File Description:
# 	Makefile for compiling raaga_test unit test application for testing DSP basemodule
#
# $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/Makefile $
# 
# Hydra_Software_Devel/5   9/17/12 2:21p gprasad
# SW7425-3952: Fixing build errors with raaga test for emulation build
# 
# Hydra_Software_Devel/4   8/14/12 3:07p rshijith
# FWRAA-478: Adding raaga_test_video build support to make file
# 
# Hydra_Software_Devel/3   8/5/11 4:25p gskerl
# SW7425-1059: Added -D_GNU_SOURCE to CFLAGS to allow pthread.h to
# provide a prototype for pthread_condattr_setclock()
# 
# Hydra_Software_Devel/2   1/14/11 3:32p gdata
# SW7422-191:[7422] Adding the raaga test application
# 
# Hydra_Software_Devel/1   12/15/10 6:51p jgarrett
# SW7422-146: Adding basic dsp unit test
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
ifeq (${SYSTEM},vxworks)
	TARGET_OS = vxworks
else
	ifeq (${SYSTEM},linuxemu)
		TARGET_OS = linuxemu
	else
		TARGET_OS = linuxuser
	endif
endif

# Directories relative to location of makefile
MAGNUM_DIR = ../../../../../magnum
ROCKFORD_DIR = ../../../../../rockford
FRAMEWORK_DIR = $(ROCKFORD_DIR)/appframework

# Name of application
ifeq (${SYSTEM},vxworks)
APP = raaga_test.out
else
ifeq ($(APPS),video)
APP = raaga_test_video
else
APP = raaga_test
endif
endif

ifeq ($(EMU),IKOS)
CFLAGS += -DIKOS_EMULATION
CFLAGS += -DIKOS
else
ifeq ($(EMU),PALLADIUM)
CFLAGS += -DPALLADIUM_EMULATION
endif
endif

ifeq (${SYSTEM},vxworks)
	CFLAGS += -DVXWORKS
else
	CFLAGS += -DLINUX
ifneq (${SYSTEM},linuxemu)
	CFLAGS += -D_GNU_SOURCE  #  This allows pthread.h to prototype pthread_condattr_setclock()
endif
endif

# Application object file list, include path, and C flags
ifeq ($(APPS), video)
APP_SOURCES = \
  raaga_test_video.c \
  raaga_test_lib_video.c \
  raaga_test_utils_video.c \
  fmm_config_video.c \
  raaga_test_manual_video.c \
  xpt_pb_video.c \
  data_feeder_video.c
else
APP_SOURCES = \
  raaga_test.c \
  raaga_test_lib.c \
  raaga_test_utils.c \
  fmm_config.c \
  raaga_test_manual.c
endif
ifneq ($(SYSTEM),linuxemu)
APP_SOURCES += xpt_pb.c
endif

$(INFO $(APP_SOURCES))

APP_INCLUDES = ./
APP_CFLAGS =

# Show build commands (y/n)?
VERBOSE=n

# Debug (y/n)?
ifeq ($(origin DEBUG),undefined)
DEBUG = y
endif

# Must define ENDIAN to be either big or little
ifeq ($(SYSTEM),vxworks)
ENDIAN = big
else
ENDIAN = little
endif

# Include object file list, include path, and C flags for common utilities and base modules 
# (sets MAGNUM_SOURCES, MAGNUM_INCLUDES, MAGNUM_CFLAGS, and MAGNUM_MODULE_DIR)
include $(FRAMEWORK_DIR)/build/magnum.inc

# Include object file list, include path, and C flags for other magnum modules required
# by this application (adds to MAGNUM_SOURCES, MAGNUM_INCLUDES, and MAGNUM_CFLAGS).
# Here is where you also define module specific makefile defines
include $(MAGNUM_DIR)/basemodules/dsp/bdsp.inc
ifneq (${SYSTEM},linuxemu)
include $(MAGNUM_DIR)/portinginterface/xpt/bxpt.inc
endif

# At this point, user can add additional files, include paths, and C flags to 
# MAGNUM_OBJ, MAGNUM_INC, and MAGNUM_CFLAGS (optional).

# additional flags for FirePath BM version
ifeq (${SYSTEM},linuxemu)
APP_INCLUDES += $(shell /projects/firepath/tools/bin/fp-pkg-config --pkg fp_binutils --print-include)
APP_LDFLAGS += $(shell /projects/firepath/tools/bin/fp-pkg-config --pkg fp_binutils --print-lib --32-bit)/system-control-proxy-c.o
MAGNUM_CFLAGS += -DFIREPATH_BM
endif

# Include other common modules (adds to COMMON_SOURCES, COMMON_INCLUDES, and
# COMMON_CFLAGS)

# Specifies how magnum files and application files are built.
include $(FRAMEWORK_DIR)/build/$(TARGET_OS)/build.inc
