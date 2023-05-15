#############################################################################
#    (c)2009-2012 Broadcom Corporation
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
# $brcm_Revision: DirectFB_1_4_15_Port/6 $
# $brcm_Date: 3/7/12 5:25p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/gfxdrivers/bcmnexus/Makefile.am $
# 
# DirectFB_1_4_15_Port/6   3/7/12 5:25p christ
# SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
# 
# DirectFB_1_4_15_Port/5   2/24/12 2:58p robertwm
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/4   2/22/12 2:33p mnelis
# SW7425-2447: DirectFB: Move bcmnexus utils into platform layer
# 
# DirectFB_1_4_15_Port/3   2/9/12 2:34p christ
# SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
#  into DFB
# 
# DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   1/27/12 7:02p christ
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/2   11/11/11 7:17p robertwm
# SW7425-1740: DirectFB: Multi-app support in Nexus user mode.
# 
# DirectFB_1_4_14_Port/1   7/28/11 11:20a christ
# SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
# 
# DirectFB_1_4_5_Port/2   6/27/11 2:37p robertwm
# SW7405-5389: DirectFB: DFB Applications shuld not link directly with
#  nexus.
# 
# DirectFB_1_4_5_Port/1   6/16/11 3:08p christ
# SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
# 
# DirectFB_1_4_Port/7   6/27/11 2:35p robertwm
# SW7405-5389: DirectFB: DFB Applications shuld not link directly with
#  nexus.
# 
# DirectFB_1_4_Port/6   10/28/10 12:15p robertwm
# SW35230-611: DirectFB: Changes to incorporate building nexus shim into
#  Makefile.
# 
# DirectFB_1_4_Port/5   8/7/10 12:16p robertwm
# SW7405-4728: DirectFB: Change name of bcmnexus graphics and system
#  shared libraries.
# 
# DirectFB_1_4_Port/4   3/2/10 10:38a robertwm
# SW3556-1069: DirectFB: Run-time dynamic linking between gfxdriver and
#  system drivers not working.
# 
# DirectFB_1_4_Port/3   2/18/10 12:24p robertwm
# SW7400-2695: DirectFB: Linking system shared lib against graphics
#  shared lib uses incorrect libtool flags.
# 
# DirectFB_1_4_Port/2   12/23/09 2:19p robertwm
# SW7550-51: Support DirectFB on 7550.
# 
# DirectFB_1_4_Port/1   7/8/09 3:18p mphillip
# PR56658: Add DirectFB 1.4.1 broadcom_files
# 
# 2   3/26/09 10:57a vsilyaev
# PR 53611: Use simply expanded variables everywhere where $(shell ...)
#  is used. It prevents evaluation of $(shell ) every where variable is
#  used
# 
#############################################################################
## Makefile.am for DirectFB/src/core/gfxcards/bcmnexus

PLATFORM_PATH = $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')

INCLUDES = \
    -I$(top_srcdir)/include         \
    -I$(top_builddir)/include       \
    -I$(top_builddir)/lib           \
    -I$(top_srcdir)/lib             \
    -I$(top_srcdir)/src             \
    -I$(top_srcdir)/platform        \
    -I$(top_srcdir)/systems/bcmnexus

AM_CPPFLAGS = ${BRCM_CFLAGS} 

internalincludedir = $(INTERNALINCLUDEDIR)/bcmnexus

internalinclude_HEADERS = \
    bcmnexus_gfx.h

GFX_SRCS = \
    bcmnexus_gfx.c

######## Nexus master lib ########

if DIRECTFB_MASTER_LIB
bcmnexus_gfx_LTLIBRARIES = libdirectfb_bcmnexus_gfx.la
bcmnexus_gfxdir = $(MODULEDIR)/gfxdrivers

if BUILD_STATIC
bcmnexus_gfx_DATA = $(bcmnexus_gfx_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_gfx_la_SOURCES = $(GFX_SRCS)

libdirectfb_bcmnexus_gfx_la_LDFLAGS = \
    -module           \
    -avoid-version    \
    -lnexus           \
    -Wl,-rpath,$(MODULEDIR)/systems \
    $(DFB_LDFLAGS)

libdirectfb_bcmnexus_gfx_la_LIBADD = \
     $(top_builddir)/platform/libinit.la \
    $(top_builddir)/systems/bcmnexus/libdirectfb_bcmnexus_sys.la

endif

######## Nexus client lib ########
if DIRECTFB_CLIENT_LIB
bcmnexus_gfx_client_LTLIBRARIES = libdirectfb_bcmnexus_gfx_client.la
bcmnexus_gfx_clientdir = $(MODULECLIENTDIR)/gfxdrivers

if BUILD_STATIC
bcmnexus_gfx_client_DATA = $(bcmnexus_gfx_client_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_gfx_client_la_SOURCES = $(GFX_SRCS)

libdirectfb_bcmnexus_gfx_client_la_LDFLAGS = \
    -module                                  \
    -avoid-version                           \
    -lnexus$(NEXUS_CLIENT_LIB_SUFFIX)        \
    -linit_client                            \
    -Wl,-rpath,$(MODULECLIENTDIR)/systems    \
    $(DFB_LDFLAGS)

libdirectfb_bcmnexus_gfx_client_la_LIBADD = \
    $(top_builddir)/systems/bcmnexus/libdirectfb_bcmnexus_sys_client.la
endif


include $(top_srcdir)/rules/libobject.make

