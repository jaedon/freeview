############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bflv.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 7/11/08 4:01p $
#
# Module Description:
#   FLV media library module descriptor
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/media/flv/bflv.inc $
# 
# 2   7/11/08 4:01p vsilyaev
# PR 31188: Added support for FLV probe
# 
# 1   7/20/07 12:43p vsilyaev
# PR 32813: Module description file
# 
# 
############################################################

BFLV_INCLUDES := $(BSEAV)/lib/media/flv
BFLV_DEFINES := B_HAS_FLV=1

BFLV_FILTER_SOURCES := \
	$(BSEAV)/lib/media/flv/bflv_parser.c 

BFLV_PROBE_SOURCES := \
	$(BSEAV)/lib/media/flv/bflv_probe.c

