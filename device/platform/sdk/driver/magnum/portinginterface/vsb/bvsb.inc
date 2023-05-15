############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bvsb.inc $
# $brcm_Revision: Hydra_Software_Devel/11 $
# $brcm_Date: 9/14/12 5:22p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/vsb/bvsb.inc $
# 
# Hydra_Software_Devel/11   9/14/12 5:22p mward
# SW7425-3922: changed VSB directory structure to remove symlinks
# 
# Hydra_Software_Devel/10   9/13/12 1:32p erickson
# SW7425-3922: don't compile in FW
# 
# Hydra_Software_Devel/9   9/12/12 10:32a enavarro
# SW7425-3922: changed VSB directory structure to remove symlinks
# 
# Hydra_Software_Devel/8   7/20/12 8:58a erickson
# SW7435-263: Updating directory structure.
# 
# Hydra_Software_Devel/7   10/30/08 1:04p dliu
# PR46516: remove chip version
# 
# Hydra_Software_Devel/6   10/30/08 1:03p dliu
# PR46516: Add compile time switch for power management
# 
# Hydra_Software_Devel/5   10/23/08 3:49p dliu
# PR48016: Spliting out IFD to MIPS code
# 
# Hydra_Software_Devel/4   10/12/08 12:03a dliu
# PR38934: Change 3548 hexfile to be chip version based
# 
# Hydra_Software_Devel/3   5/5/08 4:54p dliu
# PR38953: Add microcode to part of PI compile
# 
# Hydra_Software_Devel/2   2/28/08 5:51p dliu
# PR38953: Fix bug for BCHP_CHIP definition
# 
# Hydra_Software_Devel/1   1/11/07 2:40p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# BVSB_CHIP - Defines support for the external VSB chip to be used.  Should be either 3510 or 3520.  If not defined, assumed to be internal.
#

# Define this module
MAGNUM_MODULES += BVSB

# Determine which chip we are using for vsb
ifeq ($(BVSB_CHIP),3510)
 #
 # 3510 Support uses a different filename scheme.
 #
 BVSB_INCLUDES += $(MAGNUM)/basemodules/chp/include/3510/common
 BVSB_INCLUDES += \
	$(MAGNUM)/portinginterface/vsb/include/3510 \
	$(MAGNUM)/portinginterface/vsb/src/3510 

 BVSB_SOURCES += \
	$(MAGNUM)/portinginterface/vsb/src/3510/bvsb.c \
	$(MAGNUM)/portinginterface/vsb/src/3510/bvsb_priv.c
else 
   #
   # Handle 3520, 3560, 3563, 3548
   #
   ifneq ($(BVSB_CHIP),3520)
     BVSB_CHIP := ${BCHP_CHIP}
   endif
   # Module Includes
   BVSB_INCLUDES += \
     $(MAGNUM)/portinginterface/vsb/include/common \
     $(MAGNUM)/portinginterface/vsb/src/common \
     $(MAGNUM)/portinginterface/vsb/include/$(BVSB_CHIP) \
     $(MAGNUM)/basemodules/chp/include/$(BVSB_CHIP)/common
 
   # Module Sources
   BVSB_SOURCES += \
     $(MAGNUM)/portinginterface/vsb/src/common/bvsb.c \
     $(MAGNUM)/portinginterface/vsb/src/$(BVSB_CHIP)/bvsb_$(BVSB_CHIP).c \
     $(MAGNUM)/portinginterface/vsb/src/$(BVSB_CHIP)/bvsb_$(BVSB_CHIP)_priv.c 

     #starting with 3548, the firmware is compiled at magnum level
     ifeq ($(BVSB_CHIP),3548)
        ifneq ($(BVSB_POWER_MANAGEMENT),n)
           BVSB_DEFINES += BVSB_POWER_MANAGEMENT
        endif

        BVSB_SOURCES += \
         $(MAGNUM)/portinginterface/vsb/src/3548/bvsb_3548_ifd.c \
         $(MAGNUM)/portinginterface/vsb/src/3548/bvsb_3548_ifdcoef.c \
         $(MAGNUM)/portinginterface/vsb/src/3548/bcm3548ap_bx.c
     endif
endif
