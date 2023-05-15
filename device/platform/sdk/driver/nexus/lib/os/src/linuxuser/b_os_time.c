/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Revision: 4 $
* $brcm_Date: 4/10/12 5:30p $
*
* Description:
*   API name: OS Time
*    Library routines for Time abstraction
*
* Revision History:
*
* $brcm_Log: /nexus/lib/os/src/linuxuser/b_os_time.c $
* 
* 4   4/10/12 5:30p erickson
* SW7425-2859: use clock_gettime(CLOCK_MONOTONIC) instead of
*  gettimeofday.
* 
* 3   3/31/08 1:13p jgarrett
* PR 41043: Making time routines public
* 
* 2   3/10/08 8:54p jgarrett
* PR 40306: Fixing typedef
* 
* 1   3/10/08 1:57p jgarrett
* PR 40306: Adding oslib
* 
***************************************************************************/
#include "b_os_lib.h"
#include "b_os_priv.h"
#include <time.h>

BDBG_MODULE(b_os_time);

void B_Time_Get(B_Time *pTime)
{
    int rc;
    struct timespec ts;

    /* We no longer use gettimeofday. clock_gettime(CLOCK_MONOTONIC) is resilient to calendar time changes, which
    applications may need to perform. */
    rc = clock_gettime(CLOCK_MONOTONIC, &ts);
    if (rc!=0) { 
        BDBG_ERR(("clock_gettime returned %d", rc)); 
        pTime->tv_sec = 0;
        pTime->tv_usec = 0;
    }
    else {
        pTime->tv_sec = ts.tv_sec;
        pTime->tv_usec = ts.tv_nsec / 1000;
    }

    return;
}

long B_Time_Diff(const B_Time *pFuture, const B_Time *pPast)
{
    BDBG_ASSERT(NULL != pFuture);
    BDBG_ASSERT(NULL != pPast);

    return 1000*(pFuture->tv_sec - pPast->tv_sec) + (pFuture->tv_usec - pPast->tv_usec)/1000;
}

void B_Time_Add(B_Time *pTime, long deltaMsec)
{
    BDBG_ASSERT(NULL != pTime);
    pTime->tv_usec += (deltaMsec%1000) * 1000;
    pTime->tv_sec += deltaMsec/1000;
    if (pTime->tv_usec > 1000000) 
    {
        pTime->tv_usec -= 1000000;
        pTime->tv_sec++;
    }
}

