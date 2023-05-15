/***************************************************************************
*     (c)2004-2009 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: panel_tests.c $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 2/26/10 5:19p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/panel_tests.c $
* 
* Hydra_Software_Devel/6   2/26/10 5:19p erickson
* SW3548-2793: extend test
*
* Hydra_Software_Devel/5   5/20/09 10:08a erickson
* PR55292: added one more test
*
* Hydra_Software_Devel/4   5/20/09 10:01a erickson
* PR55292: add more tests
*
* Hydra_Software_Devel/3   4/6/09 2:20p erickson
* PR42679: update
*
* Hydra_Software_Devel/2   3/23/09 1:31p erickson
* PR42679: update
*
* Hydra_Software_Devel/1   2/24/09 11:17a erickson
* PR52428: add panel_tests
*
* 1   2/23/09 11:45a erickson
* PR52270: added example to test VDEC->VEC bypass for composite output
*
* 9   2/4/09 3:50p erickson
* PR51841: refactor
*
* 8   10/2/08 12:56p erickson
* PR40799: add shutdown and error checking
*
* 7   9/8/08 9:45a erickson
* PR46068: added panel runtime option
*
* 6   6/23/08 5:15p vsilyaev
* PR 40921: Added SCART support
*
* 5   6/20/08 5:51p vsilyaev
* PR 40921: Use NTSC resolution if no component output
*
* 4   6/19/08 2:33p vsilyaev
* PR 40921: There is no component output on the 93556 platform
*
* 3   1/23/08 9:22p vobadm
* PR35457: update docs
*
* 2   1/23/08 12:39p erickson
* PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
*
* 1   1/18/08 2:13p jgarrett
* PR 38808: Merging to main branch
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
#include "nexus_video_output.h"
#include "nexus_composite_output.h"
#include "nexus_video_window.h"
#if NEXUS_NUM_SCART_INPUTS
#include "nexus_scart_input.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#include "nexus_picture_ctrl.h"
#include "nexus_component_input.h"
#include "nexus_analog_video_decoder.h"
#endif

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
#if NEXUS_DTV_PLATFORM
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    NEXUS_CompositeOutputSettings compositeSettings;
    NEXUS_PictureCtrlContrastStretch contrastStretchSettings;
    NEXUS_PanelOutputHandle panel;
    NEXUS_AttenuationRgbSettings attenuationRgbSettings;
    unsigned i;
    void *temp;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    panel = platformConfig.outputs.panel[0];

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);

    /* Bring up VDEC and connect to the VideoWindow */
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);

    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);

    NEXUS_CompositeOutput_GetSettings(platformConfig.outputs.composite[0], &compositeSettings);
    compositeSettings.inputBypass = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);
    rc = NEXUS_CompositeOutput_SetSettings(platformConfig.outputs.composite[0], &compositeSettings);
    BDBG_ASSERT(!rc);

    /* call some panel functions before connecting */
    NEXUS_PanelOutput_GetAttenuationRgb(panel, &attenuationRgbSettings);
    rc = NEXUS_PanelOutput_SetAttenuationRgb(panel, &attenuationRgbSettings);
    BDBG_ASSERT(!rc);

    /* adding composite before panel is not common. nexus must support it */
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);

    BKNI_Sleep(1000);

    /* nexus should prevent an output from being opened twice */
    printf("next command should fail\n");
    temp = NEXUS_PanelOutput_Open(0, NULL);
    BDBG_ASSERT(!temp);

    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(panel));
    BDBG_ASSERT(!rc);

    /* call some panel functions after connecting */
    NEXUS_PanelOutput_GetAttenuationRgb(panel, &attenuationRgbSettings);
    rc = NEXUS_PanelOutput_SetAttenuationRgb(panel, &attenuationRgbSettings);
    BDBG_ASSERT(!rc);

    /* nexus should prevent an output from being added twice */
    printf("next command should fail\n");
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(panel));
    BDBG_ASSERT(rc);
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);

    /* dynamicBacklightControl performs an internal link to panel */
    NEXUS_PictureCtrl_GetContrastStretch(window, &contrastStretchSettings);
    contrastStretchSettings.enabled = true;
    contrastStretchSettings.dynamicBacklightControl = true;
    rc = NEXUS_PictureCtrl_SetContrastStretch(window, &contrastStretchSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Display_RemoveOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    NEXUS_CompositeOutput_GetSettings(platformConfig.outputs.composite[0], &compositeSettings);
    compositeSettings.inputBypass = NULL;
    rc = NEXUS_CompositeOutput_SetSettings(platformConfig.outputs.composite[0], &compositeSettings);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(1000);

    /* we should be able to remove the panel before disabling dynamicBacklightControl */
    NEXUS_Display_RemoveOutput(display, NEXUS_PanelOutput_GetConnector(panel));

    BKNI_Sleep(1000);

    for (i=0;i<100;i++) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(panel));
        BDBG_ASSERT(!rc);
        rc = NEXUS_Display_RemoveOutput(display, NEXUS_PanelOutput_GetConnector(panel));
        BDBG_ASSERT(!rc);
        if (rand() % 2) {
            NEXUS_VideoOutput_Shutdown(NEXUS_PanelOutput_GetConnector(panel));
        }
    }

    printf("next command should fail\n");
    rc = NEXUS_Display_RemoveOutput(display, NEXUS_PanelOutput_GetConnector(panel));
    BDBG_ASSERT(rc);

    /* now stop dynamicBacklightControl */
    NEXUS_PictureCtrl_GetContrastStretch(window, &contrastStretchSettings);
    contrastStretchSettings.dynamicBacklightControl = false;
    rc = NEXUS_PictureCtrl_SetContrastStretch(window, &contrastStretchSettings);
    BDBG_ASSERT(!rc);

    NEXUS_VideoWindow_Close(window);
    NEXUS_AnalogVideoDecoder_Close(analogVideoDecoder);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openOutputs = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_Uninit();

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openOutputs = true;
    rc = NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_Uninit();

    /* make sure Display_Close cleans up its outputs */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    rc = NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    panel = platformConfig.outputs.panel[0];
    display = NEXUS_Display_Open(0, NULL);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(panel));
    BDBG_ASSERT(!rc);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();
#endif

    printf("success\n");
    return 0;
}
