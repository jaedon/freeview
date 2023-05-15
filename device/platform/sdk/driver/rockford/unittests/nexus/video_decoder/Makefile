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
# $brcm_Revision: Hydra_Software_Devel/15 $
# $brcm_Date: 5/17/10 11:16a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/video_decoder/Makefile $
# 
# Hydra_Software_Devel/15   5/17/10 11:16a jtna
# SW7405-4289: add decode_ipstream to APPS list if IP support is enabled
# 
# Hydra_Software_Devel/14   5/3/10 6:02p jtna
# SW7405-4289: added support for building IP playback example app
# 
# Hydra_Software_Devel/13   5/5/09 4:29p erickson
# PR54844: add test for opening and starting mosaic channels in any order
#
# Hydra_Software_Devel/12   2/10/09 10:17a erickson
# PR42679: added userdata_capture test
#
# Hydra_Software_Devel/11   1/21/09 4:27p erickson
# PR51314: added still_decoder_stress
#
# Hydra_Software_Devel/10   1/16/09 1:39p erickson
# PR50905: added NEXUS_VideoDecoder_GetMostRecentPts test app
#
# Hydra_Software_Devel/9   10/28/08 4:50p erickson
# PR48406: added decoder_trick test
#
# Hydra_Software_Devel/8   10/23/08 3:58p erickson
# PR42679: update
#
# Hydra_Software_Devel/7   10/17/08 9:15a erickson
# PR47030: added test
#
# Hydra_Software_Devel/6   5/20/08 3:41p erickson
# PR42128: added video_decoder_dataready
#
# Hydra_Software_Devel/5   5/14/08 3:11p erickson
# PR42679: added codec_test
#
# Hydra_Software_Devel/4   5/14/08 12:29p erickson
# PR42679: make test cruisecontrol friendly
#
# Hydra_Software_Devel/3   5/14/08 10:07a erickson
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
	callbacks \
	codec_test \
	channel_change \
	dataready \
	decoder_trick \
	mosaic_decode \
	most_recent_pts \
	still_decoder_stress \
	open \
	userdata_capture

ifeq ($(PLAYBACK_IP_SUPPORT),y)
APPS += decode_ipstream
endif

ifneq ($(findstring decode_ipstream,$(APPS)),)
ifneq ($(PLAYBACK_IP_SUPPORT),y)
$(error decode_ipstream requires PLAYBACK_IP_SUPPORT=y)
endif
endif

include ../common/Makefile.inc

