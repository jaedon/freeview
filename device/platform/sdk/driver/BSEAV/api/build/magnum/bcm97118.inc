############################################################
#     Copyright (c) 2003-2009, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97118.inc $
# $brcm_Revision: 23 $
# $brcm_Date: 2/17/09 3:13p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97118.inc $
# 
# 23   2/17/09 3:13p mward
# PR46431: Fix compile with update bads.inc.
# 
# 22   2/2/09 10:47a mward
# PR46431: Specify BADS_CHIP=3255.
# 
# 21   1/23/09 10:18a mward
# PR46436: Specify BAOB_CHIP=3255.
# 
# 20   8/29/07 5:57p mward
# PR32434: Default UNIFIED_ITB_SUPPORT for 97118[RNG] (correct comment)
# 
# 19   8/13/07 6:34p mward
# PR32434: Default UNIFIED_ITB_SUPPORT for 97118[RNG] again, working now.
# 
# 18   8/8/07 6:07p mward
# PR32434: Default UNIFIED_ITB_SUPPORT for 97118[RNG] off, audio
# problems.
# 
# 17   8/6/07 12:30p mward
# PR32434: Default UNIFIED_ITB_SUPPORT for 97118[RNG]
# 
# 16   7/26/07 12:34p mward
# PR32434: Add UNIFIED_ITB_SUPPORT=y build option for 97118[RNG]
# platforms.
# 
# 15   4/23/07 11:05a mward
# PR29446: Adding B_HAS_3255=n same as B_NO_BNM=y for consistency with
# other boards.
# 
# 14   3/26/07 5:08p mward
# PR24922: If POD build is enabled,  don't set pinmuxes for XVD UARTs.
# Conflict, pins needed for POD.
# 
# 13   3/15/07 10:28a mward
# PR28683: exclude OOB_SUPPORT with B_NO_BNM=y build option.
# 
# 12   3/14/07 10:57a mward
# PR28683: exclude OOB_SUPPORT option for BCM97118RNG.
# 
# 11   3/13/07 2:23p jackli
# PR28683: add OOB_SUPPORT option for BCM97118
# 
# 10   12/6/06 6:04p mward
# PR26388: add support for I2S output.
# 
# 9   11/27/06 3:26p mward
# PR23262: Add PCM, Smartcard for 97118.
# 
# 8   10/19/06 5:27p mward
# PR25041: Disable BNM "3255" support with B_NO_BNM=y
# 
# 7   10/17/06 7:27p mward
# PR24517: Correct findstring.
# 
# 6   10/12/06 7:39p mward
# PR24517: Support 3510 demod.
# 
# 5   9/30/06 1:55p mward
# PR24666: enable netifdma
# 
# 4   9/7/06 7:04p mward
# PR24160: allow selection of build without 3255 in one place for bringup
# 
# 3   8/23/06 5:58p mward
# PR21671: 97118 temp. disable FPGA and 3255 - not ready yet.
# 
# 2   7/11/06 3:54p mward
# PR21671:  Add AOB, AUS and TNR3255ob support for 7118
# 
# 1   5/25/06 6:20p mward
# PR21671: Add support for 7118 chip 97118 board.
#  
############################################################

include  ${ROCKFORD}/modules/tnr.inc

# RAVE unified ITB support. (default on)
ifneq ($(subst N,n,${UNIFIED_ITB_SUPPORT}),n)
CFLAGS += -DUNIFIED_ITB_SUPPORT
UNIFIED_ITB_SUPPORT := y
endif

ifeq ($(subst N,n,${B_HAS_3255}),n)
B_NO_BNM := y
endif

ifneq (${B_NO_BNM},y)
include  ${ROCKFORD}/modules/rpc.inc
include  ${ROCKFORD}/modules/3255/tnr3255ib.inc

ifeq ($(findstring RNG,$(PLATFORM)),)
OOB_SUPPORT ?=y
ifeq ($(OOB_SUPPORT),y)
CFLAGS += -DB_HAS_OOB=1
BAOB_CHIP=3255
include  ${ROCKFORD}/modules/3255/aob.inc
include  ${ROCKFORD}/modules/3255/aus.inc
include  ${ROCKFORD}/modules/3255/tnr3255ob.inc
endif
endif

SRCS += bsettop_tuner_3255.c 
CFLAGS += -DB_HAS_3255=1
BADS_CHIP=3255
BADS_3255_SUPPORT=y
include  ${MAGNUM}/portinginterface/ads/bads.inc
endif

ifeq ($(findstring RNG,$(PLATFORM)),)
include  ${ROCKFORD}/modules/3510.inc
include  ${ROCKFORD}/modules/3510/vsb.inc
R_OBJ += ${ROCKFORD}/bsp/bcm97118/common/bcm3510ap 
CFLAGS += -DVSB_CHIP=3510
SRCS += bsettop_tuner_3510.c 
OBJ += portinginterface/tnr/3418/btnr_3418 \
        portinginterface/tnr/3418/btnr_3418_regio \
        portinginterface/tnr/3418/btnr_3418_smarttune
INC += portinginterface/tnr/3418
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

ifeq (${B_HAS_I2S_OUTPUT},y)
CFLAGS += -DB_HAS_I2S_OUTPUT=1
endif

ifeq ($(CABLECARD_SUPPORT),SPOD)
CFLAGS += -DSPOD_SUPPORT
else
ifeq ($(CABLECARD_SUPPORT),MPOD)
CFLAGS += -DMPOD_SUPPORT
endif
endif
