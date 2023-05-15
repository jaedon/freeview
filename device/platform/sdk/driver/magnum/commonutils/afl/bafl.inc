#############################################################################
#
#       Copyright (c) 2003-2010, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bafl.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 8/27/10 4:33p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /magnum/commonutils/afl/bafl.inc $
# 
# Hydra_Software_Devel/1   8/27/10 4:33p davidp
# SW7425-1: Merge from branch.
# 
# Hydra_Software_Devel/SW7425-1/2   8/18/10 5:04p davidp
# SW7425-1: Initial support checkin.
# 
# Hydra_Software_Devel/SW7425-1/1   7/14/10 4:30p nilesh
# SW7425-1: Added AFL commonutil
# 
#############################################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BAFL

# Module Includes
BAFL_INCLUDES += $(MAGNUM)/commonutils/afl/

# Module Sources
BAFL_SOURCES+= $(MAGNUM)/commonutils/afl/bafl.c


