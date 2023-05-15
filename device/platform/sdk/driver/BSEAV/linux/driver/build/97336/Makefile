############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 2 $
# $brcm_Date: 9/17/12 4:37p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/linux/driver/build/97336/Makefile $
# 
# 2   9/17/12 4:37p mphillip
# SW7425-3739: Update legacy driver target directories to match new
# hierarchy
# 
# 1   1/28/09 1:45p jrubio
# PR51629: add 7336
# 
# 1   2/14/07 4:35p katrep
# PR27643: Added support 97405
# 
# 
############################################################

# Check for SMP vs. non-SMP kernel configuration
ifeq ($(SMP),y)
DEFCONFIG = arch/mips/configs/bcm97336a0-smp_defconfig
else
DEFCONFIG = arch/mips/configs/bcm97336a0_defconfig
endif

include ../97400/Makefile

