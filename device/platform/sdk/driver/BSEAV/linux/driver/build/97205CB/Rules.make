############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Rules.make $
# $brcm_Revision: 1 $
# $brcm_Date: 9/13/12 6:31p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/linux/driver/build/97205CB/Rules.make $
# 
# 1   9/13/12 6:31p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/13/12 12:38p mphillip
# SW7425-3739: Remove symlinks for linux/driver/build subdirectories and
# dependencies
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

BCM_CHIP = 7405
BCM_BOARD = bcm9$(BCM_CHIP)
BCM_BOARDNUM = 9$(BCM_CHIP)
BCM_ENDIAN = le

