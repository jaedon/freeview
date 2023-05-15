/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: pcinput.c $
* $brcm_Revision: Hydra_Software_Devel/7 $
* $brcm_Date: 9/22/09 12:11p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/pcinput.c $
* 
* Hydra_Software_Devel/7   9/22/09 12:11p erickson
* SW3548-2492: add more states
*
* Hydra_Software_Devel/5   8/4/08 11:40a erickson
* PR45280: update tests for current 3556
*
* Hydra_Software_Devel/4   5/14/08 2:30p erickson
* PR39257: add bypass_display test
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
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_svideo_input.h"
#include "nexus_component_input.h"
#include "nexus_pc_input.h"
#include "nexus_analog_video_decoder.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_PcInputHandle pcInput = NULL;
    NEXUS_PcInputSettings pcInputSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    unsigned loops = 500;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Bring up display for 720p component output only */
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up VDEC */
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);

    pcInput = platformConfig.inputs.pc[0];
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_PcInput_GetConnector(pcInput), &analogVideoDecoderSettings);
    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);

    /* save for reopening */
    NEXUS_PcInput_GetSettings(pcInput, &pcInputSettings);

    while (--loops) {
        if (loops % 10 == 0) printf("loops %d\n", loops);
        switch (rand() % 5) {
        case 0:
            if (!pcInput) {
                pcInput = NEXUS_PcInput_Open(0, &pcInputSettings);
                BDBG_ASSERT(pcInput);
            }
            break;
        case 1:
            if (pcInput && !analogVideoDecoderSettings.input) {
                NEXUS_PcInput_Close(pcInput);
                pcInput = NULL;
            }
            break;
        case 2:
            if (pcInput && !analogVideoDecoderSettings.input) {
                analogVideoDecoderSettings.input = NEXUS_PcInput_GetConnector(pcInput);
                rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 3:
            if (analogVideoDecoderSettings.input) {
                analogVideoDecoderSettings.input = NULL;
                rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 4:
            if (pcInput) {
                pcInputSettings.manualAdjustEnabled = rand() % 2;
                rc = NEXUS_PcInput_SetSettings(pcInput, &pcInputSettings);
                BDBG_ASSERT(!rc);
            }
            break;
        }
        BKNI_Sleep(rand() % 500);
    }

#endif
    return 0;
}

