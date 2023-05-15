############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: blna.inc $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 7/19/12 6:00p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/lna/blna.inc $
# 
# Hydra_Software_Devel/4   7/19/12 6:00p jasonh
# SW7435-263: Updating with new CHP directory structure.
# 
# Hydra_Software_Devel/3   5/26/11 3:39p mward
# SW7125-926:  Use 3412 RDB header in magnum/basemodules/chp, not local
# copy.
# 
# Hydra_Software_Devel/SW7125-926/1   4/28/11 5:31p mward
# SW7125-926:  Use 3412 RDB header in magnum/basemodules/chp, not local
# copy.
# 
# Hydra_Software_Devel/2   8/20/10 3:52p ryhsu
# SW7420-838:3412 LNA PI driver
# 
# Hydra_Software_Devel/1   1/11/07 2:40p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# BLNA_3405_SUPPORT - If defined, support will be included for the 3405
# BLNA_3431_SUPPORT - If defined, support will be included for the 3431
# BLNA_3412_SUPPORT - If defined, support will be included for the 3412
#

# Define this module
MAGNUM_MODULES += BLNA

# Module Includes
BLNA_INCLUDES += $(MAGNUM)/portinginterface/lna/

# Module sources
BLNA_SOURCES += $(MAGNUM)/portinginterface/lna/blna.c

#
# Conditional options
#
ifneq ($(BLNA_3405_SUPPORT),)
BLNA_INCLUDES += $(MAGNUM)/portinginterface/lna/3405/

BLNA_SOURCES += $(MAGNUM)/portinginterface/lna/3405/blna_3405.c
endif

ifneq ($(BLNA_3431_SUPPORT),)
BLNA_INCLUDES += $(MAGNUM)/portinginterface/lna/3431/

BLNA_SOURCES += $(MAGNUM)/portinginterface/lna/3431/blna_3431.c
endif

ifneq ($(BLNA_3412_SUPPORT),)
BLNA_INCLUDES += $(MAGNUM)/portinginterface/lna/3412/
BLNA_INCLUDES += $(MAGNUM)/basemodules/chp/include/3412/rdb/a0

BLNA_SOURCES += $(MAGNUM)/portinginterface/lna/3412/blna_3412.c
endif
