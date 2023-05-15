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
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 12/9/11 5:01p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/surface_compositor/Makefile $
# 
# Hydra_Software_Devel/3   12/9/11 5:01p erickson
# SW7420-1992: add unittests
# 
# Hydra_Software_Devel/2   11/18/11 2:57p erickson
# SW7420-1148: add test
# 
# Hydra_Software_Devel/1   9/7/11 7:15p vsilyaev
# SW7420-1148: Test cases for the surface compositor
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
	client_flavors \
	multibuffered_stress \
	setsurface_stress

include ../common/Makefile.inc
