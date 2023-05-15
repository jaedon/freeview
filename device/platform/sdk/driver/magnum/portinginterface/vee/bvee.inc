############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bvee.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/6/12 9:07p $
#
# Module Description:
#
# $brcm_Log: /magnum/portinginterface/vee/bvee.inc $
# 
# Hydra_Software_Devel/2   9/6/12 9:07p jgarrett
# SW7425-3854: removing symlinks from VEE
# 
# Hydra_Software_Devel/1   10/19/11 8:30p venkatr
# SW7346-515 : [7346] VEE PI code initial check in
# 
# 
# 
############################################################

#
# Conditional Options for this Makefile
#

# Define this module
MAGNUM_MODULES += BVEE

# Module Includes
BVEE_INCLUDES += $(MAGNUM)/portinginterface/vee/include
BVEE_INCLUDES += $(MAGNUM)/portinginterface/vee/src
BVEE_INCLUDES += $(MAGNUM)/commonutils/sur/src

# Module Sources
BVEE_SOURCES += $(MAGNUM)/portinginterface/vee/src/bvee.c \
                $(MAGNUM)/portinginterface/vee/src/bvee_channel.c \
                $(MAGNUM)/portinginterface/vee/src/bvee_dsp_utils_priv.c \
