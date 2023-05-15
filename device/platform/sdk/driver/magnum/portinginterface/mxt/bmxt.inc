############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmxt.inc $
# $brcm_Revision: Hydra_Software_Devel/15 $
# $brcm_Date: 9/28/12 7:04p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/mxt/bmxt.inc $
# 
# Hydra_Software_Devel/15   9/28/12 7:04p jtna
# SW7425-4008: add 4538 MXT support
# 
# Hydra_Software_Devel/14   9/28/12 2:45p jtna
# SW7425-1483: refactor to use chip-specific header
# 
# Hydra_Software_Devel/13   7/19/12 4:15p jtna
# SW3472-14: add 3472 MXT support. change MTSIF in API naming to
# MTSIF_TX, in order to differentiate with MTSIF_RX
# 
# Hydra_Software_Devel/12   3/6/12 6:01p jtna
# SW7425-2574: allow BMXT to build without any RPC references
# 
# Hydra_Software_Devel/11   1/23/12 5:33p jtna
# SW7425-2125: add 4517 MXT support
# 
# Hydra_Software_Devel/10   11/2/11 11:42a jtna
# SW7425-1483: fix incorrect use of findstring()
# 
# Hydra_Software_Devel/9   10/27/11 4:50p jtna
# SW7425-1483: add 4528 support to new impl
# 
# Hydra_Software_Devel/8   10/21/11 7:28p jtna
# SW7425-1483: enable new chip-specific mxt implentation
# 
# Hydra_Software_Devel/7   10/4/11 2:08p jtna
# SW7425-1365: do not include /magnum/basemodules/chp/3128/rdb/xx/ to
# header file search path
# 
# Hydra_Software_Devel/6   9/28/11 4:47p jtna
# SW7425-1303: add 4528 support
# 
# Hydra_Software_Devel/5   9/28/11 3:42p jtna
# SW7425-1365: refactor to allow support for other chips
# 
# Hydra_Software_Devel/4   9/13/11 12:28p jtna
# SW7425-1238: update 3383 support
# 
# Hydra_Software_Devel/3   9/12/11 4:48p jtna
# SW7425-1238: add 3383 support
# 
# Hydra_Software_Devel/2   8/26/11 5:52p jtna
# SW7425-1202: general clean-up
# 
############################################################

# Define this module
MAGNUM_MODULES += BMXT

BMXT_INCLUDES += $(MAGNUM)/portinginterface/mxt
BMXT_INCLUDES += $(MAGNUM)/basemodules/hab
BMXT_INCLUDES += $(MAGNUM)/basemodules/rpc
BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/bmxt.c

# be wary of doing e.g.
# BMXT_INCLUDES += $(MAGNUM)/basemodules/chp/include/3128/rdb/$(BMXT_3128_VER_LOWER)
# BMXT_INCLUDES appears earlier than BCHP_INCLUDES and ends up forcing include files like "bchp_gio.h" from 3128, not 7425

ifeq ($(BMXT_3128_SUPPORT), y)
    BMXT_DEFINES += BMXT_3128_SUPPORT
    BMXT_INCLUDES += $(MAGNUM)/portinginterface/mxt/3128
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/3128/bmxt_3128.c
    ifeq ($(findstring bmxt_3x4x.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/3128/bmxt_3x4x.c
    endif
endif

ifeq ($(BMXT_3383_SUPPORT), y)
    BMXT_DEFINES += BMXT_3383_SUPPORT
    # 3383 RDB taken from 3128
    BMXT_INCLUDES += $(MAGNUM)/portinginterface/mxt/3128
    BMXT_INCLUDES += $(MAGNUM)/portinginterface/mxt/3383
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/3383/bmxt_3383.c
    ifeq ($(findstring bmxt_3x4x.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/3383/bmxt_3x4x.c
    endif
endif

ifeq ($(BMXT_4528_SUPPORT), y)
    BMXT_DEFINES += BMXT_4528_SUPPORT
    BMXT_INCLUDES += $(MAGNUM)/portinginterface/mxt/4528
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/4528/bmxt_4528.c
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/4528/bmxt_3x4x.c
endif

ifeq ($(BMXT_4517_SUPPORT), y)
    BMXT_DEFINES += BMXT_4517_SUPPORT
    BMXT_INCLUDES += $(MAGNUM)/portinginterface/mxt/4517
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/4517/bmxt_4517.c
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/4517/bmxt_3x4x.c
endif

ifeq ($(BMXT_3472_SUPPORT), y)
    BMXT_DEFINES += BMXT_3472_SUPPORT
    BMXT_INCLUDES += $(MAGNUM)/portinginterface/mxt/3472
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/3472/bmxt_3472.c
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/3472/bmxt_3x4x.c
endif

ifeq ($(BMXT_4538_SUPPORT), y)
    BMXT_DEFINES += BMXT_4538_SUPPORT
    BMXT_INCLUDES += $(MAGNUM)/portinginterface/mxt/4538
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/4538/bmxt_4538.c
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/4538/bmxt_3x4x.c
endif

#
# unused code
#

# default versions
BMXT_3128_VER ?= C0
BMXT_3383_VER ?= B0
BMXT_4528_VER ?= A0
BMXT_4517_VER ?= A0
BMXT_3472_VER ?= A0

BMXT_3128_VER_LOWER = $(shell awk 'BEGIN{print tolower("$(BMXT_3128_VER)")}')
BMXT_3383_VER_LOWER = $(shell awk 'BEGIN{print tolower("$(BMXT_3383_VER)")}')
BMXT_4528_VER_LOWER = $(shell awk 'BEGIN{print tolower("$(BMXT_4528_VER)")}')
BMXT_4517_VER_LOWER = $(shell awk 'BEGIN{print tolower("$(BMXT_4517_VER)")}')
BMXT_3472_VER_LOWER = $(shell awk 'BEGIN{print tolower("$(BMXT_3472_VER)")}')
