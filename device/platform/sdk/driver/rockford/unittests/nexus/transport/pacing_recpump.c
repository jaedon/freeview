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
 * $brcm_Workfile: pacing_recpump.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 7/29/10 12:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/pacing_recpump.c $
 * 
 * Hydra_Software_Devel/10   7/29/10 12:31p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/9   5/10/10 7:01p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 * 
 * Hydra_Software_Devel/8   6/25/09 4:58p jtna
 * PR55767: add comment about broken unittest
 * 
 * Hydra_Software_Devel/7   6/17/09 4:44p jtna
 * PR55767: remove misleading comment
 * 
 * Hydra_Software_Devel/6   6/11/09 5:09p jtna
 * PR55767: update NEXUS_ParserBandSettings.timestampType =>
 * NEXUS_RecpumpSettings.timestampType
 * 
 * Hydra_Software_Devel/5   5/4/09 5:12p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/4   3/10/09 10:54a erickson
 * PR52946: add index support
 *
 * Hydra_Software_Devel/3   12/16/08 12:41p erickson
 * PR42679: record longer file
 *
 * Hydra_Software_Devel/2   11/4/08 4:38p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   10/27/08 4:14p erickson
 * PR42679: added pacing testse
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

static void dataready_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    printf("overflow %s\n", (const char *)context);
}

int main(void) {
    FILE *data_file;
    const char data_fname[] = "videos/pacing_stream.mpg";
    FILE *index_file;
    const char index_fname[] = "videos/pacing_stream.sct";
    BKNI_EventHandle event;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
    NEXUS_RecpumpHandle recpump;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    unsigned total = 0;
    unsigned last_print = 0;
    NEXUS_PidChannelHandle pidChannel;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST 
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    BKNI_CreateEvent(&event);

    recpump = NEXUS_Recpump_Open(0, NULL);

    data_file = fopen(data_fname, "wb");
    if (!data_file) {
        printf("Unable to open file %s for write\n", data_fname);
        return -1;
    }

    index_file = fopen(index_fname, "wb");
    if (!index_file) {
        printf("Unable to open file %s for write\n", index_fname);
        return -1;
    }

    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = dataready_callback;
    recpumpSettings.data.dataReady.context = event;
    recpumpSettings.index.dataReady.callback = dataready_callback;
    recpumpSettings.index.dataReady.context = event;
    recpumpSettings.data.overflow.callback = overflow_callback;
    recpumpSettings.data.overflow.context = (void *)"data";
    recpumpSettings.timestampType = NEXUS_TransportTimestampType_eMod300;
    NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&addPidChannelSettings);
    addPidChannelSettings.pidType = NEXUS_PidType_eVideo;
    addPidChannelSettings.pidTypeSettings.video.index = true;
    addPidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    NEXUS_Recpump_AddPidChannel(recpump, pidChannel, &addPidChannelSettings);
    NEXUS_Recpump_Start(recpump);

    while (total < 25 * 1024 * 1024) {
        const void *data_buffer, *index_buffer;
        size_t data_buffer_size, index_buffer_size;
        int n;

        if (NEXUS_Recpump_GetIndexBuffer(recpump, &index_buffer, &index_buffer_size))
            break;
        if (NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size))
            break;
        if (data_buffer_size == 0 && index_buffer_size == 0) {
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            continue;
        }

        if (index_buffer_size) {
            n = fwrite(index_buffer, 1, index_buffer_size, index_file);
            BDBG_ASSERT(n == (int)index_buffer_size);
            NEXUS_Recpump_IndexWriteComplete(recpump, n);
        }
        if (data_buffer_size) {
            n = fwrite(data_buffer, 1, data_buffer_size, data_file);
            BDBG_ASSERT(n == (int)data_buffer_size);
            NEXUS_Recpump_DataWriteComplete(recpump, n);
        }
        total += data_buffer_size;

        if (total / 1024 / 1024 > last_print) {
            last_print = total / 1024 / 1024;
            printf("%d MB recorded\n", last_print);
        }
    }
    /* TODO: this unittest is partially broken; if you run the pacing_playpump unittest with the recorded stream
       from pacing_recpump, you will see playback racing. However, if you add an unindexed pid to recpump, you 
       won't see the problem. In both cases, the generated timestamps appear to be reasonable, so further 
       investigation is necessary */
    
    NEXUS_Recpump_Stop(recpump);
    NEXUS_Recpump_RemoveAllPidChannels(recpump);
    NEXUS_PidChannel_Close(pidChannel);
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    NEXUS_Recpump_Close(recpump);
    fclose(data_file);
    fclose(index_file);
    BKNI_DestroyEvent(event);
    NEXUS_Platform_Uninit();

    return 0;
}
