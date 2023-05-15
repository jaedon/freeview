#############################################################################
#
#       Copyright (c) 2005-2011, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile.apex $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 8/12/11 4:12p $
#
# File Description:
# 	Makefile for compiling apex (APE eXerciser)_unit test application for testing APE PI
#
# $brcm_Log: /rockford/unittests/portinginterface/ape/97422/Makefile.apex $
# 
# 
#############################################################################

# Name of application
ifeq (${SYSTEM},vxworks)
APP = apex.out
else
APP = apex
endif

VPATH += apex

# Application object file list, include path, and C flags
APP_SOURCES = \
  apex.c

APP_INCLUDES = ./ ./apex/
APP_CFLAGS =

include Makefile.common
