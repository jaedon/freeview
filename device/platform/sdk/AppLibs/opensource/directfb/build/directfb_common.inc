############################################################
#     (c)2007-2012 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
############################################################

# We base a lot of things on the particular platform we compiling for.
# This variable MUST be defined in order to continue.  Make sure it is!
ifndef NEXUS_PLATFORM
ifndef PLATFORM
$(error NEXUS_PLATFORM environment variable is required)
else
NEXUS_PLATFORM := $(PLATFORM)
endif
endif

#
# Check to ensure BCHP_VER is defined in the environment
#
ifeq ($(BCHP_VER),)
$(error BCHP_VER enironment variable not defined)
endif

ifndef NEXUS_TOP
$(error NEXUS_TOP is not defined)
endif

ifndef APPLIBS_TOP
$(error APPLIBS_TOP is not defined)
endif

#
# This is the default location of the linux kernel source.
# This can be a symlink to your latest version of kernel source.
#
#HUMAX_PLATFORM_BASE
LINUX ?= $(APPLIBS_TOP)/../os/kernel
#LINUX ?= /opt/brcm/linux

#
# Check for presence of LINUX/Makefile.
#
ifneq ($(shell test -e ${LINUX}/Makefile && echo "y"),y)
$(error Cannot find ${LINUX}/Makefile.  Please set LINUX environment variable to point to your kernel source tree)
endif

##
#
# Ensure envars are backward compatible
#
ifdef MODE
NEXUS_MODE ?= $(MODE)
endif
ifdef OS
B_REFSW_OS ?= $(OS)
endif
ifdef DEBUG
B_REFSW_DEBUG ?= $(DEBUG)
endif
ifdef VERBOSE
B_REFSW_VERBOSE ?= $(VERBOSE)
endif
ifdef ARCH
B_REFSW_ARCH ?= $(ARCH)
endif
ifdef SHAREABLE
B_REFSW_SHAREABLE ?= $(SHAREABLE)
endif

ifeq ($(B_REFSW_OS),)
B_REFSW_OS = linuxuser
endif

ifeq ($(B_REFSW_DEBUG),)
B_REFSW_DEBUG = y
endif

ifeq ($(B_REFSW_ARCH),)
B_REFSW_ARCH = mipsel-linux
endif

ifeq ($(B_REFSW_DEBUG),y)
B_REFSW_BUILD_TYPE ?= debug
else
B_REFSW_BUILD_TYPE ?= release
endif

#
# Set variables based on the toolchain
#
ifeq ($(B_REFSW_ARCH),i386-linux)
B_REFSW_CROSS_COMPILE ?=
else
B_REFSW_CROSS_COMPILE ?= $(B_REFSW_ARCH)-

CCACHE ?= $(shell which ccache 2> /dev/null)

# Define make variables
AS      = $(B_REFSW_CROSS_COMPILE)as
LD      = $(B_REFSW_CROSS_COMPILE)ld
CC      = $(B_REFSW_CROSS_COMPILE)gcc
# NOTE: We must use c++ and not g++ to avoid GPL contamination
CXX     = $(B_REFSW_CROSS_COMPILE)c++
CPP     = $(B_REFSW_CROSS_COMPILE)cpp
AR      = $(B_REFSW_CROSS_COMPILE)ar
NM      = $(B_REFSW_CROSS_COMPILE)nm
STRIP   = $(B_REFSW_CROSS_COMPILE)strip
OBJCOPY = $(B_REFSW_CROSS_COMPILE)objcopy
OBJDUMP = $(B_REFSW_CROSS_COMPILE)objdump
RANLIB  = $(B_REFSW_CROSS_COMPILE)ranlib

endif

BCHP_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
DATE           ?= $(shell date +%Y%m%d)

##
#
# Point to AppLibs target output dirs
#
APPLIBS_INSTALL_PREFIX  ?= /usr/local
APPLIBS_TARGET_TOP      ?= ${APPLIBS_TOP}/target
APPLIBS_TARGET_ROOT_DIR ?= ${APPLIBS_TARGET_TOP}/${NEXUS_PLATFORM}${BCHP_VER_LOWER}.${B_REFSW_ARCH}.${B_REFSW_BUILD_TYPE}
APPLIBS_TARGET_DIR      ?= ${APPLIBS_TARGET_ROOT_DIR}/${APPLIBS_INSTALL_PREFIX}
APPLIBS_TARGET_LIB_DIR  ?= ${APPLIBS_TARGET_DIR}/lib
APPLIBS_TARGET_MOD_DIR  ?= ${APPLIBS_TARGET_LIB_DIR}/modules
APPLIBS_TARGET_BIN_DIR  ?= ${APPLIBS_TARGET_DIR}/bin
APPLIBS_TARGET_INC_DIR  ?= ${APPLIBS_TARGET_DIR}/include

##
#
# Point to opensource top dirs
#
OPENSOURCE_TOP ?= ${APPLIBS_TOP}/opensource

##
#
# Ensure toolchain used to build utilities is correct
#
B_REFSW_TOOLCHAIN_ARCH = $(shell echo $(B_REFSW_ARCH) | cut -d- -f1)-linux

##
#
# Add in additional include paths if required
#
NEXUS_APP_INCLUDE_PATHS +=                      \
    $(MAGNUM)/basemodules/int                   \
    $(MAGNUM)/basemodules/int/$(BCHP_CHIP)      \
    $(MAGNUM)/basemodules/mem                   \
    $(MAGNUM)/portinginterface/grc/$(BCHP_CHIP) \
    $(MAGNUM)/portinginterface/grc/include      \
    $(MAGNUM)/commonutils/pxl                   \
    $(MAGNUM)/commonutils/sur/include           \
    $(MAGNUM)/commonutils/sur/$(BCHP_CHIP)


ifneq ($(findstring NEXUS_HAS_GRAPHICS3D, $(NEXUS_APP_DEFINES)),)
ifeq ($(NEXUS_MODE),proxy)
NEXUS_APP_INCLUDE_PATHS +=                                      \
    $(NEXUS_TOP)/modules/surface/$(BCHP_CHIP)/src               \
    $(NEXUS_TOP)/modules/surface/src                            \
    $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/src/linuxuser.proxy
endif
NEXUS_APP_INCLUDE_PATHS +=                                      \
    $(NEXUS_SYNCTHUNK_DIR)                                      \
    $(NEXUS_TOP)/base/include                                   \
    $(NEXUS_TOP)/base/include/$(B_REFSW_OS)                     \
    $(NEXUS_TOP)/modules/surface/$(BCHP_CHIP)/include/priv      \
    $(NEXUS_TOP)/modules/surface/include/priv                   \
    $(NEXUS_TOP)/modules/graphics2d/$(BCHP_CHIP)/src            \
    $(NEXUS_TOP)/modules/graphics3d/$(BCHP_CHIP)/include/priv   \
    $(MAGNUM)/basemodules/tmr/$(BCHP_CHIP)                      \
    $(MAGNUM)/basemodules/tmr/include                           \
    $(MAGNUM)/commonutils/avc/$(BCHP_CHIP)                      \
    $(MAGNUM)/commonutils/avc/include                           \
    $(MAGNUM)/commonutils/mrc/$(BCHP_CHIP)                      \
    $(MAGNUM)/commonutils/mrc/include                           \
    $(MAGNUM)/commonutils/fmt                                   \
    $(MAGNUM)/portinginterface/p3d/$(BCHP_CHIP)
endif

##
#
# Support Extra Targets for building
#
EXTRA_BUILD_TARGETS                            +=
EXTRA_CLEAN_TARGETS                            +=
EXTRA_DISTCLEAN_TARGETS                        +=
EXTRA_UNINSTALL_TARGETS                        +=
EXTRA_CONFIGURED_TARGETS                       +=
EXTRA_COMPILED_TARGETS                         +=
EXTRA_INSTALLED_TARGETS                        +=
EXTRA_TARGET_INSTALLED_TARGETS                 +=

##
#
# Targets that need to be built before DirectFB
#
EXTRA_PRE_DFB_BUILD_TARGETS                    +=
EXTRA_PRE_DFB_TARGET_INSTALLED_TARGETS         +=

#
# Work out Kernel Version from Kernel Source's Top Level Makefile
#
K_VERSION                                      := $(shell grep -m1 "^VERSION" ${LINUX}/Makefile      | cut -d= -f2- | sed -e "s;^[^[:digit:]]*;;")
K_PATCHLEVEL                                   := $(shell grep -m1 "^PATCHLEVEL" ${LINUX}/Makefile   | cut -d= -f2- | sed -e "s;^[^[:digit:]]*;;")
K_SUBLEVEL                                     := $(shell grep -m1 "^SUBLEVEL" ${LINUX}/Makefile     | cut -d= -f2- | sed -e "s;^[^[:digit:]]*;;")
K_EXTRAVERSION                                 := '$(shell grep -m1 "^EXTRAVERSION" ${LINUX}/Makefile | cut -d= -f2-)'
ifneq ($(findstring shell,${K_EXTRAVERSION}),)
K_EXTRAVERSION                                 :=
else
K_EXTRAVERSION                                 := $(shell echo ${K_EXTRAVERSION} | sed -e "s;^[^[:digit:]]*;;")
endif
KERNEL_VERSION                                 := ${K_VERSION}.${K_PATCHLEVEL}.${K_SUBLEVEL}
ifneq ($(K_EXTRAVERSION),)
KERNEL_VERSION                                 := ${KERNEL_VERSION}-${K_EXTRAVERSION}
endif

#
# DirectFB Core Specific Defines
#
DIRECTFB_VERSION                               ?= 1.4.17
DIRECTFB_MAJOR                                 := $(shell echo ${DIRECTFB_VERSION} | cut -d. -f1)
DIRECTFB_MINOR                                 := $(shell echo ${DIRECTFB_VERSION} | cut -d. -f2)
DIRECTFB_MICRO                                 := $(shell echo ${DIRECTFB_VERSION} | cut -d. -f3)
DIRECTFB_VERSION_VENDOR                        := v1.5
DIRECTFB_APP_MAJOR_VERSION                     := 1
DIRECTFB_APP_MINOR_VERSION                     := 0
DIRECTFB_APPLIBS                               ?= y
DIRECTFB_SHARED                                ?= y
DIRECTFB_MULTI                                 ?= n
DIRECTFB_MASTER_LIB                            ?= y
DIRECTFB_CLIENT_LIB                            ?= n
DIRECTFB_NSC_SUPPORT                           ?= n
DIRECTFB_NIR_SUPPORT                           ?= n
DIRECTFB_SW_SMOOTH_SCALING                     ?= n
DIRECTFB_SW_DITHERING                          ?= n
DIRECTFB_HW_DITHERING                          ?= y
DIRECTFB_LINOTYPE                              ?= n
DIRECTFB_GLES_SUPPORT                          ?= n
DIRECTFB_GFX_3D                                ?= n
DIRECTFB_GFX_3D_DRIV_LIB                       ?= ""
DIRECTFB_KEY_INPUT                             ?= n
DIRECTFB_IR_INPUT                              ?= y
DIRECTFB_IR_PROTOCOL                           ?= CirNec
DIRECTFB_IR_KEYCODES                           ?= cirnec
DIRECTFB_GFX_PACKET_BUFFER                     ?= y
DIRECTFB_GFX_TRAPEZOID_SUPPORT                 ?= y
DIRECTFB_GFX_SOFT_MATRIX_SUPPORT               ?= y
BUILD_TESTS                                    ?= n
BUILD_EXAMPLES                                 ?= y
BUILD_FUSION                                   ?= n
BUILD_SAWMAN                                   ?= n
BUILD_PPDFB                                    ?= n
BUILD_DIVINE                                   ?= n
BUILD_VOODOO                                   ?= ${BUILD_DIVINE}
BUILD_INSIGNIA                                 ?= n
BUILD_TACHO                                    ?= n
BUILD_FFMPEG                                   ?= n
BUILD_AUTOMATION                               ?= n
BUILD_PRIVATE_TESTS                            ?= n

DIRECTFB_SOURCE_TARBALL                        := DirectFB-$(DIRECTFB_VERSION).tar.gz
DIRECTFB_PREFIX                                ?= ${APPLIBS_INSTALL_PREFIX}
DIRECTFB_DIR                                   ?= $(OPENSOURCE_TOP)/directfb
DIRECTFB_BIN_DIR                               := ${DIRECTFB_DIR}/bin
DIRECTFB_DOCS_DIR                              := ${DIRECTFB_DIR}/docs
DIRECTFB_SRC_DIR                               := ${DIRECTFB_DIR}/src
DIRECTFB_SRC_DFB_TAR_DIR                       := ${DIRECTFB_SRC_DIR}/directfb_tarballs
DIRECTFB_SRC_BRCM_DIR                          := ${DIRECTFB_SRC_DIR}/broadcom_files
DIRECTFB_PRE_DIR                               := ${DIRECTFB_SRC_BRCM_DIR}/precompiled
DIRECTFB_PRE_INSTALL_DIR                       := ${DIRECTFB_PRE_DIR}/${NEXUS_PLATFORM}${BCHP_VER_LOWER}_${B_REFSW_ARCH}_${KERNEL_VERSION}
DIRECTFB_PRE_INSTALL_DIR                       := ${DIRECTFB_PRE_INSTALL_DIR}_${B_REFSW_BUILD_TYPE}
DIRECTFB_PUB_DIR                               := ${DIRECTFB_SRC_BRCM_DIR}/public
DIRECTFB_PRI_DIR                               := ${DIRECTFB_SRC_BRCM_DIR}/private
DIRECTFB_BRCM_DIR                              := ${DIRECTFB_SRC_DIR}/DirectFB-Broadcom
DIRECTFB_PRE_INSTALL_BIN_DIR                   := ${DIRECTFB_PRE_INSTALL_DIR}/bin
DIRECTFB_PRE_INSTALL_BIN_DFB_DIR               := ${DIRECTFB_PRE_INSTALL_BIN_DIR}/directfb
DIRECTFB_PRE_INSTALL_BIN_DFB_VER_DIR           := ${DIRECTFB_PRE_INSTALL_BIN_DFB_DIR}/${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}
DIRECTFB_PRE_INSTALL_ETC_DIR                   := ${DIRECTFB_PRE_INSTALL_DIR}/etc
DIRECTFB_PRE_INSTALL_INCLUDE_DIR               := ${DIRECTFB_PRE_INSTALL_DIR}/include
DIRECTFB_PRE_INSTALL_LIB_DIR                   := ${DIRECTFB_PRE_INSTALL_DIR}/lib
DIRECTFB_PRE_INSTALL_LIB_DFB_DIR               := ${DIRECTFB_PRE_INSTALL_LIB_DIR}/directfb-${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}-${DIRECTFB_MICRO}
ifneq ($(B_REFSW_DEBUG),y)
DIRECTFB_PRE_INSTALL_LIB_DFB_DIR               := ${DIRECTFB_PRE_INSTALL_LIB_DFB_DIR}-pure
endif
DIRECTFB_PRE_INSTALL_LIB_DFB_GFX_DIR           := ${DIRECTFB_PRE_INSTALL_LIB_DFB_DIR}/gfxdrivers
DIRECTFB_PRE_INSTALL_LIB_DFB_SYS_DIR           := ${DIRECTFB_PRE_INSTALL_LIB_DFB_DIR}/systems
DIRECTFB_PRE_INSTALL_LIB_DFB_INP_DIR           := ${DIRECTFB_PRE_INSTALL_LIB_DFB_DIR}/inputdrivers
DIRECTFB_PRE_INSTALL_LIB_DFB_IMG_DIR           := ${DIRECTFB_PRE_INSTALL_LIB_DFB_DIR}/interfaces/IDirectFBImageProvider
DIRECTFB_PRE_INSTALL_LIB_MODULES_DIR           := ${DIRECTFB_PRE_INSTALL_LIB_DIR}/modules
DIRECTFB_PUB_DFB_DIR                           := ${DIRECTFB_PUB_DIR}/DirectFB
DIRECTFB_PUB_DFB_VER1_DIR                      := ${DIRECTFB_PUB_DFB_DIR}/${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}
DIRECTFB_PUB_DFB_VER2_DIR                      := ${DIRECTFB_PUB_DFB_VER1_DIR}.${DIRECTFB_MICRO}
DIRECTFB_SRC_DFB_DIR                           := ${DIRECTFB_SRC_DIR}/DirectFB-${DIRECTFB_VERSION}
DIRECTFB_BRCM_SRC_DFB_DIR                      := ${DIRECTFB_SRC_DIR}/DirectFB-Broadcom
DIRECTFB_INCLUDE_DIR                           := ${DIRECTFB_SRC_DFB_DIR}/include
DIRECTFB_LIB_DIR                               := ${DIRECTFB_SRC_DFB_DIR}/lib
DIRECTFB_BUILD_DIR                             := ${DIRECTFB_DIR}/build
DIRECTFB_BUILD_VER_DIR                         := ${DIRECTFB_BUILD_DIR}/$(DIRECTFB_VERSION)
DIRECTFB_OBJECT_DIR                            ?= $(DIRECTFB_BUILD_VER_DIR)/$(NEXUS_PLATFORM)_$(B_REFSW_OS)
DIRECTFB_TARGET_ROOT_DIR                       ?= ${DIRECTFB_PREFIX}
DIRECTFB_TARGET_BIN_DIR                        := ${DIRECTFB_TARGET_ROOT_DIR}/bin
DIRECTFB_TARGET_BIN_DFB_DIR                    ?= ${DIRECTFB_TARGET_BIN_DIR}/directfb
DIRECTFB_TARGET_BIN_DFB_VER_DIR                := ${DIRECTFB_TARGET_BIN_DFB_DIR}/${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}
DIRECTFB_TARGET_LIB_DIR                        := ${DIRECTFB_TARGET_ROOT_DIR}/lib
DIRECTFB_TARGET_MODULES_DIR                    := ${DIRECTFB_TARGET_LIB_DIR}/modules
DIRECTFB_BRCM_HEADER_FILE                      := ${DIRECTFB_BRCM_SRC_DFB_DIR}/broadcom_header.txt
DIRECTFB_GFX_DEFINES_FILE                      := ${DIRECTFB_BRCM_SRC_DFB_DIR}/gfxdrivers/bcmnexus/bcmnexus_gfx_defines.h
DIRECTFB_SYS_DEFINES_FILE                      := ${DIRECTFB_BRCM_SRC_DFB_DIR}/systems/bcmnexus/bcmnexus_sys_defines.h
DIRECTFB_APP_DEFINES_FILE                      := ${DIRECTFB_BRCM_SRC_DFB_DIR}/tools/bcmnexus_app_defines.h
DIRECTFB_IR_INPUTMODE_FILE                     := ${DIRECTFB_BRCM_SRC_DFB_DIR}/platform/bcmnexus_ir_inputmode.h
DIRECTFB_IR_CONVERT_FILE                       := ${DIRECTFB_BRCM_SRC_DFB_DIR}/platform/bcmnexus_ir_convert.h
DIRECTFB_INSTALL_PREFIX                        := DirectFB-${DIRECTFB_VERSION}_
ifeq ($(DIRECTFB_MULTI),y)
DIRECTFB_INSTALL_PREFIX                        := ${DIRECTFB_INSTALL_PREFIX}multi_
endif
DIRECTFB_INSTALL_PREFIX                        := ${DIRECTFB_INSTALL_PREFIX}${B_REFSW_BUILD_TYPE}_build.${NEXUS_PLATFORM}${BCHP_VER}
DIRECTFB_INSTALL_DIR                           ?= ${DIRECTFB_BIN_DIR}/${DIRECTFB_INSTALL_PREFIX}
DIRECTFB_INSTALL_ROOT_DIR                      ?= ${DIRECTFB_INSTALL_DIR}/${DIRECTFB_TARGET_ROOT_DIR}
DIRECTFB_INSTALL_BIN_DIR                       := ${DIRECTFB_INSTALL_DIR}/${DIRECTFB_TARGET_BIN_DIR}
DIRECTFB_INSTALL_BIN_DFB_DIR                   := ${DIRECTFB_INSTALL_DIR}/${DIRECTFB_TARGET_BIN_DFB_DIR}
DIRECTFB_INSTALL_BIN_DFB_VER_DIR               := ${DIRECTFB_INSTALL_DIR}/${DIRECTFB_TARGET_BIN_DFB_VER_DIR}
DIRECTFB_INSTALL_ETC_DIR                       := ${DIRECTFB_INSTALL_ROOT_DIR}/etc
DIRECTFB_INSTALL_ETC_HOTPLUG_DIR               := ${DIRECTFB_INSTALL_ETC_DIR}/hotplug.d
DIRECTFB_INSTALL_INCLUDE_DIR                   := ${DIRECTFB_INSTALL_ROOT_DIR}/include
DIRECTFB_INSTALL_LIB_DIR                       := ${DIRECTFB_INSTALL_ROOT_DIR}/lib
DIRECTFB_INSTALL_LIB_MODULES_DIR               := ${DIRECTFB_INSTALL_LIB_DIR}/modules
DIRECTFB_INSTALL_LIB_DFB_DIR                   := ${DIRECTFB_INSTALL_LIB_DIR}/directfb-${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}-${DIRECTFB_MICRO}
ifneq ($(B_REFSW_DEBUG),y)
DIRECTFB_INSTALL_LIB_DFB_DIR                   := ${DIRECTFB_INSTALL_LIB_DFB_DIR}-pure
endif
DIRECTFB_INSTALL_LIB_DFB_GFX_DIR               := ${DIRECTFB_INSTALL_LIB_DFB_DIR}/gfxdrivers
DIRECTFB_INSTALL_LIB_DFB_SYS_DIR               := ${DIRECTFB_INSTALL_LIB_DFB_DIR}/systems
DIRECTFB_INSTALL_LIB_DFB_INP_DIR               := ${DIRECTFB_INSTALL_LIB_DFB_DIR}/inputdrivers
DIRECTFB_INSTALL_LIB_DFB_IMG_DIR               := ${DIRECTFB_INSTALL_LIB_DFB_DIR}/interfaces/IDirectFBImageProvider
DIRECTFB_INSTALL_SHARE_DIR                     := ${DIRECTFB_INSTALL_ROOT_DIR}/share
DIRECTFB_INSTALL_DATA_DIR                      := ${DIRECTFB_INSTALL_SHARE_DIR}/directfb-${DIRECTFB_VERSION}
DIRECTFB_INSTALL_MAN_DIR                       := ${DIRECTFB_INSTALL_SHARE_DIR}/man
DIRECTFB_INSTALL_DIRECTFB_INCLUDE_DIR          := ${DIRECTFB_INSTALL_INCLUDE_DIR}/directfb
DIRECTFB_INSTALL_DIRECTFB_INTERNAL_INCLUDE_DIR := ${DIRECTFB_INSTALL_INCLUDE_DIR}/directfb-internal
DIRECTFB_INSTALL_PKG_CONFIG_DIR                := ${DIRECTFB_INSTALL_LIB_DIR}/pkgconfig
DIRECTFB_PKG_CONFIG                            := directfb-internal
DIRECTFB_UNPACKED_FLAG                         := ${DIRECTFB_SRC_DFB_DIR}/.unpacked
DIRECTFB_CONFIGURATION_FLAG                    := ${DIRECTFB_SRC_DFB_DIR}/.configuration
DIRECTFB_CONFIGURED_FLAG                       := ${DIRECTFB_SRC_DFB_DIR}/.configured
DIRECTFB_COMPILED_FLAG                         := ${DIRECTFB_SRC_DFB_DIR}/.compiled
DIRECTFB_INSTALLED_FLAG                        := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_installed
DIRECTFB_POST_INSTALLED_FLAG                   := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_post_installed
DIRECTFB_FLAG                                  := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb

DIRECTFB_BRCM_CONFIGURATION_FLAG               := ${DIRECTFB_BRCM_SRC_DFB_DIR}/.configuration
DIRECTFB_BRCM_CONFIGURED_FLAG                  := ${DIRECTFB_BRCM_SRC_DFB_DIR}/.configured
DIRECTFB_BRCM_COMPILED_FLAG                    := ${DIRECTFB_BRCM_SRC_DFB_DIR}/.compiled
DIRECTFB_BRCM_INSTALLED_FLAG                   := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_brcm_installed
DIRECTFB_BRCM_POST_INSTALLED_FLAG              := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_brcm_post_installed
DIRECTFB_BRCM_FLAG                             := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_brcm

DIRECTFB_NFS_ROOT_DIR                          ?= ~/nfs

#
# Define Nexus driver kernel module and script
#

#
# We need to support 4 main build types with Nexus:
#
# Kernel mode - all builds behave the same thanks to the proxy
# Userspace - Single App (single Nexus build)
# Userspace - Multi  App (Nexus built for server & client)
# Userspace - Single App NSC (Nexus built for server & client, DFB linked against client)
#
# N.B you cannot do NSC and multi app builds easily and it's not a usage model people should use.
#
# P.S. If you want a client only Nexus build in userspace, build the multi app and take the client half.
#

ifeq ($(DIRECTFB_MULTI),y)
ifeq ($(DIRECTFB_NSC_SUPPORT),y)
$(error DirectFB multi app mode not supported with Nexus surface compositor, pick one multi app framework!)
endif
endif

ifeq ($(NEXUS_MODE),proxy)
# Kernel mode
NEXUS_DRIVER            = nexus.ko
NEXUS_LIB               = libnexus_kernel.so
NEXUS_SCRIPT            = nexus_kernel.974xx
NEXUS_CLIENT_FLAGS      =
NEXUS_CLIENT_LIB_SUFFIX =

else

# Userspace drivers 
NEXUS_DRIVER            = bcmdriver.ko
NEXUS_LIB               = libnexus_user.so
NEXUS_SCRIPT            = nexus_user.974xx

ifeq ($(DIRECTFB_NSC_SUPPORT),y)
# NSC single app build
# If we are compiling for Nexus surface compositor integration into DFB
# then we only need the client libs that link with Nexus client
DIRECTFB_MASTER_LIB      = n
DIRECTFB_CLIENT_LIB      = y
ifeq ($(NEXUS_WEBCPU),core1_server)
# Client Nexus build for WebCPU where Nexus master is running on different CPU
NEXUS_CLIENT_LIB_SUFFIX ?= _webcpu
else
NEXUS_CLIENT_LIB_SUFFIX ?= _client
endif
NEXUS_CLIENT_FLAGS      ?= NEXUS_MODE=client NEXUS_LIB_SUFFIX=${NEXUS_CLIENT_LIB_SUFFIX}

else
ifeq ($(DIRECTFB_MULTI),y)
# Multi App Userspace build
DIRECTFB_CLIENT_LIB      = y
NEXUS_CLIENT_LIB_SUFFIX ?= _client
NEXUS_CLIENT_FLAGS      ?= NEXUS_MODE=client NEXUS_LIB_SUFFIX=${NEXUS_CLIENT_LIB_SUFFIX}
else
# Single App userspace build
NEXUS_CLIENT_LIB_SUFFIX = 
NEXUS_CLIENT_FLAGS      = 
endif

endif
endif

export NEXUS_CLIENT_LIB_SUFFIX

#
# DirectFB directories which depend on Nexus configuration
#

DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_DIR        := ${DIRECTFB_PRE_INSTALL_LIB_DFB_DIR}${NEXUS_CLIENT_LIB_SUFFIX}
DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_GFX_DIR    := ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_DIR}/gfxdrivers
DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_SYS_DIR    := ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_DIR}/systems
DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_INP_DIR    := ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_DIR}/inputdrivers
DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_IMG_DIR    := ${DIRECTFB_PRE_INSTALL_LIB_CLIENT_DFB_DIR}/interfaces/IDirectFBImageProvider

DIRECTFB_INSTALL_LIB_CLIENT_DFB_DIR            := ${DIRECTFB_INSTALL_LIB_DFB_DIR}${NEXUS_CLIENT_LIB_SUFFIX}
DIRECTFB_INSTALL_LIB_CLIENT_DFB_GFX_DIR        := ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_DIR}/gfxdrivers
DIRECTFB_INSTALL_LIB_CLIENT_DFB_SYS_DIR        := ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_DIR}/systems
DIRECTFB_INSTALL_LIB_CLIENT_DFB_INP_DIR        := ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_DIR}/inputdrivers
DIRECTFB_INSTALL_LIB_CLIENT_DFB_IMG_DIR        := ${DIRECTFB_INSTALL_LIB_CLIENT_DFB_DIR}/interfaces/IDirectFBImageProvider


#
# Tarballs
#
DIRECTFB_TARGET_TARBALL                        := ${DIRECTFB_BIN_DIR}/${DIRECTFB_INSTALL_PREFIX}.tgz
DIRECTFB_RELEASE_TARBALL                       := ${DIRECTFB_BIN_DIR}/DirectFB-${DIRECTFB_VERSION}
ifneq ($(DIRECTFB_VERSION_VENDOR),)
DIRECTFB_RELEASE_TARBALL                       := ${DIRECTFB_RELEASE_TARBALL}_${DIRECTFB_VERSION_VENDOR}
endif
DIRECTFB_RELEASE_TARBALL                       := ${DIRECTFB_RELEASE_TARBALL}_${DATE}.tgz
DIRECTFB_RELEASE_TARBALL                       := $(shell echo -n $(DIRECTFB_RELEASE_TARBALL) | sed -e "s;\ ;_;g")

#
# DirectFB Configuration Flags
#

DIRECTFB_CONFIG_FLAGS                          +=               \
    DIRECTFB_VERSION_VENDOR=" ${DIRECTFB_VERSION_VENDOR}"       \
    --prefix=${DIRECTFB_TARGET_ROOT_DIR}                        \
    --bindir=${DIRECTFB_TARGET_BIN_DFB_VER_DIR}                 \
    --disable-fbdev --disable-sdl --disable-vnc --disable-x11   \
    --enable-jpeg --enable-zlib --enable-png --enable-gif       \
    --enable-freetype                                           \
    --disable-video4linux --disable-devmem                      \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)                            

DIRECTFB_BRCM_CONFIG_FLAGS                     +=               \
    --enable-maintainer-mode                                    \
    --prefix=${DIRECTFB_TARGET_ROOT_DIR}                        \
    --bindir=${DIRECTFB_TARGET_BIN_DFB_VER_DIR}                 \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)                            \
    --enable-debug 

#if HUMAX_PLATFORM_BASE
DIRECTFB_INPUTDRIVERS ?= linuxinput

ifeq ($(DIRECTFB_NETFLIX_SUPPORT),y)
DIRECTFB_CONFIG_FLAGS += --with-message-size=32768 
endif

#else
#DIRECTFB_INPUTDRIVERS := linuxinput
#endif

ifeq ($(DIRECTFB_KEY_INPUT),y)
ifneq ($(findstring NEXUS_HAS_KEYPAD, $(NEXUS_APP_DEFINES)),)
BCMNEXUS_KEY_INPUT                             :=y 
endif
else
BCMNEXUS_KEY_INPUT                             := n
endif

ifeq ($(BCMNEXUS_KEY_INPUT),y)
#DIRECTFB_INPUTDRIVERS := ${DIRECTFB_INPUTDRIVERS},bcmnexus_key
endif

ifeq ($(DIRECTFB_IR_INPUT),y)
ifneq ($(findstring NEXUS_HAS_IR_INPUT, $(NEXUS_APP_DEFINES)),)
BCMNEXUS_IR_INPUT                              := y
endif
else
BCMNEXUS_IR_INPUT                              := n
endif

DIRECTFB_CONFIG_FLAGS += DIRECTFB_IR_PROTOCOL=${DIRECTFB_IR_PROTOCOL}
DIRECTFB_CONFIG_FLAGS += DIRECTFB_IR_KEYCODES=${DIRECTFB_IR_KEYCODES}

DIRECTFB_CONFIG_FLAGS += --with-inputdrivers=${DIRECTFB_INPUTDRIVERS}


ifeq ($(BUILD_TESTS),y)
DIRECTFB_CONFIG_FLAGS += --with-tests
endif

# Check whether Nexus has 3D graphics support enabled first
ifneq ($(findstring NEXUS_HAS_GRAPHICS3D, $(NEXUS_APP_DEFINES)),)

ifeq ($(shell test -f "${MAGNUM}/portinginterface/p3d/${BCHP_CHIP}/bp3d_private.h" && echo "y"),y)
DIRECTFB_GFX_3D = "p3d"

ifeq ($(DIRECTFB_GLES_SUPPORT),y)
DIRECTFB_CONFIG_FLAGS += --enable-egl
DIRECTFB_BRCM_CONFIG_FLAGS += --enable-bcmnexus-gles
endif

else 

ifeq ($(shell test -f "${NEXUS_TOP}/modules/graphics3d/graphics3d.inc" && echo "y"),y)
ifneq  ($(shell grep "NEXUS_GRAPHICS3D_MAGNUM_MODULES" ${NEXUS_TOP}/modules/graphics3d/graphics3d.inc | grep "v3d"),)
DIRECTFB_GFX_3D = "v3d"
DIRECTFB_GFX_3D_DRIV_LIB = "libv3ddriver.so"
endif
endif

endif
endif

ifeq ($(DIRECTFB_SHARED),y)
DIRECTFB_CONFIG_FLAGS += --enable-shared-extlibs
else
DIRECTFB_CONFIG_FLAGS += --enable-static
endif

DIRECTFB_CONFIG_FLAGS += --with-gfxdrivers=none


ifeq ($(DIRECTFB_MULTI),y)
BUILD_FUSION = y
BUILD_SAWMAN = y
DIRECTFB_CONFIG_FLAGS += --enable-multi
endif

ifneq ($(findstring NEXUS_HAS_PICTURE_DECODER, $(NEXUS_APP_DEFINES)),)
DIRECTFB_SID ?= y
else
DIRECTFB_SID ?= n
endif

ifeq ($(DIRECTFB_SID),y)
DIRECTFB_BRCM_CONFIG_FLAGS += --enable-sid
endif

ifeq ($(DIRECTFB_NIR_SUPPORT),y)
DIRECTFB_BRCM_CONFIG_FLAGS += --enable-bcmnexus_keyboard_pointer
DIRECTFB_BRCM_CONFIG_FLAGS += DIRECTFB_IR_PROTOCOL=${DIRECTFB_IR_PROTOCOL}
DIRECTFB_BRCM_CONFIG_FLAGS += DIRECTFB_IR_KEYCODES=${DIRECTFB_IR_KEYCODES}
endif

ifeq ($(DIRECTFB_SW_SMOOTH_SCALING),y)
DIRECTFB_CONFIG_FLAGS += --with-smooth-scaling
endif

ifeq ($(DIRECTFB_SW_DITHERING),y)
DIRECTFB_CONFIG_FLAGS += --with-dither-rgb16=advanced --with-dither=advanced
endif

ifeq ($(DIRECTFB_LINOTYPE),y)
DIRECTFB_LINOTYPE = $(shell test -e "$(DIRECTFB_SRC_DFB_DIR)/interfaces/IDirectFBFont/idirectfbfont_lino.c" && echo "y" || echo "n")
endif

ifeq ($(DIRECTFB_LINOTYPE),y)
DIRECTFB_CONFIG_FLAGS += --enable-linotype
endif

ifeq ($(BUILD_VOODOO),y)
DIRECTFB_CONFIG_FLAGS += --enable-voodoo
endif

ifeq ($(B_REFSW_DEBUG),y)
DIRECTFB_CONFIG_FLAGS += --enable-debug --enable-debug-support
DIRECTFB_BRCM_CONFIG_FLAGS += --enable-debug
else
DIRECTFB_CONFIG_FLAGS += --disable-debug --disable-debug-support
DIRECTFB_BRCM_CONFIG_FLAGS += --disable-debug
endif

ifeq ($(TRACE),y)
DIRECTFB_CONFIG_FLAGS += --enable-trace
endif

#
# Setup the master/client libs
#

DIRECTFB_CONFIG_FLAGS += DIRECTFB_MASTER_LIB=$(DIRECTFB_MASTER_LIB)
DIRECTFB_CONFIG_FLAGS += DIRECTFB_CLIENT_LIB=$(DIRECTFB_CLIENT_LIB)
DIRECTFB_BRCM_CONFIG_FLAGS += DIRECTFB_MASTER_LIB=$(DIRECTFB_MASTER_LIB)
DIRECTFB_BRCM_CONFIG_FLAGS += DIRECTFB_CLIENT_LIB=$(DIRECTFB_CLIENT_LIB)

ifeq ($(DIRECTFB_MASTER_LIB),y)
DIRECTFB_BRCM_CONFIG_FLAGS += --enable-master-lib
endif

ifeq ($(DIRECTFB_CLIENT_LIB),y)
DIRECTFB_BRCM_CONFIG_FLAGS += --enable-client-lib
endif

DIRECTFB_BRCM_CONFIG_FLAGS += DIRECTFB_NSC_SUPPORT=$(DIRECTFB_NSC_SUPPORT)
DIRECTFB_BRCM_CONFIG_FLAGS += DIRECTFB_NIR_SUPPORT=$(DIRECTFB_NIR_SUPPORT)

#
# DirectFB Examples Specific Defines
#
DIRECTFB_EXAMPLES_VERSION                   ?= 1.6.0pre
DIRECTFB_EXAMPLES_MAJOR                     := $(shell echo ${DIRECTFB_EXAMPLES_VERSION} | cut -d. -f1)
DIRECTFB_EXAMPLES_MINOR                     := $(shell echo ${DIRECTFB_EXAMPLES_VERSION} | cut -d. -f2)
DIRECTFB_EXAMPLES_MICRO                     := $(shell echo ${DIRECTFB_EXAMPLES_VERSION} | cut -d. -f3)
DIRECTFB_EXAMPLES_SOURCE_TARBALL            := DirectFB-examples-$(DIRECTFB_EXAMPLES_VERSION).tar.gz
DIRECTFB_EXAMPLES_TARGET_ROOT_DIR           ?= ${DIRECTFB_PREFIX}
DIRECTFB_EXAMPLES_TARGET_BIN_DIR            := ${DIRECTFB_EXAMPLES_TARGET_ROOT_DIR}/bin
DIRECTFB_EXAMPLES_TARGET_BIN_DFB_DIR        := ${DIRECTFB_EXAMPLES_TARGET_BIN_DIR}/directfb/
DIRECTFB_EXAMPLES_TARGET_BIN_DFB_VER_DIR    := ${DIRECTFB_EXAMPLES_TARGET_BIN_DFB_DIR}/${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}
DIRECTFB_EXAMPLES_PUB_DFB_DIR               := ${DIRECTFB_PUB_DIR}/DirectFB-examples
DIRECTFB_EXAMPLES_PUB_DFB_VER1_DIR          := ${DIRECTFB_EXAMPLES_PUB_DFB_DIR}/${DIRECTFB_EXAMPLES_MAJOR}.${DIRECTFB_EXAMPLES_MINOR}
DIRECTFB_EXAMPLES_PUB_DFB_VER2_DIR          := ${DIRECTFB_EXAMPLES_PUB_DFB_VER1_DIR}.${DIRECTFB_EXAMPLES_MICRO}
DIRECTFB_EXAMPLES_SRC_DIR                   := ${DIRECTFB_SRC_DIR}/DirectFB-examples-$(DIRECTFB_EXAMPLES_VERSION)
DIRECTFB_EXAMPLES_INCLUDE_DIR               := ${DIRECTFB_EXAMPLES_SRC_DIR}/include
DIRECTFB_EXAMPLES_CONFIG_FLAGS              :=              \
    --prefix=${DIRECTFB_EXAMPLES_TARGET_ROOT_DIR}           \
    --bindir=${DIRECTFB_EXAMPLES_TARGET_BIN_DFB_VER_DIR}    \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)

ifeq ($(B_REFSW_DEBUG),y)
DIRECTFB_EXAMPLES_CONFIG_FLAGS += --enable-debug
endif

DIRECTFB_EXAMPLES_BUILD_TARGET           := directfb-examples-all
DIRECTFB_EXAMPLES_CLEAN_TARGET           := directfb-examples-clean
DIRECTFB_EXAMPLES_DISTCLEAN_TARGET       := directfb-examples-distclean
DIRECTFB_EXAMPLES_MRPROPER_TARGET        := directfb-examples-mrproper
DIRECTFB_EXAMPLES_UNINSTALL_TARGET       := directfb-examples-uninstall
DIRECTFB_EXAMPLES_UNPACKED_FLAG          := ${DIRECTFB_EXAMPLES_SRC_DIR}/.unpacked
DIRECTFB_EXAMPLES_CONFIGURATION_FLAG     := ${DIRECTFB_EXAMPLES_SRC_DIR}/.configuration
DIRECTFB_EXAMPLES_CONFIGURED_FLAG        := ${DIRECTFB_EXAMPLES_SRC_DIR}/.configured
DIRECTFB_EXAMPLES_COMPILED_FLAG          := ${DIRECTFB_EXAMPLES_SRC_DIR}/.compiled
DIRECTFB_EXAMPLES_INSTALLED_FLAG         := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_examples_installed
DIRECTFB_EXAMPLES_FLAG                   := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_examples
EXTRA_CLEAN_TARGETS                      += ${DIRECTFB_EXAMPLES_CLEAN_TARGET}
EXTRA_DISTCLEAN_TARGETS                  += ${DIRECTFB_EXAMPLES_DISTCLEAN_TARGET}
EXTRA_MRPROPER_TARGETS                   += ${DIRECTFB_EXAMPLES_MRPROPER_TARGET}
EXTRA_UNINSTALL_TARGETS                  += ${DIRECTFB_EXAMPLES_UNINSTALL_TARGET}
ifeq ($(BUILD_EXAMPLES),y)
EXTRA_BUILD_TARGETS                      += ${DIRECTFB_EXAMPLES_BUILD_TARGET}
EXTRA_CONFIGURED_TARGETS                 += ${DIRECTFB_EXAMPLES_CONFIGURED_FLAG}
EXTRA_COMPILED_TARGETS                   += ${DIRECTFB_EXAMPLES_COMPILED_FLAG}
EXTRA_INSTALLED_TARGETS                  += ${DIRECTFB_EXAMPLES_INSTALLED_FLAG}
EXTRA_TARGET_INSTALLED_TARGETS           += ${DIRECTFB_EXAMPLES_FLAG}
endif

#
# Fusion Specific Defines
#
FUSION_VERSION                      ?= 8.10.4
FUSION_MAJOR                        := $(shell echo ${FUSION_VERSION} | cut -d. -f1)
FUSION_MINOR                        := $(shell echo ${FUSION_VERSION} | cut -d. -f2)
FUSION_MICRO                        := $(shell echo ${FUSION_VERSION} | cut -d. -f3)
FUSION_SOURCE_TARBALL               := linux-fusion-$(FUSION_VERSION).tar.gz
FUSION_PUB_FUS_DIR                  := ${DIRECTFB_PUB_DIR}/linux-fusion
FUSION_PUB_FUS_VER1_DIR             := ${FUSION_PUB_FUS_DIR}/${FUSION_MAJOR}.${FUSION_MINOR}
FUSION_PUB_FUS_VER2_DIR             := ${FUSION_PUB_FUS_VER1_DIR}.${FUSION_MICRO}
FUSION_PRI_FUS_VER1_DIR             := ${DIRECTFB_SRC_BRCM_DIR}/private/linux-fusion/${FUSION_MAJOR}.${FUSION_MINOR}
FUSION_SRC_DIR                      := ${DIRECTFB_SRC_DIR}/driver/linux-fusion-$(FUSION_VERSION)
FUSION_INSTALL_DIR                  ?= $(DIRECTFB_OBJECT_DIR)
FUSION_INSTALL_INCLUDE_DIR          := $(FUSION_INSTALL_DIR)/include
FUSION_BUILD_TARGET                 := fusion-all
FUSION_CLEAN_TARGET                 := fusion-clean
FUSION_DISTCLEAN_TARGET             := fusion-distclean
FUSION_MRPROPER_TARGET              := fusion-mrproper
FUSION_UNINSTALL_TARGET             := fusion-uninstall
FUSION_UNPACKED_FLAG                := ${FUSION_SRC_DIR}/.unpacked
FUSION_CONFIGURATION_FLAG           := ${FUSION_SRC_DIR}/.configuration
FUSION_COMPILED_FLAG                := ${FUSION_SRC_DIR}/.compiled
FUSION_INSTALLED_FLAG               := ${FUSION_INSTALL_DIR}/.fusion_installed
FUSION_FLAG                         := ${FUSION_INSTALL_DIR}/.fusion

#
# SaWMan Specific Defines
#
SAWMAN_VERSION                      ?= 1.5.4
SAWMAN_MAJOR                        := $(shell echo ${SAWMAN_VERSION} | cut -d. -f1)
SAWMAN_MINOR                        := $(shell echo ${SAWMAN_VERSION} | cut -d. -f2)
SAWMAN_MICRO                        := $(shell echo ${SAWMAN_VERSION} | cut -d. -f3)
SAWMAN_SOURCE_TARBALL               := SaWMan-$(SAWMAN_VERSION).tar.gz
SAWMAN_TARGET_ROOT_DIR              ?= ${DIRECTFB_PREFIX}
SAWMAN_TARGET_BIN_DIR               := ${SAWMAN_TARGET_ROOT_DIR}/bin
SAWMAN_TARGET_BIN_SAW_DIR           := ${SAWMAN_TARGET_BIN_DIR}/sawman
SAWMAN_TARGET_BIN_SAW_VER_DIR       := ${SAWMAN_TARGET_BIN_SAW_DIR}/${SAWMAN_MAJOR}.${SAWMAN_MINOR}
SAWMAN_PUB_SAW_DIR                  := ${DIRECTFB_PUB_DIR}/SaWMan
SAWMAN_PUB_SAW_VER1_DIR             := ${SAWMAN_PUB_SAW_DIR}/${SAWMAN_MAJOR}.${SAWMAN_MINOR}
SAWMAN_PUB_SAW_VER2_DIR             := ${SAWMAN_PUB_SAW_VER1_DIR}.${SAWMAN_MICRO}
SAWMAN_SRC_DIR                      := ${DIRECTFB_SRC_DIR}/SaWMan-$(SAWMAN_VERSION)
SAWMAN_INCLUDE_DIR                  := ${SAWMAN_SRC_DIR}/include
SAWMAN_INSTALL_DIR                  ?= ${DIRECTFB_INSTALL_DIR}
SAWMAN_INSTALL_ROOT_DIR             ?= ${SAWMAN_INSTALL_DIR}/${SAWMAN_TARGET_ROOT_DIR}
SAWMAN_INSTALL_BIN_DIR              := ${SAWMAN_INSTALL_DIR}/${SAWMAN_TARGET_BIN_DIR}
SAWMAN_INSTALL_BIN_SAW_DIR          := ${SAWMAN_INSTALL_DIR}/${SAWMAN_TARGET_BIN_SAW_DIR}
SAWMAN_INSTALL_BIN_SAW_VER_DIR      := ${SAWMAN_INSTALL_DIR}/${SAWMAN_TARGET_BIN_SAW_VER_DIR}
SAWMAN_INSTALL_INCLUDE_DIR          := ${SAWMAN_INSTALL_ROOT_DIR}/include
SAWMAN_INSTALL_LIB_DIR              := ${SAWMAN_INSTALL_ROOT_DIR}/lib
SAWMAN_INSTALL_PKG_CONFIG_DIR       := ${SAWMAN_INSTALL_LIB_DIR}/pkgconfig
SAWMAN_PKG_CONFIG                   := sawman
SAWMAN_CONFIG_FLAGS                 :=          \
    --prefix=${SAWMAN_TARGET_ROOT_DIR}          \
    --bindir=${SAWMAN_TARGET_BIN_SAW_VER_DIR}   \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)

ifeq ($(B_REFSW_DEBUG),y)
SAWMAN_CONFIG_FLAGS += --enable-debug
endif

ifeq ($(TRACE),y)
SAWMAN_CONFIG_FLAGS += --enable-trace
endif

SAWMAN_BUILD_TARGET                 := sawman-all
SAWMAN_CLEAN_TARGET                 := sawman-clean
SAWMAN_DISTCLEAN_TARGET             := sawman-distclean
SAWMAN_MRPROPER_TARGET              := sawman-mrproper
SAWMAN_UNINSTALL_TARGET             := sawman-uninstall
SAWMAN_UNPACKED_FLAG                := ${SAWMAN_SRC_DIR}/.unpacked
SAWMAN_CONFIGURATION_FLAG           := ${SAWMAN_SRC_DIR}/.configuration
SAWMAN_CONFIGURED_FLAG              := ${SAWMAN_SRC_DIR}/.configured
SAWMAN_COMPILED_FLAG                := ${SAWMAN_SRC_DIR}/.compiled
SAWMAN_INSTALLED_FLAG               := ${SAWMAN_INSTALL_ROOT_DIR}/.sawman_installed
SAWMAN_FLAG                         := ${SAWMAN_INSTALL_ROOT_DIR}/.sawman
EXTRA_CLEAN_TARGETS                 += ${SAWMAN_CLEAN_TARGET}
EXTRA_DISTCLEAN_TARGETS             += ${SAWMAN_DISTCLEAN_TARGET}
EXTRA_MRPROPER_TARGETS              += ${SAWMAN_MRPROPER_TARGET}
EXTRA_UNINSTALL_TARGETS             += ${SAWMAN_UNINSTALL_TARGET}
ifeq ($(BUILD_SAWMAN),y)
EXTRA_BUILD_TARGETS                 += ${SAWMAN_BUILD_TARGET}
EXTRA_CONFIGURED_TARGETS            += ${SAWMAN_CONFIGURED_FLAG}
EXTRA_COMPILED_TARGETS              += ${SAWMAN_COMPILED_FLAG}
EXTRA_INSTALLED_TARGETS             += ${SAWMAN_INSTALLED_FLAG}
EXTRA_TARGET_INSTALLED_TARGETS      += ${SAWMAN_FLAG}
endif

#
# ++DFB Specific Defines
#
PPDFB_VERSION                       ?= 1.4.2
PPDFB_MAJOR                         := $(shell echo ${PPDFB_VERSION} | cut -d. -f1)
PPDFB_MINOR                         := $(shell echo ${PPDFB_VERSION} | cut -d. -f2)
PPDFB_MICRO                         := $(shell echo ${PPDFB_VERSION} | cut -d. -f3)
PPDFB_SOURCE_TARBALL                := ++DFB-${PPDFB_VERSION}.tar.gz
PPDFB_TARGET_ROOT_DIR               ?= ${DIRECTFB_PREFIX}
PPDFB_TARGET_BIN_DIR                := ${PPDFB_TARGET_ROOT_DIR}/bin
PPDFB_TARGET_BIN_PPD_DIR            := ${PPDFB_TARGET_BIN_DIR}/++dfb
PPDFB_TARGET_BIN_PPD_VER_DIR        := ${PPDFB_TARGET_BIN_PPD_DIR}/${PPDFB_MAJOR}.${PPDFB_MINOR}
PPDFB_PUB_PPD_DIR                   := ${DIRECTFB_PUB_DIR}/++DFB
PPDFB_PUB_PPD_VER1_DIR              := ${PPDFB_PUB_PPD_DIR}/${PPDFB_MAJOR}.${PPDFB_MINOR}
PPDFB_PUB_PPD_VER2_DIR              := ${PPDFB_PUB_PPD_VER1_DIR}.${PPDFB_MICRO}
PPDFB_SRC_DIR                       := ${DIRECTFB_SRC_DIR}/++DFB-${PPDFB_VERSION}
PPDFB_INCLUDE_DIR                   := ${PPDFB_SRC_DIR}/include
PPDFB_INSTALL_DIR                   ?= ${DIRECTFB_INSTALL_DIR}
PPDFB_INSTALL_ROOT_DIR              := ${PPDFB_INSTALL_DIR}/${PPDFB_TARGET_ROOT_DIR}
PPDFB_INSTALL_BIN_DIR               := ${PPDFB_INSTALL_DIR}/${PPDFB_TARGET_BIN_DIR}
PPDFB_INSTALL_BIN_PPD_DIR           := ${PPDFB_INSTALL_DIR}/${PPDFB_TARGET_BIN_PPD_DIR}
PPDFB_INSTALL_BIN_PPD_VER_DIR       := ${PPDFB_INSTALL_DIR}/${PPDFB_TARGET_BIN_PPD_VER_DIR}
PPDFB_INSTALL_INCLUDE_DIR           := ${PPDFB_INSTALL_ROOT_DIR}/include
PPDFB_INSTALL_LIB_DIR               := ${PPDFB_INSTALL_ROOT_DIR}/lib
PPDFB_INSTALL_PKG_CONFIG_DIR        := ${PPDFB_INSTALL_LIB_DIR}/pkgconfig
PPDFB_PKG_CONFIG                    := ++dfb
PPDFB_CONFIG_FLAGS                  :=          \
    --prefix=${PPDFB_TARGET_ROOT_DIR}           \
    --bindir=${PPDFB_TARGET_BIN_PPD_VER_DIR}    \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)            \
    CXXFLAGS=-DDIRECTFB_ENABLE_DEPRECATED

PPDFB_BUILD_TARGET                  := ppdfb-all
PPDFB_CLEAN_TARGET                  := ppdfb-clean
PPDFB_DISTCLEAN_TARGET              := ppdfb-distclean
PPDFB_MRPROPER_TARGET               := ppdfb-mrproper
PPDFB_UNINSTALL_TARGET              := ppdfb-uninstall
PPDFB_UNPACKED_FLAG                 := ${PPDFB_SRC_DIR}/.unpacked
PPDFB_CONFIGURATION_FLAG            := ${PPDFB_SRC_DIR}/.configuration
PPDFB_CONFIGURED_FLAG               := ${PPDFB_SRC_DIR}/.configured
PPDFB_COMPILED_FLAG                 := ${PPDFB_SRC_DIR}/.compiled
PPDFB_INSTALLED_FLAG                := ${PPDFB_INSTALL_ROOT_DIR}/.ppdfb_installed
PPDFB_FLAG                          := ${PPDFB_INSTALL_ROOT_DIR}/.ppdfb
EXTRA_CLEAN_TARGETS                 += ${PPDFB_CLEAN_TARGET}
EXTRA_DISTCLEAN_TARGETS             += ${PPDFB_DISTCLEAN_TARGET}
EXTRA_MRPROPER_TARGETS              += ${PPDFB_MRPROPER_TARGET}
EXTRA_UNINSTALL_TARGETS             += ${PPDFB_UNINSTALL_TARGET}
ifeq ($(BUILD_PPDFB),y)
EXTRA_BUILD_TARGETS                 += ${PPDFB_BUILD_TARGET}
EXTRA_CONFIGURED_TARGETS            += ${PPDFB_CONFIGURED_FLAG}
EXTRA_COMPILED_TARGETS              += ${PPDFB_COMPILED_FLAG}
EXTRA_INSTALLED_TARGETS             += ${PPDFB_INSTALLED_FLAG}
EXTRA_TARGET_INSTALLED_TARGETS      += ${PPDFB_FLAG}
endif

#
# DiVine Specific Defines
#
DIVINE_VERSION                      ?= 0.4.0
DIVINE_MAJOR                        := $(shell echo ${DIVINE_VERSION} | cut -d. -f1)
DIVINE_MINOR                        := $(shell echo ${DIVINE_VERSION} | cut -d. -f2)
DIVINE_MICRO                        := $(shell echo ${DIVINE_VERSION} | cut -d. -f3)
DIVINE_SOURCE_TARBALL               := DiVine-${DIVINE_VERSION}.tar.gz
DIVINE_TARGET_ROOT_DIR              ?= ${DIRECTFB_PREFIX}
DIVINE_TARGET_BIN_DIR               := ${DIVINE_TARGET_ROOT_DIR}/bin
DIVINE_TARGET_BIN_DIV_DIR           := ${DIVINE_TARGET_BIN_DIR}/divine
DIVINE_TARGET_BIN_DIV_VER_DIR       := ${DIVINE_TARGET_BIN_DIV_DIR}/${DIVINE_MAJOR}.${DIVINE_MINOR}
DIVINE_PUB_DIV_DIR                  := ${DIRECTFB_PUB_DIR}/DiVine
DIVINE_PUB_DIV_VER1_DIR             := ${DIVINE_PUB_DIV_DIR}/${DIVINE_MAJOR}.${DIVINE_MINOR}
DIVINE_PUB_DIV_VER2_DIR             := ${DIVINE_PUB_DIV_VER1_DIR}.${DIVINE_MICRO}
DIVINE_SRC_DIR                      := ${DIRECTFB_SRC_DIR}/DiVine-${DIVINE_VERSION}
DIVINE_INCLUDE_DIR                  := ${DIVINE_SRC_DIR}/include
DIVINE_INSTALL_DIR                  ?= ${DIRECTFB_INSTALL_DIR}
DIVINE_INSTALL_ROOT_DIR             := ${DIVINE_INSTALL_DIR}/${DIVINE_TARGET_ROOT_DIR}
DIVINE_INSTALL_BIN_DIR              := ${DIVINE_INSTALL_DIR}/${DIVINE_TARGET_BIN_DIR}
DIVINE_INSTALL_BIN_DIV_DIR          := ${DIVINE_INSTALL_DIR}/${DIVINE_TARGET_BIN_DIV_DIR}
DIVINE_INSTALL_BIN_DIV_VER_DIR      := ${DIVINE_INSTALL_DIR}/${DIVINE_TARGET_BIN_DIV_VER_DIR}
DIVINE_INSTALL_INCLUDE_DIR          := ${DIVINE_INSTALL_ROOT_DIR}/include
DIVINE_INSTALL_LIB_DIR              := ${DIVINE_INSTALL_ROOT_DIR}/lib
DIVINE_INSTALL_PKG_CONFIG_DIR       := ${DIVINE_INSTALL_LIB_DIR}/pkgconfig
DIVINE_PKG_CONFIG                   := divine
DIVINE_CONFIG_FLAGS                 :=          \
    --prefix=${DIVINE_TARGET_ROOT_DIR}          \
    --bindir=${DIVINE_TARGET_BIN_DIV_VER_DIR}   \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)

DIVINE_BUILD_TARGET                 := divine-all
DIVINE_CLEAN_TARGET                 := divine-clean
DIVINE_DISTCLEAN_TARGET             := divine-distclean
DIVINE_MRPROPER_TARGET              := divine-mrproper
DIVINE_UNINSTALL_TARGET             := divine-uninstall
DIVINE_UNPACKED_FLAG                := ${DIVINE_SRC_DIR}/.unpacked
DIVINE_CONFIGURATION_FLAG           := ${DIVINE_SRC_DIR}/.configuration
DIVINE_CONFIGURED_FLAG              := ${DIVINE_SRC_DIR}/.configured
DIVINE_COMPILED_FLAG                := ${DIVINE_SRC_DIR}/.compiled
DIVINE_INSTALLED_FLAG               := ${DIVINE_INSTALL_ROOT_DIR}/.divine_installed
DIVINE_FLAG                         := ${DIVINE_INSTALL_ROOT_DIR}/.divine
EXTRA_CLEAN_TARGETS                 += ${DIVINE_CLEAN_TARGET}
EXTRA_DISTCLEAN_TARGETS             += ${DIVINE_DISTCLEAN_TARGET}
EXTRA_MRPROPER_TARGETS              += ${DIVINE_MRPROPER_TARGET}
EXTRA_UNINSTALL_TARGETS             += ${DIVINE_UNINSTALL_TARGET}
ifeq ($(BUILD_DIVINE),y)
EXTRA_BUILD_TARGETS                 += ${DIVINE_BUILD_TARGET}
EXTRA_CONFIGURED_TARGETS            += ${DIVINE_CONFIGURED_FLAG}
EXTRA_COMPILED_TARGETS              += ${DIVINE_COMPILED_FLAG}
EXTRA_INSTALLED_TARGETS             += ${DIVINE_INSTALLED_FLAG}
EXTRA_TARGET_INSTALLED_TARGETS      += ${DIVINE_FLAG}
endif

#
# Insignia Specific Defines
#
INSIGNIA_VERSION                      ?= 0.1.2
INSIGNIA_MAJOR                        := $(shell echo ${INSIGNIA_VERSION} | cut -d. -f1)
INSIGNIA_MINOR                        := $(shell echo ${INSIGNIA_VERSION} | cut -d. -f2)
INSIGNIA_MICRO                        := $(shell echo ${INSIGNIA_VERSION} | cut -d. -f3)
INSIGNIA_SOURCE_TARBALL               := Insignia-${INSIGNIA_VERSION}.tar.gz
INSIGNIA_TARGET_ROOT_DIR              ?= ${DIRECTFB_PREFIX}
INSIGNIA_TARGET_BIN_DIR               := ${INSIGNIA_TARGET_ROOT_DIR}/bin
INSIGNIA_TARGET_BIN_INS_DIR           := ${INSIGNIA_TARGET_BIN_DIR}/insignia
INSIGNIA_TARGET_BIN_INS_VER_DIR       := ${INSIGNIA_TARGET_BIN_INS_DIR}/${INSIGNIA_MAJOR}.${INSIGNIA_MINOR}
INSIGNIA_PUB_INS_DIR                  := ${DIRECTFB_PUB_DIR}/Insignia
INSIGNIA_PUB_INS_VER1_DIR             := ${INSIGNIA_PUB_INS_DIR}/${INSIGNIA_MAJOR}.${INSIGNIA_MINOR}
INSIGNIA_PUB_INS_VER2_DIR             := ${INSIGNIA_PUB_INS_VER1_DIR}.${INSIGNIA_MICRO}
INSIGNIA_SRC_DIR                      := ${DIRECTFB_SRC_DIR}/Insignia-${INSIGNIA_VERSION}
INSIGNIA_INCLUDE_DIR                  := ${INSIGNIA_SRC_DIR}/include
INSIGNIA_INSTALL_DIR                  ?= ${DIRECTFB_INSTALL_DIR}
INSIGNIA_INSTALL_ROOT_DIR             := ${INSIGNIA_INSTALL_DIR}/${INSIGNIA_TARGET_ROOT_DIR}
INSIGNIA_INSTALL_BIN_DIR              := ${INSIGNIA_INSTALL_DIR}/${INSIGNIA_TARGET_BIN_DIR}
INSIGNIA_INSTALL_BIN_INS_DIR          := ${INSIGNIA_INSTALL_DIR}/${INSIGNIA_TARGET_BIN_INS_DIR}
INSIGNIA_INSTALL_BIN_INS_VER_DIR      := ${INSIGNIA_INSTALL_DIR}/${INSIGNIA_TARGET_BIN_INS_VER_DIR}
INSIGNIA_INSTALL_INCLUDE_DIR          := ${INSIGNIA_INSTALL_ROOT_DIR}/include
INSIGNIA_INSTALL_LIB_DIR              := ${INSIGNIA_INSTALL_ROOT_DIR}/lib
INSIGNIA_INSTALL_PKG_CONFIG_DIR       := ${INSIGNIA_INSTALL_LIB_DIR}/pkgconfig
INSIGNIA_CONFIG_FLAGS                 :=          \
    --prefix=${INSIGNIA_TARGET_ROOT_DIR}          \
    --bindir=${INSIGNIA_TARGET_BIN_INS_VER_DIR}   \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)

INSIGNIA_BUILD_TARGET                 := insignia-all
INSIGNIA_CLEAN_TARGET                 := insignia-clean
INSIGNIA_DISTCLEAN_TARGET             := insignia-distclean
INSIGNIA_MRPROPER_TARGET              := insignia-mrproper
INSIGNIA_UNINSTALL_TARGET             := insignia-uninstall
INSIGNIA_UNPACKED_FLAG                := ${INSIGNIA_SRC_DIR}/.unpacked
INSIGNIA_CONFIGURATION_FLAG           := ${INSIGNIA_SRC_DIR}/.configuration
INSIGNIA_CONFIGURED_FLAG              := ${INSIGNIA_SRC_DIR}/.configured
INSIGNIA_COMPILED_FLAG                := ${INSIGNIA_SRC_DIR}/.compiled
INSIGNIA_INSTALLED_FLAG               := ${INSIGNIA_INSTALL_ROOT_DIR}/.insignia_installed
INSIGNIA_FLAG                         := ${INSIGNIA_INSTALL_ROOT_DIR}/.insignia
ifeq ($(BUILD_INSIGNIA),y)
EXTRA_CLEAN_TARGETS                   += ${INSIGNIA_CLEAN_TARGET}
EXTRA_DISTCLEAN_TARGETS               += ${INSIGNIA_DISTCLEAN_TARGET}
EXTRA_MRPROPER_TARGETS                += ${INSIGNIA_MRPROPER_TARGET}
EXTRA_UNINSTALL_TARGETS               += ${INSIGNIA_UNINSTALL_TARGET}
EXTRA_BUILD_TARGETS                   += ${INSIGNIA_BUILD_TARGET}
EXTRA_CONFIGURED_TARGETS              += ${INSIGNIA_CONFIGURED_FLAG}
EXTRA_COMPILED_TARGETS                += ${INSIGNIA_COMPILED_FLAG}
EXTRA_INSTALLED_TARGETS               += ${INSIGNIA_INSTALLED_FLAG}
EXTRA_TARGET_INSTALLED_TARGETS        += ${INSIGNIA_FLAG}
endif

#
# Tacho Specific Defines
#
TACHO_VERSION                      ?= 0.1.2
TACHO_MAJOR                        := $(shell echo ${TACHO_VERSION} | cut -d. -f1)
TACHO_MINOR                        := $(shell echo ${TACHO_VERSION} | cut -d. -f2)
TACHO_MICRO                        := $(shell echo ${TACHO_VERSION} | cut -d. -f3)
TACHO_SOURCE_TARBALL               := Tacho-${TACHO_VERSION}.tar.gz
TACHO_TARGET_ROOT_DIR              ?= ${DIRECTFB_PREFIX}
TACHO_TARGET_BIN_DIR               := ${TACHO_TARGET_ROOT_DIR}/bin
TACHO_TARGET_BIN_TAC_DIR           := ${TACHO_TARGET_BIN_DIR}/tacho
TACHO_TARGET_BIN_TAC_VER_DIR       := ${TACHO_TARGET_BIN_TAC_DIR}/${TACHO_MAJOR}.${TACHO_MINOR}
TACHO_PUB_TAC_DIR                  := ${DIRECTFB_PUB_DIR}/Tacho
TACHO_PUB_TAC_VER1_DIR             := ${TACHO_PUB_TAC_DIR}/${TACHO_MAJOR}.${TACHO_MINOR}
TACHO_PUB_TAC_VER2_DIR             := ${TACHO_PUB_TAC_VER1_DIR}.${TACHO_MICRO}
TACHO_SRC_DIR                      := ${DIRECTFB_SRC_DIR}/Tacho-${TACHO_VERSION}
TACHO_INCLUDE_DIR                  := ${TACHO_SRC_DIR}/include
TACHO_INSTALL_DIR                  ?= ${DIRECTFB_INSTALL_DIR}
TACHO_INSTALL_ROOT_DIR             := ${TACHO_INSTALL_DIR}/${TACHO_TARGET_ROOT_DIR}
TACHO_INSTALL_BIN_DIR              := ${TACHO_INSTALL_DIR}/${TACHO_TARGET_BIN_DIR}
TACHO_INSTALL_BIN_TAC_DIR          := ${TACHO_INSTALL_DIR}/${TACHO_TARGET_BIN_TAC_DIR}
TACHO_INSTALL_BIN_TAC_VER_DIR      := ${TACHO_INSTALL_DIR}/${TACHO_TARGET_BIN_TAC_VER_DIR}
TACHO_INSTALL_INCLUDE_DIR          := ${TACHO_INSTALL_ROOT_DIR}/include
TACHO_INSTALL_LIB_DIR              := ${TACHO_INSTALL_ROOT_DIR}/lib
TACHO_INSTALL_PKG_CONFIG_DIR       := ${TACHO_INSTALL_LIB_DIR}/pkgconfig
TACHO_CONFIG_FLAGS                 :=          \
    --prefix=${TACHO_TARGET_ROOT_DIR}          \
    --bindir=${TACHO_TARGET_BIN_TAC_VER_DIR}   \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)

TACHO_BUILD_TARGET                 := tacho-all
TACHO_CLEAN_TARGET                 := tacho-clean
TACHO_DISTCLEAN_TARGET             := tacho-distclean
TACHO_MRPROPER_TARGET              := tacho-mrproper
TACHO_UNINSTALL_TARGET             := tacho-uninstall
TACHO_UNPACKED_FLAG                := ${TACHO_SRC_DIR}/.unpacked
TACHO_CONFIGURATION_FLAG           := ${TACHO_SRC_DIR}/.configuration
TACHO_CONFIGURED_FLAG              := ${TACHO_SRC_DIR}/.configured
TACHO_COMPILED_FLAG                := ${TACHO_SRC_DIR}/.compiled
TACHO_INSTALLED_FLAG               := ${TACHO_INSTALL_ROOT_DIR}/.tacho_installed
TACHO_FLAG                         := ${TACHO_INSTALL_ROOT_DIR}/.tacho
ifeq ($(BUILD_TACHO),y)
EXTRA_CLEAN_TARGETS                += ${TACHO_CLEAN_TARGET}
EXTRA_DISTCLEAN_TARGETS            += ${TACHO_DISTCLEAN_TARGET}
EXTRA_MRPROPER_TARGETS             += ${TACHO_MRPROPER_TARGET}
EXTRA_UNINSTALL_TARGETS            += ${TACHO_UNINSTALL_TARGET}
EXTRA_BUILD_TARGETS                += ${TACHO_BUILD_TARGET}
EXTRA_CONFIGURED_TARGETS           += ${TACHO_CONFIGURED_FLAG}
EXTRA_COMPILED_TARGETS             += ${TACHO_COMPILED_FLAG}
EXTRA_INSTALLED_TARGETS            += ${TACHO_INSTALLED_FLAG}
EXTRA_TARGET_INSTALLED_TARGETS     += ${TACHO_FLAG}
endif

#
# FFmpeg Specific Defines
#
FFMPEG_VERSION                      ?= 0.10.3
FFMPEG_MAJOR                        := $(shell echo ${FFMPEG_VERSION} | cut -d. -f1)
FFMPEG_MINOR                        := $(shell echo ${FFMPEG_VERSION} | cut -d. -f2)
FFMPEG_MICRO                        := $(shell echo ${FFMPEG_VERSION} | cut -d. -f3)
FFMPEG_TOP                          := ${OPENSOURCE_TOP}/ffmpeg
FFMPEG_SOURCE_TARBALL               := ${FFMPEG_TOP}/ffmpeg-${FFMPEG_VERSION}.tar.bz2
FFMPEG_TARGET_ROOT_DIR              ?= ${DIRECTFB_PREFIX}
FFMPEG_SRC_DIR                      := ${FFMPEG_TOP}/ffmpeg-${FFMPEG_VERSION}
FFMPEG_INCLUDE_DIR                  := ${FFMPEG_SRC_DIR}/include
FFMPEG_INSTALL_DIR                  ?= ${DIRECTFB_OBJECT_DIR}
FFMPEG_INSTALL_ROOT_DIR             := ${FFMPEG_INSTALL_DIR}
FFMPEG_INSTALL_BIN_DIR              := ${FFMPEG_INSTALL_ROOT_DIR}/bin
FFMPEG_INSTALL_INCLUDE_DIR          := ${FFMPEG_INSTALL_ROOT_DIR}/include
FFMPEG_INSTALL_LIB_DIR              := ${FFMPEG_INSTALL_ROOT_DIR}/lib
FFMPEG_INSTALL_PKG_CONFIG_DIR       := ${FFMPEG_INSTALL_LIB_DIR}/pkgconfig
FFMPEG_CONFIG_FLAGS                 :=          \
    --prefix=${FFMPEG_INSTALL_DIR}              \
    --bindir=${FFMPEG_INSTALL_BIN_DIR}          \
    --arch=$(B_REFSW_TOOLCHAIN_ARCH)            \
    --enable-cross-compile                      \
    --cross-prefix=mipsel-linux-                \
    --target-os=linux                           \
    --disable-everything                        \
    --enable-shared                             \
    --disable-static                            \
    --disable-doc                               \
    --enable-decoder=h264                       \
    --enable-decoder=mpeg1video                 \
    --enable-decoder=mpeg2video                 \
    --enable-demuxer=mpegvideo                  \
    --enable-demuxer=h264                       \
    --enable-parser=mpegvideo                   \
    --enable-parser=h264                        \
    --enable-protocol=file                      \
    --disable-swresample                        \
    --disable-ffmpeg                            \
    --disable-ffprobe                           \
    --disable-ffplay                            \
    --disable-postproc                          \
    --disable-avfilter                          \
    --disable-debug                             \
    --disable-network                           \
    --enable-small                              \
    --enable-fastdiv                            


FFMPEG_BUILD_TARGET                 := ffmpeg-all
FFMPEG_CLEAN_TARGET                 := ffmpeg-clean
FFMPEG_DISTCLEAN_TARGET             := ffmpeg-distclean
FFMPEG_MRPROPER_TARGET              := ffmpeg-mrproper
FFMPEG_UNINSTALL_TARGET             := ffmpeg-uninstall
FFMPEG_UNPACKED_FLAG                := ${FFMPEG_SRC_DIR}/.unpacked
FFMPEG_CONFIGURATION_FLAG           := ${FFMPEG_SRC_DIR}/.configuration
FFMPEG_CONFIGURED_FLAG              := ${FFMPEG_SRC_DIR}/.configured
FFMPEG_COMPILED_FLAG                := ${FFMPEG_SRC_DIR}/.compiled
FFMPEG_INSTALLED_FLAG               := ${FFMPEG_INSTALL_ROOT_DIR}/.ffmpeg_installed
FFMPEG_FLAG                         := ${FFMPEG_INSTALL_ROOT_DIR}/.ffmpeg
EXTRA_CLEAN_TARGETS                += ${FFMPEG_CLEAN_TARGET}
EXTRA_DISTCLEAN_TARGETS            += ${FFMPEG_DISTCLEAN_TARGET}
EXTRA_MRPROPER_TARGETS             += ${FFMPEG_MRPROPER_TARGET}
EXTRA_UNINSTALL_TARGETS            += ${FFMPEG_UNINSTALL_TARGET}
ifeq ($(BUILD_FFMPEG),y)
EXTRA_PRE_DFB_BUILD_TARGETS        += ${FFMPEG_BUILD_TARGET}
EXTRA_CONFIGURED_TARGETS           += ${FFMPEG_CONFIGURED_FLAG}
EXTRA_COMPILED_TARGETS             += ${FFMPEG_COMPILED_FLAG}
EXTRA_INSTALLED_TARGETS            += ${FFMPEG_INSTALLED_FLAG}
EXTRA_PRE_DFB_TARGET_INSTALLED_TARGETS     += ${FFMPEG_FLAG}
endif

##
#
# AppLibs/opensource/common directory
#
#APPLIBS_COMMON_INC                  := $(shell test "$(DIRECTFB_APPLIBS)" = "y" && test -e "$(OPENSOURCE_TOP)/common/common.inc" && echo "y" || echo "n")
# Temporarily disable this until after AppLibs 2.3...
APPLIBS_COMMON_INC                  := n

##
#
# FreeType directory
#
DFB_FREETYPE_APPLIBS                := $(shell test "$(DIRECTFB_APPLIBS)" = "y" && test -d "$(OPENSOURCE_TOP)/freetype" && echo "y" || echo "n")
ifeq ($(DFB_FREETYPE_APPLIBS),y)
DFB_FREETYPE_VERSION                ?= 2.4.9
DFB_FREETYPE_TOP                    := $(OPENSOURCE_TOP)/freetype
else
DFB_FREETYPE_VERSION                ?= 2.1.5
DFB_FREETYPE_TOP                    := $(BSEAV)/lib
endif
DFB_FREETYPE_MAJOR                  := $(shell echo ${DFB_FREETYPE_VERSION} | cut -d. -f1)
DFB_FREETYPE_MINOR                  := $(shell echo ${DFB_FREETYPE_VERSION} | cut -d. -f2)
DFB_FREETYPE_MICRO                  := $(shell echo ${DFB_FREETYPE_VERSION} | cut -d. -f3)
DFB_FREETYPE_SRC_DIR                := ${DFB_FREETYPE_TOP}/freetype-${DFB_FREETYPE_VERSION}
DFB_FREETYPE_SOURCE_TARBALL         := ${DFB_FREETYPE_TOP}/freetype-${DFB_FREETYPE_VERSION}.tar.gz
DFB_FREETYPE_INSTALL_DIR            ?= ${DIRECTFB_OBJECT_DIR}
DFB_FREETYPE_INSTALL_INCLUDE_DIR    := ${DFB_FREETYPE_INSTALL_DIR}/include
DFB_FREETYPE_INSTALL_LIB_DIR        := ${DFB_FREETYPE_INSTALL_DIR}/lib
DFB_FREETYPE_INSTALL_PKG_CONFIG_DIR := ${DFB_FREETYPE_INSTALL_LIB_DIR}/pkgconfig
DFB_FREETYPE_PKG_CONFIG             := freetype2
DFB_FREETYPE_UNPACKED_FLAG          := ${DFB_FREETYPE_SRC_DIR}/.unpacked
DFB_FREETYPE_CONFIGURATION_FLAG     := ${DFB_FREETYPE_SRC_DIR}/.configuration
DFB_FREETYPE_CONFIGURED_FLAG        := ${DFB_FREETYPE_SRC_DIR}/.configured
DFB_FREETYPE_COMPILED_FLAG          := ${DFB_FREETYPE_SRC_DIR}/.compiled
DFB_FREETYPE_INSTALLED_FLAG         := ${DFB_FREETYPE_INSTALL_DIR}/.freetype_installed
DFB_FREETYPE_FLAG                   := ${DFB_FREETYPE_INSTALL_DIR}/.freetype
DFB_FREETYPE_CONFIG_FLAGS           := --host=${B_REFSW_TOOLCHAIN_ARCH} --prefix=${DFB_FREETYPE_INSTALL_DIR}
ifeq ($(DIRECTFB_SHARED),y)
DFB_FREETYPE_CONFIG_FLAGS           += --enable-shared
else
DFB_FREETYPE_CONFIG_FLAGS           += --disable-shared
endif

##
#
# JPEG directory
#
DFB_JPEG_APPLIBS                    := $(shell test "$(DIRECTFB_APPLIBS)" = "y" && test -d "$(OPENSOURCE_TOP)/jpeg" && echo "y" || echo "n")
ifeq ($(DFB_JPEG_APPLIBS),y)
DFB_JPEG_VERSION                    ?= 8d
DFB_JPEG_TOP                        := $(OPENSOURCE_TOP)/jpeg
else
DFB_JPEG_VERSION                    ?= 6b
DFB_JPEG_TOP                        := $(BSEAV)/lib
endif
DFB_JPEG_MAJOR                      := $(shell echo ${DFB_JPEG_VERSION} | cut -d. -f1)
DFB_JPEG_MINOR                      := $(shell echo ${DFB_JPEG_VERSION} | cut -d. -f2)
DFB_JPEG_MICRO                      := $(shell echo ${DFB_JPEG_VERSION} | cut -d. -f3)
DFB_JPEG_SRC_DIR                    := ${DFB_JPEG_TOP}/jpeg-${DFB_JPEG_VERSION}
DFB_JPEG_SOURCE_TARBALL             := ${DFB_JPEG_TOP}/jpeg-${DFB_JPEG_VERSION}.tar.gz
DFB_JPEG_INSTALL_DIR                ?= ${DIRECTFB_OBJECT_DIR}
DFB_JPEG_INSTALL_INCLUDE_DIR        := ${DFB_JPEG_INSTALL_DIR}/include
DFB_JPEG_INSTALL_LIB_DIR            := ${DFB_JPEG_INSTALL_DIR}/lib
DFB_JPEG_UNPACKED_FLAG              := ${DFB_JPEG_SRC_DIR}/.unpacked
DFB_JPEG_CONFIGURATION_FLAG         := ${DFB_JPEG_SRC_DIR}/.configuration
DFB_JPEG_CONFIGURED_FLAG            := ${DFB_JPEG_SRC_DIR}/.configured
DFB_JPEG_COMPILED_FLAG              := ${DFB_JPEG_SRC_DIR}/.compiled
DFB_JPEG_INSTALLED_FLAG             := ${DFB_JPEG_INSTALL_DIR}/.jpeg_installed
DFB_JPEG_FLAG                       := ${DFB_JPEG_INSTALL_DIR}/.jpeg
DFB_JPEG_CONFIG_FLAGS               := --host=$(B_REFSW_TOOLCHAIN_ARCH) --prefix=${DFB_JPEG_INSTALL_DIR}
ifeq ($(DIRECTFB_SHARED),y)
DFB_JPEG_CONFIG_FLAGS               += --enable-shared
else
DFB_JPEG_CONFIG_FLAGS               += --disable-shared
endif

##
#
# PNG directory
#
DFB_PNG_APPLIBS                     := $(shell test "$(DIRECTFB_APPLIBS)" = "y" && test -d "$(OPENSOURCE_TOP)/libpng" && echo "y" || echo "n")
DFB_PNG_INSTALL_DIR                 ?= ${DIRECTFB_OBJECT_DIR}
ifeq ($(DFB_PNG_APPLIBS),y)
DFB_PNG_VERSION                     ?= 1.5.10
DFB_PNG_TOP                         := $(OPENSOURCE_TOP)/libpng
DFB_PNG_SRC_DIR                     := ${DFB_PNG_TOP}/libpng-${DFB_PNG_VERSION}
else
DFB_PNG_VERSION                     ?= 1.2.8
DFB_PNG_TOP                         := $(BSEAV)/lib
DFB_PNG_SRC_DIR                     := ${DFB_PNG_TOP}/libpng
DFB_PNG_INSTALL_OPTIONS             := prefix=${DFB_PNG_INSTALL_DIR}
endif
DFB_PNG_MAJOR                       := $(shell echo ${DFB_PNG_VERSION} | cut -d. -f1)
DFB_PNG_MINOR                       := $(shell echo ${DFB_PNG_VERSION} | cut -d. -f2)
DFB_PNG_MICRO                       := $(shell echo ${DFB_PNG_VERSION} | cut -d. -f3)
DFB_PNG_SOURCE_TARBALL              := ${DFB_PNG_TOP}/libpng-${DFB_PNG_VERSION}.tar.gz
DFB_PNG_INSTALL_INCLUDE_DIR         := ${DFB_PNG_INSTALL_DIR}/include
DFB_PNG_INSTALL_LIB_DIR             := ${DFB_PNG_INSTALL_DIR}/lib
DFB_PNG_INSTALL_PKG_CONFIG_DIR      := ${DFB_PNG_INSTALL_LIB_DIR}/pkgconfig
DFB_PNG_PKG_CONFIG                  := libpng12
DFB_PNG_UNPACKED_FLAG               := ${DFB_PNG_SRC_DIR}/.unpacked
DFB_PNG_CONFIGURATION_FLAG          := ${DFB_PNG_SRC_DIR}/.configuration
DFB_PNG_CONFIGURED_FLAG             := ${DFB_PNG_SRC_DIR}/.configured
DFB_PNG_COMPILED_FLAG               := ${DFB_PNG_SRC_DIR}/.compiled
DFB_PNG_INSTALLED_FLAG              := ${DFB_PNG_INSTALL_DIR}/.png_installed
DFB_PNG_FLAG                        := ${DFB_PNG_INSTALL_DIR}/.png
DFB_PNG_CONFIG_FLAGS                := --host=$(B_REFSW_TOOLCHAIN_ARCH) --prefix=${DFB_PNG_INSTALL_DIR}
ifeq ($(DIRECTFB_SHARED),y)
DFB_PNG_CONFIG_FLAGS                += --enable-shared
else
DFB_PNG_CONFIG_FLAGS                += --disable-shared
endif

##
#
# ZLIB directory
#
DFB_ZLIB_APPLIBS                    := $(shell test "$(DIRECTFB_APPLIBS)" = "y" && test -d "$(OPENSOURCE_TOP)/zlib" && echo "y" || echo "n")
ifeq ($(DFB_ZLIB_APPLIBS),y)
DFB_ZLIB_VERSION                    ?= 1.2.6
DFB_ZLIB_TOP                        := $(OPENSOURCE_TOP)/zlib
DFB_ZLIB_SRC_DIR                    := ${DFB_ZLIB_TOP}/zlib-${DFB_ZLIB_VERSION}
else
DFB_ZLIB_VERSION                    ?= 1.1.3
DFB_ZLIB_TOP                        := $(BSEAV)/lib
DFB_ZLIB_SRC_DIR                    := ${DFB_ZLIB_TOP}/zlib
ifeq ($(DIRECTFB_SHARED),y)
DFB_ZLIB_CONFIG_OPTIONS             := LDSHARED="${CC} -shared -Wl,-soname,libz.so.1"
endif
endif
DFB_ZLIB_MAJOR                      := $(shell echo ${DFB_ZLIB_VERSION} | cut -d. -f1)
DFB_ZLIB_MINOR                      := $(shell echo ${DFB_ZLIB_VERSION} | cut -d. -f2)
DFB_ZLIB_MICRO                      := $(shell echo ${DFB_ZLIB_VERSION} | cut -d. -f3)
DFB_ZLIB_SOURCE_TARBALL             := ${DFB_ZLIB_TOP}/zlib-${DFB_ZLIB_VERSION}.tar.gz
DFB_ZLIB_INSTALL_DIR                ?= ${DIRECTFB_OBJECT_DIR}
DFB_ZLIB_INSTALL_INCLUDE_DIR        := ${DFB_ZLIB_INSTALL_DIR}/include
DFB_ZLIB_INSTALL_LIB_DIR            := ${DFB_ZLIB_INSTALL_DIR}/lib
DFB_ZLIB_UNPACKED_FLAG              := ${DFB_ZLIB_SRC_DIR}/.unpacked
DFB_ZLIB_CONFIGURATION_FLAG         := ${DFB_ZLIB_SRC_DIR}/.configuration
DFB_ZLIB_CONFIGURED_FLAG            := ${DFB_ZLIB_SRC_DIR}/.configured
DFB_ZLIB_COMPILED_FLAG              := ${DFB_ZLIB_SRC_DIR}/.compiled
DFB_ZLIB_INSTALLED_FLAG             := ${DFB_ZLIB_INSTALL_DIR}/.zlib_installed
DFB_ZLIB_FLAG                       := ${DFB_ZLIB_INSTALL_DIR}/.zlib
DFB_ZLIB_CONFIG_FLAGS               := --prefix=${DFB_ZLIB_INSTALL_DIR}
ifeq ($(DIRECTFB_SHARED),y)
DFB_ZLIB_CONFIG_FLAGS               += --shared
else
DFB_ZLIB_CONFIG_FLAGS               += --static
endif

#
# DirectFB Examples Specific Defines
#
DIRECTFB_AUTOTEST_VERSION                   ?= 1.0.0
DIRECTFB_AUTOTEST_MAJOR                     := $(shell echo ${DIRECTFB_AUTOTEST_VERSION} | cut -d. -f1)
DIRECTFB_AUTOTEST_MINOR                     := $(shell echo ${DIRECTFB_AUTOTEST_VERSION} | cut -d. -f2)
DIRECTFB_AUTOTEST_MICRO                     := $(shell echo ${DIRECTFB_AUTOTEST_VERSION} | cut -d. -f3)
DIRECTFB_AUTOTEST_TARGET_ROOT_DIR           ?= ${DIRECTFB_PREFIX}
DIRECTFB_AUTOTEST_TARGET_BIN_DIR            := ${DIRECTFB_AUTOTEST_TARGET_ROOT_DIR}/bin
DIRECTFB_AUTOTEST_TARGET_BIN_DFB_DIR        := ${DIRECTFB_AUTOTEST_TARGET_BIN_DIR}/directfb/
DIRECTFB_AUTOTEST_TARGET_BIN_DFB_VER_DIR    := ${DIRECTFB_AUTOTEST_TARGET_BIN_DFB_DIR}/${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}
DIRECTFB_AUTOTEST_SRC_DIR                   := ${DIRECTFB_SRC_DIR}/DirectFB-Autotest
DIRECTFB_AUTOTEST_INCLUDE_DIR               := ${DIRECTFB_AUTOTEST_SRC_DIR}/include
DIRECTFB_AUTOTEST_CONFIG_FLAGS              :=              \
    --prefix=${DIRECTFB_AUTOTEST_TARGET_ROOT_DIR}           \
    --bindir=${DIRECTFB_AUTOTEST_TARGET_BIN_DFB_VER_DIR}    \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)

ifeq ($(B_REFSW_DEBUG),y)
DIRECTFB_AUTOTEST_CONFIG_FLAGS += --enable-debug
endif

DIRECTFB_AUTOTEST_BUILD_TARGET           := directfb-autotest-all
DIRECTFB_AUTOTEST_CLEAN_TARGET           := directfb-autotest-clean
DIRECTFB_AUTOTEST_UNINSTALL_TARGET       := directfb-autotest-uninstall
DIRECTFB_AUTOTEST_CONFIGURATION_FLAG     := ${DIRECTFB_AUTOTEST_SRC_DIR}/.configuration
DIRECTFB_AUTOTEST_CONFIGURED_FLAG        := ${DIRECTFB_AUTOTEST_SRC_DIR}/.configured
DIRECTFB_AUTOTEST_COMPILED_FLAG          := ${DIRECTFB_AUTOTEST_SRC_DIR}/.compiled
DIRECTFB_AUTOTEST_INSTALLED_FLAG         := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_autotest_installed
DIRECTFB_AUTOTEST_FLAG                   := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_autotest
EXTRA_CLEAN_TARGETS                      += ${DIRECTFB_AUTOTEST_CLEAN_TARGET}
EXTRA_UNINSTALL_TARGETS                  += ${DIRECTFB_AUTOTEST_UNINSTALL_TARGET}
ifeq ($(BUILD_AUTOMATION),y)
EXTRA_BUILD_TARGETS                      += ${DIRECTFB_AUTOTEST_BUILD_TARGET}
EXTRA_CONFIGURED_TARGETS                 += ${DIRECTFB_AUTOTEST_CONFIGURED_FLAG}
EXTRA_COMPILED_TARGETS                   += ${DIRECTFB_AUTOTEST_COMPILED_FLAG}
EXTRA_INSTALLED_TARGETS                  += ${DIRECTFB_AUTOTEST_INSTALLED_FLAG}
EXTRA_TARGET_INSTALLED_TARGETS           += ${DIRECTFB_AUTOTEST_FLAG}
endif

#
# DirectFB Broadcom Private Examples Specific Defines
#
DIRECTFB_BRCM_EXAMPLES_PRIVATE_VERSION  ?= 1.0.0
DIRECTFB_BRCM_EXAMPLES_PRIVATE_MAJOR    := $(shell echo ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_VERSION} | cut -d. -f1)
DIRECTFB_BRCM_EXAMPLES_PRIVATE_MINOR    := $(shell echo ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_VERSION} | cut -d. -f2)
DIRECTFB_BRCM_EXAMPLES_PRIVATE_MICRO                     := $(shell echo ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_VERSION} | cut -d. -f3)
DIRECTFB_BRCM_EXAMPLES_PRIVATE_TARGET_ROOT_DIR           ?= ${DIRECTFB_PREFIX}
DIRECTFB_BRCM_EXAMPLES_PRIVATE_TARGET_BIN_DIR            := ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_TARGET_ROOT_DIR}/bin
DIRECTFB_BRCM_EXAMPLES_PRIVATE_TARGET_BIN_DFB_DIR        := ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_TARGET_BIN_DIR}/directfb/
DIRECTFB_BRCM_EXAMPLES_PRIVATE_TARGET_BIN_DFB_VER_DIR    := ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_TARGET_BIN_DFB_DIR}/${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}
#DIRECTFB_BRCM_EXAMPLES_PRIVATE_SRC_DIR                   := ${DIRECTFB_SRC_DIR}/DirectFB-brcm-examples-private
DIRECTFB_BRCM_EXAMPLES_PRIVATE_SRC_DIR                   := ${OPENSOURCE_TOP}/directfb_internal/DirectFB-brcm-examples-private
DIRECTFB_BRCM_EXAMPLES_PRIVATE_INCLUDE_DIR               := ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_SRC_DIR}/include
DIRECTFB_BRCM_EXAMPLES_PRIVATE_CONFIG_FLAGS              :=              \
    --prefix=${DIRECTFB_BRCM_EXAMPLES_PRIVATE_TARGET_ROOT_DIR}           \
    --bindir=${DIRECTFB_BRCM_EXAMPLES_PRIVATE_TARGET_BIN_DFB_VER_DIR}    \
    --host=$(B_REFSW_TOOLCHAIN_ARCH)

ifeq ($(B_REFSW_DEBUG),y)
DIRECTFB_BRCM_EXAMPLES_PRIVATE_CONFIG_FLAGS += --enable-debug
endif

DIRECTFB_BRCM_EXAMPLES_PRIVATE_BUILD_TARGET           := directfb-brcm-examples-private-all
DIRECTFB_BRCM_EXAMPLES_PRIVATE_CLEAN_TARGET           := directfb-brcm-examples-private-clean
DIRECTFB_BRCM_EXAMPLES_PRIVATE_UNINSTALL_TARGET       := directfb-brcm-examples-private-uninstall
DIRECTFB_BRCM_EXAMPLES_PRIVATE_CONFIGURATION_FLAG     := ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_SRC_DIR}/.configuration
DIRECTFB_BRCM_EXAMPLES_PRIVATE_CONFIGURED_FLAG        := ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_SRC_DIR}/.configured
DIRECTFB_BRCM_EXAMPLES_PRIVATE_COMPILED_FLAG          := ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_SRC_DIR}/.compiled
DIRECTFB_BRCM_EXAMPLES_PRIVATE_INSTALLED_FLAG         := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_brcm-examples-private_installed
DIRECTFB_BRCM_EXAMPLES_PRIVATE_FLAG                   := ${DIRECTFB_INSTALL_ROOT_DIR}/.directfb_brcm-examples-private
ifeq ($(BUILD_PRIVATE_TESTS),y)
EXTRA_CLEAN_TARGETS                      += ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_CLEAN_TARGET}
EXTRA_UNINSTALL_TARGETS                  += ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_UNINSTALL_TARGET}
EXTRA_BUILD_TARGETS                      += ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_BUILD_TARGET}
EXTRA_CONFIGURED_TARGETS                 += ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_CONFIGURED_FLAG}
EXTRA_COMPILED_TARGETS                   += ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_COMPILED_FLAG}
EXTRA_INSTALLED_TARGETS                  += ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_INSTALLED_FLAG}
EXTRA_TARGET_INSTALLED_TARGETS           += ${DIRECTFB_BRCM_EXAMPLES_PRIVATE_FLAG}
endif

##
#
# Specify Nexus Flag
#
NEXUS_FLAG                          := ${DIRECTFB_OBJECT_DIR}/.nexus

##
#
# Specify the open source and Broadcom SLA files that need to be released
#
#
DIRECTFB_RELEASE_PATH := ${DIRECTFB_BUILD_DIR}/Makefile ${DIRECTFB_BUILD_DIR}/directfb_common.inc ${DIRECTFB_BUILD_DIR}/packages
DIRECTFB_RELEASE_PATH += ${DIRECTFB_DOCS_DIR} ${DIRECTFB_SRC_DFB_TAR_DIR} ${DIRECTFB_PUB_DIR}
DIRECTFB_RELEASE_PATH += $(wildcard ${DIRECTFB_BUILD_DIR}/${DIRECTFB_MAJOR}.*)

ifeq ($(DFB_FREETYPE_APPLIBS),y)
DIRECTFB_RELEASE_PATH += ${DFB_FREETYPE_SOURCE_TARBALL}
else
DIRECTFB_RELEASE_PATH += ${DFB_FREETYPE_SRC_DIR}
endif

ifeq ($(DFB_JPEG_APPLIBS),y)
DIRECTFB_RELEASE_PATH += ${DFB_JPEG_SOURCE_TARBALL}
else
DIRECTFB_RELEASE_PATH += ${DFB_JPEG_SRC_DIR}
endif

ifeq ($(DFB_PNG_APPLIBS),y)
DIRECTFB_RELEASE_PATH += ${DFB_PNG_SOURCE_TARBALL}
else
DIRECTFB_RELEASE_PATH += ${DFB_PNG_SRC_DIR}
endif

ifeq ($(DFB_ZLIB_APPLIBS),y)
DIRECTFB_RELEASE_PATH += ${DFB_ZLIB_SOURCE_TARBALL}
else
DIRECTFB_RELEASE_PATH += ${DFB_ZLIB_SRC_DIR}
endif

DIRECTFB_RELEASE_PATH += ${FFMPEG_SOURCE_TARBALL}

DIRECTFB_RELEASE_PATH += ${DIRECTFB_BRCM_DIR}
DIRECTFB_RELEASE_LIST := $(shell echo ${DIRECTFB_RELEASE_PATH} | sed -e "s;/[^ ]*AppLibs/;AppLibs/;g;s;/[^ ]*BSEAV/;BSEAV/;g")

