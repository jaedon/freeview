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
#############################################################################
WORKING_DIR := $(shell cd ../openssl; pwd)
BSEAV_TOP := $(shell cd ../../../BSEAV; pwd)
NEXUS_TOP := $(shell cd ../../../nexus; pwd)

#$(error WORKING_DIR = ${WORKING_DIR})

# include cross-compiler definitions
include ${NEXUS_TOP}/platforms/common/build/platform_app.inc
include ${NEXUS_TOP}/platforms/common/build/nexus_platforms.inc

ifeq ($(B_REFSW_ARCH),)
export B_REFSW_ARCH = mipsel-linux
endif

BSEAV_TOOLCHAIN_ARCH ?= $(B_REFSW_ARCH)
BSEAV_TARGET_DIR ?= ${WORKING_DIR}/${PLATFORM}
BSEAV_TARGET_LIB_DIR ?= ${BSEAV_TARGET_DIR}/lib
BSEAV_TARGET_INC_DIR ?= ${BSEAV_TARGET_DIR}/include
BSEAV_TARGET_BIN_DIR ?= ${BSEAV_TARGET_DIR}/bin
#$(warning BSEAV_TARGET_LIB_DIR = ${BSEAV_TARGET_LIB_DIR})
#$(warning BSEAV_TARGET_INC_DIR = ${BSEAV_TARGET_INC_DIR})
#$(warning BSEAV_TOOLCHAIN_ARCH = ${BSEAV_TOOLCHAIN_ARCH})
#$(error BSEAV_TOOLCHAIN_ARCH = ${BSEAV_TOOLCHAIN_ARCH})

OPENSSL_VER?=1.0.0f
OPENSSL_NAME?=openssl-${OPENSSL_VER}
OPENSSL_TAR_PATH:=${WORKING_DIR}
OPENSSL_SOURCE_TARBALL:=$(OPENSSL_NAME).tar.gz
OPENSSL_CFLAGS?= -02 -fPIC
OPENSSL_LDFLAGS?=
OPENSSL_BASE_FOLDER:=${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/${PLATFORM}${LOWER_BCHP_VER}/usr/local/
OPENSSL_LIB_FOLDER:=${OPENSSL_BASE_FOLDER}/lib
OPENSSL_INC_FOLDER:=${OPENSSL_BASE_FOLDER}/include
OPENSSL_PKG_FOLDER:=${OPENSSL_BASE_FOLDER}/lib/pkgconfig
OPENSSL_BIN_FOLDER:=${OPENSSL_BASE_FOLDER}/bin

OPENSSL_INC_CFLAGS:= -I${OPENSSL_INC_FOLDER} 
OPENSSL_INC_LDFLAGS:= -L${OPENSSL_LIB_FOLDER}/ -lssl -lcrypto

OPENSSL_CONFIG_ENTRY := "${BSEAV_TOOLCHAIN_ARCH}"  ,"${BSEAV_TOOLCHAIN_ARCH}-gcc :-fPIC -DLENDIAN -DTERMIO -O3 -fomit-frame-pointer -Wall :: -D_REENTRANT ::-ldl: BN_LLONG RC4_CHAR RC4_INDEX DES_INT DES_UNROLL DES_RISC2 :::::::::::::::dlfcn:linux-shared:-fPIC:-shared:.so.\\$$(SHLIB_MAJOR).\\$$(SHLIB_MINOR)",

.PHONY: clean all bin-target target-clean

all: .source .$(OPENSSL_NAME) install

.source: 
	if [ ! -d "${OPENSSL_TAR_PATH}/${OPENSSL_NAME}" ]; then \
		echo "${OPENSSL_TAR_PATH}/${OPENSSL_NAME} not found"; \
		if [ -e "${OPENSSL_TAR_PATH}/${OPENSSL_SOURCE_TARBALL}" ]; then \
			echo "================ Extracting OPENSSL tarball"; \
			tar -xzf ${OPENSSL_TAR_PATH}/${OPENSSL_SOURCE_TARBALL} -C ${OPENSSL_TAR_PATH}; \
			patch -t -d ${OPENSSL_TAR_PATH}/${OPENSSL_NAME} -p1 < openssl-Chrome.patch; \
		else \
			echo "Tarball ${OPENSSL_TAR_PATH}/${OPENSSL_SOURCE_TARBALL} is missing"; exit 1;\
		fi \
	fi

.configure: .source
	@if [ ! -e "${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/.configured" ]; then \
		echo "================ Configure OPENSSL"; \
		sed -ie 's/^\(\#\#\#\# Variety of LINUX\).*/\1 \n${OPENSSL_CONFIG_ENTRY}/' ${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/Configure; \
		cd ${OPENSSL_TAR_PATH}/${OPENSSL_NAME}; \
		CFLAGS="${OPENSSL_CFLAGS}" \
		LDFLAGS="${OPENSSL_LDFLAGS}" \
		./Configure \
		enable-tls \
		no-zlib \
		threads \
		no-idea \
		no-mdc2 \
		no-rc5 \
		no-ripemd \
		shared \
		${BSEAV_TOOLCHAIN_ARCH} \
		-pthread \
		--prefix=/usr/local; \
		${MAKE} -C ${OPENSSL_TAR_PATH}/${OPENSSL_NAME} depend; \
		touch ${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/.configured; \
	fi
	




.$(OPENSSL_NAME): .source .configure
	@echo "================ Compiling OpenSSL"
	${MAKE} -C ${OPENSSL_TAR_PATH}/${OPENSSL_NAME} ${MAKE_SUFFIX}
	${MAKE} -C ${OPENSSL_TAR_PATH}/${OPENSSL_NAME} install INSTALL_PREFIX=${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/${PLATFORM}${LOWER_BCHP_VER}  ${MAKE_SUFFIX} 
	@rm -rf openssl_ver.inc
	@echo "OPENSSL_VER="$(OPENSSL_VER) >> openssl_ver.inc
	@echo "OPENSSL_NAME="$(OPENSSL_NAME) >> openssl_ver.inc
	@echo "OPENSSL_PATH="$(OPENSSL_BASE_FOLDER) >> openssl_ver.inc
	@echo "OPENSSL_CFLAGS:="$(OPENSSL_INC_CFLAGS) >> openssl_ver.inc
	@echo "OPENSSL_LDFLAGS:="$(OPENSSL_INC_LDFLAGS) >> openssl_ver.inc
	@echo "OPENSSL_PKG_PATH:="$(OPENSSL_PKG_FOLDER) >> openssl_ver.inc
	@echo "================ OpenSSL Compiled"

install: 
	@echo "================ Installing OpenSSL"
	@mkdir -p ${BSEAV_TARGET_LIB_DIR}
	@mkdir -p ${BSEAV_TARGET_INC_DIR}
	@mkdir -p ${BSEAV_TARGET_BIN_DIR}/ssl
	@cp -af ${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/${PLATFORM}${LOWER_BCHP_VER}/usr/local/lib/*.so* ${BSEAV_TARGET_LIB_DIR}
	@cp -af ${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/${PLATFORM}${LOWER_BCHP_VER}/usr/local/lib/engines ${BSEAV_TARGET_LIB_DIR}
	@cp -af ${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/${PLATFORM}${LOWER_BCHP_VER}/usr/local/include/* ${BSEAV_TARGET_INC_DIR}
	@cp -af ${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/${PLATFORM}${LOWER_BCHP_VER}/usr/local/bin/* ${BSEAV_TARGET_BIN_DIR}
	@cp -af ${OPENSSL_TAR_PATH}/${OPENSSL_NAME}/${PLATFORM}${LOWER_BCHP_VER}/usr/local/ssl ${BSEAV_TARGET_BIN_DIR}/../

target-clean:
	@echo "================ Removing target OpenSSL binaries"
	@rm -rf ${BSEAV_TARGET_LIB_DIR}/libssl.so*
	@rm -rf ${BSEAV_TARGET_LIB_DIR}/libcrypto.so*
	@rm -rf ${BSEAV_TARGET_LIB_DIR}/engines
	@rm -rf ${BSEAV_TARGET_INC_DIR}/openssl
	@rm -rf ${BSEAV_TARGET_INC_DIR}/../ssl

clean:
	@echo "================ MAKE CLEAN"
	@rm -rf openssl_ver.inc
	@rm -rf ${OPENSSL_TAR_PATH}/${OPENSSL_NAME} 
	@rm -rf ${PLATFORM}
	@echo "================ Done"
