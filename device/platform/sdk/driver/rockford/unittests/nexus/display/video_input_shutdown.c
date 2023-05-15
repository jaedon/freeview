/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: video_input_shutdown.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 8/13/12 8:54a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/video_input_shutdown.c $
* 
* Hydra_Software_Devel/8   8/13/12 8:54a cng
* SW7429-231: Add clean up to close hdmi input, video decoder and analog
* video decoder handlers
* 
* Hydra_Software_Devel/7   8/8/12 2:00p erickson
* SW7429-231: clean shutdown
* 
* Hydra_Software_Devel/6   9/20/11 8:39a erickson
* SW7405-5416: limit runtime to 1 minute
* 
* Hydra_Software_Devel/5   9/14/11 12:25p erickson
* SW7405-5416: fix warning
* 
* Hydra_Software_Devel/4   7/11/11 3:35p erickson
* SW7405-5416: fix PIP systems
* 
* Hydra_Software_Devel/3   2/3/10 11:45a erickson
* SW3556-989: add digital
*
* Hydra_Software_Devel/2   1/26/10 1:31p erickson
* SW3556-989: add composite input, add sleep, make test run for a very
* long time
*
* Hydra_Software_Devel/1   1/25/10 4:04p erickson
* SW3556-989: first iteration of NEXUS_VideoInput_Shutdown stress. this
* exposes bug in HdmiInput.
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#include "nexus_video_decoder.h"
#include "nexus_parser_band.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_analog_video_decoder.h"
#endif
#if NEXUS_NUM_HDMI_INPUTS
#include "nexus_hdmi_input.h"
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
#include "nexus_component_output.h"
#else
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#endif
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

BDBG_MODULE(video_input_shutdown);

/* app-specific list of input instances */
typedef enum NEXUS_VideoInputList
{
    NEXUS_VideoInputList_eVideoDecoder0,
    NEXUS_VideoInputList_eVideoDecoder1,
    NEXUS_VideoInputList_eAnalogVideoDecoder0,
    NEXUS_VideoInputList_eHdmi0,
    NEXUS_VideoInputList_eHdmi1,
    NEXUS_VideoInputList_eMax
} NEXUS_VideoInputList;

const char *g_videoInputStr[NEXUS_VideoInputList_eMax] = { "VideoDecoder0", "VideoDecoder1", "AnalogVideoDecoder0", "Hdmi0", "Hdmi1" };

unsigned connectCount[NEXUS_VideoInputList_eMax];

static NEXUS_Error start_decode(NEXUS_VideoDecoderHandle videoDecoder);

int main(void)
{
    NEXUS_VideoInput input[NEXUS_VideoInputList_eMax];
    NEXUS_VideoInputResumeMode resumeMode[NEXUS_VideoInputList_eMax];
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window[NEXUS_NUM_VIDEO_WINDOWS];
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
#endif
    NEXUS_VideoDecoderHandle videoDecoder[NEXUS_NUM_VIDEO_DECODERS];
    bool videoDecoderStarted[NEXUS_NUM_VIDEO_DECODERS];
#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_HdmiInputHandle hdmiInput[NEXUS_NUM_HDMI_INPUTS];
#endif
    NEXUS_DisplaySettings displayCfg;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_VideoInputList connectedInput[NEXUS_NUM_VIDEO_WINDOWS];
    NEXUS_Error rc;
    unsigned loops = 30000; /* about 1 minute */
    unsigned i = 0;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_Memset(input, 0, sizeof(input));
    BKNI_Memset(resumeMode, 0, sizeof(resumeMode));
#if NEXUS_NUM_HDMI_INPUTS
    BKNI_Memset(hdmiInput, 0, sizeof(hdmiInput));
#endif
    BKNI_Memset(videoDecoder, 0, sizeof(videoDecoder));
    BKNI_Memset(videoDecoderStarted, 0, sizeof(videoDecoderStarted));
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    analogVideoDecoder = NULL;
#endif

    srand(time(NULL));

    /* platformConfig contains handles for ComponentInput and ComponentOutput. It knows how to set the DAC's and ADC's for this Platform. */

    NEXUS_Display_GetDefaultSettings(&displayCfg);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    displayCfg.displayType = NEXUS_DisplayType_eAuto;
    displayCfg.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displayCfg);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#else
    display = NEXUS_Display_Open(0, &displayCfg);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#endif
    for (i=0;i<NEXUS_NUM_VIDEO_WINDOWS;i++) {
        window[i] = NEXUS_VideoWindow_Open(display, i);
        BDBG_ASSERT(window[i]);
        connectedInput[i] = NEXUS_VideoInputList_eMax;
    }

    rc = NEXUS_DisplayModule_SetAutomaticPictureQuality();
    BDBG_ASSERT(!rc);

    while (--loops) {
        unsigned subIndex;
        NEXUS_VideoInputList inputType = rand() % NEXUS_VideoInputList_eMax;
        unsigned windowIndex = rand() % NEXUS_NUM_VIDEO_WINDOWS;

        switch (inputType) {
        case NEXUS_VideoInputList_eVideoDecoder0:
        case NEXUS_VideoInputList_eVideoDecoder1:
            subIndex = inputType - NEXUS_VideoInputList_eVideoDecoder0;
            if (subIndex >= NEXUS_NUM_VIDEO_DECODERS) continue;
            break;
        case NEXUS_VideoInputList_eAnalogVideoDecoder0:
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
            subIndex = inputType - NEXUS_VideoInputList_eAnalogVideoDecoder0;
            break;
#else
            continue;
#endif
        case NEXUS_VideoInputList_eHdmi0:
        case NEXUS_VideoInputList_eHdmi1:
#if NEXUS_NUM_HDMI_INPUTS
            subIndex = inputType - NEXUS_VideoInputList_eHdmi0;
            if (subIndex >= NEXUS_NUM_HDMI_INPUTS) continue;
            break;
#else
            continue;
#endif
        default:
            BDBG_ASSERT(0);
        }

        switch (rand() % 8) {
        case 0:
            switch (inputType) {
            case NEXUS_VideoInputList_eVideoDecoder0:
            case NEXUS_VideoInputList_eVideoDecoder1:
                if (!videoDecoder[subIndex]) {
                    videoDecoder[subIndex] = NEXUS_VideoDecoder_Open(subIndex, NULL);
                    BDBG_ASSERT(videoDecoder[subIndex]);
                }
                break;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
            case NEXUS_VideoInputList_eAnalogVideoDecoder0:
                if (!analogVideoDecoder) {
                    NEXUS_AnalogVideoDecoderSettings settings;

                    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
                    BDBG_ASSERT(analogVideoDecoder);

                    /* connect an input */
                    NEXUS_AnalogVideoDecoder_GetDefaultSettings(&settings);
                    settings.input = NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]);
                    settings.useDynamicDefaults = true;
                    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &settings);
                    BDBG_ASSERT(!rc);
                }
                break;
#endif
#if NEXUS_NUM_HDMI_INPUTS
            case NEXUS_VideoInputList_eHdmi0:
            case NEXUS_VideoInputList_eHdmi1:
                if (!hdmiInput[subIndex]) {
                    hdmiInput[subIndex] = NEXUS_HdmiInput_Open(subIndex, NULL);
                    BDBG_ASSERT(hdmiInput[subIndex]);
                }
                break;
#endif
            default:
                BDBG_ASSERT(0);
            }
            break;

        case 1:
            if (rand() % 5 != 0) break; /* default to having the source up more so that VideoInput Add/Remove gets stressed more */

            if (!input[inputType]) {
                switch (inputType) {
                case NEXUS_VideoInputList_eVideoDecoder0:
                case NEXUS_VideoInputList_eVideoDecoder1:
                    if (videoDecoder[subIndex]) {
                        NEXUS_VideoDecoder_Close(videoDecoder[subIndex]);
                        videoDecoder[subIndex] = NULL;
                        videoDecoderStarted[subIndex] = false;
                    }
                    break;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
                case NEXUS_VideoInputList_eAnalogVideoDecoder0:
                    if (analogVideoDecoder) {
                        NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
                        analogVideoDecoder = NULL;
                    }
                    break;
#endif
#if NEXUS_NUM_HDMI_INPUTS
                case NEXUS_VideoInputList_eHdmi0:
                case NEXUS_VideoInputList_eHdmi1:
                    if (hdmiInput[subIndex]) {
                        NEXUS_HdmiInput_Close(hdmiInput[subIndex]);
                        hdmiInput[subIndex] = NULL;
                    }
                    break;
#endif
                default:
                    BDBG_ASSERT(0);
                }
            }
            break;

        case 2:
            if (!input[inputType]) {
                switch (inputType) {
                case NEXUS_VideoInputList_eVideoDecoder0:
                case NEXUS_VideoInputList_eVideoDecoder1:
                    if (videoDecoder[subIndex]) {
                        input[inputType] = NEXUS_VideoDecoder_GetConnector(videoDecoder[subIndex]);
                        BDBG_ASSERT(input[inputType]);
                    }
                    break;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
                case NEXUS_VideoInputList_eAnalogVideoDecoder0:
                    if (analogVideoDecoder) {
                        input[inputType] = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);
                        BDBG_ASSERT(input[inputType]);
                    }
                    break;
#endif
#if NEXUS_NUM_HDMI_INPUTS
                case NEXUS_VideoInputList_eHdmi0:
                case NEXUS_VideoInputList_eHdmi1:
                    if (hdmiInput[subIndex]) {
                        input[inputType] = NEXUS_HdmiInput_GetVideoConnector(hdmiInput[subIndex]);
                        BDBG_ASSERT(input[inputType]);
                    }
                    break;
#endif
                default:
                    BDBG_ASSERT(0);
                }
            }
            break;

        case 3:
            if (input[inputType] && connectedInput[windowIndex] == NEXUS_VideoInputList_eMax) {
                rc = NEXUS_VideoWindow_AddInput(window[windowIndex], input[inputType]);
                BDBG_ASSERT(!rc);
                connectedInput[windowIndex] = inputType;
                connectCount[inputType]++;

                if (inputType == NEXUS_VideoInputList_eVideoDecoder0) {
                    if (!videoDecoderStarted[subIndex]) {
                        start_decode(videoDecoder[subIndex]);
                        videoDecoderStarted[subIndex] = true;
                    }
                }
            }
            break;

        case 4:
            if (connectedInput[windowIndex] == inputType) {
                rc = NEXUS_VideoWindow_RemoveInput(window[windowIndex], input[inputType]);
                BDBG_ASSERT(!rc);
                connectedInput[windowIndex] = NEXUS_VideoInputList_eMax;
            }
            break;

        case 5:
            if (input[inputType]) {
                NEXUS_VideoInput_Shutdown(input[inputType]);
                input[inputType] = NULL;
                resumeMode[inputType] = NEXUS_VideoInputResumeMode_eAuto;
                for (i=0;i<NEXUS_NUM_VIDEO_WINDOWS;i++) {
                    if (connectedInput[i] == inputType) {
                        connectedInput[i] = NEXUS_VideoInputList_eMax;
                    }
                }
            }
            break;
        case 6:
            if (rand() % 50 == 0) {
                BKNI_Sleep(rand() % 500);
            }
            break;
        case 7:
            if (input[inputType]) {
                if (rand() % 3 == 0) {
                    NEXUS_VideoInputSettings settings;
                    NEXUS_VideoInput_GetSettings(input[inputType], &settings);
                    if (inputType != NEXUS_VideoInputList_eVideoDecoder0 &&
                        inputType != NEXUS_VideoInputList_eVideoDecoder1) {
                        settings.mute = rand() % 2;
                        settings.repeat = rand() % 2;
                    }
                    settings.muteColor = rand();
                    rc = NEXUS_VideoInput_SetSettings(input[inputType], &settings);
                    BDBG_ASSERT(!rc);
                }

#if 0
                if (rand() % 10 == 0) {
                    if (resumeMode[inputType] == NEXUS_VideoInputResumeMode_eManual) {
                        rc = NEXUS_VideoInput_ForcePending(input[inputType]);
                        BDBG_ASSERT(!rc);
                    }
                }

                if (connectedInput[windowIndex] == inputType && rand() % 10 == 0) {
                    NEXUS_VideoInputResumeMode mode;
                    switch (rand() % 3) {
                    case 0: mode = resumeMode[inputType] = NEXUS_VideoInputResumeMode_eAuto; break;
                    case 1: mode = resumeMode[inputType] = NEXUS_VideoInputResumeMode_eManual; break;
                    case 2:
                        if (resumeMode[inputType] == NEXUS_VideoInputResumeMode_eManual) {
                            mode = NEXUS_VideoInputResumeMode_eNow;
                        }
                        break;
                    }
                    rc = NEXUS_VideoInput_SetResumeMode(input[inputType], mode);
                    BDBG_ASSERT(!rc);
                }
#endif

                if (rand() % 3 == 0) {
                    NEXUS_VideoInputStatus status;
                    rc = NEXUS_VideoInput_GetStatus(input[inputType], &status);
                    BDBG_ASSERT(!rc);
                }
            }
            break;
        default:
            break;
        }

        if (loops % 5000 == 0) {
            BDBG_WRN(("Connections:"));
            for (i=0;i<NEXUS_VideoInputList_eMax;i++) {
                BDBG_WRN(("  %s: %d", g_videoInputStr[i], connectCount[i]));
            }
        }
    }
  
    /* Make sure all handlers are closed properly  */ 
#if NEXUS_NUM_HDMI_INPUTS
    for ( i = 0 ; i < NEXUS_NUM_HDMI_INPUTS ; i++ )
    {
        if (hdmiInput[i]) {
            NEXUS_HdmiInput_Close(hdmiInput[i]);
            hdmiInput[i] = NULL;
        }
    }
#endif
    
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    if (analogVideoDecoder) {
        NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
        analogVideoDecoder = NULL;
    }    
#endif

    for ( i = 0 ; i < NEXUS_NUM_VIDEO_DECODERS ; i++ )
    {
        if (videoDecoder[i]) {
            NEXUS_VideoDecoder_Close(videoDecoder[i]);
            videoDecoder[i] = NULL;
            videoDecoderStarted[i] = false;
        }    
    }

    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}

NEXUS_PidChannelHandle videoPidChannel;

static NEXUS_Error start_decode(NEXUS_VideoDecoderHandle videoDecoder)
{
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_Error rc;

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);

    /* TODO: a global */
    if (!videoPidChannel) {
        videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);
    }

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);

    return 0;
}


