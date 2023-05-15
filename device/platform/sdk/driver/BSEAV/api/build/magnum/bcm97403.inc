############################################################
#     Copyright (c) 2003-2008, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97403.inc $
# $brcm_Revision: 7 $
# $brcm_Date: 4/25/08 2:30p $
#
# Module Description:
#
# Revision History:
# 
############################################################

# RAVE unified ITB support. 
CFLAGS += -DUNIFIED_ITB_SUPPORT

## 3510/3520  is broken for 97403 platform
#
# 3510/3520 VSB support has to be a compile-time option
# because the API's are similar. We default to
# 3520, but you can define the following in order
# to get 3510 support.
#
ifeq ($(VSB3510_SUPPORT),y)
include  ${ROCKFORD}/modules/3510.inc
include  ${ROCKFORD}/modules/3510/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97403/common/bcm3510ap 
CFLAGS += -DVSB_CHIP=3510
SRCS += bsettop_tuner_3510.c 
else
include  ${ROCKFORD}/modules/3520.inc
include  ${ROCKFORD}/modules/3520/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97403/common/bcm3520ap_a1
R_OBJ += ${ROCKFORD}/bsp/bcm97403/common/bcm3520ap_b0
SRCS += bsettop_tuner_3520.c 
endif

# default 97403 to 4500, but allow it to be turned off
ifeq ($(SDS4500_SUPPORT),)
SDS4500_SUPPORT=y
endif

ifeq ($(SDS4500_SUPPORT),y)
R_OBJ += ${ROCKFORD}/bsp/bcm97403/common/bcm4500ap 
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


