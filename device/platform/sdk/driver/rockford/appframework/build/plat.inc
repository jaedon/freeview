#############################################################################
#
#       Copyright (c) 2003-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: plat.inc $
# $brcm_Revision: Hydra_Software_Devel/87 $
# $brcm_Date: 7/18/12 3:31p $
#
# File Description: magnum build include file
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/plat.inc $
# 
# Hydra_Software_Devel/87   7/18/12 3:31p pntruong
# SW7445-8: Initial support.
# 
# Hydra_Software_Devel/86   6/26/12 3:34p pntruong
# SW7563-4: Corrected chip id for 7563.
#
# Hydra_Software_Devel/85   6/26/12 1:59p pntruong
# SW7563-4: Initial support for 7563.
#
# Hydra_Software_Devel/84   5/11/12 3:51p pntruong
# SW7429-170: Fixed build errors.
#
# Hydra_Software_Devel/83   5/11/12 1:40p pntruong
# SW7429-170: Added initial support for 97241.  Branched of from 97429.
#
# Hydra_Software_Devel/82   4/25/12 5:10p darnstein
# SW7584-22: support BVBI for 7584 chipset. I cloned the logic for 7231
# chipset.
#
# Hydra_Software_Devel/81   1/10/12 3:05p davidp
# SWCHECKFRAME-80: Enable FRAMEWORK_DO_I2C to work around HDMI issue.
#
# Hydra_Software_Devel/80   12/15/11 2:38p bselva
# SW7360-6: Add appframework support for 7360 platform
#
# Hydra_Software_Devel/79   11/23/11 2:45p tdo
# SW7435-9: add support for 7435A0 in VDC
#
# Hydra_Software_Devel/78   10/26/11 2:16p mward
# SW7435-9: Add 97435 appframework board.
#
# Hydra_Software_Devel/77   9/9/11 5:58p darnstein
# SW7429-15: first guess at support for 7429 chipset.
#
# Hydra_Software_Devel/76   3/15/11 4:06p darnstein
# SW7425-164: Adapt to changes in the "plat" script.
# - Change DEBUG to B_REFSW_DEBUG.
# - Change ARCH to B_REFSW_ARCH.
# - Change OS to B_REFSW_OS.
# - Change mips-linux-uclibc to mips-linux.
# - Change mipsel-linux-uclibc to mipsel-linux.
#
# Hydra_Software_Devel/75   3/14/11 5:41p jhaberf
# SW35330-13: replaced 35330 support with 35233 support
#
# Hydra_Software_Devel/74   2/22/11 3:59p jhaberf
# SW35330-13: added support for 35330 DTV chip
#
# Hydra_Software_Devel/73   12/5/10 1:37p etrudeau
# SWBLURAY-23579: add 7640 support
#
# Hydra_Software_Devel/72   11/11/10 6:53p darnstein
# SW7344-7: first cut at 7344 and 7346 support.
#
# Hydra_Software_Devel/71   11/9/10 4:21p jhaberf
# SW35125-1: Added 35125 DTV chip support
#
# Hydra_Software_Devel/70   11/1/10 3:49p xhuang
# SW7552-4: Add 7552 support
#
# Hydra_Software_Devel/69   10/28/10 11:29p pntruong
# SW7231-2: Initial support.
#
# Hydra_Software_Devel/68   10/26/10 6:45p hongtaoz
# SW7425-32: added i2c compile support to emu;
#
# Hydra_Software_Devel/67   8/27/10 11:43a tdo
# SW7425-24: Add appframework support for 7425
#
# Hydra_Software_Devel/66   8/5/10 11:30a xhuang
# SW7358-3: Add support for 7358
#
# Hydra_Software_Devel/65   6/22/10 11:37a vanessah
# SW7422-12:  To support appframework. Missing files added:
# magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
# do list: 1. in framework_board.c, more initialization to be done.  2.
# More registers mapping, like clock generation as well as
# BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
#
# Hydra_Software_Devel/64   2/23/10 2:45p pblanco
# SW7125-217: Use 7125 as BCHP_CHIP for 7019 and create a new Jira
# category to track this issue.
#
# Hydra_Software_Devel/63   2/23/10 1:26p pblanco
# SWCHECKFRAME-42: Added 97019 to platforms.
#
# Hydra_Software_Devel/62   1/20/10 11:48a tdo
# SW7420-541: Change to use generic platform 97420
#
# Hydra_Software_Devel/61   1/20/10 10:22a kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/SW35230-56/1   1/18/10 6:06p kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/60   1/7/10 3:41p rpan
# SW7468-65: Support for 97208 board.
#
# Hydra_Software_Devel/59   11/20/09 4:57p tdo
# SW7420-376: Change platform 97410c to 97410
#
# Hydra_Software_Devel/58   11/18/09 8:40a pntruong
# SW7408-13: Added initial Appframework/VDC support for 7408.
#
# Hydra_Software_Devel/57   11/12/09 12:18p rpan
# SW7468-21: Remove a white space after "BCHP_CHIP = 7468".
#
# Hydra_Software_Devel/56   11/11/09 11:29a rpan
# SW7468-21: Initial work for 7468.
#
# Hydra_Software_Devel/55   11/6/09 5:57p nitinb
# SW7550-65: Enable I2C for 7550
#
# Hydra_Software_Devel/54   10/15/09 4:16p paultsai
# SW35230-6: Add support for 35230
#
# Hydra_Software_Devel/53   10/15/09 9:39a tdo
# SW7420-376: Bringup appframework for 7410
#
# Hydra_Software_Devel/52   7/15/09 4:56p jhaberf
# PR53796: Removed i2c from the build until the changes for the bcm35130
# are coded.
#
# Hydra_Software_Devel/51   7/15/09 1:44p albertl
# PR55547: Added support for 7125.
#
# Hydra_Software_Devel/50   6/22/09 4:39p jessem
# PR 54380, PR 54881: Added linuxemu support and removed linuxclient
# support.
#
# Hydra_Software_Devel/49   6/19/09 5:44p darnstein
# PR55225: add support for 7342.
#
# Hydra_Software_Devel/48   6/5/09 3:05p darnstein
# PR55219: add symbols for 7340.
#
# Hydra_Software_Devel/47   5/28/09 5:40p nitinb
# PR 55491: Added support for 7550
#
# Hydra_Software_Devel/46   4/28/09 5:53p jhaberf
# PR53796: Added support for the BCM35130 DTV chip.
#
# Hydra_Software_Devel/45   3/26/09 10:57a vsilyaev
# PR 53611: Use simply expanded variables everywhere where $(shell ...)
# is used. It prevents evaluation of $(shell ) every where variable is
# used
#
# Hydra_Software_Devel/bdvd_v3.0/2   8/27/09 2:47p jquinlan
# pr 16362 - grain bringup.
#
# Hydra_Software_Devel/bdvd_v3.0/1   2/27/09 3:41p jquinlan
# 7635 changes.
# Hydra_Software_Devel/44   2/27/09 11:05a katrep
# PR52599:Add suppport for 7205
#
# Hydra_Software_Devel/43   1/27/09 4:40p tdo
# PR51638: Add appframework support for 7336
#
# Hydra_Software_Devel/42   1/20/09 9:17a rpan
# PR50391: Bring up HDMI output.
#
# Hydra_Software_Devel/41   12/19/08 10:56a katrep
# PR50567: Add support 974666 platfrom
#
# Hydra_Software_Devel/40   12/3/08 9:25a tdo
# PR48867: Update 97420 platform naming
#
# Hydra_Software_Devel/39   11/11/08 1:43p tdo
# PR48867: Update 97420 Platform naming
#
# Hydra_Software_Devel/38   9/5/08 4:25p tdo
# PR46484: Bringup appframework for 7420
#
# Hydra_Software_Devel/37   7/14/08 1:30p rpan
# PR36501: Enabled I2C support for 93539.
#
# Hydra_Software_Devel/36   7/7/08 6:13p tdo
# PR33618: Bring-up of Application Framework for 7601
#
# Hydra_Software_Devel/35   5/19/08 2:51p pntruong
# PR 38930: Renamed to match board name.
#
# Hydra_Software_Devel/34   1/21/08 3:40p maivu
# PR 38765: Support for 93549/56
#
# Hydra_Software_Devel/33   12/5/07 11:22a tdo
# PR36898: Add VDC PI support for 7335
#
# Hydra_Software_Devel/32   11/16/07 5:25p tdo
# PR36898: Add VDC PI support for 7335
#
# Hydra_Software_Devel/31   10/22/07 4:53p jessem
# PR 36129: Changed rule for sims to linuxclient.
#
# Hydra_Software_Devel/30   10/16/07 4:13p jessem
# PR 36129: Changed OS to linuxclient for IKOS-based emulation platform.
#
# Hydra_Software_Devel/29   9/29/07 11:40a tdo
# PR33618: Bring-up of Application Framework.
#
# Hydra_Software_Devel/28   9/26/07 1:36p pntruong
# PR34850: Bring-up of Application Framework.
#
# Hydra_Software_Devel/27   9/19/07 3:59p yuxiaz
# PR34524: Added 7325 support.
#
# Hydra_Software_Devel/26   7/31/07 1:45p darnstein
# PR33560: Assert FRAMEWORK_DO_FPGA for 7403 platform.
#
# Hydra_Software_Devel/25   6/7/07 12:05p yuxiaz
# PR29449: No conventional FPGA for 7405.
#
# Hydra_Software_Devel/24   4/13/07 2:28p yuxiaz
# PR 29449: Added 7405 support.
#
# Hydra_Software_Devel/23   3/31/07 12:15p shyam
# PR 29285 : Added linuxclient support for simulations/emulation
#
# Hydra_Software_Devel/22   2/7/07 1:24p jasonh
# PR 27117: Updating application framework to use module specific .inc
# files.
#
# Hydra_Software_Devel/21   12/19/06 4:06p hongtaoz
# PR25668: added 7403 support;
#
# Hydra_Software_Devel/20   11/15/06 7:25p hongtaoz
# PR25668: add appframework support for 7403;
#
# Hydra_Software_Devel/19   10/16/06 11:11a albertl
# PR23117:   Enabled I2C for 7440.
#
# Hydra_Software_Devel/18   9/27/06 11:52a maivu
# PR 24590: Added BCHP_VER_LOWER
#
# Hydra_Software_Devel/17   9/20/06 10:21a mward
# PR23330: Add support for 7118 chip 97118 board to xvd_vdc_test.
#
# Hydra_Software_Devel/SanDiego_97118_Bringup/2   8/24/06 3:00p mward
# PR21943:  disable FPGA for now - not programmed
#
# Hydra_Software_Devel/SanDiego_97118_Bringup/1   8/22/06 10:10a mward
# PR21943:  stubs and modifications for advance testing of build system
# for 97118
#
# Hydra_Software_Devel/16   8/8/06 12:43p pntruong
# PR22572: Build i2c base on FRAMEWORK_DO_I2C.
#
# Hydra_Software_Devel/15   8/7/06 2:15p pntruong
# PR22572: Need initial 93563 appframework bringup.
#
# Hydra_Software_Devel/14   8/3/06 1:16p syang
# PR 22212: turns on fpga and i2c for 7401
#
# Hydra_Software_Devel/13   8/2/06 12:33p syang
# PR 22212: use ?= to define env var so that user can override at make
# time with option
#
# Hydra_Software_Devel/12   4/20/06 3:23p erickson
# PR19738: 97435 merge
#
# Hydra_Software_Devel/11   4/13/06 2:35p syang
# PR 20748: added def for FRAMEWORK_DO_FPGA / I2C
#
# Hydra_Software_Devel/10   4/10/06 5:57p syang
# PR 20748: fix 97398 compile problem
#
# Hydra_Software_Devel/9   3/14/06 4:28p maivu
# PR 19673: Fix to support platforms with 7411, and non-7411. Builts now
# for 97438
#
# Hydra_Software_Devel/8   3/3/06 1:53p jasonh
# PR 20002: Changed makefiles to work better with 7411 versions being
# defined for non 7411 platforms.
#
# Hydra_Software_Devel/7   2/23/06 3:15p jasonh
# PR 19565: Added error checking for missing platforms and required
# chipsets.
#
# Hydra_Software_Devel/6   2/14/06 1:34p jasonh
# PR 19565: Added support for 7118, 7438, and 7440
#
# Hydra_Software_Devel/5   2/10/06 2:44p maivu
# PR 19599: Add 7411 support for 97398
#
# Hydra_Software_Devel/4   12/9/05 1:00p jasonh
# PR 18522: Updating with 7400 defines
#
# Hydra_Software_Devel/3   11/17/05 5:36p jasonh
# PR 18185: Adding support for 97398 platform
#
# Hydra_Software_Devel/2   6/28/05 3:50p jasonh
# PR 16012: Added 97401 support
#
# Hydra_Software_Devel/1   3/17/05 11:21a jasonh
# PR 13974: Added calculation of BCHP_CHIP include file.
#
#############################################################################

#############################################################################
# The purpose of this makefile is to determine the value of BCHP_CHIP based
# on the values from the plat script

# Simulation/Emulation
ifeq (${SYSTEM},linuxemu)
FRAMEWORK_DO_FPGA = 0
B_REFSW_OS = linuxemu
endif

ifeq (${SYSTEM},linuxpal)
FRAMEWORK_DO_FPGA = 0
FRAMEWORK_DO_I2C  = 0
B_REFSW_OS = linuxpal
endif

# 93560
ifeq (${PLATFORM},93560)
BCHP_CHIP = 3560
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 0
endif

# 93563
ifeq (${PLATFORM},93563)
BCHP_CHIP = 3563
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 0
endif

# 935233
ifeq (${PLATFORM},935233)
BCHP_CHIP = 35233
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  = 0
endif

# 935230
ifeq (${PLATFORM},935230)
BCHP_CHIP = 35230
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  = 0
endif

# 935125
ifeq (${PLATFORM},935125)
BCHP_CHIP = 35125
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  = 0
endif

# 93549
ifeq (${PLATFORM},93549)
BCHP_CHIP = 3548
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 93556
ifeq (${PLATFORM},93556)
BCHP_CHIP = 3556
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 0
endif

# 93573
ifeq (${PLATFORM},93573)
BCHP_CHIP = 3573
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 0
endif

# 97038
ifeq (${PLATFORM},97038)
BCHP_CHIP = 7038
FRAMEWORK_DO_FPGA ?= 1
FRAMEWORK_DO_I2C  ?= 1
endif

# 97118
ifeq (${PLATFORM},97118)
BCHP_CHIP = 7118
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97398
ifeq (${PLATFORM},97398)
BCHP_CHIP = 7038
ifeq (${BCHP_7411_VER},)
$(error BCHP_7411_VER undefined for PLATFORM=${PLATFORM})
endif
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 0
endif

# 97400
ifeq (${PLATFORM},97400)
BCHP_CHIP = 7400
FRAMEWORK_DO_FPGA ?= 1
FRAMEWORK_DO_I2C  ?= 1
endif

# 97401
ifeq (${PLATFORM},97401)
BCHP_CHIP = 7401
FRAMEWORK_DO_FPGA ?= 1
FRAMEWORK_DO_I2C  ?= 1
endif

# 97403
ifeq (${PLATFORM},97403)
BCHP_CHIP = 7403
FRAMEWORK_DO_FPGA ?= 1
FRAMEWORK_DO_I2C  ?= 1
endif

# 97405
ifeq (${PLATFORM},97405)
BCHP_CHIP = 7405
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97466
ifeq (${PLATFORM},97466)
BCHP_CHIP = 7405
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97205
ifeq (${PLATFORM},97205)
BCHP_CHIP = 7405
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif


# add 97435C by gqli 06/11/2006
ifeq (${PLATFORM},97435)
BCHP_CHIP = 7401
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif


# 97438
ifeq (${PLATFORM},97438)
BCHP_CHIP = 7438
ifeq (${BCHP_7411_VER},)
$(error BCHP_7411_VER undefined for PLATFORM=${PLATFORM})
endif
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 0
endif

# 97440
ifeq (${PLATFORM},97440)
BCHP_CHIP = 7440
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97601
ifeq (${PLATFORM},97601)
BCHP_CHIP = 7601
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 0
endif

# 97630
ifeq (${PLATFORM},97630)
BCHP_CHIP = 7630
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 0
endif

# 97640
ifeq (${PLATFORM},97640)
BCHP_CHIP = 7640
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97635
ifeq (${PLATFORM},97635)
BCHP_CHIP = 7635
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 0
endif

# 97325
ifeq (${PLATFORM},97325)
BCHP_CHIP = 7325
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97335
ifeq (${PLATFORM},97335)
BCHP_CHIP = 7335
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97340
ifeq (${PLATFORM},97340)
BCHP_CHIP = 7340
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97342
ifeq (${PLATFORM},97342)
BCHP_CHIP = 7342
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97336
ifeq (${PLATFORM},97336)
BCHP_CHIP = 7336
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97420
ifeq (${PLATFORM},97420)
BCHP_CHIP = 7420
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97422
ifeq (${PLATFORM},97422)
BCHP_CHIP = 7422
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97344
ifeq (${PLATFORM},97344)
BCHP_CHIP = 7344
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97346
ifeq (${PLATFORM},97346)
BCHP_CHIP = 7346
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97425
ifeq (${PLATFORM},97425)
BCHP_CHIP = 7425
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97435
ifeq (${PLATFORM},97435)
BCHP_CHIP = 7435
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97410c
ifeq (${PLATFORM},97410)
BCHP_CHIP = 7420
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97550
ifeq (${PLATFORM},97550)
BCHP_CHIP = 7550
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97125
ifeq (${PLATFORM},97125)
BCHP_CHIP = 7125
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97468
ifeq (${PLATFORM},97468)
BCHP_CHIP = 7468
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97208
ifeq (${PLATFORM},97208)
BCHP_CHIP = 7468
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97408
ifeq (${PLATFORM},97408)
BCHP_CHIP = 7408
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97019
ifeq (${PLATFORM},97019)
BCHP_CHIP = 7125
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97358
ifeq (${PLATFORM},97358)
BCHP_CHIP = 7358
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97231
ifeq (${PLATFORM},97231)
BCHP_CHIP = 7231
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97584
ifeq (${PLATFORM},97584)
BCHP_CHIP = 7584
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97552
ifeq (${PLATFORM},97552)
BCHP_CHIP = 7552
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97429
ifeq (${PLATFORM},97429)
BCHP_CHIP = 7429
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97241
ifeq (${PLATFORM},97241)
BCHP_CHIP = 7429
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97360
ifeq (${PLATFORM},97360)
BCHP_CHIP = 7360
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97563
ifeq (${PLATFORM},97563)
BCHP_CHIP = 7563
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# 97445
ifeq (${PLATFORM},97445)
BCHP_CHIP = 7445
FRAMEWORK_DO_FPGA ?= 0
FRAMEWORK_DO_I2C  ?= 1
endif

# Store B_REFSW_OS and MAGNUM for MAGNUM .inc files
# Store B_REFSW_OS and MAGNUM for MAGNUM .inc files
ifneq (${SYSTEM},linuxemu)
	ifneq (${SYSTEM},linuxpal)
		B_REFSW_OS = $(TARGET_OS)
	endif
endif
MAGNUM = $(MAGNUM_DIR)

# Store lower case version
BCHP_VER_LOWER := $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')

# Not supported?
ifeq (${BCHP_CHIP},)
$(error BCHP_CHIP undefined. Unsupported PLATFORM=${PLATFORM})
endif

# End of file
