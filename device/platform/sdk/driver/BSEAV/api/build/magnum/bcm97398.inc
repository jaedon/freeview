############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97398.inc $
# $brcm_Revision: 9 $
# $brcm_Date: 6/7/07 6:49p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97398.inc $
# 
# 9   6/7/07 6:49p vle
# PR 31924: Define HDMI Tx Support for platform
# 
# 8   9/21/05 5:10p bandrews
# PR17068: Fixed DRX support definitions
# 
# 7   9/13/05 7:45p bandrews
# PR17068: Added CFLAG for DRX support
# 
# 6   9/9/05 8:34p bandrews
# PR17068: Added DRX for 97398, plus two defines so rockford aud.inc has
# no dependency on chip
# 
# 5   9/7/05 5:06p vsilyaev
# PR 15377: Merged 7401 changes
# 
# Refsw_97401_Bringup/2   9/7/05 3:31p vsilyaev
# Prepare code to merge into the main line
# 
# Refsw_97401_Bringup/1   8/25/05 5:25p vsilyaev
# resolved undefined symbols
# 
# 4   8/5/05 3:06p erickson
# PR16583: moved BCM7411C_BTP_FORMAT option so it will work with
# kernelmode as well
# 
# 3   6/28/05 1:22p vsilyaev
# PR 15967: Connect PTS interupt to the PCRlib.
# 
# 2   6/9/05 5:03p vsilyaev
# PR 15459: Use special format of BTP commands for 97398(7411 decoder).
# 
# 1   3/24/05 12:54p vsilyaev
# PR 14593: 93560 support
# 
# Refsw_93560_Bringup/5   3/23/05 10:19p vsilyaev
# More fixed for 3560  and back fixed for 97398.
# 
# Refsw_93560_Bringup/4   3/23/05 3:51p dlwin
# Fixed a makefile problem
# 
# Refsw_93560_Bringup/3   3/21/05 4:37p vsilyaev
# Compile smartcard only for platforms where it present.
# 
# Refsw_93560_Bringup/2   3/18/05 9:37p vsilyaev
# Moved board specific parts into the board specific files.
# 
# Refsw_93560_Bringup/1   3/15/05 8:59p vsilyaev
# Moving code between generic BSP modules and board specific modules.
# 
############################################################

#
# 3510/3520 VSB support has to be a compile-time option
# because the API's are similar. We default to
# 3520, but you can define the following in order
# to get 3510 support.
#
ifeq ($(VSB3510_SUPPORT),y)
include  ${ROCKFORD}/modules/3510.inc
include  ${ROCKFORD}/modules/3510/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97038/common/bcm3510ap 
CFLAGS += -DVSB_CHIP=3510
SRCS += bsettop_tuner_3510.c 
else
include  ${ROCKFORD}/modules/3520.inc
include  ${ROCKFORD}/modules/3520/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97038/common/bcm3520ap_a1
SRCS += bsettop_tuner_3520.c 
endif

R_OBJ += ${ROCKFORD}/bsp/bcm97038/common/bcm4500ap 
include  ${ROCKFORD}/modules/4500.inc
include  ${ROCKFORD}/modules/4500/sds.inc

include  ${ROCKFORD}/modules/pcrlib_7411.inc

B_HAS_LEGACY_AV=y
B_HAS_PCM=y
B_HAS_SMARTCARD=y
BCFG_AUD_DRX_SUPPORT?=y
HDMI_TX_SUPPORT=y

SRCS += bsettop_tuner_4500.c 


