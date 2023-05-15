/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: analog_video.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 11/26/08 11:08a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/analog_video.c $
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

int main(void)
{
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoWindowMadSettings madSettings;
    NEXUS_VideoWindowAnrSettings anrSettings;
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    unsigned input_type;

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
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));

    /* select input */
#define MAX_INPUT_TYPES 4
    for (input_type=0;input_type<MAX_INPUT_TYPES;input_type++) {
        unsigned inputs, index;
        switch (input_type) {
        case 0: inputs = NEXUS_NUM_COMPOSITE_INPUTS; break;
        case 1: inputs = NEXUS_NUM_COMPONENT_INPUTS; break;
        case 2: inputs = NEXUS_NUM_SVIDEO_INPUTS; break;
        case 3: inputs = NEXUS_NUM_PC_INPUTS; break;
        default: BDBG_ASSERT(0); break;
        }

        for (index=0;index<inputs;index++) {
            NEXUS_AnalogVideoDecoderStatus status;
            unsigned loops;
            uint32_t coverage = 0;

            switch (input_type) {
            case 0:
                printf("composite %d\n", index);
                NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[index]), &analogVideoDecoderSettings);
                break;
            case 1:
                printf("component %d\n", index);
                NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_ComponentInput_GetConnector(platformConfig.inputs.component[index]), &analogVideoDecoderSettings);
                break;
            case 2:
                printf("svideo %d\n", index);
                NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_SvideoInput_GetConnector(platformConfig.inputs.svideo[index]), &analogVideoDecoderSettings);
                break;
            case 3:
                printf("pc %d\n", index);
                NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_PcInput_GetConnector(platformConfig.inputs.pc[index]), &analogVideoDecoderSettings);
                break;
            default:
                BDBG_ASSERT(0);
                break;
            }

            rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
            BDBG_ASSERT(!rc);

            printf("Tests: ");
            loops = 300;

            /* this allows for both randomization and ensures coverage of at least 1 instance of each test */
#define MAX_TESTS 15 /* max is 32 */
            while (--loops || (coverage != ((1<<MAX_TESTS)-1))) {
                unsigned test_index = rand() % MAX_TESTS;
                printf("%d, ", test_index);
                coverage |= (1 << test_index);
                switch (test_index) {
                case 0:
                    NEXUS_VideoWindow_GetMadSettings(window, &madSettings);
                    madSettings.deinterlace = false;
                    rc = NEXUS_VideoWindow_SetMadSettings(window, &madSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 1:
                    NEXUS_VideoWindow_GetMadSettings(window, &madSettings);
                    madSettings.deinterlace = true;
                    rc = NEXUS_VideoWindow_SetMadSettings(window, &madSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 2:
                    NEXUS_VideoWindow_GetAnrSettings(window, &anrSettings);
                    anrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eEnable;
                    rc = NEXUS_VideoWindow_SetAnrSettings(window, &anrSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 3:
                    NEXUS_VideoWindow_GetAnrSettings(window, &anrSettings);
                    anrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eDisable;
                    rc = NEXUS_VideoWindow_SetAnrSettings(window, &anrSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 4:
                    rc = NEXUS_VideoWindow_SetBrightness(window, (rand() % 65536) - 32768);
                    BDBG_ASSERT(!rc);
                    break;

                case 5:
                    rc = NEXUS_VideoWindow_SetBrightness(window, 1000);
                    BDBG_ASSERT(!rc);
                    break;

                case 6:
                    NEXUS_AnalogVideoDecoder_GetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    analogVideoDecoderSettings.position.enabled = true;
                    analogVideoDecoderSettings.position.hStart = 100;
                    analogVideoDecoderSettings.position.vStart = 100;
                    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 7:
                    NEXUS_AnalogVideoDecoder_GetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    analogVideoDecoderSettings.position.enabled = false;
                    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 8:
                    NEXUS_AnalogVideoDecoder_GetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    analogVideoDecoderSettings.analogGain.analogGain[0] = 0;
                    analogVideoDecoderSettings.analogGain.analogGain[1] = 0;
                    analogVideoDecoderSettings.analogGain.analogGain[2] = 0;
                    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 9:
                    NEXUS_AnalogVideoDecoder_GetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    analogVideoDecoderSettings.awc.enableAwc[0] = 0;
                    analogVideoDecoderSettings.awc.enableAwc[1] = 0;
                    analogVideoDecoderSettings.awc.enableAwc[2] = 0;
                    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 10:
                    rc = NEXUS_AnalogVideoDecoder_GetStatus(analogVideoDecoder, &status);
                    BDBG_ASSERT(!rc);
#if 0
                    printf("locked:   %c\n", status.locked?'y':'n');
                    if (status.unknownFormat) {
                        printf("unknown format\n");
                        printf("noSignal: %c\n", status.noSignal?'y':'n');
                    }
                    else{
                        printf("format:   %d\n", status.originalFormat);
                        printf("size:     %dx%d\n", status.avWidth, status.avHeight);
                        printf("freq:     h=%d, v=%d\n", status.hFreq, status.vFreq);
                        printf("polarity: h=%d, v=%d\n", status.hPolarity, status.hPolarity);
                        printf("blank:    h=%d, v=%d\n", status.hBlank, status.vBlank);
                    }
                    printf("\n");
#endif
                    break;

                case 11:
                    /* By using the boolean, it allows VDC to change underneath */
                    NEXUS_AnalogVideoDecoder_GetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    analogVideoDecoderSettings.useDynamicDefaults = true;
                    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 12:
                    /* By calling GetDynamicDefaultSettings, we get the same settings at first, but VDC is then not allowed to change underneath. */
                    NEXUS_AnalogVideoDecoder_GetDynamicDefaultSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 13:
                    NEXUS_AnalogVideoDecoder_GetSettings(analogVideoDecoder, &analogVideoDecoderSettings);

                    /* TODO: code to turn off dynamic defaults */
                    analogVideoDecoderSettings.s3DComb.useDynamicDefaults = true;
                    analogVideoDecoderSettings.digitalAgc.useDynamicDefaults = true;
                    analogVideoDecoderSettings.awc.useDynamicDefaults = true;
                    analogVideoDecoderSettings.ctiSettings.useDynamicDefaults = true;
                    analogVideoDecoderSettings.analogGain.useDynamicDefaults = true;
                    analogVideoDecoderSettings.delay.useDynamicDefaults = true;
                    analogVideoDecoderSettings.dcrSettings.useDynamicDefaults = true;

                    analogVideoDecoderSettings.useDynamicDefaults = false;
                    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
                    BDBG_ASSERT(!rc);
                    break;

                case 14:
                    rc = NEXUS_AnalogVideoDecoder_ResetInput(analogVideoDecoder);
                    BDBG_ASSERT(!rc);
                    break;

                case MAX_TESTS: /* if you get a duplicate case statement here, check the value of MAX_TESTS */
                default:
                   BDBG_ASSERT(0);
                   break;
                }
            }
            printf("done. coverage=%#x\n", coverage);
        }

        analogVideoDecoderSettings.input = NULL;
        NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    }
#endif

    return 0;
}


