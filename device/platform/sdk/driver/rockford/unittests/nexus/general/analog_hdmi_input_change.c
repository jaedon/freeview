/***************************************************************************
*     (c)2004-2008 Broadcom Corporation
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
* $brcm_Workfile: analog_hdmi_input_change.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 1/12/09 3:45p $
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/general/analog_hdmi_input_change.c $
* 
* Hydra_Software_Devel/2   1/12/09 3:45p erickson
* PR42679: update
*
* Hydra_Software_Devel/1   10/31/08 2:57p erickson
* PR48594: added test
*
* Hydra_Software_Devel/4   10/30/08 9:06p erickson
* PR47030: rework analog/digital memory sharing
*
* Hydra_Software_Devel/3   10/17/08 3:21p erickson
* PR47030: change NEXUS_Memory_GetStatus, added comment
*
* Hydra_Software_Devel/2   10/17/08 3:06p erickson
* PR47030: set explicit sharedAnalogDigitalMemory size
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_HDMI_INPUTS
#include "nexus_hdmi_input.h"
#endif
#include "nexus_memory.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_analog_video_decoder.h"
#endif

int main(void)
{
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_PlatformSettings platformSettings;
    NEXUS_AnalogVideoDecoderHandle adecode;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_VideoInput videoInput;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_HdmiInputHandle hdmiInput;
    unsigned i;
    BERR_Code rc;
    unsigned loops = 50;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);

    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);
    rc = NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    BDBG_ASSERT(!rc);
#else
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    adecode = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    hdmiInput = NEXUS_HdmiInput_Open(0, NULL);

    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
    rc = NEXUS_AnalogVideoDecoder_SetSettings(adecode, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);

    /* digital channel change could be only Stop/Start or could include RemoveInput/AddInput.
    these can occur in any combination. */
    for (i=0;i<loops;i++) {
        switch (rand() % 2) {
        case 0:
            printf("analog decode\n");
            videoInput = NEXUS_AnalogVideoDecoder_GetConnector(adecode);
            rc = NEXUS_VideoWindow_AddInput(window, videoInput);
            BDBG_ASSERT(!rc);

            BKNI_Sleep(rand() % 4000);
            NEXUS_VideoWindow_RemoveAllInputs(window);
            NEXUS_VideoInput_Shutdown(videoInput);
            break;

        case 1:
            printf("hdmi input\n");
            videoInput = NEXUS_HdmiInput_GetVideoConnector(hdmiInput);
            rc = NEXUS_VideoWindow_AddInput(window, videoInput);
            BDBG_ASSERT(!rc);

            BKNI_Sleep(rand() % 4000);
            NEXUS_VideoWindow_RemoveAllInputs(window);
            NEXUS_VideoInput_Shutdown(videoInput);
            break;
        }
    }

    printf("shutdown\n");
    NEXUS_AnalogVideoDecoder_Close(adecode);
    NEXUS_HdmiInput_Close(hdmiInput);
    NEXUS_Display_Close(display);

    NEXUS_Platform_Uninit();
#endif
    return 0;
}
