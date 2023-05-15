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
# $brcm_Workfile: platform_modules.inc $
# $brcm_Revision: SW7435-433/2 $
# $brcm_Date: 11/12/12 10:49a $
#
# Module Description:
#   This file defines a list of nexus modules that will be included in the build.
#
# Revision History:
#
# $brcm_Log: /nexus/platforms/common/build/platform_modules.inc $
# 
# SW7435-433/2   11/12/12 10:49a jzhu
# SW7435-246:Add hdmi input module for 3335.
# 
# SW7435-433/1   11/9/12 9:41a mward
# SW7425-4121: Update 97422 VMS 3128 tuner support.
# 
# 230   9/26/12 10:59a jzhu
# SW7435-246:sms3335 support.
# 
# 229   9/25/12 11:18a katrep
# SW7445-1:enable hdmi input
# 
# 228   9/24/12 4:55p katrep
# SW7445-1:add support for 7445
# 
# 227   9/19/12 10:41a randyjew
# SW7231-317: Remove RFM for 7231 7230
# 
# 226   9/17/12 3:03p mphillip
# SWSATFE-214: Update 4528 support after reorganization
# 
# 225   9/13/12 4:40p erickson
# SW7435-354: rearrange simple_decoder.inc so it automatically cleans up
#  first
# 
# 224   9/13/12 3:42p erickson
# SW7435-354: remove unused code
# 
# 223   9/7/12 2:10p erickson
# SW7425-3819: remove 2940 support
# 
# 222   9/6/12 3:54p erickson
# SW7435-235: move frontend.inc to nexus/modules/frontend/build
# 
# 221   8/16/12 11:53a randyjew
# SW7468-6: Remove 3d graphics support for 97468/97208
# 
# 220   8/7/12 3:04p jessem
# SW7425-2690: Removed TUNER_ENABLE_7425/7422/4528_DBS variables.
#
# 219   8/7/12 2:26p jessem
# SW7425-2690: Updated 97422/9725 DBS frontend variables.
#
# 218   8/6/12 4:10p jessem
# SW7425-2690: Corrected 97422 platform build variables.
#
# 217   8/2/12 3:53p jessem
# SW7429-204: Removed condition for including cec.inc
#
# 216   7/30/12 4:40p jgarrett
# SW7435-266: Switching to unified audio module structure
#
# 215   7/30/12 6:34p nitinb
# SW7550-667: Include graphics2d.inc for 7550
#
# 214   7/27/12 2:14p jgarrett
# SW7425-3281: Merge to main branch
#
# 213   7/26/12 5:29p mward
# SW7425-2690:  NEXUS_USE_7425_SV_BOARD and NEXUS_USE_7425_VMS_SFF (and
#  7422 versions) imply NEXUS_PLATFORM_7425_CABLE.  Don't require
#  NEXUS_PLATFORM_7425_CABLE with one of those board options.
#
# 212   7/26/12 4:31p erickson
# SW7435-235: fix version 209 checkin
#
# 211   7/26/12 3:09p erickson
# SW7435-235: fix version 209 checkin
#
# 210   7/26/12 2:58p erickson
# SW7435-235: require case-sensitive VIDEO_ENCODER_SUPPORT=n
#
# 209   7/26/12 10:10a erickson
# SW7435-235: refactor platform_modules.inc for
#  93380sms/93380vms/93383sms
#
# 208   7/24/12 10:59a jrubio
# SW7435-235: fix 7346
#
# 207   7/24/12 10:04a erickson
# SW7435-235: refactor directories to remove symbolic links
#
# 206   7/20/12 2:18p erickson
# SW7435-235: fix 7550
#
# 205   7/20/12 12:30p erickson
# SW7435-235: refactor directories to remove symbolic links
#
# 204   7/20/12 11:09a randyjew
# SW7468-247: include spi for 7468 7208
#
# 203   7/17/12 2:58p mward
# SW7425-2690:  Default on VIDEO_ENCODER_SUPPORT for 97425 97435 93383sms
#  (ViCE2 platforms).
#
# 202   7/17/12 1:19p erickson
# SW7435-235: refactor directories to remove symbolic links
#
# 201   7/17/12 7:58a erickson
# SW7435-235: refactor directories to remove symbolic links
#
# 200   7/16/12 3:09p erickson
# SW7435-235: refactor directories to remove symbolic links
#
# 199   7/16/12 2:39p erickson
# SW7435-235: refactor directories to remove symbolic links
#
# 198   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
#
# 197   6/22/12 5:42p mward
# SW7425-2983:  NEXUS_FRONTEND_3112_VER changed to
#  NEXUS_FRONTEND_31xx_VER
#
# SW7425-3281/1   6/20/12 2:35p piyushg
# SW7425-3281: Initial checkin of Nexus directory structure and header
#  files.
#
# 196   6/11/12 11:20p jessem
# SW7425-3225: Update 97422 VMS support.
#
# 195   4/4/12 6:58p mward
# SW7425-2690: don't include graphics3d.inc if GL_SUPPORT is defined.
#
# 194   4/4/12 4:46p mward
# SW7425-2690: don't automatically set V3D_SUPPORT if GL_SUPPORT is
#  defined.
#
# 193   4/3/12 11:22a erickson
# SW7425-2690: default V3D_SUPPORT=y if source exists
#
# 192   3/12/12 4:41p gkamosa
# SW7425-2543: Merge support for 3383 SMS platform
#
# Centaurus_Phase_1.0/4   3/6/12 6:03p gkamosa
# SW7425-2337: Merge branch to tip
#
# 191   3/5/12 6:13p mward
# SW7435-26:  Enable picture_decoderfor 7435.
#
# 190   2/20/12 1:23p jessem
# SW7425-2211: Added back security support for 7425B2.
#
# 189   2/17/12 4:23p jessem
# SW7425-2211: Reverted back to previous version due to missing
#  HSM/SECURITY  labels.
#
# 188   2/17/12 1:41p jessem
# SW7425-2211:Added back security support for 7425B2.
#
# 188   2/17/12 12:08p jessem
# SW7425-2211: Added back security support for 7425B2.
#
# 188   2/17/12 12:00p jessem
# SW7425-2211: Added back security support for 7425B2.
#
# 188   2/17/12 11:58a jessem
# SW7425-2211: Added back security support for 7425B2.
#
# 187   1/20/12 2:31p jtna
# SW7408-210: Deprecate use of NEXUS_POWER_STANDBY
#
# 186   1/19/12 2:54p jessem
# SW7425-2211: Temporarily removed security.inc for 7425B2.
#
# Centaurus_Phase_1.0/3   2/16/12 1:12p jmiddelk
# SW7425-2337: Add support for 3383sms platform. Re-enable security
#
# Centaurus_Phase_1.0/2   2/9/12 4:01p jmiddelk
# SW7425-2337: Adding support for 93383sms
#
# Centaurus_Phase_1.0/1   2/6/12 5:56p jmiddelk
# SW7425-2337: Adding support for 93383sms
#
# 185   12/20/11 5:32p hongtaoz
# SW7425-1878: moved default for encoder support into display.inc;
#
# 184   12/20/11 12:08p prasadv
# SW3128-56: Clean up. Nexus 3128 Support missing on 7425 SV boards.
#
# 183   12/18/11 6:48p prasadv
# SW3128-56: Clean up. Remove 3124 support. Auto detection has been added
#  to take care of 312x.
#
# 182   12/15/11 6:06p vishk
# SW3128-92: Add FW auto detection support
#
# 181   12/15/11 4:26p vsilyaev
# SW7425-1140: Merge NEXUS_Cec module support into main-line
#
# SW7425-1140/1   12/9/11 5:11p vle
# SW7425-1140: add NEXUS Cec module
#
# 180   12/8/11 11:41a mward
# SW7125-1165: Replace findstring with filter to do exact word match.
#
# 179   12/1/11 4:13p mphillip
# SW7425-1304: Change 4528 to HAB
#
# 178   11/30/11 10:26p jessem
# SW7425-1753: Removed restriction on security.inc.
#
# 177   11/21/11 7:03p mward
# SW7435-26: Disable picture_decoder for 7435 bring-up.
#
# 176   11/13/11 12:36a jessem
# SW7425-1753: Temporarily removed security until 7425 B1 HSM files are
#  ready.
#
# 175   10/31/11 4:38p mward
# SW7435-7:  Add 97435.
#
# 174   10/31/11 1:48p vishk
# SW7425-1598: Add support for 3128B0/C0
#
# 173   10/28/11 12:43p erickson
# SW7420-1992: build surface_compositor and input_router unconditionally
#
# 172   10/26/11 4:10p jtna
# SW7425-1327: fix messy indentation of nested ifeq/else/endif
#
# 171   10/25/11 11:58a vishk
# SW7425-1598: Add support for 3128B0/C0
#
# 170   10/19/11 1:58p jrubio
# SW7340-283: add NEXUS_FRONTEND_DISABLE for 734x platforms
#
# 169   10/18/11 12:03p prasadv
# SW3128-56: Use the correct compile time flag for 3124
#
# 168   10/17/11 7:29p prasadv
# SW3128-56: Add 3124 Support
#
# 167   10/14/11 1:49p hongtaoz
# SW7425-1248: default to include video encoder for 7425;
#
# 166   10/13/11 6:03p jessem
# SW7425-1251: Removed compile restriction on security.inc
#
# 165   10/11/11 6:51p vishk
# SW3128-46: add support for BCM3124 devices
#
# 165   10/11/11 6:36p vishk
# SW3128-46: add support for BCM3124 devices
#
# 164   10/11/11 6:07p jessem
# SW7425-1248: Only include video_encoder.inc when  VIDEO_ENCODER_SUPPORT
#  is defined.
#
# 163   9/13/11 4:45p jessem
# SW7425-1251: Temporarily comment out security.inc for 97425 B0.
#
# 162   8/29/11 3:55p erickson
# SW7420-1993: add input_router.inc
#
# 161   8/23/11 4:13p erickson
# SW7420-1992: rename to surface_compositor
#
# 160   8/5/11 4:23p erickson
# SW7420-1992: add surfacecmp.inc with export surfacecmp=y option
#
# 159   7/13/11 12:54p jessem
# SW7425-695: For 97425 platforms, include keypad module for 97425SV
#  boards only.
#
# 158   6/20/11 5:20p mward
# SW7400-3037:  Default GL_SUPPORT=y for 7400, but allow it to be set to
#  n.
#
# 157   6/13/11 12:19p spothana
# SW7425-622 : Add simul usage of BCM3128 and DOCSIS tuners.
#
# 156   5/12/11 4:45p jtna
# SW7550-739: include dma for all platforms
#
# 155   5/3/11 5:08p erickson
# SW7420-1819: repartition so NFE is a standard feature
#
# 154   4/27/11 6:20p vsilyaev
# SW7425-232: Always compile file module
#
# 153   4/18/11 4:50p spothana
# SW7425-369: Add NEXUS_DOCSIS_OOB_SUPPORT and NEXUS_DOCSIS_IB_SUPPORT
#
# 152   4/12/11 4:56p spothana
# SW7425-331: Compile BCM3128 support for BCM97425VMS also because a 3128
#  card can be used on this platform.
#
# 151   4/7/11 10:01a hongtaoz
# SW7425-232: add file mux module;
#
# 150   3/9/11 3:08p hongtaoz
# SW7422-171: compile 3128 only if the board supports it;
#
# 149   3/9/11 1:35p VISHK
# SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
#  board) plug on 7422 board  Fix 3128 chip rev number.
#
# 148   3/8/11 3:47p VISHK
# SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
#  board) plug on 7422 board  Add support for 3128 for 97425 platform.
#
# 147   3/4/11 5:27p garetht
# SW7420-1577: SMS/VMS phase 6.0 release
#
# 146   3/4/11 3:49p vishk
# SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
#  board) plug on 7422 board
#
# 145   2/21/11 5:22p spothana
# SW7422-96: Correct the warning message for BCM97425SV board usage.
#
# 144   2/21/11 5:16p spothana
# SW7422-96: remove an extra endif
#
# 143   2/21/11 5:09p spothana
# SW7422-96: Remove 3255 support for BCM97422 for now.
#
# 142   2/18/11 4:56p hongtaoz
# SW7425-9: remove 3255 frontend from 7425 build;
#
# 141   2/18/11 1:57p hongtaoz
# SW7425-9: merge the 7425 support;
#
# 140   2/17/11 10:38p nickh
# SW7422-10: Add 7422 and 7425 support
#
# 139   2/2/11 5:21p garetht
# SW7420-1457:sync sms & vms platforms to 97420 latest
#
# 138   12/15/10 2:39p xhuang
# SW7358-21: Add 97358/97552 support
#
# 137   12/10/10 4:34p erickson
# SW7420-1148: add simple_decoder
#
# 136   12/10/10 4:34p nitinb
# SW7420-1308: Added dma/stub/dma.inc for 7550
#
# 134   11/22/10 12:31p mward
# SW7125-729: Add NEXUS_FRONTEND_3412 for 7125 platforms.
#
# SW7125-729/1   11/10/10 5:31p mward
# SW7125-729: Add NEXUS_FRONTEND_3412 for 7125 platforms.
#
# 133   11/18/10 4:24p jgarrett
# SW7408-1: Removing ir_blaster for 7408 a0
#
# 132   11/5/10 10:32a gmohile
# SW7408-1 : Add ir_blaster suppr for 7408 B0
#
# SW7420-1177/1   11/1/10 9:04p spothana
# SW7420-1177: Adding nexus hdmi dvo module.
#
# 131   10/26/10 11:33a jtna
# SW7420-887: added temp_monitor
#
# SW7420-887/2   10/21/10 7:24p spothana
# SW7420-887: Correct the coding conventions
#
# SW7420-887/1   10/20/10 7:44p spothana
# SW7420-887: Add temperature module.
#
# 130   9/29/10 2:13p erickson
# SW7420-1123: consolidate 3D support using GL_SUPPORT variable
#
# 129   9/29/10 10:53a erickson
# SW7420-1123: refactor platform_modules.inc so that it can fork per
#  platform
#
# 128   9/29/10 9:33a erickson
# SW7420-1123: add linux usermode IPC
#
# 127   9/24/10 3:34p jtna
# SW7420-887: remove duplicate brcm_log keyword
#
# 123   8/26/10 4:23p jgarrett
# SW7408-52: Fixing up 7405 soft audio support
#
# 122   8/24/10 5:49p mward
# SW7125-35:  NEXUS_FRONTEND_3109 and NEXUS_FRONTEND_3112 either-or.
#  NEXUS_FRONTEND_3112 is the default if NEXUS_FRONTEND_3109=y is not
#  given.
#
# 121   8/24/10 3:04p mward
# SW7125-35:  Support 3109 QAM in addition to / instead of 3112 on 97125
#  platform.
#
# 120   7/26/10 10:19a mward
# SW7125-513:  Add NEXUS_FRONTEND_7125, default y, for host control of
#  7125 tuner/ds.
#
# 119   7/22/10 5:08p randyjew
# SW7468-247: Adding 3112 frontend support to 97208
#
# SW7125-513/1   7/12/10 4:43p mward
# SW7125-513:  Add NEXUS_FRONTEND_7125, default y, for host control of
#  7125 tuner/ds.
#
# 118   6/25/10 3:09p mward
# SW7125-35: Exclude BHAB from proxy build for all platforms, not just
#  93380.
#
# 117   6/11/10 3:36p mward
# SW7125-35: Eliminate the NEXUS_97125_HOST_3112 build option, always
#  build 97125 with support for host control of BCM3112 QAM DS. Eliminate
#  build option NEXUS_97125_BYPASS_3112.  No longer needed.
#
# 116   6/9/10 4:10p prasadv
# SW7400-2546: Adding 97456 platform support.
#
# 115   4/26/10 11:35a mward
# SW7125-358:  Enable Smartcard for 7125 family.
#
# SW7125-358/2   4/20/10 12:35p mward
# SW7125-358:  Remove duplicate log keyword.
#
# sw7550_125/5   2/24/10 4:51p nitinb
# SW7550-220: Add support for 7550
#
# sw7550_125/4   1/20/10 11:35a nitinb
# SW7550-193: Merge
#
# sw7550_125/SW7550-193/1   1/19/10 3:56p chengli
# SW7550-193 : exclude HDMI output src for 97530 CRB
#
# sw7550_125/3   1/18/10 11:30a nitinb
# SW7550-194: Enable ASTM for 7550
#
# sw7550_125/2   12/30/09 2:51p nitinb
# SW7550-102: Enable security for 7550
#
# sw7550_125/1   12/16/09 10:15a nitinb
# SW7550_125: Added build variables NEXUS_POWER_MANAGEMENT and
#  PVR_SUPPORT needed for 7550. Enabled smart card support for 7550.
#
# 110   3/23/10 2:24p gmohile
# SW7408-1 : Enable security for 7408
#
# 109   3/9/10 2:16p ismailk
# SW7420-618: Exclude astm.inc and sync_channel.inc from 93380sms build.
#
# 108   3/9/10 10:39a mward
# SW7400-2511: add NEXUS_ namespace to some remaining PLATFORM instances.
#
# 107   3/3/10 4:37p mward
# SW7400-2673: Remove restriction on build with both 4500 and 4501/4506,
#  default NEXUS_FRONTEND_4500=y.
#
# 106   3/2/10 8:05a vsilyaev
# SW7405-2993: Fixed kernel mode compilation error (BXDM should be
#  compiled only in the kernel mode)
#
# SW7405-2993/1   3/1/10 11:41p nilesh
# SW7405-2993: Fixed kernel mode compilation error (undefined reference
#  to BTMR_ReadTimer_isr)
#
# 105   2/25/10 6:21p mward
# SW7400-2673: Implement support for BCM4500.
#
# SW7400-2673/1   2/22/10 1:46p mward
# SW7400-2673:  Support BCM4500.
#
# 104   2/10/10 6:51p randyjew
# SW7468-6: Remove keypad support for 97468/97208
#
# 103   2/2/10 3:38p gmohile
# SW7408-1 : Add dma support for 7408
#
# 102   2/1/10 1:29p garetht
# SW7420-553: 93380VMS fails in kernel mode build. BHAB is compiled and
#  references BI2C which is not compiled
#
# 101   1/21/10 5:35p garetht
# SW7420-553: Add platform for 93380vms
#
# 99   1/20/10 2:29p garetht
# SW7420-553: Add platform for 93380vms
#
# 98   1/19/10 2:39p nickh
# SW7420-541: Rename 97420dvr2 to 97420
#
# 97   1/19/10 4:51p nissen
# SW7405-3671: Added support for building with GRC packet blits.
#
# 96   12/10/09 10:46a randyjew
# SW7468-6:update 7468/7208 support
#
# 95   12/2/09 3:40p gmohile
# SW7408-1 : Compile softaudio for 7408
#
# 94   12/1/09 5:24p randyjew
# SW7468-6: Add 7468 support
#
# 93   11/16/09 1:25p katrep
#
#
# SW7405-3408:replace findstring with filter to do exact word match
#
# 92   11/10/09 11:09a erickson
# SW7405-3351: reorder includes to correctly generated
#  nexus_driver_modules.h
#
# 91   11/6/09 2:44p gmohile
# SW7408-1 : Add 7408 support
#
# 90   11/3/09 3:02p mward
# SW7125-35: include 3112 header for registers needed for
#  NEXUS_97125_BYPASS_3112.
#
# 89   10/30/09 5:06p nickh
# SW7420-331: Rename 97410c to 97410
#
# 88   10/27/09 4:26p nitinb
# SW7550-29: Add 7550 support
#
# 87   10/20/09 4:42p garetht
# SW7420-386:  includes have to be in the correct order else we break
#  other platforms
#
# 86   10/19/09 6:35p garetht
# SW7420-386: Rebase 93380sms codebase to 97420 Phase 3.0
#
# 85   10/19/09 5:35p garetht
# SW7420-386:  Rebase 93380sms codebase to 97420 Phase 3.0
#
# 84   10/15/09 6:46p mward
# SW7125-35: NEXUS_97125_HOST_3112=y build option.
#
# 83   10/14/09 2:19p mward
# SW7125-35:  Support control of BCM3112's by host.
#
# 82   10/8/09 6:04p haisongw
# SW7405-3091:Add 97205CB Platform support
#
# 81   10/7/09 2:02p lwhite
# SW7468-6: Add 7468 support
#
# 80   10/1/09 7:34p nickh
# SW7420-223: Enable Nexus Security
#
# 79   9/24/09 10:50a mward
# SW7125-4: Remove smartcard after RDB header update.  There is no SCA in
#  the 7125 chip.
#
# 78   9/16/09 1:42p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ namespace to build variables
#
# 77   8/26/09 12:06p mward
# SW7125-4: Adding 97019 platform.
#
# 76   8/25/09 11:40a nickh
# SW7420-331: Add 97410c support
#
# 75   8/12/09 10:12p jrubio
# PR55232: disable powermanagement for 7342/7340
#
# 74   8/10/09 5:27p nickh
# PR56017: Disable Security for 7420B0 bringup
#
# 73   8/6/09 3:32p jrubio
#  PR55232: remove rfm from 7340/7342
#
# 72   7/30/09 11:14a gmohile
# PR 56512 : Merge 7401, 7403 support to mainline
#
# 71   7/16/09 2:51p mward
# PR 55545: PR 55989: Restoring rev. 69 change lost in prior checkin.
#
# 70   7/15/09 6:40p mward
# PR 55545: Support 7125.
#
# 69   7/14/09 5:17p vsilyaev
# PR 55989: Don't compile OTFPVR engine into the proxy
#
# 68   6/26/09 10:51a leisun
# PR 55693: checking in DTCP: add 7420 support for HW M2M
#
# 67   6/9/09 11:34a rjlewis
# PR55851: don't enable opengles for VxWorks
#
# 66   5/29/09 6:04p jrubio
# PR55232: add 7340/7342
#
# 65   4/16/09 10:38a nickh
# PR53912: Add 7420 MPOD support
#
# 64   4/15/09 12:26p jrubio
#  PR50837: allow dvb-ci for any supported platform. Set
#  NEXUS_DVB_CI_SUPPORT
#
# 63   4/14/09 6:28p jrubio
# PR 50409: fix dvb-ci
#
# 62   4/14/09 2:13p jrubio
# PR 50409: add 7335/7336 DVB-CI support
#
# 61   4/1/09 6:22p katrep
# PR50567: Diabled IR blaster for 7466
#
# 60   3/26/09 11:27a erickson
# PR53323: added NEXUS_BASE_ONLY option
#
# 59   3/13/09 12:03p vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
#
# 58   3/5/09 3:18p katrep
# PR52599: More support for 7205
#
# 57   3/4/09 8:57p vishk
# PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
#
# 56   3/4/09 6:15p katrep
# PR52599: Disabled uhf input for 7205
#
# 55   3/3/09 4:17p vishk
# PR 46431: DS PI: Add support for BCM3117, BCM3114, and BCM3112
#
# 54   3/2/09 4:48p mward
# PR52553: Fix 97400 build.
#
# 53   2/27/09 9:57a katrep
# PR52553: Add 7405-IFE board support
#
# 52   2/25/09 7:06p nickh
# PR52525: Implement SW workaround for 7420 3D graphics demos
#
# 51   2/23/09 5:47p mward
# PR47739: Support 4501 and 4506 tuner cards.
#
# 50   2/13/09 12:51p erickson
# PR35457: added comment
#
# 49   2/9/09 10:14p nickh
# PR51414: Enable HDMI module for 7420
#
# 48   2/9/09 5:03p jgarrett
# PR 50837: Merge to main branch
#
# 47   1/30/09 3:52p katrep
# PR50567: Disabled UHF input for 97466
#
# 46   1/28/09 3:17p jrubio
# PR51629: fix typo
#
# PR50837/1   1/29/09 10:37a jgarrett
# PR 50837: Adding 973xx frontend extensions
#
# 45   1/27/09 4:42p jrubio
# PR51629: add 7336
#
# 44   1/12/09 11:45a katrep
# PR50472: Disable GL_SUPPORT=y for 7405 C0 till kernelmode compilation
#  is resolved
#
# 43   12/30/08 2:34p jgarrett
# PR 50409: Adding dvb_ci for 7325
#
# 42   12/24/08 12:40p jgarrett
# PR 50703: Removing security and hdmi_output temp for 97420
#
# 41   12/22/08 7:06p katrep
# PR50472: Enabled 3D graphics for 7405 C0(7413)
#
# 40   12/19/08 7:48p katrep
# PR50567: Add support for 97466 platform
#
# 39   12/8/08 9:11a nickh
# PR48963: Fix 3255 frontend build issue for 97420dvr2
#
# 38   12/5/08 12:36p nickh
# PR50043: Update to reflect change in PLATFORM variable name from
#  97420DVR2 to 97420dvr2
#
# 37   12/4/08 4:03p jgarrett
# PR 49984: Adding 93520vsbdc support
#
# 36   11/19/08 2:52p nickh
# PR48963: Add 97420DVR2 support
#
# 35   11/5/08 5:37p mphillip
# PR48697: Correct 3D building for 7400 nexus 3D (and future chips) to
#  match 3549/3556 implementation
#
# 34   11/3/08 4:49p katrep
# PR48628: Add PWM to 7405 build
#
# 33   10/9/08 11:48a erickson
# PR47329: rename var to NEXUS_EXCLUDED_MODULES
#
# 32   10/8/08 12:38a erickson
# PR47375: move platform.inc to the correct place in the list
#
# 31   9/26/08 12:50p erickson
# PR47375: added PLATFORM_INC override
#
# 30   9/23/08 9:16a jgarrett
# PR 46513: Merging 97459 support to mainline
#
# 29   9/3/08 3:12p jrubio
# PR45727: fix uart include
#
# 28   8/22/08 7:52p katrep
# PR45975: Add support for 7405 MSG platform
#
# 27   8/21/08 3:17p katrep
# PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
#
# PR44830/1   7/16/08 5:58p anilmm
# PR44830: Add support for 97459
#
# 26   7/10/08 5:13p mphillip
# PR40869: Add 3D to the build process
#
# 25   5/29/08 10:43a jrubio
# PR42353: add 7335 to list to exclude 3520/dtt760z tuners
#
# 24   5/28/08 1:40p vsilyaev
# PR 42931: Sorted list of modules to exclude from the proxy build
#
# 23   5/28/08 12:13p erickson
# PR42931: add BASTMLIB to fix kernel mode
#
# 22   5/19/08 12:46p erickson
# PR40034: readd smartcard
#
# 21   5/19/08 12:33p erickson
# PR40034: backing smartcard out until all code is in
#
# 20   5/16/08 7:47p vishk
# PR 40034: Develop Nexus Smartcard Interface/Module.
# PR 40016: bsettop_smartcard: Develop SettopAPI-to-Nexus shim layer
#
# 19   4/24/08 12:59p erickson
# PR42113: add InputCapture
#
# 18   4/2/08 5:49p shyam
# PR39899: Add 7043 Transcoder support
#
# 18   4/2/08 5:46p shyam
# PR39899: Add 7043 transcoder support
#
# 17   3/31/08 4:41p erickson
# PR40817: compile with PVR_SUPPORT=n
#
# 16   3/27/08 8:20p bandrews
# PR40090: added astm module
#
# 12   3/13/08 7:41p jgarrett
# PR 39985: Adding 450x support
#
# 11   3/5/08 11:42a jrubio
# PR40169: fixing problems
#
# 10   3/5/08 11:40a jrubio
# PR40169: remove frontend code for 7325
#
# 9   3/4/08 9:51a jgarrett
# PR 39984: Adding 3510/3420 support
#
# 9   3/4/08 9:48a jgarrett
# PR 39984: Adding 3510/3420 support
#
# 8   2/29/08 1:57p erickson
# PR38682: fix 7405 kernel mode build
#
# PR39899/1   2/27/08 5:52p shyam
# PR 39899 : Integrate 7043 XCD into Nexus
#
# 7   2/20/08 1:44p erickson
# PR39405: added RFM
#
# 6   2/8/08 6:36p jgarrett
# PR 39100: Adding 2940 support
#
# 5   2/6/08 12:41p vsilyaev
# PR 38682: Added synclib
#
# 4   2/4/08 11:42a erickson
# PR36808: added SyncChannel
#
# 3   2/1/08 5:25p jgarrett
# PR 39017: Adding HdmiOutput
#
# 2   1/24/08 4:33p vsilyaev
# PR 38682: Improved kernel/proxy environment
#
# 1   1/18/08 2:17p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/14   1/15/08 6:48p vsilyaev
# PR 38682: Don't compile PVR into the linuxkernel
#
# Nexus_Devel/13   12/13/07 9:54a jgarrett
# PR 38178: Moving core.inc to proper location
#
# Nexus_Devel/12   12/6/07 11:37a jgarrett
# PR 37931: Removing UART from default build - not needed on linux
#
# Nexus_Devel/11   12/3/07 9:28p jgarrett
# PR 37837: Adding DMA
#
# Nexus_Devel/10   11/21/07 11:06a erickson
# PR37423: add io modules
#
# Nexus_Devel/9   11/15/07 4:01p erickson
# PR37137: security update
#
# Nexus_Devel/8   11/8/07 2:47p vsilyaev
# PR 36788: PVR record module
#
# Nexus_Devel/7   10/19/07 9:39a erickson
# PR36275: added graphics2d
#
# Nexus_Devel/6   10/17/07 11:25a erickson
# PR36152: remove 3520 from 97405
#
# Nexus_Devel/5   10/10/07 5:11p vsilyaev
# PR 35824: Added file module
#
# Nexus_Devel/4   10/10/07 11:29a jgarrett
# PR 35551: Adding frontend modules for 9740x boards
#
############################################################


ifeq ($(NEXUS_BASE_ONLY),y)
$(warning NEXUS_BASE_ONLY is defined)
# Minimum platform_modules.inc to run with just base and core
include $(NEXUS_TOP)/platforms/common/build/platform.inc
include $(NEXUS_TOP)/modules/core/core.inc
include $(NEXUS_TOP)/base/base.inc
NEXUS_PLATFORM_DEFINES+=NEXUS_BASE_ONLY

else
# Define Module Options
ifeq ($(NEXUS_PLATFORM),97205)
NEXUS_FRONTEND_3510 := y
NEXUS_FRONTEND_3420 := y
NEXUS_FRONTEND_4501 := y
NEXUS_FRONTEND_4501_VER := C0
NEXUS_FRONTEND_4506 := y
NEXUS_FRONTEND_4506_VER := A1
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97420 97410), $(NEXUS_PLATFORM))
NEXUS_MPOD_SUPPORT :=n
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97125 97019), $(NEXUS_PLATFORM))
NEXUS_FRONTEND_3255 :=y
endif

ifeq ($(NEXUS_PLATFORM),97420)
    ifeq ($(NEXUS_PLATFORM_7420_DBS),y)
        NEXUS_FRONTEND_4506 := y
        NEXUS_FRONTEND_4506_VER := A1
    endif
    ifeq ($(NEXUS_PLATFORM_7420_CABLE),y)
        NEXUS_FRONTEND_3255 := y
    endif
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97422), $(NEXUS_PLATFORM))
    ifeq ($(NEXUS_PLATFORM_7422_DBS),y)
        NEXUS_FRONTEND_4506 := y
        NEXUS_FRONTEND_4506_VER := A1
    else
        ifeq ($(NEXUS_USE_7422_SV_BOARD),y)
            NEXUS_FRONTEND_3112 := y
            NEXUS_FRONTEND_31xx_VER := B0
            NEXUS_PLATFORM_7422_CABLE := y
        else
            ifeq ($(NEXUS_USE_7422_VMS_SFF),y)
                NEXUS_FRONTEND_3128 := y
                NEXUS_FRONTEND_312x_OOB := y
                NEXUS_PLATFORM_7422_CABLE := y
                ifeq ($(NEXUS_FRONTEND_312x_VER),)
                    NEXUS_FRONTEND_312x_VER := C0
                endif
            endif
        endif
    endif
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97425 97435 93335), $(NEXUS_PLATFORM))
    ifeq ($(NEXUS_USE_FRONTEND_DAUGHTER_CARD),y)
        NEXUS_USE_FRONTEND_DAUGHTER_CARD := y
    endif
    ifeq ($(NEXUS_PLATFORM_7425_DBS),y)
        NEXUS_FRONTEND_4506 := y
        NEXUS_FRONTEND_4506_VER := A1
    endif

    ifeq ($(NEXUS_USE_7425_SV_BOARD),y)
        NEXUS_FRONTEND_3112 := y
        NEXUS_PLATFORM_7425_CABLE := y
    else
        ifeq ($(NEXUS_USE_7425_VMS_SFF),y)
            NEXUS_PLATFORM_7425_CABLE := y
            ifeq ($(NEXUS_PLATFORM_DOCSIS_PLUS_BCM3128_IB_SUPPORT),y)
                NEXUS_FRONTEND_3255 := y
                NEXUS_FRONTEND_3128 := y
                NEXUS_FRONTEND_312x := NEXUS_FRONTEND_3128
            endif
            ifeq ($(NEXUS_PLATFORM_DOCSIS_IB_SUPPORT),y)
                NEXUS_FRONTEND_3255 := y
            else
                NEXUS_FRONTEND_3128 := y
            endif
            ifeq ($(NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT),y)
                NEXUS_FRONTEND_3255 := y
            else
                NEXUS_FRONTEND_312x_OOB := y
            endif
        else
            ifeq ($(NEXUS_PLATFORM_7425_CABLE),y)
                NEXUS_FRONTEND_3114 := y
                NEXUS_FRONTEND_3112 := y
           endif
        endif
    endif

    ifeq ($(NEXUS_PLATFORM_7425_CABLE),y)
        ifeq ($(NEXUS_FRONTEND_312x_VER),)
            NEXUS_FRONTEND_312x_VER := C0
        else
            NEXUS_FRONTEND_312x_VER := $(NEXUS_FRONTEND_312x_VER)
        endif

        ifeq ($(NEXUS_FRONTEND_3112_VER),)
            NEXUS_FRONTEND_31xx_VER := B0
        else
            NEXUS_FRONTEND_31xx_VER := $(NEXUS_FRONTEND_3112_VER)
        endif
        ifeq ($(NEXUS_FRONTEND_3114_VER),)
            NEXUS_FRONTEND_31xx_VER := B0
        else
            NEXUS_FRONTEND_31xx_VER := $(NEXUS_FRONTEND_3114_VER)
        endif
        ifeq ($(NEXUS_FRONTEND_3117_VER),)
            NEXUS_FRONTEND_31xx_VER := B0
        else
            NEXUS_FRONTEND_31xx_VER := $(NEXUS_FRONTEND_3117_VER)
        endif
    endif

    ifeq ($(NEXUS_PLATFORM_4528_DBS),y)
        NEXUS_FRONTEND_4528 := y
        NEXUS_FRONTEND_4528_VER := A0
    endif

	ifeq ($(NEXUS_FRONTEND_3128),y)
		NEXUS_HAS_FRONTEND := y
		NEXUS_FRONTEND_3128 :=y
	endif 
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97125), $(NEXUS_PLATFORM))
    NEXUS_FRONTEND_3112 := y
    NEXUS_FRONTEND_31xx_VER := B0
    NEXUS_FRONTEND_3109 := y
    NEXUS_FRONTEND_31xx_VER := B0
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97125 97019), $(NEXUS_PLATFORM))
    NEXUS_FRONTEND_7125 ?= y
    NEXUS_FRONTEND_3412 ?= y
endif

ifeq ($(NEXUS_PLATFORM),97208)
    ifeq ($(NEXUS_PLATFORM_VER),C)
    NEXUS_FRONTEND_3112 := y
    NEXUS_FRONTEND_31xx_VER := B0
    endif
endif

ifeq ($(NEXUS_PLATFORM),97400)
NEXUS_FRONTEND_3520 := y
NEXUS_FRONTEND_DTT760X := y
NEXUS_FRONTEND_4500 := y
NEXUS_FRONTEND_4501 := y
NEXUS_FRONTEND_4501_VER := C0
NEXUS_FRONTEND_4506 := y
NEXUS_FRONTEND_4506_VER := A1
endif

ifneq ($(B_REFSW_OS),vxworks)
ifeq ($(filter $(NEXUS_PLATFORM), 97400), $(NEXUS_PLATFORM))
GL_SUPPORT ?= y
endif
endif

ifeq ($(NEXUS_DISABLE_FRONTEND),)
ifeq ($(filter $(NEXUS_PLATFORM), 97325 97335 97340 97342),$(NEXUS_PLATFORM))
NEXUS_FRONTEND_EXTENSION_INC := $(NEXUS_TOP)/extensions/frontend/$(NEXUS_PLATFORM)/frontend_ext.inc
NEXUS_FRONTEND_3440 := y
endif
endif

# Include Modules

# allow users to use a customized platform.inc for their board
# this must be placed before all other module .inc's for IMG loading support
ifeq ($(PLATFORM_INC),)
PLATFORM_INC=$(NEXUS_TOP)/platforms/common/build/platform.inc
endif
include $(PLATFORM_INC)

#
# After PLATFORM, list all modules in order from least to most dependencies.
# This is required to handle automatic closing on abnormal application termination.

include $(NEXUS_TOP)/modules/core/core.inc
include $(NEXUS_TOP)/modules/pwm/pwm.inc
include $(NEXUS_TOP)/modules/i2c/i2c.inc
include $(NEXUS_TOP)/modules/gpio/gpio.inc
include $(NEXUS_TOP)/modules/led/led.inc

include $(NEXUS_TOP)/modules/ir_input/ir_input.inc

ifneq ($(filter $(NEXUS_PLATFORM), 97466 97468 97208),$(NEXUS_PLATFORM))
    ifeq ($(NEXUS_PLATFORM),97408)
        ifneq ($(BCHP_VER),A0)
        include $(NEXUS_TOP)/modules/ir_blaster/ir_blaster.inc
        endif
    else
        include $(NEXUS_TOP)/modules/ir_blaster/ir_blaster.inc
    endif
endif

include $(NEXUS_TOP)/modules/input_capture/input_capture.inc

ifneq ($(filter $(NEXUS_PLATFORM), 97466 97205 97125 97019 97468 97208 97550 97358 97552 97445),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/uhf_input/uhf_input.inc
ifneq ($(filter $(NEXUS_PLATFORM), 97340 97346 97344 97231 97230 97342 97550 97408 97422 97425 97435 97358 93335),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/rfm/rfm.inc
endif
endif
ifeq ($(HUMAX_PLATFORM_BASE),y)
    ifneq ($(filter $(NEXUS_PLATFORM),97468 97208),$(NEXUS_PLATFORM))
    include $(NEXUS_TOP)/modules/keypad/keypad.inc
    endif
else
ifneq ($(filter $(NEXUS_PLATFORM),97468 97208 97425 97435),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/keypad/keypad.inc
endif
endif
ifeq ($(filter $(NEXUS_PLATFORM), 97425 97435), $(NEXUS_PLATFORM))
ifeq ($(NEXUS_USE_7425_SV_BOARD),y)
include $(NEXUS_TOP)/modules/keypad/keypad.inc
endif
endif

ifeq ($(NEXUS_DISABLE_FRONTEND),)
ifneq ($(filter $(NEXUS_PLATFORM), 97408),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/frontend/build/frontend.inc
endif
endif

ifneq ($(filter $(NEXUS_PLATFORM), 97408),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/spi/spi.inc
endif

ifneq ($(filter $(NEXUS_PLATFORM), 97445),$(NEXUS_PLATFORM))
ifeq ($(filter $(NEXUS_PLATFORM), 97408 97550),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/security/7550/security.inc
else
include $(NEXUS_TOP)/modules/security/security.inc
endif
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97422 97425 97435 97445),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/picture_decoder/picture_decoder.inc
endif


ifeq ($(filter $(NEXUS_PLATFORM), 97422 97425 97435 97445 93335),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/hdmi_input/hdmi_input.inc
endif


ifeq ($(GL_SUPPORT),)
ifeq ($(V3D_SUPPORT),)
V3D_SUPPORT = $(shell test -e ${NEXUS_TOP}/../rockford/middleware/v3d/khronos.inc && echo y)
endif
endif
ifeq ($(V3D_SUPPORT),y)
ifneq ($(filter $(NEXUS_PLATFORM), 97208 97468),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/graphics3d/graphics3d.inc
endif
endif

include $(NEXUS_TOP)/modules/dma/dma.inc
include $(NEXUS_TOP)/modules/transport/transport.inc

include $(NEXUS_TOP)/modules/video_decoder/video_decoder.inc
include $(NEXUS_TOP)/modules/audio/audio.inc

include $(NEXUS_TOP)/modules/surface/surface.inc
include $(NEXUS_TOP)/modules/graphics2d/graphics2d.inc
include $(NEXUS_TOP)/modules/display/display.inc
include $(NEXUS_TOP)/modules/astm/astm.inc
include $(NEXUS_TOP)/modules/sync_channel/sync_channel.inc

ifneq ($(NEXUS_BOARD_7530_CRB),y)
    ifeq ($(NEXUS_DVO_DVI_LOOPBACK_SUPPORT),y)
    include $(NEXUS_TOP)/modules/hdmi_dvo/hdmi_dvo.inc
    else
    include $(NEXUS_TOP)/modules/hdmi_output/hdmi_output.inc
    endif
endif

include $(NEXUS_TOP)/modules/cec/cec.inc

ifneq ($(filter $(NEXUS_PLATFORM), 97340 97342 97205 97408),$(NEXUS_PLATFORM))
    include $(NEXUS_TOP)/modules/smartcard/smartcard.inc
endif

ifeq ($(NEXUS_DVB_CI_SUPPORT),y)
include $(NEXUS_TOP)/modules/dvb_ci/dvb_ci.inc
endif

ifneq ($(filter $(NEXUS_PLATFORM), 97550),$(NEXUS_PLATFORM))
include $(NEXUS_TOP)/modules/surface_compositor/surface_compositor.inc
include $(NEXUS_TOP)/modules/input_router/input_router.inc
endif
include $(NEXUS_TOP)/modules/simple_decoder/simple_decoder.inc

ifeq ($(HUMAX_PLATFORM_BASE),y)
	ifeq ($(CONFIG_BCM_UART),y)
		include $(NEXUS_TOP)/modules/uart/uart.inc
	else
ifeq ($(filter $(B_REFSW_OS),linuxuser linuxkernel bare),)
# Include uart for non-linux systems only.
			include $(NEXUS_TOP)/modules/uart/uart.inc
		endif
	endif
else
ifeq ($(filter $(B_REFSW_OS),linuxuser linuxkernel bare),)
# Include uart for non-linux systems only.
include $(NEXUS_TOP)/modules/uart/uart.inc
endif
endif

ifeq ($(NEXUS_TOUCHPAD_SUPPORT),y)
include $(NEXUS_TOP)/modules/touchpad/touchpad.inc
endif

ifeq ($(NEXUS_TEMP_MONITOR_SUPPORT),y)
include $(NEXUS_TOP)/modules/temp_monitor/temp_monitor.inc
endif

ifneq ($(filter $(B_REFSW_OS),linuxkernel bare),$(B_REFSW_OS))
NEXUS_USERMODE=y
endif

ifeq ($(MSDRM_PRDY_SUPPORT),y)
# HUMAX_PLATFORM_BASE
#include $(NEXUS_TOP)/modules/wmdrmpd_core/wmdrmpd_core.inc
#include $(NEXUS_TOP)/modules/wmdrmpd_io/wmdrmpd_io.inc
endif

ifeq ($(filter $(NEXUS_PLATFORM), 97425 97435 97445 93335),$(NEXUS_PLATFORM))
ifneq (${VIDEO_ENCODER_SUPPORT},n)
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE
include $(NEXUS_TOP)/modules/video_encoder_v1243/video_encoder.inc
else
include $(NEXUS_TOP)/modules/video_encoder/video_encoder.inc
endif
include $(NEXUS_TOP)/modules/stream_mux/stream_mux.inc
ifeq ($(NEXUS_USERMODE),y)
    include $(NEXUS_TOP)/modules/file_mux/file_mux.inc
endif
endif
endif


include $(NEXUS_TOP)/base/base.inc

ifeq ($(NEXUS_USERMODE),y)
    include $(NEXUS_TOP)/modules/file/file.inc
ifneq ($(PVR_SUPPORT),n)
    # Don't compile modules that do file I/O into the kernel
        include $(NEXUS_TOP)/modules/playback/playback.inc
    endif
    include $(NEXUS_TOP)/modules/record/record.inc
endif

ifeq ($(MSDRM_PRDY_SUPPORT),y)
ifeq ($(NEXUS_USERMODE),y)
#	HUMAX_PLATFORM_BASE
#include $(NEXUS_TOP)/modules/wmdrmpd/wmdrmpd.inc
endif
endif

# GL_SUPPORT is for PX3D Graphics
ifeq ($(GL_SUPPORT),y)
    ifeq ($(NEXUS_USERMODE),y)
        include $(NEXUS_TOP)/modules/graphics3d/graphics3d.inc
    endif
endif

ifeq ($(NEXUS_MPOD_SUPPORT),y)
NEXUS_PLATFORM_DEFINES += NEXUS_HAS_MPOD
endif

#
# SPI Front Panel
# Defaults on for Specified Platforms.
# Set NEXUS_SPI_FRONTPANEL=n to turn off.
#
ifneq ($(filter $(BCHP_CHIP),7335 7336 7325 7342 7340),)
ifneq ($(NEXUS_SPI_FRONTPANEL),n)
NEXUS_CFLAGS += -DNEXUS_HAS_SPI_FRONTPANEL=1
endif
endif

endif #NEXUS_BASE_ONLY
