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
# $brcm_Revision: Hydra_Software_Devel/19 $
# $brcm_Date: 4/24/12 11:15a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/graphics2d/Makefile $
# 
# Hydra_Software_Devel/19   4/24/12 11:15a ahulse
# SW7425-2679: add picture_decoder_bad_format unittest
# 
# Hydra_Software_Devel/18   4/16/12 6:41p ahulse
# SW7425-2678: Add picture_decoder unittest
# 
# Hydra_Software_Devel/17   12/13/11 2:56p erickson
# SW7231-540: add test
# 
# Hydra_Software_Devel/16   11/28/11 5:16p vsilyaev
# SW7425-1207: Unit test for update (partial) scale blit
# 
# Hydra_Software_Devel/15   10/27/11 1:52p erickson
# SW7231-415: add unittest that would have caught this bug
# 
# Hydra_Software_Devel/14   5/20/11 2:25p erickson
# SW7346-149: test multiplexing of packet and function-based blit with a
# single context
# 
# Hydra_Software_Devel/13   3/10/11 4:06p erickson
# SW7405-5183: create packet_fill unittest
#
# Hydra_Software_Devel/12   10/21/10 2:23p vsilyaev
# SW7405-3671:  Added Nexus unittest to benchmark graphics 2D packet API
#
# Hydra_Software_Devel/11   8/16/10 1:03p erickson
# SW7405-3671: packet blit tests
#
# Hydra_Software_Devel/10   7/6/10 12:41p erickson
# SW7405-4508: create multi-threaded mutliple blitter checkpoint stress
# test
#
# Hydra_Software_Devel/9   3/19/09 10:35a erickson
# PR42679: add more apps
#
# Hydra_Software_Devel/8   1/12/09 12:30p erickson
# PR50979: rename NEXUS_Display_ConnectVideoInput api
#
# Hydra_Software_Devel/7   9/19/08 11:53a erickson
# PR47014: update
#
# Hydra_Software_Devel/6   9/18/08 11:53a erickson
# PR47014: update
#
# Hydra_Software_Devel/5   9/17/08 10:32a erickson
# PR47014: add blit test app
#
# Hydra_Software_Devel/4   9/12/08 1:00p erickson
# PR46844: added test
#
# Hydra_Software_Devel/3   9/2/08 2:50p erickson
# PR46291: added test app
#
# Hydra_Software_Devel/2   5/14/08 10:07a erickson
# PR42679: eliminate duplicate code in unittest Makefiles
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
	blit \
	blit_performance \
	blit_stress \
	checkpoint_stress \
	decode_mosaic_video_via_graphics \
	decode_video_via_graphics \
	dualcontext_multithreaded_fill \
	fillrate \
	fillrate_packet \
	memset32 \
	multithreaded_fill \
	packet_and_function \
	packet_blit_stress \
	packet_fill \
	palette_blit2 \
	palette_blit3 \
	porterduff \
	premultiply_alpha1 \
	premultiply_alpha2 \
	update_scale_blit \
	still_decoder \
    picture_decoder \
    picture_decoder_bad_format

include ../common/Makefile.inc
