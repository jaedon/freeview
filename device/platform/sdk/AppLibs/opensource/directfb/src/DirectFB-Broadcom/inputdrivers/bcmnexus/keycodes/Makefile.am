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
# $brcm_Revision: DirectFB_1_4_15_Port/3 $
# $brcm_Date: 2/24/12 3:02p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/inputdrivers/bcmnexus/keycodes/Makefile.am $
# 
# DirectFB_1_4_15_Port/3   2/24/12 3:02p robertwm
# SW7231-625: DirectFB-XS: Allow Nexus client only builds.
# 
# DirectFB_1_4_15_Port/2   11/11/11 7:21p robertwm
# SW7425-1740: DirectFB: Multi-app support in Nexus user mode.
# 
# DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
# SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
# 
# DirectFB_1_4_5_Port/1   6/14/11 6:10p christ
# SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
# 
# DirectFB_1_4_Port/2   3/17/11 1:41p christ
# SW7420-1684: DirectFB: Cannot use "Broadcom" (silver) remote.
# 
# DirectFB_1_4_Port/1   3/4/11 4:27p robertwm
# SW7420-1591: DirectFB: Need 2 passes to build DirectFB due to linking
#  problem with bcmnexus IR.
# 
#############################################################################
## Makefile.am for DirectFB/inputdrivers/keycodes/bcmnexus_ir_keycodes_xxx

INCLUDES = \
	-I$(top_srcdir)/include	\
	-I$(top_builddir)/include	\
	-I$(top_build_dir)/lib	\
	-I$(top_srcdir)/src	\
	-I$(top_srcdir)/inputdrivers/bcmnexus/core	\
	-I$(top_srcdir)/lib

AM_CPPFLAGS =  ${BRCM_CFLAGS}

IR_KEYCODES_CIRNEC_SRCS = \
	bcmnexus_ir_keycodes_cirnec.c

IR_KEYCODES_GENERIC_SRCS = \
	bcmnexus_ir_keycodes_generic.c

IR_KEYCODES_REMOTEA_SRCS = \
	bcmnexus_ir_keycodes_remotea.c 

IR_KEYCODES_LDFLAGS = \
	-avoid-version\
	-module       \
	-L${CPI_OBJECT_DIR}


######## Nexus master lib ########

if DIRECTFB_MASTER_LIB

if BCMNEXUS_IR_INPUT
bcmnexus_ir_keycodes_cirnec_LTLIBRARIES  = libdirectfb_bcmnexus_ir_keycodes_cirnec.la
bcmnexus_ir_keycodes_generic_LTLIBRARIES = libdirectfb_bcmnexus_ir_keycodes_generic.la
bcmnexus_ir_keycodes_remotea_LTLIBRARIES = libdirectfb_bcmnexus_ir_keycodes_remotea.la
endif

bcmnexus_ir_keycodes_cirnecdir  = $(MODULEDIR)/inputdrivers/bcmnexus
bcmnexus_ir_keycodes_genericdir = $(MODULEDIR)/inputdrivers/bcmnexus
bcmnexus_ir_keycodes_remoteadir = $(MODULEDIR)/inputdrivers/bcmnexus

if BUILD_STATIC
bcmnexus_ir_keycodes_cirnec_DATA  =  $(bcmnexus_ir_keycodes_cirnec_LTLIBRARIES:.la=.o)
bcmnexus_ir_keycodes_generic_DATA =  $(bcmnexus_ir_keycodes_generic_LTLIBRARIES:.la=.o)
bcmnexus_ir_keycodes_remotea_DATA =  $(bcmnexus_ir_keycodes_remotea_LTLIBRARIES:.la=.o)
endif

# CIR NEC Protocol (master instance)
libdirectfb_bcmnexus_ir_keycodes_cirnec_la_SOURCES = $(IR_KEYCODES_CIRNEC_SRCS)

libdirectfb_bcmnexus_ir_keycodes_cirnec_la_LDFLAGS = $(IR_KEYCODES_LDFLAGS)

libdirectfb_bcmnexus_ir_keycodes_cirnec_la_LIBADD = \
	$(top_builddir)/inputdrivers/bcmnexus/core/libdirectfb_bcmnexus_ir.la

# Generic (NEC) Protocol (master instance)
libdirectfb_bcmnexus_ir_keycodes_generic_la_SOURCES = $(IR_KEYCODES_GENERIC_SRCS)

libdirectfb_bcmnexus_ir_keycodes_generic_la_LDFLAGS = $(IR_KEYCODES_LDFLAGS)

libdirectfb_bcmnexus_ir_keycodes_generic_la_LIBADD = \
	$(top_builddir)/inputdrivers/bcmnexus/core/libdirectfb_bcmnexus_ir.la

# RemoteA Protocol (master instance)
libdirectfb_bcmnexus_ir_keycodes_remotea_la_SOURCES = $(IR_KEYCODES_REMOTEA_SRCS)

libdirectfb_bcmnexus_ir_keycodes_remotea_la_LDFLAGS = $(IR_KEYCODES_LDFLAGS)

libdirectfb_bcmnexus_ir_keycodes_remotea_la_LIBADD = \
	$(top_builddir)/inputdrivers/bcmnexus/core/libdirectfb_bcmnexus_ir.la

endif

######## Nexus client lib ########
if DIRECTFB_CLIENT_LIB

if BCMNEXUS_IR_INPUT
bcmnexus_ir_keycodes_cirnec_client_LTLIBRARIES  = libdirectfb_bcmnexus_ir_keycodes_cirnec_client.la
bcmnexus_ir_keycodes_generic_client_LTLIBRARIES = libdirectfb_bcmnexus_ir_keycodes_generic_client.la
bcmnexus_ir_keycodes_remotea_client_LTLIBRARIES = libdirectfb_bcmnexus_ir_keycodes_remotea_client.la
endif

if BUILD_STATIC
bcmnexus_ir_keycodes_cirnec_client_DATA  =  $(bcmnexus_ir_keycodes_cirnec_client_LTLIBRARIES:.la=.o)
bcmnexus_ir_keycodes_generic_client_DATA =  $(bcmnexus_ir_keycodes_generic_client_LTLIBRARIES:.la=.o)
bcmnexus_ir_keycodes_remotea_client_DATA =  $(bcmnexus_ir_keycodes_remotea_client_LTLIBRARIES:.la=.o)
endif

bcmnexus_ir_keycodes_cirnec_clientdir  = $(MODULECLIENTDIR)/inputdrivers/bcmnexus
bcmnexus_ir_keycodes_generic_clientdir = $(MODULECLIENTDIR)/inputdrivers/bcmnexus
bcmnexus_ir_keycodes_remotea_clientdir = $(MODULECLIENTDIR)/inputdrivers/bcmnexus


# CIR NEC Protocol (slave client instance)
libdirectfb_bcmnexus_ir_keycodes_cirnec_client_la_SOURCES = $(IR_KEYCODES_CIRNEC_SRCS)

libdirectfb_bcmnexus_ir_keycodes_cirnec_client_la_LDFLAGS = $(IR_KEYCODES_LDFLAGS)

libdirectfb_bcmnexus_ir_keycodes_cirnec_client_la_LIBADD = \
	$(top_builddir)/inputdrivers/bcmnexus/core/libdirectfb_bcmnexus_ir_client.la


# Generic (NEC) Protocol (slave client instance)
libdirectfb_bcmnexus_ir_keycodes_generic_client_la_SOURCES = $(IR_KEYCODES_GENERIC_SRCS)

libdirectfb_bcmnexus_ir_keycodes_generic_client_la_LDFLAGS = $(IR_KEYCODES_LDFLAGS)

libdirectfb_bcmnexus_ir_keycodes_generic_client_la_LIBADD = \
	$(top_builddir)/inputdrivers/bcmnexus/core/libdirectfb_bcmnexus_ir_client.la


# RemoteA Protocol (slave client instance)
libdirectfb_bcmnexus_ir_keycodes_remotea_client_la_SOURCES = $(IR_KEYCODES_REMOTEA_SRCS)

libdirectfb_bcmnexus_ir_keycodes_remotea_client_la_LDFLAGS = $(IR_KEYCODES_LDFLAGS)

libdirectfb_bcmnexus_ir_keycodes_remotea_client_la_LIBADD = \
	$(top_builddir)/inputdrivers/bcmnexus/core/libdirectfb_bcmnexus_ir_client.la

endif

include $(top_srcdir)/rules/libobject.make
