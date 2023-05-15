############################################################
#     Copyright (c) 2003-2009, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97401.inc $
# $brcm_Revision: 22 $
# $brcm_Date: 4/16/09 10:41a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97401.inc $
# 
# 22   4/16/09 10:41a gmohile
# PR 54128 : Add DDP Passthru support
# 
# 21   3/24/09 1:11p gmohile
# PR 51326 : Add SRS support for 7401
# 
# 20   3/24/08 5:06p bandrews
# PR40865: Fixed reference to astmlib build dir
# 
# 19   2/19/08 4:42p bandrews
# PR36935: SETAPI astm integration
# 
# 18   10/26/07 10:46a gmullen
# PR36449: Added unified ITB support. Defaults to unified ITB enabled.
# 
# 17   10/12/07 6:37p jrubio
# PR35007: adding 4506
# 
# 16   10/3/07 3:54p gmohile
# PR 25109 : Fix ast build error
# 
# 15   6/7/07 6:48p vle
# PR 31924: Define HDMI Tx Support for platform
# 
# 14   12/14/06 4:43p vle
# PR 26539: add 4501 C0 support to Settop API.
# 
# 13   11/14/06 3:19p jrubio
# PR25214: added b0_image for 3520
# 
# 12   8/23/06 12:53p erickson
# PR17108: convert 4501 support to B0
# 
# 11   7/27/06 4:05p qcheng
# PR22674: fix smartcard Settop API test example on 7401 A0.
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

# RAVE unified ITB support. (default on)
ifneq ($(subst N,n,${UNIFIED_ITB_SUPPORT}),n)
CFLAGS += -DUNIFIED_ITB_SUPPORT
UNIFIED_ITB_SUPPORT := y
endif

## 3510/3520  is broken for 97401 platform
#
# 3510/3520 VSB support has to be a compile-time option
# because the API's are similar. We default to
# 3520, but you can define the following in order
# to get 3510 support.
#
ifeq ($(VSB3510_SUPPORT),y)
include  ${ROCKFORD}/modules/3510.inc
include  ${ROCKFORD}/modules/3510/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97401/common/bcm3510ap 
CFLAGS += -DVSB_CHIP=3510
SRCS += bsettop_tuner_3510.c 
else
include  ${ROCKFORD}/modules/3520.inc
include  ${ROCKFORD}/modules/3520/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97401/common/bcm3520ap_a1
R_OBJ += ${ROCKFORD}/bsp/bcm97401/common/bcm3520ap_b0
SRCS += bsettop_tuner_3520.c 
endif

# default 97401 to 4500, but allow it to be turned off
ifeq ($(SDS4500_SUPPORT),)
SDS4500_SUPPORT=y
endif

ifeq ($(SDS4500_SUPPORT),y)
R_OBJ += ${ROCKFORD}/bsp/bcm97401/common/bcm4500ap 
include  ${ROCKFORD}/modules/4500.inc
include  ${ROCKFORD}/modules/4500/sds.inc
SRCS += bsettop_tuner_4500.c 
CFLAGS += -DSDS_CHIP=4500
endif

# default 97405 to 4501, but allow it to be turned off
ifeq ($(AST4501_SUPPORT),)
AST4501_SUPPORT=y
BAST_CHIP+= 4501
endif

# add 4506 support
ifeq ($(AST4506_SUPPORT),)
AST4506_SUPPORT=y
BAST_CHIP+= 4506
endif

ifeq ($(AST4501_SUPPORT),y)
include  ${MAGNUM}/portinginterface/ast/bast.inc
SRCS += bsettop_tuner_4501.c 
CFLAGS += -DAST_CHIP=450x
endif


ifeq ($(AST4506_SUPPORT),y)
SRCS += bsettop_tuner_4506.c
ifeq ($(AST4501_SUPPORT),) 
include  ${MAGNUM}/portinginterface/ast/bast.inc
CFLAGS += -DAST_CHIP=450x
endif
endif

ifeq ($(ASTM_SUPPORT),y)
include  ${MAGNUM}/syslib/astmlib/bastmlib.inc
CFLAGS += -DBASTM_SUPPORT
endif

ifeq (${SYSTEM},linux)
# Needs a special version of a kernel, don't enable by default 
#B_HAS_PLAYPUMP_IP ?= y
endif
ifeq (${B_HAS_PLAYPUMP_IP},y)
CFLAGS += -DB_HAS_PLAYPUMP_IP
CFLAGS += ${BNETIF_DMA_CFLAGS}
endif

include  ${ROCKFORD}/modules/uhf.inc

B_HAS_SMARTCARD=y
B_HAS_PCM=y
HDMI_TX_SUPPORT=y


ifeq ($(subst Y,y,${RAP_SRSTRUVOL_SUPPORT}),y)
CFLAGS += -DRAP_SRSTRUVOL_SUPPORT
endif

ifeq ($(B_PASSTHRU_DDP_SUPPORT),)
CFLAGS += -DB_PASSTHRU_DDP_SUPPORT=1 -DRAP_I2S_COMPRESS_SUPPORT
endif

