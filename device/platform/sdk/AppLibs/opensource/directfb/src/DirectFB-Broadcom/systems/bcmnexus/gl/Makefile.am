#############################################################################
#    (c)2010-2012 Broadcom Corporation
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
# $brcm_Revision: DirectFB_1_4_15_Port/3 $
# $brcm_Date: 3/7/12 5:27p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/gl/Makefile.am $
# 
# DirectFB_1_4_15_Port/3   3/7/12 5:27p christ
# SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
# 
# DirectFB_1_4_15_Port/2   11/11/11 7:27p robertwm
# SW7425-1740: DirectFB: Multi-app support in Nexus user mode.
# 
# DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
# SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
# 
# DirectFB_1_4_5_Port/2   6/27/11 12:43p robertwm
# SW7405-5389: DirectFB: DFB Applications should not link directly with
#  nexus.
# 
# DirectFB_1_4_5_Port/1   6/14/11 4:46p christ
# SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
# 
# DirectFB_1_4_Port/4   6/27/11 12:36p robertwm
# SW7405-5389: DirectFB: DFB Applications should not link directly with
#  nexus.
# 
# DirectFB_1_4_Port/3   10/28/10 12:18p robertwm
# SW35230-611: DirectFB: Changes to incorporate building nexus shim into
#  Makefile.
# 
# DirectFB_1_4_Port/2   8/6/10 8:05p robertwm
# SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
# 
# DirectFB_1_4_Port/1   8/6/10 4:36p robertwm
# SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
# 
#############################################################################
## Makefile.am for DirectFB/systems/bcmnexus/gl

INCLUDES = \
    -I$(top_srcdir)/include   \
    -I$(top_builddir)/include \
    -I$(top_builddir)/lib     \
    -I$(top_srcdir)/lib       \
    -I$(top_srcdir)/src       \
    -I$(top_srcdir)/platform    

AM_CPPFLAGS =  \
    ${BRCM_CFLAGS} \
    -I$(srcdir)/GLES

internalincludedir = $(INTERNALINCLUDEDIR)/bcmnexus/gl

internalinclude_HEADERS = 

######## Nexus master lib ########

if DIRECTFB_MASTER_LIB
NOINST_MASTER_LIBS = libdirectfb_bcmnexus_gl.la
else
NOINST_MASTER_LIBS =
endif

######## Nexus client lib ########
if DIRECTFB_CLIENT_LIB
NOINST_CLIENT_LIBS = libdirectfb_bcmnexus_gl_client.la
else
NOINST_CLIENT_LIBS =
endif

noinst_LTLIBRARIES = \
    $(NOINST_MASTER_LIBS) \
    $(NOINST_CLIENT_LIBS)

GL_SRCS =		\
    gl_activetexture.c  \
    gl_alphablend.c     \
    gl_clear.c          \
    gl_clip.c           \
    gl_color.c          \
    gl_depth.c          \
    gl_enable.c         \
    gl_error.c          \
    gl_execute.c        \
    gl_fog.c            \
    gl_get.c            \
    gl_hint.c           \
    gl_light.c          \
    gl_logicop.c        \
    gl_material.c       \
    gl_matrix.c         \
    gl_normal.c         \
    gl_pixel.c          \
    gl_primmodes.c      \
    gl_samplecoverage.c \
    gl_stencil.c        \
    gl_texcoord.c       \
    gl_teximage.c       \
    gl_texture.c        \
    gl_transform.c      \
    gl_vertex.c         \
    gl_vertexarray.c    \
    gl_viewport.c

######## Nexus master lib ########

if DIRECTFB_MASTER_LIB
libdirectfb_bcmnexus_gl_la_SOURCES = $(GL_SRCS)

libdirectfb_bcmnexus_gl_la_LDFLAGS =    \
    -lnexus                             \
    $(top_builddir)/platform/libinit.la \
    -lm                                 \
    $(DFB_LDFLAGS)
endif

######## Nexus client lib ########
if DIRECTFB_CLIENT_LIB

libdirectfb_bcmnexus_gl_client_la_SOURCES = $(GL_SRCS)

libdirectfb_bcmnexus_gl_client_la_LDFLAGS =    \
    -lnexus$(NEXUS_CLIENT_LIB_SUFFIX)          \
    $(top_builddir)/platform/libinit_client.la \
    -lm                                        \
    $(DFB_LDFLAGS)
endif
