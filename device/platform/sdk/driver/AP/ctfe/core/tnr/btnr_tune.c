
/***************************************************************************
 *     (c)2005-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: btnr_tune.c $
 * $brcm_Revision: 110 $
 * $brcm_Date: 11/15/12 11:40a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/tnr/btnr_tune.c $
 * 
 * 110   11/15/12 11:40a farshidf
 * SW7552-372: required to compile 7552, remove warning, concern only 7552
 * 
 * 109   10/18/12 6:40p farshidf
 * SW3461-287: create 5.3.1.1
 * 
 * Fw_Integration_Devel/74   10/18/12 6:38p farshidf
 * SW3461-287: create 5.3.1.1
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/69   10/18/12 6:36p farshidf
 * SW3461-208: Start AGCMonitir after RFinput mode has been set
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/68   10/16/12 4:26p shchang
 * SW7552-368: fix pop errors by disabling AGC shift-gear for cable
 *  application
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/67   10/16/12 1:32p mbsingh
 * SW3461-260: Fix external gain assignment
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/66   10/16/12 9:48a bsandeep
 * SW3472-28: correct the AON register handle for 7552
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/65   10/15/12 1:20p mbsingh
 * SW3461-260: Implement the new calculations for the RFoutgains (ie. Gain
 *  API)
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/64   10/13/12 4:59p mbsingh
 * SW3461-160: Moving variables to correct location and #ifndef out the
 *  old gain calculations to save code space
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/63   10/13/12 4:44p mbsingh
 * SW3461-160: Some more cleanup and debug message formatting. This will
 *  help in debugging issues
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/62   10/13/12 4:20p mbsingh
 * SW3461-160:  - Sum up all the gains in based on given conditions -
 *  Assign the final gain values - Clean up debug prints and variable
 *  groupings
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/61   10/13/12 3:48p mbsingh
 * SW3461-160: Adding all the remaining gain calculations (LNA_Bias,
 *  RFVGA_Rdeg, RFVGA_Bias, TRKFIl_Bias, MIXER_Bias, FGA_HiGain, LNA_Tilt,
 *  RFVGA_Tilt)
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/60   10/13/12 2:42p mbsingh
 * SW3461-160: Adding gain equations for LNA, RF VGA and Mixer FGA
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/59   10/13/12 9:35a mbsingh
 * SW3461-260:  Tuner gain API clean up - Remove redundant 3412 specific
 *  LNA tilt, boost and superboost code. This is replaced by gain API
 *  anyway - Clean up and add more comments - Remove unused old RFVGA
 *  calculation
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/58   10/12/12 6:01p mpovich
 * SW3472-28: Fix register R/W macro handles for non-channelized registers
 *  such as TM GPIO and SYS_PLL.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/57   10/10/12 4:22p shchang
 * SW3461-278: fix seeting RF input mode disables daisy setting for VHF
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/56   10/10/12 3:13p farshidf
 * SW3461-260: power up clean up
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/55   10/10/12 2:47p farshidf
 * SW3461-120: fix AGC forcing
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/54   10/10/12 2:06p shchang
 * SW7552-368: fix ADS single post RS error pop caused by AGC shift gear
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/53   10/10/12 1:30p mbsingh
 * SW3461-281: Remove LNA gain when tuner is in external LNA mode
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/52   10/10/12 12:14p farshidf
 * SW3461-120: remove the force AGC settings at boot time
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/51   9/27/12 9:45a shchang
 * SW3461-208: completely separate cable/terrestrail settings
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/50   9/26/12 2:52p mbsingh
 * SW3461-275: Add 756 +/- 5Mhz to smart tune frequency range to fis
 *  754Mhz sensitivity issue (Spur causes 754Mhz channel to fail nordig
 *  sensitivity spec)
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/49   9/21/12 11:10p shchang
 * SW3461-208: fix slow AGC speed for initTune
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/48   9/12/12 1:21p farshidf
 * SW7563-14: remove LNA access
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/47   9/10/12 3:25p mbsingh
 * SW3461-267: Gain API: Set appropriate gain bypassed flag from tuner
 *  driver
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/46   9/7/12 12:01p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 *  structure
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/45   9/7/12 11:11a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 *  structure
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/44   9/4/12 6:37p shchang
 * SW3461-208: 1. change input clock of tuner PLL to CML 2. add low-power
 *  settings to 3461B0/B1/B2
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/43   8/29/12 11:22a shchang
 * SW3461-208: 1. change tuner/SDADC common-mode selection to LPF 2.
 *  change RFAGC clock from 1350MHz to 1080MHz 3. improve ACI performance
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/42   8/21/12 5:50p mpovich
 * SW3472-6: Fix merge from 3472 dev branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/41   8/21/12 5:36p mpovich
 * SW3472-6: Fix merge from 3472 dev branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/40   8/21/12 5:14p mpovich
 * SW3472-6: Merge to 5_0 Dev branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/39   8/17/12 3:53p farshidf
 * SW3461-260: Sperate the Ter and cable tuner code base
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/38   8/15/12 6:47p farshidf
 * SW7563-14: remove external LNA calls
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/SW3472-6/7   8/1/12 11:27p mpovich
 * SW3472-6: Rebase with DEV branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/37   7/31/12 5:14p mbsingh
 * SW3461-195: Compensate for Total Gain and bypassable gain in Signal
 *  strength calculation
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/SW3472-6/6   7/31/12 10:24a mpovich
 * SW3472-6: Rebase with DEV branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/36   7/28/12 10:28a shchang
 * SW3461-208: disable smarttune for 3472
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/35   7/28/12 9:42a shchang
 * SW3461-208: 1). power/input mode de-coupling 2). SDADC calibration for
 *  3462/72 3). low-power mode 4). shift gear on LNA AGC 5). bypass mode
 *  threshold adjustment during tune
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/34   7/20/12 6:16p farshidf
 * SW7552-302: add Tuner delay according to Dave
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/SW3472-6/4   7/19/12 3:18p farshidf
 * SW3472-6: fix the tuner power up
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/33   7/18/12 4:47p farshidf
 * SW7552-302: BTNR-Tune changes to fix long lock speed in bypass mode
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/SW3472-6/3   7/18/12 10:08a mpovich
 * SW3472-6: Rebase with latest changes on DEV branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/32   7/16/12 6:53p shchang
 * SW3461-208: bypass mode enable for ISDB-T
 *
 98   7/16/12 6:02p farshidf
 * SW7552-302: fix the QAM signal strength
 * 
 * Fw_Integration_Devel/63   7/16/12 6:00p farshidf
 * SW7552-302: fix the QAM signal strength
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/31   7/16/12 5:59p farshidf
 * SW7552-302: fix the QAM signal strength
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/SW3472-6/2   7/16/12 2:29p mpovich
 * SW3472-6: Rebase with DEV branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/30   7/13/12 5:35p farshidf
 * SW7552-302: update power measurment
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/29   7/12/12 4:26p farshidf
 * SW7552-302: improve the tuner time for 7552
 * 
 * 95   7/3/12 4:13p farshidf
 * SW3461-120: remove  warning/compile issue
 * 
 * Fw_Integration_Devel/60   7/3/12 4:12p farshidf
 * SW3461-120: remove  warning/compile issue
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/27   7/3/12 3:57p farshidf
 * SW3461-120: fix the bypass isssue on 7552
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/26   7/3/12 3:24p mbsingh
 * SW3461-195: Implemting tuner driver function to support gain API
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/25   7/2/12 5:59p farshidf
 * SW3461-120: remove warning
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/24   7/2/12 4:06p shchang
 * SW3461-208: correct typo
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/23   7/2/12 3:29p shchang
 * SW3461-208: reduce FGLNA bypass mode threshold
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/22   6/28/12 4:59p mbsingh
 * SW7552-278: Fixing Signal Strength value incorrecteness during tuner
 *  gear shift
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/21   6/28/12 2:50p mbsingh
 * SW7552-278: Fix QAM Signal strength ( RF VGA degeneration compensation)
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/20   6/14/12 2:56p mbsingh
 * SW3461-195: Adding driver functions to support gain api - Adding
 *  function to calculate RFOutGains
 * 
 * 89   6/13/12 10:57a farshidf
 * SW3461-1: merge
 * 
 * Fw_Integration_Devel/54   6/13/12 10:56a farshidf
 * SW3461-1: merge
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/19   6/13/12 10:54a farshidf
 * SW3461-1: remove warning
 * 
 * 88   6/13/12 10:24a farshidf
 * SW3462-1: merge to integ
 * 
 * Fw_Integration_Devel/53   6/13/12 10:23a farshidf
 * SW3462-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/18   6/13/12 9:49a shchang
 * SW3462-208: enable dithering for 7552 Cable application
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/17   6/11/12 1:42p shchang
 * SW3462-31: fix 99MHz unlock issue due to 100MHz spur
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/16   6/8/12 2:52p shchang
 * SW3462-31: fix AGC settle speed for bypass mode
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/14   6/4/12 9:23a shchang
 * SW3462-31: fix performance issue below 130MHz
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/13   5/23/12 5:08p farshidf
 * SW3461-1: update the tuner status
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/12   5/23/12 5:06p farshidf
 * SW3461-1: move the Tuner AGC monitor out of tuner status
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/11   5/22/12 7:06p shchang
 * SW3461-208: add control for external FGLNA in branch 5 development
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/10   5/17/12 3:34p shchang
 * SW3462-27: fix ADS Degraded performance with clean (no AWGN) nominal RF
 *  level QAM signals
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/9   5/16/12 3:57p mbsingh
 * SW3462-26: Removing smart tune for "Cable" mode to get DS working for
 *  smart tune frequencies
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/8   5/15/12 3:51p shchang
 * SW3461-208: optimize RFAGC TOP for cable application
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/7   5/14/12 2:22p shchang
 * SW3461-208: fix 507MHz unlock issue due to MoCA trap/tracking filter
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/6   5/11/12 10:59a farshidf
 * SW3461-1: clean up
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/4   5/7/12 6:31p farshidf
 * SW3472-2: Make the tuner multi channel
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/3   5/7/12 11:55a shchang
 * SW3461-120: 1. optimize 6-bit ADC power/performance. 2. modify DPM LO
 *  table.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/2   5/2/12 3:09p shchang
 * SW3461-120: add dithering on/off
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   4/27/12 6:09p farshidf
 * SW3462-19: merge to branch 5.0
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/SW3462-6/3   4/27/12 5:29p farshidf
 * SW3462-19: merge to branch 5.0
 * 
 * Fw_Integration_Devel/50   4/17/12 5:28p farshidf
 * SW3461-186: merge to integ
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/24   4/17/12 7:14a shchang
 * SW-3461-184: tuner gain smooth transition
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/23   4/11/12 9:57p shchang
 * SW3461-120: fix tune stuck issue
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/22   3/29/12 5:15p shchang
 * SW3461-120: fix sync time-varying test failure
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/21   3/28/12 11:09p farshidf
 * SW3461-120: merge to dev
 * 
 * SW3461-173/3   3/28/12 11:07p farshidf
 * SW3461-120: merge to branch
 * 
 * 80   3/28/12 11:06p farshidf
 * SW3461-120: merge to main
 * 
 * SW3461-173/2   3/28/12 7:16p shchang
 * SW3461-120: fix N+/-9 far adjacent channel issue
 * 
 * SW3461-173/1   3/15/12 3:37p shchang
 * SW3461-120: fix huawei Ch.538MHz sensitivity issue
 * 
 * 76   3/12/12 3:45p farshidf
 * SW3461-171: merge to inetg
 * 
 * Fw_Integration_Devel/44   3/12/12 3:44p farshidf
 * SW3461-171: merge to inetg
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/17   3/12/12 3:39p shchang
 * SW3461-120: fix time varying sync
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/16   3/8/12 6:35p mbsingh
 * SW3461-1: Fixed tuner frequency calculation
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/15   3/7/12 10:47p shchang
 * SW3461-120: fix AGC TOP settings
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/14   3/7/12 5:49p shchang
 * SW3461-125: improve ACI performance while without affecting C/N and
 *  echo tests.
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/13   3/2/12 10:52a mbsingh
 * SW3461-145: Fix Signal Strength Calculations
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/12   2/27/12 5:51p shchang
 * SW3461-1: fix "dead zone" issue on J83A
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/11   2/23/12 12:51p farshidf
 * SW3461-120: remove warning
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/10   2/23/12 12:39p farshidf
 * SW3461-120: change BTNR_J83B_SUPPORT to   #ifdnef BTNR_J83A_SUPPORT
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/9   2/23/12 12:35p farshidf
 * SW3461-120: rename STCE40 compile flag to BTNR_J83B_SUPPORT
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/8   2/22/12 5:56p mbsingh
 * SW3461-145: Changed LNA start from mid gain to improve Signal Strength
 *  convergence on 3461
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/7   2/22/12 11:41a shchang
 * SW34610120: 1. update tuner status such that dithering can be changed
 *  without retune. 2. move SCTE-40 settings to ifdef.
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/6   2/16/12 5:09p shchang
 * SW3461-1: improve N+/-4
 * 
 * Fw_Integration_Devel/35   2/16/12 9:41a farshidf
 * SW7552-209: compile fix
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/5   2/16/12 9:40a farshidf
 * SW7552-109: compile fix
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/4   2/15/12 2:56p farshidf
 * SW3461-1: compile fix
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/3   2/15/12 9:24a farshidf
 * SW3461-120: add the 7552 register name for 7552 A0
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/2   2/14/12 10:58p shchang
 * SW3461-120: change RFVGA initial gain to maximum
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/1   2/10/12 12:06p shchang
 * SW3461-120: 1. add ADC6B reset 2. fix 7552 shift-gear
 * 
 * Fw_Integration_Devel/31   2/3/12 2:15p farshidf
 * SW3461-120: merge to main
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/27   1/31/12 7:22p shchang
 * SW3461-120: fix 1. LO DDFS reset 2. tuner status 3. PHYPLL change on-
 *  the-fly
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/26   1/30/12 11:28a farshidf
 * SW3461-120: put the A0 tuner code under its own flag
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/25   1/16/12 7:17p shchang
 * SW3461-120: differetiate PLL not lock/not use
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/24   1/16/12 1:42p shchang
 * SW3461-120: improve N+/-4 ACI performance
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/23   1/11/12 4:08p shchang
 * SW3461-120: add power mode e_Lna_Daisy inside the power control
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/22   1/11/12 10:29a farshidf
 * SW3461-1: fix the second 3461 tuner setting for UHF mode
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/21   1/5/12 6:10p shchang
 * SW3461-120: add smart tune to relsove LO spur around 845MHz
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/20   1/3/12 11:41p shchang
 * SW3461-120: B0 use PHYPLL with CMOS input; B1 use PHYPLL with CML input
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/19   12/26/11 8:50p shchang
 * SW3461-120: 1. optimize tuner settings for DVB-T LTE scenarios. 2.
 *  optimize tuner settings to pass SCTE-40 spec
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/18   12/22/11 11:10a farshidf
 * SW3461-120: merge to dev
 * 
 * 59   12/22/11 11:08a farshidf
 * SW3461-120: make A0 compatible
 * 
 * 58   12/20/11 6:40p farshidf
 * SW3128-1: remove warning
 * 
 * 57   12/15/11 5:59p farshidf
 * SW3461-118: update the tuner to work on B0 and B1
 * 
 * 56   12/8/11 3:18p shchang
 * SW3461-1: power down REFPLL, use PHYPLL to save power
 * 
 * 55   11/23/11 11:12a farshidf
 * SW3461-99: merge to integ
 * 
 * Fw_Integration_Devel/26   11/23/11 11:12a farshidf
 * SW3461-99: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/15   11/23/11 11:10a farshidf
 * SW3461-99:compile fix
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/14   11/22/11 12:24p farshidf
 * SW3461-99: warning fix
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/13   11/22/11 12:20p farshidf
 * SW3461-99: merge Dave's Mixer PLL fix
 * 
 * 52   11/21/11 11:26p farshidf
 * SW7552-139: fix compile issue
 * 
 * 51   11/16/11 6:50p shchang
 * SW3461-1: add BTNR_P_Tuner_PowerUpPLL() to resolve MIXPLL unlock issue
 * 
 * 50   11/4/11 4:11p farshidf
 * SW3461-82: merge to main
 * 
 * Fw_Integration_Devel/24   11/4/11 11:38a farshidf
 * SW3461-82: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/12   11/1/11 12:21p shchang
 * SW3461-1: increase LDO voltage to fix sensitivity issue
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/11   10/25/11 5:18p jputnam
 * CDFEDEMOD-6: Consolidated SmartTune logic such that only PLL dividers
 *  are changed rather than calling P_TunerInit().  This seems to
 *  eliminate occassional instability associated with repeatedly tuning
 *  between FreqPlanA and FreqPlanDefault
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/10   10/25/11 11:17a shchang
 * SW7552-137: fix re-scan issue below 334MHz for DVB-C
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/9   10/25/11 8:16a jputnam
 * CDFEDEMOD-6: Added 756MHz to SmartTune frequency list
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/8   10/17/11 7:42p shchang
 * SW3461-1: 1. add dithering off 2. modified registers in power control
 *  subroutine
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/7   10/16/11 9:42p farshidf
 * SW3461-1: fix th LDO osng lock issue from Dave
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/6   10/14/11 12:56a farshidf
 * SW3461-64: fix warning
 * 
 * 48   10/14/11 12:05a farshidf
 * SW3461-64: fix tuner compile issues
 * 
 * 47   10/13/11 11:58p farshidf
 * SW3461-64: merge to main
 * 
 * Fw_Integration_Devel/22   10/13/11 7:18p farshidf
 * SW3461-64: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/5   10/13/11 7:17p farshidf
 * SW3461-1: fix teh a0 compile issue
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/4   10/11/11 2:18p shchang
 * SW3461-1: optimize AGC for DVB-T2
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/3   10/5/11 1:01p shchang
 * SW3461-1:  1. Select RFAGC clock from REFPLL 2. Set internal LNA bias
 *  current 3. Remove dithering off
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/2   9/27/11 11:52a mbsingh
 * SW3461-1: Made the code compile for B0 with new register changes
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/1   9/26/11 5:26p mbsingh
 * SW3461-1: Incorporate correct register writes for B0
 * 
 * Fw_Integration_Devel/21   9/22/11 4:44p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/16   9/21/11 4:14p farshidf
 * SW3461-1: update files for B0
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/15   9/14/11 10:53p shchang
 * SW3461-1: fix tune-delay issue.
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/14   9/12/11 4:02p shchang
 * SW3461-1: 1. change LNA AGC settings 2. improve AGC shift gear
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/13   9/10/11 9:20p shchang
 * SW3461-1: 1.  fix DDFS FCW resolution. 2. add AGC shift gear. 3.
 *  sensitivity fix.
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/12   9/7/11 11:32a jputnam
 * SW3461-1: Added explicit assertion of PHYPLL reset for reliable
 *  SmartTune operation, rather than assuming PHYPLL reset is already
 *  asserted prior to P_TunerInit().
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/11   9/6/11 11:42a shchang
 * SW3461-1: fix 143/537MHz locking issue.
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/10   9/6/11 11:03a jputnam
 * SW3461-1:  Restore call to P_TunerInit() when SmartTune indicates
 *  change in PLL programming required
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/9   9/1/11 4:50p shchang
 * SW3461-1: optimize AGC settings for DVB-T2 ACI_CCI
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/8   9/1/11 9:17a jputnam
 * SW3461-1: Added Ch K8 (198.5MHz) to SmartTune list
 * 
 * 41   8/29/11 1:32p farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/16   8/29/11 1:31p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/6   8/29/11 1:30p farshidf
 * SW3461-1: add back the TunerInit to fix the AGC problem
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/5   8/26/11 4:58p farshidf
 * SW3461-1: make SmartTuneEnabled magnum compatible
 * 
 * 39   8/26/11 3:45p farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/14   8/26/11 3:38p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/4   8/26/11 2:29p farshidf
 * SW3461-42: remove the extra initTune and reduce the sleep for DCO
 *  stabilization from 100ms to 20ms
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/3   8/24/11 12:40p mbsingh
 * sw3461-1: Fixed tune to work till 1Hz resolution
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/2   8/23/11 5:04p jputnam
 * SW3461-1:  Assert load_en so that PLL divider changes are effective.
 *  Enable SmartTune by default
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/1   8/22/11 12:44p jputnam
 * SW3461-1: Added SmartTune
 * 
 * Fw_Integration_Devel/11   8/2/11 6:23p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/15   8/2/11 6:11p farshidf
 * SW3461-1: update the tuner structure
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/14   7/29/11 1:57p jputnam
 * SW3461-1: Zero reported LNA and PreADC gains in low-IF mode
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/13   7/28/11 3:54p jputnam
 * SW3461-1: Added hooks for low-IF mode
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/12   7/28/11 3:41p farshidf
 * SW3461-1: fix the LNA register usage
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/11   7/27/11 4:36p farshidf
 * SW3461-1: magnum fix
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/10   7/27/11 1:33p farshidf
 * SW3461-1: magnum compatible
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/9   7/27/11 1:15p farshidf
 * SW3461-1: make it magnum compatile
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/8   7/21/11 5:46p farshidf
 * SW3461-1: add DCO fix from Dave
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/16   7/21/11 5:42p farshidf
 * SW3461-1: add DC0 fix from Dave
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/14   7/19/11 7:25p farshidf
 * SW3461-28: remove printf
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/12   7/14/11 10:56p farshidf
 * SW3461-28: chekc-in fixes from Dave
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/11   7/12/11 4:08p farshidf
 * SW3461-17: During 2nd tune, the clocks of 6-bit ADC and DCO will be
 *  turned off, which might causes the following issues: 1. LNA AGC will
 *  be freeze 2. SNR degradation due to DCO offset
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/10   6/29/11 12:32p shchang
 * SW3461-1: replace 6-phase with 8-phase mixer
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/9   6/28/11 6:57p shchang
 * SW3461-1: fix mixer reset sequence
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/8   6/24/11 4:24p mbsingh
 * SW3461-1: SD ADC Cal function improvements
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/7   6/24/11 4:10p shchang
 * SW3461-1: adjust power settings for cable
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/6   6/23/11 5:42p mbsingh
 * SW3461-1: Remove Sleep's from tuner to reduce tune time
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/5   6/23/11 5:33p mbsingh
 * SW3461-1: Remove Sleep's from tuner to reduce tune time
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/4   6/22/11 6:49p cbrooks
 * sw3461-1:Checked tune fix for Dave
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/3   6/21/11 5:34p shchang
 * SW3461-1: fix DDFS FCW accuracy issue.
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/2   6/20/11 9:20p shchang
 * SW3461-1: add tracking filter & move AGC-related subroutine to tuner
 *  initialization
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/1   6/16/11 6:05p shchang
 * SW3461-1: add DPM feature
 * 
 * 29   6/12/11 12:46p farshidf
 * SW3461-1: clena up
 * 
 * 28   6/12/11 12:35p farshidf
 * SW3461-1: code clean up
 * 
 * 27   6/9/11 6:40p mpovich
 * SW3461-1: Merge Ver 0.4 Integ. onto main branch.
 * 
 * SW_System_4_Integ_Test/2   6/9/11 4:48p farshidf
 * SW3461-1: sync up with 7552 code
 * 
 * SW_System_4_Integ_Test/1   6/6/11 2:09p mpovich
 * SW3461-1: Merge disparate branches for test purposes.
 * 
 * Tnr_Fw_Devel_4/9   6/3/11 1:50p mbsingh
 * SW3461-1: ifdef out the LNA AGC code
 * 
 * Tnr_Fw_Devel_4/8   5/26/11 7:24p shchang
 * SW3461-1: fix AGC power-cycle
 * 
 * Tnr_Fw_Devel_4/7   5/25/11 3:19p shchang
 * SW3461-1: increase RFVGA bias for senesitivity
 * 
 * Tnr_Fw_Devel_4/6   5/25/11 3:07p shchang
 * SW3461-1: Remonving code .. for now. Need to fix it later
 * 
 * Tnr_Fw_Devel_4/5   5/25/11 10:03a farshidf
 * SW3461-6: add the timer Interrupt
 * 
 * Tnr_Fw_Devel_4/4   5/24/11 5:34p mbsingh
 * SW3461-1: Implement the LNA AGC cycle ( call back function ) function
 *  for power saving
 * 
 * Tnr_Fw_Devel_4/3   5/23/11 4:55p mbsingh
 * SW3461-1: Got Daisy output working in firmware through BBS
 * 
 * Tnr_Fw_Devel_4/2   5/23/11 4:30p mbsingh
 * SW3461-1: Got loop through working in firmware through BBS
 * 
 * Tnr_Fw_Devel_4/1   5/23/11 3:17p mbsingh
 * SW3461-1: Lower the power consumption of the tuner - LNA AGC take over
 *  point  - PHY PLL Optimization
 * 
 * 26   5/20/11 6:44a mpovich
 * SW3461-1: rename UFE (BUFE) module to TNR (BTNR).
 * 
 * TNR_3461_1/1   5/19/11 5:16p mpovich
 * SW3461-1: Change BUFE module prefix to BTNR
 * 
 * 25   5/18/11 11:09a farshidf
 * SW3461-1: merge main
 * 
 * TNR_3461_1/18   5/17/11 6:17p mbsingh
 * SW3461-1: Calibrate I and Q channel in parallel
 * 
 * TNR_3461_1/17   5/16/11 5:16p shchang
 * change LNA/RF AGC TOP to pass far adjacent scenario
 *
 * TNR_3461_1/16   5/3/11 2:58p hfu
 * To resolve fractional (0.5MHz) tune issue.  This solution is a
 *  workaround and the frequency resolution depends on variable M.
 *
 * TNR_3461_1/15   4/8/11 2:44p farshidf
 * SW3461-1: power up the PLL for DS
 *
 * TNR_3461_1/14   4/5/11 8:24a jputnam
 * SW3461-1: Replaced digital I/Q swap with analog fix when 8-phase mixer
 *  is enabled
 *
 * TNR_3461_1/13   4/4/11 5:55p jputnam
 * SW3461-1: Added I/Q swap when 8-phase mixer is used
 *
 * TNR_3461_1/12   3/24/11 11:53a jputnam
 * SW3461-1: Change TNR clock to 540MHz for cable mode
 *
 * TNR_3461_1/11   3/21/11 2:45p mbsingh
 * SW3461-1: Move SD ADC calibration after tune
 *
 * TNR_3461_1/10   3/18/11 4:09p farshidf
 * SW3461-1: merge  main
 *
 * 20   3/18/11 4:08p farshidf
 * SW3461-1: merge  main
 *
 * TNR_3461_1/9   3/17/11 5:36p mbsingh
 * SW3461-1: Added watchdog reset or mixer PLL lock
 *
 * TNR_3461_1/8   3/15/11 5:08p mbsingh
 * SW3461-1: Enabled 6 bit ADC and LNA AGC
 *
 * TNR_3461_1/7   3/15/11 3:38p mbsingh
 * SW3461-1: Adding RF AGC setup for tuner
 *
 * TNR_3461_1/6   3/14/11 8:08p lukose
 * SW3461-1: Temporarily bypass LNA to get tuner tuned. Will be removed
 *  later
 *
 * TNR_3461_1/5   3/14/11 7:41p lukose
 * SW3461-1: Ported tuner .bbs script. Tuner is tuning
 *
 * TNR_3461_1/4   3/14/11 6:27p lukose
 * SW3461-1: Fix for the DDFS calculation
 *
 * TNR_3461_1/3   3/11/11 6:49p mbsingh
 * SW3461-1: Fixing DCO Setup
 *
 * TNR_3461_1/2   3/11/11 6:06p lukose
 * SXW3461-1: Merging Tuner Changes
 *
 * 17   3/10/11 5:35p cbrooks
 * sw3461-1: New Code
 *
 * 16   3/10/11 5:00p mbsingh
 * sw3461-1:  Updating SD ADC Calibration routine based on sd_routine.bss
 *  from Dave
 *
 * 15   3/9/11 9:02p cbrooks
 * sw3461-1:Added LNA level and RFVGA level to status
 *
 * 14   3/9/11 3:01p farshidf
 * SW3461-1: fix the math function
 *
 * 13   3/8/11 8:26p cbrooks
 * sw3461-1:new code
 *
 * 12   3/8/11 2:43p cbrooks
 * sw3461-1:new code
 *
 * 11   3/7/11 9:27p cbrooks
 * sw3461-1:new code
 *
 * 9   3/6/11 6:36p cbrooks
 * sw3461-1:new code
 *
 * 7   3/6/11 5:58p cbrooks
 * SW3461-1:New TNR Code
 *
 * 6   3/2/11 4:59p mpovich
 * SW3461-1: Fix TNR struct compiler bugs.  Add HAB related updates for T2
 *  and for TNR.
 *
 * Rom_Devel_3461/1   3/2/11 4:24p mpovich
 * SW3461-1: Fix TNR struct compiler bugs.  Add HAB related updates for T2
 *  and for TNR.
 *
 * 5   3/1/11 12:59p cbrooks
 * sw3461-1:new code
 *
 * 4   2/28/11 3:38p cbrooks
 * SW3461-1:new code for tuner
 *
 * 3   2/25/11 5:04p shchang
 * sw3461-1:new code
 *
 * 2   2/24/11 3:18p farshidf
 * SW3461-1: update the code for tuner
 *
 * 1   2/24/11 11:27a farshidf
 * SW3461-1: add the initial Tuner code from Dave
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btmr.h"
#ifndef LEAP_BASED_CODE
#include "btnr.h"
#include "bdbg.h"
#include "btnr_priv.h"
#include "btnr_3x7xib_priv.h"
#endif
#include "btnr_global_clk.h"
#include "btnr_struct.h"
#ifdef  LEAP_BASED_CODE
#include "btnr_api.h"
#endif
#include "btnr_tune.h"
#include "bmth.h"
#if ((BCHP_CHIP == 3461) || (BCHP_CHIP == 3462) || (BCHP_CHIP == 3472))
#include "bchp_tm.h"
#endif
#if (BCHP_CHIP == 7552)
#include "bchp_gio_aon.h"
#endif
#include "bchp_ufe_afe.h"
#include "bchp_sdadc.h"
#ifndef LEAP_BASED_CODE
BDBG_MODULE(btnr_tune);
#define POWER2_24 16777216
#define POWER2_3 8
#define POWER2_16 65536
#define POWER2_29 536870912
#define POWER2_27 134217728
#endif
#include "bchp_ufe.h"

#include "btnr_tune_ofdm.h"
#include "btnr_tune_ads.h"


/*******************************************************************************************************************
 * BTNR_P_TunerSleep()  This routine sets the tuner delay x ms
 ******************************************************************************************************************/
void BTNR_P_TunerSleep(uint8_t x)
{
#ifdef LEAP_BASED_CODE 
	BKNI_Sleep(x);
#else
	BKNI_Delay(x*1000);
#endif
}


/*******************************************************************************************
 * BTNR_P_Daisy_Control()		This routine controls the the Daisy output
 *The PI will call the BTNR_P_Tuner_Power_Control() when it detects a change in the
 *h->pTunerParams->BTNR_Daisy_Params.Daisy_Enable THEN call this function
 *******************************************************************************************/
BERR_Code BTNR_P_Daisy_Control(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;

	if (h->pTunerParams->BTNR_Daisy_Params.Daisy_Enable == BTNR_Daisy_Params_eEnable)
	{
		/*anything not handled by BTNR_P_Tuner_Power_Control()*/

		if (h->pTunerParams->BTNR_Daisy_Params.Daisy_Source == BTNR_Daisy_Source_eVHF)
		{
			BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DAISY_VHF, 0x1);
			BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DAISY_UHF, 0x0);
		}
		else if (h->pTunerParams->BTNR_Daisy_Params.Daisy_Source == BTNR_Daisy_Source_eUHF)
		{
			BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DAISY_VHF, 0x0);
			BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DAISY_UHF, 0x1);
		}
		else if (h->pTunerParams->BTNR_Daisy_Params.Daisy_Source == BTNR_Daisy_Source_eUHF_VHF)
		{
			BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DAISY_VHF, 0x1);
			BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DAISY_UHF, 0x1);
		}
	}
	else
	{
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DAISY_VHF, 0x0);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DAISY_UHF, 0x0);
	}

	BDBG_MSG((" BTNR_P_Daisy_Control() Complete"));

/*goto label to return error code if something bad happened above*/
  return retCode;
}


/*******************************************************************************************
 * BTNR_P_LoopThru_Control()		This routine controls the LoopThru output
 *The PI will call the BTNR_P_Tuner_Power_Control() when it detects a change in the
 *h->pTunerParams->BTNR_LoopThru_Params.LoopThru_Enable THEN call this function
 *******************************************************************************************/

BERR_Code BTNR_P_LoopThru_Control(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;

	if (h->pTunerParams->BTNR_LoopThru_Params.LoopThru_Enable == BTNR_LoopThru_Params_eEnable)
	{
		/*anything not handled by BTNR_P_Tuner_Power_Control()*/
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_LT, 0x1);
	}
	else
	{
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_LT, 0x0);
	}

	BDBG_MSG((" BTNR_P_LoopThru_Control() Complete"));

/*goto label to return error code if something bad happened above*/
  return retCode;
}


/*******************************************************************************************
 * BTNR_P_DPM_Control()		This routine controls the LoopThru output
 *The PI will call the BTNR_P_Tuner_Power_Control() when it detects a change in the
 *h->pTunerParams->BTNR_DPM_Params.DPM_Enable THEN call this function
 *******************************************************************************************/
BERR_Code BTNR_P_DPM_Control(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	uint16_t temp;
	if (h->pTunerParams->BTNR_DPM_Params.DPM_Enable == BTNR_DPM_Params_eEnable)
	{
		BTNR_P_DPMSetFreq(h);
	}
	else
	{
	/*Power down DPM*/
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DPM_LOBUF_REG, 0x0);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DPM, 0x0);		
	/*Power down DPM fx clock*/
		temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch);
		temp = temp & 0x2F ;
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, temp);
	}
	BDBG_MSG(("BTNR_P_DPM_Control() Complete"));
  return retCode;
}


/******************************************************************************
 BTNR_P_TunerSetMXR()
******************************************************************************/
void BTNR_P_TunerSetMXR(BTNR_3x7x_ChnHandle h)
{
	/*MXR SR, pre_div reset*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, MXR_SR_reset, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, MXR_PREDIV_reset, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, MXR_SR_reset, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, MXR_PREDIV_reset, 0x0);
	BKNI_Delay(20); /*20 usec*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, MXR_SR_reset, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, MXR_PREDIV_reset, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, MXR_SR_reset, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, MXR_PREDIV_reset, 0x0);
	BDBG_MSG(("BTNR_P_TunerSetMXR() Complete"));
}

/******************************************************************************
 BTNR_P_TunerSetLO()
******************************************************************************/
void BTNR_P_TunerSetLO(BTNR_3x7x_ChnHandle h)
{
	/*local variables*/
	uint8_t		index, M, N, ndiv, pdiv;
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, Freq;
	uint16_t	temp;

	/* Enable LO state-machine clock */	
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch);
	temp = temp | 0x4 ;
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, temp);

	/*Optimize LO settings*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_02, lP_QPbiasCNT_1p0, 0xC);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_02, lP_QPbiasCNT2_1p0, 0x4);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_03, lP_VcREF_1p0, 0x15);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_04, lP_mainVbal_1p0, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_04, lP_mainIbal_1p0, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_03, P_divreg1p0_cntl_1p0, 0x3);

	/*Enable auto tuner*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, LO_SMtuner_resetb, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_05, LO_SMtuner_tune_en, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_05, LO_SMtuner_param_sel, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, LO_SMtuner_resetb, 0x1);

	index = h->pTunerStatus->LO_index;
	BDBG_MSG(("BTNR_P_TunerSetLO: index %d",index));

	/* Set the DDFS FCW*/
	if (h->pTunerParams->BTNR_DPM_Params.DPM_Enable != BTNR_DPM_Params_eEnable)
	{
		/*Program the values for Tuner LO*/
		/* lookup freq range */
		for (index = 0; index < BTNR_TUNER_LO_TABLE_SIZE; index++)
		{
			if (h->pTunerParams->BTNR_Acquire_Params.RF_Freq <= Tuner_LO_Freq_Table[index])
			{
				/*index = BTNR_TUNER_LO_TABLE_SIZE;*/
				break;
			}
		}
		h->pTunerStatus->LO_index = index;		
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MXR_SR6p8p12p16p,    Tuner_LO_Table[index].i_MXR_SR6p8p12p16p);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MIXER_sel_div_ratio, Tuner_LO_Table[index].i_MIXER_sel_div_ratio);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MIXER_sel,           Tuner_LO_Table[index].i_MIXER_sel);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MIXER_HRM_mode,      Tuner_LO_Table[index].i_MIXER_HRM_mode);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MIXER_sel_MUX0p6p8p, Tuner_LO_Table[index].i_MIXER_sel_MUX0p6p8p);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_03, lP_fbdivn_1p0,         Tuner_LO_Table[index].lP_fbdivn_1p0);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_03, lP_div23_sel_1p0,      Tuner_LO_Table[index].lP_div23_sel_1p0);
		M = Tuner_LO_Table[index].M_Factor;
		N = Tuner_LO_Table[index].lP_fbdivn_1p0;
	}
	else 
	{
		/*Program the values for DPM LO*/
		/* lookup freq range */
		for (index = 0; index < BTNR_DPM_LO_TABLE_SIZE; index++)
		{
			if (h->pTunerParams->BTNR_Acquire_Params.RF_Freq <= DPM_LO_Freq_Table[index])
			{
				/*index = BTNR_TUNER_DPM_LO_TABLE_SIZE;*/
				break;
			}
		}
		h->pTunerStatus->LO_index = index;		
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MXR_SR6p8p12p16p,    DPM_LO_Table[index].i_MXR_SR6p8p12p16p);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MIXER_sel_div_ratio, DPM_LO_Table[index].i_MIXER_sel_div_ratio);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MIXER_sel,           DPM_LO_Table[index].i_MIXER_sel);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MIXER_HRM_mode,      DPM_LO_Table[index].i_MIXER_HRM_mode);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01,    i_MIXER_sel_MUX0p6p8p, DPM_LO_Table[index].i_MIXER_sel_MUX0p6p8p);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_03, lP_fbdivn_1p0,         DPM_LO_Table[index].lP_fbdivn_1p0);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_03, lP_div23_sel_1p0,      DPM_LO_Table[index].lP_div23_sel_1p0);
		M = DPM_LO_Table[index].M_Factor;
		N = DPM_LO_Table[index].lP_fbdivn_1p0;
		/*Program the values for DPM*/	 
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01,    i_logen_PreSel,        DPM_Table[index].logen_PreSel);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01,    i_logen_two,           DPM_Table[index].logen_two);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01,    i_logen_six,           DPM_Table[index].logen_six);
		BKNI_Sleep(10);		
	}

	BDBG_MSG(("BTNR_P_TunerSetFreq: M=%d, N=%d",M, N));

	/* FCW = (Fc/RFPLL_FREQ)*(M/N)*2^37* where Fc is desired frequency
	 * M[2 255], N[4,8,12,16,24,32,64]
	 * FCW = ((Fc/100)*M)*2^37))/(RFPLL_FREQ/100*N)) where Fc is desired frequency*/
	BDBG_MSG(("Rf Freq=%08x",h->pTunerParams->BTNR_Acquire_Params.RF_Freq));

	Freq = (h->pTunerParams->BTNR_Acquire_Params.RF_Freq);

	ulMultA = 0x1;
	ulMultB = 0;
	ulNrmHi = 0;
	ulNrmLo = Freq*(M/4);

#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR_REFPLL_01, REF_DIV_fb);
	pdiv = 1;
	ndiv = BREG_ReadField(h->hRegister, UFE_AFE_TNR_REFPLL_02, REF_DIV_ratio); 
#elif (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, ndiv_int);
	pdiv = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, p1div);
	ndiv = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m2div);
#endif
	BMTH_HILO_64TO64_Mul(ulMultA, ulMultB, ulNrmHi, ulNrmLo, &ulMultA, &ulMultB);
	BDBG_MSG(("First %08x, %08x",ulMultA, ulMultB));
	BMTH_HILO_64TO64_Div32(ulMultA, ulMultB,  ( (REF_FREQ*(temp/ndiv/pdiv)/128)*N ), &ulNrmHi, &ulNrmLo);
	BDBG_MSG(("Second %08x, %08x",ulNrmHi, ulNrmLo));

	if (ulNrmHi > 0x0000001F)
	{
     BDBG_ERR(("DDFS is outside of the 37 bit range in BTNR_P_TunerSetFreq()"));
	}

	/* Setup DDFS*/
	/* program fcw[36:5] and fcw[4:0]*/
	ulNrmHi = (((ulNrmHi<<27) & 0xF8000000) | ((ulNrmLo>>5) & 0x07FFFFFF)); /*Get 32 msb's*/
	ulNrmLo = (ulNrmLo & 0x0000001F);                                        /*Get 5 lsb's*/
	/*Reset and Release DDFS*/
	#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0) /* (BCHP_VER == BCHP_VER_A0) */   /* TO FIX - Fixed*/
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_MXRPLL, 0x7afe  );
		BKNI_Sleep(1);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_MXRPLL, 0x7aff  );
	#else    
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02,    loddfs_resetn, 0);
		BKNI_Sleep(1);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02,    loddfs_resetn, 1);
	#endif

	/*Program FCW*/
	BDBG_MSG(("BTNR_P_TunerSetFreq: Hi=%x, Lo=%x",ulNrmHi, ulNrmLo));
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_DDFS_FCW_02, ulNrmHi);      /*32 MSB of 37 bit word*/
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_DDFS_FCW_01, ulNrmLo);      /* 5 LSB of 37 bit word*/

	/* De-assert reset/resetb for div23, cml, SR, pre_div */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, lP_rstdiv23_1p0, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, lP_cmlDIVRST, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, lP_ld_RESET_STRT_1p0, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, lP_rstdiv23_1p0, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, lP_cmlDIVRST, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, lP_ld_RESET_STRT_1p0, 0x0);
	BKNI_Sleep(1);

	/* Startup DDFS*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DDFS_01, i_fcw_strb, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DDFS_01, i_fcw_strb, 0x0);
	BKNI_Sleep(1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DDFS_01, i_fcw_strb, 0x1);
	BKNI_Sleep(1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DDFS_01, i_fcw_strb, 0x0);

	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_05, LO_SMtuner_tune_en, 0x1);
	BKNI_Sleep(1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRPLL_05, LO_SMtuner_tune_en, 0x0);
  
  /*Power down state machine clock*/
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch);
	temp = temp & 0x3B ;
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, temp);
	/*Lock Status*/
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXRPLL_07, MXRPLL_FREQ_LOCK);
	if (temp == 1)
	{
		BDBG_MSG(("MIXPLL is locked"));
	}
	else if (temp == 0)
	{
		BDBG_MSG(("MIXPLL is NOT locked"));
	}		
}

/*******************************************************************************************
 * BTNR_P_Tuner_PowerUpPLL()		This routine controls the power up/down of the tuner blocks
 *******************************************************************************************/
BERR_Code BTNR_P_Tuner_PowerUpPLL(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	/* optimize ADC6B */
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_ADC6B_01, 0xCA683600);
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_ADC6B_02, 0x54300055);
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_ADC6B_03, 0x39428002);
	/* Swap I/Q on 8/12/16-phase mixer */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_02, i_MIXER_setSR8_Q, 0xf0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_02, i_MIXER_setSR8_I, 0xc3);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_03, i_MIXER_setSR12_I, 0xE07);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_03, i_MIXER_setSR12_Q, 0xFC0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_04, i_MIXER_setSR16_I, 0xF00F);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_04, i_MIXER_setSR16_Q, 0xFF00);
	/*bias settings for LNA + MXR + FGA + LPF*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_sf_vcm, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_FB_bias_ctrl, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_BUF_vcm_ctrl, 0x0);
/*	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_Vb_sk, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_Ib_sk, 0x0);*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01, i_MIXER_vcmfb_gen_ctrl, 0x3);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_01, i_MIXER_low_I_mode, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_CM_internal_sel, 0x1);
	/* set RFAGC clock from REFPLL */	
#if (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_RFAGC_CLK_sel, 0x0);
#endif
#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)
#if (BCHP_CHIP == 7552)
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, spare00, 0x1);
#else
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_RFAGC_CLK_sel, 0x1);
#endif	
#endif		
	/*Set RFPLL and PHYPLL*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR_REFPLL_01, REF_DIV_fb, 0x14);    /*REF_DIV_fb = 20*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR_REFPLL_02, REF_DIV_ratio, 0x01); /*REF_DIV_ratio = 1*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR_REFPLL_03, REF_OUTDIV_m0, 0x01); /*REF_OUTDIV_m0 = 1*/
#if (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
	BREG_WriteField(h->hRegister, UFE_AFE_TNR_REFPLL_03, REF_CLK_bypass_enable, 0x1);
#else
	BREG_WriteField(h->hRegister, UFE_AFE_TNR_REFPLL_03, REF_CLK_bypass_enable, 0x0);
#endif

#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_1)
	if ((h->pTunerParams->BTNR_Local_Params.RevId  == BCHP_VER_B1)|| (h->pTunerParams->BTNR_Local_Params.RevId  == BCHP_VER_B2))
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, i_clk_sel, 0x1);	
	else
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, i_clk_sel, 0x0);	
#elif (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_2)  /* 3462 A0 */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, i_clk_sel, 0x1);	
#endif
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_02, ndiv_frac, 0x0);/*ndiv_frac = 0*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, p1div, 	0x03);/*p1div = 3*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, ndiv_int, 0x78);/*ndiv_int = 150*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m1div, 	0x01);/*m1div = 1*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m2div,	0x02);/*m2div = 2*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m3div, 	0x0C);/*m3div = 12*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m4div, 	0x1B);/*m4div = 27*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, m5div, 	0x00);/*m5div = 256*/
    BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, m6div, 	0x04);/*m6div = 4*/	 
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, load_en_ch6_ch1, 0x3F);  
	
#ifdef SmartTuneEnabled
    /* for multi channel tuner, TM_SYS_PLL regs are offset (from channel 0's base) using hTMSysPllReg */
#if ( BTNR_P_BCHP_TNR_NUM_CORES < 2 ) /* (e.g. not dual tuner [3472]) */
	BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL_PDIV, 2);	
    BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL_NDIV_INT, 80);
#else
    BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL0_PDIV, 2);	
    BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL0_NDIV_INT, 80);
#endif
        /* TM_SYS_PLL_CLK_XPT_216 not channelized */
#if (BTNR_P_BCHP_TNR_CORE_VER < BTNR_P_BCHP_CORE_V_1_3)
    BREG_WriteField(h->hTnr->hRegister, TM_SYS_PLL_CLK_216, DIV, 10);			
#else
    BREG_WriteField(h->hTnr->hRegister, TM_REG_PLL_CLK_XPT_216, DIV, 10);			
#endif
#endif /* SmartTuneEnabled */

	/*optimize PHYPLL*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_01, kvcox, 0x3);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_01, Icpx, 0x1F);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_01, Icpx2, 0x1);	
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_01, Rz, 0xF);
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_01, 0x00030001);
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02, 0x00208000);
#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,  0x00006CFF);
#elif (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,  0x00006806);
#endif
	BKNI_Sleep(1);
	/* De-assert reset/resetb for REFPLL/PHYPLL */
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_RESETB_01, 0x00000003);
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_RESET_01, 0xFFFFFFF0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, PHYPLL_dreset, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, PHYPLL_areset, 0);
	BKNI_Sleep(1);
	/* Initialize SD ADC */
	/*This Must Be Complete with calibration added*/
	BREG_WriteField(h->hRegister, SDADC_CTRL_SYS0, i_adcclk_reset, 0x0);
	BREG_Write32(h->hRegister, BCHP_SDADC_CTRL_PWRUP, 0x00000003);
	BREG_WriteField(h->hRegister, SDADC_CTRL_RESET, i_Ich_reset, 0x0);
	BREG_WriteField(h->hRegister, SDADC_CTRL_RESET, i_Qch_reset, 0x0);		
	
	BDBG_MSG(("BTNR_P_TunerPowerUpPLL() Complete"));
	return retCode;
}

/*******************************************************************************************
 * BTNR_P_TunerSetInputMode()		This routine set tuner input mode
 *******************************************************************************************/
BERR_Code BTNR_P_TunerSetInputMode(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	uint32_t temp_UFE_AFE_TNR0_PWRUP_01 = 0;
	switch (h->pTunerParams->BTNR_RF_Input_Mode)
	{
	case BTNR_3x7x_TunerRfInputMode_eExternalLna: 
		temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x0F0F0015);	
		if (h->pTunerParams->BTNR_Daisy_Params.Daisy_Enable == BTNR_Daisy_Params_eEnable) 
		{
			switch (h->pTunerParams->BTNR_Daisy_Params.Daisy_Source)
			{
			case BTNR_Daisy_Source_eVHF: 
				temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x00000800);
			break;
			case BTNR_Daisy_Source_eUHF: 
				temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x00001000);
			break;
			case BTNR_Daisy_Source_eUHF_VHF: 
				temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x00001800);
			break;			
			default:
				BDBG_ERR(("ERROR!!! h->pTunerParams->BTNR_Daisy_Params.Daisy_Enable, value received is %d",h->pTunerParams->BTNR_Daisy_Params.Daisy_Source));
				retCode = BERR_INVALID_PARAMETER;
			goto something_bad_happened;
			}	
		}
		if (h->pTunerParams->BTNR_LoopThru_Params.LoopThru_Enable == BTNR_LoopThru_Params_eEnable) {temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x00000400);}	
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_01, temp_UFE_AFE_TNR0_PWRUP_01);
#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02,	0x00208000);
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,	0x00002CFF);
#elif (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02,	0x00228000);
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,	0x00006806);
#endif
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_bypass, 0);
	break;
	case BTNR_3x7x_TunerRfInputMode_eInternalLna: 
	case BTNR_3x7x_TunerRfInputMode_InternalLna_Daisy: 
		temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x0FA30017);
		if (h->pTunerParams->BTNR_Daisy_Params.Daisy_Enable == BTNR_Daisy_Params_eEnable) 
		{
			switch (h->pTunerParams->BTNR_Daisy_Params.Daisy_Source)
			{
			case BTNR_Daisy_Source_eVHF: 
				temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x00000800);
			break;
			case BTNR_Daisy_Source_eUHF: 
				temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x00001000);
			break;
			case BTNR_Daisy_Source_eUHF_VHF: 
				temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x00001800);
			break;			
			default:
				BDBG_ERR(("ERROR!!! h->pTunerParams->BTNR_Daisy_Params.Daisy_Enable, value received is %d",h->pTunerParams->BTNR_Daisy_Params.Daisy_Source));
				retCode = BERR_INVALID_PARAMETER;
			goto something_bad_happened;
			}	
		}
		if (h->pTunerParams->BTNR_LoopThru_Params.LoopThru_Enable == BTNR_LoopThru_Params_eEnable) {temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x00000400);}		
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_01, temp_UFE_AFE_TNR0_PWRUP_01);
#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02,	0x00218000);
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,	0x00006CFF);
#elif (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02,	0x00238000);
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,	0x00006806);	
#endif
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_bypass, 0);
	break;
	case BTNR_3x7x_TunerRfInputMode_eStandardIf: 
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_01,	0x0F030015);
#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02,	0x00208000);
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,	0x00002CFF);
#elif (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02,	0x00228000);
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,	0x00006806);
#endif
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_bypass, 1);
	break;
	case BTNR_3x7x_TunerRfInputMode_eLowIf:
	case BTNR_3x7x_TunerRfInputMode_eBaseband:
	case BTNR_3x7x_TunerRfInputMode_eOff:
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_01, 0x00030001);
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02, 0x00208000);
#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,  0x00002CFF);
#elif (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
		BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR_PWRUP_01,  0x00006806);
#endif
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_bypass, 0);
	break;	
	default:
		BDBG_ERR(("ERROR!!! h->pTunerParams->BTNR_RF_Input_Mode, value received is %d",h->pTunerParams->BTNR_RF_Input_Mode));
		retCode = BERR_INVALID_PARAMETER;
		/*goto bottom of function to return error code*/
		goto something_bad_happened;
	}
	/* force to terrestrial mode */
	h->pTunerParams->BTNR_Internal_Params.Bias_Select = BTNR_Internal_Params_TunerBias_eACI;
	BTNR_P_TunerSetSignalPath(h);	
	if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial) 	BTNR_P_Ods_TunerSetAGCTOP(h);
	if (h->pTunerParams->BTNR_Acquire_Params.Application  == BTNR_TunerApplicationMode_eCable) 	BTNR_P_Ads_TunerSetAGCTOP(h);
	BTNR_P_TunerSetRFAGC(h);
	BTNR_P_TunerSetADC6B(h);
	BTNR_P_TunerSetLNAAGC(h);
	if (h->pTunerParams->BTNR_RF_Input_Mode == BTNR_3x7x_TunerRfInputMode_InternalLna_Daisy) {h->pTunerStatus->AGC_BW = 0;}
	else {h->pTunerStatus->AGC_BW = 1;}
	if (h->pTunerParams->BTNR_Acquire_Params.Application  == BTNR_TunerApplicationMode_eTerrestrial) BTNR_P_Ods_TunerSetAGCBW(h);
	if (h->pTunerParams->BTNR_Acquire_Params.Application  == BTNR_TunerApplicationMode_eCable) BTNR_P_Ads_TunerSetAGCBW(h);
	#ifdef BTNR_ENABLE_SDADC_CAL
	BTNR_P_CalFlashSDADC(h);
	#endif
	h->pTunerParams->BTNR_Internal_Params.RFInputModeComplete = true;
	#ifdef LEAP_BASED_CODE
	BTNR_P_TunerAGCMonitor(h);
	switch (h->pTunerParams->BTNR_Acquire_Params.Application)
	{
	case BTNR_TunerApplicationMode_eTerrestrial:		
		BTMR_StartTimer(h->htmr, 500000);   /* the timer is in Micro second */
	break;
	case BTNR_TunerApplicationMode_eCable:
		BTMR_StartTimer(h->htmr, 100000);   /* the timer is in Micro second */
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerParams->BTNR_Acquire_Params.Application, value received is %d",h->pTunerParams->BTNR_Acquire_Params.Application));
	}	
#endif
	BDBG_MSG(("BTNR_P_TunerSetInputMode() Complete"));
	BDBG_MSG((" h->pTunerParams->BTNR_RF_Input_Mode, value received is %d",h->pTunerParams->BTNR_RF_Input_Mode));
/*goto label to return error code if something bad happened above*/
something_bad_happened:
  return retCode;
}

/******************************************************************************
 BTNR_P_TunerStatusReset() - reset the status structure
******************************************************************************/
BERR_Code BTNR_P_TunerStatusReset(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;

	/* clear the lock status */
	h->pTunerStatus->Tuner_Ref_Lock_Status = BTNR_Status_eUnlock;
	h->pTunerStatus->Tuner_Mixer_Lock_Status = BTNR_Status_eUnlock;
	h->pTunerStatus->Tuner_Phy_Lock_Status = BTNR_Status_eUnlock;
	h->pTunerStatus->Tuner_RF_Freq = 0;
	h->pTunerStatus->Tuner_PreADC_Gain_x256db = (int16_t)0x8000;
	h->pTunerStatus->External_Gain_x256db = (int16_t)0x8000;

  return retCode;
}


/*******************************************************************************************************************
 * BTNR_P_TunerSetExternalFGLNASwitch()  This routine sets the external FGLNA switch
 ******************************************************************************************************************/
void BTNR_P_TunerSetExternalFGLNASwitch(BTNR_3x7x_ChnHandle h)
{
	uint32_t temp;
#if (BCHP_CHIP == 7552)
	if (h->pTunerStatus->External_FGLNA_Mode == 1)
	{
		temp = BREG_Read32(h->hRegister, BCHP_GIO_AON_DATA_LO);
		temp = temp | 0x00000010;
		BREG_Write32(h->hRegister, BCHP_GIO_AON_DATA_LO, temp);
		BDBG_MSG(("External FGLNA set to normal mode"));
		if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable) 
			BTNR_P_Ads_TunerShiftGearAGC(h);
		if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial) 
			BTNR_P_Ods_TunerShiftGearAGC(h);
	}
	else if (h->pTunerStatus->External_FGLNA_Mode == 0)
	{
		temp = BREG_Read32(h->hRegister, BCHP_GIO_AON_DATA_LO);
		temp = temp & 0xFFFFFFEF;
		BREG_Write32(h->hRegister, BCHP_GIO_AON_DATA_LO, temp);
		BDBG_MSG(("External FGLNA set to bypass mode"));
	}
#endif
#if ((BCHP_CHIP == 3461) || (BCHP_CHIP == 3462) || (BCHP_CHIP == 3472))
	if (h->pTunerStatus->External_FGLNA_Mode == 1)
	{
		temp = BREG_Read32(h->hTnr->hRegister, BCHP_TM_GPIO_DATA);
		temp = temp | 0x00000008;
		BREG_Write32(h->hTnr->hRegister, BCHP_TM_GPIO_DATA, temp);
		BDBG_MSG(("External FGLNA set to normal mode"));
	}
	else if (h->pTunerStatus->External_FGLNA_Mode == 0)
	{
		temp = BREG_Read32(h->hTnr->hRegister, BCHP_TM_GPIO_DATA);
		temp = temp & 0xFFFFFFF7;
		BREG_Write32(h->hTnr->hRegister, BCHP_TM_GPIO_DATA, temp);
		BDBG_MSG(("External FGLNA set to bypass mode"));
	}
#endif
}		

/*******************************************************************************************************************
 * BTNR_P_TunerSetExternalFGLNA()  This routine sets the external FGLNA mode
 ******************************************************************************************************************/
static void BTNR_P_TunerSetExternalFGLNA(BTNR_3x7x_ChnHandle h)
{
	if ((((h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eACI) && 
		(h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable)) ||
		(BCHP_CHIP == 3472)) &&  
		(h->pTunerStatus->Tuner_Change_Settings == 0))
	{
		BTNR_P_TunerAGCReadBack(h);
		if ((h->pTunerStatus->External_FGLNA_Mode == 1) && (h->pTunerStatus->Tuner_LNA_Gain_Code <= FGLNA_BYPASS_THRESHOLD_HIGH) )
		{h->pTunerStatus->External_FGLNA_Mode = 0; BTNR_P_TunerSetExternalFGLNASwitch(h);}
		if ((h->pTunerStatus->External_FGLNA_Mode == 0) && (h->pTunerStatus->Tuner_LNA_Gain_Code >= FGLNA_BYPASS_THRESHOLD_LOW))
		{h->pTunerStatus->External_FGLNA_Mode = 1; BTNR_P_TunerSetExternalFGLNASwitch(h);}
	}
}

/*******************************************************************************************************************
 * BTNR_P_TunerSetRFFILSwitch()  This routine sets RFFIL switches
 ******************************************************************************************************************/
static void BTNR_P_TunerSetRFFILSwitch(BTNR_3x7x_ChnHandle h)
{
	switch (h->pTunerParams->BTNR_Internal_Params.RFFIL_Select)
	{
	case BTNR_Internal_Params_TunerRFFIL_eMOCATRAP:
		/*Use MOCA Trap*/
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, RF_DPD_sel, 1);/* RF peak-detector set to MoCA trap */
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_MoCATRAP_swen, 1);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_TRKFIL, 0x0);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_bypass_en, 0x0);
	break;
	case BTNR_Internal_Params_TunerRFFIL_eTRKFIL:	
		/*Use Tracking Filter*/
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, RF_DPD_sel, 0);/* RF peak-detector set to tracking filter */
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_TRKFIL, 0x1);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_MoCATRAP_swen, 0);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_bypass_en, 0x0);
	break;
	case BTNR_Internal_Params_TunerRFFIL_eTRKBYPASS:	
		/*Use Tracking Filter Bypass*/
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, RF_DPD_sel, 0);/* RF peak-detector set to tracking filter */
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_bypass_en, 0x1);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_TRKFIL, 0x0);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_MoCATRAP_swen, 0x0);	
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerParams->BTNR_Internal_Params.RFFIL_Select, value received is %d",h->pTunerParams->BTNR_Internal_Params.RFFIL_Select));
	}	
	BDBG_MSG(("BTNR_P_TunerSetRFFILSwitch() Complete"));	
}

/*******************************************************************************************************************
 * BTNR_P_TunerRFAGCIndirectRead()  This routine does RFAGC indirect write
 ******************************************************************************************************************/
 void BTNR_P_TunerRFAGCIndirectRead(BTNR_3x7x_ChnHandle h)
{
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_01, i_raddr, h->pTunerStatus->RFAGC_indirect_addr);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_rload, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_rload, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_rload, 0x0);
	h->pTunerStatus->RFAGC_indirect_data = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFAGC_05, o_rdata);
}

/*******************************************************************************************************************
 * BTNR_P_TunerRFAGCIndirectWrite()  This routine does RFAGC indirect write
 ******************************************************************************************************************/
 void BTNR_P_TunerRFAGCIndirectWrite(BTNR_3x7x_ChnHandle h)
{
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_01, i_waddr, h->pTunerStatus->RFAGC_indirect_addr);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_04, i_wdata, h->pTunerStatus->RFAGC_indirect_data);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_wload, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_wload, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_wload, 0x0);
}


/*******************************************************************************************************************
 * BTNR_P_TunerAGCReadBack()  This routine read back the tuner AGC gain code
 ******************************************************************************************************************/
 void BTNR_P_TunerAGCReadBack(BTNR_3x7x_ChnHandle h)
{
	uint32_t ReadReg0;
	uint8_t ReadReg1;
	/*LNA AGC level*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_TESTCTRL_00, i_test_LNAAGC, 0x5);
	ReadReg0 = BREG_Read32(h->hRegister, BCHP_UFE_AFE_TNR0_LNAAGC_READBUS_00);
	ReadReg0 = (ReadReg0>>23) & 0x0000001F;
	h->pTunerStatus->Tuner_LNA_Gain_Code = ReadReg0;  /*This value is used farther down to get gain in db*/

	/*RFVGA AGC level*/
	ReadReg1 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_clk_dither);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_clk_dither, 0x1);
	h->pTunerStatus->RFAGC_indirect_addr = 0x5;
	BTNR_P_TunerRFAGCIndirectRead(h);
	ReadReg0 = h->pTunerStatus->RFAGC_indirect_data;
	h->pTunerStatus->Tuner_RFVGA_Gain_Code = ReadReg0; /*This value is used farther down to get gain in db*/
	if (ReadReg1 == 0) 
	{
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_clk_dither, 0x0);
	}
}

/*******************************************************************************************************************
 * BTNR_P_TunerSetDither()  This routine sets the tuner AGC TOP
 ******************************************************************************************************************/
void BTNR_P_TunerSetDither(BTNR_3x7x_ChnHandle h)
{
	switch (h->pTunerStatus->RFAGC_dither)
	{
	case 0:	
		if (BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_clk_dither) == 0x1)
		{
			BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_clk_dither, 0x0);
			BDBG_MSG(("dithering off"));
		}
	break;
	case 1:	
		if (BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_clk_dither) == 0x0)
		{
			BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_clk_dither, 0x1);
			BDBG_MSG(("dithering on"));
		}
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid BTNR_P_TunerSetDither"));
	}
}

/******************************************************************************
 BTNR_P_TunerAGCMonitor() - update the AGC
******************************************************************************/
void BTNR_P_TunerAGCMonitor(BTNR_3x7x_ChnHandle h)
{
	
	switch (h->pTunerStatus->Tuner_Change_Settings)
	{
	case 0:
		if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial) {BTNR_P_Ods_TunerBiasSelect(h);}
		if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable) {BTNR_P_Ads_TunerBiasSelect(h);}
		BTNR_P_TunerSetExternalFGLNA(h);
	break;
	case 1:
		if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable)
		{
			BTNR_P_Ads_TunerSetTilt(h);
			BTNR_P_Ads_TunerSetRFFILSelect(h);	
			if (h->pTunerParams->BTNR_Internal_Params.RFFIL_Select == BTNR_Internal_Params_TunerRFFIL_eTRKFIL)
			{
				BTNR_P_TunerSetRFFIL(h);
				BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_band_ctrl, h->pTunerStatus->Tuner_RFFIL_band);
				BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_BW_tuning, h->pTunerStatus->Tuner_RFFIL_tune);
			}
			BTNR_P_TunerSetRFFILSwitch(h);
			BTNR_P_Ads_TunerSetBiasSmooth(h);
		}
		if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial)	
		{
			BTNR_P_Ods_TunerSetTilt(h);
			BTNR_P_Ods_TunerSetBiasSmooth(h);
		}
	break;
	case 2:
		if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable)
			BTNR_P_Ads_TunerSetRdegSmooth(h);
		if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial)	
			BTNR_P_Ods_TunerSetRdegSmooth(h);
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerStatus->Tuner_Change_Settings, value received is %d",h->pTunerStatus->Tuner_Change_Settings));
	}
}



/*******************************************************************************************
 * BTNR_P_Calculate_RFout_Gains()	This routine calculates RF to Loopthrough and Daisy Gains	
 *******************************************************************************************/
BERR_Code BTNR_P_Calculate_RFout_Gains(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	uint32_t ReadReg0;
	#ifdef NEW_GAIN_CODE
	int32_t LNA_Gain, LNA_Ctrl_Bias_Correction, LNA_Tilt;
	int32_t idx, x, y1, y2, frequency;
	#else
	int32_t GainReg=0, GainCorrection;
	#endif

	/*Loop Through Gain Calculation*/
	h->pTunerParams->BTNR_Gain_Params.InternalGain_To_LT = 0;

	/*Daisy Gain Calculation*/
	switch (h->pTunerParams->BTNR_Daisy_Params.Daisy_Source)
	{
	case BTNR_Daisy_Source_eUHF:
		h->pTunerParams->BTNR_Gain_Params.InternalGain_To_Daisy = 0;
		break;
	case BTNR_Daisy_Source_eVHF:
		
		/* Get fresh AGC values for status calculations */
		BTNR_P_TunerAGCReadBack(h);
		
		#ifndef NEW_GAIN_CODE
		/*LNA Gain*/
		/*This was derived from the Gain curve excel file from David*/
		/*=(290*Gain-4080-256*Correction)*/
		/*correction is 0 db for 100 MHz, 0.5 db for 200,300,400,500 MHz, 1.5 db for 600,700 MHz, 2.0 db for 800 MHz, 2.5 db for 800,1000 MHz*/ 
		ReadReg0 = (int32_t)h->pTunerStatus->Tuner_LNA_Gain_Code;
		if (h->pTunerStatus->Tuner_RF_Freq < 150000000)
		{
			GainCorrection = 0; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 250000000)
		{
			GainCorrection = -128; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 350000000)
		{
			GainCorrection = -128; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 450000000)
		{
			GainCorrection = -128; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 550000000)
		{
			GainCorrection = -128; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 650000000)
		{
			GainCorrection = -396; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 750000000)
		{
			GainCorrection = -396; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 850000000)
		{
			GainCorrection = -512; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 950000000)
		{
			GainCorrection = -640; /*unit is 256*db*/
		}
		else
		{
			GainCorrection = -640; /*unit is 256*db*/
		}
		GainReg += 256*(int32_t)ReadReg0-2560 + GainCorrection;
		/*printf("%d,", GainReg/256);*/

		
		/*LNA Bias Correction*/
		ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_SPARE_01, i_LNA_ctrl_bias);
		if (ReadReg0 == 1)
		{
		GainReg -= 1024;
		}
		/*printf("%d,", GainReg/256);*/
		
		/*Assign value of Daisy Gain*/
		h->pTunerParams->BTNR_Gain_Params.InternalGain_To_Daisy = GainReg;
		#endif

		#ifdef NEW_GAIN_CODE
		/*LNA Gain*/
		ReadReg0 = (int32_t)h->pTunerStatus->Tuner_LNA_Gain_Code; /*LNA Gain Code*/
		frequency = h->pTunerParams->BTNR_Gain_Params.Frequency;
		idx = (frequency/100000000);	
		switch(idx)
		{
			case 0 : 
				y1 = LNAcode[idx][ReadReg0]; y2 = LNAcode[idx][ReadReg0];
				break;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7: 
				y1 = LNAcode[idx-1][ReadReg0]; y2 = LNAcode[idx][ReadReg0];
				break;
			default:
				y1 = LNAcode[7][ReadReg0]; y2 = LNAcode[7][ReadReg0];
		}
		x = (frequency/100000 - idx*1000);
		LNA_Gain = 256*16 + (y1*1000 + (y2-y1)*x)/(1000);
		
		/*LNA_Bias*/
		ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_SPARE_01, i_LNA_ctrl_bias);
		LNA_Ctrl_Bias_Correction = LNA_Ctrl_Bias[ReadReg0];

		/*LNA_Tilt*/
		ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_tilt);
		LNA_Tilt = (ReadReg0 == 0) ? 0 : -230;  /*unit is 256*db*/
		
		/*Assign value of Daisy Gain*/
		h->pTunerParams->BTNR_Gain_Params.InternalGain_To_Daisy = LNA_Gain + LNA_Ctrl_Bias_Correction + LNA_Tilt ;
		
		BDBG_MSG(("\n\n****** RFout Gain Calculations ******"));
		BDBG_MSG(("Gain_To_Daisy = %d ; LNA_Gain = %d ; LNA_Ctrl_Bias=%d ; LNA_Tilt = %d ", h->pTunerParams->BTNR_Gain_Params.InternalGain_To_Daisy*100/256, LNA_Gain*100/256,LNA_Ctrl_Bias_Correction*100/256,LNA_Tilt*100/256 ));
		BDBG_MSG(("\n---------------------------------"));
		#endif

		break;
	case BTNR_Daisy_Source_eUHF_VHF:
		h->pTunerParams->BTNR_Gain_Params.InternalGain_To_Daisy = 0;
		break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerParams->BTNR_Daisy_Params.Daisy_Source, value received is %d",h->pTunerParams->BTNR_Daisy_Params.Daisy_Source));
		retCode = BERR_INVALID_PARAMETER;
	}

	BDBG_MSG(("BTNR_P_Calculate_RFout_Gains() Complete"));
   return retCode;
}



/******************************************************************************
 BTNR_P_TunerStatus() - get the status structure
******************************************************************************/
BERR_Code BTNR_P_TunerStatus(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	uint32_t ReadReg0, ReadReg1, ReadReg2, ReadReg3;
	int32_t GainReg=0;
	uint8_t PreDivRatio, M, N , ndiv, pdiv;
	uint32_t ulMultA, ulMultB, ulDivisor, ulNrmHi, ulNrmLo;
	uint16_t temp;
	
	#ifdef NEW_GAIN_CODE
	int32_t Tuner_Gain, SDADC_PGA_Gain;
	int32_t LNA_Gain, RFVGA_Gain, MXR_Fga_Gain, FREQ_rsp=0; 
	int32_t LNA_Ctrl_Bias_Correction, RFVGA_Ctrl_Rdeg_Correction, RFVGA_Ctrl_Bias_Correction, TRKFIL_Buf_I_Ctrl_Correction, MIXER_Bias_Ctrl_Correction, FGA_Higain_Correction;
	int32_t LNA_Tilt, RFVGA_Tilt;
	int32_t idx, x, y1, y2, frequency, dx_lut=4096, idx_lut;
	#else
	int32_t GainCorrection;
	#endif

	/* Get fresh AGC values for status calculations */
	BTNR_P_TunerAGCReadBack(h);

	/*RefPLL Lock*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR_REFPLL_04, o_REFPLL_lock);
	h->pTunerStatus->Tuner_Ref_Lock_Status  = (ReadReg0 == 1) ? BTNR_Status_eLock : BTNR_Status_eUnlock;

	/*PhyPLL Lock*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR_REFPLL_04, o_PHYPLL_lock);
	h->pTunerStatus->Tuner_Phy_Lock_Status  = (ReadReg0 == 1) ? BTNR_Status_eLock : BTNR_Status_eUnlock;

	/*MixerPLL Lock*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXRPLL_07, MXRPLL_FREQ_LOCK);
	h->pTunerStatus->Tuner_Mixer_Lock_Status  = (ReadReg0 == 1) ? BTNR_Status_eLock : BTNR_Status_eUnlock;

	/*Ref Freq*/
	h->pTunerStatus->Tuner_Ref_Freq = REF_FREQ;

	/*REFPLL_FREQ = REF_FREQ/REF_OUTDIV_m0*REF_DIV_fb/REF_DIV_ratio = 1350 MHz*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR_REFPLL_01, REF_DIV_fb);    /*REF_DIV_fb*/
	ReadReg1 = BREG_ReadField(h->hRegister, UFE_AFE_TNR_REFPLL_02, REF_DIV_ratio); /*REF_DIV_ratio*/
	ReadReg1 = (ReadReg1 >= 1) ? ReadReg1 : 8+ReadReg1;
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR_REFPLL_03, REF_OUTDIV_m0); /*REF_OUTDIV_m0*/

	h->pTunerStatus->Tuner_RefPll_Freq = (REF_FREQ*ReadReg0)/(ReadReg1*ReadReg2);

	/*PHYPLL1_FREQ=REF_FREQ*(ndiv_int+ndiv_frac/2^24)/p1div]/m1div, m1div is per channel*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, ndiv_int);   /*ndiv_int*/
	ReadReg1 = (ReadReg1 >= 10) ? ReadReg1 : 512+ReadReg1;
	ReadReg1 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_02, ndiv_frac);  /*ndiv_frac*/
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, p1div);      /*p1div*/
	ReadReg2 = (ReadReg2 >= 1) ? ReadReg2 : 16+ReadReg2;
	ReadReg0 = (REF_FREQ*ReadReg0);
	ReadReg1 = (REF_FREQ*ReadReg1)/POWER2_24;
	ReadReg0 = (ReadReg0 + ReadReg1)/ReadReg2;

	/*now for each channels individual divider*/
	/*PHYPLL1_Freq*/
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m1div);      /*m1div*/
	ReadReg2 = (ReadReg2 >= 1) ? ReadReg2 : 256+ReadReg2;
	h->pTunerStatus->Tuner_PhyPll1_Freq = ReadReg0/ReadReg2;
	/*PHYPLL2_Freq*/
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m2div);      /*m2div*/
	ReadReg2 = (ReadReg2 >= 1) ? ReadReg2 : 256+ReadReg2;
	h->pTunerStatus->Tuner_PhyPll2_Freq = ReadReg0/ReadReg2;
	/*PHYPLL3_Freq*/
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m3div);      /*m3div*/
	ReadReg2 = (ReadReg2 >= 1) ? ReadReg2 : 256+ReadReg2;
	h->pTunerStatus->Tuner_PhyPll3_Freq = ReadReg0/ReadReg2;
	/*PHYPLL4_Freq*/
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m4div);      /*m4div*/
	ReadReg2 = (ReadReg2 >= 1) ? ReadReg2 : 256+ReadReg2;
	h->pTunerStatus->Tuner_PhyPll4_Freq = ReadReg0/ReadReg2;
	/*PHYPLL5_Freq*/
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, m5div);      /*m5div*/
	ReadReg2 = (ReadReg2 >= 1) ? ReadReg2 : 256+ReadReg2;
	h->pTunerStatus->Tuner_PhyPll5_Freq = ReadReg0/ReadReg2;
	/*PHYPLL6_Freq*/
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, m6div);      /*m6div*/
	ReadReg2 = (ReadReg2 >= 1) ? ReadReg2 : 256+ReadReg2;
	h->pTunerStatus->Tuner_PhyPll6_Freq = ReadReg0/ReadReg2;


	/* The following is for the callbacak function to ADS*/
	/*To get the RF Frequency we need to get the FCW in the DDFS */
	/*as well as the parameters in the UFE_AFE_TNR0_MXR_01 register*/

	/*Get pre-div ratio*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXR_01, 	i_MIXER_sel_div_ratio); 
	PreDivRatio = 1<<ReadReg0;
	
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXR_01, i_MIXER_HRM_mode);
	ReadReg1 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXR_01, i_MXR_SR6p8p12p16p);
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXR_01, i_MIXER_sel_MUX0p6p8p);

	/*GainReg is used later for callback status*/
	if ((ReadReg0 == 0) && (ReadReg2 == 1))
	{
		M = PreDivRatio * 1;
		GainReg = 6912;
	}
	else if ((ReadReg0 == 1) && (ReadReg1 == 0) && (ReadReg2 == 2))
	{
		M = PreDivRatio * 6;
		GainReg = 6659;  /*unit is 256*db*/
	}
	else if ((ReadReg0 == 1) && (ReadReg1 == 1) && (ReadReg2 == 3))
	{
		M = PreDivRatio * 8;
		GainReg = 6528;  /*unit is 256*db*/
	}
	else if ((ReadReg0 == 1) && (ReadReg1 == 2))
	{
		M = PreDivRatio * 12;
		GainReg = 6528;  /*unit is 256*db*/
	}
	else if ((ReadReg0 == 1) && (ReadReg1 == 3))
	{
		M = PreDivRatio * 16;
		GainReg = 6528;  /*unit is 256*db*/
	}
	else
	{
		BDBG_MSG(("ERROR!!!!! Invalid parameters read fron IC in BTNR_P_TunerStatus()"));
		M = 1;
	}

	N = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXRPLL_03, lP_fbdivn_1p0);

	/*Get FCW*/
	ReadReg0 = BREG_Read32(h->hRegister, BCHP_UFE_AFE_TNR0_DDFS_FCW_02);      /*32 MSB of 37 bit word*/
	ReadReg1 = BREG_Read32(h->hRegister, BCHP_UFE_AFE_TNR0_DDFS_FCW_01);      /* 5 LSB of 37 bit word*/
	ReadReg1 = (ReadReg1 & 0x0000001F);	

	/*Fc = (RFPLL_FREQ*(temp/ndiv/pdiv)/128)*N)*FCW/(M*2^37)*/
	/*Fc = (RFPLL_FREQ*(temp/ndiv/pdiv)/128)*N)/2^8*FCW/(M*2^29)*/
	/*largest N is 24*/
#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR_REFPLL_01, REF_DIV_fb);
	pdiv = 1;
	ndiv = BREG_ReadField(h->hRegister, UFE_AFE_TNR_REFPLL_02, REF_DIV_ratio); 
#elif (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, ndiv_int);
	pdiv = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, p1div);
	ndiv = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PHYPLL_04, m2div);
#endif

	ulMultA = REF_FREQ*(temp/ndiv/pdiv);
	ulMultB = N;
	ulDivisor = 256;
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo,  ulDivisor, &ulNrmHi, &ulNrmLo);

	ulMultA = (ReadReg0 & 0xF8000000) >> 27;
	ulMultB = ((ReadReg0 & 0x07FFFFFF)<<5) | (ReadReg1 & 0x0000001F);
	ulDivisor = M;
	if (ulDivisor == 0)
	{
		BDBG_ERR(("ERROR!!!!! ulDivisor == 0 in BTNR_P_TunerStatus()"));
	}
	/*BMTH_HILO_64TO64_Div32(ulMultA, ulMultB,  ulDivisor, &ulMultA, &ulMultB);*/
	BMTH_HILO_64TO64_Mul(ulMultA, ulMultB, ulNrmHi, ulNrmLo, &ulMultA, &ulMultB);
	BMTH_HILO_64TO64_Div32(ulMultA, ulMultB,  ulDivisor, &ulNrmHi, &ulNrmLo);

	/*Get bits 60:29*/
	ulNrmLo = (ulNrmHi*POWER2_3 & 0xFFFFFFF8) | (ulNrmLo/POWER2_29 & 0x0000007);  

	/*Assign value*/
	h->pTunerStatus->Tuner_RF_Freq = ulNrmLo;
	
	#ifndef NEW_GAIN_CODE
	/***********************************************************/
	/*Get PreADC Gain*/
	/*PreADC Gain = LNA + RFVGA + RFFIL + Mixer_FGA + SDADC_PGA*/
	/*printf("\n\t FGA,SDADC,RFFIl,LNA,RFVGA,LNACorr,RFVGACorr:");*/

	/*Mixer_FGA Gain*/
	/*The GainReg is set above and now must be scaled based on high gain*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXR_02, 	i_FGA_HiGain);
	GainReg = (ReadReg0 == 0) ? GainReg - 768 : GainReg;  /*unit is 256*db*/
	/*printf("%d,", GainReg/256);*/

	/*SDADC_PGA Gain is 0, 2, 4, 6 ... up to 14 db*/
	ReadReg0 = BREG_ReadField(h->hRegister, SDADC_CTRL_SYS0, i_ctl_adc_gain);
	GainReg -= 512*ReadReg0; /*unit is 256*db*/
	/*printf("%d,", GainReg/256);*/

	/*RFFIL Gain is either 0 or 2 db*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_MoCATRAP_swen);
	ReadReg1 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_bypass_en);
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_TRKFIL_BUF);
	ReadReg3 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_TRKFIL);
	
	if (((ReadReg0 == 1) && (ReadReg1 == 0) && (ReadReg2 == 1) && (ReadReg3 == 0)) || ((ReadReg0 == 0) && (ReadReg1 == 1) && (ReadReg2 == 1) && (ReadReg3 == 0)))
	{
		GainReg += 0; /*unit is 256*db*/
	}
	else if ((ReadReg0 == 0) && (ReadReg1 == 0) && (ReadReg2 == 1) && (ReadReg3 == 1))
	{
	  GainReg -= 512; /*unit is 256*db*/
	}
	else
	{
		BDBG_ERR(("ERROR!!!!! INVALID COMBINATION OF VALUES RECEIVED IN RFFIL Gain calculation in BTNR_P_TunerStatus())"));
		GainReg += 0; /*unit is 256*db*/
	}
	/*printf("%d,", GainReg/256);*/

	/*LNA Gain*/
	if (h->pTunerParams->BTNR_RF_Input_Mode != BTNR_3x7x_TunerRfInputMode_eExternalLna) /*Don't add LNA gain for external LNA case*/
		{
		
		/*This was derived from the Gain curve excel file from David*/
		/*=(290*Gain-4080-256*Correction)*/
		/*correction is 0 db for 100 MHz, 0.5 db for 200,300,400,500 MHz, 1.5 db for 600,700 MHz, 2.0 db for 800 MHz, 2.5 db for 800,1000 MHz*/ 
		ReadReg0 = (int32_t)h->pTunerStatus->Tuner_LNA_Gain_Code;
		if (h->pTunerStatus->Tuner_RF_Freq < 150000000)
		{
			GainCorrection = 0; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 250000000)
		{
			GainCorrection = -128; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 350000000)
		{
			GainCorrection = -128; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 450000000)
		{
			GainCorrection = -128; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 550000000)
		{
			GainCorrection = -128; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 650000000)
		{
			GainCorrection = -396; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 750000000)
		{
			GainCorrection = -396; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 850000000)
		{
			GainCorrection = -512; /*unit is 256*db*/
		}
		else if (h->pTunerStatus->Tuner_RF_Freq < 950000000)
		{
			GainCorrection = -640; /*unit is 256*db*/
		}
		else
		{
			GainCorrection = -640; /*unit is 256*db*/
		}
		GainReg += 256*(int32_t)ReadReg0-2560 + GainCorrection;
		/*printf("%d,", GainReg/256);*/
	}
	
	/*RFVGA Gain*/	
	/*This was derived from lab tests*/
	ReadReg0 = h->pTunerStatus->Tuner_RFVGA_Gain_Code;
	ReadReg0 = (ReadReg0>>16) & 0x0000FFFF;
	GainReg += -3840;
	GainReg += (ReadReg0 > 0xD200) ? 256 : 0;
	GainReg += (ReadReg0 > 0xc900) ? 256 : 0;
	GainReg += (ReadReg0 > 0xc100) ? 256 : 0;
	GainReg += (ReadReg0 > 0xb900) ? 256 : 0;
	GainReg += (ReadReg0 > 0xb200) ? 256 : 0;
	GainReg += (ReadReg0 > 0xaa00) ? 256 : 0;
	GainReg += (ReadReg0 > 0xa000) ? 256 : 0;
	GainReg += (ReadReg0 > 0x8f00) ? 256 : 0;
	GainReg += (ReadReg0 > 0x6500) ? 256 : 0;
	GainReg += (ReadReg0 > 0x5c00) ? 256 : 0;
	GainReg += (ReadReg0 > 0x5500) ? 256 : 0;
	GainReg += (ReadReg0 > 0x5000) ? 256 : 0;
	GainReg += (ReadReg0 > 0x4c00) ? 256 : 0;
	GainReg += (ReadReg0 > 0x4800) ? 256 : 0;
	GainReg += (ReadReg0 > 0x4400) ? 256 : 0;
	GainReg += (ReadReg0 > 0x4000) ? 256 : 0;
	GainReg += (ReadReg0 > 0x3c00) ? 256 : 0;
	GainReg += (ReadReg0 > 0x3800) ? 256 : 0;
	GainReg += (ReadReg0 > 0x3400) ? 256 : 0;
	GainReg += (ReadReg0 > 0x3000) ? 256 : 0;
	GainReg += (ReadReg0 > 0x2a00) ? 256 : 0;
	/*printf("%d,", GainReg/256);*/

	if (h->pTunerParams->BTNR_RF_Input_Mode != BTNR_3x7x_TunerRfInputMode_eExternalLna)
	{
		/*LNA Bias Correction*/
		ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_SPARE_01, i_LNA_ctrl_bias);
		if (ReadReg0 == 1)
		{
		GainReg -= 1024;
		}
		/*printf("%d,", GainReg/256);*/
	}

	/*RFVGA Gm Degeneration Correction*/
	/*000 = -0.75 dB
	001 = +2.00 dB
	010 = +3.50 dB
	011 = +5.00 dB
	100 = +6.00 dB
	101 = +7.00 dB
	110 = +7.50 dB*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_rdeg);
	if (ReadReg0 == 0)
	{	GainReg -= 192;	}
	else if (ReadReg0 == 1)
	{	GainReg += 512;	}
	else if (ReadReg0 == 2)
	{	GainReg += 896;	}
	else if (ReadReg0 == 3)
	{	GainReg += 1280;	}
	else if (ReadReg0 == 4)
	{	GainReg += 1536;	}
	else if (ReadReg0 == 5)
	{	GainReg += 1792;	}
	else if (ReadReg0 == 6)
	{	GainReg += 1920;	}
	else if (ReadReg0 == 7)
	{	GainReg += 2048;	}
	else 
	{	GainReg += 2048;	}
	/*printf("%d \n", GainReg/256);*/

	/*Assign value*/	
	h->pTunerStatus->Tuner_PreADC_Gain_x256db = (int16_t)GainReg;
	#endif

	#ifdef NEW_GAIN_CODE
	/***********************************************************/
	/*Get PreADC Gain*/
	/*PreADC Gain = Tuner Gain + SDADC_PGA Gain*/
	/*Tuner_Gain = LNA + RFVGA + Mixer_FGA + Freq_rsp + LNA_Bias + RFVGA_Rdeg + RFVGA_Bias + TRKFIl_Bias + MIXER_Bias + FGA_HiGain + LNA_Tilt + RFVGA_Tilt */
	/*LNA Gain*/
	ReadReg0 = (int32_t)h->pTunerStatus->Tuner_LNA_Gain_Code; /*LNA Gain Code*/
	frequency = h->pTunerStatus->Tuner_RF_Freq;
	idx = (frequency/100000000);	
	switch(idx)
	{
		case 0 : 
			y1 = LNAcode[idx][ReadReg0]; y2 = LNAcode[idx][ReadReg0];
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7: 
			y1 = LNAcode[idx-1][ReadReg0]; y2 = LNAcode[idx][ReadReg0];
			break;
		default:
			y1 = LNAcode[7][ReadReg0]; y2 = LNAcode[7][ReadReg0];
	}
	x = (frequency/100000 - idx*1000);
	LNA_Gain = 256*16 + (y1*1000 + (y2-y1)*x)/(1000);

	/*RFVGA Gain*/
	idx = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_rdeg); /*RFVGA Rdeg*/
	ReadReg0 = h->pTunerStatus->Tuner_RFVGA_Gain_Code; /*RFVGA Gain Code*/
	ReadReg0 = (ReadReg0>>16) & 0x0000FFFF;
	idx_lut = ReadReg0/dx_lut;
	y1 = RFVGAcode[idx][idx_lut]; 
	y2 = RFVGAcode[idx][idx_lut+1];
	RFVGA_Gain = 10*256 + (y1*dx_lut + (y2-y1)*( (int)ReadReg0-idx_lut*dx_lut))/dx_lut;
	
	/*Mixer FGA Gain*/
	MXR_Fga_Gain = 27*256;

	/*Frequency Response (Combined freq. response of all blocks + Mixer Phase comp'n selected based on freq*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_MoCATRAP_swen);
	ReadReg1 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_bypass_en);
	ReadReg2 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_TRKFIL_BUF);
	ReadReg3 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_TRKFIL);
	idx = (frequency/100000000);
	if (((ReadReg0 == 1) && (ReadReg1 == 0) && (ReadReg2 == 1) && (ReadReg3 == 0)) || ((ReadReg0 == 0) && (ReadReg1 == 1) && (ReadReg2 == 1) && (ReadReg3 == 0)))/*Moca Trap*/
	{
		if (idx >0 && idx <8)
		{
			FREQ_rsp = ( FREQ_Rsp_Moca[idx-1]*1000 + (FREQ_Rsp_Moca[idx]-FREQ_Rsp_Moca[idx-1])*(frequency/100000-idx*1000) )/1000;	
		}
		else if (idx == 0)  {FREQ_rsp = FREQ_Rsp_Moca[0];}
		else  {FREQ_rsp = FREQ_Rsp_Moca[7];}
	}
	else if ((ReadReg0 == 0) && (ReadReg1 == 0) && (ReadReg2 == 1) && (ReadReg3 == 1)) /*RF Filt*/
	{
	  	if (idx >0 && idx <8)
		{
			FREQ_rsp = ( FREQ_Rsp_Rffil[idx-1]*1000 + (FREQ_Rsp_Rffil[idx]-FREQ_Rsp_Rffil[idx-1])*(frequency/100000-idx*1000) )/1000;
		}
		else if (idx == 0)  {FREQ_rsp = FREQ_Rsp_Rffil[0];}
		else  {FREQ_rsp = FREQ_Rsp_Rffil[7];}
	}
	else
	{
		BDBG_ERR(("ERROR!!!!! Invalid combination of values received in RFFil Gain calculation in BTNR_P_TunerStatus())"));
	}
	
	/*LNA_Bias*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_SPARE_01, i_LNA_ctrl_bias);
	LNA_Ctrl_Bias_Correction = LNA_Ctrl_Bias[ReadReg0];

	/*RFVGA_Rdeg*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_rdeg);
	RFVGA_Ctrl_Rdeg_Correction = RFVGA_Ctrl_Rdeg[ReadReg0];

	/*RFVGA_Bias*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_bias);
	RFVGA_Ctrl_Bias_Correction = RFVGA_Ctrl_Bias[ReadReg0];

	/*TRKFIl_Bias*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_BUF_I_ctrl);
	TRKFIL_Buf_I_Ctrl_Correction = TRKFIL_Buf_I_Ctrl[ReadReg0];
	
	/*MIXER_Bias*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXR_03, i_MIXER_bias_ctrl);
	MIXER_Bias_Ctrl_Correction = MIXER_Bias_Ctrl[ReadReg0];

	/*FGA_HiGain*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXR_02, i_FGA_HiGain);
	FGA_Higain_Correction = (ReadReg0 == 0) ? -655 : 0;  /*unit is 256*db*/

	/*LNA_Tilt*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_tilt);
	LNA_Tilt = (ReadReg0 == 0) ? 0 : -230;  /*unit is 256*db*/

	/*RFVGA_Tilt*/
	ReadReg0 = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_tilt);
	RFVGA_Tilt = (ReadReg0 == 0) ? 0 : -358;  /*unit is 256*db*/

	/*Assign value for Tuner_Gain*/
	if (h->pTunerParams->BTNR_RF_Input_Mode != BTNR_3x7x_TunerRfInputMode_eExternalLna)
	{	
		Tuner_Gain = LNA_Gain + RFVGA_Gain + MXR_Fga_Gain + FREQ_rsp + LNA_Ctrl_Bias_Correction + RFVGA_Ctrl_Rdeg_Correction + RFVGA_Ctrl_Bias_Correction + TRKFIL_Buf_I_Ctrl_Correction + MIXER_Bias_Ctrl_Correction + FGA_Higain_Correction + LNA_Tilt + RFVGA_Tilt;
		BDBG_MSG(("\n\n******  Gain Calculations ******"));
		BDBG_MSG(("LNA_Gain = %d ; RFVGA_Gain = %d ; MXR_Fga_Gain=%d ; FREQ_rsp = %d ",LNA_Gain*100/256, RFVGA_Gain*100/256, MXR_Fga_Gain*100/256,FREQ_rsp*100/256 ));
		BDBG_MSG(("LNA_Ctrl_Bias = %d ; RFVGA_Ctrl_Rdeg = %d ; RFVGA_Ctrl_Bias=%d ; RFFIL_Buffer_bias = %d ; MIXER_Bias = %d ",LNA_Ctrl_Bias_Correction*100/256,RFVGA_Ctrl_Rdeg_Correction*100/256,RFVGA_Ctrl_Bias_Correction*100/256, TRKFIL_Buf_I_Ctrl_Correction*100/256, MIXER_Bias_Ctrl_Correction*100/256 ));
		BDBG_MSG(("FGA_Higain = %d ; LNA_Tilt = %d ; RFVGA_Tilt=%d ",FGA_Higain_Correction*100/256, LNA_Tilt*100/256, RFVGA_Tilt*100/256 ));
	}
	else
	{
		Tuner_Gain = RFVGA_Gain + MXR_Fga_Gain + FREQ_rsp + RFVGA_Ctrl_Rdeg_Correction + RFVGA_Ctrl_Bias_Correction + TRKFIL_Buf_I_Ctrl_Correction + MIXER_Bias_Ctrl_Correction + FGA_Higain_Correction + RFVGA_Tilt;
		BDBG_MSG(("\n\n******  Gain Calculations ******"));
		BDBG_MSG(("LNA_Gain = %d ; RFVGA_Gain = %d ; MXR_Fga_Gain=%d ; FREQ_rsp = %d ",0, RFVGA_Gain*100/256, MXR_Fga_Gain*100/256,FREQ_rsp*100/256 ));
		BDBG_MSG(("LNA_Ctrl_Bias = %d ; RFVGA_Ctrl_Rdeg = %d ; RFVGA_Ctrl_Bias=%d ; RFFIL_Buffer_bias = %d ; MIXER_Bias_Ctrl = %d ",0,RFVGA_Ctrl_Rdeg_Correction*100/256,RFVGA_Ctrl_Bias_Correction*100/256, TRKFIL_Buf_I_Ctrl_Correction*100/256, MIXER_Bias_Ctrl_Correction*100/256 ));
		BDBG_MSG(("FGA_Higain = %d ; LNA_Tilt = %d ; RFVGA_Tilt=%d ",FGA_Higain_Correction*100/256, 0, RFVGA_Tilt*100/256 ));
	}

	/*Assign value for SDADC_PGA_Gain*/
	/*SDADC_PGA Gain is 0, 2, 4, 6 ... up to 14 db*/
	ReadReg0 = BREG_ReadField(h->hRegister, SDADC_CTRL_SYS0, i_ctl_adc_gain);
	SDADC_PGA_Gain = -512*ReadReg0; 
	
	/*Assign value for PreADC_Gain */	
	h->pTunerStatus->Tuner_PreADC_Gain_x256db = (int16_t)(Tuner_Gain + SDADC_PGA_Gain);

	BDBG_MSG(("Total_PreADC_Gain = %d ; Tuner_Gain = %d ; SDADC_PGA_Gain=%d",h->pTunerStatus->Tuner_PreADC_Gain_x256db*100/256, Tuner_Gain*100/256, SDADC_PGA_Gain*100/256));
	BDBG_MSG(("\n---------------------------------"));
	#endif

	/*Zero internal gain for Low-IF mode */
	if (h->pTunerParams->BTNR_RF_Input_Mode == BTNR_3x7x_TunerRfInputMode_eLowIf) 
	{
	  h->pTunerStatus->Tuner_PreADC_Gain_x256db = 0;
	}


	/***********************************************************/
	/*Get External Gain*/
	/* External gain = External Gain Total - External Gain Bypassable (If LNA bypassed)*/

	/*External Gain Total*/
	h->pTunerStatus->External_Gain_x256db = h->pTunerParams->BTNR_Gain_Params.ExternalGain_Total;

	/*External Gain Bypassable*/
	if (h->pTunerStatus->External_FGLNA_Mode == 0) /* External LNA bypassed - subtract bypassable gain and enable bypassed flag*/
	{
		h->pTunerStatus->External_Gain_x256db -= h->pTunerParams->BTNR_Gain_Params.ExternalGain_Bypassable;   /* in case of Bypass compenaste for the signal Strength */
		h->pTunerParams->BTNR_Gain_Params.Gain_Bypassed = 1; /*Enable the Gain Bypassed flag*/
	}
	else /*External LNA not bypassed - Disable the Gain Bypassed flag*/
	{	
		h->pTunerParams->BTNR_Gain_Params.Gain_Bypassed = 0;
	}
	
	BDBG_MSG(("\nPreADC Gain = %d, ExternalGain = %d\n",h->pTunerStatus->Tuner_PreADC_Gain_x256db*100/256,h->pTunerStatus->External_Gain_x256db*100/256));
	return retCode;
}



/*******************************************************************************************
 * BTNR_P_TunerInit()		This routine initializes the tuner and is only run once
 *******************************************************************************************/
BERR_Code BTNR_P_TunerInit(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	uint32_t temp_UFE_AFE_TNR0_PWRUP_01 = 0;
	uint32_t temp_UFE_AFE_TNR0_PWRUP_02 = 0;
	/* local variables */
	uint8_t PLL_Status, LockCount, temp;

	/*Initialize the BTNR_Internal_Params_t Structure
		 *these parameters are used locally by BBS to sent parameters into the tuner functions*/
	h->pTunerParams->BTNR_Internal_Params.LNA_Enable       = INIT_BBS_LNA_ENABLE;
	h->pTunerParams->BTNR_Internal_Params.RFFIL_Select     = INIT_BBS_RFFIL_SELECT;
	h->pTunerParams->BTNR_Internal_Params.HRC_Enable       = INIT_BBS_HRC_ENABLE;
	if (h->pTunerParams->BTNR_RF_Input_Mode == BTNR_3x7x_TunerRfInputMode_eLowIf) 
	{
	  h->pTunerParams->BTNR_Internal_Params.SDADC_Input    = BTNR_Internal_Params_SDADC_Input_eExtReal;
    h->pTunerParams->BTNR_Internal_Params.IF_Freq        = h->pTunerParams->BTNR_Acquire_Params.RF_Freq;   
	} else {
		h->pTunerParams->BTNR_Internal_Params.SDADC_Input    = INIT_BBS_SDADC_INPUT;
	  h->pTunerParams->BTNR_Internal_Params.IF_Freq        = INIT_BBS_IF_FREQ;	  
	}
	/*Initialize the BTNR_Local_Params_t Structure*/
	h->pTunerParams->BTNR_Local_Params.TunerCapCntl				 = 0;
	h->pTunerParams->BTNR_Local_Params.RF_Offset         = 0; 
	h->pTunerParams->BTNR_Local_Params.Symbol_Rate         = 0;
	h->pTunerParams->BTNR_Local_Params.Total_Mix_After_ADC = (int16_t)0x8000;
	h->pTunerParams->BTNR_Local_Params.PostADC_Gain_x256db = (int16_t)0x8000;

	/* clear the lock status */
	h->pTunerStatus->Tuner_Ref_Lock_Status = BTNR_Status_eUnlock;
	h->pTunerStatus->Tuner_Mixer_Lock_Status = BTNR_Status_eUnlock;
	h->pTunerStatus->Tuner_Phy_Lock_Status = BTNR_Status_eUnlock;

	/*Get the REFPLL lock status*/
	PLL_Status = 0;
	LockCount = 0;
	while ((PLL_Status == 0) && (LockCount < REF_PLL_LOCK_TIMEOUT_MS))
	{
		BKNI_Sleep(1);
		PLL_Status = BREG_ReadField(h->hRegister,UFE_AFE_TNR_REFPLL_04, o_REFPLL_lock);
		switch (PLL_Status)
		{
		case 0 :
			if (BREG_ReadField(h->hRegister, UFE_AFE_TNR_PWRUP_01, REF_PLL_master_PWRUP) == 0)
				BDBG_MSG(("REFPLL is NOT used"));
			else
				BDBG_ERR(("REFPLL is NOT locked"));
			break;

		case 1 :
			BDBG_MSG(("REFPLL is locked"));
			break;
		default :
			BDBG_ERR(("ERROR!!! INVALID Reference PLL Locked Value: value is %d",PLL_Status));
			retCode = BERR_INVALID_PARAMETER;
			/*goto bottom of function to return error code*/
			goto something_bad_happened;
		}
		LockCount++;
	}

	/*Get the PHYPLL lock status*/
	PLL_Status = 0;
	LockCount = 0;
	while ((PLL_Status == 0) && (LockCount <PHY_PLL_LOCK_TIMEOUT_MS))
	{
		BKNI_Sleep(1);
		/*Get the PHYPLL lock status*/
		PLL_Status = BREG_ReadField(h->hRegister,UFE_AFE_TNR_REFPLL_04, o_PHYPLL_lock);
		switch (PLL_Status)
		{
		case 0 :
			temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, PHY_PLL_master_PWRUP);
			if (temp == 0)
				BDBG_ERR(("PHYPLL is NOT used"));
			else
				BDBG_ERR(("PHYPLL is NOT locked"));
			break;
		case 1 :
			BDBG_MSG(("PHYPLL is locked"));
			break;
		default :
			BDBG_ERR(("ERROR!!! INVALID PHY PLL Locked Value: value is %d",PLL_Status));
			retCode = BERR_INVALID_PARAMETER;
			/*goto bottom of function to return error code*/
			goto something_bad_happened;
		}
		LockCount++;
	}
	temp_UFE_AFE_TNR0_PWRUP_01 = BREG_Read32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_01);
	temp_UFE_AFE_TNR0_PWRUP_02 = BREG_Read32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02);
	temp_UFE_AFE_TNR0_PWRUP_01 = (temp_UFE_AFE_TNR0_PWRUP_01 | 0x000083E0);
	temp_UFE_AFE_TNR0_PWRUP_02 = (temp_UFE_AFE_TNR0_PWRUP_02 | 0x00087AFF);
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_01, temp_UFE_AFE_TNR0_PWRUP_01);
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_PWRUP_02, temp_UFE_AFE_TNR0_PWRUP_02);
	BKNI_Sleep(1);
	BTNR_P_TunerSetDCO(h);	
	h->pTunerStatus->AGC_BW = 1; 
	if (h->pTunerParams->BTNR_Acquire_Params.Application  == BTNR_TunerApplicationMode_eTerrestrial) 	{ 
		BTNR_P_Ods_TunerSetAGCTOP(h);  
		BTNR_P_Ods_TunerSetAGCBW(h);
	}
	if (h->pTunerParams->BTNR_Acquire_Params.Application  == BTNR_TunerApplicationMode_eCable) 	{
		BTNR_P_Ads_TunerSetAGCTOP(h); 
		BTNR_P_Ads_TunerSetAGCBW(h);
	}
	/*Initialization Complete*/
	BDBG_MSG(("BTNR_P_TunerInit() Complete"));

/*goto label to return error code if something bad happened above*/
something_bad_happened:
  return retCode;
}

#ifdef SmartTuneEnabled
/*******************************************************************************************
 * BTNR_P_TunerSmartTune()
 *******************************************************************************************/
static void BTNR_P_TunerSmartTune(BTNR_3x7x_ChnHandle h)
{
  bool ChangeFreqPlan = false;
  uint32_t RF_Freq = h->pTunerParams->BTNR_Acquire_Params.RF_Freq;
  if (((RF_Freq > (198000000-5000000)) && (RF_Freq < (198000000+5000000))) ||
      ((RF_Freq > (216000000-5000000)) && (RF_Freq < (216000000+5000000))) ||
	  ((RF_Freq > (506000000-5000000)) && (RF_Freq < (506000000+5000000))) ||
      ((RF_Freq > (540000000-5000000)) && (RF_Freq < (540000000+5000000))) ||
      ((RF_Freq > (675000000-5000000)) && (RF_Freq < (675000000+5000000))) || 
	  ((RF_Freq > (756000000-5000000)) && (RF_Freq < (756000000+5000000))) || 
      ((RF_Freq > (844000000-5000000)) && (RF_Freq < (844000000+5000000)))) {
    if (h->pTunerParams->BTNR_Local_Params.SmartTune != BTNR_Local_Params_SmartTune_FreqPlanA) {
      h->pTunerParams->BTNR_Local_Params.SmartTune = BTNR_Local_Params_SmartTune_FreqPlanA;
      ChangeFreqPlan = true;   
    }
  } else {
    if (h->pTunerParams->BTNR_Local_Params.SmartTune != BTNR_Local_Params_SmartTune_FreqPlanDefault) {
      h->pTunerParams->BTNR_Local_Params.SmartTune = BTNR_Local_Params_SmartTune_FreqPlanDefault;
      ChangeFreqPlan = true; 
    }    
  }
  
  if (ChangeFreqPlan) {
	/*BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, PHYPLL_dreset, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, PHYPLL_areset, 1);*/
  	switch (h->pTunerParams->BTNR_Local_Params.SmartTune) {
	  case BTNR_Local_Params_SmartTune_FreqPlanDefault :
	    BDBG_MSG(("SmartTune: Frequency Plan = Default"));
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, ndiv_int, 0x78);/*ndiv_int = 50*/			
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, m6div, 0x04);   /*m6div = 5*/
		
        /* for multi channel tuner, TM_SYS_PLL regs are offset (from channel 0's base) using hTMSysPllReg */
#if ( BTNR_P_BCHP_TNR_NUM_CORES < 2 ) /* (e.g. not dual tuner [3472]) */
        BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL_PDIV, 2);	
        BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL_NDIV_INT, 80);
#else
        BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL0_PDIV, 2);	
        BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL0_NDIV_INT, 80);
#endif
        /* TM_SYS_PLL_CLK_XPT_216 not channelized */
#if (BTNR_P_BCHP_TNR_CORE_VER < BTNR_P_BCHP_CORE_V_1_3)
  	    BREG_WriteField(h->hTnr->hRegister, TM_SYS_PLL_CLK_216, DIV, 10);			
#else
        BREG_WriteField(h->hTnr->hRegister, TM_REG_PLL_CLK_XPT_216, DIV, 10);			
#endif
		break;
	  case BTNR_Local_Params_SmartTune_FreqPlanA :
		BDBG_MSG(("SmartTune: Frequency Plan = A"));
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_03, ndiv_int, 114);/*ndiv_int = 38*/
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, m6div, 4);    /*m6div = 4*/
		
        /* for multi channel tuner, TM_SYS_PLL regs are offset (from channel 0's base) using hTMSysPllReg */
#if ( BTNR_P_BCHP_TNR_NUM_CORES < 2 ) /* (e.g. not dual tuner [3472]) */
        BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL_PDIV, 2);	
        BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL_NDIV_INT, 76);
#else
        BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL0_PDIV, 2);	
        BREG_Write32(h->hTMSysPllReg, BCHP_TM_SYS_PLL0_NDIV_INT, 76);
#endif
        /* TM_SYS_PLL_CLK_XPT_216 not channelized */
#if (BTNR_P_BCHP_TNR_CORE_VER < BTNR_P_BCHP_CORE_V_1_3)
  	    BREG_WriteField(h->hTnr->hRegister, TM_SYS_PLL_CLK_216, DIV, 10);			
#else
        BREG_WriteField(h->hTnr->hRegister, TM_REG_PLL_CLK_XPT_216, DIV, 10);			
#endif
		break;
	  default :
		BDBG_ERR(("SmartTune: Frequency Plan Invalid"));
  	}
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, load_en_ch6_ch1, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, load_en_ch6_ch1, 0x3f);
	/*BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, PHYPLL_dreset, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, PHYPLL_areset, 0);  */
	BTNR_P_TunerSetADC6B(h);	
  }
}
#endif

/*******************************************************************************************************************
 * BTNR_P_TunerSetSignalPath()  This routine sets the tuner for TOP, tilt, RFFIL mode, RFFIL switches, bias
 ******************************************************************************************************************/
void BTNR_P_TunerSetSignalPath(BTNR_3x7x_ChnHandle h)
{	
	if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable)
	{
		BTNR_P_Ads_TunerSetTilt(h);
		BTNR_P_Ads_TunerSetRFFILSelect(h);	
	}
	if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial)
	{
		BTNR_P_Ods_TunerSetTilt(h);
		BTNR_P_Ods_TunerSetRFFILSelect(h);	
	}
	if (h->pTunerParams->BTNR_Internal_Params.RFFIL_Select == BTNR_Internal_Params_TunerRFFIL_eTRKFIL)
	{
		BTNR_P_TunerSetRFFIL(h); /* set bandwidth of RF tracking filter */
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_band_ctrl, h->pTunerStatus->Tuner_RFFIL_band);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_BW_tuning, h->pTunerStatus->Tuner_RFFIL_tune);
	}
	BTNR_P_TunerSetRFFILSwitch(h);	/* set RFFIL switches */
	if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial)	
		BTNR_P_Ods_TunerSetBias(h);	/* set tuner bias */
	if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable)	
		BTNR_P_Ads_TunerSetBias(h);	/* set tuner bias */
	h->pTunerStatus->Tuner_Change_Settings = 0;
	BDBG_MSG(("BTNR_P_TunerSetSignalPath() Complete"));
	BDBG_MSG(("Tuner Bias Select = %d",h->pTunerParams->BTNR_Internal_Params.Bias_Select));
	BDBG_MSG(("Tuner RFFIL Select = %d",h->pTunerParams->BTNR_Internal_Params.RFFIL_Select));
}

/*******************************************************************************************
 * BTNR_P_TunerTune()	This routine tunes the tuner
 *******************************************************************************************/
BERR_Code BTNR_P_TunerTune(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	uint32_t	temp, Freq_pre=0, Freq_post=0;
	h->pTunerParams->BTNR_Internal_Params.Bias_Select = BTNR_Internal_Params_TunerBias_eACI;
	BTNR_P_TunerSetSignalPath(h);	

#ifdef SmartTuneEnabled
	if ((h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial) && (BCHP_CHIP != 3472))
	{
		BTNR_P_TunerSmartTune(h);
	}
#endif

	/*set the tuner input pads for testing*/
	if (h->pTunerParams->BTNR_Internal_Params.SDADC_Input != BTNR_Internal_Params_SDADC_Input_eTuner)
	{
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_pad_en, 1); /*enable pad switch*/
	}
	else
	{
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_pad_en, 0); /*disable pad switch*/
	}
	/*set DCO*/
	if (h->pTunerStatus->DCO_ON == 0) {BTNR_P_TunerSetDCO(h);}
	if ((h->pTunerParams->BTNR_Acquire_Params.RF_Freq > (100000000-5000000)) && (h->pTunerParams->BTNR_Acquire_Params.RF_Freq < (100000000+5000000))) {BTNR_P_TunerSetDCODOWN(h);}
	/*Main Tuning*/
	switch (h->pTunerParams->BTNR_RF_Input_Mode)
	{
	case BTNR_3x7x_TunerRfInputMode_eExternalLna:
	case BTNR_3x7x_TunerRfInputMode_eInternalLna:
	case BTNR_3x7x_TunerRfInputMode_eStandardIf:
		temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXRPLL_07, MXRPLL_FREQ_LOCK);
		Freq_pre = (h->pTunerStatus->Tuner_RF_Freq); 
		Freq_post = (h->pTunerParams->BTNR_Acquire_Params.RF_Freq);
		if ((h->pTunerParams->BTNR_TuneType.TuneType == BTNR_TuneType_eMiniTune) || (((((Freq_pre - Freq_post)*(Freq_pre - Freq_post)) <= 0x10) && (h->pTunerParams->BTNR_TuneType.TuneType != BTNR_TuneType_eInitTune)) && (temp ==1)))
		{
			BTNR_P_TunerSetFGA_IFLPF(h);	
		}
		else
		{
			BTNR_P_TunerSetFGA_IFLPF(h);
			BTNR_P_TunerSetFreq(h);			
		}
	break;
	case BTNR_3x7x_TunerRfInputMode_eLowIf:
	case BTNR_3x7x_TunerRfInputMode_eBaseband:
	case BTNR_3x7x_TunerRfInputMode_eOff:
	break;
	default:
		BDBG_ERR(("ERROR!!! h->pTunerParams->BTNR_RF_Input_Mode, value received is %d",h->pTunerParams->BTNR_RF_Input_Mode));
		retCode = BERR_INVALID_PARAMETER;
		/*goto bottom of function to return error code*/
		goto something_bad_happened;
	}
	if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable) {if (h->pTunerParams->BTNR_TuneType.TuneType == BTNR_TuneType_eInitTune) {BTNR_P_Ads_TunerShiftGearAGC(h);} BTNR_P_Ads_TunerBiasSelect(h);}
	if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial) {if (((Freq_pre - Freq_post)*(Freq_pre - Freq_post)) > 0x10) {BTNR_P_Ods_TunerShiftGearAGC(h);} BTNR_P_Ods_TunerBiasSelect(h);}
	BTNR_P_TunerSetSignalPath(h); 
	BDBG_MSG(("BTNR_P_TunerTune() Complete"));	
/*goto label to return error code if something bad happened above*/
something_bad_happened:
  return retCode;
}





/*******************************************************************************************
 * BTNR_P_TunerSetRFFIL()    This routine selects the correct RF filter corner frequency
 *******************************************************************************************/
void BTNR_P_TunerSetRFFIL(BTNR_3x7x_ChnHandle h)
{
	/*local variables*/
	uint8_t i;
	uint8_t j;
	uint8_t RFTRK_band_ctrl=0;
	uint8_t RFTRK_tune_ctrl=0;
	/*sequence through the LPF_Selection_Table[] from highest freq value to lowest freq value*/
	/*table is in 100 KHz resolution*/
	/*when we get to the entry with a lower freq then the TunerFreq record index and rftrk1 then exit and program*/
	i = 0;
	j = 0;
	if (h->pTunerParams->BTNR_Acquire_Params.RF_Freq <= 82000000 )
	{
	RFTRK_band_ctrl = 0;
	RFTRK_tune_ctrl = 17;
	}
	else if ((h->pTunerParams->BTNR_Acquire_Params.RF_Freq > 82000000) && (h->pTunerParams->BTNR_Acquire_Params.RF_Freq <= 143160000))
	{
	RFTRK_band_ctrl = 0;
		for (i=0;i<11;i++)
		{
			if (h->pTunerParams->BTNR_Acquire_Params.RF_Freq <= (uint32_t)((82+i*6)*1000000)+1160000)
			{
				RFTRK_tune_ctrl = LPF_Selection_Table[i];
				break;
			}
		}
	}
	else if ((h->pTunerParams->BTNR_Acquire_Params.RF_Freq > 143160000) && (h->pTunerParams->BTNR_Acquire_Params.RF_Freq <= 262000000))
	{
	RFTRK_band_ctrl = 1;
		for (i=11;i<31;i++)
		{
			if (h->pTunerParams->BTNR_Acquire_Params.RF_Freq <= (uint32_t)((82+i*6)*1000000))
			{
				RFTRK_tune_ctrl = LPF_Selection_Table[i];
				break;
			}
		}	
	}
	else if ((h->pTunerParams->BTNR_Acquire_Params.RF_Freq > 262000000) && (h->pTunerParams->BTNR_Acquire_Params.RF_Freq <= 502000000))
	{
	RFTRK_band_ctrl = 2;
		for (i=31;i<71;i++)
		{
			if (h->pTunerParams->BTNR_Acquire_Params.RF_Freq <= (uint32_t)((82+i*6)*1000000))
			{
				RFTRK_tune_ctrl = LPF_Selection_Table[i];
				break;
			}
		}		
	}	
	else
	{
		BDBG_ERR(("ERROR!!! h->pTunerParams->BTNR_Acquire_Params.RF_Freq, value received is %d",h->pTunerParams->BTNR_Acquire_Params.RF_Freq));
	}	
	h->pTunerStatus->Tuner_RFFIL_band = RFTRK_band_ctrl;
	h->pTunerStatus->Tuner_RFFIL_tune = RFTRK_tune_ctrl;
	BDBG_MSG(("BTNR_P_TunerSetRFFIL() Complete"));
}

/*******************************************************************************************
 * BTNR_P_TunerSetFGA_IFLPF()	This routine it to set the IF filter in the tuner
 *******************************************************************************************/
void BTNR_P_TunerSetFGA_IFLPF(BTNR_3x7x_ChnHandle h)
{
	/*local variables*/
	uint8_t	FGA_GAIN;
	uint8_t FGA_RC_Ctrl;
	uint8_t IFLPF_BW_Sel;
	uint8_t	IFLPF_WBW_Sel;

	/*Set FGA gain to high gain for terrestrial, low gain for cable*/
	if (h->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable)
	{
		BREG_WriteField(h->hRegister,UFE_AFE_TNR0_MXR_02, i_FGA_HiGain, 0);
		FGA_GAIN = 0;
	}
	{
		BREG_WriteField(h->hRegister,UFE_AFE_TNR0_MXR_02, i_FGA_HiGain, 1);
		FGA_GAIN = 1;
	}

	/*lookup the IFLPF_BWR_Sel and IFLPF_WBW_Sel values, they are different for each bandwidth 2,4,5,8,10 MHz */
	switch (h->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth)
	{
	case BTNR_LPF_Bandwidth_e8MHz :
		FGA_RC_Ctrl = (FGA_GAIN == 1) ? FGA_RC_CTRL_HIGHG_8MHz : FGA_RC_CTRL_LOWG_8MHz;
		IFLPF_BW_Sel  = IFLPF_BW_SEL_8MHz;
		IFLPF_WBW_Sel = IFLPF_WBW_SEL_8MHz;
		break;
	case BTNR_LPF_Bandwidth_e7MHz :
		FGA_RC_Ctrl = (FGA_GAIN == 1) ? FGA_RC_CTRL_HIGHG_7MHz : FGA_RC_CTRL_LOWG_7MHz;
		IFLPF_BW_Sel  = IFLPF_BW_SEL_7MHz;
		IFLPF_WBW_Sel = IFLPF_WBW_SEL_7MHz;
		break;
	case BTNR_LPF_Bandwidth_e6MHz :
		FGA_RC_Ctrl = (FGA_GAIN == 1) ? FGA_RC_CTRL_HIGHG_6MHz : FGA_RC_CTRL_LOWG_6MHz;
		IFLPF_BW_Sel  = IFLPF_BW_SEL_6MHz;
		IFLPF_WBW_Sel = IFLPF_WBW_SEL_6MHz;
		break;
	case BTNR_LPF_Bandwidth_e5MHz :
		FGA_RC_Ctrl = (FGA_GAIN == 1) ? FGA_RC_CTRL_HIGHG_5MHz : FGA_RC_CTRL_LOWG_5MHz;
		IFLPF_BW_Sel  = IFLPF_BW_SEL_5MHz;
		IFLPF_WBW_Sel = IFLPF_WBW_SEL_5MHz;
		break;
	case BTNR_LPF_Bandwidth_e1_7MHz :
		FGA_RC_Ctrl = (FGA_GAIN == 1) ? FGA_RC_CTRL_HIGHG_1_7MHz : FGA_RC_CTRL_LOWG_1_7MHz;;
		IFLPF_BW_Sel  = IFLPF_BW_SEL_1_7MHz;
		IFLPF_WBW_Sel = IFLPF_WBW_SEL_1_7MHz;
		break;
	case BTNR_LPF_Bandwidth_eVariable :
		if ((h->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth > MAX_LPF_VARIABLE_BW) ||
			  (h->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth < MIN_LPF_VARIABLE_BW))
		{
			FGA_RC_Ctrl = (FGA_GAIN == 1) ? FGA_RC_CTRL_HIGHG_8MHz : FGA_RC_CTRL_LOWG_8MHz;
			IFLPF_BW_Sel  = IFLPF_BW_SEL_8MHz;
			IFLPF_WBW_Sel = IFLPF_WBW_SEL_8MHz;
			BDBG_ERR(("ERROR!!!  VARIABLE LPF BANDWIDTH UNSUPPORTED SETTING TO 8 MHZ"));
			BDBG_ERR(("ERROR!!! Invalid Tuner_Variable_BW in BTNR_P_TunerSetFGA_IFLPF() , Value is %d", h->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth));
		}
		else
		{
			FGA_RC_Ctrl = (FGA_GAIN == 1) ? FGA_RC_CTRL_HIGHG_8MHz : FGA_RC_CTRL_LOWG_8MHz;
			IFLPF_BW_Sel  = IFLPF_BW_SEL_8MHz;
			IFLPF_WBW_Sel = IFLPF_WBW_SEL_8MHz;
		}
		break;
	default :
		FGA_RC_Ctrl = (FGA_GAIN == 1) ? FGA_RC_CTRL_HIGHG_8MHz : FGA_RC_CTRL_LOWG_8MHz;
		IFLPF_BW_Sel  = IFLPF_BW_SEL_8MHz;
		IFLPF_WBW_Sel = IFLPF_WBW_SEL_8MHz;
		BDBG_ERR(("ERROR!!! Invalid Tuner_BW selected in BTNR_P_TunerSetFGA_IFLPF() , Value is %d", h->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth));
		break;
	}

	/*write the bandwidth values*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_02, i_FGA_C_ctrl, FGA_RC_Ctrl);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_BW, IFLPF_BW_Sel);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_WBW, IFLPF_WBW_Sel);

	BDBG_MSG(("LPF_Bandwidth = %d", h->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth));
	BDBG_MSG(("LPF_Variable_Bandwidth = %d", h->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth));
	BDBG_MSG(("i_FGA_C_ctrl = %d", FGA_RC_Ctrl));
	BDBG_MSG(("i_LPF_BW = %d", IFLPF_BW_Sel));
	BDBG_MSG(("i_LPF_WBW = %d", IFLPF_WBW_Sel));
	BDBG_MSG(("BTNR_P_TunerSetFGA_IFLPF() Complete"));
}


/******************************************************************************
 BTNR_P_DPMSetFreq()
******************************************************************************/
void BTNR_P_DPMSetFreq(BTNR_3x7x_ChnHandle h)
{
	/*local variables*/
	uint16_t temp;

	/* power up DPM */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DPM_LOBUF_REG, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DPM, 0x1);
	/* Enable DPM fx clock */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, m5div, 0x20);
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch);
	temp = temp | 0x10 ;
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, temp);
	/* Enable divider change on the fly */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, load_en_ch6_ch1, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PHYPLL_05, load_en_ch6_ch1, 0x3f);
	/*Set LO*/
	BTNR_P_TunerSetLO(h);
	/* DPM Settings */
	/* DPM Amplitude Settings */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, DPM_6dB, 0x1); /*enabling 6dB boost*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, DPM_amp0, 0x1); /*setting the output power LSB*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, DPM_amp1, 0x1); /*setting the output power*/ 
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, DPM_amp2, 0x1); /*setting the output power MSB*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, sel_I, 0x1); /*Setting the mode for upconversion/downconversion. Toggle this bit to change the mode.*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, sel_Q, 0x0); /*Setting the mode for upconversion/downconversion*/
	/* LOGen Settings */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, i_logen_I_Qneg, 0x0);
	/* assert/de-assert DPM reset/resetb */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, i_logen_reset, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, i_logen_start, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, i_async_reset, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, reset_1p0, 0x0);
	BKNI_Delay(1); /*1 usec*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, i_logen_reset, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, i_logen_start, 0x0);
	BKNI_Delay(2); /*2 usec*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, i_logen_reset, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, i_logen_start, 0x1);
	BKNI_Delay(1); /*1 usec*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, reset_1p0, 0x1);
	BKNI_Delay(2); /*2 usec*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, reset_1p0, 0x0);
	/*enable DPM out*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, i_DPM_switch, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DAISY_UHF, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DS_01, i_DAISY_UHF_DPM_sel, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_DPM_01, i_atten, 0x1);
	BKNI_Sleep(1);
	/*MXR SR, pre_div reset*/
	BTNR_P_TunerSetMXR(h);
	/*MoCA trap*/
	h->pTunerParams->BTNR_Internal_Params.RFFIL_Select = BTNR_Internal_Params_TunerRFFIL_eMOCATRAP;
	BTNR_P_TunerSetRFFILSwitch(h);
	BDBG_MSG(("BTNR_P_DPMSetFreq() Complete"));
}

/******************************************************************************
 BTNR_P_TunerSetFreq()
******************************************************************************/
void BTNR_P_TunerSetFreq(BTNR_3x7x_ChnHandle h)
{
	/*Set LO*/
	BTNR_P_TunerSetLO(h);
	/*MXR SR, pre_div reset*/
	BTNR_P_TunerSetMXR(h);
	BDBG_MSG(("BTNR_P_TunerSetFreq() Complete"));
}



/*******************************************************************************************************************
 * BTNR_P_TunerSetADC6B()  This routine sets the tuner 6-bit ADC
 ******************************************************************************************************************/
void BTNR_P_TunerSetADC6B(BTNR_3x7x_ChnHandle h)
{
	uint16_t temp;

	/* Powerup Channel 1 */
	temp = BREG_ReadField(h->hRegister,UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch);
	temp = temp | 0x1 ;
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, temp);
	BKNI_Delay(20); /*20 usec*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_ADC6B, 0x1);
	BKNI_Delay(20); /*20 usec*/	
	/* De-assert reset/resetb for 6-bit ADC */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, ADC6B_resetb, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, ADC6B_resetb, 1);
}

/*******************************************************************************************************************
 * BTNR_P_TunerSetADC6BDOWN()  This routine power down the tuner 6-bit ADC
 ******************************************************************************************************************/
void BTNR_P_TunerSetADC6BDOWN(BTNR_3x7x_ChnHandle h)
{
	uint16_t temp;

	/* Power down Channel 1 & ADC6B*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_ADC6B, 0x0);
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch);
	temp = temp & 0x3E ;
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, temp);
}

/*******************************************************************************************************************
 * BTNR_P_TunerSetLNAAGC()  This routine sets the tuner LNA AGC
 ******************************************************************************************************************/
void BTNR_P_TunerSetLNAAGC(BTNR_3x7x_ChnHandle h)
{
	uint32_t ReadReg0;
	/*BERR_Code retCode = BERR_SUCCESS;*/

	/* negative edge */
	ReadReg0 = BREG_Read32(h->hRegister, BCHP_UFE_AFE_TNR0_LNAAGC_02);
	BREG_Write32(h->hRegister, BCHP_UFE_AFE_TNR0_LNAAGC_02, (ReadReg0 | 0x80000000));
	/* De-assert reset/resetb for LNA AGC */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, LNAAGC_resetb, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, LNAAGC_resetb, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, i_LNAAGC_dsm_srst, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, i_LNAAGC_dsm_srst, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, i_LNAAGC_agc_srst, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, i_LNAAGC_agc_srst, 0);

	/*set initial gain*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_01, init_LNA_gain, 15);

	/*unfreeze, un-bypass*/   
    BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_03, AGC_byp, 0);
    BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_03, AGC_frz, 0);
	/*start LNA AGC*/ 
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_00, lna_start, 0x0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_00, lna_start, 0x1);

	BDBG_MSG(("BTNR_P_TunerSetLNAAGC() Complete"));
}


/*******************************************************************************************************************
 * BTNR_P_TunerSetDCO()  This routine sets the tuner DCO
 ******************************************************************************************************************/
void BTNR_P_TunerSetDCO(BTNR_3x7x_ChnHandle h)
{
	uint8_t temp;

	/*Adjust LDO settings*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_REG_01, i_REG1p0_ctrl, 3);	
	/*Enable DCO Clock*/
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch);
	temp = temp | 0x8 ;
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, temp);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DCO, 1);
	/* De-assert reset/resetb for DCO */
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, i_rstb, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, i_dco_rstb, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, i_rstb, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, i_dco_rstb, 1);
	/*gear shifting*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRDCO_01, i_dco_k0, 16);
	BKNI_Sleep(10);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXRDCO_01, i_dco_k0, 8);
	BKNI_Sleep(10);
	h->pTunerStatus->DCO_ON = 1;
	BDBG_MSG(("DCO is running"));
}


/*******************************************************************************************************************
 * BTNR_P_TunerSetDCODOWN()  This routine power down the tuner DCO
 ******************************************************************************************************************/
void BTNR_P_TunerSetDCODOWN(BTNR_3x7x_ChnHandle h)
{
	uint8_t temp;
	
	/*Power Down DCO Clock*/
	temp = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch);
	temp = temp & 0x37 ;
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_02, i_pwrup_PHYPLL_ch, temp);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_pwrup_DCO, 0);
	h->pTunerStatus->DCO_ON = 0;
	BDBG_MSG(("DCO is down"));
}

/******************************************************************************
 BTNR_P_CalFlashSDADC() - calibrate SD ADC flash
******************************************************************************/
void BTNR_P_CalFlashSDADC(BTNR_3x7x_ChnHandle h)
{
	uint8_t idx;
	uint32_t ICalOffset, QCalOffset;
	uint8_t statusIch, statusQch=0;

	/*I Channel Calibration*/
	BREG_WriteField(h->hRegister, SDADC_CTRL_ICH, i_Ich_flash_resetCal, 0x1);
	BREG_WriteField(h->hRegister, SDADC_CTRL_RESET, i_Ich_reset, 0x1);
	BREG_WriteField(h->hRegister, SDADC_CTRL_ICH, i_Ich_flash_resetCal, 0x0);
	BREG_WriteField(h->hRegister, SDADC_CTRL_ICH, i_Ich_flash_cal_on, 0x1);

	/*Q Channel Calibration*/
    BREG_WriteField(h->hRegister, SDADC_CTRL_QCH, i_Qch_flash_resetCal, 0x1);
	BREG_WriteField(h->hRegister, SDADC_CTRL_RESET, i_Qch_reset, 0x1);
    BREG_WriteField(h->hRegister, SDADC_CTRL_QCH, i_Qch_flash_resetCal, 0x0);
    BREG_WriteField(h->hRegister, SDADC_CTRL_QCH, i_Qch_flash_cal_on, 0x1);

	for (statusIch=0, idx =0; (idx<2) && ( !(statusIch) || !(statusQch) ) ;idx++){
		BKNI_Sleep(1);
		#if (BTNR_P_BCHP_TNR_CORE_VER <= BTNR_P_BCHP_CORE_V_1_1) 
			statusIch = BREG_ReadField(h->hRegister,SDADC_STATUS_ICH, o_Ich_flash_cal_done);
			statusQch = BREG_ReadField(h->hRegister,SDADC_STATUS_QCH, o_Qch_flash_cal_done);
		#else
			statusIch = BREG_ReadField(h->hRegister,SDADC_STATUS_ICH, o_ch_master_done);
			statusQch = BREG_ReadField(h->hRegister,SDADC_STATUS_QCH, o_ch_master_done);		
		#endif
	}
	
	switch (statusIch)
	{
	case 0 : BDBG_MSG(("SDADC I channel calibration NOT done"));
			 if (idx>99){
				 BDBG_ERR(("SDADC I channel calibration timeout"));
				 }
			 break;
	case 1 : BDBG_MSG(("SDADC I channel calibrationis done")); break;
	default :BDBG_ERR(("ERROR!!! INVALID SDADC I Channel Calibration Value: value is %d",statusIch)); break;
	}
	
	#if (BTNR_P_BCHP_TNR_CORE_VER <= BTNR_P_BCHP_CORE_V_1_1) 
		ICalOffset = BREG_ReadField(h->hRegister,SDADC_STATUS_ICH, o_Ich_flash_caldata);
		BREG_WriteField(h->hRegister, SDADC_CTRL_ICH, i_ctl_Ich_flash_offset, ICalOffset);
	#else
		ICalOffset = BREG_ReadField(h->hRegister,SDADC_STATUS_ICH_CAL_DATA, ch_flash_caldata);
		BREG_WriteField(h->hRegister, SDADC_CTRL_ICH_CAL_DATA, i_ctl_ch_flash_offset, ICalOffset);
	#endif
	BREG_WriteField(h->hRegister, SDADC_CTRL_ICH, i_Ich_flash_cal_on, 0);

	switch (statusQch)
	{
	case 0 : BDBG_MSG(("SDADC Q channel calibration NOT done"));
			 if (idx>99){
				 BDBG_ERR(("SDADC Q channel calibration timeout"));
				 }
			 break;
	case 1 : BDBG_MSG(("SDADC Q channel calibrationis done")); break;
	default :BDBG_ERR(("ERROR!!! INVALID SDADC Q Channel Calibration Value: value is %d",statusQch)); break;
	}

	#if (BTNR_P_BCHP_TNR_CORE_VER <= BTNR_P_BCHP_CORE_V_1_1) 	
		QCalOffset = BREG_ReadField(h->hRegister,SDADC_STATUS_QCH, o_Qch_flash_caldata);
		BREG_WriteField(h->hRegister, SDADC_CTRL_QCH, i_ctl_Qch_flash_offset, QCalOffset);
	#else
		QCalOffset = BREG_ReadField(h->hRegister,SDADC_STATUS_QCH_CAL_DATA, ch_flash_caldata);
		BREG_WriteField(h->hRegister, SDADC_CTRL_QCH_CAL_DATA, i_ctl_ch_flash_offset, QCalOffset);
	#endif
	BREG_WriteField(h->hRegister, SDADC_CTRL_QCH, i_Qch_flash_cal_on, 0);

	BREG_WriteField(h->hRegister, SDADC_CTRL_RESET, i_Ich_reset, 0x0);
	BREG_WriteField(h->hRegister, SDADC_CTRL_RESET, i_Qch_reset, 0x0);
}



/*******************************************************************************************************************
 * BTNR_P_TunerSetRFAGC()  This routine sets the tuner RF AGC
 ******************************************************************************************************************/
void BTNR_P_TunerSetRFAGC(BTNR_3x7x_ChnHandle h)
{
	uint32_t ReadReg;
	/*enable dither clock*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_clk_dither, 0x1);
	/*enable PN sequence*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, spare00, 0x2);	

	/*RF AGC init*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_DAC_LPF_pwrup_RFAGC, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_RFDPD_pwrup_RFAGC, 1);
    #if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)/* (BCHP_VER == BCHP_VER_A0) */  /* TO FIX - Fixed */
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i__BB2DPD_pwrup_RFAGC, 1);
	#else
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_BB2DPD_pwrup_RFAGC, 1);
	#endif
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_PWRUP_01, i_BB1DPD_pwrup_RFAGC, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_dsm_sigdel_en, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, RFAGC_DSM_intg_reset, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, RFAGC_DSM_resetb, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, RFAGC_reset, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, RFAGC_DSM_intg_reset, 0);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESETB_01, RFAGC_DSM_resetb, 1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RESET_01, RFAGC_reset, 0);

	/*RF AGC settings*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, RF_delay, 0x2);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, RF_clk_extend, 0x2);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, BB2_sel_IQ, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, BB2_delay, 0x2);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, BB2_clk_extend, 0x2);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, BB1_sel_IQ, 0x1);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, BB1_delay, 0x2);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, BB1_clk_extend, 0x2);	
	
	/*RF AGC initial gain settings*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_freeze_gain, 0x1);
	h->pTunerStatus->RFAGC_indirect_addr = 0x5;
	h->pTunerStatus->RFAGC_indirect_data = 0xFFFFFFFF;
	BTNR_P_TunerRFAGCIndirectWrite(h);	
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_02, i_freeze_gain, 0x0);	
	
	/*RF AGC close loop settings*/
	h->pTunerStatus->RFAGC_indirect_addr = 0x0;
	BTNR_P_TunerRFAGCIndirectRead(h);	
	ReadReg = h->pTunerStatus->RFAGC_indirect_data;

	ReadReg = ReadReg & 0xFFF3BDC0;
	if (h->pTunerParams->BTNR_TunePowerMode.Tuner_Power_Mode == BTNR_Tuner_Power_Mode_eLna_Daisy_Power)
		ReadReg = ReadReg | 0x0000401A;
	else
		ReadReg = ReadReg | 0x0000401B;

	h->pTunerStatus->RFAGC_indirect_addr = 0x0;
	h->pTunerStatus->RFAGC_indirect_data = ReadReg;
	BTNR_P_TunerRFAGCIndirectWrite(h);	

	BDBG_MSG(("BTNR_P_TunerSetRFAGC() Complete"));
}







#ifdef BTNR_ENABLE_SOFTWARE_TUNE
/******************************************************************************
 BTNR_P_TunerSearchCapSoftware() - capacitor binary scan
******************************************************************************/
void BTNR_P_TunerSearchCapSoftware(BTNR_3x7x_ChnHandle h)
{
   uint32_t val;
   uint32_t tunerCapMask;
   uint8_t count1 = 8;
}
#endif


