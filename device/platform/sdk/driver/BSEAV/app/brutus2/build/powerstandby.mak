############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: powerstandby.mak $
# $brcm_Revision: 1 $
# $brcm_Date: 1/12/12 7:01p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/app/brutus2/build/powerstandby.mak $
# 
# 1   1/12/12 7:01p tokushig
# SW7405-5581: Brutus2 initial check in
# 
# 11   11/9/10 4:39p gskerl
# SW7125-637: Improved algorithm for determining kernel version
# 
# 10   3/18/10 2:50p erickson
# SW7420-532: linux 2.6.31 uses pmlib-2631.c, add namespace to #define
#
# 9   3/11/10 5:58p mward
# SW7420-70: CHIP isn't available, use PLATFORM.
#
# 8   12/21/09 2:59p erickson
# SW7420-70: allow some chips to enable POWERSTANDBY_SUPPORT without an
# external power driver
#
# 7   11/13/09 5:06p erickson
# SW7325-250: use pmlib for linux standby, use nexus for magnum-
# controlled standby
#
# 6   9/18/09 11:56a erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 5   9/1/09 5:03p katrep
# SW7405-2937:for kernel mode builds magnum modules are getting compiled
# twice
#
# 4   7/15/09 11:22a erickson
# PR56373: add linux kernel mode passive standby support
#
# 3   7/2/09 5:03p erickson
# PR56373: allow DEBUG mode
#
# 2   7/1/09 12:40p erickson
# PR56373: add echo
#
# 1   6/26/09 2:08p erickson
# PR56373: rework brutus passive standby to not exit the app
#
############################################################

# POWERSTANDBY_SUPPORT feature for Brutus
#
# Brutus code is found in brutus_power.c. main.cpp will call functions in brutus_power.c.
# brutus_power.c uses pmlib and nexus_platform_standby.
# 1) nexus_platform_standby - powers down magnum-controlled cores (audio, video, display, etc.)
# 2) pmlib - powers down OS-controlled cores (USB, SATA, ETH, etc.) and puts mips into deep sleep
#
# This file is to be included by Makefile.brutus
#
#
# Work out Kernel Version from Kernel Source's Top Level Makefile
#
ifeq ($(SYSTEM),linux)
K_VERSION       := $(shell grep -m1 "^VERSION" $(LINUX)/Makefile | cut -d\  -f3)
K_PATCHLEVEL    := $(shell grep -m1 "^PATCHLEVEL" $(LINUX)/Makefile | cut -d\  -f3)
K_SUBLEVEL      := $(shell grep -m1 "^SUBLEVEL" $(LINUX)/Makefile | cut -d\  -f3)
K_EXTRAVERSION  := $(shell grep -m1 "^EXTRAVERSION" $(LINUX)/Makefile | cut -d-  -f2-)
KERNEL_VERSION  := $(K_VERSION).$(K_PATCHLEVEL).$(K_SUBLEVEL)-$(K_EXTRAVERSION)

check-version = $(shell expr \( $(K_VERSION) \* 65536 + $(K_PATCHLEVEL) \* 256 + $(K_SUBLEVEL) \) \>= \( $(1) \* 65536 + $(2) \* 256 + $(3) \))
endif


POWERSTANDBY_CFLAGS = -DPOWERSTANDBY_SUPPORT
APP_OBJECTS += \
	brutus_power.o

ifeq ($(SYSTEM),linux)
ifeq ($(call check-version,2,6,30),1)   # if kernel >= 2.6.30
# 2.6.31
APP_OBJECTS += pmlib-2631.o
else                                    # if kernel < 2.6.30
# 2.6.18
APP_OBJECTS += pmlib.o
# compile flag
POWERSTANDBY_CFLAGS += -DPOWERSTANDBY_USE_EXTERNAL_DRIVER
# build system
EXTERNAL_POWER_DRIVER=y
endif
endif

# pmlib header files and implementation
POWERSTANDBY_CFLAGS += -I$(BSEAV)/lib/power_standby -I$(BSEAV)/linux/driver/power
vpath %c $(BSEAV)/lib/power_standby

# nexus_platform_standby header file and implementation
POWERSTANDBY_CFLAGS += -I$(NEXUS_TOP)/platform/$(NEXUS_PLATFORM)/include

