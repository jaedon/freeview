############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 4 $
# $brcm_Date: 3/24/11 4:41p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97420/Makefile $
# 
# 4   3/24/11 4:41p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 3   3/8/11 2:00p erickson
# SW7420-1576: default B_REFSW_ARCH the same as nexus
# 
# 2   3/8/11 10:58a erickson
# SW7420-1576: avoid conflicts with $LINUX/Makefile over ARCH=mips
#
# 1   3/8/11 10:19a erickson
# SW7420-1576: switch 97420 and following to use kbuild only
#
############################################################

#
# This Makefile uses Linux kbuild
# It applies to 2.6.31 and later
# Do not add anything platform- or chip-specific to this Makefile. It is generic.
#

# USE B_REFSW_ARCH here, but we hardcode ARCH=mips for kernel 
# before calling into the kbuild environment
ifeq ($(B_REFSW_ARCH),)
B_REFSW_ARCH=mipsel-linux
endif

ifdef DEBUG
B_REFSW_DEBUG ?= $(DEBUG)
endif
ifeq ($(B_REFSW_DEBUG),)
B_REFSW_DEBUG=y
endif
ifeq ($(B_REFSW_DEBUG),y)
BCM_OBJ_DIR=$(B_REFSW_ARCH).debug$(NEXUS_BIN_DIR_SUFFIX)
else
BCM_OBJ_DIR=$(B_REFSW_ARCH).release$(NEXUS_BIN_DIR_SUFFIX)
endif


.PHONY: $(BCM_OBJ_DIR)
CP = cp -f

all install: $(BCM_OBJ_DIR)
ifdef HUMAX_PLATFORM_BASE
	$(MAKE) -C ../../usermode ARCH=mips CFLAGS=HUMAX_PLATFORM_BASE $@
else
	$(MAKE) -C ../../usermode ARCH=mips $@
endif
	$(CP) ../../usermode/bcmdriver.ko $(BCM_OBJ_DIR)

$(BCM_OBJ_DIR):
	$(Q_)mkdir -p $(BCM_OBJ_DIR)

clean:
	$(MAKE) -C ../../usermode ARCH=mips $@
	$(RM) -r $(BCM_OBJ_DIR)
