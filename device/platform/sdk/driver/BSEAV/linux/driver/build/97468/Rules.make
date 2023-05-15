############################################################
#     Copyright (c) 2003-2009, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Rules.make $
# $brcm_Revision: 1 $
# $brcm_Date: 9/28/09 2:42p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/linux/driver/build/97468/Rules.make $
# 
# 1   9/28/09 2:42p lwhite
# SW7468-6: Support for 7468
# 
# 1   9/16/05 10:17a marcusk
# PR17108: Basic shell version of these files.
# 
############################################################

#
# Important toolchain and linux directories (make sure these are correct for your environment)
#

BSEAV = $(shell cd $(BUILDDIR)/../../../..; pwd)
SETTOP = $(shell cd $(BSEAV)/../SetTop; pwd)

BCM_CHIP = 7468
BCM_BOARD = bcm9$(BCM_CHIP)
BCM_BOARDNUM = 9$(BCM_CHIP)
BCM_ENDIAN = le

