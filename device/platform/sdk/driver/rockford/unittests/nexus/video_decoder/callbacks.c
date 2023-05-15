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
 * $brcm_Workfile: callbacks.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 8/2/11 4:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/callbacks.c $
 * 
 * Hydra_Software_Devel/21   8/2/11 4:54p erickson
 * SW7420-1999: verify certain callbacks are being called
 * 
 * Hydra_Software_Devel/20   5/27/11 12:24p erickson
 * SWDEPRECATED-3044: speed up test
 * 
 * Hydra_Software_Devel/19   12/13/10 8:12a mward
 * SW7400-2978:  Close window before closing display.  With changes to
 * kernel mode proxy for process isolation, the app must close objects it
 * opened, automatic closing, such as of windows in NEXUS_Display_Close()
 * will not work.
 * 
 * Hydra_Software_Devel/18   10/25/10 5:39p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/17   7/29/10 10:05a erickson
 * SW7405-4637: handle userdata ringbuffer wraps internally for easier app
 * code
 *
 * Hydra_Software_Devel/16   6/16/10 10:02a erickson
 * SW7405-4378: add more status
 *
 * Hydra_Software_Devel/15   6/3/10 4:13p erickson
 * SW7405-4378: added codedSourceHorizontalSize and
 * codedSourceVerticalSize to test
 *
 * Hydra_Software_Devel/14   11/3/09 6:39p mphillip
 * SW7405-3034: Slighly increase event timeout to make test more
 * resilient.
 *
 * Hydra_Software_Devel/13   7/23/09 12:22p erickson
 * PR56967: add more callbacks
 *
 * Hydra_Software_Devel/12   10/23/08 3:58p erickson
 * PR42679: update
 *
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_parser_band.h"

struct {
    bool source_changed;
    bool stream_changed;
    bool first_pts;
    bool first_pts_passed;
} g_events;
unsigned userdataCount = 0;

void userdata(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;

    BSTD_UNUSED(param);

    /* this callback verifies the userdata size field and whole userdata packets */
    while (1) {
        NEXUS_UserDataHeader *pHeader;
        unsigned size;
        NEXUS_Error rc;

        /* First, get the size of the userdata field. */
        rc = NEXUS_VideoDecoder_GetUserDataBuffer(videoDecoder, (void**)&pHeader, &size);
        BDBG_ASSERT(!rc);
        if (size == 0) break;

        BDBG_ASSERT(size >= pHeader->blockSize);
        BDBG_ASSERT(pHeader->blockSize == sizeof(*pHeader) + pHeader->payloadSize);
        BDBG_ASSERT(pHeader->blockSize % 4 == 0); /* 32 bit aligned */

        NEXUS_VideoDecoder_UserDataReadComplete(videoDecoder, pHeader->blockSize);
        userdataCount++;
    }
}

void sourceChanged(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    NEXUS_VideoDecoderStatus status;

    BSTD_UNUSED(param);
    NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
    printf("  source changed %dx%d, %dx%d, %dx%d\n",
        status.source.width, status.source.height,
        status.coded.width, status.coded.height,
        status.display.width, status.display.height
        );
    g_events.source_changed = true;        
}

void streamChanged(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    NEXUS_VideoDecoderStreamInformation info;

    BSTD_UNUSED(param);
    NEXUS_VideoDecoder_GetStreamInformation(videoDecoder, &info);
    printf("  stream changed %dx%d, %dx%d, %dx%d\n",
        info.sourceHorizontalSize, info.sourceVerticalSize,
        info.codedSourceHorizontalSize, info.codedSourceVerticalSize,
        info.displayHorizontalSize, info.displayVerticalSize
        );
    g_events.stream_changed = true;        
    BDBG_ASSERT(info.sourceHorizontalSize && info.sourceVerticalSize);
    BDBG_ASSERT(info.codedSourceHorizontalSize && info.codedSourceVerticalSize);
    BDBG_ASSERT(info.displayHorizontalSize && info.displayVerticalSize);
}

void ptsError(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    BSTD_UNUSED(videoDecoder);
    BSTD_UNUSED(param);
    printf("  pts error\n");
}

void fifoEmpty(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    BSTD_UNUSED(videoDecoder);
    BSTD_UNUSED(param);
    printf("  fifo empty\n");
}

void afdChanged(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    BSTD_UNUSED(videoDecoder);
    BSTD_UNUSED(param);
    printf("  afd changed\n");
}

void decodeError(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    BSTD_UNUSED(videoDecoder);
    BSTD_UNUSED(param);
    printf("  decode error\n");
}

void firstPtsPassed(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    NEXUS_VideoDecoderStatus status;
    NEXUS_Error rc;
    BSTD_UNUSED(param);
    rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
    BDBG_ASSERT(!rc);
    BDBG_ASSERT(status.firstPtsPassed);
    BDBG_ASSERT(!status.muted);
    printf("  first pts passed\n");
    g_events.first_pts_passed = true;        
}

void firstPts(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    BSTD_UNUSED(param);
    BSTD_UNUSED(videoDecoder);
    printf("  first pts\n");
    g_events.first_pts = true;        
}

int main(void)
{
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformConfiguration platformConfig;
    unsigned i,j;
    BERR_Code rc;
    unsigned loops = 15;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    for (i=0;i<loops;i++) {
        /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
        NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
        parserBandSettings.transportType = NEXUS_TransportType_eTs;
        rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
        BDBG_ASSERT(!rc);

        /* bring up display */
#if NEXUS_DTV_PLATFORM
        display = NEXUS_Display_Open(0, NULL);
        rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
        BDBG_ASSERT(!rc);
        rc = NEXUS_BoardCfg_ConfigurePanel(true, true, true);
        BDBG_ASSERT(!rc);
#else
        display = NEXUS_Display_Open(0, NULL);
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

        /* bring up decoder and connect to display */
        vdecode = NEXUS_VideoDecoder_Open(0, NULL);

        NEXUS_VideoDecoder_GetSettings(vdecode, &videoDecoderSettings);
        videoDecoderSettings.appUserDataReady.callback = userdata;
        videoDecoderSettings.appUserDataReady.context = vdecode;
        videoDecoderSettings.sourceChanged.callback = sourceChanged;
        videoDecoderSettings.sourceChanged.context = vdecode;
        videoDecoderSettings.streamChanged.callback = streamChanged;
        videoDecoderSettings.streamChanged.context = vdecode;
        videoDecoderSettings.ptsError.callback = ptsError;
        videoDecoderSettings.ptsError.context = vdecode;
        videoDecoderSettings.fifoEmpty.callback = fifoEmpty;
        videoDecoderSettings.fifoEmpty.context = vdecode;
        videoDecoderSettings.afdChanged.callback = afdChanged;
        videoDecoderSettings.afdChanged.context = vdecode;
        videoDecoderSettings.decodeError.callback = decodeError;
        videoDecoderSettings.decodeError.context = vdecode;
        videoDecoderSettings.firstPtsPassed.callback = firstPtsPassed;
        videoDecoderSettings.firstPtsPassed.context = vdecode;
        videoDecoderSettings.firstPts.callback = firstPts;
        videoDecoderSettings.firstPts.context = vdecode;
        rc = NEXUS_VideoDecoder_SetSettings(vdecode, &videoDecoderSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
        BDBG_ASSERT(!rc);

        for (j=0;j<3;j++) {
            NEXUS_StcChannelSettings stcSettings;
            NEXUS_VideoDecoderStartSettings videoProgram;

            NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
            videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
            videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);

            /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
            NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
            stcSettings.timebase = NEXUS_Timebase_e0;
            stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
            stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
            videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

            rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
            BDBG_ASSERT(!rc);
            printf("started decode %d, %d\n", i, j);

            /* decode must start within 3 seconds */
            BKNI_Sleep(3000);
            
            /* all events must have been received */
            BDBG_ASSERT(g_events.source_changed);
            BDBG_ASSERT(g_events.stream_changed);
            BDBG_ASSERT(g_events.first_pts);
            BDBG_ASSERT(g_events.first_pts_passed);

            NEXUS_VideoDecoder_Stop(vdecode);
            printf("  stopped decode, %d userdata read(s)\n", userdataCount);

            NEXUS_StcChannel_Close(videoProgram.stcChannel);
            NEXUS_PidChannel_Close(videoProgram.pidChannel);
        }

        NEXUS_VideoWindow_RemoveAllInputs(window);
        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
        NEXUS_VideoDecoder_Close(vdecode);
        NEXUS_VideoWindow_Close(window);
        NEXUS_Display_Close(display);
    }

    NEXUS_Platform_Uninit();
    return 0;
}
