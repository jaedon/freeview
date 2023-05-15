############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: botfpvr.inc $
# $brcm_Revision: 5 $
# $brcm_Date: 8/4/09 12:36p $
#
# Module Description:
#   OnTheFly PVR library descriptor
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/otfpvr/botfpvr.inc $
# 
# 5   8/4/09 12:36p vsilyaev
# PR 56736: Use common files for all chips
# 
# 4   3/31/09 7:29p vsilyaev
# PR 50887: OTFPVR support for the Nexus video decoder
# 
# 3   2/11/09 3:36p vsilyaev
# PR 50887: Further reworked OTF pvr
# 
# 2   2/3/09 4:35p vsilyaev
# PR 50887: Updated build include file to match magnum/BSEAV/NEXUS rules
# 
############################################################

BOTFPVR_INCLUDES := $(BSEAV)/lib/otfpvr

BOTFPVR_SOURCES := \
    ${BSEAV}/lib/otfpvr/botf.c \
    ${BSEAV}/lib/otfpvr/botf_gop_manager.c \
    ${BSEAV}/lib/otfpvr/botf_gop_player.c \
    ${BSEAV}/lib/otfpvr/botf_itb_feeder.c \
    ${BSEAV}/lib/otfpvr/botf_mem.c \
    ${BSEAV}/lib/otfpvr/botf_scv_parser.c \
    ${BSEAV}/lib/otfpvr/botf_platform.c

BOTFPVR_APP_INCLUDES := $(BSEAV)/lib/otfpvr

BOTFPVR_APP_SOURCES := \
    ${BSEAV}/lib/otfpvr/botf_marker.c 

