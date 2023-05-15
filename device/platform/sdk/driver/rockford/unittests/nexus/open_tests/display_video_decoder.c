/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: display_video_decoder.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/open_tests/display_video_decoder.c $
 * 
 * Hydra_Software_Devel/4   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* Nexus test app:
test different ordering for opening and closing Display and VideoDecoder
*/

#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"

int main(void)
{
    NEXUS_DisplayHandle display;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoWindowHandle window;
    NEXUS_Error rc;
    unsigned loops = 25;

    while (--loops) {
        NEXUS_PlatformSettings platformSettings;
        NEXUS_PlatformConfiguration platformConfiguration;

        /* Bring up all modules for a platform in a default configuraiton for this platform */
        rc = NEXUS_Platform_Init(NULL);
        BDBG_ASSERT(!rc);

        NEXUS_Platform_GetSettings(&platformSettings);
        NEXUS_Platform_GetConfiguration(&platformConfiguration);

        display = NEXUS_Display_Open(0, NULL);
        BDBG_ASSERT(display);
        window = NEXUS_VideoWindow_Open(display, 0);
        BDBG_ASSERT(window);
        videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
        BDBG_ASSERT(videoDecoder);
        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
        BDBG_ASSERT(!rc);
        /* NEXUS_VideoWindow_Close will automatically remove VideoInput */
        BKNI_Sleep(100);
        NEXUS_VideoWindow_Close(window);
        BKNI_Sleep(100);
        NEXUS_Display_Close(display);
        BKNI_Sleep(100);
        /* closing the last display will trigger an implicit NEXUS_VideoInput_Shutdown of all cached inputs */
        NEXUS_VideoDecoder_Close(videoDecoder);

        display = NEXUS_Display_Open(0, NULL);
        BDBG_ASSERT(display);
        window = NEXUS_VideoWindow_Open(display, 0);
        BDBG_ASSERT(window);
        videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
        BDBG_ASSERT(videoDecoder);
        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
        BDBG_ASSERT(!rc);
        /* NEXUS_Display_Close will automatically close VideoWindow */
        BKNI_Sleep(100);
        NEXUS_Display_Close(display);
        BKNI_Sleep(100);
        /* closing the last display will trigger an implicit NEXUS_VideoInput_Shutdown of all cached inputs */
        NEXUS_VideoDecoder_Close(videoDecoder);

        display = NEXUS_Display_Open(0, NULL);
        BDBG_ASSERT(display);
        window = NEXUS_VideoWindow_Open(display, 0);
        BDBG_ASSERT(window);
        videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
        BDBG_ASSERT(videoDecoder);
        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
        BDBG_ASSERT(!rc);
        /* The Shutdown is required because VideoDecoder and VideoInput are in separate modules.
        However, NEXUS_VideoInput_Shutdown will automatically call NEXUS_VideoWindow_RemoveInput */
        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
        NEXUS_VideoDecoder_Close(videoDecoder);
        /* NEXUS_Display_Close will automatically close VideoWindow */
        BKNI_Sleep(100);
        NEXUS_Display_Close(display);

        NEXUS_Platform_Uninit();
    }

    return 0;
}
