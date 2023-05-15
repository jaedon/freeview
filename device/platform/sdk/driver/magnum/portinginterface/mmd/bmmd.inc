############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmmd.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 9/6/12 5:22p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/mmd/bmmd.inc $
# 
# Hydra_Software_Devel/3   9/6/12 5:22p jtna
# SW7425-3762: remove symlinks
# 
# Hydra_Software_Devel/2   4/15/11 5:54p jtna
# SW7422-416: merge to main
# 
# Hydra_Software_Devel/mmd_devel_init/1   4/6/11 3:20p jtna
# SW7422-416: branch-out for initial devel
# 
############################################################

# Define this module
MAGNUM_MODULES += BMMD

# Module Includes
BMMD_INCLUDES += $(MAGNUM)/portinginterface/mmd/include

# Module Sources
BMMD_SOURCES += $(MAGNUM)/portinginterface/mmd/src/bmmd.c
