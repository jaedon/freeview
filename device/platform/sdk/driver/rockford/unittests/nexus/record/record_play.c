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
 * $brcm_Workfile: record_play.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 8/8/11 11:52a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/record_play.c $
 * 
 * Hydra_Software_Devel/8   8/8/11 11:52a erickson
 * SWDEPRECATED-3044: speed up tests
 * 
 * Hydra_Software_Devel/7   7/8/11 12:19p jtna
 * SW7405-5336: added basic sanity check for incoming streamer input
 * 
 * Hydra_Software_Devel/6   5/27/11 12:24p erickson
 * SWDEPRECATED-3044: quiet output
 * 
 * Hydra_Software_Devel/5   10/22/10 11:14a randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/4   7/29/10 11:56a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/3   7/15/10 11:17a mward
 * SW7405-4621: NEXUS_Time no longer available to apps.
 * 
 * Hydra_Software_Devel/2   10/14/09 11:52a jtna
 * SW7405-3205: make test finite
 * 
 * Hydra_Software_Devel/1   2/13/09 10:19a erickson
 * PR50099: added record_play test app
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_playpump.h"
#include "nexus_recpump.h"
#include "nexus_video_decoder.h"
#include "nexus_video_window.h"
#include "nexus_display.h"
#include "examples_lib.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

BDBG_MODULE(record_play);

static void set_event_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

#define TIMEOUT (3*60) /* test duration in seconds */

int main(void)  {
    FILE *file;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    const char *fname = "videos/cnnticker.mpg";
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecpumpStatus recpumpStatus;
    unsigned state;
    NEXUS_Error rc;
    struct timeval start, end;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    srand(time(NULL));
    
    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 1;
    ipsOpenSettings.remuxIndex = 1;
    ips = IpsOpen(&ipsOpenSettings);
#endif
#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif
    
    recpump = NEXUS_Recpump_Open(0, NULL);

    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = set_event_callback;
    recpumpSettings.data.dataReady.context = event;
    recpumpSettings.index.dataReady.callback = set_event_callback;
    recpumpSettings.index.dataReady.context = event;
    NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);

    playpump = NEXUS_Playpump_Open(0, NULL);
    file = fopen(fname, "rb");
    BDBG_ASSERT(file);

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = set_event_callback;
    playpumpSettings.dataCallback.context = event;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x21, NULL);

    /* Bring up display and outputs */
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    BDBG_WRN(("Panel output ready"));
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
    BDBG_ASSERT(!rc);

    rc = NEXUS_Playpump_Start(playpump);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    BDBG_ASSERT(!rc);
    state = 0;

    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&addPidChannelSettings);
    addPidChannelSettings.pidType = NEXUS_PidType_eVideo;
    addPidChannelSettings.pidTypeSettings.video.index = true;
    addPidChannelSettings.pidTypeSettings.video.codec = NEXUS_VideoCodec_eMpeg2;
    rc = NEXUS_Recpump_AddPidChannel(recpump, NEXUS_PidChannel_Open(parserBand, 0x11, NULL), &addPidChannelSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Recpump_AddPidChannel(recpump, NEXUS_PidChannel_Open(parserBand, 0x14, NULL), NULL);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Recpump_Start(recpump);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(2000);
    NEXUS_Recpump_GetStatus(recpump, &recpumpStatus);
    BDBG_ASSERT(recpumpStatus.data.bytesRecorded > 0);

    gettimeofday(&start,NULL);
    
    while (1) {
        void *buffer;
        size_t buffer_size = 0;
        int n;
        NEXUS_Error rc;
        bool wait = true;

        switch (state) {
        case 0:
            if (rand() % 30 == 0) {
                BDBG_WRN(("stop decode"));
                NEXUS_VideoDecoder_Stop(vdecode);
                state = 1;
            }
            else if (rand() % 30 == 0) {
                BDBG_WRN(("stop playpump & decode"));
                if (rand() % 2) {
                    NEXUS_Playpump_Stop(playpump);
                    NEXUS_VideoDecoder_Stop(vdecode);
                }
                else {
                    NEXUS_VideoDecoder_Stop(vdecode);
                    NEXUS_Playpump_Stop(playpump);
                }
                state = 2;
            }
            else if (rand() % 30 == 0) {
                BDBG_WRN(("stop playpump"));
                NEXUS_Playpump_Stop(playpump);
                state = 3;
            }
            break;
        case 1:
            if (rand() % 10 == 0) {
                BDBG_WRN(("start decode"));
                rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
                BDBG_ASSERT(!rc);
                state = 0;
            }
            break;
        case 2:
            if (rand() % 10 == 0) {
                BDBG_WRN(("start playpump & decode"));
                if (rand() % 2) {
                    rc = NEXUS_Playpump_Start(playpump);
                    BDBG_ASSERT(!rc);
                    rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
                    BDBG_ASSERT(!rc);
                }
                else {
                    rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
                    BDBG_ASSERT(!rc);
                    rc = NEXUS_Playpump_Start(playpump);
                    BDBG_ASSERT(!rc);
                    /* required if decoder started before playback */
                    NEXUS_PidChannel_ConsumerStarted(videoProgram.pidChannel);
                }
                state = 0;
            }
            break;
        case 3:
            if (rand() % 10 == 0) {
                BDBG_WRN(("start playpump"));
                rc = NEXUS_Playpump_Start(playpump);
                BDBG_ASSERT(!rc);
                /* required if decoder started before playback */
                NEXUS_PidChannel_ConsumerStarted(videoProgram.pidChannel);
                state = 0;
            }
            break;
        }

        if (state == 0 || state == 1) {
            if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
                break;
        }
        if (buffer_size) {
            wait = false;

            /* The first call to get_buffer will return the entire playback buffer.
            If we use it, we're going to have to wait until the descriptor is complete,
            and then we're going to underflow. So use a max size. */
    #define MAX_READ (188*1024)
            if (buffer_size > MAX_READ)
                buffer_size = MAX_READ;

            n = fread(buffer, 1, buffer_size, file);
            if (n < 0) break;
            if (n == 0) {
                fseek(file, 0, SEEK_SET);
                /* no flush here. there will be macroblocking. but let's keep the data flowing */
            }
            else {
                BDBG_MSG(("played %d bytes of data", n));
                rc = NEXUS_Playpump_ReadComplete(playpump, 0, n);
                BDBG_ASSERT(!rc);
            }
        }

        /* keep emptying record */
        {
            const void *data_buffer, *index_buffer;
            size_t data_buffer_size, index_buffer_size;
            if (NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size))
                break;
            if (NEXUS_Recpump_GetIndexBuffer(recpump, &index_buffer, &index_buffer_size))
                break;
            if (data_buffer_size) {
                wait = false;
                BDBG_MSG(("recorded %d bytes of data", data_buffer_size));
                NEXUS_Recpump_DataWriteComplete(recpump, data_buffer_size);
            }
            if (index_buffer_size) {
                wait = false;
                BDBG_MSG(("recorded %d bytes of index", index_buffer_size));
                NEXUS_Recpump_IndexWriteComplete(recpump, index_buffer_size);
            }
        }

        if (wait) {
            BDBG_MSG(("wait for event"));
            BKNI_WaitForEvent(event, BKNI_INFINITE);
        }

        gettimeofday(&end,NULL);

        if ((1000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000) > TIMEOUT*1000) {
            printf("Test completed\n");
            break;
        }
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    return 0;
}
