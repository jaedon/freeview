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
# Created:
#
# $brcm_Log: $
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
BCHP_312X_VER := C0
endif

ifeq ($(BCHP_3461_VER),)
BCHP_3461_VER := B0
endif

ifeq ($(BCHP_3462_VER),)
BCHP_3462_VER := A0
endif

ifeq ($(BCHP_3472_VER),)
BCHP_3472_VER := B0
endif

ifeq ($(BCHP_7563_VER),)
BCHP_7563_VER := A0
endif

ifeq ($(BCHP_7364_VER),)
BCHP_7364_VER := A0
endif

ifeq ($(BCHP_7584_VER),)
BCHP_7584_VER := A0
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

ifeq ($(BCHP_7366_VER),)
BCHP_7366_VER := B0
endif

ifeq ($(BCHP_45216_VER),)
BCHP_45216_VER := A0
endif

BHAB_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_312X_VER)")}')

BHAB_3461_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_3461_VER)")}')
BHAB_3462_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_3462_VER)")}')
BHAB_3472_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_3472_VER)")}')
BHAB_7584_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_7584_VER)")}')
BHAB_7563_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_7563_VER)")}')
BHAB_7364_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_7364_VER)")}')
BHAB_4528_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_4528_VER)")}')
BHAB_4538_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_4538_VER)")}')
BHAB_4517_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_4517_VER)")}')
BHAB_7366_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_7366_VER)")}')
BHAB_45216_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_45216_VER)")}')

# Source needs to know which revision
BHAB_DEFINES += BHAB_312X_VER=BCHP_VER_$(BCHP_312X_VER)
BHAB_DEFINES += BHAB_3461_VER=BCHP_VER_$(BCHP_3461_VER)

# Module Includes
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab_v1441/include/common
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab_v1441/src/common
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
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3462/bhab_3462_fw_$(BHAB_3462_VER_LOWER).c
endif

ifneq ($(BHAB_3472_SUPPORT),)
BHAB_DEFINES += BHAB_CHIP=3472
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/3472/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/leapctfe/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/3472/rdb/hab/$(BHAB_3472_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3472/bhab_3472.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/bhab_34xx_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3472/bhab_3472_fw_$(BHAB_3472_VER_LOWER).c
endif

ifneq ($(BHAB_3128_SUPPORT),)
BHAB_312X_CHIP := 3128
BHAB_RDB_CHIP := 3128
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/3128/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/leapctfe/3128/
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3128/bhab_3128.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/3128/bhab_3128_fw_c0.c
endif

ifneq ($(BHAB_7584_SUPPORT),)
BHAB_7584_CHIP := 7584
BHAB_RDB_CHIP := 7584
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/7584/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/leapctfe/7584/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/7584/rdb/hab/$(BHAB_7584_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/7584/bhab_7584.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/7584/bhab_7584_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/7584/bhab_7584_fw_$(BHAB_7584_VER_LOWER).c
endif

ifneq ($(BHAB_7563_SUPPORT),)
BHAB_DEFINES += BHAB_CHIP=7563
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/7563/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/leapctfe/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/7563/rdb/hab/$(BHAB_7563_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/7563/bhab_7563.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/bhab_7563_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/7563/bhab_7563_fw_$(BHAB_7563_VER_LOWER).c
endif

ifneq ($(BHAB_7364_SUPPORT),)
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/7364/
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/src/leapctfe/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/7364/rdb/$(BHAB_7364_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/7364/bhab_7364.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/bhab_leap_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapctfe/7364/bhab_7364_fw_$(BHAB_7364_VER_LOWER).c
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
ifeq ($(BHAB_4538_FIRMWARE),SWM)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4538/bhab_4538_swm_fw_$(BHAB_4538_VER_LOWER).c
else
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/4538/bhab_4538_fw_$(BHAB_4538_VER_LOWER).c
endif
endif

ifneq ($(BHAB_7366_SUPPORT),)
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/7366/
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/7366/rdb/hab/$(BHAB_7366_VER_LOWER)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/7366/bhab_7366.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/7366/bhab_7366_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/7366/bhab_7366_fw_$(BHAB_7366_VER_LOWER).c
endif

ifneq ($(BHAB_4548_SUPPORT),)
BHAB_DEFINES += BHAB_CHIP=4548
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/4548/rdb/a0
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab/include/7366/
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/7366/bhab_7366.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/7366/bhab_7366_priv.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab/src/leapsatfe/7366/bhab_7366_fw_$(BHAB_7366_VER_LOWER).c
endif

ifneq ($(BHAB_45216_SUPPORT),)
BHAB_DEFINES += BHAB_CHIP=45216
BHAB_INCLUDES += $(MAGNUM)/basemodules/chp/include/45216/rdb/hab/$(BHAB_45216_VER_LOWER)
BHAB_INCLUDES += $(MAGNUM)/basemodules/hab_v1441/include/45216/
BHAB_SOURCES += $(MAGNUM)/basemodules/hab_v1441/src/leapsatfe/45216/bhab_45216.c
BHAB_SOURCES += $(MAGNUM)/basemodules/hab_v1441/src/leapsatfe/45216/bhab_45216_priv.c
ifeq ($(BHAB_45216_FIRMWARE),echo)
BHAB_SOURCES += $(MAGNUM)/basemodules/hab_v1441/src/leapsatfe/45216/bhab_45216_fw_$(BHAB_45216_VER_LOWER)_echo.c
else
BHAB_SOURCES += $(MAGNUM)/basemodules/hab_v1441/src/leapsatfe/45216/bhab_45216_fw_$(BHAB_45216_VER_LOWER).c
endif
endif

# Module Sources
BHAB_SOURCES += $(MAGNUM)/basemodules/hab_v1441/src/common/bhab.c
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
