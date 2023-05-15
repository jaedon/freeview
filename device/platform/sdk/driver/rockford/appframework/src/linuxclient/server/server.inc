#############################################################################
#
#       Copyright (c) 2004-2009, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: server.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 4/28/09 2:41p $
#
# File Description: Builds emulator server 
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/src/linuxclient/server/server.inc $
# 
# Hydra_Software_Devel/1   4/28/09 2:41p jessem
# PR 54380: Initial version.
# 
#############################################################################

APP_INCLUDES += \
	$(EMU_SERVER_DIR)

vpath %.c $(EMU_SERVER_DIR)
APP_SOURCES += \
	server.c
