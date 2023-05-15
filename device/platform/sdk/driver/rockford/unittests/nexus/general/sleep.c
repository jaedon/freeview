/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: sleep.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/8/11 11:52a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/sleep.c $
 * 
 * Hydra_Software_Devel/2   8/8/11 11:52a erickson
 * SWDEPRECATED-3044: speed up tests
 * 
 * Hydra_Software_Devel/1   6/15/11 9:51a erickson
 * SWDEPRECATED-3044: add test
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "nexus_types.h"
#include "nexus_platform.h"

BDBG_MODULE(sleep);

unsigned b_get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

/* the essential requirement is that sleep/delay/waitforevetn not return in less time than requested. 
however, in an idle system, they shouldn't take much more. */
int main(void)
{
    NEXUS_Error rc;
    unsigned i, j, starttime, totaltime;
    BKNI_EventHandle event;

    srand(time(NULL));
    
    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);
    
    BKNI_CreateEvent(&event);
    
    for (i=0;i<100;) {
        unsigned loops, n;
        
        loops = 1 + rand() % 20;
        n = 1 + rand() % 50;
        starttime = b_get_time();
        for (j=0;j<loops;j++) {
            BKNI_Sleep(n);
        }
        totaltime = b_get_time() - starttime;
        BDBG_ASSERT(totaltime >= loops * n);
#if 0
        BDBG_ASSERT(totaltime < loops * n * 3 / 2);
#endif

        starttime = b_get_time();
        for (j=0;j<loops;j++) {
            BKNI_Delay(n*1000);
        }
        totaltime = b_get_time() - starttime;
        BDBG_ASSERT(totaltime >= loops * n);
#if 0
        BDBG_ASSERT(totaltime < loops * n * 3 / 2);
#endif
        
        starttime = b_get_time();
        for (j=0;j<loops;j++) {
            rc = BKNI_WaitForEvent(event, n);
            BDBG_ASSERT(rc == NEXUS_TIMEOUT);
        }
        totaltime = b_get_time() - starttime;
        BDBG_ASSERT(totaltime >= loops * n);
#if 0
/* WaitForEvent can take up to 20x more */
        BDBG_ASSERT(totaltime < loops * n * 10);
#endif
        
        if (++i % 20 == 0) BDBG_WRN(("%d loops", i));
    }
    
    BKNI_DestroyEvent(event);
    NEXUS_Platform_Uninit();

    return 0;
}

