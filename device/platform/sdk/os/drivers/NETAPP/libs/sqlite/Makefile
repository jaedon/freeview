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
# $brcm_Workfile: Makefile $
# $brcm_Revision: 4 $
# $brcm_Date: 7/7/11 4:58p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/sqlite/Makefile $
# 
# 4   7/7/11 4:58p abelous
# Adding pocibility to control prefix from outside call
# 
# 3   6/13/11 2:36p ismailk
# SW7425-710: Remove the .file with -f option
# 
# 2   6/9/11 7:17p ismailk
# SW7425-596:Create a .file and not rebuild again
#
#############################################################################
#Check if SQLite is bundled with NetApp or not 
NETAPP_TOP	:= $(wildcard ${shell pwd}/../../broadcom/netapp)
ifneq (${NETAPP_TOP},)

PWD := ${shell pwd}
OPENSOURCE ?= ${PWD}/../
APPLIBS_TOP ?= ${PWD}/../../
NEXUS_TOP ?= ${APPLIBS_TOP}/../nexus

include ${OPENSOURCE}/common/common.inc

SQLITE_VER?=3.6.23.1
SQLITE_NAME:=sqlite-$(SQLITE_VER)
SQLITE_TAR_PATH:=${PWD}
SQLITE_SOURCE_TARBALL:=$(SQLITE_NAME).tar.gz
SQLITE_CFLAGS?= -O2 -fPIC
SQLITE_LDFLAGS?= 
SQLITE_LIB_FOLDER:=${SQLITE_TAR_PATH}/${SQLITE_NAME}/${PLATFORM}${LOWER_BCHP_VER}/lib
SQLITE_INC_FOLDER:=${SQLITE_TAR_PATH}/${SQLITE_NAME}/${PLATFORM}${LOWER_BCHP_VER}/include
SQLITE_PKG_FOLDER:=${SQLITE_TAR_PATH}/${SQLITE_NAME}/${PLATFORM}${LOWER_BCHP_VER}/lib/pkgconfig
SQLITE_BIN_FOLDER:=${SQLITE_TAR_PATH}/${SQLITE_NAME}/${PLATFORM}${LOWER_BCHP_VER}/bin


SQLITE_INC_CFLAGS:= -D_REENTRANT -I${SQLITE_INC_FOLDER}
SQLITE_INC_LDFLAGS:= -L${SQLITE_LIB_FOLDER}/ -lsqlite3
SQLITE_PREFIX_DIR ?= ${SQLITE_TAR_PATH}/${SQLITE_NAME}/${PLATFORM}${LOWER_BCHP_VER}

.PHONY: clean all install target-clean

all: .$(SQLITE_NAME) install

.$(SQLITE_NAME):
	@if [ ! -d "${SQLITE_TAR_PATH}/${SQLITE_NAME}" ]; then \
		echo "${SQLITE_TAR_PATH}/${SQLITE_NAME} not found"; \
		if [ -e "${SQLITE_TAR_PATH}/${SQLITE_SOURCE_TARBALL}" ]; then \
			echo "================ Extracting SQLITE tarball"; \
			tar -xzf ${SQLITE_TAR_PATH}/${SQLITE_SOURCE_TARBALL} -C ${SQLITE_TAR_PATH}; \
		else \
			echo "Tarball ${SQLITE_TAR_PATH}/${SQLITE_SOURCE_TARBALL} is missing"; exit 1;\
		fi \
	fi
	
	@if [ ! -e "${SQLITE_TAR_PATH}/${SQLITE_NAME}/Makefile" ]; then \
		echo "================ Configure SQLITE"; \
		cd ${SQLITE_TAR_PATH}/${SQLITE_NAME}; \
		CFLAGS="${SQLITE_CFLAGS}" \
		LDFLAGS="${SQLITE_LDFLAGS}" \
		./configure \
		--prefix=${SQLITE_PREFIX_DIR} \
		--host=${APPLIBS_TOOLCHAIN_ARCH} \
		--disable-static \
		--enable-shared; \
	fi
	
	@if [ ! -e "${SQLITE_TAR_PATH}/${SQLITE_NAME}/Makefile" ]; then \
		echo "${SQLITE_NAME} is not configured!"; exit 1;\
	fi	

	@echo "================ Compiling SQLITE"	
	${MAKE} ${MAKE_OPTIONS} -C  ${SQLITE_TAR_PATH}/${SQLITE_NAME} ${MAKE_SUFFIX}
	${MAKE} -C ${SQLITE_TAR_PATH}/${SQLITE_NAME} install ${MAKE_SUFFIX}
	@if [ "$(DEBUG)" == 'y' ]; then \
		${STRIP} ${SQLITE_LIB_FOLDER}/*.so; \
	fi
	@rm -rf sqlite_ver.inc
	@echo "SQLITE_VER:="$(SQLITE_VER) >> sqlite_ver.inc
	@echo "SQLITE_NAME:="$(SQLITE_NAME) >> sqlite_ver.inc
	@echo "SQLITE_CFLAGS:="$(SQLITE_INC_CFLAGS) >> sqlite_ver.inc
	@echo "SQLITE_LDFLAGS:="$(SQLITE_INC_LDFLAGS) >> sqlite_ver.inc
	@echo "SQLITE_CONFIG_PATH:="$(SQLITE_BIN_FOLDER) >> sqlite_ver.inc
	@echo "SQLITE_PKG_PATH:="$(SQLITE_PKG_FOLDER) >> sqlite_ver.inc
	@echo "================ SQLITE Compiled!"
	@touch $@

	

install: 
	@echo "================ Installing SQLITE"
	@mkdir -p ${APPLIBS_TARGET_LIB_DIR}
	@mkdir -p ${APPLIBS_TARGET_INC_DIR}
	@mkdir -p ${APPLIBS_TARGET_BIN_DIR}/sql
	@cp -af ${SQLITE_PREFIX_DIR}/lib/*.so* ${APPLIBS_TARGET_LIB_DIR}
	@cp -af ${SQLITE_PREFIX_DIR}/include/* ${APPLIBS_TARGET_INC_DIR}
	@cp -af ${SQLITE_PREFIX_DIR}/bin/* ${APPLIBS_TARGET_BIN_DIR}/sql

target-clean:
	@echo "================ Removing target SQLITE binaries"
	@rm -rf ${APPLIBS_TARGET_LIB_DIR}/libsqlite3.so*
	@rm -rf ${APPLIBS_TARGET_INC_DIR}/sqlite*
	@rm -rf ${APPLIBS_TARGET_BIN_DIR}/sql
	
clean:
	@echo "================ MAKE CLEAN"
	@rm -rf sqlite_ver.inc
	@rm -rf ${SQLITE_TAR_PATH}/${SQLITE_NAME}	
	@rm -f .$(SQLITE_NAME)
	@echo "================ Done"

# SQLite is bundled with NetApp
else
NETAPP_TOP 			:= ${shell pwd}/../..
include ${NETAPP_TOP}/libs/common.inc
endif

