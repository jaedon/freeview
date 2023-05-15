############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97400.inc $
# $brcm_Revision: 30 $
# $brcm_Date: 3/16/10 4:29p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97400.inc $
# 
# 30   3/16/10 4:29p mward
# SW7400-2517:  Maintain REFSW_XVD_7400_SINGLEDECODE support.  Set
# B_MAX_MOSAIC_DECODES=0 at compile time if B_XVD_SINGLEDECODE=y.
# REFSW_PREREL_97400_SINGLE is deprecated, use REFSW_PREREL_97400.
# 
# 29   6/26/09 6:29p mward
# PR47739: Check for B_PASSTHRU_DDP_SUPPORT=n.
# 
# 27   3/13/09 5:37p mward
# PR49827: PR52490: Change name of SRS VIQ to SRS TrueVolume
# PR49827, PR50613: Settings for Dolby Volume and SRS Volume IQ.
# 
# PR49827/2   2/25/09 12:17p mward
# PR52490: Change name of SRS VIQ to SRS TrueVolume
# 
# PR49827/1   1/13/09 5:11p mward
# PR49827, PR50613: Settings for Dolby Volume and SRS Volume IQ.
# 
# 26   11/4/08 4:14p mward
# PR44957: Pass configured RAP formats to Settop API, too.
# 
# 25   8/11/08 5:39p mward
# PR39988: For 7400, select DDP pass-thru support at run-time, switching
# use of RAP channels if necessary.  Merge to main.
# 
# PR39988/1   6/4/08 5:05p mward
# PR39988: Detect dual output / DDP pass for 7400 at runtime.
# 
# 24   5/22/08 6:44p mward
# PR39988: Create build option B_PASSTHRU_DDP_SUPPORT=y which allows DDP
# transcode to SPDIF + Passthru to HDMI.  Dual decode is disabled with
# this option.
# 
# 23   11/28/07 12:51p mward
# PR37117: 7400 Platforms now use UNIFIED_ITB_SUPPORT unconditionally.
# 
# 22   10/12/07 6:39p jrubio
# PR35007: adding 4506
# 
# 21   10/5/07 2:54p mward
# PR32395: Fix AST build on 7400.
# 
# 20   8/29/07 5:58p mward
# PR31736: Default on UNIFIED_ITB_SUPPORT for 97400. (set in make too,
# for xpt.inc.)
# 
# 19   8/29/07 12:11p mward
# PR31736:  Now default on UNIFIED_ITB_SUPPORT for 97400.
# 
# 18   8/2/07 9:14a jgarrett
# PR 33733: Adding smartcard support
# 
# 17   6/7/07 6:48p vle
# PR 31924: Define HDMI Tx Support for platform
# 
# 16   6/1/07 10:47a gmullen
# PR31736: Added unified ITB support.
# 
# 15   5/30/07 11:42a jgarrett
# PR 31687: Adding 4501 C0 support
# 
# 14   5/9/07 1:15p jgarrett
# PR 25900: Enabling UHF for 7400B0
# 
# 13   3/12/07 12:00p arbisman
# PR28638: Add URT support to the build
# 
# 12   3/12/07 11:33a jgarrett
# PR 25900: fixing copy/paste error for A0
# 
# 11   3/8/07 3:42p jgarrett
# PR 25900: Enabling PLAYPUMP_IP for B0
# 
# 10   3/8/07 2:40p jgarrett
# PR 25900: Enabling HDMI
# 
# 9   2/22/07 4:23p jgarrett
# PR 25900: Disabling UHF for B0 (temporary)
# 
# 8   11/28/06 4:00p jgarrett
# PR 20139: Adding 3520 B0 support
# 
# 7   10/12/06 2:47p jrubio
# PR24902: Added IP STB Support
# 
# 6   8/23/06 1:04p jgarrett
# PR 23738: Adding support for 4501 B0
# 
# 5   8/21/06 5:07p jgarrett
# PR 23738: Adding 4501 support
# 
# 4   3/10/06 2:33p jgarrett
# PR 19265: Re-enabling HDMI for the 97400
# 
# 3   2/22/06 10:07a jgarrett
# PR 19674: Enabling dual-decode support for 97400
# 
# 2   1/24/06 5:30p jgarrett
# PR 19250: Update Set-Top API modules for 97400
# 
# 1   1/19/06 9:53a jgarrett
# PR 19105: Create 97400 BSP
# 
# Refsw_Devel_7400_A0/1   1/12/06 3:05p jgarrett
# PR 18997: Creating directories for 7400
# 
############################################################

## 3510/3520  is broken for 97400 platform
#
# 3510/3520 VSB support has to be a compile-time option
# because the API's are similar. We default to
# 3520, but you can define the following in order
# to get 3510 support.
#
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

# default 97400 to 4500, but allow it to be turned off
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

ifeq (${SYSTEM},linux)
# Needs a special version of a kernel, don't enable by default 
#B_HAS_PLAYPUMP_IP ?= y
endif
ifeq (${B_HAS_PLAYPUMP_IP},y)
CFLAGS += -DB_HAS_PLAYPUMP_IP
CFLAGS += ${BNETIF_DMA_CFLAGS}
endif

include ${ROCKFORD}/modules/uhf.inc

B_HAS_PCM=y
HDMI_TX_SUPPORT=y
B_HAS_SMARTCARD=y

ifeq ($(UART_B_SUPPORT),y)
CFLAGS += -DUART_B_SUPPORT
include  $(ROCKFORD)/modules/urt.inc
endif

ifneq ($(subst N,n,${B_PASSTHRU_DDP_SUPPORT}),n)
CFLAGS += -DB_PASSTHRU_DDP_SUPPORT=1 -DRAP_I2S_COMPRESS_SUPPORT
endif

#By default disable AC3+ support 
ifneq ($(subst Y,y,${RAP_DDP_SUPPORT}),y)
CFLAGS += -DB_RAP_NO_AC3PLUS_SUPPORT
endif

#By default disable AC3 support 
ifneq ($(subst Y,y,${RAP_AC3_SUPPORT}),y)
CFLAGS += -DB_RAP_NO_AC3_SUPPORT
endif


#By default disable WMA support 
ifneq ($(subst Y,y,${RAP_WMA_SUPPORT}),y)
CFLAGS += -DB_RAP_NO_WMA_SUPPORT
endif

#Dolby Volume and SRS Volume IQ are mutually exclusive
ifeq ($(subst Y,y,${RAP_DOLBYVOLUME_SUPPORT}),y)
ifeq ($(subst Y,y,${RAP_SRSTRUVOL_SUPPORT}),y)
$(error Only one of RAP_DOLBYVOLUME_SUPPORT or RAP_SRSTRUVOL_SUPPORT should be "y")
else
CFLAGS += -DRAP_DOLBYVOLUME_SUPPORT
endif
endif

ifeq ($(subst Y,y,${RAP_SRSTRUVOL_SUPPORT}),y)
ifeq ($(subst Y,y,${RAP_DOLBYVOLUME_SUPPORT}),y)
$(error Only one of RAP_DOLBYVOLUME_SUPPORT or RAP_SRSTRUVOL_SUPPORT should be "y")
else
CFLAGS += -DRAP_SRSTRUVOL_SUPPORT
endif
endif

ifneq ($(subst Y,y,${B_XVD_SINGLEDECODE}),y)
CFLAGS += -DB_MAX_MOSAIC_DECODES=12
else
CFLAGS += -DB_MAX_MOSAIC_DECODES=0
endif


