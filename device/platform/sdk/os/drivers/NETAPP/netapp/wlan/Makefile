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
# $brcm_Revision: 11 $
# $brcm_Date: 2/3/12 4:18p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /NetApp/netapp/wlan/Makefile $
# 
# 11   2/3/12 4:18p steven
# SW7231-638: NetApp Version 6.2
# 
# 10   1/30/12 3:50p steven
# SW7429-37: Fixes to bringup 7241 WiFi
# 
# 11   12/9/11 4:44p steven
# SW7231-513: USB Hotplug Support in NetApp
# 
# SW7231-513/1   12/9/11 3:36p steven
# SW7231-513: Finished working hotplug detection
# 
# 9   12/1/11 8:56p steven
# SW7425-1404: Moved kernel modules and fixes to BT support
# 
# 8   11/24/11 5:14p steven
# SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
# 
# SW7425-1404/5   11/24/11 5:01p steven
# SW7425-1404: Don't build driver in parallel build
# 
# SW7425-1404/4   11/24/11 4:02p steven
# SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
# 
# SW7425-1404/1   10/7/11 5:33p steven
# SW7425-1404: initial pass fo BT support
# 
# 7   10/4/11 3:01p steven
# SW7425-1399: Added support in the makefile to load prebuilt by default
# 
# 6   7/29/11 3:44p steven
# SW7425-749: Fixed makefile rule
# 
# 5   7/28/11 2:14p steven
# SW7425-749: Initial WoWL and P2P development checkin
# 
# 4   7/7/11 10:56p steven
# SWBLURAY-26474: ndvd SRC package build fails
# 
# 3   6/23/11 10:54a steven
# SW7420-1956: AppLibs make clean gives error
# 
# 2   6/15/11 5:27p steven
# SW7425-714: Adding NetApp support to AppLibs
# 
# 1   6/14/11 10:41a steven
# SW7425-714: Moved NetApp to AppLibs
#
############################################################################
#
# DESCRIPTION:
#   Makefile for building non-NetApp WiFi code
#
# !!!!!! IMPORTANT !!!!!!
# If your platform contains OpenSSL then set EXTERNAL_OPENSSL=1 and 
# EXTERNAL_OPENSSL_BASE=<FULL path to openssl>. also if you need to link with 
# another library like zlib then set EXTERNAL_LIBS
#
############################################################################
NETAPP_TOP = ${shell pwd}/../..
include ${NETAPP_TOP}/build/common.inc
include ../modules.inc

ifneq (,$(findstring mips,$(CC)))
	ARCH				:= mips
    ifeq (${CONFIG_CPU_BIG_ENDIAN},y)
    TARGETARCH			:= mips_be
	TARGETMACH			:= mipseb
	TARGETENV			:= linuxmips_be
    else
    TARGETARCH			:= mips
	TARGETMACH			:= mipsel
	TARGETENV			:= linuxmips
    endif
else
	TARGETARCH	:=x86
	ARCH		:=x86
	TARGETENV	:= linux
endif

################################################################################
#                              WIFI_CHIPSET:
# specify which WiFi driver to use
#vvvv###########################################################################
ifeq (${WIFI_CHIPSET}, broadcom)

# Broadcom Userspace Library Definitions
WPSENR_APP			:= wpsenr_app
WPSAPI_TESTER		:= wpsapitester
BWL_DIR				= ${WIFI_CHIPSET}/bwl
P2PLIB_DIR			= ${WIFI_DRIVER_PKG}/src/p2p/p2plib
WFI_DIR				= ${WIFI_DRIVER_PKG}/src/apps/wfi/linux
WPSCLI_DIR			= ${WIFI_DRIVER_PKG}/src/wps/wpscli
WPSAPI_DIR			= ${WIFI_DRIVER_PKG}/src/wps/wpsapi
WPSCOM_DIR			= ${WIFI_DRIVER_PKG}/src/wps/common
WPSENR_DIR			= ${WIFI_DRIVER_PKG}/src/wps/linux/enr
OUPUT_DIR	 		= ${NETAPP_TOP}/${CC}
ifeq (y, ${HUMAX_PLATFORM_BASE})
EPIVERS_DIR			= ${WIFI_DRIVER_PATH}/src/include
else
EPIVERS_DIR			= ${WIFI_DRIVER_PKG}/src/include
endif
EPIVERS				= ${EPIVERS_DIR}/epivers.h
F_PUBLIC_LIBS		+= ${NETAPP_SHARED_LIBS}


#check what libraries we need to include to build

ifeq (${NETAPP_NEEDS_WPSCLI}, y)
F_PUBLIC_INCS		+= wpscli_api.h
INC_DIRS			+= ${WPSCLI_DIR}/include
endif

WL					= wl${TARGETARCH}
DHD					= dhd${TARGETARCH}

############################
#  WiFi Library Defines
############################
# BWL Support
INC_DIRS			+= \
	${WIFI_CHIPSET} \
	${P2PLIB_DIR}/include \
	${WIFI_CHIPSET}/bwl \
	${EPIVERS_DIR} \
	${OUPUT_DIR}
	 
F_PUBLIC_INCS		+= epivers.h bwl.h typedefs.h site_typedefs.h bcmdefs.h

# WiFi Direct (P2P)
ifneq (${WIFI_P2P},n)
INC_DIRS			+= ${P2PLIB_DIR}/include
F_PUBLIC_INCS 		+= BcmP2PAPI.h
WIFI_EXAMPLES		+= bcmp2papp
endif

# WiFi Invite
ifneq (${WIFI_INVITE},n)
INC_DIRS			+= ${WFI_DIR}/include
F_PUBLIC_INCS		+= wfi_api.h
endif

# WPS 2.0 & Internal WPS library
ifneq (${WIFI_WPS},n)
INC_DIRS			+= ${WPSAPI_DIR}/common/include
F_PUBLIC_INCS		+= wps_sdk.h
endif

BLDTYPE 			?= release
else
$error "WIFI_CHIPSET ${WIFI_CHIPSET} Not supported"
endif


############################################################################
#                              NETAPP WIFI TARGETS
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
SRC_DIRS			+= .
INC_DIRS 			+= . ../
CPPFLAGS 			+= -DHAS_WIFI
SHARED_LDFLAGS		+= -Wl,-Bsymbolic,-dy,-lstdc++,-lpthread,-lm
LDFLAGS 			+= -L${NETAPP_OUTPUT_LIB_DIR} -L.
F_SRCS 				= $(filter-out \
           				$(addprefix %/, ${EXCLUDED_SRCS}), \
               			$(foreach d,${SRC_DIRS},$(wildcard ${d}/*.c ${d}/*.cpp)))
F_OBJS_COMPILED 	+= $(patsubst %.c,%.o, $(patsubst %.cpp,%.o, ${F_SRCS}))
F_OBJS              += ${F_OBJS_PRECOMPILED} ${F_OBJS_COMPILED}
CPPFLAGS       		+= $(addprefix -I, ${D_ALL_SUBDIRS} ${INC_DIRS})
F_INSTALL_LIBS		+= $(addprefix ${NETAPP_OUTPUT_LIB_DIR}/, ${F_PUBLIC_LIBS})
F_INSTALLED_INCS	+= $(addprefix ${NETAPP_OUTPUT_INC_DIR}/, $(notdir ${F_PUBLIC_INCS}))

WIFI_DRIVER_INS		+= $(addprefix ${NETAPP_OUTPUT_MODULES_DIR}/, ${WIFI_MODULES}) 
WIFI_FW_DIR			= ${NETAPP_OUTPUT_FW_DIR}/brcm


WIFI_MODULES_OPTS	=  \
	SHARED_LIB=1 \
	EXTERNAL_OPENSSL=${EXTERNAL_OPENSSL} \
	EXTERNAL_OPENSSL_BASE=${EXTERNAL_OPENSSL_BASE} \
	EXTERNAL_LIBS="${EXTERNAL_LIBS}" \
	BLDTYPE=${BLDTYPE}\
	TARGETARCH=${TARGETARCH} \
	TARGETENV=${TARGETENV} \
	TARGETOS=unix\
	CC=${CC} \
	LD=${LD} \
	LIBUSB_PATH=${NETAPP_OUTPUT_DIR} \
	STRIP=${STRIP} \
	EXTERNAL_OPENSSL_INC=${EXTERNAL_OPENSSL_BASE}/include \
	LIBDIR=${OUPUT_DIR} \
	WCN_NET_SUPPORT=y\
	WIFI_DRIVER=${WIFI_DRIVER}

############################################################################
#                              WIFI_CHIPSET Builds
############################################################################
ifeq (${WIFI_CHIPSET}, broadcom)
#WiFi Direct (P2P) Library
${BCMP2PSIG}:

${BCMP2P}:
	@echo "============ Building $@"
	${Q_}make ${MAKE_J_ARG} -C ${P2PLIB_DIR}/linux  BLDTYPE=debug \
		CFLAGS="-Wall -Wnested-externs -DWPS_WIRELESS_ENROLLEE -MD -Wformat -Wformat-security \
		-Wformat-nonliteral -DP2PAPI_ENABLE_DEBUG_SHOWKEY=1 -DP2PAPI_ENABLE_WPS=1 -DP2PAPI_ENABLE_MULTI_CHANNEL=1 \
		-DP2PAPI_ENABLE_DHCPD=0 -DP2PAPI_ENABLE_SERVICE_DISCOVERY=1 -DP2PAPI_USE_IDAUTH=1 -DP2PAPI_USE_IDSUP=1 -g \
		-D_TUDEBUGTRACE -DBCMP2P -DWLP2P -DWIFI_ACT_FRAME -DWPSCLI_NO_WPS_LOOP -fPIC -I${WIFI_DRIVER_PKG}/src/shared/bcmwifi/include" \
		CC=${CC} TARGETARCH=${TARGETARCH} BCMSTB=0 OBJDIR=obj/$(TARGETARCH)-debug-intsec ${SQUASH_OUTPUT}
	${Q_}install -m 0755 ${P2PLIB_DIR}/linux/obj/${TARGETARCH}-debug-intsec/lib${BCMP2PSIG}.a  ${NETAPP_OUTPUT_LIB_DIR}
	${Q_}install -m 0755 ${P2PLIB_DIR}/linux/obj/${TARGETARCH}-debug-intsec/lib${BCMP2P}.a  ${NETAPP_OUTPUT_LIB_DIR}
	
	
#WPS Test Library
${WPSAPI_TESTER}:
	@echo "============ Building $@"
	${Q_}make -C ${WPSAPI_DIR}/linux BLDTYPE=${BLDTYPE} TARGETARCH=${TARGETARCH} TARGETENV=${TARGETENV} TARGETOS=unix CC=${CC} LD=${LD} \
	STRIP=${STRIP} WCN_NET_SUPPORT=y SRCBASE=${WIFI_DRIVER_PKG}/src ${SQUASH_OUTPUT}
ifeq (,$(filter kirin%, ${WIFI_DRIVER_VERSION}))
	${Q_}install -m 0755 ${WPSAPI_DIR}/linux/${BLDTYPE}/${TARGETARCH}/${WPSAPI_TESTER} ${NETAPP_OUTPUT_BIN_DIR}
	${Q_}install -m 0755 ${WPSAPI_DIR}/linux/${BLDTYPE}/${TARGETARCH}/wpsenr ${NETAPP_OUTPUT_BIN_DIR}
	${Q_}install -m 0755 ${WPSAPI_DIR}/linux/${BLDTYPE}/${TARGETARCH}/wpsreg ${NETAPP_OUTPUT_BIN_DIR}
else
	${Q_}install -m 0755 ${WPSAPI_DIR}/linux/${CC}/${WPSAPI_TESTER} ${NETAPP_OUTPUT_BIN_DIR}
	${Q_}install -m 0755 ${WPSAPI_DIR}/linux/${CC}/wpsenr ${NETAPP_OUTPUT_BIN_DIR}
	${Q_}install -m 0755 ${WPSAPI_DIR}/linux/${CC}/wpsreg ${NETAPP_OUTPUT_BIN_DIR}
endif

${WPSENR_APP}:
	${Q_}cd ${WPSENR_DIR} && \
		make CC=${CC} STRIP=${STRIP} OUPUT_DIR=${OUPUT_DIR} BLDTYPE=${BLDTYPE} \
		EXTERNAL_OPENSSL=${EXTERNAL_OPENSSL} V=1 SRCBASE=${WIFI_DRIVER_PKG}/src -f wps_enr_app.mk ${SQUASH_OUTPUT}
	${Q_}install -m 0755 ${WPSENR_DIR}/${CC}/wpsenr ${NETAPP_OUTPUT_BIN_DIR}
	${Q_}install -m 0755 ${WPSENR_DIR}/${CC}/wpsreg ${NETAPP_OUTPUT_BIN_DIR}
	${Q_}install -m 0755 ${WPSENR_DIR}/${CC}/wpsapitester ${NETAPP_OUTPUT_BIN_DIR}
	
#WiFi Invite Library
${WFI}:
	@echo "============ Building $@"
	${Q_}make ${MAKE_J_ARG} -C ${WFI_DIR}/wfi_api ${WIFI_MODULES_OPTS} SRCBASE=${WIFI_DRIVER_PKG}/src outdir ${SQUASH_OUTPUT}
	${Q_}make ${MAKE_J_ARG} -C ${WFI_DIR}/wfi_api ${WIFI_MODULES_OPTS} SRCBASE=${WIFI_DRIVER_PKG}/src wfiapi ${SQUASH_OUTPUT}
	${Q_}install -m 0755 ${WFI_DIR}/wfi_api/obj/${TARGETARCH}-${BLDTYPE}/lib${WFI}.so  ${NETAPP_OUTPUT_LIB_DIR}
	

#WPSCLI Library
${WPSCLI}:
	@echo "============ Building $@"
	${Q_}make ${MAKE_J_ARG} -C ${WPSCLI_DIR}/linux ${WIFI_MODULES_OPTS} NO_WPS_THREAD=1 SRCBASE=${WIFI_DRIVER_PKG}/src lib${WPSCLI}.so ${SQUASH_OUTPUT}
	${Q_}install ${WPSCLI_DIR}/linux/obj/${TARGETARCH}-${BLDTYPE}/lib${WPSCLI}.so ${NETAPP_OUTPUT_LIB_DIR}


# WPSAPI Library	
${WPSAPI}: 
	@echo "============ Building $@"
#	${Q_}make ${MAKE_J_ARG} -C ${WPSAPI_DIR}/linux ${WIFI_MODULES_OPTS} SRCBASE=${WIFI_DRIVER_PKG}/src libs ${SQUASH_OUTPUT}
	${Q_}make ${MAKE_J_ARG} -C ${WPSAPI_DIR}/linux ${WIFI_MODULES_OPTS} SRCBASE=${WIFI_DRIVER_PKG}/src ${SQUASH_OUTPUT}
	${Q_}install -m 0755 ${OUPUT_DIR}/lib${WPSAPI}.so ${NETAPP_OUTPUT_LIB_DIR}


# WPS Common Lirary
${WPSCOM}: 
	@echo "============ Building $@"
	${Q_}make -C ${WPSCOM_DIR} ${WIFI_MODULES_OPTS} CFLAGS+="-fPIC -DWCN_NET_SUPPORT" SRCBASE=${WIFI_DRIVER_PKG}/src -f wps_common_lib.mk ${SQUASH_OUTPUT}
	${Q_}install -m 0755 ${OUPUT_DIR}/lib${WPSCOM}.so ${NETAPP_OUTPUT_LIB_DIR}


# WPS Core SoftAP Library
${WPSAP}: 
	@echo "============ Building $@"
	${Q_}make ${MAKE_J_ARG} -C ${WPSCOM_DIR} ${WIFI_MODULES_OPTS} CFLAGS+="-fPIC -DWCN_NET_SUPPORT" SRCBASE=${WIFI_DRIVER_PKG}/src -f wps_ap_lib.mk ${SQUASH_OUTPUT}
	${Q_}install -m 0755 ${OUPUT_DIR}/lib${WPSAP}.so ${NETAPP_OUTPUT_LIB_DIR}


# WPS ENR (enrollee, registrar, shared, etc....) Library
${WPSENR}: 
	@echo "============ Building $@"
	${Q_}make ${MAKE_J_ARG} -C ${WPSCOM_DIR} ${WIFI_MODULES_OPTS} CFLAGS+="-fPIC -DWCN_NET_SUPPORT" SRCBASE=${WIFI_DRIVER_PKG}/src -f wps_enr_lib.mk ${SQUASH_OUTPUT}
	${Q_}install -m 0755 ${OUPUT_DIR}/lib${WPSENR}.so ${NETAPP_OUTPUT_LIB_DIR}
	${Q_}install -m 0755 ${OUPUT_DIR}/lib${BCMCRYPTO}.a ${NETAPP_OUTPUT_LIB_DIR}


# BWL Library	
ifeq (y, ${HUMAX_PLATFORM_BASE})
${BWL}: ${EPIVERS}
	@echo "============ Building $@"
	${Q_}make ${MAKE_J_ARG} -C ${BWL_DIR} ${WIFI_MODULES_OPTS} SRCBASE=${WIFI_DRIVER_PATH}/src lib$(BWL).so
	${Q_}cp ${BWL_DIR}/lib${BWL}.so ${NETAPP_OUTPUT_LIB_DIR}
else
${BWL}: ${EPIVERS}
	@echo "============ Building $@"
	${Q_}make ${MAKE_J_ARG} -C ${BWL_DIR} ${WIFI_MODULES_OPTS} SRCBASE=${WIFI_DRIVER_PKG}/src lib$(BWL).so ${SQUASH_OUTPUT}
	${Q_}cp ${BWL_DIR}/lib${BWL}.so ${NETAPP_OUTPUT_LIB_DIR}
endif

wl: ${WL}
dhd: ${DHD}
 
${DHD}:
	@echo "============ Building $@"
	${Q_}make ${MAKE_J_ARG} -C ${WIFI_DRIVER_PKG}/src/dhd/exe ASD=0 CC=${CC} \
	DEBUG=1 TARGETARCH=${TARGETARCH} STRIP=${STRIP} ${SQUASH_OUTPUT}
	${Q_}cp ${WIFI_DRIVER_PKG}/src/dhd/exe/$@ ${NETAPP_OUTPUT_BIN_DIR}

${WL}:
	@echo "============ Building $@"
	${Q_}make ${MAKE_J_ARG} -C ${WIFI_DRIVER_PKG}/src/wl/exe ASD=0 CC=${CC} \
	DEBUG=1 TARGETARCH=${TARGETARCH} STRIP=${STRIP} ${SQUASH_OUTPUT}
	${Q_}cp ${WIFI_DRIVER_PKG}/src/wl/exe/$@ ${NETAPP_OUTPUT_BIN_DIR}

bcmp2papp:
	@echo "============ Building $@"
	${Q_}make -C ${P2PLIB_DIR}/linux/sampleapp ASD=0 CC=${CC} \
	DEBUG=1 TARGETARCH=${TARGETARCH} STRIP=${STRIP} ${SQUASH_OUTPUT}
	${Q_}cp ${P2PLIB_DIR}/linux/sampleapp/obj/${TARGETARCH}-debug-intsec/$@ ${NETAPP_OUTPUT_BIN_DIR}

${EPIVERS}:
	${Q_}make -C ${EPIVERS_DIR} ${SQUASH_OUTPUT}


# Kernel Mode Driver Build	
ifdef LINUX
##################################
#  KIRIN  #
##################################
ifneq (,$(filter kirin%, ${WIFI_DRIVER_VERSION}))
WIFI_TARGET	= mips-apdef-stadef-high-dnglimage-media-p2p
drivers: ${WIFI_DRIVER_PKG}
	@echo "============ Building ${WIFI_DRIVER_VERSION} Driver(s) for ${LINUXVER}"
	${Q_}make -C ${WIFI_DRIVER_PKG}/src/wl/linux ${WIFI_TARGET} \
	CC=${CC} LD=${LD} STRIP=${STRIP} \
	LINUXDIR=${LINUX} \
	TARGETARCH=mips LINUXVER=${LINUXVER} BRAND=linux-external-wl WLLXIW=0 \
	STBLINUX=1 BCM_DNGL_EMBEDIMAGE=1 \
	FIRMWARE="4322-bmac/roml-ag-nodis-media-p2p-mchan 43236b0-bmac/ag-nodis-media-p2p-mchan-pool" \
	NVRAMSTUBS=1 USBSHIM=1 WET=1\
	ARCH=${ARCH} \
	BRCM_WLAN_IFNAME=wln%d RPC_RETURN_WAIT_TIMEOUT_MSEC=30000 WL_DNGL_WD=0 ${SQUASH_OUTPUT}
	${Q_}cp ${WIFI_DRIVER_PKG}/src/wl/linux/obj-${WIFI_TARGET}-${LINUXVER}/*.ko ${NETAPP_OUTPUT_MODULES_DIR}
endif


##################################
# FALCON  falcon_rel_5_90_188.xx #
##################################
ifneq (,$(filter falcon_rel_5_90_188%, ${WIFI_DRIVER_VERSION}))

#Production Driver
ifneq (${WIFI_MFG_DRIVER},y)
ifeq ($(findstring 43236,$(WIFI_CHIP)),)
DNGL_IMAGE_NAME="${WIFI_CHIP}-roml/ag-p2p-apsta-idsup-af-idauth-wowlpf-fdaggr-wet"
else
DNGL_IMAGE_NAME="${WIFI_CHIP}-roml/ag-idsup-af-fdaggr-wet"
endif

# Manufacturing Driver
else
DNGL_IMAGE_NAME="${WIFI_CHIP}-roml/ag-mfgtest-seqcmds"
endif

WIFI_TARGET	= dhd-cdc-usb-reqfw-fdaggr-gpl
ifeq (${CONFIG_CPU_BIG_ENDIAN},y)
WIFI_TARGET := ${WIFI_TARGET}-be
endif

drivers: ${WIFI_FW_DIR} ${EPIVERS}
	@echo "============ Building ${WIFI_DRIVER_VERSION} Driver(s) for ${LINUXVER}"
	${Q_}make -C ${WIFI_DRIVER_PKG}/src/dhd/linux ${WIFI_TARGET} \
	LINUXDIR=${LINUX} TARGETARCH=mips WET=1 WLTEST=${WLTEST} \
	CROSS_COMPILE=${CROSS_COMPILE} \
	BRCM_WLAN_IFNAME=wln%d \
	DNGL_IMAGE_NAME=${DNGL_IMAGE_NAME} \
	ARCH=${ARCH} \
	LINUXVER=${LINUXVER} USBSHIM=1 WL_NVRAM_FILE=1 CC=${CC} STRIP=${STRIP} ${SQUASH_OUTPUT}
		${Q_}make -C ${WIFI_DRIVER_PKG}/src/usbdev/usbdl ${WIFI_MODULES_OPTS} ${SQUASH_OUTPUT}
	${Q_}if [ -e ${WIFI_DRIVER_PKG}/src/usbdev/usbdl/obj/${TARGETARCH}/bcmdl ] ; then \
		cp ${WIFI_DRIVER_PKG}/src/usbdev/usbdl/obj/${TARGETARCH}/bcmdl ${NETAPP_OUTPUT_BIN_DIR}; \
	fi
	${Q_}if [ -e ${WIFI_DRIVER_PKG}/src/usbdev/usbdl/bcmdl ] ; then \
		cp ${WIFI_DRIVER_PKG}/src/usbdev/usbdl/bcmdl ${NETAPP_OUTPUT_BIN_DIR}; \
	fi
	${Q_}if [ -d ${WIFI_DRIVER_PKG}/src/dhd/linux/${WIFI_TARGET}-${LINUXVER} ] ; then \
		cp ${WIFI_DRIVER_PKG}/src/dhd/linux/${WIFI_TARGET}-${LINUXVER}/*.ko ${NETAPP_OUTPUT_MODULES_DIR}; \
	fi
	${Q_}if [ -d ${WIFI_DRIVER_PKG}/src/dhd/linux/${WIFI_TARGET}-${KERNELVERSION} ] ; then \
		cp ${WIFI_DRIVER_PKG}/src/dhd/linux/${WIFI_TARGET}-${KERNELVERSION}/*.ko ${NETAPP_OUTPUT_MODULES_DIR}; \
	fi
	${Q_}cp ${WIFI_DRIVER_PKG}/firmware/${DNGL_IMAGE_NAME}/rtecdc.bin.trx ${WIFI_FW_DIR}/bcm4323x-firmware.bin
	${Q_}cp ${WIFI_DRIVER_PKG}/nvrams/fake43236usb_p532.nvm ${WIFI_FW_DIR}/bcm4323x.nvm
endif


##################################
# FALCON  5_90_195_xx            #
##################################
ifneq (,$(filter 5_90_195%, ${WIFI_DRIVER_VERSION}))
WIFI_TARGET 	:= dhd-cdc-sdmmc-gpl
DNGL_IMAGE_NAME	:="43362a2-roml"
drivers: ${WIFI_FW_DIR} ${EPIVERS} ${DHD}
	@echo "============ Building ${WIFI_DRIVER_VERSION} Driver(s) for ${LINUXVER}"
	${Q_}make -C ${WIFI_DRIVER_PKG}/src/dhd/linux ${WIFI_TARGET} \
	LINUXDIR=${LINUX} TARGETARCH=mips \
	CROSS_COMPILE=${CROSS_COMPILE} \
	LINUXVER=${LINUXVER} CC=${CC} STRIP=${STRIP} ${SQUASH_OUTPUT}
	${Q_}cp ${WIFI_DRIVER_PKG}/src/dhd/linux/${WIFI_TARGET}-${LINUXVER}/*.ko ${NETAPP_OUTPUT_MODULES_DIR}
	${Q_}cp ${WIFI_DRIVER_PKG}/firmware/${DNGL_IMAGE_NAME}/sdio-apsta-idsup-idauth-pno.bin ${NETAPP_OUTPUT_BIN_DIR}/rtecdc.bin
	${Q_}cp ${WIFI_DRIVER_PKG}/firmware/${DNGL_IMAGE_NAME}/bcm943362sdg.txt ${NETAPP_OUTPUT_BIN_DIR}
endif


##################################
# AARDVARK (MEDIA) #
##################################
ifneq (,$(filter aardvark%, ${WIFI_DRIVER_VERSION}))
WIFI_TARGET	= mips-apdef-stadef-high-dnglimage-media-p2p
drivers: ${WIFI_DRIVER_PKG} ${WL}
	@echo "============ Building ${WIFI_DRIVER_VERSION} Driver(s) for ${LINUXVER}"
	${Q_}make -C ${WIFI_DRIVER_PKG}/src/wl/linux ${WIFI_TARGET} \
	CC=${CC} LD=${LD} STRIP=${STRIP} \
	LINUXDIR=${LINUX} \
	TARGETARCH=mips LINUXVER=${LINUXVER} BRAND=linux-external-wl WLLXIW=0 \
	STBLINUX=1 BCM_DNGL_EMBEDIMAGE=1 \
	FIRMWARE="44322-bmac/ag-nodis-p2p-media 43236b-bmac/ag-nodis-p2p-mchan-media 4360a0-bmac/ag-nodis-p2p-mchan-media" \
	NVRAMSTUBS=1 USBSHIM=1 WET=1 \
	ARCH=${ARCH} \
	BRCM_WLAN_IFNAME=wln%d RPC_RETURN_WAIT_TIMEOUT_MSEC=30000 WL_DNGL_WD=0 ${SQUASH_OUTPUT}
	${Q_}cp ${WIFI_DRIVER_PKG}/src/wl/linux/obj-${WIFI_TARGET}-${LINUXVER}/*.ko ${NETAPP_OUTPUT_MODULES_DIR}
endif

endif

${WIFI_FW_DIR}:
	${Q_}mkdir -p ${WIFI_FW_DIR}

${OUPUT_DIR}:
	${Q_}mkdir -p ${OUPUT_DIR}/bcmcrypto
	${Q_}mkdir -p ${OUPUT_DIR}/sta
	${Q_}mkdir -p ${OUPUT_DIR}/enrollee
	${Q_}mkdir -p ${OUPUT_DIR}/registrar
	${Q_}mkdir -p ${OUPUT_DIR}/shared
	${Q_}mkdir -p ${OUPUT_DIR}/ap
	
endif # Broadcom Chipset
############################################################################
release-clean: clean
	${Q_}rm -rf ${WIFI_DRIVER_PKG} ${WIFI_DRIVER_PKG}

clean:
ifneq ($(wildcard ${WIFI_DRIVER_PKG}), )
	${Q_}if [ -e ${P2PLIB_DIR}/linux/sampleapp ] ; then \
		make -C ${P2PLIB_DIR}/linux/sampleapp ASD=0 CC=${CC} \
			DEBUG=1 TARGETARCH=${TARGETARCH} STRIP=${STRIP} clean ${SQUASH_OUTPUT};\
	fi
	${Q_}if [ -e ${WIFI_DRIVER_PKG}/src/wl/linux ] ; then \
		make -C ${WIFI_DRIVER_PKG}/src/wl/linux TARGETARCH=${TARGETARCH} SRCBASE=${WIFI_DRIVER_PKG}/src clean ${SQUASH_OUTPUT}; \
	fi
	${Q_}if [ -e ${WIFI_DRIVER_PKG}/src/dhd/linux ] ; then \
		make -C ${WIFI_DRIVER_PKG}/src/dhd/linux TARGETARCH=${TARGETARCH} SRCBASE=${WIFI_DRIVER_PKG}/src clean ${SQUASH_OUTPUT}; \
	fi
	${Q_}if [ -e ${WIFI_DRIVER_PKG}/src/dhd/exe ] ; then \
		make -C ${WIFI_DRIVER_PKG}/src/dhd/exe TARGETARCH=${TARGETARCH} clean ${SQUASH_OUTPUT}; \
	fi
	${Q_}if [ -e ${WIFI_DRIVER_PKG}/src/wl/exe ] ; then \
		make -C ${WIFI_DRIVER_PKG}/src/wl/exe TARGETARCH=${TARGETARCH} SRCBASE=${WIFI_DRIVER_PKG}/src clean ${SQUASH_OUTPUT}; \
	fi
	${Q_}rm -rf *.marker
	${Q_}-make -C ${WPSCLI_DIR}/linux ${WIFI_MODULES_OPTS} SRCBASE=${WIFI_DRIVER_PKG}/src clean ${SQUASH_OUTPUT}
	${Q_}-make -C ${WFI_DIR}/wfi_api ${WIFI_MODULES_OPTS} SRCBASE=${WIFI_DRIVER_PKG}/src clean ${SQUASH_OUTPUT}
	${Q_}-make -C ${BWL_DIR} ${WIFI_MODULES_OPTS} SRCBASE=${WIFI_DRIVER_PKG}/src clean ${SQUASH_OUTPUT}
	${Q_}if [ -e ${WPSENR_DIR} ] ; then \
		cd ${WPSENR_DIR} && make CC=${CC} STRIP=${STRIP} OUPUT_DIR=${OUPUT_DIR} EXTERNAL_OPENSSL=${EXTERNAL_OPENSSL} V=1 SRCBASE=${WIFI_DRIVER_PKG}/src -f wps_enr_app.mk ${SQUASH_OUTPUT} clean;\
	fi
	${Q_}rm -f ${F_OBJS_COMPILED} *.d
	${Q_}rm -rf ${OUPUT_DIR}
endif


distclean: clean release-clean
	${Q_}rm -rf ${WIFI_DRIVER_INS} ${F_INSTALL_LIBS} ${F_PUBLIC_LIBS} ${F_INSTALLED_INCS} 
	

vpath % ${INC_DIRS}

$(WIFI_DRIVER_PKG):
	@echo "============ Extracting tarball(s)" ;
ifneq ($(findstring bz, ${WIFI_SRC_PKG}),)
	${Q_}echo "WIFI_SRC_PKG=${WIFI_SRC_PKG}"
	${Q_}cd ${WIFI_CHIPSET}; tar jxf ${WIFI_SRC_PKG}
else
	${Q_}cd ${WIFI_CHIPSET}; tar zxf ${WIFI_SRC_PKG}
endif

	${Q_}if [ -f ${WIFI_DRIVER_PATCH} ]; then \
		cd $(WIFI_DRIVER_PKG) && patch -p1 < ${WIFI_DRIVER_PATCH} ${SQUASH_OUTPUT}; \
	fi	
	
debug:
	@echo "WIFI_SRC_PKG=${WIFI_SRC_PKG} HAS_WIFI_BUILT=${HAS_WIFI_BUILT}"
	@echo "SRCBASE=${WIFI_DRIVER_PKG}/src"
	@echo "WIFI_MODULES_OPTS=${WIFI_MODULES_OPTS}"
	@echo "F_INSTALL_LIBS=${F_INSTALL_LIBS}"
	@echo "F_PUBLIC_LIBS=${F_PUBLIC_LIBS}"

ifeq (y, ${HUMAX_PLATFORM_BASE})
install: ${OUPUT_DIR} ${F_PUBLIC_LIBS}
	${Q_}make header-files
else
install: ${WIFI_DRIVER_PKG} ${OUPUT_DIR} drivers ${WL} ${F_PUBLIC_LIBS}
	${Q_}make header-files ${SQUASH_OUTPUT}
endif

header-files: ${F_INSTALLED_INCS}

examples: ${WIFI_EXAMPLES}

${F_INSTALLED_INCS}: ${NETAPP_OUTPUT_INC_DIR}/%.h : %.h
	${Q_}install  -m 0664 $< $@	
	
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##


vpath %.h ${INC_DIRS}

############################################################################
#                             DEP FILES, PHONY
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
-include *.d
.PHONY: ${NETAPP_SHARED_LIBS} ${WPSENR_APP} all drivers unpack build distclean clean install
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##


