############################################################
#     Copyright (c) 2003-2008, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97459.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 6/11/08 5:49p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97459.inc $
# 
# 2   6/11/08 5:49p anilmm
# PR43453: Add support for 97459
# 
#
############################################################

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

# By default enable the unified itb format 
UNIFIED_ITB_SUPPORT=y

# RAVE unified ITB support. 
ifeq ($(UNIFIED_ITB_SUPPORT),y)
CFLAGS += -DUNIFIED_ITB_SUPPORT
endif

#CFLAGS += -DBMEM_SAFETY_CONFIG=BMEM_CONFIG_FASTEST
ifneq ($(subst N,n,${B_HAS_3255}),n)
include  ${ROCKFORD}/modules/3255/ads.inc
include  ${ROCKFORD}/modules/tnr.inc
include  ${ROCKFORD}/modules/3255/tnr3255ib.inc
include  ${ROCKFORD}/modules/rpc.inc

SRCS += bsettop_tuner_3255.c
CFLAGS += -DB_HAS_3255=1

OOB_SUPPORT ?=y
ifeq ($(OOB_SUPPORT),y)
CFLAGS += -DB_HAS_OOB=1
include  ${ROCKFORD}/modules/3255/aob.inc
include  ${ROCKFORD}/modules/3255/aus.inc
include  ${ROCKFORD}/modules/3255/tnr3255ob.inc
endif
endif


ifeq (${SYSTEM},linux)
# Needs a special version of a kernel, don't enable by default
# B_HAS_PLAYPUMP_IP ?= y
endif
ifeq (${B_HAS_PLAYPUMP_IP},y)
CFLAGS += -DB_HAS_PLAYPUMP_IP
CFLAGS += ${BNETIF_DMA_CFLAGS}
endif

B_HAS_SMARTCARD=n
# SAMSUNGIZE : END

include ${ROCKFORD}/modules/uhf.inc	

B_HAS_PCM=y
HDMI_TX_SUPPORT=y