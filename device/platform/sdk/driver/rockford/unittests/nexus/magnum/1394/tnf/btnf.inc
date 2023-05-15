############################################################
#     Copyright (c) 2008-2009, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: btnf.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 10/9/09 5:50p $
#
# Module Description:
#
# Revision History:
#
# Created: 10/14/2008 by Roy Lewis
#
# $brcm_Log: /rockford/unittests/nexus/magnum/1394/tnf/btnf.inc $
# 
# Hydra_Software_Devel/1   10/9/09 5:50p rjlewis
# SW7420-380: Initial release.
# 
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BTNF

# Module Includes
BTNF_INCLUDES += \
	   tnf/$(BCHP_CHIP) 

# Module Sources
BTNF_SOURCES += \
	tnf/$(BCHP_CHIP)/btnf.c

