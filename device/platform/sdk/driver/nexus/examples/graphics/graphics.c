/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: graphics.c $
 * $brcm_Revision: 18 $
 * $brcm_Date: 1/25/12 5:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/graphics/graphics.c $
 * 
 * 18   1/25/12 5:21p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 17   1/13/12 7:58p erickson
 * SW7425-1136: add HDMI output, remove unused code
 * 
 * 16   11/29/10 2:57p erickson
 * SW7420-1078: add NEXUS_Platform_GetFramebufferHeap
 * 
 * 15   8/6/10 1:09p erickson
 * SW7420-703: clarify need to set createSettings.heap. the default will
 *  work for 7420.
 *
 * 14   3/22/10 2:36a randyjew
 * SW7468-143: Support for boards without component and/or composite
 *
 * 13   11/10/09 3:17p nickh
 * SW7420-166: Explicitly pass in heap1 for allocated surfaces on 7420 as
 *  nexus defaults to heap0.
 *
 * 12   6/26/09 5:28p erickson
 * PR56102: flush dots to console
 *
 * 11   2/4/09 1:59p erickson
 * PR51841: simplify
 *
 *****************************************************************************/
/* Nexus example app: show graphics framebuffer with CPU drawing */

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_SurfaceMemory mem;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    NEXUS_Error rc;
    int i;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
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

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = videoFormatInfo.width;
    createSettings.height = videoFormatInfo.height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);

    /* fill with black */
    BKNI_Memset(mem.buffer, 0, createSettings.height * mem.pitch);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    /* graphicsSettings.position will default to the display size */
    graphicsSettings.clip.width = createSettings.width;
    graphicsSettings.clip.height = createSettings.height;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Display_SetGraphicsFramebuffer(display, surface);
    BDBG_ASSERT(!rc);

    /* draw random dots */
    for(i=0;;i++) {
        unsigned x = rand() % createSettings.width;
        unsigned y = rand() % createSettings.height;
        NEXUS_Pixel *buf = (NEXUS_Pixel *)((uint8_t *)mem.buffer+y*mem.pitch);
        buf[x] = rand();
        if (i%5000==0) {
            printf(".");
            fflush(stdout);
        }

        /* flush cached memory */
        NEXUS_Surface_Flush(surface);
    }
    return 0;
}

