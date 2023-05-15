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
 * $brcm_Workfile: tpit.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 1/19/12 4:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/tpit.c $
 * 
 * Hydra_Software_Devel/6   1/19/12 4:00p erickson
 * SW7425-2192: use NEXUS_TransportCapabilities.numTpitPids so test works
 * for all chips
 * 
 * Hydra_Software_Devel/5   10/27/11 7:07p bselva
 * SW7358-174: modify the MAX TPIT filters for 7358 platform.
 * 
 * Hydra_Software_Devel/4   7/8/11 1:51p erickson
 * SW7400-2645: fix uninitialized recpump variable. clean up pidchannels.
 * 
 * Hydra_Software_Devel/3   8/10/10 3:38p randyjew
 * SW7208-105: Fix compile warning for implicit declaration for time
 * 
 * Hydra_Software_Devel/2   7/29/10 12:35p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/1   1/8/10 10:09a erickson
 * SW7400-2645: added tpit stress test
 *
 * 2   5/29/09 3:06p erickson
 * PR54002: rework TPIT api to support simultaneous SCD indexing and TPIT
 *  filtering on the same pid
 *
 * 1   4/14/09 2:29p erickson
 * PR54002: added tpit example
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#include "nexus_transport_capabilities.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

BDBG_MODULE(tpit);

#define MAX_PID_CHANNELS 32

void dataready_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
}

int main(void) {
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle pidChannel[MAX_PID_CHANNELS];
    bool pidChannelAdded[MAX_PID_CHANNELS];
    bool pidChannelFilterSet[MAX_PID_CHANNELS];
    NEXUS_RecpumpHandle recpump = NULL;
    NEXUS_RecpumpOpenSettings recpumpOpenSettings;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpTpitFilter filter;
    NEXUS_TransportCapabilities transportCapabilities;
    NEXUS_Error rc;
    unsigned filterCount = 0;
    unsigned i;
    bool recpumpStarted = false;
    unsigned count;
    unsigned loops = 100000;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    srand(time(NULL));

    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);
    
    NEXUS_GetTransportCapabilities(&transportCapabilities);

    memset(pidChannelAdded, 0, sizeof(pidChannelAdded));
    memset(pidChannelFilterSet, 0, sizeof(pidChannelFilterSet));

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    for (i=0;i<MAX_PID_CHANNELS;i++) {
        pidChannel[i] = NEXUS_PidChannel_Open(parserBand, 0x11 + i, NULL);
        BDBG_ASSERT(pidChannel[i]);
    }

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    for (count=0;count<loops;) {
        unsigned pidIndex = rand() % MAX_PID_CHANNELS;

        switch (rand() % 8) {
        case 0:
            if (!recpump) {
                NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
                recpump = NEXUS_Recpump_Open(0, &recpumpOpenSettings);
                NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
                recpumpSettings.data.dataReady.callback = dataready_callback;
                recpumpSettings.index.dataReady.callback = dataready_callback; /* same callback */
                rc = NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 1:
            if (rand() % 70 == 0) {
                if (recpump) {
                    if (recpumpStarted) {
                        NEXUS_Recpump_Stop(recpump);
                    }
                    NEXUS_Recpump_RemoveAllPidChannels(recpump);
                    NEXUS_Recpump_Close(recpump);
                    recpump = NULL;
                    memset(pidChannelAdded, 0, sizeof(pidChannelAdded));
                    memset(pidChannelFilterSet, 0, sizeof(pidChannelFilterSet));
                    filterCount = 0;
                    recpumpStarted = false;
                }
            }
            break;
        case 2:
            if (recpump && !pidChannelAdded[pidIndex]) {
                rc = NEXUS_Recpump_AddPidChannel(recpump, pidChannel[pidIndex], NULL);
                BDBG_ASSERT(!rc);
                pidChannelAdded[pidIndex] = true;
            }
            break;
        case 3:
            if (rand() % 4 == 0) {
                if (pidChannelAdded[pidIndex]) {
                    rc = NEXUS_Recpump_RemovePidChannel(recpump, pidChannel[pidIndex]);
                    BDBG_ASSERT(!rc);
                    pidChannelAdded[pidIndex] = false;
                    if (pidChannelFilterSet[pidIndex]) {
                        pidChannelFilterSet[pidIndex] = false;
                        filterCount--;
                    }
                }
            }
            break;
        case 4:
            if (pidChannelAdded[pidIndex]) {
                NEXUS_Recpump_GetDefaultTpitFilter(&filter);
                filter.config.mpeg.randomAccessIndicatorEnable = true;
                filter.config.mpeg.randomAccessIndicatorCompValue = true;
                filter.config.mpeg.payloadUnitStartEnable = true;
                filter.config.mpeg.payloadUnitStartCompValue = true;
                if (!pidChannelFilterSet[pidIndex] && filterCount == transportCapabilities.numTpitPids) {
                    if (rand() % 10 == 0) {
                        rc = NEXUS_Recpump_SetTpitFilter(recpump, pidChannel[pidIndex], &filter);
                        BDBG_ASSERT(rc);
                        BDBG_WRN(("previous failure is expected."));
                        count++;
                    }
                }
                else {
                    rc = NEXUS_Recpump_SetTpitFilter(recpump, pidChannel[pidIndex], &filter);
                    BDBG_ASSERT(!rc);
                    count++;
                    if (count%5000==0) BDBG_WRN(("%d calls to NEXUS_Recpump_SetTpitFilter", count));
                    if (!pidChannelFilterSet[pidIndex]) {
                        pidChannelFilterSet[pidIndex] = true;
                        filterCount++;
                        BDBG_ASSERT(filterCount <= transportCapabilities.numTpitPids);
                        BDBG_MSG(("set tpit filter. %d total.", filterCount));
                    }
                }
            }
            break;
        case 5:
            if (rand() % 2 == 0) {
                if (pidChannelAdded[pidIndex]) {
                    rc = NEXUS_Recpump_SetTpitFilter(recpump, pidChannel[pidIndex], NULL);
                    BDBG_ASSERT(!rc);
                    count++;
                    if (count%5000==0) BDBG_WRN(("%d calls to NEXUS_Recpump_SetTpitFilter", count));
                    if (pidChannelFilterSet[pidIndex]) {
                        pidChannelFilterSet[pidIndex] = false;
                        BDBG_ASSERT(filterCount);
                        filterCount--;
                        BDBG_MSG(("unset tpit filter. %d total.", filterCount));
                    }
                }
            }
            break;
        case 6:
            if (recpump && !recpumpStarted && rand() % 3 == 0) {
                rc = NEXUS_Recpump_Start(recpump);
                BDBG_ASSERT(!rc);
                recpumpStarted = true;
            }
            break;
        case 7:
            if (recpumpStarted && rand() % 3 == 0) {
                NEXUS_Recpump_Stop(recpump);
                recpumpStarted = false;
            }
            break;
        }
    }

    if (recpump) {
        NEXUS_Recpump_Stop(recpump);
        NEXUS_Recpump_RemoveAllPidChannels(recpump);
        NEXUS_Recpump_Close(recpump);
    }
    for (i=0;i<MAX_PID_CHANNELS;i++) {
        NEXUS_PidChannel_Close(pidChannel[i]);
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    NEXUS_Platform_Uninit();
    return 0;
}
