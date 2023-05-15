############################################################
#     Copyright (c) 2003-2008, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 10/30/08 11:43a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/bluetooth/Makefile $
# 
# Hydra_Software_Devel/3   10/30/08 11:43a randyjew
# PR42789:  Updated for directv xm radio to bt headset demo and updated
# Brcm BT stack.
# 
# Hydra_Software_Devel/2   8/27/08 1:52p erickson
# PR42789: update
# 
# Hydra_Software_Devel/1   8/27/08 7:21a erickson
# PR42789: add bluetooth unittest
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = nexus_asrc nexus_asnk nexus_sat_asrc

include ../common/Makefile.inc
LDFLAGS += -lrt
#
# compile in BTE and bluez libraries
#
BSEAV=$(NEXUS_TOP)/../BSEAV
include Makefile.bluetooth
CFLAGS += $(BLUETOOTH_CFLAGS)
EXTRA_OBJS += $(BLUETOOTH_LIBS)

