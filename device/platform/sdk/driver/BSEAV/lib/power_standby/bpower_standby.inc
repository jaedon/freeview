############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bpower_standby.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 6/17/09 11:06a $
#
# Module Description:
#   Power standby
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/power_standby/bpower_standby.inc $
# 
# 2   6/17/09 11:06a maivu
# PR 56083: Add compile option to enable pause before going into standby
# 
# 1   10/29/08 4:00p maivu
# PR 48036: power standby library, based off power_standby app in
# rockford. Initial support for 3548/3556. Tested and confirmed working
# on 93549B0
# 
# 
############################################################

ifeq ($(STANDBY_PAUSE),y)
NEXUS_CFLAGS += -DENABLE_STANDBY_PAUSE=1
endif

BPOWER_STANDBY_INCLUDES += $(BSEAV)/lib/power_standby \
			   $(MAGNUM)/portinginterface/pwr \
			   $(MAGNUM)/portinginterface/pwr/$(BCHP_CHIP) \
			   $(BSEAV)/linux/driver/power \
			   $(MAGNUM)/basemodules/err \

BPOWER_STANDBY_SOURCES := \
	${BSEAV}/lib/power_standby/bpower_standby.c \
	${BSEAV}/lib/power_standby/bpower_priv.c \
	${BSEAV}/lib/power_standby/pmlib.c \
	$(MAGNUM)/portinginterface/pwr/$(BCHP_CHIP)/bpwr.c \
	$(MAGNUM)/portinginterface/pwr/$(BCHP_CHIP)/bpwr_priv.c \
	$(MAGNUM)/portinginterface/pwr/$(BCHP_CHIP)/bpwr_mem.c


