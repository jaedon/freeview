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
# $brcm_Workfile: Makefile $
# $brcm_Revision: 80 $
# $brcm_Date: 9/13/12 10:54p $
#
# Module Description:
#   This file defines the rules to build the 97400 platform
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/platforms/common/build/Makefile $
# 
# 80   9/13/12 10:54p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/13/12 7:03p mphillip
# SW7425-3739: Update makefiles for new linux/driver/build hierarchy
# 
# 79   9/5/12 9:25a erickson
# SW7435-339: don't auto-build server in NEXUS_WEBCPU mode
# 
# 78   8/23/12 2:05p erickson
# SW7425-3260: restore nexus_headers for MODE=proxy
# 
# 77   8/20/12 3:36p erickson
# SW7425-3260: NEXUS_MODE=client will build both client and server
# 
# 76   7/31/12 9:19a erickson
# SW7435-235: location of Makefile.app moved
# 
# 75   7/27/12 2:14p jgarrett
# SW7425-3281: Merge to main branch
# 
# 74   7/24/12 1:44p erickson
# SW7435-235: set NEXUS_PLATFORM_BUILDDIR to specific dir, not common dir
# 
# 73   7/24/12 10:04a erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# SW7425-3281/2   7/18/12 9:40a jgarrett
# SW7425-3281: Merging platform changes for 7231 release
# 
# 72   7/10/12 1:45p vsilyaev
# SW7435-256: Use BLST_AAT implementation of AA Tree
# 
# SW7425-3281/1   6/20/12 6:06p piyushg
# SW7425-3281: Add "wmdrmpd", "wmdrmpd_core" and "wmdrmpd_io" modules to
#  nexus platform.
# 
# 71   6/4/12 4:23p erickson
# SW7435-27: use BINT for core1 interrupts
# 
# 70   5/9/12 10:55a erickson
# SW7435-27: rework NEXUS_WEBCPU env variable to support core0
#  client/server, core1_server, and eventually core1_client
# 
# 69   5/8/12 5:33p erickson
# SW7435-27: add webcpu support
# 
# 68   4/3/12 9:19a erickson
# SW7425-2607: restore verification
# 
# 67   3/30/12 1:51p erickson
# SW7425-2607: back out verification until internal problem is solved
# 
# 66   3/30/12 10:12a erickson
# SW7425-2607: pass in NEXUS_TOP
# 
# 65   3/29/12 1:03p erickson
# SW7425-2607: verify that no nexus module #includes platform code in its
#  public api
# 
# 64   3/8/12 4:45p erickson
# SW7425-1969: add NEXUS_HEADERS=y option and nexus.cpp.h output
# 
# 63   10/20/11 3:13p jessem
# SW7425-1339: Merged changes from SW7425-1339 branch.
# 
# SW7425-1339/1   9/23/11 9:23p alexpan
# SW7425-1339: Disable logger module for Android builds
# 
# 62   8/22/11 3:59p jhaberf
# SWDTV-8321: resolve build issue due to bin directory not being created
# 
# 61   8/3/11 5:33p vsilyaev
# SW7420-1465, SW7405-5221: Route debug output to the special FIFO
#  instead of syslog buffer
# 
# 60   6/30/11 4:00p jhaberf
# SWDTV-7634: Makefile clean up with move to latest main line BSEAV
#  makefiles
# 
# 59   6/29/11 6:07p ttrammel
# SW7420-1819: Create bin directory in NFE version.
# 
# 58   6/23/11 1:20p ttrammel
# SW7420-1819: Remove nexus.ko from bin dir on clean.
# 
# 57   6/23/11 12:59p ttrammel
# SW7420-1819: Fix problem with driver copy to bin directory.
# 
# 56   6/6/11 3:39p vsilyaev
# SW7405-4477: Fixed typo
# 
# 55   6/6/11 3:34p vsilyaev
# SW7405-4477: Routed all debug output through buffer and use external
#  application to extract and print debug output
# 
# 54   6/6/11 10:55a ttrammel
# SW7420-1819: Update NEXUS/pi for NFE 2.0.
# 
# 53   5/18/11 5:20p nickh
# SW7425-545: Add MUXLIB's to fix build issue with nexus 3D examples
# 
# 52   5/17/11 12:02p jhaberf
# SWDTV-6928: Merge of 35233 platform code to the main
# 
# 51   5/3/11 5:08p erickson
# SW7420-1819: repartition so NFE is a standard feature
#
# 50   4/11/11 5:07p vsilyaev
# SW7425-232: Compile mp4_file mux module
#
# 49   4/7/11 6:13p vsilyaev
# SW7425-232: Updated file mux module
#
# 48   3/24/11 11:48a jgarrett
# SW7422-146: Fixing kernel mode builds
#
# 47   3/2/11 10:45a erickson
# SW7422-310: BMEM is required in proxy for nexus_map
#
# 46   2/11/11 8:39p vsilyaev
# SW7425-81: Added image interface plumbing to the video encoder module
#
# 45   1/19/11 2:03p erickson
# SW7420-1123: socket-based usermode IPC
#
# 44   12/7/10 5:47p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
#
# 43   10/21/10 2:43p mphillip
# SW7125-670: Correct name of P3D modules
#
# 42   10/21/10 2:35p mphillip
# SW7125-670: Add 3D objects to user-space
#
# 41   10/20/10 2:01p randyjew
# SW7468-129: Undo add of BXDM to Magnum client modules
#
# 40   10/18/10 3:55p randyjew
# SW7468-129: Include BXDM to MAGNUM_CLIENT_MODULES
#
# 39   10/11/10 10:45a mward
# SW7420-1123: MAGNUM_CLIENT_MODULES for GL_SUPPORT s/b   ifeq not ifneq.
#
# 38   10/4/10 11:46a gmohile
# SW7408-126 : Add BAPE_IMG to proxy build
#
# 37   9/29/10 10:53a erickson
# SW7420-1123: refactor platform_modules.inc so that it can fork per
#  platform
#
# 36   8/12/10 1:13p ttrammel
# SW7420-943: Merge NFE to main branch.
#
# 35   8/5/10 5:18p mward
# SW7125-497: use NEXUS_POWER_MANAGEMENT instead of NEXUS_FRONTEND_PWM.
#
# 34   6/28/10 5:19p ayxiang
# SW7125-497: for frontend power management feature
#
# 33   3/29/10 10:25a mward
# SW7325-649:Restore lost versions.  Use B_REFSW_DEBUG per SW7400-2511
#  and make sure it is passed to driver make.
#
#
# 31   2/12/10 10:24a erickson
# SW7405-3833: add nexus_headers option to copy header files and static
#  platform_app.inc to nexus/bin/include
#
# 30   9/16/09 3:16p erickson
# SW7400-2511: move nexus_platforms.inc into platform build dir
#
# 32   3/26/10 7:17p mananp
# SW7325-649:Missing bcmdriver.ko when DEBUG=n set for Nexus
#
# 29   9/16/09 1:42p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 28   8/31/09 4:39p erickson
# SWDEPRECATED-3366: added PLATFORM_MODULES_INC override
#
# 27   8/26/09 12:05p mward
# SW7125-4: Adding 97019 platform.
#
# 26   8/25/09 11:34a nickh
# SW7420-331: Add 97410c support
#
# 25   8/19/09 11:56a jrubio
# PR55232: add SPI frontpanel support for 7340/7342
#
# 24   7/30/09 10:53a gmohile
# PR 56512 : Add 7403 support
#
# 23   7/16/09 5:11p mward
# PR 55545: Support 97125 platform.
#
# 22   6/1/09 11:50a robertwm
# PR55602: Missing endif in Makefile and platform_app.inc
#
# 21   5/29/09 6:01p jrubio
# PR55232: add 7340/7342
#
# 20   5/15/09 12:32p erickson
# PR54843: include nexus_defs.inc after defining BCHP_CHIP
#
# 19   4/27/09 6:18p jrubio
# PR44849: fix splash, add NEXUS_BASE_ONLY support
#
# 18   4/9/09 11:41a jrubio
# PR52188: add support for 2nd Receiver
#
# 17   3/13/09 2:47p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 16   3/2/09 12:33p katrep
# PR52599: Add support for 7205
#
# 15   1/28/09 10:23a vsilyaev
# PR 51629: Added missing endif
#
# 14   1/27/09 4:41p jrubio
# PR51629: add 7336
#
# 13   12/19/08 7:48p katrep
# PR50567: Add support for 97466 platform
#
# 12   12/5/08 12:31p nickh
# PR50043: Update to reflect change in PLATFORM variable name from
#  97420DVR2 to 97420dvr2
#
# 11   11/19/08 2:44p nickh
# PR48963: Add 97420DVR2 support
#
# 10   9/29/08 2:34p vsilyaev
# PR 47440: Added hooks that allow embedding external makefiles into the
#  nexus build process
#
# 9   9/23/08 9:16a jgarrett
# PR 46513: Merging 97459 support to mainline
#
# 8   8/22/08 7:14p katrep
# PR45975: 7405 B0 should be default build
#
# PR44830/1   7/16/08 5:42p anilmm
# PR44830: Add support for 97459
#
# 7   6/23/08 1:33p katrep
# PR43038: Fixed BE builds.
#
# 6   5/20/08 6:04p jgarrett
# PR 42897: Validating nexus/bin on bcmdriver install
#
# 5   4/30/08 1:54p jrubio
# PR42353: add 7335
#
# 4   3/4/08 5:22p jrubio
# PR40169: add 7325
#
# 3   1/23/08 9:21p vobadm
# PR35457: update docs
#
# 2   1/23/08 3:13p vsilyaev
# PR 38682: Added support for linux kernel/userspace proxy mode
#
# 1   1/18/08 2:17p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/17   10/16/07 4:19p erickson
# PR36152: add 97405
#
# Nexus_Devel/16   10/12/07 12:45p jgarrett
# PR 35551: Fixing output from usermode driver
#
# Nexus_Devel/15   10/4/07 10:36a erickson
# PR34416: fix bcmdriver make, use Q_
#
# Nexus_Devel/14   10/2/07 11:54a jgarrett
# PR 34416: Revising build rules for easier platform integration
#
# Nexus_Devel/13   9/28/07 11:32a erickson
# PR34926: rename videodecoder to video_decoder
#
# Nexus_Devel/12   9/28/07 10:06a erickson
# PR35395: moved NEXUS_SYNCTHUNK_DIR to bin. added syncthunk-clean.
#
# Nexus_Devel/11   9/27/07 7:51p jgarrett
# PR 34954: Adding audio
#
# Nexus_Devel/10   9/27/07 2:02p erickson
# PR35395: added syncthunk
#
# Nexus_Devel/9   9/25/07 12:34p vsilyaev
# PR 34662: Added display module
#
# Nexus_Devel/8   9/24/07 3:21p erickson
# PR34926: added surface, transport and videodecoder to standard build
#
# Nexus_Devel/7   9/21/07 6:26p jgarrett
# PR 35002: Adding usermode driver
#
# Nexus_Devel/6   9/14/07 4:17p erickson
# PR34925: added transport and videodecoder
#
# Nexus_Devel/5   9/13/07 6:16p vsilyaev
# PR 34419: Added display and surface modules
#
# Nexus_Devel/4   9/6/07 1:03p vsilyaev
# PR 34419: Added core module
#
# Nexus_Devel/3   8/31/07 6:29p jgarrett
# PR 34416: Switching pwd to wildcard.
#
# Nexus_Devel/2   8/31/07 4:21p jgarrett
# PR 34416: Adding ifdef for 94701
#
# Nexus_Devel/1   8/31/07 4:15p jgarrett
# PR 34416: Adding platform makefile
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../.. && cd)
else
NEXUS_TOP := $(shell cd ../../..; pwd)
endif

# Include nexus definitions
include $(NEXUS_TOP)/build/nexus_defs.inc
NEXUS_PLATFORM_BUILDDIR=$(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build
include $(NEXUS_TOP)/platforms/common/build/nexus_platforms.inc

# Include platform modules
# allow outside override
ifeq ($(PLATFORM_MODULES_INC),)
PLATFORM_MODULES_INC=$(NEXUS_TOP)/platforms/common/build/platform_modules.inc
endif
include $(PLATFORM_MODULES_INC)


####################################################
# List the nexus and magnum modules which are compiled into the proxy/client.
# This list is determined by the nexus architecture and does not vary per chip.
# It's easier to maintain a small inclusion list than a large exclusion list.

# NEXUS_CLIENT_MODULES is the list of nexus modules compiled into the client/proxy library.
# It is used nexus/build/os/linuxuser/module_rules.pl, et al.
NEXUS_CLIENT_MODULES := RECORD FILE PLAYBACK WMDRMPD

# MAGNUM_CLIENT_MODULES is the list of magnum modules compiled into the client/proxy library.
# It is used in nexus/build/nexus.inc
MAGNUM_CLIENT_MODULES := BKNI BDBG BVLC BLST
ifneq ($(NEXUS_CONFIG_IMAGE),)
	# TODO: use BXVD_IMG_ONLY and BRAP_IMG_ONLY for client/proxy-only
	MAGNUM_CLIENT_MODULES += BXVD BRAP BAPE_IMG BVCE BDSP
endif
ifeq ($(GL_SUPPORT),y)
	# the following are required in user space when building with 3D support
	MAGNUM_CLIENT_MODULES += BINT BPXL BREG BSUR BTMR
	MAGNUM_CLIENT_MODULES += OPENGLES BP3D
endif
NEXUS_CLIENT_MODULES += FILE_MUX
MAGNUM_CLIENT_MODULES += BMUXLIB_FILE_MP4 BMUXLIB_FILE_ASF BMUXLIB_FILE_COMMON

ifeq ($(NEXUS_MODE),client)
ifeq ($(NEXUS_WEBCPU),core1_server)
NEXUS_CLIENT_MODULES += GRAPHICS2D
# build the full implementations of the following magnum modules
MAGNUM_CLIENT_MODULES += BGRC BGRCLIB BFMT BSUR BREG BPXL BINT
endif
endif

# NOTE: platform_modules.inc must also exclude the modules in NEXUS_CLIENT_MODULES

####################################################


ifeq ($(B_REFSW_OS),linuxuser)
ifeq ($(B_REFSW_FIRMWARE),y)
# NFE is a three pass build:
# build nexus with OS=linuxuser MODE=proxy (same as non-NFE)
# build nfe_driver (standalone driver)
# build nexus with OS=bare MODE=driver (the firmware, no linux)
EXTRA_ALL := nexusdriver nexusfirmware 
EXTRA_CLEAN := nexusdriver_clean nexusfirmware_clean 
else
ifeq ($(NEXUS_MODE),proxy)
# kernel mode is a two pass build:
# OS=linuxuser MODE=proxy
# OS=linuxkernel MODE=driver
EXTRA_ALL := nexusdriver 
EXTRA_CLEAN := nexusdriver_clean
else
ifeq ($(NEXUS_MODE),client)
ifeq ($(NEXUS_WEBCPU),)
# usermode multiprocess is a two pass build:
# OS=linuxuser NEXUS_MODE=client
# OS=linuxuser NEXUS_MODE=
EXTRA_ALL := nexusserver
EXTRA_CLEAN := nexusserver_clean
endif
else
# user mode is a one pass build which also builds bcmdriver
EXTRA_CLEAN := bcmdriver_clean 
EXTRA_INSTALL := bcmdriver_install 
endif
endif
endif
ifneq ($(B_REFSW_ANDROID),y)
EXTRA_CLEAN += logger_clean
EXTRA_INSTALL += logger_install
endif
endif

ifeq ($(filter $(NEXUS_MODE),client),)
.PHONY: nexus_headers
ifeq ($(NEXUS_HEADERS),y)
# automatically run "make nexus_headers" with normal nexus build
EXTRA_ALL   += nexus_headers
EXTRA_CLEAN += clean_nexus_headers
endif

.PHONY: verify_module_api clean_verify_module_api
EXTRA_ALL   += verify_module_api
EXTRA_CLEAN += clean_verify_module_api
endif

# Locally define rules -- all is the default rule
all: install

# bcmdriver rules
.PHONY: bcmdriver
bcmdriver:
	@echo "[Build.... bcmdriver]"
	${Q_}$(MAKE) -C $(BSEAV)/linux/driver/build B_REFSW_DEBUG=$(B_REFSW_DEBUG) --no-print-directory > /dev/null

ifeq ($(B_REFSW_DEBUG),n)
NEXUS_OBJ_DIR_SUFFIX=release
else
NEXUS_OBJ_DIR_SUFFIX=debug
endif

.PHONY: bcmdriver_clean bcmdriver_install
bcmdriver_install: bcmdriver
	@echo "[Install... bcmdriver]"
	${Q_}$(CP) $(BSEAV)/linux/driver/build/$(B_REFSW_ARCH).${NEXUS_OBJ_DIR_SUFFIX}${NEXUS_BIN_DIR_SUFFIX}/bcmdriver.ko ${NEXUS_BIN_DIR}
bcmdriver_clean:
	${Q_}$(MAKE) -C $(BSEAV)/linux/driver/build B_REFSW_DEBUG=$(B_REFSW_DEBUG) clean --no-print-directory > /dev/null

ifeq ($(B_REFSW_FIRMWARE),test)
.PHONY: nexusdriver_clean nexusdriver
NFE_BUILDDIR=$(NEXUS_TOP)/platforms/common/build/nfe_test
nexusdriver:
	@echo "[Build... test firmware + driver]"
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) B_REFSW_OS=bare NEXUS_MODE=driver
	${Q_}$(MAKE) -C $(NFE_BUILDDIR)
	-${Q_}${MKDIR} ${NEXUS_BIN_DIR}
	${Q_}$(CP) $(NFE_BUILDDIR)/nexus.ko $(NEXUS_BIN_DIR)
nexusdriver_clean:
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) B_REFSW_OS=bare NEXUS_MODE=driver clean
	${Q_}$(MAKE) -C $(NFE_BUILDDIR) clean
	${Q_}$(RM) -f ${NEXUS_BIN_DIR}/nexus.ko
else
ifeq ($(B_REFSW_FIRMWARE),y)
.PHONY: nexusdriver_clean nexusdriver nexusfirmware_clean nexusfirmware
NFE_BUILDDIR=$(NEXUS_TOP)/build/nfe_driver
nexusdriver:
	@echo "[Build... nfe driver]"
	${Q_}$(MAKE) -C $(NFE_BUILDDIR)
	-${Q_}${MKDIR} ${NEXUS_BIN_DIR}
	${Q_}$(CP) $(NFE_BUILDDIR)/nexus.ko $(NEXUS_BIN_DIR)
nexusdriver_clean:
	${Q_}$(MAKE) -C $(NFE_BUILDDIR) clean
	${Q_}$(RM) -f ${NEXUS_BIN_DIR}/nexus.ko
nexusfirmware:
	@echo "[Build... firmware]"
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) B_REFSW_OS=bare NEXUS_MODE=driver 
${NEXUS_BIN_DIR}/nexus_firmware.elf:
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) B_REFSW_OS=bare NEXUS_MODE=driver install
nexusfirmware_clean:
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) B_REFSW_OS=bare NEXUS_MODE=driver clean
else
ifeq ($(NEXUS_MODE),proxy)
.PHONY: nexusdriver_clean nexusdriver
nexusdriver:
	@echo "[Build... driver]"
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) B_REFSW_OS=linuxkernel NEXUS_MODE=driver
${NEXUS_BIN_DIR}/nexus.ko:
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) B_REFSW_OS=linuxkernel NEXUS_MODE=driver install
nexusdriver_clean:
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) B_REFSW_OS=linuxkernel NEXUS_MODE=driver clean
else
ifeq ($(NEXUS_MODE),client)
.PHONY: nexusserver_clean nexusserver
nexusserver:
	@echo "[Build... usermode multiprocess]"
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) NEXUS_MODE=
nexusserver_clean:
	${Q_}$(MAKE) -C $(NEXUS_PLATFORM_BUILDDIR) NEXUS_MODE= clean
endif
endif
endif
endif

# generic install and clean rules
install: nexus_install $(EXTRA_INSTALL)

nexus_install $(EXTRA_INSTALL): prep_dir

.PHONY: prep_dir
prep_dir:
	${Q_}${MKDIR} ${NEXUS_BIN_DIR}

clean: nexus_clean $(EXTRA_CLEAN)

# Include Nexus build last -- This defines nexus_install, nexus_clean, etc.
include $(NEXUS_TOP)/build/nexus.inc

.PHONY: logger_clean logger_install
logger_clean:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/utils/logger clean --no-print-directory

logger_install:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/utils/logger install --no-print-directory

nexus_headers:
	${Q_}echo Copying header files and platform_app.inc to ${NEXUS_BIN_DIR}/include
	${Q_}${MAKE} -C $(NEXUS_TOP)/platforms/common/build -f Makefile.app NEXUS_TOP=${NEXUS_TOP} NEXUS_PLATFORM=${NEXUS_PLATFORM} NEXUS_BIN_DIR=${NEXUS_BIN_DIR}

clean_nexus_headers:
	${Q_}$(RM) -rf ${NEXUS_BIN_DIR}/include

verify_module_api clean_verify_module_api:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/build -f Makefile.verify $@ NEXUS_TOP=$(NEXUS_TOP) NEXUS_PLATFORM=$(NEXUS_PLATFORM)
