############################################################
#     (c)2007-2012 Broadcom Corporation
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
# $brcm_Workfile: frontend.inc $
# $brcm_Revision: SW7435-433/1 $
# $brcm_Date: 11/8/12 11:45a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/frontend/build/frontend.inc $
# 
# SW7435-433/1   11/8/12 11:45a mward
# SW7125-1321: SW7435:433: 7125 uses platform-specifc versions of 3255
#  files.
# 
# 133   9/24/12 7:17p nickh
# SW7425-3990: Fix compile error when building for 3383 only
# 
# 132   9/14/12 5:22p mward
# SW7425-3922: changed VSB directory structure to remove symlinks
# 
# 131   9/7/12 2:10p erickson
# SW7425-3819: remove 2940 support
# 
# 130   9/7/12 1:57p vishk
# SW7425-3765, SW7425-3788: Remove symbolic links in pi
# 
# 129   8/3/12 11:32a jrubio
# SW7346-848: add 3472 logic
# 
# 128   7/30/12 9:05a erickson
# SW7435-235: compile FW from magnum
# 
# 127   7/17/12 12:07p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 126   6/27/12 3:05p jtna
# SW7425-3350: remove NEXUS_FRONTEND_FIXED_MTSIF_CONFIG compile option.
#  it's no longer needed
# 
# 125   6/21/12 4:08p jtna
# SW7425-2983: flatten ADS/AOB directory structure
# 
# 124   5/30/12 2:30p jtna
# SW7425-3018: remove old, unused DTV code
# 
# 123   5/17/12 4:07p mphillip
# SWSATFE-196: HR44 requires a specific SNR estimate flag
# 
# 122   5/14/12 4:15p jtna
# SW7425-2752: include BMXT for all 3128/3383/4528-using platforms
# 
# 121   5/13/12 5:05p jtna
# SW7425-2752: add NEXUS_FRONTEND_FIXED_MTSIF_CONFIG option to support
#  legacy mode
# 
# 120   5/10/12 6:52p jtna
# SW7425-2752: rework MTSIF-frontend <-> host interface
# 
# 119   5/7/12 6:14p vishk
# SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
# 
# 118   5/3/12 2:28p bselva
# SW7360-19: Add support for 7360 platform
# 
# 117   3/7/12 11:32a vishk
# SW7231-697: Setting the default to B0
# 
# 116   3/5/12 5:54p vishk
# SW7231-697: bhab_3461_priv.c does not compile when
#  NEXUS_FRONTEND_3461_VER="B1"
# 
# 115   1/13/12 10:32a vishk
# SW7552-147: 7552B0: Add Nexus OOB support Fix Typo.
# 
# 114   1/12/12 3:11p vishk
# SW7552-147: 7552B0: Add Nexus OOB support Disable OOB for A0
# 
# 113   1/11/12 5:06p vishk
# SW7552-147: 7552B0: Add Nexus OOB support
# 
# 112   1/3/12 2:14p vishk
# SW7552-147: 7552B0: Add Nexus OOB support
# 
# 111   12/28/11 4:18p vishk
# SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board
# 
# 109   12/16/11 10:05p vishk
# SW3128-92: Add FW auto detection support Add NEXUS_FRONTEND_312x_OOB
#  for conditional compilation of 3128's OOB.
# 
# 108   12/15/11 6:07p vishk
# SW3128-92: Add FW auto detection support. Add nexus support.
# 
# 107   12/5/11 2:37p jtna
# SW7425-1437: use BMXT to replace previous hack
# 
# 106   12/1/11 4:13p mphillip
# SW7425-1304: Change 4528 to HAB
# 
# 105   11/29/11 5:20p vishk
# SW7425-1722: - NEXUS_Frontend_GetQamStatus prints "Error reading
#  status." and returned NEXUS_TIMEOUT
# 
# 103   11/2/11 8:01p vishk
# SW7425-1598: Add 3128B0/C0 support Fix the NEXUS_FRONTEND_3128_VER to
#  NEXUS_FRONTEND_312x_VER.
# 
# 102   11/1/11 5:00p mphillip
# SW7425-1437: Merge 4528 to main
# 
# 101   10/31/11 1:47p vishk
# SW7425-1598: Add support for 3128B0/C0
# 
# 100   10/27/11 3:11p vishk
# SW7425-1598: Add support for 3128B0/C0
# 
# 100   10/27/11 3:09p vishk
# SW7425-1598: Add support for 3128B0/C0
# 
# 99   10/26/11 6:14p vishk
# SW7425-1598: Add support for 3128B0/C0
# 
# 98   10/21/11 7:27p jtna
# SW7425-1483: update to adapt to new bmxt.inc
# 
# 97   10/19/11 12:12p vishk
# SW7231-400: Add support for 3461B0 on new 7231B0 EUSFF board Add
#  conditional compiles for A0vsB0.
# 
# 96   10/19/11 10:25a vishk
# SW7231-400: Add support for 3461B0 on new 7231B0 EUSFF board
# 
# 95   10/12/11 4:38p vishk
# SW3128-46: add support for BCM3124 devices Fix frontend card support
#  enable/disable.
# 
# 94   10/12/11 3:56p vishk
# SW3128-46: add support for BCM3124 devices
# 
# 93   10/11/11 6:52p vishk
# SW3128-46: add support for BCM3124 devices
# 
# 92   10/7/11 2:31p vishk
# SW3128-47: Add Support for BCM3123
# 
# 91   10/5/11 4:16p jtna
# SW7425-1189: add 3383 TSMF support
# 
# 90   10/5/11 12:23p jtna
# SW7425-1189: add transport and frontend TSMF support
# 
# 89   9/26/11 6:32p randyjew
# SW7344-140: Adding 3123 support
# 
# 88   8/18/11 2:54p bselva
# SW7358-95: symlink 7358 to 7346
# 
# 87   7/25/11 3:30p jrubio
# SW7344-117: enable 7344 g3 compilation
# 
# 86   7/19/11 4:47p vishk
# SW7552-81: Support capability to tune either QAM or OFDM signals using
#  the same binary.
# 
# 85   7/12/11 5:13p adtran
# SWDTV-7772: add satellite support
# 
# 84   7/7/11 3:29p mward
# SW7125-1017:  Add and reset accumulating counters as expected by Nexus
#  API.  Compile option provided for previous behavior.
# 
# SATIP_7425_4528/1   6/29/11 3:53p lihong
# SW7425-798: setup build system for SATIP
# 
# 83   6/24/11 6:30a VISHK
# SW7346-165: NEXUS Frontend changes for multi-tuner board are not
#  platform independent Uncomment inclusion of nexus_frontend_31xx.c
# 
# 82   6/23/11 7:22p vishk
# SW7346-165: NEXUS Frontend changes for multi-tuner board are not
#  platform independent
# 
# 81   6/23/11 7:01p vishk
# SW7346-165: NEXUS Frontend changes for multi-tuner board are not
#  platform independent
# 
# 80   6/23/11 5:46p vishk
# SW7346-165: NEXUS Frontend changes for multi-tuner board are not
#  platform independent
# 
# 79   6/9/11 11:27a shyi
# SWDTV-7365: Added Sharp VA1E1CD2406 tuner support to Nexus
# 
# 78   6/8/11 2:13p vishk
# SW7552-35: Add 7552 Nexus frontend support. External Tuner support has
#  been removed as there will be no use case for it. Also, Scan features
#  have been disabled for now.
# 
# 77   6/2/11 9:09p vishk
# SW3461-5: Add THD support to 3461
# 
# 76   6/2/11 10:26a erickson
# SW3461-5: add HAB for all THD chips
# 
# 75   6/2/11 9:39a erickson
# SW3461-5: THD requires HAB
# 
# 74   5/27/11 8:48p jtna
# SW7552-35: merge 7552 frontend work
# 
# SW7552-35/2   5/27/11 8:14p jtna
# SW7552-35: update
# 
# SW7552-35/1   5/26/11 8:22p jtna
# SW7552-35: add 7552
# 
# 73   5/5/11 6:43p vishk
# SW7422-334: Add SPI support on top of I2C for the 3128 interface
# 
# 72   4/26/11 1:21p shyi
# SWDTV-6796: Integrated ADS and THD into NEXUS
# 
# 71   4/25/11 3:59p shyi
# SWDTV-6735: Added Nutune FK1605 tuner support
# 
# 70   4/18/11 4:43p spothana
# SW7425-369 : Disable OOB from 3128 and 3114 when
#  NEXUS_PLATFORM_DOCSIS_OOB_SUPPORT enabled.
# 
# 69   4/15/11 4:42p vishk
# SW7422-413: Nexus support for the 3128 OOB core for 7422 board
# 
# 68   4/15/11 3:45p shyi
# SWDTV-6597: Added Silicon Labs Si2153 tuner support
# 
# 67   3/29/11 2:59p shyi
# SWDTV-6189: Added NEXUS frontend into 35233 build
# 
# 66   3/17/11 1:13p vishk
# SW7231-32: Implement the Nexus code to support the 3461 frontend chip
# 
# 65   3/17/11 10:01a jgarrett
# SW7552-24: Adding 7552
# 
# 64   3/10/11 3:29p shyi
# SWDTV-5866: Added SEMco DTVA50CVH1019 tuner support
# 
# 63   3/8/11 3:50p VISHK
# SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
#  board) plug on 7422 board .  Add support for 3128 for 97422/ 97425
#  platform.
# 
# 62   3/4/11 3:46p vishk
# SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
#  board) plug on 7422 board
# 
# 61   2/25/11 2:48p erickson
# SW7425-107: use #if NEXUS_MAX_3255_ADSCHN test instead of special macro
#
# 60   1/20/11 10:55a xhuang
# SW7358-24: Add support for 7358
#
# 59   1/14/11 2:45p shyi
# SW35125-52: Replaced hard-coded platform name with build time
#  configurable macro name for build systems with no symbolic link
#  support
#
# 58   1/6/11 7:31p shyi
# SW35230-2441: Made channel scanning a platform specific feature
#
# 57   12/9/10 11:02a shyi
# SW35230-2520: Added LG Innotek TDTK-H801F tuner support
#
# 56   12/8/10 9:50a jrubio
# SW7344-9: add G3 support
#
# 55   12/1/10 8:19p shyi
# SW35230-2396: Added Nutune FA2328/31/32/33 tuner support
# SW35230-2457: Moved to ES2 driver for NXP TDA18273 tuner
#
# 54   11/29/10 2:51p shyi
# SW35125-31: Added support for 35125
#
# 53   11/22/10 12:36p mward
# SW7125-729:  Add nexus_amplifier_3412 for NEXUS_FRONTEND_3412.
#
# SW7125-729/1   11/10/10 6:28p mward
# SW7125-729:  Add nexus_amplifier_3412 for NEXUS_FRONTEND_3412.
#
# 52   10/14/10 11:24a shyi
# SW35230-1738: Added NXP TD1636F tuner support to NEXUS layer
# SW35230-1605: Added Sharp VA1E1ED2411 tuner support to NEXUS layer
# SW35230-1469: Added Panasonic ENGS6301D5F tuner support to NEXUS layer
# SW35230-1423: Added Panasonic ET-29 tuner support to NEXUS layer
#
# 51   9/21/10 4:47p shyi
# SW35230-1124: Fixing broken build, made the private interface between
#  NEXUS and TFE built only for TFE based platforms and tuner drivers
#
# 50   9/21/10 1:05p shyi
# SW35230-1124: Added preliminary analog demodulation support
#
# 49   8/26/10 12:00p jgarrett
# SW35230-900: Merging new tuner interfaces to main branch
#
# 48   8/20/10 4:49p erickson
# SW7420-838: add 3412
#
# 47   7/26/10 10:15a mward
# SW7125-513:  Add NEXUS_FRONTEND_7125, default y, for host control of
#  7125 tuner/ds.
#
# SW7125-513/1   7/12/10 4:42p mward
# SW7125-513:  Add NEXUS_FRONTEND_7125, default y, for host control of
#  7125 tuner/ds.
#
# SW35230-900/6   8/24/10 2:54p shyi
# SW35230-900: Removed NULL tuner driver from building and added API for
#  setting AGC script
#
# SW35230-900/5   8/20/10 4:43p shyi
# SW35230-900: Adding NXP TDA18273 tuner
# SW35230-1049: Adding NXP TDA18273 tuner support at NEXUS layer
#
# SW35230-900/4   8/20/10 3:18p shyi
# SW35230-900: Adjusting I2C address to chip address, and providing
#  function proto-types
#
# SW35230-900/3   8/18/10 3:37p shyi
# SW35230-900: Re-arranging build configuration and correcting function
#  header comments
#
# SW35230-900/2   8/18/10 11:24a shyi
# SW35230-900: Adding NULL tuner
#
# SW35230-900/1   8/12/10 8:24p shyi
# SW35230-900: Taking TFE tuner changes for I2C read and firmware
#  download capabilities
#
# 46   7/19/10 11:43a shyi
# SW35230-272: Merging to main branch
#
# SW35230-272/2   6/24/10 7:00p shyi
# SW35230-272: Adding support for ISDB-T
#
# SW35230-272/1   6/7/10 11:52a shyi
# Prepare NEXUS frontend for LG demo and initial bring-up for the
#  reference board
#
# 45   4/1/10 4:26p garetht
# SW7420-672: Clean-up logic. Add define for when 3255 (3380) does not
#  provide downstream functionality
#
# 44   3/24/10 12:21p garetht
# SW7420-672: check-in 93380vms/sms Phase 4.0 Trinity
#
# 43   3/3/10 4:36p mward
# SW7400-2673: Remove restriction on build with both 4500 and 4501/4506,
#  default NEXUS_FRONTEND_4500=y.
#
# 42   2/25/10 6:20p mward
# SW7400-2673: Implement support for BCM4500.
#
# SW7400-2673/2   2/24/10 3:34p mward
# SW7400-2673:  Support for BCM4500.
#
# SW7400-2673/1   2/22/10 1:46p mward
# SW7400-2673:  Support BCM4500.
#
# 40   12/31/09 4:07p VISHK
# SW7550-135: Support 7550 QAM/DVB-C support in Nexus
#
# 39   12/15/09 4:13p jgarrett
# SW7550-48: Defaulting QAM off
#
# 38   12/15/09 3:59p jgarrett
# SW7550-48: Enabling QAM by default, fixing TNR for 7550
#
# 37   11/6/09 5:06p jgarrett
# SW7550-48: Adding 7550 Frontend Support
#
# 36   10/5/09 3:43p haisongw
# SW7400-2531: Add 3255 Frontend GPIO control support
#
# 35   9/11/09 10:53a vishk
# SW7400-1771: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
#
# 34   8/20/09 4:48p erickson
# PR57770: convert FTM extension into more generic customer extension
#
# 33   8/5/09 4:55p jrubio
# PR55232: add 7342/7340 support
#
# 32   8/3/09 9:24a erickson
# PR57239: add NEXUS_FRONTEND_AST define
#
# 31   7/20/09 5:11p erickson
# PR56765: enable separate NEXUS_FRONTEND_FTM_EXTENSION_INC extension
#
# 30   6/19/09 11:55a vishk
# PR 56159: bcm31xxap.c and bcm31xxap.h to be moved to ads pi folder from
#  nexus
#
# 29   4/8/09 4:31p vsilyaev
# PR 54016: Added support for precompiled header
#
# 28   3/11/09 7:23p vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
#
# 27   3/11/09 6:22p vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
#
# 26   3/10/09 1:46p vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
#
# 25   3/5/09 3:18p katrep
# PR52599: More support for 7205
#
# 24   3/5/09 7:35a vishk
# PR46431: Add support for BCM3117, BCM3114, and BCM3112
#
# 23   3/4/09 8:37p vishk
# PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
#
# 22   3/3/09 4:03p vishk
# PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
#
# 20   2/9/09 5:03p jgarrett
# PR 50837: Merge to main branch
#
# 19   2/2/09 5:29p anilmm
# PR44830:  Add aob/aus chip variables for 3255
#
# 18   1/29/09 7:28p vishk
# PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
# PR 46436: AOB PI: Add support for BCM3117 and BCM3114
#
# PR50837/1   1/29/09 10:37a jgarrett
# PR 50837: Adding 3440 tuner support
#
# 17   1/23/09 3:31p jgarrett
# PR 51457: Adding frontend extension hooks for tuners and
#  frontends/demods
#
# 16   1/16/09 7:45p vishk
# PR46436: AOB PI: Add support for BCM3117 and BCM3114
#
# 15   12/31/08 1:27p erickson
# PR50787: Allow NEXUS_POWER_MANAGEMENT to be a master switch
#
# 14   10/24/08 12:28p jgarrett
# PR 48016: Adding RSP for 3549
#
# 13   9/23/08 9:16a jgarrett
# PR 46513: Merging 97459 support to mainline
#
# 12   7/22/08 10:42a jgarrett
# PR 38944: Adding rsp for 3556 frontend
#
# PR44830/1   7/22/08 6:45p anilmm
# PR44830: Adding 3255 frontend
#
# 11   5/6/08 11:34a jgarrett
# PR 41689: Adding 3548 frontend
#
# 10   5/2/08 9:57a erickson
# PR39454: 3556 impl
#
# 9   5/1/08 12:29p jgarrett
# PR 41690: Adding 3556 frontend
#
# 8   5/1/08 11:27a jgarrett
# PR 41689: PR 41690: Adding dtt76800 and dtt75409 tuners
#
# 7   4/30/08 7:03a jrubio
# PR42336: add 7325/7335 tuner
#
# 6   4/29/08 5:58p jrubio
# PR42336: add 7325 code
#
# 5   4/21/08 1:08p jgarrett
# PR41793: PR41793: Frontend: Add Thomson tuner Dtt7045 driver to support
#  8M tuner
#
# PR41793/1   4/21/08 3:03p pinglong
# PR41793: Frontend: Add Thomson tuner Dtt7045 driver to support 8M tuner
#
# 4   3/13/08 7:41p jgarrett
# PR 39985: Adding AST support for 450x
#
# 3   3/4/08 9:47a jgarrett
# PR 39984: Adding 3510 support
#
# 2   2/8/08 6:36p jgarrett
# PR 39408: Adding daughtercard support
#
# 1   1/18/08 2:14p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/4   1/10/08 2:32p jgarrett
# PR 38653: Adding OFDM
#
# Nexus_Devel/3   11/20/07 6:51p jgarrett
# PR 37130: Unifying 3563 frontend and platform
#
# Nexus_Devel/2   10/10/07 11:28a jgarrett
# PR 35551: Adding 3520/3418/dtt760x supprot
#
# Nexus_Devel/1   10/5/07 5:51p jgarrett
# PR 35551: Adding initial version
#
############################################################

#
# Conditional Options for this Makefile
#
# NEXUS_FRONTEND_4500 - Enable support for the BCM4500 Satellite demodulator (cannot be used withother Sat frontends)
# NEXUS_FRONTEND_4501 - Enable support for the BCM4501 Satellite tuner/demodulator
# NEXUS_FRONTEND_4501_VER - Define 4501 chip revision (default=C0)
# NEXUS_FRONTEND_4506 - Enable support for the BCM4506 Satellite tuner/demodulator
# NEXUS_FRONTEND_4506_VER - Define 4506 chip revision (default=A0)
# NEXUS_FRONTEND_3255 - Enable support for the BCM3255 Cable frontend
# NEXUS_FRONTEND_3510 - Enable support for the BCM3510 QAM/VSB demodulator
# NEXUS_FRONTEND_3520 - Enable support for the BCM3520 QAM/VSB demodulator
# NEXUS_FRONTEND_3418 - Enable support for the BCM3418 tuner
# NEXUS_FRONTEND_3420 - Enable support for the BCM3420 tuner
# NEXUS_FRONTEND_3440 - Enable support for the BCM3420 satellite tuner
# NEXUS_FRONTEND_3431 - Enable support for the BCM3431 amplifier
# NEXUS_FRONTEND_DTT760X - Enable support for the Thomson DTT760x tuner
# NEXUS_FRONTEND_DTT7045 - Enable support for the Thomson DTT7045 tuner
# NEXUS_FRONTEND_DTT75409 - Enable support for the Thomson DTT75409 tuner
# NEXUS_FRONTEND_DTT76800 - Enable support for the Thomson DTT76800 tuner
# NEXUS_FRONTEND_3117 - Enable support for the BCM3117 ADS/AOB Demod and tuner and AUS modulator.
# NEXUS_FRONTEND_7550_OFDM_SUPPORT - Enable OFDM support on the BCM7550
# NEXUS_FRONTEND_7550_QAM_SUPPORT - Enable QAM support on the BCM7550

######################
# Define this module #
######################
NEXUS_MODULES += FRONTEND

##################
# Module Options #
##################
NEXUS_FRONTEND_OPTIONS = NEXUS_FRONTEND_3418 \
                         NEXUS_FRONTEND_3420 \
                         NEXUS_FRONTEND_3440 \
                         NEXUS_FRONTEND_3431 \
                         NEXUS_FRONTEND_3510 \
                         NEXUS_FRONTEND_3520 \
                         NEXUS_FRONTEND_4500 \
                         NEXUS_FRONTEND_4501 \
                         NEXUS_FRONTEND_4501_VER \
                         NEXUS_FRONTEND_4506 \
                         NEXUS_FRONTEND_4506_VER \
                         NEXUS_FRONTEND_4528 \
                         NEXUS_FRONTEND_4528_VER \
                         NEXUS_FRONTEND_45216 \
                         NEXUS_FRONTEND_DTT760X \
                         NEXUS_FRONTEND_DTT7045 \
                         NEXUS_FRONTEND_DTT75409 \
                         NEXUS_FRONTEND_DTT76800 \
                         NEXUS_FRONTEND_3461 \
                         NEXUS_FRONTEND_3462 \
                         NEXUS_FRONTEND_3255 \
                         NEXUS_FRONTEND_3123 \
                         NEXUS_FRONTEND_3124 \
                         NEXUS_FRONTEND_3128 \
                         NEXUS_FRONTEND_3117 \
                         NEXUS_FRONTEND_3114 \
                         NEXUS_FRONTEND_3112 \
                         NEXUS_FRONTEND_3109

############################
# Standard Module Includes #
############################
NEXUS_FRONTEND_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/frontend/common/include
NEXUS_FRONTEND_PRIVATE_INCLUDES := $(NEXUS_TOP)/modules/frontend/common/src


NEXUS_FRONTEND_PRECOMPILED_HEADER = $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_module.h

##################
# Module Sources #
##################
NEXUS_FRONTEND_SOURCES := $(NEXUS_TOP)/modules/frontend/common/src/nexus_amplifier.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_analog.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_init.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_ofdm.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_oob.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_qam.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_satellite.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_upstream.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_vsb.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_ifd.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_tuner.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_card.c \
                          $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_card_priv.c

#######################
# Module Dependencies #
#######################
NEXUS_FRONTEND_DEPENDENCIES := BASE CORE I2C GPIO SPI TRANSPORT

##################
# Magnum Modules #
##################
NEXUS_FRONTEND_MAGNUM_MODULES =

###############
# Thunk Layer #
###############
NEXUS_FRONTEND_PRIVATE_INCLUDES += $(NEXUS_SYNCTHUNK_DIR)
NEXUS_FRONTEND_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_frontend_thunks.c

###########
# Options #
###########

#
# TNRs
#
ifdef NEXUS_FRONTEND_3418
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3418/src/nexus_tuner_3418.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3418/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3418
NEXUS_FRONTEND_TNR_CHIPS += 3418
BTNR_3418_SUPPORT := y
endif

ifdef NEXUS_FRONTEND_3420
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3420/src/nexus_tuner_3420.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3420/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3420
NEXUS_FRONTEND_TNR_CHIPS += 3420
BTNR_3420_SUPPORT := y
endif

ifdef NEXUS_FRONTEND_3440
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3440/src/nexus_tuner_3440.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3440/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3440
endif

ifdef NEXUS_FRONTEND_DTT760X
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/dtt760x/src/nexus_tuner_dtt760x.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/dtt760x/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_DTT760X
NEXUS_FRONTEND_TNR_CHIPS += DTT760X
BTNR_DTT760X_SUPPORT := y
endif

ifdef NEXUS_FRONTEND_DTT7045
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/dtt7045/src/nexus_tuner_dtt7045.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/dtt7045/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_DTT7045
NEXUS_FRONTEND_TNR_CHIPS += DTT7045
BTNR_DTT7045_SUPPORT := y
endif

ifdef NEXUS_FRONTEND_DTT75409
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/dtt75409/src/nexus_tuner_dtt75409.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/dtt75409/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_DTT75409
NEXUS_FRONTEND_TNR_CHIPS += DTT75409
BTNR_DTT75409_SUPPORT := y
endif

ifdef NEXUS_FRONTEND_DTT76800
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/dtt76800/src/nexus_tuner_dtt76800.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/dtt76800/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_DTT76800
NEXUS_FRONTEND_TNR_CHIPS += DTT76800
BTNR_DTT76800_SUPPORT := y
endif

ifeq ($(findstring $(BCHP_CHIP), 7550), $(BCHP_CHIP))
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/$(BCHP_CHIP)/src/nexus_tuner_7550.c
NEXUS_FRONTEND_TNR_CHIPS += 7550
endif

ifeq ($(NEXUS_FRONTEND_7125),y)
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/7125/src/nexus_tuner_7125.c
NEXUS_FRONTEND_TNR_CHIPS += 7125
endif

#
# LNAs
#
ifdef NEXUS_FRONTEND_3431
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3431/src/nexus_amplifier_3431.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3431/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3431
NEXUS_FRONTEND_LNA_CHIPS += 3431
BLNA_3431_SUPPORT := y
endif

ifdef NEXUS_FRONTEND_3412
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3412/src/nexus_amplifier_3412.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3412/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3412
NEXUS_FRONTEND_LNA_CHIPS += 3412
BLNA_3412_SUPPORT := y
endif

#
# SDS
#
ifdef NEXUS_FRONTEND_4500
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/4500/src/nexus_frontend_4500.c
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/4500/src/nexus_frontend_sds.c
NEXUS_FRONTEND_SOURCES += $(MAGNUM)/portinginterface/sds/4500/bcm4500ap.c
NEXUS_FRONTEND_PRIVATE_INCLUDES += $(MAGNUM)/portinginterface/sds/4500

NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/4500/include/
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/sds/bsds.inc
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_4500
endif

#
# ASTs
#
ifdef NEXUS_FRONTEND_4501
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/4501/src/nexus_frontend_4501.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/4501/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_4501
BAST_CHIP += 4501
ifdef NEXUS_FRONTEND_4501_VER
BAST_4501_VER := $(NEXUS_FRONTEND_4501_VER)
else
BAST_4501_VER := C0
endif
endif

ifdef NEXUS_FRONTEND_4506
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/4506/src/nexus_frontend_4506.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/4506/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_4506
BAST_CHIP += 4506
ifdef NEXUS_FRONTEND_4506_VER
BAST_4506_VER := $(NEXUS_FRONTEND_4506_VER)
else
BAST_4506_VER := A0
endif
endif

ifdef NEXUS_FRONTEND_4528
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/4528/src/nexus_frontend_4528.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/4528/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_4528
BHAB_4528_SUPPORT:=y
NEXUS_CFLAGS += -DBAST_USE_HAB_PI=1
BAST_CHIP += 4528
BMXT_4528_SUPPORT:=y
NEXUS_FRONTEND_DEFINES += NEXUS_HAS_MXT=1
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/hab/bhab.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/mxt/bmxt.inc
ifdef NEXUS_FRONTEND_4528_VER
BAST_4528_VER := $(NEXUS_FRONTEND_4528_VER)
else
BAST_4528_VER := A0
endif
endif

ifdef NEXUS_FRONTEND_45216
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/45216/src/nexus_frontend_45216.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/45216/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_45216
# The line below is not necessary for bsat.inc but is needed below in this file
BSAT_CHIP += 45216
BHAB_45216_SUPPORT:=y
BHAB_CHIP += 45216
BWFE_CHIP += 45216
BDSQ_CHIP += 45216
BMXT_45216_SUPPORT:=y
#BMXT_45216_SUPPORT:=y
NEXUS_FRONTEND_DEFINES += NEXUS_HAS_MXT=1
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/hab_v1441/bhab.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/mxt_v1441/bmxt.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/wfe/bwfe.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/dsq/bdsq.inc
ifeq ($(NEXUS_FRONTEND_FSK_SUPPORT),y)
BFSK_CHIP += 45216
NEXUS_FRONTEND_DEFINES += NEXUS_HAS_FSK=1
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/fsk/bfsk.inc
endif
endif

ifeq ($(findstring $(BCHP_CHIP), 7325 7335 7342 7340), $(BCHP_CHIP))
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/7325/src/nexus_frontend_73xx.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/7325/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_73XX
# The line below is not necessary for bast.inc but is needed below in this file
BAST_CHIP += $(BCHP_CHIP)
endif

ifeq ($(findstring $(BCHP_CHIP), 7346 7344 7358 7360), $(BCHP_CHIP))
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/7346/src/nexus_frontend_7346.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/7346/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_7346
# The line below is not necessary for bast.inc but is needed below in this file
BAST_CHIP += $(BCHP_CHIP)
endif
#
# VSBs
#
ifdef NEXUS_FRONTEND_3510
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3510/src/nexus_frontend_3510.c \
        $(MAGNUM)/portinginterface/vsb/src/3510/bcm3510ap.c
NEXUS_FRONTEND_PRIVATE_INCLUDES += $(MAGNUM)/portinginterface/vsb/src/3510 \
        $(MAGNUM)/portinginterface/vsb/src/common 
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3510/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3510
BVSB_CHIP := 3510
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/vsb/bvsb.inc
#ifeq ($(BCHP_CHIP),7405)
#NEXUS_FRONTEND_DEFINES += BCM_BOARD=97405
#endif
ifdef NEXUS_FRONTEND_3520
$(error can not support 3510 and 3520 simultaneously)
endif
endif

ifdef NEXUS_FRONTEND_3520
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3520/src/nexus_frontend_3520.c \
        $(MAGNUM)/portinginterface/vsb/src/3520/bcm3520ap_a1.c \
        $(MAGNUM)/portinginterface/vsb/src/3520/bcm3520ap_b0.c
NEXUS_FRONTEND_PRIVATE_INCLUDES += $(MAGNUM)/portinginterface/vsb/src/3520 \
        $(MAGNUM)/portinginterface/vsb/src/common 
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3520/include/
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3520
BVSB_CHIP := 3520
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/vsb/bvsb.inc
ifdef NEXUS_FRONTEND_3510
$(error can not support 3510 and 3520 simultaneously)
endif
endif

ifeq ($(findstring $(BCHP_CHIP), 7550), $(BCHP_CHIP))
NEXUS_FRONTEND_7550_OFDM_SUPPORT?=y
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/$(BCHP_CHIP)/src/nexus_frontend_7550_ofdm.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/$(BCHP_CHIP)/include
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_7550
ifeq ($(NEXUS_FRONTEND_7550_OFDM_SUPPORT),y)
BTNR_7550_SUPPORT ?= y
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_7550_OFDM_SUPPORT
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/thd/bthd.inc
endif
endif

ifeq ($(findstring $(BCHP_CHIP), 7552), $(BCHP_CHIP))
ifeq ($(NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD),y)
NEXUS_FRONTEND_DEFINES += NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
endif
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/$(BCHP_CHIP)/src/nexus_frontend_scan.c
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/$(BCHP_CHIP)/src/nexus_frontend_7552.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/$(BCHP_CHIP)/include
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_7552
NEXUS_FRONTEND_TNR_CHIPS += 7552
BTNR_7552_SUPPORT := y
BADS_7552_SUPPORT := y
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/thd/bthd.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ads/bads.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/tnr/btnr.inc
ifeq ($(BCHP_VER),A0)
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_7552_A0
endif
ifeq ($(BCHP_VER),B0)
BAOB_CHIP := 7552
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/aob/baob.inc
endif
endif

#
# QAM
#
ifdef NEXUS_FRONTEND_3255
ifeq ($(findstring $(BCHP_CHIP), 7125), $(BCHP_CHIP))
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/7125/src/nexus_frontend_3255.c
else
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3255/src/nexus_frontend_3255.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3255/include/
endif
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3255
ifeq ($(NEXUS_FRONTEND_ACCUMULATE_STATISTICS),n)
#use NEXUS_FRONTEND_ACCUMULATE_STATISTICS=n to keep existing behavior for 3255
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_ACCUMULATE_STATISTICS=0
else
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_ACCUMULATE_STATISTICS=1
endif
BTNR_3255IB_SUPPORT := y
BTNR_3255OB_SUPPORT := y
BADS_CHIP := 3255
BAOB_CHIP := 3255
BAUS_CHIP := 3255
ifeq ($(filter $(NEXUS_PLATFORM), 93380sms 93380vms), $(NEXUS_PLATFORM))
# 97459,97456 do not set NEXUS_PLATFORM
else
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ads/bads.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/tnr/btnr.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/hab/bhab.inc
endif
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/aob/baob.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/aus/baus.inc

NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/rpc/brpc.inc

BMXT_3383_SUPPORT:=y
NEXUS_FRONTEND_DEFINES += NEXUS_HAS_MXT=1
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/rpc/brpc.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/mxt/bmxt.inc

ifeq ($(BCHP_CHIP),7405)
NEXUS_FRONTEND_DEFINES += BCM_BOARD=97459
endif
endif # NEXUS_FRONTEND_3255

ifeq ($(findstring $(BCHP_CHIP), 7550), $(BCHP_CHIP))
NEXUS_FRONTEND_7550_QAM_SUPPORT ?= y
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/$(BCHP_CHIP)/src/nexus_frontend_7550_qam.c
ifeq ($(NEXUS_FRONTEND_7550_QAM_SUPPORT),y)
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_7550_QAM_SUPPORT
BADS_7550_SUPPORT := y
BTNR_7550_SUPPORT ?= y
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ads/bads.inc
endif
endif

ifeq ($(NEXUS_FRONTEND_7125),y)
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/7125/src/nexus_frontend_7125_qam.c
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/7125/include
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_7125
BADS_7125_SUPPORT := y
BTNR_7125_SUPPORT ?= y
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ads/bads.inc
endif

#
# These lines should be last in the file
#
# Based on earlier settings, include additional magnum modules.
ifneq ($(NEXUS_FRONTEND_LNA_CHIPS),)
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/lna/blna.inc
NEXUS_FRONTEND_DEFINES += NEXUS_AMPLIFIER_SUPPORT
endif

ifneq ($(NEXUS_FRONTEND_TNR_CHIPS),)
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/tnr/btnr.inc
NEXUS_FRONTEND_DEFINES += NEXUS_TUNER_SUPPORT
endif

ifneq ($(NEXUS_FRONTEND_IFD_CHIPS),)
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ifd/bifd.inc
NEXUS_FRONTEND_DEFINES += NEXUS_IFD_SUPPORT
endif

ifneq ($(BAST_CHIP),)
# There are multiple AST chips.  Include the magnum support here.
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ast/bast.inc
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_ast.c
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_AST
endif

ifneq ($(BSAT_CHIP),)
ifeq ($(NEXUS_FRONTEND_BYPASS_LEAP),y)
BYPASS_LEAP:=1
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_BYPASS_LEAP
endif
# There are multiple SAT chips.  Include the magnum support here.
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/sat/bsat.inc
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/common/src/nexus_frontend_sat.c
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_SAT
endif

# Allow NEXUS_POWER_MANAGEMENT to be a master switch
ifeq ($(NEXUS_POWER_MANAGEMENT),n)
BVSB_POWER_MANAGEMENT := n
endif

#
# Combo frontend chips ADS, AOb, AUS, THD, T2
#

ifdef NEXUS_FRONTEND_3472
BTNR_3472_SUPPORT := y
BODS_3472_SUPPORT := y
BHAB_3472_SUPPORT := y
ifeq ($(NEXUS_FRONTEND_3472_VER),)
BCHP_3472_VER := A0
else
BCHP_3472_VER := $(NEXUS_FRONTEND_3472_VER)
endif
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3472
NEXUS_FRONTEND_DEFINES += NEXUS_HAS_MXT=1
BMXT_3472_SUPPORT:=y
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3461/include/
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ods/bods.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/tnr/btnr.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/hab/bhab.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/mxt/bmxt.inc
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3472/src/nexus_frontend_3472.c
ifeq ($(NEXUS_USE_FRONTEND_DAUGHTER_CARD),y)
NEXUS_FRONTEND_DEFINES += NEXUS_USE_FRONTEND_DAUGHTER_CARD
endif
endif

ifdef NEXUS_FRONTEND_3462
BTNR_3462_SUPPORT := y
BADS_3462_SUPPORT := y
BODS_3462_SUPPORT := y
BHAB_3462_SUPPORT := y
ifeq ($(NEXUS_FRONTEND_3462_VER),)
BCHP_3462_VER := A0
else
BCHP_3462_VER := $(NEXUS_FRONTEND_3462_VER)
endif
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3461/include/
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ads/bads.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ods/bods.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/tnr/btnr.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/hab/bhab.inc
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3462/src/nexus_frontend_3462.c
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3462
endif

ifdef NEXUS_FRONTEND_3461
BTNR_3461_SUPPORT := y
BADS_3461_SUPPORT := y
BTC2_3461_SUPPORT := y
BHAB_3461_SUPPORT := y
BTHD_CHIP := 3461
ifeq ($(NEXUS_FRONTEND_3461_VER),)
BCHP_3461_VER := B0
else
BCHP_3461_VER := $(NEXUS_FRONTEND_3461_VER)
endif
ifeq ($(NEXUS_FRONTEND_3461_VER), A0)
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3461_A0
else
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3461_BX
endif
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3461/include/
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ads/bads.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/tnr/btnr.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/tc2/btc2.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/thd/bthd.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/hab/bhab.inc
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3461/src/nexus_frontend_3461.c
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3461
endif

ifdef NEXUS_FRONTEND_3128
BTNR_3128IB_SUPPORT := y
BADS_3128_SUPPORT := y
BHAB_3128_SUPPORT := y
NEXUS_FRONTEND_DEFINES += NEXUS_HAS_MXT=1
ifeq ($(NEXUS_FRONTEND_312x_OOB), y)
BAOB_CHIP := 3128
BTNR_3128OB_SUPPORT := y
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_312x_OOB
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/aob/baob.inc
endif
ifeq ($(NEXUS_FRONTEND_312x_VER),)
BCHP_312X_VER := B0
else
BCHP_312X_VER := $(NEXUS_FRONTEND_312x_VER)
endif
ifeq ($(NEXUS_FRONTEND_312x_VER), A0)
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_312x_A0
endif
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3128
endif

ifneq ($(BCHP_312X_VER),)

# 7344 and 7346 use 3128, but in legacy mode. MXT not required
ifneq ($(findstring $(BCHP_CHIP), 7344 7346), $(BCHP_CHIP))
BMXT_3128_SUPPORT:=y
NEXUS_FRONTEND_DEFINES += NEXUS_HAS_MXT=1
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/mxt/bmxt.inc
endif

ifeq ($(NEXUS_USE_FRONTEND_DAUGHTER_CARD),y)
NEXUS_FRONTEND_DEFINES += NEXUS_USE_FRONTEND_DAUGHTER_CARD
endif
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3128/include/
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ads/bads.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/tnr/btnr.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/hab/bhab.inc
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3128/src/nexus_frontend_3128.c
endif

ifdef NEXUS_FRONTEND_3117
BTNR_3117IB_SUPPORT := y
BTNR_3117OB_SUPPORT := y
BADS_3117_SUPPORT := y
BAUS_CHIP := 3117
BHAB_3117_SUPPORT := y
31XX_CHIP := 3117
ifeq ($(BAOB_CHIP),)
BAOB_CHIP := 3117
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_31xx_OOB
endif
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3117
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/aob/baob.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/aus/baus.inc
endif

ifdef NEXUS_FRONTEND_3114
BTNR_3114IB_SUPPORT := y
BTNR_3114OB_SUPPORT := y
BADS_3114_SUPPORT := y
BHAB_3114_SUPPORT := y
31XX_CHIP := 3117
ifeq ($(BAOB_CHIP),)
BAOB_CHIP := 3114
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_31xx_OOB
endif
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3114
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/aob/baob.inc
endif

ifdef NEXUS_FRONTEND_3112
BTNR_3112IB_SUPPORT := y
BADS_3112_SUPPORT := y
BHAB_3112_SUPPORT := y
31XX_CHIP := 3112
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3112
endif

ifdef NEXUS_FRONTEND_3109
BTNR_3109IB_SUPPORT := y
BADS_3109_SUPPORT := y
BHAB_3109_SUPPORT := y
31XX_CHIP := 3109
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_3109
endif

ifneq ($(31XX_CHIP),)
ifeq ($(NEXUS_FRONTEND_31xx_VER),)
    BCHP_31XX_VER := B0
else
    BCHP_31XX_VER := $(NEXUS_FRONTEND_31xx_VER)
endif
ifeq ($(NEXUS_FRONTEND_31xx_VER), B0)
    NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_31xx_B0
endif
NEXUS_FRONTEND_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/frontend/3117/include/
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/ads/bads.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/portinginterface/tnr/btnr.inc
NEXUS_FRONTEND_MAGNUM_MODULES += $(MAGNUM)/basemodules/hab/bhab.inc
NEXUS_FRONTEND_SOURCES += $(NEXUS_TOP)/modules/frontend/3117/src/nexus_frontend_31xx.c
endif

ifeq ($(NEXUS_PLATFORM_VER),HR44)
NEXUS_CFLAGS += -DBAST_USE_SNRE
endif

#####################
# Module Extensions #
#####################
ifneq ($(NEXUS_FRONTEND_EXTENSION_INC),)
include $(NEXUS_FRONTEND_EXTENSION_INC)
NEXUS_FRONTEND_DEFINES += NEXUS_FRONTEND_EXTENSION
endif

ifneq ($(NEXUS_FRONTEND_CUSTOMER_EXTENSION_INC),)
include $(NEXUS_FRONTEND_CUSTOMER_EXTENSION_INC)
endif

