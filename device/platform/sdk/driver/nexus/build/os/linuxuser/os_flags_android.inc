############################################################
#     (c)2010-2011 Broadcom Corporation
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
# $brcm_Workfile: os_flags_android.inc $
# $brcm_Revision: 9 $
# $brcm_Date: 11/14/11 3:31p $
#
# Module Description:
#   This file defines the Android flags for building in linuxuser mode.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/linuxuser/os_flags_android.inc $
# 
# 9   11/14/11 3:31p ttrammel
# SW7425-1375: Fix HC mr1 vs mr2 compatibility issue.
# 
# 8   11/8/11 4:41p ttrammel
# SW7425-1375: Honeycomb 3.2 aka mr2 check-in.
# 
# 7   11/8/11 10:55a ttrammel
# SW7425-1685: Remove reference to AndroidConfig.h.
# 
# 6   4/27/11 5:50p ttrammel
# SW7425-434: Add toolchain variable to build.
# 
# 5   3/15/11 12:57p ttrammel
# SW7420-1640: Send NEXUS messages to Android log.
# 
# 4   2/18/11 10:54a ttrammel
# SW7420-1506: Update toolchain path.
# 
# 3   12/23/10 3:56p ttrammel
# SW7420-1331: Update Android build to use Broadcom toolchain.
# 
# 2   11/16/10 6:27p ttrammel
# SW7420-1228: Initial Android check-in SQA fixes.
# 
# 1   11/10/10 7:04p ttrammel
# SW7420-1228: Initial Support for Android.
# 
############################################################

############################################################
# Overview:
#
# This file should only be included by os_flags.inc
#
############################################################
ANDROID_LIB = $(ANDROID)/out/target/product/bcm$(BCHP_CHIP)/obj/lib

# Default for toolchain version
ifndef B_REFSW_TOOLCHAIN_VER
B_REFSW_TOOLCHAIN_VER=4.4.5
endif

# Remove setting not applicable to Android
NEXUS_CFLAGS = -mhard-float -DBDBG_DEBUG_BUILD=1 -D_GNU_SOURCE=1 -DLINUX -pipe -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE -mips32 -W -Wstrict-prototypes -Wno-unused-value

# Android includes
NEXUS_CFLAGS += \
    -I$(ANDROID)/system/core/include \
    -I$(ANDROID)/hardware/libhardware/include \
    -I$(ANDROID)/hardware/ril/include \
    -I$(ANDROID)/dalvik/libnativehelper/include \
    -I$(ANDROID)/frameworks/base/include \
    -I$(ANDROID)/external/skia/include \
    -I$(ANDROID)/out/target/product/bcm$(BCHP_CHIP)/obj/include \
    -I$(ANDROID)/bionic/libc/arch-mips/include \
    -I$(ANDROID)/bionic/libc/include \
    -I$(ANDROID)/bionic/libstdc++/include \
    -I$(ANDROID)/bionic/libc/kernel/common \
    -I$(ANDROID)/bionic/libc/kernel/arch-mips \
    -I$(ANDROID)/bionic/libm/include \
    -I$(ANDROID)/bionic/libthread_db/include \
    -I$(ANDROID)/bionic/libm/mips \
    -I$(ANDROID)/bionic/libm \
    -I$(ANDROID)/out/target/product/bcm$(BCHP_CHIP)/obj/SHARED_LIBRARIES/libm_intermediates

# Preprocessor flags
NEXUS_CFLAGS += \
    -D__ARM_ARCH_5__ \
    -D__ARM_ARCH_5T__ \
    -D__ARM_ARCH_5E__ \
    -D__ARM_ARCH_5TE__ \
    -DANDROID \
    -DSK_RELEASE \
    -DNDEBUG \
    -DHAVE_SYS_UIO_H \
    -UDEBUG \
    -DB_REFSW_ANDROID

# Compile flags
NEXUS_CFLAGS += -EL -march=mips32 -fpic -fno-exceptions -ffunction-sections \
		-funwind-tables -fmessage-length=0

# Optimize flags
NEXUS_CFLAGS += -O2 -finline-functions -finline-limit=300 -fno-inline-functions-called-once \
		-fgcse-after-reload -frerun-cse-after-loop -frename-registers \
		-fomit-frame-pointer -fstrict-aliasing -funswitch-loops


# Loader flags
ifdef NEXUS_SHARED_LIB
NEXUS_LDFLAGS += -EL -nostdlib -Wl,-T,$(ANDROID)/build/core/mipself.xsc \
		-Wl,--gc-sections -Wl,-shared,-Bsymbolic \
		-L$(ANDROID_LIB) \
		-Wl,--no-whole-archive -lcutils -lc -lm -lgcc \
		-Wl,--no-undefined \
		-Wl,--whole-archive
else
NEXUS_LDFLAGS += -EL -Bdynamic -Wl,-T,$(ANDROID)/build/core/mipself.x \
		-Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections \
		-Wl,-z,nocopyreloc -Wl,--no-undefined -Wl,-rpath-link=$(ANDROID_LIB) \
		-L$(ANDROID_LIB) -nostdlib \
		$(ANDROID_LIB)/crtend_android.o \
		$(ANDROID_LIB)/crtbegin_dynamic.o \
		-lc \
		-lm \
		-lgcc
endif

#NEXUS_CFLAGS += -msoft-float



