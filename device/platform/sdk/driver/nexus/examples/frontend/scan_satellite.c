/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: scan_satellite.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 8/7/12 5:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/frontend/scan_satellite.c $
 * 
 * 5   8/7/12 5:37p mphillip
 * SW7231-924: Merge NEXUS_Frontend_Acquire changes to main
 * 
 * cng_unittest/1   7/30/12 5:01p cng
 * SW7231-924: Use NEXUS_Frontend_Acquire
 * 
 * 4   6/3/09 3:57p jtna
 * PR54416: test setting frequencyRange=0
 * 
 * 3   5/21/09 6:05p jtna
 * PR54416: use new naming scheme
 * 
 * 2   5/19/09 4:48p jtna
 * PR54416: update
 * 
 * 1   5/15/09 7:32p jtna
 * PR54416: added scan_satellite.c
 * 
 *****************************************************************************/
/* Example to do a satellite peak scan followed by a blind acquisition */

#include "nexus_frontend.h"
#include "nexus_platform.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MANUAL_MODE 0

BKNI_EventHandle psCallbackEvent;
BKNI_EventHandle lockCallbackEvent;
NEXUS_FrontendSatellitePeakscanResult psResult;

static void peakscan_callback(void *context, int param) 
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendSatellitePeakscanResult result;
    NEXUS_Error rc;

    BSTD_UNUSED(param);

    rc = NEXUS_Frontend_GetSatellitePeakscanResult(frontend, &result);
    BDBG_ASSERT(!rc);
    fprintf(stderr, "Peak scan callback. freq=%u, symRate=%u, power=%#x\n", 
        result.peakFrequency, result.symbolRate, result.peakPower);
    psResult = result;
    BKNI_SetEvent(psCallbackEvent);
}

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendSatelliteStatus status;

    BSTD_UNUSED(param);

    NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
    fprintf(stderr, "Lock callback. symRate=%u, locked=%u, mode=%u\n", 
        status.settings.symbolRate, status.demodLocked, status.mode);
    BKNI_SetEvent(lockCallbackEvent);
}

int main(void)
{
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendSatellitePeakscanSettings psSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendAcquireSettings settings;
    NEXUS_Error rc;
    unsigned input;

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
    settings.capabilities.satellite = true;
    frontend = NEXUS_Frontend_Acquire(&settings);
    if (!frontend) {
        fprintf(stderr, "Unable to find satellite-capable frontend\n");
        return -1;
    }
    BDBG_SetModuleLevel("nexus_frontend_ast",BDBG_eMsg);

    BKNI_CreateEvent(&psCallbackEvent);
    BKNI_CreateEvent(&lockCallbackEvent);
#if !MANUAL_MODE
    NEXUS_Frontend_GetDefaultSatellitePeakscanSettings(&psSettings);
    psSettings.frequency = 1120 * 1000000;
    /* use defaults for frequencyRange and frequencyStep */
    psSettings.peakscanCallback.callback = peakscan_callback;
    psSettings.peakscanCallback.context = frontend;

    while (1) {
        printf("Press Enter to start peak scan\n");
        getchar();
        rc = NEXUS_Frontend_SatellitePeakscan(frontend, &psSettings);
        BDBG_ASSERT(!rc);
        rc = BKNI_WaitForEvent(psCallbackEvent, 3000);
        if (rc) {
            printf("Callback not fired. Aborting\n");
            rc = 1;
            goto done;
        }
        else {
            if (!psResult.peakFrequency) {
                printf("No signal found. Continuing peak scan...\n");
                psSettings.frequency = psResult.lastFrequency + psSettings.frequencyRange + psSettings.frequencyStep;
                continue;
            }
            /* found an interesting peak frequency. try blind acquisition */
            printf("Press Enter to attempt blind acquisition\n");
            getchar();
            NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
            satSettings.mode = NEXUS_FrontendSatelliteMode_eBlindAcquisition;
            satSettings.lockCallback.callback = lock_callback;
            satSettings.lockCallback.context = frontend;
            satSettings.frequency = psResult.peakFrequency;
            satSettings.symbolRate = psResult.symbolRate;
            satSettings.carrierOffset = 0;
            rc = NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
            BDBG_ASSERT(!rc);

            rc = BKNI_WaitForEvent(lockCallbackEvent, 3000);
            if (rc) {
                printf("Could not lock. Continuing peak scan...\n");
                psSettings.frequency = psResult.lastFrequency + psSettings.frequencyRange + psSettings.frequencyStep;
                continue;
            }
            else {
                /* finished */
                printf("Enter 0 to quit, 1 to continue\n");
                scanf("%u", &input);
                if (input==0) {
                    rc = 0;
                    goto done;
                }
                else {
                    psSettings.frequency = psResult.lastFrequency + psSettings.frequencyRange + psSettings.frequencyStep;
                    continue;
                }
            }
        }
    }
#else
    NEXUS_Frontend_GetDefaultSatellitePeakscanSettings(&psSettings);
    psSettings.frequencyRange = 0;
    psSettings.peakscanCallback.callback = peakscan_callback;
    psSettings.peakscanCallback.context = frontend;
    psSettings.frequency = 1110 * 1000000;

    printf("Enter 0 to quit, 1 to continue to next frequency, 2 to try blind acquisition\n");
    for (i=0; i<20; i++) {
        scanf("%u", &input);
        switch (input) {
            case 0:
                rc = 0;
                goto done;
                break;
            case 1:
                psSettings.frequency += 1 * 1000000;
                rc = NEXUS_Frontend_SatellitePeakscan(frontend, &psSettings);
                BDBG_ASSERT(!rc);
                rc = BKNI_WaitForEvent(psCallbackEvent, 3000);
                if (rc) {
                    printf("Callback not fired. Aborting\n");
                    rc = 1;
                    goto done;
                }
                continue;
                break;
            case 2:
            default:
                NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
                satSettings.mode = NEXUS_FrontendSatelliteMode_eBlindAcquisition;
                satSettings.lockCallback.callback = lock_callback;
                satSettings.lockCallback.context = frontend;
                satSettings.frequency = psResult.peakFrequency;
                satSettings.symbolRate = psResult.symbolRate;
                satSettings.carrierOffset = 0;
                rc = NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
                BDBG_ASSERT(!rc);

                rc = BKNI_WaitForEvent(lockCallbackEvent, 3000);
                if (rc) {
                    printf("Could not lock. Press Enter to continue\n");
                    getchar();
                }
                break;
        }
    }


#endif

done:
    BKNI_DestroyEvent(psCallbackEvent);
    BKNI_DestroyEvent(lockCallbackEvent);
    NEXUS_Frontend_Release(frontend);
    NEXUS_Platform_Uninit();
    return rc;
}
