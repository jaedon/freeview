############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bthd.inc $
# $brcm_Revision: Hydra_Software_Devel/27 $
# $brcm_Date: 10/18/12 5:15p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/thd/bthd.inc $
# 
# Hydra_Software_Devel/27   10/18/12 5:15p atanugul
# SW3461-269: Merge to main
# 
# Hydra_Software_Devel/SW3461-252/1   9/28/12 5:03p atanugul
# SW3461-269: Add support for BTHD_GetVersionInfo()
# 
# Hydra_Software_Devel/26   9/7/12 10:33a atanugul
# SW7425-3788: HAB directory restructure
# 
# Hydra_Software_Devel/25   9/7/12 9:37a farshidf
# SW7425-3819: Remove symbolic links and flatten out THD directory
# structure
# 
# Hydra_Software_Devel/24   7/19/12 6:08p jasonh
# SW7435-263: Updating with new CHP directory structure.
# 
# Hydra_Software_Devel/23   10/18/11 5:26p atanugul
# SW3461-67: Add 3461 B0 Support
# 
# Hydra_Software_Devel/22   7/25/11 3:41p farshidf
# SWDTV-8085: enable ISDB_T compile flag all the time
# 
# Hydra_Software_Devel/21   7/15/11 3:01p vishk
# SW7552-59: Support ISDB-T in Nexus to match latest PI change No need to
# export BTHD_ISDBT_SUPPORT=y anymore for ISDBT support.
# 
# Hydra_Software_Devel/20   6/10/11 12:13p farshidf
# SW7552-36: remove the extra "\"
# 
# Hydra_Software_Devel/19   6/8/11 5:56p farshidf
# SWDTV-7471: add the new ISDBT flag and file
# 
# Hydra_Software_Devel/18   6/1/11 3:38p atanugul
# SW3461-5: Add THD support to 3461
# 
# Hydra_Software_Devel/17   5/18/11 5:21p farshidf
# SW7552-32: add THD files
# 
# Hydra_Software_Devel/16   5/16/11 4:39p farshidf
# SWDTV-7035: add "\n" at th end of the file
# 
# Hydra_Software_Devel/15   5/16/11 4:07p farshidf
# SWDTV-7035: change the ISDB-T flag
# 
# Hydra_Software_Devel/14   5/16/11 11:49a farshidf
# SWDTV-7035: add ISDB-T flag
# 
# Hydra_Software_Devel/13   4/21/11 6:38p farshidf
# SWDTV-6191: add the THD code
# 
# Hydra_Software_Devel/12   12/10/09 1:27p farshidf
# SW7550-38: remove the tune hack code
# 
# Hydra_Software_Devel/11   11/17/09 6:39p farshidf
# SW7550-38: add the Tune_hack flag
# 
# Hydra_Software_Devel/10   11/6/09 5:55p farshidf
# SW7550-38: add the 7550 files
# 
# Hydra_Software_Devel/9   3/26/09 2:35p dliu
# PR53611: Only assign BCHP_VER_UPPER if not already assigned
# 
# Hydra_Software_Devel/8   10/30/08 1:11p dliu
# PR46516: Change flag name for dynamic power management
# 
# Hydra_Software_Devel/8   10/30/08 1:09p dliu
# PR46516: Change flag name for power management
# 
# Hydra_Software_Devel/6   7/21/08 12:56p dliu
# PR38944: First step of moving code from AP to MIPS
# 
# Hydra_Software_Devel/5   6/5/08 9:01p dliu
# PR38934: Add microcode
# 
# Hydra_Software_Devel/4   5/1/08 2:04p dliu
# PR36603: Update to support 3556 and 2940 together for PVR application
#
############################################################
#
# Conditional Options for this Makefile
#
# BTHD_CHIP - Specifies a THD chip number to include in the build (e.g. 2940).  May specify multiple chips.
#

# Define this module
MAGNUM_MODULES += BTHD

# If the chip version is not specified, select a default
ifeq ($(BCHP_3461_VER),)
BCHP_3461_VER := A0
endif

# Source needs to know which revision
BTHD_DEFINES += BTHD_3461_VER=BCHP_VER_$(BCHP_3461_VER)

BCHP_VER_UPPER ?= $(shell awk 'BEGIN{print toupper("$(BCHP_VER)")}')

# Module Includes
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/include/common
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/src/common
BTHD_SOURCES +=	$(MAGNUM)/portinginterface/thd/src/common/bthd.c

#
# Handle 3461
#
ifeq ($(findstring 3461,$(BTHD_CHIP)),3461)
BTHD_INCLUDES += $(MAGNUM)/basemodules/chp/include/3461/common
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/include/3461
BTHD_INCLUDES += $(MAGNUM)/commonutils/fec/include

BTHD_SOURCES += \
	$(MAGNUM)/portinginterface/thd/src/3461/bthd_3461.c \
	$(MAGNUM)/portinginterface/thd/src/3461/bthd_3461_priv.c 
endif

#
# Handle 2940
#
ifeq ($(findstring 2940,$(BTHD_CHIP)),2940)
BTHD_INCLUDES += $(MAGNUM)/basemodules/chp/include/2940/common
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/2940

BTHD_SOURCES += \
	$(MAGNUM)/portinginterface/thd/2940/bthd_2940.c \
	$(MAGNUM)/portinginterface/thd/2940/bthd_2940_priv.c 
endif

#
# Handle 3556
#
ifeq ($(BCHP_CHIP),3556)
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/3556
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/

ifneq ($(BTHD_POWER_MANAGEMENT),n)
    BTHD_DEFINES += BTHD_POWER_MANAGEMENT
endif

BTHD_SOURCES += \
	$(MAGNUM)/portinginterface/thd/3556/bthd_3556.c \
	$(MAGNUM)/portinginterface/thd/3556/bthd_3556_priv.c \
	$(MAGNUM)/portinginterface/thd/3556/bthd_3556_ifdcoef.c \
	$(MAGNUM)/portinginterface/thd/3556/bthd_3556_ifd.c \
	$(MAGNUM)/portinginterface/thd/3556/microcode/$(BCHP_VER_UPPER)/bcm3556ap.c
endif

#
# Handle 7550
#

ifeq ($(BCHP_CHIP),7550)
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/src/7550
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/include/7550

ifneq ($(BTHD_POWER_MANAGEMENT),n)
    BTHD_DEFINES += BTHD_POWER_MANAGEMENT
endif

BTHD_SOURCES += \
	$(MAGNUM)/portinginterface/thd/src/7550/bthd_7550.c \
	$(MAGNUM)/portinginterface/thd/src/7550/bthd_7550_priv.c \
	$(MAGNUM)/portinginterface/thd/src/7550/bthd_7550_platform.c
endif



#
# Handle 35233
#

ifeq ($(BCHP_CHIP),35233)
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/35233
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/

ifneq ($(BTHD_POWER_MANAGEMENT),n)
    BTHD_DEFINES += BTHD_POWER_MANAGEMENT
endif


BTHD_DEFINES += BTHD_ISDBT_SUPPORT
BTHD_SOURCES += $(MAGNUM)/portinginterface/thd/35233/bthd_acq_isdbt.c


BTHD_SOURCES += \
	$(MAGNUM)/portinginterface/thd/35233/bthd_coef.c \
	$(MAGNUM)/portinginterface/thd/35233/bthd_3x7x.c \
	$(MAGNUM)/portinginterface/thd/35233/bthd_acq_dvbt.c \
	$(MAGNUM)/portinginterface/thd/35233/bthd_acq.c \
	$(MAGNUM)/portinginterface/thd/35233/bthd_api.c
endif

#
# Handle 7552
#

ifeq ($(BCHP_CHIP),7552)
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/src/3x7x
BTHD_INCLUDES += $(MAGNUM)/portinginterface/thd/include/3x7x

ifneq ($(BTHD_POWER_MANAGEMENT),n)
    BTHD_DEFINES += BTHD_POWER_MANAGEMENT
endif

BTHD_DEFINES += BTHD_ISDBT_SUPPORT
BTHD_SOURCES += $(MAGNUM)/portinginterface/thd/src/3x7x/bthd_acq_isdbt.c

BTHD_SOURCES += \
	$(MAGNUM)/portinginterface/thd/src/3x7x/bthd_coef.c \
	$(MAGNUM)/portinginterface/thd/src/3x7x/bthd_3x7x.c \
	$(MAGNUM)/portinginterface/thd/src/3x7x/bthd_acq_dvbt.c \
	$(MAGNUM)/portinginterface/thd/src/3x7x/bthd_acq.c \
	$(MAGNUM)/portinginterface/thd/src/3x7x/bthd_api.c
endif


