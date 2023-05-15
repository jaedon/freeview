############################################################
#     Copyright (c) 2004-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: platforms.mak $
# $brcm_Revision: 66 $
# $brcm_Date: 10/15/12 12:52p $
#
# Module Description: List of support platforms
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/build/platforms.mak $
# 
# 66   10/15/12 12:52p katrep
# SW7445-1:add support for 7445
# 
# 65   9/13/12 10:54p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/13/12 12:38p mphillip
# SW7425-3739: Remove symlinks for linux/driver/build subdirectories and
# dependencies
# 
# 64   8/20/12 4:22p chengs
# SW7584-42: Add Brutus support for 7584.
# 
# 63   5/18/12 8:07p bselva
# SW7360-19: Add support for 7360 platform
# 
# 62   12/21/11 3:55p mward
# SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
# 
# 61   9/23/11 3:37p katrep
# SW7429-1:7429 support
# 
# 60   2/11/11 2:19p randyjew
# SW7418-1:Add 7418 support
# 
# 59   12/22/10 4:35p katrep
# SW7231-25:add initial support for 7230
# 
# 58   12/15/10 10:43a jrubio
# SW7344-9: add 7344/7346 support
# 
# 57   12/6/10 4:26p katrep
# SW7231-4:initial support
# 
# 56   11/23/10 10:42a xhuang
# SW7358-20: Add 7358 support
# 
# 55   10/1/10 5:55p spothana
# SW7420-1078 : Add support for BCM97409 platform.
# 
# 54   8/30/10 1:49p hongtaoz
# SW7425-8: added 7425 support;
# 
# 53   6/8/10 4:01p nickh
# SW7422-7: Add 7422 support
# 
# 52   1/20/10 12:47p garetht
# SW7420-553: Add platform for 93380vms
# 
# 51   1/19/10 2:48p nickh
# SW7420-541: Rename 97420dvr2 to 97420
# 
# 50   12/8/09 3:09p gmohile
# SW7408-1 : Add 7408 platform
# 
# 49   12/4/09 9:47p randyjew
# SW7468-6: Add 7468/7208 support
# 
# 48   10/30/09 5:28p nickh
# SW7420-331: Rename 97410c to 97410
# 
# 47   10/23/09 1:26p nitinb
# SW7550-29: Add 97550 support
# 
# 46   10/19/09 3:21p erickson
# SW7405-3154: remove unused vars
#
# 45   9/28/09 5:02p lwhite
# SW7468-6: Add 7468 support
#
# 44   9/15/09 5:12p nickh
# SW7420-331: Add 7410 support
#
# 43   8/21/09 2:08p mward
# PR55545: Adding 7125 platforms. LINUX_2_6_PLATFORMS,
# LINUX_2_6_SMP_PLATFORMS.
#
# 42   8/20/09 6:45p mward
# PR55545: Adding 7125 platforms.
#
# 41   8/10/09 8:55a jrubio
# PR55232:add 7342/7340
#
# 40   2/27/09 10:50a katrep
# PR52599:Add suppport for 7205
#
# 39   2/4/09 10:30a jrubio
# PR51629: add 7336 support
#
# 38   12/19/08 10:23a katrep
# PR50567: Add support for 97466
#
# 37   12/14/08 7:21p nickh
# PR50205: Add support for 97420dvr2
#
# 36   7/1/08 1:59p maivu
# PR 44458: Add SMP support for 93549/93556
#
# 35   6/20/08 11:41a anilmm
# PR43453: Add support for 97459 - Merge to main branch
#
# 97459_Phase_2.1/1   6/9/08 6:46p anilmm
# PR43453: Add support for 97459
#
# 34   5/16/08 4:06p jgarrett
# PR 42779: Adding 93549
#
# 33   5/8/08 9:45a jrubio
# PR42353: add 7335 to platforms for nexus
#
# 32   4/24/08 9:10a erickson
# PR39453: add 93548 and 93556
#
# 31   3/4/08 5:53p jrubio
# PR40169: add 7325
#
# 30   12/10/07 6:57p mward
# PR38078: Create PLATFORM for 97018, 97018RNG
#
# 29   11/27/07 9:17a erickson
# PR37590: add 93563
#
# 28   11/21/07 3:43p katrep
# PR37217: Added 97335 platform support.
#
# 27   9/19/07 11:32p jrubio
# PR35015: adding 7325 support
#
# 26   7/28/07 3:10p jgarrett
# PR 33588: Converting back to single include directory for 7400
#
# 25   6/8/07 5:08p mward
# PR24604: 97118RNG no longer has its own kernel config.
#
# 24   5/31/07 3:13p katrep
# PR27643:Added support for 7405
#
# 23   5/18/07 3:48p jgarrett
# PR 30810: Adding chip-rev specific platforms for kernel consistency
#
# 22   2/14/07 6:23p katrep
# PR27643: Add support for 97405.
#
# 21   1/17/07 3:28p anilmm
# PR27129: add 97458A0 platform support
#
# 20   11/30/06 10:20a mward
# PR19738: Add 97435  to linux 2.6 list
#
# 19   10/31/06 9:14a erickson
# PR25109: add 7403 support
#
# 18   10/19/06 3:17p mward
# PR25036: Add 97455, 97456 PLATFORMs to LINUX_2_6_PLATFORMS for build
# system.
#
# 17   9/29/06 4:34p mward
# PR24604: create PLATFORM for 97118RNG.
#
# 16   9/20/06 1:26p gmohile
# PR 24129: Added support for 7313
#
# 15   9/13/06 9:48a erickson
# PR24230: remove 97329
#
# 14   8/3/06 6:22p mward
# PR23262: update 97118 build
#
# 13   7/18/06 8:56a erickson
# PR21941: added 97401 to linux 2.6 list
#
# 12   6/14/06 5:15p jgarrett
# PR 22013: Separating magnum and legacy platforms
#
# 11   5/26/06 9:07a mward
# PR21671: Add support for 7118 chip 97118 board.
#
# 10   4/27/06 5:40p haisongw
# PR 20984: add 97456 platform support
#
# 9   4/20/06 3:20p erickson
# PR19738: 97435 merge
#
# PROD_97435C_REFSW_Devel/1   4/18/06 7:22p gqli
# PR19738: add BCM97435C support
#
# 8   11/23/05 5:11p erickson
# PR18256: added 97455 support
#
# 7   9/16/05 12:29p erickson
# PR17150: remove 97395 and 97327
#
# 6   7/18/05 6:13p vsilyaev
# PR 15377: Refactoring of the board support package
#
# Refsw_97401_Bringup/1   7/12/05 12:20a vsilyaev
# Added support for 97401
#
# 5   3/24/05 12:53p vsilyaev
# PR 14593: 93560 support
#
# Refsw_93560_Bringup/1   3/10/05 6:48p vsilyaev
# Added 93560
#
# 3   3/3/05 4:55p erickson
# PR14180: added better comment
#
# 2   2/22/05 11:58a erickson
# PR14180: updated docs
#
# 1   2/7/05 6:55p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/6   1/27/05 1:25p erickson
# PR13158: added warning
#
# Irvine_BSEAVSW_Devel/5   1/26/05 3:19p bandrews
# PR13158: 97318AVC support
#
# Irvine_BSEAVSW_Devel/4   1/13/05 4:35p vsilyaev
# PR 13823: Added support for 97398 board.
#
# Irvine_BSEAVSW_Devel/2   12/17/04 4:48p vsilyaev
# PR13158: Added 97318
#
# Irvine_BSEAVSW_Devel/1   12/15/04 3:41p erickson
# PR13496: create makefile include file to define valid PLATFORMS
#
############################################################

#
# ATTENTION: If you add anything to the following PLATFORMS list, you must support it
# throughout the entire refsw tree. Do not modify it and check in without doing the following:
#
#   Edit BSEAV/api/include/api.mak and select the appropriate BCHP_CHIP for your PLATFORM
#
#	Look at where BCHP_CHIP is used in all .h and .c files. Modify accordingly.
#	Look at where BCM_BOARD is used in all .h and .c files. Modify accordingly.
#
#   In general, you're done only when brutus builds and runs for your new platform.
#

# Magnum-based platforms
MAGNUM_PLATFORMS = \
	93560 \
	97038 97398 \
	97401 97403 97325 97335 97342 97340 97405 97466 97205 97400 97455 97458 97456 97459 97435 97445\
	97018 97018RNG 97118 97118RNG 97420 93380vms 97410 97409 \
	97125 97025 97119 97019 97116 97550 97468 97208 97408 97422 97425 97358 97552 97231 97230 97418 97344 97346\
        97429 97428 97241 97360 97584


# Legacy (non-magnum) platforms
LEGACY_PLATFORMS = \
	97110 97111 97112 97115 \
	97312 97313 97314 97315 97317 97318 97318AVC \
	97319 97320 97328

NEXUS_PLATFORMS = \
	93563 97325 97335 97342 97340 93549 93556 97466 97205 97405 97468 97208 97550 97408 97418

# Linux 2.4 platforms
LINUX_2_4_PLATFORMS = \
	97038 97398 93560 \
	$(LEGACY_PLATFORMS)

# As support is added in BSEAV/linux/driver/build for your board, add it here.
# A platform can exist in both the 2.4 and 2.6 platform list (not true for magnum vs. legacy)

# IMPORTANT - Only add platforms here that have actual defconfig files.  If you do not, the linux include build scripts will fail.
LINUX_2_6_PLATFORMS = \
	97400 97405 97466 97205\
	97401 97403 \
	97118 \
	97455 97456 97458 97459 \
	97325 97335 97342 97340 \
	97420 97410 97409 \
	97125 97025 97119 97019 97116 97468 97208 97550 97408 97422 97425 97358 97360 97552 97231 97230 97346 97344 97418 \
        97429 97428 97241 97435 97584

# If your platform supports SMP, add it here.
LINUX_2_6_SMP_PLATFORMS = \
	97400 97405 97466 97205 97325 97335 97342 97340 93549 93556 97420 97410 97409 \
	97125 97025 97119 97019 97116 97422 97425 97358 97360 97552 97231 97230 97418 97344 97346\
        97429 97428 97241 97435 97584

# Magnum and legacy are mutually exclusive and define the set of all platforms
PLATFORMS = \
	$(MAGNUM_PLATFORMS) \
	$(NEXUS_PLATFORMS) \
	$(LEGACY_PLATFORMS)

