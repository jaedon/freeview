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
# $brcm_Revision: 10 $
# $brcm_Date: 2/3/12 4:18p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /NetApp/netapp/Makefile $
# 
# 10   2/3/12 4:18p steven
# SW7231-638: NetApp Version 6.2
# 
# 15   1/7/12 7:07p steven
# SW7429-38: NetApp Enhancements for CES
# 
# 14   12/13/11 11:51a steven
# SW7231-513: Removed the check marker so we always check preconditions
# 
# 13   12/9/11 4:44p steven
# SW7231-513: USB Hotplug Support in NetApp
# 
# SW7231-513/1   12/9/11 3:36p steven
# SW7231-513: Finished working hotplug detection
# 
# 11   12/2/11 10:40p steven
# SW7425-1404: Fix building without bluetooth
# 
# 10   12/1/11 8:56p steven
# SW7425-1404: Moved kernel modules and fixes to BT support
# 
# 9   11/24/11 5:13p steven
# SW7425-1404: Bluetooh, WLAN Driver, and Multi-Wired Interface Support
# 
# SW7425-1404/7   11/24/11 4:02p steven
# SW7425-1404: Bluetooth, WLAN driver build, and multi-interface support
# 
# SW7425-1404/2   11/4/11 10:14a steven
# SW7425-1404: Backup of BT work
# 
# 8   10/4/11 3:01p steven
# SW7425-1399: Added support in the makefile to load prebuilt by default
# 
# 7   8/19/11 12:48p steven
# SWBLURAY-26272: WoWL Integration
# 
# SWBLURAY-26272/1   8/18/11 9:21p steven
# SWBLURAY-26272: WoWL Integration
# 
# 6   7/28/11 2:14p steven
# SW7425-749: Initial WoWL and P2P development checkin
# 
# 5   7/7/11 10:56p steven
# SWBLURAY-26474: ndvd SRC package build fails
# 
# 4   6/16/11 10:57a steven
# SW7425-714: Fix for WPS and WiFi Invite and NDVD build
# 
# 3   6/15/11 5:37p steven
# SW7425-714: Disable driver build for now
# 
# 2   6/15/11 5:26p steven
# SW7425-714: Adding NetApp support to AppLibs
# 
# 1   6/14/11 10:40a steven
# SW7425-714: Moved NetApp to AppLibs
#
############################################################################
#
# DESCRIPTION:
#   Makefile for building netapp library.
#
############################################################################
NETAPP_TOP = ${shell pwd}/..
include ${NETAPP_TOP}/build/common.inc
include modules.inc


################################################################################
.DEFAULT: install

CPPFLAGS				+= -fPIC
EXTERNAL_OPENSSL 		?=0
ifdef EXTERNAL_CPPFLAGS
CPPFLAGS				+= ${EXTERNAL_CPPFLAGS}
endif
EXTERNAL_OPENSSL_BASE 	?= ${NETAPP_OUTPUT_DIR}/usr/local
LIB_BASENAME        	:= $(notdir $(shell pwd))
F_PUBLIC_INCS    		:= include/netapp.h

LDFLAGS					+= -L${NETAPP_OUTPUT_LIB_DIR} -L.
SRC_DIRS				:= . src
INC_DIRS 				+= . src include
# force disable for now
HAS_AVAHI				:=$(wildcard ${NETAPP_OUTPUT_LIB_DIR}/libavahi-core.so)
HAS_OSAPI				:=$(wildcard ${NETAPP_OUTPUT_LIB_DIR}/libosapi.so)
HAS_LIBUDEV				:=$(wildcard ${NETAPP_OUTPUT_LIB_DIR}/libudev.so)

ifeq (${DATABASE},n)
CPPFLAGS				+= -DNO_DATABASE
EXCLUDED_SRCS			+= netapp_db_sqlite.c
else
SHARED_LIBS				+= sqlite3
endif

ifeq (${DATABASE},n)
ifeq (${VOICE_RECOGNITION},n)
NO_JSON:=y
else
endif
endif

ifneq (${NO_JSON},y)
CPPFLAGS				+= -I${NETAPP_OUTPUT_INC_DIR}/json
SHARED_LIBS				+= json
endif

ifneq (${VOICE_RECOGNITION},n)
else
CPPFLAGS				+=-DNO_VOICE_RECOGNITION
EXCLUDED_SRCS			+= netapp_http.c
endif

# Option to disabled Automatic bring up of Wired interface 
ifdef DISABLE_WIRED_SUPPORT
CFLAGS					+=-DDISABLE_WIRED_SUPPORT
endif

################################################################################
# 							W I F I  	S U P O R T 
#Enable WiFi if the WiFi source code is available
################################################################################
ifneq (, ${HAS_WIFI})
CPPFLAGS				+= -DWIFI_DRIVER_VERSION="\"${WIFI_DRIVER_VERSION}\""
CPPFLAGS 				+= -DWIFI_MODULES="\"${WIFI_MODULES}\""
CPPFLAGS 				+= -DWIFI_MODULES_INSTALLED="\"${WIFI_MODULES_INSTALLED}\""

ifeq (${WIFI_WPS},n)
CPPFLAGS			+= -DNO_WIFI_WPS
EXCLUDED_SRCS		+= netapp_wps.c
endif

#Disable WiFi Invite by default
ifeq (${WIFI_INVITE},n)
CPPFLAGS			+=-DNO_WIFI_INVITE
EXCLUDED_SRCS		+= netapp_wfi.c
endif

ifneq (${WIFI_P2P},n)
STATIC_LDFLAGS		+= -l${BCMP2P} -l${BCMP2PSIG}
else
CPPFLAGS 			+= -DNO_WIFI_P2P
EXCLUDED_SRCS		+= netapp_p2p.c
endif

# Taken from modules.inc
SHARED_LIBS 		+= ${NETAPP_SHARED_LIBS}
STATIC_LDFLAGS 		+= ${NETAPP_STATIC_LDFLAGS}

ifeq (${WIFI_TEST},n)
CPPFLAGS                        += -DNO_WIFI_TEST
endif

ifeq (${EXTERNAL_OPENSSL},0)
EXTRA_LDFLAGS +=${NETAPP_OUTPUT_LIB_DIR}/libbcmcrypto.a
endif

ifneq (, ${WIFI_SRC_PKG})

ifeq (y, ${HUMAX_PLATFORM_BASE})
NETAPP_WLAN_PARAMS += HUMAX_PLATFORM_BASE=y
NETAPP_WLAN_PARAMS += WIFI_DRIVER_VERSION=$(WIFI_DRIVER_VERSION)
NETAPP_WLAN_PARAMS += WIFI_DRIVER_PATH=$(WIFI_DRIVER_PATH)
wlan:
	@echo "======== Building WiFi "
	make -C wlan $(NETAPP_WLAN_PARAMS) install 
else
wlan:
	@echo "======== Building WiFi "
	${Q_}make -C wlan install
endif
else
wlan:
endif

# No WiFi
else
CPPFLAGS 				+= -DNO_WIFI -DNO_WIFI_INVITE -DNO_WIFI_P2P -DNO_WIFI_WPS -DNO_WIFI_TEST
EXCLUDED_SRCS	+= 		\
	netapp_wifi.c		\
	netapp_wps.c		\
	netapp_wfi.c		\
	netapp_p2p.c	
wlan:

endif # NO_WIFI
###############################################################################

################################################################################
# 					B L U E T O O T H   	S U P O R T 
#Enable Bluetooth if the Bluetooth source code is available
################################################################################
ifneq (, ${HAS_BLUETOOTH})
BSA_DIR				= ${NETAPP_OUTPUT_INC_DIR}/${BT_PKG_VERSION}/3rdparty/embedded/bsa_examples/linux/libbsa/include
BTHID_DIR			= ${NETAPP_OUTPUT_INC_DIR}/${BT_PKG_VERSION}/3rdparty/embedded/brcm/linux/bthid
CFLAGS				+= -I${BSA_DIR} -I${BTHID_DIR}
SHARED_LIBS			+= bsa
BT_MODULES			+= btusb bthid
BT_MODULES_INSTALLED = $(addsuffix .ko, $(addprefix ${INST_MODULES_DIR}/, ${BT_MODULES}))
CPPFLAGS 			+= -DBT_MODULES="\"${BT_MODULES}\""
CPPFLAGS 			+= -DBT_MODULES_INSTALLED="\"${BT_MODULES_INSTALLED}\""

# enable auto pair by default for now, will remove with proximity or legacy pairing later
#CPPFLAGS			+= -DBT_HID_AUTO_PAIR

ifneq (, ${HAS_BLUETOOTH_SRC})
bluetooth:
	@echo "======== Building Bluetooth"
	${Q_}make -C bluetooth install
else
bluetooth:
endif

# No Bluetooth
else
EXCLUDED_SRCS	+= 	\
	netapp_bt.c		\
	netapp_bt_hid.c         \
	netapp_bt_av.c

CFLAGS			+= -DNO_BLUETOOTH
bluetooth:
	
endif # NO_BLUETOOTH
###############################################################################

	
###############################################################################
#							A V A H I
###############################################################################
ifneq (, ${HAS_AVAHI})
SHARED_LIBS += avahi-common avahi-core
else
CPPFLAGS		+= -DNO_AVAHI
EXCLUDED_SRCS 	+= netapp_zeroconf.c
endif
################################################################################

###############################################################################
#							L I B U D E V
# USB Hotplug support
###############################################################################
ifneq (, ${HAS_LIBUDEV})
SHARED_LIBS += udev
else
CPPFLAGS		+= -DNO_LIBUDEV
EXCLUDED_SRCS 	+= netapp_udev.c
endif
################################################################################

############################################################################
# These variable settings are all dependent on the above values.
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
SHARED_LINKER_STR 	= -Wl,-L$(NETAPP_OUTPUT_LIB_DIR),-dy,-l
SHARED_LDFLAGS		+= $(addprefix ${SHARED_LINKER_STR}, ${SHARED_LIBS})
STATIC_LDFLAGS		+= -ldhcpd -lpthread -lrt
F_PUBLIC_LIBS    	+= lib${LIB_BASENAME}.so lib${LIB_BASENAME}.a
F_PUBLIC_MAKE		= ${LIB_BASENAME}.inc

F_SRCS = $(filter-out \
           $(addprefix %/, ${EXCLUDED_SRCS}), \
               $(foreach d,${SRC_DIRS},$(wildcard ${d}/*.c ${d}/*.cpp)))

F_OBJS_COMPILED 	+= $(patsubst %.c,%.o, $(patsubst %.cpp,%.o, ${F_SRCS}))
F_OBJS              += ${F_OBJS_PRECOMPILED} ${F_OBJS_COMPILED}
CPPFLAGS       		+= $(addprefix -I, ${INC_DIRS})
F_INSTALLED_LIBS	+= $(addprefix ${NETAPP_OUTPUT_LIB_DIR}/, $(notdir ${F_PUBLIC_LIBS}))
F_INSTALLED_INCS	+= $(addprefix ${NETAPP_OUTPUT_INC_DIR}/, $(notdir ${F_PUBLIC_INCS}))
F_INSTALLED_MAKE	= ${NETAPP_OUTPUT_INC_DIR}/${F_PUBLIC_MAKE}
CPPFLAGS       		+= -I${NETAPP_OUTPUT_INC_DIR}

#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##
#
# DHCP Server Daemon
#
dhcpd:
	${Q_}make -C dhcpd install

clean-dhcpd:	
	${Q_}make -C dhcpd distclean
	

############################################################################
#                              MAIN TARGETS
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
release: wlan bluetooth
	${Q_}make -C wlan release-clean
	${Q_}make -C bluetooth release-clean

clean-netapp:
	@echo "======== Cleaning NetApp"
	${Q_}find -name '*.d' | xargs rm -f
	${Q_}rm -f ${F_OBJS_COMPILED}
	${Q_}rm -f ${F_PUBLIC_LIBS}
	${Q_}rm -rf latex html

clean: clean-netapp clean-dhcpd
ifneq (, ${WIFI_SRC_PKG})
	@echo "======== Cleaning WiFi"
	${Q_}-make -C wlan clean WIFI_CHIPSET=${WIFI_CHIPSET}
endif
ifneq (, ${HAS_BLUETOOTH_SRC})
	@echo "======== Cleaning Bluetooth"
	${Q_}-make -C bluetooth clean
endif

distclean: clean-netapp clean-examples clean-dhcpd
ifneq (, ${WIFI_SRC_PKG})
	@echo "======== Distclean WiFi"
	${Q_}-make -C wlan distclean WIFI_CHIPSET=${WIFI_CHIPSET}
endif
ifneq (, ${HAS_BLUETOOTH_SRC})
	@echo "======== Distclean Bluetooth"
	${Q_}-make -C bluetooth distclean
endif	
	@echo "======== Distclean NetApp"
	${Q_}rm -f ${F_INSTALLED_LIBS} ${F_INSTALLED_MAKE}

#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##



############################################################################
#                             BUILD RULES
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
lib${LIB_BASENAME}.so: ${F_OBJS}
	@printf "[${LD}] -------------> %s \n" $@
	${Q_}${CC} -shared -o $@  ${LDFLAGS} ${SHARED_LDFLAGS} ${STATIC_LDFLAGS} $^

lib${LIB_BASENAME}.a: ${F_OBJS} 
	@printf "[${LD}] -------------> %s \n" $@
	${Q_}${LD} -r -o $@ $^

check:
ifeq (, ${HAS_LIBUDEV})
	@echo "******************* W A R N I N G ****************************"
	@echo "libudev Was not built. udev is needed for USB hotplug support"
	@echo "******************* W A R N I N G ****************************"
endif
ifeq (, ${HAS_AVAHI})
	@echo "******************* W A R N I N G ****************************"
	@echo "Avahi Was not built. If you Want Bonjour/Zeroconf support you"
	@echo "need to build Avahi before NetApp. "
	@echo "******************* W A R N I N G ****************************"
endif
ifeq (, ${HAS_WIFI})
	@echo "******************* W A R N I N G ****************************"
	@echo "WiFi Support disabled"
	@echo "******************* W A R N I N G ****************************"
endif

ifeq (, ${HAS_BLUETOOTH})
	@echo "******************* W A R N I N G ****************************"
	@echo "Bluetooth Support is disabled because BSA code was not included or"
	@echo "the variable BLUETOOTH=n was set."
	@echo "******************* W A R N I N G ****************************"
endif
ifndef LINUX
	@echo "******************* W A R N I N G ****************************"
	@echo "LINUX is not set. This variable must be defined and point to a" 
	@echo "valid pre-built kernel to build WiFi and/or bluetooth"
	@echo "******************* W A R N I N G ****************************"
endif

idirs: 
	@ [ -d ${NETAPP_OUTPUT_LIB_DIR} ] || mkdir -p ${NETAPP_OUTPUT_LIB_DIR}
	@ [ -d ${NETAPP_OUTPUT_INC_DIR} ] || mkdir -p ${NETAPP_OUTPUT_INC_DIR}
	@ [ -d ${NETAPP_OUTPUT_BIN_DIR} ] || mkdir -p ${NETAPP_OUTPUT_BIN_DIR}
	@ [ -d ${NETAPP_OUTPUT_MODULES_DIR} ] || mkdir -p ${NETAPP_OUTPUT_MODULES_DIR}

netapp: ${F_INSTALLED_LIBS} ${F_INSTALLED_INCS}

prebuilt:
ifneq (${WIFI},n)
ifeq (${WIFI_SRC_PKG},)
ifeq (${HAS_WIFI_BUILT},)
ifeq (${HAS_PREBUILT},)
	@echo "==== Prebuilt ${NETAPP_PREBUILT_TARBALL} Not Found!"
else
	@echo "==== Unpacking ${NETAPP_PREBUILT_TARBALL}"
	${Q_}cd ${NETAPP_OUTPUT_DIR}/../../; tar xjf ${NETAPP_PREBUILT_TARBALL}
endif
endif
endif
endif

prep: prebuilt idirs

install: prep check wlan bluetooth dhcpd netapp make-include
	@echo "==== NetApp Build Completed!"
	

examples:
	@echo "======== NetApp Examples"
ifneq (, ${HAS_BLUETOOTH_SRC})
	${Q_}make -C ${BT_DIR} examples
endif
ifneq (, ${WIFI_SRC_PKG})
	${Q_}make -C ${WIFI_DIR} examples
endif

clean-examples:
	@echo "======== Cleaning NetApp Examples"
ifneq (, ${HAS_BLUETOOTH_SRC})
	${Q_}-make -C ${BT_DIR} clean-examples
endif


LDFLAGS_OUT := ${SHARED_LINKER_STR}netapp ${SHARED_LDFLAGS} ${STATIC_LDFLAGS}

# generate netapp.inc
make-include:
	${Q_}if [ -f ${F_INSTALLED_MAKE} ] ; then rm -f ${F_INSTALLED_MAKE}; fi
	@echo "======== Generating ${F_PUBLIC_MAKE}"
	@echo "CC=${CC}" >> ${F_INSTALLED_MAKE}
	@echo "CXX=${CXX}" >> ${F_INSTALLED_MAKE}
	@echo "LD=${LD}" >> ${F_INSTALLED_MAKE}
	@echo "STRIP=${STRIP}" >> ${F_INSTALLED_MAKE}
	@echo "NETAPP_LDFLAGS+=${LDFLAGS_OUT}" >> ${F_INSTALLED_MAKE}
	@echo "NETAPP_CFLAGS+=${CPPFLAGS} ${CFLAGS}" >> ${F_INSTALLED_MAKE}

${F_INSTALLED_INCS}: ${NETAPP_OUTPUT_INC_DIR}/%.h : %.h
	${Q_}install  -m 0664 $< $@


${NETAPP_OUTPUT_LIB_DIR}/%.a : %.a
	${Q_}install -m 0755 $< $@ 

${NETAPP_OUTPUT_LIB_DIR}/%.so : %.so
	${Q_}install -m 0755 $< $@ 


############################################################################
#                             DEP FILES, PHONY
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
-include *.d
.PHONY: prep install release all dhcpd wlan bluetooth install clean distclean idirs
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##

vpath %.h ${INC_DIRS}
