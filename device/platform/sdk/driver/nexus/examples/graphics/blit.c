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
 * $brcm_Workfile: blit.c $
 * $brcm_Revision: 23 $
 * $brcm_Date: 3/27/12 9:37a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/graphics/blit.c $
 * 
 * 23   3/27/12 9:37a erickson
 * SW7435-74: use unsigned to avoid out of bounds blits on wrap around
 *
 * 22   2/29/12 12:02p erickson
 * SW7405-3671: always WaitForEvent after Checkpoint
 *
 * 21   1/25/12 5:21p rjlewis
 * SW7425-1136: Added HDMI Support.
 *
 * 20   11/16/11 3:46p erickson
 * SW7425-1747: remove old code
 *
 * 19   9/16/11 1:54p erickson
 * SW7405-3671: add required checkpoint before calling
 *  NEXUS_Display_SetGraphicsFramebuffer
 *
 * 18   6/24/11 10:57a erickson
 * SW7231-199: add hdmi output
 *
 * 17   5/25/11 4:11p erickson
 * SW7420-1200: use packetSpaceAvailable callback for better performance
 *
 * 16   5/24/11 3:09p erickson
 * SW7420-1894: set packetFifoThreshold = 0 for M2MC-bound example
 *
 * 15   5/23/11 4:15p erickson
 * SW7420-1200: don't get defaults more often than necessary
 *
 * 14   11/29/10 2:57p erickson
 * SW7420-1078: add NEXUS_Platform_GetFramebufferHeap
 *
 * 13   8/16/10 11:09a erickson
 * SW7405-3671: update
 *
 * 12   8/6/10 1:11p erickson
 * SW7420-703: remove unnecessary BCHP_CHIP and SCART logic
 *
 * 11   7/27/10 11:28a erickson
 * SW7405-3671: add some color
 *
 * 10   7/27/10 10:28a erickson
 * SW7405-3671: normalize use of checkpoint, add metrics
 *
 * 9   2/24/10 9:43p mphillip
 * SW7550-277: Guard graphics examples for platforms lacking graphics2d
 *
 * 8   5/11/09 11:59a erickson
 * PR54954: use NEXUS_Graphics2D_Checkpoint in non-deprecated way
 *
 * 7   11/21/08 8:01a erickson
 * PR42679: remove unneeded NEXUS_Surface_Flush
 *
 * 6   10/31/08 12:18p erickson
 * PR42679: add panel support
 *
 * 5   10/23/08 3:24p jtna
 * PR48190: use openSettings to specify max number of async operations
 *
 * 4   8/20/08 1:47p jgarrett
 * PR45866: added NEXUS_Graphics2DOpenSettings to NEXUS_Graphics2D_Open
 *
 * 3   6/19/08 2:33p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 *****************************************************************************/
/* Nexus example app: show graphics framebuffer with CPU drawing */

#include "nexus_platform.h"
#include "nexus_surface.h"
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d.h"
#endif
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

BDBG_MODULE(blit);

#if NEXUS_HAS_GRAPHICS2D
void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}
#endif

int main(void)
{
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle checkpointEvent, spaceAvailableEvent;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoFormatInfo info;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_Error rc;
    unsigned i;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
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

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = info.width;
    createSettings.height = info.height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    surface = NEXUS_Surface_Create(&createSettings);

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&spaceAvailableEvent);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = spaceAvailableEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    /* draw gradient on left side of black framebuffer using synchronous blits */
#define SIDEBAR_WIDTH 100
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    for (i=0;i<(int)createSettings.height;i+=2) {
        fillSettings.rect.y = i;
        fillSettings.rect.width = SIDEBAR_WIDTH;
        fillSettings.rect.height = 2;
        fillSettings.color =
            (0xFF << 24) |
            (((i/2) % 0xFF) << 16) |
            (((i) % 0xFF) << 8) |
            (((i*2) % 0xFF));
        while (1) {
            rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
            if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
                BKNI_WaitForEvent(spaceAvailableEvent, BKNI_INFINITE);
                /* retry */
            }
            else {
                break;
            }
        }
    }

    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL); /* require to execute queue */
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        rc = BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
    }
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, surface);

    /* blit from left-hand gradient into the rest of the framebuffer */
    BDBG_WRN(("starting"));
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings); /* don't get defaults more often than necessary */
    for(i=0;;i++) {
        NEXUS_Error rc;

        blitSettings.source.surface = surface;
        blitSettings.source.rect.x = 0;
        blitSettings.source.rect.y = (i/300) % (createSettings.height - 140);
        blitSettings.source.rect.width = SIDEBAR_WIDTH;
        blitSettings.source.rect.height = 140;

        blitSettings.output.surface = surface;
        blitSettings.output.rect.x = (rand() % (createSettings.width-120)) + SIDEBAR_WIDTH;
        blitSettings.output.rect.y = (rand() % (createSettings.height-20));
        blitSettings.output.rect.width = 20;
        blitSettings.output.rect.height = 20;

        blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;

        while (1) {
            rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
            if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
                /* blit can fail because an internal queue is full. wait for space to open up,
                then resubmit the blit. */
                BKNI_WaitForEvent(spaceAvailableEvent, BKNI_INFINITE);
                /* retry */
            }
            else {
                break;
            }
        }

        if (i && i%50000==0) {
            /* must do checkpoint before measurement */
            rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
            if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                rc = BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
            }
            BDBG_ASSERT(!rc);
            BDBG_WRN(("%u blits completed", i));
        }
        /* no flush is needed because we're not using the CPU */
    }
#else
    printf("This application is not supported on this platform!\n");
#endif

    return 0;
}

