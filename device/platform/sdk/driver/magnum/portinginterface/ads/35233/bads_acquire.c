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
 * $brcm_Workfile: bads_acquire.c $
 * $brcm_Revision: 140 $
 * $brcm_Date: 11/15/12 11:13a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/ads/bads_acquire.c $
 * 
 * 140   11/15/12 11:13a farshidf
 * SW3128-1: move SPUR_TBL_u32[SPUR_TBL_SIZE] to bads_utils.c concern 7552
 *  only
 * 
 * Fw_Integration_Devel/39   11/15/12 11:11a farshidf
 * SW3128-1: move SPUR_TBL_u32[SPUR_TBL_SIZE] to bads_utils.c concern 7552
 *  only
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/22   11/15/12 11:10a farshidf
 * SW3128-1: move SPUR_TBL_u32[SPUR_TBL_SIZE] to bads_utils.c concern 7552
 *  only
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/21   10/5/12 3:07p mpovich
 * SW3128-87: Merge to dev branch (fix BTMR ReadTimer function to return
 *  value in microseconds).
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/SW3128-87/1   10/5/12 11:57a mpovich
 * SW3128-87: Encapsulate BTMR max timer period (MIPS_TIMER) inside BTMR
 *  module.  Modify BTMR_ReadTimer to return timer in microseconds (not in
 *  clock counts).
 * 
 * 137   10/2/12 6:59p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/36   10/2/12 6:32p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/19   9/27/12 6:49p cbrooks
 * sw3128-227:added FFT Size compensation
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/18   9/27/12 4:20p cbrooks
 * sw3128-227:Added Edefine in defs.h option for equalizer size, default
 *  is 36/36 w/18 overlap, option i n 18/24 w/6 overlap
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/17   9/27/12 11:46a cbrooks
 * sw3128-226:fiixed samsung cable on/off, scaling, mark reset
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/16   9/14/12 10:10a mpovich
 * SW3128-207:  3128 ADS cleanup and removal of unused HAB commands (per
 *  CBrooks).
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/15   8/23/12 6:09p mpovich
 * SW3128-207: Remove use of driver local parameters from API level
 *  module.
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/14   8/23/12 3:43p cbrooks
 * sw3128-1:removed ReSync_Count
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/13   8/17/12 4:23p cbrooks
 * sw3128-1:Code Cleanup
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/12   8/6/12 6:33p cbrooks
 * sw3128-1:Code Cleanup
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/11   7/26/12 2:46p farshidf
 * SW7584-41: initial clean up
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/10   7/24/12 4:01p cbrooks
 * sw3128-1:Added delay in burst detection for Annex A for a long delay
 *  problem
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/9   7/18/12 4:42p cbrooks
 * sw3128-1:rearranged functions to start cleanup
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/8   7/18/12 11:33a cbrooks
 * sw3128-1:fixec AGCC error
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/7   7/16/12 5:19p cbrooks
 * sw3128-1:undid fix for now
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/5   7/16/12 3:01p cbrooks
 * sw3128-1:Added burst changes for motorola: DAGC to first order and
 *  reduce bandwidth CPL Integrator leak from 9 to B, CPL phase
 *  accumulator leak from 0 to D
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/4   6/27/12 4:25p farshidf
 * SW3128-1: fix compile issue
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/3   6/22/12 3:12p farshidf
 * SW7425-2983:  Remove symbolic links and flatten out ADS and AOB
 *  directory structure
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/2   6/14/12 4:42p farshidf
 * SW7552-289: Add SPUR_TBL for DS
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/1   4/27/12 5:21p farshidf
 * SW3462-19: merge to 5.0 branch
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/SW3462-6/2   4/16/12 6:07p mpovich
 * SW3462-6: Rebase with devel branch.
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/3   3/29/12 6:20p farshidf
 * SW3128-139: remove warning
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/2   3/19/12 6:12p cbrooks
 * sw3128-132:Fixed Annex A symbol rate reporting
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/SW3462-6/1   2/28/12 6:01p mpovich
 * SW3462-6: Rebase with SW3462-3 dev. branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3462-3/1   2/27/12 5:02p mpovich
 * SW3462-3: 3462 initial baseline development and initial HAB support.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3462-2/3   2/27/12 4:29p mpovich
 * SW3462-2: Initial support for T-2 in 3462.
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/1   2/28/12 12:29p cbrooks
 * sw3128-1:fixed SlowScan mode
 * 
 * Fw_Integration_Devel/24   2/9/12 12:14p farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/15   2/9/12 11:40a cbrooks
 * sw3128-1:fixed baud loop droppouts
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/14   1/30/12 5:16p cbrooks
 * sw3128-1:Fixxed FFT window, added FOI timing gain
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/13   1/25/12 6:02p cbrooks
 * sw3128-1:changed timeout in FFT routine
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3462-2/2   1/30/12 5:35p mpovich
 * SW3462-2: Support for 3462.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3462-2/1   1/12/12 5:54p mpovich
 * SW3462-2: Support for 3462.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/12   12/22/11 7:51p mpovich
 * SW3461-105: Eliminate chance of infinite loop in ADS Delay function.
 * 
 * Fw_Integration_Devel/22   12/22/11 6:36a mpovich
 * SW3461-105: Eliminate chance of infinite loop in ADS Delay function.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/11   12/22/11 6:32a mpovich
 * SW3461-105: Eliminate chance of infinite loop in ADS Delay function.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/10   12/21/11 6:01p mpovich
 * SW3461-105: Eliminate chance of infinite loop in ADS Delay function.
 * 
 * Fw_Integration_Devel/21   12/15/11 12:22p farshidf
 * SW3461-118: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/9   12/12/11 4:13p cbrooks
 * sw3128-1:added new coeffs for FOI timing loop
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/8   11/21/11 6:54p mpovich
 * SW3128-71: Support for a single, common 3128 chip family F/W binary.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3128-71/1   11/17/11 6:58p mpovich
 * SW3128-71: Support for common 3128 family chip F/W.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/7   10/31/11 3:51p cbrooks
 * sw3128-1:Fixed delay in FFT
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/6   10/28/11 12:25p farshidf
 * SW3461-75: fix compile error for A0 build
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/5   10/28/11 12:10p farshidf
 * SW3461-75: Move to new IRQ handling
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/4   10/24/11 10:50a farshidf
 * SW3128-55: new power up code
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/3   10/21/11 6:29p cbrooks
 * sw3128-1:Added fix to CWC
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/2   10/21/11 6:16p cbrooks
 * sw3128-1:added Taks constant spur freequency
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/1   10/21/11 4:15p cbrooks
 * sw3128-1:fixed symbol rate lookup for ACI rejection fine tuning
 * 
 * Fw_Integration_Devel/17   10/18/11 6:38p farshidf
 * SW7552-134: reset FE fifo
 * 
 * Fw_Integration_Devel/16   10/12/11 2:30p farshidf
 * SW3128-54: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/8   10/12/11 11:43a mpovich
 * SW3128-38: Merge ADS Early Exit feature to devel. branch.
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/7   10/6/11 11:44a farshidf
 * SW3128-1: add support for 3128 C0
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/SW3128-38/3   10/5/11 4:18p mpovich
 * SW3128-38: Early exit feature - encapsulate early exit flag within
 *  bads_acquire module.
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/SW3128-38/2   10/4/11 3:02p mpovich
 * SW3128-38: Early exit feature for downstream acquire.
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/SW3128-38/1   10/4/11 12:53a mpovich
 * SW3128-38: Early exit feature for downstream acquire.
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/4   9/15/11 3:14p cbrooks
 * SW3128-1:Changed DFE_OVERLAP_LEAK to oxD
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/3   8/30/11 10:41p mpovich
 * SW3128-24: Move front-end 3128/3461 core drivers to dedicated AP VOB
 *  directory.  Add support for 3128 Rev. B0 chip.
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/SW3128-45/1   8/28/11 10:35p mpovich
 * SW3128-45: Build Chip Revision B0.
 * 
 * 105   8/24/11 12:22p farshidf
 * SW3461-18: merge to main
 * 
 * Fw_Integration_Devel/11   8/24/11 11:59a farshidf
 * SW3461-38: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/2   8/23/11 4:48p mpovich
 * SW3128-24: Merge latest of SW3128-24 to devel. branch.
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/SW3128-24/3   8/23/11 4:32p mpovich
 * SW3128-24: Rebase with 2.0 devel. branch.
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/1   8/17/11 5:57p farshidf
 * SW3461-1: merge the B0 changes
 * 
 * 103   8/12/11 3:12p farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/8   8/12/11 2:10p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/SW3128-24/2   8/17/11 10:12a mpovich
 * SW3128-24: Update from SW3128-32.
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/SW3128-32/3   8/16/11 7:34p mpovich
 * SW3128-32: Assign Scan Status params inside BADS_P_Acquire().
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/SW3128-32/2   8/16/11 6:31p cbrooks
 * sw3128-1:added new FFT
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/SW3128-24/1   8/16/11 6:37p mpovich
 * SW3128-24: Pick up SW3128-32, "Get Scan Status" HAB command
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/SW3128-32/1   8/16/11 5:34p cbrooks
 * sw3128-1:added scanstatus structure
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/21   7/29/11 11:20a cbrooks
 * sw3128-1:update the retry
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/20   7/28/11 4:37p cbrooks
 * sw3128-1:Fixed two pass fast/slow acquisition
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/19   7/28/11 12:55p cbrooks
 * sw3128-1:removed test feature from acquisition test
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/18   7/27/11 7:58p cbrooks
 * sw3128-1:Added Early Exit
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/17   7/27/11 12:35p cbrooks
 * sw3128-1:removed debug print message
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/16   7/26/11 2:59p farshidf
 * SW3128-1: merge to dev branch
 * 
 * 101   7/26/11 2:58p farshidf
 * SW3128-1: remove warning
 * 
 * 100   7/25/11 2:17p farshidf
 * SW3128-1: remove warning
 * 
 * 99   7/25/11 10:31a farshidf
 * SW3128-1: merge to main
 * 
 * 102   7/27/11 4:55p farshidf
 * SW3128-1: remove un-necessay print
 * 
 * 101   7/26/11 2:58p farshidf
 * SW3128-1: remove warning
 * 
 * 100   7/25/11 2:17p farshidf
 * SW3128-1: remove warning
 * 
 * 99   7/25/11 10:31a farshidf
 * SW3128-1: merge to main
 * 
 * Fw_Integration_Devel/7   7/25/11 10:16a farshidf
 * SW3128-1: fix Merge
 * 
 * Fw_Integration_Devel/6   7/25/11 10:13a farshidf
 * SW3128-1: fix merge
 * 
 * Fw_Integration_Devel/5   7/25/11 10:10a farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/15   7/22/11 4:51p cbrooks
 * sw3128-1:Added SLow Scan for RFI interference
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/14   7/19/11 12:59p farshidf
 * SW3128-1: remove extra include
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/13   7/18/11 6:37p cbrooks
 * sw3128-1:removed reack count from slow mode
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/12   7/18/11 6:09p cbrooks
 * sw3128-1:Added Baseand timing loop phase detector
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/11   7/18/11 3:43p farshidf
 * SW3128-21: update
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/10   7/18/11 3:38p farshidf
 * SW3128-21: update teh FFt IRQ
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/9   7/18/11 1:02p cbrooks
 * sw3128-1:fixed timing loop mistake
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/8   7/18/11 9:58a farshidf
 * SW3128-28: compile fix
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/7   7/15/11 6:42p farshidf
 * SWDTV-7869: fix
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/6   7/15/11 6:27p farshidf
 * SWDTV-7869: fix compile error
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/5   7/15/11 6:17p farshidf
 * SWDTV-7869: check-in the B0 code
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/4   7/15/11 5:40p cbrooks
 * sw3128-1:added slow acquire for RFI support
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/3   7/14/11 7:13p cbrooks
 * sw3128-1:added CWC for 3461
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/2   7/14/11 4:36p farshidf
 * SW3128-28: fix the FFT interrupt fro back end chip
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/1   6/30/11 6:02p cbrooks
 * sw3128-1:added retry capablility
 * 
 * Fw_Integration_Devel/1   6/29/11 12:37p farshidf
 * SW3461-13: merge to integration branch
 * 
 * Fw_Integration_Devel/Ads_Fw_Devel_Rc04/2   6/22/11 5:32p cbrooks
 * sw3128-1:fixed FEC reset problem
 * 
 * Fw_Integration_Devel/Ads_Fw_Devel_Rc04/1   6/21/11 5:17p cbrooks
 * sw3128-1:Added symbol rate to callback
 * 
 * 95   6/9/11 6:13p mpovich
 * SW3461-1: Merge Ver 0.4 Integ. onto main branch.
 * 
 * SW_System_4_Integ_Test/4   6/9/11 2:14p mpovich
 * SW3461-1: Rebase with main branch.
 * 
 * 94   6/9/11 9:57a farshidf
 * SW3128-1: add criticalsection for magnum compatibilty
 * 
 * 93   6/7/11 6:02p farshidf
 * SW3128-1: add 3123 support
 * 
 * 92   6/7/11 3:13p farshidf
 * SW3128-1: merge to main
 * 
 * SW_System_4_Integ_Test/3   6/7/11 1:49p farshidf
 * SW3128-1: sync up with backend
 * 
 * SW_System_4_Integ_Test/2   6/7/11 11:03a farshidf
 * SW3128-1: merge to integration branch
 * 
 * Ads_Fw_Devel_3/4   6/7/11 10:56a cbrooks
 * sw3128-1:Added 7552 IMC
 * 
 * Ads_Fw_Devel_3/3   6/1/11 5:55p farshidf
 * SW3128-1: merge from main
 * 
 * 91   6/1/11 5:51p farshidf
 * SW3128-1: Add double wrtite
 * 
 * 90   5/20/11 6:38a mpovich
 * SW3461-1: rename UFE (BUFE) module to TNR (BTNR).
 * 
 * TNR_3461_1/1   5/19/11 6:26p mpovich
 * SW3461-1: Change BUFE module name to BTNR
 * 
 * 89   5/18/11 3:57p farshidf
 * SW3128-1: remove the flag fro 3128/3461
 * 
 * 88   5/18/11 3:23p farshidf
 * SW3128-1: add 3128 DS fix from Charlie
 * 
 * 86   5/17/11 4:40p mpovich
 * SW3461-1: fix 3461 cannot acquire DS problem (added debug printfs if
 *  BKNI event cannot be created).
 * 
 * 85   5/13/11 2:10p farshidf
 * SW3128-1: fix from charlie
 * 
 * 84   5/11/11 4:59p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_3/20   5/11/11 4:22p cbrooks
 * sw3128-1:merging 330 changes
 * 
 * Hydra_Software_Devel/SWDTV-7035/2   5/10/11 12:27p farshidf
 * SWDTV-7035: update to latest
 * 
 * ADS_3128_3/18   5/10/11 12:17p farshidf
 * SW3128-1: update callback
 * 
 * ADS_3128_3/17   5/10/11 11:00a cbrooks
 * sw3128-1:Changed status
 * 
 * ADS_3128_3/16   5/7/11 3:53p cbrooks
 * sw3128-1:clean up
 * 
 * ADS_3128_3/15   5/7/11 3:47p cbrooks
 * sw3128-1:corrected freq offset
 * 
 * ADS_3128_3/14   5/5/11 8:46p cbrooks
 * sw3128-1:new code
 * 
 * ADS_3128_3/13   5/5/11 8:28p cbrooks
 * sw3128-1:Clean up
 * 
 * ADS_3128_3/12   5/5/11 8:14p cbrooks
 * sw3128-1:Cleanup Code
 * 
 * ADS_3128_3/11   5/3/11 10:29p cbrooks
 * sw3128-1:added delat in FFT
 * 
 * ADS_3128_3/10   5/3/11 7:42p cbrooks
 * sw3128-1:fft changes
 * 
 * ADS_3128_3/9   5/3/11 7:34p cbrooks
 * sw3128-1:fixed FFT
 * 
 * ADS_3128_3/8   5/3/11 4:30p cbrooks
 * sw3128-1:new code for FFT timeout
 * 
 * ADS_3128_3/7   5/1/11 3:49p cbrooks
 * sw3128-1:new code
 * 
 * ADS_3128_3/6   5/1/11 3:30p cbrooks
 * sw3128-1:Cleaned up Channel Scan Code
 * 
 * ADS_3128_3/4   4/28/11 8:07p cbrooks
 * sw3128-1:new code for FFT
 * 
 * ADS_3128_3/3   4/28/11 3:51p farshidf
 * SW3128-1: merge main
 * 
 * 81   4/28/11 8:50p farshidf
 * SW3128-1: remove IRQ
 * 
 * 79   4/28/11 4:36p farshidf
 * SW3128-1: sync up with 35530
 * 
 * 78   4/28/11 3:50p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_3/2   4/28/11 2:34p cbrooks
 * sw3128-1:fixed carrier range
 * 
 * ADS_3128_3/1   4/28/11 1:08p cbrooks
 * sw3128-1:New Code for scan
 * 
 * 74   4/26/11 7:32p farshidf
 * SW3128-1: compile fix for 3461
 * 
 * 73   4/26/11 6:57p farshidf
 * SW3128-1: make it host compatible
 * 
 * 72   4/26/11 6:48p farshidf
 * SW3128-1: error fixed
 * 
 * 71   4/26/11 6:47p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_2/7   4/26/11 5:50p farshidf
 * SW3128: compile fix
 * 
 * ADS_3128_2/6   4/26/11 4:59p farshidf
 * SW3128-1: update
 * 
 * ADS_3128_2/5   4/26/11 4:52p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_2/4   4/26/11 4:25p cbrooks
 * sw3128-1:New code for 330
 * 
 * ADS_3128_2/3   4/13/11 5:04p cbrooks
 * sw3128-1:new cwc code
 * 
 * ADS_3128_2/2   4/11/11 8:31p cbrooks
 * SW3128-1:Added CWC code
 * 
 * ADS_3128_2/1   4/11/11 12:46p cbrooks
 * sw3128-1:New CWC code
 * 
 * 56   3/24/11 4:15p farshidf
 * SW3128-1: add support for 3124
 * 
 * 55   3/18/11 4:29p farshidf
 * SW3461-1: merge  main
 * 
 * ADS_3128_1/2   3/17/11 12:29p cbrooks
 * sw3128-1:Added BERT resync warning
 * 
 * ADS_3128_1/1   3/16/11 5:21p cbrooks
 * sw3128-1:new bert
 * 
 * 54   3/7/11 11:19p farshidf
 * SW3128-1: remove the OOb power control
 * 
 * 53   3/4/11 12:34p farshidf
 * SW3128-1: add the power seq for DS
 * 
 * 52   3/4/11 12:16p farshidf
 * SW3128-1: add the power seq for DS
 * 
 * 51   3/4/11 12:00p farshidf
 * SW3128-1: fix the last channel issue
 * 
 * 50   3/4/11 11:55a farshidf
 * SW3128-1: fxi compile issue
 * 
 * 49   3/4/11 11:53a farshidf
 * SW3128-1: add the power seq for DS cores
 * 
 * 48   3/3/11 5:38p farshidf
 * SW3128-1: comment the power down code
 * 
 * 44   2/28/11 5:57p cbrooks
 * sw3128-1:Changed acqwords to internal_params
 * 
 * 43   2/23/11 7:39p cbrooks
 * sw3128-1:new code
 * 
 * 42   2/23/11 6:56p cbrooks
 * sw3128-1:new code
 * 
 * 41   2/15/11 7:20p cbrooks
 * SW3128-1:New Code
 * 
 * 40   2/9/11 10:50a cbrooks
 * SW3128-1:LIC EST CODE
 * 
 * 39   2/8/11 11:55a farshidf
 * SW3128-1: reduce the number of event per channel
 * 
 * 38   2/7/11 5:41p farshidf
 * SW3128-1: adapt to new rdb
 * 
 * 37   2/7/11 3:38p farshidf
 * SW3128-1: remove printf
 * 
 * 36   2/7/11 2:26p cbrooks
 * sw3128-1:new code
 * 
 * 35   2/4/11 4:26p farshidf
 * SW3128-1: remove the timer 7 set
 * 
 * 34   2/4/11 10:36a farshidf
 * SW3128-1: add printf
 * 
 * 33   2/3/11 8:02p mpovich
 * SW3128-1: Fix precedence in conditional expression to remove compiler
 *  warning.
 * 
 * Rom_Devel_3128/1   2/3/11 8:01p mpovich
 * SW3128-1: Fix precedence in conditional expression to remove compiler
 *  warning.
 * 
 * 32   2/3/11 6:51p farshidf
 * SW3128-1: update
 * 
 * 31   2/3/11 6:46p cbrooks
 * sw3128-1:new code
 * 
 * 30   1/31/11 9:11p cbrooks
 * sw3128-1:new test code
 * 
 * 29   1/31/11 7:51p cbrooks
 * sw3128-1:new code
 * 
 * 27   1/30/11 6:40p cbrooks
 * sw3128-1:new code
 * 
 * 26   1/30/11 6:39p cbrooks
 * sw3128-1:new code
 * 
 * 25   1/30/11 6:35p cbrooks
 * sw3128-1:FFT Code
 * 
 * 24   1/28/11 3:42p farshidf
 * SW3128-1: adapt the files to 3461
 * 
 * 23   1/26/11 4:12p farshidf
 * SW3128-1: clean up
 * 
 * 22   1/26/11 3:28p cbrooks
 * sw3128-1:Cleanup Code
 * 
 * 21   1/25/11 9:29p cbrooks
 * sw3128-1: Cleanup Code
 * 
 * 20   1/21/11 6:12p farshidf
 * SW3461-1: update names
 * 
 * 19   1/17/11 1:24p cbrooks
 * sw3128-1:New Code
 * 
 * 18   12/22/10 2:21p farshidf
 * SW3128-1: update
 * 
 * 17   12/18/10 10:40a farshidf
 * SW3128-1: update
 * 
 * 16   12/17/10 4:16p farshidf
 * SW3128-1: update
 * 
 * 15   12/16/10 6:16p farshidf
 * SW3128-1: clean up
 * 
 * 14   12/15/10 5:42p cbrooks
 * SW3128-1:New COde
 * 
 * 13   12/15/10 4:32p cbrooks
 * SW3128-1:new code
 * 
 * 12   12/14/10 4:22p cbrooks
 * SW3128-1: update
 * 
 * 11   12/14/10 3:18p cbrooks
 * SW3128-1:new code
 * 
 * 10   12/14/10 2:34p farshidf
 * SW3128-1: update
 * 
 * 9   12/14/10 2:11p cbrooks
 * SW3128-1:New Code
 * 
 * 8   12/7/10 4:15p cbrooks
 * SW3128-1:New Code
 * 
 * 7   12/3/10 7:12p farshidf
 * SW3128-1: update files
 * 
 * 6   12/3/10 3:52p farshidf
 * SW3128-1: clean up
 * 
 * 5   12/1/10 3:21p cbrooks
 * SW3128-1: ADS update
 * 
 * 4   11/30/10 11:19a farshidf
 * SW3461-1: add BMTH_2560log10 to math lib
 * 
 * 3   11/24/10 9:36p cbrooks
 * SW3128-1: Third Try
 * 
 * 2   11/22/10 6:08p farshidf
 * SW3128-1: remove old code
 * 
 * 1   11/12/10 5:45p farshidf
 * SW3128-1: add ADS code
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bmth.h"
#include "bkni.h"
#ifdef LEAP_BASED_CODE 
#include "btmr.h"
#include "bads_def.h"
#include "bads_irq.h"
#include "bchp_leap_ctrl.h"
#if (BCHP_FAMILY == 7584)
#include "bchp_clkgen.h"
#include "bwfe_global_clk.h"
#else
#include "bchp_tm.h" 
#endif
#include "bads_api.h"
#if (BCHP_FAMILY==3128)
#include "bwfe_global_clk.h"
#include "bchp_ds_b_topm.h"
#include "bchp_ds_b_tops.h"
#include "bchp_ds_wfe_cz_0.h"
#define WFE 1
#define IMC 0
#define HRC 0
#endif
#if (BCHP_FAMILY==3461) || (BCHP_FAMILY==3462) 
#include "btnr_global_clk.h"
#define WFE 0
#define IMC 1
#define HRC 0   /*disabled and not used*/
#endif
#else   /* host chip based code */
#if (BCHP_CHIP==35233)
#include "bads_global_clk.h"
#define WFE 0
#define IMC 0
#define HRC 0 
#endif
#if (BCHP_CHIP==7552)
#include "bads_global_clk.h"
#define WFE 0
#define IMC 0
#define HRC 0 
#endif
#include "bads.h"
#include "bads_mth.h"
#include "bads_3x7x_priv.h"
#endif  /* host chip based code */

#include "bads_acquire.h"
#include "bads_utils.h"
#include "bads_priv.h"
#include "bads_coef.h"
#include "bmth.h"
#include "bchp_ds_topm.h"


/*****************************************trick to get emulation to run*/
#include "bchp_ds_tops.h"

/*registers needed for the functions in this file*/
#include "bchp_ds.h" 

#ifndef LEAP_BASED_CODE
BDBG_MODULE(bads_acquire);
#endif


/*******************************************************************************************
 *BADS_P_Initialize()		This routine initializes the ADS and is only run once
 *******************************************************************************************/
BERR_Code BADS_P_Initialize(BADS_3x7x_ChannelHandle hChn)
{
	BERR_Code RetCode_u32 = BERR_SUCCESS;

	BDBG_MSG(("BADS_P_Initialize "));
	/********************************************************************************
	*The BADS_Acquire_Params_t and BADS_Scan_Params_t structures are set by the PI
	*The code will not work properly if they are not initialized by the PI   
	********************************************************************************/
	
	/*Initialize the BADS_Internal_Params_t Structure
	 *these parameters are used locally to send parameters into the ADS functions*/
	hChn->pChnAcqParam->BADS_Internal_Params.BigEQ_te                   = INIT_BIG_EQ;
	hChn->pChnAcqParam->BADS_Internal_Params.CWC_te                     = INIT_CWC;
	hChn->pChnAcqParam->BADS_Internal_Params.CFL_te                     = INIT_CFL;
	hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te                   = INIT_DDAGC;
	hChn->pChnAcqParam->BADS_Internal_Params.IMC_te                     = INIT_IMC;
	hChn->pChnAcqParam->BADS_Internal_Params.Acquisition_Test_te        = INIT_ACQUISITION_TEST;
	hChn->pChnAcqParam->BADS_Internal_Params.Video_Carrier_Annex_B_te   = INIT_VIDEO_CANCELLATION_ANNEX_B;
	hChn->pChnAcqParam->BADS_Internal_Params.Dual_Scan_te               = INIT_DUAL_SCAN;
	hChn->pChnAcqParam->BADS_Internal_Params.FOI_Timing_te              = INIT_FOI_TIMING;
	hChn->pChnAcqParam->BADS_Internal_Params.Callback_Enable_te         = INIT_CALLBACK_ENABLE;
	hChn->pChnAcqParam->BADS_Internal_Params.Timing_Scan_Threshold_u32  = INIT_TIMING_SCAN_THRESHOLD;
	hChn->pChnAcqParam->BADS_Internal_Params.Carrier_Scan_Threshold_u32 = INIT_CARRIER_SCAN_THRESHOLD;
                      
	/*Initialize the BADS_Local_Params_t structure*/
	hChn->pChnAcqParam->BADS_Local_Params.AcqType_te            = BADS_Local_Params_AcqType_eFastAcquire;
	hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te          = BADS_Local_Params_AcqStatus_eNoPower;
	hChn->pChnAcqParam->BADS_Local_Params.Carrier_Search_u32    = 150000;
	hChn->pChnAcqParam->BADS_Local_Params.Upper_Baud_Search_u32 = Q256_Q1024_ANNEXB_SYMBOL_RATE;
	hChn->pChnAcqParam->BADS_Local_Params.Lower_Baud_Search_u32 = Q256_Q1024_ANNEXB_SYMBOL_RATE;
	hChn->pChnAcqParam->BADS_Local_Params.Q1024A_b              = false;
	hChn->pChnAcqParam->BADS_Local_Params.Q512A_b               = false;
	hChn->pChnAcqParam->BADS_Local_Params.Q256A_b               = false;
	hChn->pChnAcqParam->BADS_Local_Params.Q256B_b               = true;
	hChn->pChnAcqParam->BADS_Local_Params.Q64A_b                = false;	
	hChn->pChnAcqParam->BADS_Local_Params.Q64B_b                = false;	
	hChn->pChnAcqParam->BADS_Local_Params.Q16A_b                = false;	
	hChn->pChnAcqParam->BADS_Local_Params.Q128A_b               = false;	
	hChn->pChnAcqParam->BADS_Local_Params.Q32A_b                = false;	
	hChn->pChnAcqParam->BADS_Local_Params.Q1024B_b              = false;	
	hChn->pChnAcqParam->BADS_Local_Params.IS_b                  = false;	
	hChn->pChnAcqParam->BADS_Local_Params.Flip_Spectrum_b       = false;	

	hChn->pChnAcqParam->BADS_Local_Params.DoneFirstTimeFlag_te  = BADS_Local_Params_eEnable;
	hChn->pChnAcqParam->BADS_Local_Params.Annex_te				= BADS_Local_Params_Annex_eAnnexB;
	hChn->pChnAcqParam->BADS_Local_Params.QAM_te				= BADS_Local_Params_QAM_eQam64;
	hChn->pChnAcqParam->BADS_Local_Params.Old_CBERC1_u32		= 0;
	hChn->pChnAcqParam->BADS_Local_Params.Old_UERC1_u32			= 0;
	hChn->pChnAcqParam->BADS_Local_Params.Old_NBERC1_u32		= 0;
	hChn->pChnAcqParam->BADS_Local_Params.BadBlockCount_u32		= 0;
	hChn->pChnAcqParam->BADS_Local_Params.StuckFECCount_u32		= 0;
	hChn->pChnAcqParam->BADS_Local_Params.TestLockFlag_te		= BADS_Local_Params_eDisable;
	hChn->pChnAcqParam->BADS_Local_Params.FECSpectrum_te		= BADS_Local_Params_FECSpectrum_eNotInverted;
	hChn->pChnAcqParam->BADS_Local_Params.ElapsedTime_u32       = 0;
	hChn->pChnAcqParam->BADS_Local_Params.TotalTime_u32         = 0;
	hChn->pChnAcqParam->BADS_Local_Params.SampleRate_u32		= F_HS;    /*This is defined in the global_clk.h file from the WFE or UFE*/
	hChn->pChnAcqParam->BADS_Local_Params.VIDRate_u32           = 2*F_HS;  /*This is ALWAYS 2*F_HS*/
	/*The following were added for the api.c and will be initialized by the PI
	hChn->pChnAcqParam->BADS_Local_Params.lock_status 
	hChn->pChnAcqParam->BADS_Local_Params.Acquire_done 
	hChn->pChnAcqParam->BADS_Local_Params.EarlyExit
	hChn->pChnAcqParam->BADS_Local_Params.LockUpdate*/
  
	/*This is to make sure the acquisition test is disabled at initialization*/
	BREG_Write32(hChn->hRegister, BCHP_DS_SPARE, 0);

	return RetCode_u32;
}
 
/*******************************************************************************************
 * BADS_P_ChnLockStatus()		This routine gets the lock status of the ADS
 *******************************************************************************************/
BERR_Code BADS_P_ChnLockStatus(BADS_3x7x_ChannelHandle hChn)
{
	BERR_Code RetCode_u32 = BERR_SUCCESS;
	uint32_t ReadReg_u32;
	uint32_t HighThresh_u32, LowThresh_u32, SNR_u32;
	uint32_t ReSyncFlag_u32;
	uint32_t BMPG1_u32, CBERC1_u32, UERC1_u32, NBERC1_u32; 
	BADS_3x7x_ChnLockStatus_UnlockLock_t FECStatus_te;
	BADS_3x7x_ChnLockStatus_UnlockLock_t CurrentFECStatus_te;

	/***************************/
	/*Determine QAM Lock Status*/
	/***************************/
	HighThresh_u32 = BREG_ReadField(hChn->hRegister, DS_EQ_SNRHT, SNRHTHRESH);
	LowThresh_u32 =  BREG_ReadField(hChn->hRegister, DS_EQ_SNRLT, SNRLTHRESH);
	SNR_u32 =  BREG_ReadField(hChn->hRegister, DS_EQ_SNR, ERRVAL);

	if ((SNR_u32 < HighThresh_u32) && (SNR_u32 > LowThresh_u32)) 
   {
		hChn->pChnLockStatus->QLK_te = BADS_3x7x_ChnLockStatus_eLock;
   }
   else
   {
		hChn->pChnLockStatus->QLK_te = BADS_3x7x_ChnLockStatus_eUnlock;
   }
	
	/***************************************/
	/*Determine SNR                        */
	/*BBS will divide by 256 to get dB     */
	/***************************************/
	/*SNR in dB is different for each QAM Mode*/
    /*16   QAM  125.4405 dB - 20*log(ERRVAL) or if scaled by 256: 32113-5120*log(ERRVAL)*/
    /*32   QAM  122.4302 dB - 20*log(ERRVAL) or if scaled by 256: 31342-5120*log(ERRVAL)*/
	/*64   QAM  125.6524 dB - 20*log(ERRVAL) or if scaled by 256: 32167-5120*log(ERRVAL)*/
	/*128  QAM  122.5374 dB - 20*log(ERRVAL) or if scaled by 256: 31370-5120*log(ERRVAL)*/    
	/*256  QAM  125.7038 dB - 20*log(ERRVAL) or if scaled by 256: 32180-5120*log(ERRVAL)*/
	/*512  QAM  122.5638 dB - 20*log(ERRVAL) or if scaled by 256: 31376-5120*log(ERRVAL)*/    
	/*1024 QAM  125.7138 dB - 20*log(ERRVAL) or if scaled by 256: 32183-5120*log(ERRVAL)*/
	
	/*Get the QAM mode from the FEC*/
	ReadReg_u32 = BREG_Read32(hChn->hRegister, BCHP_DS_FECL);
	ReadReg_u32 = ((ReadReg_u32>>4) & 0x0000000F);

	switch (ReadReg_u32)
	{
		case 3: hChn->pChnLockStatus->SNR_u32 = 32113 - (BMTH_2560log10(SNR_u32)<<1); break;		/*16 QAM Mode*/
		case 4: hChn->pChnLockStatus->SNR_u32 = 31342 - (BMTH_2560log10(SNR_u32)<<1); break;		/*32 QAM Mode*/
		case 5: hChn->pChnLockStatus->SNR_u32 = 32167 - (BMTH_2560log10(SNR_u32)<<1); break;		/*64 QAM Mode*/
		case 6: hChn->pChnLockStatus->SNR_u32 = 31370 - (BMTH_2560log10(SNR_u32)<<1); break;		/*128 QAM Mode*/
		case 7:	hChn->pChnLockStatus->SNR_u32 = 32180 - (BMTH_2560log10(SNR_u32)<<1); break;		/*256 QAM Mode*/
		case 8:	hChn->pChnLockStatus->SNR_u32 = 31376 - (BMTH_2560log10(SNR_u32)<<1); break;		/*512 QAM Mode*/
		case 9:	hChn->pChnLockStatus->SNR_u32 = 32183 - (BMTH_2560log10(SNR_u32)<<1); break;		/*1024 QAM Mode*/
		default: BDBG_MSG(("WARNING!!! UNSUPPORTED OR UNDEFINED QAM_MODE")); break;
	}

	/*SNR AVERAGE*/
	/*BBS will divide by 256 to get dB*/
	hChn->pChnLockStatus->SNRAVG_u32 = ((hChn->pChnLockStatus->SNR_u32*(SNR_LEAKY_AVG)) + (16384-(SNR_LEAKY_AVG))*hChn->pChnLockStatus->SNRAVG_u32)/16384;
	

	/***************************/
	/*Determine FEC Lock Status*/
	/***************************/

	/*clear the MPEG bad block resync flag */
	ReSyncFlag_u32 = 0;

	/*Get the currect lock condition*/
	CurrentFECStatus_te = hChn->pChnLockStatus->FLK_te;

	/*Get the latest block error counter values*/	
	BMPG1_u32  =  BREG_ReadField(hChn->hRegister, DS_BMPG1, BMPGCNTVAL);
	CBERC1_u32 =  BREG_ReadField(hChn->hRegister, DS_CBERC1, CBERCCNTVAL);
	UERC1_u32  =  BREG_ReadField(hChn->hRegister, DS_UERC1, UERCCNTVAL);
	NBERC1_u32 =  BREG_ReadField(hChn->hRegister, DS_NBERC1, NBERCCNTVAL);


	/*Check for at least one good block since the last call*/
	if ((NBERC1_u32 - hChn->pChnAcqParam->BADS_Local_Params.Old_NBERC1_u32) >= NUM_CLEAN_BLOCKS_TO_LOCK)
	{
		/*If Annex B then check for MPEG Checksum False Sync*/
		/*This is and implementation from Jorge's BCM3250/7015/7100 App note*/
		/*Document 3250/7015/7100-An01-R*/
		if ((BMPG1_u32 > 10) && (hChn->pChnAcqParam->BADS_Local_Params.Annex_te == BADS_Local_Params_Annex_eAnnexB))
		{
			if ((10*UERC1_u32 < 4*BMPG1_u32) || (3*UERC1_u32 > 5*BMPG1_u32))
			{
				/*resync FEC, false MPEG Lock detected*/
				ReSyncFlag_u32 = 1;
				BREG_WriteField(hChn->hRegister, DS_RST, FECRST, 1);
				BREG_WriteField(hChn->hRegister, DS_RST, FECRST, 0);
			}
		}
		/*Reset Bad Block Counter*/
		hChn->pChnAcqParam->BADS_Local_Params.BadBlockCount_u32 = 0;
		
		/*Declare Lock*/
		FECStatus_te = BADS_3x7x_ChnLockStatus_eLock;
	}
	else
	{
		/*Wait for NUM_BAD_BLOCK_TO_UNLOCK bad blocks without a good one to declare unlock*/
		if((UERC1_u32 - hChn->pChnAcqParam->BADS_Local_Params.Old_UERC1_u32 + hChn->pChnAcqParam->BADS_Local_Params.BadBlockCount_u32) > NUM_BAD_BLOCK_TO_UNLOCK)	
		{
	
			/*Reset Bad Block Counter*/
			hChn->pChnAcqParam->BADS_Local_Params.BadBlockCount_u32 = 0;
			
			/*Declare UnLock*/
			FECStatus_te = BADS_3x7x_ChnLockStatus_eUnlock;
		}
		else
		{
			if (CurrentFECStatus_te == BADS_3x7x_ChnLockStatus_eUnlock) 
			{
				/*Declare UnLock*/
				FECStatus_te = BADS_3x7x_ChnLockStatus_eUnlock;
			}
			else
			{
				/*NUM_BAD_BLOCK_TO_UNLOCK bad blocks did not come along without a good one so wait until next call*/
				/*increment bad block counter*/
				hChn->pChnAcqParam->BADS_Local_Params.BadBlockCount_u32 += (UERC1_u32 - hChn->pChnAcqParam->BADS_Local_Params.Old_UERC1_u32);
			
				/*Declare Lock*/
				FECStatus_te = BADS_3x7x_ChnLockStatus_eLock;
			}
		}
	}
	 
	/*Check for stuck FEC condition, all counters did not move since the last call to BADS_75xx_P_Get_LockStatus()*/
	if ((CBERC1_u32 - hChn->pChnAcqParam->BADS_Local_Params.Old_CBERC1_u32 == 0) && (UERC1_u32 - hChn->pChnAcqParam->BADS_Local_Params.Old_UERC1_u32 == 0) && 
			(NBERC1_u32 - hChn->pChnAcqParam->BADS_Local_Params.Old_NBERC1_u32 == 0))
	{
		if ((hChn->pChnAcqParam->BADS_Local_Params.StuckFECCount_u32 >= STUCK_FEC_RESET_COUNT) || (CurrentFECStatus_te == BADS_3x7x_ChnLockStatus_eUnlock))
		{
		  /*Reset Counters*/
			hChn->pChnAcqParam->BADS_Local_Params.StuckFECCount_u32 = 0;
			hChn->pChnAcqParam->BADS_Local_Params.BadBlockCount_u32 = 0;

			/*Declare UnLock*/
 			FECStatus_te = BADS_3x7x_ChnLockStatus_eUnlock;
		}	
		else
		{
			hChn->pChnAcqParam->BADS_Local_Params.StuckFECCount_u32++;
		}
	}
 
 
	/*store counter values for the next call, BMPG1_u32 is not needed from call to call*/
	hChn->pChnAcqParam->BADS_Local_Params.Old_CBERC1_u32 = CBERC1_u32;
	hChn->pChnAcqParam->BADS_Local_Params.Old_UERC1_u32  = UERC1_u32;
	hChn->pChnAcqParam->BADS_Local_Params.Old_NBERC1_u32 = NBERC1_u32;

	/*clear FEC counters to prevent overflow*/
	/*Resync The FEC for bad MPEG packet count*/
	/*Reset FEC Counters*/
	if ((BMPG1_u32 > POWER2_31_M1) || (CBERC1_u32 > POWER2_31_M1) || (UERC1_u32 > POWER2_31_M1) || (NBERC1_u32 > POWER2_31_M1) || (ReSyncFlag_u32 == 1)) 
	{	
		if (ReSyncFlag_u32 == 0) 
		{
			BDBG_MSG(("                                                  Reseting FEC Counters"));
		}
		else
		{
			BDBG_MSG(("                                                  RESYNCING"));
		}
		ReSyncFlag_u32 = 0;
		BREG_Write32(hChn->hRegister, BCHP_DS_TPFEC, 0x000F8000);
		hChn->pChnAcqParam->BADS_Local_Params.Old_UERC1_u32 = 0;
		hChn->pChnAcqParam->BADS_Local_Params.Old_NBERC1_u32 = 0;
		hChn->pChnAcqParam->BADS_Local_Params.Old_CBERC1_u32 = 0;
	}

	/*This is used by the BADS_P_AcquisitionPercentageTest() to force the lock detector to return unlocked*/
	FECStatus_te = (hChn->pChnAcqParam->BADS_Local_Params.TestLockFlag_te == BADS_Local_Params_eEnable) ? BADS_3x7x_ChnLockStatus_eUnlock : FECStatus_te;

	/*assign values to the status structure to be read by BBS, API, AND used by the PI*/
	hChn->pChnLockStatus->FLK_te = FECStatus_te;

    /* assign "No signal" status (for BBS, API, PI) */
    if ( (hChn->pChnLockStatus->FLK_te != BADS_3x7x_ChnLockStatus_eLock) && (hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te != BADS_Local_Params_AcqStatus_eEarlyExit) &&
         ((hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te == BADS_Local_Params_AcqStatus_eNoPower) ||
          (hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te == BADS_Local_Params_AcqStatus_eNoTiming)) )
    {
        hChn->pChnLockStatus->NoSignal_b = true;
    }
    else
    {
        hChn->pChnLockStatus->NoSignal_b = false;
    }

	/*Reset the ReAcquire_Count after the PI resets the ReAck_Count*/
	hChn->pChnLockStatus->ReAcquire_Count_u32 = (hChn->pChnLockStatus->ReAck_Count_u32 == 0) ?  0 : hChn->pChnLockStatus->ReAcquire_Count_u32;

	/*LockStatus Complete*/
  return RetCode_u32;
}

/***************************************************************************
 * BADS_P_Acquire()
 ***************************************************************************/
BERR_Code BADS_P_Acquire(BADS_3x7x_ChannelHandle hChn)
{
	BERR_Code RetCode_u32 = BERR_SUCCESS;
	BADS_P_AdsCallbackData_t CallbackFrontend;
	bool EarlyExit_b=false;

	uint8_t  Index_u8;
	uint8_t	 RetryIndex_u8=0, RetryIndexEnd_u8;
	uint8_t  BaudIndex_u8, BaudIndexStart_u8, BaudIndexEnd_u8;
	uint8_t	 CarrierIndex_u8, CarrierIndexEnd_u8;
	uint32_t FFT_TimingFreq_u32=0, FFT_TimingBin_u32=0;
	uint8_t  EQIndex_u8, EQIndexEnd_u8;
	uint8_t  QamIndex_u8=0;
	bool     QamTry_b;
	uint8_t  FECIndex_u8=0, FECIndexEnd_u8;
	bool     FECTry_b;

	uint32_t CarrierStepSize_u32;
	int32_t  CarrierOffset_i32=0;
	int32_t  VID_Error_i32;
	uint32_t Symbol_Rate_u32=0;
	int32_t  Carrier_Error_i32=0, Phase_Error_i32;
	uint8_t CWC_Length_u8;
	uint32_t EQ_u32[144];
	uint32_t FFE_Scale_u32;
	int32_t  FFE_I_i32=0, FFE_Q_i32=0;
	uint32_t FFE_Scaled_Value_u32;
	uint32_t ReadReg_u32;
	uint16_t FEC_TimeOut_u16;
	
	uint32_t StartTime_u32, EndTime_u32, ElapsedTime_u32;
	

	/****************************************************************************************************
	* 1: ADS Core initialization: Reset timers, flags, and clocks
	*****************************************************************************************************/
	BDBG_MSG(("BADS_P_Acquire "));
	/*Get start time*/
	BTMR_ReadTimer(hChn->hAds->hStatusTimer, &StartTime_u32);
	
	/*Get the clock sample rate from frontend*/
	hChn->pChnAcqParam->BADS_Local_Params.SampleRate_u32		= F_HS;    /*This is defined in the global_clk.h file from the WFE or UFE*/
	hChn->pChnAcqParam->BADS_Local_Params.VIDRate_u32           = 2*F_HS;  /*This is ALWAYS 2*F_HS*/

	/*Reset the Early Exit flag when acquire is just starting */
	hChn->pChnAcqParam->BADS_Local_Params.EarlyExit = false;

	/*Initialize structures, Range Check and map Acquisition/Scan setting*/
	/*Check if this is the first time to call acquire
	 *The BADS_P_Initialize() will set the DoneFirstTimeFlag to BADS_Local_Params_eEnable*/
	if (hChn->pChnAcqParam->BADS_Local_Params.DoneFirstTimeFlag_te == BADS_Local_Params_eDisable) 
	{
		BADS_P_Initialize(hChn);
	}
	/*Range Check the Inputs
	 *Map hChn->pChnAcqParam->BADS_Acquire_Params.Qam_Mode to hChn->pChnAcqParam->BADS_Local_Params.QAM_te
	 *Map hChn->pChnAcqParam->BADS_Acquire_Params.Annex to hChn->pChnAcqParam->BADS_Local_Params.Annex_te
	 *This is done because the scan may change the Qam mode and Annex while searching*/
	RetCode_u32 = BADS_P_Range_Check(hChn);
	if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/

	/*Set RetryIndexEnd_u8 for each acquisition type when Auto Acquire is on*/
	/*Set RetryIndexEnd_u8 to 0 when Auto Acquire is off*/
	RetryIndexEnd_u8 = 0;
	if (hChn->pChnAcqParam->BADS_Acquire_Params.Auto_te == BADS_Acquire_Params_eEnable)
	{
		switch (hChn->pChnAcqParam->BADS_Acquire_Params.AcqType_te)
		{
		case BADS_Acquire_Params_AcqType_eAuto : RetryIndexEnd_u8 = NUM_RETRIES_IF_AUTOACQUIRE_AND_AUTO; break; 
		case BADS_Acquire_Params_AcqType_eFast : RetryIndexEnd_u8 = NUM_RETRIES_IF_AUTOACQUIRE_AND_FAST; break; 
		case BADS_Acquire_Params_AcqType_eSlow : RetryIndexEnd_u8 = NUM_RETRIES_IF_AUTOACQUIRE_AND_SLOW; break; 
		case BADS_Acquire_Params_AcqType_eScan : RetryIndexEnd_u8 = NUM_RETRIES_IF_AUTOACQUIRE_AND_SCAN; break;
		default: BDBG_ERR(("ERROR!!! Undefined AcqType in BADS_P_Acquire() %d",hChn->pChnAcqParam->BADS_Acquire_Params.AcqType_te)); 
				 RetCode_u32 = BERR_INVALID_PARAMETER;
				 goto something_bad_happened; /*goto bottom of function to leave early with error*/
				 break;
		}
	}

	/*This loop will retry the complete acquisition the number of times set by RetryIndexEnd_u8*/ 
	for (RetryIndex_u8=0;RetryIndex_u8<=RetryIndexEnd_u8;RetryIndex_u8++)
	{
	if (PRINT_DEBUG==1) BDBG_ERR(("RetryIndex_u8 = %d RetryIndexEnd_u8 = %d",RetryIndex_u8, RetryIndexEnd_u8));
	
		
		/************************************************************************************************************
		* 2: Reset time counters, flags, and FEC counters
		*************************************************************************************************************/
		
		/*Reset total time counter when ReAck_Count = 0*/
		if (hChn->pChnLockStatus->ReAck_Count_u32 == 0)
		{
			hChn->pChnAcqParam->BADS_Local_Params.TotalTime_u32 = 0;
		}
	
		/*Begin by being unlocked, increment ReAck counter, clear test lock flag used by BADS_P_AcquisitionPercentageTest()*/
		hChn->pChnLockStatus->FLK_te = BADS_3x7x_ChnLockStatus_eUnlock;
		hChn->pChnLockStatus->QLK_te = BADS_3x7x_ChnLockStatus_eUnlock;
		hChn->pChnLockStatus->ReAck_Count_u32++;     /*This is reset to 0 by the PI*/
		hChn->pChnLockStatus->ReAcquire_Count_u32++; /*This is reset to 0 on the first call to BADS_P_ChnLockStatus() after the PI resets the Reack_Count to 0*/
		hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te = BADS_Local_Params_AcqStatus_eNoPower;
		hChn->pChnAcqParam->BADS_Local_Params.TestLockFlag_te = BADS_Local_Params_eDisable;
	
		/*Reset FEC Counters and hold FEC in reset*/
		BREG_WriteField(hChn->hRegister, DS_RST, FECRST, 1);		
		BREG_Write32(hChn->hRegister, BCHP_DS_TPFEC, 0x000F9F00);
		hChn->pChnAcqParam->BADS_Local_Params.Old_CBERC1_u32 = 0;
		hChn->pChnAcqParam->BADS_Local_Params.Old_UERC1_u32 = 0;
		hChn->pChnAcqParam->BADS_Local_Params.Old_NBERC1_u32 = 0;
		hChn->pChnAcqParam->BADS_Local_Params.StuckFECCount_u32 = STUCK_FEC_RESET_COUNT;
	
		/*************************************************************************************************************
		 *BEGIN INITIALIZATION
		 *************************************************************************************************************/
		
		/*Determine Acquisition Type and Parameters*/
		BADS_P_Get_AcquisitionScan_Settings(hChn);	
		
		/*Reset ADS core: start with loops frozen: Initailize and reset IMC*/ 
#if WFE
		BREG_Write32(hChn->hRegister, BCHP_DS_RST, 0x8006B679);	
		BREG_Write32(hChn->hRegister, BCHP_DS_FRZ, 0x027FE78A);
#else
	#if IMC
		BREG_Write32(hChn->hRegister, BCHP_DS_RST, 0x8017B679);
		BREG_Write32(hChn->hRegister, BCHP_DS_FRZ, 0x0B7FE78A);
		BREG_WriteField(hChn->hRegister, DS_EQ_CTL, IMC_EN, 0);
		BREG_WriteField(hChn->hRegister, DS_EQ_IMC, RESET, 1);
	#else
		BREG_Write32(hChn->hRegister, BCHP_DS_RST, 0x8006B679);
		BREG_Write32(hChn->hRegister, BCHP_DS_FRZ, 0x027FE78A);
	#endif
#endif

		/*Setup output clock, clear bus errors*/
		BREG_Write32(hChn->hRegister, BCHP_DS_CLK, 0x00000400);					/*Enable dedicated parallel/serial output clock*/ 
		BREG_WriteField(hChn->hRegister, DS_ICB_CTL, BUS_ERR_EN, 0);			/*Internal Configuration Bus Control and Status and Stops bus errors*/


		/*Setup AGCB and AGCB_IR*/
		BREG_Write32(hChn->hRegister, BCHP_DS_AGCB, 0x0a001201);				/*TH=0.078125, BW=2^-9,  Reset AGCB*/
		BREG_Write32(hChn->hRegister, BCHP_DS_AGCBI, 0x05000000);	            /*preload integrator with the tracking value at lock*/				 
#if IMC
		BREG_Write32(hChn->hRegister, BCHP_DS_AGCB_IR, 0x0a001201);									 
		BREG_Write32(hChn->hRegister, BCHP_DS_AGCBI_IR , 0x05000000);								 
#endif
	 	
		/*************************************************************************************************************/
		/*INITIALIZATION FINISHED: BEGIN ACQUISITION*/
		/*************************************************************************************************************/

		/*Release AGCB/AGCB_IR loops*/
		BREG_WriteField(hChn->hRegister, DS_FRZ,	AGCBFRZ, 0);				 
#if IMC
		BREG_WriteField(hChn->hRegister, DS_FRZ,	AGCBFRZ_IR, 0);				
#endif
	
		/*wait for AGCB/AGCB_IR to settle*/
		EarlyExit_b = BADS_P_ADS_SLEEP(hChn, AGCB_CONVERGENCE_TIME_MS);
		if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
	
		/*Reduce AGCB/AGCB_IR bandwidth to tracking values*/
		BREG_WriteField(hChn->hRegister, DS_AGCB, AGCBBW, 0xf);				 /*Reduce AGCB BW 2^-12*/
#if IMC
		BREG_WriteField(hChn->hRegister, DS_AGCB_IR, AGCBBW, 0xf);			 /*Reduce AGCB_IR BW 2^-12*/
#endif

		/*3: Setup the BaudIndexStart_u8 and BaudIndexEnd_u8 to search for the baud rate*/
		Index_u8 = 0;
		BaudIndexStart_u8 = 0;
		BaudIndexEnd_u8 = 0;
		while (Index_u8 < NUM_BAUD_RATES)
		{
			if (hChn->pChnAcqParam->BADS_Local_Params.Upper_Baud_Search_u32 <= BaudRates_TBL_u32[Index_u8])
			{
				BaudIndexStart_u8 = Index_u8;
			}
			if (hChn->pChnAcqParam->BADS_Local_Params.Lower_Baud_Search_u32 <= BaudRates_TBL_u32[Index_u8])
			{
				BaudIndexEnd_u8 = Index_u8;
			}
		Index_u8++;
		}

		/*Loop each Baud Rate*/
		for (BaudIndex_u8=BaudIndexStart_u8;BaudIndex_u8<=BaudIndexEnd_u8;BaudIndex_u8++)
		{
			if (PRINT_DEBUG==1) BDBG_ERR(("BaudIndex_u8 = %d BaudIndexStart_u8 = %d BaudIndexEnd_u8 = %d",BaudIndex_u8,BaudIndexStart_u8, BaudIndexEnd_u8));

			/*4: Setup the CarrierIndexEnd_u8 to find the Baud Rate*/
			/*The search will take the +- the carrier search range and divide it into the number of PreFilter/Nyquist filter regions to get the number of steps needed*/
			/*Number of steps will always be odd with first step centered at 0 carrier offset with a telescoping search away from 0*/
#if (ADS_INTERNAL_ERROR_CHECKING > 0)
			/*Check that divisor will not be 0*/
			if ((PRE_NYQUIST_FILTER_BW_1MHZ == 0) || (BaudRates_TBL_u32[BaudIndex_u8] == 0))
			{
				BDBG_ERR(("ERROR1 in BADS_P_Acquire()"));
				RetCode_u32 = BERR_INVALID_PARAMETER;
				goto something_bad_happened; /*goto bottom of function to leave early with error*/
			}
#endif
#if (ADS_INTERNAL_ERROR_CHECKING > 1)
			/*Check that divisor will not be 0*/
			if ((PRE_NYQUIST_FILTER_BW_1MHZ < 8) || (BaudRates_TBL_u32[BaudIndex_u8] < 1000000))
			{
				BDBG_ERR(("ERROR2 in BADS_P_Acquire()"));
				RetCode_u32 = BERR_INVALID_PARAMETER;
				goto something_bad_happened; /*goto bottom of function to leave early with error*/
			}
#endif
			CarrierIndexEnd_u8 = (uint8_t)(2*hChn->pChnAcqParam->BADS_Local_Params.Carrier_Search_u32*1000/(PRE_NYQUIST_FILTER_BW_1MHZ*BaudRates_TBL_u32[BaudIndex_u8]));
			CarrierIndexEnd_u8 = (CarrierIndexEnd_u8%2) ? CarrierIndexEnd_u8 + 1 : CarrierIndexEnd_u8;
			/*divide by 0 is not possible due to (CarrierIndexEnd_u8 == 0) checking*/
			CarrierStepSize_u32 = (CarrierIndexEnd_u8 == 0) ? 0 : (hChn->pChnAcqParam->BADS_Local_Params.Carrier_Search_u32 - (PRE_NYQUIST_FILTER_BW_1MHZ*BaudRates_TBL_u32[BaudIndex_u8]/2000))/((uint32_t)CarrierIndexEnd_u8/2);

			/*Loop each Carrier Offset*/
			for (CarrierIndex_u8=0;CarrierIndex_u8<=CarrierIndexEnd_u8;CarrierIndex_u8++)
			{
				CarrierOffset_i32 = (CarrierIndex_u8%2 == 0) ? (int32_t)CarrierIndex_u8/2 : (-1-(int32_t)CarrierIndex_u8)/2;
				CarrierOffset_i32 = CarrierOffset_i32*(int32_t)CarrierStepSize_u32;
				if (PRINT_DEBUG==1) BDBG_ERR(("CarrierIndex_u8 = %d CarrierOffset_i32 = %d CarrierIndexEnd_u8 = %d",CarrierIndex_u8, CarrierIndexEnd_u8,CarrierOffset_i32));

				/*Reset and program CFL*/
				BREG_WriteField(hChn->hRegister, DS_FRZ,	CFLFRZ, 1);	
				BREG_Write32(hChn->hRegister, BCHP_DS_CFLI, 0);
				BREG_WriteField(hChn->hRegister, DS_CFL, CFLRST, 0x00000001);	
				RetCode_u32 = BADS_P_Set_CFL_Frequency(hChn, hChn->pChnAcqParam->BADS_Local_Params.SampleRate_u32, 0);
				if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
	
				/*Reset and program the TL:*/
				BREG_WriteField(hChn->hRegister, DS_FRZ,	TLFRZ, 1);
				BREG_WriteField(hChn->hRegister, DS_TLI,	TLVAL, 0);
				/*FOI Timing Loop Setup or Transition Tracker Setup*/
				if (hChn->pChnAcqParam->BADS_Internal_Params.FOI_Timing_te == BADS_Internal_Params_eEnable)
				{
					BREG_Write32(hChn->hRegister, BCHP_DS_TL , 0x00000609);				  /*Full precision PD, Enable BaseBand, reset TL*/
					BREG_Write32(hChn->hRegister, BCHP_DS_TLC , 0x00440000);
					BREG_Write32(hChn->hRegister, BCHP_DS_TLAGCI , 0x06000000);			/*Set Timing Loop AGC to Gain 768x*/
				}
				else
				{
					BREG_Write32(hChn->hRegister, BCHP_DS_TLAGCI , 0x00100000);			/*Set Timing Loop AGC to Gain 8x*/
					BREG_Write32(hChn->hRegister, BCHP_DS_TL , 0x00000089);				  /*Full precision PD, Enable Transition Tracker, reset TL*/
				}
				RetCode_u32 = BADS_P_Set_TL_Frequency(hChn, hChn->pChnAcqParam->BADS_Local_Params.VIDRate_u32, BaudRates_TBL_u32[BaudIndex_u8]);
				if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
				
				/*Setup Frontmost Mixer Loop using callback function to WFE/UFE or CFL loop if callback is disabled*/
				if (hChn->pChnAcqParam->BADS_Internal_Params.Callback_Enable_te == BADS_Internal_Params_eEnable)
				{
					CallbackFrontend.hTunerChn = (hChn->pCallbackParam[BADS_Callback_eTuner]);
					CallbackFrontend.Freq_Offset = CarrierOffset_i32;
					CallbackFrontend.Symbol_Rate = BaudRates_TBL_u32[BaudIndex_u8];
					CallbackFrontend.Mode = BADS_CallbackMode_eSetMode;
					BKNI_EnterCriticalSection();
					(hChn->pCallback[BADS_Callback_eTuner])(&CallbackFrontend);	
					BKNI_LeaveCriticalSection();
				}
				else
				{
					RetCode_u32 = BADS_P_Set_CFL_Frequency(hChn, hChn->pChnAcqParam->BADS_Local_Params.SampleRate_u32, (-CarrierOffset_i32));
					if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
				}

				/*Attempt to get Timing Tone for this BaudIndex_u8 and CarrierIndex_u8 by running Timing FFT*/
				/*Tone found is indicated by FFT_TimingFreq_u32 != 0*/
				RetCode_u32 = BADS_P_Get_TimingScan_Advanced_FFT(hChn, BaudRates_TBL_u32[BaudIndex_u8], false, &FFT_TimingFreq_u32);
				if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
				if (PRINT_DEBUG==1) BDBG_ERR(("CarrierOffset_i32 = %d FFT_TimingFreq_u32 = %d",CarrierIndexEnd_u8,FFT_TimingFreq_u32));
				hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te = BADS_Local_Params_AcqStatus_eNoTiming;
				
				/*5: Continue to determine timing lock if a timing tone has been found by FFT*/
				if (FFT_TimingFreq_u32 !=0) /*Timing has been found by FFT*/
				{
					/*Start timing loop with Trk1 coefficients using the result returned by the Timing FFT*/
					BREG_WriteField(hChn->hRegister, DS_TLC, COMBO_COEFFS, TimingLoopTrk1Coeffs_TBL_u16[BaudIndex_u8]);
					RetCode_u32 = BADS_P_Set_TL_Frequency(hChn, hChn->pChnAcqParam->BADS_Local_Params.VIDRate_u32, FFT_TimingFreq_u32);
					if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
					BREG_WriteField(hChn->hRegister, DS_TLI,	TLVAL, 0);
					BREG_WriteField(hChn->hRegister, DS_FRZ,	TLFRZ, 0);

					/*Wait TL_TIME_ACQ_BAUD_SAMPLES for timing loop convergence*/
					EarlyExit_b = BADS_P_ADS_SLEEP(hChn, TL_TIME_ACQ_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
					if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
	
					/*Reduce timing loop coefficients to Trk2 coefficients*/
					BREG_WriteField(hChn->hRegister, DS_TLC, COMBO_COEFFS, TimingLoopTrk2Coeffs_TBL_u16[BaudIndex_u8]);	

					/*Wait TL_TIME_TRK_BAUD_SAMPLES for timing loop to settle*/
					EarlyExit_b = BADS_P_ADS_SLEEP(hChn, TL_TIME_TRK_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
					if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
		
					/*Use FFT to check if timing is locked for this BaudIndex_u8 and CarrierIndex_u8 indicated by FFT_TimingBin_u32 = 2048*/
					RetCode_u32 = BADS_P_Get_TimingScan_Advanced_FFT(hChn, BaudRates_TBL_u32[BaudIndex_u8], true, &FFT_TimingBin_u32);
					if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
					if (PRINT_DEBUG==1) BDBG_ERR(("FFT_TimingFreq_u32 = %d FFT_TimingBin_u32 = %d",FFT_TimingFreq_u32, FFT_TimingBin_u32));
	
					/*6: Continue to carrier lock if timing loop is locked*/
					if (FFT_TimingBin_u32 == 2048) /*Timing is locked*/
					{
						/*Reset Equalizer*/
						/*The Equalizer Initial Settings are in a Lookup Table in acquire.h*/
						BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CTL, DS_EQ_CTL_TBL_u32[BADS_Local_Params_QAM_eQam256]);
						BREG_Write32(hChn->hRegister, BCHP_DS_EQ_LEAK, 0x00000000);			/*no leakage*/
						BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FMTHR, 0x00000020);		/*set main tap threshhold to gen IRQ*/
						if (hChn->pChnAcqParam->BADS_Internal_Params.BigEQ_te == BADS_Internal_Params_eEnable)
						{
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFE, 0x11101119);			/*Main tap at tap 17, length = 36 taps, main mu = 2^-6, other mu = 2^-8, update at symbol rate*/
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFEU17, 0x30000000);		/*Initialize Main Tap 18*/
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_DFE, 0x00001829);			/*DFE length = 36 taps, mu = 2^-8, reset taps*/
						}
						else
						{
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFE, 0x0B101089);			/*Main tap at tap 11, length = 18 taps, main mu = 2^-6, other mu = 2^-8, update at symbol rate*/
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFEU11, 0x30000000);		/*Initialize Main Tap 18*/
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_DFE, 0x00001819);			/*DFE length = 24 taps, mu = 2^-8, reset taps*/
						}

						/*Reset CWC*/
						BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC, 0xFF0000F0);
	
						/*Release Equalizer FFE, FFE Main Tap Real and ONLY THE NON_OVERLAP DFE taps*/
						BREG_WriteField(hChn->hRegister, DS_FRZ,	FFEFRZ, 0);					 
						BREG_WriteField(hChn->hRegister, DS_FRZ,	FFRZMR, 0);				  
						if (hChn->pChnAcqParam->BADS_Internal_Params.BigEQ_te == BADS_Internal_Params_eEnable)
						{
							BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ19_24, 0);    
							BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ25_30, 0);
							BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ31_36, 0);
						}
						else
						{
							BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ7_12, 0);    
							BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ13_18, 0); 
							BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ19_24, 0); 
						}

						/*Wait CMA_TIME_BLIND1_BAUD_SAMPLES samples for FFE/DFE to converge in CMA mode*/
						/*This is the first pass of the EQ in CMA mode without HUM-AGC*/
						EarlyExit_b = BADS_P_ADS_SLEEP(hChn, CMA_TIME_BLIND1_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
						if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/

						/*Reduce FFE Mu's*/
						BREG_WriteField(hChn->hRegister, DS_EQ_FFE, MAINSTEP, 0x2);		/*main mu = 2^-8*/
						BREG_WriteField(hChn->hRegister, DS_EQ_FFE, STEP, 0x3);			/*other mu = 2^-10*/  
						
						/*Wait CMA_TIME_BLIND2_BAUD_SAMPLES samples for FFE/DFE to settle in CMA mode*/
						EarlyExit_b = BADS_P_ADS_SLEEP(hChn, CMA_TIME_BLIND2_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
						if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/

						/*Freeze Equalizer to run Carrier FFT*/
						BREG_WriteField(hChn->hRegister, DS_FRZ,	FFEFRZ, 1);					  
						BREG_WriteField(hChn->hRegister, DS_FRZ,	FFRZMR, 1);				    
						BREG_WriteField(hChn->hRegister, DS_FRZ,	COMBO_DFEFRZ, 0x3F); 

	
						/*Determine symbol rate to be used by the Carrier FFT and run Carrier FFT*/
						RetCode_u32 = BADS_P_Get_VID_Error(hChn, hChn->pChnAcqParam->BADS_Local_Params.VIDRate_u32, &VID_Error_i32);
						if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
						Symbol_Rate_u32 = (uint32_t)((int32_t)FFT_TimingFreq_u32 + VID_Error_i32);
						RetCode_u32 = BADS_P_Get_CarrierScan_Advanced_FFT(hChn, Symbol_Rate_u32, &Carrier_Error_i32);
						if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
						if (PRINT_DEBUG==1) BDBG_ERR(("Symbol_Rate_u32 = %d  CarrierOffset_i32 = %d Carrier_Error_i32 = %d",Symbol_Rate_u32, CarrierOffset_i32, Carrier_Error_i32));
	
						/*Write Carrier Offset to the Frontmost Mixer Loop using callback function to WFE/UFE or CFL loop if callback is disabled*/
						if (hChn->pChnAcqParam->BADS_Internal_Params.Callback_Enable_te == BADS_Internal_Params_eEnable)
						{
							CallbackFrontend.hTunerChn = (hChn->pCallbackParam[BADS_Callback_eTuner]);
							CallbackFrontend.Freq_Offset = (CarrierOffset_i32+Carrier_Error_i32);
							CallbackFrontend.Symbol_Rate = 1;  /*This used to be Symbol_Rate_u32 but that caused dropouts in 3461*/
							CallbackFrontend.Mode = BADS_CallbackMode_eSetMode;
							BKNI_EnterCriticalSection();
							(hChn->pCallback[BADS_Callback_eTuner])(&CallbackFrontend);
							BKNI_LeaveCriticalSection();
						}
						else
						{
							RetCode_u32 = BADS_P_Set_CFL_Frequency(hChn, hChn->pChnAcqParam->BADS_Local_Params.SampleRate_u32, (-CarrierOffset_i32-Carrier_Error_i32));
							if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
						}

						/**********************************************************************/
						/*At this point the FFT has found the Timing Offset and Carrier Offset*/
						/**********************************************************************/		

						/*7: Allow for fast/slow scan if Dual Scan is enabled during Scan and SlowAcquireScan modes using EQIndexEnd_u8*/
						if ((hChn->pChnAcqParam->BADS_Internal_Params.Dual_Scan_te == BADS_Internal_Params_eEnable) &&
								((hChn->pChnAcqParam->BADS_Local_Params.AcqType_te == BADS_Local_Params_AcqType_eScan) ||
								 (hChn->pChnAcqParam->BADS_Local_Params.AcqType_te == BADS_Local_Params_AcqType_eSlowAcquireScan)))
						{
							EQIndexEnd_u8 = 1; /*Two passes: Fast EQ Convergence followed by Slow EQ Convergence  during non-directed acquisitions*/
						}
						else
						{
							EQIndexEnd_u8 = 0; /*One pass:Fast or slow during directed acquisitions*/
						}
	
						for (EQIndex_u8=0;EQIndex_u8<=EQIndexEnd_u8;EQIndex_u8++)
						{
							if (PRINT_DEBUG==1) BDBG_ERR(("EQIndex_u8 = %d  EQIndex_u8 = %d",EQIndex_u8,EQIndex_u8));
								
							/*Reset Equalizer here since the EQ solution with carrier offset may not be right*/
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_LEAK, 0x00000000);			/*no leakage*/
							if (hChn->pChnAcqParam->BADS_Internal_Params.BigEQ_te == BADS_Internal_Params_eEnable)
							{
								BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFE, 0x11101119);			/*Main tap at tap 17, length = 36 taps, main mu = 2^-6, other mu = 2^-8, update at symbol rate*/
								BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFEU17, 0x2E400000);		/*Initialize Main Tap 20*/
								BREG_Write32(hChn->hRegister, BCHP_DS_EQ_DFE, 0x00001829);			/*DFE length = 36 taps, mu = 2^-8, reset taps*/
							}
							else
							{
								BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFE, 0x0B101089);			/*Main tap at tap 11, length = 18 taps, main mu = 2^-6, other mu = 2^-8, update at symbol rate*/
								BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFEU11, 0x2E400000);		/*Initialize Main Tap 18*/
								BREG_Write32(hChn->hRegister, BCHP_DS_EQ_DFE, 0x00001819);			/*DFE length = 18 taps, mu = 2^-8, reset taps*/
							}

							/*Reset hum-AGC Gain offset, leakage, reset loop integrator*/
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_AGC, 0x00400001);
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_AGCI, 0);
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_AGCC, 0x00a00000);		/*AGCLCOEFF = 160*2^-15, AGCICOEFF = 0*/
							BREG_Write32(hChn->hRegister, BCHP_DS_EQ_AGCPA, 0x00000000);
							
							/*Release EQ and DD-AGC if enabled and ONLY THE NON_OVERLAP DFE taps*/
							BREG_WriteField(hChn->hRegister, DS_FRZ,	FFEFRZ, 0);			/*Release FFE*/
							if (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable)
							{	
								BREG_WriteField(hChn->hRegister, DS_EQ_CTL, HUM_EN, 1);
								BREG_WriteField(hChn->hRegister, DS_FRZ,	HUMAGCFRZ, 0);	/*Release HUM-AGC Loop*/
							}
							else
							{
								BREG_WriteField(hChn->hRegister, DS_FRZ,	FFRZMR, 0);		/*Release FFE Main Tap Real*/
							}
							if (hChn->pChnAcqParam->BADS_Internal_Params.BigEQ_te == BADS_Internal_Params_eEnable)
							{
								BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ19_24, 0);    
								BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ25_30, 0);
								BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ31_36, 0);
							}
							else
							{
								BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ7_12, 0);    
								BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ13_18, 0); 
								BREG_WriteField(hChn->hRegister, DS_FRZ,	DFEFRZ19_24, 0); 
							}
									
							/*Wait CMA_TIME_LOCKED1_BAUD_SAMPLES samples for FFE/DFE to converge in CMA mode*/
							/*This is the second pass of the EQ in CMA mode without HUM-AGC*/
							EarlyExit_b = BADS_P_ADS_SLEEP(hChn, CMA_TIME_LOCKED1_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
							if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
		
							/*Reduce FFE Mu's */
							BREG_WriteField(hChn->hRegister, DS_EQ_FFE, MAINSTEP, 0x2);		/*main mu = 2^-8*/
							BREG_WriteField(hChn->hRegister, DS_EQ_FFE, STEP, 0x3);			/*other mu = 2^-10*/  
								
							/*Wait CMA_TIME_LOCKED2_BAUD_SAMPLES samples for FFE/DFE to settle in CMA mode*/
							EarlyExit_b = BADS_P_ADS_SLEEP(hChn, CMA_TIME_LOCKED2_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
							if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
		
							/*Freeze Equalizer to read coeffs*/
							BREG_WriteField(hChn->hRegister, DS_FRZ,	FFEFRZ, 1);					  
							BREG_WriteField(hChn->hRegister, DS_FRZ,	FFRZMR, 1);				    
							BREG_WriteField(hChn->hRegister, DS_FRZ,	COMBO_DFEFRZ, 0x3F); 
								
							/*Read Equalizer Coefficients*/
							for (Index_u8=0;Index_u8<144;Index_u8++)
							{
								EQ_u32[Index_u8] = BREG_Read32(hChn->hRegister, BCHP_DS_EQ_FFEU0 + 4*Index_u8);
							}
		
							/*Setup CWC*/
							if (hChn->pChnAcqParam->BADS_Internal_Params.CWC_te == BADS_Internal_Params_eEnable)
							{		
								/*Reset/Freeze ALL CWC integrators, Freeze ALL CWC Taps*/
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, LF_RESET, 0xF);
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, LF_FRZ, 0xF);
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, FREEZE, 0xF);
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, PLL_MODE, CWC_PLL);	                     
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, STEPSIZE1, CWC_MU1);	
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, STEPSIZE2, CWC_MU2);
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, STEPSIZE3, CWC_MU3);
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, STEPSIZE4, CWC_MU4);
#if (BCHP_FAMILY!=7584) /* 7584 review */
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC_FSBAUD, FCW_BAUD_SEL, 1);					        
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC_FSBAUD, FSBAUD_Hz, hChn->pChnAcqParam->BADS_Local_Params.SampleRate_u32/2);							 
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC_FSCARR, FCW_CARR_SEL, 1);					      
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC_FSCARR, FSCARR_Hz, hChn->pChnAcqParam->BADS_Local_Params.VIDRate_u32);									   
#endif		
								/*CWC Auto needs callback enabled*/
								/*BADS_P_Set_CWC_Auto() sets CWC Length, CWC FIN1-4 and Foffset1-4*/
								if (hChn->pChnAcqParam->BADS_Internal_Params.Callback_Enable_te == BADS_Internal_Params_eEnable)
								{			
									/*Get the RF Frequency using the callback function*/
									CallbackFrontend.hTunerChn = (hChn->pCallbackParam[BADS_Callback_eTuner]);
									CallbackFrontend.Mode = BADS_CallbackMode_eRequestMode;
									BKNI_EnterCriticalSection();
									(hChn->pCallback[BADS_Callback_eTuner])(&CallbackFrontend);
									BKNI_LeaveCriticalSection();
#if WFE
									RetCode_u32 = BADS_P_Set_CWC_Auto(hChn, Symbol_Rate_u32, CallbackFrontend.Total_Mix_After_ADC, &CWC_Length_u8);
									if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
#else
									RetCode_u32 = BADS_P_Set_CWC_Auto(hChn, Symbol_Rate_u32, (int32_t)CallbackFrontend.RF_Freq + CallbackFrontend.Total_Mix_After_ADC, &CWC_Length_u8);
									if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
#endif
								}

								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, LENGTH, CWC_Length_u8);
								/*If Length is 0 write F, 1 write E, 2 write C, 3 write 8, 4 write 0*/
								/*Because of crazy CWC programming use (15 - power 2^length - 1)*/
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, LF_FRZ, (15 - ((1<<CWC_Length_u8) - 1)));		/*Unfreeze integrator*/
								BREG_WriteField(hChn->hRegister, DS_EQ_CWC, FREEZE, (15 - ((1<<CWC_Length_u8) - 1)));		/*Unfreeze Tap Freeze*/
								
								/*Shift the Equalizer based on the CWC length but compensate for FFT length*/
								/*Shift the main tap location to the left by the CWC_Length if FFT is 36 taps, CWC_Length - 1 if FFT is 35 taps etc*/
								/*Recalculate CWC_Length to compendate for FFE's of less than 35 taps*/
								switch (BREG_ReadField(hChn->hRegister, DS_EQ_FFE, LENGTH))
								{
									case 35: CWC_Length_u8 = CWC_Length_u8; break;
									case 34: CWC_Length_u8 = (CWC_Length_u8 == 0) ? 0 : CWC_Length_u8 - 1; break;
									case 33: CWC_Length_u8 = (CWC_Length_u8 <= 1) ? 0 : CWC_Length_u8 - 1; break;
									case 32: CWC_Length_u8 = (CWC_Length_u8 <= 2) ? 0 : CWC_Length_u8 - 1; break;
									default: CWC_Length_u8 = 0;
								}			
								BREG_WriteField(hChn->hRegister, DS_EQ_FFE, MAIN, (BREG_ReadField(hChn->hRegister, DS_EQ_FFE, MAIN) - (uint32_t)CWC_Length_u8));
								/*Shift FFE Taps to make up for the CWC and preserve the correct number of post FFE taps*/
								for (Index_u8=0;Index_u8<72;Index_u8++)
								{
									EQ_u32[Index_u8] = EQ_u32[Index_u8+2*CWC_Length_u8];
								}
								/*Reset the stolen FFE taps to 0*/
								for (Index_u8=0;Index_u8<2*CWC_Length_u8;Index_u8++)
								{
									EQ_u32[71-Index_u8] = 0;
								}

							}

							/*8: Cycle through the QAM modes from 256Q/64Q/128Q/32Q/16Q/1024Q using QamIndex_u8 0-5*/
							for (QamIndex_u8=0;QamIndex_u8<6;QamIndex_u8++)
							{
								switch (QamIndex_u8)
								{
								case 0: QamTry_b = ((hChn->pChnAcqParam->BADS_Local_Params.Q256A_b == true) || (hChn->pChnAcqParam->BADS_Local_Params.Q256B_b == true)) ? true : false; 
												hChn->pChnAcqParam->BADS_Local_Params.QAM_te = BADS_Local_Params_QAM_eQam256;
												FFE_Scale_u32 = (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable) ? 256	: 256;
												break;
								case 1: QamTry_b = ((hChn->pChnAcqParam->BADS_Local_Params.Q64A_b == true) || (hChn->pChnAcqParam->BADS_Local_Params.Q64B_b == true)) ? true : false; 
												hChn->pChnAcqParam->BADS_Local_Params.QAM_te = BADS_Local_Params_QAM_eQam64;
												FFE_Scale_u32 = (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable) ? 254	: 253;
												break;
								case 2: QamTry_b =  (hChn->pChnAcqParam->BADS_Local_Params.Q128A_b == true) ? true : false; 
												hChn->pChnAcqParam->BADS_Local_Params.QAM_te= BADS_Local_Params_QAM_eQam128;
												FFE_Scale_u32 = (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable) ? 237	: 233;
												break;
								case 3: QamTry_b =  (hChn->pChnAcqParam->BADS_Local_Params.Q32A_b == true) ? true : false; 
												hChn->pChnAcqParam->BADS_Local_Params.QAM_te = BADS_Local_Params_QAM_eQam32;
												FFE_Scale_u32 = (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable) ? 234	: 227; 
												break;
								case 4: QamTry_b =  (hChn->pChnAcqParam->BADS_Local_Params.Q16A_b == true) ? true : false; 
												hChn->pChnAcqParam->BADS_Local_Params.QAM_te = BADS_Local_Params_QAM_eQam16;
												FFE_Scale_u32 = (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable) ? 248	: 242;
												break;
								case 5: QamTry_b =  (hChn->pChnAcqParam->BADS_Local_Params.Q1024B_b == true) ? true : false; 
												hChn->pChnAcqParam->BADS_Local_Params.QAM_te = BADS_Local_Params_QAM_eQam1024;
												FFE_Scale_u32 = (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable) ? 256	: 257;
												break;
								default : 
									BDBG_ERR(("INVALID QamIndex_u8 Value"));
									break;
								}
		
								/*9: If QAM modes match user selection set QamTry_b to true*/
								if (QamTry_b == true)
								{
									/*Freeze Equalizer to write scaled coeffs*/
									BREG_WriteField(hChn->hRegister, DS_FRZ,	FFEFRZ, 1);					  
									BREG_WriteField(hChn->hRegister, DS_FRZ,	FFRZMR, 1);				    
									BREG_WriteField(hChn->hRegister, DS_FRZ,	COMBO_DFEFRZ, 0x3F);
		
									/*Scale the EQ coeffs differently for each QAM Mode*/
									for (Index_u8=0;Index_u8<144;Index_u8++)
									{
										if (Index_u8 < 72)	/*FFE Coeffs to be scaled*/
										{
											if (Index_u8%2==0)
											{
												/*Strip out I and Q portions of EQ coefficients*/
												/*check sign of coefficient and convert to int32_t */
												/*scale coefficient*/
												/*Saturate I and Q portions of EQ coefficients to -POWER2_24 to +POWER2_24_M1*/
												FFE_Scaled_Value_u32 = (((EQ_u32[Index_u8]>>8) & 0x00FFFF00) | ((EQ_u32[Index_u8+1]>>24) & 0x000000FF));
												FFE_I_i32 = ((FFE_Scaled_Value_u32 & 0x00800000) !=0) ? -1*(POWER2_24-(int32_t)FFE_Scaled_Value_u32) : (int32_t)FFE_Scaled_Value_u32;
												FFE_I_i32 = ((int32_t)FFE_Scale_u32*FFE_I_i32)/POWER2_8;
												FFE_I_i32 =	(FFE_I_i32 > POWER2_24_M1) ? POWER2_24_M1 : (FFE_I_i32 < -POWER2_24) ? -POWER2_24 : FFE_I_i32;

												FFE_Scaled_Value_u32 = (((EQ_u32[Index_u8]<<8) & 0x00FFFF00) | ((EQ_u32[Index_u8+1]>>16) & 0x000000FF));
												FFE_Q_i32 = ((FFE_Scaled_Value_u32 & 0x00800000) !=0) ? -1*(POWER2_24-(int32_t)FFE_Scaled_Value_u32) : (int32_t)FFE_Scaled_Value_u32;
												FFE_Q_i32 = ((int32_t)FFE_Scale_u32*FFE_Q_i32)/POWER2_8;
												FFE_Q_i32 =	(FFE_Q_i32 > POWER2_24_M1) ? POWER2_24_M1 : (FFE_Q_i32 < -POWER2_24) ? -POWER2_24 : FFE_Q_i32;
																					
												/*Create upper 16 bits of FFE I/Q coeffs*/
												FFE_Scaled_Value_u32 = (uint32_t)(((FFE_I_i32<<8) & 0xFFFF0000) | ((FFE_Q_i32>>8) & 0x0000FFFF));
											}
											else 
											{
												/*Create lower 16 bits of FFE I/Q coeffs*/
												FFE_Scaled_Value_u32 = (uint32_t)(((FFE_I_i32<<24) & 0xFF000000) | ((FFE_Q_i32<<16) & 0x00FF0000));
											}
											/*write scaled FFE coefficient*/
											BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFEU0 + 4*Index_u8, FFE_Scaled_Value_u32);
										}
										else /*DFE Coeffs NOT to be scaled*/
										{
											BREG_Write32(hChn->hRegister, BCHP_DS_EQ_FFEU0 + 4*Index_u8, EQ_u32[Index_u8]);
										}
									}
								
									/*************************************************************************************************************/
									/*Now that the EQ has been scaled to the right constellation, start EQ with HUM-AGC/IMC/CWC if they are enabled*/
									/**************************************************************************************************************/
									
									/*Setup CFL:*/
									BREG_WriteField(hChn->hRegister, DS_FRZ,	CFLFRZ, 1);
									BREG_Write32(hChn->hRegister, BCHP_DS_CFLI, 0);
									BREG_WriteField(hChn->hRegister, DS_CFLC, COMBO_COEFFS, FrequencyLoopCoeffs_TBL_u16[BaudIndex_u8]);
		
									/*Setup EQ:*/
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CTL, DS_EQ_CTL_TBL_u32[hChn->pChnAcqParam->BADS_Local_Params.QAM_te]);
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_LEAK, 0x00000000);
																		
		 							/*Reset hum-AGC Gain offset, leakage, reset loop integrator*/
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_AGC, 0x00400001);
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_AGCI, 0);
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_AGCC, 0x00a00000);		/*AGCLCOEFF = 160*2^-15, AGCICOEFF = 0*/
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_AGCPA, 0x00000000);
										
									/*Setup CPL:*/
									BREG_WriteField(hChn->hRegister, DS_FRZ,	CPLFRZ, 1);
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CPLI, 0);
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CPL, DS_EQ_CPL_TBL_u32[hChn->pChnAcqParam->BADS_Local_Params.QAM_te]);
									BREG_WriteField(hChn->hRegister, DS_EQ_CPLC, COMBO_COEFFS, PhaseLoopAcqCoeffs_TBL_u32[BaudIndex_u8]);	
				
									/*Configure SNR performance Monitoring*/
									BREG_WriteField(hChn->hRegister, DS_EQ_SNRLT, SNRLTHRESH, SNRLTHRESH_TBL_u32[hChn->pChnAcqParam->BADS_Local_Params.QAM_te]);
									BREG_WriteField(hChn->hRegister, DS_EQ_SNRHT, SNRHTHRESH, SNRHTHRESH_TBL_u32[hChn->pChnAcqParam->BADS_Local_Params.QAM_te]);
		
									/*Set CWC BWs to acquisition BWs and reset integrators and turn off leakage*/
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_LFC1, CWC_ACQ_LFC1);  
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_LFC2, CWC_ACQ_LFC2);  
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_LFC3, CWC_ACQ_LFC3);  
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_LFC4, CWC_ACQ_LFC4); 
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_INT1, 0);
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_INT2, 0);
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_INT3, 0);
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_INT4, 0);
									BREG_WriteField(hChn->hRegister, DS_EQ_CWC_LEAK, CWC_LEAK1, 0);
									BREG_WriteField(hChn->hRegister, DS_EQ_CWC_LEAK, CWC_LEAK2, 0);
									BREG_WriteField(hChn->hRegister, DS_EQ_CWC_LEAK, CWC_LEAK3, 0);
									BREG_WriteField(hChn->hRegister, DS_EQ_CWC_LEAK, CWC_LEAK4, 0);
	
									/*Release EQ and DD-AGC if enabled*/
									BREG_WriteField(hChn->hRegister, DS_FRZ,	FFEFRZ, 0);			/*Release FFE*/
									if (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable)
									{	
										BREG_WriteField(hChn->hRegister, DS_EQ_CTL, HUM_EN, 1);
										BREG_WriteField(hChn->hRegister, DS_FRZ,	HUMAGCFRZ, 0);	/*Release HUM-AGC Loop*/
									}
									else
									{
										BREG_WriteField(hChn->hRegister, DS_FRZ,	FFRZMR, 0);		/*Release FFE Main Tap Real*/
									}

									if (hChn->pChnAcqParam->BADS_Internal_Params.BigEQ_te == BADS_Internal_Params_eEnable)
									{
										BREG_WriteField(hChn->hRegister, DS_FRZ,	COMBO_DFEFRZ, 0);	/*Release DFE Taps 0-35*/
									}
									else
									{
										BREG_WriteField(hChn->hRegister, DS_FRZ,	COMBO_DFEFRZ, 0x30);	/*Release DFE Taps 0-23*/
									}

									/*use reduced FFE Mus*/				
									BREG_WriteField(hChn->hRegister, DS_EQ_FFE, MAINSTEP, 0x2);		/*main mu = 2^-8*/
									BREG_WriteField(hChn->hRegister, DS_EQ_FFE, STEP, 0x3);			/*other mu = 2^-10*/    
		
#if IMC
									/*Start IMC*/
									if (hChn->pChnAcqParam->BADS_Internal_Params.IMC_te == BADS_Internal_Params_eEnable)
									{ 
										BREG_WriteField(hChn->hRegister, DS_EQ_CTL, IMC_EN, 1);
										BREG_Write32(hChn->hRegister, BCHP_DS_EQ_IMC, 0x0c001119);   /*36 IMC taps, main tap is 0 left of FFE main, mu = 2^-6, full LMS, update sym, leak off, coarse mode off*/
										BREG_WriteField(hChn->hRegister, DS_FRZ,	IMCFRZ, 0);		 /*Release Image Canceller*/
									}	
#endif
									/*RFI support for second pass if Dual Scan is selected OR if SlowAcquire is selected: Longer CMA congervenge time*/
									if((EQIndex_u8 != 0) || (hChn->pChnAcqParam->BADS_Local_Params.AcqType_te == BADS_Local_Params_AcqType_eSlowAcquire))	
									{
										if ((hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam1024) ||
											(hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam512) || 
											(hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam256) ||
											(hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam128) ||
											(hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam32))
										{
											EarlyExit_b = BADS_P_ADS_SLEEP(hChn, CMA_TIME_SLOW_TRIM1_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
											if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
										}
										else
										{
											EarlyExit_b = BADS_P_ADS_SLEEP(hChn, CMA_TIME_SLOW_TRIM2_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
											if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
										}
									}
									else
									{
											EarlyExit_b = BADS_P_ADS_SLEEP(hChn, CMA_TIME_FAST_TRIM_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
											if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
									}
	
									/*Start Carrier Phase Loop, this is a very small sweep of a KHz or so*/
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CPLI, (uint32_t)PhaseLoopSweep_FFT_TBL_struct[BaudIndex_u8].PosSweepStart);	
									BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CPLSWP, (uint32_t)PhaseLoopSweep_FFT_TBL_struct[BaudIndex_u8].PosSweepRate);
									BREG_WriteField(hChn->hRegister, DS_FRZ,	CPLFRZ, 0);					/*Release Carrier Phase Loop Freeze*/	
									EarlyExit_b = BADS_P_ADS_SLEEP(hChn, (unsigned int)PhaseLoopSweep_FFT_TBL_struct[BaudIndex_u8].SweepTime);
									if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
									BREG_WriteField(hChn->hRegister, DS_EQ_CPLSWP, SWEEP, 0);				/*Stop Sweeping*/
		
		
									/*Reduce Carrier Phase Loop coefficient to Tracking Bandwidths*/
									BREG_WriteField(hChn->hRegister, DS_EQ_CPLC, COMBO_COEFFS, PhaseLoopTrkCoeffs_TBL_u32[BaudIndex_u8]);	
		
									/*Set DDAGC BW for Tracking Mode*/
									if (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable)
									{ 
										BREG_Write32(hChn->hRegister, BCHP_DS_EQ_AGCC, 0x00080000);				/*AGCLCOEFF = 8*2^-15, AGCICOEFF = 0*/	
									}
		
									/*Switch EQ to LMS mode*/
									BREG_WriteField(hChn->hRegister, DS_EQ_CTL, CMAEN, 0);
	
									/*RFI support for second pass if Dual Scan is selected OR if SlowAcquire is selected: Longer LMS-DD congervenge time*/
									if((EQIndex_u8 != 0) || (hChn->pChnAcqParam->BADS_Local_Params.AcqType_te == BADS_Local_Params_AcqType_eSlowAcquire))	
									{
										if ((hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam1024) ||
											(hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam512) || 
											(hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam256) ||
											(hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam128) ||
											(hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam32))
										{
											EarlyExit_b = BADS_P_ADS_SLEEP(hChn, LMS_TIME_SLOW_TRIM1_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
											if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
										}
										else
										{
											EarlyExit_b = BADS_P_ADS_SLEEP(hChn, LMS_TIME_SLOW_TRIM2_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
											if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
										}
									}
									else
									{
										EarlyExit_b = BADS_P_ADS_SLEEP(hChn, LMS_TIME_FAST_TRIM_BAUD_SAMPLES*1000/FFT_TimingFreq_u32 + 1);
										if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
									}
	
#if IMC
									/*Reduce IMC Bandwidth*/
									if (hChn->pChnAcqParam->BADS_Internal_Params.IMC_te == BADS_Internal_Params_eEnable)
									{
										BREG_WriteField(hChn->hRegister, DS_EQ_IMC, STEP, 2);			/*Set IMC Step Size = 2^-8*/
									}
#endif

/************************************/
/*Hack2 alert for 512/1024 QAM      */
/************************************/
									/*extra time seems to be needed before switching to DD-AGC in 512/1024 QAM*/
									if ((hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam512) || (hChn->pChnAcqParam->BADS_Local_Params.QAM_te == BADS_Local_Params_QAM_eQam1024))
									{
										EarlyExit_b = BADS_P_ADS_SLEEP(hChn, 15);
										if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
									}
		 
									/*Switch DDAGC to DD mode*/
									if (hChn->pChnAcqParam->BADS_Internal_Params.DDAGC_te == BADS_Internal_Params_eEnable)
									{
										BREG_WriteField(hChn->hRegister, DS_EQ_CTL, FNEN, 0);		 
									} 
		
									/*Start DFE Overlap Leak*/
									BREG_WriteField(hChn->hRegister, DS_EQ_LEAK, DFE_LEAK_OVERLAP, 0xD); 
			
									/*Set the final Phase Loop parameters*/
									BREG_WriteField(hChn->hRegister, DS_EQ_CPL, CPLFREQEN, 0);		 
									BREG_WriteField(hChn->hRegister, DS_EQ_CPL, CPLFTYPE, 0);			 
									BREG_WriteField(hChn->hRegister, DS_EQ_CPL, CPLLK, 0);	
	
									/*Unfreeze Front loop and start leak in carrier phase loop integrator and accumulator*/
									if (hChn->pChnAcqParam->BADS_Internal_Params.CFL_te == BADS_Internal_Params_eEnable)
									{
										BREG_WriteField(hChn->hRegister, DS_FRZ,	CFLFRZ, 0);		
										BREG_WriteField(hChn->hRegister, DS_EQ_CPL, CPLLK, 0xB);    /*leak at 2^-16*/ 
										BREG_WriteField(hChn->hRegister, DS_EQ_CPL, CPLPALK, 0x9);  /*leak at 2^-10*/ 
										/*This delay is needed because setting the CPLPALK to 0xD causes a loss of acquisition reliablility for 128 QAM*/
										EarlyExit_b = BADS_P_ADS_SLEEP(hChn, 7300000/FFT_TimingFreq_u32 + 1);
										if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
										BREG_WriteField(hChn->hRegister, DS_EQ_CPL, CPLPALK, 0xD); /*leak at 2^-6*/ 
									}
			
									/*Set the final CWC Loop parameters*/ 
									if (hChn->pChnAcqParam->BADS_Internal_Params.CWC_te == BADS_Internal_Params_eEnable)
									{	
										BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_LFC1, CWC_TRK_LFC1);  
										BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_LFC2, CWC_TRK_LFC2);  
										BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_LFC3, CWC_TRK_LFC3);  
										BREG_Write32(hChn->hRegister, BCHP_DS_EQ_CWC_LFC4, CWC_TRK_LFC4);
										BREG_WriteField(hChn->hRegister, DS_EQ_CWC_LEAK, CWC_LEAK1, CWC_LK1);
										BREG_WriteField(hChn->hRegister, DS_EQ_CWC_LEAK, CWC_LEAK2, CWC_LK2);
										BREG_WriteField(hChn->hRegister, DS_EQ_CWC_LEAK, CWC_LEAK3, CWC_LK3);
										BREG_WriteField(hChn->hRegister, DS_EQ_CWC_LEAK, CWC_LEAK4, CWC_LK4);
									}
	
									/*******************************************************************************************/
									/*ACQUISITION FINISHED: BEGIN LOCK CHECKING/SPECTRAL INVERSION/AUTOINVERSION*/
									/*******************************************************************************************/
	
/************************************/
/*Hack3 alert add for non square    */
/************************************/
							
									/*If carrier loop is unlocked set FECIndexEnd_u8 to 0 to bypass FEC checking otherwise set it to 2 to try AnnexA then AnnexB FEC*/
									/*Check that the phase loop is locked by making sure there is not much carrier offset in the loop, this is a weak lock detector*/
									hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te = BADS_Local_Params_AcqStatus_eNoCarrier;
									RetCode_u32 = BADS_P_Get_CPL_Error(hChn, Symbol_Rate_u32, &Phase_Error_i32);
									if (RetCode_u32 != BERR_SUCCESS) goto something_bad_happened; /*goto bottom of function to return error code*/
									Phase_Error_i32 = (Phase_Error_i32 < 0) ? -1*Phase_Error_i32 : Phase_Error_i32; 
									FECIndexEnd_u8 = (Phase_Error_i32 > MAX_PHASE_ERROR) ? 0 : 2;
									if (PRINT_DEBUG==1) BDBG_ERR(("Phase_Error_i32 = %d",Phase_Error_i32));
									
									/*10: Cycle through FECIndex to try AnnexA then Annex B*/
									for (FECIndex_u8=0;FECIndex_u8<FECIndexEnd_u8;FECIndex_u8++)
									{
										switch (FECIndex_u8)
										{
										/*Try if QamIndex_u8 is 0 and Q256A, 1 and Q64A, 2, 3, 4*/
										case 0:	
											FECTry_b = false;
											if (((QamIndex_u8 == 0) && (hChn->pChnAcqParam->BADS_Local_Params.Q256A_b == true)) ||
												((QamIndex_u8 == 1) && (hChn->pChnAcqParam->BADS_Local_Params.Q64A_b == true)) ||
												((QamIndex_u8 >= 2) && (QamIndex_u8 <= 4)))
											{
												FECTry_b = true;
											}
											hChn->pChnAcqParam->BADS_Local_Params.Annex_te = BADS_Local_Params_Annex_eAnnexA;
											FEC_TimeOut_u16 = ANNEXA_FEC_LOCK_TIMEOUT; 
											break;
										/*Try if QamIndex_u8 is 0 and Q256B, 1 and Q64B, 5*/
										case 1:
											FECTry_b = false;
											if (((QamIndex_u8 == 0) && (hChn->pChnAcqParam->BADS_Local_Params.Q256B_b == true)) ||
												((QamIndex_u8 == 1) && (hChn->pChnAcqParam->BADS_Local_Params.Q64B_b == true)) ||
												 (QamIndex_u8 == 5))
											{
												/*Only try if the symbol rate is within the Annex B bounds*/
												if ((((QamIndex_u8 == 0) || (QamIndex_u8 == 5)) && (Symbol_Rate_u32 >= MIN_Q256_Q1024_ANNEXB_SYMBOL_RATE) && (Symbol_Rate_u32 <= MAX_Q256_Q1024_ANNEXB_SYMBOL_RATE)) ||
													  (((QamIndex_u8 == 1) && (Symbol_Rate_u32 >= MIN_Q64_ANNEXB_SYMBOL_RATE) && (Symbol_Rate_u32 <= MAX_Q64_ANNEXB_SYMBOL_RATE))))
												{
													FECTry_b = true;
												}
											}
											hChn->pChnAcqParam->BADS_Local_Params.Annex_te = BADS_Local_Params_Annex_eAnnexB;	
											FEC_TimeOut_u16 = ANNEXB_FEC_LOCK_TIMEOUT; 
											break;
										default : 
											BDBG_ERR(("INVALID FECIndex_u8 Value"));
											break;
										}
											
										/*11: If FECIndex_u8 match user selection set FECTry_b to true, then program FEC and see if it locks*/
										hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te = BADS_Local_Params_AcqStatus_eNoFECLock;
										if (FECTry_b == true)
										{
											/*Program the FEC*/
											BADS_P_ProgramFEC(hChn);
											/*Set the FEC spectrum status Flag*/
											hChn->pChnAcqParam->BADS_Local_Params.FECSpectrum_te = BADS_Local_Params_FECSpectrum_eNotInverted;
											/*Check if the spectrum is to be inverted from the default setting*/
											if (hChn->pChnAcqParam->BADS_Local_Params.IS_b == true)
											{
												ReadReg_u32 = BREG_Read32(hChn->hRegister, BCHP_DS_FECL);
												ReadReg_u32 = ReadReg_u32 ^ 0x00000008;												
												BREG_Write32(hChn->hRegister, BCHP_DS_FECL, ReadReg_u32);
												hChn->pChnAcqParam->BADS_Local_Params.FECSpectrum_te = BADS_Local_Params_FECSpectrum_eInverted;
											}

											/*Reset FEC Counters*/
											BREG_WriteField(hChn->hRegister, DS_RST, FECRST, 1);		
											BREG_WriteField(hChn->hRegister, DS_RST, FECRST, 0);
											BREG_Write32(hChn->hRegister, BCHP_DS_TPFEC, 0x000F9F00);
											hChn->pChnAcqParam->BADS_Local_Params.Old_CBERC1_u32 = 0;
											hChn->pChnAcqParam->BADS_Local_Params.Old_UERC1_u32 = 0;
											hChn->pChnAcqParam->BADS_Local_Params.Old_NBERC1_u32 = 0;
											hChn->pChnAcqParam->BADS_Local_Params.StuckFECCount_u32 = STUCK_FEC_RESET_COUNT;
											/*Wait for FEC to sync and output data UP TO FEC_TimeOut_u16 ms*/
											Index_u8=0;
											while ((Index_u8<FEC_TimeOut_u16) && (BREG_ReadField(hChn->hRegister, DS_NBERC1, NBERCCNTVAL) == 0))
											{
												EarlyExit_b = BADS_P_ADS_SLEEP(hChn, 1);
												if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
												Index_u8++;
											}
											/*Check Lock*/
											BADS_P_ChnLockStatus(hChn);
		
											/*Check if Flip_Spectrum_b is true to try inverted spectrum*/
											if (hChn->pChnAcqParam->BADS_Local_Params.Flip_Spectrum_b == true)
											{
												/*If FEC is not locked try inverting the spectrum*/
												if (hChn->pChnLockStatus->FLK_te == BADS_3x7x_ChnLockStatus_eUnlock)
												{
													/*Invert Spectrum if FEC is not locked*/
													ReadReg_u32 = BREG_Read32(hChn->hRegister, BCHP_DS_FECL);
													ReadReg_u32 = ReadReg_u32 ^ 0x00000008;											/*Perform Spectral Inversion in the FEC*/
													BREG_Write32(hChn->hRegister, BCHP_DS_FECL, ReadReg_u32);
													BREG_WriteField(hChn->hRegister, DS_RST, FECRST, 1);			/*Reset FEC*/
													BREG_WriteField(hChn->hRegister, DS_RST, FECRST, 0);
													BREG_Write32(hChn->hRegister, BCHP_DS_TPFEC, 0x000F9F00);
													hChn->pChnAcqParam->BADS_Local_Params.Old_CBERC1_u32 = 0;
													hChn->pChnAcqParam->BADS_Local_Params.Old_UERC1_u32 = 0;
													hChn->pChnAcqParam->BADS_Local_Params.Old_NBERC1_u32 = 0;
													hChn->pChnAcqParam->BADS_Local_Params.StuckFECCount_u32 = STUCK_FEC_RESET_COUNT;
													if (hChn->pChnAcqParam->BADS_Local_Params.IS_b == true)
													{
														hChn->pChnAcqParam->BADS_Local_Params.FECSpectrum_te = BADS_Local_Params_FECSpectrum_eInvertedAutoInvert;
													}
													else
													{
														hChn->pChnAcqParam->BADS_Local_Params.FECSpectrum_te = BADS_Local_Params_FECSpectrum_eNotInvertedAutoInvert;
													}
	
													/*Wait for FEC to sync and output data UP TO FEC_TimeOut_u16 ms*/
													Index_u8=0;
													while ((Index_u8<FEC_TimeOut_u16) && (BREG_ReadField(hChn->hRegister, DS_NBERC1, NBERCCNTVAL) == 0))
													{
														EarlyExit_b = BADS_P_ADS_SLEEP(hChn, 1);
														if (EarlyExit_b == true) goto please_leave_early; /*goto bottom of function to leave early*/
														Index_u8++;
													}
													/*Check Lock*/
													BADS_P_ChnLockStatus(hChn);
												}
											}
		
											/*Reset SNR and Output Interface*/
											BREG_Write32(hChn->hRegister, BCHP_DS_EQ_SNR , 0x0000000F);					 
											BREG_Write32(hChn->hRegister, BCHP_DS_OI_VCO , 0x00000004);					 
											BREG_Write32(hChn->hRegister, BCHP_DS_OI_CTL , 0x00020011);	
											BREG_Write32(hChn->hRegister, BCHP_DS_OI_CTL , 0x00020011);	
											BREG_Write32(hChn->hRegister, BCHP_DS_OI_OUT, 0);							 
		
											/*Reset/Resync BER registers in chip*/
											BREG_Write32(hChn->hRegister, BCHP_DS_BER, 0x00000004);
											BREG_Write32(hChn->hRegister, BCHP_DS_BERI, 0xFFFFFFFF);
											BREG_Write32(hChn->hRegister, BCHP_DS_OI_BER_CTL, 0x00000004);
											BREG_Write32(hChn->hRegister, BCHP_DS_OI_BER, 0xFFFFFFFF);
		
											/*Report Lock Status*/
											if (hChn->pChnLockStatus->FLK_te == BADS_3x7x_ChnLockStatus_eLock)
											{
												/*RFI support*/
												if((EQIndex_u8 != 0) || (hChn->pChnAcqParam->BADS_Local_Params.AcqType_te == BADS_Local_Params_AcqType_eSlowAcquire))	
												{
													hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te = BADS_Local_Params_AcqStatus_eLockedSlow;
												}
												else
												{
													hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te = BADS_Local_Params_AcqStatus_eLockedFast;
												}
	
												/*leave function if locked, this is not an Early Exit but a normal exit*/
												EarlyExit_b = false;
												goto please_leave_early; /*goto bottom of function to leave early*/
											}

										} /*FECTry_b*/
									} /*FECIndex_u8*/
								} /*QamTry_b*/
							} /*QamIndex_u8*/
						} /*EQIndex_u8*/
					} /*FFT_TimingBin_u32 ==2048*/
				} /*FFT_TimingFreq_u32 !=0*/
			} /*CarrierIndex_u8*/
		} /*BaudIndex_u8*/
	}/*Retry Index_u8*/

/*goto label to return error code if something bad happened above*/
something_bad_happened:
/*goto label to exit early*/
please_leave_early:
	
	/*Check if we are performing an early exit from function*/
	if (EarlyExit_b == true) 
	{
		hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te = BADS_Local_Params_AcqStatus_eEarlyExit;
        BDBG_MSG(("ADS Acq. Early Exit."));
	}

	/*Assign the scan status parameters*/
	BADS_P_Set_ScanStatus_Params(hChn, QamIndex_u8, FECIndex_u8, CarrierOffset_i32, Carrier_Error_i32, Symbol_Rate_u32);

	/*Get elapsed time*/
	BTMR_ReadTimer(hChn->hAds->hStatusTimer, &EndTime_u32);
	ElapsedTime_u32 = (EndTime_u32 >= StartTime_u32) ? (EndTime_u32 - StartTime_u32) : ((BTMR_ReadTimerMax() - StartTime_u32) + EndTime_u32);
	hChn->pChnAcqParam->BADS_Local_Params.ElapsedTime_u32 = (uint16_t)(ElapsedTime_u32/(1000));
	hChn->pChnAcqParam->BADS_Local_Params.TotalTime_u32 += hChn->pChnAcqParam->BADS_Local_Params.ElapsedTime_u32;

	/*print acquisition results and elapsed time*/
	if(hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te == BADS_Local_Params_AcqStatus_eLockedSlow)	
	{
		BDBG_MSG(("Locked Slow: ElapsedTime_u32= %d ms TotalTime= %d ms",hChn->pChnAcqParam->BADS_Local_Params.ElapsedTime_u32,hChn->pChnAcqParam->BADS_Local_Params.TotalTime_u32));
	}
	else if (hChn->pChnAcqParam->BADS_Local_Params.AcqStatus_te == BADS_Local_Params_AcqStatus_eLockedFast)
	{
		BDBG_MSG(("Locked Fast: ElapsedTime_u32= %d ms TotalTime= %d ms",hChn->pChnAcqParam->BADS_Local_Params.ElapsedTime_u32,hChn->pChnAcqParam->BADS_Local_Params.TotalTime_u32));
	}
	else
	{
		BDBG_MSG(("Unlocked: ElapsedTime_u32= %d ms TotalTime= %d ms",hChn->pChnAcqParam->BADS_Local_Params.ElapsedTime_u32,hChn->pChnAcqParam->BADS_Local_Params.TotalTime_u32));
	}

	/*Test for acquisition probability using the spare register*/
	if (hChn->pChnAcqParam->BADS_Internal_Params.Acquisition_Test_te == BADS_Internal_Params_eEnable)
	{
		BADS_P_AcquisitionPercentageTest(hChn);
	}

	return RetCode_u32;
}

/***************************************************************************
 * BADS_P_AbortAcquire()
 ***************************************************************************/
BERR_Code BADS_P_AbortAcquire(BADS_3x7x_ChannelHandle hChn)
{
	BERR_Code RetCode_u32 = BERR_INVALID_PARAMETER;
	if ( NULL != hChn && DEV_MAGIC_ID == hChn->magicId )
	{
		BDBG_MSG(("BADS_P_AbortAcquire"));
		/* Request Early Exit */
		hChn->pChnAcqParam->BADS_Local_Params.EarlyExit = true;
		RetCode_u32 = BERR_SUCCESS;
	}
	return ( RetCode_u32 );
}


/********************************************************************************************/
/*HAB called function to Read DS FFE or DFE                                                 */
/********************************************************************************************/
BERR_Code BADS_P_HAB_Read_FFEDFE(BADS_3x7x_ChannelHandle hChn, uint8_t *HAB_Buffer_pu8, uint8_t Size_HAB_u8, uint8_t FFE_u8)	
{
	BERR_Code RetCode_u32 = BERR_SUCCESS;
	uint8_t count_u8;
	uint32_t Temp_u32;
	uint32_t CWC_Length_u32, ReadReg_u32;

	if ((Size_HAB_u8 != 36*4 + 1) || (FFE_u8>1))
	{
		BDBG_ERR(("ERROR!!! Incorrect HAB Size or FFE/DFE flag in BWFE_P_HAB_Read_FFE()"));
		RetCode_u32 = BERR_INVALID_PARAMETER;
		/*goto bottom of function to return error code*/
		goto something_bad_happened;
	}
	/*Assign Base Address to FFE or DFE*/
	Temp_u32 = (FFE_u8 == 1) ? BCHP_DS_EQ_FFEU0 : BCHP_DS_EQ_DFEU0;

	/*Copy chip data into HAB_Temp_Buffer, read FFE*/
	for (count_u8=0;count_u8<36;count_u8++)
	{
		ReadReg_u32 = BREG_Read32(hChn->hRegister, Temp_u32 + (count_u8*8));
		/*Zero out ONLY FFE taps if CWC is used, they share taps*/
		CWC_Length_u32 = (FFE_u8 == 1) ? BREG_ReadField(hChn->hRegister, DS_EQ_CWC, LENGTH) : 0;
		if (((CWC_Length_u32 == 4) && (count_u8>31)) || ((CWC_Length_u32 == 3) && (count_u8>32)) || ((CWC_Length_u32 == 2) && (count_u8>33)) || ((CWC_Length_u32 == 1) && (count_u8>34)))
		{
			ReadReg_u32 = (FFE_u8 == 1) ? 0 : ReadReg_u32;
		}
		*HAB_Buffer_pu8 = (uint8_t)((ReadReg_u32 >> 24) & 0x000000FF);
		HAB_Buffer_pu8++;
		*HAB_Buffer_pu8 = (uint8_t)((ReadReg_u32 >> 16) & 0x000000FF);
		HAB_Buffer_pu8++;
		*HAB_Buffer_pu8 = (uint8_t)((ReadReg_u32 >> 8 ) & 0x000000FF);
		HAB_Buffer_pu8++;
		*HAB_Buffer_pu8 = (uint8_t)(ReadReg_u32 & 0x000000FF);
		HAB_Buffer_pu8++;
	}

	/*Copy main Tap Location to end*/
	ReadReg_u32 = BREG_ReadField(hChn->hRegister, DS_EQ_FFE, MAIN);
	*HAB_Buffer_pu8 = (uint8_t)ReadReg_u32;


/*goto label to return error code if something bad happened above*/
something_bad_happened:
	return RetCode_u32;
}

/********************************************************************************************
 * Read Constellation                                                 
 ********************************************************************************************/
BERR_Code BADS_P_Read_Constellation(
    BADS_3x7x_ChannelHandle hChn,       /* [in] Device channel handle */
    int16_t nbrToGet_i16,               /* [in] Number values to get */
    int16_t *iVal_pi16,                  /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal_pi16,                  /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten_pi16             /* [out] Number of values gotten/read */
    )
{
	BERR_Code RetCode_u32 = BERR_SUCCESS;
	int16_t count_i16;
	int32_t ReadReg_u32;

#if (ADS_INTERNAL_ERROR_CHECKING > 1)
	/*Check that number of points requested is not negative*/
	if (nbrToGet_i16 < 0)
	{
		BDBG_ERR(("ERROR: REQUESTED CONSTELLATION POINTS < 0 in BADS_P_Read_Constellation()\n"));
		RetCode_u32 = BERR_INVALID_PARAMETER;
		goto something_bad_happened; /*goto bottom of function to leave early with error*/
	}
#endif

	/*Read constellation points from chip*/
    for (count_i16 = 0; count_i16 < nbrToGet_i16 ; count_i16++)
    {
        ReadReg_u32 = BREG_Read32(hChn->hRegister, BCHP_DS_EQ_SOFT);
        iVal_pi16[count_i16] = ((ReadReg_u32 & 0xffff0000) >> 16) ;
        qVal_pi16[count_i16] = (ReadReg_u32 & 0x0000ffff);
    }
    *nbrGotten_pi16 = nbrToGet_i16; 

#if (ADS_INTERNAL_ERROR_CHECKING > 0)
/*goto label to return error code if something bad happened above*/
something_bad_happened:
#endif	 

	return RetCode_u32;
}









