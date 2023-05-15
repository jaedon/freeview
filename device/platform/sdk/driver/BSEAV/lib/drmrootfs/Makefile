############################################################################
#     Copyright (c) 2011-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
############################################################################
#
# DESCRIPTION:
#   Makefile for building drmrootfs library.
#
############################################################################

MAGNUM_TOP := $(shell cd ../../../magnum; pwd)
NEXUS_TOP := $(shell cd ../../../nexus; pwd)


ifeq ($(NEXUS_MODE),proxy)
B_REFSW_OS = linuxkernel
PUBLIC_LIBS    += libdrmrootfs.a
else
B_REFSW_OS = linuxuser
PUBLIC_LIBS    += libdrmrootfs.a libdrmrootfs.so
endif

ifeq ($(ANDROID_BUILD),y)
LIB_DIR      = lib/android
else
LIB_DIR      = lib/$(B_REFSW_OS)
endif

# Include nexus definitions
include $(NEXUS_TOP)/build/nexus_defs.inc
include $(NEXUS_TOP)/build/os/linuxuser/os_tools.inc

ifndef BCHP_VER
$(error chip version not defined, export BCHP_VER=your version before compiling)
endif

#$(error ARCH $(ARCH) $(B_REFSW_ARCH))
#Note, Nexus's platform_app.inc will take care of loading magnum base module paths
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc

# Sanity check that we received a valid platform
ifndef BCHP_CHIP
$(error Unsupported platform $(NEXUS_PLATFORM), export PLATFORM="your chip numbper" before compiling)
endif


# Convert include paths into single variable
NEXUS_APP_INCLUDE_PATHS := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_PUBLIC_INCLUDES))
NEXUS_APP_DEFINES := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_DEFINES))
NEXUS_APP_DEFINES += $(foreach module, $(NEXUS_MODULES),NEXUS_HAS_$(module))

include $(NEXUS_TOP)/base/base.inc
include $(NEXUS_TOP)/modules/dma/dma.inc
include $(NEXUS_TOP)/modules/core/core.inc

include $(MAGNUM_TOP)/basemodules/mem/bmem.inc
include $(MAGNUM_TOP)/portinginterface/hsm/bhsm.inc

# Convert magnum includes into the same variable
NEXUS_APP_INCLUDE_PATHS += $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES))
NEXUS_APP_DEFINES += $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES))

CFLAGS += -DBSTD_CPU_ENDIAN=${NEXUS_ENDIAN}
CFLAGS += $(addprefix -I, $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES)))
CFLAGS += $(addprefix -D, $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
CFLAGS += $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))


TOOLCHAIN_DIR=$(TOOLCHAIN)

ifeq ($(ANDROID_BUILD),y)
ARCH ?= mips-linux-android
else
ARCH ?= mipsel-linux-uclibc
endif

SYSTEM ?= linux

ifeq ($(ARCH),mipsel-linux)
TOOLCHAIN_DIR=/opt/toolchains/mipsel
else
#
# Discover the uclibc toolchain directory assuming the compiler exists in bin subdir
# Use which and dirname bash shell commands.
#
TOOLCHAIN_DIR=$(shell dirname $(shell dirname $(shell which mipsel-linux-uclibc-gcc)))
endif

ifeq ($(B_REFSW_OS),linuxkernel)
USE_KERNEL_TIME ?= n  
ifeq ($(USE_KERNEL_TIME), y)
CFLAGS += -DKERNEL_TIME_DEFINE
endif
CFLAGS += -DMODULE -DLINUX -D__KERNEL__ #-DDRM_NO_NATIVE_64_TYPES 
CFLAGS += -nostdinc -msoft-float -fshort-wchar -O3 -fno-pic -G0 -mno-abicalls -mlong-calls
CFLAGS += -I$(MAGNUM)/basemodules/std/types/linuxkernel 
ifeq ($(SMP), y)
CFLAGS += -I$(LINUX)/include.mipsel-uclibc.$(PLATFORM)-smp
CFLAGS += -I$(LINUX)/include.mipsel-uclibc.$(PLATFORM)-smp/asm-mips/mach-brcmstb
CFLAGS += -I$(LINUX)/include.mipsel-uclibc.$(PLATFORM)-smp/asm/mach-generic/
else
CFLAGS += -I$(LINUX)/include
CFLAGS += -I$(LINUX)/arch/mips/include
CFLAGS += -I$(LINUX)/arch/mips/include/asm-generic
CFLAGS += -I$(LINUX)/arch/mips/include/asm/mach-rm
CFLAGS += -I$(LINUX)/arch/mips/include/asm/brcmceg
CFLAGS += -I$(LINUX)/arch/mips/include/asm/brcmceg/common
CFLAGS += -I$(LINUX)/arch/mips/include/asm/mach-generic
endif 
CFLAGS += -I$(TOOLCHAIN_DIR)/$(ARCH)/include
CFLAGS += -I$(TOOLCHAIN_DIR)/lib/gcc/$(ARCH)/4.5.3/include
endif

ifeq ($(ANDROID_BUILD),y)
CFLAGS += -EL
endif

############################################################################
#                              
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
SRCS	+= $(wildcard *.c )
OBJS 	+= $(patsubst %.c,%.o, ${SRCS})
CFLAGS  += -g -fshort-wchar  -fpic

PUBLIC_INCS    += drm_data.h

ifeq ($(ANDROID_BUILD),y)
LDFLAGS += -EL
LDFLAGS				+= -L$(NEXUS_TOP)/bin -lnexus
endif
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##



# Must export MSDRM_PRDY_SUPPORT to build the lib with playready support. 
ifeq ($(MSDRM_PRDY_SUPPORT), y)
CFLAGS += -DB_HAS_MSDRM_PRDY=1
endif

############################################################################
#                              MAIN TARGETS
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
all: ${PUBLIC_LIBS}

clean: 
	rm -f *.o *.d 
	rm ${LIB_DIR}/*.a
ifneq ($(NEXUS_MODE),proxy)
	rm ${LIB_DIR}/*.so
endif

#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##



############################################################################
#                             BUILD RULES
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
libdrmrootfs.so: ${OBJS}
	${CC} -shared -o  $(LIB_DIR)/$@  ${LDFLAGS}  $^ -Wl,-dy

libdrmrootfs.a: ${OBJS} 
#	$(LD) $(LIBFLAGS) $(LDFLAGS) $^ -o $(LIB_DIR)/$@
	${AR} rc  $(LIB_DIR)/$@  $^ 


############################################################################
#                             DEP FILES, PHONY
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
-include *.d
.PHONY: all install clean distclean 
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##





