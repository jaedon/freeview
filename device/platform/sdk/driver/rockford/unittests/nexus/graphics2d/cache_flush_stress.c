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
 * $brcm_Workfile: cache_flush_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/27/12 5:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/cache_flush_stress.c $
 * 
 * Hydra_Software_Devel/1   6/27/12 5:12p jtna
 * SW7231-777: added cache_flush_stress test
 *
 *****************************************************************************/

/* tests sequence of 1) HW write, 2) flush, 3) CPU read 
   and verifies (HW write == CPU read) */

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_memory.h"
#include "nexus_display.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>

BDBG_MODULE(cache_flush_stress);

NEXUS_Graphics2DHandle gfx;
BKNI_EventHandle event;

#define WIDTH 720
#define HEIGHT 576
#define TEST_COUNT 100000

void VerifyCopyArea(NEXUS_SurfaceMemory mem, unsigned dx, unsigned dy, unsigned w, unsigned h)
{
    unsigned x, y, xs, ys;
    uint32_t *dst_line, *src_line;

    for (ys=0, y=dy; y<dy+h; y++, ys++) {
        dst_line = (uint32_t*)(&((uint8_t*)mem.buffer)[mem.pitch * y]);
        src_line = (uint32_t*)(&((uint8_t*)mem.buffer)[mem.pitch * ys]);
        
        for (xs=0, x=dx; x<dx+w; x++, xs++) {
            BDBG_ASSERT(dst_line[x]==src_line[xs]);
        }
    }
}

void CopyArea(NEXUS_SurfaceHandle dst, int x, int y, int w, int h, int dx, int dy){
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_Error rc;

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = dst;
    blitSettings.source.rect.x = x;
    blitSettings.source.rect.y = y;
    blitSettings.source.rect.width = w;
    blitSettings.source.rect.height = h;

    blitSettings.output.surface = dst;
    blitSettings.output.rect.x = x + dx;
    blitSettings.output.rect.y = y + dy;
    blitSettings.output.rect.width = w;
    blitSettings.output.rect.height = h;

    blitSettings.dest.surface = blitSettings.output.surface;
    blitSettings.dest.rect = blitSettings.output.rect;

    blitSettings.horizontalFilter = NEXUS_Graphics2DFilterCoeffs_ePointSample;
    blitSettings.verticalFilter = NEXUS_Graphics2DFilterCoeffs_ePointSample;

    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
    blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;

    rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
    BDBG_ASSERT(!rc);
}

void Test(NEXUS_SurfaceHandle surface, NEXUS_SurfaceMemory mem)
{
    unsigned x, y, w, h;
    unsigned count = 0;
    NEXUS_Error rc;

    while(count<TEST_COUNT) {
        /* copy some portion of first quadrant to fourth quadrant */
        w = rand() % (WIDTH/2);
        if (w==0) { w = 1; }
        
        h = rand() % (HEIGHT/2);
        if (h==0) { h = 1; }

        x = rand() % (WIDTH/2);
        if (x+w > WIDTH/2) { x = WIDTH/2 - w; }
        x = x + WIDTH/2;

        y = rand() % (HEIGHT/2);
        if (y+h > HEIGHT/2) { y = (HEIGHT/2) - h; }
        y = y + HEIGHT/2;

        CopyArea(surface, 0, 0, w, h, x, y);

        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {        
            rc = BKNI_WaitForEvent(event, 0xffffffff);
            BDBG_ASSERT(!rc);
        }
        NEXUS_Surface_Flush(surface);

        VerifyCopyArea(mem, x, y, w, h);

        if (count && count%100==0) {
            BKNI_Printf("%u tries\n", count);
        }
        count++;
    }
}

void complete(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(void)
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceMemory mem;
    NEXUS_Graphics2DOpenSettings gfxOpenSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Error rc;
    
    srand((int)time(NULL));

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    display = NEXUS_Display_Open(0, &displaySettings);

    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = WIDTH;
    createSettings.height = HEIGHT;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);

    BKNI_CreateEvent(&event);

    NEXUS_Graphics2D_GetDefaultOpenSettings(&gfxOpenSettings);
    gfxOpenSettings.preAllocPacketMemory = true;
    gfxOpenSettings.packetFifoThreshold = 0;
    gfx = NEXUS_Graphics2D_Open(0, &gfxOpenSettings);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.clip.width = createSettings.width;
    graphicsSettings.clip.height = createSettings.height;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, surface);
    BDBG_ASSERT(!rc);

    /* divide the surface into four quadrants. the first quadrant is filled randomly. 
       second and third quadrants are filled red and blue. fourth quadrant is our test area */
    {
        uint32_t *line, x, y;
        for (y=0; y<HEIGHT; y++) {
            line = (uint32_t*)(&((uint8_t*)mem.buffer)[mem.pitch * y]);
            for (x=0; x<WIDTH; x++) {        
                if (y<HEIGHT/2 && x>=WIDTH/2) {
                    line[x] = 0xFFFF0000;
                }
                else if (y>=HEIGHT/2 && x<WIDTH/2) {
                    line[x] = 0xFF0000FF;
                }
                else if (y>=HEIGHT/2 && x>=WIDTH/2) {
                    line[x] = 0xFF000000;
                }
                else {
                    line[x] = rand();
                }
            }
        }
        NEXUS_Surface_Flush(surface);
    }
    
    Test(surface, mem);
    BKNI_Printf("success\n");

    return 0;
}

