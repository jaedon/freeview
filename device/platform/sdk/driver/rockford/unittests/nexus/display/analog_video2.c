/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: analog_video2.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 6/1/09 11:51a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/analog_video2.c $
* 
* Hydra_Software_Devel/2   6/1/09 11:51a erickson
* PR55607: make test finite, faster
*
* Hydra_Software_Devel/1   1/19/09 12:25p erickson
* PR50742: test app which recreated PR 50742 quickly
*
* Hydra_Software_Devel/8   11/26/08 11:08a erickson
* PR42679: automate test
*
* Hydra_Software_Devel/7   10/23/08 3:59p erickson
* PR42679: update
*
* Hydra_Software_Devel/6   8/7/08 8:33a erickson
* PR45280: use NEXUS_AnalogVideoDecoder_GetDynamicDefaultSettings
*
* Hydra_Software_Devel/5   8/4/08 12:34p erickson
* PR45214: update status
*
* Hydra_Software_Devel/4   8/4/08 11:40a erickson
* PR45280: update tests for current 3556
*
* Hydra_Software_Devel/3   2/4/08 10:39a erickson
* PR38679: update for 740x
*
* Hydra_Software_Devel/2   1/23/08 12:38p erickson
* PR38919: update unittests
*
* Hydra_Software_Devel/1   12/19/07 11:37a erickson
* PR38213: added display unittests
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#include "nexus_video_input_vbi.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_svideo_input.h"
#include "nexus_component_input.h"
#include "nexus_pc_input.h"
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

void vbi_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    /* push some app delay back into nexus */
    BKNI_Sleep(rand() % 20);
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
    unsigned count = 0;
    unsigned loops = 5000;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Bring up display for 720p component output only */
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    display = NEXUS_Display_Open(0, &displayCfg);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    window = NEXUS_VideoWindow_Open(display, 0);

    while (--loops) {
        NEXUS_VideoInput videoInput;
        NEXUS_VideoInputVbiSettings videoInputVbiSettings;

        analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
        videoInput = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);

        NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
        rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
        BDBG_ASSERT(!rc);

        NEXUS_VideoInput_GetVbiSettings(videoInput, &videoInputVbiSettings);
        videoInputVbiSettings.closedCaptionEnabled = true;
        videoInputVbiSettings.closedCaptionDataReady.callback = vbi_callback;
        videoInputVbiSettings.teletextEnabled = true;
        videoInputVbiSettings.teletextBufferSize = 1000;
        videoInputVbiSettings.teletextDataReady.callback = vbi_callback;
        videoInputVbiSettings.gemStarEnabled = true;
        videoInputVbiSettings.gemStarBufferSize = 1000;
        videoInputVbiSettings.wssChanged.callback = vbi_callback;
        videoInputVbiSettings.cgmsChanged.callback = vbi_callback;
        rc = NEXUS_VideoInput_SetVbiSettings(videoInput, &videoInputVbiSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_VideoWindow_AddInput(window, videoInput);
        BDBG_ASSERT(!rc);

        if (rand() % 3 == 0) {
            BKNI_Sleep(10 * (rand() % 4));
        }

        NEXUS_VideoWindow_RemoveAllInputs(window);

        NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
        count++;
        if (count % 10 == 0) printf("%d\n", count);
    }

#endif

    return 0;
}


