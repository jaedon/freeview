#############################################################################
#    (c)2009-2012 Broadcom Corporation
#
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
# USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
# ANY LIMITED REMEDY.
#
#############################################################################

APPLIBS_TOP := $(shell cd ../..; pwd)

# include cross-compiler definitions
include $(APPLIBS_TOP)/common/common.inc

#JPEG_VER ?= 6b
JPEG_VER ?= 8d
JPEG_NAME ?= jpeg-$(JPEG_VER)
JPEG_TOP ?= $(APPLIBS_TOP)/opensource/jpeg
JPEG_DIR ?= $(APPLIBS_TOP)/opensource/jpeg/jpeg-$(JPEG_VER)
JPEG_SOURCE_TARBALL ?= $(JPEG_TOP)/$(JPEG_NAME).tar.gz

JPEG_LIB_FOLDER:=$(JPEG_DIR)/.libs
JPEG_INC_FOLDER:=$(JPEG_DIR)

JPEG_INC_CFLAGS:= -I$(JPEG_INC_FOLDER) -I$(APPLIBS_TARGET_INC_DIR)/jpeg
JPEG_INC_LDFLAGS:= -L$(JPEG_LIB_FOLDER) -L$(APPLIBS_TARGET_LIB_DIR) -ljpeg

all: jpeg-source jpeg-config jpeg install

.PHONY: jpeg-source jpeg-config jpeg install clean

jpeg-source:
	@if ! [ -e "$(JPEG_DIR)/.unpacked" ]; then \
		cd $(JPEG_TOP); \
		tar -xzkf $(JPEG_SOURCE_TARBALL); \
		touch $(JPEG_DIR)/.unpacked; \
	fi

clean:
	@echo libjpeg clean
	@if [ -d "$(JPEG_DIR)" ]; then \
		$(MAKE) -C $(JPEG_DIR) clean; \
	fi
	@rm -rf $(APPLIBS_TARGET_LIB_DIR)/libjpeg.so*
	@rm -f $(JPEG_DIR)/.configured $(JPEG_TOP)/libjpeg_ver.inc
	@rm -rf $(JPEG_DIR)

jpeg-config:
	@if ! [ -e "$(JPEG_DIR)/.configured" ]; then \
		$(call fn-jpeg-configure); \
	fi

jpeg:
	@echo "================ Build JPEG-$(JPEG_VER)"
	@$(MAKE) -C $(JPEG_DIR) V=0
	@if [ $$? -ne 0 ]; then \
		echo -e "\n**************************************************"; \
		echo "* ERROR: Failed to compile JPEG-$(JPEG_VER) - Aborting! *"; \
		echo "**************************************************"; \
		/bin/false; \
	else \
		echo "================ Creating JPEG-$(JPEG_VER) inc file"; \
		rm -rf libjpeg_ver.inc; \
		echo "JPEG_VER:="$(JPEG_VER) >> $(JPEG_TOP)/libjpeg_ver.inc; \
		echo "JPEG_NAME:="$(JPEG_NAME) >> $(JPEG_TOP)/libjpeg_ver.inc; \
		echo "JPEG_CFLAGS:="$(JPEG_INC_CFLAGS) >> $(JPEG_TOP)/libjpeg_ver.inc; \
		echo "JPEG_LDFLAGS:="$(JPEG_INC_LDFLAGS) >> $(JPEG_TOP)/libjpeg_ver.inc; \
		if ! [ -e "$(JPEG_DIR)/.libs/libjpeg.a" ]; then \
			$(MKDIR) $(JPEG_DIR)/.libs; \
			$(CP) $(JPEG_DIR)/libjpeg.a $(JPEG_DIR)/.libs/. ;\
		fi; \
		echo "================ JPEG-$(JPEG_VER) Compiled" ; \
	fi

install: 	
	@echo "================ Installing JPEG"
	@mkdir -p ${APPLIBS_TARGET_LIB_DIR}
	@mkdir -p ${APPLIBS_TARGET_INC_DIR}
	@cp -af ${JPEG_DIR}/.libs/*.so* ${APPLIBS_TARGET_LIB_DIR}

define fn-jpeg-configure
	echo "================ Configure JPEG-$(JPEG_VER)"; \
	cd $(JPEG_DIR); \
	export AS=$(AS); \
	export LD=$(LD); \
	export CC=$(CC); \
	export CXX=$(CXX); \
	export AR=$(AR); \
	export NM=$(NM); \
	export STRIP=$(STRIP); \
	export OBJCOPY=$(OBJCOPY); \
	export OBJDUMP=$(OBJDUMP); \
	export RANLIB=$(RANLIB); \
	export CFLAGS="-fPIC -DPIC -O2"; \
	export CXXFLAGS="-fPIC -DPIC -O2"; \
	export CPPFLAGS="-fPIC -DPIC -O2"; \
	$(JPEG_DIR)/configure --host=$(B_REFSW_ARCH) --prefix=/usr --enable-shared --enable-silent-rules; \
	if [ $$? -ne 0 ]; then \
		echo -e "\n**************************************************"; \
		echo "* ERROR: Failed to configure JPEG-$(JPEG_VER) - Aborting! *"; \
		echo "**************************************************"; \
		/bin/false; \
	else \
		touch $(JPEG_DIR)/.configured; \
	fi
endef
