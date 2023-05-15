############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bavi.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 3/12/10 6:13p $
#
# Module Description:
#   AVI media library module descriptor
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/avi/bavi.inc $
# 
# 2   3/12/10 6:13p vsilyaev
# SW3556-1077: Custom player to handle non interleaved files
# 
# 1   7/20/07 12:43p vsilyaev
# PR 32813: Module description file
# 
# 
############################################################

BAVI_INCLUDES := $(BSEAV)/lib/avi
BAVI_DEFINES := B_HAS_AVI=1

BAVI_SOURCES := \
	$(BSEAV)/lib/avi/bavi_util.c  \
	$(BSEAV)/lib/avi/bavi_parser.c 

BAVI_FILTER_SOURCES := \
	$(BSEAV)/lib/avi/bavi_stream.c 

BAVI_PLAYER_SOURCES := \
	$(BSEAV)/lib/avi/bavi_player.c \
	$(BSEAV)/lib/avi/bavi_player_custom.c

BAVI_PROBE_SOURCES := \
	$(BSEAV)/lib/avi/bavi_probe.c

