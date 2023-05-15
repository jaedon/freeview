############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: baob.inc $
# $brcm_Revision: Hydra_Software_Devel/9 $
# $brcm_Date: 9/28/12 2:33p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/aob/baob.inc $
# 
# Hydra_Software_Devel/9   9/28/12 2:33p atanugul
# SW3128-228: Add support for  BAOB_GetVersionInfo()
# 
# Hydra_Software_Devel/8   7/19/12 5:53p jasonh
# SW7435-263: Updating with new CHP directory structure.
# 
# Hydra_Software_Devel/7   12/13/11 12:34p atanugul
# SW3128-76: Add "No Signal" support to BAOB_GetLockStatus()
# 
# Hydra_Software_Devel/6   11/9/11 4:49p farshidf
# SW7552-139: add AOB support
# 
# Hydra_Software_Devel/5   3/10/09 4:07p vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/4   3/4/09 8:49p vishk
# PR46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/3   3/3/09 4:09p vishk
# PR46436: AOB PI: Add support for BCM3117 and BCM3114
# 
# Hydra_Software_Devel/2   1/16/09 7:38p vishk
# PR46436: AOB PI: Add support for BCM3117 and BCM3114
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

# Define this module
MAGNUM_MODULES += BAOB

# If the chip version is not specified, select a default
ifeq ($(BCHP_312X_VER),)
BCHP_312X_VER := A0
endif

# Source needs to know which revision
BAOB_DEFINES += BAOB_312X_VER=BCHP_VER_$(BCHP_312X_VER)

# Module Includes
BAOB_INCLUDES += $(MAGNUM)/portinginterface/aob/
BAOB_INCLUDES += $(MAGNUM)/commonutils/fec/include

# Module Includes
BAOB_INCLUDES += $(MAGNUM)/portinginterface/aob/$(BAOB_CHIP)
BAOB_INCLUDES += $(MAGNUM)/basemodules/chp/include/$(BAOB_CHIP)/common     

# Module Sources
BAOB_SOURCES += $(MAGNUM)/portinginterface/aob/$(BAOB_CHIP)/baob.c
 

#
# Handle 7552
#

ifeq ($(BCHP_CHIP),7552)
BAOB_SOURCES += \
	$(MAGNUM)/portinginterface/aob/$(BAOB_CHIP)/baob_acquire.c \
	$(MAGNUM)/portinginterface/aob/$(BAOB_CHIP)/baob_status.c \
	$(MAGNUM)/portinginterface/aob/$(BAOB_CHIP)/baob_utils.c
endif
