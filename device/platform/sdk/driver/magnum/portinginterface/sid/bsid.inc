############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bsid.inc $
# $brcm_Revision: Hydra_Software_Devel/6 $
# $brcm_Date: 9/6/12 8:59p $
#
# Module Description:
#
# Revision History:
#
# Created: 04/16/2007 by Fabrizio Basso
#
# $brcm_Log: /magnum/portinginterface/sid/bsid.inc $
# 
# Hydra_Software_Devel/6   9/6/12 8:59p fbasso
# SW7425-3898: removed symbolic links from directory.
# 
# Hydra_Software_Devel/5   1/13/11 2:45p etrudeau
# SWBLURAY-24226: add 7640 support
# 
# Hydra_Software_Devel/4   3/20/09 2:43p parijat
# PR 52982: BSID PI support for 7635 "Dune" chip
# 
# Hydra_Software_Devel/3   3/20/09 2:34p parijat
# PR 52982: BSID PI support for 7635 "Dune" chip
# 
# Hydra_Software_Devel/2   7/10/08 11:20a fbasso
# PR 44533: added support for 7601.
# 
# Hydra_Software_Devel/1   3/3/08 2:09p vobadm
# PR38951: Add sid to CC
# Added file element "arc".
# Added file element "bsid_fw_prot.c".
# Added file element "sid.hex".
# Added file element "sid_pi.doc".
# Added file element "srec".
# PR38951: Add sid to CC
# Added file element "bsid.c".
# Added file element "bsid.h".
# Added file element "bsid_err.h".
# Added file element "bsid_fw.c".
# Added file element "bsid_img.c".
# Added file element "bsid_img.h".
# Added file element "bsid_msg.c".
# Added file element "bsid_msg.h".
# Added file element "bsid_priv.c".
# Added file element "bsid_priv.h".
# Added directory element "tools".
# 
# bdvd_v10/3   4/16/07 3:23p fbasso
# modified to match equivalen under rockford/modules
# 
# bdvd_v10/2   4/16/07 3:16p fbasso
# Generated this file.
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BSID

# Module Conditional Compile Flags
BSID_DEFINES += BSID_USE_CACHED_MEMORY

# Module Includes
BSID_INCLUDES += $(MAGNUM)/portinginterface/sid/include/

# Module Sources
BSID_SOURCES += $(MAGNUM)/portinginterface/sid/src/bsid.c
BSID_SOURCES += $(MAGNUM)/portinginterface/sid/src/bsid_priv.c
BSID_SOURCES += $(MAGNUM)/portinginterface/sid/src/bsid_msg.c
BSID_SOURCES += $(MAGNUM)/portinginterface/sid/src/bsid_img.c
BSID_SOURCES += $(MAGNUM)/portinginterface/sid/src/bsid_fw.c
