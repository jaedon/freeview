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
 * $brcm_Workfile: hd_graphics.c $
 * $brcm_Revision: 16 $
 * $brcm_Date: 6/8/12 11:54a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/graphics/hd_graphics.c $
 * 
 * 16   6/8/12 11:54a jtna
 * SW7231-843: add shutdown code
 * 
 * 15   2/29/12 4:47p erickson
 * SW7405-3671: always WaitForEvent after Checkpoint
 *
 * 14   1/25/12 5:22p rjlewis
 * SW7425-1136: Added HDMI Support.
 *
 * 13   11/16/11 3:46p erickson
 * SW7425-1747: remove old code
 *
 * 12   5/23/11 4:14p erickson
 * SW7405-3671: NEXUS_Graphics2D_Checkpoint is now required
 *
 * 11   11/29/10 2:57p erickson
 * SW7420-1078: add NEXUS_Platform_GetFramebufferHeap
 *
 * 10   9/24/10 2:29p jtna
 * SWDEPRECATED-3505: fixed incorrect framebuffer size printf
 *
 * 9   8/6/10 1:11p erickson
 * SW7420-703: remove unnecessary BCHP_CHIP and SCART logic
 *
 * 8   7/28/10 4:17p erickson
 * SW7405-4675: move NEXUS_HAS_GFD_VERTICAL_UPSCALE
 *
 * 7   3/22/10 2:36a randyjew
 * SW7468-143: Support for boards without component and/or composite
 *
 * 6   2/24/10 9:43p mphillip
 * SW7550-277: Guard graphics examples for platforms lacking graphics2d
 *
 * 5   11/10/09 3:19p nickh
 * SW7420-166: Explicitly pass in heap1 for allocated surfaces on 7420 as
 *  nexus defaults to heap0.
 *
 * 4   4/16/09 4:43p erickson
 * PR54270: use check pattern which can demonstrate vertical upscale
 *
 * 3   2/5/09 11:41a erickson
 * PR45796: add GFD vertical scaler support
 *
 * 2   1/30/09 10:28a erickson
 * PR49926: update
 *
 * 1   12/3/08 2:46p erickson
 * PR49926: added hd_graphics example
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d.h"
#endif
#include "nexus_display.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>

/*
This example shows a typical method for doing graphics on an HD system.

The app renders into a fixed size SD buffer called the offscreen surface.
A framebuffer is allocated based on the maximum display format height.
This height requirement exists because the GFD (graphics feeder) hardware only has horizontal upscale capability.

The application blits from the offscreen surface into the framebuffer to do the vertical upscale.
Then, the framebuffer is configured to perform the horizontal upscale.

This example steps through a list of VEC display formats (if applicable) to show how this method is dynamic.
*/

/* For set-tops, the VEC can support multiple formats. The graphics framebuffer must adapt. */
#define TOTAL_FORMATS 4
NEXUS_VideoFormat g_format[TOTAL_FORMATS] =
{
    NEXUS_VideoFormat_eNtsc,
    NEXUS_VideoFormat_ePal,
    NEXUS_VideoFormat_e720p,
    NEXUS_VideoFormat_e1080i
};

static void complete(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(void)
{
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_SurfaceHandle offscreenSurface, framebuffer;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle checkpointEvent;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_Graphics2DBlitSettings blitSettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    NEXUS_Error rc;
    unsigned i, j;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
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

    /* Set the offscreen surface to be a fixed SD size. Many applications are
    hardcoded to render to a 720x480 framebuffer. */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = 480;
    offscreenSurface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(offscreenSurface, &mem);

    /* CPU fill of simple vertical stripe pattern */
    for (i=0;i<createSettings.height;i++) {
        for (j=0;j<createSettings.width;j++) {
            /* create checker board */
            ((uint32_t*)((uint8_t*)mem.buffer + i*mem.pitch))[j] = (((i/50)%2) ^ ((j/50)%2)) ? 0x00000000 : 0xFFFFFFFF;
        }
    }
    NEXUS_Surface_Flush(offscreenSurface);

    /* Create the framebuffer which can be variable sized using GFD clipping */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720; /* The width is fixed to 720 and will be upscaled */
#if NEXUS_HAS_GFD_VERTICAL_UPSCALE
    createSettings.height = 480; /* The height is fixed to 480 and will be upscaled */
#else
    createSettings.height = 1080; /* The maximum height is 1080. For formats with height<1080, we clip. */
#endif
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    framebuffer = NEXUS_Surface_Create(&createSettings);

    gfx = NEXUS_Graphics2D_Open(0, NULL);
    BKNI_CreateEvent(&checkpointEvent);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    for (i=0;i<TOTAL_FORMATS;i++) {
        NEXUS_Display_GetSettings(display, &displaySettings);
        displaySettings.format = g_format[i];
        rc = NEXUS_Display_SetSettings(display, &displaySettings);
        BDBG_ASSERT(!rc);

        NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);

        NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);

        /* when the display format changes, Nexus automatically disables graphics
           to allow the app to recomposite and scale before showing graphics again. */
        graphicsSettings.enabled = true;

        /* the position rect sets the destination and is always equal to the full display */
        graphicsSettings.position.width = videoFormatInfo.width;
        graphicsSettings.position.height = videoFormatInfo.height;

        /* the clipping is the portion of the framebuffer we will use. */
        graphicsSettings.clip.width = 720; /* the difference between clip.width and position.width will trigger a GFD horizontal upscale. */
#if NEXUS_HAS_GFD_VERTICAL_UPSCALE
        graphicsSettings.clip.height = 480; /* the difference between clip.height and position.height will trigger a GFD vertical upscale. */
#else
        graphicsSettings.clip.height = videoFormatInfo.height; /* there is no GFD vertical upscale, so this must equal position.height. */
#endif
        rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
        BDBG_ASSERT(!rc);

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = offscreenSurface; /* always blit from the whole offscreen surface */
        blitSettings.output.surface = framebuffer;
        blitSettings.output.rect = graphicsSettings.clip; /* blit into the clipped region of the framebuffer that will be displayed. */
        blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            rc = BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
        }
        BDBG_ASSERT(!rc);

        /* Now we can grow graphics */
        rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
        BDBG_ASSERT(!rc);

        printf("offscreen %dx%d -> framebuffer %dx%d -> display %dx%d\n",
            720, 480,
            graphicsSettings.clip.width, graphicsSettings.clip.height,
            videoFormatInfo.width, videoFormatInfo.height);

        BKNI_Sleep(10*1000);
    }

    /* Shutdown */
    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(checkpointEvent);
    NEXUS_Surface_Destroy(offscreenSurface);
    NEXUS_Surface_Destroy(framebuffer);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

#else
    printf("This application is not supported on this platform!\n");
#endif

    return 0;
}

