############################################################
#     (c)2003-2012 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile: os_flags.inc $
# $brcm_Revision: 34 $
# $brcm_Date: 10/22/12 3:12p $
#
# Module Description:
#   This file defines the OS-specific flags for building in linuxkernel mode.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/linuxkernel/os_flags.inc $
# 
# 34   10/22/12 3:12p erickson
# SW7425-2130: suppress warnings for DEBUG=n
# 
# 33   9/14/12 10:46a mward
# SW7435-235: /BSEAV/linux/driver/build/97401/ eliminated.  Use 97400 as
#  representative pre-2.6.31 build.
# 
# 32   8/20/12 10:00a erickson
# SW7435-235: don't use symlink
# 
# 31   3/20/12 8:45a erickson
# SW7435-37: add ^ to grep, consolidate grep/test logic
# 
# 30   2/22/12 4:55p erickson
# SW7435-37: extend -m 1 to similar greps
# 
# 29   2/21/12 6:02p mward
# SW7435-37:  grep -m 1 to reject other lines matching VERSION.
# 
# 28   2/21/12 8:50a erickson
# SW7346-703: add linux 3.3 support
# 
# 27   1/27/12 5:14p mward
# SW7125-1221:  Add -Wno-pointer-sign, which is used by Kbuild.
# 
# 26   7/18/11 2:32p jhaberf
# SWDTV-7208: check in on behalf of mstefan. Merge of DTV kernel mode
#  changes with the main
# 
# 25   5/18/11 12:55p erickson
# SW7346-200: add 2.6.37 and beyond test for linux configuration
# 
# 24   4/19/11 5:35p erickson
# SW7425-365: add linux 2.6.37 support
# 
# 23   3/18/11 1:06p erickson
# SW7346-122: switch default linuxkernel build to -O2, allow
#  customization
#
# 22   1/20/11 3:09p erickson
# SW7346-15: consolidate where -O is set in linux kernel mode
#
# 21   11/11/10 3:05p vsilyaev
# SW7405-4995: Use -mmemcpy option to force use of memcpy when assigning
#  compound types
#
# 20   11/10/10 6:59p ttrammel
# SW7420-1228: Initial Support for Android.
#
# 19   7/27/10 2:20p erickson
# SW7125-523: extend B_REFSW_ARCH for other variants of big-endian uclibc
#
# 18   4/16/10 10:38a katrep
# SW7405-3154:fixed kernelmode build for 2618 kernel without precompiled
#  header files
#
# 17   10/19/09 3:26p erickson
# SW7405-3154: add 2.6.31 support
#
# 16   10/14/09 5:52p erickson
# SW7400-2511: default LINUX_INC
#
# 15   10/14/09 5:28p erickson
# SW7400-2511: remove unnecessary include which was causing naming
#  conflicts
#
# 14   10/14/09 10:50a erickson
# SW7400-2511: add B_REFSW_ namespace
#
# 13   10/12/09 5:16p erickson
# SW7420-228: fix 2.6.18 kernel mode
#
# 12   10/9/09 3:20p erickson
# SW7420-228: linux 2.6.31 kernel mode support
#
# 11   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 10   9/4/09 12:11p erickson
# SW7400-2502: support big endian
#
# 9   7/30/09 11:34a vsilyaev
# PR 57223: Removed -fno-strict-aliasing
#
# 8   3/26/09 2:20p vsilyaev
# PR 53611: Use -MMD files to create dependency file (this would exclude
#  #include <header.h> files)
#
# 7   3/26/09 10:57a vsilyaev
# PR 53611: Use simply expanded variables everywhere where $(shell ...)
#  is used. It prevents evaluation of $(shell ) every where variable is
#  used
#
# 6   10/20/08 12:00p jrubio
# PR47919: fix kernelmode support for 73x5
#
# 5   8/18/08 10:28a katrep
# PR45674: Compiler warnings in DEBUG=n builds
#
# 4   8/1/08 12:33p vsilyaev
# PR 45320: Added rules to build linux include path
#
# 3   7/1/08 3:35p vsilyaev
# PR 44452: Added setting of RSRVD option
#
# 2   3/4/08 4:20p jrubio
# PR40169: adding 7325
#
# 1   1/18/08 12:47p vsilyaev
# PR 38682: OS dependent compiler flags
#
#
############################################################

LINUX ?= /opt/brcm/linux

noop: all

# parse the Linux Makefile
LINUX_VERSION = $(shell grep -m 1 '^VERSION = ' ${LINUX}/Makefile | awk '{print $$3}')
LINUX_PATCHLEVEL = $(shell grep -m 1 '^PATCHLEVEL = ' ${LINUX}/Makefile | awk '{print $$3}')
LINUX_SUBLEVEL = $(shell grep -m 1 '^SUBLEVEL = ' ${LINUX}/Makefile | awk '{print $$3}')

LINUX_VER_GE_3 =  $(shell test $(LINUX_VERSION) -eq 3 && echo y)
ifeq ($(LINUX_VER_GE_3),y)
	LINUX_VER_GE_2_6 = y
	LINUX_VER_GE_2_6_31 = y
	LINUX_VER_GE_2_6_37 = y
else
LINUX_VER_GE_2_6 ?= $(shell test $(LINUX_PATCHLEVEL) -eq 6 && echo 'y')
ifeq ($(LINUX_VER_GE_2_6),y)
LINUX_VER_GE_2_6_31 =  $(shell test $(LINUX_SUBLEVEL) -ge 31 && echo y)
LINUX_VER_GE_2_6_37 =  $(shell test $(LINUX_SUBLEVEL) -ge 37 && echo y)
LINUX_VER_EQ_2_6_12 := $(shell test $(LINUX_SUBLEVEL) -eq 12 && echo 'y')
endif
endif

# for linux 2.6.25 and earlier, we have logic to autoconfigure the kernel if LINUX_INC is not defined.
# manual configuration required for 2.6.31 and beyond.
ifeq ($(LINUX_INC),)
ifneq ($(LINUX_VER_GE_2_6_31),y)
include $(NEXUS_TOP)/../BSEAV/linux/driver/build/97400/Linux.make
endif
endif
LINUX_INC ?= $(LINUX)/include

ifeq ($(LINUX_VER_GE_2_6_31),y)
ifeq ($(LINUX_VER_GE_2_6_37),y)
LINUX_CONFIGURED = $(shell test -f $(LINUX_INC)/generated/autoconf.h && echo "y")
else
LINUX_CONFIGURED = $(shell test -f $(LINUX_INC)/linux/autoconf.h && echo "y")
endif
ifneq ($(LINUX_CONFIGURED),y)
$(error $(LINUX) must be configured before using it to build a driver.)
endif
endif

ifeq ($(LINUX_VER_GE_2_6),y)
 NEXUS_CFLAGS += -fno-common
 ifeq ($(LINUX_VER_EQ_2_6_12),y)
  ifeq ($(shell grep -e '^EXTRAVERSION =-[1-3]' $(LINUX)/Makefile > /dev/null && echo 'y'),)
  # Version 2.6.12-4.0 or later
  NEXUS_CFLAGS += -DHAVE_RSRVD=1
  endif
 else
  # Non 2.6.12 version, assume 2.6.18 or later
  NEXUS_CFLAGS += -DHAVE_RSRVD=1
 endif
endif

ifeq ($(B_REFSW_OPTIMIZER_LEVEL),)
B_REFSW_OPTIMIZER_LEVEL = 2
endif

NEXUS_CFLAGS += -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -Wa,--trap -DMODULE -mlong-calls
NEXUS_CFLAGS += -msoft-float -pipe
NEXUS_CFLAGS += -DLINUX -D__KERNEL__

ifeq ($(LINUX_VER_GE_2_6_31),y)
NEXUS_CFLAGS += \
    -I$(LINUX_INC) \
    -I$(LINUX)/arch/mips/include \
    -I$(LINUX)/arch/mips/include/asm/mach-brcmstb \
    -I$(LINUX)/arch/mips/include/asm/mach-generic
else
ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
B_REFSW_TOOLCHAIN_ARCH=mips-linux-uclibc
else
B_REFSW_TOOLCHAIN_ARCH=mipsel-linux-uclibc
endif
STD_INC ?= $(B_REFSW_TOOLCHAIN_DIR)/$(B_REFSW_TOOLCHAIN_ARCH)/include
GCC_BASE ?= $(shell (test -d $(B_REFSW_TOOLCHAIN_DIR)/lib/gcc-lib && echo $(B_REFSW_TOOLCHAIN_DIR)/lib/gcc-lib/$(B_REFSW_TOOLCHAIN_ARCH)) || echo $(B_REFSW_TOOLCHAIN_DIR)/lib/gcc/$(B_REFSW_TOOLCHAIN_ARCH))
GCC_VER ?= $(shell (ls $(GCC_BASE)))
GCC_INC ?= $(GCC_BASE)/$(GCC_VER)/include
NEXUS_CFLAGS += -nostdinc -I$(LINUX_INC) -I$(STD_INC) -I$(GCC_INC)
NEXUS_CFLAGS += \
    -I$(LINUX_INC)/asm-mips/mach-brcmstb \
    -I$(LINUX_INC)/asm-mips/mach-mips \
    -I$(LINUX_INC)/asm/mach-generic/
endif


NEXUS_CFLAGS += -DBSTD_CPU_ENDIAN=$(NEXUS_ENDIAN)

ifeq ($(LINUX_VER_GE_2_6),y)
ifeq ($(NEXUS_PLATFORM),97325)
NEXUS_CFLAGS += -ffreestanding -finline-limit=100000  -mabi=32 -march=mips32r2 -Wa,-32 -Wa,-march=mips32r2 -Wa,-mips32r2 -fno-strict-aliasing
else
NEXUS_CFLAGS += -ffreestanding -finline-limit=100000  -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32
endif
else
NEXUS_CFLAGS += -mips2
endif


# Use memcpy to assign structures
NEXUS_CFLAGS += -mmemcpy

# Release builds generate unused values. We suppress this warning.
ifeq ($(B_REFSW_DEBUG),y)
NEXUS_CFLAGS += -Wall
NEXUS_CFLAGS += -Wno-unused-value
endif

# Kbuild uses this flag. We suppress this warning, which may occur inside Linux kernel code/headers.
NEXUS_CFLAGS += -Wno-pointer-sign

ifeq ($(B_REFSW_ANDROID),y)
NEXUS_CFLAGS += -EL -DB_REFSW_ANDROID
NEXUS_LDFLAGS += -EL
endif


NEXUS_LDFLAGS += -G 0 --warn-common

# Flag to generate dependencies
CDEP_FLAG = -MMD



