#############################################################################
#    (c)2010-2012 Broadcom Corporation
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
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTLIBCURLLAR PURPOSE,
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
# $brcm_Workfile: Makefile $
# $brcm_Revision: 5 $
# $brcm_Date: 6/9/11 6:54p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /AppLibs/opensource/libpng/Makefile $
# 
# 5   6/9/11 6:54p ismailk
# SW7425-596: Change CFLAGS to CPPFLAGS
# 
# 4   6/9/11 5:17p ismailk
# SW7425-596: Copy libpng libraries to target folder
# 
# 3   5/19/11 7:36p ismailk
# SW7425-596:Deprecate the ARCH variable and set the B_REFSW_ARCH
#
# 2   5/11/11 8:05p curtis
# SW7420-1866: Common makefile for opensource library
#
#############################################################################
PWD := ${shell pwd}
OPENSOURCE ?= ${PWD}/../
APPLIBS_TOP ?= ${PWD}/../../
NEXUS_TOP ?= ${APPLIBS_TOP}/../nexus

ifeq ($(ANDROID_BUILD), y)
else
include ${OPENSOURCE}/common/common.inc
endif

-include ${OPENSOURCE}/zlib/libzlib_ver.inc

ifeq ($(RVU_SUPPORT), y)
PNG_VER ?= 1.2.29
else
PNG_VER ?= 1.5.10
endif
PNG_NAME ?= libpng-${PNG_VER}
PNG_TAR_PATH ?= ${PWD}
PNG_SOURCE_TARBALL ?= $(PNG_NAME).tar.gz

PNG_LIB_FOLDER:=${PNG_TAR_PATH}/${PNG_NAME}/${PLATFORM}${LOWER_BCHP_VER}/lib
PNG_INC_FOLDER:=${PNG_TAR_PATH}/${PNG_NAME}/${PLATFORM}${LOWER_BCHP_VER}/include
PNG_PKG_FOLDER:=${PNG_TAR_PATH}/${PNG_NAME}/${PLATFORM}${LOWER_BCHP_VER}/lib/pkgconfig
PNG_BIN_FOLDER:=${PNG_TAR_PATH}/${PNG_NAME}/${PLATFORM}${LOWER_BCHP_VER}/bin

PNG_INC_CFLAGS:= -I${PNG_INC_FOLDER} -I${APPLIBS_TARGET_INC_DIR}/png
PNG_INC_LDFLAGS:= -L${PNG_LIB_FOLDER} -L${APPLIBS_TARGET_LIB_DIR} -lpng

PNG_LIBVER := $(shell echo ${PNG_VER} | egrep -o "[0-9+]\.[0-9+]" | sed 's/\.//')

.PHONY: all install target-clean clean

all: .source .$(PNG_NAME) install

.source:
	@if [ ! -e ${OPENSOURCE}/zlib/${LIBZLIB_NAME}/libz.so ]; then \
		echo "Zlib must be built prior to building PNG."; \
		exit 1; \
	fi

	@if [ ! -e "${PNG_TAR_PATH}/${PNG_NAME}" ]; then \
		echo "================ Extracting ${PNG_NAME} tarball"; \
		if [ -e "${PNG_TAR_PATH}/${PNG_SOURCE_TARBALL}" ]; then \
			tar -xzf ${PNG_TAR_PATH}/${PNG_SOURCE_TARBALL} -C ${PNG_TAR_PATH}; \
		else \
			echo "Tarball ${PNG_TAR_PATH}/${PNG_SOURCE_TARBALL} is missing"; exit 1;\
		fi; \
	fi
	@mkdir -p ${PNG_TAR_PATH}/${PNG_NAME}/${PLATFORM}${LOWER_BCHP_VER}
	@touch $@

.$(PNG_NAME): .source
	@echo "================ Configure PNG-${PNG_VER}"; \
	cd  ${PNG_TAR_PATH}/${PNG_NAME}; \
	export LDFLAGS=${LIBZLIB_LDFLAGS}; \
	export CPPFLAGS="-fPIC -DPIC -O2 ${LIBZLIB_CFLAGS}"; \
	export AS=${AS}; \
	export LD=${LD}; \
	export CC=${CC}; \
	export CXX=${CXX}; \
	export AR="${AR}"; \
	export NM=${NM}; \
	export STRIP=${STRIP}; \
	export OBJCOPY=${OBJCOPY}; \
	export OBJDUMP=${OBJDUMP}; \
	export RANLIB=${RANLIB}; \
	chmod u+w Makefile; \
	/bin/sh configure \
	--host=${APPLIBS_TOOLCHAIN_ARCH} \
	--prefix=${PNG_TAR_PATH}/${PNG_NAME}/${PLATFORM}${LOWER_BCHP_VER} \
	--enable-shared;

	@echo "================ Build PNG-${PNG_VER}"
	@${MAKE} -C ${PNG_TAR_PATH}/${PNG_NAME} ${MAKE_SUFFIX}
	@${MAKE} -C ${PNG_TAR_PATH}/${PNG_NAME} install ${MAKE_SUFFIX}
	@rm -rf libpng_ver.inc
	@echo "LIBPNG_VER:="$(PNG_VER) >> libpng_ver.inc
	@echo "LIBPNG_NAME:="$(PNG_NAME) >> libpng_ver.inc
	@echo "LIBPNG_CFLAGS:="$(PNG_INC_CFLAGS) >> libpng_ver.inc
	@echo "LIBPNG_LDFLAGS:="$(PNG_INC_LDFLAGS) >> libpng_ver.inc
	@echo "LIBPNG_CONFIG_PATH:="$(PNG_BIN_FOLDER) >> libpng_ver.inc
	@echo "LIBPNG_PKG_PATH:="$(PNG_PKG_FOLDER) >> libpng_ver.inc
	@echo "================ PNG-${PNG_VER} Compiled!"
	@touch $@
	@if ! [ -e "${PNG_TAR_PATH}/${PNG_NAME}/.libs/libpng.so" ]; then \
		cd ${PNG_TAR_PATH}/${PNG_NAME}/.libs/; \
		ln -s libpng$(PNG_LIBVER).so libpng.so; \
	fi

install:
	@echo "================ Installing PNG-${PNG_VER}"
	@mkdir -p ${APPLIBS_TARGET_LIB_DIR}
	@mkdir -p ${APPLIBS_TARGET_INC_DIR}/png
	@cp -af ${PNG_TAR_PATH}/${PNG_NAME}/.libs/*.so* ${APPLIBS_TARGET_LIB_DIR}
	@cp -af ${PNG_TAR_PATH}/${PNG_NAME}/*.h         ${APPLIBS_TARGET_INC_DIR}/png
	@cp -af ${PNG_TAR_PATH}/${PNG_NAME}/${PLATFORM}${LOWER_BCHP_VER}/lib/*.so* ${APPLIBS_TARGET_LIB_DIR}
	@cp -afr ${PNG_TAR_PATH}/${PNG_NAME}/${PLATFORM}${LOWER_BCHP_VER}/include/* ${APPLIBS_TARGET_INC_DIR}/png

target-clean:
	@echo "================ Removing PNG-${PNG_VER} target binaries"
	@rm -rf ${APPLIBS_TARGET_LIB_DIR}/libpng*
	@rm -rf ${APPLIBS_TARGET_INC_DIR}/png
	@rm -rf .source .$(PNG_NAME)

clean:
	@echo "================ MAKE PNG-${PNG_VER} CLEAN "
	@rm -rf libpng_ver.inc
	@rm -rf ${PNG_TAR_PATH}/${PNG_NAME}
	@rm -rf .source .$(PNG_NAME)
	@echo "================ Done"

