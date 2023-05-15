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
# $brcm_Workfile: common.inc $
# $brcm_Revision: 7 $
# $brcm_Date: 2/3/12 4:18p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /NetApp/build/common.inc $
# 
# 7   2/3/12 4:18p steven
# SW7231-638: NetApp Version 6.2
# 
# 6   1/30/12 3:46p steven
# SW7346-638: Adding src tarball creation for release target
# 
# 
#
############################################################################
ifndef NETAPP_TOP
$(error NETAPP_TOP not defined and needs to point to the NetApp VOB root directory)
endif

ifeq (${VERBOSE},y)
Q_					=
SQUASH_OUTPUT		=
else
Q_					=@
SQUASH_OUTPUT		= > /dev/null 2>&1
endif

###############################################################################
# 					B U I L D   E N V I R O N M E N T 
# Figure out how we're going to build NetApp (cross compile or for i386)
################################################################################
ifndef LINUX
LINUXNAME       :=`uname -r`
LINUX           :=/lib/modules/${LINUXNAME}/build
SKIP_CROSS_SET  = y
endif

KERNELVERSION           := $(shell cat ${LINUX}/include/config/kernel.release)
LINUXVER                := $(shell echo ${KERNELVERSION} | sed -e 's/-/ /' | sed 's/\s.*//')

KCONFIG_FILE 			:= ${LINUX}/.config
-include ${KCONFIG_FILE}

ifndef SKIP_CROSS_SET
ifeq (${CONFIG_CPU_BIG_ENDIAN},y)
NETAPP_ENDIAN           = be
ENDIAN_FLAGS			+= -EB
CROSS_HOST				= mips-linux
CROSS_COMPILE 			= ${CROSS_HOST}-
else
NETAPP_ENDIAN           = le
CROSS_HOST				= mipsel-linux
CROSS_COMPILE 			= ${CROSS_HOST}-
ENDIAN_FLAGS			+= -EL
endif
endif



################################################################################
#        A P P L I B S   C H E C K
# Check if we are building inside of AppLibs, if so then put the output dir in APPLIBS_TARGET_DIR
#
APPLIBS_TOP = ${NETAPP_TOP}/../..
APPLIBS_INC:=$(wildcard ${APPLIBS_TOP}/common/common.inc)
ifneq (${APPLIBS_INC},)
include ${APPLIBS_INC}
NETAPP_OUTPUT_DIR := ${APPLIBS_TARGET_DIR}
endif
################################################################################

CFLAGS					+= ${ENDIAN_FLAGS} 
CC 						= ${CROSS_COMPILE}gcc
CXX 					= ${CROSS_COMPILE}g++
LD 						= ${CROSS_COMPILE}ld
STRIP 					= ${CROSS_COMPILE}strip
AR						= ${CROSS_COMPILE}ar

NETAPP_TARGET_DIR       ?= /usr/local
NETAPP_STAGE_DIR        = ${NETAPP_TOP}/stage
NETAPP_OUTPUT_DIR		?= ${NETAPP_STAGE_DIR}${NETAPP_TARGET_DIR}

NETAPP_PREBUILT_DIR     = ${NETAPP_TOP}/prebuilt
NETAPP_OUTPUT_LIB_DIR 	= ${NETAPP_OUTPUT_DIR}/lib
NETAPP_OUTPUT_FW_DIR 	= ${NETAPP_OUTPUT_DIR}/firmware
NETAPP_OUTPUT_INC_DIR 	= ${NETAPP_OUTPUT_DIR}/include
NETAPP_OUTPUT_BIN_DIR 	= ${NETAPP_OUTPUT_DIR}/bin
INST_MODULES_DIR		= ${NETAPP_TARGET_DIR}/lib/modules
NETAPP_OUTPUT_MODULES_DIR= ${NETAPP_OUTPUT_LIB_DIR}/modules
NETAPP_PREBUILT_TARBALL	= ${NETAPP_PREBUILT_DIR}/NetApp_${KERNELVERSION}_${NETAPP_ENDIAN}.tbz
HAS_PREBUILT			= $(wildcard ${NETAPP_PREBUILT_TARBALL})

NETAPP_HEADER_FILE		= $(wildcard ${NETAPP_TOP}/netapp/include/*.h)
NETAPP_VERSION_MAJOR	= $(strip $(shell cat ${NETAPP_HEADER_FILE} | grep NETAPP_VERSION_MAJOR | sed -e 's/^.*NETAPP_VERSION_MAJOR//'))
NETAPP_VERSION_MINOR	= $(strip $(shell cat ${NETAPP_HEADER_FILE} | grep NETAPP_VERSION_MINOR | sed -e 's/^.*NETAPP_VERSION_MINOR//'))
NETAPP_VERSION_INC		= $(strip $(shell cat ${NETAPP_HEADER_FILE} | grep NETAPP_VERSION_INC   | sed -e 's/^.*NETAPP_VERSION_INC//'))

CFLAGS              += -DTARGET_DIR="\"${NETAPP_TARGET_DIR}\""

################################################################################

ifneq (${MULTI_BUILD},n)
HOST_NPROCS = $(shell grep '^processor' /proc/cpuinfo | wc -l)
HOST_LOAD   = $(shell uptime | sed -e 's/^.*load average: //' -e 's/[.].*//')
MAKE_J_ARG  = $(shell if [ ${HOST_NPROCS} -gt ${HOST_LOAD} ] ; then let "X = ${HOST_NPROCS} - ${HOST_LOAD}" && echo -n "-j $$X"  ; fi)
endif

UDHCPC_LITE_SCRIPT      = udhcpc_lite.script
CFLAGS                  += -DUDHCPC_LITE_SCRIPT="\"${NETAPP_TARGET_DIR}/bin/${UDHCPC_LITE_SCRIPT}\""


%.o : %.c
	@printf "[${CC}] <------------- %s \n" $<
	${Q_} ${CC}  ${CFLAGS} ${CPPFLAGS} -c $<  -o $@

%.o : %.cpp
	@printf "[${CXX}] <------------- %s \n" $<
	${Q_} ${CXX}  ${CFLAGS} ${CPPFLAGS} -c $<  -o $@
