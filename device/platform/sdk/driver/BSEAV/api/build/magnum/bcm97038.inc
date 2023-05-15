############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97038.inc $
# $brcm_Revision: 5 $
# $brcm_Date: 6/7/07 6:49p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97038.inc $
# 
# 5   6/7/07 6:49p vle
# PR 31924: Define HDMI Tx Support for platform
# 
# 4   11/28/06 6:40p bandrews
# PR25214: A1 and B0 3520 images were split into two files
# 
# 3   9/7/05 5:06p vsilyaev
# PR 15377: Merged 7401 changes
# 
# Refsw_97401_Bringup/1   8/25/05 5:25p vsilyaev
# resolved undefined symbols
# 
# 2   3/24/05 12:53p vsilyaev
# PR 14593: 93560 support
# 
# Refsw_93560_Bringup/10   3/24/05 10:48a vsilyaev
# Fixed 4500 support.
# 
# Refsw_93560_Bringup/9   3/24/05 10:34a vsilyaev
# Fixed code for 3510/3520 VSB.
# 
# Refsw_93560_Bringup/8   3/23/05 11:05p vsilyaev
# Back fixes for 97038.
# 
# Refsw_93560_Bringup/7   3/23/05 10:19p vsilyaev
# More fixed for 3560  and back fixed for 97398.
# 
# Refsw_93560_Bringup/6   3/23/05 5:34p vsilyaev
# Fixed syntax.
# 
# Refsw_93560_Bringup/5   3/21/05 4:37p vsilyaev
# Compile smartcard only for platforms where it present.
# 
# Refsw_93560_Bringup/4   3/18/05 9:37p vsilyaev
# Moved board specific parts into the board specific files.
# 
# Refsw_93560_Bringup/3   3/16/05 12:51p vsilyaev
# Isolated BSP code.
# 
# Refsw_93560_Bringup/2   3/15/05 8:59p vsilyaev
# Moving code between generic BSP modules and board specific modules.
# 
# Refsw_93560_Bringup/1   3/10/05 6:53p vsilyaev
# Refactored board specific and BSP files
# 
# 1   2/7/05 6:53p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/5   1/18/05 9:51a dlwin
# PR 12944: Added support to compile qam_cm.c.
# 
# Irvine_BSEAVSW_Devel/4   5/12/04 10:23a erickson
# PR10967: added standard header
############################################################

R_INC += ${ROCKFORD}/commondrivers/qam/bcm97038/
R_OBJ += \
 ${ROCKFORD}/commondrivers/qam/bcm97038/ib_qam_scripts \
 ${ROCKFORD}/commondrivers/qam/bcm97038/ob_qam_scripts \
 ${ROCKFORD}/commondrivers/qam/bcm97038/pa_ara2001 \
 ${ROCKFORD}/commondrivers/qam/bcm97038/qam_cm

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
R_OBJ += ${ROCKFORD}/bsp/bcm97038/common/bcm3520ap_b0
SRCS += bsettop_tuner_3520.c 
endif

B_HAS_LEGACY_AV=y
B_HAS_PCM=y
B_HAS_SMARTCARD=y
HDMI_TX_SUPPORT=y

R_OBJ += ${ROCKFORD}/bsp/bcm97038/common/bcm4500ap  \
		 ${ROCKFORD}/bsp/bcm97038/common/bcmmux

include  ${ROCKFORD}/modules/4500.inc
include  ${ROCKFORD}/modules/4500/sds.inc

SRCS += \
	bsettop_tuner_4500.c \
	bsettop_tuner_3250.c 


