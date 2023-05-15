/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: set_stc.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/6/09 11:32a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/set_stc.c $
 * 
 * Hydra_Software_Devel/1   11/6/09 11:32a erickson
 * SW7405-3287: verify that each StcChannel can manage its own STC value,
 * even with a shared stcIndex
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_stc_channel.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

BDBG_MODULE(change_timebase);

#if defined(LINUX)
#include <sys/time.h>
#elif defined(VXWORKS)
#include <timeCommon.h>
#elif defined (NEXUS_BASE_OS_ucos)
#include "type.h"
#include "ucos.h"
#endif
unsigned b_get_time(void)
{
#if defined(LINUX)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
#elif defined(VXWORKS)
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec*1000 + tv.tv_nsec/1000/1000;
#elif defined(NEXUS_BASE_OS_ucos)
    const unsigned msPerTick = 10;
    long osTicks = OSTimeGet();
    return (osTicks * msPerTick);
#else
#error Please implement b_get_time for your OS
#endif
}

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel[NEXUS_NUM_STC_CHANNELS];
    uint32_t lastStc[NEXUS_NUM_STC_CHANNELS];
    unsigned lastTime[NEXUS_NUM_STC_CHANNELS];
    unsigned loops = 0;
    unsigned i;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    for (i=0;i<NEXUS_NUM_STC_CHANNELS;i++) {
        NEXUS_StcChannelSettings stcSettings;
        NEXUS_StcChannel_GetDefaultSettings(i, &stcSettings);
        stcSettings.timebase = NEXUS_Timebase_e0;
        stcSettings.mode = NEXUS_StcChannelMode_eHost;
        stcSettings.modeSettings.host.transportType = NEXUS_TransportType_eTs;
        stcSettings.stcIndex = i % NEXUS_NUM_TIMEBASES;
        stcChannel[i] = NEXUS_StcChannel_Open(i, &stcSettings);
        NEXUS_StcChannel_GetStc(stcChannel[i], &lastStc[i]);
        lastTime[i] = b_get_time();
    }

    while (++loops < 2000) {
        unsigned i = rand() % NEXUS_NUM_STC_CHANNELS;
        uint32_t stc;
        unsigned now = b_get_time();
        int diff;

        if (loops % 10 == 0) printf("loops %d\n", loops);
        switch (rand() % 3) {
        case 0:
            NEXUS_StcChannel_GetStc(stcChannel[i], &stc);
            diff = stc;
            diff -= lastStc[i] + (now - lastTime[i]) * 45;
            if (diff < 0) diff *= -1;
            if (diff > 45 * 2) {
                BDBG_ERR(("fail %x, %x, %x, %d, %d, %d", stc, lastStc[i], stc-lastStc[i], now, lastTime[i], now - lastTime[i]));
            }
            BDBG_ASSERT(diff < 45 * 2); /* within 2 msec */
            if (rand() % 2) {
                lastStc[i] = stc;
                lastTime[i] = now;
            }
            break;
        case 1:
            lastStc[i] += rand() % 0x1000000;
            NEXUS_StcChannel_SetStc(stcChannel[i], lastStc[i]);
            lastTime[i] = now;
            printf("set stc[%d] to %x at %d\n", i, lastStc[i], lastTime[i]);
            break;
        case 2:
            BKNI_Sleep(rand() % 200);
            break;
        }
    }

    for (i=0;i<NEXUS_NUM_STC_CHANNELS;i++) {
        NEXUS_StcChannel_Close(stcChannel[i]);
    }

    NEXUS_Platform_Uninit();

    return 0;
}
