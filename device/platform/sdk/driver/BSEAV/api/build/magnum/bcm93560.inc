############################################################
#     Copyright (c) 2003-2005, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm93560.inc $
# $brcm_Revision: 12 $
# $brcm_Date: 9/21/05 5:10p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm93560.inc $
# 
# 12   9/21/05 5:10p bandrews
# PR17068: Fixed DRX support definitions
# 
# 11   9/13/05 7:45p bandrews
# PR17068: Added CFLAG for SPDIF RX support
# 
# 10   9/9/05 8:34p bandrews
# PR17068: Added DRX for 97398, plus two defines so rockford aud.inc has
# no dependency on chip
# 
# 9   9/7/05 5:00p vsilyaev
# PR 15377: Merged 7401 changes
# 
# Refsw_97401_Bringup/3   9/7/05 3:32p vsilyaev
# Prepare code to merge into the main line
# 
# Refsw_97401_Bringup/2   8/25/05 5:25p vsilyaev
# resolved undefined symbols
# 
# 8   7/18/05 6:12p vsilyaev# PR 15377: Refactoring of the board support package# # Refsw_97401_Bringup/1   7/12/05 1:45a vsilyaev
# Added support for 97401
# 
# 7   6/28/05 1:22p vsilyaev
# PR 15967: Connect PTS interupt to the PCRlib.
# 
# 6   4/27/05 11:53a rgreen
# PR15055: Add support for HDMI Rx to 3560
# Move bsettop_base_hdmi_rx to board specific
# 
# 5   4/13/05 8:45p dlwin
# PR 14655: Added ENABLE_AGC_SWITCH build option.
# 
# 4   3/31/05 5:15p dlwin
# PR 14655: Added changes to support demo builds.
# 
# 3   3/24/05 8:35p vsilyaev
# PR14553: Moved avswitch to appropriate place
# 
# 2   3/24/05 2:45p marcusk
# PR14553: Added support for bswch.
# 
# 1   3/24/05 12:53p vsilyaev
# PR 14593: 93560 support
# 
# Refsw_93560_Bringup/5   3/23/05 5:34p vsilyaev
# Added 3560 in-chip VSB support
# 
# Refsw_93560_Bringup/4   3/21/05 4:37p vsilyaev
# Compile smartcard only for platforms where it present.
# 
# Refsw_93560_Bringup/3   3/15/05 8:58p vsilyaev
# Moving code between generic BSP modules and board specific modules.
# 
############################################################
#
# This file is for the board related code which is _not_ part of the BSP layer
############################################################
SRCS += \
	bsettop_base_hdmi_rx.c

R_OBJ += \
 ${ROCKFORD}/bsp/bcm93560/common/bcm3560ap_a0 \

include  $(ROCKFORD)/modules/vsb.inc

include  ${ROCKFORD}/modules/pcrlib_7411.inc

B_HAS_LEGACY_AV=y
B_HAS_PCM=y
BCFG_AUD_SPDIF_RX_SUPPORT?=y

# This is a special mode used for demo only, it bypasses the AV switch and
# uses blue component as a composite input.
ifeq ($(ENABLE_BYPASS_AV_SWITCH),y)
CFLAGS += -DBYPASS_AV_SWITCH_FOR_CVBS=1
endif

# This is a special mode to support sharpness setting which is not standard
# VDC API.  The API for sharpness exist on a special branch.
ifeq ($(ENABLE_SHARPNESS_SETTING),y)
CFLAGS += -DENABLE_SHARPNESS_SETTING=1
endif

# This is a special mode to support HW request to use UART 2 IO pins for
# controlling AGC on tuner.
ifeq ($(ENABLE_AGC_SWITCH),y)
CFLAGS += -DENABLE_AGC_SWITCH=1
endif

