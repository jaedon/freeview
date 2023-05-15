############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97435.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 6/7/07 6:48p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97435.inc $
# 
# 2   6/7/07 6:48p vle
# PR 31924: Define HDMI Tx Support for platform
# 
# 1   4/20/06 3:21p erickson
# PR19738: 97435 merge
# 
# PROD_97435C_REFSW_Devel/1   4/19/06 11:06a gqli
# PR19738: add BCM97435C support
#
# 10   3/15/06 4:51p vsilyaev
# PR20221: NetIF DMA interface
# 
# 9   2/24/06 5:24p erickson
# PR17108: added 4501 AST
# 
# 8   2/17/06 10:07a erickson
# PR19738: added #define for SDS_CHIP
# 
# 7   2/17/06 9:36a erickson
# PR19738: make 4500 optional for 97401, but default it on
# 
# 6   12/14/05 10:38a jrubio
# PR18491: Add compilation of uhf module
# 
# 5   11/30/05 4:53p erickson
# PR17108: added 7401 pcm
# 
# 4   10/20/05 5:25p erickson
# PR17108: added 7401 hdmi
# 
# 3   10/20/05 4:02p vsilyaev
# PR 17710: Added 3517 support
# 
# 2   10/19/05 5:51p vsilyaev
# PR 17710: Added support for 4500 SDS
# 
# 1   7/18/05 6:13p vsilyaev
# PR 15377: Refactoring of the board support package
# 
############################################################

## 3510/3520  is broken for 97401 platform
#
# 3510/3520 VSB support has to be a compile-time option
# because the API's are similar. We default to
# 3520, but you can define the following in order
# to get 3510 support.
#
include  ${ROCKFORD}/modules/3510.inc
include  ${ROCKFORD}/modules/3510/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97435/common/bcm3510ap 
CFLAGS += -DVSB_CHIP=3510
SRCS += bsettop_tuner_3510.c 

# default 97401 to 4500, but allow it to be turned off

ifeq (${SYSTEM},linux)
# Needs a special version of a kernel, don't enable by default 
#B_HAS_PLAYPUMP_IP ?= y
endif
ifeq (${B_HAS_PLAYPUMP_IP},y)
CFLAGS += -DB_HAS_PLAYPUMP_IP
CFLAGS += ${BNETIF_DMA_CFLAGS}
endif

include  ${ROCKFORD}/modules/uhf.inc

B_HAS_PCM=y
HDMI_TX_SUPPORT=y

