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
# $brcm_Date: 2/15/12 1:32p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/playback/otfpvr/Makefile $
# 
# Hydra_Software_Devel/3   2/15/12 1:32p erickson
# SW7425-2358: fix build
# 
# Hydra_Software_Devel/2   5/6/10 4:20p vsilyaev
# SWDEPRECATED-3571: Moved away from the top-level tests
# 
# Hydra_Software_Devel/1   3/31/09 7:28p vsilyaev
# PR 50887: OTFPVR support for the Nexus video decoder
# 
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../../nexus; pwd)
endif
BSEAV = ${NEXUS_TOP}/../BSEAV

APPS = \
	playpump_otf 

EXTRA_SRC += ${BOTFPVR_APP_SOURCES}

include ${BSEAV}/lib/otfpvr/botfpvr.inc

CFLAGS += -I${BOTFPVR_APP_INCLUDES}

include ../../common/Makefile.inc
