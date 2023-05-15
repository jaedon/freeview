############################################################
#     Copyright (c) 2003-2013, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: $
# $brcm_Revision: $
# $brcm_Date: $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: $
#
############################################################

# Define this module
MAGNUM_MODULES += BMXT

BMXT_INCLUDES += $(MAGNUM)/portinginterface/mxt_v1441/include
BMXT_INCLUDES += $(MAGNUM)/basemodules/hab_v1441
BMXT_INCLUDES += $(MAGNUM)/basemodules/rpc
BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt_v1441/src/bmxt.c

# be wary of doing e.g.
# BMXT_INCLUDES += $(MAGNUM)/basemodules/chp/include/3128/rdb/$(BMXT_3128_VER_LOWER)
# BMXT_INCLUDES appears earlier than BCHP_INCLUDES and ends up forcing include files like "bchp_gio.h" from 3128, not 7425

ifeq ($(BMXT_3128_SUPPORT), y)
    BMXT_DEFINES += BMXT_3128_SUPPORT
    ifeq ($(findstring bmxt_3128.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_3128.c
    endif
endif

ifeq ($(BMXT_3383_SUPPORT), y)
    BMXT_DEFINES += BMXT_3383_SUPPORT
    ifeq ($(findstring bmxt_3128.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_3128.c
    endif
endif

ifeq ($(BMXT_4528_SUPPORT), y)
    BMXT_DEFINES += BMXT_4528_SUPPORT
    ifeq ($(findstring bmxt_3128.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_3128.c
    endif
endif

ifeq ($(BMXT_4517_SUPPORT), y)
    BMXT_DEFINES += BMXT_4517_SUPPORT
    ifeq ($(findstring bmxt_3128.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_3128.c
    endif
endif

ifeq ($(BMXT_3472_SUPPORT), y)
    BMXT_DEFINES += BMXT_3472_SUPPORT
    ifeq ($(findstring bmxt_3128.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_3128.c
    endif
endif

ifeq ($(BMXT_4538_SUPPORT), y)
    BMXT_DEFINES += BMXT_4538_SUPPORT
    ifeq ($(findstring bmxt_4538.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_4538.c
    endif
endif

ifeq ($(BMXT_3384_SUPPORT), y)
    BMXT_DEFINES += BMXT_3384_SUPPORT
    ifeq ($(findstring bmxt_4538.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_4538.c
    endif
endif

ifeq ($(BMXT_4548_SUPPORT), y)
    BMXT_DEFINES += BMXT_4548_SUPPORT
    ifeq ($(findstring bmxt_4538.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_4538.c
    endif
endif

ifeq ($(BMXT_7366_SUPPORT), y)
    BMXT_DEFINES += BMXT_7366_SUPPORT
    ifeq ($(findstring bmxt_4538.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_4538.c
    endif
endif

ifeq ($(BMXT_45216_SUPPORT), y)
    BMXT_DEFINES += BMXT_45216_SUPPORT
    ifeq ($(findstring bmxt_4538.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt_v1441/src/bmxt_4538.c
    endif
endif

ifeq ($(BMXT_7364_SUPPORT), y)
    BMXT_DEFINES += BMXT_7364_SUPPORT
    ifeq ($(findstring bmxt_4538.c, $(BMXT_SOURCES)), )
    BMXT_SOURCES += $(MAGNUM)/portinginterface/mxt/src/bmxt_4538.c
    endif
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
