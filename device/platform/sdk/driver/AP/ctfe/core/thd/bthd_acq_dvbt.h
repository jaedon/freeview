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
* $brcm_Workfile: bthd_acq_dvbt.h $
* $brcm_Revision: 8 $
* $brcm_Date: 5/17/12 5:34p $
*
* [File Description:]
*
* Revision History:
*
* $brcm_Log: /AP/ctfe/core/thd/bthd_acq_dvbt.h $
* 
* 8   5/17/12 5:34p farshidf
* SW3461-211: FW release 5.0
* 
* Fw_Integration_Devel/3   5/17/12 3:55p farshidf
* SW3461-211: FW release 5.0
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/1   4/26/12 6:23p farshidf
* SW3461-193: add support for multichannel THD core
* 
* Fw_Integration_Devel/2   10/13/11 6:58p farshidf
* SW3461-64: merge to integ
* 
* Fw_Integration_Devel/AP_V3_0_THD_DEV/1   9/26/11 2:40p mbsingh
* SW3461-1: Merge from 2.0 dev branch (This is mainly a shift to WPO
*  algorithm)
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/3   9/14/11 4:58p mbsingh
* - Fix function naming
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/2   9/8/11 10:49p mbsingh
* SW3461-1:  Separated SetFW function cleanly now to make DVBT work for
*  ISDBT containing chips also.  In initial merged code from THD_FTT_WPO
*  branch DVBT would work only in non ISDBT chips due to wrong SetFW
*  function selection
* 
* 6   8/24/11 5:55p farshidf
* SW7552-60: merge to main
* 
* Fw_Integration_Devel/1   8/24/11 5:48p farshidf
* SW7552-60: add the isdb-t constellation
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/1   8/24/11 5:41p farshidf
* SW7552-60: add the softdecision for isdbt
* 
* 5   6/8/11 6:24p farshidf
* SW3461-1: merge main
* 
* Hydra_Software_Devel/2   5/5/11 12:31p farshidf
* SWDTV-6857: latest DVB-T code from 3461 label FW_3461_A0_05032011_1344
* 
* 4   5/3/11 1:37p mpovich
* SW3461-1: Merge to main of intermediate label, FW_3461_A0_05022011_1100
*  (05/02/2011 Pace build).
* 
* DVB_3461_1/2   4/21/11 6:05p farshidf
* SW3461-1: merge main
* 
* 3   4/20/11 4:56p farshidf
* SW3461-1: make it host compatible
* 
* 2   4/8/11 3:11p farshidf
* SW3461-1: main merge
* 
* DVB_3461_1/1   4/7/11 10:59p jputnam
* SW3461-1: Removed obsolete lock functions.  Reverted state machine so
*  that SP, TPS, and FEC states are revisited upon FFTWindowMode change.
*  Force RS_SYNC_LOSS interrupt if P_Acquire returns without locking to
*  force proper update of lock state.
* 
* 1   1/6/11 10:03a jputnam
* SW3461-1: Separated DVB-T specific functions into dedicated file
* 
* 
***************************************************************************/
#ifndef _BTHD_ACQ_DVBT_H__
#define _BTHD_ACQ_DVBT_H__

#if __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Function prototypes
 ***************************************************************************/
void BTHD_P_DvbtSetMode(BTHD_3x7x_ChnHandle,THD_TransmissionMode_t,THD_GuardInterval_t,THD_Qam_t,THD_CodeRate_t,THD_CodeRate_t,THD_DvbtHierarchy_t);
BTHD_RESULT BTHD_P_DvbtSetTPS(BTHD_3x7x_ChnHandle,THD_TransmissionMode_t,THD_GuardInterval_t);
void BTHD_P_DvbtSetOI(BTHD_3x7x_ChnHandle);
void BTHD_P_DvbtSetEq(BTHD_3x7x_ChnHandle,THD_CoChannelMode_t);
void BTHD_P_DvbtSetViterbi(BTHD_3x7x_ChnHandle,THD_CodeRate_t,THD_CodeRate_t);
BTHD_RESULT BTHD_P_DvbtSetFEC(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_DvbtGetNotch(BTHD_3x7x_ChnHandle,THD_TransmissionMode_t);
BTHD_RESULT BTHD_P_DvbtSetICE(BTHD_3x7x_ChnHandle,THD_TransmissionMode_t,THD_GuardInterval_t);
void BTHD_P_DvbtSetFrame(BTHD_3x7x_ChnHandle);
void BTHD_P_DvbtStatus(BTHD_3x7x_ChnHandle) ;
void BTHD_P_DvbtResetStatus(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_DvbtAcquire(BTHD_3x7x_ChnHandle);
BERR_Code BTHD_P_GetDvbtSoftDecisionBuf(BTHD_3x7x_ChnHandle, int16_t, int16_t *, int16_t *, int16_t *); 
void BTHD_P_DvbtSetFWFtt( BTHD_3x7x_ChnHandle,THD_FFTWindowMode_t,THD_TransmissionMode_t,THD_GuardInterval_t);

#ifdef __cplusplus
}
#endif

#endif /* BTHD_ACQ_DVBT_H__ */

