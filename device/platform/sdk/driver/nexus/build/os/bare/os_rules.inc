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
# $brcm_Revision: 5 $
# $brcm_Date: 8/22/12 3:18p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/bare/os_rules.inc $
# 
# 5   8/22/12 3:18p erickson
# SW7420-1819: fix nfe
# 
# 4   6/6/11 4:17p ttrammel
# SW7420-1819: Add clean rule for nexus_firmware_elf.
# 
# 3   6/6/11 3:32p ttrammel
# SW7420-1819: Remove Linux references and changed file paths.
# 
# 2   6/6/11 10:50a ttrammel
# SW7420-1819: Update NEXUS/pi for NFE 2.0.
# 
# 1   5/3/11 5:10p erickson
# SW7420-1819: repartition so NFE is a standard feature
# 
############################################################

NEXUS_FIRMWARE_ELF_FILE=$(subst nexus.ko,nexus_firmware.elf,$(NEXUS_DRIVER))

nexus_install: nexus_all
	@echo [Install... driver]
	$(Q_)$(MKDIR) ${NEXUS_BIN_DIR}
ifdef NEXUS_STATIC_LIB
	@$(CP) $(NEXUS_STATIC_LIB) ${NEXUS_BIN_DIR}
	@$(CP) $(MAGNUM_LIB) ${NEXUS_BIN_DIR}
endif
ifeq ($(B_REFSW_FIRMWARE),y)
	@$(CP) $(NEXUS_FIRMWARE_ELF_FILE) ${NEXUS_BIN_DIR}
endif
	@$(CP) $(NEXUS_TOP)/build/nexus.linuxkernel ${NEXUS_BIN_DIR}/nexus

nexus_all: $(MAGNUM_LIB) ${NEXUS_STATIC_LIB} ${NEXUS_FIRMWARE_ELF_FILE}

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

ifeq ($(B_REFSW_FIRMWARE),y)
$(NEXUS_OBJ_DIR)/stubs_rev.o: $(NEXUS_TOP)/platforms/common/src/bare/stubs_rev.S $(NEXUS_OBJ_DIR)
	@echo [Compiling...... stubs_rev.S]
	${Q_}if ! [ -e $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/bin ] ; then ${Q_}${MKDIR} -p $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/bin; fi
	${Q_}$(CC) $(NEXUS_CFLAGS)  -o $@ -c $<

$(NEXUS_OBJ_DIR)/memset.o: $(NEXUS_TOP)/platforms/common/src/bare/libgcc/memset.S $(NEXUS_OBJ_DIR)
	${Q_}if ! [ -e $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/bin ] ; then ${Q_}${MKDIR} -p $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/bin; fi
	${Q_}$(CC) $(NEXUS_CFLAGS) -I$(NEXUS_TOP)/platforms/common/src/bare/ -o $@ -c $<

$(NEXUS_OBJ_DIR)/memcpy_brcm.o: $(NEXUS_TOP)/platforms/common/src/bare/libgcc/memcpy_brcm.S $(NEXUS_OBJ_DIR)
	${Q_}if ! [ -e $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/bin ] ; then ${Q_}${MKDIR} -p $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/bin; fi
	${Q_}$(CC) $(NEXUS_CFLAGS) -I$(NEXUS_TOP)/platforms/common/src/bare/ -o $@ -c $<

BARE_ASM_OBJECTS = $(NEXUS_OBJ_DIR)/stubs_rev.o $(NEXUS_OBJ_DIR)/memset.o $(NEXUS_OBJ_DIR)/memcpy_brcm.o

$(NEXUS_FIRMWARE_ELF_FILE): $(NEXUS_OBJECTS) $(MAGNUM_OBJECTS) $(MAGNUM_PRECOMPILED_OBJECTS) $(BARE_ASM_OBJECTS)
	@echo [Linking...... nexus_firmware.elf]
ifeq ($(B_REFSW_DEBUG),n)
	${Q_}$(LD) ${NEXUS_LDFLAGS} $(NEXUS_EXTRALDFLAGS) \
		--strip-debug -r \
		$(NEXUS_OBJECTS) $(MAGNUM_OBJECTS) $(MAGNUM_PRECOMPILED_OBJECTS) $(BARE_ASM_OBJECTS) \
		-o $@
	${Q_}$(STRIP) --strip-unneeded \
		--keep-symbol=B_Forward_Jumptable \
		--keep-symbol=pB_Reverse_Jumptable \
		$@
	#${Q_}$(NEXUS_TOP)/build/tools/jumptable/clearstrtab.sh $@ $(READELF)
else
	${Q_}$(LD) ${NEXUS_LDFLAGS} $(NEXUS_EXTRALDFLAGS) \
		-r \
		$(NEXUS_OBJECTS) $(MAGNUM_OBJECTS) $(MAGNUM_PRECOMPILED_OBJECTS) $(BARE_ASM_OBJECTS) \
		-o $@
endif
 
else
$(NEXUS_FIRMWARE_ELF_FILE): 

endif

nexus_clean: ${OTHER_CLEANS}
	-$(Q_)$(RM) -r ${NEXUS_STATIC_LIB} ${MAGNUM_LIB} $(NEXUS_OBJ_DIR) $(NEXUS_SYNCTHUNK_DIR) $(NEXUS_TOP)/build/os/$(B_REFSW_OS)/module_rules.inc $(NEXUS_TOP)/build/module_vars.inc
ifdef NEXUS_STATIC_LIB
	-$(Q_)$(RM) ${NEXUS_BIN_DIR}/$(notdir ${NEXUS_STATIC_LIB}) ${NEXUS_BIN_DIR}/$(notdir ${MAGNUM_LIB})
endif
ifeq ($(B_REFSW_FIRMWARE),y)
	-$(Q_)$(RM) ${NEXUS_BIN_DIR}/$(notdir $(NEXUS_FIRMWARE_ELF_FILE))
endif

# Include dependencies
NEXUS_DEP_FILES := $(wildcard $(NEXUS_OBJ_DIR)/*/*.d)
ifneq ($(NEXUS_DEP_FILES),)
-include $(wildcard $(NEXUS_OBJ_DIR)/*/*.d)
endif

