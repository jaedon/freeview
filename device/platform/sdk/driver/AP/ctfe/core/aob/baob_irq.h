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
* $brcm_Workfile: baob_irq.h $
* $brcm_Revision: 2 $
* $brcm_Date: 11/3/11 12:07p $
*
* [File Description:]
*
* Revision History:
*
* $brcm_Log: /AP/ctfe/core/aob/baob_irq.h $
* 
* 2   11/3/11 12:07p farshidf
* SW3128-60: merge to main
* 
* Fw_Integration_Devel/1   11/3/11 11:45a farshidf
* SW3128-60: merge to Integ
* 
* Fw_Integration_Devel/AP_V3_0_AOB_DEV/1   10/28/11 2:24p farshidf
* SW3461-75: move to new IRQ handler
* 
* 1   3/21/11 5:57p farshidf
* SW3128-1: add irq for AOB
* 
* 1   1/17/11 2:59p farshidf
* SW3128-1: add the aob irq
* 
***************************************************************************/

#ifndef _INT_AOB_H
#define _INT_AOB_H

#if __cplusplus
extern "C" {
#endif

#include "bchp_leap_l1.h"
#include "bint.h"


#define BCHP_INT_ID_AOB_SNR_LS_INTR           ((BCHP_LEAP_L1_INTR_STATUS_OOB_INTR_SHIFT << 8) | BCHP_OOB_IRQSTA2A_SNR_LS_2A_SHIFT)
#define BCHP_INT_ID_AOB_SNR_IS_INTR           ((BCHP_LEAP_L1_INTR_STATUS_OOB_INTR_SHIFT << 8) | BCHP_OOB_IRQSTA2A_SNR_IS_2A_SHIFT)
#define BCHP_INT_ID_AOB_FEC_LS_INTR           ((BCHP_LEAP_L1_INTR_STATUS_OOB_INTR_SHIFT << 8) | BCHP_OOB_IRQSTA2A_FEC_LS_2A_SHIFT)
#define BCHP_INT_ID_AOB_FEC_IS_INTR           ((BCHP_LEAP_L1_INTR_STATUS_OOB_INTR_SHIFT << 8) | BCHP_OOB_IRQSTA2A_FEC_IS_2A_SHIFT)



#if __cplusplus
}
#endif

#endif /* _INT_AOB_H */

