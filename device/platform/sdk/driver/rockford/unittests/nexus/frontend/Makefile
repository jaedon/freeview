############################################################
#     Copyright (c) 2003-2009, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 5/15/09 12:00p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/frontend/Makefile $
# 
# Hydra_Software_Devel/3   5/15/09 12:00p erickson
# PR54843: rework test to be more generic
# 
# Hydra_Software_Devel/2   11/11/08 5:09p jtna
# PR42679: added unittest frontend.c
# 
# Hydra_Software_Devel/1   10/30/08 10:41p erickson
# PR48572: added test
# 
# Hydra_Software_Devel/1   6/18/08 3:18p vsilyaev
# PR 42392: DMA unit test
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

APPS = tune_untune \
        frontend

include ../common/Makefile.inc


