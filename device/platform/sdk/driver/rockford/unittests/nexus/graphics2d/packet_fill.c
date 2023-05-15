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
 * $brcm_Workfile: packet_fill.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/31/12 5:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/packet_fill.c $
 * 
 * Hydra_Software_Devel/2   7/31/12 5:45p mward
 * SW7435-279: move hwFifoSize from NEXUS_Graphics2DOpenSettings to
 * NEXUS_Graphics2DModuleSettings.core[]
 * 
 * Hydra_Software_Devel/1   3/10/11 4:09p erickson
 * SW7405-5183: add packet_fill unittest
 * 
 * Hydra_Software_Devel/1   3/10/11 4:06p erickson
 * SW7405-5183: create packet_fill unittest
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

BDBG_MODULE(packet_fill);

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

static void fill_0(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle surface, BKNI_EventHandle spaceAvailableEvent, BKNI_EventHandle checkpointEvent)
{
    void *buffer, *next;
    size_t size;
    uint32_t color;
    unsigned coverage = 0;
    int rc;
    NEXUS_SurfaceMemory mem;

    rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, 1024);
    BDBG_ASSERT(!rc);
    if (!size) {
        /* internal queue is full. wait for space to become available. */
        BKNI_WaitForEvent(spaceAvailableEvent, BKNI_INFINITE);
        return;
    }

    next = buffer;

    while (1) {
        bool execute = (coverage == 0xF) && (rand()%4==0);
        unsigned cmd = rand() % 4;

        coverage |= 1 << cmd; /* we have to know when a minimum set of packets has been sent */

        /* don't let the randomness overflow the fifo */
#define SW_FIFO_SIZE (500 * 1024)
        if ((uint8_t*)next - (uint8_t*)buffer > SW_FIFO_SIZE - 1024) break;

        switch (cmd) {
        case 0:
            {
            BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, OutputFeeder, execute);
            NEXUS_Surface_InitPlane(surface, &pPacket->plane);
            next = ++pPacket;
            }
            break;

        case 1:
            {
            BM2MC_PACKET_Blend copyColor = {BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, false,
                BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
            BM2MC_PACKET_Blend copyAlpha = {BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, false,
                BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
            BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *)next;
            BM2MC_PACKET_INIT( pPacket, Blend, execute);
            pPacket->color_blend = copyColor;
            pPacket->alpha_blend = copyAlpha;
            pPacket->color = 0;
            next = ++pPacket;
            }
            break;

        case 2:
            {
            BM2MC_PACKET_PacketSourceColor *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, SourceColor, execute);
            pPacket->color = color = rand();
            next = ++pPacket;
            }
            break;

        case 3:
            {
            BM2MC_PACKET_PacketFillBlit *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, FillBlit, execute);
            pPacket->rect.x = 0;
            pPacket->rect.y = 0;
            pPacket->rect.width = rand() % 100 + 1;
            pPacket->rect.height = rand() % 100 + 1;
            next = ++pPacket;
            }
            break;
        }

        if (execute) break;
    }

    rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
    BDBG_ASSERT(!rc);

    checkpoint(gfx, checkpointEvent);
    NEXUS_Surface_Flush(surface);
    NEXUS_Surface_GetMemory(surface, &mem);
    BDBG_ASSERT(*(uint32_t*)mem.buffer == color);
}

static void fill_1(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle surface, BKNI_EventHandle spaceAvailableEvent, BKNI_EventHandle checkpointEvent)
{
    void *buffer, *next;
    size_t size;
    uint32_t color;
    unsigned coverage = 0;
    int rc;
    NEXUS_SurfaceMemory mem;

    rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, 1024);
    BDBG_ASSERT(!rc);
    if (!size) {
        /* internal queue is full. wait for space to become available. */
        BKNI_WaitForEvent(spaceAvailableEvent, BKNI_INFINITE);
        return;
    }

    next = buffer;

    while (1) {
        bool execute = (coverage == 0x7) && (rand()%4==0);
        unsigned cmd = rand() % 3;

        coverage |= 1 << cmd; /* we have to know when a minimum set of packets has been sent */

        /* don't let the randomness overflow the fifo */
#define SW_FIFO_SIZE (500 * 1024)
        if ((uint8_t*)next - (uint8_t*)buffer > SW_FIFO_SIZE - 1024) break;

        switch (cmd) {
        case 0:
            {
            BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, OutputFeeder, execute);
            NEXUS_Surface_InitPlane(surface, &pPacket->plane);
            next = ++pPacket;
            }
            break;

        case 1:
            {
            BM2MC_PACKET_Blend copyColor = {BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eOne, false,
                BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
            BM2MC_PACKET_Blend copyAlpha = {BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eOne, false,
                BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
            BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *)next;
            BM2MC_PACKET_INIT( pPacket, Blend, execute);
            pPacket->color_blend = copyColor;
            pPacket->alpha_blend = copyAlpha;
            pPacket->color = color = rand();
            next = ++pPacket;
            }
            break;

        case 2:
            {
            BM2MC_PACKET_PacketFillBlit *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, FillBlit, execute);
            pPacket->rect.x = 0;
            pPacket->rect.y = 0;
            pPacket->rect.width = rand() % 100 + 1;
            pPacket->rect.height = rand() % 100 + 1;
            next = ++pPacket;
            }
            break;
        }

        if (execute) break;
    }

    rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
    BDBG_ASSERT(!rc);

    checkpoint(gfx, checkpointEvent);
    NEXUS_Surface_Flush(surface);
    NEXUS_Surface_GetMemory(surface, &mem);
    BDBG_ASSERT(*(uint32_t*)mem.buffer == color);
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
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
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

    {
        NEXUS_Graphics2DOpenSettings openSettings;
        NEXUS_Graphics2D_GetDefaultOpenSettings(&openSettings);
        openSettings.packetFifoSize = SW_FIFO_SIZE;
        gfx = NEXUS_Graphics2D_Open(0, &openSettings);
    }

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
    for(i=0;i<20000;i++) {
        switch (rand() % 2) {
        case 0: fill_0(gfx, framebuffer, spaceAvailableEvent, checkpointEvent); break;
        case 1: fill_1(gfx, framebuffer, spaceAvailableEvent, checkpointEvent); break;
        }
        if (i && i%2000==0) {
            checkpoint(gfx, checkpointEvent);
            BDBG_WRN(("%d fills completed", i));
        }
    }
    checkpoint(gfx, checkpointEvent); /* prevent access after free */

    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Display_Close(display);
    NEXUS_Surface_Destroy(framebuffer);
    NEXUS_Platform_Uninit();

    return 0;
}
