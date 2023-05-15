############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bast.inc $
# $brcm_Revision: Hydra_Software_Devel/25 $
# $brcm_Date: 10/25/12 10:36a $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/ast/bast.inc $
# 
# Hydra_Software_Devel/25   10/25/12 10:36a ronchan
# SWSATFE-223: add bhab include path
# 
# Hydra_Software_Devel/24   10/24/12 3:50p mphillip
# SW7425-4035: Merge 4538 support branch to main
# 
# Hydra_Software_Devel/SW7425-4035/2   10/23/12 4:46p mphillip
# SW7425-4035: Correct a directory typo for 4528/4538
# 
# Hydra_Software_Devel/SW7425-4035/1   10/4/12 5:23p mphillip
# SW7425-4035: Initial 4538 support
# 
# Hydra_Software_Devel/23   10/16/12 2:18p ronchan
# SWSATFE-214: add bfec include directory
# 
# Hydra_Software_Devel/22   9/6/12 6:47p ronchan
# SWSATFE-214: update to new directory structure
# 
# Hydra_Software_Devel/21   7/19/12 5:58p jasonh
# SW7435-263: Updating with new CHP directory structure.
# 
# Hydra_Software_Devel/20   4/26/12 11:47a ronchan
# SW7360-13: add support for 7360
# 
# Hydra_Software_Devel/19   10/6/11 9:18a enavarro
# SWSATFE-150: removed bast_g3_priv_notch.c; added bast_g3_priv_cwc.c
# 
# Hydra_Software_Devel/18   6/30/11 2:41p vishk
# SWSATFE-86:  Add support to include the
# magnum/basemodules/chp/4528/rdb/host directory.
# 
# Hydra_Software_Devel/17   6/28/11 5:22p enavarro
# SWSATFE-86: added support for 4528
# 
# Hydra_Software_Devel/16   1/13/11 11:23a jrubio
# SWSATFE-75: add lna compilation
# 
# Hydra_Software_Devel/15   10/8/10 3:54p ronchan
# SWSATFE-75: add support for g3 satellite chips
# 
# Hydra_Software_Devel/14   8/10/09 5:58p jrubio
# PR57301: add BAST_AFEC_CORE_DISABLED
# 
# Hydra_Software_Devel/13   8/10/09 3:52p jrubio
# PR55232: add 7340/7342
# 
# Hydra_Software_Devel/12   1/31/09 1:27a jrubio
# PR51629: add 7336 support
# 
# Hydra_Software_Devel/11   1/21/09 9:47a ronchan
# PR 51274: remove 4506 A0 support
# 
# Hydra_Software_Devel/10   4/29/08 5:22p jrubio
# PR42336: add bast.c compilation for 73xx tuners
# 
# Hydra_Software_Devel/9   4/22/08 11:23a rjlewis
# PR40352: missing include directory.
# 
# Hydra_Software_Devel/8   12/27/07 3:31p agin
# PR38452: Integrate SDS into 97335 reference software.
# 
# Hydra_Software_Devel/7   12/5/07 6:34p agin
# PR37869: Need to compile 73xx files only if specified.
# 
# Hydra_Software_Devel/6   12/5/07 3:36p agin
# PR37869: Integrate SDS into 97325 reference software.
# 
# Hydra_Software_Devel/5   12/3/07 2:49p cnovak
# PR32395: Define passed to modules must be of the form BCHP_VER_XX.
# 
# Hydra_Software_Devel/4   10/12/07 6:05p jrubio
# PR35007: include microcode path
# 
# Hydra_Software_Devel/3   10/3/07 3:00p cnovak
# PR32395: Fixed error in setting the defines.
# 
# Hydra_Software_Devel/2   10/3/07 10:46a enavarro
# PR 32395: added 4506 support (from Chris Novak)
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
MAGNUM_MODULES += BAST

# Add dependency on BFEC
BAST_INCLUDES += $(MAGNUM)/commonutils/fec/include


##############################
# Handle 4501
##############################
ifeq ($(findstring 4501,$(BAST_CHIP)),4501)

# If the chip version is not specified, select a default
ifeq ($(BAST_4501_VER),)
BAST_4501_VER := C0
endif

BAST_4501_VER_UPPER := $(strip $(BAST_4501_VER))
BAST_4501_VER_UPPER := $(subst a,A,$(BAST_4501_VER_UPPER))
BAST_4501_VER_UPPER := $(subst b,B,$(BAST_4501_VER_UPPER))
BAST_4501_VER_UPPER := $(subst c,C,$(BAST_4501_VER_UPPER))

BAST_4501_VER_LOWER := $(strip $(BAST_4501_VER))
BAST_4501_VER_LOWER := $(subst A,a,$(BAST_4501_VER_LOWER))
BAST_4501_VER_LOWER := $(subst B,b,$(BAST_4501_VER_LOWER))
BAST_4501_VER_LOWER := $(subst C,c,$(BAST_4501_VER_LOWER))

# Source needs to know which revision
BAST_DEFINES += BAST_4501_VER=BCHP_VER_$(BAST_4501_VER_UPPER)

# Module Includes
BAST_INCLUDES += $(MAGNUM)/basemodules/chp/include/4501/common
BAST_INCLUDES += \
	$(MAGNUM)/portinginterface/ast/include/common \
	$(MAGNUM)/portinginterface/ast/include/4501 \
	$(MAGNUM)/portinginterface/ast/src/common \
	$(MAGNUM)/portinginterface/ast/src/4501
# Module Sources
BAST_SOURCES += \
	$(MAGNUM)/portinginterface/ast/src/common/bast.c \
	$(MAGNUM)/portinginterface/ast/src/4501/bast_4501.c \
	$(MAGNUM)/portinginterface/ast/src/4501/bast_4501_priv.c \
	$(MAGNUM)/portinginterface/ast/src/4501/bast_4501_fw.c
endif


##############################
# Handle 4506
##############################
ifeq ($(findstring 4506,$(BAST_CHIP)),4506)

# If the chip version is not specified, select a default
ifeq ($(BAST_4506_VER),)
BAST_4506_VER := A1
endif

BAST_4506_VER_UPPER := $(strip $(BAST_4506_VER))
BAST_4506_VER_UPPER := $(subst a,A,$(BAST_4506_VER_UPPER))
BAST_4506_VER_UPPER := $(subst b,B,$(BAST_4506_VER_UPPER))
BAST_4506_VER_UPPER := $(subst c,C,$(BAST_4506_VER_UPPER))

BAST_4506_VER_LOWER := $(strip $(BAST_4506_VER))
BAST_4506_VER_LOWER := $(subst A,a,$(BAST_4506_VER_LOWER))
BAST_4506_VER_LOWER := $(subst B,b,$(BAST_4506_VER_LOWER))
BAST_4506_VER_LOWER := $(subst C,c,$(BAST_4506_VER_LOWER))

# Source needs to know which revision
BAST_DEFINES += BAST_4506_VER=BCHP_VER_$(BAST_4506_VER_UPPER)

# Module Includes
BAST_INCLUDES += $(MAGNUM)/basemodules/chp/include/4506/common
BAST_INCLUDES += \
	$(MAGNUM)/portinginterface/ast/include/common \
	$(MAGNUM)/portinginterface/ast/include/4506 \
	$(MAGNUM)/portinginterface/ast/src/common \
	$(MAGNUM)/portinginterface/ast/src/4506
# Module Sources
BAST_SOURCES += \
	$(MAGNUM)/portinginterface/ast/src/common/bast.c \
	$(MAGNUM)/portinginterface/ast/src/4506/bast_4506.c \
	$(MAGNUM)/portinginterface/ast/src/4506/bast_4506_priv.c \
	$(MAGNUM)/portinginterface/ast/src/4506/bast_4506_fw.c
endif


##############################
# Handle 4517
##############################
ifeq ($(findstring 4517,$(BAST_CHIP)),4517)

# If the chip version is not specified, select a default
ifeq ($(BAST_4517_VER),)
BAST_4517_VER := B0
endif

BAST_4517_VER_UPPER := $(strip $(BAST_4517_VER))
BAST_4517_VER_UPPER := $(subst a,A,$(BAST_4517_VER_UPPER))
BAST_4517_VER_UPPER := $(subst b,B,$(BAST_4517_VER_UPPER))
BAST_4517_VER_UPPER := $(subst c,C,$(BAST_4517_VER_UPPER))

BAST_4517_VER_LOWER := $(strip $(BAST_4517_VER))
BAST_4517_VER_LOWER := $(subst A,a,$(BAST_4517_VER_LOWER))
BAST_4517_VER_LOWER := $(subst B,b,$(BAST_4517_VER_LOWER))
BAST_4517_VER_LOWER := $(subst C,c,$(BAST_4517_VER_LOWER))

# Source needs to know which revision
BAST_DEFINES += BAST_4517_VER=BCHP_VER_$(BAST_4517_VER_UPPER)

# Module Includes
BAST_INCLUDES += $(MAGNUM)/basemodules/chp/include/4517/common
BAST_INCLUDES += $(MAGNUM)/basemodules/chp/include/4517/rdb/$(BAST_4517_VER_LOWER)
BAST_INCLUDES += $(MAGNUM)/basemodules/hab/include/common
BAST_INCLUDES += $(MAGNUM)/basemodules/hab/include/4517
BAST_INCLUDES += \
	$(MAGNUM)/portinginterface/ast/include/common \
	$(MAGNUM)/portinginterface/ast/include/4517 \
	$(MAGNUM)/portinginterface/ast/src/common \
	$(MAGNUM)/portinginterface/ast/src/4517
# Module Sources
BAST_SOURCES += \
	$(MAGNUM)/portinginterface/ast/src/common/bast.c \
	$(MAGNUM)/portinginterface/ast/src/4517/bast_4517.c \
	$(MAGNUM)/portinginterface/ast/src/4517/bast_4517_priv.c
endif


##############################
# Handle 4528
##############################
ifeq ($(findstring 4528,$(BAST_CHIP)),4528)

# If the chip version is not specified, select a default
ifeq ($(BAST_4528_VER),)
BAST_4528_VER := A0
endif

BAST_4528_VER_UPPER := $(strip $(BAST_4528_VER))
BAST_4528_VER_UPPER := $(subst a,A,$(BAST_4528_VER_UPPER))
BAST_4528_VER_UPPER := $(subst b,B,$(BAST_4528_VER_UPPER))
BAST_4528_VER_UPPER := $(subst c,C,$(BAST_4528_VER_UPPER))

BAST_4528_VER_LOWER := $(strip $(BAST_4528_VER))
BAST_4528_VER_LOWER := $(subst A,a,$(BAST_4528_VER_LOWER))
BAST_4528_VER_LOWER := $(subst B,b,$(BAST_4528_VER_LOWER))
BAST_4528_VER_LOWER := $(subst C,c,$(BAST_4528_VER_LOWER))

# Source needs to know which revision
BAST_DEFINES += BAST_4528_VER=BCHP_VER_$(BAST_4528_VER_UPPER)

# Module Includes
BAST_INCLUDES += $(MAGNUM)/basemodules/chp/include/4528/common
BAST_INCLUDES += $(MAGNUM)/basemodules/chp/include/4528/rdb/hab/$(BAST_4528_VER_LOWER)
BAST_INCLUDES += $(MAGNUM)/basemodules/hab/include/common
BAST_INCLUDES += $(MAGNUM)/basemodules/hab/include/4528
BAST_INCLUDES += \
	$(MAGNUM)/portinginterface/ast/include/common \
	$(MAGNUM)/portinginterface/ast/include/4528 \
	$(MAGNUM)/portinginterface/ast/src/common \
	$(MAGNUM)/portinginterface/ast/src/4528
# Module Sources
BAST_SOURCES += \
	$(MAGNUM)/portinginterface/ast/src/common/bast.c \
	$(MAGNUM)/portinginterface/ast/src/4528/bast_4528.c \
	$(MAGNUM)/portinginterface/ast/src/4528/bast_4528_priv.c
endif


##############################
# Handle 4538
##############################
ifeq ($(findstring 4538,$(BAST_CHIP)),4538)

# If the chip version is not specified, select a default
ifeq ($(BAST_4538_VER),)
BAST_4538_VER := A0
endif

BAST_4538_VER_UPPER := $(strip $(BAST_4538_VER))
BAST_4538_VER_UPPER := $(subst a,A,$(BAST_4538_VER_UPPER))
BAST_4538_VER_UPPER := $(subst b,B,$(BAST_4538_VER_UPPER))
BAST_4538_VER_UPPER := $(subst c,C,$(BAST_4538_VER_UPPER))

BAST_4538_VER_LOWER := $(strip $(BAST_4538_VER))
BAST_4538_VER_LOWER := $(subst A,a,$(BAST_4538_VER_LOWER))
BAST_4538_VER_LOWER := $(subst B,b,$(BAST_4538_VER_LOWER))
BAST_4538_VER_LOWER := $(subst C,c,$(BAST_4538_VER_LOWER))

# Source needs to know which revision
BAST_DEFINES += BAST_4538_VER=BCHP_VER_$(BAST_4538_VER_UPPER)

# Module Includes
BAST_INCLUDES += $(MAGNUM)/basemodules/chp/include/4538/common
BAST_INCLUDES += $(MAGNUM)/basemodules/chp/include/4538/rdb/hab/$(BAST_4538_VER_LOWER)
BAST_INCLUDES += $(MAGNUM)/basemodules/hab/include/common
BAST_INCLUDES += $(MAGNUM)/basemodules/hab/include/4538
BAST_INCLUDES += \
	$(MAGNUM)/portinginterface/ast/include/common \
	$(MAGNUM)/portinginterface/ast/include/4538 \
	$(MAGNUM)/portinginterface/ast/src/common \
	$(MAGNUM)/portinginterface/ast/src/4538
# Module Sources
BAST_SOURCES += \
	$(MAGNUM)/portinginterface/ast/src/common/bast.c \
	$(MAGNUM)/portinginterface/ast/src/4538/bast_4538.c \
	$(MAGNUM)/portinginterface/ast/src/4538/bast_4538_priv.c
endif


##############################
# Handle G2
##############################
ifeq ($(findstring $(BCHP_CHIP), 7325 7335 7342 7340), $(BCHP_CHIP))
# Module Includes
BAST_INCLUDES += \
	$(MAGNUM)/portinginterface/ast/include/common \
	$(MAGNUM)/portinginterface/ast/include/g2 \
	$(MAGNUM)/portinginterface/ast/src/common \
	$(MAGNUM)/portinginterface/ast/src/g2/common \
	$(MAGNUM)/portinginterface/ast/src/g2/$(BCHP_CHIP)
# Module Sources
BAST_SOURCES += \
	$(MAGNUM)/portinginterface/ast/src/common/bast.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv_diseqc.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv_ftm.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv_ldpc.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv_lna.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv_mi2c.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv_qpsk.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv_sds.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv_tuner.c \
	$(MAGNUM)/portinginterface/ast/src/g2/common/bast_g2_priv_turbo.c \
	$(MAGNUM)/portinginterface/ast/src/g2/$(BCHP_CHIP)/bast_$(BCHP_CHIP)_priv.c

ifeq ($(BAST_AFEC_CORE_DISABLED),y)
BAST_DEFINES += BAST_AFEC_CORE_DISABLED
endif

endif


##############################
# Handle G3
##############################
ifeq ($(findstring $(BCHP_CHIP), 7344 7346 7358 7360), $(BCHP_CHIP))
# Module Includes
BAST_INCLUDES += \
	$(MAGNUM)/portinginterface/ast/include/common \
	$(MAGNUM)/portinginterface/ast/include/g3 \
	$(MAGNUM)/portinginterface/ast/src/common \
	$(MAGNUM)/portinginterface/ast/src/g3/common \
	$(MAGNUM)/portinginterface/ast/src/g3/$(BCHP_CHIP)
# Module Sources
BAST_SOURCES += \
	$(MAGNUM)/portinginterface/ast/src/common/bast.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3.c	\
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_acq.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_dft.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_diseqc.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_ftm.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_hp.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_ldpc.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_mi2c.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_plc.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_cwc.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_qpsk.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_snr.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_timer.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_tuner.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_turbo.c \
	$(MAGNUM)/portinginterface/ast/src/g3/common/bast_g3_priv_lna.c \
	$(MAGNUM)/portinginterface/ast/src/g3/$(BCHP_CHIP)/bast_$(BCHP_CHIP)_priv.c
endif
