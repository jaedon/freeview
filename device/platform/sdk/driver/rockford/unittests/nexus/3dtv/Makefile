############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 2/22/10 2:20p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/3dtv/Makefile $
# 
# Hydra_Software_Devel/2   2/22/10 2:20p erickson
# SW7405-3625: make Makefile dtv/settop friendly
#
# Hydra_Software_Devel/1   1/19/10 2:33p erickson
# SW3548-2727: initial 3dtv phase1 app
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

NEXUS_PLATFORM ?= $(PLATFORM)
ifneq ($(filter $(NEXUS_PLATFORM),93549 93556 93563 935230),)
# DTV
APPS = 3dtv_phase1
else
# Settop
APPS = 3dsettop_pig
endif

include ../common/Makefile.inc


