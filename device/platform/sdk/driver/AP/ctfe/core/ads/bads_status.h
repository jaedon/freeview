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
 * $brcm_Workfile: bads_status.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/5/12 11:26a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/ads/bads_status.h $
 * 
 * 4   9/5/12 11:26a farshidf
 * SW3128-213: 3128 Fw release 5.6
 * 
 * Fw_Integration_Devel/1   8/30/12 2:19p farshidf
 * SW3128-213: 3128 Fw release 5.6
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/1   8/6/12 6:34p cbrooks
 * sw3128-1:Code Cleanup
 * 
 * 3   1/26/11 6:54p cbrooks
 * sw3128-1:Fixed prototypes
 * 
 * 2   1/26/11 3:28p cbrooks
 * sw3128-1:Cleanup Code
 * 
 * 1   12/17/10 4:11p farshidf
 * SW3128-1: new files
 * 
 ***************************************************************************/
#ifndef BADS_STATUS_H__
#define BADS_STATUS_H__

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * ADS Function Prototypes Used by PI or Local 
 *****************************************************************************/
void BADS_P_ChnStatusReset(BADS_3x7x_ChannelHandle hChn);
BERR_Code BADS_P_ChnStatus(BADS_3x7x_ChannelHandle hChn);

#ifdef __cplusplus
}
#endif

#endif