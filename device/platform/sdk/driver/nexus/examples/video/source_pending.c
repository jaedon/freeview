/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: source_pending.c $
* $brcm_Revision: 7 $
* $brcm_Date: 1/26/12 11:23a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/examples/video/source_pending.c $
* 
* 7   1/26/12 11:23a rjlewis
* SW7425-1136: Added HDMI Support.
* 
* 6   3/22/10 2:41a randyjew
* SW7468-143: Support for boards without component and/or composite
* 
* 5   2/11/10 10:37a erickson
* SW3548-2705: fix settop
*
* 4   1/22/10 10:29a erickson
* SW3548-2705: refactor to use
*  NEXUS_AnalogVideoDecoder_GetDefaultSettings with explicit
*  useDynamicDefaults = true
*
* 3   10/17/08 12:25p erickson
* PR47030: fix
*
* 2   5/9/08 12:44p erickson
* PR39453: add sourcePending callback, set status.sourcePending based on
*  VDC dirty bits.
*
* 1   4/30/08 4:11p erickson
* PR39453: added source_pending example
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_component_input.h"
#include "nexus_analog_video_decoder.h"
#endif
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_video_window.h"
#include "nexus_video_adj.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

struct AppContext {
    NEXUS_VideoInput videoInput;
    NEXUS_VideoWindowHandle window;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
#endif
};

void source_pending(void *context, int param)
{
    struct AppContext *pAppContext = context;
    NEXUS_VideoInputStatus status;
    NEXUS_VideoWindowAnrSettings anrSettings;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
#endif
    NEXUS_Error rc;

    BSTD_UNUSED(param);

    /* At this point, the source is connected and so VDC can make a recommendation regarding the default settings.
    If the application wishes, it can read that recommendation and make some incremental change. */
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoder_GetDynamicDefaultSettings(pAppContext->analogVideoDecoder, &analogVideoDecoderSettings);
    analogVideoDecoderSettings.useDynamicDefaults = false;
    /* Now we can make app-specific incremental changes:
    analogVideoDecoderSettings.xxx = yyy; */
    rc = NEXUS_AnalogVideoDecoder_SetSettings(pAppContext->analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);
#endif

    /* the application can also invoke PQ changes during the source pending callback */
    printf("source pending triggered...\n");
    NEXUS_VideoInput_GetStatus(pAppContext->videoInput, &status);
    if (status.height > 576) {
        NEXUS_VideoWindow_GetAnrSettings(pAppContext->window, &anrSettings);
        /* TODO: anrSettings.pixelFormat = XXX; */
        rc = NEXUS_VideoWindow_SetAnrSettings(pAppContext->window, &anrSettings);
        BDBG_ASSERT(!rc);
    }
    else {
        NEXUS_VideoWindow_GetAnrSettings(pAppContext->window, &anrSettings);
        /* TODO: anrSettings.pixelFormat = XXX; */
        rc = NEXUS_VideoWindow_SetAnrSettings(pAppContext->window, &anrSettings);
        BDBG_ASSERT(!rc);
    }

    rc = NEXUS_VideoInput_SetResumeMode(pAppContext->videoInput, NEXUS_VideoInputResumeMode_eNow);
    BDBG_ASSERT(!rc);
    printf("source resumed.\n");
}

int main(void)
{
    struct AppContext appContext;
    struct AppContext *pAppContext = &appContext;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_VideoInputSettings videoInputSettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
		}
    }
#endif

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    /* Bring up VDEC and connect to the VideoWindow */
    pAppContext->analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    pAppContext->window = NEXUS_VideoWindow_Open(display, 0);
    pAppContext->videoInput = NEXUS_AnalogVideoDecoder_GetConnector(pAppContext->analogVideoDecoder);
    rc = NEXUS_VideoWindow_AddInput(pAppContext->window, pAppContext->videoInput);
    BDBG_ASSERT(!rc);
#endif

    /* enable app-driven source pending */
    rc = NEXUS_VideoInput_SetResumeMode(pAppContext->videoInput, NEXUS_VideoInputResumeMode_eManual);
    BDBG_ASSERT(!rc);

    NEXUS_VideoInput_GetSettings(pAppContext->videoInput, &videoInputSettings);
    videoInputSettings.sourcePending.callback = source_pending;
    videoInputSettings.sourcePending.context = pAppContext;
    rc = NEXUS_VideoInput_SetSettings(pAppContext->videoInput, &videoInputSettings);
    BDBG_ASSERT(!rc);

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    {
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_AnalogVideoDecoder_GetDefaultSettings(&analogVideoDecoderSettings);
    /* the simplest method is to assign the input, but allow VDC to internally set optimal AnalogVideoDecoder settings. */
    analogVideoDecoderSettings.input = NEXUS_ComponentInput_GetConnector(platformConfig.inputs.component[0]);
    analogVideoDecoderSettings.useDynamicDefaults = true;
    rc = NEXUS_AnalogVideoDecoder_SetSettings(pAppContext->analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);
    }

    /* Please note that the all other settings in analogVideoDecoderSettings are ignored and provide no information to the application.
    Because of this, incremental changes cannot be made. See source_pending() at the top of this example to learn how to make incremental changes. */
#endif

    printf("Modify your component input signal to trigger source pending events\n");
    getchar();

    return 0;
}
