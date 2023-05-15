/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: pq_custom_data.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 3/26/10 12:27p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/pq_custom_data.c $
* 
* Hydra_Software_Devel/2   3/26/10 12:27p erickson
* SW3548-2858: update for settop
*
* Hydra_Software_Devel/1   3/26/10 11:46a erickson
* SW3548-2858: adding test for anr/dnr/contrast stretch custom data
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_svideo_input.h"
#include "nexus_component_input.h"
#include "nexus_pc_input.h"
#include "nexus_analog_video_decoder.h"
#else
#include "nexus_video_decoder.h"
#endif
#include "nexus_picture_ctrl.h"
#include "nexus_video_adj.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_video_window_tnt.h"
#include "nexus_video_window_xvycc.h"
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#include "nexus_custom_video_adjust.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(pq_custom_data);

int main(void)
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoInput videoInput;
    NEXUS_PictureCtrlContrastStretch contrastStretch;
    NEXUS_Error rc;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
#else
    NEXUS_VideoDecoderHandle videoDecoder;
#endif
#if NEXUS_DTV_PLATFORM
    NEXUS_VideoWindowAnrSettings anrSettings;
#endif
    NEXUS_VideoWindowDnrSettings dnrSettings;
#define DATASIZE 1024
    unsigned char data[DATASIZE];
    unsigned loops = 1000;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    display = NEXUS_Display_Open(0, &displayCfg);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    displayCfg.displayType = NEXUS_DisplayType_eAuto;
    displayCfg.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displayCfg);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    videoInput = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);
#else
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    videoInput = NEXUS_VideoDecoder_GetConnector(videoDecoder);
#endif
    rc = NEXUS_VideoWindow_AddInput(window, videoInput);
    BDBG_ASSERT(!rc);

    BKNI_Memset(data, 0, DATASIZE);
    while (--loops) {
        unsigned action = rand() % 6;
        printf("action %d\n", action);
        switch (action) {
#if NEXUS_DTV_PLATFORM
        case 0:
            NEXUS_VideoWindow_GetAnrSettings(window, &anrSettings);
            anrSettings.anr.mode = rand() % NEXUS_VideoWindowFilterMode_eMax;
            rc = NEXUS_VideoWindow_SetCustomAnrSettings(window, &anrSettings, data, DATASIZE);
            BDBG_ASSERT(!rc);
            break;
        case 1:
            NEXUS_VideoWindow_GetAnrSettings(window, &anrSettings);
            rc = NEXUS_VideoWindow_SetCustomAnrSettings(window, &anrSettings, NULL, 0);
            BDBG_ASSERT(!rc);
            break;
#endif
        case 2:
            NEXUS_VideoWindow_GetDnrSettings(window, &dnrSettings);
            dnrSettings.bnr.mode = rand() % NEXUS_VideoWindowFilterMode_eMax;
            dnrSettings.mnr.mode = rand() % NEXUS_VideoWindowFilterMode_eMax;
            dnrSettings.dcr.mode = rand() % NEXUS_VideoWindowFilterMode_eMax;
            rc = NEXUS_VideoWindow_SetCustomDnrSettings(window, &dnrSettings, data, DATASIZE);
            BDBG_ASSERT(!rc);
            break;
        case 3:
            NEXUS_VideoWindow_GetDnrSettings(window, &dnrSettings);
            rc = NEXUS_VideoWindow_SetCustomDnrSettings(window, &dnrSettings, NULL, 0);
            BDBG_ASSERT(!rc);
            break;
        case 4:
            NEXUS_PictureCtrl_GetContrastStretch(window, &contrastStretch);
            contrastStretch.enabled = rand() % 2;
            rc = NEXUS_PictureCtrl_SetCustomContrastStretch(window, &contrastStretch, data, DATASIZE/sizeof(int16_t));
            BDBG_ASSERT(!rc);
            break;
        case 5:
            NEXUS_PictureCtrl_GetContrastStretch(window, &contrastStretch);
            rc = NEXUS_PictureCtrl_SetCustomContrastStretch(window, &contrastStretch, NULL, 0);
            BDBG_ASSERT(!rc);
            break;
        }

        BKNI_Sleep(rand() % 200);
    }

    return 0;
}
