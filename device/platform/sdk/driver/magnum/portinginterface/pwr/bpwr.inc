############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bpwr.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 9/21/12 7:22p $
#
# Module Description:
#
# Revision History:
#
# Created: 10/19/2007 by Jesse Montesino
#
# $brcm_Log: /magnum/portinginterface/pwr/bpwr.inc $
# 
# Hydra_Software_Devel/3   9/21/12 7:22p nickh
# SW7425-3971: Update with new directory structure
# 
# Hydra_Software_Devel/2   2/4/10 6:29p nickh
# SW7420-573: Future test port
# 
# Hydra_Software_Devel/1   12/20/07 10:30a jessem
# PR 38367: Initial version.
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BPWR

# Module Includes
BPWR_INCLUDES += $(MAGNUM)/portinginterface/pwr/include \
		 $(MAGNUM)/portinginterface/pwr/src/$(BCHP_CHIP)

# Module Sources
BPWR_SOURCES += $(MAGNUM)/portinginterface/pwr/src/bpwr.c \
		$(MAGNUM)/portinginterface/pwr/src/bpwr_mem.c  \
		$(MAGNUM)/portinginterface/pwr/src/$(BCHP_CHIP)/bpwr_priv.c 

