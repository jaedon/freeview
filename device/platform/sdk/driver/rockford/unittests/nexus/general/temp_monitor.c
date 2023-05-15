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
 * $brcm_Workfile: temp_monitor.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/7/12 12:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/temp_monitor.c $
 * 
 * Hydra_Software_Devel/1   3/7/12 12:51p erickson
 * SW7346-376: add temp_monitor unittest
 * 
 * Hydra_Software_Devel/1   3/7/12 12:50p erickson
 * SW7346-376: add temp_monitor unittest
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include <stdio.h>
#include <stdlib.h>

#if NEXUS_HAS_TEMP_MONITOR
#include "nexus_temp_monitor.h"

static void complete(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void print_status(NEXUS_TempMonitorHandle tempMonitor)
{
    int rc;
    NEXUS_TempMonitorStatus status;
    rc = NEXUS_TempMonitor_GetStatus(tempMonitor, &status);
    BDBG_ASSERT(!rc);
    if (!status.valid) {
        printf("status not valid\n");
    }
    else {
        printf("status: %#x,%#x,%#x; %d degrees C\n",
            status.top,
            status.middle,
            status.bottom,
            status.onChipTemperature);
    }
}

int main(void)
{
    NEXUS_Error rc;
    unsigned loops = 5;
    BKNI_EventHandle dataReadyEvent;
    
    srand(time(NULL));

    rc = NEXUS_Platform_Init(NULL);
    if (rc) return -1;
    
    BKNI_CreateEvent(&dataReadyEvent);

    while (loops--) {
        unsigned loops2 = 20;
        NEXUS_TempMonitorHandle tempMonitor;
        NEXUS_TempMonitorSettings settings;

        NEXUS_TempMonitor_GetDefaultSettings(&settings);
        tempMonitor = NEXUS_TempMonitor_Open(0, rand()%2?NULL:&settings);
        BDBG_ASSERT(tempMonitor);

#if 1
/* stress case */
        while (loops2--) {
            switch (rand()%3) {
            case 0:
                NEXUS_TempMonitor_GetSettings(tempMonitor, &settings);
                settings.enabled = rand()%2;
                settings.dataReady.callback = complete;
                settings.dataReady.context = dataReadyEvent;
#define MAX_INTERVAL 5000
                settings.measureInterval = rand()%MAX_INTERVAL;
                rc = NEXUS_TempMonitor_SetSettings(tempMonitor, &settings);
                BDBG_ASSERT(!rc);
                
#if 0
/* unless OTP is programmed, board will get no dataready interrupts */
                if (settings.enabled && settings.measureInterval) {
                    rc = BKNI_WaitForEvent(dataReadyEvent, MAX_INTERVAL*2);
                    BDBG_ASSERT(!rc);
                }
#endif
                break;
            case 1:
                print_status(tempMonitor);
                break;    
            case 2:
                BKNI_Sleep(rand()%3000);
                break;
            }
        }
#else
        /* simple case */
        NEXUS_TempMonitor_GetSettings(tempMonitor, &settings);
        settings.enabled = true;
        settings.dataReady.callback = complete;
        settings.dataReady.context = dataReadyEvent;
        settings.measureInterval = 1000;
        rc = NEXUS_TempMonitor_SetSettings(tempMonitor, &settings);
        BDBG_ASSERT(!rc);
        rc = BKNI_WaitForEvent(dataReadyEvent, 0xffffffff);
        BDBG_ASSERT(!rc);
        print_status(tempMonitor);
#endif

        NEXUS_TempMonitor_Close(tempMonitor);
    }

    BKNI_DestroyEvent(dataReadyEvent);
    NEXUS_Platform_Uninit();

    return 0;
}
#else
int main() { printf("no temp_monitor support\n"); return 0; }
#endif
