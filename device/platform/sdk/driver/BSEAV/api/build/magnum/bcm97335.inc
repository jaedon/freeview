############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97335.inc $
# $brcm_Revision: 3 $
# $brcm_Date: 12/27/07 7:30p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97335.inc $
# 
# 3   12/27/07 7:30p agin
# PR38452: Integrate SDS into 97335 reference software.
# 
# 2   12/5/07 4:05p katrep
# PR37217: Added 7335 support
# 
# 12   11/5/07 10:34a katrep
# PR27643: Enabled smartcard support
# 
# 11   10/12/07 5:18p jrubio
# PR35007: must compile both 4501 and 4506^
# 
# 10   10/11/07 1:56p jrubio
# PR35007: adding 4506 tuner support
# 
# 9   10/9/07 3:35a katrep
# PR35844: Added PCM playback support for 7405.
# 
# 8   10/6/07 12:47a katrep
# PR34354: Added compile time support for various raptor flags in settop
# api.
# 
# 7   9/20/07 1:58p katrep
# PR34354: Diabled AC3,AC3 plus audio support as default options.
# 
# 6   7/12/07 1:54p katrep
# PR27643: 7405 first audio integration.
# 
# 5   6/28/07 1:51p katrep
# PR27643: Enable unified itb format as default option.
# 
# 4   6/7/07 6:48p vle
# PR 31924: Define HDMI Tx Support for platform
# 
# 3   6/1/07 11:44a gmullen
# PR27642: Added unified ITB support.
# 
# 2   5/31/07 3:07p katrep
# PR27643:Added support for 7405
# 
# 1   3/26/07 7:09p katrep
# PR27643: Added support for 7405
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


#dont need this for 7335 but, need it now to compile  
ifeq ($(AST4501_SUPPORT),)
AST4501_SUPPORT=y
BAST_CHIP+= 4501
endif

# add 4506 support
ifeq ($(AST4506_SUPPORT),)
AST4506_SUPPORT=y
BAST_CHIP+= 4506
endif

# add 7335 support
ifeq ($(AST7335_SUPPORT),)
AST7335_SUPPORT=y
BAST_CHIP+= 7335
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

ifeq ($(AST7335_SUPPORT),y)
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

