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
# $brcm_Date: 9/28/09 11:45a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/linux/driver/build/97550/Rules.make $
# 
# 1   9/28/09 11:45a nitinb
# SW7550-46: Add Linux user mode driver support for 7550
# 
############################################################

#
# Important toolchain and linux directories (make sure these are correct for your environment)
#

BSEAV = $(shell cd $(BUILDDIR)/../../../..; pwd)
SETTOP = $(shell cd $(BSEAV)/../SetTop; pwd)

BCM_CHIP = 7550
BCM_BOARD = bcm9$(BCM_CHIP)
BCM_BOARDNUM = 9$(BCM_CHIP)
BCM_ENDIAN = le

