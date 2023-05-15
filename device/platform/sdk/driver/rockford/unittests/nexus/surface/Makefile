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
# $brcm_Revision: Hydra_Software_Devel/6 $
# $brcm_Date: 4/6/09 2:20p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/surface/Makefile $
# 
# Hydra_Software_Devel/6   4/6/09 2:20p erickson
# PR42679: update
#
# Hydra_Software_Devel/5   1/30/09 4:26p erickson
# PR46300: removed NEXUS_Surface_CreateSpecial
#
# Hydra_Software_Devel/4   12/30/08 9:16a erickson
# PR50743: add palette test
#
# Hydra_Software_Devel/3   12/18/08 12:00a erickson
# PR50438: added test for surface create
#
# Hydra_Software_Devel/2   12/4/08 9:54a erickson
# PR46302: build compression app if extension set
#
# Hydra_Software_Devel/1   10/31/08 10:31a jtna
# PR42679: added new unittest for Surface module
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
	surface \
	open \
	palette

ifneq ($(NEXUS_SURFACE_EXTENSION_INC),)
APPS += compression_app
endif

include ../common/Makefile.inc
