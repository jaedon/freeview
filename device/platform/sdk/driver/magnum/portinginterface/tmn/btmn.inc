############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: btmn.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/6/12 8:16p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/tmn/btmn.inc $
# 
# Hydra_Software_Devel/2   9/6/12 8:16p agin
# SWNOOS-563:  Implement new directory structure.
# 
# Hydra_Software_Devel/1   4/22/10 2:47p brianlee
# SW7340-167: Initial version.
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BTMN

# Module Includes
BTMN_INCLUDES += $(MAGNUM)/portinginterface/tmn/include

# Module Sources
BTMN_SOURCES += $(MAGNUM)/portinginterface/tmn/src/btmn.c

