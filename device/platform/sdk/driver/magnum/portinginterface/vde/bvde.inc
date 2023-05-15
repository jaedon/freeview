############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bvde.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/6/12 8:45p $
#
# Module Description:
#
# $brcm_Log: /magnum/portinginterface/vde/bvde.inc $
# 
# Hydra_Software_Devel/2   9/6/12 8:45p jgarrett
# SW7425-3854: Removing symlinks from VDE
# 
# Hydra_Software_Devel/1   5/31/11 4:44p speter
# SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
# Video Decode using Raaga DSP
# 
############################################################

#
# Conditional Options for this Makefile
#

# Define this module
MAGNUM_MODULES += BVDE

# Module Includes
BVDE_INCLUDES += $(MAGNUM)/portinginterface/vde/include \
                 $(MAGNUM)/portinginterface/vde/src

# Module Sources
BVDE_SOURCES += $(MAGNUM)/portinginterface/vde/src/bvde.c \
                $(MAGNUM)/portinginterface/vde/src/bvde_channel.c \
                $(MAGNUM)/portinginterface/vde/src/bvde_dsp_utils_priv.c \
