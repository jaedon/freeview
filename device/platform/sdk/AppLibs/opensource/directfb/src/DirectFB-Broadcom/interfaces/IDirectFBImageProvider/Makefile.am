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
# $brcm_Revision: DirectFB_1_4_15_Port/9 $
# $brcm_Date: 3/7/12 5:25p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/interfaces/IDirectFBImageProvider/Makefile.am $
# 
# DirectFB_1_4_15_Port/9   3/7/12 5:25p christ
# SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
# 
# DirectFB_1_4_15_Port/8   2/24/12 3:05p robertwm
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/7   2/22/12 2:35p mnelis
# SW7425-2447: DirectFB: Move bcmnexus utils into platform layer
# 
# DirectFB_1_4_15_Port/6   2/13/12 6:03p christ
# SW7425-2333: DirectFB: Add FFmpeg imageprovider support for AVC frame
#  decode
# 
# DirectFB_1_4_15_Port/5   2/9/12 2:34p christ
# SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
#  into DFB
# 
# DirectFB_1_4_15_Port/4   2/7/12 3:58p mnelis
# SW7425-2333: DirectFB: Add FFmpeg imageprovider support for AVC frame
#  decode
# 
# DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   1/27/12 7:02p christ
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/3   11/11/11 7:23p robertwm
# SW7425-1740: DirectFB: Multi-app support in Nexus user mode.
# 
# DirectFB_1_4_15_Port/2   9/29/11 2:11p christ
# SW7425-1360: DirectFB: Merge in upstream DirectFB 1.4.15 changes.
# 
# DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
# SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
# 
# DirectFB_1_4_5_Port/3   6/27/11 1:15p apandav
# SWDTV-7694: DirectFB: Add support for BMP image provider
# 
# DirectFB_1_4_5_Port/2   6/16/11 6:19p christ
# SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
# 
# DirectFB_1_4_5_Port/1   6/14/11 5:48p christ
# SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
# 
# DirectFB_1_4_Port/4   10/7/10 2:03p robertwm
# SW7405-4906: DirectFB: Need to make a DirectFB-1.4.5 Release
# 
# DirectFB_1_4_Port/3   3/2/10 10:39a robertwm
# SW3556-1069: DirectFB: Run-time dynamic linking between gfxdriver and
#  system drivers not working.
# 
# DirectFB_1_4_Port/2   1/14/10 6:54p robertwm
# SW3556-996: DirectFB: Implement ImageProvider for DirectFB-1.4.1 using
#  SID.
# 
# DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
# PR56658: Add DirectFB 1.4.1 broadcom_files
# 
#############################################################################
## Makefile.am for DirectFB/interfaces/IDirectFBImageProvider

idirectfbimageproviderdir = $(MODULEDIR)/interfaces/IDirectFBImageProvider

if FFMPEG_PROVIDER
FFMPEG_PROVIDER_LTLIB = libidirectfbimageprovider_ffmpeg.la
else
FFMPEG_PROVIDER_LTLIB = 
endif

if SID_PROVIDER
NEXUS_PROVIDER_LTLIB                   = libidirectfbimageprovider_nexus.la
NEXUS_CLIENT_PROVIDER_LTLIB            = libidirectfbimageprovider_nexus_client.la
else
NEXUS_PROVIDER_LTLIB                   = 
NEXUS_CLIENT_PROVIDER_LTLIB            =
endif

INCLUDES = \
	-I$(top_builddir)/include           \
	-I$(top_builddir)/lib	            \
	-I$(top_builddir)/src	            \
	-I$(top_srcdir)/include             \
	-I$(top_srcdir)/lib                 \
	-I$(top_srcdir)/src                 \
	-I$(top_srcdir)/platform            \
	-I$(top_srcdir)/gfxdrivers/bcmnexus \
	-I$(top_srcdir)/systems/bcmnexus

AM_CPPFLAGS = -DDATADIR=\"${RUNTIME_SYSROOT}@DATADIR@\" ${BRCM_CFLAGS}

idirectfbimageprovider_LTLIBRARIES = \
	$(FFMPEG_PROVIDER_LTLIB)


if BUILD_STATIC
idirectfbimageprovider_DATA = $(idirectfbimageprovider_LTLIBRARIES:.la=.o)
endif

libidirectfbimageprovider_ffmpeg_la_SOURCES = idirectfbimageprovider_ffmpeg.c

libidirectfbimageprovider_ffmpeg_la_LDFLAGS = -avoid-version -module -lavformat -lavutil -lavcodec -lswscale


if SID_PROVIDER

######## Nexus master lib ########

if DIRECTFB_MASTER_LIB
idirectfbimageprovider_nexus_LTLIBRARIES = \
	$(NEXUS_PROVIDER_LTLIB)

idirectfbimageprovider_nexusdir = $(MODULEDIR)/interfaces/IDirectFBImageProvider

if BUILD_STATIC
idirectfbimageprovider_nexus_DATA = $(idirectfbimageprovider_nexus_LTLIBRARIES:.la=.o)
endif

libidirectfbimageprovider_nexus_la_SOURCES = idirectfbimageprovider_nexus.c

libidirectfbimageprovider_nexus_la_LIBADD = \
	$(top_builddir)/platform/libinit.la \
	$(top_builddir)/systems/bcmnexus/libdirectfb_bcmnexus_sys.la 

libidirectfbimageprovider_nexus_la_LDFLAGS = -avoid-version -module -lnexus
endif

######## Nexus client lib ########
if DIRECTFB_CLIENT_LIB
idirectfbimageprovider_nexus_client_LTLIBRARIES = \
	$(NEXUS_CLIENT_PROVIDER_LTLIB)

if BUILD_STATIC
idirectfbimageprovider_nexus_client_DATA = $(idirectfbimageprovider_nexus_client_LTLIBRARIES:.la=.o)
endif

idirectfbimageprovider_nexus_clientdir = $(MODULECLIENTDIR)/interfaces/IDirectFBImageProvider

libidirectfbimageprovider_nexus_client_la_SOURCES = idirectfbimageprovider_nexus.c

libidirectfbimageprovider_nexus_client_la_LIBADD = \
	$(top_builddir)/platform/libinit_client.la \
	$(top_builddir)/systems/bcmnexus/libdirectfb_bcmnexus_sys_client.la

libidirectfbimageprovider_nexus_client_la_LDFLAGS = -avoid-version -module -lnexus$(NEXUS_CLIENT_LIB_SUFFIX)
endif

endif

include $(top_srcdir)/rules/libobject.make
