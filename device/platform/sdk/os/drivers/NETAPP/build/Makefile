############################################################################
#    (c)2006-2012 Broadcom Corporation
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
# $brcm_Revision: 14 $
# $brcm_Date: 2/3/12 4:18p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /NetApp/build/Makefile $
# 
# 14   2/3/12 4:18p steven
# SW7231-638: NetApp Version 6.2
# 
# 10   1/25/12 9:31a steven
# SW7346-638: Auto-Generated NetApp Release Candidate
# 
# 10   1/25/12 9:31a steven
# SW7346-638: Auto-Generated NetApp Release Candidate
# 
############################################################################
#
# DESCRIPTION: Makefile for building netapp and all it's dependancies
#
############################################################################
NETAPP_TOP 		= ${shell pwd}/..
NETAPP_LIBS_DIR = ${NETAPP_TOP}/libs
CURRENT_DATE	= $(shell date +%Y%m%d)
include ${NETAPP_TOP}/build/common.inc
include ${NETAPP_TOP}/netapp/modules.inc
NETAPP_SOURCE_TARBALL	= ${NETAPP_TOP}/../NetApp_v${NETAPP_VERSION_MAJOR}.${NETAPP_VERSION_MINOR}.${NETAPP_VERSION_INC}_${KERNELVERSION}_${NETAPP_ENDIAN}_src.tar.bz2

TOOLCHAIN_PATH ?= $(dir $(shell which ${CC}))
NETAPP_TARGET_PREP = netapp_target_prep.sh

NETAPP_DEP_LIBS	:= $(shell cd ../libs; ls)

ifeq (${DATABASE},n)
NETAPP_DEP_LIBS	:= $(filter-out sqlite, ${NETAPP_DEP_LIBS})
endif

#JSON-C is used by Voice recognition and database backend
ifeq (${DATABASE},n)
ifeq (${VOICE_RECOGNITION},n) 
NETAPP_DEP_LIBS	:= $(filter-out json-c, ${NETAPP_DEP_LIBS})
endif
endif

ifeq (${HOTPLUG},n)
NETAPP_DEP_LIBS	:= $(filter-out udev, ${NETAPP_DEP_LIBS})
endif

ifeq (${VOICE_RECOGNITION},n)
NETAPP_DEP_LIBS	:= $(filter-out flac, ${NETAPP_DEP_LIBS})
endif

ifeq (${ZEROCONF},n)
NETAPP_DEP_LIBS	:= $(filter-out avahi, ${NETAPP_DEP_LIBS})
endif

ifeq (${IPERF},n)
NETAPP_DEP_LIBS	:= $(filter-out iperf, ${NETAPP_DEP_LIBS})
endif

ifeq (${WIFI},n)
NETAPP_DEP_LIBS	:= $(filter-out usb, ${NETAPP_DEP_LIBS})
endif


NETAPP_DEP_LIBS	:= $(filter-out common.inc, ${NETAPP_DEP_LIBS})
		
ifeq (${WIFI_DRIVER}, falcon)
LIBUSB_NEEDED = libusb
endif

examples: install
	${Q_}make -C ../netapp examples
	${Q_}make -C ../examples install NETAPP_OUTPUT_DIR=${NETAPP_OUTPUT_DIR}

help:
	@echo ""
	@echo "**************************************************************"
	@echo "            NetApp Version ${NETAPP_VERSION_MAJOR}.${NETAPP_VERSION_MINOR}.${NETAPP_VERSION_INC}"
	@echo "**************************************************************"
	@echo "Options:"
ifneq (${VOICE_RECOGNITION},n)
	@echo "    Voice Recognition:   ENABLED  (VOICE_RECOGNITION=y)"
else
	@echo "    Bluetooth Voice:     DISABLED (VOICE_RECOGNITION=n)"
endif
ifneq (${HOTPLUG},n)
	@echo "    Hotplug Support:     ENABLED  (HOTPLUG=y)"
else
	@echo "    Hotplug Support:     DISABLED (HOTPLUG=n)"
endif
ifneq (${ZEROCONF},n)
	@echo "    Zeroconfig (Bonjour):ENABLED  (ZEROCONF=y)"
else
	@echo "    Zeroconfig (Bonjour):DISABLED (ZEROCONF=n)"
endif
ifneq (${DATABASE},n)
	@echo "    Database Support:    ENABLED  (DATABASE=y)"
else
	@echo "    Database Support:    DISABLED (DATABASE=n)"
endif
ifneq (${WIFI},n)
	@echo "    WiFi Support:        ENABLED  (WIFI=y)"
ifneq (${WIFI_INVITE},n)
	@echo "    WiFi Invite:         ENABLED  (WIFI_INVITE=y)"
else
	@echo "    WiFi Invite:         DISABLED (WIFI_INVITE=n)"
endif
ifneq (${WIFI_P2P},n)
	@echo "    WiFi Direct (P2P):   ENABLED  (WIFI_P2P=y)"
else
	@echo "    WiFi Direct (P2P):   DISABLED (WIFI_P2P=n)"
endif
ifneq (${WIFI_WPS},n)
	@echo "    WiFi WPS:            ENABLED  (WIFI_WPS=y)"
else
	@echo "    WiFi (WPS):          DISABLED (WIFI_WPS=n)"
endif
ifneq (${WIFI_TEST},n)
	@echo "    WiFi Test APIs:      ENABLED  (WIFI_TEST=y)"
else
	@echo "    WiFi Test APIs:      DISABLED (WIFI_TEST=n)"
endif
else
	@echo "    WiFi Support:        DISABLED (WIFI=n)"
	@echo "    WiFi Invite:         DISABLED"
	@echo "    WiFi Direct (P2P):   DISABLED"
	@echo "    WiFi (WPS):          DISABLED"
	@echo "    WiFi Test APIs:      DISABLED"
endif
ifeq (${WIFI_MFG_DRIVER},y)
	@echo "    WiFi Driver Type:    MANUFACTURING"
else
	@echo "    WiFi Driver Type:    PRODUCTION"
endif
ifneq (${BLUETOOTH},n)
	@echo "    Bluetooth Support:   ENABLED  (BLUETOOTH=y)"
else
	@echo "    Bluetooth Support:   DISABLED (BLUETOOTH=n)"
endif
	@echo ""
	@echo "Info:"
ifndef LINUX
	@echo "ERR:LINUX:               NOT SET!"
	@exit -1
else
ifeq (,${KCONFIG_FILE})
	@echo "LINUX=${LINUX} KCONFIG_FILE=${KCONFIG_FILE} ERR:LINUX: Your Kernel has not been built or ${LINUX}/.config is not found"
	@exit -1
else
	@echo "    LINUX:               ${LINUX}"
endif
endif
ifdef NEXUS_TOP
	@echo "    NEXUS_TOP:           ${NEXUS_TOP}"
endif
	@echo "    NETAPP_OUTPUT_DIR:   ${NETAPP_OUTPUT_DIR}"
	@echo "    NETAPP_TARGET_DIR:   ${NETAPP_TARGET_DIR}"
ifdef DISABLE_WIRED_SUPPORT
	@echo "    DISABLE_WIRED_SUPPORT:${DISABLE_WIRED_SUPPORT}"
endif
	@echo "    Kernel Version:      ${KERNELVERSION}"
	@echo "    Toolchain:           ${TOOLCHAIN_PATH}"
ifneq (${WIFI},n)
	@echo "    WiFi Version:        ${WIFI_DRIVER_VERSION} (WIFI_DRIVER=${WIFI_DRIVER})"
ifeq ($(findstring ${WIFI_CHIP},${WIFI_SUPPORTED_CHIPS}), ${WIFI_CHIP})
	@echo "    WiFi Chip:           ${WIFI_CHIP} (WIFI_CHIP=${WIFI_CHIP})"
else
	@echo "ERR:WiFi Chip:           Invalid Chips, Supported types(WIFI_CHIP): ${WIFI_SUPPORTED_CHIPS}"
	@exit -1
endif
endif
ifneq (${BLUETOOTH},n)
	@echo "    Bluetooth Version:   ${BT_PKG_VERSION}"
endif	
	@echo "********************************************************************"
	@sleep 1

${NETAPP_PREBUILT_TARBALL}:  dirs ${LIBUSB_NEEDED}
	${Q_}make -C ../netapp wlan
	${Q_}make -C ../netapp bluetooth
	${Q_}cd ${NETAPP_OUTPUT_DIR}/../../; tar -cjf ${NETAPP_PREBUILT_TARBALL} .
	${Q_}make distclean ${SQUASH_OUTPUT}
	@echo "==== Finished Creating $@"

# needed for Falcon driver
ifeq (${WIFI_DRIVER},falcon)
libusb:
	${Q_}make -C ../libs/usb install
else
libusb:
endif
    	   
tarball: ${NETAPP_SOURCE_TARBALL}

prebuilt: ${NETAPP_PREBUILT_TARBALL}

${NETAPP_SOURCE_TARBALL}: ${NETAPP_PREBUILT_TARBALL}
	@echo "==== Creating Release tarball"
	@cd ${NETAPP_TOP}/; tar --exclude=.git -cjf $@ . 
	@echo "==== Finished Creating $@"

install: all
	${Q_}install ${NETAPP_TOP}/etc/${UDHCPC_LITE_SCRIPT} ${NETAPP_OUTPUT_BIN_DIR}
	${Q_}cp -f netapp_target_prep.sh ${NETAPP_OUTPUT_BIN_DIR}

dirs: help
	@echo "==== Starting NetApp Build"
	${Q_}if ! [ -d ${NETAPP_OUTPUT_FW_DIR}     ] ; then 	mkdir -p ${NETAPP_OUTPUT_FW_DIR}; fi
	${Q_}if ! [ -d ${NETAPP_OUTPUT_LIB_DIR}     ] ; then 	mkdir -p ${NETAPP_OUTPUT_LIB_DIR}; fi
	${Q_}if ! [ -d ${NETAPP_OUTPUT_INC_DIR}     ] ; then 	mkdir -p ${NETAPP_OUTPUT_INC_DIR}; fi
	${Q_}if ! [ -d ${NETAPP_OUTPUT_BIN_DIR}     ] ; then 	mkdir -p ${NETAPP_OUTPUT_BIN_DIR}; fi
	${Q_}if ! [ -d ${NETAPP_OUTPUT_MODULES_DIR} ] ; then 	mkdir -p ${NETAPP_OUTPUT_MODULES_DIR}; fi
	${Q_}if ! [ -d ${NETAPP_PREBUILT_DIR}       ] ; then 	mkdir -p ${NETAPP_PREBUILT_DIR}; fi

docs:
	${Q_}make -C ../docs
	${Q_}make -C ../docs clean

libs: help dirs
	${Q_}for lib in ${NETAPP_DEP_LIBS}; do \
		if [ -d ${NETAPP_LIBS_DIR}/$$lib ] ; then \
			make ${MAKE_J_ARG} -C ${NETAPP_LIBS_DIR}/$$lib install; \
		fi; \
	done
	
distclean-libs:
	${Q_}for lib in ${NETAPP_DEP_LIBS}; do \
		if [ -d ${NETAPP_LIBS_DIR}/$$lib ] ; then \
			make ${MAKE_J_ARG} -C ${NETAPP_LIBS_DIR}/$$lib distclean; \
		fi; \
	done

clean-libs:
	${Q_}for lib in ${NETAPP_DEP_LIBS}; do \
		if [ -d ${NETAPP_LIBS_DIR}/$$lib ] ; then \
			make ${MAKE_J_ARG} -C ${NETAPP_LIBS_DIR}/$$lib clean; \
		fi; \
	done

clean-netapp:
	${Q_}make -C ../netapp clean

ifeq (${HUMAX_PLATFORM_BASE},y)
NETAPP_INSTALL_PARAMS += HUMAX_PLATFORM_BASE=y
NETAPP_INSTALL_PARAMS += WIFI_DRIVER_VERSION=$(WIFI_DRIVER_VERSION)
NETAPP_INSTALL_PARAMS += WIFI_DRIVER_PATH=$(WIFI_DRIVER_PATH)
netapp: dirs
	${Q_}make -C ../netapp prep
	${Q_}make -C ../netapp $(NETAPP_INSTALL_PARAMS) install 
else
netapp: dirs
	${Q_}make -C ../netapp prep
	${Q_}make -C ../netapp install
endif

distclean-netapp:
	${Q_}make -C ../netapp distclean

clean-examples: 
	${Q_}if [ -e ${NETAPP_OUTPUT_INC_DIR}/netapp.inc ] ; then \
		make -C ../netapp clean-examples && \
		make -C ../examples clean NETAPP_OUTPUT_DIR=${NETAPP_OUTPUT_DIR}; \
	fi
	

# build libs if we are not in applibs tree
ifeq (${APPLIBS_INC},)
clean: help clean-examples clean-netapp clean-libs
all: dirs libs netapp
distclean: help clean-examples distclean-libs distclean-netapp
ifeq (${HUMAX_PLATFORM_BASE},y)
			rm -rf $(NETAPP_TOP)/stage $(NETAPP_TOP)/opt $(NETAPP_TOP)/netapp/wlan/broadcom/bwl/mips
endif
else
clean: help clean-examples clean-netapp
all: dirs netapp
distclean: help clean-examples distclean-netapp
endif
	${Q_}if [ -e ${BLUETOOTH_SRC} ] || [ -e ${WIFI_DRIVER_PKG} ] || [ -e ${NETAPP_PREBUILT_TARBALL} ] ; then \
		echo "==== removing ${NETAPP_STAGE_DIR}"; \
		if [ -d ${NETAPP_STAGE_DIR} ] ; then rm -rf  ${NETAPP_STAGE_DIR};  fi \
	fi

clean-to-origin: distclean 
	${Q_}rm -rf ${NETAPP_PREBUILT_DIR} ${NETAPP_TOP}/../*.bz2
	

#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##


############################################################################
#                             DEP FILES, PHONY
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##

.PHONY: help examples install all clean distclean idirs prebuilt docs netapp
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##
