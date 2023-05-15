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
 * $brcm_Workfile: packet_blit_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 3/2/11 1:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/packet_blit_stress.c $
 * 
 * Hydra_Software_Devel/3   3/2/11 1:44p randyjew
 * SW7344-31: Add begin/end prints for expected failures
 * 
 * Hydra_Software_Devel/2   9/22/10 5:11p erickson
 * SW7405-4882: add NEXUS_Graphics2D_ConvertFilter and
 * NEXUS_Graphics2D_ConvertColorMatrix tests
 *
 * Hydra_Software_Devel/1   8/16/10 1:03p erickson
 * SW7405-3671: packet blit tests
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

#define BM2MC_PACKET_BlendFactor_eMax (BM2MC_PACKET_BlendFactor_eInverseConstantAlpha+1)
#define BM2MC_PACKET_PacketType_eMax (BM2MC_PACKET_PacketType_eRestoreState+1)

void set_blend(BM2MC_PACKET_Blend *pBlend)
{
    pBlend->a = rand() % BM2MC_PACKET_BlendFactor_eMax;
    pBlend->b = rand() % BM2MC_PACKET_BlendFactor_eMax;
    pBlend->sub_cd = rand() % 2;
    pBlend->c = rand() % BM2MC_PACKET_BlendFactor_eMax;
    pBlend->d = rand() % BM2MC_PACKET_BlendFactor_eMax;
    pBlend->sub_e = rand() % 2;
    pBlend->e = rand() % BM2MC_PACKET_BlendFactor_eMax;
}

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
    NEXUS_MemoryStatus memStatus;
    NEXUS_SurfaceMemory mem;
    BM2MC_PACKET_Plane framebufferPlane, offscreenPlane;
    bool sent[BM2MC_PACKET_PacketType_eMax];

    srand(time(NULL));

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    BKNI_Memset(sent, 0, sizeof(sent));

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
    createSettings.heap = platformConfig.heap[0];
#endif
    rc = NEXUS_Heap_GetStatus(createSettings.heap, &memStatus);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);

    framebuffer = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(framebuffer, &mem);
    framebufferPlane.address = memStatus.offset + (uint8_t*)mem.buffer - (uint8_t*)memStatus.addr;
    framebufferPlane.pitch = mem.pitch;
    framebufferPlane.format = createSettings.pixelFormat;

    offscreen = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(offscreen, &mem);
    offscreenPlane.address = memStatus.offset + (uint8_t*)mem.buffer - (uint8_t*)memStatus.addr;
    offscreenPlane.pitch = mem.pitch;
    offscreenPlane.format = createSettings.pixelFormat;

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

    /* before we start, check a few things */
    for (i=0;i<10000;i++) {
        BM2MC_PACKET_FilterCoeffs coeffs;
        BM2MC_PACKET_ColorMatrix packetMatrix;
        NEXUS_Graphics2DColorMatrix matrix;

        rc = NEXUS_Graphics2D_ConvertFilter( rand()%NEXUS_Graphics2DFilterCoeffs_eMax, rand()%200, rand()%200, &coeffs);
        BDBG_ASSERT(!rc);

        BKNI_Memset(&matrix, rand(), sizeof(matrix));
        rc = NEXUS_Graphics2D_ConvertColorMatrix(&matrix, &packetMatrix);
        BDBG_ASSERT(!rc);
    }
    {
        void *buffer;
        unsigned size;
        /* verify that NEXUS_Graphics2D_PacketWriteComplete fails if we complete a size too large */
        rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, 1000);
        BDBG_ASSERT(!rc);
	    BDBG_WRN(("BEGIN EXPECTED FAILURE"));
        rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, size+1);
        BDBG_ASSERT(rc);

        /* verify NEXUS_Graphics2D_GetPacketBuffer is not destructive and that minSize doesn't limit size */
        for (i=0;i<100;i++) {
            unsigned size2;
            rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size2, 1000+100*i);
            BDBG_ASSERT(!rc);
            BDBG_ASSERT(size == size2);
        }
	    BDBG_WRN(("END EXPECTED FAILURE"));
        /* it's ok to complete nothing */
        rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, 0);
        BDBG_ASSERT(!rc);
    }

    for (i=0;i<500000;i++) {
        BM2MC_PACKET_Rectangle outRect, srcRect;
        NEXUS_Error rc;
        unsigned op = rand() % BM2MC_PACKET_PacketType_eMax;
        void *buffer, *next;
        size_t size;
        bool execute = rand() % 2;

#define MIN_SIZE 1024
        rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, MIN_SIZE);
        BDBG_ASSERT(!rc);
        if (!size) {
            /* internal queue is full. wait for space to become available. */
            BDBG_WRN(("BKNI_WaitForEvent(spaceAvailableEvent)"));
            rc = BKNI_WaitForEvent(spaceAvailableEvent, 1000);
            BDBG_ASSERT(!rc);
            continue;
        }
        next = buffer;

add_more_packets:
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

        execute = execute && sent[BM2MC_PACKET_PacketType_eOutputFeeder];

        switch (op) {
#if 0
/* TODO */
    BM2MC_PACKET_PacketType_eSourceFeeders,              /* source planes and color */
    BM2MC_PACKET_PacketType_eSourceNone,                 /* source none (default) */
    BM2MC_PACKET_PacketType_eSourceControl,              /* source control of zero_pad and chroma_filter */
    BM2MC_PACKET_PacketType_eDestinationNone,            /* destination none (default) */
    BM2MC_PACKET_PacketType_eDestinationControl,         /* destination control of zero_pad and chroma_filter */
    BM2MC_PACKET_PacketType_eOutputControl,              /* output control of dither and chroma_filter */
    BM2MC_PACKET_PacketType_eBlendColor,                 /* blend color */
    BM2MC_PACKET_PacketType_eRop,                        /* ternary raster operation */
    BM2MC_PACKET_PacketType_eSourceColorkey,             /* source colorkeys, masks, and replacements */
    BM2MC_PACKET_PacketType_eSourceColorkeyEnable,       /* source colorkey enable */
    BM2MC_PACKET_PacketType_eDestinationColorkey,        /* destination colorkeys, masks, and replacements */
    BM2MC_PACKET_PacketType_eDestinationColorkeyEnable,  /* destination colorkey enable */
    BM2MC_PACKET_PacketType_eFilter,                     /* filter coefficient table */
    BM2MC_PACKET_PacketType_eFilterEnable,               /* filter enable */
    BM2MC_PACKET_PacketType_eFilterControl,              /* filter control of rounding, channel adjustment and ordering */
    BM2MC_PACKET_PacketType_eSourceColorMatrix,          /* source color matrix */
    BM2MC_PACKET_PacketType_eSourceColorMatrixEnable,    /* source color matrix enable */
    BM2MC_PACKET_PacketType_eSourcePalette,              /* source palette address */
    BM2MC_PACKET_PacketType_eAlphaPremultiply,           /* multiply pixel color by pixel alpha before blending */
    BM2MC_PACKET_PacketType_eMirror,                     /* reverse read/write order for source, destination and output feeders */
    BM2MC_PACKET_PacketType_eScaleBlendBlit,             /* scale and blend/rop/colorkey source rectangle to output rectangle */
    BM2MC_PACKET_PacketType_eDestripeBlit,               /* YCbCr420 destripe blit */
    BM2MC_PACKET_PacketType_eSaveState,                  /* save state */
    BM2MC_PACKET_PacketType_eRestoreState                /* restore state */
#endif

        case BM2MC_PACKET_PacketType_eOutputFeeder:
            {
            BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
            execute = false; /* TODO: GRC should check minimum status before allowing execute=true to proceed. */
            BM2MC_PACKET_INIT(pPacket, OutputFeeder, execute);
            pPacket->plane = framebufferPlane;
            next = ++pPacket;
            }
            break;
        case BM2MC_PACKET_PacketType_eDestinationFeeder:
            {
            BM2MC_PACKET_PacketDestinationFeeder *pPacket = next;
            execute = false;
            BM2MC_PACKET_INIT(pPacket, DestinationFeeder, execute);
            pPacket->plane = framebufferPlane;
            pPacket->color = rand();
            next = ++pPacket;
            }
            break;
        case BM2MC_PACKET_PacketType_eSourceFeeder:
            {
            BM2MC_PACKET_PacketSourceFeeder *pPacket = next;
            execute = false;
            BM2MC_PACKET_INIT(pPacket, SourceFeeder, execute);
            pPacket->plane = offscreenPlane;
            pPacket->color = rand();
            next = ++pPacket;
            }
            break;
        case BM2MC_PACKET_PacketType_eSourceColor:
            {
            BM2MC_PACKET_PacketSourceColor *pPacket = next;
            execute = false;
            BM2MC_PACKET_INIT(pPacket, SourceColor, execute );
            pPacket->color = rand();
            next = ++pPacket;
            sent[BM2MC_PACKET_PacketType_eSourceFeeder] = false; /* TODO: SourceColor shouldn't wipe out SourceFeeder */
            }
            break;
        case BM2MC_PACKET_PacketType_eBlend:
            {
            BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *)next;
            execute = false;
            BM2MC_PACKET_INIT( pPacket, Blend, execute);
            set_blend(&pPacket->color_blend);
            set_blend(&pPacket->alpha_blend);
            pPacket->color = rand();
            next = ++pPacket;
            }
            break;
        case BM2MC_PACKET_PacketType_eScaleBlit:
            {
            BM2MC_PACKET_PacketScaleBlit *pPacket = next;
            execute = execute && sent[BM2MC_PACKET_PacketType_eSourceFeeder];
            execute = execute && sent[BM2MC_PACKET_PacketType_eBlend];
            BM2MC_PACKET_INIT(pPacket, ScaleBlit, execute);
            pPacket->src_rect = srcRect;
            pPacket->out_rect = outRect;
            next = ++pPacket;
            }
            break;
        case BM2MC_PACKET_PacketType_eCopyBlit:
            {
            BM2MC_PACKET_PacketCopyBlit *pPacket = next;
            execute = execute && sent[BM2MC_PACKET_PacketType_eSourceFeeder];
            execute = execute && sent[BM2MC_PACKET_PacketType_eBlend];
            BM2MC_PACKET_INIT(pPacket, CopyBlit, execute);
            pPacket->src_rect = srcRect;
            pPacket->out_point.x = srcRect.x; /* can't use outRect.x because of bounds check */
            pPacket->out_point.y = srcRect.y;
            next = ++pPacket;
            }
            break;
        case BM2MC_PACKET_PacketType_eBlendBlit:
            {
            BM2MC_PACKET_PacketBlendBlit *pPacket = next;
            execute = execute && sent[BM2MC_PACKET_PacketType_eSourceFeeder];
            execute = execute && sent[BM2MC_PACKET_PacketType_eDestinationFeeder];
            execute = execute && sent[BM2MC_PACKET_PacketType_eBlend];
            BM2MC_PACKET_INIT(pPacket, BlendBlit, execute);
            pPacket->src_rect = srcRect;
            pPacket->out_point.x = srcRect.x; /* can't use outRect.x because of bounds check */
            pPacket->out_point.y = srcRect.y;
            pPacket->dst_point = pPacket->out_point;
            next = ++pPacket;
            }
            break;
        case BM2MC_PACKET_PacketType_eFillBlit:
            {
            BM2MC_PACKET_PacketFillBlit *pPacket = next;
            execute = execute && sent[BM2MC_PACKET_PacketType_eSourceColor];
            execute = execute && sent[BM2MC_PACKET_PacketType_eBlend];
            BM2MC_PACKET_INIT(pPacket, FillBlit, execute);
            pPacket->rect = outRect;
            next = ++pPacket;
            }
            break;
        case BM2MC_PACKET_PacketType_eResetState:
            {
            BM2MC_PACKET_PacketResetState *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, ResetState, execute);
            next = ++pPacket;
            BKNI_Memset(sent, 0, sizeof(sent));
            }
            break;
        }
        sent[op] = true;

        {
            unsigned used = (uint8_t*)next - (uint8_t*)buffer;

            /* we cannot overrun */
            BDBG_ASSERT(used <= size);

            /* if there's room for at least MIN_SIZE next time, we don't have to complete now. */
            if ((used + MIN_SIZE > size) || (rand() % 10 == 0))
            {
                rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, used);
                BDBG_ASSERT(!rc);
            }
            else {
                goto add_more_packets;
            }
        }

        if (rand() % 100 == 0) {
            rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
            if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                rc = BKNI_WaitForEvent(checkpointEvent, 1000);
            }
            BDBG_ASSERT(!rc);
        }
    }
#endif

    return 0;
}

