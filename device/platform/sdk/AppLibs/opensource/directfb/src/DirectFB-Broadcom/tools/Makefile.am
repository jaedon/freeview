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
# $brcm_Revision: DirectFB_1_4_15_Port/14 $
# $brcm_Date: 3/15/12 2:06p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/tools/Makefile.am $
# 
# DirectFB_1_4_15_Port/14   3/15/12 2:06p christ
# SW7231-719: DirectFB-1.4.15: Single App build with userspace Nexus
#  drivers fails to link
# 
# DirectFB_1_4_15_Port/13   3/7/12 5:27p christ
# SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
# 
# DirectFB_1_4_15_Port/12   2/27/12 1:13p robertwm
# SW7231-690: DirectFB-1.4.15: Add decode server/client test to help with
#  DFB platform and Nexus MP usage.
# 
# DirectFB_1_4_15_Port/11   2/24/12 9:56p robertwm
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/10   2/24/12 9:29p robertwm
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/9   2/24/12 3:13p robertwm
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/8   1/24/12 12:23p christ
# SW7231-607: DirectFB: Update BRCM tree to support X11 builds.
# 
# DirectFB_1_4_15_Port/7   11/7/11 8:27p scchang
# SW7420-2122: DirectFB: USB hotplug support
# 
# DirectFB_1_4_15_Port/6   10/28/11 4:18p christ
# SW7425-1128: DirectFB: Add extra stereoscopic test applications
# 
# DirectFB_1_4_15_Port/5   10/19/11 3:38p christ
# SW7420-2102: DirectFB: Require example DFB multi-app test applications
#  with video
# 
# DirectFB_1_4_15_Port/3   10/13/11 11:40a robertwm
# SW7231-396: DirectFB: Add pre-allocation of memory test to DFB.
# 
# DirectFB_1_4_15_Port/2   9/29/11 2:11p christ
# SW7425-1360: DirectFB: Merge in upstream DirectFB 1.4.15 changes.
# 
# DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
# SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
# 
# DirectFB_1_4_5_Port/3   6/22/11 4:21p christ
# SW7405-5380: DirectFB: Cannot autogenerate the Makefile.in files.
# 
# DirectFB_1_4_5_Port/2   6/17/11 3:04p christ
# SW7420-1943: DirectFB: Texture triangles and setmatrix software
#  integration for DFB1.4.5
# 
# DirectFB_1_4_5_Port/1   6/14/11 5:09p christ
# SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
# 
# DirectFB_1_4_Port/DirectFB_1_4_1/1   6/10/11 5:21p christ
# SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
#  display.
# 
# DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   4/12/11 6:39p tross
# Added df_andi3d
# 
# DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/3/11 2:23p tross
# Remove building of df_brcmTest.c from Bluray platforms.
# 
# DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:20p tross
# Merge in Bluray support.
# 
# DirectFB_1_4_Port/11   5/20/11 6:09p scchang
# SW7420-1890: DirectFB: Texture triangles and setmatrix software
#  integration
# 
# DirectFB_1_4_Port/10   2/16/11 1:49p robertwm
# SW7422-265: DirectFB: Remove circular dependency when building OGL2.0
#  V3D applications.
# 
# DirectFB_1_4_Port/9   12/14/10 2:40p robertwm
# SW7422-140: DirectFB: Support OpenGL ES 2.0 applications on the VC-4
#  core.
# 
# DirectFB_1_4_Port/8   12/14/10 11:12a robertwm
# SW7422-140: DirectFB: Support OpenGL ES 2.0 applications on the VC-4
#  core.
# 
# DirectFB_1_4_Port/7   10/14/10 3:48p robertwm
# SW7405-4906: DirectFB: Need to make a DirectFB-1.4.5 Release.
# 
# DirectFB_1_4_Port/6   8/9/10 8:42p robertwm
# SW7405-4183: DirectFB: Add support for Texture Triangles in the
#  graphics driver.
# 
# DirectFB_1_4_Port/5   4/13/10 3:05p robertwm
# SW7405-4199: DirectFB: Define FONT in Makefiles rather than in
#  individual applications.
# SW7405-4198: DirectFB: Add videoAndGfxLayersTest.c to tools dir.
# 
# DirectFB_1_4_Port/4   4/13/10 1:16p robertwm
# SW7405-4198: DirectFB: Add videoAndGfxLayersTest.c to tools dir.
# 
# DirectFB_1_4_Port/3   2/19/10 12:22p robertwm
# SW7405-3943: DirectFB: Depopulate DirectFB target installation root.
# 
# DirectFB_1_4_Port/2   12/4/09 1:11p robertwm
# SW7405-3547: DirectFB: Create test harness for exercising all drawing
#  and blitting combinations.
# 
# DirectFB_1_4_Port/1   7/8/09 3:20p mphillip
# PR56658: Add DirectFB 1.4.1 broadcom_files
# 
#############################################################################
## Makefile.am for DirectFB/tools

INCLUDES = \
	-I$(top_builddir)/include \
	-I$(top_builddir)/lib     \
	-I$(top_builddir)/src     \
	-I$(top_srcdir)/include	  \
	-I$(top_srcdir)/lib	  \
	-I$(top_srcdir)/src       \
	-I$(top_srcdir)/platform \
	-I$(top_srcdir)/systems/bcmnexus


AM_CPPFLAGS = -DDATADIR=\"${RUNTIME_SYSROOT}@DATADIR@\" \
              -DFONTDIR=\"${RUNTIME_SYSROOT}@DATADIR@/fonts\" \
              -DFONT=\"${RUNTIME_SYSROOT}@DATADIR@/fonts/decker.ttf\"


AM_CFLAGS = ${BRCM_CFLAGS} $(FREETYPE_CFLAGS) $(LIBPNG_CFLAGS) \
	    -DDATADIR=\"${RUNTIME_SYSROOT}@DATADIR@\" \
            -DFONTDIR=\"${RUNTIME_SYSROOT}@DATADIR@/fonts\" \
            -DFONT=\"${RUNTIME_SYSROOT}@DATADIR@/fonts/decker.ttf\" \
	     -I$(NEXUS_TOP)/examples/multiprocess -I$(NEXUS_TOP)/lib/ipc -Istubs

AM_LDFLAGS = ${DFB_LIBS}
#AM_CXXFLAGS = $(LIBPNG_CFLAGS)

BRCM_DF_BRCM_TEST = df_brcmTest
BRCM_WINDOW_PREALLOC = df_window_prealloc

if DIRECTFB_MASTER_LIB

if DIRECTFB_NSC_SUPPORT
BRCM_SERVER_TESTS = nxsmaster
BRCM_PLAYBACK_CLIENT_TESTS = 
else
if DIRECTFB_MULTI
# Kernel mode only have master lib 
BRCM_PLAYBACK_CLIENT_TESTS = decode_client_dfb
BRCM_SERVER_TESTS = playback_dfb decode_server_dfb
else
BRCM_SERVER_TESTS = playback_dfb
endif 
endif 

endif # DIRECTFB_MASTER_LIB

if DIRECTFB_CLIENT_LIB

if DIRECTFB_NSC_SUPPORT
BRCM_PLAYBACK_CLIENT_TESTS =
BRCM_SERVER_TESTS =
else
BRCM_PLAYBACK_CLIENT_TESTS = decode_client_dfb
BRCM_SERVER_TESTS =
endif

endif # DIRECTFB_CLIENT_LIB

PLATFORM_IPC_STUBS = stubs/ipc_stubs_server.h stubs/ipc_stubs_client.h

if BCMNEXUS_GLES
BRCM_GLES_APPS = \
	dfbtest_egl_only        \
	dfbtest_egl_pixmap      \
	dfbtest_gl
else
BRCM_GLES_APPS =
endif


bin_PROGRAMS = \
	dfbtest_blit_all                  \
	df_screen_encoder                 \
	$(BRCM_GLES_APPS)                 \
	$(BRCM_SERVER_TESTS)		  \
	$(BRCM_PLAYBACK_CLIENT_TESTS)	  \
	$(BRCM_DF_BRCM_TEST)		  \
	$(BRCM_WINDOW_PREALLOC)         

dfbtest_blit_all_SOURCES = dfbtest_blit_all.c
dfbtest_blit_all_LDADD = $(libdirectfb) $(libfusion) $(libdirect)

df_screen_encoder_SOURCES = df_screen_encoder.c
df_screen_encoder_LDADD   = $(libdirectfb) $(libfusion) $(libdirect)

df_brcmTest_SOURCES = df_brcmTest.c
df_brcmTest_LDADD   = $(libdirectfb) $(libfusion) $(libdirect)

playback_dfb_SOURCES = playback_dfb.c
playback_dfb_LDADD   = $(libdirectfb) $(libfusion) $(libdirect) -lnexus -lm -L$(top_srcdir) $(top_builddir)/platform/libinit.la

nxsmaster_SOURCES = nxsmaster.c $(NEXUS_TOP)/lib/ipc/bipc_util.c $(NEXUS_TOP)/lib/ipc/bipc_server.c $(NEXUS_TOP)/lib/ipc/bipc_client.c
nxsmaster_LDADD   = -lnexus -lm -L$(top_srcdir) $(top_builddir)/platform/libinit.la

decode_server_dfb_SOURCES = decode_server_dfb.c
decode_server_dfb_LDADD   = $(libdirectfb) $(libfusion) $(libdirect) -lnexus -lm -L$(top_srcdir) $(top_builddir)/platform/libinit.la

decode_client_dfb_SOURCES = decode_client_dfb.c
decode_client_dfb_LDADD   = $(libdirectfb) $(libfusion) $(libdirect) -lnexus$(NEXUS_CLIENT_LIB_SUFFIX) -lm -L$(top_srcdir) $(top_builddir)/platform/libinit$(NEXUS_CLIENT_LIB_SUFFIX).la

if DIRECTFB_CLIENT_LIB
df_window_prealloc_SOURCES = df_window_prealloc.c
df_window_prealloc_LDADD   = $(libdirectfb) $(libfusion) $(libdirect) -lnexus$(NEXUS_CLIENT_LIB_SUFFIX) -lm -L$(top_srcdir) $(top_builddir)/platform/libinit_client.la 
else
if DIRECTFB_MASTER_LIB
df_window_prealloc_SOURCES = df_window_prealloc.c
df_window_prealloc_LDADD   = $(libdirectfb) $(libfusion) $(libdirect) -lnexus -lm -L$(top_srcdir) $(top_builddir)/platform/libinit.la
endif
endif

if BCMNEXUS_GLES
dfbtest_egl_only_SOURCES = dfbtest_egl_only.c
dfbtest_egl_only_LDADD   = $(libdirectfb) $(libfusion) $(libdirect) $(top_builddir)/systems/bcmnexus/gl/libdirectfb_bcmnexus_gl.la

dfbtest_egl_pixmap_SOURCES = dfbtest_egl_pixmap.c
dfbtest_egl_pixmap_LDADD   = $(libdirectfb) $(libfusion) $(libdirect) $(top_builddir)/systems/bcmnexus/gl/libdirectfb_bcmnexus_gl.la

dfbtest_gl_SOURCES = dfbtest_gl.c
dfbtest_gl_LDADD   = $(libdirectfb) $(libfusion) $(libdirect) $(top_builddir)/systems/bcmnexus/gl/libdirectfb_bcmnexus_gl.la
endif
