############################################################
#     Copyright (c) 2005-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: btc2.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 10/18/12 5:16p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/tc2/btc2.inc $
# 
# Hydra_Software_Devel/5   10/18/12 5:16p atanugul
# SW3461-269: Merge to main
# 
# Hydra_Software_Devel/SW3461-252/1   9/28/12 5:06p atanugul
# SW3461-269: Add support for BTC2_GetVersionInfo()
# 
# Hydra_Software_Devel/4   9/7/12 10:33a atanugul
# SW7425-3788: HAB directory restructure
# 
# Hydra_Software_Devel/3   10/18/11 5:26p atanugul
# SW3461-67: Add 3461 B0 Support
# 
# Hydra_Software_Devel/2   3/17/11 1:14p vishk
# SW7231-32: Implement the Nexus code to support the 3461 frontend chip
# 
# Hydra_Software_Devel/1   3/16/11 4:13p atanugul
# SW3461-3: Add support for BCM3461
#
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#
# BTC2_3117_SUPPORT - If defined, support will be added for the 3117 TC2 


# Define this module
MAGNUM_MODULES += BTC2

# If the chip version is not specified, select a default
ifeq ($(BCHP_3461_VER),)
BCHP_3461_VER := A0
endif

# Source needs to know which revision
BTC2_DEFINES += BTC2_3461_VER=BCHP_VER_$(BCHP_3461_VER)

# Module Includes
BTC2_INCLUDES += $(MAGNUM)/portinginterface/tc2/include
BTC2_INCLUDES += $(MAGNUM)/portinginterface/tc2/src/common
BTC2_INCLUDES += $(MAGNUM)/commonutils/fec/include

# Module Sources
BTC2_SOURCES +=	$(MAGNUM)/portinginterface/tc2/src/common/btc2.c


ifneq ($(BTC2_3461_SUPPORT),)
BTC2_INCLUDES += $(MAGNUM)/basemodules/chp/3461
BTC2_INCLUDES += $(MAGNUM)/portinginterface/tc2/src/3461
BTC2_SOURCES += \
	$(MAGNUM)/portinginterface/tc2/src/3461/btc2_3461.c \
	$(MAGNUM)/portinginterface/tc2/src/3461/btc2_3461_priv.c
endif
 
