/******************************************************************************
*     (c)2010-2012 Broadcom Corporation
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
 *****************************************************************************/
/***************************************************************************
*     (c)2005-2012 Broadcom Corporation
*  
* 
* $brcm_Workfile: bthd_acq.c $
* $brcm_Revision: 86 $
* $brcm_Date: 10/18/12 6:01p $
*
* [File Description:]
*
* Revision History:
*
 * $brcm_Log: /AP/ctfe/core/thd/bthd_acq.c $
 * 
 * 86   10/18/12 6:01p farshidf
 * SW3461-287: create 5.3.1.1
 * 
 * Fw_Integration_Devel/39   10/17/12 5:25p mpovich
 * SW3472-21: remove warning for 7552 (merge).
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/20   10/17/12 4:35p mpovich
 * SW3461-290: THD common status SNR is always zero fix.
 * 
 * Fw_Integration_Devel/38   10/17/12 4:32p mpovich
 * SW3461-290: THD common status SNR is always zero fix.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/18   10/16/12 5:27p bsandeep
 * SW3472-21: remove warning for 7552
 * 
 * Fw_Integration_Devel/37   10/8/12 2:32p mpovich
 * SW3472-16: Add SSI and SQI to ISDBT Layers in Status (Merge to FW
 *  Integ. branch).
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/17   10/8/12 2:16p mpovich
 * SW3472-16: Fix potentially unused variable warnings in SSI function.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/16   10/8/12 2:06p mpovich
 * SW3472-16: Add SSI and SQI to ISDBT layer status (Merge to Sys Dev
 *  branch).
 * 
 * SW3472-17/1   9/12/12 6:21p jchien
 * SW3472-16: Add SSI and SQI to ISDBT status.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/10   9/12/12 6:16p jchien
 * SW3472-6: Rebase.
 * 
 * 83   8/29/12 11:35a farshidf
 * SW7552-336: merge to main
 * 
 * Fw_Integration_Devel/35   8/29/12 11:34a farshidf
 * SW7552-336: merge to main
 * 
 * Fw_Integration_Devel/35   8/29/12 11:34a farshidf
 * SW7552-336: merge to main
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/14   8/27/12 5:55p farshidf
 * SW7552-334: re-structure the thd PI files to use the Thd files in AP
 *  vob
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/13   8/21/12 5:28p mpovich
 * SW3472-6: Merge to 5_0 Dev branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/9   9/12/12 6:06p jchien
 * SW3472-16: Add SSI and SQI for ISDBT Status.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/8   8/21/12 5:26p mpovich
 * SW3472-6: Fix non-ISDBT compatibility during compilation.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/12   8/21/12 5:12p mpovich
 * SW3472-6: Merge to 5_0 Dev branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/7   8/17/12 11:35a jchien
 * SW3472-6: Remove debug code and add mem access bit width
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/6   8/14/12 2:48p mpovich
 * SW3472-6: Rebase with DEV branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/11   8/6/12 3:24p farshidf
 * SW7552-248: use the callback for 7552 instead of event
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/5   8/2/12 4:35p mpovich
 * SW3472-6: Fix DVB-T "Get Channel Estimators" function (restore dummy
 *  reads for DVB-T when reading the second portion of the data from the
 *  read-buffer).
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/4   7/30/12 6:08p mpovich
 * SW3472-6: Update Timing Response function for ISDB-T, per BBS 7552
 *  logic for "start" value.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/3   7/27/12 2:00p mpovich
 * SW3472-6: Fix compiler warning.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/2   7/27/12 1:18p mpovich
 * SW3472-6: Update Channel Estimators and Channel Coefficients for ISDB-
 *  T.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/1   7/26/12 5:48p jchien
 * SW3472-6: fix WPO, fix FW_WIN, turn on condition
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/10   7/17/12 3:06p jchien
 * SW7552-302: FFTW_Sync update
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/9   7/12/12 3:19p farshidf
 * SW7552-305: improve Acqusition time
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/8   7/5/12 4:21p farshidf
 * SW7552-295: kernel mode crash due to too much memory usage in
 *  FFTTrigger in stack pointer
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/7   6/20/12 3:12p farshidf
 * SW3472-5:  Add mobile channel estimation mode � Fix re-acquisition
 *  count � improved Doppler performance � Improved AWGN performance �
 *  Integrated ISDBT WPO for coherent mode
 * 
 * 77   6/14/12 3:45p farshidf
 * SW3461-227: Make the 3461/3462 5.0 release
 * 
 * Fw_Integration_Devel/29   6/14/12 3:23p farshidf
 * SW3461-227: Make the 3461/3462 5.0 release
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/6   6/11/12 4:16p jputnam
 * SW3461-207: Create explicit ConfigParams structure and remove TSMode
 *  from AcquireParameters and Status
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/5   5/22/12 3:38p mpovich
 * SW3461-214: Fix THD does not auto-acquire issue.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/4   5/21/12 1:40p farshidf
 * SW3461-211:mode TS mode to common status
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/3   5/11/12 4:26p farshidf
 * SW3461-207: Make TS-Mode part of the Acquire command and remove it for
 *  Internal acquire mode
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/2   5/8/12 6:25p farshidf
 * SW3461-1: clean up
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/1   4/26/12 6:23p farshidf
 * SW3461-193: add support for multichannel THD core
 * 
 * 71   3/29/12 7:57p farshidf
 * SW7552-239: remove warning
 * 
 * Fw_Integration_Devel/23   3/29/12 7:56p farshidf
 * Sw7552-239: remove warning
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/6   3/29/12 7:55p farshidf
 * SW7552-239: remove warning
 * 
 * 70   3/27/12 11:53a farshidf
 * SW7552-207: ISDBT fix
 * 
 * Fw_Integration_Devel/22   3/27/12 11:52a farshidf
 * SW7552-207: ISDBT fix
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/5   3/26/12 5:50p farshidf
 * SW3461-1: fix ISDB-T by Jack
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/4   3/23/12 12:38p farshidf
 * SW3128-125: FW version 4.6
 * 
 * SW3461-173/4   3/21/12 8:52p hfu
 * SW3461-179: Previous version does not solve the issue.  Added back the
 *  bad pilot estimation which was removed in previous check-in.
 * 
 * SW3461-173/3   3/16/12 4:46p hfu
 * SW3461-179: For Huawei immediate FW release, removed bad pilot
 *  estimation.
 * 
 * SW3461-173/2   3/15/12 7:37p hfu
 * SW3461-179: Removed check RS lock from Acquiring FEC state.
 * 
 * SW3461-173/1   3/15/12 2:05p farshidf
 * SW3461-173: add Jeff's changes
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/3   3/9/12 11:26a jputnam
 * SW3461-175: Implement SmartNotch to notch known tuner spurs for
 *  improved acquisition probability
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/2   3/8/12 4:44p farshidf
 * SW3461-1: merge to main
 * 
 * 67   3/8/12 4:37p farshidf
 * SW3461-1: add tuner status on timer base
 * 
 * 66   3/8/12 12:01a farshidf
 * SW3461-171: merge to main
 * 
 * Fw_Integration_Devel/19   3/7/12 10:52p farshidf
 * SW3461-171: merge to integ
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/1   3/7/12 10:43p mpovich
 * SW3461-159: Merge to Dev. branch.
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/SW3461-159/1   3/7/12 9:55p mpovich
 * SW3461-159: Add debug printf for "Early Exit".
 * 
 * Fw_Integration_Devel/18   2/8/12 2:49p farshidf
 * SW3461-1: remove warning
 * 
 * 64   1/25/12 4:08p farshidf
 * SW3461-110: merge to integ/main
 * 
 * Fw_Integration_Devel/17   1/25/12 4:06p farshidf
 * SW3461-110: merge to integ/main
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/25   1/24/12 3:25p mpovich
 * SW3461-110:  Modify THD acquisition logic to abort during various
 *  stages if expected events (flagged by register reads) do not occur
 *  within expected timeframe.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/24   1/24/12 11:20a mpovich
 * SW3461-110: Fix comment(s).
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/23   1/24/12 10:47a mpovich
 * SW3461-110:  Modify THD acquisition logic to abort during various
 *  stages if expected events (flagged by register reads) do not occur
 *  within expected timeframe.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/22   1/24/12 8:44a mpovich
 * SW3461-110:  Modify THD acquisition logic to abort during various
 *  stages if expected events (flagged by register reads) do not occur
 *  within expected timeframe.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/21   1/12/12 7:35p jputnam
 * SW3461-133: Added Ssi and Sqi computation code
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/20   1/6/12 1:54p farshidf
 * SW3461-40: merge to Dev branch
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/SW3461-40/7   12/29/11 2:53p mpovich
 * SW3461-40: Rebase with DEV branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/18   12/14/11 2:51p mbsingh
 * SW3461-1: Remove statics from THD code. Now ready for 7552 merge !
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/17   12/12/11 6:30p hfu
 * SW3461-108: Changed BTHD_P_Status() function's SNR calculation.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/16   12/9/11 4:13p mbsingh
 * SW3461-1: Add back static to fix crash
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/15   11/22/11 2:46p mbsingh
 * SW3461-1: Fix more of different type variables assigned to each other
 *  and other cleanup
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/14   11/22/11 1:11p mbsingh
 * SW3461-1: Remove some of the static's and a variable assignment
 *  mismatch
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/13   11/14/11 10:19a mpovich
 * SW3461-39: Leave "No Signal" status flag in its prior state when
 *  starting an acquire (flag is updated at the end of the acquire
 *  function).  Ensure that "No Signal" IRQ only generated once after a
 *  commanded acquire.
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/SW3461-39/2   11/11/11 3:11p mpovich
 * SW3461-39: Leave "No Signal" status flag in its prior state when
 *  starting an acquire (flag is updated at the end of the acquire
 *  function).
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/12   11/3/11 3:47p hfu
 * SW3461-1,  Changed check in description for
 *  Fw_Integration_Devel/AP_V3_0_THD_DEV/11  to fix a compile error.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/11   11/3/11 3:05p hfu
 *  SW3461-73: (1) Added conditions for FW_REF_RST in P_AcquireTrack().
 *  (2) Change FW min_scale from 1.5 to 1.125 in P_DvbtSetFwFtt().  Also
 *  changed one commented out line
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/SW3461-40/6   11/2/11 4:12p mpovich
 * SW3461-40: Abort-Early for THD acquire (Check for, and return "Abort-
 *  Early" before returning "No Signal").
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/SW3461-40/5   11/1/11 3:46p mpovich
 * SW3461-40: Abort-Early for THD acquire (Initial
 *  concept/implementation).
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/SW3461-40/4   11/1/11 2:45p mpovich
 * SW3461-40: Rebase with devel. branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/10   10/28/11 12:12p farshidf
 * SW3461-75: Move to new IRQ handling
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/9   10/27/11 5:32p jputnam
 * SW3461-73: Improve acquisition probability by: (1) Add FW_REF_RST in
 *  P_AcquireTrack() to match deployed 7550 firmware, (2) Change FW
 *  min_scale from 1.125 to 1.5 in P_DvbtSetFwFtt(), and (3) Enable FW
 *  slip during acquisition in  P_DvbtSetFwFtt() (disabled later in
 *  tracking)
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/SW3461-40/3   10/27/11 2:33p mpovich
 * SW3461-40: Abort-Early for THD acquire (Initial
 *  concept/implementation).
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/SW3461-40/2   10/27/11 11:32a mpovich
 * SW3461-40: Abort-Early for THD acquire (Initial
 *  concept/implementation).
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/SW3461-40/1   10/24/11 10:02p mpovich
 * SW3461-40: Abort-Early for THD acquire (Initial
 *  concept/implementation).
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/8   10/19/11 6:18p hfu
 * SW3461-1, Fixed a compile error when BDBG_MSG is on during make
 *  process.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/7   10/14/11 12:31a farshidf
 * SW3461-64: merge warning fix
 * 
 * 57   10/14/11 12:27a farshidf
 * SW3461-64: fix warning
 * 
 * 56   10/13/11 11:50p farshidf
 * SW3461-64: merge to main
 * 
 * Fw_Integration_Devel/11   10/13/11 6:57p farshidf
 * SW3461-64: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/6   10/12/11 5:36p hfu
 * SW3461-61. BER changes in DVBT QAM64, 2/3, 2k mode after retune is
 *  performed.   FW fix: Changed the delay time before DvbtGetNotch() from
 *  32 to 64 for 2K mode.  In addition to 2K mode cci_threshold change
 *  from 0xC to 0xF which is made in bthd_acq_dvbt.c
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/5   10/7/11 5:12p hfu
 *  SW3461-62: 3461B0 FW3.0 DVB-T Nordig Tests failed at multiecho and
 *  outsideguard tests.  Fixed the bug in setting
 *  BCHP_THD_CORE_CE_RECORD_CFG.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/4   10/7/11 4:41p hfu
 * SW3461-61: BER changes in DVBT QAM64, 2/3, 2k mode after retune is
 *  performed.   Changed EQ Pilots Noise Averager Forgetting Factor during
 *  acquire process to prevent Notch filter to be turned on for AWGN
 *  channel .
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/3   9/30/11 4:00p mbsingh
 * - Fixing a bug from the previous checkin
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/2   9/27/11 12:25p mbsingh
 * SW3461-1: Due to THD core v5.1, TPS values can be read directly from
 *  snooper. Using this enhancement to optimize FTT algo.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/1   9/26/11 2:55p mbsingh
 * SW3461-1: Merge from 2.0 dev branch (This is mainly a shift to WPO
 *  algorithm)
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/9   9/21/11 5:37p jputnam
 * SW3461-1: Fix tuner callback so that it works with SmartTune defined or
 *  not defined
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/8   9/20/11 9:48a mpovich
 * SW3461-39: Add "No Signal" to T2 Status and THD Status HAB commands.
 *  Add "FEC Lock" to THD Status HAB command.
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/SW3461-39/1   9/20/11 9:04a mpovich
 * SW3461-39: Support for FEC lock and No Signal in status.
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/7   9/15/11 2:18p mbsingh
 * - Fix function naming
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/6   9/9/11 6:07p mbsingh
 * SW3461-1:  - Change // comments to fix compile on non-leap chips -
 *  Remove MAXPEAX stuff - Ifdef GetChannelSpan function for ISDBT
 *  containing chips only - Added back AcquirechangeFFTwindow function
 *  (needed by ISDB-T)   - Added back FW_WIN enable for ISDBT
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/5   9/8/11 10:47p mbsingh
 * SW3461-1:  Separated SetFW function cleanly now to make DVBT work for
 *  ISDBT containing chips also.  In initial merged code from THD_FTT_WPO
 *  branch DVBT would work only in non ISDBT chips due to wrong SetFW
 *  function
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/4   9/8/11 10:05p mbsingh
 * SW3461-1: Merging from THD_FTT_WPO branch                   -
 *  Architectural change to new FTT algo for acquisition
 * 
 * Fw_Integration_Devel/AP_V0_6_THD_DEV/Thd_ftt_wpo/3   8/24/11 10:56a hzou
 * Update state machine to do FFTTrigger after TPS synch, and only redo
 *  TPS synch is missing first peak or having on-the-gurad echos. Also
 *  reduced wait time in FFTTtrigger
 * 
 * Fw_Integration_Devel/AP_V0_6_THD_DEV/Thd_ftt_wpo/2   8/18/11 3:58p hzou
 * Improved on-the-guard pre-echo in FTT
 * 
 * Fw_Integration_Devel/AP_V0_6_THD_DEV/Thd_ftt_wpo/1   8/18/11 10:49a hzou
 * Ported in chnages for FTT and WPO
 * 
 * Fw_Integration_Devel/10   9/22/11 4:57p farshidf
 * SW3461-1: disable smarttune
 * 
 * Fw_Integration_Devel/9   8/26/11 5:25p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/3   8/26/11 5:22p farshidf
 * SW3461-1: make it magnum compatible
 * 
 * 48   8/24/11 5:54p farshidf
 * SW7552-60: merge to main
 * 
 * Fw_Integration_Devel/8   8/24/11 5:47p farshidf
 * SW7552-60: add the isdb-t constellation
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/2   8/24/11 5:40p farshidf
 * SW7552-60: add the softdecision for isdbt
 * 
 * 47   8/24/11 12:32p farshidf
 * SW3461-38: merge to main
 * 
 * Fw_Integration_Devel/7   8/24/11 12:07p farshidf
 * SW3461-38: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_THD_DEV/1   8/22/11 12:45p jputnam
 * SW3461-1: Added SmartTune
 * 
 * Fw_Integration_Devel/5   8/12/11 2:11p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_THD_DEV/6   8/10/11 3:45p farshidf
 * SW3461-1: update status printf
 * 
 * Fw_Integration_Devel/AP_V0_6_THD_DEV/5   7/22/11 12:30p jputnam
 * Added SignalStrength
 * 
 * Fw_Integration_Devel/AP_V0_6_THD_DEV/4   7/12/11 4:13p farshidf
 * SW3461-17: compile fix
 * 
 * Fw_Integration_Devel/AP_V0_6_THD_DEV/3   7/12/11 3:28p jchien
 * AP_V0_6_THD_DEV:  checkLock state: add three layer check for isdbt.
 * 
 * Fw_Integration_Devel/AP_V0_6_THD_DEV/2   7/12/11 12:17p farshidf
 * SW3461-17: fix compilation issue
 * 
 * Fw_Integration_Devel/AP_V0_6_THD_DEV/1   7/7/11 5:10p farshidf
 * SW3461-17: add the T2 and Thd callback to tuner
 * 
 * Fw_Integration_Devel/2   6/29/11 12:38p farshidf
 * SW3461-13: merge to integration branch
 * 
 * Fw_Integration_Devel/Dvbt_Fw_Devel_Rc05/1   6/20/11 1:55p mpovich
 * SW3461-1: Merge post 0.4 release made to Dvb_Fw_Devel_4 and
 *  SW_System_4_Integ branches.
 * 
 * Fw_Integration_Devel/1   6/20/11 1:53p mpovich
 * SW3461-1: Merge post 0.4 release made to Dvb_Fw_Devel_4 and
 *  SW_System_4_Integ branches.
 * 
 * SW_System_4_Integ_Test/5   6/20/11 1:52p mpovich
 * SW3461-1: Rebase System_4 integ Test branch with main / 0.4 release.
 * 
 * SW_System_4_Integ_Test/4   6/17/11 6:51p mpovich
 * SW3461-1: Pick up post 3461 version 0.4 release change.
 * 
 * Dvb_Fw_Devel_4/6   6/17/11 5:40p jputnam
 * SW3461-1: Resurrect proper return value logic inside P_GetChannelSpan()
 *  which was lost in some previous merge
 * 
 * 44   6/10/11 2:48p farshidf
 * SW3461-1: minor compile fix
 * 
 * 43   6/9/11 6:36p mpovich
 * SW3461-1: merge main
 * 
 * SW_System_4_Integ_Test/3   6/9/11 5:58p farshidf
 * SW3461-1: fix the auto acquire for 3461
 * 
 * SW_System_4_Integ_Test/2   6/9/11 12:21p mpovich
 * SW3461-1: Integrate latest from all devel. branches.  Remove end of
 *  line comments per coding standards.
 * 
 * 42   6/8/11 6:18p farshidf
 * SW3461-1: merge main
 * 
 * Dvb_Fw_Devel_4/5   6/8/11 6:16p farshidf
 * SW3461-1: sync up with magnum
* 
* SW_System_4_Integ_Test/1   6/6/11 1:53p mpovich
* SW3461-1: Integ. test all development branches together.
* 
* Hydra_Software_Devel/17   6/3/11 5:15p farshidf
* SW7552-36: compile fix
* 
* Hydra_Software_Devel/16   6/3/11 4:43p farshidf
* SW7552-36: merge to main
* 
* Hydra_Software_Devel/SWDTV-7361/2   6/2/11 2:15p mbsingh
* SWDTV-7361: DVB-T Channel Scan Improvements
* 
* Hydra_Software_Devel/SWDTV-7361/1   6/2/11 2:08p mbsingh
* SWDTV-7361: Initialize status spares, Hold retcode in spare1
* 
* Hydra_Software_Devel/15   6/3/11 4:41p farshidf
* SW7552-36: merge to main
* 
* Hydra_Software_Devel/14   6/1/11 11:44a farshidf
* SW7552-36: clean up the code
* 
* Hydra_Software_Devel/13   5/31/11 7:37p farshidf
* SW7552-36: remove ISDB-T warning
* 
* Hydra_Software_Devel/12   5/27/11 10:53a farshidf
* SW7552-36: compile fix
* 
* Hydra_Software_Devel/17   6/3/11 5:15p farshidf
* SW7552-36: compile fix
* 
* Hydra_Software_Devel/16   6/3/11 4:43p farshidf
* SW7552-36: merge to main
* 
* Hydra_Software_Devel/15   6/3/11 4:41p farshidf
* SW7552-36: merge to main
* 
* Hydra_Software_Devel/SWDTV-7146/2   5/25/11 3:31p jchien
* SWDTV-7146: add isdbt HW status reset
* 
* Hydra_Software_Devel/SWDTV-7146/1   5/24/11 12:20p jchien
* SWDTV-7146: add RS_SYNC FEC block monitor. Add fast_acq.
* 
* Hydra_Software_Devel/11   5/18/11 12:11p farshidf
* SWDTV-7035: fix the memory allocation for ISDB-T
* 
* Hydra_Software_Devel/10   5/6/11 6:43p farshidf
* SWDTV-6857: fix teh scan mode
* 
* Hydra_Software_Devel/9   5/6/11 6:22p farshidf
* SWDTV-6857: add scan
* 
* Hydra_Software_Devel/8   5/6/11 2:21p farshidf
* SWDTV-6857: update the auto acquire mode
* 
* Hydra_Software_Devel/7   5/5/11 8:34p farshidf
* SWDTV-6857: merge main
* 
* Hydra_Software_Devel/SWDTV-6857/4   5/5/11 7:04p farshidf
* SWDTV-6857: add flag around ISDB-T variable
* 
* Hydra_Software_Devel/SWDTV-6857/3   5/5/11 6:47p farshidf
* SWDTV-6857: merge main
* 
* Hydra_Software_Devel/SWDTV-6857/1   5/3/11 4:23p jchien
* SWDTV-6857: add ISDBT changes
* 
* 40   5/3/11 1:33p mpovich
* SW3461-1: Merge to main of intermediate label, FW_3461_A0_05022011_1100
*  (05/02/2011 Pace build).
* 
* DVB_3461_1/27   4/28/11 10:05a jputnam
* SW3461-1: Reverted FW min_scale to 1.5 for InSpan mode. Added
*  GetChannelSpan() and associated parameter in status structure.
* 
* DVB_3461_1/26   4/27/11 8:30a jputnam
* SW3461-1:  Added #define ChangeFFTWindowSeamless for experimentation
*  with seamless FW switching to improve acquisition time
* 
* DVB_3461_1/25   4/27/11 7:56a jputnam
* SW3461-1: Changed FW min_scale from 1.5 to 1.0625 to allow better
*  detection of weak pre-echoes for InSpan mode
* 
* DVB_3461_1/24   4/26/11 9:21p jputnam
* SW3461-1: Removed code associated with frequency-domain FFT trigger
*  algorithm.  Development moved to another branch. Removed unconditional
*  return value of THD_AcquireResult_Lock in AcquireCheckLock() function
* 
* DVB_3461_1/23   4/25/11 11:43a jputnam
* SW3461-1: Move additional statement inside #ifndef FFTTriggerPosition
*  statement
* 
* 39   4/21/11 6:22p farshidf
* SW3461-1: merge main
* 
* DVB_3461_1/22   4/21/11 6:21p farshidf
* SW3461-1: merge main
* 
* DVB_3461_1/21   4/21/11 6:03p farshidf
* SW3461-1: merge main
* 
* 38   4/21/11 6:01p farshidf
* SW3461-1: make the files host compatible
* 
* 37   4/21/11 5:45p farshidf
* SW3461-1: merge main
* 
* DVB_3461_1/20   4/21/11 5:45p farshidf
* SW3461-1: merge main
* 
* DVB_3461_1/19   4/15/11 9:37p jputnam
* SW3461-1: Firmware FFT trigger positioning functional. Feature can be
*  disabled by removing #define FFTTriggerPosition in bthd_acq.h
* 
* DVB_3461_1/18   4/13/11 4:21p jchien
* SW3461-1: fix isdbt related bugs
* 
* DVB_3461_1/17   4/13/11 10:33a jputnam
* SW3461-1: Hooked-up GetChannelEstimateBuf() and
*  GetInterpolatorCoefficientsBuf() in API. Added code supporting Time
*  Response and Frequency Interpolator Response displays
* 
* DVB_3461_1/16   4/12/11 10:25a jputnam
* SW3461-1: Added P_GetChannelEstimateBuf() and
*  P_GetInterpolatorCoefficientsBuf() functions. Reduced FI leakage to
*  2^(-20)
* 
* DVB_3461_1/15   4/11/11 6:50p jputnam
* SW3461-1: Cosmetic cleanup of GetFFTTirggerPosition() function
* 
* DVB_3461_1/14   4/11/11 4:16p mbsingh
* SW3461-1: Adding FFT Trigger position determination Function
* 
* 36   4/21/11 5:29p farshidf
* SW3461-1: make it host compatible
* 
* 35   4/20/11 4:55p farshidf
* SW3461-1: make it host compatible
* 
* 34   4/19/11 12:03p farshidf
* SW3461-1: make host compatible
* 
* 33   4/11/11 3:49p farshidf
* SW3461-1: merge main
* 
* DVB_3461_1/13   4/11/11 9:16a jputnam
* SW3461-1: Functional CoChannel mode
* 
* DVB_3461_1/12   4/8/11 11:56a jputnam
* SW3461-1: Automatic ChannelEstimatorMode now functional
* 
* DVB_3461_1/11   4/7/11 10:58p jputnam
* SW3461-1: Removed obsolete lock functions. Reverted state machine so
*  that SP, TPS, and FEC states are revisited upon FFTWindowMode change.
*  Force RS_SYNC_LOSS interrupt if P_Acquire returns without locking to
*  force proper update of lock state.
* 
* DVB_3461_1/10   4/7/11 9:28p jputnam
* SW3461-1: Initial version of interrupt-based lock detection
* 
* DVB_3461_1/9   4/6/11 5:27p farshidf
* SW3461-1: compile fix
* 
* DVB_3461_1/8   4/6/11 5:22p farshidf
* SW3461-1: remove not used code
* 
* DVB_3461_1/7   4/6/11 2:12p jputnam
* SW3461-1: Removed obsolete AcqRequest field from Common Acquire Params
* 
* DVB_3461_1/6   4/5/11 10:01p jputnam
* SW3461-1: Corrected RS retention threshold
* 
* DVB_3461_1/5   4/5/11 9:14p jputnam
* SW3461-1: Revamped P_Lock and P_DvbtLock functions. Added proper Acq
*  and Ret thresholds for RS lock detectors. Cosmetic clean-up of code
*  and printfs.
* 
* DVB_3461_1/4   4/5/11 8:00p mpovich
* SW3461-1: Update THD Status HAB command to include missing parameters:
*  Acquire Mode, and block counters.
* 
* DVB_3461_1/3   4/4/11 5:12p jputnam
* SW3461-1: Fixed P_GetSoftDecisionBuf()
* 
* 28   4/4/11 4:33p mpovich
* SW3461-1: Update THD Status HAB command to include missing parameters:
*  Acquire Mode, and block counters.
* 
* 27   3/25/11 6:29p mpovich
* SW3461-1: Merge latest from T2 and THD to the main branch.
* 
* DVB_3461_1/2   3/18/11 4:13p farshidf
* SW3461-1: merge  main
* 
* 26   3/14/11 10:19a farshidf
* SW3461-1: merge to main branch
* 
* DVB_3461_1/1   3/13/11 11:45a jputnam
* SW3461-1: Suppress TS clock in serial mode for compatibility with 7231
*  reference design
* 
* 25   3/9/11 3:44p jputnam
* SW3461-1: Moved status copy from BTHD_P_Status() up to BTHD_Status()
* 
* 24   3/8/11 12:39p farshidf
* SW3461-1: fix teh THD API
* 
* 23   3/4/11 4:52p farshidf
* SW3461-1: add the power seq
* 
* 22   2/25/11 5:45p jputnam
* SW3461-1: Added hook for emulation with and without UFE
* 
* 21   2/8/11 11:57a jputnam
* SW3461-1: Added spectral inversion field to status
* 
* 20   1/21/11 5:57p farshidf
* SW3461-1: update names
* 
* 19   1/20/11 4:22p farshidf
* SW3461-1: update the thd/isdbt
* 
* 18   1/6/11 10:08a jputnam
* SW3461-1: Major cosmetic overhaul - Conform to new API conventions
*  common to all demods - Conform to naming conventions for private
*  functions and typedefs - Separate DVB-T and ISDB-T params, status,
*  functions
* 
* 17   1/3/11 5:35p jputnam
* SW3461-1: changed enumerated type and #define names from THD_ to BTHD_
* 
* 16   12/16/10 6:48p farshidf
* SW3461-1: clean up
* 
* 15   12/13/10 5:07p farshidf
* SW3461-1: add the sync and out of sync irq
* 
* 14   12/3/10 4:37p farshidf
* SW3461-1: printf update
* 
* 13   12/2/10 8:59a jputnam
* SW3461-1: ifdef out redundant BMTH_2560log10() and
*  BTHD_SetDefaultSettings() as these functions have already been moved
*  to bmth.c and bthd_api.c respectively
* 
* 12   11/30/10 11:19a farshidf
* SW3461-1: add BMTH_2560log10 to math lib
* 
* 11   11/30/10 10:44a farshidf
* SW3461-1: update the thd files
* 
* 10   11/23/10 5:43p farshidf
* SW3461-1: update the timer and printf
* 
* 9   11/15/10 12:40p farshidf
* SW3461-1: compile fix
* 
* 8   11/11/10 9:43a jputnam
* SW3461-1: Acquisition successful in EMULATION_ENABLED.  TL_FCW is temporarily
*  hard-coded in SetFrequency() due to error in 64-bit computation.
* 
* 7   11/4/10 2:36p farshidf
* SW3461-1: clean up the irq
* 
* 6   11/4/10 10:06a jputnam
* SW3461-1: Separated Acquire() function into multiple functions called
*  by a new top-level acquisition state machine
* 
* 1   8/17/10 5:12p farshidf
* SW3461-1: initial THD code
* 
* 
***************************************************************************/
#ifdef BTHD_ISDBT_SUPPORT   /* this may impact DVB-T for 7552 Jeff to review */
#define FASTACQ 
#endif
#include "bstd.h"
#include "bmth.h"
#include "bkni.h"
#include "btmr.h"


#ifndef LEAP_BASED_CODE
#include "bthd_3x7x.h"
#endif
#include "bthd_api.h"
#include "bthd_acq.h"
#include "bchp_thd_core.h"

#include "bthd_acq_dvbt.h"
#ifdef BTHD_ISDBT_SUPPORT
#include "bthd_acq_isdbt.h"
#endif
#include "bthd_coef.h"

#include "bchp_thd_intr2.h"
#include "bchp_thd_intr2b.h"
#ifdef LEAP_BASED_CODE
#include "bchp_leap_ctrl.h"
#include "bthd_irq.h"
#endif
#ifdef EMULATION_ENABLED
#include "bchp_thd_intr.h"
#endif

#ifndef LEAP_BASED_CODE
BDBG_MODULE(bthd_acq);
#endif

/* Convert milliseconds to Sample-rate counts  */
#define BTHD_P_MSEC_TO_FSCNT(TIME_MSEC) ((uint32_t)((TIME_MSEC)*54000U))

/***************************************************************************
 * BTHD_P_OSleep() 
 ***************************************************************************/
BERR_Code BTHD_P_OSleep(BTHD_3x7x_ChnHandle h, 
			uint32_t symbols, 
			uint32_t N, 
			uint32_t D)
{
  uint32_t samples = symbols*(N + (N >> (5-D)));  
  BERR_Code eEventWaitResult;
  
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FBCNT,samples);
  BINT_EnableCallback(h->hFbcCntCallback);
  eEventWaitResult = BTHD_P_WaitForEventOrAbort(h, h->hFbcntZeroEvent, 400); 
  if ( eEventWaitResult != BERR_SUCCESS && eEventWaitResult != BTHD_ERR_ACQUIRE_ABORTED ) {
    BDBG_MSG(("BTHD_P_OSleep:No FBCNT zero"));
  }
  BINT_DisableCallback(h->hFbcCntCallback);
  return ( eEventWaitResult );
}

/***************************************************************************
* BTHD_P_Read_FSCnt_Time()
***************************************************************************/
static uint32_t BTHD_P_Read_FSCnt_Time ( BTHD_3x7x_ChnHandle h )
{
  return ( BREG_Read32(h->hRegister, BCHP_THD_CORE_FSCNT) ); 
}

/***************************************************************************
* BTHD_P_Check_FSCnt_TimeExceeded()
***************************************************************************/
static bool BTHD_P_Check_FSCnt_TimeExceeded ( BTHD_3x7x_ChnHandle h, uint32_t uStartFsCount, uint32_t uTimeoutInFsCounts )
{
  bool bTimeout = false;
  uint32_t uCurrFsCount, w_time = 0; 

  uCurrFsCount = BTHD_P_Read_FSCnt_Time(h);

  if ( uStartFsCount < uCurrFsCount) {
      /* The counter is wrapped */
      w_time = (0xffffffffU - uCurrFsCount) + uStartFsCount; 
  }
  else {
      w_time = uStartFsCount - uCurrFsCount; 
  }

  if ( w_time > uTimeoutInFsCounts ) {
      bTimeout = true;
  }
  
  /* Timeout period was exceeded */
  return ( bTimeout );
}
#if 1

/***************************************************************************
* BTHD_P_Wait_CE_Recording_Status_Done()
***************************************************************************/
bool BTHD_P_Wait_CE_Recording_Status_Done ( BTHD_3x7x_ChnHandle h, uint32_t uTimeOutMsec )
{
  bool bTimedOutWaiting = false;
  uint32_t start_stamp; 
  uint32_t uTimeOutFScounts = BTHD_P_MSEC_TO_FSCNT(uTimeOutMsec);

  start_stamp = BTHD_P_Read_FSCnt_Time(h);
  while ( (0 == BREG_ReadField(h->hRegister, THD_CORE_CE_RECORD_CFG, DONE)) && (! bTimedOutWaiting) ){
    bTimedOutWaiting = BTHD_P_Check_FSCnt_TimeExceeded(h, start_stamp, uTimeOutFScounts);
  }
  if ( bTimedOutWaiting ) {
      BDBG_MSG(("\t\tWait THD_CORE_CE_RECORD_CFG DONE timeout at 0x%08X", BTHD_P_Read_FSCnt_Time(h)));
  }
  return ( ! bTimedOutWaiting );
}

/******************************************************************************
 * BTHD_P_GetChannelEstimateBuf()
 ******************************************************************************/
BERR_Code BTHD_P_GetChannelEstimateBuf(BTHD_3x7x_ChnHandle h,uint32_t *pData, THD_SnooperMode_t mode, uint32_t uDataLenOutputMax, uint32_t *puDataLenOutput )
{
  BERR_Code eResult = BERR_SUCCESS;
#ifdef BTHD_ISDBT_SUPPORT
  static const uint32_t uNUM_ACTIVE_CARRIERS = 1404;
#endif
  uint32_t val,idx,start=0,step=3,size=285, first_half_read_len=0, second_half_start, size_returned_words = 0;
  /* Num samples to be discarded before buffering second-half (second-portion) of collected data */
  uint32_t second_half_num_discarded_samples = 0;  
  uint32_t N=bthd_transmission_mode[BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE)];
  
  BKNI_Memset(pData,0,uDataLenOutputMax);

  if (mode == THD_SnooperMode_Ce) {
#ifdef BTHD_ISDBT_SUPPORT
    if ( h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT ) 
    {
      /* ISDB-T */
      step = 3; 
      size = 256; 
      switch ( N >> 10 )
      {
        /* 2K */
        case 2 : start = (uNUM_ACTIVE_CARRIERS * 1 / 2); break;
        /* 4K */
        case 4 : start = (uNUM_ACTIVE_CARRIERS * 18 / 13); break;
        /* 8K */
        case 8 : start = (uNUM_ACTIVE_CARRIERS * 44 / 13); break;
        default:
          BDBG_ERR(("Invalid FFT MODE"));
          start = (uNUM_ACTIVE_CARRIERS * 44 / 13); 
      }
    }
    else
#endif /* BTHD_ISDBT_SUPPORT */
    {
      /* DVB-T */
    /* Use this selection, to get center 256 pilots for time response display */
      step = 3;
      size = 256; 
    start = 852*(N >> 11) - 3*127;
    }
    /* read 256 samples or fewer, depending on buffer limit */
    size = MIN(uDataLenOutputMax/sizeof(uint32_t),size);

    BREG_Write32(h->hRegister,BCHP_THD_CORE_CE_RECORD_CFG,0x01000000 + (mode << 25) + (step << 16) + start ); /* Snooper snapshot mode */

    /* while (!BREG_ReadField(h->hRegister, THD_CORE_CE_RECORD_CFG, DONE)); */
    /* Wait 1 second for internal buffer to fill */
    if ( ! BTHD_P_Wait_CE_Recording_Status_Done(h, 1000) ) {
      eResult = BERR_TIMEOUT;
      goto BTHD_P_GetChannelEstimateBuf_Exit;
    }

    BREG_Write32(h->hRegister,BCHP_THD_CORE_CE_READ_INDEX,0x00000000);
    for (idx=0; idx<size; idx++) {
      val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_READ_DATA);
      pData[idx] = val;
    }  
    size_returned_words = size;

  } else {
    start = 0;

#ifdef BTHD_ISDBT_SUPPORT
    if ( h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT ) 
    {
      /* ISDB-T */

      /* By default, ISDB-T does not require discarding data when reading [the second portion of] the read-buffer */
      second_half_num_discarded_samples = 0;

      switch ( N >> 10 )
      {
        /* 2K */
        case 2 : step = 6; size = 234; first_half_read_len = 234;  second_half_start = 0; break;
        /* 4K */
        case 4 : step = 11; size = 256; first_half_read_len = 256; second_half_start = 0; break;
        /* 8K */
        case 8 : step = 22; size = 256; first_half_read_len = 256; second_half_start = 0; break;
        default:
          BDBG_ERR(("Invalid FFT MODE"));
          /* (1404 * N)/(256*2048), with round-up */
          step = (((N * uNUM_ACTIVE_CARRIERS)>>8)+1024) >> 11;
          /* (N/2048)*1404/2, with round-up */
          second_half_start = (((N>>11) * uNUM_ACTIVE_CARRIERS)+1)/2;
          first_half_read_len = 256; 
      }
      /* read 256 samples or fewer, depending on buffer limit */
      size = MIN(uDataLenOutputMax/sizeof(uint32_t),size);
    }
    else
#endif /* BTHD_ISDBT_SUPPORT */
    {
      /* DVB-T */
    step = 6*(N >> 11);
      /* read 285 samples or fewer, depending on buffer limit */
      size = MIN(uDataLenOutputMax/sizeof(uint32_t),285);
      first_half_read_len =  256;
      second_half_start = 1704*(N >> 11) - step*255;   
      /* DVB-T requires discarding this number of samples prior to reading the second portion of the read-buffer (not to exceed 256 ever) */
      second_half_num_discarded_samples = MIN(256, (256 - second_half_start/step));
    }
    BREG_Write32(h->hRegister,BCHP_THD_CORE_CE_RECORD_CFG,0x01000000 + (mode << 25) + (step << 16) + start ); /* Snooper snapshot mode */

    /* while (!BREG_ReadField(h->hRegister, THD_CORE_CE_RECORD_CFG, DONE)); */
    /* Wait 1 second for internal buffer to fill */
    if ( ! BTHD_P_Wait_CE_Recording_Status_Done(h, 1000) ) {
      eResult = BERR_TIMEOUT;
      goto BTHD_P_GetChannelEstimateBuf_Exit;
    }

    BREG_Write32(h->hRegister,BCHP_THD_CORE_CE_READ_INDEX,0x00000000);
    for (idx=0; idx<first_half_read_len; idx++) {
      val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_READ_DATA);
      pData[idx] = val;
    }
    
    /* More data to read from second half ?*/
    if ( first_half_read_len < size ) {
      /* Adjust sample-reads for the second half of the output buffer */
      BREG_Write32(h->hRegister,BCHP_THD_CORE_CE_RECORD_CFG,0x01000000 + (mode << 25) + (step << 16) + second_half_start ); /* Snooper snapshot mode */
    /* while (!BREG_ReadField(h->hRegister, THD_CORE_CE_RECORD_CFG, DONE)); */
    /* Wait 1 second for internal buffer to fill */
    if ( ! BTHD_P_Wait_CE_Recording_Status_Done(h, 1000) ) {
      eResult = BERR_TIMEOUT;
      goto BTHD_P_GetChannelEstimateBuf_Exit;
    }

    /*BREG_Write32(h->hRegister,BCHP_THD_CORE_CE_READ_INDEX,256-start/step); */
    BREG_Write32(h->hRegister,BCHP_THD_CORE_CE_READ_INDEX,0x00000000);
    
      /* Discard samples before reading-in the second (last) portion of the data */
      for ( idx = 0; idx < second_half_num_discarded_samples; idx++ ) {
        val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_READ_DATA);
    }           

      /* Read second-half (last) portion of the read-buffer */
      for ( idx = first_half_read_len; idx < size; idx++ ) {
        val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_READ_DATA);
        pData[idx] = val;  
      }
    }

    size_returned_words = size;
  }
  
BTHD_P_GetChannelEstimateBuf_Exit:

  if ( NULL != puDataLenOutput ) {
    *puDataLenOutput = size_returned_words*sizeof(uint32_t);
    }
  return ( eResult );
}

/******************************************************************************
 * BTHD_P_GetInterpolatorCoefficientsBuf()
 ******************************************************************************/
BERR_Code BTHD_P_GetInterpolatorCoefficientsBuf(BTHD_3x7x_ChnHandle h,uint32_t *pData, THD_InterpolatorMode_t mode, uint32_t uDataLenOutputMax, uint32_t *puDataLenOutput )
{
  uint32_t val=0,idx,phase, idx_max, idx_storage, size_returned_words = 0;
  
  BKNI_Memset(pData,0,uDataLenOutputMax);
  
  if (mode == THD_InterpolatorMode_Ti) {
    idx_max = MIN(16, uDataLenOutputMax/sizeof(uint32_t));
    for (idx=0; idx<idx_max; idx+=2) {
      switch(idx) {
        case 0:  val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_TCOEF01); break;
        case 2:  val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_TCOEF23); break;
        case 4:  val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_TCOEF45); break;
        case 6:  val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_TCOEF67); break;
        case 8:  val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_TCOEF89); break;
        case 10: val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_TCOEF1011); break;
        case 12: val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_TCOEF1213); break;                                
        case 14: val = BREG_Read32(h->hRegister,BCHP_THD_CORE_CE_TCOEF14); break;        
      }
      pData[idx]   = (val & 0xffff0000);
      pData[idx+1] = (val & 0x0000ffff) << 16;
    }
    size_returned_words = idx_max;
  } else {
    /* Total coefficients = (11 sets) x (36 coeff/set) = 396.  Read only the 36 coefficients for the center set for display
       which correspond to indices 180-191, 192-203, 204-215 for the three polyphase subfilters */
    BREG_Write32(h->hRegister,BCHP_THD_CORE_FI_ADDR,36*5);     
    idx_max = MIN(36, uDataLenOutputMax/sizeof(uint32_t));
    for (phase=0; phase<3; phase++) {   
      for (idx=0; idx<12; idx++) {
        val = BREG_Read32(h->hRegister,BCHP_THD_CORE_FI_COEF);
        if ( (idx_storage = (3*idx+phase)) < idx_max ) {
            pData[idx_storage] = ((val & 0x3fff0000) << 2) | ((val & 0x00003fff) << 2);
      }  
    }  
    }  
    size_returned_words = idx_max;
  }

  if ( NULL != puDataLenOutput ) {
    *puDataLenOutput = size_returned_words*sizeof(uint32_t);
  }
  return BERR_SUCCESS;
}

/***************************************************************************
 * BTHD_P_Init()  
 ***************************************************************************/
BERR_Code BTHD_P_Init(BTHD_3x7x_ChnHandle h)
{
  /* Reset FEC error counters */
  BREG_WriteField(h->hRegister,  THD_CORE_RST, RS_ERC_RST, 1 );	           
  BREG_WriteField(h->hRegister,  THD_CORE_RST, RS_ERC_RST, 0 );  
  return BERR_SUCCESS;
}

/***************************************************************************
 * BTHD_P_EnableLockInterrupts()
 ***************************************************************************/
void BTHD_P_EnableLockInterrupts(BTHD_3x7x_ChnHandle h)
{
#ifdef BTHD_ISDBT_SUPPORT
  uint32_t segs_b, segs_c;
  /* Reset lock detector, clear interrupt status, and enable lock interrupts */  

  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 0 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_B, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_B, 0 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_C, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_C, 0 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_B, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_C, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 0 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_B, 0 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_C, 0 );

    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_INTR, 1 );
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_LOSS_INTR, 1 );
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_B_INTR, 1 );
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_LOSS_B_INTR, 1 );
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_C_INTR, 1 );
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_LOSS_C_INTR, 1 );
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_INTR, 0 );
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_LOSS_INTR, 0 );   
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_B_INTR, 0 );
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_LOSS_B_INTR, 0 );   
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_C_INTR, 0 );
    BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_LOSS_C_INTR, 0 );   
	BTHD_P_IsdbtResetLockSetClrFlag(h);  /* clear previous interrupt flags */
  } else {
#endif
  BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 1 );
  BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 0 );
  BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_INTR, 1 );
  BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_LOSS_INTR, 1 );
  BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_INTR, 0 );
  BREG_WriteField(h->hRegister, THD_INTR2B_CPU_CLEAR, RS_SYNC_LOSS_INTR, 0 );   
#ifdef BTHD_ISDBT_SUPPORT
  }
#endif

#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
  	segs_b = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_SEG);
  	segs_c = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_SEG);
    BINT_EnableCallback(h->hRsSyncCallback);
    BINT_EnableCallback(h->hRsSyncLossCallback);
	if (segs_b) {
	  BINT_EnableCallback(h->hRsSyncBCallback);
      BINT_EnableCallback(h->hRsSyncLossBCallback);
	}
	if (segs_c) {
      BINT_EnableCallback(h->hRsSyncCCallback);
      BINT_EnableCallback(h->hRsSyncLossCCallback);
	}
  } else {
#endif 

  BINT_EnableCallback(h->hRsSyncCallback);
  BINT_EnableCallback(h->hRsSyncLossCallback);

#ifdef BTHD_ISDBT_SUPPORT
  }
#endif 
}
  
/***************************************************************************
 * BTHD_P_AbortAcquire()
 ***************************************************************************/
void BTHD_P_AbortAcquire ( BTHD_3x7x_ChnHandle h )
{
  volatile bool prevAbortFlagValue = h->pInternalAcquireParam->AbortAcquireRequested;

  /* Set flag to abort an acquire already in-progress */
  h->pInternalAcquireParam->AbortAcquireRequested = true;

  /* On a new Abort, signal the event being waited on just once. (Do not trigger the event once Abort flag is set) */ 
  if ( (! prevAbortFlagValue) && (h->pInternalAcquireParam->hCurrAbortableEventHandle != NULL) ) 
  {
    /* After setting the abort flag, signal the "abortable" event being waited on */
    BKNI_SetEvent(h->pInternalAcquireParam->hCurrAbortableEventHandle);
  }
}

/***************************************************************************
 * BTHD_P_AbortAcquireReset()
 ***************************************************************************/
void BTHD_P_AbortAcquireReset ( BTHD_3x7x_ChnHandle h )
{
  /* Nullify the (now stale) "abortable" event handle */
  h->pInternalAcquireParam->hCurrAbortableEventHandle = NULL;
  /* Reset the "abort acquire" flag */
  h->pInternalAcquireParam->AbortAcquireRequested = false;
}

/***************************************************************************
 * BTHD_P_IsAbortAcquireRequested()
 ***************************************************************************/
bool BTHD_P_IsAbortAcquireRequested ( BTHD_3x7x_ChnHandle h )
{
  /* Get the "abort acquire" flag */
  return ( h->pInternalAcquireParam->AbortAcquireRequested );
}

/***************************************************************************
 * BTHD_P_WaitForEventOrAbort() 
 * Assumption: Multiple threads do not concurrently run a receiver's acquisition.
 *             If so, Abort-Early is merely prolonged by the longest timeout 
 *             among all events' concurrently pended during the acquire.
 ***************************************************************************/
BERR_Code BTHD_P_WaitForEventOrAbort ( BTHD_3x7x_ChnHandle h, BKNI_EventHandle event, int timeoutMsec )
{
  BERR_Code eResult = BERR_SUCCESS;

  /* BDBG_MSG(("BTHD_P_WaitForEventOrAbort")); */

  /* Always check for Abort-Early condition first.  Exit without waiting once the Abort condition has set. */
  if ( BTHD_P_IsAbortAcquireRequested(h) )
  {
    eResult = BTHD_ERR_ACQUIRE_ABORTED;
  }
  else
  {
    /* Copy event handle to be waited on. */
    /* This copy is signaled by the abort function on an Abort condition. */
    h->pInternalAcquireParam->hCurrAbortableEventHandle = event;

    /* First wait for the desired event */
    eResult = BKNI_WaitForEvent(event, timeoutMsec);

    /* Immediately nullify the event-handle copy so that the "Abort Acquire" function will no longer signal this event */
    h->pInternalAcquireParam->hCurrAbortableEventHandle = NULL;

    /* Check to see if the event was triggered for, or during, an Abort-Early condition */
    if ( BTHD_P_IsAbortAcquireRequested(h) )
    {
      /* On an Abort condition only: reset the event in case it subsequently set a second time for either an Abort or for the actual event itself. */
      BKNI_ResetEvent(event);
      eResult = BTHD_ERR_ACQUIRE_ABORTED;
    }
  }
  if ( eResult == BTHD_ERR_ACQUIRE_ABORTED )
  {
    BDBG_MSG(("BTHD_P_WaitForEventOrAbort: Early Exit"));
  }

  return ( eResult );
}

/***************************************************************************
 * BTHD_P_MapWaitForEventResult_To_THD_AcqResult()  
 ***************************************************************************/
BTHD_RESULT BTHD_P_MapWaitForEventResult_To_THD_AcqResult ( BERR_Code eEventWaitResult, BTHD_RESULT successCode, BTHD_RESULT failureCode )
{
  /* eEventWaitResult success: successCode, Failure: failureCode, or,
  * THD_AcquireResult_AbortedEarly if "Acquire Aborted Early" Event took place */

  /* Map BERR_Code from WaitForEvent() into THD result (depending on having successfully signaled the event, or not) */
  if ( eEventWaitResult == BERR_SUCCESS ) {
    return(successCode);
  }
  else
  {
    /* The event-wait returned "Acquire Aborted Early" failure code */
    if ( eEventWaitResult == BTHD_ERR_ACQUIRE_ABORTED ) {
      return(THD_AcquireResult_AbortedEarly);
    }
    return(failureCode);
  }
}

/***************************************************************************
 * BTHD_P_Acquire()  
 ***************************************************************************/
BERR_Code BTHD_P_Acquire(BTHD_3x7x_ChnHandle h)
{
  uint32_t  ReacquireCount;
  BERR_Code retCode;
#ifdef SmartTuneEnabled  
  int16_t TunerGain;
  uint16_t SmartTune;
  uint32_t RfFreq;
#endif
  
  BINT_DisableCallback(h->hRsSyncCallback);
  BINT_DisableCallback(h->hRsSyncLossCallback);

#ifdef BTHD_ISDBT_SUPPORT  
  BINT_DisableCallback(h->hRsSyncBCallback);
  BINT_DisableCallback(h->hRsSyncLossBCallback);
  BINT_DisableCallback(h->hRsSyncCCallback);
  BINT_DisableCallback(h->hRsSyncLossCCallback);
#endif

#ifdef SmartTuneEnabled  
  BTHD_3x7x_P_TNR_callback(h,BTHD_CallbackMode_eSmartTune,&TunerGain,&SmartTune,&RfFreq);
  if (SmartTune)
    h->pInternalAcquireParam->SampleFreq = 51300000;  
  else
    h->pInternalAcquireParam->SampleFreq = 54000000;  
  BDBG_MSG(("BTHD_P_Acquire: SmartTune = %d, Fs = %d MHz",SmartTune,h->pInternalAcquireParam->SampleFreq));
#endif    

  BTHD_P_ResetStatus(h);
  BTHD_P_ResetAll(h);
  BTHD_P_Config(h);
  BTHD_P_ResetAcquire(h);

  /* Reset lock flag(s), keeping the existing "No Signal" condition */
  h->ThdLockStatus &= (1U << THD_LockStatusBit_NoDVBTSignal);   


  /* Reset "Abort-Early" signal/flag when starting an acquire */
  BTHD_P_AbortAcquireReset(h);

#ifdef BTHD_ISDBT_SUPPORT  
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT)
    retCode = BTHD_P_IsdbtAcquire(h);
  else
#endif
  retCode = BTHD_P_DvbtAcquire(h);

  h->pStatus->ThdCommonStatus.spare[0] = retCode;

  h->pStatus->ThdCommonStatus.ReacquireCount++;

  switch (retCode) {
  case THD_AcquireResult_Lock:          BDBG_MSG(("\tTHD Lock\n"));            break;
  case THD_AcquireResult_NoFFTLock:     BDBG_MSG(("\tTHD No FFT Lock\n"));     break;
  case THD_AcquireResult_NoCarrierLock: BDBG_MSG(("\tTHD No Carrier Lock\n")); break;
  case THD_AcquireResult_NoSPLock:      BDBG_MSG(("\tTHD No SP Lock\n"));      break;
  case THD_AcquireResult_NoTPSLock:     BDBG_MSG(("\tTHD No TPS Lock\n"));     break;
  case THD_AcquireResult_NoFECLock:     BDBG_MSG(("\tTHD No FEC Lock\n"));     break;
  case THD_AcquireResult_NoSignal:      BDBG_MSG(("\tTHD No Signal\n"));       break;
  case THD_AcquireResult_NoDVBTSignal:  BDBG_MSG(("\tTHD No DVBT Signal\n"));  break;
  case THD_AcquireResult_AbortedEarly:  BDBG_MSG(("\tTHD Acq Aborted Early\n"));  break;
  default: BDBG_MSG(("BTHD_ThdTop:retCode = %d", retCode));
  }

  if ((retCode == THD_AcquireResult_NoSignal) || 
      (retCode == THD_AcquireResult_NoDVBTSignal) || 
      (retCode == THD_AcquireResult_NoSPLock))
  {
    h->ThdLockStatus |= (1U << THD_LockStatusBit_NoDVBTSignal);
    if (h->pAcquireParam->CommonAcquireParam.AcquireMode & THD_AcquireMode_Scan) {
      h->pAcquireParam->CommonAcquireParam.AcquireMode &= ~THD_AcquireMode_Scan;
#ifndef LEAP_BASED_CODE
	  	BKNI_EnterCriticalSection();
	    if(h->pCallback[BTHD_Callback_eLockChange] != NULL )
	        (h->pCallback[BTHD_Callback_eLockChange])(h->pCallbackParam[BTHD_Callback_eLockChange] );
	    BKNI_LeaveCriticalSection();
#else
      BKNI_SetEvent(h->hLockEvent);
#endif
      return retCode;
   }

   if (h->pAcquireParam->CommonAcquireParam.AcquireMode & THD_AcquireMode_Auto) 
   {
#ifndef LEAP_BASED_CODE	
	   BKNI_SetEvent(h->hIntEvent);
#else

	   BKNI_SetAnyEvent(h->hIntEvent, BKNI_TIMER_THD_FLAG);
#endif
		return retCode;
   }
  }
  else
  {
    h->ThdLockStatus &= (~(1U << THD_LockStatusBit_NoDVBTSignal));
  }

/*
  if ((retCode == BTHD_AcquireResult_NoSignal) || 
      (retCode == BTHD_AcquireResult_NoDVBTSignal) || 
      (retCode == BTHD_AcquireResult_NoTPSLock)  || 
      (retCode == BTHD_AcquireResult_NoFECLock))
  {
    if (h->pAcquireParam->CommonAcquireParam.AcquireMode & THD_AcquireMode_Scan) {
      h->pAcquireParam->CommonAcquireParam.AcquireMode &= ~THD_AcquireMode_Scan;
      BTHD_P_UpdateStatusChange(h, BCHP_LEAP_CTRL_HOST_IRQ_GOT_LOCK_MASK);
      return retCode;
    }
  }
*/
  /* If lock was achieved, then clear the status but preserve the reacquire count. */
  if ((retCode == THD_AcquireResult_Lock) && (retCode != THD_AcquireResult_InitLockState)) {    
    ReacquireCount = h->pStatus->ThdCommonStatus.ReacquireCount;
    BTHD_P_ResetStatusHW(h);
    BTHD_P_ResetStatus(h);
	h->pStatus->ThdCommonStatus.spare[0] = retCode;
    h->pStatus->ThdCommonStatus.ReacquireCount = ReacquireCount;
    BTHD_P_SetTs(h);
  }
#if 0
  /* Check for loss-of-lock in ce_mode=Fixed with timing FD enabled.  If lock is lost, try timing PD */
  if (!(h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT)) {
    /* Check for FEC lock */
    if (!((h->ThdLockStatus>> THD_LockStatusBit_SystemLock) & 1) && 
	        (h->pStatus->ThdCommonStatus.ChannelEstimatorMode == THD_ChannelEstimatorMode_Fixed) && 
	        !BREG_ReadField(h->hRegister,  THD_CORE_CP, PHASE_FREQ_SELECT )) {
      BDBG_MSG(("BTHD_ThdTop:\tLock fallback from timing FD to PD"));
      BREG_WriteField(h->hRegister, THD_CORE_CP, F_SPACE, 2 );                              /* 12-bin timing pd pilot spacing (mandatory for long-echo channels)*/
      BREG_WriteField(h->hRegister, THD_CORE_CP, PHASE_FREQ_SELECT, 1 );                    /* Timing pd enable*/
      BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_TPHASE_FRZ, 0 );                       /* Unfreeze timing pd*/  
      BREG_Write32(h->hRegister,  BCHP_THD_CORE_TL_COEF,0x00010200 * (2048/2048) );         /* Ki=2^(-13),Kl=2^(-4) (for CCI)*/
      BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_MISC,0x00000200 );                       /* FFT window leak to timing loop disabled (prevents "wandering" at low SNR)*/
    }
  }

  /* Check for lock */
  /*if((h->ThdLockStatus>> BTHD_LockStatusBit_SystemLock) & 1) {
    BTHD_P_UpdateStatusChange(h, BCHP_LEAP_CTRL_HOST_IRQ_GOT_LOCK_MASK);
  }*/ 
#endif
  BTHD_P_EnableLockInterrupts(h); 
  return retCode;
}

/***************************************************************************
 * BTHD_P_SetTs()
 ***************************************************************************/
void BTHD_P_SetTs(BTHD_3x7x_ChnHandle h)
{
	uint32_t TSMode=3;
	switch (h->pConfigParam->TSMode){
  	case THD_TSMode_Serial:     TSMode=3; break;  /* Serial smooth */
  	case THD_TSMode_Parallel:   TSMode=1; break;  /* Parallel smooth */
  	default:  BDBG_WRN(("BTHD_P_SetTs: THD_TSMode_None not supported")); break;
  }  
	BREG_Write32(h->hRegister, BCHP_THD_CORE_OI,(TSMode                               << 0) | 
	                                            (h->pConfigParam->TSInvertClock       << 4) |
	                                            (h->pConfigParam->TSInvertValid       << 5) |
	                                            (h->pConfigParam->TSInvertSync        << 6) |
	                                            (h->pConfigParam->TSInvertError       << 7) |
	                                            (h->pConfigParam->TSSuppressClock     << 8) |
	                                            (h->pConfigParam->TSSuppressData      << 10) |
	                                            (h->pConfigParam->TSSuppressClockEdge << 11) |
																						  (h->pConfigParam->TSSyncLength        << 12) |
	                                            (h->pConfigParam->TSHeaderLength      << 13) |
															                (h->pConfigParam->TSDeleteHeader      << 14));
	
}

/***************************************************************************
* BTHD_P_SetFrequency()
***************************************************************************/
void BTHD_P_SetFrequency(BTHD_3x7x_ChnHandle h)
{
  uint32_t bw, tl_fcw, tl_n, tl_d, cl_n;
  uint32_t cl_fcw=0,CenterFreq;
  uint32_t pouthi, poutlo, pouthi2;

  /* Set the baseband rate */
  bw = bthd_bandwidth[h->pAcquireParam->CommonAcquireParam.Bandwidth];
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    tl_n = 512000000 * bw / 3;      /* at bw = 6MHz, fcw = 2 * (512/63) MHz */
    tl_d = h->pInternalAcquireParam->SampleFreq * 63;
  }
  else {
    tl_n = (2 * 8000000) * bw;
    tl_d = h->pInternalAcquireParam->SampleFreq * 7;
  }
  /* tl_fcw = (tl_n * (1ULL << 32)) / tl_d; */
  BMTH_HILO_32TO64_Mul((tl_n * 2), 0x80000000, &pouthi, &poutlo);
  BMTH_HILO_64TO64_Div32(pouthi, poutlo, tl_d, &pouthi2, &tl_fcw);
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_TL_FCW, tl_fcw );

  /* Set the carrier frequency */
  if ((h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) && 
      (h->pAcquireParam->CommonAcquireParam.CenterFreq > h->pInternalAcquireParam->SampleFreq)) {                
    CenterFreq = h->pAcquireParam->CommonAcquireParam.CenterFreq - 45000000 - 11250000 + 13500000;
  } else
    CenterFreq = h->pAcquireParam->CommonAcquireParam.CenterFreq;

  cl_n   = CenterFreq ? (long)(h->pInternalAcquireParam->SampleFreq - CenterFreq) : 0L;
  /* cl_fcw = (cl_n * (1LL << 32)) / (long)h->pAcquireParam->CommonAcquireParam.SampleFreq; */
  BMTH_HILO_32TO64_Mul((cl_n * 2), 0x80000000, &pouthi, &poutlo);
  BMTH_HILO_64TO64_Div32(pouthi, poutlo, h->pInternalAcquireParam->SampleFreq, &pouthi2, &cl_fcw);
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_FCW, cl_fcw );

  /* BDBG_MSG(("BTHD_P_SetFrequency:\tTL_FCW = 0x%08x\n", tl_fcw)); */
  /* BDBG_MSG(("BTHD_P_SetFrequency:\tCL_FCW = 0x%08x\n", cl_fcw)); */
}

/***************************************************************************
 * BTHD_P_SetFrontEnd()
 ***************************************************************************/
void BTHD_P_SetFrontEnd(BTHD_3x7x_ChnHandle h)
{
  if (h->pInternalAcquireParam->FrontEndMode == THD_FrontEndMode_Baseband) {
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_GLB,0x00000028 );  /* Fs=54,Baseband input,BE=disabled,ICE=acquisition */
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_FE,0x00000008 );   /* I/Q aligned, posedge, spinv=0, 2's complement */
  } else {
    BDBG_MSG(("BTHD_SetFrontEnd:IF input mode"));
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_GLB,0x00000020 );  /* Fs=54,IF input,BE=disabled,ICE=acquisition */
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_FE,0x00000008 );   /* I/Q aligned, posedge, spinv=0, 2's complement */
  }
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT)			
    BREG_WriteField(h->hRegister, THD_CORE_GLB, STD, 2);         /* ISDBT Mode */
}

/***************************************************************************
 * BTHD_P_SetNotch()
 ***************************************************************************/
void BTHD_P_SetNotch(BTHD_3x7x_ChnHandle h)
{
  uint32_t bw,fcw;
  int32_t x[4],y,n,num_notches;
  uint32_t pouthi, poutlo, poutlo2, utmp, tmp;

#ifdef SmartNotchEnabled  
  int16_t TunerGain;
  uint16_t SmartTune;
  uint32_t RfFreq;
  int32_t SmartNotchFreq;
  int32_t SmartNotchNum = 6;
  int32_t SmartNotchFreqList[6] = {198000000,216000000,506000000,540000000,675000000,844000000};
#endif

  /* Set the baseband rate */
  bw = bthd_bandwidth[h->pAcquireParam->CommonAcquireParam.Bandwidth];
  switch(h->pAcquireParam->CommonAcquireParam.CoChannelMode) {
  case THD_CoChannelMode_None:  break;
  case THD_CoChannelMode_Auto:
    if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
      /* NTSC Co-channel */
      x[0] = -1893000; /* NTSC picture carrier */             
      x[1] =  1687000; /* NTSC color carrier */
      x[2] =  2607000; /* NTSC FM audio carrier */
      num_notches = 3;
    }
    else {
      /* PAL Co-channel */
      x[0] = -2750000 + (long)(500000*(8-bw)); /* PALB/G picture  carrier {-2.75,-2.25,-1.75} MHz for BW={8,7,6} MHz */
      x[1] =  1550000 + (long)(500000*(8-bw)); /* PALB/G color    carrier { 1.55, 2.05, 2.55} MHz for BW={8,7,6} MHz */
      x[2] =  2750000 + (long)(500000*(8-bw)); /* PALB/G FM audio carrier { 2.75, 3.25, 3.75} MHz for BW={8,7,6} MHz */
      num_notches = 3;
    }
#ifdef SmartNotchEnabled  
  	BTHD_3x7x_P_TNR_callback(h,BTHD_CallbackMode_eRequestMode,&TunerGain,&SmartTune,&RfFreq);
		for (n=0; n<SmartNotchNum; n++) {
 			SmartNotchFreq = SmartNotchFreqList[n] - RfFreq;	
 			/* BDBG_MSG(("{%d,%d,%d,%d}",n,RfFreq,SmartNotchFreqList[n],SmartNotchFreq)); */
 			if ((SmartNotchFreq >= -(int32_t)((bw*1000000)>>1)) && (SmartNotchFreq <= ((int32_t)(bw*1000000)>>1)) && (num_notches < 4)) {
 				x[num_notches] = SmartNotchFreq;
   			num_notches++; 
   			h->pInternalAcquireParam->SmartNotchPresent = true;
   			break; 	
    	}
		}
#endif    

    /* Compute FCWs */
    for (n=0; n<num_notches; n++) {
      if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {      
        /* Hard-coded for 6MHz channels */
        if (n) {
          if ((x[n]-x[n-1]) < 0) {
            tmp = ~(x[n]-x[n-1]);		
            tmp = tmp + 1;		
          } else
            tmp = (x[n]-x[n-1]);

          BMTH_HILO_32TO64_Mul(tmp, (0x1000000 * 63), &pouthi, &poutlo);
          BMTH_HILO_64TO64_Div32(pouthi, poutlo, 512000000, &utmp, &poutlo2);
          if ((x[n]-x[n-1]) < 0) {
            y = ~poutlo2;
            y = y + 1;	
          }
          else
            y = poutlo2;
          /*y = ((x[n]-x[n-1]) * (1LL<<24) * 63)/(512000000);*/
        } else {
          if ((x[n]-0     ) < 0) {
            tmp = ~(x[n]-0);		
            tmp = tmp + 1;		
          } else
            tmp = (x[n]-0     );

          BMTH_HILO_32TO64_Mul(tmp, (0x1000000 * 63), &pouthi, &poutlo);
          BMTH_HILO_64TO64_Div32(pouthi, poutlo, 512000000, &utmp, &poutlo2);
          if ((x[n]-0) < 0) {
            y = ~poutlo2;
            y = y + 1;	
          } else
            y = poutlo2;
          /*y = ((x[n]-0     ) * (1LL<<24) * 63)/(512000000);	*/
        }
      } else {
        if (n) {
          if ((x[n]-x[n-1]) < 0) {
            tmp = ~(x[n]-x[n-1]);		
            tmp = tmp + 1;		
          } else
            tmp = (x[n]-x[n-1]);

          BMTH_HILO_32TO64_Mul(tmp, (0x1000000 * 7), &pouthi, &poutlo);
          BMTH_HILO_64TO64_Div32(pouthi, poutlo, (bw*8000000), &utmp, &poutlo2);
          if ((x[n]-x[n-1]) < 0) {
            y = ~poutlo2;
            y = y + 1;	
          } else
            y = poutlo2;
          /*y = ((x[n]-x[n-1]) * (1LL<<24) * 7)/(bw*8000000);*/
        } else {
          if ((x[n]-0) < 0) {
            tmp = ~(x[n]-0);		
            tmp = tmp + 1;		
          } else
            tmp = (x[n]-0);
          BMTH_HILO_32TO64_Mul(tmp, (0x1000000 * 7), &pouthi, &poutlo);
          BMTH_HILO_64TO64_Div32(pouthi, poutlo, (bw*8000000), &utmp, &poutlo2);
          if ((x[n]-0) < 0) {
            y = ~poutlo2;
            y = y + 1;	
          } else
            y = poutlo2;
          /*y = ((x[n]-0     ) * (1LL<<24) * 7)/(bw*8000000);	*/
        }
      }
      if (y < 0)
        fcw = ((uint32_t)((1U << 24) + y)) << 8;
      else
        fcw = ((uint32_t)y) << 8; 
      switch(n) {
      case 0: BREG_Write32(h->hRegister,  BCHP_THD_CORE_NOTCH0_FCW, fcw ); break;
      case 1: BREG_Write32(h->hRegister,  BCHP_THD_CORE_NOTCH1_FCW, fcw ); break;
      case 2: BREG_Write32(h->hRegister,  BCHP_THD_CORE_NOTCH2_FCW, fcw ); break;
      case 3: BREG_Write32(h->hRegister,  BCHP_THD_CORE_NOTCH3_FCW, fcw ); break;
      default: break;
      }
    }

    /* Enable notch filter */
    BREG_WriteField(h->hRegister, THD_CORE_BYP, NOTCH_BYP, 0);  
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_NOTCH_DEPTH,0x00000aaa );        /* Acquisition notch depth and width */
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_NOTCH_WIDTH,0x00000ddd );                        
#ifdef SmartNotchEnabled
    BREG_WriteField(h->hRegister,  THD_CORE_NOTCH_DEPTH, DEPTH3, 0xa );
    BREG_WriteField(h->hRegister,  THD_CORE_NOTCH_WIDTH, WIDTH3, 0xd );                        
#endif
    /* BDBG_MSG(("BTHD_P_SetNotch:\tTHD_CORE_NOTCH0_FCW = 0x%08x", BREG_Read32(h->hRegister,  BCHP_THD_CORE_NOTCH0_FCW)); */
    /* BDBG_MSG(("BTHD_P_SetNotch:\tTHD_CORE_NOTCH1_FCW = 0x%08x", BREG_Read32(h->hRegister,  BCHP_THD_CORE_NOTCH1_FCW)); */
    /* BDBG_MSG(("BTHD_P_SetNotch:\tTHD_CORE_NOTCH2_FCW = 0x%08x", BREG_Read32(h->hRegister,  BCHP_THD_CORE_NOTCH2_FCW)); */

    break;        
  case THD_CoChannelMode_Other: break;
  default: break;
  }
  return;
}

/***************************************************************************
 * BTHD_P_SetTransGuard()
 *** ************************************************************************/
THD_TransGuardResult_t BTHD_P_SetTransGuard(BTHD_3x7x_ChnHandle h)
{
  uint32_t n,d,maxN=0,maxD=0,N;
  uint32_t curCorr, maxMinRatio, maxThreshold;
  uint32_t maxCorr = 0; /* maxCorr initialization */
  uint32_t minCorr[3];
  THD_TransGuardResult_t mode_guard_result = THD_TransGuardResult_None;
  BERR_Code retCode = BERR_SUCCESS;
  uint32_t nMin = 0, nMax = 0, nInc = 1;
  uint32_t dMin = 0, dMax = 3;
  uint32_t length;

  switch (h->pAcquireParam->CommonAcquireParam.TransGuardMode) {
    case THD_TransGuardMode_Auto_DVBT:  nMin = 0; nMax = 1; nInc = 1; break;
    case THD_TransGuardMode_Auto_ISDBT: nMin = 1; nMax = 2; nInc = 1; break;
    case THD_TransGuardMode_Auto:       nMin = 0; nMax = 2; nInc = 1; break;
    default: break;
  }

  if (h->pAcquireParam->CommonAcquireParam.TransGuardMode != THD_TransGuardMode_Manual) {
    /* FFT window max correlation,postcursor bias,disable fft,sync_symbols=32,beta=2^(-3),slip_limit=0 */
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW,0x00a01020 );                               
    for (n=nMin; n<=nMax; n+=nInc) {
      minCorr[n] = 0x7fffffff; /* minCorr initialization */
      BREG_WriteField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE, n);

      /* Define Guard Interval search range */
      if (h->pAcquireParam->CommonAcquireParam.TransGuardMode == THD_TransGuardMode_Auto_ISDBT)
        switch (n) {
	        case 1:   dMin = 1; dMax = 3; break; /* 8K: {1/4, 1/8, 1/16} */
	        case 2:   dMin = 2; dMax = 3; break; /* 4K: {1/4, 1/8} */
	        default:  dMin = 0; dMax = 3; break;
	      }
      else {                                 /* Default: {1/4, 1/8, 1/16, 1/32} */
        dMin = 0;
        dMax = 3;
      }

      for (d=dMin; d<=dMax; d++) {
        BREG_WriteField(h->hRegister, THD_CORE_TPS_OV, GUARD, d);
        switch (n) {
	        case 0:  length = 2048 >> (5-d); break;
	        case 1:  length = 8192 >> (5-d); break;
	        case 2:  length = 4096 >> (5-d); break;
	        default: length = 2048 >> (5-d); break;
        }
        BREG_WriteField(h->hRegister, THD_CORE_FW_SEARCH, MVG_SUM_LENGTH, length); /* L=D */ 
        BREG_WriteField(h->hRegister, THD_CORE_RST, FW_RST, 0);
		BINT_EnableCallback( h->hFwCorrMaxCallback);
        retCode = BTHD_P_WaitForEventOrAbort(h, h->hFwCorrMaxEvent, 500);
        if (retCode != BERR_SUCCESS) {
          /*BDBG_MSG(("BTHD_SetTransGuard:\tNo hFwCorrMaxEvent")); */
        }

		BINT_DisableCallback( h->hFwCorrMaxCallback);
        curCorr = BREG_Read32(h->hRegister,  BCHP_THD_CORE_FW_CORR_MAX ) >> d;

        if (curCorr > maxCorr) {
	        /* Update maxCorr, maxN, maxD */
          maxCorr = curCorr;
          maxN = n;
          maxD = d;
        }
        if (curCorr < minCorr[n])
          minCorr[n] = curCorr; /* Update minCorr */

        BREG_WriteField(h->hRegister, THD_CORE_RST, FW_RST, 1);
        /*BDBG_MSG(("N=%d,D=%d,Cur=%08x,Max=%08x,Min=%08x,maxN=%d,maxD=%d",n,d,curCorr,maxCorr,minCorr[n],maxN,maxD); */
      }
    }
    
    maxMinRatio = minCorr[maxN] * h->pInternalAcquireParam->TransGuardMaxMinRatioThreshold[maxD];

    /* Normalize max correlation threshold relative to current DAGC threshold. 
       For consistency with previous chips (2930,3556), h->pAcquireParam->CommonAcquireParam.TransGuardMaxThreshold 
       is defined assuming a DAGC threshold=14dB=0x028c4 ((uint32_t)(pow(10.0,-14/10)* 262144)). */
    maxThreshold = (h->pInternalAcquireParam->TransGuardMaxThreshold / 0x028c4) * BREG_ReadField(h->hRegister, THD_CORE_DAGC, THRESH);

    /* Threshold scaling based on mode (2k,4k,8k) */
    switch (maxN) {
      case 1: maxThreshold = 3* maxThreshold ; break;       /* 8K (multiply by 3) */
      case 2:   maxThreshold = (maxThreshold >> 1) ; break; /* 4K (multiply by 2) */
      default:  break;
    }

    if ((maxCorr < maxThreshold) && ((maxCorr << 4) < maxMinRatio))
      mode_guard_result = THD_TransGuardResult_NoSignal;
    else
      if ((maxCorr << 4) < maxMinRatio)
        mode_guard_result = THD_TransGuardResult_CoChannelPresent;
    
    BREG_WriteField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE, maxN);
    BREG_WriteField(h->hRegister, THD_CORE_TPS_OV, GUARD, maxD);
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW, 0x00202050 );
    BREG_WriteField(h->hRegister, THD_CORE_FW_SEARCH, MVG_SUM_LENGTH, 8); /* L=8*/

    N = bthd_transmission_mode[maxN];
    BTHD_P_OSleep(h,2,N,maxD);
    /* Reset BE and FFT (Necessary to improve acquisition probability) */
    BREG_WriteField(h->hRegister, THD_CORE_RST, BE_RST, 1 ); 
    BREG_WriteField(h->hRegister, THD_CORE_RST, BE_RST, 0 );      
    BREG_WriteField(h->hRegister, THD_CORE_RST, FFT_RST, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST, FFT_RST, 0 );
    BREG_WriteField(h->hRegister, THD_CORE_FW, OUT_ENABLE, 1);

    /*BDBG_MSG(("BTHD_SetTransGuard:\tN=%d, D=%d", maxN, maxD); */
  }
  return mode_guard_result;
}

/***************************************************************************
 * BTHD_P_SetCE()
 ***************************************************************************/
void BTHD_P_SetCE( BTHD_3x7x_ChnHandle h, 
		   THD_ChannelEstimatorMode_t ChannelEstimatorMode)
{

  switch (ChannelEstimatorMode) {
  case THD_ChannelEstimatorMode_Fixed:  
  
    if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) 
		BREG_WriteField(h->hRegister, THD_CORE_CE, AVG_FF, 0x4);        /* CE averager beta = 2^(-5), -HZ, changed for DirecTV AWGN */    
	else
		BREG_WriteField(h->hRegister, THD_CORE_CE, AVG_FF, 0x3);        /* CE averager beta = 2^(-5), -HZ, changed for DirecTV AWGN */

	BREG_WriteField(h->hRegister, THD_CORE_BYP, CE_AVG_BYP, 0);     /* CE averager unbypassed */
    BREG_WriteField(h->hRegister, THD_CORE_EQ, NSE_AVG_FF, 0x5 );   /* EQ noise averager beta=2^(-8) */
    BREG_WriteField(h->hRegister, THD_CORE_BYP, EXP_AVG_NSE, 0x0 ); /* EQ nse + avg nse exponents unbypassed */
    break;
  case THD_ChannelEstimatorMode_Pedestrian: 
    BREG_WriteField(h->hRegister, THD_CORE_CE, AVG_FF, 0x0);        /* CE averager beta = 2^(-1) */
    BREG_WriteField(h->hRegister, THD_CORE_BYP, CE_AVG_BYP, 0);     /* CE averager bypassed */
    BREG_WriteField(h->hRegister, THD_CORE_EQ, NSE_AVG_FF, 0x5 );   /* EQ noise averager beta=2^(-8) */
    BREG_WriteField(h->hRegister, THD_CORE_BYP, EXP_AVG_NSE, 0x0 ); /* EQ nse + avg nse exponents unbypassed */
    break;
  case THD_ChannelEstimatorMode_Mobile:   
    BREG_WriteField(h->hRegister, THD_CORE_CE, AVG_FF, 0x0);        /* CE averager beta = 2^(-1) */
    BREG_WriteField(h->hRegister, THD_CORE_BYP, CE_AVG_BYP, 1);     /* CE averager bypassed */
    BREG_WriteField(h->hRegister, THD_CORE_EQ, NSE_AVG_FF, 0x5 );   /* EQ noise averager beta=2^(-8) */
    BREG_WriteField(h->hRegister, THD_CORE_BYP, EXP_AVG_NSE, 0x3 ); /* EQ nse + avg nse exponents unbypassed */
    break;
  default:
    BDBG_WRN(("BTHD_SetCE: BTHD_ChannelEstimatorMode_Auto not supported")); 
    break;
  }    

  /*
  switch (ChannelEstimatorMode) {
  case BTHD_ChannelEstimatorMode_Fixed:      BDBG_MSG(("BTHD_P_SetCE:\t\tFixed")); break;
  case BTHD_ChannelEstimatorMode_Pedestrian: BDBG_MSG(("BTHD_P_SetCE:\t\tPedestrian")); break;
  case BTHD_ChannelEstimatorMode_Mobile:     BDBG_MSG(("BTHD_P_SetCE:\t\tMobile")); break;
  default:                                   BDBG_WRN(("BTHD_P_SetCE: BTHD_ChannelEstimatorMode_Auto not supported")); break;
  }
  */
}

/***************************************************************************
 * BTHD_P_WriteTICoef()
 ***************************************************************************/
void BTHD_P_WriteTICoef( BTHD_3x7x_ChnHandle h)
{
  /* time_interp_polyphase_1pre_3post_1_16_15dB (1/16) */
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF01,  0x0f8922af);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF23,  0x3cf55eb4);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF45,  0x5a9a4ede);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF67,  0x324b02d5);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF89,  0xf51beb17);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF1011,0xecdafd40);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF1213,0x00a6034b);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF14,  0x03d00000);

  /* 2-tap time interpolator coefficients */
  /*
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF01,  0x18003000);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF23,  0x48006000);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF45,  0x48003000);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF67,  0x18000000);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF89,  0x00000000);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF1011,0x00000000);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF1213,0x00000000);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_TCOEF14,  0x00000000);
  */
}   

/***************************************************************************
 * BTHD_P_WriteFICoef()
 ***************************************************************************/
void BTHD_P_WriteFICoef( BTHD_3x7x_ChnHandle h, 
			 THD_FrequencyInterpolatorMode_t FrequencyInterpolatorMode)
{
  int ndx;
  uint32_t coef;

  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FI_ADDR, 0 );
  for (ndx = 0; ndx < THD_FrequencyInterpolatorCoefLength; ++ndx) {
    coef = bthd_freq_coef_table[ FrequencyInterpolatorMode*THD_FrequencyInterpolatorCoefLength + ndx ];
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_FI_COEF, coef );
  }
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FI, 0x00000000);
}

#ifdef BTHD_ISDBT_SUPPORT 
/***************************************************************************
* BTHD_P_GetChannelSpan()
***************************************************************************/
BTHD_RESULT BTHD_P_GetChannelSpan(BTHD_3x7x_ChnHandle h, THD_FFTWindowMode_t FFTWindowMode, THD_TransmissionMode_t TransmissionMode,THD_GuardInterval_t GuardInterval)
{ 
  bool bInSpan = false;

  /*------- Variables -------*/
  /* General */
  uint32_t n,k,idx,value,start=0;
  uint16_t K,N,D,guard;
  uint32_t fscntInit = BREG_Read32(h->hRegister, BCHP_THD_CORE_FSCNT);
  
  /* Channel Estimate */
   int16_t ceI[256],ceQ[256],ce_num_avg=4, z;
  
  /* Channel Impulse Response */
  int32_t  cirI,cirQ;
   uint32_t cir[256];

  /* Peak Detection */
  uint32_t paths=0;
  uint32_t cir_max_val=0,cir_max_idx=0,cir_thresh,thresh=8; /* thresh_dB = thresh*3dB = 24dB */
  uint32_t start_idx, cur_max, idx_max;
  uint32_t peak_val[128];
  int16_t peak_location[128], peak_width[128];	

  /* Span */
  int16_t span=0;
  uint32_t right_edge_idx=0;
  
  /*---------- Algorithm --------*/	
  /* Get channel estimates for center 256 pilots using snooper */
  if (TransmissionMode == THD_TransmissionMode_2k) 
    {start = 468; K = 1705; N = 2048;} 
  else 
    {start = 3024; K = 6817;N = 8192;}  
  for (z=0; z<ce_num_avg; z++) {   /* Average CE over multiple captures to improve SNR */
    BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_RECORD_CFG,0x01030000 + start );  /* Capture CE, snapshot mode, step=3 */
    /* while (!BREG_ReadField(h->hRegister, THD_CORE_CE_RECORD_CFG, DONE)); */
    /* Wait 1 second for internal buffer to fill */
    if ( ! BTHD_P_Wait_CE_Recording_Status_Done(h, 1000) ) {
      goto BTHD_P_GetChannelSpan_Abort;
    }
    BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_READ_INDEX,0x00000000 );
    for (n=0; n<256; n++) {
      value = BREG_Read32(h->hRegister, BCHP_THD_CORE_CE_READ_DATA );
      if (z == 0) {
        ceI[n] = (int16_t)(((value >> 16) & 0xfff) << 4)/ce_num_avg;
        ceQ[n] = (int16_t)(((value >>  4) & 0xfff) << 4)/ce_num_avg;        
      } else {
        ceI[n] += (int16_t)(((value >> 16) & 0xfff) << 4)/ce_num_avg;
        ceQ[n] += (int16_t)(((value >>  4) & 0xfff) << 4)/ce_num_avg;         
      }
    }    
  }

  /* Compute CIR using IDFT (should be changed to IFFT for better efficiency) */
  for (k=0; k<256; k++) {
    /* Compute DFT */
    cirI = 0;
    cirQ = 0;
    for (n=0; n<256; n++) {
      idx = (k*n) % 256;
      cirI += ((int32_t)( ceI[n]*dft_table[idx][0]) + (int32_t)(ceQ[n]*dft_table[idx][1])) >> 8;
      cirQ += ((int32_t)(-ceI[n]*dft_table[idx][1]) + (int32_t)(ceQ[n]*dft_table[idx][0])) >> 8;
    }
    cirI = cirI >> 16;
    cirQ = cirQ >> 16;

    /* Compute Power */
    cir[k] = (uint32_t)(cirI*cirI + cirQ*cirQ);

    /* Compute Max */
    if (cir[k] > cir_max_val) {
      cir_max_val = cir[k];
      cir_max_idx = k;
    }
  }
	/* BDBG_MSG(("\t\tMax of CIR at index:%d , height: %d",cir_max_idx,cir_max_val)); */

  /* Determine number of paths in CIR */
  cir_thresh = cir_max_val >> thresh;
  start_idx = 0; cur_max = cir[0]; idx_max =0;
  for (k=1; k<256; k++) {
	  if (cir[k] > cir_thresh && cir[k-1] <= cir_thresh) {
		  start_idx=k;
		  cur_max = cir[k];
		  idx_max = k;
	  } else if (cir[k] > cir_thresh && cir[k-1] >= cir_thresh) {
		  if (cir[k] > cur_max) {
			  cur_max = cir[k];
			  idx_max = k;
		  }
	  } else if (cir[k] < cir_thresh && cir[k-1] >= cir_thresh){
  		peak_val[paths] = cur_max;
  		peak_width[paths] = k-1-start_idx;
  		peak_location[paths] = idx_max;
  		/* BDBG_MSG(("\t\tPath at Index:%d, height: %d, width: %d",peak_location[paths],peak_val[paths], peak_width[paths])); */
  		paths++;
	  } else {
		  /* Do Nothing */
	  }		 
  }

  /* Determine guard interval */
  switch (GuardInterval) {
    case THD_GuardInterval_1_32: D = N/32; break;
    case THD_GuardInterval_1_16: D = N/16; break;
    default: BDBG_MSG(("Unrecognized GuardInterval - defaulting to BTHD_GuardInterval_e1_8"));
    /* fall through */
    case THD_GuardInterval_1_8: D = N/8; break;
    case THD_GuardInterval_1_4: D = N/4; break;
  }
  guard = (uint16_t)((256*D*3)/N);
    
  /* Determine span */
  if (paths > 1) {
    if (FFTWindowMode == THD_FFTWindowMode_OutOfSpanPre) {
      for (k=0; k<paths-1; k++)
        if ((peak_location[k] <= guard) && (peak_location[k+1] > guard))    
          right_edge_idx = k;
      span = peak_location[right_edge_idx] + (256 - peak_location[right_edge_idx+1]);
    } else
  	  span = peak_location[paths-1] - peak_location[0];    
  }
  
  /* Convert span to microseconds and save to status structure */
  switch (h->pAcquireParam->CommonAcquireParam.Bandwidth) {
    case THD_Bandwidth_8MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*8); break;
    case THD_Bandwidth_7MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*7); break;
    case THD_Bandwidth_6MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*6); break;
    case THD_Bandwidth_5MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*5); break;
    default: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*8); break;        
  }

  if (span <= (15*guard/16))
    bInSpan = true; 
  else
    bInSpan = false; 

  goto BTHD_P_GetChannelSpan_Exit;


BTHD_P_GetChannelSpan_Abort:
  bInSpan = false;
  BDBG_MSG(("\t\tBTHD_P_GetChannelSpan_Abort (timeout)"));
  goto BTHD_P_GetChannelSpan_Exit;

BTHD_P_GetChannelSpan_Exit:
  BDBG_MSG(("\t\tPaths     : %d",paths));
  BDBG_MSG(("\t\tSpan      : %dus (%d)",h->pStatus->ThdCommonStatus.ChannelSpan,span));
  BDBG_MSG(("\t\tTime      : %dms",(fscntInit - BREG_Read32(h->hRegister, BCHP_THD_CORE_FSCNT))/(h->pInternalAcquireParam->SampleFreq/1000)));

  return (bInSpan);
}
#endif

/***************************************************************************
* BTHD_P_GetFFTTriggerPosition()
***************************************************************************/
bool BTHD_P_GetFFTTriggerPosition(BTHD_3x7x_ChnHandle h, THD_TransmissionMode_t TransmissionMode,THD_GuardInterval_t GuardInterval)
{ 
  /* completed GetFFTTriggerPosition successfully */
  bool bCompletedSuccess = false;

  /*------- Variables -------*/
  /* General */
  uint32_t i,j;
  uint32_t n,k,idx,value,start=0;
  uint16_t K,N,D,m;
 /* uint32_t fscntInit = BREG_Read32(h->hRegister, BCHP_THD_CORE_FSCNT);*/

  /* Channel Estimate */
  int16_t z, ceI[256], ceQ[256],ce_num_avg=4;
  
  /*FFT Output*/
  int16_t fftI,fftQ;

  /* Channel Impulse Response */
  int32_t  cirI,cirQ;
  uint32_t cir[256];

  /* Peak Detection */
  bool     peak_det = false;  
  uint32_t paths=0;
  uint32_t cir_max_val=0,cir_max_idx=0,cir_thresh,thresh=7; /* thresh_dB = thresh*3dB = 24dB */
  uint32_t start_idx, cur_max, idx_max;
  uint32_t peak_val[64];
  uint16_t peak_location[64], peak_width[64];	

  /* First Peak Detection */
  int32_t left_edge=8; 
  uint16_t tps_num = 6,tps_loc, tps_index, tps_num_avg=4, ce_one_k; /* keep tps_num to 6 after talking to Jeff p*/
  uint16_t tps_loc_2k[7] = {569, 595, 688, 790, 901,1073,1219};
  uint16_t tps_loc_8k[7] = {3173,3298,3391,3442,3458,3617,3754};
  #ifndef BCHP_THD_CORE_V_4_0
  uint16_t tps_start_2k = 5 ;
  uint16_t tps_start_8k = 31;
  uint16_t num_tps_read;
  #ifdef BTHD_ISDBT_SUPPORT
  uint16_t ac1_start_8k = 93; /* 5065-11*432=313, 8*11+5=93 */
  uint16_t ac1_start_4k = 40; /* 2236-10*216=76, 4*10=40 */
  uint16_t ac1_start_2k = 18; /* 1012-9*108=40, 2*9=18 */
  #endif /* BTHD_ISDBT_SUPPORT */
  #endif 
  int32_t slope=0, slope_scaled=0; 
  int32_t aI, aQ, bI, bQ, cI, cQ, dI, dQ;
  int32_t error[64], error_min, error_tmp;
  uint16_t error_min_idx;
  int16_t phase_idx,exp_idx,flt_idx;

   /* Span */
  int16_t span=0;
  bool missed_first_peak = false;
  bool out_of_span = false;
  bool in_span = false;  
  bool on_guard = false;  
  
  /* Minimum ISI */
  /*uint32_t circirc[256];*/
  int32_t guard, x, y;
  uint32_t isi_pre, isi_post, isi_y, isi_min;
  uint32_t isi_min_offset=0;	

  /* Offset and Slope Calculation */
  int32_t offset_no_modulo=0, offset=0, offset_scaled=0;

  int32_t tps_rotI[7], tps_rotQ[7];

  #ifdef BTHD_ISDBT_SUPPORT
  /* For ISDB-T, use AC1 instead of TPS */
  /* use center_segs */
  /*
  uint16_t ac1_loc_2k[7] = {614, 640, 683, 727, 832, 853, 868};
  uint16_t ac1_loc_4k[7] = {1277, 1394, 1397, 1414, 1432, 1565, 1595};
  uint16_t ac1_loc_8k[7] = {2681, 2798, 2801, 2818, 2836, 2969, 2999}; 
  */
  /* uint16_t ac1_loc_8k[9] = {2548, 2599, 2681, 2798, 2801, 2818, 2836, 2969, 2999}; */
  /* uint16_t ac1_loc_8k[11] = {2465, 2492, 2548, 2599, 2681, 2798, 2801, 2818, 2836, 2969, 2999}; */
  /* uint16_t ac1_loc_8k[14] = {2465,2492,2548,2599,2681,2798,2801,2818,2836,2969,2999,3085,3124,3143}; */
  /* Use right_segs: 8K: #12, #10,  4K: #8,#10.  2K: #12,#10,#8,#6 */
  uint16_t ac1_loc_2k[7] = {1012, 1061, 1088, 1144, 1195, 1277, 1394};
  uint16_t ac1_loc_4k[7] = {2236, 2257, 2272, 2357, 2416, 2465, 2492};
  uint16_t ac1_loc_8k[7] = {5065, 5080, 5165, 5224, 5273, 5300, 5356}; 
  /* uint16_t ac1_loc_8k[9] = {4939, 5044, 5065, 5080, 5165, 5224, 5273, 5300, 5356}; */
  /* uint16_t ac1_loc_8k[11] = {4852, 4895, 4939, 5044, 5065, 5080, 5165, 5224, 5273, 5300, 5356}; */ 
  /* uint16_t ac1_loc_8k[11] = {4895, 4939, 5044, 5065, 5080, 5165, 5224, 5273, 5300, 5356, 5407}; */
  /* uint16_t ac1_loc_8k[12] = {4852, 4895, 4939, 5044, 5065, 5080, 5165, 5224, 5273, 5300, 5356, 5407}; */
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
	    tps_num = 7;  
  }
  #endif
  
  /*---------- Algorithm --------*/	
/*  BDBG_MSG(("\t\t{%d,%d,%d}",BREG_Read32(h->hRegister, BCHP_THD_CORE_FW_OFFSET),BREG_Read32(h->hRegister, BCHP_THD_CORE_FW_CORR_INDEX),BREG_Read32(h->hRegister, BCHP_THD_CORE_FW_ABS_MIN_INDEX))); */

  /* Get channel estimates for center 256 pilots using snooper */
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
/* last 256*3 tones*/
    if (TransmissionMode == THD_TransmissionMode_8k) 
      {start = 4764; K=5617; N = 8192;} /* use seg 11+12 */ 
	    /* {start = 2424; K=5617; N = 8192;} */ /* use center 256*3 bins */
	else if (TransmissionMode == THD_TransmissionMode_4k)
	  {start = 1956; K=2809; N = 4096;}
	else 
	  {start = 318; K=1405; N = 2048;}
  }	else {
#endif
  if (TransmissionMode == THD_TransmissionMode_2k) 
    {start = 468; K = 1705; N = 2048;} 
  else 
    {start = 3024; K = 6817;N = 8192;}  

#ifdef BTHD_ISDBT_SUPPORT
  }
#endif

  /* calculate guard interval length */
  switch (GuardInterval) {
    case THD_GuardInterval_1_32: D = N/32; break;
    case THD_GuardInterval_1_16: D = N/16; break;
    default: BDBG_MSG(("Unrecognized GuardInterval - defaulting to BTHD_GuardInterval_e1_8"));
    /* fall through */
    case THD_GuardInterval_1_8: D = N/8; break;
    case THD_GuardInterval_1_4: D = N/4; break;
  }
  guard = (uint16_t)((256*D*3)/N);

  for (z=0; z<ce_num_avg; z++) {   /* Average CE over multiple captures to improve SNR */
    BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_RECORD_CFG,0x01030000 + start );  /* Capture CE, snapshot mode, step=3 */
    /* Wait 10 msec for internal buffer to fill */
    if ( ! BTHD_P_Wait_CE_Recording_Status_Done(h, 10) ) {
        goto BTHD_P_GetFFTTriggerPosition_Abort;
    }
    BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_READ_INDEX,0x00000000 );
    for (n=0; n<256; n++) {
      value = BREG_Read32(h->hRegister, BCHP_THD_CORE_CE_READ_DATA );
      if (z == 0) {
        ceI[n] = (int16_t)(((value >> 16) & 0xfff) << 4)/ce_num_avg;
        ceQ[n] = (int16_t)(((value >>  4) & 0xfff) << 4)/ce_num_avg;        
      } else {
        ceI[n] += (int16_t)(((value >> 16) & 0xfff) << 4)/ce_num_avg;
        ceQ[n] += (int16_t)(((value >>  4) & 0xfff) << 4)/ce_num_avg;         
      }
    }    
  }

  /* Early-exit abort triggered ? */
  if ( BTHD_P_IsAbortAcquireRequested(h) ) {
    goto BTHD_P_GetFFTTriggerPosition_Abort;
  }

  /* Average TPS error for each path over multiple captures to improve SNR */
  for (k=0; k<tps_num_avg; k++) { 
	#ifdef BCHP_THD_CORE_V_4_0
    /* Get fft data for center 256*3 carriers using snooper */
    for (m=0; m<3; m++ ) {
  		BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_RECORD_CFG,0x09010000 + start + m*256 );  /* Capture CE, snapshot mode, step=1 */
        /* Wait 10 msec for internal buffer to fill */
  	    if ( ! BTHD_P_Wait_CE_Recording_Status_Done(h, 10) )  {
          goto BTHD_P_GetFFTTriggerPosition_Abort;
        }
  		BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_READ_INDEX,0x00000000 );
  		for (n=0; n<256; n++) {
  			value = BREG_Read32(h->hRegister, BCHP_THD_CORE_CE_READ_DATA );
  			fftI = (int16_t)(((value >> 16) & 0xfff) << 4);
  			fftQ = (int16_t)(((value >>  4) & 0xfff) << 4);
  			if (k)
  			  h->fftpwr[m*256 + n] += (fftI*fftI + fftQ*fftQ);
  			else
   			  h->fftpwr[m*256 + n] = (fftI*fftI + fftQ*fftQ); 			
  		}
    }
	#else
	#ifdef BTHD_ISDBT_SUPPORT 
    if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
      /* Get AC1 carriers using snooper */
   	  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_RECORD_CFG,0x09210000 );  /* Capture CE, snapshot mode, CP Only, step=1 */
      /* Wait 10 msec for internal buffer to fill */
  	    if ( ! BTHD_P_Wait_CE_Recording_Status_Done(h, 10) )  {
          goto BTHD_P_GetFFTTriggerPosition_Abort;
        }
  		  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_READ_INDEX,0x00000000 );
          
      if (TransmissionMode == THD_TransmissionMode_8k)
        num_tps_read = 104;
      else if (TransmissionMode == THD_TransmissionMode_4k)
        num_tps_read = 52; 
      else
        num_tps_read = 26; 
  	  for (n=0; n<num_tps_read; n++) {
  		  value = BREG_Read32(h->hRegister, BCHP_THD_CORE_CE_READ_DATA );
  		  fftI = (int16_t)(((value >> 16) & 0xfff) << 4);
  		  fftQ = (int16_t)(((value >>  4) & 0xfff) << 4);
  		  if (k)
  		    h->fftpwr[n] += (fftI*fftI + fftQ*fftQ);
  		  else
   		    h->fftpwr[n] = (fftI*fftI + fftQ*fftQ); 			
      }
    }
    else {
    #endif
	    /* Get all tps carriers using snooper */
   	  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_RECORD_CFG,0x09410000 );  /* Capture CE, snapshot mode, TPS Only, step=1 */
      /* Wait 10 msec for internal buffer to fill */
      if ( ! BTHD_P_Wait_CE_Recording_Status_Done(h, 10) ) {
        goto BTHD_P_GetFFTTriggerPosition_Abort;
      }
  	  BREG_Write32(h->hRegister, BCHP_THD_CORE_CE_READ_INDEX,0x00000000 );
          
      if (TransmissionMode == THD_TransmissionMode_2k)
        num_tps_read = 17;
      else
        num_tps_read = 68; 
  	  for (n=0; n<num_tps_read; n++) {
  		  value = BREG_Read32(h->hRegister, BCHP_THD_CORE_CE_READ_DATA );
  		  fftI = (int16_t)(((value >> 16) & 0xfff) << 4);
  		  fftQ = (int16_t)(((value >>  4) & 0xfff) << 4);
  		  if (k)
  		    h->fftpwr[n] += (fftI*fftI + fftQ*fftQ);
  		  else
   		    h->fftpwr[n] = (fftI*fftI + fftQ*fftQ); 			
  	  }
    #ifdef BTHD_ISDBT_SUPPORT   
    } 
    #endif
	#endif
  }

  /* Early-exit abort triggered ? */
  if ( BTHD_P_IsAbortAcquireRequested(h) ) {
    goto BTHD_P_GetFFTTriggerPosition_Abort;
  }

  /* Compute CIR using IDFT (should be changed to IFFT for better efficiency) */
  for (k=0; k<256; k++) {
    /* Compute DFT */
    cirI = 0;
    cirQ = 0;
    for (n=0; n<256; n++) {
      idx = (k*n) % 256;
      cirI += ((int32_t)( ceI[n]*dft_table[idx][0]) + (int32_t)(ceQ[n]*dft_table[idx][1])) >> 8;
      cirQ += ((int32_t)(-ceI[n]*dft_table[idx][1]) + (int32_t)(ceQ[n]*dft_table[idx][0])) >> 8;
    }
    cirI = cirI >> 16;
    cirQ = cirQ >> 16;

    /* Compute Power */
    cir[k] = (uint32_t)(cirI*cirI + cirQ*cirQ);

    /* Compute Max */
    if (cir[k] > cir_max_val) {
      cir_max_val = cir[k];
      cir_max_idx = k;
    }
	  /* BDBG_MSG(("\t\tCIR at index:%d , height: %d",k,cir[k])); */
  }
	/* BDBG_MSG(("\t\tMax of CIR at index:%d , height: %d",cir_max_idx,cir_max_val)); */

  /* Determine number of paths in CIR */
  cir_thresh = cir_max_val >> thresh;
  start_idx = 0; cur_max = cir[0]; idx_max =0;
  for (k=1; k<256; k++) {
	  if (cir[k] > cir_thresh && cir[k-1] <= cir_thresh) {
		  start_idx=k;
		  cur_max = cir[k];
		  idx_max = k;
      peak_det = true;
	  } else if (cir[k] > cir_thresh && cir[k-1] >= cir_thresh) {
		  if (cir[k] > cur_max) {
			  cur_max = cir[k];
			  idx_max = k;
		  }
	  } else if (cir[k] < cir_thresh && cir[k-1] >= cir_thresh){
  		peak_val[paths] = cur_max;
  		peak_width[paths] = k-1-start_idx;
  		peak_location[paths] = idx_max;
/*  		BDBG_MSG(("\t\tPath at Index:%3d, height: %10d, width: %2d, at 0x%x",peak_location[paths],peak_val[paths], peak_width[paths], BREG_Read32(h->hRegister, BCHP_THD_CORE_FSCNT))); */
		if (paths < 63)
		{
			paths++;
		}
      peak_det = false;
	  } else {
		  /* Do Nothing */
      peak_det = false;
	  }		 
  }
  /* Peak at the end */
  if (peak_det) {
  		peak_val[paths] = cur_max;
  		peak_width[paths] = k-1-start_idx;
  		peak_location[paths] = idx_max;
  	   /*	BDBG_MSG(("\t\tLast Path at Index:%d, height: %d, width: %d",peak_location[paths],peak_val[paths], peak_width[paths]));  */
  		paths++;
  }

  /* Find first Peak */
  /* 2K: 256 Scattered pilots- {468:3:1233} ; TPS pilots- {569, 595, 688, 790, 901,1073,1219} */
  /* 8K: 256 Scattered pilots- {3024:3:3789}; TPS pilots- {3173,3298,3391,3442,3458,3617,3754} */
  for (error_min = 1<<30, error_min_idx=0, i=0; i<paths ; i++) { /* For all paths in CIR */  
    error[i] = 0;  
  	slope = peak_location[i] - 1 + left_edge; /* Find IFFT domain shift required */
	
  	for (j=0;j<tps_num ;j++ ) { /* For all TPS carriers - calculate error */
#ifdef BTHD_ISDBT_SUPPORT
      if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
  		if (TransmissionMode == THD_TransmissionMode_8k) 
  			tps_loc = ac1_loc_8k[j]; 
  		else if  (TransmissionMode == THD_TransmissionMode_4k) 
  			tps_loc = ac1_loc_4k[j];
		  else
			  tps_loc = ac1_loc_2k[j];
      } else {
#endif
  		if (TransmissionMode == THD_TransmissionMode_2k) 
  			tps_loc = tps_loc_2k[j]; 
  		else 
  			tps_loc = tps_loc_8k[j];
#ifdef BTHD_ISDBT_SUPPORT
      }
#endif

    #ifndef BCHP_THD_CORE_V_4_0
      /* Alternative way to read out AC1 or TPS */
      #ifdef BTHD_ISDBT_SUPPORT
      if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
        if (TransmissionMode == THD_TransmissionMode_8k) 
  			  tps_index = ac1_start_8k + j; 
        else if (TransmissionMode == THD_TransmissionMode_4k) 
  			  tps_index = ac1_start_4k + j; 
  		  else 
  			  tps_index = ac1_start_2k + j;
      } else {
		  #endif
		  if (TransmissionMode == THD_TransmissionMode_2k) 
  			tps_index = tps_start_2k + j; 
  		else 
  			tps_index = tps_start_8k + j;
      #ifdef BTHD_ISDBT_SUPPORT
      }
      #endif
		#endif
		
  		/* for ( tps_rotI[j] = 0, tps_rotQ[j] = 0, m=0 ;m<12 ;m++) { */ /* 12 interpolation points */
  		for ( tps_rotI[j] = 0, tps_rotQ[j] = 0, m=0 ;m<24 ;m++) { /* 24 interpolation points */
  		  /* Find Filter Coeffs */
  			flt_idx = m*3 + 2 - (int16_t)(tps_loc%3);
  			aI= interpI[flt_idx];
  			aQ= interpQ[flt_idx];
  			
  			/* Find Unrotated Channel Estimate */
  			/* ce_one_k = (uint16_t)3*(((tps_loc/3)) - 5); */ /* 12 interpolation points */
  			ce_one_k = (uint16_t)3*(((tps_loc/3)) - 11); /* 24 interpolation points */
  			bI= ceI[(ce_one_k - start)/3 + m];
  			bQ= ceQ[(ce_one_k - start)/3 + m];

  			/* Find Weighting Complex Exponential */
  			phase_idx = ((ce_one_k +3*(m) - (K-1)/2)*slope/3)%256;
  			exp_idx = (uint16_t) (255-((phase_idx +3*256/2)%256));
  			cI = dft_table[ exp_idx ][0];
  			cQ = dft_table[ exp_idx ][1];
  			/*BDBG_MSG(("ce_one_k=%4d, phase_idx=%3d, exp_idx=%3d, flt_idx=%2d, cI=%6d, cQ=%6d, aI=%6d, aQ=%6d",ce_one_k, phase_idx, exp_idx, flt_idx, cI, cQ, aI, aQ)); */
  			/*BDBG_MSG(("BTHD_P_GetFFTTrigger: ce_one_k=%d, phase_idx=%d, exp_idx=%d, flt_idx=%d, cI=%d, cQ=%d, aI=%d, aQ=%d",ce_one_k, phase_idx, exp_idx, flt_idx, cI, cQ, aI, aQ)); */
  			
  			/* Calculate Resultant TPS Estimate */
  			dI = (int32_t)(aI*bI - aQ*bQ) >> 15;
  			dQ = (int32_t)(aQ*bI + bQ*aI) >> 15;
  			tps_rotI[j] = tps_rotI[j] + ( ((int32_t)cI*dI - (int32_t)cQ*dQ) >> 15 );
  			tps_rotQ[j] = tps_rotQ[j] + ( ((int32_t)cQ*dI + (int32_t)cI*dQ) >> 15 );
  		}

   		/* Detector Based on rotated TPS values */	
	#ifdef BCHP_THD_CORE_V_4_0
    #ifdef BTHD_ISDBT_SUPPORT
      if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
  		/*	BDBG_MSG(("\t\t\tFFTPWR[%d]=%d, TPS_ROT[%d]=%d", tps_loc-start, ((int32_t)((h->sg[tps_loc - start])/tps_num_avg)/16)*9, j, 
                                                         (int32_t)((tps_rotI[j]*tps_rotI[j]) + (tps_rotQ[j]*tps_rotQ[j])) )); 				*/
  		  error_tmp = ( ((int32_t)((h->fftpwr[tps_loc - start])/tps_num_avg)/16)*9 - (int32_t)(tps_rotI[j]*tps_rotI[j]) - (int32_t)(tps_rotQ[j]*tps_rotQ[j]) ); 
      } else {
    #endif
  		error_tmp = ( (h->fftpwr[tps_loc - start]/tps_num_avg) - (tps_rotI[j]*tps_rotI[j]) - (tps_rotQ[j]*tps_rotQ[j]) );
    #ifdef BTHD_ISDBT_SUPPORT
      }
    #endif		
	#else
    #ifdef BTHD_ISDBT_SUPPORT
      if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
  		/*	BDBG_MSG(("\t\t\tFFTPWR[%d]=%d, TPS_ROT[%d]=%d", tps_loc-start, ((int32_t)((h->fftpwr[tps_loc - start])/tps_num_avg)/16)*9, j, 
                                                         (int32_t)((tps_rotI[j]*tps_rotI[j]) + (tps_rotQ[j]*tps_rotQ[j])) ));  				*/
  		  error_tmp = ( ((int32_t)((h->fftpwr[tps_index])/tps_num_avg)/16)*9 - (int32_t)(tps_rotI[j]*tps_rotI[j]) - (int32_t)(tps_rotQ[j]*tps_rotQ[j]) ); 
      } else {
    #endif
		  error_tmp = ( (h->fftpwr[tps_index]/tps_num_avg) - (tps_rotI[j]*tps_rotI[j]) - (tps_rotQ[j]*tps_rotQ[j]) );
    #ifdef BTHD_ISDBT_SUPPORT
      }
    #endif		
  #endif
     /* BDBG_MSG(("\t\t\tPath= %2d, TPS_idx = %4d, Err = %d",i, tps_loc, (int32_t)error_tmp )); */
  		if (error_tmp >=0) {
  			error[i] += error_tmp;
  		}	else {
  		  error[i] += -1*error_tmp;
  		}
    }
    error[i] /= tps_num_avg;
    /*BDBG_MSG(("\t\tPath = %2d, idx = %3d, Slope = %3d, Err = %10d, at 0x%x",i,peak_location[i],slope,error[i], BREG_Read32(h->hRegister, BCHP_THD_CORE_FSCNT))); */
  }

 	/* Find min Error */
 	for (i=0; i<paths; i++)	{
   	if (error_min > error[i]) {
 	  	error_min = error[i];
 		  error_min_idx = i;
		  /* BDBG_MSG(("\t\tMin Path = %d, Min Err = %d",error_min_idx,error_min)); */
 	  }
 	}
/*  BDBG_MSG(("\t\tMin Path = %d, Min Err = %d",error_min_idx,error_min));  */

  /* Determine Span */
  if (error_min_idx>0) {
    if (peak_location[error_min_idx] == 255) /* Special case of echo wrap around at end */
	    span = peak_location[paths-2] - peak_location[0];
    else {
      k = error_min_idx-1;
      while (k && ((peak_location[error_min_idx]-peak_location[k]) < left_edge)) k--; /* Do not consider peaks within left_edge of peak w/minimum error in span calculation */
	    span = 256 + peak_location[k] - peak_location[error_min_idx];
	    missed_first_peak = true;	  
    }
  } else if (peak_location[paths-1] == 255) /* Special case of echo wrap around at end */
	  span = peak_location[paths-2] - peak_location[error_min_idx];
  else
	  span = peak_location[paths-1] - peak_location[error_min_idx];
  if (span > 220)
    out_of_span = true;    

  if (span <= (15*guard/16))
    in_span = true;

  if ( (span <= guard+2) & (span >= guard-3) )  /* For the special case of on-the-GI echos, force FFTT adjustment */
	  on_guard = true;	      
  
  /* BDBG_MSG(("\t\tSpan=%d ", span)); */

  /* Continue with ISI, offset, slope calculations only if the time-domain processing missed the first peak */
  if (missed_first_peak | on_guard) {
    /* Determine FW Offset for minimum ISI */
	#if 0
    for (i=0;i<256;i++ ) /* Circular Shift */
  	  circirc[i] = cir[ (i+peak_location[error_min_idx])%256 ];
	#endif

    for (isi_min = 1<<30,y=0; y<(256-guard); y++) {
  	  for ( isi_pre=0,x=0; x<=y; x++) /* ISI_Pre */
  		  isi_pre += (y-x)*(cir[(x+peak_location[error_min_idx])%256]/256);
  	  for (isi_post=0, x=y+guard; x<256; x++) /* ISI_Post */
  		  isi_post += (x-y-guard+1)*(cir[(x+peak_location[error_min_idx])%256]/256);
  	  isi_y = isi_pre + isi_post;

  	  if (isi_y < isi_min) {
  		  isi_min = isi_y;
  		  isi_min_offset = y;
  	  }
    }
    BDBG_MSG(("\t\tISI_Min=%d, ISI_min_offset=%d ",isi_min,isi_min_offset)); 

    /* Determine Offset and Slope */
    offset_no_modulo = isi_min_offset + peak_location[error_min_idx] - peak_width[error_min_idx];
    /* Special case for on-the-guard echo */
    if (on_guard & ~missed_first_peak) { /* special case of on-guard post echo */
      if (peak_location[error_min_idx] == 255) /* Special case of wrap around echo */
        offset_no_modulo = offset_no_modulo - 2 + peak_width[error_min_idx];
      else
        offset_no_modulo = - peak_location[error_min_idx] + 2 ;
    }
    else if (on_guard & missed_first_peak) /* special case of on-guard pre echo */
      offset_no_modulo = offset_no_modulo - 2 + peak_width[error_min_idx];

    if (peak_location[error_min_idx] > cir_max_idx) { /* Determine offset */
  	  BDBG_MSG(("\t\tModulo offset"));
  	  offset = 256 - offset_no_modulo;
    } else {
  	  offset = offset_no_modulo;
    }


    if (span <= guard) { /*Determine slope*/
	    slope = 0;
    } else {
      slope = peak_location[error_min_idx] + offset;
  	  BDBG_MSG(("\t\tSlope before Modulo = %d", slope));
      if (peak_location[error_min_idx] == 255) { /* special case for on-the-GI peak*/ 
        slope = offset - 1;
      }
      else if ((peak_location[error_min_idx] > cir_max_idx) && (slope > guard)) {
  	    BDBG_MSG(("\t\tModulo slope"));
  	    slope = slope - 256 - cir_max_idx + left_edge; 
  	  }	
    }

    /* Scale from IFFT-point time-domain to N-point freq-domain */
    offset_scaled = (int16_t)((offset*N)/(3*256));
    slope_scaled  = (slope*N)/(3*256);
  }

  h->pStatus->ThdCommonStatus.FFTTriggerOffset = offset_scaled;
  h->pStatus->ThdCommonStatus.FFTTriggerSlope = slope_scaled;
  h->pStatus->ThdCommonStatus.FFTTriggerMissed = missed_first_peak;
  h->pStatus->ThdCommonStatus.FFTTriggerOutOfSpan = out_of_span;
  h->pStatus->ThdCommonStatus.FFTTriggerInSpan = in_span;
  h->pStatus->ThdCommonStatus.FFTTriggerOnGuard = on_guard;
     
  /* Convert span to microseconds and save to status structure */
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    switch (h->pAcquireParam->CommonAcquireParam.Bandwidth) {
      case THD_Bandwidth_8MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*63)/(32*8); break;
      case THD_Bandwidth_7MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*63)/(32*7); break;
      case THD_Bandwidth_6MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*63)/(32*6); break;
      case THD_Bandwidth_5MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*63)/(32*5); break;
      default: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*63)/(32*6); break;        
    } 
  } else {
#endif
  switch (h->pAcquireParam->CommonAcquireParam.Bandwidth) {
    case THD_Bandwidth_8MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*8); break;
    case THD_Bandwidth_7MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*7); break;
    case THD_Bandwidth_6MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*6); break;
    case THD_Bandwidth_5MHz: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*5); break;
    default: h->pStatus->ThdCommonStatus.ChannelSpan = (span*(N >> 11)*7)/(3*8); break;        
  }
#ifdef BTHD_ISDBT_SUPPORT
  }
#endif

#if 0
  BDBG_MSG(("\t\tPaths     : %d",paths));
  BDBG_MSG(("\t\tMissed    : %d",missed_first_peak));
  BDBG_MSG(("\t\tOutOfSpan : %d",out_of_span));    
  BDBG_MSG(("\t\tOnGuard   : %d",on_guard));    
  BDBG_MSG(("\t\tOffset    : %d (%d)",offset_scaled,offset));
  BDBG_MSG(("\t\tSlope     : %d (%d)",slope_scaled,slope));    
  BDBG_MSG(("\t\tSpan      : %dus (%d)",h->pStatus->ThdCommonStatus.ChannelSpan,span));
  /*BDBG_MSG(("\t\tTime      : %dms",(fscntInit - BREG_Read32(h->hRegister, BCHP_THD_CORE_FSCNT))/54000));*/
#endif
  bCompletedSuccess = true;
  goto BTHD_P_GetFFTTriggerPosition_Exit;

BTHD_P_GetFFTTriggerPosition_Abort:
  BDBG_MSG(("\t\tBTHD_P_GetFFTTriggerPosition Abort (timeout or early-exit)"));
  bCompletedSuccess = false;  
  h->pStatus->ThdCommonStatus.FFTTriggerMissed = true;
  if ( BTHD_P_IsAbortAcquireRequested(h) ) {
      BDBG_MSG(("\t\tBTHD_P_GetFFTTriggerPosition: Early-Exit abort request is active."));
  }

BTHD_P_GetFFTTriggerPosition_Exit:
  return ( bCompletedSuccess );
}

/***************************************************************************
* BTHD_P_SetFFTTriggerPosition()
***************************************************************************/
void BTHD_P_SetFFTTriggerPosition(BTHD_3x7x_ChnHandle h, THD_TransmissionMode_t TransmissionMode,THD_GuardInterval_t GuardInterval)
{ 
  BSTD_UNUSED(TransmissionMode);
  BSTD_UNUSED(GuardInterval);
  /*uint32_t N = bthd_transmission_mode[TransmissionMode];*/
  
  BDBG_MSG(("\t\t{%d,%d,%d,%d}",BREG_Read32(h->hRegister, BCHP_THD_CORE_FW_OFFSET),BREG_Read32(h->hRegister, BCHP_THD_CORE_FW_CORR_INDEX),BREG_Read32(h->hRegister, BCHP_THD_CORE_FW_ABS_MIN_INDEX),h->pStatus->ThdCommonStatus.FFTTriggerOffset));

  BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 1 );                    /* Deassert FFT window reference index reset (active-low) */
  BINT_EnableCallback( h->hFwSyncCallback);

  BKNI_WaitForEvent(h->hFwSyncEvent, 10);
  /* BREG_WriteField(h->hRegister,THD_CORE_FW_MISC, TL_SHIFT, 0); */
  /* BREG_WriteField(h->hRegister, THD_CORE_FW, LIMIT, 0); */                   /* FW slip_limit=0 */
  BREG_WriteField(h->hRegister, THD_CORE_FW_OFFSET, OFFSET, h->pStatus->ThdCommonStatus.FFTTriggerOffset );        /* Apply pre-FFT trigger offset */
  BREG_WriteField(h->hRegister, THD_CORE_FW_SLIP, SLIP_OFFSET, h->pStatus->ThdCommonStatus.FFTTriggerSlope );      /* Apply post-FFT slope */
  BKNI_WaitForEvent(h->hFwSyncEvent, 10);
  BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 0 );                 /* Reset FFT window reference index (active low) for new fw_offset to take effect*/
  BKNI_WaitForEvent(h->hFwSyncEvent, 10);  
  BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 1 );                 /* Deassert FFT window reference index reset (active-low) */

  BINT_DisableCallback( h->hFwSyncCallback);

  BDBG_MSG(("\t\t{%d,%d,%d}",BREG_Read32(h->hRegister, BCHP_THD_CORE_FW_OFFSET),BREG_Read32(h->hRegister, BCHP_THD_CORE_FW_CORR_INDEX),BREG_Read32(h->hRegister, BCHP_THD_CORE_FW_ABS_MIN_INDEX)));


}

/***************************************************************************
* BTHD_P_StatusSsi()
***************************************************************************/
void BTHD_P_StatusSsi(BTHD_3x7x_ChnHandle h)
{
  int16_t   		prel[3], ssi[3]={0,0,0};
  int32_t			n=0, NumOfLoops;

#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
  	int16_t 	refSignalLevel[4][5] = 		   	/* mod,cod */
	  {
	  { -94, -93, -92, -90, -90 }, /* QPSK		*/
	  { -88, -86, -85, -82, -81 }, /* 16-QAM	*/
	  { -83, -81, -79, -77, -76 }, /* 64-QAM	*/
	  { -94, -93, -92, -90, -90 }  /* DQPSK		*/
	  };
    prel[0] = h->pStatus->ThdCommonStatus.SignalStrength/256 - refSignalLevel[h->pStatus->StdStatus.IsdbtStatus.Qam[0]][h->pStatus->StdStatus.IsdbtStatus.CodeRate[0]];
	prel[1] = h->pStatus->ThdCommonStatus.SignalStrength/256 - refSignalLevel[h->pStatus->StdStatus.IsdbtStatus.Qam[1]][h->pStatus->StdStatus.IsdbtStatus.CodeRate[1]];
	prel[2] = h->pStatus->ThdCommonStatus.SignalStrength/256 - refSignalLevel[h->pStatus->StdStatus.IsdbtStatus.Qam[2]][h->pStatus->StdStatus.IsdbtStatus.CodeRate[2]];
/*	BDBG_MSG(("signal strength: %d, reference: %d", h->pStatus->ThdCommonStatus.SignalStrength/256, refSignalLevel[h->pStatus->StdStatus.IsdbtStatus.Qam[0]][h->pStatus->StdStatus.IsdbtStatus.CodeRate[0]])); */
	NumOfLoops = 3;
  } else {
#endif	
  	int16_t 	refSignalLevel[3][5] = 			/* mod,cod */
	  {
	  { -93, -91, -90, -89, -88 }, /* QPSK		*/
	  { -87, -85, -84, -83, -82 }, /* 16-QAM	*/
	  { -82, -80, -78, -77, -76 }  /* 64-QAM	*/
	  };
    prel[0] = h->pStatus->ThdCommonStatus.SignalStrength/256 - refSignalLevel[h->pStatus->StdStatus.DvbtStatus.Qam][h->pStatus->StdStatus.DvbtStatus.CodeRateHP];
    NumOfLoops = 1;
#ifdef BTHD_ISDBT_SUPPORT
  }
#endif	

  while (n < NumOfLoops) {
    if ( prel[n] >= 35 )				    	ssi[n] = 100; 
    else if ( prel[n] >= 20  && prel[n] < 35 )	ssi[n] = (prel[n]-20)*2/3 + 90;	
    else if ( prel[n] >= 0   && prel[n] < 20 )  ssi[n] = 4*prel[n] + 10;
    else if ( prel[n] >= -15 && prel[n] < 0  )  ssi[n] = (prel[n] + 15)*2/3;
/*    else if ( prel[n] < -15 )			       	ssi[n] = 0;  */
    else 								       	ssi[n] = 0;
	n = n + 1;
  }

#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
	h->pStatus->StdStatus.IsdbtStatus.Ssi[0] = ssi[0];
	h->pStatus->StdStatus.IsdbtStatus.Ssi[1] = ssi[1];
	h->pStatus->StdStatus.IsdbtStatus.Ssi[2] = ssi[2];
/*    BDBG_MSG(("\t ssi_a: %d,  ssi_b: %d,  ssi_c: %d",h->pStatus->StdStatus.IsdbtStatus.Ssi[0], h->pStatus->StdStatus.IsdbtStatus.Ssi[1], h->pStatus->StdStatus.IsdbtStatus.Ssi[2])); */
  } else
#endif	
 	h->pStatus->ThdCommonStatus.Ssi = ssi[0]; 

}

/***************************************************************************
* BTHD_P_StatusSqi()
***************************************************************************/

void BTHD_P_StatusSqi(BTHD_3x7x_ChnHandle h)
{
  int16_t 	ber_sqi[3]={0,0,0};
  int16_t 	cnrel[3]={0,0,0}, sqi[3]={0,0,0};
  int32_t	n=0, NumOfLoops=1;
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
  int16_t CNRARIBx256[4][5] =  /* mod,cod */
	{
    {1254, 1690, 1920, 2048, 2330}, /* QPSK:   256*{  4.9,  6.6,  7.5,  8.0,  9.1 } */
    {2944, 3456, 3738, 3994, 4147}, /* 16-QAM: 256*{ 11.5, 13.5, 14.6, 15.6, 16.2 } */
    {4224, 4787, 5146, 5453, 5632}, /* 64-QAM: 256*{ 16.5, 18.7, 20.1, 21,3, 22.0 } */    
    {1587, 1971, 2227, 2458, 2662}  /* DQPSK:  256*{  6.6,  8.0,  9.0, 10.0, 10.6 } */
	};
	/* 2^31 = 0x80000000, 1e-3*(2^31) = 2147484, 1e-7*(2^31)=215, 1e-2*(2^31) =  21474836*/
	/* 2560log(1e2)/128-40 = 0,  2560log(1e7)/128-40 = 100*/
	if (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[0]) {
	  if 	  (h->pStatus->StdStatus.IsdbtStatus.VitBER[0] <= 215)	   	ber_sqi[0] = 100;
	  else if (h->pStatus->StdStatus.IsdbtStatus.VitBER[0] > 21474836) 	ber_sqi[0] = 0;
	  else	  ber_sqi[0] = (BMTH_2560log10((0x80000000)/h->pStatus->StdStatus.IsdbtStatus.VitBER[0])/128) - 40;
	  cnrel[0] = (h->pStatus->ThdCommonStatus.SNR - CNRARIBx256[h->pStatus->StdStatus.IsdbtStatus.Qam[0]][h->pStatus->StdStatus.IsdbtStatus.CodeRate[0]]) >> 8;
/*	  BDBG_MSG(("ber_sqi: %d, cnrel: %d, snrpilot: , %d, snr_target: %d", ber_sqi[0], cnrel[0], h->pStatus->ThdCommonStatus.SNR, CNRARIBx256[h->pStatus->StdStatus.IsdbtStatus.Qam[0]][h->pStatus->StdStatus.IsdbtStatus.CodeRate[0]])); */
 	}
	if (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[1]) {
	  if 	  (h->pStatus->StdStatus.IsdbtStatus.VitBER[1] <= 215)	   	ber_sqi[1] = 100;
	  else if (h->pStatus->StdStatus.IsdbtStatus.VitBER[1] > 21474836) 	ber_sqi[1] = 0;
	  else	  ber_sqi[1] = (BMTH_2560log10((0x80000000)/h->pStatus->StdStatus.IsdbtStatus.VitBER[1])/128) - 40;
	  cnrel[1] = (h->pStatus->ThdCommonStatus.SNR - CNRARIBx256[h->pStatus->StdStatus.IsdbtStatus.Qam[1]][h->pStatus->StdStatus.IsdbtStatus.CodeRate[1]]) >> 8;
 	}
	if (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[2]) {
	  if 	  (h->pStatus->StdStatus.IsdbtStatus.VitBER[2] <= 215)	   	ber_sqi[2] = 100;
	  else if (h->pStatus->StdStatus.IsdbtStatus.VitBER[2] > 21474836) 	ber_sqi[2] = 0;
	  else	  ber_sqi[2] = (BMTH_2560log10((0x80000000)/h->pStatus->StdStatus.IsdbtStatus.VitBER[2])/128) - 40;
	  cnrel[2] = (h->pStatus->ThdCommonStatus.SNR - CNRARIBx256[h->pStatus->StdStatus.IsdbtStatus.Qam[2]][h->pStatus->StdStatus.IsdbtStatus.CodeRate[2]]) >> 8;
 	}
    NumOfLoops = 3;
  } else {
#endif	
  int16_t CNNordigP1x256[3][5] =  /* mod,cod */
	{
    {1305, 1766, 2022, 2278, 2483}, /* QPSK:   256*{  5.1,  6.9,  7.9,  8.9,  9.7 } */
    {2764, 3353, 3737, 3993, 4096}, /* 16-QAM: 256*{ 10.8, 13.1, 14.6, 15.6, 16.0 } */
    {4224, 4787, 5171, 5529, 5760}  /* 64-QAM: 256*{ 16.5, 18.7, 20.2, 21.6, 22.5 } */    
	};
	if (h->pStatus->StdStatus.DvbtStatus.TS_TBERC) {
	  if 	  (h->pStatus->StdStatus.DvbtStatus.VitBER <= 215)	    ber_sqi[0] = 100;
	  else if (h->pStatus->StdStatus.DvbtStatus.VitBER > 2147484)   ber_sqi[0] = 0;
	  else	  ber_sqi[0] = (BMTH_2560log10((0x80000000)/h->pStatus->StdStatus.DvbtStatus.VitBER)/128) - 40;
	  cnrel[0] = (h->pStatus->ThdCommonStatus.SNRPilot - CNNordigP1x256[h->pStatus->StdStatus.DvbtStatus.Qam][h->pStatus->StdStatus.DvbtStatus.CodeRateHP]) >> 8;
 	}																										  
    NumOfLoops = 1;
#ifdef BTHD_ISDBT_SUPPORT
  }
#endif	

  while (n < NumOfLoops) {   
    if ( cnrel[n] >= 3 )  						sqi[n] = ber_sqi[n];	
    else if ( cnrel[n] >= -7 && cnrel[n] < 3 )	sqi[n] = (((cnrel[n]-3)/10)+1)*ber_sqi[n];
/*    else if ( cnrel < -7 )				  	sqi[n] = 0;	*/
    else 										sqi[n] = 0;
    n = n + 1;
  }
  
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
	h->pStatus->StdStatus.IsdbtStatus.Sqi[0] = sqi[0];
	h->pStatus->StdStatus.IsdbtStatus.Sqi[1] = sqi[1];
	h->pStatus->StdStatus.IsdbtStatus.Sqi[2] = sqi[2];
/*    BDBG_MSG(("\t sqi_a: %d,  sqi_b: %d,  sqi_c: %d",h->pStatus->StdStatus.IsdbtStatus.Sqi[0], h->pStatus->StdStatus.IsdbtStatus.Sqi[1], h->pStatus->StdStatus.IsdbtStatus.Sqi[2])); */
  } else
#endif	
 	h->pStatus->ThdCommonStatus.Sqi = sqi[0]; 
}

/***************************************************************************
* BTHD_P_StatusSNR()
***************************************************************************/
static void BTHD_P_StatusSNR ( BTHD_3x7x_ChnHandle h )
{
  if ( h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_DVBT ) {
    /* DVB-T "common status" SNR is derived (here) from SNRPilot */
    h->pStatus->ThdCommonStatus.SNR = h->pStatus->ThdCommonStatus.SNRPilot;
  } else {
    #ifdef BTHD_ISDBT_SUPPORT
      /* Nothing: ISDB-T "common status" SNR is derived and assigned in BTHD_P_IsdbtStatus() */
    #endif	
  }
}

/***************************************************************************
 * BTHD_P_Status()
 ***************************************************************************/
void BTHD_P_Status(BTHD_3x7x_ChnHandle h)
{
  uint32_t value,bw,N,eq_snrp,ce_beta,ce_byp;
  long int cl_int,tl_int;
  int16_t TunerGain;
  uint16_t SmartTune;  
  int32_t  snr_factor, tmp;
  uint32_t pouthi,poutlo,poutlo2,utmp,RfFreq;
  
#ifdef EMULATION_ENABLED
  h->pStatus->ThdCommonStatus.Lock = 6;
  return;
#endif

  /* Determine transmission parameters */
  value = BREG_Read32(h->hRegister,  BCHP_THD_CORE_TPS_OV);
  h->pStatus->ThdCommonStatus.TransmissionMode = ((value >> 24) & 0x3);
  h->pStatus->ThdCommonStatus.GuardInterval = ((value >> 20) & 0x3);
  h->pStatus->ThdCommonStatus.Bandwidth = h->pAcquireParam->CommonAcquireParam.Bandwidth;

  /* Determine spctral inversion (only works for baseband input mode) */
  if (BREG_ReadField(h->hRegister, THD_CORE_FE, NEGATE_Q))
    h->pStatus->ThdCommonStatus.Spectrum = THD_Spectrum_Inverted;
  else
    h->pStatus->ThdCommonStatus.Spectrum = THD_Spectrum_Normal;
    
  /* Compute DAGC gain */
  value  = BREG_ReadField(h->hRegister,  THD_CORE_DAGC_FILT,  LFO  ) ;
  h->pStatus->ThdCommonStatus.DagcGain = (short)((long int)(BMTH_2560log10(value) << 1) - 38632);  /* When the gain = -1.0, 0dB =  floor(2*2560*log10(hex2dec('2000000'))) = 38632 = adjust factor, Application takes 20*log10(x/256.0) to obtain value in dB */

  /* Compute signal strength */
  BTHD_3x7x_P_TNR_callback(h,BTHD_CallbackMode_eRequestMode,&TunerGain,&SmartTune,&RfFreq);

  h->pStatus->ThdCommonStatus.SignalStrength = 256*THD_SignalStrengthCalibrationFactor - ((int32_t)TunerGain + h->pStatus->ThdCommonStatus.DagcGain);
  /* BDBG_MSG(("\tSignalStrength: %d dBm",h->pStatus->ThdCommonStatus.SignalStrength/256)); */

  /* Compute carrier offset */
  bw = bthd_bandwidth[h->pAcquireParam->CommonAcquireParam.Bandwidth];
  cl_int = (long int)BREG_Read32(h->hRegister,  BCHP_THD_CORE_CL_INT );
  if (cl_int < 0)
    cl_int = -cl_int;

  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    /* freq = (cl_int / N / 2^32) * (2 * 512 / 63) * (bw / 6) * 1000000  */
    utmp = (1000000*bw);
    BMTH_HILO_32TO64_Mul(utmp, cl_int, &pouthi, &poutlo);
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, 378, &utmp, &poutlo2);
    BMTH_HILO_64TO64_Div32(utmp, poutlo2, 0x400000, &pouthi, &poutlo);  /* this is the same as >> 22 */
  } else {
    /* freq = (cl_int / N / 2^32) * (2 * 64 / 7) * (bw / 8) * 1000000 */
    utmp = (15625*bw);
    BMTH_HILO_32TO64_Mul(utmp, cl_int, &pouthi, &poutlo);
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, 7, &utmp, &poutlo2);
    BMTH_HILO_64TO64_Div32(utmp, poutlo2, 0x400000, &pouthi, &poutlo);  /* this is the same as >> 22 */
  }
  N = bthd_transmission_mode[h->pStatus->ThdCommonStatus.TransmissionMode]/1024;
  h->pStatus->ThdCommonStatus.CarrierOffset = (poutlo / N);

  cl_int = BREG_Read32(h->hRegister,  BCHP_THD_CORE_CL_INT );
  if (cl_int < 0)
    h->pStatus->ThdCommonStatus.CarrierOffset = -h->pStatus->ThdCommonStatus.CarrierOffset;

  /* Compute timing offset */
  tl_int  = (long int)BREG_Read32(h->hRegister,  BCHP_THD_CORE_TL_INT );  
  if (tl_int < 0)
    tl_int = -tl_int;

  /* tmp = (((((long long int)h->pAcquireParam->CommonAcquireParam.SampleFreq)*tl_int)/1000000)*15625) >> (16 + 6 + 10);
     h->pStatus->ThdCommonStatus.TimingOffset = (long int)(tmp / N);
  */
  BMTH_HILO_32TO64_Mul(h->pInternalAcquireParam->SampleFreq, tl_int, &pouthi, &poutlo);
  BMTH_HILO_64TO64_Div32(pouthi, poutlo, 1000000, &utmp, &poutlo2);
  BMTH_HILO_64TO64_Mul(utmp, poutlo2, 0, 15625, &utmp, &poutlo);
  h->pStatus->ThdCommonStatus.TimingOffset = (long int)(utmp / N);

  tl_int  = (long int)BREG_Read32(h->hRegister,  BCHP_THD_CORE_TL_INT );  
  if (tl_int < 0)
    h->pStatus->ThdCommonStatus.TimingOffset = -h->pStatus->ThdCommonStatus.TimingOffset;

/* Compute pilot SNR, pilotSNR = 256 * BBS_SNR 
     pilotSNR   = 256 * (b-m*10*log10(EQ_SNRP)) ,   
                = 256*b - m*256*10*log10(EQ_SNRP)
                
    m = 1.03, b (ce_beta = snr_factor_table[][])is depended on GI and CE_AVG_FF.  
  */
  h->pStatus->ThdCommonStatus.SNRPilot = 0;
  eq_snrp = BREG_Read32(h->hRegister,  BCHP_THD_CORE_EQ_SNRP);
  if (eq_snrp) {
   /* ce_beta has 4 possible values.*/
    const uint16_t snr_factor_8K_table[][5] = {
      { /* Guard interval: 1/32 */
        19706, 
        20702,
        21104,
        21270,
        21344
      },
      { /* Guard interval: 1/16 */
        19699,
        20680,
        21083,
        21249,
        21357
      },
      { /* Guard interval: 1/8 */
        19658,
        20626,
        21032,
        21221,
        21343
      },
      { /* Guard interval: 1/4 */
        19478,
        20558,
        20936,
        21139,
        21220
      }
    };
    
    const uint16_t snr_factor_2K_table[][5] = {
      { /* Guard interval: 1/32 */
        19786,
        20781,
        21144,
        21326,
        21400       
















      },
      { /* Guard interval: 1/16 */
        19804,
        20785,
        21163,
        21330,
        21374
















      },
      { /* Guard interval: 1/8 */
        19796,
        20781,
        21164,
        21324,
        21413
















      },
      { /* Guard interval: 1/4 */
        19794,
        20770,
        21162,
        21332,
        21408
















      }
    };

    ce_byp = BREG_ReadField(h->hRegister,  THD_CORE_BYP, CE_AVG_BYP); 
        
    if (ce_byp == 0)
    {
        ce_beta = BREG_ReadField(h->hRegister,  THD_CORE_CE, AVG_FF);
        if (ce_beta > 4)  /* limited ce_beta < 5 */
            ce_beta = 4;
        
        if (h->pStatus->ThdCommonStatus.TransmissionMode == THD_TransmissionMode_2k)
            snr_factor = snr_factor_2K_table[h->pStatus->ThdCommonStatus.GuardInterval][ce_beta];
        else
            snr_factor = snr_factor_8K_table[h->pStatus->ThdCommonStatus.GuardInterval][ce_beta];
                        
        tmp = ((103*BMTH_2560log10(eq_snrp))/100); 
                
        if (snr_factor > (tmp))




            h->pStatus->ThdCommonStatus.SNRPilot = snr_factor - tmp;
        else
            h->pStatus->ThdCommonStatus.SNRPilot = 0;   /* Should never happen, prevent SNRPilot becoming random number. */            
        


    }
    else
    {}        /*Place holder for ce_byp = 1, */        
  }

#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) 
    BTHD_P_IsdbtStatus(h);
  else 
#endif
    BTHD_P_DvbtStatus(h);  
						
  BTHD_P_StatusSNR(h);
  BTHD_P_StatusSsi(h);
  BTHD_P_StatusSqi(h); 

  /* Update Lock Status */
  h->pStatus->ThdCommonStatus.Lock = ((h->ThdLockStatus >> THD_LockStatusBit_SystemLock) & 1);
  h->pStatus->ThdCommonStatus.NoSignal = ((h->ThdLockStatus >> THD_LockStatusBit_NoDVBTSignal) & 1);
  h->pStatus->ThdCommonStatus.FECLock = ((h->ThdLockStatus >> THD_LockStatusBit_FECLock) & 1);

  /* Update Auto Acquire Mode Status */
  h->pStatus->ThdCommonStatus.AcquireMode = h->pAcquireParam->CommonAcquireParam.AcquireMode;
}

/***************************************************************************
 * BTHD_P_ResetAll()
 ***************************************************************************/
void BTHD_P_ResetAll(BTHD_3x7x_ChnHandle h)
{
  uint32_t reg1, reg4, reg5, regmem;

  reg1 = BREG_Read32(h->hRegister,  BCHP_THD_CORE_SWSPARE1);   
  reg4 = BREG_Read32(h->hRegister, BCHP_THD_CORE_SWSPARE4 );
  reg5 = BREG_Read32(h->hRegister, BCHP_THD_CORE_SWSPARE5 ); 
  regmem = BREG_Read32(h->hRegister, BCHP_THD_CORE_SCB_ADDR_OFFSET ); 

  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RST ,0xffffffff );       /* Reset datapath and register interface */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RST2,0x000001ff );
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RST ,0x007a1008 );       /* Hold RS_ERC,RS,FEC,BCH_ERC,TPS,FW,FE FIFO in reset */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RST2,0x00000061 );       /* Hold TMCC_ERC,TMCC,NSE_AVG in reset */
  BREG_Write32(h->hRegister,  BCHP_THD_INTR2_CPU_CLEAR,0x001fffff);  /* Clear all MIPS interrupt status */
  BREG_Write32(h->hRegister,  BCHP_THD_INTR2_PCI_CLEAR,0x001fffff);  /* Clear all PCI interrupt status */

  BREG_Write32(h->hRegister, BCHP_THD_CORE_SWSPARE1, reg1);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_SWSPARE4, reg4);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_SWSPARE5, reg5);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_SCB_ADDR_OFFSET, regmem);
}

/***************************************************************************
 * BTHD_P_ResetAcquire()
 ***************************************************************************/
void BTHD_P_ResetAcquire(BTHD_3x7x_ChnHandle h)
{
  uint32_t reg1, reg4, reg5, regmem;

  reg1 = BREG_Read32(h->hRegister,  BCHP_THD_CORE_SWSPARE1);   
  reg4 = BREG_Read32(h->hRegister, BCHP_THD_CORE_SWSPARE4 );
  reg5 = BREG_Read32(h->hRegister, BCHP_THD_CORE_SWSPARE5 ); 
  regmem = BREG_Read32(h->hRegister, BCHP_THD_CORE_SCB_ADDR_OFFSET ); 

  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RST ,0xfffffffe );       /* Reset datapath */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RST2,0x000000ff );
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FRZ,0xffffffff );        /* Freeze all */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RST ,0x007a1008 );       /* Hold RS_ERC,RS,FEC,BCH_ERC,TPS,FW,FE FIFO in reset */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RST2,0x00000161 );       /* Release active-low FW_REF reset, hold TMCC_ERC,TMCC,NSE_AVG in reset */
  BREG_Write32(h->hRegister,  BCHP_THD_INTR2_CPU_CLEAR,0x001fffff);  /* Clear all MIPS interrupt status */
  BREG_Write32(h->hRegister,  BCHP_THD_INTR2_PCI_CLEAR,0x001fffff);  /* Clear all PCI interrupt status */

  BREG_Write32(h->hRegister, BCHP_THD_CORE_SWSPARE1, reg1);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_SWSPARE4, reg4);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_SWSPARE5, reg5);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_SCB_ADDR_OFFSET, regmem);
#ifdef SmartNotchEnabled  
  h->pInternalAcquireParam->SmartNotchPresent = false;
#endif  
}

/***************************************************************************
 * BTHD_P_ResetStatus()
 ***************************************************************************/
void BTHD_P_ResetStatus(BTHD_3x7x_ChnHandle h)
{
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    BTHD_P_IsdbtResetStatus(h);
    h->pStatus->ThdCommonStatus.Lock = false; 
  } else
#endif
    BTHD_P_DvbtResetStatus(h);
    h->pStatus->ThdCommonStatus.Lock = 0;
}

/***************************************************************************
 * BTHD_P_ResetStatusHW()
 ***************************************************************************/
void BTHD_P_ResetStatusHW(BTHD_3x7x_ChnHandle h)
{
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 1 );       /* Reset FEC error counters */
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 0 );
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_B, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_B, 0 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_C, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_C, 0 );
  }
#endif
}

/***************************************************************************
 * BTHD_P_Config()
 ***************************************************************************/
void BTHD_P_Config(BTHD_3x7x_ChnHandle h)
{
	uint32_t k;

  /*************************************************************************
   * Configuration
   *************************************************************************/
  BTHD_P_SetFrontEnd(h);                                             /* Setup frontend */
  BTHD_P_SetFrequency(h);                                            /* Setup frequencies (channel bandwidth,carrier frequency) */
  BTHD_P_WriteTICoef(h);                                             /* Setup time interpolator coefficients */
  BTHD_P_WriteFICoef(h,THD_FrequencyInterpolatorMode_InSpan);       /* Setup frequency interpolator coefficients */

  BREG_WriteField(h->hRegister, THD_CORE_DAGC, THRESH, 0x00a3d );    /* DAGC threshold=20dB (uint32_t)(pow(10.0,-20/10)* 262144) */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL,0x00000600 );         /* Carrier loop int_wait=6 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW,0x00212050 );         /* FFT window postcursor bias,enable fft,sync_symbols=32,beta=2^(-5),slip_limit=0 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_SEARCH,0x01802008 );  /* FFT window min_scale=1.5,out-of-span pre or in-span echoes,1st peak mode,L=8 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_OFFSET,0x00000010 );  /* FFT window offset=16 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_MISC,0x00000200 );    /* FFT window leak to timing loop disabled, disallow zeros */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_SPAN,0x00000200 );    /* FFT window span scale = 0.25 */
  /*BREG_Write32(h->hRegister,  BCHP_THD_CORE_CE_LMS,0x66030210 );  */   /* CE lms FI center leakage=2^-12,FI edge leakage=2^-12,TI mu=2^-8, FI center mu=2^-4, FI edge mu=2^-6 */  
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CE_LMS,0x55830212 );     /* CE lms FI center leakage=2^-14,FI edge leakage=2^-14,FI leak center only,TI mu=2^-8, FI center mu=2^-4, FI edge mu=2^-6, FI adapt center only */  
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_EQ_SNR_CTL,0x00000003 ); /* EQ SNR estimator beta=2^(-12) */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_BER_SYNC,0x004001ff );   /* BERT sync thresholds */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_TPS_SYNC,0x00000076 );   /* TPS sync thresholds */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_OI,0x00000003);          /* OI burst mode */
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_FAST, TDI_ACCL, 0x1 );    /* enable TMCC fast acquisition */
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_SYNC_CTRL,0x00005656);   
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_AC_CTRL,0x00000001);     
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_SYNC_CTRL_B,0x00005656); /* AC and RT (uncr+corr)/(uncr+corr+clean) */
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_AC_CTRL_B,0x00000001);   
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_SYNC_CTRL_C,0x00005656);
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_AC_CTRL_C,0x00000001);   /* lock if ( ZERO (uncr+corr) per ONE (uncr_corr+clean) = one clean block */  
  } else {
#endif
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_SYNC_CTRL,0x00002656); /* RS lock detector acq_numer=Uncorrected, acq_denom=Total, trk_numer=Corrected+Uncorrected,trk_denom=Total */  
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_AC_CTRL,0x00000001);  /* RS lock achieved if 0 uncorrected out of 1 total */  
  /*BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_RT_CTRL,0x01ff0200); */ /* RS lock lost if >511 (uncorrected+corrected) out of 512 total (i.e. no clean blocks out of 512) */ 
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_RT_CTRL,0x07ff0800);  /* RS lock lost if >2047 (uncorrected+corrected) out of 2048 total (i.e. no clean blocks out of 2048) */ 
#ifdef BTHD_ISDBT_SUPPORT    
  }
#endif

 /*Initialize Status Spares*/
 /*Note: On using a spare, remove its initilization here, as init would overwrite the spare value in case of unlocked and reacquire*/
  for (k=1; k<8; k++)
    h->pStatus->ThdCommonStatus.spare[k] = (k+1)*0x1111;
}

/***************************************************************************
* BTHD_P_AcquireInit()
***************************************************************************/
BTHD_RESULT BTHD_P_AcquireInit( BTHD_3x7x_ChnHandle h, THD_FFTWindowMode_t FFTWindowMode )
{
  uint32_t value;
  uint32_t N=2048;
  uint32_t ice,cl_int,cl_int_ice;

  THD_TransGuardResult_t TransGuardResult;
  THD_TransmissionMode_t TransmissionMode = h->pAcquireParam->CommonAcquireParam.TransmissionMode;
  THD_GuardInterval_t GuardInterval;
  BERR_Code eEventWaitResult;
   
#ifdef EMULATION_ENABLED
#ifndef UFE_ENABLED
  /* For THD-only emulation: apply input data to THD testport*/
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_TP,    0x00000021);      /* TPOUT_SEL=BE,TPOUT_EN=1 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_TP_FE, 0x00001000);      /* TPIN_EN=1 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_TP_FW, 0x00ff2110);      /* CRC_EN,TPOUT_SEL=1,TPOUT_EN */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_TP_BE, 0x00000710);      /* TPOUT_SEL=bsft,TPOUT_EN=1 */
#endif
  /* Signal testbench to start input data */
  BREG_Write32(h->hRegister,  BCHP_THD_INTR_AP_SET, 0x00000040 );
#endif

  /*************************************************************************
   * Configuration
   *************************************************************************/
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT)
    BREG_WriteField(h->hRegister, THD_CORE_GLB, STD, 2);             /* ISDBT mode */
  BREG_WriteField(h->hRegister, THD_CORE_GLB, BE_MODE, 0 );          /* BE=disabled */
  BREG_WriteField(h->hRegister, THD_CORE_GLB, ICE_MODE, 1 );         /* ICE=acquisition */
  BREG_WriteField(h->hRegister, THD_CORE_RST, FE_FIFO_RST, 0 );      /* Relese FE FIFO reset */

  value = ((uint32_t)h->pAcquireParam->CommonAcquireParam.GuardInterval << 20) | 
          ((uint32_t)h->pAcquireParam->CommonAcquireParam.TransmissionMode << 24) | (1U << 31);
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_TPS_OV, value );
  BTHD_P_SetNotch(h);                                                /* Setup notch filter */

  BREG_WriteField(h->hRegister, THD_CORE_DAGC, KL, 0x6 );            /* DAGC K=2(-12) */

  /* BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW,0x00212051 ); */   /* FFT window postcursor bias,enable fft,sync_symbols=32,beta=2^(-5),slip_limit=1 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW,0x03212051 );         /* -HZ, FFT window use variable search range, full 6400 mem, postcursor bias,enable fft,sync_symbols=32,beta=2^(-5),slip_limit=1 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CE,0x02020008 );         /* CE use CPs, ACT=2, ACB=0, auto-reacq SP phase,beta=2^(-1) */
  BREG_WriteField(h->hRegister, THD_CORE_BYP, CE_AVG_BYP, 1 );
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CP,0xc2029080 );         /* CP t/c_modulo_add=1,timing PD average 32 symbols,timing PD beta=2^(-2),use freq detector,enable phase ramp comp,trk_range=4,acq_range=128 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_EQ,0x00003100 );         /* EQ nse avg all beta=2^(-9),nse avg beta=2^(-4),linear nse interpolation */
  BREG_WriteField(h->hRegister, THD_CORE_BYP, EQ_BYP, 0x19);         /* EQ bypass nse + avg nse exp, unbypass chan exp, unbypass nse averager, bypass bit scaling */
  BREG_WriteField(h->hRegister, THD_CORE_RST2,CP_BAD_RST, 1 );
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CP_BAD,0x00000c40 );     /* CP bad thresh=9dB, window=+/-4 pilots, auto, disabled for CPP */
#if (BCHP_CHIP==3472)  /* this register exist only in 3472 RDB */
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT)
	BREG_WriteField(h->hRegister, THD_CORE_EQ2,ISDBT_SFT_FORMAT, 0x0);  /* 7-bit width to reduce external memory*/
/*	BREG_WriteField(h->hRegister, THD_CORE_EQ2,ISDBT_SFT_FORMAT, 0x1); */ /* 6-bit width to reduce external memory*/
/*	BREG_WriteField(h->hRegister, THD_CORE_EQ2,ISDBT_SFT_FORMAT, 0x2); */ /* 5-bit width to reduce external memory*/
#endif /* BTHD_ISDBT_SUPPORT */
#endif
#if 0
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CP_BAD,0x00000c41 );     /* CP bad thresh=9dB, window=+/-4 pilots, auto, enabled for CPP */
  BREG_WriteField(h->hRegister, THD_CORE_CP_CPE, USE_CP_BAD, 1 );
  BREG_WriteField(h->hRegister, THD_CORE_CP_CGE, USE_CP_BAD, 1 );
  BREG_WriteField(h->hRegister, THD_CORE_CE_LMS, TIME_USE_CP_BAD, 1);
#endif
  /*************************************************************************
   * Acquisition
   *************************************************************************/
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FRZ,0x07f0f1e0 );        /* Unfreeze NOTCH,CE AVG,FFT Win,DAGC */
#ifndef FASTACQ
  BKNI_Sleep(10);
#endif

  /*  Co-Channel (Always enable video carrier notch before SetTransGuard() - improves 2K mode/guard detection with analog TV CCI) */
  if (h->pAcquireParam->CommonAcquireParam.CoChannelMode == THD_CoChannelMode_Auto) {
#ifdef SmartNotchEnabled
		if (h->pInternalAcquireParam->SmartNotchPresent)  
    	BREG_WriteField(h->hRegister, THD_CORE_FRZ, NOTCH_FRZ ,0x20 ); /* Unfreeze Notch DDFS 0,1,2,3,0M */  
    else              
#endif  
    BREG_WriteField(h->hRegister, THD_CORE_FRZ, NOTCH_FRZ ,0x3c );   /* Unfreeze Notch DDFS 0,0M */                  
    BREG_Write32(h->hRegister, BCHP_THD_CORE_NOTCH_SWP2,0x04001001 );/* Dwell=0x40,Step_Size=0x4,Num_Steps=0x1 */
    BREG_Write32(h->hRegister, BCHP_THD_CORE_NOTCH_SWP3,0x00000000 );/* Threshold = 0 */
    BREG_Write32(h->hRegister, BCHP_THD_CORE_NOTCH_SWP1,0x801b2200 );/* Start sweep,enable carrier adjust,enable notch0 pwr,sweep  notch width = 0xb,enable thresh,auto enable */
    while (  BREG_ReadField(h->hRegister,  THD_CORE_NOTCH_STATUS1, DONE ) != 1 );
  }

  TransGuardResult = BTHD_P_SetTransGuard(h);
  BREG_Write32(h->hRegister, BCHP_THD_CORE_INTR_STATUS2_CLR,0xffffffff);

  /* Early-Abort occurred (during BTHD_P_SetTransGuard())? */
  if ( BTHD_P_IsAbortAcquireRequested(h) ) {
    /* Acquire aborted early waiting for fwSyncEvent */
    return (THD_AcquireResult_AbortedEarly);
  }

  if (TransGuardResult == THD_TransGuardResult_NoSignal)
    return (THD_AcquireResult_NoSignal);

  TransmissionMode = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE );
  N = bthd_transmission_mode[TransmissionMode];
  GuardInterval = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, GUARD);

  /* Co-Channel */
  if (h->pAcquireParam->CommonAcquireParam.CoChannelMode == THD_CoChannelMode_Auto)
  	{
#ifdef SmartNotchEnabled  
		if (h->pInternalAcquireParam->SmartNotchPresent)  
    	BREG_WriteField(h->hRegister, THD_CORE_FRZ, NOTCH_FRZ ,0x20 );     

    else
#endif    
    BREG_WriteField(h->hRegister, THD_CORE_FRZ, NOTCH_FRZ, 0x30 );	
  	}

  /* Impulse Noise */
  if (h->pInternalAcquireParam->ImpulseMode == THD_ImpulseMode_Auto) {                        
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_IMPE_MA_THR,0x00118eed);             /* Set impulse eraser moving averager threshold to -7dB */
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_IMPE_INST_THR,0x000a24b0);           /* Set impulse eraser instantaneous threshold to -9dB */
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_IMPE,0x00000040);                    /* Enable impulse eraser,11-tap moving average */
    value = BREG_Read32(h->hRegister,  BCHP_THD_CORE_IMPE_ERASE_CNT);
    if (value < 50)
      BREG_Write32(h->hRegister,  BCHP_THD_CORE_IMPE,0x00000041);                  /* Disable impulse eraser,11-tap moving average */
  }

  /* FFT Window and Loops */
#ifndef FASTACQ
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_COEF,0x02000000);                   /* Carrier loop Ki=2^(-6),Kl=0 */
#else
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_COEF,0x08000000);                   /* Carrier loop Ki=2^(-4),Kl=0 */
#endif
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_TL_COEF,0x02000000 * (N/2048));        /* Timing loop  Ki=2^(-6)*N/2048,Kl=0 (freq detector acquisition BW) */

#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
  /* -HZ BTHD_P_SetFW(h,FFTWindowMode,TransmissionMode,GuardInterval); */
  BTHD_P_IsdbtSetFWFtt(h,FFTWindowMode,TransmissionMode,GuardInterval);	
  }
  else
#endif
  {
  BTHD_P_DvbtSetFWFtt(h,FFTWindowMode,TransmissionMode,GuardInterval);	
  }
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CL_FRZ, 0 );                         /* Unfreeze carrier loop */ 


  if (BREG_ReadField(h->hRegister,  THD_CORE_INTR_STATUS, FW_SYNC))
    BDBG_WRN(("BTHD_P_AcquireInit:\tFW_SYNC Interrupt Status is set before FW Search!\n"));
  BREG_WriteField(h->hRegister, THD_CORE_INTR_STATUS_CLR, FW_SYNC, 1);             /* Clear interrupt */
  BKNI_ResetEvent(h->hFwSyncEvent);
  BINT_EnableCallback( h->hFwSyncCallback);
  BREG_WriteField(h->hRegister, THD_CORE_RST, FW_RST, 0);                          /* Release FFT window reset */  
/*    BDBG_MSG((" ***FW_SYNC_begin "));  */
  eEventWaitResult = BTHD_P_WaitForEventOrAbort(h, h->hFwSyncEvent, 200);
/*    BDBG_MSG((" ***FW_SYNC_end "));  */
  BINT_DisableCallback(h->hFwSyncCallback);
  BKNI_ResetEvent(h->hFwSyncEvent);

  if ( eEventWaitResult == BTHD_ERR_ACQUIRE_ABORTED ) {
    return (THD_AcquireResult_AbortedEarly);
  }
  if ( eEventWaitResult != BERR_SUCCESS && eEventWaitResult != BTHD_ERR_ACQUIRE_ABORTED ) {
    BDBG_MSG(("\nBTHD_P_AcquireInit:\t failed THD_CORE_INTR_STATUS = %80x\t", BREG_Read32(h->hRegister,  BCHP_THD_CORE_INTR_STATUS)));
  }
  /* Integer Carrier Acquisition */
  if (h->pAcquireParam->CommonAcquireParam.CarrierRange == THD_CarrierRange_Narrow)
    value =  48 * (N/2048);                                                        /* CP acq_range={ 48, 96,192} for {2K,4K,8K} (works for +/-210kHz offset in 8MHz channels)*/
  else
    value = 144 * (N/2048);
  BREG_WriteField(h->hRegister, THD_CORE_CP, ACQ_RANGE, value);                    /* CP acq_range=192 for {4K,8K} (temporary fix for ICE acquisition failure in Portable channel at low SNR) */
  BREG_WriteField(h->hRegister, THD_CORE_DAGC, THRESH, 0x028c4 );                  /* DAGC threshold=14dB (uint32_t)(pow(10.0,-14/10)* 262144) */
  eEventWaitResult = BTHD_P_OSleep(h,2,N,GuardInterval);
  if ( eEventWaitResult == BTHD_ERR_ACQUIRE_ABORTED ) {
    /* Acquire aborted early waiting for fwSyncEvent */
    return (THD_AcquireResult_AbortedEarly);
  }
  /* Determine ICE value which occurs most often */
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) 
    ice = BTHD_P_IsdbtSetICE(h,TransmissionMode,GuardInterval);
    else
#endif
    ice = BTHD_P_DvbtSetICE(h,TransmissionMode,GuardInterval);
#ifdef SmartNotchEnabled  
		if (h->pInternalAcquireParam->SmartNotchPresent)                      
			BREG_WriteField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH3, 0);
#endif
  /* Early-Abort occurred (during BTHD_P_IsdbtSetICE() / BTHD_P_DvbtSetICE())? */
  if ( BTHD_P_IsAbortAcquireRequested(h) ) {
    /* Acquire aborted early waiting for fwSyncEvent */
    return (THD_AcquireResult_AbortedEarly);
  }

#ifdef BTHD_ISDBT_SUPPORT
  if (h->pStatus->ThdCommonStatus.LowICECount)                                                     /* Exit on ICE lock failure */
    return (THD_AcquireResult_NoSignal);
#endif
  cl_int = BREG_Read32(h->hRegister,  BCHP_THD_CORE_CL_INT);
  if (ice >> 10)
    cl_int_ice = cl_int + ((0xfffff800 | ice) << 21);                              /* Sign extend for negative ICE value */
  else
    cl_int_ice = cl_int + (ice << 21); 
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_INT,cl_int_ice);  
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CL_FRZ, 0 );                         /* Unfreeze carrier loop */

  /* Reduce input level to BE and DAGC bandwidths */
  BREG_WriteField(h->hRegister, THD_CORE_DAGC, THRESH, 0x00a3d );                  /* DAGC threshold=20dB (uint32_t)(pow(10.0,-20/10)* 262144) */
  BREG_WriteField(h->hRegister, THD_CORE_DAGC, KL,0x8 );                           /* DAGC K=2(-14) */
#ifndef FASTACQ
  eEventWaitResult = BTHD_P_OSleep(h,10,N,GuardInterval);
#endif
  /* BREG_Write32(h->hRegister,  BCHP_THD_CORE_CP2,0x10000007 ); */                      /* CP pilot thresh=1.0, use thresh */

  BREG_WriteField(h->hRegister, THD_CORE_GLB, BE_MODE, 0 );                        /* BE=disabled */
  BREG_WriteField(h->hRegister, THD_CORE_GLB, ICE_MODE, 0 );                       /* ICE=tracking */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, TL_FRZ, 1 );                         /* Freeze timing loop */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_CFINE_FRZ,1 );                    /* Freeze fine carrier estimator */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_TFINE_FRZ,1 );                    /* Freeze fine timing estimator */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CE_AVG_FRZ, 1 );                     /* Freeze CE averager */
  BREG_WriteField(h->hRegister, THD_CORE_BYP, CPE_BYP, 1 );                        /* Bypass CPE */
  BREG_WriteField(h->hRegister, THD_CORE_CE, SP_PHASE_REACQ_MODE, 1 );             /* Enable auto-reacq SP phase */
  BREG_WriteField(h->hRegister, THD_CORE_RST, TL_RST,1 );                          /* Reset timing loop */
  BREG_WriteField(h->hRegister, THD_CORE_RST, TL_RST,0 );
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_FEC_BCH_TPS_SNR_RST ,0x3b );      /* Assert RS, FEC, BCH, TPS, and SNR resets */
  BREG_WriteField(h->hRegister, THD_CORE_RST2, NSE_AVG_RST, 1 );                   /* Assert noise averager reset*/

  if ( eEventWaitResult == BTHD_ERR_ACQUIRE_ABORTED ) {
    /* Acquire aborted early during previous delay(s) */
    return (THD_AcquireResult_AbortedEarly);
  }
  return(THD_AcquireResult_InitLockState);
}

/***************************************************************************
 * BTHD_P_AcquireSP()
 ***************************************************************************/
BTHD_RESULT BTHD_P_AcquireSP( BTHD_3x7x_ChnHandle h )
{
  BERR_Code eEventWaitResult;
  BTHD_RESULT return_val = THD_AcquireResult_InitLockState;

  /* Enable continuous pilot processing and the BE */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, TL_FRZ, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_TFINE_FRZ, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_GLB, BE_MODE, 1 );  /* BE=enabled */
  BREG_WriteField(h->hRegister, THD_CORE_GLB, ICE_MODE, 0 ); /* ICE=tracking */

  /* Scattered Pilot Synchronization */ 
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    /* Do not wait for SP Lock... unreliable at this point because TMCC has not yet identified
       which segments contain scattered pilots */ 
    BREG_WriteField(h->hRegister, THD_CORE_CE, ACE_ACT ,0x00);                    
  }
  else {
    BREG_WriteField(h->hRegister, THD_CORE_CE, ACE_ACT ,0x82);
	BINT_EnableCallback( h->hSpSyncCallback);

    eEventWaitResult = BTHD_P_WaitForEventOrAbort(h, h->hSpSyncEvent, 100);
    /* eEventWaitResult success: return_val, Failure: THD_AcquireResult_NoSPLock, or,
     * THD_AcquireResult_AbortedEarly if "Acquire Aborted Early" Event took place */
    return_val = BTHD_P_MapWaitForEventResult_To_THD_AcqResult(eEventWaitResult, return_val, THD_AcquireResult_NoSPLock);

	BINT_DisableCallback( h->hSpSyncCallback);
  }

  return(return_val);
}

/***************************************************************************
 * BTHD_P_AcquireFFTTrigger()
 ***************************************************************************/
BTHD_RESULT BTHD_P_AcquireFFTTrigger (BTHD_3x7x_ChnHandle h)
{
  BTHD_RESULT return_val = THD_AcquireResult_InitLockState;
  uint32_t N;
  THD_TransmissionMode_t TransmissionMode;
  THD_GuardInterval_t GuardInterval;
  BERR_Code eEventWaitResult;

  TransmissionMode = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE );
  N = bthd_transmission_mode[TransmissionMode];
  GuardInterval = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, GUARD);

#ifndef FASTACQ
  eEventWaitResult = BTHD_P_OSleep(h,1,N,GuardInterval);              /* Wait 64 OFDM symbols for span calculation to settle */     
#else
  eEventWaitResult = BTHD_P_OSleep(h,1,N,GuardInterval);
#endif
  if ( eEventWaitResult == BTHD_ERR_ACQUIRE_ABORTED ) {
    /* Acquire aborted early waiting for fwSyncEvent */
    return_val = THD_AcquireResult_AbortedEarly;
    goto BTHD_P_AcquireFFTTrigger_Exit;
  }

  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_SPAN,0x00000140 );      /* FFT window span scale = 1.25 (better for detecting long weak echoes) */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_CFINE_FRZ, 0 );

  /* Begin Channel Estimation and Common Phase Estimation */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CE_AVG_FRZ, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_BYP, CPE_BYP, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_CE, SP_PHASE_REACQ_MODE, 0 ); /* Disable auto-reacq SP phase */
 
  /* FFT Trigger Position */
  BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 0 );        /* Assert FFT window reference index reset (active-low) */
  BTHD_P_OSleep(h,1,N,GuardInterval);
  BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 1 );        /* De-assert FFT window reference index reset (active low) */
  BTHD_P_OSleep(h,8,N,GuardInterval);        

  if ( ! BTHD_P_GetFFTTriggerPosition( h, TransmissionMode, GuardInterval) )
  {
    if ( BTHD_P_IsAbortAcquireRequested(h) )
      return_val = THD_AcquireResult_AbortedEarly;
    else
      return_val = THD_AcquireResult_NoSPLock;

    goto BTHD_P_AcquireFFTTrigger_Exit;
  }

  if (h->pStatus->ThdCommonStatus.FFTTriggerOutOfSpan) {
    BTHD_P_WriteFICoef(h,THD_FrequencyInterpolatorMode_OutOfSpanPost); /* Setup frequency interpolator coefficients for out-of-span channels */
  }
 	
  if (h->pStatus->ThdCommonStatus.FFTTriggerMissed || h->pStatus->ThdCommonStatus.FFTTriggerOnGuard) {
  	BTHD_P_SetFFTTriggerPosition( h, TransmissionMode, GuardInterval);    
  }

  /* Early-exit abort triggered (during BTHD_P_GetFFTTriggerPosition()) ? */
  if ( BTHD_P_IsAbortAcquireRequested(h) ) {
    return_val = THD_AcquireResult_AbortedEarly;
    goto BTHD_P_AcquireFFTTrigger_Exit;
  }

  /* Re-acquire SP phase */  
  BREG_WriteField(h->hRegister, THD_CORE_CE, SP_PHASE_REACQ_MODE, 1 );             /* Enable auto-reacq SP phase */
  BINT_EnableCallback( h->hSpSyncCallback);

  eEventWaitResult = BTHD_P_WaitForEventOrAbort(h, h->hSpSyncEvent, 10);
  /* eEventWaitResult success: return_val, Failure: THD_AcquireResult_NoSPLock, or,
   * THD_AcquireResult_AbortedEarly if "Acquire Aborted Early" Event took place */
  return_val = BTHD_P_MapWaitForEventResult_To_THD_AcqResult(eEventWaitResult, return_val, THD_AcquireResult_NoSPLock);

  BINT_DisableCallback( h->hSpSyncCallback);

BTHD_P_AcquireFFTTrigger_Exit:
  return(return_val);
}

/***************************************************************************
 * BTHD_P_AcquireTPS()
 ***************************************************************************/
BTHD_RESULT BTHD_P_AcquireTPS( BTHD_3x7x_ChnHandle h)
{
  BTHD_RESULT return_val = THD_AcquireResult_InitLockState;
  uint32_t N;
  THD_TransmissionMode_t TransmissionMode;
  THD_GuardInterval_t GuardInterval;
  BERR_Code eSleepResult;

  TransmissionMode = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE );
  N = bthd_transmission_mode[TransmissionMode];
  GuardInterval = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, GUARD);

#ifndef FASTACQ
  eSleepResult = BTHD_P_OSleep(h,16,N,GuardInterval);                                 /* Wait 64 OFDM symbols for span calculation to settle */     
#else
  eSleepResult = BTHD_P_OSleep(h,10,N,GuardInterval);
#endif
  if ( eSleepResult == BTHD_ERR_ACQUIRE_ABORTED ) {
    return(THD_AcquireResult_AbortedEarly);
  }

  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_SPAN,0x00000140 );      /* FFT window span scale = 0.1875 (better for detecting long weak echoes) */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_CFINE_FRZ, 0 );

  /* Begin Channel Estimation and Common Phase Estimation */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CE_AVG_FRZ, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_BYP, CPE_BYP, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_CE, SP_PHASE_REACQ_MODE, 0 ); /* Disable auto-reacq SP phase */
 
  BREG_WriteField(h->hRegister, THD_CORE_RST, SNR_RST, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_RST2,NSE_AVG_RST, 1 );        /* Reset noise averager (necessary for proper co-channel filter detection when CP bad is used) */
  BREG_WriteField(h->hRegister, THD_CORE_RST2,NSE_AVG_RST, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_RST2,CP_BAD_RST, 0 );

  /* Search for TPS (DVB-T) or TMCC (ISDB-T) */
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    BTHD_P_IsdbtSetFEC(h);
    BTHD_P_IsdbtSetOI(h);
    return_val = BTHD_P_IsdbtSetTMCC(h, TransmissionMode, GuardInterval);  /* Setup TMCC and wait for lock or timeout */
  } 
  else
#endif
    return_val = BTHD_P_DvbtSetTPS(h, TransmissionMode, GuardInterval);    /* Setup TPS and wait for lock or timeout */

  return(return_val);
}

/***************************************************************************
 * BTHD_P_AcquireFEC()
 ***************************************************************************/
BTHD_RESULT BTHD_P_AcquireFEC( BTHD_3x7x_ChnHandle h, bool FirstTime )
{
  BTHD_RESULT return_val = THD_AcquireResult_InitLockState;
  uint32_t N,notchwidth;
  bool CoChannelPresent = false;
  THD_TransmissionMode_t TransmissionMode;
  THD_GuardInterval_t GuardInterval;
 #ifdef BTHD_ISDBT_SUPPORT  
  BERR_Code eEventWaitResult;
  uint32_t segs_b=0, segs_c=0;
 #endif
  TransmissionMode = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE );
  N = bthd_transmission_mode[TransmissionMode];
  GuardInterval = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, GUARD);

  BREG_WriteField(h->hRegister, THD_CORE_CE, SP_PHASE_REACQ_MODE, 0 ); /* Disable auto-reacq SP phase */

  /* Configure OI and wait for FEC sync (DVB-T only) */
  if (h->pAcquireParam->CommonAcquireParam.Standard != THD_Standard_ISDBT) {
    BTHD_P_DvbtSetOI(h);                                               /* Setup output interface */
    return_val = BTHD_P_DvbtSetFEC(h);                                 /* Setup FEC and wait for lock or timeout */
  }
  
  if (FirstTime) {
    /* Check for Cochannel Interference and remove notch filter if not needed */
    if (h->pAcquireParam->CommonAcquireParam.CoChannelMode != THD_CoChannelMode_None) {
      /* Need to enable the CE averager in order to estimate the noise */
      BREG_WriteField(h->hRegister, THD_CORE_CE, AVG_FF, 0x0);                       /* CE averager beta = 2^(-4) */
      BREG_WriteField(h->hRegister, THD_CORE_BYP, CE_AVG_BYP, 0);                    /* CE averager unbypassed */
      if (TransmissionMode == THD_TransmissionMode_2k)
        BTHD_P_OSleep(h,64,N,GuardInterval);
      else
        BTHD_P_OSleep(h,32,N,GuardInterval);
      
      /* Determine CCI presence using snooper (Look for video, color, or audio to guarantee robust CCI detection) */
  #ifdef BTHD_ISDBT_SUPPORT    
      if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT)
        CoChannelPresent = BTHD_P_IsdbtGetNotch(h, TransmissionMode);
      else {
  #endif    
  			BREG_WriteField(h->hRegister, THD_CORE_BYP, EXP_AVG_NSE, 0x0 );              /* EQ nse + avg nse exponents unbypassed */
  			CoChannelPresent = BTHD_P_DvbtGetNotch(h, TransmissionMode);
  #ifdef BTHD_ISDBT_SUPPORT    
  		}        
  #endif		
  		BREG_WriteField(h->hRegister, THD_CORE_BYP, CE_AVG_BYP, 1);                    /* CE averager bypassed */
  		if (!CoChannelPresent) {
          uint32_t uNotchWidthStartTimeCnt = BTHD_P_Read_FSCnt_Time(h);
          do {                                                                         /*  Gradually reduce notch to avoid burst error */
            notchwidth = BREG_ReadField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH4);
            if (notchwidth)
              BREG_WriteField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH4, notchwidth - 1);
            notchwidth = BREG_ReadField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH3);
            if (notchwidth)
              BREG_WriteField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH3, notchwidth - 1);
            notchwidth = BREG_ReadField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH2);
            if (notchwidth)
              BREG_WriteField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH2, notchwidth - 1);
            notchwidth = BREG_ReadField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH1);
            if (notchwidth)
              BREG_WriteField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH1, notchwidth - 1);
            notchwidth = BREG_ReadField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH0);
            if (notchwidth)
              BREG_WriteField(h->hRegister, THD_CORE_NOTCH_WIDTH, WIDTH0, notchwidth - 1);
            notchwidth = BREG_Read32(h->hRegister, BCHP_THD_CORE_NOTCH_WIDTH);

            /* Delay, and check if Early-Exit was requested while delaying */
            if ( BTHD_ERR_ACQUIRE_ABORTED == BTHD_P_OSleep(h,3,N,GuardInterval) ) {
                return_val = THD_AcquireResult_AbortedEarly;
                goto BTHD_P_AcquireFEC_Exit;
            }
            /* Check 1 second exceeded adjusting notch */
            if ( BTHD_P_Check_FSCnt_TimeExceeded(h, uNotchWidthStartTimeCnt, BTHD_P_MSEC_TO_FSCNT(1000)) ) {
              BDBG_MSG(("\t\tNotchwidth whileLoop time out at 0x%08X", BTHD_P_Read_FSCnt_Time(h)));
              return_val = THD_AcquireResult_NoFECLock;
              goto BTHD_P_AcquireFEC_Exit;
            }
          } while (notchwidth);
  		}
    }

    /* Check if Early-Exit occurred */
    if ( BTHD_P_IsAbortAcquireRequested(h) ) {
      /* Acquire aborted early waiting for fwSyncEvent */
      return_val = THD_AcquireResult_AbortedEarly;
      goto BTHD_P_AcquireFEC_Exit;
    }

    /* Update status structure with auto-detected modes */  
    h->pStatus->ThdCommonStatus.CoChannelPresent = CoChannelPresent;
    if (h->pStatus->ThdCommonStatus.CoChannelPresent)
      h->pStatus->ThdCommonStatus.CoChannelMode = THD_CoChannelMode_Auto;
    else
      h->pStatus->ThdCommonStatus.CoChannelMode = THD_CoChannelMode_None;
  }
  BREG_WriteField(h->hRegister,  THD_CORE_EQ, NSE_AVG_ALL_FF, 3 );         /* EQ nse avg all beta=2^(-12) */
  
  /* Wait for data at FEC output */
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    segs_b = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_SEG);
    segs_c = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_SEG);
    BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 0 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 0 ); 
	BINT_EnableCallback( h->hRsSyncCallback);
	if (segs_b) {
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_B, 1 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_B, 0 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_B, 1 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_B, 0 ); 
	  BINT_EnableCallback( h->hRsSyncBCallback);
	}
	if (segs_c) {
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_C, 1 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_C, 0 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_C, 1 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_C, 0 ); 
	  BINT_EnableCallback( h->hRsSyncCCallback);
	}
    eEventWaitResult = BTHD_P_WaitForEventOrAbort(h, h->hRsSyncEvent, 540);
    /* eEventWaitResult success: THD_AcquireResult_Lock, Failure: THD_AcquireResult_NoFECLock, or,
     * THD_AcquireResult_AbortedEarly if "Acquire Aborted Early" Event took place */
    return_val = BTHD_P_MapWaitForEventResult_To_THD_AcqResult(eEventWaitResult, THD_AcquireResult_Lock, THD_AcquireResult_NoFECLock);

    BINT_DisableCallback( h->hRsSyncCallback);
	if (segs_b)
      BINT_DisableCallback( h->hRsSyncBCallback);		
	if (segs_c)
      BINT_DisableCallback( h->hRsSyncCCallback);		
  } else {
#endif
#if 0
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 1 );
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 1 );
  BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 0 ); 

  BINT_EnableCallback( h->hRsSyncCallback);

  eEventWaitResult = BTHD_P_WaitForEventOrAbort(h, h->hRsSyncEvent, 100);
  /* eEventWaitResult success: THD_AcquireResult_Lock, Failure: THD_AcquireResult_NoFECLock, or,
   * THD_AcquireResult_AbortedEarly if "Acquire Aborted Early" Event took place */
  return_val = BTHD_P_MapWaitForEventResult_To_THD_AcqResult(eEventWaitResult, THD_AcquireResult_Lock, THD_AcquireResult_NoFECLock);


  BINT_DisableCallback( h->hRsSyncCallback);
#endif
#ifdef BTHD_ISDBT_SUPPORT    
  }
#endif

BTHD_P_AcquireFEC_Exit:
  return(return_val);
}

#ifdef BTHD_ISDBT_SUPPORT
/***************************************************************************
 * BTHD_P_AcquireChangeFFTWindow()
 ***************************************************************************/
BTHD_RESULT BTHD_P_AcquireChangeFFTWindow( BTHD_3x7x_ChnHandle h, THD_FFTWindowMode_t FFTWindowMode)
{
  BTHD_RESULT return_val = THD_AcquireResult_InitLockState;
  uint32_t N;
  THD_TransmissionMode_t TransmissionMode;
  THD_GuardInterval_t GuardInterval;
  BERR_Code eEventWaitResult;

  /* Determine N */
  TransmissionMode = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE );
  N = bthd_transmission_mode[TransmissionMode];
  GuardInterval = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, GUARD);

#ifndef ChangeFFTWindowSeamless     
  BREG_WriteField(h->hRegister, THD_CORE_GLB, BE_MODE, 0 );                        /* BE=disabled */
  BREG_WriteField(h->hRegister, THD_CORE_GLB, ICE_MODE, 0 );                       /* ICE=tracking */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, TL_FRZ, 1 );                         /* Freeze timing loop */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_CFINE_FRZ,1 );                    /* Freeze fine carrier estimator */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_TFINE_FRZ,1 );                    /* Freeze fine timing estimator */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CE_AVG_FRZ, 1 );                     /* Freeze CE averager */
  BREG_WriteField(h->hRegister, THD_CORE_BYP, CPE_BYP, 1 );                        /* Bypass CPE */
  BREG_WriteField(h->hRegister, THD_CORE_CE, SP_PHASE_REACQ_MODE, 1 );             /* Enable auto-reacq SP phase */
  BREG_WriteField(h->hRegister, THD_CORE_RST, TL_RST,1 );                          /* Reset timing loop */
  BREG_WriteField(h->hRegister, THD_CORE_RST, TL_RST,0 );
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_FEC_BCH_TPS_SNR_RST ,0x3b );      /* Assert RS, FEC, BCH, TPS, and SNR resets */
  BREG_WriteField(h->hRegister, THD_CORE_RST2, NSE_AVG_RST, 1 );                   /* Assert noise averager reset*/
#endif

  /* Reset/freeze various modules */
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_FEC_BCH_TPS_SNR_RST ,0x38 );      /* Assert RS, FEC, BCH */
  /* BREG_WriteField(h->hRegister, THD_CORE_CE, SP_PHASE_REACQ_MODE, 1 ); */       /* Enable auto-reacq SP phase */
  BREG_Write32(h->hRegister, BCHP_THD_CORE_FW_WIN,0x00000000);                     /* Disable time-domain windowing */

#ifndef ChangeFFTWindowSeamless
  /* Non-seamless switch of FFT window mode (requires SP, TPS re-acquire) */
  BTHD_P_SetFW(h, FFTWindowMode, TransmissionMode, GuardInterval);

  BINT_EnableCallback( h->hFwSyncCallback);

  eEventWaitResult = BTHD_P_WaitForEventOrAbort(h, h->hFwSyncEvent, 200);
  /* eEventWaitResult success: return_val, Failure: THD_AcquireResult_NoFFTLock, or,
   * THD_AcquireResult_AbortedEarly if "Acquire Aborted Early" Event took place */
  return_val = BTHD_P_MapWaitForEventResult_To_THD_AcqResult(eEventWaitResult, return_val, THD_AcquireResult_NoFFTLock);


  BINT_DisableCallback( h->hFwSyncCallback);

  BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 0 );                    /* Reset FFT window reference index */
  BTHD_P_OSleep(h,1,N,GuardInterval);  
  BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 1 );
#else
  /* Seamless switch of FFT window mode (hopefully eliminates SP, TPS re-acquire) */
  BINT_EnableCallback( h->hFwSyncCallback);
  BKNI_WaitForEvent(h->hFwSyncEvent, 10);
  BTHD_P_SetFW(h, FFTWindowMode, TransmissionMode, GuardInterval);
  BKNI_WaitForEvent(h->hFwSyncEvent, 10);
  BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 0 );                    /* Assert FFT window reference index reset (active-low) */
  /* BREG_Write32(h->hRegister, h->hRegister, BCHP_THD_CORE_FBCNT, N>>1  ); */
  BKNI_WaitForEvent(h->hFwSyncEvent, 10);
  BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 1 );                    /* Deassert FFT window reference index reset (active-low) */
  BINT_DisableCallback( h->hFwSyncCallback);
 
#endif

  return(return_val);
}
#endif

/***************************************************************************
 * BTHD_P_AcquireTrack()
 ***************************************************************************/
BTHD_RESULT BTHD_P_AcquireTrack( BTHD_3x7x_ChnHandle h, THD_FFTWindowMode_t FFTWindowMode, THD_ChannelEstimatorMode_t ChannelEstimatorMode)
{
  BTHD_RESULT return_val = THD_AcquireResult_InitLockState;
  uint32_t N;
  THD_TransmissionMode_t TransmissionMode;
  THD_GuardInterval_t GuardInterval;
  BERR_Code eEventWaitResult;
#ifdef BTHD_ISDBT_SUPPORT
  uint32_t segs_b, segs_c;
#endif
  TransmissionMode = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE );
  N = bthd_transmission_mode[TransmissionMode];
  GuardInterval = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, GUARD);

  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FEC_SYNC,0x07001f1f );                 /* Force FEC sync retention */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_VIT,0x00000687 );                      /* Force Viterbi sync retention */    
  BTHD_P_DvbtSetFrame(h);
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) 
		BREG_Write32(h->hRegister,  BCHP_THD_CORE_DAGC,0x05000A3D );                     /* -HZ, reduce BW for DirecTV AWGN, DAGC threshold=20dB, thresh = (thd_u32)(pow(10.0,-17/10)* 262144), K=2^(-8) */
  else
	  BREG_Write32(h->hRegister,  BCHP_THD_CORE_DAGC,0x04000A3D );                     /* DAGC threshold=20dB, thresh = (thd_u32)(pow(10.0,-17/10)* 262144), K=2^(-8) */

  /* Channel Estimator */
  if (h->pInternalAcquireParam->ChannelEstimatorMode == THD_ChannelEstimatorMode_Auto)
    ChannelEstimatorMode = THD_ChannelEstimatorMode_Fixed;
  else
    ChannelEstimatorMode = h->pInternalAcquireParam->ChannelEstimatorMode;
  BTHD_P_SetCE(h, ChannelEstimatorMode);
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_MISC,0x00000208 );                  /* FFT window leak to timing loop enabled w/limiting (prevents "V" noise response) */

  /* Common Gain Estimator */
  BREG_WriteField(h->hRegister, THD_CORE_CP_CGE, THRESH, 0x16b );                  /* CGE target back-off = 9dB (floor(10^(-9/20)*2^10)) */
  if (h->pAcquireParam->CommonAcquireParam.Standard != THD_Standard_ISDBT)
    BREG_WriteField(h->hRegister, THD_CORE_CP_CGE, ENABLE, 1 );                    /* CGE enable */

  /* Reduce Timing and Carrier Bandwidths */
  if (!BREG_ReadField(h->hRegister, THD_CORE_NOTCH_STATUS1,BYP_DP))
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_TL_COEF,0x00100000 * (N/2048) );     /* Ki=2^(-11)*N/2048,Kl=2^0 (better for CCI) */  
  else
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_TL_COEF,0x00020000 * (N/2048) );     /* Ki=2^(-14)*N/2048,Kl=2^0 (better for echo tests inside/outside guard interval) */
  
  /*BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_COEF,0x08000000);*/                   /* Carrier loop Ki=2^(-4),Kl=0 for improved phase noise tracking */

  /* FW Tracking Parameters */
  BREG_WriteField(h->hRegister, THD_CORE_FW, LIMIT, 0);                            /* FW slip_limit=0 (fixes "breathing noise response") */
  BREG_WriteField(h->hRegister, THD_CORE_FW, AVG_FF, 7);                           /* Reduce FW avg. FF  */

  if (!(h->pStatus->ThdCommonStatus.FFTTriggerMissed || h->pStatus->ThdCommonStatus.FFTTriggerOnGuard)) {
    BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 1 );                    /* Reset FFT window reference index */
    BTHD_P_OSleep(h,1,N,GuardInterval);  
    BREG_WriteField(h->hRegister, THD_CORE_RST2, FW_REF_RST, 0 );
    }


  /* Cochannel Interference Tracking Parameters */
  if ((h->pAcquireParam->CommonAcquireParam.CoChannelMode != THD_CoChannelMode_None) && h->pStatus->ThdCommonStatus.CoChannelPresent) {
    if (ChannelEstimatorMode == THD_ChannelEstimatorMode_Fixed) {                 /* Enable timing PD for fixed mode */
      BREG_WriteField(h->hRegister, THD_CORE_CP, F_SPACE, 2 );                     /* 12-bin timing pd pilot spacing (mandatory for long-echo channels)*/
      BREG_WriteField(h->hRegister, THD_CORE_CP, PHASE_FREQ_SELECT, 1 );           /* Timing pd enable*/
      BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_TPHASE_FRZ, 0 );              /* Unfreeze timing pd  */
      BREG_Write32(h->hRegister,  BCHP_THD_CORE_TL_COEF,0x00010200 * (N/2048) );   /* Ki=2^(-13),Kl=2^(-4) (for CCI)*/
      BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_MISC,0x00000200 );              /* FFT window leak to timing loop disabled (prevents "wandering" at low SNR) */
    }  

    BTHD_P_DvbtSetEq(h, THD_CoChannelMode_Auto);  

    if (TransmissionMode == THD_TransmissionMode_2k)
      BREG_WriteField(h->hRegister, THD_CORE_EQ, EXP_OFFSET, 0x3f8);               /* EQ exponent offset = -8 for 2K (helps with color pattern (not color bars))*/
    if (TransmissionMode == THD_TransmissionMode_8k)
      BREG_WriteField(h->hRegister, THD_CORE_CE, AVG_FF, 0x1);                     /* CE set beta=2^-2 for 8K w/CCI*/

    BREG_WriteField(h->hRegister, THD_CORE_BYP, NOTCH_BYP, 0);                     /* Unbypass notch filter*/
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_NOTCH_DEPTH,0x0000081a );
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_NOTCH_WIDTH,0x00000bcb );
#ifdef SmartNotchEnabled
		if (h->pInternalAcquireParam->SmartNotchPresent)  
    	BREG_WriteField(h->hRegister, THD_CORE_FRZ, NOTCH_FRZ ,0x20 ); 							 /* Unfreeze Notch DDFS 0,1,2,3,0M */                
    else
#endif    
    BREG_WriteField(h->hRegister, THD_CORE_FRZ, NOTCH_FRZ ,0x30 );                 /* Unfreeze Notch DDFS,2,1,0,0M*/
  }
#if 1
  /* Bad Pilot Estimation */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CP_BAD,0x00000c41 );     /* CP bad thresh=9dB, window=+/-4 pilots, auto, enabled for CPP */
  BREG_WriteField(h->hRegister, THD_CORE_CP_CPE, USE_CP_BAD, 1 );
  BREG_WriteField(h->hRegister, THD_CORE_CP_CGE, USE_CP_BAD, 1 );
  BREG_WriteField(h->hRegister, THD_CORE_CE_LMS, TIME_USE_CP_BAD, 1);
#endif
  /* Adaptive Time/Frequency Interpolators */
  if (h->pAcquireParam->CommonAcquireParam.Standard != THD_Standard_ISDBT) {
      BREG_WriteField(h->hRegister, THD_CORE_CE_LMS, TIME_MU, 0 );                   /* Adaptive time interpolator mu=2^(-2) */    
	  BREG_WriteField(h->hRegister, THD_CORE_CE_LMS, TIME_ENABLE, 1 );               /* Adaptive time interpolator enable */
    /*BREG_WriteField(h->hRegister, THD_CORE_CE_LMS, FREQ_ENABLE, 1 );  */             /* Adaptive frequency interpolator enable */
  }

  /* Wait for data at FEC output */
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    segs_b = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_SEG);
    segs_c = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_SEG);

    BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 0 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 1 );
    BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 0 ); 
	BINT_EnableCallback( h->hRsSyncCallback);
	if (segs_b) {
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_B, 1 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_B, 0 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_B, 1 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_B, 0 ); 
	  BINT_EnableCallback( h->hRsSyncBCallback);
	}
	if (segs_c) {
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_C, 1 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_ERC_RST_C, 0 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_C, 1 );
      BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST_C, 0 ); 
	  BINT_EnableCallback( h->hRsSyncCCallback);
	}
    eEventWaitResult = BTHD_P_WaitForEventOrAbort(h, h->hRsSyncEvent, 514);
    /* eEventWaitResult success: THD_AcquireResult_Lock, Failure: THD_AcquireResult_NoFECLock, or,
     * THD_AcquireResult_AbortedEarly if "Acquire Aborted Early" Event took place */
    return_val = BTHD_P_MapWaitForEventResult_To_THD_AcqResult(eEventWaitResult, THD_AcquireResult_Lock, THD_AcquireResult_NoFECLock);

    BINT_DisableCallback( h->hRsSyncCallback);
	if (segs_b)
      BINT_DisableCallback( h->hRsSyncBCallback);		
	if (segs_c)
      BINT_DisableCallback( h->hRsSyncCCallback);	

    /* -HZ, fixed for ISDB-T, only turn on FW_WIN if InSpan, N=4K or 8K, and CE=Fixed */
    if (h->pStatus->ThdCommonStatus.FFTTriggerInSpan && (TransmissionMode != THD_TransmissionMode_2k) && (ChannelEstimatorMode == THD_ChannelEstimatorMode_Fixed)) {
		BREG_Write32(h->hRegister, BCHP_THD_CORE_FW_WIN,0x00000001);                   /* Time-domain windowing enable */  
/*      BDBG_MSG(("\tCE in Fixed, turn on FW_WIN"));  */
    }
  } else {
#endif
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 1 );                     /* Reset FEC error counters */
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_ERC_RST, 0 );
  BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 1 );                   /* Reset lock detector */
  BREG_WriteField(h->hRegister, THD_CORE_RST2, RS_SYNC_RST, 0 );   

  /* -HZ, fixed for DVB-T, only turn on FW_WIN if InSpan, N=4K or 8K, and CE=Fixed, need to regress */
  /* if (h->pStatus->ThdCommonStatus.FFTTriggerInSpan && (TransmissionMode != THD_TransmissionMode_2k)) */
  if (h->pStatus->ThdCommonStatus.FFTTriggerInSpan && (TransmissionMode != THD_TransmissionMode_2k) && (ChannelEstimatorMode == THD_ChannelEstimatorMode_Fixed)) 
    BREG_Write32(h->hRegister, BCHP_THD_CORE_FW_WIN,0x00000001);                   /* Time-domain windowing enable */  

  BINT_EnableCallback( h->hRsSyncCallback);

  eEventWaitResult = BTHD_P_WaitForEventOrAbort(h, h->hRsSyncEvent, 200);
  /* eEventWaitResult success: THD_AcquireResult_Lock, Failure: THD_AcquireResult_NoFECLock, or,
   * THD_AcquireResult_AbortedEarly if "Acquire Aborted Early" Event took place */
  return_val = BTHD_P_MapWaitForEventResult_To_THD_AcqResult(eEventWaitResult, THD_AcquireResult_Lock, THD_AcquireResult_NoFECLock);


  BINT_DisableCallback( h->hRsSyncCallback);

#ifdef BTHD_ISDBT_SUPPORT
  }
#endif
  /* Update status structure with auto-detected modes */
  h->pStatus->ThdCommonStatus.ChannelEstimatorMode = ChannelEstimatorMode;
  h->pStatus->ThdCommonStatus.FFTWindowMode = FFTWindowMode;

  return(return_val);
}

/***************************************************************************
 * BTHD_P_AcquireCheckLock()
 ***************************************************************************/
BTHD_RESULT BTHD_P_AcquireCheckLock( BTHD_3x7x_ChnHandle h )
{
  BTHD_RESULT return_val = THD_AcquireResult_InitLockState;
  uint32_t N, rs_rt_ctrl, rs_rt_ctrl_b, rs_rt_ctrl_c;
  THD_TransmissionMode_t TransmissionMode;
  THD_GuardInterval_t GuardInterval;
  BERR_Code eSleepResult;

  h->pInternalAcquireParam->AllowRsSyncEvent = false; /* Must disable this, otherwise lock ISR will not set the lock status */

  rs_rt_ctrl = BREG_Read32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL);
  rs_rt_ctrl_b = BREG_Read32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL_B);
  rs_rt_ctrl_c = BREG_Read32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL_C);

#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
/*    BTHD_P_IsdbtSetRsAc(h, 3, 2);   */
	BTHD_P_IsdbtSetRsRt(h, 1, 2);		  /* evaluate 3000/2^3=  800 packets for three layers */
  } else 
#endif
    BREG_Write32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL, 0x00ff0100);  /* RS lock lost if >255 (uncorrected+corrected) out of 256 total (i.e. no clean blocks out of 256) */   
  BTHD_P_EnableLockInterrupts(h);  

  TransmissionMode = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE );
  N = bthd_transmission_mode[TransmissionMode];
  GuardInterval = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, GUARD);
 
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT)
    eSleepResult = BTHD_P_OSleep(h,300,N,GuardInterval);
  else 
#endif
    eSleepResult = BTHD_P_OSleep(h,75,N,GuardInterval);

  if ( eSleepResult == BTHD_ERR_ACQUIRE_ABORTED ) {
    return_val = THD_AcquireResult_AbortedEarly;
  }
  else if (!((h->ThdLockStatus >> THD_LockStatusBit_SystemLock) & 1)) {
    return_val = THD_AcquireResult_NoFECLock;
  } 
  else {
    return_val = THD_AcquireResult_Lock;
#ifdef BTHD_ISDBT_SUPPORT
    if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
      BREG_Write32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL,   rs_rt_ctrl);    
      BREG_Write32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL_B, rs_rt_ctrl_b);    
      BREG_Write32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL_C, rs_rt_ctrl_c);    
    } else
#endif
    BREG_Write32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL, rs_rt_ctrl);    
  }
  return(return_val);
}

/***************************************************************************
 * BTHD_P_AcquireChangeChannelEstimator()
 ***************************************************************************/
BTHD_RESULT BTHD_P_AcquireChangeChannelEstimator(BTHD_3x7x_ChnHandle h, THD_ChannelEstimatorMode_t ChannelEstimatorMode)
{
  uint32_t N;
  THD_TransmissionMode_t TransmissionMode;
  THD_GuardInterval_t GuardInterval;
#ifdef BTHD_ISDBT_SUPPORT
  uint32_t segs_b, segs_c, isdbt_ti=0, isdbt_ti_b=0, isdbt_ti_c=0;
#endif /* BTHD_ISDBT_SUPPORT */

  TransmissionMode = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE );
  N = bthd_transmission_mode[TransmissionMode];
  GuardInterval = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, GUARD);

  BTHD_P_SetCE(h, ChannelEstimatorMode);
  /* -HZ, FW_WIN should be set in AcquireTrack, only turn off FW_WIN if CE is changed to Mobile mode */
  if (ChannelEstimatorMode != THD_ChannelEstimatorMode_Fixed) {
    BDBG_MSG(("\tAcquireChangeChannelEstimator: Pedestrian and Mobile"));
    if (TransmissionMode != THD_TransmissionMode_2k)
      BREG_Write32(h->hRegister, BCHP_THD_CORE_FW_WIN,0x00000000);         /* Disable time-domain windowing */
  }
  BREG_Write32(h->hRegister, BCHP_THD_CORE_FW_MISC,0x00000208 );           /* FFT window leak to timing loop enabled w/limiting*/
  BREG_Write32(h->hRegister, BCHP_THD_CORE_TL_COEF,0x00020000 * (N/2048)); /* Ki=2^(-14)*N/2048,Kl=2^0 (better for echo tests inside/outside guard interval)*/
  BREG_WriteField(h->hRegister, THD_CORE_CP, PHASE_FREQ_SELECT, 0 );       /* Timing FD enable */
    
#ifdef BTHD_ISDBT_SUPPORT
  if (h->pAcquireParam->CommonAcquireParam.Standard == THD_Standard_ISDBT) {
    /*  Add delay for data to flush through time interleaver based on max time interleaver depth */
    segs_b = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_SEG);
    segs_c = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_SEG);    
    isdbt_ti   = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_TI);
    isdbt_ti_b = segs_b ? BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_TI) : 0;
    isdbt_ti_c = segs_c ? BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_TI) : 0;
    isdbt_ti   = (isdbt_ti > isdbt_ti_b) ? isdbt_ti : isdbt_ti_b;
    isdbt_ti   = (isdbt_ti > isdbt_ti_c) ? isdbt_ti : isdbt_ti_c;
    BTHD_P_OSleep(h,(96*isdbt_ti),N,GuardInterval);                        /*  Time for TI to flush */       
    BDBG_MSG(("BTHD_P_AcquireChangeChannelEstimator:\tWaiting %d symbols for TI to flush\n",96*isdbt_ti));
  } else
#endif
    BTHD_P_OSleep(h,30,N,GuardInterval);

  return(THD_AcquireResult_InitLockState);
}


#endif