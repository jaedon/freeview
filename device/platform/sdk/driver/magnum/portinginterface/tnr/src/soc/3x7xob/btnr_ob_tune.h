/***************************************************************************
 *     (c)2005-2011 Broadcom Corporation
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
 * $brcm_Workfile: btnr_ob_tune.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/7/12 10:27a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/3x7xob/btnr_ob_tune.h $
 * 
 * Hydra_Software_Devel/1   9/7/12 10:27a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/1   12/8/11 11:27p farshidf
 * SW7552-170: first version of OOB tuner for B0 verification
 * 
 * 
 ***************************************************************************/

#ifndef _BTNR_OB_TUNE_H__
#define _BTNR_OB_TUNE_H__           

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * TNR Function Prototypes Used by PI or Local 
 *****************************************************************************/
BERR_Code BTNR_Ob_P_Tuner_Power_Control(BTNR_Ob_3x7x_Handle h);
BERR_Code BTNR_Ob_P_TunerInit(BTNR_Ob_3x7x_Handle h);


/*****************************************************************************
 * TNR Function Prototypes Used Local 
 *****************************************************************************/

void BTNR_Ob_P_TunerSetLNAAGC(BTNR_Ob_3x7x_Handle h);
void BTNR_Ob_P_TunerSetRFAGC(BTNR_Ob_3x7x_Handle h);
void BTNR_Ob_P_CalFlashSDADC(BTNR_Ob_3x7x_Handle h);





#ifdef __cplusplus
}
#endif

#endif /* _BTNR_TUNE_H__ */


