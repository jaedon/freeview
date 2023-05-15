/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: video_input_status.c $
* $brcm_Revision: Hydra_Software_Devel/7 $
* $brcm_Date: 9/21/09 2:57p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/video_input_status.c $
* 
* Hydra_Software_Devel/7   9/21/09 2:57p erickson
* SW3548-2475: add NEXUS_AnalogVideoDecoderStatus
*
* Hydra_Software_Devel/6   10/23/08 3:59p erickson
* PR42679: update
*
* Hydra_Software_Devel/5   8/4/08 11:40a erickson
* PR45280: update tests for current 3556
*
* Hydra_Software_Devel/4   3/12/08 5:24p erickson
* PR40412: added analogVideoDecoderSettings.sourceChanged
*
* Hydra_Software_Devel/3   2/4/08 10:39a erickson
* PR38679: update for 740x
*
* Hydra_Software_Devel/2   1/23/08 12:38p erickson
* PR38919: update unittests
*
* Hydra_Software_Devel/1   1/9/08 12:38p erickson
* PR38213: added video_input_status.c test
*
* Nexus_Devel/12   1/3/08 9:20p erickson
* PR38213: rename to AnalogVideoDecoder
*
* Nexus_Devel/11   12/28/07 3:55p erickson
* PR38213: added all analog inputs using PlatformConfiguration
*
* Nexus_Devel/10   12/19/07 11:37a erickson
* PR38213: simplified example to component input only. see
* rockford/unittests/nexus/display for test code.
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_component_input.h"
#include "nexus_analog_video_decoder.h"
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

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
NEXUS_DisplayHandle display;
NEXUS_VideoWindowHandle window;
NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;

const char *aspectRatioStr[NEXUS_AspectRatio_eMax] = {
    "NEXUS_AspectRatio_eUnknown",
    "NEXUS_AspectRatio_eSquarePixel",
    "NEXUS_AspectRatio_e4x3",
    "NEXUS_AspectRatio_e16x9",
    "NEXUS_AspectRatio_e221x1",
    "NEXUS_AspectRatio_e15x9",
    "NEXUS_AspectRatio_eSar"};

const char *videoFormatStr[NEXUS_VideoFormat_eMax] = {
    "NEXUS_VideoFormat_eUnknown",
    "NEXUS_VideoFormat_eNtsc",
    "NEXUS_VideoFormat_eNtscJapan",
    "NEXUS_VideoFormat_ePalM",
    "NEXUS_VideoFormat_ePalN",
    "NEXUS_VideoFormat_ePalNc",
    "NEXUS_VideoFormat_ePalB",
    "NEXUS_VideoFormat_ePalB1",
    "NEXUS_VideoFormat_ePalD",
    "NEXUS_VideoFormat_ePalD1",
    "NEXUS_VideoFormat_ePalG",
    "NEXUS_VideoFormat_ePalH",
    "NEXUS_VideoFormat_ePalK",
    "NEXUS_VideoFormat_ePalI",
    "NEXUS_VideoFormat_eSecam",
    "NEXUS_VideoFormat_e480p",
    "NEXUS_VideoFormat_e576p",
    "NEXUS_VideoFormat_e1080i",
    "NEXUS_VideoFormat_e1080i50hz",
    "NEXUS_VideoFormat_e1080p",
    "NEXUS_VideoFormat_e1080p24hz",
    "NEXUS_VideoFormat_e1080p25hz",
    "NEXUS_VideoFormat_e1080p30hz",
    "NEXUS_VideoFormat_e1080p60hz",
    "NEXUS_VideoFormat_e1250i50hz",
    "NEXUS_VideoFormat_e720p",
    "NEXUS_VideoFormat_e720p50hz",
    "NEXUS_VideoFormat_e720p24hz",
    "NEXUS_VideoFormat_eVesa",
    "NEXUS_VideoFormat_eCustom0",
    "NEXUS_VideoFormat_eCustom1"};

const char *frameRateStr[NEXUS_VideoFrameRate_eMax] = {
    "NEXUS_VideoFrameRate_eUnknown",
    "NEXUS_VideoFrameRate_e23_976",
    "NEXUS_VideoFrameRate_e24",
    "NEXUS_VideoFrameRate_e25",
    "NEXUS_VideoFrameRate_e29_97",
    "NEXUS_VideoFrameRate_e30",
    "NEXUS_VideoFrameRate_e50",
    "NEXUS_VideoFrameRate_e59_94",
    "NEXUS_VideoFrameRate_e60"};

void getstatus(void)
{
    NEXUS_VideoInputStatus status;
    NEXUS_AnalogVideoDecoderStatus analogStatus;

    NEXUS_VideoInput_GetStatus(NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder), &status);
    printf("VideoInput Status videoPresent=%c, %dx%d, %s, %s, %s\n",
        status.videoPresent?'y':'n', status.width, status.height, aspectRatioStr[status.aspectRatio], videoFormatStr[status.format], frameRateStr[status.frameRate]);

    NEXUS_AnalogVideoDecoder_GetStatus(analogVideoDecoder, &analogStatus);
    printf("AnalogVideoDecoder Status locked=%d, noSignal=%d, unknownFormat=%d\n",
        analogStatus.locked, analogStatus.noSignal, analogStatus.unknownFormat);
}

void sourceChanged(void *context, int param)
{
    BSTD_UNUSED(param);
    printf("VideoInput source changed: %p\n", context);
    getstatus();
}

void analogSourceChanged(void *context, int param)
{
    BSTD_UNUSED(param);
    printf("AnalogVideoDecoder source changed: %p\n", context);
    getstatus();
}
#endif

int main(void)
{
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_DisplaySettings displayCfg;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_VideoInputSettings videoInputSettings;
    NEXUS_VideoInput videoInput;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    unsigned i;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

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

    /* Bring up VDEC and connect to the VideoWindow */
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    videoInput = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);
    NEXUS_VideoWindow_AddInput(window, videoInput);

    /* TODO: must SetSettings after connnection */
    NEXUS_VideoInput_GetSettings(videoInput, &videoInputSettings);
    videoInputSettings.sourceChanged.callback = sourceChanged;
    videoInputSettings.sourceChanged.context = videoInput;
    NEXUS_VideoInput_SetSettings(videoInput, &videoInputSettings);

    for (i=0;i<NEXUS_NUM_COMPONENT_INPUTS;i++) {
        if (!platformConfig.inputs.component[i]) continue;
        videoInput = NEXUS_ComponentInput_GetConnector(platformConfig.inputs.component[i]);
        NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(videoInput, &analogVideoDecoderSettings);
        analogVideoDecoderSettings.sourceChanged.callback = analogSourceChanged;
        analogVideoDecoderSettings.sourceChanged.context = analogVideoDecoder;
        NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);

        printf("Component %d\n", i);
        BKNI_Sleep(10000);
        getstatus();
    }

    for (i=0;i<NEXUS_NUM_COMPOSITE_INPUTS;i++) {
        if (!platformConfig.inputs.composite[i]) continue;
        videoInput = NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[i]);
        NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(videoInput, &analogVideoDecoderSettings);
        analogVideoDecoderSettings.sourceChanged.callback = analogSourceChanged;
        analogVideoDecoderSettings.sourceChanged.context = analogVideoDecoder;
        NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
        printf("Composite %d\n", i);
        BKNI_Sleep(10000);
        getstatus();
    }

    for (i=0;i<NEXUS_NUM_PC_INPUTS;i++) {
        if (!platformConfig.inputs.pc[i]) continue;
        videoInput = NEXUS_PcInput_GetConnector(platformConfig.inputs.pc[i]);
        NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(videoInput, &analogVideoDecoderSettings);
        analogVideoDecoderSettings.sourceChanged.callback = analogSourceChanged;
        analogVideoDecoderSettings.sourceChanged.context = analogVideoDecoder;
        NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
        printf("PC %d\n", i);
        BKNI_Sleep(10000);
        getstatus();
    }
#endif

    return 0;
}
