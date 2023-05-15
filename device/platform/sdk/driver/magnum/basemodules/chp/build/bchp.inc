############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bchp.inc $
# $brcm_Revision: Hydra_Software_Devel/45 $
# $brcm_Date: 9/7/12 4:43p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/chp/build/bchp.inc $
# 
# Hydra_Software_Devel/45   9/7/12 4:43p gmohile
# SW7435-347 : Fix bchp_pwr.inc include
# 
# Hydra_Software_Devel/44   9/7/12 3:56p gmohile
# SW7435-347 : Remove symbolic links for PWR implementation in CHP
# 
# Hydra_Software_Devel/43   9/6/12 5:19p jasonh
# SW7435-346: Move pwr directory into common.
# 
# Hydra_Software_Devel/42   7/23/12 3:06p randyjew
# SW7563-2: Add 7563 support.
# 
# Hydra_Software_Devel/41   7/20/12 4:05p mward
# SW7435-263: Check for pwr directory, allow old pwr files arrangement.
# 
# Hydra_Software_Devel/40   7/20/12 3:27p jessem
# SW7435-263: Added to include path for pwr
#
# Hydra_Software_Devel/39   7/19/12 4:27p jasonh
# SW7435-263: AVS move.
#
# Hydra_Software_Devel/38   7/19/12 4:00p jasonh
# SW7435-263: Updating directory structure.
#
# Hydra_Software_Devel/37   7/19/12 1:16p jasonh
# SW7435-263: Updating directory structure.
#
# Hydra_Software_Devel/36   7/19/12 10:01a jasonh
# SW7435-263: Updating directory structure.
#
# Hydra_Software_Devel/35   6/5/12 1:03p rjlewis
# SW7429-180: enabled AVS for 7429.
#
# Hydra_Software_Devel/34   5/18/12 3:44p gmohile
# SW7429-155 : Add 7429 Power Management Support
#
# Hydra_Software_Devel/33   4/19/12 11:59a gmohile
# SW7435-61 : 7435 power management support
#
# Hydra_Software_Devel/32   3/14/12 5:38p jtna
# SW7468-401: add back 7468 power management
#
# Hydra_Software_Devel/31   2/23/12 3:06p randyjew
# SW7468-401: Remove Pm2.0 for 7468 temporarily.
#
# Hydra_Software_Devel/30   12/13/11 8:11p bselva
# SW7360-6: Add appframework support for 7360 platform
#
# Hydra_Software_Devel/29   12/8/11 1:52p mward
# SW7125-1165: Replace findstring with filter to do exact word match.
#
# Hydra_Software_Devel/28   11/12/11 9:19p jessem
# SW7425-1596: Ported 7425 PM files to chip version specific directories.
#
# Hydra_Software_Devel/27   11/10/11 5:56p xhuang
# SW7552-9: add AVS support
#
# Hydra_Software_Devel/26   10/31/11 3:26p mward
# SW7125-7:  Enable AVS for 7435.
#
# Hydra_Software_Devel/25   10/27/11 3:52p gmohile
# SW7231-426 : Add 7231 B0 power management support
#
# Hydra_Software_Devel/24   10/27/11 1:53p xhuang
# SW7552-9: Add 7552 power management support
#
# Hydra_Software_Devel/23   10/20/11 3:47p bselva
# SW7358-169: Changes added for power management support
#
# Hydra_Software_Devel/23   10/20/11 3:42p bselva
# SW7358-169: Changes added for power management support
#
# Hydra_Software_Devel/22   8/31/11 2:00p rjlewis
# SW7346-117: added AVS support for 7344.
#
# Hydra_Software_Devel/21   8/20/11 5:58p rjlewis
# SW7346-117: added AVS support for 7231.
#
# Hydra_Software_Devel/20   8/19/11 3:41p rjlewis
# SW7346-117: Added support for avs files for 7358.
#
# Hydra_Software_Devel/19   8/11/11 1:37p rjlewis
# SW7346-117: Added support for AVS for 7346.
#
# Hydra_Software_Devel/18   8/8/11 4:55p rjlewis
# SW7346-117: merging to main.
#
# Hydra_Software_Devel/SW7346-117/2   8/8/11 4:01p rjlewis
# SW7346-117: only support the 7425 until after merge into the main.
#
# Hydra_Software_Devel/SW7346-117/1   8/4/11 5:57p rjlewis
# SW7346-117: add support for AVS source files.
#
# Hydra_Software_Devel/17   6/23/11 11:20a nickh
# SW7425-533: Add back PM support
#
# Hydra_Software_Devel/16   6/22/11 7:03p nickh
# SW7425-533: Disable 7425 Power management until its confirmed to be
# working
#
# Hydra_Software_Devel/15   6/20/11 1:19p randyjew
# SW7344-104: add PM2.0 for 7344/7346
#
# Hydra_Software_Devel/14   6/17/11 1:29p jhaberf
# SWDTV-7435: adoption of Power Management 2.0 for the 35233
#
# Hydra_Software_Devel/13   6/9/11 7:18p nickh
# SW7425-533: Add BCHP_PWR support for 7425
#
# Hydra_Software_Devel/12   5/31/11 7:27p randyjew
# SW7208-158: Add 7468 chip to pm2.0
#
# Hydra_Software_Devel/11   4/29/11 3:47p gmohile
# SW7231-128 : Add bchp_pwr support for 7231
#
# Hydra_Software_Devel/10   4/12/11 3:21p mward
# SW7125-905:  Add 7125 to list of chips with BCHP_PWR support.
#
# Hydra_Software_Devel/9   3/8/11 2:24p jtna
# SW7420-972: default BCHP_PWR_SUPPORT to =n
#
# Hydra_Software_Devel/8   3/4/11 7:18p spothana
# SW7420-1466 : Include CTK power management only if BCHP_PWR_CTK_SUPPORT
# is defined.
#
# Hydra_Software_Devel/7   12/3/10 3:17p gmohile
# SW7408-190 : Initial version for power management 2.0
#
# Hydra_Software_Devel/6   10/1/10 12:45p jtna
# SW7420-972: allow BCHP_PWR_SUPPORT=n at compile-time
#
# Hydra_Software_Devel/5   9/9/10 1:56p jtna
# SW7420-972: added BCHP_DEFINES += BCHP_PWR_SUPPORT
#
# Hydra_Software_Devel/4   8/19/10 3:10p jtna
# SW7405-4433: merge BCHP_PWR
#
# Hydra_Software_Devel/3   9/10/08 6:55p shyam
# PR46778 : Add 7043 includes
#
# Hydra_Software_Devel/2   4/2/08 5:37p shyam
# PR 39899 : Add Transcoder Version numbers BENC_VER_LOWER
#
# Hydra_Software_Devel/1   1/11/07 2:36p jgarrett
# PR27004: Adding new .inc files
#
############################################################

#
# Conditional Options for this Makefile
#
# BCHP_QAM_CHIP - If defined to a chip number (i.e. 3250), qam support for the specified chip is included.
# BCHP_7411_VER - If defined (in lower-case), specifies the revision of the 7411 chip to be supported.
#

# Define this module
MAGNUM_MODULES += BCHP

#
# CHP and other modules require the definition of the variables BCHP_VER and BCHP_CHIP.
# Note, these must be UPPER-CASE.
#
# For compatiblity, these defines are still required at the application level.
# BCHP_DEFINES := BCHP_CHIP=$(BCHP_CHIP) BCHP_VER=$(BCHP_VER)
# #
# # 7411 requires a similar definition.
# #
# ifneq ($(BCHP_7411_VER),)
# BCHP_DEFINES += BCHP_7411_VER=$(BCHP_7411_VER)
# endif

# Make sure we have appropriate lower-case equivalents of BCHP_VER and BCHP_7411_VER
BCHP_VER_LOWER := $(subst A,a,$(BCHP_VER))
BCHP_VER_LOWER := $(subst B,b,$(BCHP_VER_LOWER))
BCHP_VER_LOWER := $(subst C,c,$(BCHP_VER_LOWER))
BCHP_VER_LOWER := $(subst D,d,$(BCHP_VER_LOWER))
BCHP_VER_LOWER := $(subst E,e,$(BCHP_VER_LOWER))
BCHP_VER_LOWER := $(subst F,f,$(BCHP_VER_LOWER))
BCHP_VER_LOWER := $(subst G,g,$(BCHP_VER_LOWER))
BCHP_VER_LOWER := $(strip $(subst H,h,$(BCHP_VER_LOWER)))

BCHP_7411_VER_LOWER := $(subst A,a,$(BCHP_7411_VER))
BCHP_7411_VER_LOWER := $(subst B,b,$(BCHP_7411_VER_LOWER))
BCHP_7411_VER_LOWER := $(subst C,c,$(BCHP_7411_VER_LOWER))
BCHP_7411_VER_LOWER := $(subst D,d,$(BCHP_7411_VER_LOWER))
BCHP_7411_VER_LOWER := $(subst E,e,$(BCHP_7411_VER_LOWER))
BCHP_7411_VER_LOWER := $(subst F,f,$(BCHP_7411_VER_LOWER))
BCHP_7411_VER_LOWER := $(subst G,g,$(BCHP_7411_VER_LOWER))
BCHP_7411_VER_LOWER := $(strip $(subst H,h,$(BCHP_7411_VER_LOWER)))

BENC_CHIP_VER_LOWER := $(subst A,a,$(BENC_CHIP_VER))
BENC_CHIP_VER_LOWER := $(subst B,b,$(BENC_CHIP_VER_LOWER))
BENC_CHIP_VER_LOWER := $(subst C,c,$(BENC_CHIP_VER_LOWER))

#
# Always include support for the chip specified by BCHP_CHIP and BCHP_VER
#
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/include/common
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/include/$(BCHP_CHIP)/common
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/include/$(BCHP_CHIP)/rdb/$(BCHP_VER_LOWER)
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/src/common

BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/common/bchp.c
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/$(BCHP_CHIP)/bchp_$(BCHP_CHIP).c

# BCHP_PWR basemodule. 
include $(MAGNUM)/basemodules/chp/build/bchp_pwr.inc

# BCHP_AVS code.  More extensive AVS solution is being included in the newer 40nm parts.
ifeq ($(filter $(BCHP_CHIP), 7425 7435 7344 7346 7358 7231 7552 7360 7429 7563), $(BCHP_CHIP))
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/common/bchp_avs.c
endif

#
# Handle conditional options
#
ifneq ($(BCHP_QAM_CHIP),)
# 3250 support
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/include/$(BCHP_QAM_CHIP)/common
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/$(BCHP_QAM_CHIP)/bchp_$(BCHP_QAM_CHIP).c
endif

ifneq ($(BCHP_7411_VER),)
# 7411 chp support
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/include/7411/common
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/include/7411/rdb/$(BCHP_7411_VER_LOWER)
BCHP_SOURCES += $(MAGNUM)/basemodules/chp/src/7411/bchp_7411.c
endif

ifneq ($(BENC_CHIP),)
# 7043 support
BCHP_INCLUDES += $(MAGNUM)/basemodules/chp/include/$(BENC_CHIP)/rdb/$(BENC_CHIP_VER_LOWER)
endif
