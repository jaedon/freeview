/***************************************************************************
 *     (c)2007-2009 Broadcom Corporation
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
 * $brcm_Workfile: scrambling_status.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 7/29/10 12:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/scrambling_status.c $
 * 
 * Hydra_Software_Devel/7   7/29/10 12:34p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/6   9/25/09 12:57p erickson
 * SW7405-2903: update test
 *
 * Hydra_Software_Devel/5   6/8/09 4:03p erickson
 * PR40079: update API
 *
 **************************************************************************/

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_input_band.h"
#include "nexus_parser_band.h"
#include "nexus_pid_channel_scrambling.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

BDBG_MODULE(parser_band);

int main(void)
{
    NEXUS_Error rc;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_PidChannelScramblingSettings scramblingSettings;
    NEXUS_PidChannelScramblingStatus status;
    unsigned short pid = 0x31;
    unsigned i;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* must open one to enable parser band */
    pidChannel = NEXUS_PidChannel_Open(parserBand, pid, NULL);
    BDBG_ASSERT(pidChannel);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    NEXUS_PidChannel_GetDefaultScramblingSettings(&scramblingSettings);

    scramblingSettings.pusiOnly = false;
    rc = NEXUS_PidChannel_StartScramblingCheck(pidChannel, &scramblingSettings);
    BDBG_ASSERT(!rc);
    for (i=0;i<20;i++) {
        rc = NEXUS_PidChannel_GetScramblingStatus(pidChannel, &status);
        BDBG_ASSERT(!rc);
        printf("Any packet: exists %d, scrambled %d\n", status.pidExists, status.scrambled);
        BKNI_Sleep(1000);
    }
    NEXUS_PidChannel_StopScramblingCheck(pidChannel);

    scramblingSettings.pusiOnly = true;
    rc = NEXUS_PidChannel_StartScramblingCheck(pidChannel, &scramblingSettings);
    BDBG_ASSERT(!rc);
    for (i=0;i<20;i++) {
        rc = NEXUS_PidChannel_GetScramblingStatus(pidChannel, &status);
        BDBG_ASSERT(!rc);
        printf("PUSI packets only: exists %d, scrambled %d\n", status.pidExists, status.scrambled);
        BKNI_Sleep(1000);
    }
    NEXUS_PidChannel_StopScramblingCheck(pidChannel);

    NEXUS_PidChannel_Close(pidChannel);
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    NEXUS_Platform_Uninit();

    return 0;
}