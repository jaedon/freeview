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
# $brcm_Workfile: magnum.inc $
# $brcm_Revision: Hydra_Software_Devel/88 $
# $brcm_Date: 10/9/12 9:40a $
#
# File Description: magnum build include file
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/magnum.inc $
# 
# Hydra_Software_Devel/88   10/9/12 9:40a vanessah
# SW7445-10: add afl support for 7445
# 
# Hydra_Software_Devel/87   9/24/12 4:19p pntruong
# SW7425-3971: Adapted to new pwr directory structure changes.
#
# Hydra_Software_Devel/86   7/20/12 1:50p pntruong
# SW7445-8: Initial 7445 support.
#
# Hydra_Software_Devel/85   6/26/12 3:28p pntruong
# SW7563-4: Initial version.
#
# Hydra_Software_Devel/84   6/22/12 10:23a tdo
# SW7584-5: Temporarily remove PM for 7584 until it's fully supported.
#
# Hydra_Software_Devel/83   5/11/12 1:41p pntruong
# SW7429-170: Added initial support for 97241.  Branched of from 97429.
#
# Hydra_Software_Devel/83   5/11/12 1:40p pntruong
# SW7429-170: Added initial support for 97241.  Branched of from 97429.
#
# Hydra_Software_Devel/82   4/27/12 3:17p chengs
# SW7584-5: Add appframework support for 7584.
#
# Hydra_Software_Devel/81   4/26/12 1:34p davidp
# SW7584-5: Add 7584 to platforms that use afl common utils.
#
# Hydra_Software_Devel/80   2/8/12 10:54a tdo
# SW7425-2330: Enable power management 2.0 in appframework
#
# Hydra_Software_Devel/79   2/7/12 4:44p tdo
# SW7425-2330: Enable power management 2.0 in appframework
#
# Hydra_Software_Devel/78   2/7/12 3:54p tdo
# SW7425-2330: Enable power management 2.0 in appframework
#
# Hydra_Software_Devel/77   12/20/11 3:41p davidp
# SW7360-4: Add 7360 support.
#
# Hydra_Software_Devel/76   12/15/11 2:38p bselva
# SW7360-6: Add appframework support for 7360 platform
#
# Hydra_Software_Devel/75   12/5/11 2:58p davidp
# SW7435-8: Add 7435 support, include afl module.
#
# Hydra_Software_Devel/74   10/28/11 11:41a pntruong
# SW7435-9: Initial support.
#
# Hydra_Software_Devel/73   10/10/11 5:35p davidp
# SW7429-17: Add support for 7429 platform.
#
# Hydra_Software_Devel/72   10/6/11 2:04p pntruong
# SW7429-16: Initial support.
#
# Hydra_Software_Devel/71   4/13/11 1:31a hongtaoz
# SW7425-340: add conditional compile for rdc, gio;
#
# Hydra_Software_Devel/70   3/21/11 1:50p davidp
# SWDTV-5972: Include magnum/commonutils/alf directory for 35233
# platform.
#
# Hydra_Software_Devel/69   3/16/11 5:02p pntruong
# SW7425-164: Default B_REFSW_DEBUG=y if unset in framework build system
# so not every single app needs to specify.
#
# Hydra_Software_Devel/68   3/15/11 3:57p darnstein
# SW7425-164: Adapt to changes in the "plat" script.
# - Change DEBUG to B_REFSW_DEBUG.
# - Change ARCH to B_REFSW_ARCH.
# - Change OS to B_REFSW_OS.
# - Change mips-linux-uclibc to mips-linux.
# - Change mipsel-linux-uclibc to mipsel-linux.
#
# Hydra_Software_Devel/67   3/14/11 7:08p jhaberf
# SW35330-13: replaced 35330 support with 35233 support
#
# Hydra_Software_Devel/66   1/24/11 5:38p davidp
# SWBLURAY-23545: Add 7640 to modules that need AFL module.
#
# Hydra_Software_Devel/65   1/17/11 10:51a etrudeau
# SWBLURAY-23675: add 7630 to exclude chip list for PWR PI and add RDF
# for 7630 and 7640
#
# Hydra_Software_Devel/65   1/17/11 7:45a etrudeau
# SWBLURAY-23675: remove 7640 from PWR chip list and add RDF for 7640
#
# Hydra_Software_Devel/64   12/17/10 9:28a davidp
# SW7552-11: Add 7552 platform.
#
# Hydra_Software_Devel/63   12/10/10 9:47a etrudeau
# SWBLURAY-23579: remove dependency on PWR for 7640 bringup
#
# Hydra_Software_Devel/63   12/10/10 9:46a etrudeau
# SWBLURAY-23579: remove dependency on PWR for 7640 bringup
#
# Hydra_Software_Devel/62   11/19/10 6:43p davidp
# SW7344-5: Add 7344 and 7344 platforms to include commonutils/afl/.
#
# Hydra_Software_Devel/61   11/11/10 7:01p darnstein
# SW7344-7: typo in previous check-in.
#
# Hydra_Software_Devel/60   11/11/10 6:53p darnstein
# SW7344-7: first cut at 7344 and 7346 support.
#
# Hydra_Software_Devel/59   11/9/10 4:34p jhaberf
# SW35125-1: Added 35125 DTV chip support
#
# Hydra_Software_Devel/58   11/1/10 6:39p xhuang
# SW7552-4: remove pwr dependence
#
# Hydra_Software_Devel/57   9/10/10 5:57p davidp
# SW7358-11: Add afl commonutils for platforms that have an AVD Rev K
# core
#
# Hydra_Software_Devel/56   8/27/10 5:11p vanessah
# SW7422-12:  Remove pwr dependency
#
# Hydra_Software_Devel/55   8/27/10 1:38p darnstein
# SW3548-3022: merge to production branch.
#
# Hydra_Software_Devel/SW3548-3022/1   8/26/10 5:33p darnstein
# SW3548-3022: compile the new BUDP software module.
#
# Hydra_Software_Devel/54   5/12/10 1:48p pntruong
# SW7400-2418: remove lagacy platforms from the list of pwr.
#
# Hydra_Software_Devel/53   3/26/10 2:07p paultsai
# SW35230-43: Fix 35230 compile
#
# Hydra_Software_Devel/52   3/18/10 2:53p tdo
# SW7400-2418: pwr is not available for 7410 yet
#
# Hydra_Software_Devel/51   3/11/10 4:06p pntruong
# SW7400-2418: 7340/7342 pwr are not yet available.
#
# Hydra_Software_Devel/50   3/11/10 1:48p tdo
# SW7400-2418: AppFramework: add calls to BPWR_Open/BPWR_Close before
# calling BCHP_Open
#
# Hydra_Software_Devel/49   2/26/10 11:37a nilesh
# SW7405-2993: Added XDM back
#
# Hydra_Software_Devel/48   2/25/10 4:52p darnstein
# SW7405-2993: Back out XDM. It was causing builds to fail.
#
# Hydra_Software_Devel/47   2/25/10 4:26p nilesh
# SW7405-2993: XDM Merge
#
# Hydra_Software_Devel/SW7405-2993/1   11/20/09 4:46p nilesh
# SW7405-2993: Added XDM commonutil
#
# Hydra_Software_Devel/46   10/23/09 10:48a pntruong
# SW7340-66: Add endianness support in appframework build system.
#
# Hydra_Software_Devel/45   9/1/09 3:13p yuxiaz
# SW7550-28: Add GIO pinmux support for 7125, 7342 and 7550.
#
# Hydra_Software_Devel/44   8/27/09 12:04a pntruong
# SW7550-28: Make it more portability.
#
# Hydra_Software_Devel/43   12/2/08 5:41p tdo
# PR46484: Bringup vdc_test
#
# Hydra_Software_Devel/42   11/25/08 1:01p jessem
# PR 49655: Added rsp to IKOS build list
#
# Hydra_Software_Devel/41   11/11/08 1:48p tdo
# PR48867: Update 97420 Platform naming
#
# Hydra_Software_Devel/40   9/5/08 4:25p tdo
# PR46484: Bringup appframework for 7420
#
# Hydra_Software_Devel/39   6/20/08 10:51a jessem
# PR 43759: Added vdb to commonutils.
#
# Hydra_Software_Devel/38   3/5/08 4:55p pntruong
# PR40247: Bring-up of Application Framework.
#
# Hydra_Software_Devel/37   2/25/08 10:39p albertl
# PR34853:  Added 3548 MRC support.
#
# Hydra_Software_Devel/36   1/25/08 9:50a maivu
# PR 34853: Stub out MRC for now, so we can build 93548/56
#
# Hydra_Software_Devel/35   10/22/07 4:49p jessem
# PR 36129: Changed rule for sims to linuxclient.
#
# Hydra_Software_Devel/34   10/16/07 4:25p jessem
# PR 36129: Fixed endif bug.
#
# Hydra_Software_Devel/33   10/16/07 4:11p jessem
# PR 36129: Modified IKOS-specific support.
#
# Hydra_Software_Devel/32   10/5/07 6:41p pntruong
# PR34850: Bring-up of Application Framework.  stubbed out mrc for simple
# vdc build.
#
# Hydra_Software_Devel/31   9/29/07 11:39a tdo
# PR33618: Bring-up of Application Framework.
#
# Hydra_Software_Devel/30   3/31/07 11:30a shyam
# PR 29285 : Added linuxclient support for simulations/emulation
#
# Hydra_Software_Devel/29   3/29/07 11:09p rgreen
# PR27181: Add commmonutil acm (Audio Common Module) to appframework
#
# Hydra_Software_Devel/28   2/7/07 1:30p jasonh
# PR 27117: Updating to use module specific .inc files.
#
# Hydra_Software_Devel/27   10/13/06 8:16p albertl
# PR23111:  Added mrc for 7440.
#
# Hydra_Software_Devel/26   9/22/06 11:11a syang
# PR 23932: change to #define LINUX in each app's Makefile (most of them
# already implemented this way)
#
# Hydra_Software_Devel/25   9/21/06 1:48p syang
# PR 23932: pass LINUX from env variable to C code macro #define
#
# Hydra_Software_Devel/24   8/18/06 5:00p albertl
# PR23117:  Added 7440 support.
#
# Hydra_Software_Devel/23   8/8/06 11:59a pntruong
# PR22572: Need initial 93563 appframework bringup.  No need to compile
# i2c when FRAMEWORK_DO_I2C is defined to 0.
#
# Hydra_Software_Devel/22   6/19/06 5:05p maivu
# PR 22212: Backup previous i2c check-ins, since it broke 97398 built.
#
# Hydra_Software_Devel/21   6/13/06 4:44p syang
# PR 21689: correct typo with the usage of #if
#
# Hydra_Software_Devel/19   6/13/06 3:00p syang
# PR 21689: not include i2c if the chip doesn't support
#
# Hydra_Software_Devel/18   5/10/06 3:31p jessem
# PR 17530: Added tmr to generic BASEMODULES. Removed addition of tmr to
# chip specific BASEMODULES.
#
# Hydra_Software_Devel/17   4/25/06 10:22a maivu
# PR 20485: Added tmr for 7411
#
# Hydra_Software_Devel/16   4/13/06 2:43p syang
# PR 20748:  xfer FRAMEWORK_DO_FPGA / I2C from env of make to macro of
# *.h file
#
# Hydra_Software_Devel/15   4/10/06 10:46a jessem
# PR18367: Added VxWorks support.
#
# Hydra_Software_Devel/14   3/21/06 10:27a jasonh
# PR 20002: Updated with ARC and IMG modules for 7411 based platforms.
#
# Hydra_Software_Devel/13   3/3/06 1:52p jasonh
# PR 20002: Changed makefiles to work better with 7411 versions being
# defined for non 7411 platforms.
#
# Hydra_Software_Devel/12   2/14/06 1:31p jasonh
# PR 19565: Added support for 7118. Moved from REV to VER.
#
# Hydra_Software_Devel/11   2/10/06 5:00p maivu
# PR 19599: Fixed errors with prevous check-in for non-97398 builts
#
# Hydra_Software_Devel/10   2/8/06 8:11p hongtaoz
# PR19082: bring up xvd_vdc_test for 7400;
#
# Hydra_Software_Devel/9   1/13/06 12:31p jgarrett
# PR 18997: Creating directories for 7400
#
# Hydra_Software_Devel/Refsw_Devel_7400_A0/1   1/12/06 2:59p jgarrett
# PR 18997: Creating directories for BCM7400
#
# Hydra_Software_Devel/8   12/8/05 6:51p hongtaoz
# PR18233: add Mosaic mode support (single window working);
#
# Hydra_Software_Devel/7   10/7/05 4:41p hongtaoz
# PR17460: added MRC module;
#
# Hydra_Software_Devel/6   9/14/05 5:48p jasonh
# PR 17148: Added support to makefile for new defines
#
# Hydra_Software_Devel/5   8/31/05 5:22p jasonh
# PR 16943: Added support for MAGNUM_MODULE_DIR
#
# Hydra_Software_Devel/4   6/30/05 12:07p jasonh
# PR 16012: Cleaned up comments.
#
# Hydra_Software_Devel/3   3/17/05 10:35a jasonh
# PR 13974: Adding include of plat.inc to get BCHP_CHIP value
#
# Hydra_Software_Devel/2   3/17/05 9:50a jasonh
# PR 13974: Removing non-existant module.
#
# Hydra_Software_Devel/1   1/19/05 4:26p jasonh
# PR 13863: Upgraded version of application framework.
#
# Hydra_Software_Devel/2   12/16/03 1:13p frusso
# PR8916:  Removed vpath, added BFRAMEWORK_CFLAGS
#
# Hydra_Software_Devel/1   12/9/03 2:34p frusso
# PR8916: initial checkin
#
#############################################################################

#############################################################################
# The purpose of this makefile is to add object files, include paths,
# and C-flags required to build magnum base modules, common utilities, and
# "lower" porting interfaces.
#
# The following defines are the result:
#  MAGNUM_SOURCES
#  MAGNUM_INCLUDES
#  MAGNUM_CFLAGS
#
# This file also defines the location for all the magnum modules
#  MAGNUM_MODULE_DIR

#############################################################################
# Store directory containing the module directory (MAGNUM_MODULE_DIR)
MAGNUM_MODULE_DIR = $(FRAMEWORK_DIR)/build/modules/magnum/

#############################################################################
# Get appropriate value for BCHP_CHIP
include ${FRAMEWORK_DIR}/build/plat.inc

FRAMEWORK_DO_PM ?= n
ifeq (${FRAMEWORK_DO_PM},y)
ifneq ($(findstring $(PLATFORM), 97425 97420 97408 97125 97231 97468 97344 97346 97358 97552 97360),)
BCHP_PWR_SUPPORT=y
BXVD_POWER_MANAGEMENT=y
endif
endif

#############################################################################
# basemodules	(add to MAGNUM_SOURCES and MAGNUM_INCLUDES)

BASEMODULES = err std dbg kni mem chp int reg tmr

include $(foreach module, ${BASEMODULES}, ${MAGNUM_DIR}/basemodules/${module}/b${module}.inc)

# hack to get around test directory name
include ${MAGNUM_DIR}/basemodules/test/btst.inc

#############################################################################
# commonutils (add to MAGNUM_SOURCES and MAGNUM_INCLUDES)

COMMONUTILS = avc fmt img lst mth pxl sur acm rsp xdm udp

ifneq ($(PLATFORM), 97443)
	COMMONUTILS += mrc
endif

ifneq ($(BCHP_7411_VER),)
	COMMONUTILS += arc
endif

ifeq ($(BVDB_SUPPORT),1)
	COMMONUTILS += vdb
endif

ifneq ($(BRDC_SUPPORT),0)
	COMMONUTILS += rdc
endif

ifeq ($(findstring $(BCHP_CHIP), 35233 7231 7135 7344 7346 7358 7360 7422 7425 7435 7445 7429 7552 7584 7640 7563), $(BCHP_CHIP))
	COMMONUTILS += afl
endif

ifeq ($(findstring $(BCHP_CHIP), 7630 7640), $(BCHP_CHIP))
	COMMONUTILS += rdf
endif

include $(foreach module, ${COMMONUTILS}, ${MAGNUM_DIR}/commonutils/${module}/b${module}.inc)

#############################################################################
# "lower" porting interfaces (add to MAGNUM_SOURCES and MAGNUM_INCLUDES)

ifneq ($(BGIO_SUPPORT),0)
LOWERPIS  = gio
endif

ifneq ($(FRAMEWORK_DO_I2C),0)
LOWERPIS += i2c
endif

#############################################################################
# does not have pwr but already in dedicate branch.
# 93560 93563 97038 97118 97398 97438
ifeq ($(findstring $(PLATFORM), 97550 97435 97241 97429 97205 97208 97408 97410 97440 97466 97468 97661 97340 97342 935230 935125 935233 97422 97425 97420 97125 97231 97344 97346 97358 97552 97630 97640 97360 97584 97563 97445),)
CFLAGS += -DBPWR_SUPPORT
BPWR_TEST_PORT=1
LOWERPIS += pwr
endif

include $(foreach module, ${LOWERPIS}, ${MAGNUM_DIR}/portinginterface/${module}/b${module}.inc)

#############################################################################
# defines required to build magnum
# PLATFORM_DEFINES is a value local to this makefile.

PLATFORM_DEFINES = \
	BCHP_CHIP=$(BCHP_CHIP) \
	BCHP_REV=$(BCHP_VER) \
	BCHP_VER=BCHP_VER_$(BCHP_VER) \
	FRAMEWORK_PLATFORM=$(PLATFORM) \
	FRAMEWORK_DO_FPGA=$(FRAMEWORK_DO_FPGA) \
	FRAMEWORK_DO_I2C=$(FRAMEWORK_DO_I2C)

# Additional chipset versions
ifeq ($(PLATFORM), 97398)
PLATFORM_DEFINES += \
	BCHP_7411_VER=BCHP_VER_$(BCHP_7411_VER)
endif

ifneq ($(findstring $(B_REFSW_ARCH), mips-vxworks mips-linux),)
PLATFORM_DEFINES += BSTD_CPU_ENDIAN=BSTD_ENDIAN_BIG
else
PLATFORM_DEFINES += BSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
endif

# default is debug build
B_REFSW_DEBUG ?= y
ifeq ($(B_REFSW_DEBUG),y)
PLATFORM_DEFINES += BDBG_DEBUG_BUILD=1
endif

ifdef BCHP_QAM_CHIP
PLATFORM_DEFINES += BCHP_QAM_CHIP=$(BCHP_QAM_CHIP)
endif

ifdef BCHP_SDS_CHIP
PLATFORM_DEFINES += BCHP_SDS_CHIP=$(BCHP_SDS_CHIP)
endif

ifdef BCHP_VSB_CHIP
PLATFORM_DEFINES += BCHP_VSB_CHIP=$(BCHP_VSB_CHIP)
endif

MAGNUM_CFLAGS += $(addprefix -D, ${PLATFORM_DEFINES})

#
# End of file
#
