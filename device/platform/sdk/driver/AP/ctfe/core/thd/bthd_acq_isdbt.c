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
* $brcm_Workfile: bthd_acq_isdbt.c $
* $brcm_Revision: 17 $
* $brcm_Date: 10/18/12 6:01p $
*
* [File Description:]
*
* Revision History:
*
* $brcm_Log: /AP/ctfe/core/thd/bthd_acq_isdbt.c $
* 
* 17   10/18/12 6:01p farshidf
* SW3461-287: create 5.3.1.1
* 
* Fw_Integration_Devel/13   10/18/12 5:20p farshidf
* SW3461-287: Create Release 5.3.1.1
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/10   10/12/12 5:23p mpovich
* SW3472-17: Merge to Sys dev branch.
* 
* SW3472-17/1   9/12/12 6:24p jchien
* SW3472-16: Add SSI and SQI to ISDBT status.
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/7   9/12/12 6:06p jchien
* SW3472-16: Integer fractional frequency fix.
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/6   8/17/12 11:33a jchien
* SW3472-6: Remove debug code
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/5   7/30/12 10:28a mpovich
* SW3472-6: Rebase with SYS_DEV branch.
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/4   7/26/12 5:48p jchien
* SW3472-6: fix WPO, fix FW_WIN, turn on condition
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/3   7/17/12 3:13p jchien
* SW7552-305: FFTW_Sync update and differntiel mode detection
* 
* 15   8/29/12 11:37a farshidf
* SW7552-336: merge to main
* 
* Fw_Integration_Devel/12   8/29/12 11:36a farshidf
* SW7552-336: merge to main
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/9   8/21/12 5:13p mpovich
* SW3472-6: Merge to 5_0 Dev branch.
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/9   8/21/12 5:13p mpovich
* SW3472-6: Merge to 5_0 Dev branch.
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/6   8/17/12 11:33a jchien
* SW3472-6: Remove debug code
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/5   7/30/12 10:28a mpovich
* SW3472-6: Rebase with SYS_DEV branch.
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/4   7/26/12 5:48p jchien
* SW3472-6: fix WPO, fix FW_WIN, turn on condition
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/3   7/17/12 3:13p jchien
* SW7552-305: FFTW_Sync update and differntiel mode detection
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/8   7/12/12 3:24p farshidf
* SW7552-305: merge to 3472
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/2   7/12/12 3:23p farshidf
* SW7552-305: merge to 3472
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/7   7/12/12 3:19p farshidf
* SW7552-305: improve Acqusition time
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/6   6/20/12 5:23p farshidf
* SW3472-5: fix compile issue
* 
* 11   6/20/12 4:36p farshidf
* SW3472-5: merge to main/integ
* 
* Fw_Integration_Devel/9   6/20/12 4:36p farshidf
* SW3472-5: merge to main/integ
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/5   6/20/12 3:14p farshidf
* SW3472-5:  Add mobile channel estimation mode � Fix re-acquisition
*  count � improved Doppler performance � Improved AWGN performance �
*  Integrated ISDBT WPO for coherent mode
* 
* 10   5/24/12 11:28a farshidf
* SW3461-1: merge to main
* 
* Fw_Integration_Devel/8   5/24/12 11:27a farshidf
* SW3461-1: merge to main
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/4   5/22/12 5:38p farshidf
* SW3461-211: make the isdbt code to compile for 3461
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/3   5/22/12 5:19p farshidf
* SW3461-211: make Isdbt to build on 5 branch for 7552
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/2   5/21/12 6:51p farshidf
* SW3461-213: Make the Tuner Branch 5.0 to compile for 7552
* 
* Fw_Integration_Devel/AP_V5_0_THD_DEV/1   4/26/12 6:23p farshidf
* SW3461-193: add support for multichannel THD core
* 
* Fw_Integration_Devel/6   10/13/11 6:58p farshidf
* SW3461-64: merge to integ
* 
* Fw_Integration_Devel/AP_V3_0_THD_DEV/1   9/26/11 2:40p mbsingh
* SW3461-1: Merge from 2.0 dev branch (This is mainly a shift to WPO
*  algorithm)
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
* Fw_Integration_Devel/5   8/24/11 5:48p farshidf
* SW7552-60: add the isdb-t constellation
* 
* Fw_Integration_Devel/AP_V2_0_THD_DEV/1   8/24/11 5:41p farshidf
* SW7552-60: add the softdecision for isdbt
* 
* 5   8/24/11 12:33p farshidf
* SW3461-38: merge to main
* 
* Fw_Integration_Devel/4   8/17/11 4:06p farshidf
* SW3461-1: update the printf
* 
* 4   8/17/11 4:04p farshidf
* SW3461-1: update the printf
* 
* 3   7/20/11 12:38p farshidf
* SW7552-60: warning fix
* 
* 2   7/20/11 12:11p farshidf
* SW7552-60: merge to main
* 
* Fw_Integration_Devel/2   7/20/11 12:07p farshidf
* SW7552-60: latest ISDB-T
* 
* Fw_Integration_Devel/AP_V0_6_THD_DEV/1   7/12/11 3:39p jchien
* AP_V0_6_THD_DEV: add ResetLockSetClrFlag and SetRsRt
* 
* Fw_Integration_Devel/1   6/29/11 12:39p farshidf
* SW3461-13: merge to integration branch
* 
* Fw_Integration_Devel/Dvbt_Fw_Devel_Rc05/1   6/14/11 6:44p jchien
* DVBT_FW_Devel_Rc05: fix a bug introduced during code port in
*  state_track.  add seamless to speed up acquisition
* 
* 1   6/8/11 5:41p farshidf
* SW3461-1: merge main
* 
* Hydra_Software_Devel/4   6/1/11 11:49a farshidf
* SW7552-36: clean up
* 
* Hydra_Software_Devel/3   5/31/11 7:37p farshidf
* SW7552-36: remove ISDB-T warning
* 
* Hydra_Software_Devel/2   5/24/11 1:56p farshidf
* SWDTV-7146: merge to main
* 
* Hydra_Software_Devel/SWDTV-7146/2   5/24/11 12:22p jchien
* SWDTV-7146: add ISDBT RS_SYNC monitor for three layers
* 
* Hydra_Software_Devel/SWDTV-7146/1   5/17/11 4:41p jchien
* SWDTV-7146: fix isdbt status
* 
* Hydra_Software_Devel/1   5/16/11 5:18p farshidf
* SWDTV-7035: merge to main
* 
* Hydra_Software_Devel/SWDTV-6857/1   5/3/11 4:20p jchien
* SWDTV-6857: initial check-in
* 
* 1   3/4/11 5:30p jchien
* SW3461-1: Initial check-in
* 
* 
***************************************************************************/
#include "bstd.h"
#include "bmth.h"
#include "bkni.h"
#include "btmr.h"

#ifndef LEAP_BASED_CODE
#include "bthd_3x7x.h"
#endif

#include "bchp_thd_core.h"
#include "bthd_api.h"
#include "bthd_acq.h"
#include "bthd_acq_isdbt.h"
#include "bthd_coef.h"

#include "bchp_thd_intr2.h"
#include "bchp_thd_intr2b.h"

#ifdef LEAP_BASED_CODE
#include "bchp_leap_ctrl.h"
#include "bthd_irq.h"
#endif

#ifndef LEAP_BASED_CODE
BDBG_MODULE(bthd_acq_isdbt);
#endif

#ifdef BTHD_ISDBT_SUPPORT

/***************************************************************************
 * BTHD_P_IsdbtSetFWFtt()
 ***************************************************************************/
void BTHD_P_IsdbtSetFWFtt( BTHD_3x7x_ChnHandle h, 
		   THD_FFTWindowMode_t FFTWindowMode, 
		   THD_TransmissionMode_t TransmissionMode, 
		   THD_GuardInterval_t GuardInterval)
{

  uint32_t fw=0,fw_search=0,N=bthd_transmission_mode[TransmissionMode];
  BSTD_UNUSED(FFTWindowMode);
  BSTD_UNUSED(GuardInterval);

  /* First Peak */
  fw_search = 0x11202000 | (8 * (N/2048));  /* FFT window min_scale=1.125 (0.5dB),start_index_mode=1,1st peak mode,L=8*N/2048 */
  /* fw_search = 0x11802000 | (8 * (N/2048)); */  /* FFT window min_scale=1.5,start_index_mode=1,1st peak mode,L=8*N/2048*/
  /*fw = 0x03214050; */  /* Decrease average beta from 2^(-7) to 2^(-6) for freq-domain trigger position algorithm for faster convergence time */
  fw = 0x03214051; /* Decrease average beta from 2^(-7) to 2^(-6) for freq-domain trigger position algorithm for faster convergence time, enable slip */

  /* -HZ, re-enabled for ISDB-T GI=1/16 tests, Disable expanded search mode for GI=1/4 due to acquisition instability */
  if (GuardInterval == THD_GuardInterval_1_4) 
    fw = fw & 0xfcffffff;

#ifdef FASTACQ
  fw = (fw & 0xffff00ff) | 0x00002000;
#endif
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW,fw );
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_SEARCH,fw_search);
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_OFFSET,0x00000010 );  /* FFT window offset=16 */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_SPAN,0x00000200 );      /* FFT window span scale = 0.25 */
}



/***************************************************************************
 * BTHD_P_SetFW()
 ***************************************************************************/
void BTHD_P_SetFW( BTHD_3x7x_ChnHandle h, 
		   THD_FFTWindowMode_t FFTWindowMode, 
		   THD_TransmissionMode_t TransmissionMode, 
		   THD_GuardInterval_t GuardInterval)
{
  uint32_t fw= 0,fw_search=0,N=bthd_transmission_mode[TransmissionMode];

  switch (FFTWindowMode) {
  case THD_FFTWindowMode_InSpan:
    /*fw_search = 0x11102000 | (8 * (N/2048)); */ /* FFT window min_scale=1.0625,start_index_mode=1,1st peak mode,L=8*N/2048 */
    fw_search = 0x11802000 | (8 * (N/2048));  /* FFT window min_scale=1.5,start_index_mode=1,1st peak mode,L=8*N/2048 */
    fw = 0x03214071;
    break;
  case THD_FFTWindowMode_OutOfSpanPost:  
    fw_search = 0x11400000 | (8 * (N/2048));  /* FFT window min_scale=1.25,start_index_mode=0,1st peak mode,L=8*N/2048 */
    fw = 0x03014071;      
    break;
  case THD_FFTWindowMode_OutOfSpanPre:  
    fw_search = 0x11402000 | (8 * (N/2048));  /* FFT window min_scale=1.5,start_index_mode=1,1st peak mode,L=8*N/2048 */    
    fw = 0x03014071;
    break;
  default:
    BDBG_WRN(("BTHD_SetFW: THD_FFTWindowMode_Auto not supported")); 
    break;
  } 

  /* Disable expanded search mode for GI=1/4 due to acquisition instability */
  /* if (GuardInterval == 3) */
  fw = fw & 0xfcffffff;

#ifdef FASTACQ
  fw = (fw & 0xffff00ff) | 0x00002000;
#endif
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW,fw );
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_SEARCH,fw_search);
  if (FFTWindowMode == THD_FFTWindowMode_OutOfSpanPre) {
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_OFFSET,0x00000000 );  /* FFT window offset=0 */
  } else {
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_OFFSET,0x00000010 );  /* FFT window offset=16 */
  }
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_MISC,0x0000020a );      /* FFT window leak to timing loop disabled, disallow zeros */
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_FW_SPAN,0x00000200 );      /* FFT window span scale = 0.25 */

  /* Setup frequency interpolator coefficients for out-of-span conditions */
  if (FFTWindowMode != THD_FFTWindowMode_InSpan) {
    if (FFTWindowMode == THD_FFTWindowMode_OutOfSpanPre) {
      if (GuardInterval == THD_GuardInterval_1_4)
        BTHD_P_WriteFICoef(h,THD_FrequencyInterpolatorMode_OutOfSpanPre_1_4);
      else if (GuardInterval == THD_GuardInterval_1_8)
        BTHD_P_WriteFICoef(h,THD_FrequencyInterpolatorMode_OutOfSpanPre_1_8);
      else
        BTHD_P_WriteFICoef(h,THD_FrequencyInterpolatorMode_OutOfSpanPre_1_16);
    }
    else
      BTHD_P_WriteFICoef(h,THD_FrequencyInterpolatorMode_OutOfSpanPost);
  }
  
  /*
  switch (FFTWindowMode) {
  case BTHD_FFTWindowMode_InSpan:        BDBG_MSG(("BTHD_P_SetFW:\t\tIn-Span")); break;
  case BTHD_FFTWindowMode_OutOfSpanPost: BDBG_MSG(("BTHD_P_SetFW:\t\tOut-of-Span Postcursor")); break;
  case BTHD_FFTWindowMode_OutOfSpanPre:  BDBG_MSG(("BTHD_P_SetFW:\t\tOut-of-Span Precursor")); break;
  default:                              BDBG_WRN(("BTHD_P_SetFW: THD_FFTWindowMode_Auto not supported")); break;
  }
  */
}

/******************************************************************************
 * BTHD_P_GetIsdbtSoftDecisionBuf()
 ******************************************************************************/
BERR_Code BTHD_P_GetIsdbtSoftDecisionBuf(BTHD_3x7x_ChnHandle h,  /* [in] BTHD handle */
    int16_t nbrToGet,                 /* [in] Number values to get */
    int16_t *pI,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *pQ,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                /* [out] Number of values gotten/read */
    )
{

  uint32_t softValue, layerType;
  uint8_t idx=0;
  uint32_t layerAQam = bthd_transmission_mode_isdbt[BREG_ReadField(h->hRegister, THD_CORE_TMCC_LAYER_0, LAYERA_QAM )];
  uint32_t layerBQam = bthd_transmission_mode_isdbt[BREG_ReadField(h->hRegister, THD_CORE_TMCC_LAYER_1, LAYERB_QAM )];
  uint32_t layerCQam = bthd_transmission_mode_isdbt[BREG_ReadField(h->hRegister, THD_CORE_TMCC_LAYER_1, LAYERC_QAM )];
  uint32_t N = bthd_transmission_mode[BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE )];
  uint32_t GuardInterval = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, GUARD);

  BSTD_UNUSED(nbrGotten);
  BSTD_UNUSED(nbrToGet);
  

  while (idx < 30) {
    softValue = BREG_Read32(h->hRegister, BCHP_THD_CORE_SOFT_READ_DATA);
	layerType  = ((softValue >> 2) & 0x3);
    pI[idx] = (int)((softValue >> 18) & 0x3FFF);
    pQ[idx] = (int)((softValue >> 4) & 0x3FFF);

    if ((pI[idx] & 0x2000) == 0x2000)
        pI[idx] -= 0x4000;
    if ((pQ[idx] & 0x2000) == 0x2000)
         pQ[idx] -= 0x4000;
     switch (layerType)
     {
        case 0:
             pI[idx] = pI[idx] * layerAQam;
             pQ[idx] = pQ[idx] * layerAQam;
             break;
         case 1:
              pI[idx] =  pI[idx] * layerBQam;
              pQ[idx] =  pQ[idx] * layerBQam;
              break;
          case 2:
               pI[idx] =  pI[idx] * layerCQam;
               pQ[idx] =  pQ[idx] * layerCQam;
               break;
            default:
				break;
       }
    idx++;
    BTHD_P_OSleep(h,1,N,GuardInterval);
  }

  return BERR_SUCCESS;
}

/***************************************************************************
 * BTHD_P_IsdbtSetTMCC()
 ***************************************************************************/
BTHD_RESULT BTHD_P_IsdbtSetTMCC( BTHD_3x7x_ChnHandle h, 
			       THD_TransmissionMode_t TransmissionMode, 
			       THD_GuardInterval_t GuardInterval)
{
  BTHD_RESULT return_val = THD_AcquireResult_InitLockState;
  uint32_t temp_0, temp_1, N=bthd_transmission_mode[TransmissionMode];
  BREG_WriteField(h->hRegister, THD_CORE_RST2, TMCC_RST, 1);      
  BREG_WriteField(h->hRegister, THD_CORE_RST2, TMCC_ERC_RST, 1);  
  BREG_WriteField(h->hRegister, THD_CORE_RST2, TMCC_ERC_RST, 0);  
  BREG_WriteField(h->hRegister, THD_CORE_RST2, ISDBT_TDI_RST, 1);
  BREG_WriteField(h->hRegister, THD_CORE_RST2, ISDBT_TDI_RST, 0);

  if (h->pAcquireParam->StdAcquire.IsdbtAcquireParam.TMCCMode == THD_IsdbtTMCCMode_Manual) {
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_0, PRFLAG,     h->pAcquireParam->StdAcquire.IsdbtAcquireParam.Pr);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_QAM, h->pAcquireParam->StdAcquire.IsdbtAcquireParam.Qam[0]);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_CR,  h->pAcquireParam->StdAcquire.IsdbtAcquireParam.CodeRate[0]);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_TI,  h->pAcquireParam->StdAcquire.IsdbtAcquireParam.TimeInt[0]);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_SEG, h->pAcquireParam->StdAcquire.IsdbtAcquireParam.Segments[0]);

    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_QAM, h->pAcquireParam->StdAcquire.IsdbtAcquireParam.Qam[1]);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_CR,  h->pAcquireParam->StdAcquire.IsdbtAcquireParam.CodeRate[1]);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_TI,  h->pAcquireParam->StdAcquire.IsdbtAcquireParam.TimeInt[1]);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_SEG, h->pAcquireParam->StdAcquire.IsdbtAcquireParam.Segments[1]);

    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_QAM, h->pAcquireParam->StdAcquire.IsdbtAcquireParam.Qam[2]);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_CR,  h->pAcquireParam->StdAcquire.IsdbtAcquireParam.CodeRate[2]);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_TI,  h->pAcquireParam->StdAcquire.IsdbtAcquireParam.TimeInt[2]);
    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_SEG, h->pAcquireParam->StdAcquire.IsdbtAcquireParam.Segments[2]);

    BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_0, USE_OV, 1);
    BREG_WriteField(h->hRegister, THD_CORE_RST2, TMCC_RST, 0);      
  } else {
    BREG_WriteField(h->hRegister, THD_CORE_RST2, TMCC_RST, 0);      
	BINT_EnableCallback( h->hTmccSyncCallback);
    if (BKNI_WaitForEvent(h->hTmccSyncEvent, 540) != BERR_SUCCESS) {
      return_val = THD_AcquireResult_NoTPSLock;
    }
	BINT_DisableCallback( h->hTmccSyncCallback);

    if (return_val != THD_AcquireResult_NoTPSLock) {
      BTHD_P_OSleep(h,1,N,GuardInterval);
      temp_0 =  BREG_Read32(h->hRegister, BCHP_THD_CORE_TMCC_OV_0) & 0xfffe0000;
      temp_0 |= BREG_ReadField(h->hRegister, THD_CORE_TMCC_MISC, PRFLAG) << 16;
      temp_0 |= BREG_Read32(h->hRegister, BCHP_THD_CORE_TMCC_LAYER_0) & 0x0000ffff;
      temp_1 = BREG_Read32(h->hRegister, BCHP_THD_CORE_TMCC_LAYER_1);
      BREG_Write32(h->hRegister, BCHP_THD_CORE_TMCC_OV_0, temp_0);
      BREG_Write32(h->hRegister, BCHP_THD_CORE_TMCC_OV_1, temp_1);
	  BREG_WriteField(h->hRegister, THD_CORE_TMCC_OV_0, USE_OV, 1);
/* 
	  temp_0 = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_SEG) * 230;
	  temp_0 |= (temp_0 -1) << 16;
	  BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_RT_CTRL,temp_0);
	  temp_0 = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_SEG) * 230;
	  if (temp_0 > 0) {
	  	temp_0 |= (temp_0 -1) << 16;
	  	BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_RT_CTRL_B,temp_0);
	  }
	  temp_0 = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_SEG) * 230;
	  if (temp_0 > 0) {
	  	temp_0 |= (temp_0 -1) << 16;
	  	BREG_Write32(h->hRegister,  BCHP_THD_CORE_RS_RT_CTRL_C,temp_0);
	  }
*/
    }
  }
  return return_val;
}	

/***************************************************************************
 * BTHD_P_IsdbtSetOI()
 ***************************************************************************/
void BTHD_P_IsdbtSetOI( BTHD_3x7x_ChnHandle h )
{
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_OI_N, 0x00000001 );
  BREG_Write32(h->hRegister,  BCHP_THD_CORE_OI_D, 0x00000002 );
  /*
  BDBG_MSG(("BTHD_P_ISDBTSetOI:\tOIN = 0x%08x", 0x00000001));
  BDBG_MSG(("                  \tOID = 0x%08x", 0x00000002));
  */
}

/***************************************************************************
 * BTHD_P_IsdbtSetFEC()
 ***************************************************************************/
BTHD_RESULT BTHD_P_IsdbtSetFEC( BTHD_3x7x_ChnHandle h )
{

  BTHD_RESULT return_val = THD_AcquireResult_InitLockState;

  /* Set soft decision exponent parameters */
  BREG_WriteField(h->hRegister, THD_CORE_EQ, FORMAT_OV, 0);
  BREG_WriteField(h->hRegister, THD_CORE_EQ, EXP_OFFSET, 0);

  /* Configure Viterbi and release resets */
  BREG_Write32(h->hRegister, BCHP_THD_CORE_VIT, 0x00000100);           /* VIT reacq, no cycle slip, no spectrum inversion */
  BREG_WriteField(h->hRegister, THD_CORE_RST, FEC_RST, 1 );  
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_RST, 1 );
  BREG_WriteField(h->hRegister, THD_CORE_RST, FEC_RST, 0 );  
  BREG_WriteField(h->hRegister, THD_CORE_RST, RS_RST, 0 );
  return return_val;
}

/***************************************************************************
 * BTHD_P_IsdbtGetNotch()
 ***************************************************************************/
BTHD_RESULT BTHD_P_IsdbtGetNotch( BTHD_3x7x_ChnHandle h, 
				 THD_TransmissionMode_t TransmissionMode)
{ 
  uint32_t iteration=0, start=0,idx,value,max_idx=0, max=0, NumCoChannelFound = 0;
  uint32_t errpwr;
  bool CoChannelPresent = false;

  /* Get average pilot noise level.  Scale factor of 45 gives good match to mean snooper level for white noise */
  errpwr = BREG_Read32(h->hRegister, BCHP_THD_CORE_EQ_SNRP)/45;

  /* Determine whether analog TV CCI is present using snooper. 
     Look for the presence of any 2 of the video, color, or audio carriers.  
     This 2-out-of-3 criterion allows differentiation between true analog TV CCI 
     and spurs which often occur around sensitivity. If this criterion is too 
     strict, consider using a 1-out-of-3 criterion for input power levels above 
     sensitivity and this 2-out-of-3 criterion for input power levels near sensitivity */
  iteration = 0;
  while ((iteration < 3) && !CoChannelPresent) {
    switch (iteration) {
      /* Snooper captures only active carriers.  For ISDBT, there are 4617 active carriers. */
      /* start = 2808 - 128*3 - f*8192*(63/512) */
      case 0: if (TransmissionMode == THD_TransmissionMode_2k) start=000; else start = 0516; break;  /* Video carrier (f = -1.893) */
      case 1: if (TransmissionMode == THD_TransmissionMode_2k) start=936; else start = 4124; break;  /* Color carrier (f =  1.687) */
      case 2: if (TransmissionMode == THD_TransmissionMode_2k) start=936; else start = 4848; break;  /* Audio carrier (f =  2.607) */
    }
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_CE_RECORD_CFG,0x05030000 + start );  /* CE snooper capture exponents, snapshot mode, step=3 */

    while (!BREG_ReadField(h->hRegister,  THD_CORE_CE_RECORD_CFG, DONE));
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_CE_READ_INDEX,0x00000000 );
    for (idx=0; idx<256; idx++) {
      value = BREG_Read32(h->hRegister,  BCHP_THD_CORE_CE_READ_DATA );
	  if ((idx == 0) || (value > max)) {
	    max = value;
		max_idx = idx;
	  }
    }
    if (max >= 16*errpwr)                                                                     /* CCI present if min_exp-mean_exp > 3 */
      NumCoChannelFound++;
    if (NumCoChannelFound > 1)
      CoChannelPresent = true;
    iteration++;
  }
  return CoChannelPresent;
}

/***************************************************************************
 * BTHD_P_IsdbtSetICE ()
 ***************************************************************************/
BTHD_RESULT BTHD_P_IsdbtSetICE( BTHD_3x7x_ChnHandle h, 
			       THD_TransmissionMode_t TransmissionMode,
			       THD_GuardInterval_t GuardInterval)
{
  int n,k;
  uint32_t ice[16],ice_val[16],ice_cnt[16],ice_found,ice_val_max,ice_cnt_max,ice_index_max;
  uint32_t ice_cnt_best=0,ice_val_best=0,ice_index_best=0,spinv_best=0;
  uint32_t N=bthd_transmission_mode[TransmissionMode];
  uint32_t iteration = 0;
  uint32_t carrier_off   = 0;
  uint32_t carrier_step  = 1;
  uint32_t carrier_limit = 1;
  int32_t  cl_int, cl_fcw;
  bool     ice_done=0;

/*  if ( !(BREG_Read32(h->hRegister, BCHP_THD_CORE_RESERVED2) & 0x80000000) )  */		/* no more reserved2 */
  switch (GuardInterval) {
      case THD_GuardInterval_1_4:  carrier_limit = 2;  carrier_step = 1; break;
      case THD_GuardInterval_1_8:  carrier_limit = 4;  carrier_step = 2; break;
      case THD_GuardInterval_1_16: carrier_limit = 8;  carrier_step = 4; break;
      case THD_GuardInterval_1_32: carrier_limit = 16; carrier_step = 8; break;
  }

  /* Collect ICE values with carrier loop frozen */
  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CL_FRZ, 1);
  cl_int = BREG_Read32(h->hRegister,  BCHP_THD_CORE_CL_INT); 
  cl_fcw = BREG_Read32(h->hRegister,  BCHP_THD_CORE_CL_FCW);
          
  for (iteration=0; iteration<2; iteration++) {
    for (carrier_off=0; carrier_off < carrier_limit; carrier_off += carrier_step) {
      /* To aid AC1 demodulation, offset initial carrier frequency */
      if (iteration == 0)
        BREG_Write32(h->hRegister, BCHP_THD_CORE_CL_INT, cl_int + (carrier_off << 21));
	  else  /* bug fix for spectral inversion.  need to keep the integrator programmed to the inverted value */
		BREG_Write32(h->hRegister, BCHP_THD_CORE_CL_INT, ((cl_int ^ 0xffffffff) + 1U) + (carrier_off << 21));

      BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_CINT_FRZ, 0 );
      BTHD_P_OSleep(h,3,N,GuardInterval);
      for (n=0; n<16; n++) {
        ice[n] = BREG_ReadField(h->hRegister,  THD_CORE_CL_IEST, INT) + carrier_off;
        ice_val[n] = 0;
        ice_cnt[n] = 0;
        BTHD_P_OSleep(h,1,N,GuardInterval);
      	/* BDBG_MSG(("BTHD_P_DvbtSetICE:\tICE = 0x%08x\n",ice[n]); */
	  }

    /* Identify unique ICE values and count number-of-occurrences of each */
      ice_val[0] = ice[0];
      ice_cnt[0] = 1;
      for (n=1; n<16; n++) {
        ice_found = 0;
        for (k=0; k<n; k++) {
	      if (ice[n] == ice_val[k]) {  /* found, increment the count */
	  	    ice_cnt[k] = ice_cnt[k]+1;
	        ice_found = 1;        
		  }
        }
        if (!ice_found) {              /* not found, add it to the table */
	      ice_val[k] = ice[n];
	      ice_cnt[k] = 1;
        }
      }
      /* Determine value which occurs most often */  
      ice_val_max = ice_val[0];
      ice_cnt_max = ice_cnt[0];
      ice_index_max = 0;
      for (n=1; n<16; n++) {
        if (ice_cnt[n] > ice_cnt_max) {
	      ice_val_max = ice_val[n];
		  ice_cnt_max = ice_cnt[n];
		  ice_index_max = n;
        }
      }
      /* BDBG_MSG(("BTHD_P_DvbtSetICE:\tIC Offset = %d, ICE Max = 0x%08x (%d occurences)\n", carrier_off, ice_val[ice_index_max],ice_cnt_max); */
      /* Keep track of the best result for comparison */
      if (ice_cnt_max > ice_cnt_best) {
        ice_cnt_best   =  ice_cnt_max;
        ice_val_best   =  ice_val_max;
        ice_index_best =  ice_index_max;
        spinv_best     =  iteration;
      }

    /* Check for valid ICE and exit */
      if ( ice_cnt_best >= 12 ) {
		ice_done = 1;
        break;
 	  }
	}

	if (ice_done)
	  break;

    /* Try spectral inversion on second iteration */
    BDBG_MSG(("BTHD_P_IsdbtSetICE:\tTrying spectral inversion\n"));
    if (h->pInternalAcquireParam->FrontEndMode == THD_FrontEndMode_Baseband) {
      BREG_WriteField(h->hRegister, THD_CORE_FE, NEGATE_Q, 1);
	  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_INT, (cl_int ^ 0xffffffff) + 1U);
    } else {
      /*  Negate carrier fcw and integrator */ 
      BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_FCW, (cl_fcw ^ 0xffffffff) + 1U);
      BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_INT, (cl_int ^ 0xffffffff) + 1U);   
    }

    /* Freeze and wait 10 symbols */
    BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_CINT_FRZ, 1 );             /*  Freeze integer carrier estimator*/
    BTHD_P_OSleep(h,10,N,GuardInterval);
  }

  /* Reset CL_INT back to initial value (allowing for spectral inversion) */
  if (iteration)
    BREG_Write32(h->hRegister, BCHP_THD_CORE_CL_INT, (cl_int ^ 0xffffffff) + 1U);
  else
    BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_INT, cl_int);

  /* Uninvert signal if needed.  
     This can happen if spectral inversion was tested but noninverted spectrum yielded a better ICE */
  if (iteration && !spinv_best) {
    if (h->pInternalAcquireParam->FrontEndMode == THD_FrontEndMode_Baseband) {
      BREG_WriteField(h->hRegister, THD_CORE_FE, NEGATE_Q, 0);
	  BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_INT, cl_int);	
    } else {
      BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_FCW, cl_fcw);
      BREG_Write32(h->hRegister,  BCHP_THD_CORE_CL_INT, cl_int);
    }
  }
  if (spinv_best)
    BDBG_MSG(("BTHD_P_IsdbtSetICE:\tSpectral Inversion Detected\n"));
  
  /* Detection flag for suspect ICE result */
  if (ice_cnt_best < 6) {
    BDBG_WRN(("BTHD_P_IsdbtSetICE:\tLow ICE Count Detected\n"));
    h->pStatus->ThdCommonStatus.LowICECount = 1;
  }
  else {
    h->pStatus->ThdCommonStatus.LowICECount = 0;
  }

  BREG_WriteField(h->hRegister, THD_CORE_FRZ, CP_CINT_FRZ, 1 );
  BTHD_P_OSleep(h,1,N,GuardInterval);

  /* BDBG_MSG(("BTHD_P_DvbtSetICE:\tICE Max = 0x%08x (%d occurences)",ice_val_best,ice_cnt_best)); */
  return(ice_val_best);
}

/***************************************************************************
 * BTHD_P_IsdbtStatus()
 ***************************************************************************/
void BTHD_P_IsdbtStatus( BTHD_3x7x_ChnHandle h ) 
{
  uint32_t  CERC,CBERC,NBERC,UBERC,TBERC,NFERC,UFERC,TFERC,eq_snr;
  uint32_t  CERC_B,CBERC_B,NBERC_B,UBERC_B,TBERC_B;
  uint32_t  CERC_C,CBERC_C,NBERC_C,UBERC_C,TBERC_C;
  uint32_t  sft_scale;
  uint32_t  tmp_a=0, tmp_b=0, tmp_c=0, scale;
  uint32_t  pouthi, poutlo, pouthi2, poutlo2, pouthi3, poutlo3;

  const uint8_t scale_table[] = { 29, 27, 25, 29 }; /* Support invalid QAM.*/
  const uint8_t sft_scale_table[][4] = \
  {
    {  2,  2,  2,  2 },
    { 10, 10, 20, 52 },
    { 42, 42, 60, 108},
    {  2,  2,  2,  2 } 
  };
  /* Determine transmission parameters */
  h->pStatus->StdStatus.IsdbtStatus.Ews  			= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_MISC, ALERT);
  h->pStatus->StdStatus.IsdbtStatus.Pr  			= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_0, PRFLAG); 
  h->pStatus->StdStatus.IsdbtStatus.Qam[0]  		= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_QAM);
  h->pStatus->StdStatus.IsdbtStatus.Qam[1]  		= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_QAM);
  h->pStatus->StdStatus.IsdbtStatus.Qam[2]  		= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_QAM); 
  h->pStatus->StdStatus.IsdbtStatus.CodeRate[0] 	= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_CR);
  h->pStatus->StdStatus.IsdbtStatus.CodeRate[1] 	= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_CR); 
  h->pStatus->StdStatus.IsdbtStatus.CodeRate[2] 	= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_CR);  
  h->pStatus->StdStatus.IsdbtStatus.TimeInt[0] 	= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_TI); 
  h->pStatus->StdStatus.IsdbtStatus.TimeInt[1] 	=  		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_TI); 
  h->pStatus->StdStatus.IsdbtStatus.TimeInt[2] 	=  		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_TI); 
  h->pStatus->StdStatus.IsdbtStatus.Segments[0] 	= 		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_SEG); 
  h->pStatus->StdStatus.IsdbtStatus.Segments[1] 	=  		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_SEG); 
  h->pStatus->StdStatus.IsdbtStatus.Segments[2] 	=  		BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_SEG); 

  /* Compute data SNR */
  /* Layer A SNR */
  eq_snr = BREG_Read32(h->hRegister, BCHP_THD_CORE_EQ_SNR);
  if (eq_snr >> (8+2)) {
	scale		= 	scale_table[h->pStatus->StdStatus.IsdbtStatus.Qam[0]];
	sft_scale	=	sft_scale_table[h->pStatus->StdStatus.IsdbtStatus.Qam[0]][0];
	tmp_a 		=	(sft_scale << (scale-2))/(eq_snr >> (8+2));
    h->pStatus->StdStatus.IsdbtStatus.SNRData[0] = BMTH_2560log10(tmp_a) - 6165;
  }
  /* Layer B SNR */
  eq_snr = BREG_Read32(h->hRegister, BCHP_THD_CORE_EQ_SNR_B);
  if (eq_snr >> (8+2)) {
	scale		= 	scale_table[h->pStatus->StdStatus.IsdbtStatus.Qam[1]];
	sft_scale	=	sft_scale_table[h->pStatus->StdStatus.IsdbtStatus.Qam[1]][0];
	tmp_b 		=	(sft_scale << (scale-2))/(eq_snr >> (8+2));
  	h->pStatus->StdStatus.IsdbtStatus.SNRData[1] = BMTH_2560log10(tmp_b) - 6165;
  }
  /* Layer C SNR */
  eq_snr = BREG_Read32(h->hRegister, BCHP_THD_CORE_EQ_SNR_C);
  if (eq_snr >> (8+2)) {
	scale		= 	scale_table[h->pStatus->StdStatus.IsdbtStatus.Qam[2]];
	sft_scale	=	sft_scale_table[h->pStatus->StdStatus.IsdbtStatus.Qam[2]][0];
	tmp_c 		=	(sft_scale << (scale-2))/(eq_snr >> (8+2));
    h->pStatus->StdStatus.IsdbtStatus.SNRData[2] = BMTH_2560log10(tmp_c) - 6165;
  }
  /* Layer Weighted SNR */
  BMTH_HILO_32TO64_Mul(tmp_a, h->pStatus->StdStatus.IsdbtStatus.Segments[0], &pouthi, &poutlo);
  BMTH_HILO_32TO64_Mul(tmp_b, h->pStatus->StdStatus.IsdbtStatus.Segments[1], &pouthi2, &poutlo2);
  BMTH_HILO_64TO64_Add(pouthi, poutlo, pouthi2, poutlo2, &pouthi3, &poutlo3);
  BMTH_HILO_32TO64_Mul(tmp_c, h->pStatus->StdStatus.IsdbtStatus.Segments[2], &pouthi, &poutlo);
  BMTH_HILO_64TO64_Add(pouthi, poutlo, pouthi3, poutlo3, &pouthi2, &poutlo2);
  BMTH_HILO_64TO64_Div32(pouthi2, poutlo2,  13, &pouthi, &poutlo);
  h->pStatus->ThdCommonStatus.SNR 				 = BMTH_2560log10(poutlo) - 6165;

  /* Read hardware error counters */
  BREG_WriteField(h->hRegister, THD_CORE_FEC, CAPERC, 1 ); 

  NFERC = BREG_Read32(h->hRegister,  BCHP_THD_CORE_NFERC );
  UFERC = BREG_Read32(h->hRegister,  BCHP_THD_CORE_UFERC );
  TFERC = NFERC + UFERC; 

  CERC  = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_CERC );
  NBERC = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_NBERC );
  CBERC = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_CBERC );
  UBERC = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_UBERC );
  TBERC = NBERC + CBERC + UBERC;

  CERC_B  = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_CERC_B );
  NBERC_B = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_NBERC_B );
  CBERC_B = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_CBERC_B );
  UBERC_B = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_UBERC_B );
  TBERC_B = NBERC_B + CBERC_B + UBERC_B;

  CERC_C  = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_CERC_C );
  NBERC_C = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_NBERC_C );
  CBERC_C = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_CBERC_C );
  UBERC_C = BREG_Read32(h->hRegister,  BCHP_THD_CORE_RS_UBERC_C );
  TBERC_C = NBERC_C + CBERC_C + UBERC_C;

  BREG_WriteField(h->hRegister, THD_CORE_FEC, CAPERC, 0 ); 

  /* Update software error counters */
  h->pStatus->StdStatus.IsdbtStatus.TS_NFERC = h->pStatus->StdStatus.IsdbtStatus.TS_NFERC_ref + NFERC;
  h->pStatus->StdStatus.IsdbtStatus.TS_UFERC = h->pStatus->StdStatus.IsdbtStatus.TS_UFERC_ref + UFERC;
  h->pStatus->StdStatus.IsdbtStatus.TS_TFERC = h->pStatus->StdStatus.IsdbtStatus.TS_TFERC_ref + TFERC;

  h->pStatus->StdStatus.IsdbtStatus.TS_CERC[0]  = h->pStatus->StdStatus.IsdbtStatus.TS_CERC_ref[0]  + CERC;
  h->pStatus->StdStatus.IsdbtStatus.TS_NBERC[0] = h->pStatus->StdStatus.IsdbtStatus.TS_NBERC_ref[0] + NBERC;
  h->pStatus->StdStatus.IsdbtStatus.TS_CBERC[0] = h->pStatus->StdStatus.IsdbtStatus.TS_CBERC_ref[0] + CBERC;
  h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[0] = h->pStatus->StdStatus.IsdbtStatus.TS_UBERC_ref[0] + UBERC;
  h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[0] = h->pStatus->StdStatus.IsdbtStatus.TS_TBERC_ref[0] + TBERC;

  h->pStatus->StdStatus.IsdbtStatus.TS_CERC[1]  = h->pStatus->StdStatus.IsdbtStatus.TS_CERC_ref[1]  + CERC_B;
  h->pStatus->StdStatus.IsdbtStatus.TS_NBERC[1] = h->pStatus->StdStatus.IsdbtStatus.TS_NBERC_ref[1] + NBERC_B;
  h->pStatus->StdStatus.IsdbtStatus.TS_CBERC[1] = h->pStatus->StdStatus.IsdbtStatus.TS_CBERC_ref[1] + CBERC_B;
  h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[1] = h->pStatus->StdStatus.IsdbtStatus.TS_UBERC_ref[1] + UBERC_B;
  h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[1] = h->pStatus->StdStatus.IsdbtStatus.TS_TBERC_ref[1] + TBERC_B;

  h->pStatus->StdStatus.IsdbtStatus.TS_CERC[2]  = h->pStatus->StdStatus.IsdbtStatus.TS_CERC_ref[2]  + CERC_C;
  h->pStatus->StdStatus.IsdbtStatus.TS_NBERC[2] = h->pStatus->StdStatus.IsdbtStatus.TS_NBERC_ref[2] + NBERC_C;
  h->pStatus->StdStatus.IsdbtStatus.TS_CBERC[2] = h->pStatus->StdStatus.IsdbtStatus.TS_CBERC_ref[2] + CBERC_C;
  h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[2] = h->pStatus->StdStatus.IsdbtStatus.TS_UBERC_ref[2] + UBERC_C;
  h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[2] = h->pStatus->StdStatus.IsdbtStatus.TS_TBERC_ref[2] + TBERC_C;

  /* Reset hardware error counters every 10 one-second frames */
  if (TFERC >= THD_StatusFramesForReset) {
    BTHD_P_ResetStatusHW(h);
    h->pStatus->StdStatus.IsdbtStatus.TS_CERC_ref[0]  = h->pStatus->StdStatus.IsdbtStatus.TS_CERC[0];
    h->pStatus->StdStatus.IsdbtStatus.TS_NBERC_ref[0] = h->pStatus->StdStatus.IsdbtStatus.TS_NBERC[0];
    h->pStatus->StdStatus.IsdbtStatus.TS_CBERC_ref[0] = h->pStatus->StdStatus.IsdbtStatus.TS_CBERC[0];
    h->pStatus->StdStatus.IsdbtStatus.TS_UBERC_ref[0] = h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[0];
    h->pStatus->StdStatus.IsdbtStatus.TS_TBERC_ref[0] = h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[0];

    h->pStatus->StdStatus.IsdbtStatus.TS_CERC_ref[1]  = h->pStatus->StdStatus.IsdbtStatus.TS_CERC[1];
    h->pStatus->StdStatus.IsdbtStatus.TS_NBERC_ref[1] = h->pStatus->StdStatus.IsdbtStatus.TS_NBERC[1];
    h->pStatus->StdStatus.IsdbtStatus.TS_CBERC_ref[1] = h->pStatus->StdStatus.IsdbtStatus.TS_CBERC[1];
    h->pStatus->StdStatus.IsdbtStatus.TS_UBERC_ref[1] = h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[1];
    h->pStatus->StdStatus.IsdbtStatus.TS_TBERC_ref[1] = h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[1];

    h->pStatus->StdStatus.IsdbtStatus.TS_CERC_ref[2]  = h->pStatus->StdStatus.IsdbtStatus.TS_CERC[2];
    h->pStatus->StdStatus.IsdbtStatus.TS_NBERC_ref[2] = h->pStatus->StdStatus.IsdbtStatus.TS_NBERC[2];
    h->pStatus->StdStatus.IsdbtStatus.TS_CBERC_ref[2] = h->pStatus->StdStatus.IsdbtStatus.TS_CBERC[2];
    h->pStatus->StdStatus.IsdbtStatus.TS_UBERC_ref[2] = h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[2];
    h->pStatus->StdStatus.IsdbtStatus.TS_TBERC_ref[2] = h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[2];

    h->pStatus->StdStatus.IsdbtStatus.TS_NFERC_ref = h->pStatus->StdStatus.IsdbtStatus.TS_NFERC;
    h->pStatus->StdStatus.IsdbtStatus.TS_UFERC_ref = h->pStatus->StdStatus.IsdbtStatus.TS_UFERC;
    h->pStatus->StdStatus.IsdbtStatus.TS_TFERC_ref = h->pStatus->StdStatus.IsdbtStatus.TS_TFERC;
  }  

  /* Compute Viterbi BER and TS PER */
  if (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[0]) {
    /* h->pStatus->StdStatus.IsdbtStatus.TS_PER[0] = (h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[0] * (1ULL<<31)) / h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[0]; */
    BMTH_HILO_32TO64_Mul(0x80000000, (h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[0]), &pouthi, &poutlo);
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[0]), &pouthi, &h->pStatus->StdStatus.IsdbtStatus.TS_PER[0] );
    /* h->pStatus->StdStatus.IsdbtStatus.VitBER[0] = (h->pStatus->StdStatus.IsdbtStatus.TS_CERC[0] * (1ULL<<31)) / (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[0] * 8 * 188ULL); */
    BMTH_HILO_32TO64_Mul(0x80000000, (h->pStatus->StdStatus.IsdbtStatus.TS_CERC[0]), &pouthi, &poutlo);
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (8 * 188), &pouthi, &poutlo );
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[0]), &pouthi, &h->pStatus->StdStatus.IsdbtStatus.VitBER[0]);
  }
  if (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[1]) {
    BMTH_HILO_32TO64_Mul(0x80000000, (h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[1]), &pouthi, &poutlo);
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[1]), &pouthi, &h->pStatus->StdStatus.IsdbtStatus.TS_PER[1] );
    BMTH_HILO_32TO64_Mul(0x80000000, (h->pStatus->StdStatus.IsdbtStatus.TS_CERC[1]), &pouthi, &poutlo);
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (8 * 188), &pouthi, &poutlo );
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[1]), &pouthi, &h->pStatus->StdStatus.IsdbtStatus.VitBER[1]);
  }
  if (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[2]) {
    BMTH_HILO_32TO64_Mul(0x80000000, (h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[2]), &pouthi, &poutlo);
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[2]), &pouthi, &h->pStatus->StdStatus.IsdbtStatus.TS_PER[2] );
    BMTH_HILO_32TO64_Mul(0x80000000, (h->pStatus->StdStatus.IsdbtStatus.TS_CERC[2]), &pouthi, &poutlo);
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (8 * 188), &pouthi, &poutlo );
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[2]), &pouthi, &h->pStatus->StdStatus.IsdbtStatus.VitBER[2]);
  }
  if (h->pStatus->StdStatus.IsdbtStatus.TS_TFERC){
    /* h->pStatus->StdStatus.IsdbtStatus.TS_ESR = ((100ULL * h->pStatus->StdStatus.IsdbtStatus.TS_UFERC) / h->pStatus->StdStatus.IsdbtStatus.TS_TFERC); */
    BMTH_HILO_32TO64_Mul(100, (h->pStatus->StdStatus.IsdbtStatus.TS_UFERC), &pouthi, &poutlo);
    BMTH_HILO_64TO64_Div32(pouthi, poutlo, (h->pStatus->StdStatus.IsdbtStatus.TS_TFERC), &pouthi, &h->pStatus->StdStatus.IsdbtStatus.TS_ESR );
  }
}

 
/***************************************************************************
 * BTHD_P_IsdbtResetStatus()
 ***************************************************************************/
void BTHD_P_IsdbtResetStatus( BTHD_3x7x_ChnHandle h )
{
  h->pStatus->StdStatus.IsdbtStatus.TS_UFERC     = 0;  
  h->pStatus->StdStatus.IsdbtStatus.TS_NFERC     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_TFERC     = 0;

  h->pStatus->StdStatus.IsdbtStatus.TS_UFERC_ref = 0;  
  h->pStatus->StdStatus.IsdbtStatus.TS_NFERC_ref = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_TFERC_ref = 0;

  h->pStatus->StdStatus.IsdbtStatus.TS_ESR       = 0;

  h->pStatus->StdStatus.IsdbtStatus.TS_CERC[0]      = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_CBERC[0]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[0]     = 0;  
  h->pStatus->StdStatus.IsdbtStatus.TS_NBERC[0]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[0]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.VitBER[0]       = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_PER[0]       = 0;

  h->pStatus->StdStatus.IsdbtStatus.TS_CERC[1]      = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_CBERC[1]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[1]     = 0;  
  h->pStatus->StdStatus.IsdbtStatus.TS_NBERC[1]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[1]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.VitBER[1]       = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_PER[1]       = 0;

  h->pStatus->StdStatus.IsdbtStatus.TS_CERC[2]      = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_CBERC[2]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_UBERC[2]     = 0;  
  h->pStatus->StdStatus.IsdbtStatus.TS_NBERC[2]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_TBERC[2]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.VitBER[2]       = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_PER[2]       = 0;

  h->pStatus->StdStatus.IsdbtStatus.TS_CERC_ref[0]      = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_CBERC_ref[0]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_UBERC_ref[0]     = 0;  
  h->pStatus->StdStatus.IsdbtStatus.TS_NBERC_ref[0]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_TBERC_ref[0]     = 0;

  h->pStatus->StdStatus.IsdbtStatus.TS_CERC_ref[1]      = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_CBERC_ref[1]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_UBERC_ref[1]     = 0;  
  h->pStatus->StdStatus.IsdbtStatus.TS_NBERC_ref[1]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_TBERC_ref[1]     = 0;

  h->pStatus->StdStatus.IsdbtStatus.TS_CERC_ref[2]      = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_CBERC_ref[2]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_UBERC_ref[2]     = 0;  
  h->pStatus->StdStatus.IsdbtStatus.TS_NBERC_ref[2]     = 0;
  h->pStatus->StdStatus.IsdbtStatus.TS_TBERC_ref[2]     = 0;
}

/***************************************************************************
 * BTHD_P_IsdbtResetLockSetClrFlag()
 ***************************************************************************/
void BTHD_P_IsdbtResetLockSetClrFlag( BTHD_3x7x_ChnHandle h )
{
  h->pAcquireParam->IsdbtLocaleParam.set_a = 0;
  h->pAcquireParam->IsdbtLocaleParam.set_b = 0;
  h->pAcquireParam->IsdbtLocaleParam.set_c = 0;
  h->pAcquireParam->IsdbtLocaleParam.clr_a = 0;
  h->pAcquireParam->IsdbtLocaleParam.clr_b = 0;
  h->pAcquireParam->IsdbtLocaleParam.clr_c = 0;
}

/***************************************************************************
 * BTHD_P_IsdbtSetRSRT()
 ***************************************************************************/
void BTHD_P_IsdbtSetRsRt( BTHD_3x7x_ChnHandle h, uint32_t nom_scale, uint32_t t_div)
{

  uint32_t count_a, count_b, count_c;
  	BSTD_UNUSED(nom_scale);
  /* 3000 /13 ~ 230 per segment.  set to 256 to approximate*/
  count_a = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_0, LAYERA_SEG) * (256 >> t_div);
/*  count_a |= (count_a>>nom_scale) << 16; */
  count_a |= (count_a-1) << 16; 
  BREG_Write32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL, count_a);  			   
  count_b = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERB_SEG) * (256 >> t_div);
/*  count_b |= (count_b>>nom_scale) << 16; */
  count_b |= (count_b-1) << 16; 
  BREG_Write32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL_B, count_b);  
  count_c = BREG_ReadField(h->hRegister, THD_CORE_TMCC_OV_1, LAYERC_SEG) * (256 >> t_div);
/*  count_c |= (count_c>>nom_scale) << 16; */
  count_c |= (count_c-1) << 16; 
  BREG_Write32(h->hRegister, BCHP_THD_CORE_RS_RT_CTRL_C, count_c);  
/*  BDBG_MSG(("RsRt count_a: %0x, count_b: %0x, count_c: %0x, t_div: t_div: %0x", count_a, count_b, count_c, t_div)); */
}

/***************************************************************************
 * BTHD_P_IsdbtAcquire()
 ***************************************************************************/
BTHD_RESULT BTHD_P_IsdbtAcquire( BTHD_3x7x_ChnHandle h )
{
  BTHD_RESULT return_val=THD_AcquireResult_InitLockState;

  THD_FFTWindowMode_t FFTWindowMode=THD_FFTWindowMode_InSpan, FFTWindowModeList[3];
  THD_ChannelEstimatorMode_t ChannelEstimatorMode=THD_ChannelEstimatorMode_Fixed, ChannelEstimatorModeList[3];
  THD_State_t NextState=THD_State_Init, LastState=THD_State_Init, State=THD_State_Init;
  bool Done=false, acqProfileValid[16];
  uint32_t retries=0, ChannelEstimatorRetries, fscntInit=0xffffffff, acqProfile[16], k;
  uint32_t TPSRetries=0, AllowedTPSRetries=1;
  char     *acqProfileString[16];
  uint32_t tmp_0=0, tmp_1=0, tmp_2=0, DPSK_cnt=0;
  bool Skip_FFTTrigger=false;
  THD_TransmissionMode_t TransmissionMode;

  /* Initialize Candidate FFT Window Modes */
/*
  FFTWindowModeList[0] = THD_FFTWindowMode_InSpan;
  FFTWindowModeList[1] = THD_FFTWindowMode_OutOfSpanPre;
  FFTWindowModeList[2] = THD_FFTWindowMode_OutOfSpanPost;
  if (h->pInternalAcquireParam->FFTWindowMode == THD_FFTWindowMode_Auto) {
    FFTWindowMode = FFTWindowModeList[0];
#ifdef FFTTriggerPosition
    FFTWindowRetries = 2;
#else
	FFTWindowRetries = 1;
#endif
  } else {
    FFTWindowMode = h->pInternalAcquireParam->FFTWindowMode;
    FFTWindowRetries = 0;
  }
*/
  /* Initialize Candidate Channel Estimator Modes */
  ChannelEstimatorModeList[0] = THD_ChannelEstimatorMode_Fixed;
  ChannelEstimatorModeList[1] = THD_ChannelEstimatorMode_Pedestrian;
  ChannelEstimatorModeList[2] = THD_ChannelEstimatorMode_Mobile;
  if (h->pInternalAcquireParam->ChannelEstimatorMode == THD_ChannelEstimatorMode_Auto) {
    ChannelEstimatorMode = ChannelEstimatorModeList[0];
    ChannelEstimatorRetries = 2;
  } else {
    ChannelEstimatorModeList[0] = h->pInternalAcquireParam->ChannelEstimatorMode;
    ChannelEstimatorRetries = 0;
  }

  /* Use Fscnt for acquisition time profiling */
  BREG_Write32(h->hRegister, BCHP_THD_CORE_FSCNT,fscntInit);

  acqProfileString[THD_State_Init] = "Init";
  acqProfileString[THD_State_SP] = "SP";
  acqProfileString[THD_State_TPS] = "TMCC";
  acqProfileString[THD_State_FFTTrigger] = "FFTTrigger";
  acqProfileString[THD_State_FEC] = "FEC";
  acqProfileString[THD_State_Track] = "Track";
  acqProfileString[THD_State_ChangeFFTWindow] = "ChangeFFTWindow";
  acqProfileString[THD_State_CheckLock] = "CheckLock";
  acqProfileString[THD_State_ChangeChannelEstimator] = "ChangeChannelEstimator";
  acqProfileString[THD_State_Done] = "Done";
  for (k=0; k<16; k++)
    acqProfileValid[k] = false;

  /* State Machine */
  while (!Done) {
    switch(State) {

    case THD_State_Init:
      BDBG_MSG(("\tInit"));
      h->pInternalAcquireParam->AllowRsSyncEvent = true;
      return_val = BTHD_P_AcquireInit(h,FFTWindowMode);
      if ((return_val == THD_AcquireResult_NoSignal) || (return_val == THD_AcquireResult_NoFFTLock))
		    NextState = THD_State_Done;
      else
		    NextState = THD_State_SP;
      break;

    case THD_State_SP:
      BDBG_MSG(("\tSP"));
      return_val = BTHD_P_AcquireSP(h);
      if (return_val == THD_AcquireResult_NoSPLock)
	      NextState = THD_State_Done;
      else
	      NextState = THD_State_TPS;
      break;

    case THD_State_TPS:
      BDBG_MSG(("\tTMCC"));
      return_val = BTHD_P_AcquireTPS(h);

      if (return_val != THD_AcquireResult_NoTPSLock) {
        tmp_0 = BREG_Read32(h->hRegister, BCHP_THD_CORE_TMCC_LAYER_0);
        tmp_1 = (tmp_0 & 0x00003000) >> 12;    /* Layer A QAM*/
        tmp_2 = (tmp_0 & 0x0000000f);          /* Layer A seg*/
		/* BDBG_MSG(("BTHD_P_IsdbtAcquire: \t QAM_A: %d, Seg_A: %d", tmp_1, tmp_2));  */
        if (tmp_1 == 3) DPSK_cnt = DPSK_cnt + tmp_2; /* Count DPSK seg*/ 
        tmp_0 = BREG_Read32(h->hRegister, BCHP_THD_CORE_TMCC_LAYER_1);
        tmp_1 = (tmp_0 & 0x30000000) >> 28;    /* Layer B QAM*/
        tmp_2 = (tmp_0 & 0x000f0000) >> 16;    /* Layer B seg*/
  	    /* BDBG_MSG(("BTHD_P_IsdbtAcquire: \t QAM_B: %d, Seg_B: %d", tmp_1, tmp_2));  */
        if (tmp_1 == 3) DPSK_cnt = DPSK_cnt + tmp_2; /* Count DPSK seg*/ 
        tmp_1 = (tmp_0 & 0x00003000) >> 12;    /* Layer C QAM*/
        tmp_2 = (tmp_0 & 0x0000000f);          /* Layer C seg*/
 		/* BDBG_MSG(("BTHD_P_IsdbtAcquire: \t QAM_C: %d, Seg_C: %d", tmp_1, tmp_2));  */
        if (tmp_1 == 3) DPSK_cnt = DPSK_cnt + tmp_2; /* Count DPSK seg*/ 
        TransmissionMode = BREG_ReadField(h->hRegister, THD_CORE_TPS_OV, TRANS_MODE );
        if ((DPSK_cnt > 0) && (TransmissionMode == THD_TransmissionMode_2k)) {
          Skip_FFTTrigger = true;
        } else if ((DPSK_cnt > 1) && (TransmissionMode == THD_TransmissionMode_4k)) {
          Skip_FFTTrigger = true;
        } else if ((DPSK_cnt > 1) && (TransmissionMode == THD_TransmissionMode_8k)) {
          Skip_FFTTrigger = true;
        }
        if (Skip_FFTTrigger) BDBG_MSG(("BTHD_P_IsdbtAcquire: \t%d DPSK seg detected, skip FFTTrigger!", DPSK_cnt));
      }
      if (return_val == THD_AcquireResult_AbortedEarly) {
        NextState = THD_State_Done;
      } else if ( (TPSRetries < AllowedTPSRetries) && (!Skip_FFTTrigger)) {
	      NextState = THD_State_FFTTrigger;
        TPSRetries++;
      }
      else {
        if (return_val == THD_AcquireResult_NoTPSLock)
	        NextState = THD_State_Done;
        else
	        NextState = THD_State_FEC;
      }
      break;

    case THD_State_FFTTrigger:
      BDBG_MSG(("\tFFTTrigger"));
      return_val = BTHD_P_AcquireFFTTrigger(h);
      if (return_val == THD_AcquireResult_AbortedEarly) {
          NextState = THD_State_Done;
      }
      else { 
        if (h->pStatus->ThdCommonStatus.FFTTriggerMissed | h->pStatus->ThdCommonStatus.FFTTriggerOnGuard)
	        NextState  = THD_State_TPS; 
        else
	        NextState  = THD_State_FEC; 
      }
      break;

    case THD_State_FEC:
      BDBG_MSG(("\tFEC"));
	    BTHD_P_IsdbtSetRsRt(h,3,0); 			/* 0: based on approximately 3000 packets */
	    BTHD_P_IsdbtResetLockSetClrFlag(h);   /* clear previous interrupt flags */
      return_val = BTHD_P_AcquireFEC(h,(retries==0));
      if ( (return_val == THD_AcquireResult_NoFECLock) || (return_val == THD_AcquireResult_AbortedEarly) ) {
	      NextState = THD_State_Done;
      } else {
	      NextState = THD_State_Track;
	      retries = 0;
      }
      break;

    case THD_State_ChangeFFTWindow:
      BDBG_MSG(("\tChangeFFTWindow"));
      FFTWindowMode = FFTWindowModeList[retries];
      return_val = BTHD_P_AcquireChangeFFTWindow(h,FFTWindowMode);
      if (return_val == THD_AcquireResult_NoFFTLock)
	      NextState = THD_State_Done;
      else
#ifndef ChangeFFTWindowSeamless
	      NextState = THD_State_SP;
#else
	      NextState = THD_State_FEC;
#endif      
      break;

    case THD_State_Track:
      BDBG_MSG(("\tTrack"));
	    BTHD_P_IsdbtResetLockSetClrFlag(h);   /* clear previous interrupt flags */
      ChannelEstimatorMode = ChannelEstimatorModeList[retries];
      return_val = BTHD_P_AcquireTrack(h,FFTWindowMode,ChannelEstimatorMode);
      if (return_val == THD_AcquireResult_AbortedEarly ) {
      	NextState = THD_State_Done;
      }
      else {
	      NextState = THD_State_CheckLock;
      }
      break;

    case THD_State_CheckLock:
      BDBG_MSG(("\tCheckLock"));
      return_val = BTHD_P_AcquireCheckLock(h);
      if (return_val == THD_AcquireResult_NoFECLock) {
	      if (retries < ChannelEstimatorRetries) {
	        retries++;
	        NextState = THD_State_ChangeChannelEstimator;
	      } else
	        NextState = THD_State_Done;
      } else
	      NextState = THD_State_Done;
      break;

    case THD_State_ChangeChannelEstimator:
      BDBG_MSG(("\tChangeChannelestimator"));
      ChannelEstimatorMode = ChannelEstimatorModeList[retries];
      return_val = BTHD_P_AcquireChangeChannelEstimator(h,ChannelEstimatorMode);
      NextState = THD_State_CheckLock;
      break;

    case THD_State_Done:
      BDBG_MSG(("\tDone"));
      h->pInternalAcquireParam->AllowRsSyncEvent = false;
      BKNI_ResetEvent(h->hRsSyncEvent);      
      Done = true;
#ifdef EMULATION_ENABLE
	  BREG_Write32(h->hRegister,  BCHP_THD_INTR_AP_SET, 0x00000080 );
#endif
      break;
    }

    acqProfile[State] = BREG_Read32(h->hRegister, BCHP_THD_CORE_FSCNT);
    acqProfileValid[State] = true;
    if (!Done)
      LastState=State;
    State=NextState;
  }

  /* Update status structure with auto-detected modes */
  h->pStatus->ThdCommonStatus.ChannelEstimatorMode = ChannelEstimatorMode;
  h->pStatus->ThdCommonStatus.FFTWindowMode = FFTWindowMode;

  /* Report acquisition time */
  if (LastState > THD_State_Track)   	LastState = THD_State_Track;
  h->pStatus->ThdCommonStatus.AcquisitionTime = (fscntInit - acqProfile[LastState])/54000;
  BDBG_MSG(("BTHD_P_Acquire:\tAcquisition Time = %d msec",h->pStatus->ThdCommonStatus.AcquisitionTime));  
  /*
  for (k=0; k<16; k++)
    if (acqProfileValid[k])
      BDBG_MSG(("\t\t%d msec: %s", (fscntInit - acqProfile[k])/54000, acqProfileString[k]));
  */  
  
  return(return_val);
}

#endif /* BTHD_ISDBT_SUPPORT */

