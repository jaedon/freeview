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
 * $brcm_Workfile: bads_utils.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 9/5/12 11:26a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/ads/bads_utils.h $
 * 
 * 8   9/5/12 11:26a farshidf
 * SW3128-213: 3128 Fw release 5.6
 * 
 * Fw_Integration_Devel/2   8/30/12 2:19p farshidf
 * SW3128-213: 3128 Fw release 5.6
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/3   8/17/12 4:23p cbrooks
 * sw3128-1:Code Cleanup
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/2   8/6/12 6:34p cbrooks
 * sw3128-1:Code Cleanup
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/1   7/18/12 4:43p cbrooks
 * sw3128-1:rearranged functions to start cleanup
 * 
 * 6   3/18/11 4:33p farshidf
 * SW3461-1: merge  main
 * 
 * ADS_3128_1/1   3/15/11 5:14p cbrooks
 * bcm3128-1:Added new HAB for CWC and changed FFEDFE command
 * 
 * 5   2/8/11 11:48a farshidf
 * SW3128-1: update the constellation function
 * 
 * 4   1/26/11 6:54p cbrooks
 * sw3128-1:Fixed prototypes
 * 
 * 3   1/26/11 3:28p cbrooks
 * sw3128-1:Cleanup Code
 * 
 * 2   1/25/11 9:30p cbrooks
 * sw3128-1: Cleanup Code
 * 
 * 1   12/17/10 4:12p farshidf
 * SW3128-1: new files
 * 
 ***************************************************************************/
#ifndef BADS_UTILS_H__
#define BADS_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * ADS Function Prototypes Used Local 
 *****************************************************************************/
BERR_Code BADS_P_Range_Check(BADS_3x7x_ChannelHandle);
BERR_Code BADS_P_Set_CFL_Frequency(BADS_3x7x_ChannelHandle hChn, uint32_t F_HS_u32, int32_t CFL_Frequency_i32);
BERR_Code BADS_P_Set_TL_Frequency(BADS_3x7x_ChannelHandle hChn, uint32_t F_1S_u32, uint32_t Symbol_Rate);
BERR_Code BADS_P_Get_CFL_Frequency(BADS_3x7x_ChannelHandle hChn, uint32_t F_HS_u32, int32_t *CFL_Frequency_pi32);
BERR_Code BADS_P_Get_TL_Frequency(BADS_3x7x_ChannelHandle hChn, uint32_t F_1S_u32, uint32_t *TL_Frequency_pu32);
BERR_Code BADS_P_Get_CFL_Error(BADS_3x7x_ChannelHandle hChn, uint32_t F_HS_u32, int32_t *CFL_Error_pi32);
BERR_Code BADS_P_Get_VID_Error(BADS_3x7x_ChannelHandle hChn, uint32_t F_1S_u32, int32_t *VID_Error_pi32);
BERR_Code BADS_P_Get_CPL_Error(BADS_3x7x_ChannelHandle hChn, uint32_t Symbol_Rate_u32, int32_t *CPL_Error_pi32);
BERR_Code BADS_P_Get_TimingScan_Advanced_FFT(BADS_3x7x_ChannelHandle hChn, uint32_t Upper_Baud_Search, bool ReturnBin, uint32_t *TimingScanResult_pu32);
BERR_Code BADS_P_Get_CarrierScan_Advanced_FFT(BADS_3x7x_ChannelHandle hChn, uint32_t Symbol_Rate, int32_t *CarrierScanResult_pi32);
void BADS_P_ProgramFEC(BADS_3x7x_ChannelHandle hChn);
void BADS_P_AcquisitionPercentageTest(BADS_3x7x_ChannelHandle hChn);
BERR_Code BADS_P_Set_CWC_Auto(BADS_3x7x_ChannelHandle hChn, uint32_t Symbol_Rate, int32_t CWC_Offset_Freq, uint8_t *CWC_LengthResult_pu8);
void BADS_P_Get_AcquisitionScan_Settings(BADS_3x7x_ChannelHandle hChn);
void BADS_P_Set_ScanStatus_Params(BADS_3x7x_ChannelHandle hChn, uint8_t QamIndex_u8, uint8_t FECIndex_u8, int32_t CarrierOffset_i32, int32_t Carrier_Error_i32, uint32_t Symbol_Rate_u32); 
bool BADS_P_ADS_SLEEP(BADS_3x7x_ChannelHandle hChn, unsigned int Delay);	

#ifdef __cplusplus
}
#endif

#endif