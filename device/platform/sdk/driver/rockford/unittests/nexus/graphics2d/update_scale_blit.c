/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: update_scale_blit.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 1/9/12 5:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/update_scale_blit.c $
 * 
 * Hydra_Software_Devel/6   1/9/12 5:28p nissen
 * SW7425-1207: Limited vertical downscale to less than 3x to prevent
 * hang. Reduced test count from 10000 to 1000 to speed up test. Changed
 * test to continue when finding error.
 * 
 * Hydra_Software_Devel/5   12/28/11 5:55p vsilyaev
 * SW7425-1207: Scan through different scale factors
 * 
 * Hydra_Software_Devel/4   12/9/11 7:39p nissen
 * SW7425-1207 : Switched source surface width and height.
 * 
 * Hydra_Software_Devel/3   11/29/11 5:22p vsilyaev
 * SW7425-1207: Fixed stranded call to Checkpoint
 * 
 * Hydra_Software_Devel/2   11/28/11 5:28p vsilyaev
 * SW7425-1207: Added initial test
 * 
 * Hydra_Software_Devel/1   11/28/11 5:15p vsilyaev
 * SW7425-1207: Unit test for update (partial) scale blit
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
#include <stdio.h>
#include <math.h>

BDBG_MODULE(fill);

#if NEXUS_HAS_GRAPHICS2D
void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void checkpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle event)
{
    int rc;
    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        BKNI_WaitForEvent(event, BKNI_INFINITE);
    }
}

#endif

#define MAX_SCALE_FACTOR   2
#define MAX_REGION  128
#if NEXUS_HAS_GRAPHICS2D
static const BM2MC_PACKET_Blend copyColor = {BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, false,
    BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
static const BM2MC_PACKET_Blend copyAlpha = {BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, false,
    BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};


static bool fill(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle surface, const NEXUS_Rect *rect, NEXUS_Pixel color)
{
    void *buffer, *next;
    const size_t limit = 1024;
    NEXUS_Error rc;
    size_t size;

    rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, limit);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    if(size==0) {
        return false;
    }
    BDBG_ASSERT(size>=limit);
    next = buffer;
    {
        BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, OutputFeeder, false);
        NEXUS_Surface_InitPlane(surface, &pPacket->plane);
        next = ++pPacket;
    }
    {
        BM2MC_PACKET_PacketSourceColor *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, SourceColor, false );
        pPacket->color = color;
        next = ++pPacket;
    }
    {
        BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *)next;
        BM2MC_PACKET_INIT( pPacket, Blend, false );
        pPacket->color_blend = copyColor;
        pPacket->alpha_blend = copyAlpha;
        pPacket->color = 0;
        next = ++pPacket;
    }
    {
        BM2MC_PACKET_PacketFillBlit *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, FillBlit, true);
        pPacket->rect.x = rect->x;
        pPacket->rect.y = rect->y;
        pPacket->rect.width = rect->width;
        pPacket->rect.height = rect->height;
        next = ++pPacket;
    }
    rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
    BDBG_ASSERT(!rc);
    return true;
}

static bool scale(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle srcSurface, const NEXUS_Rect  *srcRect, NEXUS_SurfaceHandle dstSurface, const NEXUS_Rect *dstRect)
{
    void *buffer, *next;
    const size_t limit = 1024;
    NEXUS_Error rc;
    size_t size;

    rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, limit);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    if(size==0) {
        return false;
    }
    BDBG_ASSERT(size>=limit);
    next = buffer;
    {
        BM2MC_PACKET_PacketSourceFeeder *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, SourceFeeder, false );
        NEXUS_Surface_InitPlane(srcSurface, &pPacket->plane);
        pPacket->color = 0;
        next = ++pPacket;
   }

   {
        BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, OutputFeeder, false);
        NEXUS_Surface_InitPlane(dstSurface, &pPacket->plane);
        next = ++pPacket;
   }

   {
        BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *)next;
        BM2MC_PACKET_INIT( pPacket, Blend, false );
        pPacket->color_blend = copyColor;
        pPacket->alpha_blend = copyAlpha;
        pPacket->color = 0;
        next = ++pPacket;
   }
   {
        BM2MC_PACKET_PacketScaleBlit *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, ScaleBlit, true);
        pPacket->src_rect.x = srcRect->x;
        pPacket->src_rect.y = srcRect->y;
        pPacket->src_rect.width = srcRect->width;
        pPacket->src_rect.height = srcRect->height;
        pPacket->out_rect.x = dstRect->x;
        pPacket->out_rect.y = dstRect->y;
        pPacket->out_rect.width = dstRect->width;
        pPacket->out_rect.height = dstRect->height;
        next = ++pPacket;
    }
    rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
    BDBG_ASSERT(!rc);
    return true;
}

static bool update_scale(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle srcSurface, const NEXUS_Rect  *srcRect, NEXUS_SurfaceHandle dstSurface, const NEXUS_Rect *dstRect, const NEXUS_Rect *updateRect)
{
    void *buffer, *next;
    const size_t limit = 1024;
    NEXUS_Error rc;
    size_t size;

    rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, limit);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    if(size==0) {
        return false;
    }
    BDBG_ASSERT(size>=limit);
    next = buffer;
    {
        BM2MC_PACKET_PacketSourceFeeder *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, SourceFeeder, false );
        NEXUS_Surface_InitPlane(srcSurface, &pPacket->plane);
        pPacket->color = 0;
        next = ++pPacket;
   }

   {
        BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, OutputFeeder, false);
        NEXUS_Surface_InitPlane(dstSurface, &pPacket->plane);
        next = ++pPacket;
   }

   {
        BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *)next;
        BM2MC_PACKET_INIT( pPacket, Blend, false );
        pPacket->color_blend = copyColor;
        pPacket->alpha_blend = copyAlpha;
        pPacket->color = 0;
        next = ++pPacket;
   }
   {
        BM2MC_PACKET_PacketUpdateScaleBlit *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, UpdateScaleBlit, true);
        pPacket->src_rect.x = srcRect->x;
        pPacket->src_rect.y = srcRect->y;
        pPacket->src_rect.width = srcRect->width;
        pPacket->src_rect.height = srcRect->height;
        pPacket->out_rect.x = dstRect->x;
        pPacket->out_rect.y = dstRect->y;
        pPacket->out_rect.width = dstRect->width;
        pPacket->out_rect.height = dstRect->height;
        pPacket->update_rect.x = updateRect->x;
        pPacket->update_rect.y = updateRect->y;
        pPacket->update_rect.width = updateRect->width;
        pPacket->update_rect.height = updateRect->height;
        next = ++pPacket;
    }
    rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
    BDBG_ASSERT(!rc);
    return true;
}

static bool copy(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle srcSurface, const NEXUS_Rect *srcRect, NEXUS_SurfaceHandle dstSurface, unsigned dstX, unsigned dstY)
{
    void *buffer, *next;
    const size_t limit = 1024;
    NEXUS_Error rc;
    size_t size;

    rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, limit);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    if(size==0) {
        return false;
    }
    BDBG_ASSERT(size>=limit);
    next = buffer;
    {
        BM2MC_PACKET_PacketSourceFeeder *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, SourceFeeder, false );
        NEXUS_Surface_InitPlane(srcSurface, &pPacket->plane);
        pPacket->color = 0;
        next = ++pPacket;
   }

   {
        BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, OutputFeeder, false);
        NEXUS_Surface_InitPlane(dstSurface, &pPacket->plane);
        next = ++pPacket;
   }

   {
        BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *)next;
        BM2MC_PACKET_INIT( pPacket, Blend, false );
        pPacket->color_blend = copyColor;
        pPacket->alpha_blend = copyAlpha;
        pPacket->color = 0;
        next = ++pPacket;
   }
   {
        BM2MC_PACKET_PacketBlendBlit *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, BlendBlit, true);
        pPacket->src_rect.x = srcRect->x;
        pPacket->src_rect.y = srcRect->y;
        pPacket->src_rect.width = srcRect->width;
        pPacket->src_rect.height = srcRect->height;
        pPacket->out_point.x = dstX;
        pPacket->out_point.y = dstY;
        next = ++pPacket;
    }
    rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
    BDBG_ASSERT(!rc);
    return true;
}

static bool  surface_compare( const NEXUS_SurfaceMemory *surface1, const NEXUS_SurfaceMemory *surface2, const NEXUS_Rect *region)
{
    unsigned y;

    for(y=0;y<region->height;y++) {
        NEXUS_Pixel *pixel1 = (NEXUS_Pixel *)((uint8_t *)surface1->buffer+y*surface1->pitch) + region->x;
        NEXUS_Pixel *pixel2 = (NEXUS_Pixel *)((uint8_t *)surface2->buffer+y*surface1->pitch) + region->x;
        unsigned x;
        for(x=9;x<region->width;x++, pixel1++, pixel2++) {
            if(*pixel1 != *pixel2) {
                BDBG_ERR(("difference found at %u,%u (%#x,%#x)", region->x+x, region->y+y, *pixel1, *pixel2));
                return false;
            }
        }
    }
    return true;
}

#endif

#define RUN(op,args) do { if(op args) { break; } BKNI_WaitForEvent(spaceAvailableEvent, BKNI_INFINITE); }while(true)

int main(void)
{
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_SurfaceHandle framebuffer, golden, source; 
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle checkpointEvent, spaceAvailableEvent;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_SurfaceMemory framebufferMem, goldenMem, sourceMem;
    unsigned i;
    unsigned framebuffer_width, framebuffer_height;
    unsigned source_width, source_height;
    NEXUS_Rect rect, srcRect, dstRect;
    int xscale, yscale;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
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
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif
#endif

    {
        NEXUS_VideoFormatInfo info;
        NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);
        framebuffer_width = info.width;
        framebuffer_height = info.height;
    }

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = framebuffer_width;
    createSettings.height = framebuffer_height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);

    framebuffer = NEXUS_Surface_Create(&createSettings);
    BDBG_ASSERT(framebuffer);

    golden = NEXUS_Surface_Create(&createSettings);
    BDBG_ASSERT(golden);


    source_width = framebuffer_width*(1<<MAX_SCALE_FACTOR);
    source_height = framebuffer_height*(1<<MAX_SCALE_FACTOR);
    createSettings.width = source_width;
    createSettings.height = source_height;
    source = NEXUS_Surface_Create(&createSettings); /* copy a framebuffer */
    BDBG_ASSERT(source);

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&spaceAvailableEvent);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = spaceAvailableEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    /* fill with black */
    rect.x = rect.y = 0;
    rect.width = framebuffer_width;
    rect.height = framebuffer_height;
    RUN(fill, (gfx, framebuffer, &rect, 0));

    rect.x = rect.y = 0;
    rect.width = source_width;
    rect.height = source_height;
    RUN(fill, (gfx, source, &rect, 0));
    
    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);

    checkpoint(gfx, checkpointEvent);

    srcRect.x = srcRect.y = dstRect.x = dstRect.y = 0;
    srcRect.width = source_width;
    srcRect.height = source_height;
    dstRect.width = framebuffer_width;
    dstRect.height = framebuffer_height;
 
    /* fill random boxes */
    BDBG_LOG(("starting"));
    for(i=0;;) {
        NEXUS_Pixel color;

        rect.width = (rand()%(source_width/8))+1;
        rect.height = (rand()%(source_height/8))+1;
        rect.x = rand() % (source_width-rect.width);
        rect.y = rand() % (source_height-rect.height);
        color = (rand()%(0xFFFFFF+1) | (rand()%(0xFF+1))<<24);

        RUN(fill, (gfx, source , &rect, color));

        i++;
        if (i%10000==0) {
            /* must do checkpoint before measurement */

            RUN(scale, (gfx, source, &srcRect, framebuffer, &dstRect));
            checkpoint(gfx, checkpointEvent);
            BDBG_LOG(("%u fills completed", i));
            if(i>=4*10000) {
                break;
            }
        }
    }
    NEXUS_Surface_Flush(source);
    NEXUS_Surface_GetMemory(source, &sourceMem);
    for(i=0;i<100000;i++) {
        unsigned x = rand() % source_width;
        unsigned y = rand() % source_height;
        NEXUS_Pixel *buf = (NEXUS_Pixel *)((uint8_t *)sourceMem.buffer+y*sourceMem.pitch);
        buf[x] = rand();
    }
    /* flush cached memory */
    NEXUS_Surface_Flush(source);
    RUN(scale, (gfx, source, &srcRect, framebuffer, &dstRect));
    checkpoint(gfx, checkpointEvent);


    srcRect.x = srcRect.y = 0;
    srcRect.width = framebuffer_width;
    srcRect.height = framebuffer_height;
    RUN(copy, (gfx, framebuffer, &srcRect, source, 0, 0));


    NEXUS_Surface_GetMemory(golden, &goldenMem);
    NEXUS_Surface_GetMemory(framebuffer, &framebufferMem);
    for(xscale=-MAX_SCALE_FACTOR*10;xscale<=MAX_SCALE_FACTOR*10;xscale++) {
        for(yscale=-MAX_SCALE_FACTOR*10;yscale<MAX_SCALE_FACTOR*8;yscale++) {
            srcRect.width = framebuffer_width*pow(2, xscale/10.0);
            srcRect.height = framebuffer_height*pow(2, yscale/10.0);

            dstRect.x = dstRect.y = 0;
            dstRect.width = framebuffer_width;
            dstRect.height = framebuffer_height;
            BDBG_LOG(("scaling %u,%u -> %u,%u", srcRect.width, srcRect.height, dstRect.width, dstRect.height));
            /* scale and copy result to destination */
            RUN(scale, (gfx, source, &srcRect, golden, &dstRect));
            RUN(copy, (gfx, golden, &dstRect, framebuffer, 0, 0));

            checkpoint(gfx, checkpointEvent);
            NEXUS_Surface_Flush(golden);
            NEXUS_Surface_Flush(framebuffer);

            if(!surface_compare( &framebufferMem, &goldenMem, &dstRect)) {
               /* surfaces should match first time around */
               BDBG_ASSERT(0);
            }
            for(i=0;i<1000;) {
                NEXUS_Rect updateRect;

                updateRect.x = 0;
                updateRect.y = 0;
                updateRect.width = (rand()%(dstRect.width))+1;
                updateRect.height = (rand()%(dstRect.height))+1;
                if(dstRect.width != updateRect.width) {
                    updateRect.x = rand() % (dstRect.width-updateRect.width);
                }
                if(dstRect.height != updateRect.height) {
                    updateRect.y = rand() % (dstRect.height-updateRect.height);
                }
                RUN(fill, (gfx, framebuffer, &updateRect, 0)); 
                RUN(update_scale, (gfx, source, &srcRect, framebuffer, &dstRect, &updateRect));
                i++;
                if (i%1000==0) {
                    /* must do checkpoint before measurement */
                    checkpoint(gfx, checkpointEvent);
/*                    BDBG_LOG(("%u scales completed", i));*/
                    NEXUS_Surface_Flush(framebuffer);
                    if(!surface_compare( &framebufferMem,  &goldenMem, &dstRect)) {
/*                        return -1;*/
                    }
                }
            }
        }
    }

#endif

    return 0;
}
