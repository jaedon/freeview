############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bape.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 6/25/12 4:20p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/27/2009 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/ape/bape.inc $
# 
# Hydra_Software_Devel/2   6/25/12 4:20p jgarrett
# SW7425-2952: Restructuring magnum directories
# 
# Hydra_Software_Devel/1   11/20/09 5:24p jgarrett
# SW7408-17: Adding APE
# 
############################################################

# Include makefile from chip-specific directory
ifeq ($(BCHP_CHIP),7408)
include $(MAGNUM)/portinginterface/ape/build/bape_mips.inc
else
include $(MAGNUM)/portinginterface/ape/build/bape_raaga.inc
endif

