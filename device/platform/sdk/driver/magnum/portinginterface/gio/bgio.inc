############################################################
#     Copyright (c) 2003-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bgio.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 1/11/07 2:39p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/gio/bgio.inc $
#
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
#
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BGIO

# Module Include Path
BGIO_INCLUDES += $(MAGNUM)/portinginterface/gio/include

# Module Sources
BGIO_SOURCES += $(MAGNUM)/portinginterface/gio/src/bgio.c
BGIO_SOURCES += $(MAGNUM)/portinginterface/gio/src/bgio_priv.c
BGIO_SOURCES += $(MAGNUM)/portinginterface/gio/src/bgio_pin.c
BGIO_SOURCES += $(MAGNUM)/portinginterface/gio/src/bgio_pin_priv.c
BGIO_SOURCES += $(MAGNUM)/portinginterface/gio/src/bgio_pintable_priv.c

#
# End of file
#