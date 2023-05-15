/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: analog_channel_change.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 5/7/10 6:36p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/analog_channel_change.c $
* 
* Hydra_Software_Devel/4   5/7/10 6:36p mward
* SW7125-253:  Allow compilation with C++, eliminate "invalid conversion"
* errors.
* 
* Hydra_Software_Devel/3   11/3/08 10:42a erickson
* PR42679: add status
*
* Hydra_Software_Devel/2   10/24/08 11:30a erickson
* PR42679: shorten test
*
* Hydra_Software_Devel/1   8/22/08 11:08p erickson
* PR45711: added unittest for analog channel change
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#include "nexus_video_input_vbi.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_analog_video_decoder.h"
#endif
#include "nexus_picture_ctrl.h"
#include "nexus_video_adj.h"
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

void source_pending_callback(void *context, int param)
{
    NEXUS_VideoInput videoInput = (NEXUS_VideoInput)context;
    BSTD_UNUSED(param);
    printf("source pending\n");
    NEXUS_VideoInput_SetResumeMode(videoInput, NEXUS_VideoInputResumeMode_eNow);
}

int main(void)
{
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_VideoWindowHandle window;
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    unsigned total_loops = 200;
    unsigned loops = 0;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Bring up display for 720p component output only */
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
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up VDEC */
    while (loops++ < total_loops) {
        NEXUS_VideoInput videoInput;
        NEXUS_VideoInputSettings settings;
        NEXUS_VideoInputVbiSettings vbiSettings;

        analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);

        videoInput = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);

        NEXUS_VideoInput_GetVbiSettings(videoInput, &vbiSettings);
        vbiSettings.closedCaptionEnabled = true;
        rc = NEXUS_VideoInput_SetVbiSettings(videoInput, &vbiSettings);
        BDBG_ASSERT(!rc);

        NEXUS_VideoInput_SetResumeMode(videoInput, NEXUS_VideoInputResumeMode_eManual);
        NEXUS_VideoInput_GetSettings(videoInput, &settings);
        settings.sourcePending.callback = source_pending_callback;
        settings.sourcePending.context = videoInput;
        rc = NEXUS_VideoInput_SetSettings(videoInput, &settings);
        BDBG_ASSERT(!rc);

        NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
        rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_VideoWindow_AddInput(window, videoInput);
        BDBG_ASSERT(!rc);

        BKNI_Sleep(rand() % 1000);

        NEXUS_VideoWindow_RemoveAllInputs(window);
        NEXUS_VideoInput_Shutdown(videoInput);
        NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);

        if (loops % 20 == 0) printf("%d channel changes done\n", loops);
    }

    NEXUS_Platform_Uninit();
#endif

    return 0;
}

