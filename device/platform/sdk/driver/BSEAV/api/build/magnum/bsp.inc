############################################################
#     copyright (c) 2003-2006, broadcom corporation
#     all rights reserved
#     confidential property of broadcom corporation
#
#  this software may only be used subject to an executed software license
#  agreement  between the user and broadcom.  you have no right to use or
#  exploit this material except subject to the terms of such an agreement.
#
# $brcm_workfile: bsp.inc $
# $brcm_revision: refsw_93560_bringup/1 $
# $brcm_date: 3/10/05 6:53p $
#
# module description:
#
# revision history:
#
# created: 02/09/2001 by marcus kellerman
#
# $brcm_Log: /BSEAV/api/build/magnum/bsp.inc $
# 
# 53   10/10/12 2:54p chengs
# SW7584-42: Add support for 7584.
# 
# 52   9/6/12 11:29p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/6/12 5:28p mphillip
# SW7425-3739: Remove symlinks
# 
# 51   5/18/12 7:29p bselva
# SW7360-19: Add support for 7360 platform
# 
# 50   2/15/11 7:14p xhuang
# SW7358-34: Add 7358/7552 support
# 
# 48   12/22/10 4:35p katrep
# SW7231-25:add initial support for 7230
# 
# 47   12/15/10 10:09a jrubio
# SW7344-9: add 7344/7346 support
# 
# 46   12/6/10 4:22p katrep
# SW7231-4:initial support
# 
# 45   11/10/10 5:38p spothana
# SW7420-1078: Add BCM97409 support.
# 
# 44   10/20/10 3:29p nickh
# SW7422-80: Add 97422 support
# 
# 43   1/20/10 5:38p garetht
# SW7420-553: Add platform for 93380vms
# 
# 42   1/19/10 2:46p nickh
# SW7420-541: Rename 97420dvr2 to 97420
# 
# 41   10/30/09 5:21p nickh
# SW7420-331: Rename 97410c to 97410
# 
# 40   9/15/09 5:07p nickh
# SW7420-331: Add 7410 support
# 
# 39   6/25/09 8:46p leisun
# PR 55693: Chechking in DTCP-IP lib, add 7420 build for bcrypt
# 
# 38   6/20/08 5:57p anilmm
# PR43453: Add support for 97459
# 
# 37   4/25/08 2:30p bandrews
# PR36149: Moved syslib framework impl out of board and into base
# 
# 36   3/28/08 12:38p bandrews
# PR36149: Moved inclusion of bsyslib_framework.inc to file that compiles
# syslib framework impl
# 
# 35   3/27/08 9:27p bandrews
# PR36149: Moved inclusion of syslib framework files to bsp.inc, where
# the impl is compiled
# 
# 34   2/15/08 9:09p bandrews
# PR36149: SETAPI syslib impl
# 
# 33   12/10/07 6:37p mward
# PR38078: Create PLATFORM for 97018, 97018RNG
# 
# 32   11/26/07 12:34p katrep
# PR37217: 7335 bringup
# 
# 31   10/3/07 6:00p jrubio
# PR35015: fix 7325
# 
# 30   9/20/07 11:33a jrubio
# PR35015: adding 7325 support
# 
# 29   4/16/07 1:05p erickson
# PR29890: move 7401 code to api/src/CHIP
# 
# 28   1/17/07 11:33a anilmm
# PR27129:add 97458A0 platform support
#
# 27   11/16/06 5:07p jgarrett
# PR 25230: Moving usermode driver to BSEAV
#
# 26   10/31/06 9:14a erickson
# PR25109: add 7403 support
#
# 25   9/29/06 4:29p mward
# PR24604: create PLATFORM for 97118RNG.
#
# 24   9/6/06 1:26p vsilyaev
# PR 24124: Added framework to detect stack overflow
#
# 23   8/18/06 6:07p katrep
# PR23114: Added support for 7440
#
# 22   5/26/06 9:02a mward
# PR21671: Add support for 7118 chip 97118 board.
#
# 21   4/27/06 5:24p haisongw
# PR 20984: add 97456 platform support
#
# 20   4/20/06 3:18p erickson
# PR19738: 97435 merge
#
# PROD_97435C_REFSW_Devel/1   4/18/06 6:55p gqli
# PR19738: add BCM97435C support
#
# 19   2/23/06 12:08p vsilyaev
# PR 18701: Use RMC_Monitor module istead of RMM
#
# 18   1/19/06 9:53a jgarrett
# PR 19105: Create 97400 BSP
#
# Refsw_Devel_7400_A0/2   1/19/06 9:43a jgarrett
# PR 19105: Creating BSP for 97400
#
# 17   1/13/06 4:13p vsilyaev
# PR 18003: pass memory size configuration as a compiler option
#
# refsw_93560_bringup/1   3/10/05 6:53p vsilyaev
# refactored board specific and bsp files
#
# 1   2/7/05 6:53p dlwin
# merge down for release 2005_refsw_mergetomain:
#
# irvine_bseavsw_devel/8   1/13/05 4:35p vsilyaev
# pr 13823: added support for 97398 board.
#
# irvine_bseavsw_devel/7   10/21/04 4:46p erickson
# pr12857: switch to 3520 support by default
#
# irvine_bseavsw_devel/6   5/12/04 10:23a erickson
# pr10967: added standard header
############################################################

include $(BSEAV)/api/build/Makefile.common

MAGNUM = $(shell cd "$(BSEAV)/../magnum" && ${PWD})
ROCKFORD = $(shell cd "${MAGNUM}/../rockford" && ${PWD})

ifeq (${SYSTEM}, linux)
OS := linuxuser
else
OS := ${SYSTEM}
endif

BCM_BOARD = bcm${BCM_BOARD_NO}

ifeq (${PLATFORM}, 97038)
BCHP_CHIP := 7038
BCHP_QAM_CHIP := 3250
endif

ifeq (${PLATFORM}, 97398)
BCHP_CHIP := 7038
BCHP_XVD_CHIP := 7411
endif

ifeq (${PLATFORM}, 93560)
BCHP_CHIP := 3560
BCHP_XVD_CHIP := 7411
endif

ifeq (${PLATFORM}, 97401)
BCHP_CHIP := 7401
BCHP_XVD_CHIP := 7401
endif

ifeq (${PLATFORM}, 97403)
BCHP_CHIP := 7403
BCHP_XVD_CHIP := 7403
endif

ifeq (${PLATFORM}, 97405)
BCHP_CHIP := 7405
BCHP_XVD_CHIP := 7405
endif

ifeq ($(PLATFORM), 93380vms)
BCHP_CHIP := 7420
BCHP_XVD_CHIP := 7420
endif

ifeq (${PLATFORM}, 97420)
BCHP_CHIP := 7420
BCHP_XVD_CHIP := 7420
endif

ifeq (${PLATFORM}, 97410)
BCHP_CHIP := 7420
BCHP_XVD_CHIP := 7420
endif

ifeq (${PLATFORM}, 97409)
BCHP_CHIP := 7420
BCHP_XVD_CHIP := 7420
endif

ifeq (${PLATFORM}, 97325) 
BCHP_CHIP := 7325
BCHP_XVD_CHIP := 7325
endif


ifeq (${PLATFORM}, 97335) 
BCHP_CHIP := 7335
BCHP_XVD_CHIP := 7335
endif


ifeq (${PLATFORM}, 97435)
BCHP_CHIP := 7401
BCHP_XVD_CHIP := 7401
endif

ifeq (${PLATFORM}, 97455)
BCHP_CHIP := 7401
BCHP_XVD_CHIP := 7401
endif

ifeq (${PLATFORM}, 97458)
BCHP_CHIP := 7403
BCHP_XVD_CHIP := 7403
endif

ifeq (${PLATFORM}, 97459)
BCHP_CHIP := 7405
BCHP_XVD_CHIP := 7405
endif

ifeq (${PLATFORM}, 97456)
BCHP_CHIP := 7400
BCHP_XVD_CHIP := 7400
endif

ifeq (${PLATFORM}, 97400)
BCHP_CHIP := 7400
BCHP_XVD_CHIP := 7400
endif

ifneq ($(findstring $(PLATFORM),97118 97118RNG),)
BCHP_CHIP := 7118
BCHP_XVD_CHIP := 7118
endif

ifneq ($(findstring $(PLATFORM),97018 97018RNG),)
BCHP_CHIP := 7118
BCHP_XVD_CHIP := 7118
endif

ifeq (${PLATFORM}, 97440)
BCHP_CHIP := 7440
BCHP_XVD_CHIP := 7440
endif

ifeq (${PLATFORM}, 97422)
BCHP_CHIP := 7422
BCHP_XVD_CHIP := 7422
endif

ifeq (${PLATFORM}, 97231)
BCHP_CHIP := 7231
BCHP_XVD_CHIP := 7231
endif

ifeq (${PLATFORM}, 97230)
BCHP_CHIP := 7231
BCHP_XVD_CHIP := 7231
endif

ifeq (${PLATFORM}, 97418)
BCHP_CHIP := 7344
BCHP_XVD_CHIP := 7344
endif
ifeq (${PLATFORM}, 97344)
BCHP_CHIP := 7344
BCHP_XVD_CHIP := 7344
endif
ifeq (${PLATFORM}, 97346)
BCHP_CHIP := 7346
BCHP_XVD_CHIP := 7346
endif
ifeq (${PLATFORM}, 97358)
BCHP_CHIP := 7358
BCHP_XVD_CHIP := 7358
endif
ifeq (${PLATFORM}, 97360)
BCHP_CHIP := 7360
BCHP_XVD_CHIP := 7360
endif
ifeq (${PLATFORM}, 97552)
BCHP_CHIP := 7552
BCHP_XVD_CHIP := 7552
endif
ifeq (${PLATFORM}, 97584)
BCHP_CHIP := 7584
BCHP_XVD_CHIP := 7584
endif
include  $(ROCKFORD)/modules/basemodules.inc
include  $(ROCKFORD)/modules/chp.inc
include  $(ROCKFORD)/modules/int.inc
include  $(ROCKFORD)/modules/mem.inc
include  $(ROCKFORD)/modules/mrc.inc
include  $(ROCKFORD)/modules/mth.inc
include  $(ROCKFORD)/modules/reg.inc
include  $(ROCKFORD)/modules/fmt.inc
include  $(ROCKFORD)/modules/i2c.inc
ifeq ($(findstring $(BCHP_CHIP),7401 7403 7405 7038 7400 7118 7440 7325 7335 7344 7346), $(BCHP_CHIP))
include  $(ROCKFORD)/modules/tmr.inc
endif
include  $(ROCKFORD)/modules/${BCHP_CHIP}.inc

ifdef BCHP_XVD_CHIP
include  $(ROCKFORD)/modules/${BCHP_XVD_CHIP}.inc
ifeq (${SYSTEM}, linuxkernel)
CFLAGS += ${CFLAGS_IMG}
else
ifeq (${PLATFORM}, 97398)
include  $(ROCKFORD)/modules/${BCHP_XVD_CHIP}_img.inc
include  $(ROCKFORD)/modules/${BCHP_XVD_CHIP}/rap_img.inc
endif
endif
include  $(ROCKFORD)/modules/img.inc
endif


ifdef BCHP_QAM_CHIP
include  $(ROCKFORD)/modules/${BCHP_QAM_CHIP}.inc
include  $(ROCKFORD)/modules/int_qam.inc
endif

ifeq (${PLATFORM}, 93560)
R_INC += ${ROCKFORD}/commondrivers/swch/93560/
R_OBJ += \
 ${ROCKFORD}/commondrivers/swch/93560/bswch \
 ${ROCKFORD}/commondrivers/swch/93560/bswch_name
endif

ifeq (${PLATFORM}, 93560)
R_INC += ${ROCKFORD}/commondrivers/swch/93560/
endif


R_INC += ${ROCKFORD}/bsp/bcm9${CHIP}/common

ifeq (,$(filter-out linux linuxkernel , ${SYSTEM}))
R_OBJ += \
 ${ROCKFORD}/bsp/bcm${PLATFORM}/common/int1
endif

R_OBJ += ${ROCKFORD}/bsp/bcm9${CHIP}/common/bcmtm

ifeq (,$(filter-out 97018 97118 97455 97400 97401 97403 97405 97038 97440 97325, ${PLATFORM}))
R_INC += ${ROCKFORD}/commondrivers/fpga/${PLATFORM}
R_OBJ += \
 ${ROCKFORD}/commondrivers/fpga/${PLATFORM}/bfpga_name \
 ${ROCKFORD}/commondrivers/fpga/${PLATFORM}/bfpga
endif

SRCS +=  \
	bsettop_board.c \
	bsettop_bsp_${BCHP_CHIP}.c \
	bsettop_${SYSTEM}.c \
	bsettop_priv.c \
	bsettop_core.c \
	bsettop_core_event.c \
	bsettop_core_stackguard.c

ifneq ($(SYSTEM),linuxkernel)
SRCS += bsettop_isr.c
endif

ifeq (${SYSTEM},linux)
CFLAGS += -I${BSEAV}/linux/driver/usermode
endif

# Due to the symlink reorganization, the files which were in 7401 need to override some src/magnum 
# files (but the rest are in src/magnum).  These files are in magnum-superset.
B_API_IMPL = $(BSETTOP)/src/magnum
ifneq ($(findstring $(BCHP_CHIP),7038 3560), $(BCHP_CHIP))
vpath %.c $(BSETTOP)/src/magnum-superset
endif

# This should be first
vpath %.c $(B_API_IMPL)
vpath %.c $(B_API_IMPL)/board

CFLAGS += -I$(B_API_IMPL)/board

ifneq ($(B_BOARD_MEM),)
# pass board specific memory size
CFLAGS += -DB_BOARD_MEM=${B_BOARD_MEM}
endif

