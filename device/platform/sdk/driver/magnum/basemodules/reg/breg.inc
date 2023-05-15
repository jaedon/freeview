############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: breg.inc $
# $brcm_Revision: Hydra_Software_Devel/7 $
# $brcm_Date: 1/21/10 10:13a $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/reg/breg.inc $
# 
# Hydra_Software_Devel/7   1/21/10 10:13a kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
# 
# Hydra_Software_Devel/SW35230-56/1   1/18/10 6:29p kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
# 
# Hydra_Software_Devel/6   5/12/09 4:20p jessem
# PR 54881: Replaced linuxclient with linuxemu.
# 
# Hydra_Software_Devel/5   4/8/09 2:12p shyam
# PR52386 : Support for splash reg write captures
# 
# Hydra_Software_Devel/4   3/26/09 11:25a shyam
# PR50711 : Add make option for splash
# 
# Hydra_Software_Devel/3   10/22/07 4:41p jessem
# PR 36129: Changed rule for sims  to use linuxclient and breg_client.c
# instead.
# 
# Hydra_Software_Devel/2   4/2/07 2:59p shyam
# PR 29285 : Added linuxclient support for simulations/emulation
# 
# Hydra_Software_Devel/1   1/11/07 2:37p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# BCHP_7411_VER - If defined (in lower-case), specifies the revision of the 7411 chip to be supported.
#

# Define this module
MAGNUM_MODULES += BREG

# Module Includes
BREG_INCLUDES += $(MAGNUM)/basemodules/reg

BREG_SOURCES += \
	$(MAGNUM)/basemodules/reg/breg_i2c.c \
	$(MAGNUM)/basemodules/reg/breg_spi.c

ifeq ($(BREG_CAPTURE),y)
BREG_DEFINES += BREG_CAPTURE
endif

ifeq ($(SYSTEM),linuxemu)
BREG_SOURCES += $(MAGNUM)/basemodules/reg/breg_client.c
else
ifeq ($(SYSTEM),linuxpal)
BREG_SOURCES += $(MAGNUM)/basemodules/reg/breg_mem_pal.c
else
BREG_SOURCES += $(MAGNUM)/basemodules/reg/breg_mem.c
endif
endif

#
# Handle conditional options
#
ifneq ($(BCHP_7411_VER),)
# 7411 support
BREG_INCLUDES += $(MAGNUM)/basemodules/reg/7411
endif

