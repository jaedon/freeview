############################################################################
#    (c)2006-2011 Broadcom Corporation
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
# $brcm_Workfile: $
# $brcm_Revision: $
# $brcm_Date: $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: $
# 
#
############################################################################

################################################################################
# 					B L U E T O O T H   	S U P O R T 
# Enable Bluetooth if the Bluetooth source code is available
################################################################################
ifneq (${BLUETOOTH},n)
BT_CHIPSET			= broadcom
BT_DIR				= ${NETAPP_TOP}/netapp/bluetooth
BT_CHIPSET_PKG_TYPE	= .tbz
BT_PKG_VERSION		= bsa_mi_01.00.05.00.56
BT_CHIPSET_PKG		= ${BT_DIR}/${BT_CHIPSET}/${BT_PKG_VERSION}
HAS_BLUETOOTH_BUILT	:= $(wildcard ${NETAPP_OUTPUT_LIB_DIR}/libbsa.so)
BLUETOOTH_SRC		:= ${BT_CHIPSET_PKG}${BT_CHIPSET_PKG_TYPE}
HAS_BLUETOOTH_SRC	:= $(wildcard ${BLUETOOTH_SRC})

ifneq (,${HAS_BLUETOOTH_SRC})
HAS_BLUETOOTH			= ${HAS_BLUETOOTH_SRC}
else
ifneq (,${HAS_BLUETOOTH_BUILT})
HAS_BLUETOOTH			= ${HAS_BLUETOOTH_BUILT}
endif
endif

endif


################################################################################
# 							W I F I  	S U P O R T 
# Enable WiFi if the WiFi source code is available
################################################################################
ifneq (${WIFI},n)
WIFI_INVITE     =n
WIFI_DIR        := ${NETAPP_TOP}/netapp/wlan
WIFI_DRIVER     ?= aardvark
WIFI_CHIP       ?=43236b

ifeq (${WIFI_DRIVER}, kirin)
WIFI_DRIVER_VERSION ?= kirin_rel_5_102_98_42
endif

ifeq (${WIFI_DRIVER}, falcon)
WIFI_DRIVER_VERSION ?= falcon_rel_5_90_188_59
endif

ifeq (${WIFI_DRIVER}, aardvark)
WIFI_DRIVER_VERSION ?= aardvark_rel_6_30_100_10
endif

WIFI_CHIPSET			?= broadcom
WIFI_DIR				:= ${NETAPP_TOP}/netapp/wlan
WIFI_SUPPORTED_CHIPS    := 43236b 43238b 4360a0

# DEPRECIATED
#WIFI_WPS_PKG_VERSION	:= WPS_REL_1_79_0_20110407
#WIFI_WPS_PKG			:= ${WIFI_DIR}/${WIFI_CHIPSET}/${WIFI_WPS_PKG_VERSION}

HAS_WIFI_BUILT			:= $(wildcard ${NETAPP_OUTPUT_LIB_DIR}/libbwl.so)
ifeq (${HUMAX_PLATFORM_BASE},y)
WIFI_SRC_PKG            := $(WIFI_DRIVER_PATH)
else
WIFI_SRC_PKG			:= $(wildcard ${WIFI_DIR}/${WIFI_CHIPSET}/${WIFI_DRIVER_VERSION}*.bz2)
ifeq (${WIFI_SRC_PKG},)
WIFI_SRC_PKG            := $(wildcard ${WIFI_DIR}/${WIFI_CHIPSET}/${WIFI_DRIVER_VERSION}*.tgz)
endif
endif

WIFI_DRIVER_PKG			:= $(strip ${WIFI_DIR}/${WIFI_CHIPSET}/${WIFI_DRIVER_VERSION})
WIFI_DRIVER_PATCH		=${WIFI_DRIVER_PKG}.patch

# Check if the WiFi driver is a manufacturing driver or not
ifneq ($(wildcard ${WIFI_DRIVER_PKG}_mfg.*tgz),)
WIFI_DRIVER_PKG         := $(addsuffix _mfg, ${WIFI_DRIVER_PKG})
WIFI_P2P=n
WIFI_INVITE=n
WIFI_WPS=n
WIFI_MFG_DRIVER=y
CFLAGS += -DWIFI_MFG_DRIVER
endif


ifneq (,$(filter falcon%, ${WIFI_DRIVER_VERSION}))
WIFI_MODULES            := dhd
else
WIFI_MODULES            := bcm_usbshim wl
endif

WIFI_MODULES_INSTALLED  = $(addsuffix .ko, $(addprefix ${INST_MODULES_DIR}/, ${WIFI_MODULES}))

ifneq (,${WIFI_SRC_PKG})
HAS_WIFI				:= ${WIFI_SRC_PKG}
else
ifneq (,${HAS_WIFI_BUILT})
HAS_WIFI				:= ${HAS_WIFI_BUILT}
endif
endif


BWL                     := bwl
WFI                     := wfiapi
WPSCLI                  := wpscli
WPSAPI                  := wpsapi
WPSENR                  := wpsenr
WPSCOM                  := wpscom
WPSAP                   := wpsap
BCMP2P                  := bcmp2p
BCMP2PSIG               := bcmp2psig
BCMCRYPTO               := bcmcrypto
NETAPP_SHARED_LIBS      += ${BWL}

ifneq (${WIFI_WPS}, n)
NETAPP_NEEDS_WPS    = y
NETAPP_NEEDS_WPSCLI = y
endif

ifneq (${WIFI_P2P},n)
NETAPP_NEEDS_WPS    = y
NETAPP_NEEDS_WPSCLI = y
endif

ifneq (${WIFI_INVITE},n)
NETAPP_NEEDS_WPS    = y
NETAPP_NEEDS_WPSCLI = y
endif

ifeq (${NETAPP_NEEDS_WPS}, y)
NETAPP_SHARED_LIBS       += ${WPSENR} ${WPSCOM} ${WPSAPI} ${WPSAP}
NETAPP_STATIC_LDFLAGS    += -lbcmcrypto
endif

ifeq (${NETAPP_NEEDS_WPSCLI}, y)
NETAPP_SHARED_LIBS       += ${WPSCLI}
endif

ifneq (${WIFI_P2P},n)
NETAPP_SHARED_LIBS       += ${BCMP2P} ${BCMP2PSIG}
endif

endif
