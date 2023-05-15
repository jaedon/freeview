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
 * $brcm_Workfile: video_primer.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 6/11/12 11:16a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/video/video_primer.c $
 * 
 * 7   6/11/12 11:16a erickson
 * SW7425-3170: refactor VideoDecoderPrimer api
 *
 * 6   1/26/12 11:23a rjlewis
 * SW7425-1136: Added HDMI Support.
 *
 * 5   8/3/11 6:24p jtna
 * SW7425-1040: do not set pcr offsetThreshold to 0; it's no longer needed
 *
 * 4   3/2/10 3:56p rjain
 * SW7550-254: Updating the display part
 *
 * 3   7/30/09 10:54a erickson
 * PR54880: enable TSM
 *
 * 2   6/29/09 3:15p erickson
 * PR54880: add #include
 *
 * 1   5/19/09 10:31a erickson
 * PR54880: add video_primer
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_primer.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

/* the following define the input and its characteristics -- these will vary by input */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2

#define TOTAL_PRIMERS 3
unsigned short g_pid[TOTAL_PRIMERS] = {0x11, 0x21, 0x41}; /* these are the video pid numbers of different programs */

int main(void)
{
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderPrimerHandle primer[TOTAL_PRIMERS];
    NEXUS_VideoDecoderStartSettings videoProgram[TOTAL_PRIMERS];
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Timebase timebase = NEXUS_Timebase_e0;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif
    unsigned index = 0;
    unsigned i;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the pid channels */
    for (i=0;i<TOTAL_PRIMERS;i++) {
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[i]);
        videoProgram[i].codec = VIDEO_CODEC;
        videoProgram[i].pidChannel = NEXUS_PidChannel_Open(parserBand, g_pid[i], NULL);

        NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
        stcSettings.autoConfigTimebase = false; /* must do it manually */
        stcSettings.timebase = timebase;
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        stcSettings.modeSettings.pcr.pidChannel = videoProgram[i].pidChannel;
        stcSettings.stcIndex = 0;
        videoProgram[i].stcChannel = NEXUS_StcChannel_Open(i, &stcSettings);
    }

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format.
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }
#endif

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

/* you can turn off the primer to compare channel change with no acceleration */
#define USE_PRIMER 1

#if USE_PRIMER
    for (i=0;i<TOTAL_PRIMERS;i++) {
        primer[i] = NEXUS_VideoDecoderPrimer_Open(videoDecoder);
        NEXUS_VideoDecoderPrimer_Start(primer[i], &videoProgram[i]);
    }
#endif

    while (1) {
        NEXUS_TimebaseSettings timebaseSettings;

        NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
        timebaseSettings.sourceSettings.pcr.pidChannel = videoProgram[index].pidChannel;
        timebaseSettings.sourceSettings.pcr.maxPcrError = 0xff;
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
        NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);

#if USE_PRIMER
        NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode(primer[index], videoDecoder);
#else
        NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram[index]);
#endif

        printf("Press ENTER to switch programs\n");
        getchar();

        NEXUS_VideoDecoder_Stop(videoDecoder);
#if USE_PRIMER
        NEXUS_VideoDecoderPrimer_Start(primer[index], &videoProgram[index]);
#endif

        if (++index == TOTAL_PRIMERS) index = 0;
    }

    return 0;
}
