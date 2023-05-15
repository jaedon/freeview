/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: open.c $
* $brcm_Revision: Hydra_Software_Devel/12 $
* $brcm_Date: 4/5/12 1:37p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/open.c $
* 
* Hydra_Software_Devel/12   4/5/12 1:37p katrep
* SW7405-3559: Don't use NEXUS_DisplayTimingGenerator_eEncoder if Raaga
* encoder is enabled
* 
* Hydra_Software_Devel/11   1/24/12 6:22p mward
* SW7405-5339:  Don't use NEXUS_DisplayTimingGenerator_eEncoder if no
* video encoders.
* 
* Hydra_Software_Devel/10   7/18/11 2:02p mward
* SW7405-5339:  Don't use NEXUS_DisplayTimingGenerator_e656Output if no
* 656 outputs.
* 
* Hydra_Software_Devel/9   6/17/11 5:23p erickson
* SW7405-5339: enable NEXUS_Display_Open test, reduce loops
* 
* Hydra_Software_Devel/8   5/27/11 2:00p erickson
* SW7405-5339: modify display open test
* 
* Hydra_Software_Devel/7   5/10/10 6:55p mward
* SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
* and other errors and warnings.
* 
* Hydra_Software_Devel/6   4/6/09 2:20p erickson
* PR42679: update
*
* Hydra_Software_Devel/5   12/8/08 1:55p erickson
* PR50020: protect against destroyed tf_isr/bf_isr INT callback handles
*
* Hydra_Software_Devel/4   9/2/08 5:24p erickson
* PR46473: remove TODO
*
* Hydra_Software_Devel/3   8/19/08 12:22p erickson
* PR45794: add more AnalogVideoDecoder shutdown cases
*
* Hydra_Software_Devel/2   8/4/08 11:40a erickson
* PR45280: update tests for current 3556
*
* Hydra_Software_Devel/1   7/7/08 4:06p erickson
* PR44619: test variations of open/close
*
* Hydra_Software_Devel/3   2/4/08 10:39a erickson
* PR38679: update for 740x
*
* Hydra_Software_Devel/2   1/23/08 12:38p erickson
* PR38919: update unittests
*
* Hydra_Software_Devel/1   12/19/07 12:47p erickson
* PR38213: added autodetect
*
* Hydra_Software_Devel/1   12/19/07 11:37a erickson
* PR38213: added display unittests
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_PANEL_OUTPUTS
#include "nexus_panel_output.h"
#endif
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_svideo_input.h"
#include "nexus_component_input.h"
#include "nexus_pc_input.h"
#include "nexus_analog_video_decoder.h"
#include "nexus_video_adj.h"
#include "nexus_picture_ctrl.h"
#endif
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(open);

int main(void)
{
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
#endif
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_ComponentOutputHandle component;
#endif
#if NEXUS_NUM_PANEL_OUTPUTS
    NEXUS_PanelOutputHandle panel;
#endif
    unsigned i;
    void *handle0;
    void *handle1;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
#if NEXUS_DTV_PLATFORM
    platformSettings.openInputs = false;
#endif
    platformSettings.openOutputs = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Bring up display for 720p component output only */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    {
    NEXUS_ComponentOutputSettings settings;
    NEXUS_ComponentOutput_GetDefaultSettings(&settings);
    settings.dacs.YPrPb.dacY = NEXUS_VideoDac_e0;
    settings.dacs.YPrPb.dacPr = NEXUS_VideoDac_e2;
    settings.dacs.YPrPb.dacPb = NEXUS_VideoDac_e1;
    component = NEXUS_ComponentOutput_Open(0, &settings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(component));
    BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_PANEL_OUTPUTS
    panel = NEXUS_PanelOutput_Open(0, NULL);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(panel));
    BDBG_ASSERT(!rc);
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);
#endif

#if NEXUS_NUM_COMPONENT_OUTPUTS
    /* close output without removing */
    NEXUS_ComponentOutput_Close(component);
#endif
#if NEXUS_NUM_PANEL_OUTPUTS
    /* close output without removing */
    NEXUS_PanelOutput_Close(panel);
#endif
    /* auto remove intputs */
    NEXUS_VideoWindow_Close(window);
    /* auto remove outputs */
    NEXUS_Display_Close(display);

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
#endif

#if NEXUS_NUM_PANEL_OUTPUTS
    for (i=0;i<10;i++) {
        handle0 = NEXUS_PanelOutput_Open(0, NULL);
        BDBG_ASSERT(handle0);
        handle1 = NEXUS_PanelOutput_Open(0, NULL);
        BDBG_ASSERT(!handle1);
        NEXUS_PanelOutput_Close(handle0);
    }
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
    for (i=0;i<10;i++) {
        NEXUS_ComponentOutputSettings settings;
        NEXUS_ComponentOutput_GetDefaultSettings(&settings);
        handle0 = NEXUS_ComponentOutput_Open(0, &settings);
        BDBG_ASSERT(handle0);
        handle1 = NEXUS_ComponentOutput_Open(0, &settings);
        BDBG_ASSERT(!handle1);
        NEXUS_ComponentOutput_Close((NEXUS_ComponentOutputHandle)handle0);
    }
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    for (i=0;i<10;i++) {
        NEXUS_CompositeOutputSettings settings;
        NEXUS_CompositeOutput_GetDefaultSettings(&settings);
        handle0 = NEXUS_CompositeOutput_Open(0, &settings);
        BDBG_ASSERT(handle0);
        handle1 = NEXUS_CompositeOutput_Open(0, &settings);
        BDBG_ASSERT(!handle1);
        NEXUS_CompositeOutput_Close((NEXUS_CompositeOutputHandle)handle0);
    }
#endif

    /* reopen platform with inputs/outputs so we can do live tests */
    NEXUS_Platform_Uninit();
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    /* test variations of NEXUS_Display_Open */
    for (i=0;i<100;i++) {
        unsigned index = rand() % NEXUS_NUM_DISPLAYS;
        NEXUS_DisplaySettings displaySettings;
        NEXUS_Display_GetDefaultSettings(&displaySettings);
        displaySettings.format = rand() % NEXUS_VideoFormat_eMax;
        displaySettings.displayType = rand() % NEXUS_DisplayType_eMax;
        if (displaySettings.displayType == NEXUS_DisplayType_eDvo || displaySettings.displayType == NEXUS_DisplayType_eLvds) {
            index = 0;
        }
        do {
            displaySettings.timingGenerator = rand() % NEXUS_DisplayTimingGenerator_eMax;
        } while ( 
        #if (NEXUS_NUM_656_OUTPUTS==0)
                 (displaySettings.timingGenerator == NEXUS_DisplayTimingGenerator_e656Output) ||
        #endif
        #if (NEXUS_NUM_VIDEO_ENCODERS==0 || NEXUS_NUM_DSP_VIDEO_ENCODERS!=0)
                 (displaySettings.timingGenerator == NEXUS_DisplayTimingGenerator_eEncoder) ||
        #endif
        0 );
        BDBG_WRN(("displaySettings.timingGenerator = %u",displaySettings.timingGenerator));
        displaySettings.aspectRatio = rand() % NEXUS_DisplayAspectRatio_eMax;
        display = NEXUS_Display_Open(index, &displaySettings);
        if (display) {
            NEXUS_Display_Close(display);
        }
        else {
            BDBG_WRN(("cannot open display: %d %d %d %d %d", index, displaySettings.format, displaySettings.displayType, displaySettings.timingGenerator, displaySettings.aspectRatio));
        }
    }

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    /**
    test variations of AnalogVideoDecoder Close
    **/
    /* Bring up display for 720p component output only */
    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);
    /* if you close the window first, the input is automatically removed */
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);

    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_Close(window);
    /* order to NEXUS_Display_Close and NEXUS_AnalogVideoDecoder_Close should not matter */
    NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
    NEXUS_Display_Close(display);

    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_RemoveInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);
    /* multiple calls to NEXUS_VideoInput_Shutdown are allowed */
    NEXUS_VideoInput_Shutdown(NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    NEXUS_VideoInput_Shutdown(NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    NEXUS_VideoInput_Shutdown(NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);

    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_RemoveInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder)); /* temp */
    BDBG_ASSERT(!rc);
    /* NEXUS_VideoInput_Shutdown is not required for AnalogVideoDecoder */
    NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);

    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);
    NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);

    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);
    NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
#endif

    printf("calling NEXUS_Platform_Uninit\n");

    NEXUS_Platform_Uninit();

    return 0;
}

