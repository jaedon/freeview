############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/14 $
# $brcm_Date: 6/22/12 2:37p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/general/Makefile $
# 
# Hydra_Software_Devel/14   6/22/12 2:37p cng
# SW7425-2982: Add Unit Test: NEXUS_Platform_Init/Uninit
# 
# Hydra_Software_Devel/13   3/7/12 12:50p erickson
# SW7346-376: add temp_monitor unittest
# 
# Hydra_Software_Devel/12   6/15/11 9:51a erickson
# SWDEPRECATED-3044: add test
# 
# Hydra_Software_Devel/11   6/3/11 2:17p erickson
# SW7405-5349: update memory test, add optional profiling
# 
# Hydra_Software_Devel/10   5/12/09 4:35p erickson
# PR52109: add external_interrupts
# 
# Hydra_Software_Devel/9   4/20/09 10:42a jtna
# PR54011: added watchdog
# 
# Hydra_Software_Devel/8   3/9/09 2:39p erickson
# PR50748: added test
# 
# Hydra_Software_Devel/7   2/11/09 10:08a erickson
# PR52007: added convert test
# 
# Hydra_Software_Devel/6   1/12/09 3:45p erickson
# PR48035: add memory test
# 
# Hydra_Software_Devel/5   10/31/08 2:56p erickson
# PR48594: added test
# 
# Hydra_Software_Devel/4   10/24/08 3:39p erickson
# PR42679: add regread
# 
# Hydra_Software_Devel/3   10/23/08 3:59p erickson
# PR42679: update
# 
# Hydra_Software_Devel/2   10/17/08 12:22p erickson
# PR47030: add test
# 
# Hydra_Software_Devel/1   7/9/08 12:05p erickson
# PR44643: add settimeofday test
# 
# Hydra_Software_Devel/9   5/14/08 2:30p erickson
# PR39257: add bypass_display test
# 
# Hydra_Software_Devel/8   5/14/08 10:07a erickson
# PR42679: eliminate duplicate code in unittest Makefiles
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
	alloc \
	analog_digital_channel_change \
	analog_hdmi_input_change \
	boot \
	convert \
	external_interrupts \
	memory \
	memory_heaps \
	regread \
	settimeofday \
	sleep \
	temp_monitor \
	watchdog \
	platform_init

ifeq ($(BPROFILE_SUPPORT),y)
CFLAGS += -DBPROFILE_SUPPORT
endif

include ../common/Makefile.inc

# Explicit rule for exceptions
write_vbi: write_vbi.c ttdata0.c ttdata1.c $(VXOBJS)
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS)
