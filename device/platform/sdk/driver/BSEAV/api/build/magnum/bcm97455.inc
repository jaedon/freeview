############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97455.inc $
# $brcm_Revision: 14 $
# $brcm_Date: 11/30/07 3:37p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97455.inc $
# 
# 14   11/30/07 3:37p prasadv
# PR37810: Adding UNIFIED_ITB_SUPPORT for 97455 as per 97401.
# 
# 13   6/7/07 6:48p vle
# PR 31924: Define HDMI Tx Support for platform
# 
# 12   4/4/07 12:32p mward
# PR29446: Use B_HAS_3255=n build option instead of B_NO_3255=y for more
# clarity and consistency.
# 
# 11   4/3/07 4:55p mward
# PR29446: Create B_NO_3255=y build option to test boards with 3255
# without using it
# 
# 10   1/15/07 5:11p haisongw
# PR27129: enable PCM and SMARTCARD support for 97455
#
# 9   10/16/06 5:23p haisongw
# PR24789: add 7401c0 support for 97455 Platform
#
# 8   10/10/06 4:38p haisongw
# PR24845: add B_HAS_OOB for non-oob support
#
# 7   9/20/06 6:59p haisongw
# PR24338: add 97455B0 Board support
#
# 6   9/20/06 2:32p jackli
# PR24025: enable netifdma
#
# 5   8/28/06 2:46p haisongw
# PR 22368: Add AOB, AUS and TNR3255ob support
#
# 4   7/10/06 4:00p haisongw
# PR 22368: Add AOB, AUS and TNR3255ob support
#
# 3   5/16/06 1:38a erickson
# PR18491: added uhf
#
# 2   4/11/06 5:23p dlwin
# PR 19978: Merge to main branch for 3255 support.
#
# PROD_DSG_REFSW_Devel/1   4/3/06 2:06p haisongw
# PR 19978: add  tuner Settop API support on BCM97455 platform
#
# 1   11/23/05 5:11p erickson
# PR18256: added 97455 support
#
############################################################

# RAVE unified ITB support. (default on)
ifneq ($(subst N,n,${UNIFIED_ITB_SUPPORT}),n)
CFLAGS += -DUNIFIED_ITB_SUPPORT
UNIFIED_ITB_SUPPORT := y
endif

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

ifeq ($(BCM97455MBV00),y)
CFLAGS += -DBCM97455MBV00
endif

ifeq (${SYSTEM},linux)
# Needs a special version of a kernel, don't enable by default
# B_HAS_PLAYPUMP_IP ?= y
endif
ifeq (${B_HAS_PLAYPUMP_IP},y)
CFLAGS += -DB_HAS_PLAYPUMP_IP
CFLAGS += ${BNETIF_DMA_CFLAGS}
endif

B_HAS_SMARTCARD=y
B_HAS_PCM=y
HDMI_TX_SUPPORT=y