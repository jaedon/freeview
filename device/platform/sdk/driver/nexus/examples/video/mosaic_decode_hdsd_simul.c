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
 * $brcm_Workfile: mosaic_decode_hdsd_simul.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 1/26/12 11:23a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/video/mosaic_decode_hdsd_simul.c $
 * 
 * 3   1/26/12 11:23a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 2   1/7/11 3:29p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 1   3/26/10 2:15p erickson
 * SW7405-3965: add mosaic test app that works in hd/sd simul mode
 *
 * 9   3/22/10 2:40a randyjew
 * SW7468-143: Support for boards without component and/or composite
 *
 * 8   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 7   9/18/09 2:45p erickson
 * SWDEPRECATED-3942: rework example based on HW requirement that a PCR
 *  pid can only be routed to one PCR_OFFSET block. TSM now works.
 *
 * 6   8/11/09 2:22p erickson
 * PR56965: add TSM
 *
 * 5   7/15/08 5:03p erickson
 * PR44714: fixed CIF size
 *
 * 4   2/25/08 9:50a jgarrett
 * PR 39435: Fixing includes
 *
 * 3   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 *
 * 2   1/23/08 12:39p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
******************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_mosaic_video_decoder.h"
#include "nexus_mosaic_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

BDBG_MODULE(mosaic_decode);

#define NUM_MOSAICS 4
#if (NEXUS_NUM_COMPONENT_OUTPUTS && NEXUS_NUM_COMPOSITE_OUTPUTS)
#define NUM_DISPLAYS 2
#else
#define NUM_DISPLAYS 1
#endif

struct {
    unsigned short videoPid;
    unsigned short pcrPid;
    NEXUS_VideoCodec videoCodec;
} g_programs[NUM_MOSAICS] = {
    {0x1001, 0x100, NEXUS_VideoCodec_eH264},
    {0x1002, 0x100, NEXUS_VideoCodec_eH264},
    {0x1003, 0x100, NEXUS_VideoCodec_eH264},
    {0x1004, 0x100, NEXUS_VideoCodec_eH264}
};

int main(void)
{
    NEXUS_VideoDecoderHandle videoDecoder[NUM_MOSAICS];
    NEXUS_VideoDecoderStartSettings videoProgram[NUM_MOSAICS];
    NEXUS_PidChannelHandle pcrPidChannel[NUM_MOSAICS]; /* up to a max of NUM_MOSAICS */
    unsigned short pcrPidAssignment[NUM_MOSAICS]; /* this corresponds to the pcrPidChannel */
    NEXUS_StcChannelHandle stcChannel[NUM_MOSAICS]; /* up to a max of NUM_MOSAICS */

    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_DisplayHandle display[NUM_DISPLAYS];
    NEXUS_VideoWindowHandle window[NUM_DISPLAYS];
    NEXUS_VideoWindowHandle mosaicWindow[NUM_DISPLAYS][NUM_MOSAICS];
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Timebase timebase = NEXUS_Timebase_e0;
    NEXUS_TimebaseSettings timebaseSettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_DisplaySettings displaySettings;
#endif
    unsigned i, w;
    NEXUS_Error rc;

    BKNI_Memset(pcrPidChannel, 0, sizeof(pcrPidChannel));
    BKNI_Memset(pcrPidAssignment, 0, sizeof(pcrPidAssignment));

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* bring up display */
    for (w=0;w<NUM_DISPLAYS;w++) {
        NEXUS_DisplaySettings displaySettings;
        NEXUS_Display_GetDefaultSettings(&displaySettings);
        if (w == 0) {
            displaySettings.format = NEXUS_VideoFormat_e1080i;
        }
        display[w] = NEXUS_Display_Open(w, &displaySettings);
        window[w] = NEXUS_VideoWindow_Open(display[w], 0);
        switch (w) {
#if NEXUS_NUM_COMPONENT_OUTPUTS
        case 0: NEXUS_Display_AddOutput(display[0], NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0])); break;
#if NEXUS_NUM_COMPOSITE_OUTPUTS
        case 1: NEXUS_Display_AddOutput(display[1], NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0])); break;
#endif
#elif NEXUS_NUM_COMPOSITE_OUTPUTS
        case 0: NEXUS_Display_AddOutput(display[0], NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0])); break;
#endif
        }
    }

#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display[0], NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display[0], &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display[0], &displaySettings);
		}
    }
#endif

    /* bring up decoder and connect to display */
    for (i=0;i<NUM_MOSAICS;i++) {
        NEXUS_VideoDecoderOpenMosaicSettings openSettings;
        NEXUS_StcChannelSettings stcSettings;
        unsigned j;

        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[i]);
        videoProgram[i].codec = g_programs[i].videoCodec;
        videoProgram[i].pidChannel = NEXUS_PidChannel_Open(parserBand, g_programs[i].videoPid, NULL);

        NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(&openSettings);
        openSettings.maxWidth = 352; /* CIF */
        openSettings.maxHeight = 288; /* CIF */
        videoDecoder[i] = NEXUS_VideoDecoder_OpenMosaic(0, i, &openSettings);

        /* see if this PCR is already used by another mosaic. */
        for (j=0;pcrPidChannel[j];j++) {
            if (g_programs[i].pcrPid == pcrPidAssignment[j]) {
                /* It is, so reuse the StcChannel. We can't route the same PCR to more than one PCR_OFFSET block. */
                videoProgram[i].stcChannel = stcChannel[j];
                break;
            }
        }
        if (!videoProgram[i].stcChannel) {
            /* It is not. So open a new pcr pid and StcChannel. */
            BDBG_ASSERT(!pcrPidChannel[j]);
            pcrPidAssignment[j] = g_programs[i].pcrPid;
            pcrPidChannel[j] = NEXUS_PidChannel_Open(parserBand, g_programs[i].pcrPid, NULL);

            /* each PCR gets its own StcChannel (i.e. PCR_OFFSET HW), but we use a shared Timebase (i.e. DPCR and STC HW) */
            NEXUS_StcChannel_GetDefaultSettings(j, &stcSettings);
            stcSettings.timebase = timebase;
            stcSettings.autoConfigTimebase = false;
            stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
            stcSettings.modeSettings.pcr.pidChannel = pcrPidChannel[j];
            stcSettings.stcIndex = 0; /* shared STC for all mosaics */
            stcChannel[j] = NEXUS_StcChannel_Open(j, &stcSettings);
            videoProgram[i].stcChannel = stcChannel[j];
        }

        for (w=0;w<NUM_DISPLAYS;w++) {
            mosaicWindow[w][i] = NEXUS_VideoWindow_OpenMosaic(window[w], i);
            rc = NEXUS_VideoWindow_AddInput(mosaicWindow[w][i], NEXUS_VideoDecoder_GetConnector(videoDecoder[i]));
        BDBG_ASSERT(!rc);

            NEXUS_VideoWindow_GetSettings(mosaicWindow[w][i], &windowSettings);
        windowSettings.position.x = 50 + (120 * i);
        windowSettings.position.y = 50;
        windowSettings.position.width = 100;
        windowSettings.position.height = 100;
        windowSettings.visible = true;
        windowSettings.zorder = i;
            rc = NEXUS_VideoWindow_SetSettings(mosaicWindow[w][i], &windowSettings);
        BDBG_ASSERT(!rc);
    }
    }

    /* share one timebase. ideally the timebase is set to the pcr associated with the current audio. */
    NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
    timebaseSettings.sourceSettings.pcr.pidChannel = pcrPidChannel[0];
    rc = NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);
    BDBG_ASSERT(!rc);

    /* Start Decoders */
    for (i=0;i<NUM_MOSAICS;i++) {
        rc = NEXUS_VideoDecoder_Start(videoDecoder[i], &videoProgram[i]);
        BDBG_ASSERT(!rc);
    }

    for (;;) {
        for (i=0;i<NUM_MOSAICS;i++) {
            NEXUS_VideoDecoderStatus status;
            uint32_t stc;
            NEXUS_VideoDecoder_GetStatus(videoDecoder[i], &status);
            NEXUS_StcChannel_GetStc(videoProgram[i].stcChannel, &stc);
            printf("decode[%d] %dx%d, pts %#x, stc %#x (diff %d)\n",
                i, status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference);
        }
        BKNI_Sleep(1000);
    }

    return 0;
}
