/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: synclock_master.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 7/15/11 8:53a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/synclock_master.c $
 * 
 * Hydra_Software_Devel/3   7/15/11 8:53a erickson
 * SW7405-5394: add newline to eof
 * 
 * Hydra_Software_Devel/2   7/7/11 12:41p erickson
 * SW7405-5394: reduce runtime for automation
 * 
 * Hydra_Software_Devel/1   6/28/11 9:46a erickson
 * SW7405-5366: add unittest for synclock master and delay status
 * 
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

static void print_status(NEXUS_VideoDecoderHandle vdecode0, NEXUS_VideoDecoderHandle vdecode1, NEXUS_VideoWindowHandle window0, NEXUS_VideoWindowHandle window1, NEXUS_DisplayHandle display);

int main(void)
{
    NEXUS_VideoDecoderHandle vdecode0, vdecode1 = NULL;
    NEXUS_VideoDecoderStartSettings videoProgram0, videoProgram1;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand0 = NEXUS_ParserBand_e0;
    NEXUS_ParserBand parserBand1 = NEXUS_ParserBand_e1;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window[2];
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Error rc;
    unsigned i;
    
    srand(time(NULL));

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* It's important to use two parser bands so that the default maxDataRate of 25 Mbps per parser band will not be exceeded. */
    NEXUS_ParserBand_GetSettings(parserBand0, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand0, &parserBandSettings);

    NEXUS_ParserBand_GetSettings(parserBand1, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand1, &parserBandSettings);

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram0);
    videoProgram0.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram0.pidChannel = NEXUS_PidChannel_Open(parserBand0, 0x31, NULL);
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram1);
    videoProgram1.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram1.pidChannel = NEXUS_PidChannel_Open(parserBand1, 0x11, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram0.pidChannel; /* PCR on video pid */
    videoProgram0.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e1;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram1.pidChannel; /* PCR on video pid */
    videoProgram1.stcChannel = NEXUS_StcChannel_Open(1, &stcSettings);

    /* bring up display */
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");
#else
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#endif
    window[0] = NEXUS_VideoWindow_Open(display, 0);
    window[1] = NEXUS_VideoWindow_Open(display, 1);
    if (!window[1]) {
        printf("PIP not supported\n");
    }

    /* bring up decoder and connect to display */
    vdecode0 = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    rc = NEXUS_VideoWindow_AddInput(window[0], NEXUS_VideoDecoder_GetConnector(vdecode0));
    BDBG_ASSERT(!rc);
    vdecode1 = NEXUS_VideoDecoder_Open(1, NULL); /* take default capabilities */
    if (vdecode1) {
        rc = NEXUS_VideoWindow_AddInput(window[1], NEXUS_VideoDecoder_GetConnector(vdecode1));
        BDBG_ASSERT(!rc);
    }

    /* Start Decoders */
    rc = NEXUS_VideoDecoder_Start(vdecode0, &videoProgram0);
    BDBG_ASSERT(!rc);
    if (vdecode1) {
        rc = NEXUS_VideoDecoder_Start(vdecode1, &videoProgram1);
        BDBG_ASSERT(!rc);
    }

    for (i=0;i<200;i++) {
        NEXUS_VideoWindowSettings windowSettings;
        NEXUS_DisplaySettings displaySettings;
        unsigned windowIndex = rand()%2;
        
        if (!window[windowIndex]) continue;
        
        switch (rand() % 4) {
        case 0:
            NEXUS_VideoWindow_GetSettings(window[windowIndex], &windowSettings);    
            windowSettings.autoMaster = rand() % 2;
            rc = NEXUS_VideoWindow_SetSettings(window[windowIndex], &windowSettings);    
            BDBG_ASSERT(!rc);
            break;
        case 1:
            NEXUS_VideoWindow_GetSettings(window[windowIndex], &windowSettings);    
            windowSettings.preferSyncLock = rand() % 2;
            rc = NEXUS_VideoWindow_SetSettings(window[windowIndex], &windowSettings);    
            BDBG_ASSERT(!rc);
            break;
        case 2:
            NEXUS_VideoWindow_GetSettings(window[windowIndex], &windowSettings);    
            windowSettings.delay = rand() % 3;
            rc = NEXUS_VideoWindow_SetSettings(window[windowIndex], &windowSettings);    
            BDBG_ASSERT(!rc);
            break;
        case 3:
            NEXUS_Display_GetSettings(display, &displaySettings);    
            switch (rand()%4) {
            case 0: displaySettings.format = NEXUS_VideoFormat_eNtsc; break;    
            case 1: displaySettings.format = NEXUS_VideoFormat_e480p; break;
            case 2: displaySettings.format = NEXUS_VideoFormat_e720p; break;
            case 3: displaySettings.format = NEXUS_VideoFormat_e1080i; break;
            }
            rc = NEXUS_Display_SetSettings(display, &displaySettings);    
            BDBG_ASSERT(!rc);
            break;
        }
        
        print_status(vdecode0, vdecode1, window[0], window[1], display);
        BKNI_Sleep(rand() % 1000);
    }

    return 0;
}

static void print_status(NEXUS_VideoDecoderHandle vdecode0, NEXUS_VideoDecoderHandle vdecode1, NEXUS_VideoWindowHandle window0, NEXUS_VideoWindowHandle window1, NEXUS_DisplayHandle display)
{
    NEXUS_DisplayStatus displayStatus;
    NEXUS_VideoWindowStatus windowStatus;
    NEXUS_VideoDecoderStatus status;

    NEXUS_VideoDecoder_GetStatus(vdecode0, &status);
    printf("decode0 %dx%d, pts %#x (diff %d)\n",
        status.source.width, status.source.height, status.pts, status.ptsStcDifference);

    if (vdecode1) {
        NEXUS_VideoDecoder_GetStatus(vdecode1, &status);
        printf("decode1 %dx%d, pts %#x (diff %d)\n",
            status.source.width, status.source.height, status.pts, status.ptsStcDifference);
    }
    
    NEXUS_Display_GetStatus(display, &displayStatus);
    printf("display rate %d.%3dHz\n", displayStatus.refreshRate/1000, displayStatus.refreshRate%1000);

    NEXUS_VideoWindow_GetStatus(window0, &windowStatus);
    printf("window0 delay %d vsyncs, phase delay %d usec, total delay %d usec\n", 
        windowStatus.delay, 
        windowStatus.phaseDelay, 
        (windowStatus.delay*(1000000*1000/displayStatus.refreshRate)) + windowStatus.phaseDelay);
    if (vdecode1) {            
        NEXUS_VideoWindow_GetStatus(window1, &windowStatus);
        printf("window1 delay %d vsyncs, phase delay %d usec, total delay %d usec\n", 
            windowStatus.delay, 
            windowStatus.phaseDelay, 
            (windowStatus.delay*(1000000*1000/displayStatus.refreshRate)) + windowStatus.phaseDelay);
    }    
    printf("\n");
}

