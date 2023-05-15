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
# $brcm_Workfile: platform_app.inc $
# $brcm_Revision: 28 $
# $brcm_Date: 7/24/12 10:04a $
#
# Module Description:
#   This file defines the variables needed to build an app using
#   nexus.  The output is two variables. NEXUS_APP_INCLUDE_PATHS
#   is a list of include paths needed to find the relevant files
#   needed to build a nexus applications.  NEXUS_APP_DEFINES
#   is a list of definitions needed to be defined in the app.
#   In order to use these variables, you need to convert to the
#   style your compiler needs -- e.g.
#   $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) and
#   $(addprefix -D,$(NEXUS_APP_DEFINES))
#
# Revision History:
# $brcm_Log: /nexus/platforms/common/build/platform_app.inc $
# 
# 28   7/24/12 10:04a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 27   3/29/12 1:02p erickson
# SW7425-2607: add option to exclude PLATFORM
# 
# 26   8/9/11 11:46a vsilyaev
# SW7420-1849: Added BSEAV/lib/bfile path for applications
# 
# 25   12/9/10 3:33p erickson
# SW7420-1308: add NEXUS_STUB_MODULES so that stub modules can have their
#  API included but still avoid having NEXUS_HAS_<MODULE> defined
# 
# 24   7/27/10 2:54p erickson
# SW7405-4621: nexus/base/include is no longer includeable by apps
#
# 23   7/14/10 10:32a erickson
# SW7405-4621: prepare for removal of nexus/base/include from app include
#  path
#
# 22   9/16/09 3:16p erickson
# SW7400-2511: move nexus_platforms.inc into platform build dir
#
# 21   9/16/09 1:42p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 20   9/1/09 3:52p jgarrett
# SW7335-529: Replacing direct access to MAGNUM_MODULES with
#  NEXUS_EXTRA_MODULES to avoid magnum paths leaking into application
#  include paths
#
# 19   8/26/09 12:06p mward
# SW7125-4: Adding 97019 platform.
#
# 18   8/25/09 12:37p erickson
# SW7420-331: fix last checkin
#
# 17   8/25/09 11:37a nickh
# SW7420-331: Add 97410c support
#
# 16   7/30/09 11:07a gmohile
# PR 56512 : Merge 7403 support to mainline
#
# 15   7/15/09 6:39p mward
# PR 55545: Support 7125.
#
# 13   6/1/09 11:44a robertwm
# PR55062: Missing endif in Makefile and platform_app.inc
#
# 12   5/29/09 6:02p jrubio
# PR55232: add 7340/7342
#
# 11   3/5/09 3:18p katrep
# PR52599: More support for 7205
#
# 10   3/2/09 12:33p katrep
# PR52599: Add support for 7205
#
# 9   1/28/09 10:23a vsilyaev
# PR 51629: Added missing endif
#
############################################################
ifndef NEXUS_TOP
$(error NEXUS_TOP is not defined)
endif

# Include nexus definitions
include $(NEXUS_TOP)/build/nexus_defs.inc
include $(NEXUS_TOP)/platforms/common/build/nexus_platforms.inc

# Include nexus modules
ifeq ($(PLATFORM_MODULES_INC),)
PLATFORM_MODULES_INC=$(NEXUS_TOP)/platforms/common/build/platform_modules.inc
endif
include $(PLATFORM_MODULES_INC)

# Remove BASE from public API. Leaving it in for now to ease the transition.
NEXUS_MODULES := $(patsubst BASE,,$(NEXUS_MODULES))

ifeq ($(NEXUS_EXCLUDE_PLATFORM),y)
NEXUS_MODULES := $(patsubst PLATFORM,,$(NEXUS_MODULES))
endif

# Convert include paths into single variable
NEXUS_APP_INCLUDE_PATHS := $(foreach module, $(NEXUS_MODULES) $(NEXUS_STUB_MODULES), $(NEXUS_$(module)_PUBLIC_INCLUDES))
NEXUS_APP_DEFINES := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_DEFINES))
NEXUS_APP_DEFINES += $(foreach module, $(NEXUS_MODULES),NEXUS_HAS_$(module))

# Sanity check that no modules have leaked into the build system
ifneq ($(MAGNUM_MODULES),)
$(error MAGNUM_MODULES should not be defined.  It's defined as $(MAGNUM_MODULES))
endif

# Add in magnum base module paths also
include $(MAGNUM)/basemodules/kni/bkni.inc
include $(MAGNUM)/basemodules/std/bstd.inc
include $(MAGNUM)/basemodules/dbg/bdbg.inc
include $(MAGNUM)/basemodules/chp/bchp.inc
include $(MAGNUM)/basemodules/reg/breg.inc
include $(MAGNUM)/basemodules/err/berr.inc
include $(MAGNUM)/commonutils/lst/blst.inc

# Convert magnum includes into the same variable
NEXUS_APP_INCLUDE_PATHS += $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES))
NEXUS_APP_DEFINES += $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES))

ifeq ($(filter FILE, ${NEXUS_MODULES}), FILE)
include $(BSEAV)/lib/bfile/bfile.inc
NEXUS_APP_INCLUDE_PATHS += ${BFILE_INCLUDES}
endif



