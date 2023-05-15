/******************************************************************************
 *    (c)2011 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: baob_acquire.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/22/12 4:43p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /magnum/portinginterface/aob/7552/baob_acquire.h $
 * 
 * Hydra_Software_Devel/1   6/22/12 4:43p farshidf
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure
 * 
 * 5   3/25/11 6:32p mpovich
 * SW3128-1: Merge latest from AOB branch to the main branch.
 * 
 * AOB_3128_1/4   3/23/11 1:04p mpovich
 * SW3128-1: Rebase AOB from main branch
 * 
 * 4   3/22/11 5:03p mpovich
 * SW3128-1: Add latest AOB driver changes.
 * 
 * AOB_3128_1/3   3/21/11 6:25p farshidf
 * SW3461-1: update naming
 * 
 * AOB_3128_1/2   3/18/11 4:37p cbrooks
 * sw3128-1:Added Status for OOB
 * 
 * AOB_3128_1/1   3/17/11 6:16p cbrooks
 * sw3128-1:Added OOB code
 * 
 * 3   3/11/11 3:49p farshidf
 * SW3128-1: latest chnages from Charlie
 * 
 * 2   3/8/11 2:45p cbrooks
 * sw3128-1:NEW CODE
 * 
 * 1   2/23/11 7:09p cbrooks
 * sw3128-1:new code
 * 
 *****************************************************************************/
#ifndef BAOB_ACQUIRE_H__
#define BAOB_ACQUIRE_H__

#ifdef __cplusplus
extern "C" {
#endif
 
#define CLOCK_INVERT					 0 /*1 to invert clock,  0 to not invert clock*/
#define BAOB_IF_FREQUENCY			 0 /*BAOB_P_Set_CFL_Frequency() currently only supports 0*/
#define DAGC_FREEZE						 0 /*1 to freeze,  0 to not freeze*/
#define AGC_WAIT_TIME					10 /*AGC wait time in ms*/ 
#define BAUD_WAIT_TIME				50 /*Baud Loop wait time in ms*/ 
#define CARRIER_WAIT_TIME			50 /*Carrier Loop wait time in ms*/ 
#define ANNEXA_FEC_WAIT_TIME  1000 /*FEC wait time in ms*/ 
#define ANNEXB_FEC_WAIT_TIME  100  /*FEC wait time in ms*/ 
#define CABLECARD_WAIT_TIME    10  /*FEC wait time in ms*/ 

/*****************************************************************************
 * ADS Function Prototypes Used by PI or Local 
 *****************************************************************************/
BERR_Code BAOB_P_PowerDn(BAOB_3x7x_Handle h);
BERR_Code BAOB_P_PowerUp(BAOB_3x7x_Handle h);
BERR_Code BAOB_P_Init(BAOB_3x7x_Handle h);
BERR_Code BAOB_P_Get_LockStatus(BAOB_3x7x_Handle h);
BERR_Code BAOB_P_Acquire(BAOB_3x7x_Handle h);

/*****************************************************************************
 * OOB Function Prototypes Used Local 
 *****************************************************************************/


#ifdef __cplusplus
}
#endif

#endif