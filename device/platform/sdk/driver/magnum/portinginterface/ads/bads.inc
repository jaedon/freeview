############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bads.inc $
# $brcm_Revision: Hydra_Software_Devel/49 $
# $brcm_Date: 10/30/12 12:43p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/ads/bads.inc $
# 
# Hydra_Software_Devel/49   10/30/12 12:43p atanugul
# SW7584-54: Add ADS PI support for 7584
# 
# Hydra_Software_Devel/48   9/28/12 2:16p atanugul
# SW3128-228: Add support for  BADS_GetVersionInfo()
# 
# Hydra_Software_Devel/47   9/7/12 1:48p vishk
# SW7425-3765, SW7425-3788: Move the firmware file to hab.
# 
# Hydra_Software_Devel/46   9/7/12 12:03p atanugul
# SW7425-3788: restructure HAB directory
# 
# Hydra_Software_Devel/45   9/7/12 10:02a atanugul
# SW7425-3788: HAB directory restructure
# 
# Hydra_Software_Devel/44   7/24/12 6:05p atanugul
# SW3128-200: Fix build error
# 
# Hydra_Software_Devel/43   7/9/12 3:05p atanugul
# SW3461-243: 3461 FW version 4.14
# 
# Hydra_Software_Devel/42   6/22/12 4:26p mward
# SW7425-2983: move 7125 ADS code
# 
# Hydra_Software_Devel/41   6/22/12 12:54p farshidf
# SW7425-2983: add 7550 and 7552 ADS code
# 
# Hydra_Software_Devel/40   6/21/12 4:24p atanugul
# SW7425-2983: Remove symbolic links and flatten out ADS and AOB
# directory structure; cleanup
# 
# Hydra_Software_Devel/39   6/21/12 3:38p vishk
# SW7425-2983: Remove symbolic links for 31xx family.
# 
# Hydra_Software_Devel/38   6/21/12 3:20p atanugul
# SW7425-2983: Remove symbolic links and flatten out ADS and AOB
# directory structure; cleanup
# 
# Hydra_Software_Devel/37   6/19/12 4:46p atanugul
# SW7425-2983: Update bads.inc based on the new ADS directory structure;
# Fix build error
# 
# Hydra_Software_Devel/36   6/18/12 6:10p atanugul
# SW7425-2983: Update bads.inc based on the new ADS directory structure
# 
# Hydra_Software_Devel/35   6/18/12 5:16p atanugul
# SW7425-2983: Remove symbolic links and flatten out ADS directory
# structure
# 
# Hydra_Software_Devel/34   6/18/12 4:52p atanugul
# SW7425-2983: Update bads.inc based on the new ADS directory structure
# 
# Hydra_Software_Devel/33   4/10/12 4:10p atanugul
# SW3462-4: Add ADSsupport for 3462
# 
# Hydra_Software_Devel/32   2/22/12 2:16p farshidf
# SW7552-217: Enable Burst mode for DS only for china based on compile
# flag
# 
# Hydra_Software_Devel/31   2/22/12 2:11p farshidf
# SW7552-217: Enable Burst mode for DS only for china based on compile
# flag
# 
# Hydra_Software_Devel/30   12/16/11 12:02p atanugul
# SW3128-92: Add  FW auto detection support
# 
# Hydra_Software_Devel/29   11/8/11 5:30p atanugul
# SW3128-64: Add 3128 B0 Support
# 
# Hydra_Software_Devel/28   10/18/11 5:26p atanugul
# SW3461-67: Add 3461 B0 Support
# 
# Hydra_Software_Devel/27   10/4/11 3:53p atanugul
# SW3128-53: Add support for 3128 B0
# 
# Hydra_Software_Devel/26   8/31/11 4:21p atanugul
# SW3128-46: Add support for BCM3124
# 
# Hydra_Software_Devel/25   8/30/11 11:18a atanugul
# SW3128-47: Add Support for BCM3123
# 
# Hydra_Software_Devel/24   6/7/11 3:44p farshidf
# SW7552-36: add new file
# 
# Hydra_Software_Devel/23   6/7/11 3:09p farshidf
# SW7552-36: add the new coef file
# 
# Hydra_Software_Devel/22   5/18/11 5:07p farshidf
# SW7552-31: add the DS files
# 
# Hydra_Software_Devel/21   4/20/11 1:31p farshidf
# SWDTV-6190: add the 35233 files
# 
# Hydra_Software_Devel/20   3/16/11 4:23p atanugul
# SW3461-3: Add support for BCM3461
# 
# Hydra_Software_Devel/19   3/16/11 11:50a vishk
# SW7231-76 : Add support for FBTSFF 97231 + 3128 board.
# 
# Hydra_Software_Devel/18   3/4/11 3:17p vishk
# SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
# board) plug on 7422 board
# 
# Hydra_Software_Devel/17   3/2/11 3:58p atanugul
# SW3128-2: Add Support for BCM3128
# 
# Hydra_Software_Devel/16   2/1/11 2:14p atanugul
# SW3128-2: Add Support for BCM3128
# 
# Hydra_Software_Devel/SW7125-35/3   8/24/10 1:43p mward
# SW7125-35: Merge changes from main.
# 
# Hydra_Software_Devel/SW7125-35/2   7/26/10 9:57a mward
# SW7125-513:  Add NEXUS_FRONTEND_7125, default y, for host control of
# 7125 tuner/ds.
# 
# Hydra_Software_Devel/SW7125-35/SW7125-513/1   7/12/10 4:41p mward
# SW7125-513:  Add NEXUS_FRONTEND_7125, default y, for host control of
# 7125 tuner/ds.
# 
# Hydra_Software_Devel/SW7125-35/1   10/14/09 4:05p mward
# SW7125-35:  Allow 3255 and 31xx in same platform.
# 
# Hydra_Software_Devel/8   8/11/09 4:51p vishk
# PR 57480: Bcm31xx: Add support Bcm31xx B0 chips
# 
# Hydra_Software_Devel/7   6/19/09 11:56a vishk
# PR 56159: bcm31xxap.c and bcm31xxap.h to be moved to ads pi folder from
# nexus
# 
# Hydra_Software_Devel/6   3/10/09 3:40p vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/5   2/15/09 9:57a vishk
# PR46431: ADS PI, Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/4   2/12/09 3:55p vishk
# PR46431: ADS PI, Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/2   1/29/09 7:26p vishk
# PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
# PR 46436: AOB PI: Add support for BCM3117 and BCM3114
# 
# Hydra_Software_Devel/1   1/11/07 2:40p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#
# BADS_3117_SUPPORT - If defined, support will be added for the 3117 ADS 
# BADS_3109_SUPPORT - If defined, support will be added for the 3109 ADS

# Define this module
MAGNUM_MODULES += BADS

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

BADS_312x_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_312X_VER)")}')
BADS_3461_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_3461_VER)")}')
BADS_3462_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_3462_VER)")}')

# Source needs to know which revision
BADS_DEFINES += BADS_312X_VER=BCHP_VER_$(BCHP_312X_VER)
BADS_DEFINES += BADS_3461_VER=BCHP_VER_$(BCHP_3461_VER)

# Module Includes
BADS_INCLUDES += $(MAGNUM)/portinginterface/ads/include
BADS_INCLUDES += $(MAGNUM)/portinginterface/ads/src/common
BADS_INCLUDES += $(MAGNUM)/commonutils/fec/include

# Module Sources
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/common/bads.c

ifneq ($(BADS_3461_SUPPORT),)
BADS_DEFINES += BADS_CHIP=3461
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/leap/bads_leap_priv.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/leap/3461/bads_3461.c
endif

ifneq ($(BADS_3462_SUPPORT),)
BADS_DEFINES += BADS_CHIP=3462
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/leap/bads_leap_priv.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/leap/3462/bads_3462.c
endif

ifneq ($(BADS_3128_SUPPORT),)
BADS_DEFINES += BADS_3128_SUPPORT
BADS_DEFINES += BADS_CHIP=3128
BADS_312X_CHIP := 3128
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/leap/bads_leap_priv.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/leap/3128/bads_3128.c
endif

ifneq ($(BADS_7584_SUPPORT),)
BADS_DEFINES += BADS_7584_SUPPORT
BADS_DEFINES += BADS_CHIP=7584
BADS_312X_CHIP := 7584
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/leap/bads_leap_priv.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/leap/7584/bads_7584.c
endif

ifneq ($(BADS_3109_SUPPORT),)
BADS_31XX_CHIP := 3109
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/8051/3109/bads_3109.c
endif

ifneq ($(BADS_3112_SUPPORT),)
BADS_31XX_CHIP := 3112
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/8051/3112/bads_3112.c
endif
 
ifneq ($(BADS_3114_SUPPORT),)
BADS_31XX_CHIP := 3114
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/8051/3114/bads_3114.c
endif
 
ifneq ($(BADS_3117_SUPPORT),)
BADS_31XX_CHIP := 3117
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/8051/3117/bads_3117.c
endif

ifneq ($(BADS_31XX_CHIP),)
BADS_INCLUDES += $(MAGNUM)/portinginterface/ads/include/
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/8051/bads_31xx_priv.c
endif

ifneq ($(BADS_7550_SUPPORT),)
BADS_INCLUDES += $(MAGNUM)/portinginterface/ads/src/soc/
BADS_INCLUDES += $(MAGNUM)/portinginterface/ads/src/soc/7550/
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7550/bads_7550_api.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7550/bads_7550.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7550/bads_7550_acquire.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7550/bads_7550_status.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7550/bads_7550_utils.c
endif



ifneq ($(BADS_35233_SUPPORT),)
BADS_INCLUDES += $(MAGNUM)/portinginterface/ads/35233/
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/7552/bads_coef.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/35233/bads_3x7x.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/35233/bads_acquire.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/35233/bads_status.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/35233/bads_utils.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/35233/bads_api.c
endif

ifneq ($(BADS_7552_SUPPORT),)
ifneq ($(BADS_7552_ANNEXA_BURST_SUPPORT),)
BADS_DEFINES += BADS_ANNEXA_BURST_SUPPORT
endif
BADS_INCLUDES += $(MAGNUM)/portinginterface/ads/src/soc/
BADS_INCLUDES += $(MAGNUM)/portinginterface/ads/src/soc/7552/
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/bads_3x7x_priv.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7552/bads_coef.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7552/bads_7552.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7552/bads_acquire.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7552/bads_status.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7552/bads_utils.c
endif

ifneq ($(BADS_7125_SUPPORT),)
BADS_INCLUDES += $(MAGNUM)/portinginterface/ads/src/soc/7125/
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7125/bads_7125_priv.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7125/bads_7125.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7125/bads_7125_acquire.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7125/bads_7125_status.c
BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/soc/7125/bads_7125_utils.c
endif

ifeq ($(BADS_CHIP),3255)
    BADS_SOURCES += $(MAGNUM)/portinginterface/ads/src/docsis/bads_docsis_priv.c
endif
 
