############################################################################
#     Copyright (c) 2003, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: tools.mak $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 4/1/03 8:02p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/inc/tools.mak $
# 
# Hydra_Software_Devel/1   4/1/03 8:02p vsilyaev
# Framework make include files - initial version.
# 
# 
###########################################################################

# Defines macros for cross-development tools
S		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CCPP		= $(CROSS_COMPILE)g++
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
RANLIB		= $(CROSS_COMPILE)ranlib
CP		= cp -f
RM		= rm -f
MKDIR		= mkdir 
SORT 	= sort
SED 	= sed
TOUCH	= touch

