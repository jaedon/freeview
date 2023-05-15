############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 7 $
# $brcm_Date: 9/13/12 10:54p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/app/brutus2/build/Makefile $
# 
# 7   9/13/12 10:54p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/13/12 12:38p mphillip
# SW7425-3739: Remove symlinks for linux/driver/build subdirectories and
# dependencies
# 
# 6   3/14/12 3:06p jrubio
# SW7346-644: workaround to remove .py gdb file
# 
# 5   2/27/12 9:17a jrubio
# SW7346-644: fix Makefile
# 
# 4   2/21/12 4:10p jrubio
# SW7346-644: add B_REFSW_ARCH
# 
# 3   2/15/12 12:33p jrubio
# SW7346-644: fix the copy of BE fonts
# 
# 2   2/14/12 5:13p tokushig
# SW7405-5581: remove settop utils from make because of conflict with
# nexus based tspsimgr
# 
# 1   1/12/12 6:59p tokushig
# SW7405-5581: Brutus2 initial check in
# 
# 97405_PlayReady/1   2/28/11 10:56a tokushig
# SW7405-4386: Add playready dat files to samples directory (will be
# included in tar file during make install).
# 
# 164   2/15/11 7:31p xhuang
# SW7358-34: Add 7358/7552 support
# 
# 163   2/15/11 10:34a nitinb
# SW7550-667: Enable SOFT_GFX_SUPPORT only for 7550 A0, A1
# 
# 162   2/11/11 2:12p randyjew
# SW7418-1:Add 7418 Support
# 
# 161   1/7/11 10:31a jrubio
# SW7344-9: add Playback Ip as default
# 
# 160   12/27/10 4:09p erickson
# SW7400-2511: fix
#
# 159   12/27/10 3:37p erickson
# SW7400-2511: fix STATIC_BRUTUS=y builds
#
# 158   12/15/10 10:35a jrubio
# SW7344-9: add 7344/7346 support
#
# 157   12/7/10 1:29p gmohile
# SW7408-190 : enable power management for 7408
#
# 156   12/6/10 4:50p gmohile
# SW7408-190 : Temporarily disable power management for 7408
#
# 155   12/3/10 3:27p gmohile
# SW7408-190 : Add power management supprot for 7408
#
# 154   11/18/10 1:24p gmohile
# SW7408-1 : Enable livemedia support for 7408
#
# 153   10/12/10 11:29a jrubio
# SW7340-216: revert back and disable 7340/7342 power management
#
# 152   10/12/10 10:43a jrubio
# SW7340-216: turn off power management
#
# 151   10/1/10 5:46p spothana
# SW7420-1078 : Add support for BCM97409 platform.
#
# 150   8/19/10 5:55p spothana
# SW7420-970: Copying the DTCP keys to the reference tar ball if
# DTCP_IP_SUPPORT is enabled.
#
# 149   8/4/10 7:14p nitinb
# SW7550-496: Setting SOFT_GFX_SUPPORT conditionally
#
# 148   7/30/10 9:07a erickson
# SW7420-918: support static build for B_REFSW_SHAREABLE=n. also removed
# old magnum-based settop api code.
#
# 147   7/1/10 6:30p mward
# SW7125-38: default on Netaccel support on 7125 family.
#
# 146   5/6/10 12:03p jtna
# SW7420-527: copy libsqlite symlinks properly
#
# 145   5/5/10 5:00p jtna
# SW7420-527: makefile update for libtune
#
# 144   4/23/10 2:08p nickh
# SW7420-709: Remove DBS environment variable
#
# 143   4/23/10 10:51a mward
# SW7125-253: More robust method for finding toolchain library path.
#
# 142   4/19/10 3:38p nickh
# SW7420-709: Add DBS support
#
# 141   4/2/10 6:20p mward
# SW7125-253:  stbgcc-4.4.3-1.0 has mips[el]-linux-uclibc not mips[el]-
# linux directories.
#
# 140   3/31/10 11:53a erickson
# SW7550-277: note that MODE is deprecated
#
# 139   3/31/10 11:53a erickson
# SW7550-277: merge SW7550-277
#
# sw7550_123/2   2/25/10 4:11p rjain
# SW7550-277:  removing PVR_SUPPORT=n
#
# sw7550_123/1   12/16/09 5:14a nitinb
# SW7550_123: For7550, need power management in nexus but not in brutus.
# Make default NEXUS_POWER_MANAGEMENT=y and POWERSTANDBY_SUPPORT=n.
#
# 138   3/30/10 7:48p jtna
# SW7420-527: add libtune echo
#
# 137   3/8/10 11:15a jtna
# SW7420-527: add libtune libraries to tarball
#
# 136   2/11/10 10:45a gmohile
# SW7408-1 : Disable netaccel and livemedia support for 7408
#
# 135   1/20/10 5:12p garetht
# SW7420-553: Add platform for 93380vms
#
# 134   1/19/10 3:06p nickh
# SW7420-541: Rename 97420dvr2 to 97420
#
# 133   12/21/09 2:59p erickson
# SW7420-70: allow some chips to enable POWERSTANDBY_SUPPORT without an
# external power driver
#
# 132   12/11/09 6:12p mphillip
# SW7550-112: Merge 7550 changes to main branch
#
# Refsw_7550/1   12/3/09 12:04a nitinb
# SW7550-64: Add default build variables for 7550
#
# 131   12/9/09 11:53a gmohile
# SW7408-1 : Add 7408 support
#
# 130   12/8/09 3:28p gmohile
# SW7408-1 : Disable Power management for 7408
#
# 129   12/4/09 9:50p randyjew
# SW7468-6: Add 7468/7208 support
#
# 128   11/17/09 3:57p katrep
# SW7405-314:Makefile should bundle pmdriver
#
# 7405_phase65/1   8/13/09 2:33p katrep
# PR57690: Inlcude bcmkni.ko and bcmpmdriver.ko in the final tarball
#
# 127   11/17/09 2:13p erickson
# SW7405-3408: use $(filter) instead of $(findstring)
#
# 126   10/30/09 5:51p nickh
# SW7420-331: Rename 97410c to 97410
#
# 125   10/19/09 3:29p mward
# SW7125-38: default off Netaccel support on 7125 family until it's fully
# tested.
#
# 124   9/24/09 7:10p nickh
# SW7420-70: Disable Powerstandby for 7420
#
# 123   9/22/09 6:40p mward
# SW7400-2435: Default NEXUS_POWER_MANAGEMENT=y for 7400 platforms.
# SW7125-4: Default NEXUS_POWER_MANAGEMENT=n for 7125 platforms.
#
# 122   9/20/09 3:20p nickh
# SW7420-331: Add 7410 support
#
# 121   9/18/09 11:56a erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 120   8/28/09 9:28a gmohile
# SW7403-796 : Disable powerstandby for 7401/7403
#
# 119   8/20/09 11:07a jrubio
# PR55232: turn off IP for 734X
#
# 118   8/18/09 10:14a jrubio
# PR55232: disable power for 7340/7342
#
# 117   7/28/09 10:15a mward
# PR57145:  Set NEXUS_POWER_MANAGEMENT=n for 7400 until new power
# management scheme is fixed/implemented on 7400.
#
# 116   7/15/09 11:22a erickson
# PR56373: add linux kernel mode passive standby support
#
# 115   7/14/09 4:14p erickson
# PR56373: add NEXUS_POWER_MANAGEMENT check
#
# 114   7/14/09 10:18a lwhite
# PR53773: Allow envir variable override of DTCP/IP HW cypher
#
# 113   7/13/09 3:32p jtna
# PR56505: decode util depends on livemedia, which is built in
# Makefile.brutus
#
# 112   7/1/09 12:52p erickson
# PR56373: move POWERSTANDBY_SUPPORT to Makefile so that power driver
# builds could be supported with the same switch
#
# 111   6/30/09 5:10p katrep
# PR56373: Diabled POWERSTANDBY in kernelmode as default option
#
# 110   6/26/09 2:10p erickson
# PR56373: rework brutus passive standby to not exit the app
#
# 109   6/26/09 1:35p ssood
# PR53773: Enabling HW encryption/decryption offload using M2M
#
# 108   6/24/09 4:28p ssood
# PR53773: Adding support for compiling DTCP/IP + SSL libraries
#
# 107   6/17/09 4:55p jrubio
# PR39626: disable Power standby for 7325 after PWR PI rework
#
# 106   2/27/09 3:31p katrep
# PR52599:Add suppport for 7205
#
# 105   2/18/09 5:22p jrubio
# PR52261: enable TUNER_SUPPORT_7335 on by default
#
# 104   12/24/08 12:39p jgarrett
# PR 50703: Disabling PLAYBACK_IP_SUPPORT for 7420 (temp)
#
# 103   12/19/08 7:39p katrep
# PR50567: Add Support for 97466 platform
#
# 102   8/28/08 11:12a jrubio
# PR39626: Add POwerstandby to 7325/7335 by default
#
# 101   8/21/08 4:38p rjlewis
# PR28665: PLAYBACK_IP_SUPPORT no longer works on VxWorks.  Default to
# off until fixed.
#
# 100   8/18/08 12:31p erickson
# PR42789: temp remove hciconfig install
#
# 99   8/5/08 12:53p erickson
# PR42739: simplify PLAYBACK_IP_SUPPORT default code. it now defaults on
# for 93556 and 93548 too.
#
# 98   7/11/08 11:19a katrep
# PR44751: Use KERNELMODE=y to build nexus in kernelmode.
#
# 97   7/7/08 5:12p erickson
# PR44452: use config variable when insmod'ing nexus.ko
#
# 96   6/17/08 3:45p katrep
# PR43636: Add MSDRM_PD_SUPPORT ,MSDRM_ND_SUPPORT flags
#
# 95   6/14/08 2:22p katrep
# PR42739: Enable PLAYBACK_IP_SUPPORT=y for 7405 nexus
#
# 94   6/13/08 2:12p vsilyaev
# PR 42318: Load nexus driver for nexus kernel build
#
# 93   6/10/08 2:08p katrep
# PR42328:Add support for 7405 power management
#
# 92   6/4/08 11:23a rjlewis
# PR40352: the value should always be yes or no.
#
# 91   5/23/08 10:50p ssood
# PR42242: set NETACCEL_STANDALONE flag for netaccel if B_HAS_PLAYPUMP_IP
# is not set
#
# 90   5/21/08 11:59a rjlewis
# PR40352: vxworks: combine the object modules for easier loading.
#
# 89   5/16/08 2:25p jgarrett
# PR 42774: Disabling PLAYBACK_IP_SUPPORT in nexus builds until support
# exists
#
# 88   5/8/08 3:18p ssood
# PR42242: IP Playpump support is being deprecated by accelerated sockets
# i/f
#
# 87   4/28/08 5:06p ssood
# PR42242: build flag changes to allow NetAccel build even when Playpump
# IP is disabled
#
# 86   4/18/08 7:51p mphillip
# PR39626: user-mode power_standby requires bcmkni.ko
#
# 85   4/18/08 7:31p mphillip
# PR39626: Need pm.PLATFORM too for correct module insertion for
# power_standby
#
# 84   4/18/08 6:55p mphillip
# PR39626: Build, copy, and install the power management dependencies.
#
# 83   4/18/08 3:43p mphillip
# PR39626: Set +x on settop.power
#
# 82   4/18/08 3:41p mphillip
# PR39626: Transform CHIP in settop.power, too
#
# 81   4/17/08 11:08a mphillip
# PR39626: Copy power_standby into the install directory
#
# 80   4/3/08 2:46p ssood
# PR40798: Enable IPSTB env variables if PLAYBACK_IP_SUPPORT is set
#
# 79   4/3/08 2:29p mphillip
# PR39626: Copy settop.power when POWERSTANDBY_SUPPORT=y
#
# 78   4/2/08 7:02p mward
# PR40798: Default PLAYBACK_IP_SUPPORT=n for 7400.
#
# 77   3/27/08 11:28a erickson
# PR39626: allow POWERSTANDBY_SUPPORT to be defaulted off
#
# 76   2/27/08 4:20p mphillip
# PR39626: Add power_standby to build process
#
# 75   2/21/08 4:49p jrubio
# PR39814: add netaccel compilation
#
# 74   2/18/08 2:34p ssood
# PR39645: Add NetAccel support in User Mode Builds
#
# 73   2/13/08 4:52p tokushig
# PR39534: added bluetooth remote capabilities to brutus
#
# 72   12/17/07 11:25a katrep
# PR37217: Compile IP as default for 7335
#
# 71   12/10/07 6:47p mward
# PR38078: Create PLATFORM for 97018, 97018RNG
#
# 70   12/10/07 10:16a mward
# PR33903: Fix BCMEMAC_SUPPORT flag for 97118, 97118RNG
#
# 69   11/27/07 10:00a erickson
# PR37590: remove make -q option because it's not working and not
# essential
#
# SanDiego_Brutus_3D/2   12/20/07 4:58p tokushig
# added support for bluetooth remote.  note that user must press and hold
# down <enter> and <start> remote keys before starting brutus and must
# keep them held down until graphics are displayed on screen - this is
# so the remote can pair with the bluetooth usb dongle
#
# SanDiego_Brutus_3D/1   12/5/07 5:10p tokushig
# gfx3d integrated but doesn't play nice with bwin yet.
#
# SanDiego_Brutus_BTRemote/1   12/17/07 1:26p randyjew
# BT remote demo
#
# 68   11/7/07 3:49p jrubio
# PR35015: disable NETACCEL for 7325
#
# 67   11/6/07 5:09p rjlewis
# PR36223: You can build static vxworks library but you don't have to.
#
# 66   10/18/07 10:20a rjlewis
# PR36223: vxworks builds always static.
#
# 65   10/3/07 3:57p jrubio
# PR35015: add 7118RNG support
#
# 64   9/26/07 11:31a ptimariu
# PR25598: added WMS_SUPPORT check
#
# 63   9/20/07 11:36a jrubio
# PR35015: adding 7325 support
#
# 62   8/21/07 10:58a jrubio
# PR33903: added BCMEMAC_SUPPORT flag for non BCMEMAC platforms and for
# USB eth
#
# 61   8/20/07 11:04a mward
# PR33786: Default PLAYBACK_IP_SUPPORT=y for 9745x cable platforms.
#
# 60   8/10/07 12:54a ssood
# PR33786: Net DMA & IP Streamer refactoring: NETACCEL_SUPPORT is enabled
# for kernel mode when B_HAS_PLAYPUMP_IP is enabled
#
# 59   8/2/07 5:06p rjlewis
# PR28665: don't default IPSTB to on for VxWorks yet.
#
# 58   8/1/07 11:07a gmohile
# PR 25109 : re-enable Playback ip support
#
# 57   7/20/07 11:44a erickson
# PR32813: remove PLAYBACK_IP_SUPPORT=y default until code is fixed
#
# 56   7/18/07 8:27a erickson
# PR26252: don't install asfip without PLAYBACK_IP_SUPPORT
#
# 55   7/16/07 5:27p rjlewis
# PR28665: IPSTB defaults to on for VxWorks too.
#
# 54   5/8/07 1:59p jgarrett
# PR 20139: Enabling B_HAS_PLAYPUMP_IP when SMP=y
#
# 53   3/8/07 3:48p jgarrett
# PR 25900: Enabling IP for 7400 B0
#
# 52   3/6/07 5:26p ptimariu
# PR20685: add drm nd install
#
# 51   2/28/07 2:22p erickson
# PR21426: fix clockdebug
#
# 50   2/28/07 11:30a erickson
# PR21426: clockdebug doesn't build for kernel mode and that's ok. don't
# print error message.
#
# 49   2/22/07 4:23p jgarrett
# PR 25900: Disabling playpump IP for B0
#
# 48   2/9/07 4:50p jgarrett
# PR 20139: Disabling B_HAS_PLAYPUMP_IP when SMP=y for compatibility
#
# 47   1/30/07 2:44p ssood
# PR27364: B_HAS_PLAYPUMP_IP not explictly defined for user mode
#
# 45   12/21/06 3:00p ssood
# PR26252: Fixed compile errors due to LIVEMEDIA_SUPPORT left defined
# when PLAYBACK_IP_SUPPORT=n & Compile ASF independent of
# PLAYBACK_IP_SUPPORT
#
# 44   12/19/06 4:28p ssood
# PR26686: B_HAS_PLAYPUMP_IP variable not getting set correctly
#
# 43   12/15/06 12:10p erickson
# PR26252: PLAYBACK_IP_SUPPORT and KERNELMODE_SETTOPAPI require
# B_HAS_PLAYPUMP_IP
#
# 42   12/14/06 6:23p erickson
# PR26252: default PLAYBACK_IP_SUPPORT for RAVE platforms
#
# 41   12/12/06 2:54p ptimariu
# PR24026: added asfip to install
#
# 40   11/30/06 11:34a jgarrett
# PR 20139: Adding conditional build of driver
#
# 39   9/14/06 9:42a jgarrett
# PR 21039: removing clockdebug from 97400 for now
#
# 38   9/6/06 10:50a erickson
# PR21426: build clock_debug for more platforms
#
# 37   8/17/06 12:50p erickson
# PR21941: prevent EXPOSE_SETTOPAPI_INTERNALS when KERNELMODE_SETTOPAPI
# is defined
#
# 36   8/11/06 3:13p erickson
# PR23574: support bigendian linux
#
# 35   8/3/06 1:51a ptimariu
# PR20685: look for wmdrmpd10 as MS DRM dir
#
# PR20685_MSDRM/4   7/28/06 7:07p ptimariu
# PR20685: look for wmdrmpd10 as MS DRM dir
#
# PR20685_MSDRM/3   7/25/06 5:44p ptimariu
# PR20685: added drm dir check
#
# 34   7/24/06 3:36p ptimariu
# PR20685: use MAKE in drm install
#
# 33   7/20/06 6:08p ptimariu
# PR20685: added drm key install
#
# 32   7/19/06 5:39p jgarrett
# PR 19909: Fixing -j submakes
#
# 31   7/14/06 11:29a jgarrett
# PR19909: PR 19909: Fixing extra printout for clock_debug
# PR 19909: Fixing vxWorks noise
# PR 19909: Merging to latest baseline
# PR 19909: Reducing brutus build output
#
# PR19909/4   7/13/06 6:49p jgarrett
# PR 19909: Fixing extra printout for clock_debug
#
# PR19909/3   7/7/06 1:29p jgarrett
# PR 19909: Fixing vxWorks noise
#
# PR19909/2   7/6/06 4:43p jgarrett
# PR 19909: Merging to latest baseline
#
# 30   5/31/06 4:24p tokushig
# PR21891: Merging updated picture viewer into mainline.
#
# SanDiego_Brutus_Skin/1   5/30/06 4:39p tokushig
# merge from main
#
# 29   5/23/06 5:16p rjlewis
# PR19496: don't specify directory.
#
# 27   3/7/06 1:05p erickson
# PR17108: added playback util to default list of utils in make install
#
# PR19909/1   7/6/06 10:51a jgarrett
# PR 19909: Reducing brutus build output
#
# 25   2/15/06 3:16p erickson
# PR17108: fix kernelmode make clean
#
# 24   1/18/06 11:55a rjlewis
# PR19044: support for vxworks.
#
# 23   12/22/05 5:01p vsilyaev
# PR 18019: Added support for the 97401 platform
#
# 22   8/17/05 4:46p erickson
# PR16717: don't insmod 7041 on C boards. remove nonexistent 7042 code.
#
# 21   8/11/05 7:43p mphillip
# PR15139: Fix for proxy build, so that settop.PLATFORM_proxy is in the
# tarball (instead of settop.PLATFORM)
#
# 20   8/4/05 4:44p erickson
# PR15139: consolidated system-specific build configuration into
# BSEAV/build/refsw_inc.mak
#
# 19   7/19/05 2:41p mphillip
# PR15896: Re-order the tar command parameters so that it works on the
# current Fedora Core release.
#
# 18   6/17/05 2:32p mphillip
# PR15896: Adjust install dependencies slightly when building in kernel-
# mode.  This should probably be re-visited post-release.
#
# 17   6/10/05 3:19p erickson
# PR15845: added rm before creating settop.PLATFORM script with sed
#
# 16   5/24/05 11:42a vsilyaev
# PR 15577:  Another round of dependency fixing.
#
# 15   5/24/05 9:44a vsilyaev
# PR 15577: Another round of dependency fixing.
#
# 14   5/23/05 5:02p vsilyaev
# PR 15577: Tied up dependency, so brutus would build after SettoApi has
# build completed,
#
# 13   5/5/05 2:29p erickson
# PR15159: added quotes for DOS builds
#
# 12   3/9/05 9:17a erickson
# PR14384: store big endian/little endian fonts in build directory
#
# 11   3/8/05 8:21p vsilyaev
# PR14343: One more fix for kernel mode build
#
# 10   3/8/05 8:07p vsilyaev
# PR14343: Yet another fix for kernelmode build.
#
# 9   3/8/05 4:22p erickson
# PR14343: fixed install for kernelmode env variable, and added print-
# brutus-tarball rule
#
# 8   3/8/05 11:04a erickson
# PR14343: added env-variable driven kernelmode build process
#
# 7   2/24/05 1:20p erickson
# PR14180: made static/shared option consistent
#
# 6   2/22/05 1:54p erickson
# PR14180: DEBUG is defaults in tools.mak now
#
# 5   2/22/05 11:46a vsilyaev
# PR 13158: Added template handling for settop.${PLATFORM} scripts.
#
# 4   2/17/05 4:21p erickson
# PR14180: vxworks re-bringup
#
# 3   2/17/05 1:53p erickson
# PR9497: make vxworks-friendly
#
# 2   2/12/05 11:11a dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/129   2/10/05 10:01a erickson
# PR13272: applied use of Q_ most thoroughly
#
# Irvine_BSEAVSW_Devel/127   11/10/04 6:22p arbisman
# PR13168: Add support for 97110/7111 under vxWorks
#
# Irvine_BSEAVSW_Devel/126   11/4/04 10:14a erickson
# PR12878: don't add clock_debug rule for non-7038
#
# Irvine_BSEAVSW_Devel/125   10/26/04 5:47p marcusk
# PR12878: Updated makefile to build and install clock_debug when utils
# are built.
#
# Irvine_BSEAVSW_Devel/124   10/22/04 9:38a erickson
# PR13070: added support to build Settop API util without building settop
# api libs if desired
#
# Irvine_BSEAVSW_Devel/123   9/21/04 1:54p marcusk
# PR12708: Updated with install directions.
#
# Irvine_BSEAVSW_Devel/122   9/16/04 3:43p liangl
# PR12680: brutus.cfg needs to be changed to brutus.vxworks.cfg in brutus
# makefile
#
# Irvine_BSEAVSW_Devel/121   7/26/04 6:50p liangl
# PR11643: merging brutus vxworks support
#
# Irvine_BSEAVSW_Devel/120   6/23/04 3:06p erickson
# PR11135: use -d instead of -a because -a preserves root ownership on
# some platforms
#
# Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/5   6/17/04 12:46p liangl
# compiled for vxworks
#
# Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/4   6/10/04 6:39p jjordan
# Fix error introduced by the merge
#
# Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/3   6/10/04 3:44p jjordan
# Merge in preparation for next release
#
# Irvine_BSEAVSW_Devel/119   6/2/04 2:43p erickson
# PR11204: changed/removed deprecated settop api functions
#
# Irvine_BSEAVSW_Devel/117   5/26/04 3:58p erickson
# PR11135: got install procedure working for both user and kernel modes
# for 97038
#
# Irvine_BSEAVSW_Devel/116   5/26/04 11:07a erickson
# PR11135: rearrange some rules and get normal and proxy working
#
# Irvine_BSEAVSW_Devel/115   5/25/04 11:55a erickson
# PR11135: refactored brutus makefiles to be more modular in order to
# support proxy
#
# Irvine_BSEAVSW_Devel/114   5/12/04 10:27a erickson
# PR10967: add standard header
############################################################

#
# Reference Software Makefile
#
# This Makefile will build 3 things
# 1) driver(s)
# 2) settop api (shared or static)
# 3) brutus
#
# PLATFORM is required to be defined.
#

include tools.mak
BSEAV = $(shell cd "../../.." && ${PWD})
NEXUS = $(shell cd "../../../../nexus" && ${PWD})
BINDIR = ${BSEAV}/bin
ifeq ($(PLATFORM),)
PLATFORM = $(NEXUS_PLATFORM)
endif
ifeq ($(DEBUG),)
DEBUG = $(B_REFSW_DEBUG)
endif
ifeq ($(NEXUS_MODE),)
# MODE is deprecated
NEXUS_MODE = $(MODE)
endif

ifeq ($(SYSTEM),vxworks)
APP = brutus.out
else
APP = brutus
endif
DRIVERDIR=$(BSEAV)/${SYSTEM}/driver/build

ifeq ($(SYSTEM),vxworks)
OTHER_MAKES = api
else
OTHER_MAKES = driver api cheapdebug
endif

#nexus Kernel Mode builds
ifeq ($(KERNELMODE),y)
NEXUS_MODE=proxy
export NEXUS_MODE
endif
ifeq ($(NEXUS_MODE),proxy)
KERNELMODE=y
export KERNELMODE
endif

ifeq ($(B_REFSW_ARCH),)
B_REFSW_ARCH ?= mipsel-linux
export B_REFSW_ARCH
endif


ifeq ($(findstring $(PLATFORM), 97550),$(PLATFORM))
ifeq ($(filter $(BCHP_VER), A0 A1),$(BCHP_VER))
ifeq ($(SOFT_GFX_SUPPORT),)
export SOFT_GFX_SUPPORT=y
endif
endif
export NEXUS_USE_SW_FILTER=y
endif

ifeq ($(findstring $(PLATFORM), 97550 97408),$(PLATFORM))
export NEXUS_POWER_MANAGEMENT=y
export POWERSTANDBY_SUPPORT=n
endif

# Passive standby (aka POWERSTANDBY)
# If defaults on for Linux systems.
ifeq ($(filter $(PLATFORM), 97401 97403 97125 97340 97342 97468 97208 97019 97025 97119 97116 97420 97410 97409 93380vms 97344 97346 97418 97358 97552), $(PLATFORM))
ifneq ($(subst Y,y,${NEXUS_POWER_MANAGEMENT}),y)
NEXUS_POWER_MANAGEMENT :=n
endif
endif
ifneq ($(SYSTEM),vxworks)
ifneq ($(NEXUS_POWER_MANAGEMENT),n)
ifeq ($(POWERSTANDBY_SUPPORT),)
POWERSTANDBY_SUPPORT=y
export POWERSTANDBY_SUPPORT
endif
endif
endif
ifeq ($(POWERSTANDBY_SUPPORT),y)
include powerstandby.mak
endif


#enable 7335 tuner by default
ifeq ($(TUNER_ENABLE_7335),)
export TUNER_ENABLE_7335=y
endif

# Nexus uses MSDRM_PD_SUPPORT and MSDRM_ND_SUPPORT
ifeq ($(MSDRM_PD_SUPPORT),y)
DRM_SUPPORT=y
endif

ifeq ($(MSDRM_ND_SUPPORT),y)
DRMND_SUPPORT=y
endif

# These aren't supported in VxWorks.
ifeq ($(SYSTEM),vxworks)
# IP playback only being supported on Linux platforms
PLAYBACK_IP_SUPPORT=n
NETACCEL_SUPPORT=n
NETACCEL_STANDALONE=n
BCMEMAC_SUPPORT=n
endif

# Brutus now defaults to IP Settop mode for most platforms (some exceptions like 97400)
# You can turn this default off with PLAYBACK_IP_SUPPORT=n
ifneq ($(filter $(PLATFORM), 97400 97325 97420 97410 97409 93380vms 97550 ), $(PLATFORM))
ifneq ($(PLAYBACK_IP_SUPPORT),n)
PLAYBACK_IP_SUPPORT=y
export PLAYBACK_IP_SUPPORT
endif
endif

# Enable IPSTB related env variable if PLAYBACK_IP_SUPPORT is on
ifeq ($(PLAYBACK_IP_SUPPORT),y)
ifeq ($(DTCP_IP_SUPPORT),y)
SSL_SUPPORT=y
export SSL_SUPPORT
ifneq ($(DTCP_IP_HARDWARE_DECRYPTION),n)
DTCP_IP_HARDWARE_DECRYPTION=y
endif #DTCP_IP_HARDWARE_DECRYPTION
ifneq ($(DTCP_IP_HARDWARE_ENCRYPTION),n)
DTCP_IP_HARDWARE_ENCRYPTION=y
endif #DTCP_IP_HARDWARE_ENCRYPTION
export DTCP_IP_HARDWARE_ENCRYPTION
export DTCP_IP_HARDWARE_DECRYPTION
export DTCP_IP_SUPPORT
endif
ifeq ($(SSL_SUPPORT),y)
OPENSSL_DIR := $(BSEAV)/lib/openssl/src
export OPENSSL_DIR
endif
# Netaccel (previously called Net dma) support is enabled by default in both kernel & user mode
# Netaccel support provides HTTP/TCP & RTP/UDP Streaming, Accelerated socket i/f
# default off Netaccel support on platforms that don't support it or on which it's not fully tested
ifeq ($(filter $(PLATFORM),97408 97550), $(PLATFORM))
ifneq ($(NETACCEL_SUPPORT),y)
NETACCEL_SUPPORT=n
export NETACCEL_SUPPORT
endif
endif

ifneq ($(NETACCEL_SUPPORT),n)
NETACCEL_SUPPORT=y
export NETACCEL_SUPPORT
ifeq ($(B_HAS_PLAYPUMP_IP),y)
NETACCEL_STANDALONE=n
else
NETACCEL_STANDALONE=y
export NETACCEL_STANDALONE
endif
# Turn off BCMEMAC support for 7118,
ifeq ($(filter $(PLATFORM),97018 97018RNG 97118 97118RNG), $(PLATFORM))
BCMEMAC_SUPPORT=n
else
BCMEMAC_SUPPORT=y
endif
export BCMEMAC_SUPPORT
endif #NETACCEL_SUPPORT
ifneq ($(LIVEMEDIA_SUPPORT),n)
LIVEMEDIA_SUPPORT=y
export LIVEMEDIA_SUPPORT
endif #LM
endif #PB

.PHONY: settop bin_install $(OTHER_MAKES) utils brutus
.PHONY: do_install

#
# The name of the installation tar ball
#
BUILDDATE = `date +%Y%m%d`
BRUTUS_TAR_BALL = refsw-$(BUILDDATE).$(PLATFORM)-$(SYSTEM)
ifeq ($(findstring linux,$(B_REFSW_ARCH)),linux)
BRUTUS_TAR_BALL := $(BRUTUS_TAR_BALL)-uclibc
else
BRUTUS_TAR_BALL := $(BRUTUS_TAR_BALL)-glibc
endif
BRUTUS_TAR_BALL := $(BRUTUS_TAR_BALL).bin.tgz

#
# Set B_REFSW_SHAREABLE based on a variety of flags
#
ifeq ($(filter y,$(STATIC_SETTOPAPI) $(STATIC_BRUTUS)),y)
B_REFSW_SHAREABLE=n
endif
ifeq ($(filter n,$(B_REFSW_SHAREABLE) $(SHAREABLE)),n)
B_REFSW_SHAREABLE=n
endif

# bin_install is the default rule.
# 'all' builds everything but doesn't perform bin_install or install.
# bin_install copies only binaries into BSEAV/bin. It allows you to
#  run from that directory without having your configuration files overwritten.
# install builds an installation tarball in BSEAV/bin.

bin_install: all settop ${BINDIR}
	@echo [Install... binaries]
	${Q_}$(MAKE) -f Makefile.brutus INSTALL_DIR=$(BINDIR) install
ifneq (${B_REFSW_SHAREABLE},n)
	${Q_}$(CP) $(BSETTOP_LIBS) ${BINDIR}
ifeq ($(SYSTEM),linux)
	@(cd ${BINDIR};touch -f ${APP} $(notdir ${BSETTOP_LIBS}))
endif
endif
ifeq ($(SYSTEM),linux)
	${Q_}$(MAKE) INSTALL_DIR=$(BINDIR) -C $(DRIVERDIR) install
endif
ifeq ($(EXTERNAL_POWER_DRIVER),y)
	${Q_}$(MAKE) INSTALL_DIR=$(BINDIR) -f Makefile.power -C $(DRIVERDIR) install
endif

ifeq ($(SYSTEM),vxworks)
ifneq (${B_REFSW_SHAREABLE=n},n)
#       Timesaving step: combine all the objects into a single load module
	@echo "[Combining load modules...]"
ifeq ($(BUILD_SYSTEM),nexus)
#       Note: get lots of errors combining libs and brutus at present time.
	${Q_}$(LD) -r -o libs.out $(BSETTOP_LIBS)
	@$(CP) libs.out ${BINDIR}
else
	${Q_}$(LD) -r -o both.out $(BSETTOP_LIBS) $(BINDIR)/brutus.out
	@$(CP) both.out ${BINDIR}
endif
endif
endif
ifeq ($(MAKECMDGOALS),)
	@echo "Build Complete"
endif

# this must come after bin_install
include $(BSEAV)/api/include/api.mak
include tools.mak

as-lib: check_environment $(OTHER_MAKES) brutus_lib
all: check_environment $(OTHER_MAKES) brutus
clean: check_environment $(addprefix clean-,$(OTHER_MAKES)) clean-utils clean-brutus

print-brutus-tarball:
	@echo $(BRUTUS_TAR_BALL)

${BINDIR}:
	$(Q_)${MKDIR} ${BINDIR}

ifeq ($(BSETTOP_LIBS),)
LIB_STRING = ${APP}
else
LIB_STRING = $(notdir $(BSETTOP_LIBS))
endif

ifeq ($(SYSTEM),vxworks)
settop:
	@echo "No settop script for vxworks"
else
DRIVER := bcmdriver.ko

ifeq ($(BUILD_SYSTEM),nexus)
ifeq ($(NEXUS_MODE),proxy)
	DRIVER:=nexus.ko config=\"$$\{config\}\"
endif
endif

settop: ${BINDIR}
	@${RM} ${BINDIR}/$@
	@sed \
	-e "s/%%LIBS%%/$(LIB_STRING)/g" \
	-e "s/%%BRUTUS%%/${APP}/g" \
	-e "s/%%PLATFORM%%/${PLATFORM}/g" \
	$@ >${BINDIR}/$@
	@chmod +x ${BINDIR}/$@
	@if [ -e settop.${PLATFORM} ]; then \
		$(RM) ${BINDIR}/settop.${PLATFORM}; \
		sed \
		-e "s/%%CHIP%%/$(CHIP)/g" \
		-e "s/%%BCHP_REV%%/$(BCHP_REV)/g" \
		-e "s/%%PLATFORM%%/${PLATFORM}/g" \
		-e "s/%%DRIVER%%/${DRIVER}/g" \
		settop.${PLATFORM} >${BINDIR}/settop.${PLATFORM}; \
	fi
endif

#
# Pass link options into Settop API build.
#
ifneq ($(B_REFSW_SHAREABLE),)
LINK_OPTION = B_REFSW_SHAREABLE=$(B_REFSW_SHAREABLE)
endif

api:
	${Q_}$(MAKE) $(LINK_OPTION) -C $(BSETTOP)/build

clean-api:
	${Q_}$(MAKE) -C $(BSETTOP)/build veryclean
utils: api brutus
#	${Q_}$(MAKE) $(LINK_OPTION) -C $(BSETTOP)/utils/decode application
#	${Q_}$(MAKE) $(LINK_OPTION) -C $(BSETTOP)/utils/getpids application
#	${Q_}$(MAKE) $(LINK_OPTION) -C $(BSETTOP)/utils/userio application
#	${Q_}$(MAKE) $(LINK_OPTION) -C $(BSETTOP)/utils/playback application
clean-utils:
#	${Q_}$(MAKE) -C $(BSETTOP)/utils/decode clean
#	${Q_}$(MAKE) -C $(BSETTOP)/utils/getpids clean
#	${Q_}$(MAKE) -C $(BSETTOP)/utils/userio clean
#	${Q_}$(MAKE) -C $(BSETTOP)/utils/playback clean
driver:
ifeq ($(SYSTEM),linux)
	@if ! $(MAKE) -q -C $(DRIVERDIR); then \
		$(MAKE) -C $(DRIVERDIR); \
	fi
else
	${Q_}$(MAKE) -C $(DRIVERDIR)
endif
ifeq ($(EXTERNAL_POWER_DRIVER),y)
	${Q_}$(MAKE) -f Makefile.power -C $(DRIVERDIR)
endif
clean-driver:
	${Q_}$(MAKE) -C $(DRIVERDIR) clean
ifeq ($(EXTERNAL_POWER_DRIVER),y)
	${Q_}$(MAKE) -f Makefile.power -C $(DRIVERDIR) clean
endif
brutus_lib: api
	${Q_}$(MAKE) -f Makefile.brutus as_lib
brutus: api
	${Q_}$(MAKE) -f Makefile.brutus
clean-brutus:
	${Q_}$(MAKE) -f Makefile.brutus clean
cheapdebug:
ifeq ($(SYSTEM),linux)
	@if ! $(MAKE) -q -C $(BSEAV)/linux/bin/cheapdebug; then \
		$(MAKE) -C $(BSEAV)/linux/bin/cheapdebug; \
	fi
else
	${Q_}$(MAKE) -C $(BSEAV)/linux/bin/cheapdebug
endif
clean-cheapdebug:
	${Q_}$(MAKE) -C $(BSEAV)/linux/bin/cheapdebug clean

INSTALL_DIR = ${BINDIR}/setup
ifeq ($(SYSTEM),vxworks)
PRERENDERED_FONTS = fonts/mswebfonts/be/*.bwin_font
else
ifeq ($(B_REFSW_ARCH),mips-linux)
PRERENDERED_FONTS = fonts/mswebfonts/be/*.bwin_font
else
PRERENDERED_FONTS = fonts/mswebfonts/le/*.bwin_font
endif
endif

#
# This is the main installation rule.
# It will build the binaries and utilities, and then
# perform the actual installation procedure.
#
install: bin_install utils
	${Q_}$(MAKE) do_install

#
# INSTALL_DIR is phony because we always want to remove it and
# recreate it.
#
.PHONY: $(INSTALL_DIR)
$(INSTALL_DIR):
	@$(RM) -rf $(INSTALL_DIR)
	@${MKDIR} $(INSTALL_DIR)

#
# Copy everything into temporary INSTALL_DIR and tar it up.
# The final tarball is left in BINDIR.
#
do_install:  $(INSTALL_DIR) install-binaries install-support-files make-tarball

ifeq ($(SYSTEM), vxworks)
make-tarball:
else
make-tarball:install-binaries install-support-files
endif
	@echo [Build..... tarball]
	@(cd ${INSTALL_DIR};tar -pzckf ${BINDIR}/${BRUTUS_TAR_BALL} .)
	@$(RM) -rf $(INSTALL_DIR)
	@echo ""
	@echo "*********************************************"
	@echo "* Installation tarball created in $(BINDIR)."
	@echo "*"
	@echo "* To complete installation perform the"
	@echo "* following operations on the settop box:"
	@echo "*"
	@echo "* gunzip -c ${BRUTUS_TAR_BALL} | tar xvpf -"
	@echo "* ./settop install"
	@echo "* ./settop brutus"
	@echo "*********************************************"
	@echo ""

#
# Install drivers, libraries and apps
#
.PHONY: install-binaries
install-binaries: $(INSTALL_DIR)
ifneq ($(SYSTEM),vxworks)
	${Q_}$(MAKE) INSTALL_DIR=$(INSTALL_DIR) -C $(DRIVERDIR) install
endif
ifeq ($(EXTERNAL_POWER_DRIVER),y)
	${Q_}$(MAKE) -f Makefile.power  INSTALL_DIR=$(INSTALL_DIR) -C $(DRIVERDIR) install
endif
	${Q_}$(MAKE) -f Makefile.brutus INSTALL_DIR=$(INSTALL_DIR) install
	${Q_}$(MAKE) -f Makefile.brutus INSTALL_DIR=$(INSTALL_DIR) install-config-files
	${Q_}$(MAKE) $(LINK_OPTION) -C $(BSETTOP)/build INSTALL_DIR=$(INSTALL_DIR) install

#
# For some environments, we need to grab libraries from the
# toolchain because they are not present in the rootfs by default.
#

TOOLCHAIN_LIB_DIR:=$(shell dirname `$(CC) -print-file-name=libgcc_s.so` 2>/dev/null)

.PHONY: install-support-files
ifeq ($(SYSTEM),vxworks)
install-support-files:
else
install-support-files: $(INSTALL_DIR)
endif
ifeq ($(findstring linux,$(B_REFSW_ARCH)),linux)
	@echo [Install... toolchain libraries]
	$(Q_)$(CP) -d $(TOOLCHAIN_LIB_DIR)/libstdc++.so* $(INSTALL_DIR)
	$(Q_)$(CP) -d $(TOOLCHAIN_LIB_DIR)/libgcc_s.so* $(INSTALL_DIR)
ifeq ($(BLUETOOTH_SUPPORT),y)
	#$(Q_)$(CP) -d $(BSEAV)/lib/bluetooth/bluez-libs-2.25/src/.libs/sbin/hciconfig $(INSTALL_DIR)
endif
	$(Q_)$(RM) $(INSTALL_DIR)/libstdc++.so.6.0.14-gdb.py
	$(Q_)$(STRIP) --strip-all $(INSTALL_DIR)/libstdc++.so*
	$(Q_)$(STRIP) --strip-all $(INSTALL_DIR)/libgcc_s.so*
endif
	@echo [Install... scripts]
	$(Q_)$(CP) $(BINDIR)/settop $(INSTALL_DIR)
	$(Q_)$(CP) settop.install $(INSTALL_DIR)

ifeq ($(DTCP_IP_SUPPORT),y)
	@echo [Install... dtcp_ip keys]
	@$(CP)  $(NEXUS)/lib/dtcp_ip/data/* $(INSTALL_DIR)
endif

ifeq ($(BUILD_SYSTEM),proxy)
	@if [ -e $(BINDIR)/settop.${PLATFORM}_proxy ]; then ${CP} $(BINDIR)/settop.${PLATFORM}_proxy ${INSTALL_DIR}; fi
else
	@if [ -e $(BINDIR)/settop.${PLATFORM} ]; then ${CP} $(BINDIR)/settop.${PLATFORM} ${INSTALL_DIR}; fi
endif
	@echo [Install... fonts]
	@${MKDIR} $(INSTALL_DIR)/fonts
	@$(CP) $(PRERENDERED_FONTS) $(INSTALL_DIR)/fonts
#	@echo [Install... utils]
#	@$(CP) $(BSETTOP)/utils/decode/decode $(INSTALL_DIR)
#	@$(CP) $(BSETTOP)/utils/playback/playback $(INSTALL_DIR)
#	@$(CP) $(BSETTOP)/utils/getpids/getpids $(INSTALL_DIR)
#	@$(CP) $(BSETTOP)/utils/userio/userio $(INSTALL_DIR)
#	@-$(CP) $(BSEAV)/linux/bin/cheapdebug/cheapdebug $(INSTALL_DIR)
ifeq ($(MSDRM_PRDY_SUPPORT),y)
ifeq ($(MSDRM_PD_SUPPORT),y)
	$(error MSDRM_PD_SUPPORT and MSDRM_PRDY_SUPPORT cannot be enabled at the same time.)
else
	@echo [Install... playready data files]
	@$(CP) $(BSEAV)/lib/playready/samples/dat/*.dat $(INSTALL_DIR)
endif
endif

ifeq ($(DRM_SUPPORT),y)
ifneq ($(shell test -d $(BSEAV)/lib/drm/wmdrmpd10 && echo y), y)
$(error Error, no DRM libraries found! please contact Broadcom for DRM licensing details)
else
	@echo [Install... drm keys]
	${Q_}$(MAKE) -C $(BSEAV)/lib/drm/build INSTALL_DIR=$(INSTALL_DIR) install
endif
endif
ifeq ($(DRMND_SUPPORT),y)
	@echo [Install... drmnd]
	${Q_}$(MAKE) -C $(BSEAV)/lib/drmnd/build INSTALL_DIR=$(INSTALL_DIR) install
endif
ifeq ($(PLAYBACK_IP_SUPPORT),y)
ifeq ($(ASF_SUPPORT),y)
ifeq ($(WMS_SUPPORT),y)
	@echo [Install... asfip]
	${Q_}$(MAKE) -C $(BSEAV)/lib/rtp/asf/build INSTALL_DIR=$(INSTALL_DIR) install
endif
endif
endif
ifeq ($(BRUTUS_LIBTUNE_SUPPORT),y)
	@echo [Install... libtune]
	$(Q_)$(CP) $(BINDIR)/libtune.so $(INSTALL_DIR)
	$(Q_)$(CP) -d $(BINDIR)/libsqlite3.so* $(INSTALL_DIR)
# alternatively, it could be copied from AppLibs/opensource/sqlite/sqlite-3.6.23.1/97420b0/lib/
endif

# DME: Why have a separate install rule? Let's use install: and INSTALL_DIR
ifeq ($(SYSTEM),vxworks)
install_files:
	@${MKDIR} $(TSFS_DIR)
	@${MKDIR} $(TSFS_DIR)/fonts
	@${MKDIR} $(TSFS_DIR)/images
	@${MKDIR} $(TSFS_DIR)/pictures
	@${MKDIR} $(TSFS_DIR)/scripts
	@${MKDIR} $(TSFS_DIR)/videos
	@${MKDIR} $(TSFS_DIR)/skins

	@echo [Install... fonts]
	@$(CP) $(PRERENDERED_FONTS) $(TSFS_DIR)/fonts
	# @$(CP) -r $(BSEAV)/app/brutus/samples/images $(TSFS_DIR)
	# @$(CP) -r $(BSEAV)/app/brutus/samples/skins $(TSFS_DIR)

	# @echo [Install... utils]
	# @$(CP) $(BSEAV)/app/brutus/samples/bcmlogo.png $(TSFS_DIR)
	# @$(CP) $(BSEAV)/app/brutus/samples/programguide.txt $(TSFS_DIR)
	# @$(CP) $(BSEAV)/app/brutus/samples/brutus.vxworks.cfg $(TSFS_DIR)/brutus.cfg
	# @$(CP) $(BSEAV)/app/brutus/samples/sample.info $(TSFS_DIR)
	# @$(CP) $(BSEAV)/app/brutus/samples/$(PLATFORM).channels.txt $(TSFS_DIR)/channels.txt

	@echo ""
	@echo "*******************************************************"
	@echo "* Successful install of Brutus files to $(TSFS_DIR)."
	@echo "*******************************************************"
	@echo ""
endif

