/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: b_os_time.c $
* $brcm_Revision: 3 $
* $brcm_Date: 5/18/11 4:41p $
*
* Description:
*   API name: OS Time
*    Library routines for Time abstraction
*
* Revision History:
*
* $brcm_Log: /nexus/lib/os/src/ucos_ii/b_os_time.c $
* 
* 3   5/18/11 4:41p agin
* SWNOOS-458:  Fix compiler warnings.
* 
* 2   3/22/11 12:57a agin
* SWNOOS-448:  Support RMAGNUM interface over the SCB bus.
* 
* 1   2/17/11 6:45a agin
* SWNOOS-441:  Support UCOS Diags.
* 
***************************************************************************/
#include "b_os_lib.h"
#include "b_os_priv.h"
#include "bdbg.h"
#include "ucos_ii.h"

BDBG_MODULE(b_os_time);

extern uint32_t total_ticks;

void B_Time_Get(B_Time *pTime)
{
    total_ticks=OSTimeGet();
    pTime->tv_sec = total_ticks/100;
    pTime->tv_usec = (total_ticks%100)*10000;
}

long B_Time_Diff(const B_Time *pFuture, const B_Time *pPast)
{
    /* NOTE: the b_timer impl in this file requires that past/future inversion return
    a negative value. please adjust b_timer usage if this functionality changes. */
    return 1000*(pFuture->tv_sec - pPast->tv_sec) + (pFuture->tv_usec - pPast->tv_usec)/1000;
}

void B_Time_Add(B_Time *pTime, long deltaMsec)
{
	pTime->tv_usec += (deltaMsec%1000) * 1000;
	pTime->tv_sec += deltaMsec/1000;
	if (pTime->tv_usec > 1000000) {
		pTime->tv_usec -= 1000000;
		pTime->tv_sec++;
	}
	return;
}


