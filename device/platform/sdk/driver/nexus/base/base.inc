############################################################
#     (c)2003-2012 Broadcom Corporation
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
# $brcm_Workfile: base.inc $
# $brcm_Revision: 18 $
# $brcm_Date: 7/31/12 6:44p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/base/base.inc $
# 
# 18   7/31/12 6:44p vsilyaev
# SW7435-287: Added framework to measure run-time stack usage
# 
# 17   7/20/12 3:25p vsilyaev
# SW7435-267: Added code to provide/print callback timing statistics
# 
# 16   7/20/12 10:10a erickson
# SW7435-263: change location of bchp.inc
# 
# 15   7/17/12 11:11a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 14   7/16/12 5:18p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 13   7/9/12 3:48p vsilyaev
# SW7435-256: Use BLST_AAT implementation of AA Tree
# 
# 12   6/21/12 2:55p vsilyaev
# SW7420-2315: Use AA tree for address->handle lookup
# 
# 11   6/19/12 12:41p vsilyaev
# SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
# 
# 10   5/3/11 5:07p erickson
# SW7420-1819: repartition so NFE is a standard feature
# 
# 9   1/19/11 11:11a erickson
# SW7420-1123: usermode server doesn't use external scheduler
# 
# 8   12/28/10 1:47p erickson
# SWDEPRECATED-2425: bstd.h requires chp and reg in normal builds
#
# 7   12/7/10 5:47p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
#
# 6   10/21/10 4:50p mphillip
# SW7125-670: Remove 3D define from base.inc
#
# 5   9/29/10 9:33a erickson
# SW7420-1123: add linux usermode IPC
#
# 4   9/16/09 1:41p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 3   11/4/08 3:59p mphillip
# PR47441: Add a 3D proxy-mode define to modify graphics3d's behavior in
#  proxy mode
#
# 2   6/5/08 2:19p vsilyaev
# PR 42318: Rearranged scheduller to share the same context in user and
#  kernel mode, this allows to serve callback from kernel to user with
#  single transition kernel<->user
#
# 1   1/18/08 2:12p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/11   1/16/08 3:37p vsilyaev
# PR 38682: Don't execute KNI/DBG initialization in the linuxkernel
#
# Nexus_Devel/10   10/23/07 4:16p vsilyaev
# PR 36199: Added memory mapper sub-module
#
# Nexus_Devel/9   10/10/07 4:19p vsilyaev
# PR 35824: Added NEXUS_Time type
#
# Nexus_Devel/8   9/13/07 5:20p vsilyaev
# PR 34419: Use plugins for OS
#
# Nexus_Devel/7   9/13/07 2:45p vsilyaev
# PR 34419: Updated for new directory layout
#
# Nexus_Devel/6   9/11/07 5:08p jgarrett
# PR 34416: Removing IO line
#
# Nexus_Devel/5   9/6/07 1:04p vsilyaev
# PR 34419: Added core module
#
# Nexus_Devel/4   9/5/07 3:03p vsilyaev
# PR 34419: Added core files
#
# Nexus_Devel/3   9/4/07 11:40a jgarrett
# PR 34416: Moving lib/utils to private includes
#
# Nexus_Devel/2   8/31/07 4:15p jgarrett
# PR 34416: Adding baseline build system
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += BASE

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_BASE_PUBLIC_INCLUDES = \
    $(NEXUS_TOP)/base/include \
    $(NEXUS_TOP)/base/include/public
    
NEXUS_BASE_PRIVATE_INCLUDES = \
    $(MAGNUM)/../BSEAV/lib/utils \
    $(NEXUS_TOP)/base/src/$(B_REFSW_OS) \
    $(NEXUS_TOP)/base/src
    
##################
# Module Sources #
##################
NEXUS_BASE_SOURCES = \
    $(NEXUS_TOP)/base/src/nexus_base.c \
    $(NEXUS_TOP)/base/src/nexus_base_object.c \
    $(NEXUS_TOP)/base/src/nexus_base_mmap.c \
    $(NEXUS_TOP)/base/src/nexus_base_scheduler.c \
    $(NEXUS_TOP)/base/src/nexus_base_stackcheck.c \
    $(NEXUS_TOP)/base/src/nexus_base_statistics.c \
    $(NEXUS_TOP)/base/src/$(B_REFSW_OS)/nexus_base_os.c

ifeq ($(B_REFSW_OS),linuxemu)
NEXUS_BASE_PUBLIC_INCLUDES += $(NEXUS_TOP)/base/include/linuxuser
else
ifeq ($(B_REFSW_OS),bare)
NEXUS_BASE_PUBLIC_INCLUDES += $(NEXUS_TOP)/base/include/linuxkernel
else
NEXUS_BASE_PUBLIC_INCLUDES += $(NEXUS_TOP)/base/include/$(B_REFSW_OS)
endif
endif

##################
# Module Defines #
##################
NEXUS_BASE_DEFINES :=  NEXUS_BASE_OS_$(B_REFSW_OS)=1
ifeq ($(NEXUS_MODE),proxy)
NEXUS_BASE_DEFINES += NEXUS_BASE_MODE_PROXY=1
endif
ifeq ($(B_REFSW_OS),bare)
NEXUS_BASE_DEFINES += NEXUS_BASE_MODE_PROXY=1
endif

#######################
# Module Dependencies #
#######################
# Base has no module dependencies
NEXUS_BASE_DEPENDENCIES :=

##################
# Magnum Modules #
##################
NEXUS_BASE_MAGNUM_MODULES := \
    $(MAGNUM)/basemodules/chp/build/bchp.inc \
    $(MAGNUM)/basemodules/dbg/bdbg.inc \
    $(MAGNUM)/basemodules/err/berr.inc \
    $(MAGNUM)/basemodules/kni/bkni.inc \
    $(MAGNUM)/basemodules/std/bstd.inc \
    $(MAGNUM)/commonutils/lst/blst.inc \
    $(MAGNUM)/basemodules/reg/breg.inc

NEXUS_EXTRA_MODULES += BAA_TREE 

