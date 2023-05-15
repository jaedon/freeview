############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bbcp.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 9/6/12 6:32p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/bcp/bbcp.inc $
# 
# Hydra_Software_Devel/3   9/6/12 6:32p agin
# SWNOOS-563:  Implement new directory structure.
# 
# Hydra_Software_Devel/1   2/26/12 7:17p agin
# SWNOOS-518:  Create BICAP PI.
# 
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BBCP

# Module Includes
BBCP_INCLUDES += $(MAGNUM)/portinginterface/bcp/include

# Module Sources
BBCP_SOURCES += $(MAGNUM)/portinginterface/bcp/src/bbcp.c

