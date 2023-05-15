/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: blit_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/7/11 11:21a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/blit_stress.c $
 * 
 * Hydra_Software_Devel/3   10/7/11 11:21a jessem
 * SW7425-1435: Specified which heap to use.
 *
 * Hydra_Software_Devel/2   1/21/11 3:35p randyjew
 * SW7405-3671: NEXUS_FillOp_eBlend not supported
 *
 * Hydra_Software_Devel/1   8/16/10 1:03p erickson
 * SW7405-3671: packet blit tests
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d.h"
#endif
#include "nexus_display.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#endif
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <time.h>
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
    NEXUS_SurfaceHandle framebuffer, offscreen;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle spaceAvailableEvent, checkpointEvent;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoFormatInfo info;
    int i;
    NEXUS_Error rc;

    srand(time(NULL));

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if (platformConfig.outputs.component[0]) {
        NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    }
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = info.width;
    createSettings.height = info.height;
#if BCHP_CHIP == 7420
    createSettings.heap = platformConfig.heap[1];
#else
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
#endif
    framebuffer = NEXUS_Surface_Create(&createSettings);
    offscreen = NEXUS_Surface_Create(&createSettings);

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&spaceAvailableEvent);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = spaceAvailableEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = offscreen;
    fillSettings.color = 0xFF00FF00;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);

    for (i=0;i<50000;i++) {
        NEXUS_Rect outRect, srcRect;
        NEXUS_Graphics2DFillSettings fillSettings;
        NEXUS_Graphics2DBlitSettings blitSettings;
        NEXUS_Error rc;
        unsigned op = rand() % 4;

        outRect.x = rand() % info.width;
        outRect.y = rand() % info.height;
        outRect.width = rand() % (info.width - outRect.x) + 1;
        outRect.height = rand() % (info.height - outRect.y) + 1;

        srcRect.x = rand() % info.width;
        srcRect.y = rand() % info.height;
        srcRect.width = rand() % (info.width - srcRect.x) + 1;
        srcRect.height = rand() % (info.height - srcRect.y) + 1;

        /* can't do 7x downscale */
        if ((outRect.width <= srcRect.width / 7) || (outRect.height <= srcRect.height / 7)) {
            outRect = srcRect;
        }

        switch (op) {
        case 0:
            NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
            blitSettings.source.surface = offscreen;
            blitSettings.source.rect = srcRect;
            if (rand() % 10 == 0) {
                blitSettings.dest.surface = framebuffer;
            }
            blitSettings.output.surface = framebuffer;
            blitSettings.output.rect = outRect;
            if (blitSettings.dest.surface) {
                blitSettings.dest.rect = blitSettings.output.rect;
            }
            /* TODO: add more params */
            BDBG_MSG(("NEXUS_Graphics2D_Blit: %d,%d,%d,%d -> %d,%d,%d,%d",
                blitSettings.source.rect.x, blitSettings.source.rect.y, blitSettings.source.rect.width, blitSettings.source.rect.height,
                blitSettings.output.rect.x, blitSettings.output.rect.y, blitSettings.output.rect.width, blitSettings.output.rect.height));
            rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
            if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
                rc = BKNI_WaitForEvent(spaceAvailableEvent, 5000);
                BDBG_ASSERT(!rc);
            }
            else {
                BDBG_ASSERT(!rc);
            }
            break;

        case 1:
            BDBG_MSG(("NEXUS_Graphics2D_FastBlit: %d,%d,%d,%d -> %d,%d,%d,%d",
                srcRect.x, srcRect.y, srcRect.width, srcRect.height,
                outRect.x, outRect.y, outRect.width, outRect.height ));
            rc = NEXUS_Graphics2D_FastBlit(gfx,
                framebuffer,
                &outRect,
                offscreen,
                &srcRect,
                NEXUS_BlitColorOp_eCopySource,
                rand() % 2 ? NEXUS_BlitAlphaOp_eCopySource : NEXUS_BlitAlphaOp_eCopyConstant,
                rand());
            if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
                rc = BKNI_WaitForEvent(spaceAvailableEvent, 5000);
                BDBG_ASSERT(!rc);
            }
            else {
                BDBG_ASSERT(!rc);
            }
            break;

        case 2:
            NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
            fillSettings.surface = framebuffer;
            if (rand() % 10 == 0) {
                fillSettings.rect = outRect;
            }
            fillSettings.color = rand();
            fillSettings.colorOp = rand() % NEXUS_FillOp_eMax;
            fillSettings.alphaOp = rand() % NEXUS_FillOp_eMax;
	     /* NEXUS_FillOp_eBlend not supported*/
	     if(fillSettings.alphaOp == NEXUS_FillOp_eBlend)
	         break;
            BDBG_MSG(("NEXUS_Graphics2D_Fill: %d,%d,%d,%d",
                fillSettings.rect.x, fillSettings.rect.y, fillSettings.rect.width, fillSettings.rect.height));
            rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
            if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
                rc = BKNI_WaitForEvent(spaceAvailableEvent, 5000);
                BDBG_ASSERT(!rc);
            }
            else {
                BDBG_ASSERT(!rc);
            }
            break;

        case 3:
            if (rand() % 10 == 0) {
                BDBG_MSG(("NEXUS_Graphics2D_Checkpoint"));
                rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
                if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                    rc = BKNI_WaitForEvent(checkpointEvent, 5000);
                    BDBG_ASSERT(!rc);
                }
                else {
                    BDBG_ASSERT(!rc);
                }
            }
            break;

        /* TODO: add porter duff blit & fill */
        }

        if (i && i%1000==0) {
            BDBG_WRN(("%d fills/blits completed", i));
        }
    }
#endif

    return 0;
}

