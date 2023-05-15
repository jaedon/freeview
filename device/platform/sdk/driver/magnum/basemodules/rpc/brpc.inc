############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: brpc.inc $
# $brcm_Revision: Hydra_Software_Devel/6 $
# $brcm_Date: 9/7/12 2:41p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/rpc/brpc.inc $
# 
# Hydra_Software_Devel/6   9/7/12 2:41p marcusk
# SW7425-3901: removed symlinks
# 
# Hydra_Software_Devel/5   8/25/11 9:12a agin
# SWNOOS-481:  Add no_os and ucos_ii support.
# 
# Hydra_Software_Devel/4   9/24/09 10:00a erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
# 
# Hydra_Software_Devel/3   10/18/07 10:21a rjlewis
# PR36223: add vxworks support.
# 
# Hydra_Software_Devel/2   1/17/07 3:09p mward
# PR27004: $(findstring find,in) reversed.
# 
# Hydra_Software_Devel/1   1/11/07 2:37p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BRPC

# Backward-compatibility
B_REFSW_OS ?= $(OS)

# Module Includes
BRPC_INCLUDES += $(MAGNUM)/basemodules/rpc/include

# Module Sources
BRPC_SOURCES += $(MAGNUM)/basemodules/rpc/src/brpc.c
BRPC_SOURCES += $(MAGNUM)/basemodules/rpc/src/$(B_REFSW_OS)/brpc_socket.c

# Sanity check B_REFSW_OS value
BRPC_SUPPORTED_OS := linuxuser linuxkernel no_os ucos_ii vxworks
ifeq ($(findstring $(B_REFSW_OS),$(BRPC_SUPPORTED_OS)),)
$(error [rpc] Illegal OS [$(B_REFSW_OS)].  Supported OS values are [$(BRPC_SUPPORTED_OS)])
endif

