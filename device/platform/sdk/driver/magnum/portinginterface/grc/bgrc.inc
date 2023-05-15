############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bgrc.inc $
# $brcm_Revision: Hydra_Software_Devel/7 $
# $brcm_Date: 9/7/12 3:02p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/grc/bgrc.inc $
# 
# Hydra_Software_Devel/7   9/7/12 3:02p nissen
# SW7405-5690: Update for new directories.
# 
# Hydra_Software_Devel/6   3/17/11 1:42p erickson
# SW7420-1575: add BGRC_PACKET_VERIFY_SURFACE_RECTANGLE env option
#
# Hydra_Software_Devel/5   7/12/10 2:32p nissen
# SW7405-3671: Added private packet file.
#
# Hydra_Software_Devel/4   1/19/10 1:10p nissen
# SW7405-3671: Changed files for packet blits.
#
# Hydra_Software_Devel/3   1/9/10 11:14a nissen
# SW7405-3671: Build packet blits for 7405 only.
#
# Hydra_Software_Devel/2   1/9/10 11:02a nissen
# SW7405-3671: Added support for packet blits.
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
MAGNUM_MODULES += BGRC

# Module Includes
BGRC_INCLUDES += $(MAGNUM)/portinginterface/grc/include

# Module Sources
ifeq ($(GRCPACKET_SUPPORT),y)
BGRC_SOURCES += $(MAGNUM)/portinginterface/grc/src/bgrc_packet.c
BGRC_SOURCES += $(MAGNUM)/portinginterface/grc/src/bgrc_packet_priv.c
BGRC_DEFINES += BGRC_PACKET_MODE
else
BGRC_SOURCES += $(MAGNUM)/portinginterface/grc/src/bgrc.c
BGRC_SOURCES += $(MAGNUM)/portinginterface/grc/src/bgrc_private.c
BGRC_SOURCES += $(MAGNUM)/portinginterface/grc/src/bgrc_coeff_private.c
BGRC_DEFINES += BGRC_NON_PACKET_MODE
endif

ifeq ($(BGRC_PACKET_VERIFY_SURFACE_RECTANGLE),y)
BGRC_DEFINES += BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE
endif
