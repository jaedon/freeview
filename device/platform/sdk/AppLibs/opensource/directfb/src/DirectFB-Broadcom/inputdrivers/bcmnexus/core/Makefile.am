#############################################################################
#    (c)2008-2012 Broadcom Corporation
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
# $brcm_Revision: DirectFB_1_4_15_Port/7 $
# $brcm_Date: 3/7/12 5:25p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/inputdrivers/bcmnexus/core/Makefile.am $
# 
# DirectFB_1_4_15_Port/7   3/7/12 5:25p christ
# SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
# 
# DirectFB_1_4_15_Port/6   2/24/12 3:00p robertwm
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/5   2/9/12 2:34p christ
# SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
#  into DFB
# 
# DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   1/27/12 7:02p christ
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/4   11/11/11 7:19p robertwm
# SW7425-1740: DirectFB: Multi-app support in Nexus user mode.
# 
# DirectFB_1_4_15_Port/3   11/2/11 2:41p christ
# SW7420-2121: DirectFB: Refactor input drivers (ir + keypad) to move
#  Nexus usage into platform layer
# 
# DirectFB_1_4_15_Port/2   11/2/11 2:23p christ
# SW7420-2121: DirectFB: Refactor input drivers (ir + keypad) to move
#  Nexus usage into platform layer
# 
# DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
# SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
# 
# DirectFB_1_4_5_Port/1   6/14/11 6:10p christ
# SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
# 
# DirectFB_1_4_Port/8   3/14/11 4:49p robertwm
# SW7420-1655: DirectFB: Linker warning reported when trying to link
#  keycodes.so with ir.so.
# 
# DirectFB_1_4_Port/7   3/4/11 4:15p robertwm
# SW7420-1591: DirectFB: Need 2 passes to build DirectFB due to linking
#  problem with bcmnexus IR.
# 
# DirectFB_1_4_Port/6   3/2/11 12:07p christ
# SW7420-1558: DirectFB: Support runtime switching of IR codes for using
#  different remotes.
# 
# DirectFB_1_4_Port/5   12/14/10 11:09a scchang
# SW7420-1313: Add bluetooth handset support
# 
# DirectFB_1_4_Port/4   10/28/10 12:16p robertwm
# SW35230-611: DirectFB: Changes to incorporate building nexus shim into
#  Makefile.
# 
# DirectFB_1_4_Port/3   2/24/10 6:39p robertwm
# SW7468-125: DirectFB: Provide ability to separately build IR and Keypad
#  input drivers.
# 
# DirectFB_1_4_Port/2   2/18/10 12:28p robertwm
# SW7400-2695: DirectFB: Linking system shared lib against graphics
#  shared lib uses incorrect libtool flags.
# 
# DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
# PR56658: Add DirectFB 1.4.1 broadcom_files
# 
# 2   7/15/08 3:51p ttrammel
# PR44738: DirectFB Nexus KIR driver build is broken.
# 
#############################################################################
## Makefile.am for DirectFB/inputdrivers/core/bcmnexus_ir

INCLUDES = \
    -I$(top_srcdir)/include   \
    -I$(top_builddir)/include \
    -I$(top_build_dir)/lib    \
    -I$(top_srcdir)/src       \
    -I$(top_srcdir)/lib       \
    -I$(top_srcdir)/platform  \
    -I$(top_srcdir)/systems/bcmnexus \
    -I$(top_srcdir)/gfxdrivers/bcmnexus


AM_CPPFLAGS =  ${BRCM_CFLAGS}

IR_SRCS = \
    bcmnexus_ir.c

KEY_SRCS = \
    bcmnexus_key.c

KEYBOARD_POINTER_SRCS = \
    bcmnexus_keyboard_pointer.c keyboard_pointer.c

# HUMAX_PLATFORM_BASE	
RF4CE_SRCS = \
    bcmnexus_rf4ce.c
# HUMAX_PLATFORM_BASE End
######## Nexus master lib ########

if DIRECTFB_MASTER_LIB

### IR support ###
if BCMNEXUS_IR_INPUT
bcmnexus_ir_LTLIBRARIES  = libdirectfb_bcmnexus_ir.la
bcmnexus_irdir           = $(MODULEDIR)/inputdrivers

if BUILD_STATIC
bcmnexus_ir_DATA         = $(bcmnexus_ir_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_ir_la_SOURCES = $(IR_SRCS)

libdirectfb_bcmnexus_ir_la_LDFLAGS = \
    -avoid-version      \
    -L${CPI_OBJECT_DIR} \
    -lnexus

libdirectfb_bcmnexus_ir_la_LIBADD = \
    $(top_builddir)/platform/libinit.la

endif

# Keypad support
if BCMNEXUS_KEY_INPUT
bcmnexus_key_LTLIBRARIES = libdirectfb_bcmnexus_key.la
bcmnexus_keydir          = $(MODULEDIR)/inputdrivers

if BUILD_STATIC
bcmnexus_key_DATA        = $(bcmnexus_key_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_key_la_SOURCES = $(KEY_SRCS)

libdirectfb_bcmnexus_key_la_LDFLAGS = \
    -avoid-version      \
    -module             \
    -L${CPI_OBJECT_DIR} \
    -lnexus

libdirectfb_bcmnexus_key_la_LIBADD = \
    $(top_builddir)/platform/libinit.la

endif
# Keyboard pointer support
if BCMNEXUS_KEYBOARD_POINTER_INPUT
bcmnexus_keyboard_pointer_LTLIBRARIES = libdirectfb_bcmnexus_keyboard_pointer.la
bcmnexus_keyboard_pointerdir          = $(MODULEDIR)/inputdrivers

if BUILD_STATIC
bcmnexus_keyboard_pointer_DATA        = $(bcmnexus_keyboard_pointer_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_keyboard_pointer_la_SOURCES = $(KEYBOARD_POINTER_SRCS)

libdirectfb_bcmnexus_keyboard_pointer_la_LDFLAGS = \
    -avoid-version      \
    -module             \
    -L${CPI_OBJECT_DIR} \
    -lnexus

libdirectfb_bcmnexus_keyboard_pointer_la_LIBADD = \
    $(top_builddir)/platform/libinit.la
endif

# HUMAX_PLATFORM_BASE
if BCMNEXUS_RF4CE_INPUT
bcmnexus_rf4ce_LTLIBRARIES  = libdirectfb_bcmnexus_rf4ce.la
bcmnexus_rf4cedir           = $(MODULEDIR)/inputdrivers

if BUILD_STATIC
bcmnexus_rf4ce_DATA         = $(bcmnexus_rf4ce_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_rf4ce_la_SOURCES = $(RF4CE_SRCS)

libdirectfb_bcmnexus_rf4ce_la_LDFLAGS = \
    -avoid-version      \
    -L${CPI_OBJECT_DIR} \
    -lnexus

libdirectfb_bcmnexus_rf4ce_la_LIBADD = \
    $(top_builddir)/platform/libinit.la

endif
# HUMAX_PLATFORM_BASE End

endif
######## Nexus client lib ########
if DIRECTFB_CLIENT_LIB

### IR support ###
if BCMNEXUS_IR_INPUT
bcmnexus_ir_client_LTLIBRARIES = libdirectfb_bcmnexus_ir_client.la
bcmnexus_ir_clientdir          = $(MODULECLIENTDIR)/inputdrivers

if BUILD_STATIC
bcmnexus_ir_client_DATA  = $(bcmnexus_ir_client_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_ir_client_la_SOURCES = $(IR_SRCS)

libdirectfb_bcmnexus_ir_client_la_LDFLAGS = \
    -avoid-version      \
    -L${CPI_OBJECT_DIR} \
    -lnexus$(NEXUS_CLIENT_LIB_SUFFIX)

libdirectfb_bcmnexus_ir_client_la_LIBADD =  \
    $(top_builddir)/platform/libinit_client.la

endif

if BCMNEXUS_KEY_INPUT
bcmnexus_key_client_LTLIBRARIES = libdirectfb_bcmnexus_key_client.la
bcmnexus_key_clientdir          = $(MODULECLIENTDIR)/inputdrivers

if BUILD_STATIC
bcmnexus_key_client_DATA        = $(bcmnexus_key_client_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_key_client_la_SOURCES = $(KEY_SRCS)

libdirectfb_bcmnexus_key_client_la_LDFLAGS = \
    -avoid-version      \
    -module             \
    -L${CPI_OBJECT_DIR} \
    -lnexus$(NEXUS_CLIENT_LIB_SUFFIX)

libdirectfb_bcmnexus_key_client_la_LIBADD = \
    $(top_builddir)/platform/libinit_client.la

endif

### BCMNEXUS_KEYBOARD_POINTER support ###
if BCMNEXUS_KEYBOARD_POINTER_INPUT
bcmnexus_keyboard_pointer_client_LTLIBRARIES = libdirectfb_bcmnexus_keyboard_pointer_client.la
bcmnexus_keyboard_pointer_clientdir          = $(MODULECLIENTDIR)/inputdrivers

if BUILD_STATIC
bcmnexus_keyboard_pointer_client_DATA  = $(bcmnexus_keyboard_pointer_client_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_keyboard_pointer_client_la_SOURCES = $(KEYBOARD_POINTER_SRCS)

libdirectfb_bcmnexus_keyboard_pointer_client_la_LDFLAGS = \
    -avoid-version      \
    -L${CPI_OBJECT_DIR} \
    -lnexus$(NEXUS_CLIENT_LIB_SUFFIX)

libdirectfb_bcmnexus_keyboard_pointer_client_la_LIBADD =  \
    $(top_builddir)/platform/libinit_client.la

endif

# HUMAX_PLATFORM_BASE
if BCMNEXUS_RF4CE_INPUT
bcmnexus_rf4ce_client_LTLIBRARIES = libdirectfb_bcmnexus_rf4ce_client.la
bcmnexus_rf4ce_clientdir          = $(MODULECLIENTDIR)/inputdrivers

if BUILD_STATIC
bcmnexus_rf4ce_client_DATA  = $(bcmnexus_rf4ce_client_LTLIBRARIES:.la=.o)
endif

libdirectfb_bcmnexus_rf4ce_client_la_SOURCES = $(RF4CE_SRCS)

libdirectfb_bcmnexus_rf4ce_client_la_LDFLAGS = \
    -avoid-version      \
    -L${CPI_OBJECT_DIR} \
    -lnexus$(NEXUS_CLIENT_LIB_SUFFIX)

libdirectfb_bcmnexus_rf4ce_client_la_LIBADD =  \
    $(top_builddir)/platform/libinit_client.la

endif
# HUMAX_PLATFORM_BASE End

endif

include $(top_srcdir)/rules/libobject.make
