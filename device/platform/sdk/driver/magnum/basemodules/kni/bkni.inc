############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bkni.inc $
# $brcm_Revision: Hydra_Software_Devel/15 $
# $brcm_Date: 9/6/12 10:28a $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/kni/bkni.inc $
# 
# Hydra_Software_Devel/15   9/6/12 10:28a vsilyaev
# SW7425-3760: Symbolic link removal
# 
# Hydra_Software_Devel/SW7425-3760/2   9/5/12 1:34p vsilyaev
# SW7425-3760: Added top level include path
# 
# Hydra_Software_Devel/SW7425-3760/1   9/5/12 1:02p vsilyaev
# SW7425-3760: Removed symlinks
# 
# Hydra_Software_Devel/14   7/22/11 9:36a agin
# SWNOOS-473:  Rename no-os to no_os.
# 
# Hydra_Software_Devel/13   5/3/11 1:41p erickson
# SW7420-1819: add bare OS support to magnum basemodules
# 
# Hydra_Software_Devel/12   12/27/10 4:11p erickson
# SW7405-705: add export BKNI_TRACK_MALLOCS=y option
#
# Hydra_Software_Devel/11   7/20/10 2:50p jfisher
# SW7572-30:  Add ucos_ii support.
#
# Hydra_Software_Devel/10   1/21/10 10:57a kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/SW35230-56/1   1/18/10 6:42p kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/9   11/20/09 5:23p jgarrett
# SW7408-17: Adding bkernel
#
# Hydra_Software_Devel/8   9/15/09 1:37p erickson
# SW7400-2511: use B_REFSW_OS
#
# Hydra_Software_Devel/7   6/9/09 12:38p vsilyaev
# PR 55554: Fixed WIN32 build
#
# Hydra_Software_Devel/6   5/29/09 4:31p jessem
# PR 54881: Added linuxemu.
#
# Hydra_Software_Devel/5   6/19/08 4:35p cnovak
# PR43697: Clean up uCOS port and move back to Hyrdra branch.
#
# Hydra_Software_Devel/4   10/16/07 1:51p jessem
# PR 36129: Added linuxclient for use with Linux-based IKOS emulation
# platform.
#
# Hydra_Software_Devel/3   7/25/07 11:27a vsilyaev
# PR 32813: Added Windows MINGW support
#
# Hydra_Software_Devel/2   1/12/07 7:24a erickson
# PR25902: For linux kernel, the bkni.c file is compiled separately.
#
# Hydra_Software_Devel/1   1/11/07 2:37p jgarrett
# PR27004: Adding new .inc files
#
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BKNI

# Backward-compatibility
B_REFSW_OS ?= $(OS)

# Options
ifeq ($(BKNI_TRACK_MALLOCS),y)
BKNI_DEFINES += BKNI_TRACK_MALLOCS=1
endif

# Module Includes
BKNI_INCLUDES += $(MAGNUM)/..

ifeq ($(findstring $(B_REFSW_OS), linuxclient linuxemu linuxpal bare win32 no_os wince),${B_REFSW_OS})
BKNI_INCLUDES += $(MAGNUM)/basemodules/kni/linuxuser
ifeq ($(findstring $(B_REFSW_OS), bare ),${B_REFSW_OS})
BKNI_INCLUDES += $(MAGNUM)/basemodules/kni/$(B_REFSW_OS) 
endif
else
BKNI_INCLUDES += $(MAGNUM)/basemodules/kni/$(B_REFSW_OS) 
endif

# Sanity check the OS value
BKNI_SUPPORTED_OS := linuxuser linuxkernel linuxclient linuxemu linuxpal nucleus ucos vxworks win32 bkernel ucos_ii bare no_os
ifeq ($(findstring $(B_REFSW_OS), $(BKNI_SUPPORTED_OS)),)
$(error [kni] OS must be one of the following: $(BKNI_SUPPORTED_OS))
endif

# For linux kernel, the bkni.c file is compiled separately.
ifneq ($(SYSTEM),linuxkernel)
# Include the basic KNI for this OS
ifeq ($(findstring $(B_REFSW_OS), win32),$(B_REFSW_OS))
# win32 has bkni_$(B_REFSW_OS).c as the filename
BKNI_SOURCES += $(MAGNUM)/basemodules/kni/$(B_REFSW_OS)/bkni_$(B_REFSW_OS).c
else
# others are just bkni.c
BKNI_SOURCES += $(MAGNUM)/basemodules/kni/$(B_REFSW_OS)/bkni.c
endif
endif

# Include ISO source for OS environments that use it
BKNI_ISO_OS :=
ifeq ($(findstring $(B_REFSW_OS), ${BKNI_ISO_OS}), $(B_REFSW_OS))
BKNI_SOURCES += $(MAGNUM)/basemodules/kni/$(B_REFSW_OS)/bkni_iso_c.c
endif


