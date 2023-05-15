############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: config.mk $
# $brcm_Revision: 1 $
# $brcm_Date: 1/12/12 7:02p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/app/brutus2/build/config.mk $
# 
# 1   1/12/12 7:02p tokushig
# SW7405-5581: Brutus2 initial check in
# 
# 4   11/17/09 2:13p erickson
# SW7405-3408: use $(filter) instead of $(findstring)
# 
# 3   9/20/06 1:19p gmohile
# PR 24129: Added support for 7313
# 
# 2   9/13/06 11:39a erickson
# PR24230: remove 7327 and 7329
# 
# 1   2/7/05 7:40p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/27   12/14/04 7:17p bandrews
# PR13158: 7318 support
# 
# Irvine_BSEAVSW_Devel/26   9/22/04 2:51p bandrews
# PR12687: 7312 and 7327 initial support
# 
# Irvine_BSEAVSW_Devel/25   9/15/04 2:40p erickson
# PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
#
# Irvine_BSEAVSW_Devel/24   8/18/04 12:35p bandrews
# PR11657: 7329 Support
#
# Irvine_BSEAVSW_Devel/23   6/1/04 4:23p bandrews
# PR10919: Added 7314
#
# Irvine_BSEAVSW_Devel/22   5/12/04 10:24a erickson
# PR10967: added standard header
############################################################

# config.mk - allows for conditional compilation of apps
# based on CHIP and PLATFORM
#
# Inputs: CHIP and PLATFORM
# Outputs: CFLAGS CXXFLAGS

#
# CHIP and PLATFORM defines
#

CHIP_GROUPS =

#
# Special defines for families (or related groups) of chips
# These should always end with _FAMILY
#
ifeq ($(filter $(CHIP),7312 7313 7314 7315 7317 7318 7319 7320 7328),$(CHIP))
CHIP_GROUPS+=-DBCM73XX_FAMILY
endif
ifeq ($(filter $(CHIP),7319 7320 7328),$(CHIP))
CHIP_GROUPS+=-DBCM732X_FAMILY
endif
ifeq ($(filter $(CHIP),7110 7112 7115),$(CHIP))
CHIP_GROUPS+=-DBCM71XX_FAMILY
endif

#
# Extra defines for chip flavors. These are chips which are minor
# variations another "main" chips. The main chip is used for BCHP_CHIP
# and the flavor is added an an extra.
# These should always end with _FLAVOR
#
ifeq ($(PLATFORM),97319)
CHIP_GROUPS+=-DBCM7319_FLAVOR
endif
ifeq ($(PLATFORM),97317)
CHIP_GROUPS+=-DBCM7317_FLAVOR
endif
ifeq ($(PLATFORM),97314)
CHIP_GROUPS+=-DBCM7314_FLAVOR # both 14 and 17
CHIP_GROUPS+=-DBCM7317_FLAVOR
endif
ifeq ($(PLATFORM),97112)
CHIP_GROUPS+=-DBCM7112_FLAVOR
endif

# NOTE: To conditionally compile using the "main" chip number,
# use BCHP_CHIP == ####.

# Now modify CFLAGS
CFLAGS += $(CHIP_GROUPS)
