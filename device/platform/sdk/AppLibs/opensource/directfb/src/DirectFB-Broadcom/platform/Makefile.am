#############################################################################
#    (c)2009-2011 Broadcom Corporation
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
# $brcm_Workfile: Makefile.am $
# $brcm_Revision: DirectFB_1_4_14_Port/1 $
# $brcm_Date: 7/28/11 11:23a $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/Makefile.am $
# 
# 
#############################################################################
## Makefile.am for DirectFB/platform

INCLUDES = \
	-I$(top_srcdir)/include             \
	-I$(top_builddir)/include           \
	-I$(top_srcdir)/systems/bcmnexus    \
	-I$(top_srcdir)/gfxdrivers/bcmnexus \
	-I$(NEXUS_TOP)/lib/ipc               \
	-I$(top_srcdir)/platform/stubs       \
	-I.

AM_CPPFLAGS =  ${BRCM_CFLAGS}

if DIRECTFB_MASTER_LIB
lib_LTLIBRARIES = libinit.la
endif

if BCMNEXUS_GLES
# only for proxy mode
BCMNEXUS_PROXY_3D = \
    p3d_nexus_proxy.c
else
BCMNEXUS_PROXY_3D = 
endif

if DIRECTFB_NSC_SUPPORT
DISPLAY_BACKEND_FILES = platform_nexus_surface_compositor_display.c
else
DISPLAY_BACKEND_FILES = platform_nexus_display.c 
endif


if DIRECTFB_NIR_SUPPORT
INPUT_BACKEND_FILES = platform_nexus_input_router.c
else
INPUT_BACKEND_FILES = platform_nexus_input.c
endif


libinit_la_SOURCES = \
    platform_init.c  \
    platform_nexus_init.c \
    $(DISPLAY_BACKEND_FILES) \
    $(INPUT_BACKEND_FILES) \
    $(BCMNEXUS_PROXY_3D)

if DIRECTFB_MULTI
libinit_la_SOURCES += platform_ipc_server.c $(NEXUS_TOP)/lib/ipc/bipc_util.c $(NEXUS_TOP)/lib/ipc/bipc_server.c $(NEXUS_TOP)/lib/ipc/bipc_client.c
endif
libinit_la_LDFLAGS = \
    -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
    -release $(LT_RELEASE)                               \
    -lnexus                                              \
    $(DFB_LDFLAGS)

libinit_la_CFLAGS = \
    -DDIRECTFB_PREFIX=\"${DIRECTFB_PREFIX}\"  \
    -DDIRECTFB_GFX_3D_DRIV_LIB=\"${DIRECTFB_GFX_3D_DRIV_LIB}\"


######## Nexus client lib ########
if DIRECTFB_CLIENT_LIB
libinit_client_LTLIBRARIES = libinit_client.la
libinit_clientdir = $(MODULECLIENTDIR)/../

if BUILD_STATIC
libinit_client_DATA = $(libinit_client_LTLIBRARIES:.la=.o)
endif

libinit_client_la_SOURCES = $(libinit_la_SOURCES)

libinit_client_la_CFLAGS= $(libinit_la_CFLAGS) -DBCMNEXUS_CLIENT_LIBRARY

libinit_client_la_LDFLAGS = \
    -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
    -release $(LT_RELEASE)                               \
    -lnexus$(NEXUS_CLIENT_LIB_SUFFIX)                    \
    $(DFB_LDFLAGS)

endif


distclean-local:
	rm -f bcmnexus_ir_convert.h
	rm -f bcmnexus_ir_inputmode.h
	rm -fr stubs
include $(top_srcdir)/rules/libobject.make
