############################################################
#     Copyright (c) 2005-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bods.inc $
# $brcm_Revision: Hydra_Software_Devel/8 $
# $brcm_Date: 12/4/12 4:08p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/ods/bods.inc $
# 
# Hydra_Software_Devel/8   12/4/12 4:08p atanugul
# SW3462-51: Add Support for GetVersionInfo() API
# 
# Hydra_Software_Devel/7   9/10/12 12:48p erickson
# SW7425-3788: fix compilation
# 
# Hydra_Software_Devel/6   9/7/12 10:03a atanugul
# SW7425-3788: HAB directory restructure
# 
# Hydra_Software_Devel/5   7/24/12 5:58p atanugul
# SW3472-8: Add FW support for 3472
# 
# Hydra_Software_Devel/4   7/19/12 11:33a atanugul
# SW3472-8: Add support for 3472
# 
# Hydra_Software_Devel/3   7/16/12 11:51a atanugul
# SW3472-8: Add support for 3472
# 
# Hydra_Software_Devel/2   7/9/12 5:06p atanugul
# SW3472-8: Add PI support for 3472
# 
# Hydra_Software_Devel/1   5/25/12 2:38p atanugul
# SW3462-7: Initial Version
# 
#
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#
# BODS_3462_SUPPORT - If defined, support will be added for the 3462 ODS 

# Define this module
MAGNUM_MODULES += BODS

# If the chip version is not specified, select a default
ifeq ($(BCHP_3462_VER),)
BCHP_3462_VER := A0
endif

# Source needs to know which revision
BODS_DEFINES += BODS_3462_VER=BCHP_VER_$(BCHP_3462_VER)

# Module Includes
BODS_INCLUDES += $(MAGNUM)/portinginterface/ods/include
BODS_INCLUDES += $(MAGNUM)/portinginterface/ods/src/common
BODS_INCLUDES += $(MAGNUM)/commonutils/fec/include

# Module Sources
BODS_SOURCES +=	$(MAGNUM)/portinginterface/ods/src/common/bods.c

ifneq ($(BODS_3472_SUPPORT),)
BODS_INCLUDES += $(MAGNUM)/basemodules/chp/3472
BODS_DEFINES += BODS_CHIP=3472
BODS_SOURCES += \
    $(MAGNUM)/portinginterface/ods/src/leap/bods_leap_priv.c \
    $(MAGNUM)/portinginterface/ods/src/leap/3472/bods_3472.c 
endif

ifneq ($(BODS_3462_SUPPORT),)
BODS_INCLUDES += $(MAGNUM)/basemodules/chp/3462
BODS_DEFINES += BODS_CHIP=3462
BODS_SOURCES += \
    $(MAGNUM)/portinginterface/ods/src/leap/bods_leap_priv.c \
	$(MAGNUM)/portinginterface/ods/src/leap/3462/bods_3462.c    
endif
