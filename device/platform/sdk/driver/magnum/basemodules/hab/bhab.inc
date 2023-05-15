############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bhab.inc $
# $brcm_Revision: Hydra_Software_Devel/29 $
# $brcm_Date: 9/28/12 1:37p $
#
# Module Description:
#
# Revision History:
#
# Created: 
#
# $brcm_Log: /magnum/basemodules/hab/bhab.inc $
# 
# Hydra_Software_Devel/29   9/28/12 1:37p atanugul
# SW3128-228: Add support for BHAB_GetVersionInfo()
# 
# Hydra_Software_Devel/28   9/7/12 1:10p vishk
# SW7425-3788: Remove symbolic links and flatten out HAB directory
# structure for 3117/3114/3112/3109 frontends.
# 
# Hydra_Software_Devel/27   9/7/12 12:27p atanugul
# SW7425-3788: restructure HAB directory
# 
# Hydra_Software_Devel/26   9/7/12 10:33a atanugul
# SW7425-3788: HAB directory restructure
# 
# Hydra_Software_Devel/25   9/7/12 10:13a enavarro
# SW7425-3788: HAB directory restructure
# 
# Hydra_Software_Devel/24   9/7/12 9:58a atanugul
# SW7425-3788: HAB directory restructure
# 
# Hydra_Software_Devel/23   7/20/12 9:48a atanugul
# SW7435-263: Fix build error
# 
# Hydra_Software_Devel/22   7/20/12 9:24a atanugul
# SW7435-263: Fix build error
# 
# Hydra_Software_Devel/21   7/19/12 12:07p atanugul
# SW3472-8: Add EWS support to 3472; power on LEAP memories before FW
# download for 3472
# 
# Hydra_Software_Devel/20   7/9/12 5:06p atanugul
# SW3472-8: Add PI support for 3472
# 
# Hydra_Software_Devel/19   6/26/12 6:26p mphillip
# SW7425-3327: Symlink HAB RDB headers into a separate hab-specific
# directory
# 
# Hydra_Software_Devel/18   6/21/12 5:38p mphillip
# SW7425-1304: Update BHAB_INCLUDES to include the bhab rdb
# 
# Hydra_Software_Devel/17   4/10/12 3:07p atanugul
# SW3462-4: Add HAB support for 3462
# 
# Hydra_Software_Devel/16   12/16/11 12:02p atanugul
# SW3128-92: Add  FW auto detection support
# 
# Hydra_Software_Devel/15   12/1/11 4:13p mphillip
# SW7425-1304: Change 4528 to HAB
# 
# Hydra_Software_Devel/14   10/18/11 5:26p atanugul
# SW3461-67: Add 3461 B0 Support
# 
# Hydra_Software_Devel/13   10/4/11 3:53p atanugul
# SW3128-53: Add support for 3128 B0
# 
# Hydra_Software_Devel/12   8/30/11 4:20p atanugul
# SW3128-47: Add Support for BCM3123
# 
# Hydra_Software_Devel/11   8/30/11 12:09p atanugul
# SW3128-47: Add Support for BCM3123
# 
# Hydra_Software_Devel/10   3/16/11 4:29p atanugul
# SW3461-3: Add support for BCM3461
# 
# Hydra_Software_Devel/9   3/16/11 11:46a vishk
# SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
# board) plug on 7422 board
# 
# Hydra_Software_Devel/8   3/4/11 3:12p vishk
# SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
# board) plug on 7422 board
# 
# Hydra_Software_Devel/7   3/2/11 3:58p atanugul
# SW3128-2: Add Support for BCM3128
# 
# Hydra_Software_Devel/6   2/1/11 12:45p atanugul
# SW3128-2: Add Support for BCM3128
# 
# Hydra_Software_Devel/5   3/10/09 1:45p vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/4   3/5/09 7:35a vishk
# PR46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/3   3/4/09 8:48p vishk
# PR46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/2   1/16/09 7:14p vishk
# PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
# PR 46436: AOB PI: Add support for BCM3117 and BCM3114
# 
# Hydra_Software_Devel/1   10/20/08 2:52p gmohile
# PR 47386 : Checkin initial version
# 
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BHAB

# If the chip version is not specified, select a default
ifeq ($(BCHP_312X_VER),)
BCHP_312X_VER := A0
endif

ifeq ($(BCHP_3461_VER),)
BCHP_3461_VER := A0
endif

ifeq ($(BCHP_3462_VER),)
BCHP_3462_VER := A0
endif

ifeq ($(BCHP_3472_VER),)
BCHP_3472_VER := A0
endif

ifeq ($(BCHP_4528_VER),)
BCHP_4528_VER := A0
endif

ifeq ($(BCHP_4517_VER),)
BCHP_4517_VER := B0
endif

ifeq ($(BCHP_4538_VER),)
BCHP_4538_VER := A0
endif

BHAB_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_312X_VER)")}')

BHAB_3461_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_3461_VER)")}')
BHAB_3462_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_3462_VER)")}')
BHAB_3472_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_3472_VER)")}')
BHAB_4528_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_4528_VER)")}')
BHAB_4538_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_4538_VER)")}')
BHAB_4517_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_4517_VER)")}')

# Source needs to know which revision
BHAB_DEFINES += BHAB_312X_VER=BCHP_VER_$(BCHP_312X_VER)
BHAB_DEFINES += BHAB_3461_VER=BCHP_VER_$(BCHP_3461_VER)

# Module Includes
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/common
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/common
BHAB_INCLUDES += $(MAGNUM)/commonutils/fec/include

ifneq ($(BHAB_3461_SUPPORT),)
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/3461/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/leapctfe/3461/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/3461/rdb/hab/$(BHAB_3461_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3461/bhab_3461.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3461/bhab_3461_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3461/bhab_3461_fw_$(BHAB_3461_VER_LOWER).c
endif

ifneq ($(BHAB_3462_SUPPORT),)
BHAB_DEFINES += BHAB_CHIP=3462
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/3462/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/leapctfe/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/3462/rdb/hab/$(BHAB_3462_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3462/bhab_3462.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/bhab_34xx_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3462/bhab_3462_fw_$(BHAB_3461_VER_LOWER).c
endif

ifneq ($(BHAB_3472_SUPPORT),)
BHAB_DEFINES += BHAB_CHIP=3472
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/3472/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/leapctfe/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/3472/rdb/hab/$(BHAB_3472_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3472/bhab_3472.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/bhab_34xx_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3472/bhab_3472_fw_$(BHAB_3461_VER_LOWER).c
endif

ifneq ($(BHAB_3128_SUPPORT),)
BHAB_312X_CHIP := 3128
BHAB_RDB_CHIP := 3128
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/3128/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/leapctfe/3128/
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3128/bhab_3128.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3128/bhab_3128_fw_c0.c
endif

ifneq ($(BHAB_3117_SUPPORT),)
BHAB_31xx_CHIP := 3117
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/8051/3117/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/8051/
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/8051/3117/bhab_3117.c
endif

ifneq ($(BHAB_3114_SUPPORT),)
BHAB_31xx_CHIP := 3114
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/8051/3114/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/8051/
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/8051/3114/bhab_3114.c
endif

ifneq ($(BHAB_3112_SUPPORT),)
BHAB_31xx_CHIP := 3112
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/8051/3112/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/8051/
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/8051/3112/bhab_3112.c
endif

ifneq ($(BHAB_3109_SUPPORT),)
BHAB_31xx_CHIP := 3109
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/8051/3109/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/8051/
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/8051/3109/bhab_3109.c
endif

ifneq ($(BHAB_4528_SUPPORT),)
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/4528/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/4528/rdb/hab/$(BHAB_4528_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4528/bhab_4528.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4528/bhab_4528_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4528/bhab_4528_fw_$(BHAB_4528_VER_LOWER).c
endif

ifneq ($(BHAB_4517_SUPPORT),)
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/4517/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/4517/rdb/hab/$(BHAB_4517_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4517/bhab_4517.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4517/bhab_4517_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4517/bhab_4517_fw_$(BHAB_4517_VER_LOWER).c
endif

ifneq ($(BHAB_4538_SUPPORT),)
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/4538/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/4538/rdb/hab/$(BHAB_4538_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4538/bhab_4538.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4538/bhab_4538_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4538/bhab_4538_fw_$(BHAB_4538_VER_LOWER).c
endif

# Module Sources
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/common/bhab.c
ifneq ($(BHAB_31xx_CHIP),)
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/$(BHAB_31xx_CHIP)/common
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/8051/
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/8051/bhab_31xx_priv.c
ifeq ($(BCHP_31XX_VER), B0)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/8051/bhab_31xx_fw_b0.c
else
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/8051/bhab_31xx_fw_a0.c
endif
endif

ifneq ($(BHAB_312X_CHIP),)
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/$(BHAB_RDB_CHIP)/rdb/hab/$(BHAB_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/$(BHAB_312X_CHIP)/bhab_312x_priv.c
endif
