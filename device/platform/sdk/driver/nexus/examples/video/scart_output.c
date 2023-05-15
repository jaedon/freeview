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
* $brcm_Workfile: scart_output.c $
* $brcm_Revision: 1 $
* $brcm_Date: 7/15/09 9:05a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/examples/video/scart_output.c $
* 
* 1   7/15/09 9:05a erickson
* PR56806: added scart_output
*
* 11   6/23/09 2:56p erickson
* PR56220: add basic PQ to analog_video example
*
* 10   4/7/09 4:00p erickson
* PR35457: add required NEXUS_TimebaseSourceType_eAnalog setting
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
#include "nexus_timebase.h"
#include "nexus_video_input.h"
#include "nexus_component_input.h"
#include "nexus_analog_video_decoder.h"
#include "nexus_component_output.h"
#include "nexus_video_window.h"
#include "nexus_video_adj.h"
#if NEXUS_NUM_SCART_INPUTS
#include "nexus_scart_input.h"
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
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_VideoWindowMadSettings madSettings;
    NEXUS_VideoWindowAnrSettings anrSettings;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* By default NEXUS_DisplaySettings.timebase and NEXUS_AnalogVideoDecoderSettings.timebase will be NEXUS_Timebase_e0,
    but we must set the timebase to analog */
    NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eAnalog;
    NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto; /* VEC, not panel output */
    displaySettings.format = NEXUS_VideoFormat_ePal;
    display = NEXUS_Display_Open(0, &displaySettings);
    BDBG_ASSERT(platformConfig.inputs.scart[0]);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
	BDBG_ASSERT(!rc);

    /* Bring up VDEC and connect to the VideoWindow */
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoWindow_AddInput(window, NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder));

    /* Basic picture quality requires MAD (deinterlacing) */
    NEXUS_VideoWindow_GetMadSettings(window, &madSettings);
    madSettings.deinterlace = true;
    rc = NEXUS_VideoWindow_SetMadSettings(window, &madSettings);
    BDBG_ASSERT(!rc);

    /* Basic picture quality requires ANR. SECAM requries the DrDb filter in the ANR block. */
    NEXUS_VideoWindow_GetAnrSettings(window, &anrSettings);
    anrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eEnable;
    rc = NEXUS_VideoWindow_SetAnrSettings(window, &anrSettings);
    BDBG_ASSERT(!rc);

    BDBG_ASSERT(platformConfig.inputs.composite[0]);
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);
    printf("Composite 0 to Scart composite output\n");
    getchar();
#endif

    return 0;
}
