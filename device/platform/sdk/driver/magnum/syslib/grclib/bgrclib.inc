############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bgrclib.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 9/7/12 8:32a $
#
# Module Description:
#
# Revision History:
#
# Created:
#
# $brcm_Log: /magnum/syslib/grclib/bgrclib.inc $
# 
# Hydra_Software_Devel/5   9/7/12 8:32a marcusk
# SW7231-988: Removed symlinks
# 
# Hydra_Software_Devel/SW7231-988/1   9/6/12 8:41a marcusk
# SW7231-988: Removed symlinks
# 
# Hydra_Software_Devel/4   7/28/10 11:53a erickson
# SW7405-3671: add GRCPACKET_SUPPORT env to match GRC
# 
# Hydra_Software_Devel/3   1/8/10 2:11p nissen
# SW7405-3671: Added support for packet blit build.
# 
# Hydra_Software_Devel/2   10/19/07 9:28a dkaufman
# PR36190: Fixed include path
# 
# Hydra_Software_Devel/1   10/18/07 12:48p vobadm
# PR36190:  Adding directories and files
# 
# 
############################################################

# Define this module
MAGNUM_MODULES += BGRCLIB

# Module Includes
BGRCLIB_INCLUDES += $(MAGNUM)/syslib/grclib/include

# Module Sources
ifeq ($(GRCPACKET_SUPPORT),y)
BGRCLIB_DEFINES += GRCPACKET_SUPPORT=1
BGRCLIB_SOURCES += $(MAGNUM)/syslib/grclib/src/bgrclib_packet.c
else
BGRCLIB_SOURCES += $(MAGNUM)/syslib/grclib/src/bgrclib.c
endif

