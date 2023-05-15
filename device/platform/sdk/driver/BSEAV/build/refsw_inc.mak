###########################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: refsw_inc.mak $
# $brcm_Revision: 86 $
# $brcm_Date: 10/10/12 2:51p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/build/refsw_inc.mak $
# 
# 86   10/10/12 2:51p chengs
# SW7584-42: Add Brutus support for 7584.
# 
# 85   9/10/12 6:02p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/10/12 4:05p mphillip
# SW7425-3739: Change from hard-coded include paths to module.inc
# variable values
# 
# 84   9/6/12 10:27a vsilyaev
# SW7425-3760: Symbolic link removal
# 
# SW7425-3760/1   9/5/12 1:34p vsilyaev
# SW7425-3760: Added top level include path
# 
# 83   7/19/12 7:14p jasonh
# SW7435-263: Updating CHP directory structure.
# 
# 82   12/21/11 3:55p mward
# SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
# 
# 81   12/20/11 3:15p vsilyaev
# SW7425-2039: Use greater or equal tests for the kernel versions
# 
# 80   10/19/11 4:34p mward
# SW7125-1119: Change DEBUG to B_REFSW_DEBUG.
# 
# 79   10/19/11 2:47p mward
# SW7125-1119:  Change DEBUG to B_REFSW_DEBUG.
# 
# 78   9/23/11 3:37p katrep
# SW7429-1:7429 support
# 
# 77   4/11/11 5:29p jhaberf
# SWDTV-6551: Added support for 2.6.37 kernel for DTV chips
# 
# 76   3/24/11 4:20p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 75   12/22/10 4:35p katrep
# SW7231-25:add initial support for 7230
# 
# 74   12/6/10 4:27p katrep
# SW7231-4:initial support
# 
# 73   11/22/10 5:39p xhuang
# SW7358-20: Add 97358 support
# 
# 72   11/10/10 5:37p spothana
# SW7420-1078: Add BCM97409 platform support.
# 
# 71   8/30/10 1:52p hongtaoz
# SW7425-8: add 7425 support;
# 
# 70   6/8/10 4:06p nickh
# SW7422-7: Add 97422 support
# 
# 69   2/1/10 3:04p gmohile
# SW7408-1 : Add 7408 support
# 
# 68   2/1/10 3:02p gmohile
# SW7408-1 : Add 7408 support
# 
# 67   1/20/10 4:55p garetht
# SW7420-553: Add platform for 93380vms
# 
# 66   1/19/10 3:18p nickh
# SW7420-541: Rename 97420dvr2 to 97420
# 
# 65   12/4/09 9:46p randyjew
# SW7468-6: Add 7208 support
# 
# 64   10/30/09 5:31p nickh
# SW7420-331: Rename 97410c to 97410
# 
# 63   10/27/09 3:19p nitinb
# SW7550-46: Add 7550 support
# 
# 62   10/2/09 12:49p maivu
# SW3548-2511: Add support for 2631 kernel
# 
# 60   10/2/09 11:02a maivu
# SW3548-2511: Add 3548/3556 support
# 
# 60   10/2/09 11:01a maivu
# SW3548-2511: Add 3548/3556 support
# 
# 59   9/28/09 5:02p lwhite
# SW7468-6: Add 7468 support
# 
# 58   9/22/09 5:05p erickson
# SW3548-2493: add BDBG_DEFINES to standard CFLAGS
#
# 57   9/15/09 6:13p nickh
# SW7420-331: Add 7410 support
#
# 56   8/20/09 6:45p mward
# PR55545: Adding 7125 platforms.
#
# 55   8/10/09 8:56a jrubio
# PR55232:add 7342/7340
#
# 54   7/30/09 11:51a vsilyaev
# PR 57223: Removed -fno-strict-aliasing
#
# 53   2/4/09 10:30a jrubio
# PR51629: add 7336 support
#
# 52   1/30/09 9:21p vsilyaev
# PR 51741: Use MAGNUM_SYSTEM_DEFINES option to set system wide options
#
# 51   12/14/08 8:11p nickh
# PR50205: Add support for 97420dvr2
#
# 50   6/20/08 11:47a anilmm
# PR43453: Add support for 97459
#
# 49   4/19/08 3:40p mward
# PR39963: Adding -fno-strict-aliasing to eliminate warnings with GCC
# 4.2.0. (2.6.18-4.1 NPTL) (release build).
#
# 48   2/26/08 3:14p mward
# PR39963: Adding -fno-strict-aliasing to eliminate warnings with GCC
# 4.2.0. (2.6.18-4.1 NPTL)
#
# 47   2/6/08 1:59p jrubio
# PR39363: fix compile warnings for 7325 -fno-strict-aliasing
#
# 46   1/21/08 12:34p rjlewis
# PR38655: add in 7325 vxworks support.
#
# 45   11/21/07 3:43p katrep
# PR37217: Added 97335 platform support.
#
# 44   11/7/07 3:43p jrubio
# PR35015: add 7325 CFLAGS
#
# 43   11/6/07 11:43a jrubio
# PR35015: adding 7325 new instructions
#
# 42   9/25/07 12:42p rjlewis
# PR34613: fix up the vxworks search paths for library modules.
#
# 41   8/20/07 5:05p jgarrett
# PR34172: PR 34172: Added default linux directory
#
# 40   8/15/07 2:31p vsilyaev
# PR 34089: Use C99 standard to compile linux user level code
#
# 39   8/13/07 3:14p jrubio
# PR33830: add 2618 flag
#
# PR34172/1   8/20/07 5:00p jgarrett
# PR 34172: Added default linux directory
#
# 38   7/29/07 2:51p jgarrett
# PR 33588: Fixing check for kernel version
#
# 37   7/25/07 11:25a vsilyaev
# PR 32813: Added Windows MINGW support
#
# 36   7/5/07 3:22p ahulse
# PR23504: fix b_get_kernel_memory for 2.6 kernel
#
# 35   6/1/07 5:42p rjlewis
# PR31765: adding VxWorks support for 97403.
#
# 34   2/14/07 6:23p katrep
# PR27643: Add support for 97405.
#
# 33   1/29/07 12:33p erickson
# PR25902: linux kernel cflags are now explicit, for future isolation
#
# 32   1/17/07 5:48p anilmm
# PR27129: add 97458A0 platform support
#
# 31   1/15/07 10:44a vsilyaev
# PR 26928: Fixed link flags to reserved spaced for common symbols
#
# 30   1/8/07 2:19p gmohile
# PR 25109: add 7403 support
#
# 29   12/6/06 1:19p rjlewis
# PR19044: need a compile time define for VxWorks 6 builds.
#
# 28   11/28/06 3:58p jgarrett
# PR 24898: Adding include/asm-mips/mach-brcmstb for proper SMP support
#
# 27   11/17/06 11:07a jgarrett
# PR 20139: Fixing kernel build flags for 2.6 kernel
#
# 26   9/29/06 4:34p mward
# PR24604: create PLATFORM for 97118RNG.
#
# 25   8/11/06 3:13p erickson
# PR23574: support bigendian linux
#
# 23   7/20/06 8:53a jgarrett
# PR 19909: Removing mips64 from release builds, this was a merge error.
#
# 22   7/19/06 12:33p jgarrett
# PR 19909: Removing ?= where vxworks functionality is required.  Vxworks
# make does not support that for 5.5.
#
# 21   7/14/06 11:31a jgarrett
# PR 19909: Merging to latest baseline
#
# PR19909/3   7/13/06 7:11p jgarrett
# PR 19909: 97398 should use 32-bit ISA
#
# PR19909/2   7/6/06 5:22p jgarrett
# PR 19909: Making optimizer level override possible.
#
# PR19909/1   7/6/06 4:57p jgarrett
# PR 19909: Increasing optimization and setting -mips32/-mips64
# appropriately
#
# 20   6/30/06 2:38p erickson
# PR22386: use -O for refsw for now.
#
# 19   6/30/06 9:40a erickson
# PR21941: support ARCH=i386-linux (no mips compiler flags)
#
# 18   6/26/06 11:38a mphillip
# PR21883: Optimization flags set for newer platforms
#
# 17   5/26/06 9:09a mward
# PR21671: Add support for 7118 chip 97118 board.
#
# 16   4/20/06 3:21p erickson
# PR19738: 97435 merge
#
# PROD_97435C_REFSW_Devel/1   4/18/06 7:23p gqli
# PR19738: add BCM97435C support
#
# 15   2/15/06 3:18p erickson
# PR17108: fix kernelmode make clean
#
# 14   2/13/06 5:51p jgarrett
# PR 19250: Updating build rules for 7400
#
# 13   1/24/06 1:05p rjlewis
# PR19044: typo on vxworks 6 build option.
#
# 12   1/18/06 11:57a rjlewis
# PR19044: new definitions for vxworks 6.
#
# 11   12/22/05 3:09p rjlewis
# PR17282: SYSTEM_vxworks already defined earlier (and set to a value).
#
# 10   12/9/05 4:19p rjlewis
# PR18214: add suppor for TMR.
#
# 9   12/9/05 3:24p vsilyaev
# PR 18019: Added support for 2.6 kernel
#
# 8   11/23/05 3:04p rjlewis
# PR17108: Can't use 64 bits with 7401.
#
# 7   9/15/05 11:19a erickson
# PR17048: basemodules/reg is required too
#
# 6   9/15/05 10:55a erickson
# PR17148: added basemodules/chp which is now required by bstd.h
#
# 5   9/14/05 2:15p erickson
# PR17148: added BCHP_VER define and removed unused defines
#
# 4   8/15/05 7:14p jrubio
# PR16609: Added vxworks specific defines
#
# 3   8/9/05 2:04p erickson
# PR15139: require BSEAV only for B_REFSW_MAGNUM_INCLUDE_DIRS
#
# 2   8/8/05 5:22p erickson
# PR15139: vxworks build fix
#
# 1   8/4/05 4:39p erickson
# PR15139: moved system-specific general purpose build configuration to
# BSEAV/build
#
###########################################################

###############################################
#
# Inputs:
#    BSEAV - this is required only for the optional MAGNUM defines
#    PLATFORM - this is required
#    SYSTEM - linux (default), linuxkernel, vxworks
#    B_REFSW_DEBUG - y (default) or n
# Outputs:
#    B_REFSW_CFLAGS - compiler flags
#    B_REFSW_LDFLAGS - linker flags
#    B_REFSW_MAGNUM_CFLAGS
#    DEBUG_SUFFIX - helpful for directory names
#
# All internal variables should be prefixed with B_REFSW_ to avoid namespace pollution.

# We base a lot of things on the particular platform we compiling for.
# This variable MUST be defined in order to continue.  Make sure it is!
ifeq ($(PLATFORM),)
$(error PLATFORM environment variable is required)
endif

###############################################
#
# Set defaults
#
ifeq ($(SYSTEM),)
SYSTEM = linux
endif

# Set B_REFSW_DEBUG flag for debug interface, default y.
# If you set B_REFSW_DEBUG=n, please realize that you won't even get error messages.
ifeq ($(B_REFSW_DEBUG),)
B_REFSW_DEBUG = y
endif

# Set a default optimization level.
ifeq ($(OPTIMIZER_LEVEL),)
OPTIMIZER_LEVEL = 2
endif

###############################################
#
# Magnum DBG defines
#

ifeq (${B_REFSW_DEBUG},y)
#
# debug
#
B_REFSW_CFLAGS += -DBDBG_DEBUG_BUILD=1 -DBRCM_DBG
DEBUG_SUFFIX = debug
# Add future platforms here (leave 97038 alone -- its in maintainence mode).
ifeq ($(findstring $(PLATFORM),97398 97401 97403 97400 97405 97335 97342 97340 97118 97118RNG 97455 97458 97456 97459 97420 97410 97409 93380vms 97125 97025 97119 97019 97116 97468 97208 93549 93556 97550 97408 97422 97425 97435 97358 97552 97231 97230 97429 97428 97241 97584), $(PLATFORM))
 B_REFSW_CFLAGS += -O$(OPTIMIZER_LEVEL)
 ifneq ($(findstring $(B_REFSW_ARCH), i386-mingw i386-linux mips-vxworks), $(B_REFSW_ARCH))
 B_REFSW_CFLAGS += -mips32
 endif
endif
ifeq ($(findstring $(PLATFORM),97325), $(PLATFORM))
 B_REFSW_CFLAGS += -O$(OPTIMIZER_LEVEL)
 ifneq ($(findstring $(B_REFSW_ARCH), i386-mingw i386-linux mips-vxworks), $(B_REFSW_ARCH))
 B_REFSW_CFLAGS += -mips32r2
 endif
endif
ifneq ($(SYSTEM),linuxkernel)
B_REFSW_CFLAGS += -g
B_REFSW_LDFLAGS += -g
endif

else
#
# release
#
DEBUG_SUFFIX = release
B_REFSW_CFLAGS += -DNDEBUG
# Add future platforms here (leave 97038 alone -- its in maintainence mode).
ifeq ($(findstring $(PLATFORM),97398 97401 97403 97400 97405 97335 97342 97340 97118 97118RNG 97455 97458 97456 97459 97420 97410 97409 93380vms 97468 97208 93549 93556 97550 97408 97422 97425 97435 97358 97552 97231 97230 97584), $(PLATFORM))
 B_REFSW_CFLAGS += -O$(OPTIMIZER_LEVEL)
 ifneq ($(findstring $(B_REFSW_ARCH),i386-linux mips-vxworks), $(B_REFSW_ARCH))
 B_REFSW_CFLAGS += -mips32
 endif
 ifeq ($(findstring $(PLATFORM),97325), $(PLATFORM))
 B_REFSW_CFLAGS += -O$(OPTIMIZER_LEVEL)
 ifneq ($(findstring $(B_REFSW_ARCH), i386-mingw i386-linux mips-vxworks), $(B_REFSW_ARCH))
 B_REFSW_CFLAGS += -mips32r2
 endif
 endif
else
ifneq ($(SYSTEM),linuxkernel)
# -O (not -O2) is required for correct float math
B_REFSW_CFLAGS += -O -Wuninitialized
endif
endif

endif

ifeq ($(SYSTEM),linuxkernel)
B_REFSW_LDFLAGS += -r -d
LINUX ?= /opt/brcm/linux
LINUX_VER_GE_2_6 ?= $(shell (grep 'PATCHLEVEL = 6' ${LINUX}/Makefile >/dev/null && echo 'y'))
ifeq ($(LINUX_VER_GE_2_6),y)
 B_REFSW_CFLAGS += -fno-common
 LINUX_PATCHLEVEL = $(shell grep 'PATCHLEVEL = ' ${LINUX}/Makefile | awk '{print $$3}')
 LINUX_SUBLEVEL = $(shell grep 'SUBLEVEL = ' ${LINUX}/Makefile | awk '{print $$3}')
 LINUX_VER_GE_2_6_31 = $(shell test $(LINUX_PATCHLEVEL) -eq 6 -a $(LINUX_SUBLEVEL) -ge 31 && echo y)
 LINUX_VER_EQ_2_6_12 = $(shell (grep 'SUBLEVEL = 12' ${LINUX}/Makefile >/dev/null && echo 'y'))
 LINUX_VER_EQ_2_6_18 = $(shell (grep 'SUBLEVEL = 18' ${LINUX}/Makefile >/dev/null && echo 'y'))
 LINUX_VER_EQ_2_6_31 = $(shell (grep 'SUBLEVEL = 31' ${LINUX}/Makefile >/dev/null && echo 'y'))
 LINUX_VER_EQ_2_6_37 = $(shell (grep 'SUBLEVEL = 37' ${LINUX}/Makefile >/dev/null && echo 'y'))
 ifeq ($(LINUX_VER_EQ_2_6_12),y)
  ifeq ($(shell grep -e 'EXTRAVERSION =-[1-3]' $(LINUX)/Makefile > /dev/null && echo 'y'),)
  # Version 2.6.12-4.0 or later
  B_REFSW_CFLAGS += -DHAVE_RSRVD=1
  endif
 else
  # Non 2.6.12 version, assume 2.6.18 or later
  B_REFSW_CFLAGS += -DHAVE_RSRVD=1
 endif
endif
endif

###############################################
#
# SYSTEM-specific defines.
# The following values are supported: linux, linuxkernel, vxworks
# Keep all rules explicit and positive. Don't add "ifneq linux".

B_REFSW_CFLAGS += -DB_SYSTEM_${SYSTEM}=1

#
# linux
#
ifeq ($(SYSTEM),linux)
ifeq ($(B_REFSW_ARCH),mips-linux)
B_REFSW_CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_BIG
else
B_REFSW_CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
endif
B_REFSW_CFLAGS += -D_GNU_SOURCE=1
B_REFSW_CFLAGS_SHARED = -fpic -DPIC
B_REFSW_CFLAGS += -DLINUX
B_REFSW_CFLAGS += -pipe
B_REFSW_CFLAGS += -W
# Get the uclibc version for a runtime check in bsettop_init()
ifneq ($(B_REFSW_ARCH),i386-linux)
UCLIBC_VERSION = $(shell basename $(TOOLCHAIN_DIR)/mipsel-linux/lib/libuClibc*)
B_REFSW_CFLAGS += -DUCLIBC_VERSION=\"$(UCLIBC_VERSION)\"
endif
B_REFSW_CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
ifneq ($(BUILD_SYSTEM),legacy)
# NOTE: This will be upgraded to -pedantic-errors soon.
B_REFSW_CFLAGS += -pedantic
endif
B_REFSW_C_ONLY_FLAGS = -std=c99
endif

ifeq ($(SYSTEM),win32)
B_REFSW_CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
ifeq ($(B_REFSW_ARCH),i386-mingw)
B_REFSW_CFLAGS += -D_GNU_SOURCE=1
B_REFSW_CFLAGS_SHARED = -fpic -DPIC
B_REFSW_CFLAGS += -pipe
B_REFSW_CFLAGS += -W
B_REFSW_CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
B_REFSW_CFLAGS += -pedantic
endif
endif

#
# linux kernel
#
ifeq ($(SYSTEM),linuxkernel)
ifeq ($(B_REFSW_ARCH),mips-linux)
B_REFSW_CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_BIG
else
B_REFSW_CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
endif
ifeq ($(LINUX_VER_GE_2_6),y)
ifeq ($(PLATFORM),97325)
B_REFSW_CFLAGS += -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -Wa,--trap -DMODULE -mlong-calls \
	          -fno-common -ffreestanding -finline-limit=100000  -mabi=32 -march=mips32r2 -Wa,-32 -Wa,-march=mips32r2 -Wa,-mips32r2 \
	          -Wa,--trap -DMODULE -mlong-calls
else
B_REFSW_CFLAGS += -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -Wa,--trap -DMODULE -mlong-calls \
	          -fno-common -ffreestanding -finline-limit=100000  -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 \
	          -Wa,--trap -DMODULE -mlong-calls
endif
else
B_REFSW_CFLAGS += -mips2 -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -Wa,--trap -DMODULE -mlong-calls
endif
# -O2 is required for kernel inline functions
B_REFSW_CFLAGS += -O2
B_REFSW_CFLAGS += -msoft-float
B_REFSW_CFLAGS += -pipe

ifeq ($(LINUX_VER_GE_2_6_31),y)
B_REFSW_LINUXKERNEL_CFLAGS += -I${LINUX_INC} -DLINUX -D__KERNEL__ 
B_REFSW_LINUXKERNEL_CFLAGS += -I${LINUX_INC}/asm-mips \
	-I${LINUX}/arch/mips/include \
	-I${LINUX}/arch/mips/include/asm/mach-brcmstb \
	-I${LINUX}/arch/mips/include/asm/mach-generic
else
B_REFSW_LINUXKERNEL_CFLAGS += -nostdinc -I${LINUX_INC} -I${STD_INC} -I${GCC_INC} -DLINUX -D__KERNEL__
B_REFSW_LINUXKERNEL_CFLAGS += -I${LINUX_INC}/asm-mips/mach-brcmstb -I${LINUX_INC}/asm-mips/mach-mips
B_REFSW_LINUXKERNEL_CFLAGS += -I${LINUX_INC}/asm/mach-generic/
endif

endif

#
# vxworks
#
ifeq ($(SYSTEM),vxworks)
ifeq ($(WIND_BASE),)
$(error WIND_BASE is required. It is normally defined by torVars.bat or wrenv.exe)
endif
B_REFSW_CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_BIG
B_REFSW_CFLAGS += -DVXWORKS
B_REFSW_CFLAGS += -W
B_REFSW_LDFLAGS += -r -nostdlib

# Platform specific definitions (legacy)
ifeq ($(findstring $(PLATFORM),97111 97110 97312 97318 97318AVC), $(PLATFORM))
B_REFSW_CFLAGS += -DTOOL_FAMILY=gnu -DCPU=MIPS32 -DTOOL=sfgnu -mips2 -msoft-float -DSOFT_FLOAT
else
# Platform specific definitions (current)
ifeq ($(findstring $(PLATFORM),97401 97403 97400 97405 97335 97340 97342 97325 97435 97118 97118RNG 97468 97408), $(PLATFORM))
B_REFSW_CFLAGS += -DTOOL_FAMILY=gnu -DCPU=MIPS32 -DTOOL=sfgnu -mips2 -msoft-float -DSOFT_FLOAT
B_REFSW_LDFLAGS += -L$(WIND_BASE)/target/lib/mips/MIPS32/sfcommon
else
B_REFSW_CFLAGS += -DTOOL_FAMILY=gnu -DCPU=MIPS64 -DTOOL=gnu -mips3
B_REFSW_LDFLAGS += -L$(WIND_BASE)/target/lib/mips/MIPS64/common
endif
B_REFSW_CFLAGS += -G 0 -mno-branch-likely -fno-builtin -EB -DMIPSEB
endif

# Tool specific definitions
ifeq ($(vxWorksVersion),6)
ifneq ($(findstring $(PLATFORM),97401 97403 97400 97405 97325 97335 97340 97342 97435 97118 97118RNG), $(PLATFORM))
B_REFSW_CFLAGS += -mabi=o64 -mgp64
endif
B_REFSW_CFLAGS += -DVXWORKS6
B_REFSW_CFLAGS += -fdollars-in-identifiers
B_REFSW_CFLAGS += -I$(WIND_BASE)/target/h/wrn/coreip/ -I$(WIND_BASE)/target/h
endif
endif

###############################################
#
# Generic magnum defines. Useful if code is not chip-specific.
#
ifeq (${SYSTEM},linux)
B_REFSW_OS = linuxuser
else
B_REFSW_OS = ${SYSTEM}
endif

ifneq ($(BSEAV),)
MAGNUM := $(BSEAV)/../magnum
include ${MAGNUM}/basemodules/dbg/bdbg.inc
include ${MAGNUM}/basemodules/err/berr.inc
include ${MAGNUM}/basemodules/std/bstd.inc
include ${MAGNUM}/basemodules/chp/bchp.inc
include ${MAGNUM}/basemodules/reg/breg.inc
include ${MAGNUM}/basemodules/kni/bkni.inc
include ${MAGNUM}/basemodules/tmr/btmr.inc
include ${MAGNUM}/commonutils/lst/blst.inc

B_REFSW_MAGNUM_INCLUDE_DIRS = -I${BSEAV}/..
B_REFSW_MAGNUM_INCLUDE_DIRS += $(addprefix -I,$(BDBG_INCLUDES) $(BERR_INCLUDES) $(BSTD_INCLUDES) $(BCHP_INCLUDES) $(BREG_INCLUDES) $(BKNI_INCLUDES) $(BTMR_INCLUDES) $(BLST_INCLUDES))
# clear MAGNUM_MODULES to avoid a Nexus build error
MAGNUM_MODULES=
endif

B_REFSW_GENERIC_MAGNUM_CFLAGS += -DBCHP_CHIP=GENERIC -DBCHP_VER=BCHP_VER_A0
B_REFSW_CFLAGS += $(addprefix -D,$(MAGNUM_SYSTEM_DEFINES))
B_REFSW_CFLAGS += $(addprefix -D,$(BDBG_DEFINES))
