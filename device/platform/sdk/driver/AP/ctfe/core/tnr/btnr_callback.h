/*************************************************************************
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
* $brcm_Workfile: btnr_callback.h $
* $brcm_Revision: 6 $
* $brcm_Date: 5/17/12 5:35p $
*
* [File Description:]
*
* Revision History:
*
* $brcm_Log: /AP/ctfe/core/tnr/btnr_callback.h $
* 
* 6   5/17/12 5:35p farshidf
* SW3461-211: FW release 5.0
* 
* Fw_Integration_Devel/4   5/17/12 3:55p farshidf
* SW3461-211: FW release 5.0
* 
* Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   5/7/12 6:31p farshidf
* SW3472-2: Make the tuner multi channel
* 
* 5   8/12/11 3:14p farshidf
* SW3461-1: merge to main
* 
* Fw_Integration_Devel/3   8/12/11 2:35p farshidf
* SW3461-1: merge to main
* 
* 4   7/27/11 1:51p farshidf
* SW3461-1: merge to integ/ main
* 
* Fw_Integration_Devel/2   7/27/11 1:50p farshidf
* SW3461-1: merge to integ/ main
* 
* Fw_Integration_Devel/AP_V0_6_TNR_DEV/1   7/27/11 1:36p farshidf
* SW3461-1: magnum compatible
* 
* 3   7/20/11 11:36a farshidf
* SW7552-60: mereg the latest tuner code
* 
* Fw_Integration_Devel/1   6/29/11 12:39p farshidf
* SW3461-13: merge to integration branch
* 
* Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/1   6/21/11 5:15p cbrooks
* sw3461-1:Changed Callback Function
* 
* 2   5/20/11 6:42a mpovich
* SW3461-1: rename UFE (BUFE) module to TNR (BTNR).
* 
* TNR_3461_1/1   5/19/11 5:11p mpovich
* SW3461-1: Change BUFE module prefix to BTNR
* 
* 1   5/17/11 5:13p farshidf
* SW3461-1: add the callbacks
* 
* 1   5/17/11 1:40p farshidf
* SW3128-1: add the new callbacks
* 
***************************************************************************/

#ifndef _BTNR_CALLBACK_ADS_H__
#define _BTNR_CALLBACK_ADS_H__

#if __cplusplus
extern "C" {
#endif

/*API Functions in bwfe_callback_to_ads.c*/
BERR_Code BTNR_P_Set_RF_Offset(BTNR_3x7x_ChnHandle hChn, int32_t RF_Offset, uint32_t Symbol_Rate);
BERR_Code BTNR_P_Get_RF_Status(BTNR_3x7x_ChnHandle hChn);
BERR_Code BTNR_P_LNA_AGC_Power(BTNR_3x7x_ChnHandle hTnr, uint16_t Mode);

#ifdef __cplusplus
}
#endif

#endif /* _BWFE_CALLACK_TO_ADS_H__ */