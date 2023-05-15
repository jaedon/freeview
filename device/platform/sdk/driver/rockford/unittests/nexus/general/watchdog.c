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
 * $brcm_Workfile: watchdog.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/3/11 4:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/watchdog.c $
 * 
 * Hydra_Software_Devel/2   6/3/11 4:27p erickson
 * SWDEPRECATED-3768: adjust min/max
 * 
 * Hydra_Software_Devel/1   4/20/09 10:38a jtna
 * PR54011: added watchdog
 * 
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "nexus_platform.h"
#include "nexus_watchdog.h"

#define TIMEOUT1 20
#define TIMEOUT2 60

int main(void)
{
    unsigned timeout, sleep, i;
    bool status;
    NEXUS_Error rc;

    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);

    /* Part1: sanity checks on basic functionality */
    rc = NEXUS_Watchdog_StopTimer();
    BDBG_ASSERT(!rc); /* stopping before starting should trigger a warning but still succeed */

    rc = NEXUS_Watchdog_StartTimer();
    BDBG_ASSERT(rc); /* start without first setting timeout value should fail */

    NEXUS_Watchdog_GetLastResetStatus(&status);
    BDBG_ASSERT(status==false);

    rc = NEXUS_Watchdog_SetTimeout(NEXUS_WATCHDOG_MIN_TIMEOUT-1);
    BDBG_ASSERT(rc);
    rc = NEXUS_Watchdog_SetTimeout(NEXUS_WATCHDOG_MAX_TIMEOUT+1);
    BDBG_ASSERT(rc);
    
    BKNI_Printf("\nSetting watchdog timeout for %u seconds\n", TIMEOUT1);
    rc = NEXUS_Watchdog_SetTimeout(TIMEOUT1);
    BDBG_ASSERT(!rc);

    BKNI_Printf("Overriding watchdog timeout for %u seconds\n", TIMEOUT2);
    rc = NEXUS_Watchdog_SetTimeout(TIMEOUT2);
    BDBG_ASSERT(!rc);

    BKNI_Printf("\nStarting watchdog timer...\n"); 
    rc = NEXUS_Watchdog_StartTimer();
    BDBG_ASSERT(!rc);

    BKNI_Sleep((TIMEOUT1+1)*1000);
    /* if the override of timeout value worked, we should still be alive here */
    NEXUS_Watchdog_StopTimer();
    BKNI_Printf("Still alive...\n");

    BKNI_Printf("Timer stopped after %u seconds, and waiting an extra %u seconds...\n", TIMEOUT1+1, TIMEOUT2-TIMEOUT1);
    BKNI_Sleep((TIMEOUT2-TIMEOUT1)*1000);
    /* If the StopTimer worked, we should still be alive here */
    BKNI_Printf("Still alive...\n");

    /* Part2: Test repeatedly restarting the timer */
    timeout = rand()%(NEXUS_WATCHDOG_MAX_TIMEOUT/4) + 1; /* minimum of 1 */

    BKNI_Printf("\nSetting watchdog timeout for %u seconds\n", timeout);
    rc = NEXUS_Watchdog_SetTimeout(timeout);
    BDBG_ASSERT(!rc);

    BKNI_Printf("Starting watchdog timer\n");
    rc = NEXUS_Watchdog_StartTimer();
    BDBG_ASSERT(!rc);

    for (i=0; i<10; i++) {
        sleep = (rand() % timeout);
        BKNI_Sleep(sleep*1000);
        BKNI_Printf("   Starting watchdog timer again before timeout (in %u seconds)\n", sleep);
        rc = NEXUS_Watchdog_StartTimer();
        BDBG_ASSERT(!rc);

        rc = NEXUS_Watchdog_SetTimeout(rand()); /* this should fail */
        BDBG_ASSERT(rc);
    }

    BKNI_Printf("Still alive...\n");
    NEXUS_Watchdog_StopTimer(); /* remember to stop the timer before we exit */

#if 1
    BKNI_Printf("Test completed\n");
    NEXUS_Platform_Uninit();
#else /* this should assert a chip reset and will break the automated unittest */
    /* Part3: Test if stopping the timer and starting it again maintains the timeout value */

    BKNI_Printf("Starting watchdog timer again (previous timeout value %u seconds)\n", timeout);
    NEXUS_Watchdog_StartTimer();
    sleep = 0;
    while(1) {
        BKNI_Sleep(1000);
        BKNI_Printf("%3u seconds elapsed\n", ++sleep);
    }
#endif

    return 0;
}

