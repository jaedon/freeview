/***************************************************************************
 *     (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: blit_client.c $
 * $brcm_Revision: 31 $
 * $brcm_Date: 11/27/12 1:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/blit_client.c $
 * 
 * 31   11/27/12 1:40p erickson
 * SW7435-421: undo previous change. IPC comes up before nexus join and
 *  comes down after nexus uninit.
 * 
 * 30   11/6/12 12:12p erickson
 * SW7425-1364: sync up blit.c and blit_client.c
 * 
 * 29   11/6/12 10:14a erickson
 * SW7425-1364: improve error messages and client options
 * 
 * 28   10/16/12 4:42p erickson
 * SW7435-421: fix memory leak
 * 
 * 27   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 26   7/9/12 1:57p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 * 
 * 25   4/26/12 4:34p erickson
 * SW7425-2882: standardize fully mapped heap
 * 
 * 24   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 * 
 * 23   3/27/12 9:37a erickson
 * SW7435-74: use unsigned to avoid out of bounds blits on wrap around
 * 
 * 22   3/23/12 4:48p erickson
 * SW7346-747: increase GetPacketBuffer minsize
 * 
 * 21   3/14/12 10:15a erickson
 * SW7425-2616: find heap with eFull mapping for packet blit, fail if
 *  can't find it
 * 
 * 20   12/19/11 10:09a erickson
 * SW7425-1795: use eFull heap for graphics2d packet fifo or playpump fifo
 * 
 * 19   12/9/11 1:45p erickson
 * SW7425-1364: remove client movement support. see cmd_client.
 * 
 * 18   11/22/11 1:40p vsilyaev
 * SW7420-2148: Use proper arguments for NEXUS_SurfaceClient_UpdateSurface
 * 
 * 17   10/27/11 2:37p erickson
 * SW7425-1364: rename to brc
 * 
 * 16   10/26/11 3:56p erickson
 * SW7425-1364: extend app ipc
 * 
 * 15   10/12/11 5:19p vsilyaev
 * SW7425-1364: Added client initiated movement support
 * 
 * 14   10/5/11 11:24a vsilyaev
 * SW7425-1364: Added reference server
 * 
 * 1   10/4/11 5:44p vsilyaev
 * SW7425-1364: Reference applicaion IPC and reference server
 * 
 **************************************************************************/

#include "nexus_platform_client.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#if NEXUS_HAS_SURFACE_COMPOSITOR
#include "nexus_surface_client.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "refsw_session_simple_client.h"

BDBG_MODULE(blit_client);

#define USE_PACKET_BLIT 1

#if NEXUS_HAS_SURFACE_COMPOSITOR
static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

int main(int argc, const char * const argv[])
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, displayedEvent;
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_Error rc;
    unsigned i;
    int gfx_client_id = -1;
    NEXUS_SurfaceComposition composition;
    int curarg = 1;
    
    while (argc > curarg) {
        if (!strcmp(argv[curarg], "-client") && argc>curarg+1) {
            gfx_client_id = atoi(argv[++curarg]);
        }
        curarg++;
    }

    simple_client_init(argv[0], &authSettings);
    printf("using certificate:'%s'\n", (char *)authSettings.certificate.data);
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        printf("cannot join: %d\n", rc);
        return -1;
    }

    NEXUS_Platform_GetClientConfiguration(&clientConfig);
    if (gfx_client_id == -1 && clientConfig.resources.surfaceClient.total) {
        gfx_client_id = clientConfig.resources.surfaceClient.id[0];
    }

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = 480;
    /* createSettings.heap is NULL. proxy will populate. */
    surface = NEXUS_Surface_Create(&createSettings);

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&packetSpaceAvailableEvent);
    BKNI_CreateEvent(&displayedEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    /* draw gradient on left side of black framebuffer using synchronous blits */
#define SIDEBAR_WIDTH 100
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
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
                BKNI_WaitForEvent(packetSpaceAvailableEvent, BKNI_INFINITE);
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

    blit_client = NEXUS_SurfaceClient_Acquire(gfx_client_id);
    if (!blit_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed. run client with '-client X' to change ids.", gfx_client_id));
        return -1;
    }
    
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.displayed.callback = complete;
    client_settings.displayed.context = displayedEvent;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_SurfaceClient_SetSurface(blit_client, surface);
    BDBG_ASSERT(!rc);
    rc = BKNI_WaitForEvent(displayedEvent, 5000);
    BDBG_ASSERT(!rc);

    if (simple_client_state.rca_client) {
        brc_client_get_composition(simple_client_state.rca_client, &composition);
    }
    
    /* blit from left-hand gradient into the rest of the framebuffer */
    BDBG_WRN(("starting"));
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings); /* don't get defaults more often than necessary */
    for(i=0;;) {
#if USE_PACKET_BLIT
        NEXUS_Error rc;
        void *buffer, *next;
        size_t size;
        unsigned j;
        BM2MC_PACKET_Plane surfacePlane;    
        static const BM2MC_PACKET_Blend copyColor = {BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, false,
            BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
        static const BM2MC_PACKET_Blend copyAlpha = {BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, false,
            BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
            
        NEXUS_Surface_InitPlane(surface, &surfacePlane);

        rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, 2048);
        BDBG_ASSERT(!rc);
        if (!size) {
            /* internal queue is full. wait for space to become available. */
            BKNI_WaitForEvent(packetSpaceAvailableEvent, BKNI_INFINITE);
            continue;
        }

        next = buffer;

        {
        BM2MC_PACKET_PacketSourceFeeder *pPacket = next;
        /* if your app crashes here, your NEXUS_Graphics2DOpenSettings.heap (or default heap) is likely 
        not CPU-accessible by the application. */
        BM2MC_PACKET_INIT(pPacket, SourceFeeder, false );
        pPacket->plane = surfacePlane;
        pPacket->color = 0;
        next = ++pPacket;
        }

        {
        BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, OutputFeeder, false);
        pPacket->plane = surfacePlane;
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

        for (j=0;j<50;j++) {
            BM2MC_PACKET_PacketScaleBlit *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, ScaleBlit, true);
            pPacket->src_rect.x = 0;
            pPacket->src_rect.y = (i/300) % (createSettings.height - 140);
            pPacket->src_rect.width = SIDEBAR_WIDTH;
            pPacket->src_rect.height = 140;
            pPacket->out_rect.x = (rand() % (createSettings.width-120)) + SIDEBAR_WIDTH;
            pPacket->out_rect.y = (rand() % (createSettings.height-20));
            pPacket->out_rect.width = 20;
            pPacket->out_rect.height = 20;
            next = ++pPacket;

            i++;
        }

        rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
        BDBG_ASSERT(!rc);    
#else
        unsigned j;
        for (j=0;j<50;j++) {
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
                    BKNI_WaitForEvent(packetSpaceAvailableEvent, BKNI_INFINITE);
                    /* retry */
                }
                else {
                    break;
                }
            }
            i++;
        }
#endif

        /* must do checkpoint before UpdateSurface because server uses different blitter fifo. */
        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            rc = BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
        }
        BDBG_ASSERT(!rc);

        /* tell server to blit */
        rc = NEXUS_SurfaceClient_UpdateSurface(blit_client, NULL);
        BDBG_ASSERT(!rc);
        rc = BKNI_WaitForEvent(displayedEvent, 5000);
        BDBG_ASSERT(!rc);
        if (i && i%50000==0) {
            BDBG_WRN(("%u blits completed", i));
        }
        /* no flush is needed because we're not using the CPU */
    }

    NEXUS_SurfaceClient_Release(blit_client);
    BKNI_DestroyEvent(displayedEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Platform_Uninit();
    simple_client_shutdown();
    return 0;
}
#else
int main(void)
{
    printf("ERROR: NEXUS_SurfaceCompositor not supported\n");
    return -1;
}
#endif
