############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 9/29/11 1:32p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/hd-dvi/Makefile $
# 
# Hydra_Software_Devel/1   9/29/11 1:32p katrep
# SW7231-387:add hd-dvi unit tests with external nsxp chip
# 
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

# move this to another location
NXP_TOP := $(shell pwd)/nxp

APPS =   hddvi_input_nxp  

EXTRA_SRC = $(NXP_TOP)/tmdlHdmiRx.c $(NXP_TOP)/tmdlHdmiRx_cfg.c $(NXP_TOP)/tmdlHdmiRx_IW_RTX.c $(NXP_TOP)/tmbslTDA1997X_functions.c

CFLAGS += -I$(NXP_TOP)  
CFLAGS += -DTMFL_NO_RTOS  
NEXUS_C_STD = c99

include ../common/Makefile.inc
