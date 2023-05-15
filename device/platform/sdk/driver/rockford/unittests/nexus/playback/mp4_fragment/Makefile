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
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 5/19/11 6:07p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/playback/mp4_fragment/Makefile $
# 
# Hydra_Software_Devel/2   5/19/11 6:07p vsilyaev
# SW7425-615: Added helper application to debug transitions betwen
# different source resolutions
# 
# Hydra_Software_Devel/1   5/7/10 1:25p vsilyaev
# SW7405-3773: Unittest for playback of fragmented of fragmented MP4 file
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
	play_mp4fragment \
	play_pes

CFLAGS += -I${BSEAV}/lib/media/build

include ../../common/Makefile.inc
