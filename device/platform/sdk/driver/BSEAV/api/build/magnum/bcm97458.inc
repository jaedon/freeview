############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97458.inc $
# $brcm_Revision: 6 $
# $brcm_Date: 8/8/07 10:40a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97458.inc $
# 
# 6   8/8/07 10:40a ayxiang
# PR33836: sync with 97403 4.0 release
# 
# 5   6/26/07 10:55a ayxiang
# PR32518: added unified ITB support to 97458
# 
# 4   6/7/07 6:49p vle
# PR 31924: Define HDMI Tx Support for platform
# 
# 3   4/4/07 12:31p mward
# PR29446: Use B_HAS_3255=n build option instead of B_NO_3255=y for more
# clarity and consistency.
# 
# 2   4/3/07 4:56p mward
# PR29446: Create B_NO_3255=y build option to test boards with 3255
# without using it
# 
# 1   1/15/07 5:38p haisongw
# PR27129: add 97458A0 platform support
#
############################################################

# RAVE unified ITB support. 
CFLAGS += -DUNIFIED_ITB_SUPPORT

ifneq ($(subst N,n,${B_HAS_3255}),n)
include  ${ROCKFORD}/modules/3255/ads.inc
include  ${ROCKFORD}/modules/tnr.inc
include  ${ROCKFORD}/modules/rpc.inc
include  ${ROCKFORD}/modules/3255/tnr3255ib.inc

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

include  ${ROCKFORD}/modules/uhf.inc


ifeq (${SYSTEM},linux)
# Needs a special version of a kernel, don't enable by default
# B_HAS_PLAYPUMP_IP ?= y
endif
ifeq (${B_HAS_PLAYPUMP_IP},y)
CFLAGS += -DB_HAS_PLAYPUMP_IP
CFLAGS += ${BNETIF_DMA_CFLAGS}
endif

include  ${ROCKFORD}/modules/uhf.inc

#SRCS += bsettop_base_hdmi.c
B_HAS_SMARTCARD=y
B_HAS_PCM=y
HDMI_TX_SUPPORT=y