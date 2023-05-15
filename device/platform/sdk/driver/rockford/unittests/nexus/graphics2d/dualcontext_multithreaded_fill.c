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
 * $brcm_Workfile: dualcontext_multithreaded_fill.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/28/11 4:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/dualcontext_multithreaded_fill.c $
 * 
 * Hydra_Software_Devel/2   10/28/11 4:46p erickson
 * SW7231-415: when intentionally-bad checkpoint is issued, we can't be
 * sure if we'll get 1 or 2 events
 *
 * Hydra_Software_Devel/1   10/27/11 1:52p erickson
 * SW7231-415: add unittest that would have caught this bug
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d.h"
#endif
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

BDBG_MODULE(dualcontext_multithreaded_fill);

#if NEXUS_HAS_GRAPHICS2D
/* this data is setup by the main thread, but used by each blit thread */
struct app_data {
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceHandle surface;
    bool done; /* used to terminate threads cleanly */
} g_data;

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

/* continually blit from a dedicated thread.
each thread gets its own NEXUS_Graphics2D instance and event */
void *blit_thread(void *context)
{
    NEXUS_Error rc;
    struct app_data *app_data = context;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceEvent;
    unsigned id = pthread_self();
    unsigned cnt = 0;
    BM2MC_PACKET_Plane framebufferPlane;
    BM2MC_PACKET_Blend copyColor = {BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
    BM2MC_PACKET_Blend copyAlpha = {BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};

    rc = BKNI_CreateEvent(&checkpointEvent);
    BDBG_ASSERT(!rc);
    rc = BKNI_CreateEvent(&packetSpaceEvent);
    BDBG_ASSERT(!rc);
    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Surface_InitPlane(app_data->surface, &framebufferPlane);

    while (!app_data->done) {
        if (rand()%2) {
            /* function blit */
            NEXUS_Graphics2DFillSettings fillSettings;

            if (++cnt % 10000 == 0) BKNI_Printf("blit_thread(%d): %d\n", id, cnt);

            NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
            fillSettings.surface = app_data->surface;
            fillSettings.rect.x = rand() % (app_data->createSettings.width-20);
            fillSettings.rect.y = rand() % (app_data->createSettings.height-20);
            fillSettings.rect.width = 20;
            fillSettings.rect.height = 20;
            fillSettings.colorOp = NEXUS_FillOp_eCopy;
            fillSettings.alphaOp = NEXUS_FillOp_eCopy;

            fillSettings.color = (rand()%0xFFFFFF) | ((rand()%0xFF)<<24);

repeat_fill:
            rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
            if (rc) {
                do {
                    rc = BKNI_WaitForEvent(packetSpaceEvent, 1000);
                } while (rc == NEXUS_TIMEOUT);
                BDBG_ASSERT(!rc);
                goto repeat_fill;
            }
        }
        else {
            void *buffer, *next;
            size_t size;

            /* packet blit */
repeat_getbuffer:
            rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, 1024);
            if (rc) {
                rc = BKNI_WaitForEvent(packetSpaceEvent, 1000);
                do {
                    rc = BKNI_WaitForEvent(packetSpaceEvent, 1000);
                } while (rc == NEXUS_TIMEOUT);
                BDBG_ASSERT(!rc);
                goto repeat_getbuffer;
            }

            next = buffer;

            {
            BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, OutputFeeder, false);
            pPacket->plane = framebufferPlane;
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
            BM2MC_PACKET_PacketSourceColor *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, SourceColor, false );
            pPacket->color = (rand()%0xFFFFFF) | (rand()%0xFF<<24);
            next = ++pPacket;
            }

            {
            BM2MC_PACKET_PacketFillBlit *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, FillBlit, true);
            pPacket->rect.x = rand() % (app_data->createSettings.width-20);
            pPacket->rect.y = rand() % (app_data->createSettings.height-20);
            pPacket->rect.width = 20;
            pPacket->rect.height = 20;
            next = ++pPacket;
            }

            rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
            BDBG_ASSERT(!rc);
        }

        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
#define TEST_BAD_CHECKPOINT 1
#if TEST_BAD_CHECKPOINT
            bool absorb_all_events = false;
            /* if we timeout, we must keep waiting. the callback must come. */

            /* once in a while, issue a bad checkpoint. this tests if nexus protects
            itself adequately. */            
            if (rand()%2000 == 0) {
                rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
                if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                    /* at this point, we don't know if we'll get 1 or 2 events. there is no way to
                    know if they'll be delivered separately or collapsed. so, we'll just 
                    eat all events for a second to make sure it's clear. */
                    BDBG_WRN(("second checkpoint issued and queued"));
                    absorb_all_events = true;
                }
                else {
                    BDBG_ASSERT(!rc || rc == NEXUS_NOT_AVAILABLE);
                }
            }
#endif
            
            rc = BKNI_WaitForEvent(checkpointEvent, 1000);
            BDBG_ASSERT(!rc);
            
#if TEST_BAD_CHECKPOINT
            if (absorb_all_events) {
                /* may have one or one more event coming */
                rc = BKNI_WaitForEvent(checkpointEvent, 1000);
                BDBG_ASSERT(!rc || rc == NEXUS_TIMEOUT);
                rc = 0;
            }
#endif
        }
        BDBG_ASSERT(!rc);
    }
    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceEvent);
    return NULL;
}
#endif

int main(void)
{
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DHandle gfx;
    int i;
#define MAX_THREADS 4
    pthread_t thread[MAX_THREADS];
    struct app_data app_data;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_Memset(&app_data, 0, sizeof(app_data));

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif

    NEXUS_Surface_GetDefaultCreateSettings(&app_data.createSettings);
    app_data.createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    app_data.createSettings.width = 720;
    app_data.createSettings.height = 480;
    app_data.createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    app_data.surface = NEXUS_Surface_Create(&app_data.createSettings);

    /* fill with black. this instance of the blitter is not used in the threads. */
    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = app_data.surface;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_Close(gfx);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, app_data.surface);
    BDBG_ASSERT(!rc);

    /* fill random boxes from multiple threads*/
    for (i=0;i<MAX_THREADS;i++) {
        rc = pthread_create(&thread[i], NULL, blit_thread, &app_data);
        BDBG_ASSERT(!rc);
    }

#if 0
    printf("press ENTER to quit\n");
    getchar();
#else
    /* run for 2 minutes */
    BKNI_Sleep(1000 * 120);
#endif

    /* take down all the threads */
    app_data.done = true;
    for (i=0;i<MAX_THREADS;i++) {
        pthread_join(thread[i], NULL);
    }

    /* take down the system */
    NEXUS_Display_Close(display);
    NEXUS_Surface_Destroy(app_data.surface);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}
