############################################################
#     (c)2003-2010 Broadcom Corporation
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
# $brcm_Revision: 7 $
# $brcm_Date: 1/8/10 10:56a $
#
# Module Description:
#   This file defines the OS-specific rules for building in ucos.
#
# Revision History:
#
# $brcm_Log: /nexus/build/os/ucos/os_rules.inc $
# 
# 7   1/8/10 10:56a erickson
# SW7550-121: modify bash-specific syntax
#
# 6   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 5   3/13/09 2:44p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 4   9/19/08 2:52p cnovak
# PR43697: Don't touch bsp or kernel libraries if we're not building the
#  bsp.
#
# 3   9/19/08 12:52p cnovak
# PR43697: Disable building of the BSP.
#
# 2   8/7/08 5:50p cnovak
# PR43697: Add rules to build the bsp. This will likely be removed later,
#  but necessary for now.
#
# 1   6/17/08 3:18p cnovak
# PR43697: Add uCOS to Nexus.
#
############################################################

BUILD_BSP = n

nexus_install: nexus_all
	@echo [Install... driver]
	$(Q_)$(MKDIR) ${NEXUS_BIN_DIR}
	@$(CP) $(NEXUS_STATIC_LIB) ${NEXUS_BIN_DIR}
	@$(CP) $(MAGNUM_LIB) ${NEXUS_BIN_DIR}
ifeq ($(BUILD_BSP),y)
	@$(CP) $(BSP_LIB) ${NEXUS_BIN_DIR}
	@$(CP) $(KERNEL_LIB) ${NEXUS_BIN_DIR}
endif


ifeq ($(BUILD_BSP),y)
nexus_all: $(NEXUS_STATIC_LIB) $(MAGNUM_LIB) $(BSP_LIB) $(BSP_OBJECTS)
else
nexus_all: $(NEXUS_STATIC_LIB) $(MAGNUM_LIB)
endif

ifeq ($(BUILD_BSP),y)
#
# For uCOS, we need to build the bsp. Build everything in our defined
# directories
#
BSP_SOURCE_DIRS += $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/src/$(B_REFSW_OS)/sde
BSP_SOURCE_DIRS += $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/src/$(B_REFSW_OS)/bsp

BSP_C_SOURCES += $(foreach srcdir,$(BSP_SOURCE_DIRS),$(wildcard $(srcdir)/*.c))
BSP_A_SOURCES += $(foreach srcdir,$(BSP_SOURCE_DIRS),$(wildcard $(srcdir)/*.s))

BSP_OBJECTS += $(sort $(addprefix $(NEXUS_OBJ_DIR)/BSP/,$(patsubst %.c,%.$(NEXUS_OBJ_SUFFIX),$(notdir $(BSP_C_SOURCES)))))
BSP_OBJECTS += $(sort $(addprefix $(NEXUS_OBJ_DIR)/BSP/,$(patsubst %.s,%.$(NEXUS_OBJ_SUFFIX),$(notdir $(BSP_A_SOURCES)))))

BSP_CFLAGS += $(foreach srcdir,$(BSP_SOURCE_DIRS),-I$(srcdir))

BSP_AFLAGS += -D_ASMLANGUAGE -x assembler-with-cpp -DMIPS_SDE
# The code that uses these flags needs to be cleaned up and removed
BSP_AFLAGS += -DSTACK_ADRS=0x84000000 -DROM_TEXT_ADRS=0x80001000

vpath %.c $(BSP_SOURCE_DIRS)
vpath %.s $(BSP_SOURCE_DIRS)

# Rule for bsp source
$(NEXUS_OBJ_DIR)/BSP/%.o: %.c
	-$(Q_)$(MKDIR) $(NEXUS_OBJ_DIR)/BSP >/dev/null 2>&1
	@echo "[Compile... $(notdir $<) (bsp)]"
	$(Q_)$(CC) $(CDEP_FLAG) $(NEXUS_CFLAGS) $(BSP_CFLAGS) -c $< -o $@

$(NEXUS_OBJ_DIR)/BSP/%.o: %.s
	-$(Q_)$(MKDIR) $(NEXUS_OBJ_DIR)/BSP >/dev/null 2>&1
	@echo "[Assemble... $(notdir $<) (bsp)]"
	$(Q_)$(CC) $(CDEP_FLAG) $(NEXUS_CFLAGS) $(BSP_CFLAGS) $(BSP_AFLAGS) -c $< -o $@

# Rule for the bsp libray
$(BSP_LIB): $(BSP_OBJECTS)
	@echo "[Archive... $(notdir $@)]"
	-$(Q_)$(AR) rcs $@ $^

endif

# Rule for magnum sources
$(MAGNUM_OBJECTS): $(NEXUS_OBJ_DIR)/MAGNUM/%.$(NEXUS_OBJ_SUFFIX): %.c
	-$(Q_)$(MKDIR) $(NEXUS_OBJ_DIR)/MAGNUM >/dev/null 2>&1
	@echo "[Compile... $(notdir $<) (magnum)]"
	$(Q_)$(CC) $(CDEP_FLAG) $(NEXUS_CFLAGS) -c $< -o $@

# Rule for magnum library
$(MAGNUM_LIB): $(MAGNUM_OBJECTS) $(MAGNUM_PRECOMPILED_OBJECTS)
	@echo "[Archive... $(notdir $@)]"
	-$(Q_)$(AR) rcs $@ $^

# Rule for nexus library
$(NEXUS_STATIC_LIB): $(NEXUS_OBJECTS) $(MAGNUM_OBJECTS) $(MAGNUM_PRECOMPILED_OBJECTS)
	@echo "[Archive... $(notdir $@)]"
	-$(Q_)$(AR) rcs $@ $^

nexus_clean:
	-$(Q_)$(RM) -rf $(NEXUS_TOP)/libmagnum.a $(NEXUS_TOP)/libnexus.* $(NEXUS_OBJ_DIR)/* $(NEXUS_SYNCTHUNK_DIR)/*

# Include dependencies
NEXUS_DEP_FILES := $(wildcard $(NEXUS_OBJ_DIR)/*/*.d)
ifneq ($(NEXUS_DEP_FILES),)
-include $(wildcard $(NEXUS_OBJ_DIR)/*/*.d)
endif

