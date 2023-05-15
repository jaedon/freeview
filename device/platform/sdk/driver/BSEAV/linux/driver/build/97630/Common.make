############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Common.make $
# $brcm_Revision: 1 $
# $brcm_Date: 12/10/10 5:41p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97630/Common.make $
# 
# 1   12/10/10 5:41p etrudeau
# SWBLURAY-23579: merge 7630 support to Hydra to get ready for 7640 chip
# 
# bdvd_v3.0/1   9/11/09 4:38p jquinlan
# pr 16362
# 
# bdvd_v3.0/1   2/27/09 4:44p jquinlan
# new file
# 
# BCM7601_bringup_temp/1   7/10/08 4:46p fbasso
# Add support for 7601.
# 
############################################################

#
# Common driver build for all platforms
# This should not be used to build usermode code
#
# Inputs: DRIVERS
# Outputs: Lots of stuff
#

BSEAV=$(shell cd ../../../..;pwd)

#
# Configure linux kernel source
# Output is LINUX_INC
#
include Linux.make
# defines TOOLCHAIN_DIR, DEBUG, ARCH
include tools.mak

ifeq ($(DEBUG),)
DEBUG=y
endif

ifeq ($(DEBUG),y)
BCM_OBJ_DIR=$(ARCH).debug
else
BCM_OBJ_DIR=$(ARCH).release
endif

#########################################################
#
# Standard include directories
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
	
$(BSEAV):
	@test -d $@ || (echo "BSEAV is incorrect"; test 0 == 1)
	
$(BCM_OBJ_DIR):
	${Q_}mkdir -p $(BCM_OBJ_DIR)
	
.PHONY: checkdirs
checkdirs: $(LINUX_INC) $(STD_INC) $(GCC_BASE) $(BSEAV) $(BCM_OBJ_DIR)

ifeq ($(DEBUG),y)
CFLAGS += -DCONFIG_PROC_FS
endif

#########################################################
#
# soft float
#
# PR8054: We can't use the fpu in kernel mode unless it's
# in a critical section. The Linux exception handler doesn't
# store the fpu registers.

CFLAGS += -msoft-float
FLOATLIB_OBJS += fp-bit-single.o fp-bit-double.o
FLOATLIB_DIR = $(BSEAV)/linux/driver/floatlib/src

#########################################################
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

