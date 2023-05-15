############################################################################
#     Copyright (c) 2002,2003,2004 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: usermode.mak $
# $brcm_Revision: 54 $
# $brcm_Date: 5/15/12 3:35p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/linux/driver/build/usermode/usermode.mak $
# 
# 54   5/15/12 3:35p bselva
# SW7360-19: Add support for 7360 platform
# 
# 53   4/17/12 7:05p chengs
# SW7584-3: Add 7584 support.
# 
# 52   9/22/11 6:09p katrep
# SW7429-1:add support 7428 & bond outs
# 
# 51   4/6/11 4:27p jhaberf
# SW35330-13: Added support for 35233 DTV chip
# 
# 50   2/14/11 2:36p randyjew
# SW7418-1:Add 97418 Support
# 
# 49   12/22/10 4:35p katrep
# SW7231-25:add initial support for 7230
# 
# 48   12/8/10 9:24a jrubio
# SW7344-9: add 7346/7344 support
# 
# 47   12/6/10 4:56p katrep
# SW7231-4:initial support
# 
# 46   12/1/10 3:48p jhaberf
# SW35125-1: Added 35125 DTV chip support
# 
# 45   11/22/10 5:33p xhuang
# SW7358-20: Add 97358 support
# 
# 44   10/1/10 5:29p spothana
# SW7420-1078: Add support for BCM97409 platform.
# 
# 43   8/30/10 2:01p hongtaoz
# SW7425-8: add 7425 support;
# 
# 42   6/8/10 4:29p nickh
# SW7422-7: Add 7422 support
# 
# 41   1/20/10 2:14p garetht
# SW7420-553: Add platform for 93380vms
# 
# 40   1/19/10 3:19p nickh
# SW7420-541: Rename 97420dvr2 to 97420
# 
# 39   12/4/09 9:33p randyjew
# SW7468-6: Add 7208 support
# 
# 38   11/4/09 5:25p gmohile
# SW7408-1 : Add 97408 platform
# 
# 37   10/30/09 5:40p nickh
# SW7420-331: Rename 97410c to 97410
# 
# 36   10/27/09 3:19p nitinb
# SW7550-46: Add 7550 support
# 
# 35   10/15/09 1:41p jhaberf
# SW35230-1: Added 35230 support.
# 
# 34   10/14/09 3:10p garetht
# SW7420-386: Rebase 93380sms codebase to 97420 Phase 3.0
# 
# 33   10/14/09 11:37a haisongw
# SW7405-3091: Add 97205CB platform support in trinity.
#
# 32   9/28/09 3:57p lwhite
# SW7468-6: Support for 7468
#
# 31   9/15/09 6:33p nickh
# SW7420-331: Add 7410 support
#
# 30   8/20/09 3:57p mward
# PR55545:  Add 7125 family platforms.
#
# 29   8/10/09 11:59a jrubio
# PR55232: add 7340/7342
#
# 28   7/7/09 2:37p gqli
# PR56512: Add 7403 nexus support
#
# 27   6/29/09 9:33a gmohile
# PR 56400 : Add 7401 nexus support
#
# 26   6/22/09 3:11p mward
# PR 56215: Default BUILD_SYSTEM=nexus if top-level nexus directory is
# found.
#
# 25   6/8/09 5:55p prasadv
# PR50240: Settop API based release was using bcm_driver.c instead of
# umdrv.c in the user mode. Made this minor change for 97456.
#
# 24   5/6/09 10:01a jhaberf
# PR53796: Added BCM35130 to the list of chips that build with
# bcm_driver.c instead of umdrv.mod.c.
#
# 23   4/23/09 6:20p jhaberf
# PR53796: Added support for BCM35130 DTV chip
#
# 22   3/2/09 10:44a katrep
# PR52599: Add support 7305
#
# 21   2/4/09 10:15a jrubio
# PR51629: add 7336 support
#
# 20   1/15/09 3:13p prasadv
# PR50240: Adding 97456 Support
#
# 19   12/19/08 7:39p katrep
# PR50567: Add Support for 97466 platform
#
# 18   12/13/08 11:09a nickh
# PR47901: Add 7420 support for new user mode driver
#
# 17   10/9/08 10:29a mward
# PR47739: Build 97400 Nexus using new driver.
#
# 16   9/29/08 3:37p rpan
# PR47411: Continue to use the new driver even if app framework is
# broken.
#
# 15   9/29/08 3:00p rpan
# PR47411: Roll back the change which tried to add 3548/3556 specific
# interrupt table.
#
# 14   10/16/08 2:41p maivu
# PR 47192: Added support for 93549/93556
#
# 13   7/8/08 3:14p dlwin
# PR 44546: Added bcmdriver support for 7601
#
# 12   6/23/08 3:03p anilmm
# PR43453: Add support for 97459
#
# 11   11/30/07 11:43a katrep
# PR37430: rename bcmdriver.c bcmdriver.h to bcm_driver.c bcm_driver.h
# due to name conflicts during kernel mode builds.
#
# 10   11/21/07 1:19p katrep
# PR37430: Added suppport for upto 128 bit interrupt
# controller,reoganized the driver code.
#
# 9   8/10/07 12:31a ssood
# PR33786: changes to accomodate Net DMA & IP Streamer refactoring into
# bcmnetaccel driver
#
# 8   7/6/07 11:24p ssood
# PR30010: Net DMA refactoring to support various UDP/TCP receive side
# modules
#
# 7   4/24/07 9:14a jrubio
# PR30010: Making btcp_oe.ko optional
#
# 6   4/19/07 7:16p jrubio
# PR30010: added paths to needed files
#
# 5   1/29/07 12:33p erickson
# PR25902: rework linux drivers to remove duplication
#
# 4   11/28/06 3:59p jgarrett
# PR 24898: Adding include/asm-mips/mach-brcmstb to include path in order
# to support CPU feature overrides required for SMP
#
# 3   11/21/06 8:31a jjordan
# PR26085: Fix NET DMA build following deprecated use of SetTop
#
# 2   11/16/06 5:12p jgarrett
# PR 25230: Converting to .inc file
#
# ##old brcm_Log: /SetTop/linux/driver/usermodedrv/Makefile##
#
# Irvine_BSEAVSW_Devel/25   7/6/06 5:06p qcheng
# PR22147: update ICAM2.2 source code files for BCM97400.
#
############################################################################

#
# Configure linux kernel source
# Output is LINUX_INC
#

# Guess BUILD_SYSTEM based on existence of nexus directory.
ifndef BUILD_SYSTEM
ifeq ($(wildcard $(BSEAV)/../nexus),)
# This is magnum.  Leave the variable unset.
else
# Nexus code exists.  Assume nexus.
BUILD_SYSTEM := nexus
endif
endif

ICM_CLI_SUPPORT ?= y
ifeq ($(ICM_CLI_SUPPORT),y)
CFLAGS += -DBICM_ICAM2
endif

#For now change to the new driver only for following platforms
#For 97400, only for Nexus
#$(warning $(findstring $(PLATFORM), 97405 97459 97325 97335 97601 ))
ifeq ($(BUILD_SYSTEM),nexus)
ifeq ($(findstring $(PLATFORM), 97401 97403 97455 97400 97456 97405 97205 97466 97459 97325 97335 97340 97342 97601 93549 93556 935233 935230 935125 97420 97410 97409 97125 97019 97119 97116 97025 97205CB 97468 97208 93380sms 93380vms 97550 97408 97422 97425 97358 97552 97231 97230 97418 97344 97346 97429 97428 97241 97584 97360), $(PLATFORM))
OBJS = bcm_driver.o interrupt_table.o
CFLAGS += -I$(BSEAV)/linux/driver/usermode/
else
OBJS = umdrv.o
endif
else
ifeq ($(findstring $(PLATFORM), 97405 97459 97325 97335 97340 97342 97418 97344 97346 97601 93549 93556 935233 935230 935125 97420 97410 97409 97468 97208 93380sms 93380vms 97422 97425 97358 97552 97231 97230 97429 97428 97241 97584 97360), $(PLATFORM))
OBJS = bcm_driver.o interrupt_table.o
CFLAGS += -I$(BSEAV)/linux/driver/usermode/
else
OBJS = umdrv.o
endif
endif

ifeq (${B_HAS_PLAYPUMP_IP},y)
CFLAGS += -DB_NETIF_DMA=1 -I$(BSEAV)/api/include/
OBJS += netif_dma_stub.o
endif

CFLAGS += ${B_REFSW_CFLAGS} ${B_REFSW_LINUXKERNEL_CFLAGS}
CFLAGS += -I../../../../../magnum/commonutils/lst/

ifeq ($(LINUX_VER_GE_2_6),y)
OBJS += umdrv.mod.o
CFLAGS += -DKBUILD_MODNAME=bcmdriver
MOD_EXT = ko
else
MOD_EXT = o
endif

UMDRV = $(BCM_OBJ_DIR)/bcmdriver.${MOD_EXT}
DRIVERS += $(UMDRV)

# rules
clean-umdrv:
	${Q_}$(RM) -r $(BCM_OBJ_DIR)

$(BCM_OBJ_DIR)/%.o: $(BSEAV)/linux/driver/usermode/%.c
	@echo [Compile... $(notdir $<)]
	${Q_}$(CC) -MMD -c $(CFLAGS) $< -o $@

$(BCM_OBJ_DIR)/%.o: $(BSEAV)/linux/driver/usermode/$(PLATFORM)/%.c
	@echo [Compile... $(notdir $<)]
	${Q_}$(CC) -MMD -c $(CFLAGS) $< -o $@

$(UMDRV): $(addprefix ${BCM_OBJ_DIR}/, ${OBJS})
	@echo [Linking... $(notdir $@)]
	${Q_}$(LD) -r $^ -o $@ $(LDFLAGS)

-include $(BCM_OBJ_DIR)/*.d

