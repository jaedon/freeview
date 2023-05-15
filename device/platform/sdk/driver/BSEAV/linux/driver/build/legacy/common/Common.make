############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Common.make $
# $brcm_Revision: 8 $
# $brcm_Date: 3/24/11 4:49p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/common/Common.make $
# 
# 8   3/24/11 4:49p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 7   11/30/06 11:26a jgarrett
# PR 26266: Fixing unconditional mkdir (caused some things to rebuild
# always)
# 
# 6   11/16/06 5:05p jgarrett
# PR 25230: Adding 2.4/2.6 distinction for proper build flags
# 
# 5   9/19/06 5:18p gmohile
# PR 24129: Added support for 7313
# 
# 4   8/21/06 1:04p erickson
# PR22882: convert soft-float to use source, not checked in binaries
# 
# 3   7/14/06 11:31a jgarrett
# PR 19909: Merging to latest baseline
# 
# PR19909/2   7/13/06 6:49p jgarrett
# PR 19909: Fixing install rule printout
# 
# PR19909/1   7/13/06 5:36p jgarrett
# PR 19909: Quieting down build messages
# 
# 2   6/12/06 1:45p jgarrett
# PR 20139: Changing GCC_BASE for 2.6.x
# 
# 1   2/8/05 12:17a dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/31   12/7/04 6:35p bandrews
# PR13158: Initial 7318 support
# 
# Irvine_BSEAVSW_Devel/30   9/16/04 6:15p bandrews
# PR12686: Added 7327 7312
# 
# Irvine_BSEAVSW_Devel/29   8/26/04 4:12p erickson
# PR12476: removed deprecated compile option
# 
# Irvine_BSEAVSW_Devel/28   8/19/04 1:14p erickson
# PR12101: added -DUSE_LEGACY_KERNAL_INTERFACE for code shared between
# legacy and magnum
# 
# Irvine_BSEAVSW_Devel/27   8/19/04 10:29a erickson
# PR12101: use tspsi_validate to check both transport message filter
# correction and section length
#
# Irvine_BSEAVSW_Devel/26   8/12/04 6:25p bandrews
# PR11657: Added 2nd graphics device for 7329
#
# Irvine_BSEAVSW_Devel/25   8/12/04 4:32p bandrews
# PR11657: Added 7329 support
#
# Irvine_BSEAVSW_Devel/24   7/26/04 1:48p erickson
# PR11674: bcmdmadrv.c should always be included because of mem-to-mem
# DMA for settop api double buffering
#
# Irvine_BSEAVSW_Devel/23   5/25/04 10:10a bandrews
# PR11203: Removed EXPORT_SYMTAB
#
# Irvine_BSEAVSW_Devel/22   5/12/04 10:31a erickson
# PR10967: use std header
############################################################

#
# Common driver build for all platforms
# This should not be used to build usermode code
#
# Inputs: SETTOP, BSEAV, BUILDDIR, BCM_CHIP, DRIVERS
# Outputs: Lots of stuff
#

.PHONY: checkdirs

DEBUG ?= y
BCM_ENDIAN ?= le
TRANSMSG_SUPPORT ?= y
#
# Configure linux kernel source
# Output is LINUX_INC
#
include ../common/Linux.make
include ../common/tools.mak

ifeq ($(DEBUG),y)
BCM_OBJ_DIR=$(B_REFSW_ARCH).debug
else
BCM_OBJ_DIR=$(B_REFSW_ARCH).release
endif
LDFLAGS += -G 0

#
# Standard toolchain directories
# See tools.mak for TOOLCHAIN_DIR
#
STD_INC ?= $(TOOLCHAIN_DIR)/mipsel-linux/include
GCC_BASE ?= $(shell (test -d $(TOOLCHAIN_DIR)/lib/gcc-lib && echo $(TOOLCHAIN_DIR)/lib/gcc-lib/mipsel-linux) || echo $(TOOLCHAIN_DIR)/lib/gcc/mipsel-linux)

#
# BEWARE: If you have more than one version of gcc installed, 
# the following line is going to have a problem.
#
GCC_VER = $(shell (ls $(GCC_BASE)))
GCC_INC = $(GCC_BASE)/$(GCC_VER)/include

$(STD_INC):
	@test -d $@ || (echo "STD_INC is incorrect"; test 0 == 1)

$(GCC_BASE):
	@test -d $@ || (echo "GCC_BASE is incorrect"; test 0 == 1)
	
$(SETTOP):
	@test -d $@ || (echo "SETTOP is incorrect"; test 0 == 1)

$(BSEAV):
	@test -d $@ || (echo "BSEAV is incorrect"; test 0 == 1)
	
$(BUILDDIR):
	@test -d $@ || (echo "BUILDDIR is incorrect"; test 0 == 1)
	
$(BCM_OBJ_DIR):
	${Q_}mkdir -p $(BCM_OBJ_DIR)
	
checkdirs: $(LINUX_INC) $(STD_INC) $(GCC_BASE) $(SETTOP) $(BSEAV) $(BUILDDIR) $(BCM_OBJ_DIR)

CFLAGS += \
	-DMPEGV_VIRTUAL_VSYNC \
	-DBCM_KERNEL_CACHE_FLUSH \
	-DBCM_TRICK_MODE_SUPPORT \
	-DUSE_LEGACY_KERNAL_INTERFACE \
	-DUSE_BCM_DEVICE_ARRAY

ifeq ($(LINUX_VER_GE_2_6),y)
# 2.6 flags
CFLAGS += \
	-D__KERNEL__  -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing \
	-fno-common -ffreestanding -O2 -fomit-frame-pointer -G 0  -mno-abicalls -fno-pic -pipe  \
	-finline-limit=100000  -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 \
	-Wa,--trap -DMODULE -mlong-calls -nostdinc
else
# 2.4 flags
CFLAGS += \
	-mips2 -Wa,--trap -nostdinc \
	-fomit-frame-pointer -fno-strict-aliasing -fno-pic \
	-mlong-calls -mno-abicalls \
	-G 0 \
	-Wall
endif

COMMON_DRIVER_INFRASTRUCTURE += \
	bcmdevice.c			\
	bcmkernel.c			\
	bcmmemmgr.c			\
	bcmhw.c				\
	brcm_dbg.c			\
	genericlist.c		\
	intmgr.c			\
	bcmprocfs.c			\
	bcm_linux.c 		\
	bcmcfg.c 			\
	bcminitstack.c		\
	bcmreg.c				\
	bcmio_drv.c

COMMON_DRIVER_BACKEND += \
	bcmplaydrv.c 		\
	bcmdecodedrv.c		\
	bcmwindowdrv.c		\
	bcmdbgdrv.c			\
	bge_intmgr.c		\
	bcmtimer.c			\
	bcmoutputdrv.c		\
	bcmvolumedrv.c		\
	bcmdmadrv.c			

ifeq ($(TRANSMSG_SUPPORT),y)
COMMON_DRIVER_BACKEND += \
    bcmmsgdrv.c         \
    msglib.c			\
	tspsi_validate.c
else
CFLAGS += -DNO_TRANSMSG
endif

# These should be removed. We eventually want all warnings.
CFLAGS += -Wno-comment -Wno-unused -Wno-format

LDFLAGS += --warn-common

# Keep this until the old RFM code goes away
CFLAGS += -DRFM_SIMPLIFIED
# __BCM93725B__ is necessary for any set-top (non PCI), I think this
# is only used in SetTop/kernelinterface/bcmkernel.c/h
CFLAGS += -D__BCM93725B__

ifneq ("$(LINUX_INC)","" )
	CFLAGS += -I$(LINUX_INC)
endif
ifneq ("$(STD_INC)","" )
	CFLAGS += -I$(STD_INC)
endif
ifneq ("$(GCC_INC)","" )
	CFLAGS += -I$(GCC_INC)
endif

ifeq ($(DEBUG),y)
	CFLAGS += -DBRCM_DBG -DBRCM_DEBUG -DCONFIG_PROC_FS -DMEM_DEBUG
endif
ifeq ($(BCM_ENDIAN),le)
	CFLAGS += -D_LITTLE_ENDIAN_ -DBRCM_ENDIAN_LITTLE
else
	CFLAGS += -DBRCM_ENDIAN_BIG
endif
CFLAGS += -DLINUX -O2 -D__KERNEL__ -DMODULE 
ifeq ($(BCM_CHIP),venom2)
CFLAGS += -DBCM7030
else
CFLAGS += -DBCM$(BCM_CHIP)
endif

# additional defines
ifeq ($(findstring $(BCM_CHIP),7312 7313 7314 7315 7317 7318 7319 7320 7327 7328 7329), $(BCM_CHIP))
CFLAGS += -DBCM73XX
endif
ifeq ($(findstring $(BCM_CHIP),7319 7320 7327 7328 7329), $(BCM_CHIP))
CFLAGS += -DBCM732X
endif

# PR8054: We can't use the fpu in kernel mode unless it's
# in a critical section. The Linux exception handler doesn't
# store the fpu registers.
CFLAGS += -msoft-float

# deprecated. drivers should switch to compiling from source
FLOATLIB = $(BSEAV)/linux/driver/floatlib/$(B_REFSW_ARCH)/soft-fp.a

# compiile from source
FLOATLIB_OBJS += fp-bit-single.o fp-bit-double.o
FLOATLIB_DIR = $(BSEAV)/linux/driver/floatlib/src

#
# Driver installation
#
ifeq ($(INSTALL_DIR),)
install:
	$(error INSTALL_DIR is undefined)
else
install:
	@echo "[Install... $(sort $(notdir $(DRIVERS)))]"
	${Q_}$(CP) $(DRIVERS) $(INSTALL_DIR)
endif

#
# Clean up drivers
#
clean: $(OTHER_CLEANS)
	${Q_}$(RM) -r $(BCM_OBJ_DIR)

