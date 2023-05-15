############################################################
#     (c)2003-2012 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile: platform.inc $
# $brcm_Revision: SW7435-433/3 $
# $brcm_Date: 12/3/12 4:50p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/platforms/common/build/platform.inc $
# 
# SW7435-433/3   12/3/12 4:50p mward
# SW7436-1105:  Default NEXUS_POWER_MANAGEMENT=y for 97346, 97344
# 
# SW7435-433/2   11/28/12 4:12p jessem
# SW7429-383: Enabled power management by default to 7429, 7428, 7241.
#
# SW7435-433/1   11/1/12 5:07p mward
# SW7429-339: Added support for 7241_80211ACUSFF.
#
# 212   9/28/12 1:37p jzhu
# SW7435-246:3335 support.
#
# 211   8/31/12 2:54p erickson
# SW7425-3418: rework user mode callbacks: filter redundant callbacks on
#  server. sync NEXUS_StopCallbacks across all callback threads.
#
# 210   8/24/12 4:41p erickson
# SW7435-235: b_objdb requires BSEAV/lib/utils
#
# 209   8/23/12 10:13a erickson
# SW7435-235: remove symlinks
#
# 208   8/21/12 3:35p erickson
# SW7435-235: remove symlinks
#
# 207   8/21/12 2:12p erickson
# SW7435-235: remove symlinks
#
# 206   8/10/12 11:43a erickson
# SW7425-2734: fix WEBCPU client build
#
# 205   8/1/12 4:00p erickson
# SW7425-2734: allow server to limit resources for untrusted clients
#
# 204   7/27/12 2:14p jgarrett
# SW7425-3281: Merge to main branch
#
# 203   7/24/12 10:04a erickson
# SW7435-235: refactor directories to remove symbolic links
#
# 202   7/17/12 5:27p mward
# SW7425-2690:  Default on NEXUS_POWER_MANAGEMENT for 97425 and 93380sms.
#
# 201   7/17/12 2:04p mward
# SW7425-2690:  Default on NEXUS_POWER_MANAGEMENT for 97125 97019 97435.
#
# 200   7/16/12 3:55p vishk
# SW7231-895: Add support for 97231B2 EUSFF_V20 board.
#
# SW7425-3281/2   7/18/12 9:40a jgarrett
# SW7425-3281: Merging platform changes for 7231 release
#
# 199   7/2/12 10:28a katrep
# SW7429-183:Cleaned up platform define and build issue with no frontend
#
# 198   6/26/12 3:55p vishk
# SW7429-183: Add support for 7241DCSFBTSFF board
#
# SW7425-3281/1   6/20/12 6:06p piyushg
# SW7425-3281: Add "wmdrmpd", "wmdrmpd_core" and "wmdrmpd_io" modules to
#  nexus platform.
#
# 197   6/11/12 11:20p jessem
# SW7425-3225: Update 97422 VMS support.
#
# 196   6/4/12 4:07p erickson
# SW7435-27: clarify comments
#
# 195   5/31/12 5:03p vishk
# SW7231-828: Merge to mainline
#
# AlphaBoxee/1   5/31/12 4:07p vishk
# SW7231-828: Please provide ATSC(MX603+BCM3517) tuner driver.
#
# 194   5/23/12 2:12p bselva
# SW7360-19: Add support for 7360 platform
#
# 193   5/18/12 5:00p mward
# SW7435-165:  NEXUS_PLATFORM_7435_1U4T no longer needed.
#
# 192   5/16/12 11:59a erickson
# SW7231-748: no thunks for NEXUS_BASE_ONLY
#
# 191   5/9/12 3:20p erickson
# SW7231-748: enable platform module lock
#
# 190   5/9/12 10:55a erickson
# SW7435-27: rework NEXUS_WEBCPU env variable to support core0
#  client/server, core1_server, and eventually core1_client
#
# 189   5/8/12 5:34p erickson
# SW7435-27: add webcpu support
#
# 188   5/7/12 6:19p vishk
# SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
#
# 187   3/28/12 6:35p mward
# SW7435-64:  Add NEXUS_BDSP_VP6_SUPPORT to NEXUS_PLATFORM_DEFINES if
#  BDSP_VP6_SUPPORT=y, to differentiate platforms (such as 7435) which
#  can decode VP6 in AVD.
#
# 186   3/28/12 3:36p hongtaoz
# SW7435-7: condition 1u4t build for 97435 platform;
#
# 185   3/22/12 6:08p randyjew
# SW7418-86:Add 7418SFF_RVU support
#
# 184   3/19/12 7:07p nickh
# SW7429-83: Add NEXUS_PLATFORM_7429_CABLE to support legacy OOB
#
# 183   3/16/12 2:47p mward
# SW7435-7:  export NEXUS_PLATFORM_7435_1U4T=n to build for the 1u2t or
#  1u1t use case / CFE (default to 4 transcode mode).
#
# 182   3/12/12 4:41p gkamosa
# SW7425-2543: Merge support for 3383 SMS platform
#
# Centaurus_Phase_1.0/2   3/6/12 6:03p gkamosa
# SW7425-2337: Merge branch to tip
#
# 181   3/4/12 3:57p mward
# SW7435-7:  export NEXUS_PLATFORM_7435_1U4T=y to build for the 1u4t use
#  case / CFE.
#
# 180   2/24/12 1:50p jgarrett
# SW7425-2081: add platform_cable_frontend extension
#
# 179   1/24/12 2:44p vsilyaev
# SW7425-2258: Added Nexus video decoder extension to decode MJPEG
#
# 178   1/16/12 6:06p gmohile
# SW7425-1473 : Merge raaga encoder support
#
# 177   12/22/11 9:36a erickson
# SW7420-1148: nexus source always needs private include to
#  NEXUS_SYNCTHUNK_DIR, if only for nexus_class_verification.h stub
#
# Centaurus_Phase_1.0/1   2/6/12 5:56p jmiddelk
# SW7425-2337: Adding support for 93383sms
#
# 176   12/18/11 6:44p prasadv
# SW3128-56: Clean up. Remove 3124 support. Auto detection has been added
#  to take care of 312x.
#
# 175   12/1/11 4:13p mphillip
# SW7425-1304: Change 4528 to HAB
#
# 174   11/9/11 1:38p erickson
# SW7420-1148: NEXUS_SYNCTHUNK_DIR
#
# 173   11/8/11 6:06p jgarrett
# SW7422-400: Adding audio debug log support in kernel mode
#
# 172   11/7/11 4:00p bandrews
# SW7231-446: merge to main
#
# SW7231-446/1   11/4/11 10:13p bandrews
# SW7231-446: remove legacy defines
#
# 171   11/3/11 3:14p bandrews
# SW7231-391: default enum index reversion macros to on until problems
#  are cleared up elsewhere
#
# 170   10/31/11 7:06p bandrews
# SW7231-391: merge to main
#
# SW7420-2078/1   10/25/11 7:27p bandrews
# SW7231-409: added compile flags for legacy parser band and timebase
#  enum usage
#
# 169   10/31/11 4:38p mward
# SW7435-7:  Add 97435.
#
# 168   10/25/11 3:35p mward
# SW7346-517: don't use kbuild for linux 2.6.18
#
# 167   10/21/11 4:51p erickson
# SW7346-517: use kbuild for nexus.ko
#
# 166   10/17/11 7:24p prasadv
# SW3128-56: Add 3124 Support
#
# 165   10/12/11 3:06p khat
# SWNOOS-486: Define DIAGS and DIAGS_MEM_DMA_TEST module
#
# 164   10/6/11 4:54p randyjew
# SW7344-198: Add NEXUS_PLATFORM_7344SV for 4506 support
#
# 163   10/3/11 11:02a randyjew
# SW7344-104: Disable Power Mangement default for 7344/7418/7346
#
# 162   9/15/11 2:07p ttrammel
# SW7425-1294: Add missing libgcc file to NFE build.
#
# 161   9/2/11 12:39p erickson
# SW7420-1148: move NEXUS_SERVER_SUPPORT to nexus_defs.inc for general
#  use
#
# 160   8/24/11 6:44p randyjew
# SW7468-6: 7208 default to Power Management Disable
#
# 159   8/24/11 5:07p randyjew
# SW7344-140: Adding 7418SFF_H Platform
#
# 158   8/18/11 2:25p mward
# SW7420-1123:  Don't force GL_SUPPORT=n if NEXUS_SERVER_SUPPORT=y, since
#  that setting defaults on.
#
# 157   8/17/11 5:12p bselva
# SW7358-92: Add the changes required for HDI port
#
# 156   8/15/11 11:27a mward
# SW7420-1123:  GL_SUPPORT (3D via PX3D) is not supported in usermode
#  multiprocess.  Force GL_SUPPORT=n for NEXUS_MODE=client, and if
#  NEXUS_SERVER_SUPPORT=y.
#
# 155   8/3/11 5:33p vsilyaev
# SW7420-1465, SW7405-5221: Route debug output to the special FIFO
#  instead of syslog buffer
#
# 154   7/22/11 9:52a agin
# SWNOOS-473:  Add no_os.
#
# 153   6/22/11 7:09p nickh
# SW7425-533: Disable Power management for 7425 until its confirmed to be
#  working
#
# 152   6/20/11 5:41p jgarrett
# SW7425-409: Merging VP6 support to main branch
#
# 151   6/20/11 1:03p randyjew
# SW7344-104: Add for 7344/7346  PM 2.0
#
# 150   6/20/11 2:12p jhaberf
# SWDTV-6551: Force ISDBT to be built for the 35233
#
# 149   6/17/11 1:44p jhaberf
# SWDTV-7435: adoption of Power Management 2.0 for the 35233
#
# 148   6/13/11 12:18p spothana
# SW7425-622:Add simul usage of BCM3128 and DOCSIS tuners.
#
# 147   6/9/11 7:21p nickh
# SW7425-533: Enable Power Management for 7425
#
# SW7425-409/1   6/6/11 7:16p jgarrett
# SW7425-409: Adding DSP video decoder extensions for VDE
#
# 146   6/6/11 3:35p ttrammel
# SW7420-1819: Remove Linux references and fix changed file paths.
#
# 145   6/6/11 11:05a ttrammel
# SW7420-1819: Update NEXUS/pi for NFE 2.0.
#
# 144   6/2/11 9:51a erickson
# SW7550-284: fix NEXUS_BASE_ONLY
#
# 143   6/1/11 5:31p erickson
# SW7550-284: fix NEXUS_BASE_ONLY
#
# 142   5/31/11 7:31p randyjew
# SW7208-158: Add 97208 to nexus pwr
#
# 141   5/24/11 3:22p gmohile
# SW7231-128 : Enable Power management for 7231
#
# SW7231-128/2   5/20/11 12:11p gmohile
# SW7231-128 : Enable power management for 7231
#
# SW7231-128/1   5/20/11 12:07p gmohile
# SW7231-128 : Enable power management for 7231
#
# 140   5/17/11 12:02p jhaberf
# SWDTV-6928: Merge of 35233 platform code to the main
#
# 139   5/12/11 1:15p erickson
# SW7420-1819: fix OS=linuxkernel with no MODE=driver
#
# 138   5/3/11 5:08p erickson
# SW7420-1819: repartition so NFE is a standard feature
#
# 137   4/18/11 10:47a spothana
# SW7425-369: Add NEXUS_DOCSIS_OOB_SUPPORT and NEXUS_DOCSIS_IB_SUPPORT
#
# 136   4/12/11 2:37p erickson
# SW7420-1123: fix
#
# 135   4/12/11 2:22p erickson
# SW7420-1123: only default NEXUS_SERVER_SUPPORT=y for usermode
#
# 134   4/11/11 12:04p erickson
# SW7420-1123: default NEXUS_SERVER_SUPPORT=y
#
# 133   4/4/11 10:21a erickson
# SW7420-1671: add special nexus_driver.c dependency
#
# 132   4/1/11 9:59a erickson
# SW7420-1671: refactor object database for multiprocess stress
#
# 131   3/22/11 5:00p randyjew
# SW7344-35: Add btst_video_shmoo.c/.h to 7344a0 only
#
# 130   3/17/11 1:43p vishk
# SW7231-32: Implement the Nexus code to support the 3461 frontend chip
#
# 129   3/17/11 11:49a vsilyaev
# SW7422-216: Drive ZSP_VIDEO support from the RAP defines
#
# 128   3/16/11 1:38p VISHK
# SW7231-76: Add support for FBTSFF 97231 + 3128 board.
#
# 127   3/16/11 1:29p vishk
# SW7231-76: Add support for FBTSFF 97231 + 3128 board.
#
# 126   3/9/11 3:08p hongtaoz
# SW7422-171: compile 3128 only if the board supports it;
#
# 125   3/2/11 10:46a erickson
# SW7422-310: dynamic fake address mapping scheme using nexus_map.c
#
# 124   3/1/11 12:17p spothana
# SW7420-1550: Add NEXUS_ZORDER_PIP_SWAP
#
# 123   2/28/11 1:10p jessem
# SW7422-295: Added btst_video_shmoo.c/.h to 7422a0 only.
#
# 122   2/28/11 11:28a jessem
# SW7425-113: Added btst_video_shmoo.c/.h to 9752a0 build only.
#
# 121   2/25/11 2:52p gmohile
# SW7408-210 : Deprecate use of NEXUS_POWER_STANDBY
#
# 120   2/18/11 1:56p hongtaoz
# SW7425-9: merge the 7425 support;
#
# 119   2/17/11 10:38p nickh
# SW7422-10: Add 7422 and 7425 support
#
# 118   2/17/11 4:24p gmohile
# SW7408-210 : Merge PM 2.0 support to mainline
#
# 117   2/15/11 4:01p erickson
# SW7420-1491: support local functions in ipc client
#
# 116   2/15/11 3:24p erickson
# SW7420-1491: use attr{local=true} to get rid of code duplication
#
# 115   1/19/11 3:42p erickson
# SW7420-1123: prevent unsupported combination
#
# 114   1/19/11 2:03p erickson
# SW7420-1123: socket-based usermode IPC
#
# 113   1/19/11 3:19p xhuang
# SW7358-21: remove PCI for 7358/7552
#
# 112   1/11/11 9:59a erickson
# SW7346-8: add NEXUS_PLATFORM_COMMON_DIR
#
# 111   12/21/10 6:46p katrep
# SW7231-25:add support for 97230
#
# 110   12/20/10 4:26p erickson
# SW7422-2: rework nexus_platform_core, add board-specific file and API,
#  mmap after determining heaps
#
# 109   12/15/10 2:26p xhuang
# SW7358-21: Add 97358/97552 support
#
# 108   12/8/10 2:33p jrubio
# SW7344-9: add 7344/7346/7231 support
#
# 107   12/7/10 5:51p erickson
# SW7420-1148: refactor kernel mode proxy for process isolation
#
# 106   12/7/10 1:29p gmohile
# SW7408-190 : enable power management for 7408
#
# 105   12/6/10 4:50p gmohile
# SW7408-190 : Temporarily disable power management for 7408
#
# 104   12/6/10 2:08p gmohile
# SW7408-190 : Default power management on for 7408
#
# 103   12/2/10 8:16p spothana
# SW7420-1177: Adding nexus hdmi dvo module.
#
# SW7420-1177/1   11/1/10 9:04p spothana
# SW7420-1177: Adding nexus hdmi dvo module.
#
# 102   10/26/10 11:33a jtna
# SW7420-887: added temp_monitor
#
# 101   10/21/10 4:08p erickson
# SW7420-1176: set NEXUS_PLATFORM_DEPENDENCIES using NEXUS_MODULES. this
#  eliminates another customization point in the build system.
#
# 100   10/15/10 11:20a randyjew
# SW7468-112: Adding RAP_REALVIDEO_SUPPORT to turn on/off ZSP Video
#  decoder builds.
#
# 99   10/7/10 5:24p nickh
# SW7422-10: Add cable and DBS environment variable support
#
# 98   9/29/10 2:13p erickson
# SW7420-1123: consolidate 3D support using GL_SUPPORT variable
#
# 97   9/29/10 10:53a erickson
# SW7420-1123: refactor platform_modules.inc so that it can fork per
#  platform
#
# 96   9/28/10 5:09p hongtaoz
# SW7425-9: removed intmgr from nexus compile;
#
# 95   9/28/10 2:51p erickson
# SW7420-1123: add linux usermode IPC
#
# 94   9/24/10 3:32p jtna
# SW7420-887: added touchpad module
#
# 93   9/22/10 3:04p hongtaoz
# SW7425-9: adding nexus emu platform;
#
# 92   9/8/10 12:06p vsilyaev
# SW7468-129: Added video decoder on ZSP
#
# 91   8/12/10 1:18p ttrammel
# SW7420-943: Merge NFE to main branch.
#
# 90   8/11/10 6:23p mward
# SW7125-569:  NEXUS_BASE_ONLY build must exclude
#  nexus_platform_frontend_power_management.c.
#
# 89   8/4/10 4:57p spothana
# SW7420-886: Add 97410 UMA mode using 32 bit MEMC0 only
#
# 88   7/27/10 5:30p jgarrett
# SW7340-194: Removing warnings from kernel-mode 3rd party code
#
# 87   7/13/10 5:09p garetht
# SW7420-893: Cut release for 93380VMS Trinity Phase 4.1
#
# 86   7/13/10 10:26a gskerl
# SW7125-237: Added 97019 97025 97119 and 97116 to the list of platforms
#  that get nexus_platform_frontend_power_management.c
#
# 85   7/7/10 11:47a mward
# SW7400-2842: Add an option NEXUS_VIDEO_DECODER_SINGLEDECODE=y for XVD
#  singledecode on 97400.
#
# 84   6/29/10 6:52p mward
# SW7125-497: For proper build without NEXUS_FRONTEND_PWM, build the
#  nexus_platform_frontend_power_management.c but stub API functions.
#
# SW7125-35/2   6/29/10 6:42p mward
# SW7125-497: For proper build without NEXUS_FRONTEND_PWM, build the
#  nexus_platform_frontend_power_management.c but stub API functions.
#
# SW7125-35/1   6/29/10 12:03p mward
# SW7125-497: add new files for frontend Power Management
#
# 83   6/28/10 5:23p ayxiang
# SW7125-497: add new files for frontend Power Management
#
# 82   6/28/10 12:04p mphillip
# SW7405-4091: Fix typo in 3d filename
#
# 81   6/28/10 11:49a mphillip
# SW7405-4091: Add new 3d file to platform.inc
#
# 80   6/11/10 3:36p mward
# SW7125-35: Eliminate the NEXUS_97125_HOST_3112 build option, always
#  build 97125 with support for host control of BCM3112 QAM DS. Eliminate
#  build option NEXUS_97125_BYPASS_3112.  No longer needed.
#
# 79   6/9/10 4:13p prasadv
# SW7400-2546: Adding 97456 platform support.
#
# 78   5/29/10 3:57p gskerl
# SW7125-403: Started adding support for active standby for 7125
#
# 77   5/11/10 5:36p randyjew
# SW7468-6:NEXUS_POWER_STANDBY default off on 7468/7208
#
# 76   4/19/10 3:50p nickh
# SW7420-709: Add DBS and Cable frontend Platform environment variables
#
# 75   4/9/10 11:20a rjain
# SW7550-284: Add support for NEXUS_BASE_ONLY
#
# 74   4/8/10 6:41p nitinb
# SW7550-137: Merge from SW7550-137 branch
#
# 73   3/24/10 1:07p garetht
# SW7420-672: check-in 93380vms/sms Phase 4.0 Trinity
#
# 72   3/19/10 5:18p mward
# SW7405-3986: NEXUS_POWER_STANDBY default off on 7125 platforms.
#
# 71   3/19/10 10:08a erickson
# SW7405-3832: add NEXUS_PLATFORM define, deprecate PLATFORM
#
# 70   3/18/10 4:44p erickson
# SW7405-4060: remove NEXUS_NUM_ defines. platform.inc is only used
#  internally, and NEXUS_NUM_ defines must match internally and
#  externally. moved logic to nexus_platform_features.h.
#
# 69   3/17/10 5:22p gmohile
# SW7408-1 : Power Standby not supported on 7408
#
# 68   3/11/10 7:43p ismailk
# SW7420-618: Don't include NEXUS_CONFIG_IMAGE for 93380sms proxy build.
#
# 67   3/11/10 2:38p jrubio
# SW7405-3986: STANDBY not supported by 7340/7342
#
# 66   3/8/10 4:25p erickson
# SW7405-3986: rename macro to NEXUS_POWER_STANDBY
#
# 65   2/5/10 5:23p jgarrett
# SW7408-71: Adding kernel mode support
#
# 64   1/28/10 4:36p erickson
# SW7420-532: refactor nexus platform standby for linux 2.6.31
#
# 63   1/19/10 3:14p nickh
# SW7420-331: Rename 97410c to 97410
#
# 62   1/19/10 3:13p nickh
# SW7420-541: Rename 97420dvr2 to 97420
#
# 61   12/20/09 7:10p randyjew
# SW7468-6:Disable PWR management for 7208
#
# 60   12/11/09 2:56p mphillip
# SW7550-112: Merge 7550 changes to main branch
#
# Refsw_7550/4   12/3/09 12:09a nitinb
# SW7550-29: Disable POWER_MANAGEMENT by default
#
# Refsw_7550/3   11/26/09 8:50a nitinb
# SW7550-74 : Merge CRB changes
#
# 59   12/9/09 5:34p randyjew
# SW7468-6:Update 7468/7208 support
#
# 58   12/1/09 5:49p randyjew
# SW7468-6: Add 7468 support
#
# 58   12/1/09 5:27p randyjew
# SW7468-6: Add 7468 support
#
# 58   12/1/09 5:26p randyjew
# SW7468-6: Add 7468 support
#
# 58   12/1/09 5:20p randyjew
# SW7468-6: Add 7468 support
#
# 57   11/23/09 12:10p mward
# SW7125-35: Default off option to bypass 3112's on 97125, since DOCSIS
#  software now does the bypass.
#
# 56   11/18/09 1:38p gmohile
# SW7408-1 : Add 7408 support
#
# 55   11/16/09 6:43p mward
# SW7405-3395: Remove misplaced \.
#
# 54   11/16/09 1:25p katrep
# SW7405-3408:replace findstring with filter to do exact word match
#
# 53   11/13/09 5:03p erickson
# SW7325-250: add nexus_platform_standby support
#
# 52   11/12/09 3:37p gmohile
# SW7408-1 : Update 7408 support
#
# 51   11/12/09 2:26p gmohile
# SW7405-3395 : Fix use of NEXUS_FPGA_SUPPORT
#
# 50   11/10/09 11:09a erickson
# SW7405-3351: include NEXUS_SYNCTHUNK_DIR into private includes for
#  linuxkernel and linuxuser.proxy to pick up auto-generated
#  nexus_driver_modules.h
#
# 49   11/3/09 3:48p mward
# SW7125-35: Add an option to bypass 3112's on 97125.
#
# 48   10/15/09 6:45p mward
# SW7125-35: NEXUS_97125_HOST_3112=y build option.
#
# 47   10/8/09 5:53p mward
# SW7125-4: 7125 has GRAPHICS3D.
#
# 46   9/22/09 6:44p mward
# SW7400-2435: Default NEXUS_POWER_MANAGEMENT=y for 7400 platforms.
# SW7125-4: Default NEXUS_POWER_MANAGEMENT=n for 7125 platforms.
#
# 45   9/16/09 1:42p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 44   9/1/09 1:02p mward
# SW7125-4: Disable SW streamer select for now, use DIP switch.
#
# 43   8/31/09 7:30p mward
# SW7125-4: Disable SW streamer select for now, use DIP switch.
#
# 42   8/25/09 11:47a nickh
# SW7420-331: Add 97410c support
#
# 41   8/10/09 6:36p jrubio
# PR55232: disable power management for 7342/7340
#
# 40   8/5/09 4:43p katrep
# PR56569: add nexus platform sw get version api
#
# 39   8/5/09 12:04p katrep
# PR56569: platfrom sofftware release version
#
# 38   7/30/09 11:03a gmohile
# PR 56512 : Merge 7401, 7403 support to mainline
#
# 37   7/28/09 10:18a mward
# PR57145:  Set NEXUS_POWER_MANAGEMENT=n for 7400 until new power
#  management scheme is fixed/implemented on 7400.
#
# 36   7/24/09 1:00p mward
# PR 55545: 97125 has FPGA like 97400.
#
# 35   7/13/09 12:41p vsilyaev
# PR 55989: Enabled OTFPVR for the 97405 platform
#
# 34   6/26/09 6:33p mward
# PR47739: Check for B_PASSTHRU_DDP_SUPPORT=n.
#
# 33   6/24/09 2:47p mphillip
# PR54926: Merge down procfs support
#
# PR54926/1   6/24/09 12:05a mphillip
# PR54926: Add procfs to build
#
# 32   6/19/09 6:18p mward
# PR 47739: DDP passthru on 7400 requires RAP_I2S_COMPRESS_SUPPORT=y.
#
# 31   4/30/09 7:13p katrep
# PR43491: 97405 needs disable i2s outputs for 656/i2s inputs
#
# 30   4/28/09 11:46a jrubio
# PR40169: NEXUS_FPGA_SUPPORT is only for 7400
#
# 29   2/27/09 9:57a katrep
# PR52553: Add 7405-IFE board support
#
# 28   12/30/08 2:24p jgarrett
# PR 50409: Adding DVB_CI
#
# 27   12/22/08 7:06p katrep
# PR50472: Enabled 3D graphics for 7405 C0(7413)
#
# 26   12/17/08 1:48p katrep
# PR49875: Add support for 7466
#
# 25   12/5/08 3:15p nickh
# PR50043: Update to reflect change in PLATFORM variable name from
#  97420DVR2 to 97420dvr2
#
# 24   11/19/08 2:58p nickh
# PR48963: Add 97420DVR2 support
#
# 23   11/5/08 12:15p vsilyaev
# PR 48694: Separated handling of linuxkernel and linuxkernel with
#  driver/ioctls/proxy
#
# 22   11/3/08 4:49p katrep
# PR48628: Add PWM to 7405 build
#
# 21   10/14/08 2:58p erickson
# PR47896: fix path
#
# 20   9/8/08 7:00p shyam
# PR40732 : Re-organize 97043 platform specific code into seperate files
#
# 19   8/22/08 7:12p katrep
# PR45975: Support compilation for 7405 MSG board.
#
# 18   8/21/08 3:16p katrep
# PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
#
# 17   7/23/08 1:53p erickson
# PR44598: NEXUS_Platform_GetDefaultSettings must now be called across
#  the proxy
#
# 16   7/11/08 12:45p mphillip
# PR40869: Add Graphic3D to platform build
#
# 15   7/2/08 10:23a vsilyaev
# PR 44452: Added file to keep NEXUS_Platform_GetDefaultSettings
#
# 14   5/19/08 7:44p shyam
# PR 42787 : Add support for 97043 BSP
#
# 13   5/16/08 7:45p vishk
# PR 40034: Develop Nexus Smartcard Interface/Module.
# PR 40016: bsettop_smartcard: Develop SettopAPI-to-Nexus shim layer
#
# 12   4/24/08 12:59p erickson
# PR42113: add InputCapture
#
# 11   4/2/08 5:41p shyam
# PR39899: Add 7043 Transcoder support
#
# 10   3/24/08 10:27p bandrews
# PR40090: astm integration
#
# 9   3/20/08 2:16p erickson
# PR36068: remove rockford from standard config spec
#
# PR39899/1   2/27/08 5:52p shyam
# PR 39899 : Integrate 7043 XCD into Nexus
#
# 8   2/20/08 1:42p erickson
# PR39405: added RFM
#
# 7   2/7/08 11:26a vsilyaev
# PR 38682: Added message/syslog routing
#
# 6   2/4/08 11:42a erickson
# PR36808: added SyncChannel
#
# 5   2/1/08 5:34p jgarrett
# PR 39017: Adding HdmiOutput
#
# 5   2/1/08 5:25p jgarrett
# PR 39017: Adding HdmiOutput
#
# 4   2/1/08 5:03p vsilyaev
# PR 38682: Added tracking of open/close calls
#
# 3   1/29/08 5:55p vsilyaev
# PR 38682: Support of callbacks in the linux kernel/user proxy
#  environment
#
# 2   1/24/08 4:29p vsilyaev
# PR 38682: Added proxied environment
#
# 1   1/18/08 2:17p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/23   1/18/08 1:37p vsilyaev
# PR 38682: Updated nexus kernel environment
#
# Nexus_Devel/22   1/16/08 3:27p vsilyaev
# PR 38682: Compile linuxkernel specific modules
#
# Nexus_Devel/21   12/3/07 9:28p jgarrett
# PR 37837: Adding DMA
#
# Nexus_Devel/20   11/29/07 2:21p jgarrett
# PR 37756: Moving public includes to include directory
#
# Nexus_Devel/19   11/21/07 11:06a erickson
# PR37423: add io modules
#
# Nexus_Devel/18   11/15/07 4:01p erickson
# PR37137: security update
#
# Nexus_Devel/17   11/8/07 2:47p vsilyaev
# PR 36788: PVR record module
#
# Nexus_Devel/16   11/5/07 4:30p erickson
# PR36725: move os code into subdir
#
# Nexus_Devel/15   10/19/07 9:39a erickson
# PR36275: added graphics2d
#
# Nexus_Devel/14   10/15/07 5:11p vsilyaev
# PR 35824: Added playback module
#
# Nexus_Devel/13   10/10/07 5:11p vsilyaev
# PR 35824: Added file module
#
# Nexus_Devel/12   10/10/07 11:29a jgarrett
# PR 35551: Adding frontend
#
# Nexus_Devel/11   10/5/07 5:51p jgarrett
# PR 35551: Adding frontend
#
# Nexus_Devel/10   10/5/07 1:42p jgarrett
# PR 35744: Adding i2c module
#
# Nexus_Devel/9   9/28/07 11:32a erickson
# PR34926: rename videodecoder to video_decoder
#
# Nexus_Devel/8   9/27/07 7:52p jgarrett
# PR 34954: Adding audio
#
# Nexus_Devel/7   9/26/07 5:36p vsilyaev
# PR 34662: Added platform configuration
#
# Nexus_Devel/6   9/26/07 1:45p vsilyaev
# PR 34662: Implementation of display framebuffer
#
# Nexus_Devel/5   9/25/07 1:31p vsilyaev
# PR 34662: Added display module
#
# Nexus_Devel/4   9/25/07 12:26p erickson
# PR34926: added VIDEODECODER
#
# Nexus_Devel/3   9/24/07 3:58p erickson
# PR34925: added TRANSPORT
#
# Nexus_Devel/2   9/24/07 1:00p jgarrett
# PR 35002: Created nexus_platform_core.c
#
# Nexus_Devel/1   9/21/07 6:26p jgarrett
# PR 35002: Adding 97400 platform
#
############################################################

#
# Conditional Options for this Makefile
#
# None (yet)
#

######################
# Define this module #
######################
NEXUS_MODULES += PLATFORM

##################
# Module Options #
##################

##########################
# Default Module Options #
##########################
ifeq ($(filter $(NEXUS_PLATFORM), 97400 97401 97403 ), $(NEXUS_PLATFORM))
NEXUS_PLATFORM_OPTIONS = NEXUS_FPGA_SUPPORT
ifndef NEXUS_FPGA_SUPPORT
NEXUS_FPGA_SUPPORT :=y
endif
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97400 97456 97401 97403), $(NEXUS_PLATFORM))
ifneq ($(subst N,n,${B_PASSTHRU_DDP_SUPPORT}),n)
RAP_I2S_COMPRESS_SUPPORT=y
endif
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97420 97408 97231 935233 97125 97019 97025 97119 97116 97425 93380sms 97435 97429 97428 97241 97344 97346), $(NEXUS_PLATFORM))
ifneq ($(subst N,n,${NEXUS_POWER_MANAGEMENT}),n)
NEXUS_POWER_MANAGEMENT :=y
endif
else
ifeq ($(subst Y,y,${NEXUS_POWER_MANAGEMENT}),y)
NEXUS_POWER_MANAGEMENT :=y
endif
endif

ifndef NEXUS_CONFIG_IMAGE
ifeq ($(filter $(B_REFSW_OS),linuxkernel bare),$(B_REFSW_OS))
ifeq ($(NEXUS_MODE),driver)
NEXUS_CONFIG_IMAGE :=y
endif
endif
endif

ifeq ($(NEXUS_MODE),proxy)
ifneq ($(filter $(NEXUS_PLATFORM), 93380sms), $(NEXUS_PLATFORM))
ifndef NEXUS_CONFIG_IMAGE
NEXUS_CONFIG_IMAGE :=y
endif
endif
endif

ifeq ($(NEXUS_PLATFORM_COMMON_DIR),)
NEXUS_PLATFORM_COMMON_DIR=$(NEXUS_TOP)/platforms/common
endif
ifeq ($(NEXUS_PLATFORM_SPECIFIC_DIR),)
NEXUS_PLATFORM_SPECIFIC_DIR=$(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)
endif

ifeq ($(NEXUS_PLATFORM),935233)
BTHD_ISDBT_SUPPORT=y
endif

###################
# Module Includes #
###################
NEXUS_PLATFORM_PUBLIC_INCLUDES = \
    $(NEXUS_PLATFORM_COMMON_DIR)/include \
    $(NEXUS_PLATFORM_SPECIFIC_DIR)/include \

# must add nexus/platforms to resolve #include-style symlinks
NEXUS_PLATFORM_PUBLIC_INCLUDES += $(NEXUS_TOP)/platforms

NEXUS_PLATFORM_PRIVATE_INCLUDES += $(NEXUS_PLATFORM_COMMON_DIR)/src \
                                   $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS) \
                                   $(NEXUS_SYNCTHUNK_DIR) \
                                   $(MAGNUM)/../BSEAV/lib/utils

ifeq ($(filter $(NEXUS_MODE),proxy driver),$(NEXUS_MODE))
NEXUS_PLATFORM_PRIVATE_INCLUDES += $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxkernel/common
endif

##################
# Module Sources #
##################
NEXUS_PLATFORM_SOURCES = \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_config.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_core.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_$(NEXUS_PLATFORM).c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_interrupt.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_platform_server.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_map.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_settings.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_standby.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_version.c \
    $(NEXUS_TOP)/base/src/b_objdb.c \
    $(NEXUS_TOP)/base/src/nexus_client_resources.c

NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_SPECIFIC_DIR)/src/nexus_platform_pinmux.c \
    $(NEXUS_PLATFORM_SPECIFIC_DIR)/src/nexus_platform_vcxo.c \
    $(NEXUS_PLATFORM_SPECIFIC_DIR)/src/nexus_platform_standby_$(BCHP_CHIP).c

ifeq ($(HUMAX_PLATFORM_BASE),y)
NEXUS_PLATFORM_SOURCES += \
	$(NEXUS_PLATFORM_DIR)/platforms/$(PLATFORM)/src/humax_pinmux.c
endif

ifneq ($(NEXUS_BASE_ONLY),y)
NEXUS_PLATFORM_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_platform_thunks.c
NEXUS_PLATFORM_SOURCES += $(NEXUS_PLATFORM_SPECIFIC_DIR)/src/nexus_platform_frontend.c

ifeq ($(filter $(NEXUS_PLATFORM), 97125 97019 97025 97119 97116), $(NEXUS_PLATFORM))
NEXUS_PLATFORM_SOURCES += $(NEXUS_PLATFORM_SPECIFIC_DIR)/src/nexus_platform_frontend_power_management.c
endif

endif

ifneq ($(filter $(NEXUS_PLATFORM), 97550 97358 97552 97360), $(NEXUS_PLATFORM))
NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_platform_pci.c
endif

ifeq ($(NEXUS_FPGA_SUPPORT),y)
NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/bfpga.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/bfpga_name.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_fpga.c
endif

ifeq ($(B_REFSW_OS),linuxemu)
NEXUS_PLATFORM_PRIVATE_INCLUDES += $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/client

NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/client/client.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/client/pagelib.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/client/mutex.c
endif

FLOATLIB=$(NEXUS_TOP)/../BSEAV/linux/driver/floatlib

ifeq ($(B_REFSW_OS),linuxkernel)
    NEXUS_PLATFORM_SOURCES += \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_driver_udivdi3.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_driver_procfs.c \
        $(FLOATLIB)/src/fp-bit-single.c \
        $(FLOATLIB)/src/fp-bit-double.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_platform_os.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_interrupt_map.c

    # Remove warnings for 3rd-party FP code
    NEXUS_PLATFORM_EXTRA_CFLAGS += -w

    ifeq ($(NEXUS_MODE),driver)
    ifneq ($(LINUX_VER_GE_2_6_31),y)
    NEXUS_PLATFORM_SOURCES += \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_driver.mod.c
    endif
    NEXUS_PLATFORM_SOURCES += \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_driver.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_generic_driver.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_driver_callbacks.c

        # nexus_generic_driver.c depends on the thunk in order to trap NEXUS_Platform_Init/Uninit
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_generic_driver.c: $(NEXUS_SYNCTHUNK_DIR)/nexus_platform_ioctl.h
    endif

    ifeq ($(NEXUS_CONFIG_IMAGE),y)
    NEXUS_PLATFORM_SOURCES += \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_img_kernel.c
    endif

    ifeq ($(GL_SUPPORT),y)
    NEXUS_PLATFORM_SOURCES += \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_driver_graphics3d.c
    endif
endif

ifeq ($(B_REFSW_OS),bare)
    NEXUS_PLATFORM_PRIVATE_INCLUDES += $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxkernel
# __divdi3.o & friends are libgcc files obtained from uclinux-rootfs-2.6.31-2.4.tar.bz2.tar.bz2, uclinux-rootfs/user/root_sign/libgcc
    NEXUS_PLATFORM_SOURCES += \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxkernel/nexus_generic_driver.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxkernel/nexus_driver_callbacks.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxkernel/nexus_img_kernel.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxkernel/nexus_interrupt_map.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxkernel/nexus_driver_procfs.c \
        $(FLOATLIB)/src/fp-bit-single.c \
        $(FLOATLIB)/src/fp-bit-double.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/nexus_platform_os.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/jumptable_fwd.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/libgcc/ucmpdi2.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/libgcc/__divdi3.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/libgcc/__udivdi3.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/libgcc/__moddi3.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/libgcc/__umoddi3.c \
        $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/libgcc/__udivmoddi4.c

        # NEXUS_PLATFORM_SOURCES can only take .c files, so the build rule for these files is in build/os/bare/os_rules.inc
        # $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/stubs_rev.S
        # $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/libgcc/memset.S
        # $(NEXUS_PLATFORM_COMMON_DIR)/src/bare/libgcc/memcpy_brcm.S

    # nexus_generic_driver.c depends on the thunk in order to trap NEXUS_Platform_Init/Uninit
    $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxkernel/nexus_generic_driver.c: $(NEXUS_SYNCTHUNK_DIR)/nexus_platform_ioctl.h
endif

ifeq ($(B_REFSW_OS),linuxuser)
ifeq ($(NEXUS_MODE),)
NEXUS_PLATFORM_PRIVATE_INCLUDES += $(MAGNUM)/../BSEAV/linux/driver/usermode
NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_platform_os.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_debug_log.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_audio_log.c

ifeq ($(NEXUS_SERVER_SUPPORT),y)
NEXUS_EXTRA_INCLUDES += $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)
NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_driver_callbacks.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxuser/nexus_socket.c
endif
endif
endif

ifeq ($(B_REFSW_OS),ucos)
NEXUS_PLATFORM_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/core/$(BCHP_CHIP)/src
NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS)/nexus_platform_os.c
endif

ifeq ($(NEXUS_MODE),proxy)
NEXUS_PLATFORM_PRIVATE_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)
NEXUS_PLATFORM_PROXY_SOURCES = \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS).$(NEXUS_MODE)/nexus_platform_os.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_map.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_debug_log.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/nexus_platform_audio_log.c
ifeq ($(NEXUS_CONFIG_IMAGE),y)
NEXUS_PLATFORM_PROXY_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS).$(NEXUS_MODE)/nexus_platform_image.c
endif
NEXUS_PLATFORM_LOCAL_SOURCES = \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS).$(NEXUS_MODE)/nexus_platform_version_local.c
NEXUS_PLATFORM_SOURCES := $(NEXUS_PLATFORM_PROXY_SOURCES)
endif

ifeq ($(NEXUS_MODE),client)
NEXUS_PLATFORM_CLIENT_SOURCES := \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS).${NEXUS_MODE}/nexus_platform_os.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS).${NEXUS_MODE}/nexus_platform_client.c \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/linuxuser/nexus_socket.c
NEXUS_PLATFORM_LOCAL_SOURCES = \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS).$(NEXUS_MODE)/nexus_platform_version_local.c
NEXUS_PLATFORM_SOURCES := ${NEXUS_PLATFORM_CLIENT_SOURCES}
NEXUS_PLATFORM_PRIVATE_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)
NEXUS_LIB_SUFFIX :=_client
ifeq ($(GL_SUPPORT),y)
$(warning GL_SUPPORT=y is not supported in NEXUS_MODE=client.  Setting GL_SUPPORT=n.)
GL_SUPPORT := n
endif

ifeq ($(NEXUS_WEBCPU),core1_server)
NEXUS_PLATFORM_CLIENT_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/$(B_REFSW_OS).${NEXUS_MODE}/nexus_webcpu_platform_os.c \
    $(NEXUS_TOP)/modules/core/$(BCHP_CHIP)/src/nexus_core_convert.c \
    $(NEXUS_TOP)/base/src/b_objdb.c
NEXUS_PLATFORM_SOURCES := $(NEXUS_PLATFORM_CLIENT_SOURCES)
NEXUS_PLATFORM_PRIVATE_INCLUDES += $(MAGNUM)/../BSEAV/linux/driver/usermode
NEXUS_LIB_SUFFIX :=_webcpu
# see below for NEXUS_PLATFORM_DEFINES
endif

endif


##################
# Module Defines #
##################

ifneq ($(NEXUS_PLATFORM_DEFINES),)
$(warning NEXUS_PLATFORM_DEFINES is being cleared. Currently set to NEXUS_PLATFORM_DEFINES=$(NEXUS_PLATFORM_DEFINES))
endif

NEXUS_PLATFORM_DEFINES =

ifneq ($(NEXUS_WEBCPU),)
# general flag for IP socket IPC
NEXUS_PLATFORM_DEFINES += NEXUS_WEBCPU
ifeq ($(NEXUS_WEBCPU),core1_server)
# core1_server runs M2MC1 locally
NEXUS_PLATFORM_DEFINES += NEXUS_WEBCPU_core1_server
endif
endif

ifeq ($(NEXUS_CONFIG_IMAGE),y)
NEXUS_PLATFORM_DEFINES += NEXUS_CONFIG_IMAGE
endif

ifeq ($(NEXUS_FPGA_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_FPGA_SUPPORT
endif

ifneq ($(NEXUS_MODE),)
NEXUS_PLATFORM_DEFINES += NEXUS_MODE_$(NEXUS_MODE)
endif

ifeq ($(NEXUS_PLATFORM),97550)
ifeq ($(NEXUS_BOARD_7530_CRB),y)
NEXUS_PLATFORM_DEFINES += NEXUS_BOARD_7530_CRB=1
$(warning Compiling for 7530 CRB Board)
endif
ifeq ($(NEXUS_BOARD_7550_CRB),y)
NEXUS_PLATFORM_DEFINES += NEXUS_BOARD_7550_CRB=1
$(warning Compiling for 7550 CRB Board)
endif
endif

ifeq ($(NEXUS_PLATFORM),97410)
ifeq ($(NEXUS_PLATFORM_7410_UMA_32b),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7410_UMA_32b=1
endif
endif

ifeq ($(NEXUS_PLATFORM),97420)
ifeq ($(NEXUS_PLATFORM_7420_DBS),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7420_DBS=1
endif
ifeq ($(NEXUS_PLATFORM_7420_CABLE),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7420_CABLE=1
endif
endif

ifeq ($(NEXUS_PLATFORM),97231)
ifeq ($(NEXUS_PLATFORM_7231_FBTSFF),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7231_FBTSFF=1
endif
ifeq ($(NEXUS_PLATFORM_7231_CSFF),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7231_CSFF=1
endif
ifeq ($(NEXUS_PLATFORM_7231_EUSFF),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7231_EUSFF=1
endif
ifeq ($(NEXUS_PLATFORM_7231_EUSFF_V20),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7231_EUSFF_V20=1
endif
ifeq ($(NEXUS_PLATFORM_7231_3520),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7231_3520=1
endif
endif

ifeq ($(NEXUS_PLATFORM),97241)
ifeq ($(NEXUS_PLATFORM_7241_C2SFF),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7241_C2SFF=1
endif
ifeq ($(NEXUS_PLATFORM_7241_DCSFBTSFF),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7241_DCSFBTSFF=1
endif
ifeq ($(NEXUS_PLATFORM_7241_80211ACUSFF),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7241_80211ACUSFF=1
endif
ifeq ($(USE_SPI_FRONTEND),y)
NEXUS_PLATFORM_DEFINES += USE_SPI_FRONTEND=1
endif
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97420 97409), $(NEXUS_PLATFORM))
ifeq ($(NEXUS_ZORDER_PIP_SWAP),y)
NEXUS_PLATFORM_DEFINES += NEXUS_ZORDER_PIP_SWAP=1
endif
endif

ifeq ($(NEXUS_PLATFORM),97344)
ifeq ($(BCHP_VER), A0)
NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/btst_video_shmoo.c
endif
ifeq ($(NEXUS_PLATFORM_7344SV),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7344SV=1
endif
ifeq ($(NEXUS_PLATFORM_7418SFF_H),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7418SFF_H=1
endif
ifeq ($(NEXUS_PLATFORM_7418SFF_RVU),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7418SFF_RVU=1
endif
endif

ifeq ($(NEXUS_PLATFORM),97422)
ifeq ($(BCHP_VER), A0)
NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/btst_video_shmoo.c
endif
ifeq ($(NEXUS_PLATFORM_7422_DBS),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7422_DBS=1
endif
ifeq ($(NEXUS_PLATFORM_7422_CABLE),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7422_CABLE=1
endif
ifeq ($(NEXUS_USE_7422_SV_BOARD),y)
NEXUS_PLATFORM_DEFINES += NEXUS_USE_7422_SV_BOARD=1
endif
ifeq ($(NEXUS_USE_7422_VMS_SFF),y)
NEXUS_PLATFORM_DEFINES += NEXUS_USE_7422_VMS_SFF=1
endif
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97425 97435 93383sms 93335), $(NEXUS_PLATFORM))
ifeq ($(BCHP_CHIP), 7425)
ifeq ($(BCHP_VER), A0)
NEXUS_PLATFORM_SOURCES += \
    $(NEXUS_PLATFORM_COMMON_DIR)/src/btst_video_shmoo.c
endif
endif

ifeq ($(NEXUS_PLATFORM_7425_DBS),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7425_DBS=1
endif
ifeq ($(NEXUS_PLATFORM_4528_DBS),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_4528_DBS=1
endif
ifeq ($(NEXUS_USE_7425_SATIPSW),y)
NEXUS_PLATFORM_DEFINES += NEXUS_USE_7425_SATIPSW=1
endif
ifeq ($(NEXUS_PLATFORM_7425_CABLE),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_7425_CABLE=1
ifeq ($(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT=1
endif
ifeq ($(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT=1
endif
ifeq ($(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_DOCSIS_IB_SUPPORT=1
endif
ifeq ($(NEXUS_USE_7425_SV_BOARD),y)
NEXUS_PLATFORM_DEFINES += NEXUS_USE_7425_SV_BOARD=1
endif
ifeq ($(NEXUS_USE_7425_VMS_SFF),y)
NEXUS_PLATFORM_DEFINES += NEXUS_USE_7425_VMS_SFF=1
endif
endif
endif
ifeq ($(NEXUS_PLATFORM_7429_CABLE),y)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT=1
endif

ifeq ($(NEXUS_VIDEO_DECODER_SINGLEDECODE),y)
$(warning Compiling Singledecode XVD)
NEXUS_PLATFORM_DEFINES += NEXUS_VIDEO_DECODER_SINGLEDECODE=1
endif

# PLATFORM is deprecrated in favor of NEXUS_PLATFORM

ifeq ($(filter $(NEXUS_PLATFORM), 93380sms 93380vms 93383sms), $(NEXUS_PLATFORM))

ifeq ($(filter $(NEXUS_PLATFORM), 93380sms 93380vms), $(NEXUS_PLATFORM))
NEXUS_PLATFORM_DEFINES += PLATFORM=93380
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM=93380
endif

ifeq ($(filter $(NEXUS_PLATFORM), 93383sms), $(NEXUS_PLATFORM))
NEXUS_PLATFORM_DEFINES += PLATFORM=93383
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM=93383
endif

else

ifeq ($(filter $(RAP_REALVIDEO_SUPPORT) $(RAP_VP6_SUPPORT), y), y)
NEXUS_PLATFORM_DEFINES += NEXUS_NUM_ZSP_VIDEO_DECODERS=1
endif

ifeq ($(filter $(BDSP_VP6_SUPPORT), y), y)
NEXUS_PLATFORM_DEFINES += NEXUS_NUM_DSP_VIDEO_DECODERS=1
NEXUS_PLATFORM_DEFINES += NEXUS_BDSP_VP6_SUPPORT=1
endif

ifeq ($(filter $(BDSP_ENCODER_SUPPORT), y), y)
NEXUS_PLATFORM_DEFINES += NEXUS_NUM_DSP_VIDEO_ENCODERS=1
endif

ifeq ($(filter $(BSID_MJPEG_SUPPORT), y), y)
NEXUS_PLATFORM_DEFINES += NEXUS_NUM_SID_VIDEO_DECODERS=1
endif

ifeq ($(NEXUS_TOUCHPAD_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_TOUCHPAD_SUPPORT
endif

ifeq ($(NEXUS_TEMP_MONITOR_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_TEMP_MONITOR_SUPPORT
endif

ifeq ($(NEXUS_DVO_DVI_LOOPBACK_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_DVO_DVI_LOOPBACK_SUPPORT
endif

NEXUS_PLATFORM_DEFINES += PLATFORM=$(NEXUS_PLATFORM)
NEXUS_PLATFORM_DEFINES += NEXUS_PLATFORM=$(NEXUS_PLATFORM)
endif

# GL_SUPPORT is for 3D Graphics
ifeq ($(GL_SUPPORT),y)
# NEXUS_HAS_3D is needed in addition to NEXUS_HAS_GRAPHICS3D. In linux kernel mode, the
# platform must provide the 3D interrupt, but the Graphics3D module will live in user space.
NEXUS_PLATFORM_DEFINES += NEXUS_HAS_3D
endif

# Add DIAGS module
ifeq ($(DIAGS),y)
NEXUS_PLATFORM_DEFINES += DIAGS
endif

# Add DIAGS_MEM_DMA_TEST
ifeq ($(DIAGS_MEM_DMA_TEST),y)
NEXUS_PLATFORM_DEFINES += DIAGS_MEM_DMA_TEST
endif


#######################
# Module Dependencies #
#######################
NEXUS_PLATFORM_DEPENDENCIES = $(filter-out PLATFORM, $(NEXUS_MODULES))

# V3D_SUPPORT is for VC4 3D Graphics
ifeq ($(V3D_SUPPORT),y)
NEXUS_PLATFORM_DEPENDENCIES += GRAPHICS3D
endif

ifeq ($(MSDRM_PRDY_SUPPORT),y)
NEXUS_PLATFORM_DEPENDENCIES += WMDRMPD
endif

ifeq ($(MSDRM_PRDY_SUPPORT),y)
NEXUS_PLATFORM_DEPENDENCIES += WMDRMPD_CORE
endif

ifeq ($(MSDRM_PRDY_SUPPORT),y)
NEXUS_PLATFORM_DEPENDENCIES += WMDRMPD_IO
endif

##################
# Magnum Modules #
##################
NEXUS_PLATFORM_MAGNUM_MODULES =

ifeq ($(B_REFSW_OS),linuxemu)
NEXUS_PLATFORM_MAGNUM_MODULES += $(MAGNUM)/basemodules/test/btst.inc
endif

ifneq ($(NEXUS_PLATFORM_7425_CABLE),)
include $(NEXUS_TOP)/extensions/platform/cable_frontend/platform_cable_frontend.inc
endif

ifneq ($(NEXUS_PLATFORM_7422_CABLE),)
include $(NEXUS_TOP)/extensions/platform/cable_frontend/platform_cable_frontend.inc
endif

