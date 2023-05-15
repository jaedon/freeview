############################################################
#     Copyright (c) 2003-2008, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 36 $
# $brcm_Date: 5/16/08 2:25p $
#
# Module Description: Top-level Settop API Makefile
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/api/build/Makefile $
# 
# 36   5/16/08 2:25p jgarrett
# PR 42774: Defaulting BUILD_SYSTEM based on nexus
# 
# 35   10/16/07 12:37p erickson
# PR36068: api/build/Makefile is now a simple switch based on
# BUILD_SYSTEM. previous Makefile moved to Makefile.magnum.
# 
############################################################

include tools.mak

# Guess BUILD_SYSTEM based on existence of nexus directory.
ifndef BUILD_SYSTEM
ifeq ($(wildcard ../../../nexus),)
# This is magnum.  Leave the variable unset.
else
# Nexus code exists.  Assume nexus.
BUILD_SYSTEM := nexus
endif
endif

# This should file remain simple. 
# It is only a switch to a BUILD_SYSTEM-specific Makefile.

ifeq ($(BUILD_SYSTEM),nexus)
all clean veryclean install:
	@$(MAKE) -C nexus $@
else
# Makefile.magnum supports magnum and legacy
all clean veryclean install:
	@$(MAKE) -f Makefile.magnum $@
endif
