############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bastmlib.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 9/6/12 8:52p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/syslib/astmlib/bastmlib.inc $
# 
# Hydra_Software_Devel/3   9/6/12 8:52p bandrews
# SW7425-3850: merge to main
# 
# Hydra_Software_Devel/SW7425-3850/1   9/4/12 5:48p bandrews
# SW7425-3850: remove symlinks
# 
# Hydra_Software_Devel/2   7/17/09 6:56p bandrews
# PR49215: playback support
# 
# Hydra_Software_Devel/1   3/24/08 3:08p bandrews
# PR40865: Fixed
# 
# Hydra_Software_Devel/3   2/15/08 10:04p bandrews
# PR36148: Updated based on reviews
# 
# Hydra_Software_Devel/2   1/25/08 9:23p bandrews
# PR36148: Updated based on simulation
# 
# Hydra_Software_Devel/1   10/19/07 9:30p bandrews
# PR36148: Initial implementation
############################################################

# Define this module
MAGNUM_MODULES += BASTMLIB

# Module Includes
BASTMLIB_INCLUDES += $(MAGNUM)/syslib/astmlib/include

BASTMLIB_SOURCES += \
    $(MAGNUM)/syslib/astmlib/src/bastmlib.c \
    $(MAGNUM)/syslib/astmlib/src/bastmlib_priv.c \
    $(MAGNUM)/syslib/astmlib/src/bastmlib_clock_coupling_task.c \
    $(MAGNUM)/syslib/astmlib/src/bastmlib_clock_reference.c \
    $(MAGNUM)/syslib/astmlib/src/bastmlib_presentation_task.c \
    $(MAGNUM)/syslib/astmlib/src/bastmlib_presenter.c

