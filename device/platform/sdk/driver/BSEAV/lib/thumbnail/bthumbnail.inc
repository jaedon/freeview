############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bthumbnail.inc $
# $brcm_Revision: 3 $
# $brcm_Date: 9/14/09 3:10p $
#
# Module Description:
#   Media framework module descriptor
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/thumbnail/bthumbnail.inc $
# 
# 3   9/14/09 3:10p mward
# SW7400-2517: Maintain Native Settop API (BUILD_SYSTEM=magnum) build for
# 97400.
# 
# 2   8/14/09 3:12p erickson
# PR55994: refactor so that bthumbnail_extractor can be used standlone
# with nexus
#
# 1   3/17/08 9:22a erickson
# PR40307: add thumbnail library
#
############################################################

BTHUMBNAIL_DEPENDS := BFILE BTHUMBNAIL

BTHUMBNAIL_INCLUDES := $(BSEAV)/lib/thumbnail
BTHUMBNAIL_DEFINES :=

BTHUMBNAIL_SOURCES := \
	${BSEAV}/lib/thumbnail/bthumbnail.c \
	${BSEAV}/lib/thumbnail/bthumbnail_manager.c \
	${BSEAV}/lib/thumbnail/bthumbnail_stream.c
ifeq ($(BUILD_SYSTEM),nexus)
BTHUMBNAIL_SOURCES += \
	${BSEAV}/lib/thumbnail/bthumbnail_extractor.c
endif
