############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile.inc $
# $brcm_Revision: 133 $
# $brcm_Date: 3/16/10 4:37p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/api/build/magnum/Makefile.inc $
# 
# 133   3/16/10 4:37p mward
# SW7400-2517:  Add calls to BPWR_Open/BPWR_Close required to avoid
# potential GISB timeout issues.  See SW7400-2418.
# 
# 132   3/3/10 2:40p mward
# SW7400-2517: SW7405-2993:  Build xdm.
# 
# 131   12/31/08 10:30a dbayani
# PR 50782: Move SVP functions in application and PI layer to xpt PI
# layer.
# 
# 130   6/20/08 6:07p anilmm
# PR43453: Add support for 97459
# 
# 129   4/25/08 2:30p bandrews
# PR36149: Moved syslib framework impl out of board and into base
# 
# 128   4/25/08 12:16p qcheng
# PR42163: Move BICM_LINUX_USER_MODE_DRV out of bicm.inc
# 
# 127   3/28/08 12:39p bandrews
# PR36149: Moved inclusion of bsyslib_framework.inc to file that builds
# syslib framework impl
# 
# 126   3/28/08 10:32a gmohile
# PR 41022 : Enable bcrypt support for 7405
# 
# 125   3/24/08 4:56p bandrews
# PR40865: Removed build dir and renamed inc file
# 
# 124   2/29/08 2:17p dbayani
# PR 38074: Update to support new SVP structure.
# 
# 123   2/15/08 9:19p bandrews
# PR36149: SETAPI syslib impl
# 
# 122   2/12/08 3:57p mphillip
# PR36787: Add Multi2 support to bstream_set_encryption
# 
# 121   1/3/08 2:09p mphillip
# PR35854: Merge p3d changes
# 
# PR36550/PR35854/1   12/3/07 5:32p mphillip
# PR35854: Update build for kernelmode 3D
# 
# 120   12/21/07 12:24p agin
# PR38430: Support BCM97315FP keypad for BCM97325 and BCM97335.
# 
# 119   12/10/07 6:39p mward
# PR38078: Create PLATFORM for 97018, 97018RNG
# 
# 118   12/7/07 4:28p dbayani
# PR 37935: Updated to support latest svplib.
# 
# 117   12/3/07 5:00p katrep
# PR37217: added 7335
# 
# 116   11/29/07 4:17p gmohile
# PR 34640 : Enable Keyladder for divx Drm
# 
# 115   10/30/07 11:12a mward
# PR36550:  Set HSM defaults to work without restricted code.
# 
# 114   10/29/07 5:55p mphillip
# PR36550: Add B_HAS_KEYLADDER for conditional compilation of non-bcrypto
# keyladder code, and default BCRYPTO_HAS_KEYLADDER to off.
# 
# 113   10/25/07 4:03p rjlewis
# PR34869: add support for 97405 of bcm93520-3431-3418.
# 
# 112   10/3/07 4:36p jrubio
# PR35015: adding 7325 support
# 
# 111   9/19/07 6:52p qcheng
# PR35130: ICAM22 : Change icm.inc from rockford\modules to magnum PI
# directory
# 
# 110   7/26/07 12:38p erickson
# PR32868: added bmsglib
# 
# 109   7/17/07 3:10p gmohile
# PR 33056: Add B_CONFIG_IMAGE support for 7403
# 
# 108   7/16/07 4:51p mphillip
# PR32275: CPS support for recording
# 
# 107   7/11/07 12:36a katrep
# PR27643: pickup brap.inc & bxvd.inc from their PI dirs for 7405.
# 
# 106   6/27/07 5:16p mward
# PR32537: Add B_CONFIG_IMAGE support for  97118[RNG].
# 
# 105   6/14/07 4:00p ahulse
# PR23100: B_CONFIG_IMAGE support for 7400
# 
# 104   6/13/07 6:20p mward
# PR31483: Add 97118RNG bcrypto support
# 
# 103   6/13/07 4:39p mphillip
# PR31483: Add 7118 bcrypto support
# 
# 102   6/12/07 3:00p ahulse
# PR23100: B_CONFIG_IMAGE support for 7401
# 
# 101   5/31/07 3:28p katrep
# PR27643:Updated for 7405 bringup
# 
# 101   5/31/07 3:07p katrep
# PR27643:Added support for 7405
# 
# 100   5/18/07 1:01p gmohile
# PR 30229 : Add B_DECODE_USER_DATA_CAPTURE
# 
# 99   5/11/07 5:16p lyang
# PR 27711: Add 7400B0, 7405A0  support to HSM. Purge and shared *.h
# source udpated
# 
# 
# 97   5/11/07 5:02p jgarrett
# PR30221: PR 25900: Enabling RFM for 7400B0.
# PR 30221: Enabling 3D for 7400
# 
# 96   5/2/07 5:42p haisongw
# PR27129, PR28294, PR25763: Enabling bcrypto for 97456, 97455 and 97458
# platform
#
# 95   4/26/07 3:03p jgarrett
# PR 30221: Adding 3D support for 7400
#
# 94   4/16/07 1:05p erickson
# PR29890: move 7401 code to api/src/CHIP
#
# 93   4/13/07 6:57p jgarrett
# PR 25900: Enabling bcrypto for 7400B0
#
# 92   4/12/07 11:34a ahulse
# PR29145: Add DCS_SUPPORT flag
#
# 91   4/12/07 10:58a ahulse
# PR29145: Add support for DCS
#
# 90   4/12/07 10:29a gmohile
# PR 29188 : add bcrypt support for 7403
#
# 89   3/28/07 5:38p dbayani
# PR 29157: Added SVP support.
#
# 88   3/21/07 11:10a mphillip
# PR26200: bcrypt.inc should be included after the algorithm support is
# configured
#
# 87   3/20/07 2:18p mphillip
# PR26200: Enable key ladder support
#
# 86   3/8/07 2:44p jgarrett
# PR 25900: Enabling HDMI
#
# 85   3/7/07 11:28a mphillip
# PR28440: Temporary workaround -- disable SW support for now
#
# 84   3/6/07 3:10p jgarrett
# PR 25900: Re-enabing VBI for the 7400 b0
#
# 83   2/22/07 4:23p jgarrett
# PR 25900: Disabling some features on 97400 B0
#
# 82   2/15/07 12:51p rjlewis
# PR27791: different library for different os.
#
# 81   2/9/07 2:13p mphillip
# PR27750: Disable software bcrypto in kernel mode
#
# 80   2/8/07 6:00p haisongw
# PR27767: enable SMARTCARD support for 97456 platform
#
# 79   1/11/07 2:32p jgarrett
# PR 27004: Adding transition.inc
#
# 79   1/11/07 1:48p jgarrett
# PR 27004: Adding transition.inc
#
# 78   1/9/07 1:51p jgarrett
# PR 26914: Adding tnr files for 97400
#
# 77   1/4/07 11:24a rjlewis
# PR22341: Vxworks v5 doesn't do ?=
#
# 76   12/21/06 2:19p erickson
# PR25109: add 7403 support
#
# 75   12/7/06 5:04p lyang
# PR 24863: HSM: hide out some HSM information in the makefiles
#
# 74   12/1/06 11:15a erickson
# PR25109: add 7403 support
#
# 73   11/28/06 11:23a mphillip
# PR26154: Build support for 7401 RSA
#
# 72   11/27/06 3:27p mward
# PR23262: Add PCM, Smartcard for 97118.
#
# 71   11/22/06 4:05p qcheng
# PR24859: Add ICAM2.2 support for 7118A0
#
# 70   11/16/06 10:26p qcheng
# PR25950: Add ICAM2.2 support for 7401 C0 and 7118 A0
#
# 69   11/15/06 12:07p gmohile
# PR 24823: add support for 3520_3431_3418 DC on 97401
#
# 68   10/31/06 9:13a erickson
# PR25109: add 7403 support
#
# 67   10/24/06 3:10p gmohile
# PR 24910: Modified to use new LNA PI
#
# 66   9/29/06 5:03p mward
# PR24604: clean up debug.
#
# 65   9/29/06 4:34p mward
# PR24604: create PLATFORM for 97118RNG.
#
# 64   9/15/06 3:01p mphillip
# PR23943: Required define for HSM-dependent BCRYPT
#
# 63   9/14/06 1:30p mphillip
# PR23943: Support BCHP_CHIP for bcrypto_
#
# 61   8/22/06 11:57a mward
# PR21671: Re-enable HSM for 7118.
#
# 60   8/16/06 6:07p mward
# PR21671: Re-enable dma for 7118.
#
# 59   8/11/06 5:57p mphillip
# PR23573: Rename bcrypt_ to bcrypto_
#
# 58   8/11/06 3:49p rgreen
# PR22187: Fix HDCP Key Loader to use standalone HSM PI
#
# 57   8/11/06 3:06p erickson
# PR21941: B_HAS_HSM should always be defined for 740x
#
# 56   8/4/06 4:16p mward
# PR21671: Disable dma for 7118 due to RDB headers issue.  Disable HSM
# for 7118 - not implemented.
#
# 55   8/4/06 2:14a btan
# PR22187: Only support Generic mode by default.
#
# 54   8/2/06 4:48p jgarrett
# PR 20139: Fixing bdma build error for non-7038 platforms
#
# 53   8/2/06 1:21p mphillip
# PR19544: Compilation support
#
# 52   7/27/06 4:04p qcheng
# PR22674: fix smartcard Settop API test example on 7401 A0.
#
# 51   7/6/06 10:44a rjlewis
# PR22341: vxworks ignores the ?= assignments.
#
# 50   6/6/06 5:20p mward
# PR21671: Add support for 7118 chip 97118 board (need kbd, led)
#
# 49   6/6/06 4:06p mward
# PR21671: Add support for 7118 chip 97118 board
#
# 48   5/10/06 9:56a rgreen
# PR21039: Use magnum naming conventions for macros BHDM_CEC_SUPPORT vs
# HDMI_CEC_SUPPORT
#
# 47   5/2/06 3:11p rgreen
# PR20139: Re-Enable compilation for HSM for HDCP
#
# 46   4/28/06 1:55p haisongw
# PR 20984: add 97456 platform support
#
# 45   4/20/06 3:20p erickson
# PR19738: 97435 merge
#
# 44   4/18/06 4:01p bandrews
# PR20120: Defaulted sync to on for all platforms
#
# 43   4/17/06 5:09p jgarrett
# PR 20951: Merging skinned UI into mainline.
#
# SanDiego_Brutus_Skin/2   4/13/06 3:15p tokushig
# merge from main to SanDiego_Brutus_Skin branch
#
# SanDiego_Brutus_Skin/1   3/21/06 10:27a tokushig
# removed -O2 and add -g compilation flags for debugger support
#
# 42   4/11/06 3:55p bandrews
# PR19434: Added conditional compile BSYNC_SUPPORT and runtime flag
# sync_enabled
#
# 41   3/28/06 10:54a jgarrett
# PR 20139: Disabling HSM for 7400 (Doesn't exist yet)
#
# 40   3/24/06 11:23a erickson
# PR20407: HSM_SUPPORT defaults off, but only applies to usage in settop
# api
#
# 39   3/24/06 10:10a erickson
# PR20407: added HSM_SUPPORT, default on, set B_HAS_HSM on cmdline
#
# 38   3/10/06 2:33p jgarrett
# PR 19265: Re-enabling HDMI for the 97400
#
# 37   2/3/06 4:53p jgarrett
# PR 19250: Update Set-Top API for 97400
#
# 36   2/1/06 5:37p rgreen
# PR14359,PR19283: Add HDMI CEC support
# make CEC an optional compile option
#
# 35   1/23/06 10:53p vle
# PR 17423: P3D library is optional component
#
# 34   1/23/06 8:14p rgreen
# PR17748: Modify HDMI HDCP to use HSM for chips that support AEGIS
#
# 33   1/20/06 11:53a erickson
# PR17108: convert filter-out to findstring for 7401/7400 support
#
# 32   1/20/06 10:58a vsilyaev
# PR 17423: OpenGLES library is optional component
#
# 31   1/19/06 9:53a jgarrett
# PR 19105: Create 97400 BSP
#
# Refsw_Devel_7400_A0/2   1/19/06 9:42a jgarrett
# PR 19105: Create BCM97400 Set-Top API BSP
#
# 30   1/14/06 11:41p vle
# PR17812, PR18396: add rfm support for 97401
#
# 29   1/5/06 8:34p qcheng
# PR18909: Add ICAM and keyladder support for 97401
#
# 28   1/5/06 5:48p qcheng
# PR18909: Add ICAM and keyladder support for 97401
#
# 26   11/21/05 2:24p nissen
# PR 17423: Added P3D and OpenGL ES.
#
# 24   10/20/05 5:25p erickson
# PR17108: added 7401 hdmi
#
# 23   10/20/05 11:53a vsilyaev
# PR 17108: Use 7411 specific xvd.inc file
#
# 22   10/20/05 11:42a erickson
# PR17108: get 97401 xvd userdata ready
#
# 21   10/17/05 2:12p erickson
# PR17650: added B_DECODE_CRC_CAPTURE
#
# 20   10/14/05 4:35p vsilyaev
# PR 17607: Added pvrlib for 7401
#
# 19   10/13/05 12:18p erickson
# PR17108: added kpd and led for 97401
#
# 18   9/21/05 9:36p dlwin
# PR 15377: Added support for different types of Raptor cores.
#
# 17   9/16/05 1:15p erickson
# PR17150: remove reference to 97395
#
# 16   9/8/05 6:15p mphillip
# PR16870: Add Macrovision support to the cflags
#
# 15   9/7/05 5:01p vsilyaev
# PR 15377: Merged 7401 changes
#
# Refsw_97401_Bringup/6   9/7/05 3:32p vsilyaev
# Prepare code to merge into the main line
#
# Refsw_97401_Bringup/5   8/25/05 3:28p vsilyaev
# VBI support now optional
#
# Refsw_97401_Bringup/4   8/24/05 7:19p vsilyaev
# Added compile type support for 7401 SettopApi
#
# Refsw_97401_Bringup/3   8/24/05 7:17p vsilyaev
# Added compile type support for 7401 SettopApi
#
# Refsw_97401_Bringup/2   8/24/05 6:34p vsilyaev
# Added compile type support for 7401 SettopApi
#
# Refsw_97401_Bringup/1   8/1/05 5:25p vsilyaev
# Fixed build for 97398 platform
#
# 11   6/3/05 2:53p rgreen
# PR15055: Add HDMI Rx Support
# Add initial interrupt handling, HDMI packet handling/reading, and HDCP
# key loading
#
# 10   4/13/05 11:14a erickson
# PR14784: added 97315 frontpanel keypad support from commondrivers
#
# 9   4/11/05 6:28p dlwin
# PR 14756: Added support DTT760x tuner.
#
# 8   3/30/05 3:15p dlwin
# PR 14653: Add support S/W AGC for BTSC<->IFD
#
# 7   3/25/05 8:11p dlwin
# PR 14615: Added for tuner 1.
#
# 6   3/24/05 12:54p vsilyaev
# PR 14593: 93560 support
#
# Refsw_93560_Bringup/12   3/21/05 4:37p vsilyaev
# Compile smartcard only for platforms where it present.
#
# Refsw_93560_Bringup/11   3/18/05 9:37p vsilyaev
# Moved board specific parts into the board specific files.
#
# Refsw_93560_Bringup/10   3/18/05 8:32p vsilyaev
# Removed HDMI_OUTPUT from the 93560 build
#
# Refsw_93560_Bringup/7   3/17/05 4:01p vsilyaev
# Mega commit for further support of 93560.
#
# Refsw_93560_Bringup/6   3/16/05 5:57p vsilyaev
# Use 3560 specific version of pvrlib.
#
# Refsw_93560_Bringup/5   3/16/05 2:45p vsilyaev
# Put MVD back.
#
# Refsw_93560_Bringup/4   3/16/05 12:52p vsilyaev
# Isolated BSP code.
#
# Refsw_93560_Bringup/3   3/15/05 9:19p vsilyaev
# Removed MPEG decoder (MVD)
#
# Refsw_93560_Bringup/2   3/10/05 7:33p vsilyaev
# Removed modules not created for 3560
#
# Refsw_93560_Bringup/1   3/10/05 6:52p vsilyaev
# Refactored board specific and BSP files
#
# 4   3/9/05 7:44p vsilyaev
# PR 14402: Moved compose into the toplevel makefile.
#
# 3   3/2/05 6:36p vsilyaev
# PR 14305: Added raptor code
#
# 2   2/25/05 5:14p vsilyaev
# PR 14226: Added image related modules.
#
# 1   2/7/05 6:53p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/1   2/7/05 1:29p erickson
# PR13908: split magnum's Makefile.core into two pieces, one for CFLAGS,
# the other for internal source
#
# Irvine_BSEAVSW_Devel/16   2/2/05 4:44p erickson
# PR13908: moved board code into subdir
#
# Irvine_BSEAVSW_Devel/15   2/2/05 4:21p erickson
# PR13908: b_board refactoring
#
# Irvine_BSEAVSW_Devel/14   1/28/05 11:13a erickson
# PR13908: refactor to make external include of Makefile.core correct
#
# Irvine_BSEAVSW_Devel/13   1/26/05 11:25a erickson
# PR13908: made BCM_BOARD_NO public
#
# Irvine_BSEAVSW_Devel/12   1/25/05 5:39p erickson
# PR13908: Settop API makefile rework
#
# Irvine_BSEAVSW_Devel/11   1/25/05 5:12p erickson
# PR13908: Settop API makefile rework
#
# Irvine_BSEAVSW_Devel/10   1/13/05 4:35p vsilyaev
# PR 13823: Added support for 97398 board.
#
# Irvine_BSEAVSW_Devel/9   5/26/04 4:06p erickson
# PR11135: no longer using bkni_task
#
# Irvine_BSEAVSW_Devel/8   5/26/04 11:08a erickson
# PR11135: added bkni_task and bsettop_core_event
#
# Irvine_BSEAVSW_Devel/7   5/12/04 10:23a erickson
# PR10967: added standard header
############################################################

#
# This file defines CFLAGS and other Makefile variables used to build
# Reference Software. It does not defines SRCS or modify vpath.
# This can be included by external applications.
#

include  ${BSEAV}/api/build/magnum/bsp.inc
include $(BSETTOP)/build/magnum/bcm${PLATFORM}.inc

BSETTOP = $(BSEAV)/api

ifdef B_HAS_LEGACY_AV
include  $(ROCKFORD)/modules/aud.inc
include  $(ROCKFORD)/modules/mvd.inc
include  ${ROCKFORD}/modules/pcrlib.inc
else
include  ${ROCKFORD}/modules/${CHIP}/pcrlib.inc
endif

# Don't build these for 740x chips
ifneq ($(findstring $(CHIP),7118 7400 7401 7403 7405 7325 7335), $(CHIP))
include  $(ROCKFORD)/modules/urt.inc
include  $(ROCKFORD)/modules/btslib.inc
endif

include  $(ROCKFORD)/modules/vbilib.inc
include  $(ROCKFORD)/modules/vbi.inc
include  $(ROCKFORD)/modules/grc.inc
include  $(ROCKFORD)/modules/icp.inc
include  $(ROCKFORD)/modules/irb.inc
include  $(ROCKFORD)/modules/kir.inc
include  $(ROCKFORD)/modules/pxl.inc
include  $(ROCKFORD)/modules/rdc.inc
include  $(ROCKFORD)/modules/rsp.inc
include  $(ROCKFORD)/modules/spi.inc
include  $(ROCKFORD)/modules/sur.inc
include  $(ROCKFORD)/modules/vdc.inc
include  $(ROCKFORD)/modules/dma.inc
ifeq ($(MACROVISION_SUPPORT),y)
include  $(ROCKFORD)/modules/vdc_macrovision.inc
else
ifeq ($(DCS_SUPPORT),y)
BVDC_DCS := y
include $(MAGNUM)/portinginterface/vdc/bvdc.inc
else
include  $(ROCKFORD)/modules/vdc_nomacrovision.inc
endif
endif
ifneq ($(CHIP), 7118)
include  $(ROCKFORD)/modules/hdm.inc
endif
BHDM_CEC_SUPPORT ?= n
ifeq ($(BHDM_CEC_SUPPORT),y)
include  $(ROCKFORD)/modules/hdm_cec.inc
endif
ifeq ($(findstring $(CHIP),3560 7118),)
include  ${ROCKFORD}/modules/hdmlib.inc
endif

# 3D specific modules
ifeq ($(findstring $(CHIP),7038 7400), $(CHIP))
ifeq ($(B_GLES_SUPPORT), y)
ifneq ($(SYSTEM),linuxkernel)
include  $(ROCKFORD)/modules/p3d.inc
include  $(ROCKFORD)/modules/opengles.inc
CFLAGS += -DB_GLES_SUPPORT=1
endif
endif
endif

ifeq ($(CHIP), 7038)
# 7038 specific modules
include  $(ROCKFORD)/modules/clock_debug.inc
include  $(ROCKFORD)/modules/ifd.inc

include  ${ROCKFORD}/modules/pvrlib.inc
include  $(ROCKFORD)/modules/rfm.inc
include  $(ROCKFORD)/modules/scd.inc
include  $(ROCKFORD)/modules/xpt.inc
endif

ifeq ($(findstring $(CHIP),7118 7400 7401 7403 7405 7325 7335), $(CHIP))
include  $(ROCKFORD)/modules/xpt.inc
include  $(ROCKFORD)/modules/rfm.inc
include  ${ROCKFORD}/modules/${CHIP}/pvrlib.inc
endif

ifeq ($(findstring $(CHIP),7118 7400 7401 7403 7405 7325 7335), $(CHIP))
include  $(ROCKFORD)/modules/scd.inc
endif

BICM_SUPPORT ?= n
ifeq ($(BICM_SUPPORT), y)

BICM_LINUX_USER_MODE_DRV ?= y
ifeq ($(BICM_LINUX_USER_MODE_DRV), y)
CFLAGS += -DBICM_LINUX_USER_MODE_DRV
endif

#include  $(ROCKFORD)/modules/icm.inc
include $(ROCKFORD)/../magnum/portinginterface/xpt/$(CHIP)/icm/bicm.inc
endif

BSVP_SUPPORT ?= n
ifeq ($(BSVP_SUPPORT), y)
CFLAGS += -D__LINUX__ -DSVP_TEST
include $(ROCKFORD)/../magnum/portinginterface/xpt/$(CHIP)/svp/bsvp.inc
endif

# HSM Module
ifeq ($(findstring $(CHIP),7118 7400 7401 7403 7405 7325 7335), $(CHIP))
# set the defaults to work with the minimal code distribution
CUST ?=G 
BHSM_BSECK ?=OFF
BHSM_KEYLADDER ?=OFF 
BHSM_MISC ?=OFF 
BHSM_OTPMSP ?=OFF

ifeq ($(DIVX_DRM_SUPPORT), y)
BHSM_KEYLADDER = ON
endif

include $(ROCKFORD)/../magnum/portinginterface/hsm/bhsm.inc

CFLAGS += -DBHSM_AEGIS_SUPPORT
CFLAGS += -DB_HAS_HSM=1

endif

ifeq ($(CHIP), 3560)
# 3560 specific modules
include  $(ROCKFORD)/modules/hdr.inc
include  $(ROCKFORD)/modules/hdrlib.inc
include  $(ROCKFORD)/modules/3560/xpt.inc
include  ${ROCKFORD}/modules/3560/pvrlib.inc
endif

ifdef BCHP_XVD_CHIP
ifeq ($(BCHP_XVD_CHIP), 7411)
include  $(ROCKFORD)/modules/${BCHP_XVD_CHIP}/xvd.inc
include  $(ROCKFORD)/modules/${BCHP_XVD_CHIP}/rap.inc
else

ifeq ($(findstring $(PLATFORM),97400 97456),$(PLATFORM))
include  ${MAGNUM}/portinginterface/pwr/bpwr.inc
# XVD defaults off for settops, force it on, so that we can
# power on properly after running a Nexus build
BXVD_POWER_MANAGEMENT := y
endif

ifeq ($(findstring $(CHIP),7400 7401 7403 7405 7118 7325 7335), $(CHIP))

ifeq ($(SYSTEM),linuxkernel)
BRAP_IMG_EXTERNAL := y
BXVD_IMG_EXTERNAL := y
endif
include $(MAGNUM)/portinginterface/xvd/bxvd.inc
include $(MAGNUM)/commonutils/xdm/bxdm.inc
include $(MAGNUM)/portinginterface/rap/brap.inc
else
include  $(ROCKFORD)/modules/xvd.inc
include  $(ROCKFORD)/modules/rap.inc

endif

endif
endif

include $(MAGNUM)/syslib/msglib/bmsglib.inc

# Support for syslib framework
include ${MAGNUM}/syslib/framework/bsyslib_framework.inc

ifdef BCHP_QAM_CHIP
include  $(ROCKFORD)/modules/3250/kpd.inc
include  $(ROCKFORD)/modules/3250/led.inc
include  $(ROCKFORD)/modules/qds.inc
include  $(ROCKFORD)/modules/qob.inc
include  $(ROCKFORD)/modules/qus.inc
include  ${ROCKFORD}/modules/tnr3250ob.inc
endif

ifneq ($(findstring $(PLATFORM), 97325 97335 97398 93560),)
CFLAGS += -I$(ROCKFORD)/commondrivers/kpd/97315fp
vpath %.c $(ROCKFORD)/commondrivers/kpd/97315fp
SRCS += bkpd.c
endif

ifeq ($(findstring $(CHIP),7118 7400 7401 7403 7405), $(CHIP))
include  $(ROCKFORD)/modules/kpd.inc
include  $(ROCKFORD)/modules/led.inc
endif

ifneq ($(findstring $(PLATFORM), 97038 93560 ),)
include  ${ROCKFORD}/modules/tnr.inc
include  ${ROCKFORD}/modules/tnr3418.inc
include  ${ROCKFORD}/modules/lna.inc
include  ${ROCKFORD}/modules/lna3405.inc
endif
ifneq ($(findstring $(PLATFORM), 93560 ),)
include  ${ROCKFORD}/modules/tnrdtt760x.inc
endif

# Support for 3520-3431-3418 tuner card.
ifeq ($(findstring $(PLATFORM),97401 97403 97400 97405),$(PLATFORM))
include  ${ROCKFORD}/modules/tnr.inc
include  ${ROCKFORD}/modules/tnr3418.inc
include  ${ROCKFORD}/modules/lna.inc
include  ${ROCKFORD}/modules/lna3431.inc
endif

# Support for on board 3510/3420 QAM/VSB tuner.
ifeq ($(findstring $(PLATFORM),97405),$(PLATFORM))
include  ${ROCKFORD}/modules/tnr.inc
include  ${ROCKFORD}/modules/tnr3420.inc
include  ${ROCKFORD}/modules/lna.inc
include  ${ROCKFORD}/modules/lna3431.inc
endif

# Default keyladder to off
KEYLADDER_SUPPORT ?= n
ifeq ($(KEYLADDER_SUPPORT),y)
CFLAGS += -DB_HAS_KEYLADDER=1
endif

ifeq ($(findstring $(PLATFORM),97401 97400 97403 97455 97456 97458 97459 97018 97018RNG 97118 97118RNG 97405),$(PLATFORM))
# Enable bcrypto support by default for platforms on which it has been tested and is required
BCRYPTO_SUPPORT ?= y
endif

ifeq ($(BCRYPTO_SUPPORT),y)
# Software support should not be enabled for kernel mode, as that would require linking openssl into the kernel module
ifeq ($(SYSTEM),linuxkernel)
BCRYPTO_SW_SUPPORT ?= n
else
BCRYPTO_SW_SUPPORT ?= n
#currently disabled while software support issues are worked through.
#use this instead for software algorithm support (DRM requires this):
#BCRYPTO_SW_SUPPORT ?= y
endif
BCRYPTO_HW_SUPPORT ?= y
BCRYPTO_DES_SUPPORT ?= y
BCRYPTO_AES_SUPPORT ?= y
BCRYPTO_RSA_SUPPORT ?= y
ifeq ($(BCRYPTO_HW_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_HW
CFLAGS += -DBCRYPT_HW_M2M
BCRYPTO_RC4_SUPPORT ?= n
BCRYPTO_MD5_SUPPORT ?= n
BCRYPTO_SHA1_SUPPORT ?= y
BCRYPTO_CMAC_SUPPORT ?= n
BCRYPTO_KEYLADDER_SUPPORT ?= n
endif
ifeq ($(BCRYPTO_SW_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_SW
CRYPTO_LDFLAGS += ${ROCKFORD}/bcrypt/${BCHP_CHIP}/cryptolib/openssl/lib/${SYSTEM}/libcrypto.a
BCRYPTO_RC4_SUPPORT ?= y
BCRYPTO_MD5_SUPPORT ?= y
BCRYPTO_SHA1_SUPPORT ?= y
BCRYPTO_CMAC_SUPPORT ?= y
BCRYPTO_KEYLADDER_SUPPORT ?= n
endif
include  ${ROCKFORD}/modules/bcrypt.inc
ifeq ($(BCRYPTO_DES_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_DES
endif
ifeq ($(BCRYPTO_AES_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_AES
endif
ifeq ($(BCRYPTO_RC4_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_RC4
endif
ifeq ($(BCRYPTO_MD5_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_MD5
endif
ifeq ($(BCRYPTO_SHA1_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_SHA1
endif
ifeq ($(BCRYPTO_CMAC_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_CMAC
endif
ifeq ($(BCRYPTO_RSA_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_RSA
endif
ifeq ($(BCRYPTO_KEYLADDER_SUPPORT),y)
CFLAGS += -DB_HAS_BCRYPTO_KEYLADDER
endif
endif
ifeq ($(MULTI2_SUPPORT),y)
CFLAGS += -DB_HAS_MULTI2
endif

ifeq ($(INTERNAL_DOCSIS_SUPPORT),y)
CFLAGS += -DINTERNAL_DOCSIS_SUPPORT
endif

CFLAGS += -I$(B_API_IMPL)
CFLAGS += -I$(B_API_IMPL)/board
CFLAGS += -D_POSIX_C_SOURCE=199506 -D_ISOC9X_SOURCE

ifeq ($(MACROVISION_SUPPORT),y)
CFLAGS += -DMACROVISION_SUPPORT
endif

ifeq ($(DCS_SUPPORT),y)
CFLAGS += -DDCS_SUPPORT
endif

ifeq ($(CPS_SUPPORT),y)
CFLAGS += -DCPS_SUPPORT
endif

SRCS +=	$(addsuffix .c, ${notdir ${R_OBJ}})
vpath %.c $(sort $(dir ${R_OBJ}))
CFLAGS += $(addprefix -I, $(sort ${R_INC}))

ifeq ($(B_DECODE_CRC_CAPTURE),y)
CFLAGS += -DB_DECODE_CRC_CAPTURE
endif

ifeq ($(B_DECODE_USERDATA_CAPTURE),y)
CFLAGS += -DB_DECODE_USERDATA_CAPTURE
endif

# vxworks can't handle the ?= tests (need this to default to y)
ifeq ($(SYSTEM),vxworks)
ifeq ($(BSYNC_SUPPORT),)
BSYNC_SUPPORT = y
endif
endif

BSYNC_SUPPORT ?= y
ifeq ($(BSYNC_SUPPORT),y)
CFLAGS += -DBSYNC_SUPPORT
endif

# Include transition.inc file at the end of the build
# Do not fail on this, it should be harmless on older builds
ifeq ($(wildcard $(ROCKFORD)/modules/transition.inc),)
$(warning $(ROCKFORD)/modules/transition.inc is missing.  This may cause problems in your build.)
else
include $(ROCKFORD)/modules/transition.inc
endif

