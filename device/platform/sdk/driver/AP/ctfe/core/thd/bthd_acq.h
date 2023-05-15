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
 * $brcm_Workfile: bthd_acq.h $
 * $brcm_Revision: 30 $
 * $brcm_Date: 8/29/12 11:38a $
 *
 * [File Description:]
 *
 * Revision History:
 *
* $brcm_Log: /AP/ctfe/core/thd/bthd_acq.h $
* 
* 30   8/29/12 11:38a farshidf
* SW7552-336: merge to main
* 
* Fw_Integration_Devel/7   8/29/12 11:38a farshidf
* SW7552-336: merge to main
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/2   8/21/12 5:12p mpovich
* SW3472-6: Merge to 5_0 Dev branch.
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/1   7/27/12 1:18p mpovich
* SW3472-6: Update Channel Estimators and Channel Coefficients for ISDB-
*  T.
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/1   4/26/12 6:23p farshidf
* SW3461-193: add support for multichannel THD core
* 
* Fw_Integration_Devel/5   2/8/12 5:11p farshidf
* SW3461-1: remove warning
* 
* 27   2/8/12 5:10p farshidf
* SW3461-1: remove warning
* 
* 26   2/8/12 2:51p farshidf
* SW3461-1: remove warning
* 
* Fw_Integration_Devel/4   2/8/12 2:47p farshidf
* SW3461-1: remove warning
* 
* Fw_Integration_Devel/3   1/20/12 6:30p farshidf
* SW3461-1: merge to integ
* 
* Fw_Integration_Devel/AP_V3_0_THD_DEV/2   1/6/12 1:54p farshidf
* SW3461-40: merge to Dev branch
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
* Fw_Integration_Devel/AP_V3_0_THD_DEV/1   9/26/11 2:40p mbsingh
* SW3461-1: Merge from 2.0 dev branch (This is mainly a shift to WPO
*  algorithm)
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/4   9/9/11 6:08p mbsingh
* SW3461-1:  - Added back AcquirechangeFFTwindow function (needed by
*  ISDB-T)
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/3   9/8/11 10:48p mbsingh
* SW3461-1:  Separated SetFW function cleanly now to make DVBT work for
*  ISDBT containing chips also.  In initial merged code from THD_FTT_WPO
*  branch DVBT would work only in non ISDBT chips due to wrong SetFW
*  function selection
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/2   9/8/11 10:05p mbsingh
* SW3461-1: Merging from THD_FTT_WPO branch                   -
*  Architectural change to new FTT algo for acquisition
* 
* Fw_Integration_Devel/AP_V0_6_THD_DEV/Thd_ftt_wpo/1   8/18/11 10:50a hzou
* Ported changes for FTT and WPO
* 
* 23   8/24/11 5:54p farshidf
* SW7552-60: merge to main
* 
* Fw_Integration_Devel/1   8/24/11 5:47p farshidf
* SW7552-60: add the isdb-t constellation
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/1   8/24/11 5:40p farshidf
* SW7552-60: add the softdecision for isdbt
* 
* 22   6/8/11 6:25p farshidf
* SW3461-1: merge main
* 
* Dvb_Fw_Devel_4/1   6/8/11 5:20p farshidf
* SW3461-1: sync up with backend chip
* 
* Hydra_Software_Devel/2   5/27/11 10:51a farshidf
* SW7552-36: compiel fix
* 
* 20   4/21/11 6:21p farshidf
* SW3461-1: merge main
* 
* 19   4/21/11 5:47p farshidf
* SW3461-1: merge main
* 
* DVB_3461_1/5   4/21/11 5:46p farshidf
* SW3461-1: merge main
* 
* DVB_3461_1/4   4/15/11 9:37p jputnam
* SW3461-1: Firmware FFT trigger positioning functional. Feature can be
*  disabled by removing #define FFTTriggerPosition in bthd_acq.h
* 
* DVB_3461_1/3   4/12/11 10:25a jputnam
* SW3461-1: Added P_GetChannelEstimateBuf() and
*  P_GetInterpolatorCoefficientsBuf() functions. Reduced FI leakage to
*  2^(-20)
* 
* DVB_3461_1/2   4/11/11 9:16a jputnam
* SW3461-1: Functional CoChannel mode
* 
* DVB_3461_1/1   4/7/11 9:29p jputnam
* SW3461-1: Initial version of interrupt-based lock detection
* 
* 14   3/9/11 3:44p jputnam
* SW3461-1: Moved status copy from BTHD_P_Status() up to BTHD_Status()
* 
* 13   3/4/11 4:52p farshidf
* SW3461-1: add the power seq
* 
* 12   1/21/11 5:57p farshidf
* SW3461-1: update names
* 
* 11   1/6/11 10:09a jputnam
* SW3461-1: Major cosmetic overhaul - Conform to new API conventions
*  common to all demods - Conform to naming conventions for private
*  functions and typedefs - Separate DVB-T and ISDB-T params, status,
*  functions
* 
* 10   1/3/11 5:36p jputnam
* SW3461-1: changed enumerated type and #define names from THD_ to BTHD_
* 
* 9   12/16/10 6:48p farshidf
* SW3461-1: clean up
* 
* 8   11/30/10 10:49a farshidf
* SW3461-1: update the thd files
* 
* 7   11/16/10 6:35p farshidf
* SW3461-1: clean up
* 
* 6   11/4/10 2:31p farshidf
* SW3461-1: clean up the irq
* 
* 5   11/4/10 10:07a jputnam
* SW3461-1: Separated Acquire() function into multiple functions called
*  by a new top-level acquisition state machine
* 
* 4   9/23/10 5:44p farshidf
* SW3461-1: add the hab request command
* 
* 3   9/3/10 5:56p farshidf
* SW3461-1: bug fix
* 
* 2   8/27/10 4:42p farshidf
* SW3461-1: fix compile issues
* 
* 1   8/17/10 5:12p farshidf
* SW3461-1: initial THD code
 * 
 * 
 ***************************************************************************/
#ifndef _BTHD_ACQ_H__
#define _BTHD_ACQ_H__

#if __cplusplus
extern "C" {
#endif

/* #define ChangeFFTWindowSeamless */
#define BTHD_CHK_RETCODE(x) \
  { if ((retCode = (x)) != BERR_SUCCESS) goto done; }

typedef uint32_t BTHD_RESULT;

/***************************************************************************
 * Register Field Combinations Definition
 ***************************************************************************/
#define BCHP_THD_CORE_EQ_EXP_USE_FORMAT_OV_SHIFT BCHP_THD_CORE_EQ_FORMAT_OV_SHIFT 
#define BCHP_THD_CORE_EQ_EXP_USE_FORMAT_OV_MASK  (BCHP_THD_CORE_EQ_FORMAT_OV_MASK | BCHP_THD_CORE_EQ_USE_FORMAT_OV_MASK) 

#define BCHP_THD_CORE_BYP_EXP_AVG_NSE_SHIFT BCHP_THD_CORE_BYP_EXP_NSE_BYP_SHIFT 
#define BCHP_THD_CORE_BYP_EXP_AVG_NSE_MASK  (BCHP_THD_CORE_BYP_EXP_NSE_BYP_MASK | BCHP_THD_CORE_BYP_EXP_AVG_NSE_BYP_MASK) 

#define BCHP_THD_CORE_CE_ACE_ACT_SHIFT BCHP_THD_CORE_CE_ACE_SHIFT 
#define BCHP_THD_CORE_CE_ACE_ACT_MASK  (BCHP_THD_CORE_CE_ACE_MASK | BCHP_THD_CORE_CE_ACT_MASK)

#define BCHP_THD_CORE_RST_RS_FEC_BCH_TPS_SNR_RST_SHIFT BCHP_THD_CORE_RST_SNR_RST_SHIFT 
#define BCHP_THD_CORE_RST_RS_FEC_BCH_TPS_SNR_RST_MASK  (BCHP_THD_CORE_RST_SNR_RST_MASK | BCHP_THD_CORE_RST_TPS_RST_MASK  | BCHP_THD_CORE_RST_TPS_SAVE_RST_MASK | BCHP_THD_CORE_RST_BCH_ERC_RST_MASK | BCHP_THD_CORE_RST_FEC_RST_MASK | BCHP_THD_CORE_RST_RS_RST_MASK) 

#define BCHP_THD_CORE_FRZ_NOTCH_FRZ_SHIFT BCHP_THD_CORE_FRZ_NOTCH0M_FRZ_SHIFT 
#define BCHP_THD_CORE_FRZ_NOTCH_FRZ_MASK  (BCHP_THD_CORE_FRZ_NOTCH0M_FRZ_MASK | BCHP_THD_CORE_FRZ_NOTCH0_FRZ_MASK  | BCHP_THD_CORE_FRZ_NOTCH1_FRZ_MASK | BCHP_THD_CORE_FRZ_NOTCH2_FRZ_MASK | BCHP_THD_CORE_FRZ_NOTCH3_FRZ_MASK | BCHP_THD_CORE_FRZ_NOTCH4_FRZ_MASK | BCHP_THD_CORE_FRZ_NOTCH_DDFS_FRZ_MASK)

#define BCHP_THD_CORE_TPS_OV_RX_INFO_SHIFT BCHP_THD_CORE_TPS_OV_QAM_SHIFT
#define BCHP_THD_CORE_TPS_OV_RX_INFO_MASK  (BCHP_THD_CORE_TPS_OV_QAM_MASK | BCHP_THD_CORE_TPS_OV_reserved5_MASK  | BCHP_THD_CORE_TPS_OV_HIERARCHY_MASK | BCHP_THD_CORE_TPS_OV_INDEPTH_MASK | BCHP_THD_CORE_TPS_OV_reserved4_MASK | BCHP_THD_CORE_TPS_OV_CRATE_HP_MASK)

#define BCHP_THD_CORE_TPS_RX_INFO_SHIFT BCHP_THD_CORE_TPS_QAM_SHIFT
#define BCHP_THD_CORE_TPS_RX_INFO_MASK  (BCHP_THD_CORE_TPS_QAM_MASK | BCHP_THD_CORE_TPS_reserved5_MASK  | BCHP_THD_CORE_TPS_HIERARCHY_MASK | BCHP_THD_CORE_TPS_INDEPTH_MASK | BCHP_THD_CORE_TPS_reserved4_MASK | BCHP_THD_CORE_TPS_CRATE_HP_MASK)

#define BCHP_THD_CORE_BYP_EQ_BYP_SHIFT BCHP_THD_CORE_BYP_EQ_BIT_SCALING_BYP_SHIFT
#define BCHP_THD_CORE_BYP_EQ_BYP_MASK  (BCHP_THD_CORE_BYP_EQ_BIT_SCALING_BYP_MASK | BCHP_THD_CORE_BYP_NSE_AVG_BYP_MASK  | BCHP_THD_CORE_BYP_EXP_CHAN_BYP_MASK | BCHP_THD_CORE_BYP_EXP_NSE_BYP_MASK | BCHP_THD_CORE_BYP_EXP_AVG_NSE_BYP_MASK) 

/* Implementation of API functions */

    
/* Internal THD Functions */
BERR_Code BTHD_P_GetChannelEstimateBuf(BTHD_3x7x_ChnHandle, uint32_t *, THD_SnooperMode_t, uint32_t uDataLenOutputMax, uint32_t *puDataLenOutput );
BERR_Code BTHD_P_GetInterpolatorCoefficientsBuf(BTHD_3x7x_ChnHandle, uint32_t *, THD_InterpolatorMode_t, uint32_t uDataLenOutputMax, uint32_t *puDataLenOutput );
BERR_Code BTHD_P_OSleep(BTHD_3x7x_ChnHandle,uint32_t,uint32_t,uint32_t);
void BTHD_P_UpdateStatusChange(BTHD_3x7x_ChnHandle,uint32_t);
BERR_Code BTHD_P_Init(BTHD_3x7x_ChnHandle);
BERR_Code BTHD_P_Acquire(BTHD_3x7x_ChnHandle);
void BTHD_P_SetTs(BTHD_3x7x_ChnHandle);
void BTHD_P_SetFrequency(BTHD_3x7x_ChnHandle);
void BTHD_P_SetFrontEnd(BTHD_3x7x_ChnHandle);
void BTHD_P_SetNotch(BTHD_3x7x_ChnHandle);
THD_TransGuardResult_t BTHD_P_SetTransGuard(BTHD_3x7x_ChnHandle);

void BTHD_P_SetCE( BTHD_3x7x_ChnHandle,THD_ChannelEstimatorMode_t);
void BTHD_P_WriteFICoef( BTHD_3x7x_ChnHandle,THD_FrequencyInterpolatorMode_t);
void BTHD_P_Status(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_Lock(BTHD_3x7x_ChnHandle);
void BTHD_P_ResetAll(BTHD_3x7x_ChnHandle);
void BTHD_P_ResetAcquire(BTHD_3x7x_ChnHandle);
void BTHD_P_ResetStatus(BTHD_3x7x_ChnHandle);
void BTHD_P_ResetStatusHW(BTHD_3x7x_ChnHandle);
void BTHD_P_Config(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_AcquireInit(BTHD_3x7x_ChnHandle,THD_FFTWindowMode_t);
BTHD_RESULT BTHD_P_AcquireSP(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_AcquireFFTTrigger(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_AcquireTPS(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_AcquireFEC(BTHD_3x7x_ChnHandle,bool);
BTHD_RESULT BTHD_P_AcquireChangeFFTWindow(BTHD_3x7x_ChnHandle,THD_FFTWindowMode_t);
BTHD_RESULT BTHD_P_AcquireTrack(BTHD_3x7x_ChnHandle,THD_FFTWindowMode_t,THD_ChannelEstimatorMode_t);
BTHD_RESULT BTHD_P_AcquireCheckLock(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_AcquireChangeChannelEstimator(BTHD_3x7x_ChnHandle,THD_ChannelEstimatorMode_t);
BTHD_RESULT BTHD_P_MapWaitForEventResult_To_THD_AcqResult ( BERR_Code eEventWaitResult, BTHD_RESULT successCode, BTHD_RESULT failureCode );
bool BTHD_P_Wait_CE_Recording_Status_Done ( BTHD_3x7x_ChnHandle h, uint32_t uTimeOutMsec );
void BTHD_P_AbortAcquire ( BTHD_3x7x_ChnHandle h) ;

/* Sets the Abort-Early event/condition during an acquisition */
void BTHD_P_AbortAcquire ( BTHD_3x7x_ChnHandle h );
/* Resets the Abort-Early event/condition during an acquisition */
void BTHD_P_AbortAcquireReset ( BTHD_3x7x_ChnHandle h );
/* Queries an aquisition's Abort-Early event/condition */
bool BTHD_P_IsAbortAcquireRequested ( BTHD_3x7x_ChnHandle h );
/* Wait for an event (or Abort-Early) during an acquisition */
BERR_Code BTHD_P_WaitForEventOrAbort ( BTHD_3x7x_ChnHandle h, BKNI_EventHandle event, int timeoutMsec );
/* Acquire Aborted Early BERR_Code */
#define BTHD_ERR_ACQUIRE_ABORTED  BERR_MAKE_CODE(BERR_THD_ID, 1)

#ifdef __cplusplus
}
#endif

#endif /* BTHD_ACQ_H__ */

