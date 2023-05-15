############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#  $brcm_Workfile: blive_ext.mak $
#  $brcm_Revision: 7 $
#  $brcm_Date: 10/7/11 2:17p $
# 
#  Revision History:s
#  $brcm_Log: /BSEAV/lib/livemedia/blive_ext/include/blive_ext.mak $
# 
# 7   10/7/11 2:17p ssood
# SW7425-1436: Rename DEBUG to B_REFSW_DEBUG
# 
# 6   3/24/11 4:35p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 5   3/9/11 7:47p ssood
# SW7420-1617: use B_REFSW_ARCH instead of ARCH for default arch variable
# 
# 4   4/22/10 6:04p mward
# SW7400-2511: don't add B_REFSW_ to ARCH, BSEAV build system uses ARCH.
# 
#  Inputs: BSEAV, B_REFSW_ARCH
#  Outputs: BLIVE_EXT_CFLAGS, BLIVE_EXT_LDFLAGS
############################################################

BLIVE_EXT_DIR = $(BSEAV)/lib/livemedia/blive_ext
ifeq ($(B_REFSW_DEBUG),y)
DEBUG_SUFFIX=debug
else
DEBUG_SUFFIX=release
endif

BLIVE_EXT_LIBDIR = $(BLIVE_EXT_DIR)/lib/$(B_REFSW_ARCH).$(DEBUG_SUFFIX)

BLIVE_EXT_CFLAGS = -I$(BLIVE_EXT_DIR)/include
BLIVE_EXT_LDFLAGS = -L$(BLIVE_EXT_LIBDIR) -lblive_ext
