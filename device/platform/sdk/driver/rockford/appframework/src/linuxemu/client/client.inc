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
# $brcm_Workfile: client.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 5/5/09 4:47p $
#
# File Description: Builds emulator client 
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/src/linuxemu/client/client.inc $
# 
# Hydra_Software_Devel/1   5/5/09 4:47p jessem
# PR 54380: Initial version.
# 
#############################################################################

APP_INCLUDES += \
	$(EMU_CLIENT_DIR)

vpath %.c $(EMU_CLIENT_DIR)
APP_SOURCES += \
	client.c \
	pagelib.c \
	mutex.c \
	intmgr.c \
	memmgr.c \
	main.c
	
