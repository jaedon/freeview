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
* $brcm_Workfile: bthd_acq_isdbt.h $
* $brcm_Revision: 7 $
* $brcm_Date: 8/29/12 11:40a $
*
* [File Description:]
*
* Revision History:
*
* $brcm_Log: /AP/ctfe/core/thd/bthd_acq_isdbt.h $
* 
* 7   8/29/12 11:40a farshidf
* SW7552-336: merge to main
* 
* Fw_Integration_Devel/6   8/29/12 11:39a farshidf
* SW7552-336: merge to main
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/2   8/21/12 5:13p mpovich
* SW3472-6: Merge to 5_0 Dev branch.
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/3   7/26/12 5:48p jchien
* SW3472-6: fix WPO, fix FW_WIN, turn on condition
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/2   7/17/12 3:22p jchien
* SW7552-305: FFTW_sync update
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/1   7/10/12 5:31p farshidf
* SW3472-6: add the ISDB-T Api changes
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/1   4/26/12 6:24p farshidf
* SW3461-193: add support for multichannel THD core
* 
* Fw_Integration_Devel/4   10/13/11 6:58p farshidf
* SW3461-64: merge to integ
* 
* Fw_Integration_Devel/AP_V3_0_THD_DEV/1   9/26/11 2:40p mbsingh
* SW3461-1: Merge from 2.0 dev branch (This is mainly a shift to WPO
*  algorithm)
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/2   9/8/11 10:50p mbsingh
* SW3461-1:  Separated SetFW function cleanly now to make DVBT work for
*  ISDBT containing chips also.  In initial merged code from THD_FTT_WPO
*  branch DVBT would work only in non ISDBT chips due to wrong SetFW
*  function selection
* 
* 4   8/24/11 5:55p farshidf
* SW7552-60: merge to main
* 
* Fw_Integration_Devel/3   8/24/11 5:48p farshidf
* SW7552-60: add the isdb-t constellation
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/1   8/24/11 5:41p farshidf
* SW7552-60: add the softdecision for isdbt
* 
* 3   8/12/11 3:14p farshidf
* SW3461-1: merge to main
* 
* Fw_Integration_Devel/2   8/12/11 2:34p farshidf
* SW3461-1: merge to main
* 
* 2   7/20/11 12:11p farshidf
* SW7552-60: merge to main
* 
* Fw_Integration_Devel/1   7/20/11 12:08p farshidf
* SW7552-60: latest ISDB-T
* 
* Fw_Integration_Devel/AP_V0_6_THD_DEV/1   7/12/11 4:06p jchien
* AP_V0_6_THD_DEV: add IsdbtSetRsRt &IsdbtResetLockSetClrFlag
* 
* 1   6/8/11 5:42p farshidf
* SW3461-1: merge main
* 
* Hydra_Software_Devel/2   5/31/11 7:37p farshidf
* SW7552-36: remove ISDB-T warning
* 
* Hydra_Software_Devel/1   5/16/11 5:18p farshidf
* SWDTV-7035: merge to main
* 
* Hydra_Software_Devel/SWDTV-6857/1   5/3/11 4:21p jchien
* SWDTV-6857: initial check-in
* 
* 1   4/11/11 jchien - initial checkin
* SW3461-1: 
* 
* 
***************************************************************************/
#ifndef _BTHD_ACQ_ISDBT_H__
#define _BTHD_ACQ_ISDBT_H__

#if __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Function prototypes
 ***************************************************************************/

BTHD_RESULT BTHD_P_IsdbtSetTMCC(BTHD_3x7x_ChnHandle,THD_TransmissionMode_t,THD_GuardInterval_t);
void BTHD_P_IsdbtSetOI(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_IsdbtSetFEC(BTHD_3x7x_ChnHandle);
BTHD_RESULT BTHD_P_IsdbtGetNotch(BTHD_3x7x_ChnHandle,THD_TransmissionMode_t);
BTHD_RESULT BTHD_P_IsdbtSetICE(BTHD_3x7x_ChnHandle,THD_TransmissionMode_t,THD_GuardInterval_t);
void BTHD_P_IsdbtStatus(BTHD_3x7x_ChnHandle) ;
void BTHD_P_IsdbtResetStatus(BTHD_3x7x_ChnHandle);
void BTHD_P_IsdbtResetLockSetClrFlag(BTHD_3x7x_ChnHandle);
void BTHD_P_IsdbtSetRsRt(BTHD_3x7x_ChnHandle, uint32_t, uint32_t);
BTHD_RESULT BTHD_P_IsdbtAcquire(BTHD_3x7x_ChnHandle);
BERR_Code BTHD_P_GetIsdbtSoftDecisionBuf(BTHD_3x7x_ChnHandle, int16_t, int16_t *, int16_t *, int16_t *); 
void BTHD_P_IsdbtSetFWFtt( BTHD_3x7x_ChnHandle,THD_FFTWindowMode_t,THD_TransmissionMode_t,THD_GuardInterval_t);
void BTHD_P_SetFW( BTHD_3x7x_ChnHandle,THD_FFTWindowMode_t,THD_TransmissionMode_t,THD_GuardInterval_t);

#ifdef __cplusplus
}
#endif

#endif /* BTHD_ACQ_ISDBT_H__ */

