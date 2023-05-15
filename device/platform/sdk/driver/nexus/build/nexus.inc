############################################################
#     (c)2003-2011 Broadcom Corporation
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
# $brcm_Workfile: nexus.inc $
# $brcm_Revision: 35 $
# $brcm_Date: 11/9/11 3:52p $
#
# Module Description:
#       This makefile contains the OS-independent rules for building
#       nexus modules.
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/build/nexus.inc $
# 
# 35   11/9/11 3:52p erickson
# SW7420-1148: if GL_SUPPORT=y, don't generate or build proxy for
#  graphics3d module
# 
# 34   5/3/11 5:07p erickson
# SW7420-1819: repartition so NFE is a standard feature
# 
# 32   12/15/10 11:59a erickson
# SW35230-2585: move circular DEPENDENCIES check into module_vars.pl
# 
# 31   12/14/10 5:01p erickson
# SW7420-1264: use NEXUS_BIN_DIR and NEXUS_BUILD_DIR for final and
#  intermediate binaries
# 
# 30   12/9/10 3:33p erickson
# SW7420-1308: add NEXUS_STUB_MODULES so that stub modules can have their
#  API included but still avoid having NEXUS_HAS_<MODULE> defined
# 
# 29   9/29/10 9:33a erickson
# SW7420-1123: add linux usermode IPC
#
# 28   8/26/10 6:01p mward
# SW7125-576:  Adding a list variable MAGNUM_NO_STRICT_ALIASING_SRCS, of
#  files to be compiled with -fno-strict-aliasing.  Only files with
#  strict-aliasing warnings caused by kernel headers or other non-
#  Broadcom source should be listed.
#
# 27   8/11/10 4:49p ttrammel
# SW7420-943: Merge NFE to main branch.
#
# NFE/1   5/10/10 5:47p ttrammel
# SW7405-4315: Initial NFE check-in.
#
# 26   11/24/09 1:06p erickson
# SW7420-457: remove security code from general nexus.inc file
#
# 25   11/17/09 2:02p erickson
# SW7405-3408: use $(filter) instead of $(findstring)
#
# 24   11/5/09 4:16p erickson
# SW7400-2511: add more build variables to NEXUS_OPTIONS
#
# 23   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 22   9/1/09 3:52p jgarrett
# SW7335-529: Replacing direct access to MAGNUM_MODULES with
#  NEXUS_EXTRA_MODULES to avoid magnum paths leaking into application
#  include paths
#
# 21   6/23/09 9:11a erickson
# PR54881: Provide emulation support in Nexus
#
# 20   4/8/09 4:31p vsilyaev
# PR 54016: Added support for precompiled header
#
# 19   3/26/09 11:06a vsilyaev
# PR 53611: Use simply expanded variables in complex substitutions
#
# 18   3/16/09 6:16p yili
# PR52990:Add region verification
#
# 17   3/16/09 5:28p vsilyaev
# PR 53225: Added suffix to the MAGNUM_PRECOMPILED_DIR
#
# 16   3/9/09 4:15p yili
# PR52990:Adding region verification to Nexus
#
# 15   11/6/08 12:45p erickson
# PR47312: added comment regarding PUBLIC_INCLUDES
#
# 14   10/6/08 4:04p vsilyaev
# PR 47440: Added option to link other static libraries with nexus
#
# 13   10/3/08 12:15p vsilyaev
# PR 45361: Fixed BTRC and BPROFILE support with -std=c89
#
# 12   10/3/08 11:18a vsilyaev
# PR 45361: Fixed BTRC and BPROFILE support with -std=c89
#
# 11   9/29/08 2:37p vsilyaev
# PR 47440: Added hooks that allow embedding external makefiles into the
#  nexus build process
#
# 10   9/25/08 10:25a erickson
# PR45361: default to c99 for BTRC_SUPPORT or BPROFILE_SUPPORT
#
# 9   9/16/08 9:20a erickson
# PR45458: convert SYSTEM to OS
#
# 8   6/27/08 8:58a erickson
# PR43846: convert to $(PERL)
#
# 7   6/4/08 11:21a rjlewis
# PR40352: can't set this for VxWorks.
#
# 6   4/21/08 7:29p jgarrett
# PR 41575: Adding binary object support
#
# 5   4/14/08 4:31p vsilyaev
# PR 41083: Added BPROFILE
#
# 4   3/27/08 5:09p rjlewis
# PR40352: file was being included twice during make process.
#
# 3   1/23/08 9:21p vobadm
# PR35457: update docs
#
# 2   1/23/08 3:17p vsilyaev
# PR 38682: Added support for linux kernel/userspace proxy mode
#
# 1   1/18/08 2:12p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/12   1/18/08 1:10p vsilyaev
# PR 38682: Repartition os_tools.inc and os_rules.inc, to os_tools,
# os_flags, os_flags_nexus and os_rules.inc
#
# Nexus_Devel/11   11/12/07 6:49p jgarrett
# PR 36953: Workaround for bad 3563 rap definitions
#
# Nexus_Devel/10   11/8/07 9:19a jgarrett
# PR 34416: Adding standard module define
#
# Nexus_Devel/9   10/8/07 5:04p jgarrett
# PR 34416: Ensuring syncthunk dir is available prior to vpath
#
# Nexus_Devel/8   10/2/07 11:54a jgarrett
# PR 34416: Removing nexus.h generation
#
# Nexus_Devel/7   9/24/07 4:18p jgarrett
# PR 35002: Adding platform to module include paths for
# platform_features.h.
#
# Nexus_Devel/6   9/21/07 11:19a erickson
# PR34254: rename platform to platforms
#
# Nexus_Devel/5   9/14/07 4:16p jgarrett
# PR 34416: Adding BDBG_DEBUG_BUILD
#
# Nexus_Devel/4   9/4/07 12:29p jgarrett
# PR 34416: Adding bchp and breg to nexus build
#
# Nexus_Devel/3   9/4/07 11:40a jgarrett
# PR 34416: Adding endian to nexus.h
#
# Nexus_Devel/2   8/31/07 6:28p jgarrett
# PR 34416: Adding nexus.h regeneration on header file addition
#
# Nexus_Devel/1   8/31/07 4:15p jgarrett
# PR 34416: Adding build framework
#
############################################################

############################################################
# Overview:
#
# This is an internal nexus Makefile include. It is used for processing the module .inc files and generated build rules.
# This file is included by nexus/platform/$(NEXUS_PLATFORM)/build/Makefile
#
# This file should never be included by applications.
# Applications should include nexus/platform/$(NEXUS_PLATFORM)/build/platform_app.inc into their Makefile.
# Please see nexus/build/nexus_defs.inc for a .inc which is common for internal nexus use and application use.
#
############################################################

# Make sure files are included in the correct order
ifndef NEXUS_DEFS_INCLUDED
$(error nexus_defs.inc was not included first.)
endif

ifneq ($(B_REFSW_OS),vxworks)
BSTD_OMIT_DEFAULT_CONFIG := 1
endif

# Create NEXUS_SOURCE_DIRS from all modules in NEXUS_MODULES
NEXUS_SOURCE_DIRS := $(sort $(foreach module,$(NEXUS_MODULES),$(dir $(NEXUS_$(module)_SOURCES))))
# Add the nexus source dirs to the search path for .c files.
ifdef NEXUS_SYNCTHUNK_DIR
$(shell $(MKDIR) $(NEXUS_SYNCTHUNK_DIR))
endif
vpath %.c $(NEXUS_SOURCE_DIRS)

ifeq ($(BPROFILE_SUPPORT),y)
NEXUS_CFLAGS_BPROFILE = -DB_HAS_BPROFILE=1 -finstrument-functions
endif


# Create NEXUS CFLAGS from base include path and defines
NEXUS_DEFINES := $(sort $(foreach module, $(NEXUS_MODULES),$(NEXUS_$(module)_DEFINES)))
NEXUS_DEFINES += $(foreach module, $(NEXUS_MODULES),NEXUS_HAS_$(module))
NEXUS_CFLAGS += -I$(NEXUS_TOP)/base/include/$(B_REFSW_OS)/config $(addprefix -D,$(NEXUS_DEFINES))
NEXUS_CFLAGS += $(addprefix -I,$(NEXUS_EXTRA_INCLUDES))
NEXUS_CFLAGS += $(addprefix -I,$(NEXUS_PLATFORM_PUBLIC_INCLUDES))
NEXUS_MAGNUM_PRECOMPILED_HEADER := ${MAGNUM}/basemodules/std/bstd.h
NEXUS_PRECOMPILED_HEADER := ${NEXUS_TOP}/base/include/nexus_base.h

# HUMAX_PLATFORM_BASE for HUMAX modification
ifeq ($(strip $(HUMAX_PLATFORM_BASE)),y)
NEXUS_CFLAGS += $(addprefix -include ,$(INC_AUTOCONF))
NEXUS_CFLAGS += $(addprefix -D,$(BCM_FIX))
endif

# Create NEXUS_XXX_CFLAGS and NEXUS_XXX_OBJECTS using PERL
$(shell $(PERL) $(NEXUS_TOP)/build/module_vars.pl $(NEXUS_BUILD_DIR)/module_vars.inc $(NEXUS_MODULES) $(NEXUS_STUB_MODULES))

# Include result into makefile
include $(NEXUS_BUILD_DIR)/module_vars.inc

# Include NEXUS OS-specific flags
include $(NEXUS_TOP)/build/os/$(B_REFSW_OS)/os_flags_nexus.inc

# Include Magnum Modules
NEXUS_MAGNUM_MODULES := $(foreach module,$(NEXUS_MODULES),$(NEXUS_$(module)_MAGNUM_MODULES))
include $(sort $(NEXUS_MAGNUM_MODULES))

# Include any extra modules requested by nexus .inc files as well.
MAGNUM_MODULES += $(sort $(NEXUS_EXTRA_MODULES))

# The 3563 RAP code has some .o files that should be .c
# Expand Magnum Variables
MAGNUM_INCLUDES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES)))

ifneq ($(filter ${NEXUS_MODE},client proxy),)
# For client/proxy environments, we only compile in a small subset of magnum modules
MAGNUM_SOURCES := $(patsubst %.o,%.c,$(sort $(foreach module, $(NEXUS_EXTRA_MODULES) ${MAGNUM_CLIENT_MODULES}, $($(module)_SOURCES))))
else
# Don't compile magnum modules that are accesses using proxy interface
MAGNUM_SOURCES := $(patsubst %.o,%.c,$(sort $(foreach module, ${MAGNUM_MODULES}, $($(module)_SOURCES))))
endif

MAGNUM_DEFINES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
MAGNUM_CFLAGS := $(addprefix -D,$(MAGNUM_DEFINES)) $(addprefix -I,$(MAGNUM_INCLUDES))
MAGNUM_SOURCE_DIRS := $(foreach module,$(MAGNUM_MODULES),$(dir $($(module)_SOURCES)))
MAGNUM_OPTIONS := $(sort $(foreach module,$(MAGNUM_MODULES),$($(module)_OPTIONS)))
MAGNUM_NOPROFILE_SRCS += bdbg.c bdbg_output.c bdbg_os_priv.c bkni.c
ifeq ($(B_REFSW_OS), linuxemu)
NEXUS_CFLAGS += $(addprefix -I,$(NEXUS_PLATFORM_PRIVATE_INCLUDES))
MAGNUM_NOPROFILE_SRCS += breg_client.c
else
MAGNUM_NOPROFILE_SRCS += breg_mem.c
endif
MAGNUM_STATIC_LIBS := $(sort $(foreach module,$(MAGNUM_MODULES),$($(module)_STATIC_LIBS)))

#MAGNUM_NO_STRICT_ALIASING_SRCS lists files which will be compile with -fno-strict-aliasing.  strict-aliasing is a useful and
#important optimization, which is turned off by including a file in this list.  Don't add files to this list unless strict-aliasing
#warnings are caused by code outside our control (Linux kernel headers).  If at all possible, fix the condition that causes
#the warning.
MAGNUM_NO_STRICT_ALIASING_SRCS := brpc_socket.c

# Set other makes
NEXUS_OTHER_MAKES := $(sort $(foreach module, $(NEXUS_MODULES),$(NEXUS_$(module)_MAKES))) $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_MAKES)))

# Remove pre-compiled files from the build
MAGNUM_PRECOMPILED_DIR := $(NEXUS_BUILD_DIR)/libmagnum.$(B_REFSW_OS).$(B_REFSW_ARCH)
MAGNUM_PRECOMPILED_SOURCES := $(patsubst %.$(NEXUS_OBJ_SUFFIX),%.c,$(notdir $(wildcard $(MAGNUM_PRECOMPILED_DIR)/*)))

# Check that all precompiled files are actually being requested by the build.  Otherwise, flags are probably mismatched somewhere.
$(foreach precomp,$(MAGNUM_PRECOMPILED_SOURCES),$(if $(filter $(precomp),$(notdir $(MAGNUM_SOURCES))),,$(warning precompiled file $(precomp) is not being used in your build)))

MAGNUM_OBJECTS := $(addprefix $(NEXUS_OBJ_DIR)/MAGNUM/,$(patsubst %.c,%.$(NEXUS_OBJ_SUFFIX),$(filter-out $(MAGNUM_PRECOMPILED_SOURCES),$(notdir $(MAGNUM_SOURCES)))))
MAGNUM_PRECOMPILED_OBJECTS += $(wildcard $(MAGNUM_PRECOMPILED_DIR)/*)

# Add the magnum source dirs to the search path for .c files.
vpath %.c $(MAGNUM_SOURCE_DIRS)

# Add all magnum CFLAGS to NEXUS_CFLAGS
NEXUS_CFLAGS += $(MAGNUM_CFLAGS)

# Include os-dependencies and expand nexus rules
# Sanity Check that we have included an OS-dependent set of defines
ifndef NEXUS_OBJ_SUFFIX
$(error NEXUS_OBJ_SUFFIX has not been defined.  Stop.)
endif

ifndef NEXUS_OBJ_DIR
$(error NEXUS_OBJ_DIR has not been defined.  Stop.)
endif

ifndef NEXUS_ENDIAN
$(error NEXUS_ENDIAN has not been defined.  Stop.)
endif

# Generate module rules using PERL
ifeq ($(GL_SUPPORT),y)
# for P3D, graphics3d is user-mode only even in kernel mode
MODULE_RULES_OPTIONS=-no_proxy GRAPHICS3D
endif
$(shell $(PERL) $(NEXUS_TOP)/build/os/$(B_REFSW_OS)/module_rules.pl $(NEXUS_BUILD_DIR)/module_rules.inc $(MODULE_RULES_OPTIONS) $(NEXUS_MODULES))

include $(NEXUS_BUILD_DIR)/module_rules.inc

# Create NEXUS_OPTIONS from list of all options
NEXUS_OPTIONS := $(sort $(foreach module, $(NEXUS_MODULES),$(NEXUS_$(module)_OPTIONS)))

# Create single list of all nexus objects for convenience
NEXUS_OBJECTS := $(foreach module,$(NEXUS_MODULES),$(NEXUS_$(module)_OBJECTS))

# Include OS-specific rules
include $(NEXUS_TOP)/build/os/$(B_REFSW_OS)/os_rules.inc

# Debug rule to dump all options specified in NEXUS_OPTIONS
define OPTION_template
@echo $(1)=$(if $($(1)),$($(1)),"<value not set>")

endef

NEXUS_OPTIONS += NEXUS_PLATFORM NEXUS_MODE B_REFSW_OS B_REFSW_DEBUG B_REFSW_ARCH B_REFSW_SHAREABLE NEXUS_POWER_MANAGEMENT B_REFSW_FIRMWARE

.PHONY: nexus_print_options
nexus_print_options:
ifneq ($(NEXUS_OPTIONS),)
	@echo "---Nexus Options---"
	$(foreach option,$(sort $(NEXUS_OPTIONS)),$(call OPTION_template,$(option)))
endif
ifneq ($(MAGNUM_OPTIONS),)
	@echo "---Magnum Options---"
	$(foreach option,$(sort $(MAGNUM_OPTIONS)),$(call OPTION_template,$(option)))
endif

# NOTE: It's important that NEXUS_$(module)_PUBLIC_INCLUDES not be rolled up into a single, global include path inside nexus.inc.
# Having per-module include paths is required for perserving modularity by preventing unmanaged calls from one module to another.
# See platform_app.inc for a rolled up NEXUS_$(module)_PUBLIC_INCLUDES usable by applications to access the entire Nexus public API.

