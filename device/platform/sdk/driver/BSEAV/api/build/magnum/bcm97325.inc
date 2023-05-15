############################################################
#     Copyright (c) 2003-2008, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97325.inc $
# $brcm_Revision: 12 $
# $brcm_Date: 2/7/08 6:11p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97325.inc $
# 
# 12   2/7/08 6:11p jrubio
# PR39415: add 7325
# 
# 11   12/27/07 11:31a agin
# PR37869: Eliminate references to 4500.
# 
# 10   12/5/07 2:02p agin
# PR37869: Integrate SDS into 97325 reference software.
# 
# 9   12/4/07 5:31p jrubio
# PR35015: added new rap options
# 
# 8   11/21/07 11:32a jrubio
# PR35015: added some more changes
# 
# 7   11/6/07 11:05a jrubio
# PR35015: adding sds back in
# 
# 6   11/2/07 11:48a jrubio
# PR35015: disable UIT, and sds5
# 
# 5   11/1/07 10:18a jrubio
# PR35015: remove sds
# 
# 4   10/29/07 3:51p jrubio
# PR35015: add ast and sds tuner to 7325
# 
# 3   10/25/07 10:49p jrubio
# PR35015: took aout support for all tuner cards
# 
# 2   10/3/07 4:36p jrubio
# PR35015: making some changes
# 
# 
# 
############################################################

#By default use the raptor fw which supports ddp,ac3 plus.

#By default disable Ac3+ support 
ifneq ($(subst Y,y,${RAP_DDP_SUPPORT}),y)
CFLAGS += -DB_RAP_NO_AC3PLUS_SUPPORT
endif

#By default disable Ac3 support 
ifneq ($(subst Y,y,${RAP_AC3_SUPPORT}),y)
CFLAGS += -DB_RAP_NO_AC3_SUPPORT
endif


#By default disable WMA support 
ifneq ($(subst Y,y,${RAP_WMA_SUPPORT}),y)
CFLAGS += -DB_RAP_NO_WMA_SUPPORT
endif

include  ${ROCKFORD}/modules/tnr.inc


# RAVE unified ITB support. (default on)
ifneq ($(subst N,n,${UNIFIED_ITB_SUPPORT}),n)
CFLAGS += -DUNIFIED_ITB_SUPPORT
UNIFIED_ITB_SUPPORT := y
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

# add 7325 support
ifeq ($(AST7325_SUPPORT),)
AST7325_SUPPORT=y
BAST_CHIP+= 7325
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

ifeq ($(AST7325_SUPPORT),y)
SRCS += bsettop_tuner_73xx.c
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
HDMI_TX_SUPPORT=y
B_HAS_PCM=y
CFLAGS += -DB_HAS_PCM=y

