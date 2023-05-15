############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bdbg.inc $
# $brcm_Revision: Hydra_Software_Devel/17 $
# $brcm_Date: 10/22/12 3:08p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/dbg/bdbg.inc $
# 
# Hydra_Software_Devel/17   10/22/12 3:08p erickson
# SW7435-419: B_REFSW_DEBUG_LEVEL=err should not remove LOG
# 
# Hydra_Software_Devel/16   9/6/12 10:27a vsilyaev
# SW7425-3760: Symbolic link removal
# 
# Hydra_Software_Devel/SW7425-3760/1   9/5/12 12:21p vsilyaev
# SW7425-3760: Removed symlinks
# 
# Hydra_Software_Devel/15   2/22/12 1:57p erickson
# SW7425-2130: fix logic error
# 
# Hydra_Software_Devel/14   2/21/12 1:59p erickson
# SW7425-2130: add new DBG options for compact debug mode
# 
# Hydra_Software_Devel/13   7/22/11 9:28a agin
# SWNOOS-473:  Add no_os.
# 
# Hydra_Software_Devel/12   5/3/11 1:41p erickson
# SW7420-1819: add bare OS support to magnum basemodules
# 
# Hydra_Software_Devel/11   4/4/11 6:14p vsilyaev
# SW7405-5221: Added option to forward debug output to FIFO
# 
# Hydra_Software_Devel/10   7/20/10 2:43p jfisher
# SW7572-30:  Add ucos_ii support.
# 
# Hydra_Software_Devel/9   6/10/10 7:20p vsilyaev
# SW7468-69: Fixed Win32/MINGW build
# 
# Hydra_Software_Devel/9   6/10/10 1:38p vsilyaev
# SW7468-69: Fixed Win32/MINGW build
# 
# Hydra_Software_Devel/9   6/10/10 1:37p vsilyaev
# SW7468-69: Fixed Win32/MINGW build
# 
# Hydra_Software_Devel/8   1/21/10 10:44a kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
# 
# Hydra_Software_Devel/SW35230-56/1   1/18/10 6:39p kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
# 
# Hydra_Software_Devel/7   11/20/09 5:23p jgarrett
# SW7408-17: adding bkernel
# 
# Hydra_Software_Devel/6   9/22/09 5:06p erickson
# SW3548-2493: add standard compile-time debug levels
#
# Hydra_Software_Devel/5   9/15/09 1:37p erickson
# SW7400-2511: use B_REFSW_OS
#
# Hydra_Software_Devel/4   5/29/09 4:37p jessem
# PR 54881: Added linuxemu.
#
# Hydra_Software_Devel/3   10/16/07 1:48p jessem
# PR 36129: Added linuxclient for use with Linux-based IKOS emulation
# platform.
#
# Hydra_Software_Devel/2   7/25/07 11:27a vsilyaev
# PR 32813: Added Windows MINGW support
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
MAGNUM_MODULES += BDBG

# Backward-compatibility
B_REFSW_OS ?= $(OS)

# Module Includes
BDBG_INCLUDES += $(MAGNUM)/basemodules/dbg

# Sanity Check OS provided
BDBG_SUPPORTED_OS := linuxuser linuxkernel linuxclient linuxemu linuxpal vxworks ucos nucleus win32 bkernel ucos_ii bare no_os
ifeq ($(findstring $(B_REFSW_OS), $(BDBG_SUPPORTED_OS)),)
$(error [dbg] OS must be one of the following: $(BDBG_SUPPORTED_OS))
endif

BDBG_SOURCES += $(MAGNUM)/basemodules/dbg/bdbg.c \
                $(MAGNUM)/basemodules/dbg/bdbg_fifo.c 

ifeq ($(findstring $(B_REFSW_OS), linuxclient linuxemu linuxpal),${B_REFSW_OS})
BDBG_SOURCES += $(MAGNUM)/basemodules/dbg/linuxuser/bdbg_os_priv.c
else
BDBG_SOURCES += $(MAGNUM)/basemodules/dbg/$(B_REFSW_OS)/bdbg_os_priv.c
endif

ifeq ($(B_REFSW_OS),vxworks)
BDBG_SOURCES += $(MAGNUM)/basemodules/dbg/$(B_REFSW_OS)/bdbg_output.c
else
BDBG_SOURCES += $(MAGNUM)/basemodules/dbg/bdbg_output.c
endif

#
# Options for reducing binary size:
# B_REFSW_DEBUG=n
#    this is the minimum size, but all error messages are removed. do not use this mode if your system still has bugs.
# B_REFSW_DEBUG=y B_REFSW_DEBUG_LEVEL=err
#    this gives you full BDBG_ERR, BERR_TRACE and BDBG_ASSERT, which are essential for debug
# B_REFSW_DEBUG=y B_REFSW_DEBUG_LEVEL=err B_REFSW_DEBUG_COMPACT_ERR=y
#    this gives you every BDBG_ERR, BERR_TRACE and BDBG_ASSERT, but with a minimal size
#

# compile time level: ERR and LOG, no WRN, MSG, or TRACE
ifeq ($(B_REFSW_DEBUG_LEVEL),err)
BDBG_DEFINES += BDBG_NO_WRN=1 BDBG_NO_MSG=1
endif
# compile time level: ERR, WRN and LOG, no MSG or TRACE
ifeq ($(B_REFSW_DEBUG_LEVEL),wrn)
BDBG_DEFINES += BDBG_NO_MSG=1
endif

# reduce size by reusing a single format string for all errors
ifeq ($(B_REFSW_DEBUG_COMPACT_ERR),y)
BDBG_DEFINES += B_REFSW_DEBUG_COMPACT_ERR=1
endif

ifeq ($(B_REFSW_DEBUG_ASSERT_FAIL),n)
# BDBG_ASSERT will print ERR, but not fail
# the env variable is positive logic, but the macro is negative logic to not introduce a new default
BDBG_DEFINES += B_REFSW_DEBUG_ASSERT_NO_FAIL=1
endif
