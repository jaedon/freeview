############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bvlc.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 9/22/10 12:18p $
#
# Module Description:
#
# Revision History:
#
# Created: 
#
# $brcm_Log: /magnum/commonutils/vlc/bvlc.inc $
# 
# Hydra_Software_Devel/1   9/22/10 12:18p jtna
# SW7405-4865: added bvlc.inc
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BVLC

# Module Includes
BVLC_INCLUDES += $(MAGNUM)/commonutils/vlc
BVLC_SOURCES +=  $(MAGNUM)/commonutils/vlc/bvlc.c

