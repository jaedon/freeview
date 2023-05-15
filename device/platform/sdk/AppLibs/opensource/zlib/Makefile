#############################################################################
#    (c)2010-2011 Broadcom Corporation
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
# $brcm_Revision: 3 $
# $brcm_Date: 6/9/11 5:15p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/zlib/Makefile $
# 
# 3   6/9/11 5:15p ismailk
# SW7425-596: Copy the zlib libraries
# 
# 2   5/11/11 7:59p curtis
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
ifeq ($(RVU_SUPPORT), y)
ZLIB_VER?=1.2.3
else
ZLIB_VER?=1.2.6
endif
ZLIB_NAME?=zlib-${ZLIB_VER}
ZLIB_TAR_PATH?=${PWD}
ZLIB_SOURCE_TARBALL?=$(ZLIB_NAME).tar.gz

ZLIB_LIB_FOLDER:=${ZLIB_TAR_PATH}/${ZLIB_NAME}/${PLATFORM}${LOWER_BCHP_VER}/lib
ZLIB_INC_FOLDER:=${ZLIB_TAR_PATH}/${ZLIB_NAME}/${PLATFORM}${LOWER_BCHP_VER}/include
ZLIB_PKG_FOLDER:=${ZLIB_TAR_PATH}/${ZLIB_NAME}/${PLATFORM}${LOWER_BCHP_VER}/lib/pkgconfig
ZLIB_BIN_FOLDER:=${ZLIB_TAR_PATH}/${ZLIB_NAME}/${PLATFORM}${LOWER_BCHP_VER}/bin

ZLIB_INC_CFLAGS:= -I${ZLIB_INC_FOLDER} -I${APPLIBS_TARGET_INC_DIR}/zlib
ZLIB_INC_LDFLAGS:= -L${ZLIB_LIB_FOLDER} -L${APPLIBS_TARGET_LIB_DIR} -lz 

.PHONY: all install target-clean clean

all: .source .$(ZLIB_NAME) install

.source:
	@if [ ! -e "${ZLIB_TAR_PATH}/${ZLIB_NAME}" ]; then \
		echo "================ Extracting ${ZLIB_NAME} tarball"; \
		if [ -e "${ZLIB_TAR_PATH}/${ZLIB_SOURCE_TARBALL}" ]; then \
			tar -xzf ${ZLIB_TAR_PATH}/${ZLIB_SOURCE_TARBALL} -C ${ZLIB_TAR_PATH}; \
		else \
			echo "Tarball ${ZLIB_TAR_PATH}/${ZLIB_SOURCE_TARBALL} is missing"; exit 1;\
		fi; \
	fi
	@mkdir -p ${ZLIB_TAR_PATH}/${ZLIB_NAME}/${PLATFORM}${LOWER_BCHP_VER}
	@touch $@

.$(ZLIB_NAME): .source
	@echo "================ Configure Zlib-${ZLIB_VER}"
	@cd  ${ZLIB_TAR_PATH}/${ZLIB_NAME}; \
	export CFLAGS="-fPIC -DPIC -O2"; \
	export AS=${AS}; \
	export LD=${LD}; \
	export CC=${CC}; \
	export CXX=${CXX}; \
	export AR="${AR} rc"; \
	export NM=${NM}; \
	export STRIP=${STRIP}; \
	export OBJCOPY=${OBJCOPY}; \
	export OBJDUMP=${OBJDUMP}; \
	export RANLIB=${RANLIB}; \
	chmod u+w Makefile; \
	/bin/sh configure \
	--prefix=${ZLIB_TAR_PATH}/${ZLIB_NAME}/${PLATFORM}${LOWER_BCHP_VER} \
	--shared ;
	@echo "================ Build Zlib-${ZLIB_VER}"
	@${MAKE} -C ${ZLIB_TAR_PATH}/${ZLIB_NAME} ${MAKE_SUFFIX}
	@${MAKE} -C ${ZLIB_TAR_PATH}/${ZLIB_NAME} install${MAKE_SUFFIX}
	@rm -rf libzlib_ver.inc 
	@echo "LIBZLIB_VER:="$(ZLIB_VER) >> libzlib_ver.inc
	@echo "LIBZLIB_NAME:="$(ZLIB_NAME) >> libzlib_ver.inc
	@echo "LIBZLIB_CFLAGS:="$(ZLIB_INC_CFLAGS) >> libzlib_ver.inc
	@echo "LIBZLIB_LDFLAGS:="$(ZLIB_INC_LDFLAGS) >> libzlib_ver.inc
	@echo "LIBZLIB_CONFIG_PATH:="$(ZLIB_BIN_FOLDER) >> libzlib_ver.inc
	@echo "LIBZLIB_PKG_PATH:="$(ZLIB_PKG_FOLDER) >> libzlib_ver.inc	
	@echo "================ Zlib-${ZLIB_VER} Compiled!"
	@touch $@

install: 
	@echo "================ Installing Zlib-${ZLIB_VER}"
	@mkdir -p ${APPLIBS_TARGET_LIB_DIR}
	@mkdir -p ${APPLIBS_TARGET_INC_DIR}/zlib
	@cp -af ${ZLIB_TAR_PATH}/${ZLIB_NAME}/*.so*	${APPLIBS_TARGET_LIB_DIR}
	@cp -af ${ZLIB_TAR_PATH}/${ZLIB_NAME}/*.h	${APPLIBS_TARGET_INC_DIR}/zlib
	@cp -af ${ZLIB_TAR_PATH}/${ZLIB_NAME}/${PLATFORM}${LOWER_BCHP_VER}/lib/*.so* ${APPLIBS_TARGET_LIB_DIR}
	@cp -af ${ZLIB_TAR_PATH}/${ZLIB_NAME}/${PLATFORM}${LOWER_BCHP_VER}/include/* ${APPLIBS_TARGET_INC_DIR}/zlib

target-clean:
	@echo "================ Removing Zlib-${ZLIB_VER} target binaries"
	@rm -rf ${APPLIBS_TARGET_LIB_DIR}/libz*
	@rm -rf ${APPLIBS_TARGET_INC_DIR}/zlib
	@rm -rf .source .$(ZLIB_NAME)

clean:
	@echo "================ MAKE Zlib-${ZLIB_VER} CLEAN"
	@rm -rf libzlib_ver.inc
	@rm -rf ${ZLIB_TAR_PATH}/${ZLIB_NAME}
	@rm -rf .source .$(ZLIB_NAME)
	@echo "================ Done"

