############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bchp_pwr.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 10/19/12 9:44a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/basemodules/chp/build/bchp_pwr.inc $
# 
# Hydra_Software_Devel/5   10/19/12 9:44a nickh
# SW7420-2371: testport module
# 
# Hydra_Software_Devel/4   9/14/12 12:07p hauxwell
# SW7429-297 : build is broken due to addition spaces at the end of a
# variable definition bchp_pwr.inc
# 
# Hydra_Software_Devel/3   9/12/12 3:44p gmohile
# SW7435-347 : Remove duplicate directories
# 
# Hydra_Software_Devel/2   9/7/12 5:08p gmohile
# SW7435-347 : Fix build error
# 
# Hydra_Software_Devel/1   9/7/12 3:56p gmohile
# SW7435-347 : Remove symbolic links for PWR implementation in CHP
# 
#
############################################################

# BCHP_PWR basemodule. 
ifeq ($(filter $(BCHP_CHIP), 7420 7408 7125 7231 7344 7346 7425 7358 7552 7360 7563 7468 7435 7429), $(BCHP_CHIP))

ifeq ($(BCHP_PWR_SUPPORT),y)
BCHP_DEFINES += BCHP_PWR_SUPPORT
endif

# the source files need to get included in the library unconditionally
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/common/bchp_pwr.c

ifeq ($(shell test -e $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)/pwr && echo y),y)

BPWR_CHIP_VER_LOWER := $(BCHP_VER_LOWER)

ifeq ($(BCHP_CHIP), 7231)
    ifeq ($(filter $(BCHP_VER_LOWER), b0 b1 b2), $(BCHP_VER_LOWER))
	BPWR_CHIP_VER_LOWER := b0
    else 
	BPWR_CHIP_VER_LOWER := a0
    endif
endif
ifeq ($(BCHP_CHIP), 7425)
    ifeq ($(filter $(BCHP_VER_LOWER), b0 b1 b2), $(BCHP_VER_LOWER))
	BPWR_CHIP_VER_LOWER := b0
    else 
	BPWR_CHIP_VER_LOWER := a1
    endif
endif
ifeq ($(BCHP_CHIP), 7429)
    ifeq ($(filter $(BCHP_VER_LOWER), a0 b0), $(BCHP_VER_LOWER))
	BPWR_CHIP_VER_LOWER := a0
    endif
endif
ifeq ($(BCHP_CHIP), 7344)
    ifeq ($(filter $(BCHP_VER_LOWER), b0 b1), $(BCHP_VER_LOWER))
	BPWR_CHIP_VER_LOWER := b0
    else 
	BPWR_CHIP_VER_LOWER := a0
    endif
endif

BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/include/$(BCHP_CHIP)/common/pwr/$(BPWR_CHIP_VER_LOWER)
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)/pwr/$(BPWR_CHIP_VER_LOWER)
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)/pwr/$(BPWR_CHIP_VER_LOWER)/bchp_pwr_resources.c
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)/pwr/$(BPWR_CHIP_VER_LOWER)/bchp_pwr_impl.c

else
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)/bchp_pwr_resources.c
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)/bchp_pwr_impl.c
endif

ifeq ($(BCHP_CHIP),7420)
ifeq ($(BPWR_TESTPORT), y)
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)/bchp_pwr_tport1.c
else
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)/bchp_pwr_tport2.c
endif
endif

endif
