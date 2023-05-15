############################################################################
#     Copyright (c) 2011 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 7 $
# $brcm_Date: 9/19/12 3:17p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/linux/driver/usermode/Makefile $
# 
# 7   9/19/12 3:17p katrep
# SW7445-45:build for 7445
# 
# 6   9/18/12 3:04p katrep
# SW7445-45:driver build for 7445
#
# 5   5/8/12 3:46p erickson
# SW7435-27: fix linux 3.3 build
# 
# 4   6/9/11 4:04p jtna
# SW7405-3833: replace make with $(MAKE)
# 
# 3   3/23/11 4:34p erickson
# SW7420-1576: default LINUX to /opt/brcm/linux if undefined
#
# 2   3/8/11 10:58a erickson
# SW7420-1576: avoid conflicts with $LINUX/Makefile over ARCH=mips
#
# 1   3/7/11 4:39p erickson
# SW7420-1576: build bcmdriver with kbuild for linux 2.6.31 and following
#
############################################################################

CWD := $(shell pwd)

# NOTE: $LINUX/Makefile defaults ARCH=mips. It cannot be set to mipsel-linux here.
CROSS_COMPILE ?= mipsel-linux
OBJCOPY ?= $(CROSS_COMPILE)-objcopy
CP = cp -f

ifeq ($(PLATFORM),)
PLATFORM=$(NEXUS_PLATFORM)
endif
ifeq ($(PLATFORM),)
$(error You must define PLATFORM)
endif

ifeq ($(LINUX),)
LINUX = /opt/brcm/linux
endif

ifdef HUMAX_PLATFORM_BASE
AUTOCONF = $(addprefix -include ,$(INC_AUTOCONF))
endif

all:
ifdef HUMAX_PLATFORM_BASE
	$(MAKE) -C $(LINUX) M=$(CWD) modules ARCH=mips CFLAGS_MODULE="-DHUMAX_PLATFORM_BASE -DPLATFORM=$(PLATFORM) $(AUTOCONF)" PLATFORM=$(PLATFORM)
else
	$(MAKE) -C $(LINUX) M=$(CWD) modules ARCH=mips PLATFORM=$(PLATFORM)
endif
	@# disable MODVERSIONS if kernel symvers are missing
	@if [ -s $(LINUX)/Modules.symvers ]; then \
		$(OBJCOPY) --remove-section __versions bcmdriver.ko ; \
	fi

clean:
	$(MAKE) -C $(LINUX) M=$(CWD) clean ARCH=mips

ifeq ($(INSTALL_DIR),)
install:
	$(error INSTALL_DIR is undefined)
else
install: all
	@echo "[Install... bcmdriver.ko]"
	${Q_}$(CP) bcmdriver.ko $(INSTALL_DIR)
endif

