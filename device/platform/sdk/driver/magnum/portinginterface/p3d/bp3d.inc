############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bp3d.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/7/12 3:22p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/p3d/bp3d.inc $
# 
# Hydra_Software_Devel/2   9/7/12 3:22p nissen
# SW7405-5690: Update for new directories.
# 
# Hydra_Software_Devel/1   1/11/07 2:41p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BP3D

# Module Includes
BP3D_INCLUDES += $(MAGNUM)/portinginterface/p3d/include

# Module Sources
BP3D_SOURCES += $(MAGNUM)/portinginterface/p3d/src/bp3d.c
BP3D_SOURCES += $(MAGNUM)/portinginterface/p3d/src/bp3d_displaylist.c
BP3D_SOURCES += $(MAGNUM)/portinginterface/p3d/src/bp3d_layer.c
BP3D_SOURCES += $(MAGNUM)/portinginterface/p3d/src/bp3d_memory.c
BP3D_SOURCES += $(MAGNUM)/portinginterface/p3d/src/bp3d_primitive.c
BP3D_SOURCES += $(MAGNUM)/portinginterface/p3d/src/bp3d_render.c
BP3D_SOURCES += $(MAGNUM)/portinginterface/p3d/src/bp3d_tile.c

