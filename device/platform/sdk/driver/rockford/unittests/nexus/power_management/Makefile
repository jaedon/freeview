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
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 8/1/12 2:17p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/power_management/Makefile $
# 
# Hydra_Software_Devel/3   8/1/12 2:17p ahulse
# SW7425-3613: add power_management/pm_picture_decoder
# 
# Hydra_Software_Devel/2   3/29/12 6:23p jtna
# SW7425-2681: add power_management/standby_transition.c
# 
# Hydra_Software_Devel/1   1/12/11 12:12p jtna
# SW7420-1320: add power_management unittests
# 
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

# needed to link with pmlib
PMLIB_PATH=$(NEXUS_TOP)/../BSEAV/lib/power_standby
CFLAGS += -I$(PMLIB_PATH)
vpath %.c $(PMLIB_PATH)
EXTRA_SRC += pmlib-263x.c
#TODO: pmlib-263x.c only required for standby_transition.c

APPS = \
	standby_transition \
	pm_picture_decoder

include ../common/Makefile.inc
