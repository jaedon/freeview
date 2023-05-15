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
# $brcm_Workfile: core.inc $
# $brcm_Revision: 39 $
# $brcm_Date: 8/2/12 8:55a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/core/core.inc $
# 
# 39   8/2/12 8:55a erickson
# SW7435-290: no bprofile for usermode clients
# 
# 38   7/16/12 5:18p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 37   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 36   1/19/12 10:31a erickson
# SW7425-2218: add NEXUS_AVS_EXTENSION. fix core thunk.
# 
# 35   4/15/11 7:40p vsilyaev
# SW7425-232: Move video codec conversion to local file
# 
# 34   4/11/11 9:30a erickson
# SW7420-1772: use NEXUS_TOUCHPAD_SUPPORT to set BCHP_PWR_CTK_SUPPORT
# 
# 33   3/24/11 6:25p jtna
# SW7420-972: consolidate PM compile options
# 
# 32   3/10/11 3:43p jtna
# SW7420-972: clean up PM compile options
# 
# 31   2/17/11 5:01p jtna
# SW7408-210: merge for 7408
# 
# 30   2/3/11 6:37p vsilyaev
# SW7420-1441: Use local (non proxied) implementation for function that
#  flush CPU cache
# 
# 29   12/3/10 3:49p erickson
# SW7408-190: 7408 merge
# 
# 28   10/1/10 3:02p jtna
# SW7420-972: NEXUS_POWER_MANAGEMENT option sets BCHP_PWR_SUPPORT option
# 
# 27   9/22/10 12:39p jtna
# SW7405-4865: moved bvlc.c and .h to magnum/commonutils/vlc
# 
# 26   8/25/10 2:42p jtna
# SW7420-972: define NEXUS_POWER_MANAGEMENT=2 for 7420. enable PX3D power
#  management.
# 
# 25   8/19/10 3:59p jtna
# SW7405-4433: 7420 will use nexus_power_management_2.c
# 
# 24   5/12/10 6:28p mward
# SW7125-403:  Allow NEXUS_POWER_MANAGEMENT=y to enable the feature for
#  chips that don't default it on.
# 
# 23   3/22/10 5:10p mward
# SW7400-2511: Default NEXUS_POWER_MANAGEMENT on for 7400.
# 
# 22   12/17/09 4:20p erickson
# SW3556-933: added new format. moved all type lookup and conversion code
#  into separate .c so we don't have to rev nexus_core.c so often.
# 
# 21   9/16/09 5:30p erickson
# SW7400-2511: move NEXUS_POWER_MANAGEMENT to core.inc
#
# 20   9/16/09 1:40p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 19   9/1/09 3:52p jgarrett
# SW7335-529: Replacing direct access to MAGNUM_MODULES with
#  NEXUS_EXTRA_MODULES to avoid magnum paths leaking into application
#  include paths
#
# 18   4/20/09 10:37a jtna
# PR54011: added nexus_watchdog.c
#
# 17   3/30/09 12:51p erickson
# PR53544: remove vbi dependency from core
#
# 16   3/19/09 3:20p erickson
# PR53323: added magnum dependencies
#
# 15   1/30/09 9:21p vsilyaev
# PR 51741: Use MAGNUM_SYSTEM_DEFINES option to set system wide options
#
# 14   1/30/09 12:01p vsilyaev
# PR 51471: Added NEXUS_SYSTEM_DEFINES that could be used as 'NEXUS_SYS
#
# 13   10/30/08 1:22p erickson
# PR40799: add bpwr
#
# 12   9/26/08 11:10a erickson
# PR40799: add internal power management API
#
# 11   8/21/08 3:16p katrep
# PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
#
# 10   4/28/08 5:59p vsilyaev
# PR 42158: Added instrumentation to measure DMA throughput
#
# 9   4/18/08 10:57a vsilyaev
# PR 41896: Changed configuration to reentrant for BINT and BMEM base
#  modules
#
# 8   4/14/08 4:46p vsilyaev
# PR 41083: Fixed typo
#
# 7   4/10/08 5:45p vsilyaev
# PR 41083: Added BTRC support
#
# 6   4/9/08 2:42p vsilyaev
# PR 41083: Added linux user performance sampling for BMIPS4380
#
# 5   4/8/08 10:40a vsilyaev
# PR 41083: Added bprofile support
#
# 4   3/24/08 3:14p bandrews
# PR40090: Moved syslib framework impl into core
#
# 3   3/6/08 6:20p bandrews
# PR36808: Moved to core module
#
# 2   1/31/08 12:45p vsilyaev
# PR 38682: Build proxy for the core module
#
# 1   1/18/08 2:14p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/7   12/7/07 8:20p jgarrett
# PR 38020: Adding memory routines
#
# Nexus_Devel/6   9/21/07 10:50a erickson
# PR35002: compilation errors
#
# Nexus_Devel/5   9/18/07 5:41p jgarrett
# PR 35002: Split to chip-specific directories
#
# Nexus_Devel/4   9/14/07 4:59p erickson
# PR34925: update
#
# Nexus_Devel/3   9/13/07 2:37p vsilyaev
# PR 34419: Updated for new directory layout
#
# Nexus_Devel/2   9/10/07 5:34p vsilyaev
# PR 34419: Added array of memory heaps
#
# Nexus_Devel/1   9/6/07 1:02p vsilyaev
# PR 34419: Core module
#
############################################################

#
# Conditional Options for this Makefile
#
# BCHP_CHIP - Chip number (i.e. 7401)
# BCHP_VER - Chip Revision in upper-case (i.e. C2)

######################
# Define this module #
######################
NEXUS_MODULES += CORE

##################
# Module Options #
##################
NEXUS_CORE_OPTIONS := BCHP_CHIP BCHP_VER

###################
# Module Includes #
###################
NEXUS_CORE_PUBLIC_INCLUDES := \
    $(NEXUS_TOP)/modules/core/include \
    $(NEXUS_TOP)/base/include/public
NEXUS_CORE_PRIVATE_INCLUDES = $(NEXUS_TOP)/modules/core/src $(NEXUS_SYNCTHUNK_DIR)

##################
# Module Sources #
##################
NEXUS_CORE_SOURCES := \
	$(NEXUS_TOP)/modules/core/src/nexus_core.c \
	$(NEXUS_TOP)/modules/core/src/nexus_core_utils.c \
	$(NEXUS_TOP)/modules/core/src/nexus_core_convert.c \
	$(NEXUS_TOP)/modules/core/src/nexus_memory.c \
	$(NEXUS_TOP)/modules/core/src/nexus_core_profile.c \
	$(NEXUS_TOP)/modules/core/src/nexus_syslib_framework.c \
	$(NEXUS_TOP)/modules/core/src/nexus_core_img.c \
	$(NEXUS_TOP)/modules/core/src/nexus_watchdog.c

NEXUS_CORE_LOCAL_SOURCES := \
	$(NEXUS_TOP)/modules/core/src/nexus_memory_local.c \
	$(NEXUS_TOP)/modules/core/src/nexus_core_convert_local.c 

NEXUS_CORE_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_core_thunks.c

#
# Power management
#
ifeq ($(NEXUS_POWER_MANAGEMENT),y)
NEXUS_CORE_SOURCES += $(NEXUS_TOP)/modules/core/src/nexus_power_management_2.c
NEXUS_CFLAGS += -DNEXUS_POWER_MANAGEMENT
BCHP_PWR_SUPPORT=y
else
NEXUS_CORE_SOURCES += $(NEXUS_TOP)/modules/core/src/nexus_power_management.c
BCHP_PWR_SUPPORT=n
endif

ifeq ($(NEXUS_TOUCHPAD_SUPPORT),y)
BCHP_PWR_CTK_SUPPORT=y
else
BCHP_PWR_CTK_SUPPORT=n
endif

##################
# Module Defines #
##################
NEXUS_CORE_DEFINES := BCHP_CHIP=$(BCHP_CHIP) BCHP_VER=BCHP_VER_$(BCHP_VER) BMEM_REENTRANT_CONFIG=BMEM_REENTRANT BINT_REENTRANT_CONFIG=BINT_REENTRANT $(MAGNUM_SYSTEM_DEFINES)


#######################
# Module Dependencies #
#######################
# Base has no module dependencies
NEXUS_CORE_DEPENDENCIES := BASE

##################
# Magnum Modules #
##################
NEXUS_CORE_MAGNUM_MODULES =  \
    $(MAGNUM)/basemodules/int/bint.inc \
    $(MAGNUM)/basemodules/mem/bmem.inc \
    $(MAGNUM)/basemodules/tmr/btmr.inc \
    $(MAGNUM)/commonutils/avc/bavc.inc \
    $(MAGNUM)/commonutils/fmt/bfmt.inc \
    $(MAGNUM)/commonutils/mrc/bmrc.inc \
    $(MAGNUM)/commonutils/mth/bmth.inc \
    $(MAGNUM)/commonutils/pxl/bpxl.inc \
    $(MAGNUM)/commonutils/sur/bsur.inc \
    $(MAGNUM)/commonutils/img/bimg.inc \
    $(MAGNUM)/commonutils/vlc/bvlc.inc \
    $(MAGNUM)/syslib/framework/bsyslib_framework.inc

# Optional profiler support
ifneq ($(NEXUS_MODE),client)
ifeq ($(BPROFILE_SUPPORT),y)
NEXUS_CORE_MAGNUM_MODULES += \
    $(BSEAV)/lib/bprofile/bprofile.inc
# BSEAV makefiles don't include their module names into MAGNUM_MODULES
NEXUS_EXTRA_MODULES += BPROFILE
NEXUS_CORE_DEFINES +=B_HAS_SYMTABLE
ifneq ($(BPROFILE_COUNTERS),)
NEXUS_CORE_DEFINES += BPROFILE_CFG_PERF_COUNTER=$(BPROFILE_COUNTERS)
endif
endif
endif

# Optional BTRC (manual profiler) support
ifeq ($(BTRC_SUPPORT),y)
NEXUS_CORE_MAGNUM_MODULES += \
    $(BSEAV)/lib/bprofile/bprofile.inc
# BSEAV makefiles don't include their module names into MAGNUM_MODULES
NEXUS_EXTRA_MODULES += BTRC
NEXUS_CORE_DEFINES +=B_HAS_TRC=1
endif
NEXUS_CORE_DEFINES += NEXUS_PROFILE_OS_$(B_REFSW_OS)
NEXUS_SYMBOLS_SRC := bsymbols.c

#####################
# Module Extensions #
#####################
ifneq ($(NEXUS_AVS_EXTENSION_INC),)
include $(NEXUS_AVS_EXTENSION_INC)
NEXUS_CORE_DEFINES += NEXUS_AVS_EXTENSION
endif
