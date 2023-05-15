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
 * $brcm_Workfile: read_vbi.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 3/27/12 12:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/read_vbi.c $
 * 
 * Hydra_Software_Devel/7   3/27/12 12:00p erickson
 * SW7435-77: set allowTeletext
 * 
 * Hydra_Software_Devel/6   7/29/10 11:06a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/5   1/30/09 2:54p erickson
 * PR51676: add VideoDecoder support for settop test
 *
 * Hydra_Software_Devel/4   10/16/08 1:54p erickson
 * PR47070: add gemstar
 *
 * Hydra_Software_Devel/3   2/7/08 12:13p erickson
 * PR38679: update
 *
 * Hydra_Software_Devel/2   2/4/08 10:39a erickson
 * PR38679: update for 740x
 *
 * Hydra_Software_Devel/1   2/1/08 4:15p erickson
 * PR38679: added vbi loopback steps
 *
 * 1   2/1/08 3:21p erickson
 * PR38679: add vbi example
 *
 * 2   1/23/08 12:39p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_video_input.h"
#include "nexus_video_input_vbi.h"
#include "nexus_video_window.h"

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS && 1
#define USE_ANALOG 1
#else
#define USE_ANALOG 0
#endif

#if USE_ANALOG
#include "nexus_analog_video_decoder.h"
#else
#include "nexus_video_decoder.h"
#include "nexus_parser_band.h"
#endif

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

void wss_changed(void *context, int param)
{
    NEXUS_VideoInput videoInput = context;
    uint16_t wss;
    BSTD_UNUSED(param);
    NEXUS_VideoInput_GetWss(videoInput, &wss);
    printf("\nwss changed: %d\n", wss);
}

void cgms_changed(void *context, int param)
{
    NEXUS_VideoInput videoInput = context;
    uint32_t cgms;
    BSTD_UNUSED(param);
    NEXUS_VideoInput_GetCgms(videoInput, &cgms);
    printf("\ncgms changed %d\n", cgms);
}

void gs_data_ready(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    printf("gs data ready\n");
}

int main(void)
{
    NEXUS_VideoInput videoInput;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_VideoInputVbiSettings videoInputVbiSettings;
    NEXUS_VideoWindowHandle window;
#if USE_ANALOG
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
#else
    NEXUS_InputBand inputBand;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_VideoDecoderStartSettings videoProgram;
#endif
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    char buf[256];
    FILE *foutput;
    NEXUS_Error rc;
    NEXUS_VideoFormat inputFormat;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    platformSettings.displayModuleSettings.vbi.allowTeletext = true;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    printf("Enter composite video input format (NTSC or PAL):\n");
    gets(buf);
    if (strstr(buf, "pal") == buf)
        inputFormat = NEXUS_VideoFormat_ePal;
    else
        inputFormat = NEXUS_VideoFormat_eNtsc;

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = inputFormat; /* have output match input */
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

#if USE_ANALOG
    /* bring up VDEC */
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    videoInput = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);

    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
#if 1
    analogVideoDecoderSettings.autoDetectionMode.autoMode = false;
    analogVideoDecoderSettings.autoDetectionMode.manualFormat = inputFormat;
#else
    analogVideoDecoderSettings.autoDetectionMode.autoDetectPal = (inputFormat == NEXUS_VideoFormat_ePal);
    analogVideoDecoderSettings.autoDetectionMode.autoDetectNtsc = (inputFormat != NEXUS_VideoFormat_ePal);
#endif
    NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    NEXUS_VideoWindow_AddInput(window, videoInput);
#else
    /* digital video */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif    
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x21, NULL); /* mummy_MI_5element_q64.mpg stream has CC */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    videoInput = NEXUS_VideoDecoder_GetConnector(videoDecoder);
    NEXUS_VideoWindow_AddInput(window, videoInput);
#ifdef IP_STREAMER_SUBST
        ipsStartSettings.vDecoder = videoDecoder;
        ipsStartSettings.aDecoder = NULL;
        ipsStartSettings.stcChannel = NULL;
        IpsStart(ips, &ipsStartSettings);
#endif
    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
#endif


    /* TODO: enable automatic routing for other VBI types */
    printf(
    "Option\n"
    "1) Get CGMS\n"
    "2) Get WSS\n"
    "3) TBD\n"
    "4) Read Closed Caption\n"
    "5) Read Teletext\n"
    "6) Read GemStar\n"
    "0) Disable all VBI capture\n"
    );
    gets(buf);

    switch (atoi(buf)) {
    case 1:
        printf("Registering CGMS callback...\n");
        NEXUS_VideoInput_GetVbiSettings(videoInput, &videoInputVbiSettings);
        videoInputVbiSettings.cgmsChanged.callback = cgms_changed;
        videoInputVbiSettings.cgmsChanged.context = videoInput;
        NEXUS_VideoInput_SetVbiSettings(videoInput, &videoInputVbiSettings);
        break;

    case 2:
        printf("Registering WSS callback...\n");
        NEXUS_VideoInput_GetVbiSettings(videoInput, &videoInputVbiSettings);
        videoInputVbiSettings.wssChanged.callback = wss_changed;
        videoInputVbiSettings.wssChanged.context = videoInput;
        NEXUS_VideoInput_SetVbiSettings(videoInput, &videoInputVbiSettings);
        break;

    case 4:
        printf("Enter filename:\n");
        gets(buf);
        foutput = fopen(buf, "w+");
        BDBG_ASSERT(foutput);

        NEXUS_VideoInput_GetVbiSettings(videoInput, &videoInputVbiSettings);
        videoInputVbiSettings.closedCaptionEnabled = true;
        rc = NEXUS_VideoInput_SetVbiSettings(videoInput, &videoInputVbiSettings);
        BDBG_ASSERT(!rc);

        while (1) {
            unsigned i, num;
            NEXUS_ClosedCaptionData ccData[3]; /* use array to test > 1 */
            rc = NEXUS_VideoInput_ReadClosedCaption(videoInput, ccData, 3, &num);
            BDBG_ASSERT(!rc);
            printf("got %d\n", num);
            for (i=0;i<num;i++) {
                fwrite(&ccData[i].data[0], 1, 1, foutput);
                fwrite(&ccData[i].data[1], 1, 1, foutput);
            }
            if (num) {
                continue;
            }

            /* no data */
            fflush(foutput);
            BKNI_Sleep(100);
        }
        break;

    case 5:
        printf("Enter filename:\n");
        gets(buf);
        foutput = fopen(buf, "w+");
        BDBG_ASSERT(foutput);

        NEXUS_VideoInput_GetVbiSettings(videoInput, &videoInputVbiSettings);
        videoInputVbiSettings.teletextEnabled = true;
        videoInputVbiSettings.teletextBufferSize = 20; /* TBD */
        rc = NEXUS_VideoInput_SetVbiSettings(videoInput, &videoInputVbiSettings);
        BDBG_ASSERT(!rc);

        while (1) {
            unsigned num;
            NEXUS_TeletextLine ttLine;
            rc = NEXUS_VideoInput_ReadTeletext(videoInput, &ttLine, 1, &num);
            BDBG_ASSERT(!rc);
            if (num) {
                fwrite(&ttLine.framingCode, 1, 1, foutput);
                fwrite(ttLine.data, sizeof(ttLine.data), 1, foutput);
                continue;
            }

            /* no data */
            fflush(foutput);
            BKNI_Sleep(100);
        }
        break;

    case 6:
        NEXUS_VideoInput_GetVbiSettings(videoInput, &videoInputVbiSettings);
        videoInputVbiSettings.gemStarEnabled = true;
        videoInputVbiSettings.gemStarBufferSize = 20; /* TBD */
/*         videoInputVbiSettings.gemStarDataReady.callback = gs_data_ready; */
        videoInputVbiSettings.gemStar.baseLineTop = 15;
        videoInputVbiSettings.gemStar.lineMaskTop = 0x1f;
        videoInputVbiSettings.gemStar.baseLineBottom = 15 + 263;
        videoInputVbiSettings.gemStar.lineMaskBottom = 0x1f;
        rc = NEXUS_VideoInput_SetVbiSettings(videoInput, &videoInputVbiSettings);
        BDBG_ASSERT(!rc);

        while (1) {
            unsigned num;
            NEXUS_GemStarData gsData;
            rc = NEXUS_VideoInput_ReadGemStar(videoInput, &gsData, 1, &num);
            BDBG_ASSERT(!rc);
            if (num) {
                uint32_t no_gemstar[5] = {0x20202020,0x20202020,0x20202020,0x20202020,0x20202020};

                /* this is needed to keep the console from flooding with nothing */
                if (!BKNI_Memcmp(gsData.data, no_gemstar, sizeof(no_gemstar)))
                    continue;

                printf("gsData %c %x: %x %x %x %x %x\n",
                    gsData.topField?'t':'b',
                    gsData.lineMask,
                    gsData.data[0], gsData.data[1], gsData.data[2], gsData.data[3],  gsData.data[4]);
                continue;
            }
            BKNI_Sleep(100);
        }
        break;

    case 7:
        printf("Enable automatic routing of CC to Display...\n");
        NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
        displayVbiSettings.vbiSource = videoInput;
        displayVbiSettings.closedCaptionRouting = true;
        displayVbiSettings.closedCaptionEnabled = true;
        NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
        break;

    default:
        NEXUS_VideoInput_GetVbiSettings(videoInput, &videoInputVbiSettings);
        videoInputVbiSettings.wssChanged.callback = NULL;
        videoInputVbiSettings.cgmsChanged.callback = NULL;
        videoInputVbiSettings.closedCaptionEnabled = false;
        videoInputVbiSettings.teletextEnabled = false;
        videoInputVbiSettings.gemStarEnabled = false;
        NEXUS_VideoInput_SetVbiSettings(videoInput, &videoInputVbiSettings);

        NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
        displayVbiSettings.vbiSource = NULL;
        displayVbiSettings.closedCaptionRouting = false;
        displayVbiSettings.closedCaptionEnabled = false;
        displayVbiSettings.teletextRouting = false;
        displayVbiSettings.teletextEnabled = false;
        NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);
        break;
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    return 0;
}

