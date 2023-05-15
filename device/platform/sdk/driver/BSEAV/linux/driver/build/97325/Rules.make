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
# $brcm_Revision: 2 $
# $brcm_Date: 7/23/09 6:36p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/linux/driver/build/97325/Rules.make $
# 
# 2   7/23/09 6:36p jrubio
# PR57079: fix SMTC BE build
# 
# 
############################################################

#
# Important toolchain and linux directories (make sure these are correct for your environment)
#

BSEAV = $(shell cd $(BUILDDIR)/../../../..; pwd)
SETTOP = $(shell cd $(BSEAV)/../SetTop; pwd)

BCM_CHIP = 7325
BCM_BOARD = bcm9$(BCM_CHIP)
BCM_BOARDNUM = 9$(BCM_CHIP)
ifeq ($(ARCH),mips-uclibc)
BCM_ENDIAN = be
else
BCM_ENDIAN = le
endif
