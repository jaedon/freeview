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
* $brcm_Workfile: scart_input.c $
* $brcm_Revision: 3 $
* $brcm_Date: 4/22/09 2:53p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/examples/video/scart_input.c $
* 
* 3   4/22/09 2:53p erickson
* PR40921: fix compilation, rename example to scart_input
*
* 2   6/19/08 10:46a erickson
* PR42678: switch to output_type=panel|component
*
* 1   5/21/08 5:52p erickson
* PR40921: add ScartInput to platform and examples
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_SCART_INPUTS
#include "nexus_scart_input.h"
#endif
#if NEXUS_NUM_PANEL_OUTPUTS
#include "nexus_panel_output.h"
#endif
#include "nexus_component_output.h"
#include "nexus_analog_video_decoder.h"
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    NEXUS_DisplayHandle display, bypassDisplay = NULL;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    BDBG_ASSERT(display);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eBypass;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    bypassDisplay = NEXUS_Display_Open(1, &displaySettings);

    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);

    /* Bring up VDEC and connect to the VideoWindow */
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));
    BDBG_ASSERT(!rc);

#if NEXUS_NUM_SCART_INPUTS
    BDBG_ASSERT(platformConfig.inputs.scart[0]);
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_ScartInput_GetVideoConnector(platformConfig.inputs.scart[0]), &analogVideoDecoderSettings);
    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);

    if (bypassDisplay) {
        rc = NEXUS_Display_AddOutput(bypassDisplay, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
        BDBG_ASSERT(!rc);
    }
#endif

    return 0;
}
