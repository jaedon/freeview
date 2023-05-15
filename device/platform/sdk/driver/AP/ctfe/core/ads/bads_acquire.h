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
 * $brcm_Workfile: bads_acquire.h $
 * $brcm_Revision: 30 $
 * $brcm_Date: 9/5/12 11:25a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/ads/bads_acquire.h $
 * 
 * 30   9/5/12 11:25a farshidf
 * SW3128-213: 3128 Fw release 5.6
 * 
 * Fw_Integration_Devel/8   8/30/12 2:18p farshidf
 * SW3128-213: 3128 Fw release 5.6
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/3   8/6/12 6:33p cbrooks
 * sw3128-1:Code Cleanup
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/2   7/18/12 4:43p cbrooks
 * sw3128-1:rearranged function to start cleanup
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/1   4/27/12 5:21p farshidf
 * SW3462-19: merge to 5.0 branch
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/SW3462-6/1   2/28/12 6:02p mpovich
 * SW3462-6: Rebase with SW3462-3 dev. branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3462-3/1   2/27/12 5:02p mpovich
 * SW3462-3: 3462 initial baseline development and initial HAB support.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3462-2/1   1/30/12 5:37p mpovich
 * SW3462-2: Support for 3462.
 * 
 * Fw_Integration_Devel/5   10/12/11 2:43p mpovich
 * SW3128-38: Merge to integ. branch from devel. branch.
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/2   10/12/11 11:45a mpovich
 * SW3128-38: Merge ADS Early Exit feature to devel. branch.
 * 
 * 26   10/12/11 11:26a farshidf
 * SW3128-54: merge to main
 * 
 * Fw_Integration_Devel/4   10/12/11 10:40a farshidf
 * SW3128-54: merge to Integ branch
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/1   10/6/11 11:47a farshidf
 * SW3128-1: update for 3128c0
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/SW3128-38/1   10/5/11 4:19p mpovich
 * SW3128-38: Early exit feature - encapsulate early exit flag within
 *  bads_acquire module.
 * 
 * 25   8/12/11 3:12p farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/2   8/12/11 2:10p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/4   7/27/11 7:58p cbrooks
 * sw3128-1:added early exit
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/3   7/15/11 6:43p farshidf
 * SWDTV-7869: fix
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/2   7/15/11 6:27p farshidf
 * SWDTV-7869: fix compile error
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/1   7/15/11 6:22p farshidf
 * SWDTV-7869: add the new FFT function for B0
 * 
 * 23   6/9/11 6:14p mpovich
 * SW3461-1: Merge Ver 0.4 Integ. onto main branch.
 * 
 * SW_System_4_Integ_Test/2   6/9/11 2:15p mpovich
 * SW3461-1: Rebase with main branch.
 * 
 * 22   6/7/11 3:14p farshidf
 * SW3128-1: merge to main
 * 
 * SW_System_4_Integ_Test/1   6/6/11 1:57p mpovich
 * SW3461-1: Integ. test all development branches together.
 * 
 * Ads_Fw_Devel_3/1   5/27/11 12:28p cbrooks
 * sw3128-1:Added IMC and CWC
 * 
 * 21   4/28/11 3:57p farshidf
 * SW3128-1: merge to main
 * 
 * ADS_3128_3/1   4/28/11 1:08p cbrooks
 * sw3128-1:New Code for scan
 * 
 * 20   4/26/11 6:49p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_2/5   4/26/11 4:53p farshidf
 * SW3128-1: merge main
 * 
 * 19   4/18/11 5:32p farshidf
 * SW3128-1: update to make it work with host chip
 * 
 * 18   4/15/11 4:48p farshidf
 * SW3128-1: merge main
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
 * 16   1/31/11 7:55p cbrooks
 * sw3128-1:new code
 * 
 * 15   1/30/11 6:35p cbrooks
 * sw3128-1:FFT Code
 * 
 * 14   1/26/11 6:54p cbrooks
 * sw3128-1:Fixed prototypes
 * 
 * 13   1/26/11 3:28p cbrooks
 * sw3128-1:Cleanup Code
 * 
 * 12   1/25/11 9:29p cbrooks
 * sw3128-1: Cleanup Code
 * 
 * 11   12/17/10 4:16p farshidf
 * SW3128-1: update
 * 
 * 
 ***************************************************************************/

#ifndef _BADS_ACQUIRE_H__
#define _BADS_ACQUIRE__H__           

#ifdef __cplusplus
extern "C" {
#endif


/*Combined Timing Loop Coefficients*/
#define BCHP_DS_TLC_COMBO_COEFFS_SHIFT BCHP_DS_TLC_TLICOEFF_SHIFT
#define BCHP_DS_TLC_COMBO_COEFFS_MASK (BCHP_DS_TLC_reserved1_MASK | BCHP_DS_TLC_TLLCOEFF_SIGN_MASK | BCHP_DS_TLC_TLLCOEFF_MASK | BCHP_DS_TLC_reserved2_MASK | BCHP_DS_TLC_TLICOEFF_SIGN_MASK | BCHP_DS_TLC_TLICOEFF_MASK)

/*Combined Frequency Loop Coefficients*/
#define BCHP_DS_CFLC_COMBO_COEFFS_SHIFT BCHP_DS_CFLC_CFLICOEFF_SHIFT
#define BCHP_DS_CFLC_COMBO_COEFFS_MASK (BCHP_DS_CFLC_reserved1_MASK | BCHP_DS_CFLC_CFLLCOEFF_SIGN_MASK | BCHP_DS_CFLC_CFLLCOEFF_MASK | BCHP_DS_CFLC_reserved2_MASK | BCHP_DS_CFLC_CFLICOEFF_SIGN_MASK | BCHP_DS_CFLC_CFLICOEFF_MASK)

/*Combined Phase Loop Coefficients*/
#define BCHP_DS_EQ_CPLC_COMBO_COEFFS_SHIFT BCHP_DS_EQ_CPLC_CPLICOEFF_SHIFT
#define BCHP_DS_EQ_CPLC_COMBO_COEFFS_MASK (BCHP_DS_EQ_CPLC_CPLLCOEFF_MASK | BCHP_DS_EQ_CPLC_CPLICOEFF_MASK)

/*Combined DFE Taps*/
#define BCHP_DS_FRZ_COMBO_DFEFRZ_SHIFT BCHP_DS_FRZ_DFEFRZ1_6_SHIFT
#define BCHP_DS_FRZ_COMBO_DFEFRZ_MASK (BCHP_DS_FRZ_DFEFRZ31_36_MASK | BCHP_DS_FRZ_DFEFRZ25_30_MASK | BCHP_DS_FRZ_DFEFRZ19_24_MASK | BCHP_DS_FRZ_DFEFRZ13_18_MASK | BCHP_DS_FRZ_DFEFRZ7_12_MASK | BCHP_DS_FRZ_DFEFRZ1_6_MASK)

/*combined FEC counter clears*/
#define BCHP_DS_TPFEC_CLEARCNT2_SHIFT  BCHP_DS_TPFEC_CLR_BMPG2_SHIFT
#define BCHP_DS_TPFEC_CLEARCNT2_MASK   (BCHP_DS_TPFEC_CLR_UERC2_MASK | BCHP_DS_TPFEC_CLR_NBERC2_MASK | BCHP_DS_TPFEC_CLR_CBERC2_MASK | BCHP_DS_TPFEC_CLR_BMPG2_MASK)

/*****************************************************************************
 * ADS Function Prototypes Used by PI or Local 
 *****************************************************************************/
BERR_Code BADS_P_Initialize(BADS_3x7x_ChannelHandle hChn);
BERR_Code BADS_P_ChnLockStatus(BADS_3x7x_ChannelHandle hChn);
BERR_Code BADS_P_Acquire(BADS_3x7x_ChannelHandle hChn);
BERR_Code BADS_P_AbortAcquire(BADS_3x7x_ChannelHandle hChn); /* Acquire Early Exit */
BERR_Code BADS_P_HAB_Read_FFEDFE(BADS_3x7x_ChannelHandle hChn, uint8_t *HAB_Buffer, uint8_t Size_HAB, uint8_t FFE);
BERR_Code BADS_P_Read_Constellation(
    BADS_3x7x_ChannelHandle hChn,       /* [in] Device channel handle */
    int16_t nbrToGet_i16,                /* [in] Number values to get */
    int16_t *iVal_pi16,                  /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal_pi16,                  /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten_pi16              /* [out] Number of values gotten/read */
    );

#ifdef __cplusplus
}
#endif

#endif /* _BADS_ACQUIRE_PRIV_H__ */