###########################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: api.mak $
# $brcm_Revision: 94 $
# $brcm_Date: 8/23/12 2:45p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/include/api.mak $
# 
# 94   8/23/12 2:45p erickson
# SW7401-4517: remove old code
# 
# 93   8/20/12 9:32a erickson
# SW7584-42: merge
# 
# SW7584-42/1   8/20/12 4:36p chengs
# SW7584-42: Add Brutus support for 7584.
# 
# 92   5/18/12 8:48a erickson
# SW7360-19: add 7360
# 
# 91   2/1/12 12:06p erickson
# SW7435-7: merge
# 
# SW7435-7/1   12/21/11 4:06p mward
# SW7435-7: Support Brutus build on (new, 7435-based) platform 97435.
# 
# 90   10/26/11 3:09p jgarrett
# SW7429-1: Adding 97241/97428/97429 support
# 
# 89   5/18/11 1:06p erickson
# SW7420-1264: use NEXUS_BIN_DIR
# 
# 88   5/2/11 4:20p mphillip
# SW7358-34: Merge 7552 support to main
# 
# SW7358-34/1   2/17/11 10:48a xhuang
# SW7358-34: Add 7358/7552 support
# 
# 87   4/27/11 4:37p jgarrett
# SW7358-39: Adding 97358
# 
# 86   4/5/11 11:47a mphillip
# SW7422-412: Merge 7422/7425 codebases
# 
# 85   3/24/11 4:18p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 84   2/21/11 9:57a jgarrett
# SW7418-1: Adding 97418
# 
# 83   12/22/10 5:12p vsilyaev
# SW7231-25: Added 97230 platform
# 
# 82   12/10/10 9:52a jgarrett
# SW7231-4: Adding 7231,7344,7346
# 
# 81   11/18/10 5:04p vsilyaev
# SW7420-1078: Merged BCM97409 support
# 
# SW7420-1078/1   11/11/10 5:57p spothana
# SW7420-1078: Add BCM97409 support.
# 
# 80   11/5/10 11:43a vsilyaev
# SW7425-49: Added 97425 platform
# 
# 79   10/20/10 4:30p vsilyaev
# SW7422-80: Added 7422 platform
# 
# 78   1/21/10 5:36p mphillip
# SW7420-553: Add platform for 93380vms
# 
# 77   1/19/10 6:50p mphillip
# SW7420-541: Rename 97420dvr2 to 97420
# 
# 76   12/11/09 4:57p mphillip
# SW7550-112: Merge 7550 changes to main branch
# 
# Refsw_7550/4   12/11/09 2:24p rjain
# SW7550-112: Merging LATEST FROM  main branch.
# 
# Refsw_7550/3   11/5/09 11:25a nitinb
# sw7550-64: Initial brutus build for 7550
#
# 75   12/10/09 10:53a jgarrett
# SW7468-6: Adding 97468
# 
# 74   11/5/09 11:32a vsilyaev
# SW7408-1 : Add 7408 support
# 
# SW7408-1/1   11/5/09 10:50a gmohile
# SW7408-1 : Add 7408 support
# 
# 73   11/3/09 6:58p vsilyaev
# SW7420-331: Rename 97410c to 97410
# 
# 72   9/24/09 12:18p jgarrett
# SW7125-28: Merge to main branch
# 
# 71   9/16/09 9:02a vsilyaev
# SW7420-331: Added support for 97410c platform
# 
# PR55545/1   8/20/09 7:20p mward
# PR55545: Adding 7125 platforms.
# 
# 70   8/6/09 11:59a jgarrett
# PR 55232: Adding 7340/7342 support
# 
# 69   8/5/09 6:10p vsilyaev
# PR53773: Remove bcrypt dependency for DTCP/IP
# 
# PR53773/2   8/5/09 6:02p lwhite
# PR53773: Remove bcrypt dependency for DTCP/IP
# 
# 68   6/24/09 2:58p vsilyaev
# PR53773: Adding DTCP/IP + SSL Compilation support
# 
# PR53773/1   6/24/09 2:25p ssood
# PR53773: Adding DTCP/IP + SSL Compilation support
# 
# 67   3/26/09 10:56a vsilyaev
# PR 53611: Use simply expanded variables everywhere where $(shell ...)
# is used. It prevents evaluation of $(shell ) every where variable is
# used
# 
# 66   3/13/09 2:53p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
# 
# 65   3/2/09 11:03a jgarrett
# PR 52599: Adding 97205
# 
# 64   2/4/09 10:38a jrubio
# PR51629: add 7336 support
# 
# 63   12/24/08 12:39p jgarrett
# PR 50703: Changes for 7420 bringup
# 
# 62   12/19/08 7:48p vsilyaev
# PR 50567: Added 97466 platform
# 
# 61   11/6/08 10:29a jgarrett
# PR 48700: Bumping version number
# 
# 60   9/29/08 11:56a vsilyaev
# PR47283: LIVEMEDIA libraries conditionaly included
# 
# PR47283/1   9/24/08 10:08a lwhite
# PR47283: LIVEMEDIA libraries conditionaly included
# 
# 59   7/21/08 3:55p jgarrett
# PR 42739: Adding IP support libraries for nexus builds
# 
# 58   6/23/08 5:24p vsilyaev
# PR 43453: Added 97459 platform
# 
# 57   6/12/08 10:09a jgarrett
# PR 42739: Adding nexus IP
# 
# 56   6/11/08 11:53a jgarrett
# PR 43489: Adding brutus support for 93549/93556
# 
# 55   6/10/08 10:50a jgarrett
# PR 40352: Setting nexus library options for vxworks
# 
# 54   5/21/08 2:02p vsilyaev
# PR 40352: VxWorks uses different extension for libraries
# 
# 53   5/16/08 2:25p jgarrett
# PR 42774: Defaulting BUILD_SYSTEM based on nexus
# 
# 52   3/31/08 6:04p jgarrett
# PR 40606: Adding oslib
#
# 97459_Phase_2.1/1   6/9/08 4:04p anilmm
# PR43453: Add support for 97459
# 
# 51   1/21/08 11:41a jgarrett
# PR 38808: Fixing typo in nexus top
# 
# 50   1/18/08 4:54p jgarrett
# PR 38808: Updating nexus location
# 
# 49   1/7/08 11:07a jgarrett
# PR 36068: Adding NEXUS_TOP
# 
# 48   12/14/07 5:49p vsilyaev
# PR38078: PR38078: Separate BCM_BOARD_NO and BCM_BOARD for 97108  -
# handle same as 97118 except for BCM_BOARD_STR.
# PR38078: Create PLATFORM for 97018, 97018RNG
# 
# PR38078/2   12/11/07 4:26p mward
# PR38078: Separate BCM_BOARD_NO and BCM_BOARD for 97108  - handle same
# as 97118 except for BCM_BOARD_STR.
# 
# PR38078/1   12/10/07 7:13p mward
# PR38078: Create PLATFORM for 97018, 97018RNG
# 
# 47   11/28/07 9:39a jgarrett
# PR 36068: Adding 3563
# 
# 46   11/26/07 1:42p jgarrett
# PR 37217: Adding 7335
# 
# 45   10/19/07 3:38p vsilyaev
# PR 35015: Added 7325 platform
# 
# 44   10/16/07 2:33p vsilyaev
# PR 36068: Added NEXUS platform
# 
# 43   1/18/07 10:13a vsilyaev
# PR27129: add 97458A0 platform support
#
# 42   11/17/06 10:43a erickson
# PR25230: move bcmplayer from SetTop/bcmplayer to BSEAV/lib/bcmplayer.
# Remove SetTop from Brutus/Settop API build.
#
# 41   10/31/06 9:15a erickson
# PR25109: add 7403 and 7405
#
# 40   10/3/06 10:26a gmohile
# PR 24129: Added support for 7313
#
# 39   9/29/06 4:29p mward
# PR24604: create PLATFORM for 97118RNG.
#
# 38   9/15/06 5:22p mphillip
# PR24027: set up BSETTOP_LIBS for static builds, too.  This causes
# apps/utilities to be re-linked if libsettop.a has been rebuilt.
#
# 37   8/17/06 4:07p katrep
# PR23114: Get the build going for 97440
#
# 36   6/6/06 3:21p erickson
# PR21875: removed deprecated BCHP_REV support
#
# 35   5/26/06 9:02a mward
# PR21671: Add support for 7118 chip 97118 board.
#
# 34   4/28/06 3:52p dlwin
# PR 20984: add 97456 platform support
#
# 34   4/28/06 3:51p dlwin
# PR 20984: add 97456 platform support
#
# 33   4/20/06 3:15p erickson
# PR19738: 97435 merge
#
# 32   4/7/06 3:50p vsilyaev
# PR 20680: Added bfile library
#
# 31   2/7/06 5:00p erickson
# PR17108: still need BCHP_REV
#
# 30   1/18/06 11:52a rjlewis
# PR19044: linux vxworks builds case sensitive.
#
# 29   1/16/06 11:23a gmullen
# PR18998: Added 97400 support.
#
# 28   12/12/05 7:31p rjlewis
# PR17651: DOS filenames are case insensitive.
#
# 27   11/23/05 5:10p erickson
# PR18256: added 97455 support
#
# 26   10/17/05 2:49p dlwin
# PR 17651: changes for 7411 D0
#
# 25   10/13/05 4:41p dlwin
# PR 17328: Merged the change from IP STB to main-branch.
#
# 24   9/28/05 5:01p erickson
# PR17069: distiguish between 97318 and 97318AVC correctly
#
# 23   9/26/05 3:43p erickson
# PR17229: changed default BCHP_VER and added check for 7038 to avoid
# problems
#
# 22   9/15/05 10:55a erickson
# PR17148: removed unnecessary BCHP_REV_XX define
#
# 21   9/14/05 5:06p erickson
# PR17148: still support BCHP_REV in environment for now
#
# 20   9/14/05 2:16p erickson
# PR17148: replaced BCHP_REV with BCHP_VER, added BCHP_7411_VER where
# needed, removed unused FAMILY And FLAVOR defines
#
# 19   9/7/05 5:06p vsilyaev
# PR 15377: Merged 7401 changes
#
# Refsw_97401_Bringup/2   8/24/05 2:40p vsilyaev
# Backmerge from /main/
#
# 18   8/5/05 2:46p erickson
# PR16583: use BUILD_SYSTEM=proxy for proxy builds so that PLATFORM
# information is available in the proxy layer
#
# 17   8/4/05 4:41p erickson
# PR15139: collected generic build configuration into
# BSEAV/build/refsw_inc.mak
#
# 16   7/18/05 6:14p vsilyaev
# PR 15377: Refactoring of the board support package
#
# Refsw_97401_Bringup/1   7/12/05 1:47a vsilyaev
# Added support for 97401
#
# 15   4/27/05 7:18p wesleyl
# PR15072: fix 97318 vxworks build
#
# 14   4/27/05 2:33p wesleyl
# PR15072: Change to use BSETTOP_LDFLAGS insteads of BSETTOP_LIB
#
# 13   4/27/05 2:25p wesleyl
# PR15072:Fixed conditional build for non-97318 platform
#
# 12   4/26/05 4:57p erickson
# PR14830: back out bad vxworks merge
#
# 11   4/22/05 2:33p erickson
# PR14708: allow BCHP_REV env overrides for DOS
#
# 10   4/15/05 5:26p haisongw
# PR14830: VxWorks 7111 DSG/Brutus integration
#
# 9   4/9/05 4:37p wesleyl
# PR3093: Fixed BSETTOP_MAGNUM_CFLAGS for legacy build
#
# 8   4/9/05 4:18p wesleyl
# PR3093: Add VxWorks support for 97318
#
# 7   4/8/05 6:03p vsilyaev
# PR 14735: Export DEBUG flags as BSETTOP_MAGNUM_CFLAGS.
#
# 6   3/24/05 12:54p vsilyaev
# PR 14593: 93560 support
#
# Refsw_93560_Bringup/1   3/10/05 6:53p vsilyaev
# Added 93560 support
#
# 4   2/22/05 2:04p erickson
# PR14180: no _ between PLATFORM and BCHP_REV in case platform has no
# BCHP_REV
#
# 3   2/22/05 1:52p erickson
# PR14180: added BCHP_REV to BSETTOP_BIN
#
# 2   2/17/05 1:49p erickson
# PR9497: make defs vxworks-friendly
#
# 1   2/7/05 7:04p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
###############################################################

#
# This file should be included by Makefile's in order
# to build the Settop API. Before including, you must
# define PLATFORM and BSEAV.
#
# Then use the following:
# BSETTOP_BIN - location of shared libraries
# BSETTOP_CFLAGS - compiler flags needed to include Settop API headers
# BSETTOP_LDFLAGS - linker flags for using the Settop API library
# BSETTOP_LIBS - shared libraries which must be copied to the target
#

BSETTOP := ${BSEAV}/api
MAGNUM := $(BSEAV)/../magnum
NEXUS_TOP := $(BSEAV)/../nexus

# Get system-specific CFLAGS and LDFLAGS based on SYSTEM, PLATFORM, DEBUG
include $(BSEAV)/build/refsw_inc.mak

BUILD_SYSTEM := nexus

###############################################
#
# Verify PLATFORM and SYSTEM
#

include $(BSEAV)/build/platforms.mak

.PHONY: check_environment
check_environment:
ifndef PLATFORM
        $(error PLATFORM is undefined. Valid platforms are $(PLATFORMS).)
endif
ifeq ($(findstring $(PLATFORM),$(PLATFORMS)),)
        $(error PLATFORM is defined to an unknown value. Valid platforms are $(PLATFORMS).)
endif
ifndef BSEAV
        $(error BSEAV is undefined. You must define this.)
endif
ifeq ($(findstring $(SYSTEM),linux linuxkernel vxworks),)
        $(error SYSTEM is defined to an unknown value $(SYSTEM).)
endif

B_REFSW_ARCH ?= mipsel-linux
BCM_BOARD_NO = $(PLATFORM)

# convert PLATFORM to CHIP
ifeq ($(PLATFORM),97125)
CHIP=7125
endif
ifeq ($(PLATFORM),97025)
CHIP=7125
endif
ifeq ($(PLATFORM),97468)
CHIP=7468
endif
ifeq ($(PLATFORM),97208)
CHIP=7468
endif
ifeq ($(PLATFORM),97119)
CHIP=7125
endif
ifeq ($(PLATFORM),97019)
CHIP=7125
endif
ifeq ($(PLATFORM),97116)
CHIP=7125
endif
ifeq ($(PLATFORM),97420)
CHIP=7420
BCM_BOARD_NO=97420
endif
ifeq ($(PLATFORM),93380vms)
CHIP=7420
BCM_BOARD_NO=97420
endif
ifeq ($(PLATFORM),97410)
CHIP=7420
BCM_BOARD_NO=97410
endif
ifeq ($(PLATFORM),97409)
CHIP=7420
BCM_BOARD_NO=97409
endif
ifeq ($(PLATFORM),97422)
    ifeq ($(BCHP_VER),A0)
    CHIP := 7422
    else
    CHIP := 7425
    endif
BCM_BOARD_NO=97422
endif
ifeq ($(PLATFORM),97425)
CHIP=7425
BCM_BOARD_NO=97425
endif
ifeq ($(PLATFORM),97231)
CHIP=7231
BCM_BOARD_NO=97231
endif
ifeq ($(PLATFORM),97344)
CHIP=7344
BCM_BOARD_NO=97344
endif
ifeq ($(PLATFORM),97346)
CHIP=7346
BCM_BOARD_NO=97346
endif
ifeq ($(PLATFORM),97400)
CHIP=7400
endif
ifeq ($(PLATFORM),97405)
CHIP=7405
endif
ifeq ($(PLATFORM),97408)
CHIP=7408
endif
ifeq ($(PLATFORM),97550)
CHIP=7550
endif
ifeq ($(PLATFORM),97552)
CHIP=7552
BCM_BOARD_NO=97552
endif
ifeq ($(PLATFORM),97584)
CHIP=7584
BCM_BOARD_NO=97584
endif
ifeq ($(PLATFORM),97466)
CHIP=7405
endif
ifeq ($(PLATFORM),97205)
CHIP=7405
endif
ifeq ($(PLATFORM),97325)
CHIP=7325
endif 
ifeq ($(PLATFORM),97335)
CHIP=7335
endif 
ifeq ($(PLATFORM),97336)
CHIP=7336
endif
ifeq ($(PLATFORM),97342)
CHIP=7342
endif
ifeq ($(PLATFORM),97340)
CHIP=7340
endif
ifeq ($(PLATFORM),97435)
CHIP=7435
endif
ifeq ($(PLATFORM),97230)
CHIP=7231
BCM_BOARD_NO=97230
endif
ifeq ($(PLATFORM),97418)
CHIP=7344
BCM_BOARD_NO=97418
endif
ifeq ($(PLATFORM),97358)
CHIP=7358
BCM_BOARD_NO=97358
endif
ifeq ($(PLATFORM),97429)
CHIP=7429
BCM_BOARD_NO=97429
endif
ifeq ($(PLATFORM),97428)
CHIP=7429
BCM_BOARD_NO=97428
endif
ifeq ($(PLATFORM),97241)
CHIP=7429
BCM_BOARD_NO=97241
endif
ifeq ($(PLATFORM),97360)
CHIP=7360
BCM_BOARD_NO=97360
endif


# Default the chip revision to something vanilla.
ifeq ($(BCHP_VER),)
BCHP_VER = A0
endif

ifeq (${SYSTEM},vxworks)
ifneq ($(OSTYPE),linux)
# DOS is case insensitive
PLATFORM_PATH = $(BCHP_VER)
else
# VxWorks on linux is case sensitive
PLATFORM_PATH := $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
endif
else
PLATFORM_PATH := $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
endif

# The location of the generated binaries
BSETTOP_BIN =${BSETTOP}/lib/$(BUILD_SYSTEM)${PLATFORM}$(BCHP_VER)_${B_REFSW_ARCH}.$(DEBUG_SUFFIX)

# SYSTEM-specific include must precede general include dir
BSETTOP_CFLAGS += -I$(BSETTOP)/include/$(SYSTEM)
BSETTOP_CFLAGS += -I${BSETTOP}/include -I${BSEAV}/lib/bfile

# Required Magnum chip definitions based on PLATFORM
BSETTOP_CFLAGS += -DBCHP_CHIP=${CHIP} -DBCHP_VER=BCHP_VER_$(BCHP_VER)
# Settop API board definitions based on PLATFORM
BSETTOP_CFLAGS += -DBCM_BOARD=$(BCM_BOARD_NO)

# add definitions from refsw_inc.mak
BSETTOP_CFLAGS += $(B_REFSW_CFLAGS)
BSETTOP_LDFLAGS += $(B_REFSW_LDFLAGS)
CFLAGS_SHARED = $(B_REFSW_CFLAGS_SHARED)

#
# BSETTOP_LIBS are binaries which must be installed to the target.
# If you are doing a static build, BSETTOP_LIBS is not needed.
#
ifeq (${SYSTEM},vxworks)

BLIB_EXT=out
BSETTOP_LDFLAGS += -nostdlib -r -Wl,-EB -Wl,-X
ifeq ($(SHAREDLIB),no)
BSETTOP_LDFLAGS += -L${BSEAV}/vxworks/driver/build/${PLATFORM}/${B_REFSW_ARCH}.$(DEBUG_SUFFIX)
BSETTOP_LDFLAGS += -L${BSETTOP_BIN} -lsettop
endif

else

ifeq ($(SHAREDLIB),no)
BLIB_EXT=a
BSETTOP_LDFLAGS += ${BSETTOP_BIN}/libsettop.${BLIB_EXT} -lm -lpthread
else
BLIB_EXT=so
BSETTOP_LDFLAGS = -L${BSETTOP_BIN} -lsettop -lm -lpthread
endif

endif

BSETTOP_LIBS = ${BSETTOP_BIN}/libsettop.${BLIB_EXT}

# NEXUS_BIN_DIR must be defined since api.mak doesn't include platform_app.inc.
# this code should stay in sync with nexus_defs.inc
ifeq ($(NEXUS_BIN_DIR),)
# location of final binaries
NEXUS_BIN_DIR = ${NEXUS_TOP}/bin${NEXUS_BIN_DIR_SUFFIX}
endif

ifeq ($(SHAREDLIB),no)
ifeq ($(PLAYBACK_IP_SUPPORT),y)
BSETTOP_LDFLAGS += $(NEXUS_BIN_DIR)/libb_playback_ip.${BLIB_EXT}
endif
ifeq ($(DTCP_IP_SUPPORT),y)
BSETTOP_LDFLAGS += $(NEXUS_BIN_DIR)/libb_dtcp_ip.${BLIB_EXT}
endif
BSETTOP_LDFLAGS += $(NEXUS_BIN_DIR)/libnexus.${BLIB_EXT} $(NEXUS_BIN_DIR)/libb_os.${BLIB_EXT} 
else
ifneq (${SYSTEM},vxworks)
BSETTOP_LDFLAGS += -L$(NEXUS_BIN_DIR) -lnexus -lb_os
ifeq ($(PLAYBACK_IP_SUPPORT),y)
BSETTOP_LDFLAGS += -lb_playback_ip
endif
ifeq ($(DTCP_IP_SUPPORT),y)
BSETTOP_LDFLAGS += -lb_dtcp_ip
endif
ifeq ($(SOFT_GFX_SUPPORT),y)
BSETTOP_LDFLAGS += -lb_softgfx
endif #SOFT_GFX_SUPPORT
endif
endif
BSETTOP_LIBS += $(NEXUS_BIN_DIR)/libnexus.${BLIB_EXT} $(NEXUS_BIN_DIR)/libb_os.${BLIB_EXT}
ifeq ($(PLAYBACK_IP_SUPPORT),y)
BSETTOP_LIBS += $(NEXUS_BIN_DIR)/libb_playback_ip.${BLIB_EXT}
endif
ifeq ($(DTCP_IP_SUPPORT),y)
BSETTOP_LIBS += $(NEXUS_BIN_DIR)/libb_dtcp_ip.${BLIB_EXT}
endif
ifeq ($(SOFT_GFX_SUPPORT),y)
BSETTOP_LIBS += $(NEXUS_BIN_DIR)/libb_softgfx.${BLIB_EXT}
endif #SOFT_GFX_SUPPORT

ifeq ($(PLAYBACK_IP_SUPPORT),y)
ifeq ($(LIVEMEDIA_SUPPORT),y)
LIVEMEDIA_DIR = $(BSEAV)/lib/livemedia/live
BSETTOP_LDFLAGS += ${LIVEMEDIA_DIR}/liveMedia/libliveMedia.a
BSETTOP_LDFLAGS += ${LIVEMEDIA_DIR}/groupsock/libgroupsock.a
BSETTOP_LDFLAGS += ${LIVEMEDIA_DIR}/UsageEnvironment/libUsageEnvironment.a
BSETTOP_LDFLAGS += ${LIVEMEDIA_DIR}/BasicUsageEnvironment/libBasicUsageEnvironment.a
BSETTOP_LDFLAGS += ${LIVEMEDIA_DIR}/../blive_ext/lib/${B_REFSW_ARCH}.$(DEBUG_SUFFIX)/libblive_ext.a
endif #LIVEMEDIA
ifeq ($(SSL_SUPPORT),y)
BSETTOP_LDFLAGS += -L$(OPENSSL_DIR) -lssl -lcrypto -ldl
endif #SSL_SUPPORT
endif #PB_IP
