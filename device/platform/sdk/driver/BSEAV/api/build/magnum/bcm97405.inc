############################################################
#     Copyright (c) 2003-2008, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97405.inc $
# $brcm_Revision: 14 $
# $brcm_Date: 2/1/08 4:59p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97405.inc $
# 
# 14   2/1/08 4:59p katrep
# PR39180: Added support for ofdm/DVB-T THD tuner
# 
# 13   1/18/08 3:37p katrep
# PR38239: Added support for external analog input.656 for video/i2s
# input for audio. Enable this feature by compiling with
# B_HAS_EXTERNAL_ANALOG=1 compile time flag.
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

#
# On 97405 3510 is the default option
#
ifeq ($(VSB3510_SUPPORT),)
VSB3510_SUPPORT=y
endif

ifeq ($(VSB3510_SUPPORT),y)
include  ${ROCKFORD}/modules/3510.inc
include  ${ROCKFORD}/modules/3510/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97400/common/bcm3510ap 
CFLAGS += -DVSB_CHIP=3510
SRCS += bsettop_tuner_3510.c 
else
include  ${ROCKFORD}/modules/3520.inc
include  ${ROCKFORD}/modules/3520/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97400/common/bcm3520ap_a1
R_OBJ += ${ROCKFORD}/bsp/bcm97400/common/bcm3520ap_b0
SRCS += bsettop_tuner_3520.c 
endif

# default 97405 to 4500, but allow it to be turned off
ifeq ($(SDS4500_SUPPORT),)
SDS4500_SUPPORT=y
endif

ifeq ($(SDS4500_SUPPORT),y)
R_OBJ += ${ROCKFORD}/bsp/bcm97405/common/bcm4500ap 
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

ifeq (${B_HAS_I2S_OUTPUT},y)
CFLAGS += -DB_HAS_I2S_OUTPUT=1
endif

ifeq (${B_HAS_EXTERNAL_ANALOG},y)
CFLAGS += -DB_HAS_EXTERNAL_ANALOG=1
endif


# add DVB-T tuner  support
ifeq ($(B_HAS_OFDM),)
THD2940_SUPPORT=y
endif

ifeq ($(THD2940_SUPPORT),y)
BTHD_CHIP+= 2940
include  ${MAGNUM}/portinginterface/thd/bthd.inc
SRCS += bsettop_tuner_2940.c
CFLAGS += -DTHD_CHIP=2940
CFLAGS += -DB_HAS_OFDM
endif

