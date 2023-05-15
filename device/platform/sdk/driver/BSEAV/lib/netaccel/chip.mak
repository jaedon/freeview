############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: chip.mak $
# $brcm_Revision: 30 $
# $brcm_Date: 10/10/12 2:50p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/netaccel/chip.mak $
# 
# 30   10/10/12 2:50p chengs
# SW7584-42: Add Brutus support for 7584.
# 
# 29   5/18/12 8:13p bselva
# SW7360-19: Add support for 7360 platform
# 
# 28   4/26/12 6:08p mward
# SW7435-139:  Add 7435.
# 
# 27   2/2/12 2:49p katrep
# SW7429-1:add support for 7429
# 
# 26   2/16/11 7:59p xhuang
# SW7358-34: Add 7358/7552 support
# 
# 25   2/14/11 11:46a spothana
# SW7422-258: Add Trinity support to BCM97425
# 
# 24   1/11/11 8:07p spothana
# SW7422-96 : Add Trinity support for BCM97422 platform.
# 
# 23   12/22/10 4:35p katrep
# SW7231-25:add initial support for 7230
# 
# 22   12/15/10 10:43a jrubio
# SW7344-9: add 7344/7346 support
# 
# 21   12/9/10 5:54p katrep
# SW7231-4:add 7231 support
# 
# 20   11/10/10 5:37p spothana
# SW7420-1078 : Add BCM97409 platform support.
# 
# 19   2/5/10 1:20p gmohile
# SW7408-1 : Add 7408 netaccel binaries
# 
# 18   1/20/10 5:49p garetht
# SW7420-553: Add platform for 93380vms
# 
# 17   1/19/10 3:20p nickh
# SW7420-541: Rename 97420dvr2 to 97420
# 
# 16   12/4/09 9:43p randyjew
# SW7468-6: Add 7468/7208 support
# 
# 15   10/30/09 5:37p nickh
# SW7420-331: Rename 97410c to 97410
# 
# 14   10/19/09 3:31p ssood
# SW7420-340: enabling netaccel support for 93380sms
# 
# 13   9/30/09 5:47p mward
# SW7125-4: Add 7125.
# 
# 12   9/15/09 6:29p nickh
# SW7420-331: Add 7410 support
# 
# 11   8/21/09 5:09p jrubio
# PR55232: add 7342 suppport
# 
# 10   3/2/09 10:44a katrep
# PR52599: Add support 7305
# 
# 9   2/4/09 10:32a jrubio
# PR51629: add 7336 support
# 
# 8   12/24/08 12:36p ssood
# PR48963: Add 7420 support
# 
# 7   12/19/08 7:39p katrep
# PR50567: Add Support for 97466 platform
# 
# 6   7/29/08 8:11a erickson
# PR42739: add 3548/3556
#
# 5   6/20/08 7:13p anilmm
# PR43453: Add support for 97459
#
# 4   12/17/07 11:37a katrep
# PR37217: Created netaccel dirs and added usermode binary.
#
# 3   12/10/07 6:59p mward
# PR38078: Create PLATFORM for 97018, 97018RNG
#
# 2   12/4/07 11:31a mward
# PR35015: added 7325
#
# 1   11/28/07 5:25p mward
# PR35552: Move netaccel driver copying logic from Brutus makefile to
# netaccel one.
#
#
############################################################

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

ifeq ($(findstring $(PLATFORM), 97405 97205 97466), $(PLATFORM))
BCHP_CHIP := 7405
BCHP_XVD_CHIP := 7405
endif

ifeq (${PLATFORM}, 97335)
BCHP_CHIP := 7335
BCHP_XVD_CHIP := 7335
endif

ifeq (${PLATFORM}, 97340)
BCHP_CHIP := 7340
BCHP_XVD_CHIP := 7340
endif

ifeq (${PLATFORM}, 97342)
BCHP_CHIP := 7342
BCHP_XVD_CHIP := 7342
endif

ifneq ($(findstring $(PLATFORM), 97468 97208),)
BCHP_CHIP := 7468
BCHP_XVD_CHIP := 7468
endif

ifeq (${PLATFORM}, 97336)
BCHP_CHIP := 7336
BCHP_XVD_CHIP := 7336
endif

ifeq (${PLATFORM}, 97455)
BCHP_CHIP := 7401
BCHP_XVD_CHIP := 7401
endif

ifeq (${PLATFORM}, 97458)
BCHP_CHIP := 7403
BCHP_XVD_CHIP := 7403
endif

ifeq (${PLATFORM}, 97456)
BCHP_CHIP := 7400
BCHP_XVD_CHIP := 7400
endif

ifeq (${PLATFORM}, 97459)
BCHP_CHIP := 7405
BCHP_XVD_CHIP := 7405
endif

ifeq (${PLATFORM}, 97400)
BCHP_CHIP := 7400
BCHP_XVD_CHIP := 7400
endif

ifneq ($(findstring $(PLATFORM), 97018 97018RNG 97118 97118RNG),)
BCHP_CHIP := 7118
BCHP_XVD_CHIP := 7118
endif

ifneq ($(findstring $(PLATFORM), 97019 97025 97119 97125 97116),)
BCHP_CHIP := 7125
BCHP_XVD_CHIP := 7125
endif

ifeq (${PLATFORM}, 97440)
BCHP_CHIP := 7440
BCHP_XVD_CHIP := 7440
endif

ifeq (${PLATFORM}, 97325)
BCHP_CHIP := 7325
BCHP_XVD_CHIP := 7325
endif

ifeq (${PLATFORM}, 93549)
BCHP_CHIP := 3548
BCHP_XVD_CHIP := 3548
endif

ifeq (${PLATFORM}, 93556)
BCHP_CHIP := 3556
BCHP_XVD_CHIP := 3556
endif

ifneq ($(findstring $(PLATFORM), 97420 97410 97409 93380sms, 93380vms),)
BCHP_CHIP := 7420
BCHP_XVD_CHIP := 7420
endif

ifeq (${PLATFORM},97422)
BCHP_CHIP := 7422
BCHP_XVD_CHIP := 7422
endif

ifeq (${PLATFORM},97425)
BCHP_CHIP := 7425
BCHP_XVD_CHIP := 7425
endif

ifeq (${PLATFORM}, 97408)
BCHP_CHIP := 7408
BCHP_XVD_CHIP := 7408
endif

ifeq (${PLATFORM}, 97231)
BCHP_CHIP := 7231
BCHP_XVD_CHIP := 7231
endif

ifeq (${PLATFORM}, 97230)
BCHP_CHIP := 7231
BCHP_XVD_CHIP := 7231
endif

ifeq (${PLATFORM}, 97429)
BCHP_CHIP := 7429
BCHP_XVD_CHIP := 7429
endif

ifeq (${PLATFORM}, 97428)
BCHP_CHIP := 7429
BCHP_XVD_CHIP := 7429
endif

ifeq (${PLATFORM}, 97241)
BCHP_CHIP := 7429
BCHP_XVD_CHIP := 7429
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

ifeq (${PLATFORM},97435)
BCHP_CHIP := 7435
BCHP_XVD_CHIP := 7435
endif

ifeq (${PLATFORM}, 97584)
BCHP_CHIP := 7584
BCHP_XVD_CHIP := 7584
endif

