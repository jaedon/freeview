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
 * $brcm_Workfile: packet_and_function.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/23/12 4:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/packet_and_function.c $
 * 
 * Hydra_Software_Devel/2   7/23/12 4:39p jessem
 * SW7425-3568: Added BKNI_DestroyEvent(event).
 *
 * Hydra_Software_Devel/1   5/20/11 2:25p erickson
 * SW7346-149: test multiplexing of packet and function-based blit with a
 * single context
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d.h"
#include "bm2mc_packet.h"
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

BDBG_MODULE(packet_and_function);

/* this test uses both packet and function api's in a single Graphics2D context.
they should not interfere with each other. the checkpoint and packetSpaceAvailable
callbacks should work together. */

void set_event(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void checkpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle event)
{
    NEXUS_Error rc;

    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        rc = BKNI_WaitForEvent(event, BKNI_INFINITE);
    }
    BDBG_ASSERT(!rc);
}

static void check_pixel(NEXUS_SurfaceHandle surface, const NEXUS_Rect *pRect, uint32_t color)
{
    NEXUS_SurfaceMemory mem;
    uint8_t *ptr;

    NEXUS_Surface_Flush(surface);
    NEXUS_Surface_GetMemory(surface, &mem);
    ptr = mem.buffer;
    ptr += pRect->y*mem.pitch;
    ptr += sizeof(uint32_t)*pRect->x;
    BDBG_ASSERT(*(uint32_t*)ptr == color);
}

static void packet_fill0(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle surface, uint32_t color, BKNI_EventHandle spaceAvailableEvent)
{
    void *buffer, *next;
    size_t size;
    int rc;

    rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, 1024);
    BDBG_ASSERT(!rc);
    if (!size) {
        /* internal queue is full. wait for space to become available. */
        rc = BKNI_WaitForEvent(spaceAvailableEvent, 1000);
        BDBG_ASSERT(!rc);
    }

    next = buffer;
    {
    BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
    BM2MC_PACKET_INIT(pPacket, OutputFeeder, false);
    NEXUS_Surface_InitPlane(surface, &pPacket->plane);
    next = ++pPacket;
    }

    {
    BM2MC_PACKET_Blend copyColor = {BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
    BM2MC_PACKET_Blend copyAlpha = {BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
    BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *)next;
    BM2MC_PACKET_INIT( pPacket, Blend, false);
    pPacket->color_blend = copyColor;
    pPacket->alpha_blend = copyAlpha;
    pPacket->color = 0;
    next = ++pPacket;
    }

    {
    BM2MC_PACKET_PacketSourceColor *pPacket = next;
    BM2MC_PACKET_INIT(pPacket, SourceColor, false);
    pPacket->color = color;
    next = ++pPacket;
    }

    rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
    BDBG_ASSERT(!rc);
}

static void packet_fill1(NEXUS_Graphics2DHandle gfx, const NEXUS_Rect *pRect, BKNI_EventHandle spaceAvailableEvent)
{
    void *buffer, *next;
    size_t size;
    int rc;

    rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, 1024);
    BDBG_ASSERT(!rc);
    if (!size) {
        /* internal queue is full. wait for space to become available. */
        rc = BKNI_WaitForEvent(spaceAvailableEvent, 1000);
        BDBG_ASSERT(!rc);
    }

    next = buffer;
    {
    BM2MC_PACKET_PacketFillBlit *pPacket = next;
    BM2MC_PACKET_INIT(pPacket, FillBlit, true);
    pPacket->rect.x = pRect->x;
    pPacket->rect.y = pRect->y;
    pPacket->rect.width = pRect->width;
    pPacket->rect.height = pRect->height;
    next = ++pPacket;
    }

    rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
    BDBG_ASSERT(!rc);
}

static void function_fill(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle surface, const NEXUS_Rect *pRect, uint32_t color, BKNI_EventHandle spaceAvailableEvent)
{
    NEXUS_Graphics2DFillSettings fillSettings;
    int rc;

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.rect = *pRect;
    fillSettings.color = color;
    for(;;) {
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
            rc = BKNI_WaitForEvent(spaceAvailableEvent, 1000);
            BDBG_ASSERT(!rc);
        } else {
            BDBG_ASSERT(rc==NEXUS_SUCCESS);
            break;
        }
    }
}

int main(void)
{
    NEXUS_SurfaceHandle framebuffer;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle spaceAvailableEvent, checkpointEvent;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_VideoFormatInfo info;
    int i;
    NEXUS_Error rc;
    uint32_t packet_color = 0;
    unsigned checks = 0;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = info.width;
    createSettings.height = info.height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    framebuffer = NEXUS_Surface_Create(&createSettings);

    BKNI_CreateEvent(&spaceAvailableEvent);
    BKNI_CreateEvent(&checkpointEvent);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.packetSpaceAvailable.callback = set_event;
    gfxSettings.packetSpaceAvailable.context = spaceAvailableEvent;
    gfxSettings.checkpointCallback.callback = set_event;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    /* fill with black */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    checkpoint(gfx, checkpointEvent);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);

    /* fill random boxes */
    BDBG_WRN(("starting"));
    for(i=0;i<200000;i++) {
        NEXUS_Rect rect = {100,100,50,50};
        switch (rand() % 3) {
        case 0:
            if (!packet_color || rand() % 10 == 0) {
                packet_color = rand()|0xFF000000;
                packet_fill0(gfx, framebuffer, packet_color, spaceAvailableEvent);
            }
            /* same color every packet blit */
            packet_fill1(gfx, &rect, spaceAvailableEvent);

            if (rand() % 100 == 0) {
                /* don't checkpoint every time so the queue will fill */
                checkpoint(gfx, checkpointEvent);
                check_pixel(framebuffer, &rect, packet_color);
                checks++;
            }
            break;
        case 1:
            rect.x += 300;
            /* change color every function blit */
            {
                uint32_t function_color = rand()|0xFF000000;
                function_fill(gfx, framebuffer, &rect, function_color, spaceAvailableEvent);
                if (rand() % 100 == 0) {
                    /* don't checkpoint every time so the queue will fill */
                    checkpoint(gfx, checkpointEvent);
                    check_pixel(framebuffer, &rect, function_color);
                    checks++;
                }
            }
            break;
        case 2:
            /* throw in a little timing variation */
            if (rand() % 100 == 0) {
                BKNI_Sleep(rand() % 100);
            }
            break;
        }
        if (i && i%1000==0) {
            checkpoint(gfx, checkpointEvent);
            BDBG_WRN(("%d iterations: %d checks", i, checks));
        }
    }
    checkpoint(gfx, checkpointEvent); /* prevent access after free */

    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Display_Close(display);
    BKNI_DestroyEvent(spaceAvailableEvent);
    BKNI_DestroyEvent(checkpointEvent);
    NEXUS_Surface_Destroy(framebuffer);
    NEXUS_Platform_Uninit();

    return 0;
}
