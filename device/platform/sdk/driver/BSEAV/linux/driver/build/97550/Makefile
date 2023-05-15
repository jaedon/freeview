############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 2 $
# $brcm_Date: 9/17/12 4:38p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/linux/driver/build/97550/Makefile $
# 
# 2   9/17/12 4:38p mphillip
# SW7425-3739: Update legacy driver target directories to match new
# hierarchy
# 
# 1   9/28/09 11:45a nitinb
# SW7550-46: Add Linux user mode driver support for 7550
#
############################################################

ifeq ($(BCHP_VER),A0)
    DEFCONFIG = arch/mips/configs/bcm7550a0_defconfig
else
    CHIP=7550
endif

include ../97400/Makefile

