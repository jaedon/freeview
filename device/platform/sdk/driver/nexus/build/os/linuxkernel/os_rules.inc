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
# $brcm_Workfile: os_rules.inc $
# $brcm_Revision: 28 $
# $brcm_Date: 5/19/12 2:16p $
#
# Module Description:
#   This file defines the OS-specific rules for building in linuxkernel mode.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/linuxkernel/os_rules.inc $
# 
# 28   5/19/12 2:16p ttrammel
# SWANDROID-97: Add new Android toolchain prefix.
# 
# SWANDROID-97/1   5/15/12 12:27p alexpan
# SWANDROID-97: Add new Android toolchain to list of mips ARCHs.
# 
# 27   3/9/12 9:32a ttrammel
# SWANDROID-44: Add Android toolchain to list of mips ARCHs.
# 
# SWANDROID-44/1   3/9/12 9:51p alexpan
# SWANDROID-44: Add MIPS toolchain support when specifying ARCH
# 
# 26   3/5/12 10:59a mward
# SW7435-37:  "make modules" invocation needs to specify ARCH.
# 
# 25   2/8/12 3:21p ttrammel
# SW7425-2063: Pass crosstool prefix used for NEXUS to Kbuild.
# 
# SW7425-2063/1   12/28/11 12:23p alexpan
# SW7425-2063: Fix cross-compile build error using MIPS toolchain
# 
# 24   10/24/11 9:03a erickson
# SW7346-517: don't use kbuild for linux 2.6.18
#
# 23   10/21/11 4:51p erickson
# SW7346-517: use kbuild for nexus.ko
#
# 22   8/10/11 2:50p erickson
# SW7420-1123: add nexus.client script
#
# 21   5/3/11 5:08p erickson
# SW7420-1819: repartition so NFE is a standard feature
#
# 20   8/26/10 6:02p mward
# SW7125-576:  Adding a list variable MAGNUM_NO_STRICT_ALIASING_SRCS, of
#  files to be compiled with -fno-strict-aliasing.  Only files with
#  strict-aliasing warnings caused by kernel headers or other non-
#  Broadcom source should be listed.
#
# 19   8/12/10 1:10p ttrammel
# SW7420-943: Merge NFE to main branch.
#
# NFE/3   5/18/10 3:28p ttrammel
# SW7405-4315: Initial NFE check-in. Add quiet option.
#
# NFE/2   5/11/10 6:36p ttrammel
# SW7405-4315: Initial NFE check-in.
#
# NFE/1   5/11/10 11:08a ttrammel
# SW7405-4315: Initial NFE check-in.
#
# 18   11/10/09 11:08a erickson
# SW7405-3351: add generate_nexus_driver_modules.pl to auto-generate
#  nexus_driver_modules.h based on NEXUS_MODULES list
#
# 17   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 16   4/30/09 10:56a erickson
# PR53225: merge
#
# PR53225/1   4/29/09 4:59p shyam
# PR53225 : Improved clean rule to remove perl outputs
#
# 15   4/8/09 4:31p vsilyaev
# PR 54016: Added support for precompiled header
#
# 14   4/1/09 4:46p vsilyaev
# PR 53225: Fixed nexus_clean rule to delete right files
#
# 13   3/30/09 3:50p vsilyaev
# PR 53611: Create the object directory only when needed
#
# 12   3/26/09 11:07a vsilyaev
# PR 53611: Don't link static libraries unless directed
#
# 11   3/13/09 2:44p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 10   11/5/08 12:15p vsilyaev
# PR 48694: Separated handling of linuxkernel and linuxkernel with
#  driver/ioctls/proxy
#
# 9   8/1/08 12:33p vsilyaev
# PR 45320: Added rules to build linux include path
#
# 8   6/10/08 3:26p vsilyaev
# PR 43184: Fixed clean rule
#
# 7   4/21/08 7:29p jgarrett
# PR 41575: Adding precompiled magnum objects
#
# 6   2/29/08 1:15p erickson
# PR38682: update kernel mode install
#
# 5   2/22/08 2:28p erickson
# PR36808: cp "nexus" script to nexus/bin on make
#
# 4   2/7/08 11:06p vsilyaev
# PR 38682: Strip debug information from the loadable module
#
# 3   1/23/08 9:21p vobadm
# PR35457: update docs
#
# 2   1/23/08 3:15p vsilyaev
# PR 38682: Added support for linux kernel/userspace proxy mode
#
# 1   1/18/08 2:15p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/3   1/18/08 1:10p vsilyaev
# PR 38682: Repartition os_tools.inc and os_rules.inc, to os_tools,
# os_flags, os_flags_nexus and os_rules.inc
#
# Nexus_Devel/2   1/15/08 11:12a vsilyaev
# PR 38682: Improved linuxkernel build
#
#
############################################################

ifeq ($(filter ${B_REFSW_ARCH}, mipsel-linux mipsel-uclibc mipsel-linux-uclibc mips-linux mips-uclibc mips-linux-uclibc mips-linux-gnu mips-linux-android), ${B_REFSW_ARCH})
B_REFSW_LINUX_ARCH=mips
endif

nexus_install: nexus_all
	@echo [Install... driver]
	$(Q_)$(MKDIR) ${NEXUS_BIN_DIR}
ifeq ($(NEXUS_MODE),driver)
	@$(CP) $(NEXUS_DRIVER) ${NEXUS_BIN_DIR}
ifeq ($(B_REFSW_FIRMWARE),y)
	@echo [Install... firmware]
	@$(CP) $(subst nexus.ko,nexus_firmware.elf,$(NEXUS_DRIVER)) ${NEXUS_BIN_DIR}
endif
else
ifdef NEXUS_STATIC_LIB
	@$(CP) $(NEXUS_STATIC_LIB) ${NEXUS_BIN_DIR}
	@$(CP) $(MAGNUM_LIB) ${NEXUS_BIN_DIR}
endif
endif
	@$(CP) $(NEXUS_TOP)/build/nexus.linuxkernel ${NEXUS_BIN_DIR}/nexus
	@$(CP) $(NEXUS_TOP)/build/nexus.client ${NEXUS_BIN_DIR}

ifeq ($(NEXUS_MODE),driver)
nexus_all: ${LINUX_INC} $(NEXUS_DRIVER) $(MAGNUM_LIB)
else
nexus_all: ${LINUX_INC} $(MAGNUM_LIB) ${NEXUS_STATIC_LIB}
endif

# Generate nexus_driver_modules.h using PERL
$(shell $(PERL) $(NEXUS_TOP)/build/os/$(B_REFSW_OS)/generate_nexus_driver_modules.pl $(NEXUS_SYNCTHUNK_DIR)/nexus_driver_modules.h $(NEXUS_MODULES))

$(NEXUS_OBJ_DIR)/MAGNUM/exists:
	-$(Q_)$(MKDIR) $@

${NEXUS_MAGNUM_PRECOMPILED_HEADER_H}.gch: ${NEXUS_MAGNUM_PRECOMPILED_HEADER} ${NEXUS_OBJ_DIR}/MAGNUM/exists
	@echo "[Compile... $(notdir $<)]"
	$(Q_)$(CC) $(CDEP_FLAG) $(NEXUS_CFLAGS) -c $< -o $@

${NEXUS_PRECOMPILED_HEADER_H}.gch: ${NEXUS_PRECOMPILED_HEADER} ${NEXUS_OBJ_DIR}/MAGNUM/exists
	@echo "[Compile... $(notdir $<)]"
	$(Q_)$(CC) $(CDEP_FLAG) $(NEXUS_CFLAGS) ${NEXUS_BASE_CFLAGS} -c $< -o $@

# Rule for magnum sources
$(MAGNUM_OBJECTS): $(NEXUS_OBJ_DIR)/MAGNUM/%.$(NEXUS_OBJ_SUFFIX): %.c $(NEXUS_OBJ_DIR)/MAGNUM/exists ${NEXUS_MAGNUM_PRECOMPILED_HEADER_H}.gch
	@echo "[Compile... $(notdir $<) (magnum)]"
	$(Q_)$(CC) $(CDEP_FLAG) -include ${NEXUS_MAGNUM_PRECOMPILED_HEADER_H} $(if $(filter $(notdir $<),$(MAGNUM_NO_STRICT_ALIASING_SRCS)),-fno-strict-aliasing,) $(NEXUS_CFLAGS) -c $< -o $@

ifdef NEXUS_STATIC_LIB
# Rule for magnum library
$(MAGNUM_LIB): $(MAGNUM_OBJECTS) $(MAGNUM_PRECOMPILED_OBJECTS)
	@echo "[Archive... $(notdir $@)]"
	-$(Q_)$(AR) rcs $@ $^

# Rule for nexus library
$(NEXUS_STATIC_LIB): $(NEXUS_OBJECTS) $(MAGNUM_OBJECTS) $(MAGNUM_PRECOMPILED_OBJECTS)
	@echo "[Archive... $(notdir $@)]"
	-$(Q_)$(AR) rcs $@ $^
endif

# Rule for nexus driver
$(NEXUS_DRIVER): $(NEXUS_OBJECTS) $(MAGNUM_OBJECTS) $(MAGNUM_PRECOMPILED_OBJECTS) $(NEXUS_EXTRALIBS)
	@echo [Link...... nexus.ko]
ifeq ($(LINUX_VER_GE_2_6_31),y)
	${Q_}$(LD) -Map ${NEXUS_DRIVER}.map ${NEXUS_LDFLAGS} $(NEXUS_EXTRALDFLAGS) --strip-debug -r $^  -o $(NEXUS_OBJ_DIR)/nexus.o_shipped
	# build a dynamic Kbuild file
	@echo "obj-m += nexus.o" >$(NEXUS_OBJ_DIR)/Kbuild
	@echo "nexus-y += nexus.o_shipped" >>$(NEXUS_OBJ_DIR)/Kbuild
	$(MAKE) -C $(LINUX) M=$(NEXUS_OBJ_DIR) ARCH=$(B_REFSW_LINUX_ARCH) modules PLATFORM=$(NEXUS_PLATFORM) CROSS_COMPILE=$(B_REFSW_CROSS_COMPILE)
else
	@# do not use kbuild for 2.6.18
	${Q_}$(LD) -Map ${NEXUS_DRIVER}.map ${NEXUS_LDFLAGS} $(NEXUS_EXTRALDFLAGS) --strip-debug -r $^  -o $@
endif

nexus_clean: ${OTHER_CLEANS}
	-$(Q_)$(RM) -r $(NEXUS_DRIVER) ${NEXUS_STATIC_LIB} ${MAGNUM_LIB} $(NEXUS_OBJ_DIR) $(NEXUS_SYNCTHUNK_DIR) $(NEXUS_TOP)/build/os/$(B_REFSW_OS)/module_rules.inc $(NEXUS_TOP)/build/module_vars.inc
ifdef NEXUS_STATIC_LIB
	-$(Q_)$(RM) ${NEXUS_BIN_DIR}/$(notdir ${NEXUS_STATIC_LIB}) ${NEXUS_BIN_DIR}/$(notdir ${MAGNUM_LIB})
endif

# Include dependencies
NEXUS_DEP_FILES := $(wildcard $(NEXUS_OBJ_DIR)/*/*.d)
ifneq ($(NEXUS_DEP_FILES),)
-include $(wildcard $(NEXUS_OBJ_DIR)/*/*.d)
endif

