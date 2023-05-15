/***************************************************************************
 *     (c)2011 Broadcom Corporation
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
 * $brcm_Workfile: multibuffered_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 12/20/11 9:22a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/surface_compositor/multibuffered_stress.c $
 * 
 * Hydra_Software_Devel/4   12/20/11 9:22a erickson
 * SW7420-1992: fix warning
 * 
 * Hydra_Software_Devel/3   12/12/11 3:46p erickson
 * SW7420-1992: fix app shutdown
 * 
 * Hydra_Software_Devel/2   12/9/11 5:01p erickson
 * SW7420-1992: add unittests
 * 
 * Hydra_Software_Devel/1   11/18/11 2:57p erickson
 * SW7420-1148: add test
 * 
 **************************************************************************/
#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_memory.h"
#include "nexus_base_mmap.h"
#include "nexus_surface_compositor.h"
#include "nexus_surface_client.h"
#include "nexus_display.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

BDBG_MODULE(multibuffered_stress);

static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void print_usage(const char *app)
{
    printf(
    "usage: nexus %s OPTIONS\n", app);
    printf(
    "options:\n"
    "  --help|-h    print this help screen\n"
    "  -timeout X   run for X seconds, then exit (default 120)\n"
    );
}

#define NUM_CLIENTS 25

static bool g_done[NUM_CLIENTS];
static void *thread_entry(void *context);

#include <sys/time.h>
static unsigned b_get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

int main(int argc, char **argv)
{
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlatformConfiguration platformConfig;
    int rc;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_SurfaceCompositorSettings surface_compositor_settings;
    NEXUS_SurfaceCompositorHandle surface_compositor;
    int curarg = 1;
    unsigned timeout = 2*60; /* seconds */
    unsigned i;
    NEXUS_SurfaceClientHandle blit_client[NUM_CLIENTS];
    pthread_t thread[NUM_CLIENTS];
    unsigned starttime;
    
    while (curarg < argc) {
        if (!strcmp(argv[curarg], "--help") || !strcmp(argv[curarg], "-h")) {
            print_usage(argv[0]);
            return 0;
        }
        else if (!strcmp(argv[curarg], "-timeout") && curarg+1<argc) {
            /* 0 means getchar() */
            timeout = atoi(argv[++curarg]);
        }
        else {
            fprintf(stderr,"invalid argument %s\n", argv[curarg]);
            print_usage(argv[0]);
            return -1;
        }
        curarg++;
    }
    srand(time(NULL));

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif

    surface_compositor = NEXUS_SurfaceCompositor_Create(0);
    NEXUS_SurfaceCompositor_GetSettings(surface_compositor, &surface_compositor_settings);
    NEXUS_Display_GetGraphicsSettings(display, &surface_compositor_settings.display[0].graphicsSettings);
    surface_compositor_settings.display[0].graphicsSettings.enabled = true;
    surface_compositor_settings.display[0].display = display;
    surface_compositor_settings.display[0].framebuffer.heap = NEXUS_Platform_GetFramebufferHeap(0);
    surface_compositor_settings.display[0].framebuffer.width = 720;
    surface_compositor_settings.display[0].framebuffer.height = 480;
    rc = NEXUS_SurfaceCompositor_SetSettings(surface_compositor, &surface_compositor_settings);
    BDBG_ASSERT(!rc);
    
    starttime = b_get_time();
    BKNI_Memset(blit_client, 0, sizeof(blit_client));
    while (!timeout || b_get_time() - starttime < timeout*1000) {
        unsigned i = rand() % NUM_CLIENTS;
        
        if (!blit_client[i]) {
            blit_client[i] = NEXUS_SurfaceCompositor_CreateClient(surface_compositor, i);
            BDBG_ASSERT(blit_client[i]);
            g_done[i] = false;
            rc = pthread_create(&thread[i], NULL, thread_entry, (void*)i);
            BDBG_ASSERT(!rc);
        }
        else if (rand() % 5 == 0) {
            NEXUS_SurfaceCompositorClientSettings settings;
            NEXUS_SurfaceCompositor_GetClientSettings(surface_compositor, blit_client[i], &settings);
            settings.composition.virtualDisplay.width = 720;
            settings.composition.virtualDisplay.height = 480;
            if (rand() % 10 == 0) {
                /* test full screen case */
                settings.composition.position.width = settings.composition.virtualDisplay.width;
                settings.composition.position.height = settings.composition.virtualDisplay.height;
                settings.composition.position.x = 0;
                settings.composition.position.y = 0;
            }
            else {
                /* TODO: expand range of possible sizes once GRC handles double scale */
                settings.composition.position.width = 50 + (rand()%100);
                settings.composition.position.height = 50 + (rand()%100);
                settings.composition.position.x = rand()%(settings.composition.virtualDisplay.width-settings.composition.position.width);
                settings.composition.position.y = rand()%(settings.composition.virtualDisplay.height-settings.composition.position.height);
            }
            settings.composition.visible = true;
            settings.composition.zorder = rand();
            rc = NEXUS_SurfaceCompositor_SetClientSettings(surface_compositor, blit_client[i], &settings);
            BDBG_ASSERT(!rc);
        }
        else if (rand() % 20 == 0) {
            g_done[i] = true;    
            pthread_join(thread[i], NULL);
            NEXUS_SurfaceCompositor_DestroyClient(blit_client[i]);
            blit_client[i] = NULL;
        }
    }
        
    /* exit all */
    for (i=0;i<NUM_CLIENTS;i++) {
        if (blit_client[i]) {
            g_done[i] = true;    
            pthread_join(thread[i], NULL);
            NEXUS_SurfaceCompositor_DestroyClient(blit_client[i]);
        }
    }
    
    NEXUS_SurfaceCompositor_Destroy(surface_compositor);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();
    return 0;
}

static void *thread_entry(void *context)
{
    unsigned client_id = (unsigned)context;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle recycledEvent;
#define NUM_SURFACES 25
    struct {
        NEXUS_SurfaceHandle handle;
    } surface[NUM_SURFACES];
    unsigned submit_ptr = 0, recycle_ptr = 0;
    int rc;
    unsigned i;
    unsigned skip_recycle = 0;
    unsigned background_color, bar_color;
    
    BKNI_CreateEvent(&recycledEvent);
    gfx = NEXUS_Graphics2D_Open(0, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.pollingCheckpoint = true;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);
    
    background_color = 0xFF000000|rand();
    bar_color = 0xFF000000|rand();
    for (i=0;i<NUM_SURFACES;i++) {
        NEXUS_SurfaceCreateSettings settings;
        NEXUS_Graphics2DFillSettings fill;
        
        NEXUS_Surface_GetDefaultCreateSettings(&settings);
        settings.width = 100;
        settings.height = 100;
        surface[i].handle = NEXUS_Surface_Create(&settings);
        BDBG_ASSERT(surface[i].handle);
        
        NEXUS_Graphics2D_GetDefaultFillSettings(&fill);
        fill.surface = surface[i].handle;
        fill.color = background_color;
        do {
            rc = NEXUS_Graphics2D_Fill(gfx, &fill);
            if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) BKNI_Sleep(1);
        } while (rc);
        
        fill.color = bar_color;
        fill.rect.width = settings.width/NUM_SURFACES;
        fill.rect.height = settings.height;
        fill.rect.x = i*fill.rect.width;
        fill.rect.y = 0;
        do {
            rc = NEXUS_Graphics2D_Fill(gfx, &fill);
            if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) BKNI_Sleep(1);
        } while (rc);
    }
    do {
        BKNI_Sleep(1);
        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    } while (rc == NEXUS_GRAPHICS2D_BUSY);
    BDBG_ASSERT(!rc);
    
    blit_client = NEXUS_SurfaceClient_Acquire(client_id);
    BDBG_ASSERT(blit_client);
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.recycled.callback = complete;
    client_settings.recycled.context = recycledEvent;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);
    
#define FIFO_DEPTH(WPTR,RPTR) ((WPTR)>=(RPTR)?(WPTR)-(RPTR):NUM_SURFACES-((RPTR)-(WPTR)))

    while (!g_done[client_id]) {
#if 0
        /* simple, lock step push/recycle. not much of a unittest. */
        unsigned n;
        
        rc = NEXUS_SurfaceClient_PushSurface(blit_client, surface[submit_ptr].handle, NULL, false);
        BDBG_ASSERT(!rc);
        BDBG_MSG(("push %d", submit_ptr));
        if (++submit_ptr == NUM_SURFACES) {
            submit_ptr = 0;
        }
        
        if (FIFO_DEPTH(submit_ptr, recycle_ptr)>1) {
            NEXUS_SurfaceHandle recycledSurface;
            
            rc = BKNI_WaitForEvent(recycledEvent, 2000);
            BDBG_ASSERT(!rc);
            rc = NEXUS_SurfaceClient_RecycleSurface(blit_client, &recycledSurface, 1, &n);
            BDBG_ASSERT(!rc);
            BDBG_ASSERT(n);
            BDBG_ASSERT(recycledSurface == surface[recycle_ptr].handle);
            BDBG_MSG(("recycle %d", recycle_ptr));
            if (++recycle_ptr == NUM_SURFACES) {
                recycle_ptr = 0;
            }
        }
#else
        /* randomization */
        unsigned n;
        
        if (FIFO_DEPTH(submit_ptr, recycle_ptr)<NUM_SURFACES-1) {
            rc = NEXUS_SurfaceClient_PushSurface(blit_client, surface[submit_ptr].handle, NULL, false);
            BDBG_ASSERT(!rc);
            BDBG_MSG(("client%d: push %d", client_id, submit_ptr));
            if (++submit_ptr == NUM_SURFACES) {
                submit_ptr = 0;
            }
        }
        
        if (FIFO_DEPTH(submit_ptr, recycle_ptr)>1) {
            rc = BKNI_WaitForEvent(recycledEvent, 2000);
            BDBG_ASSERT(!rc);
        }
        
        if (rand()%2 || skip_recycle > NUM_SURFACES/2) {
            skip_recycle = 0;
            do {
                NEXUS_SurfaceHandle recycledSurface;
                rc = NEXUS_SurfaceClient_RecycleSurface(blit_client, &recycledSurface, 1, &n);
                BDBG_ASSERT(!rc);
                if (n) {
                    BDBG_ASSERT(recycledSurface == surface[recycle_ptr].handle);
                    BDBG_MSG(("client%d: recycle %d", client_id, recycle_ptr));
                    if (++recycle_ptr == NUM_SURFACES) {
                        recycle_ptr = 0;
                    }
                }
            } while (n);
        }
        else {
            /* build up some recycles, but not too many */
            skip_recycle++;
        }
        
        /* once in a while, have a clear or release/acquire. client must clean up on release. */
        if (rand()%50 == 0) {
            if (rand()%2) {
                NEXUS_SurfaceClient_Clear(blit_client);
            }
            else {
                /* release and reacquire should clear */
                NEXUS_SurfaceClient_Release(blit_client);
                blit_client = NEXUS_SurfaceClient_Acquire(client_id);
                BDBG_ASSERT(blit_client);
                NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
                client_settings.recycled.callback = complete;
                client_settings.recycled.context = recycledEvent;
                rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
                BDBG_ASSERT(!rc);
            }
            /* reset queue. recycle should be clean. */
            recycle_ptr = submit_ptr = 0;
        }
#endif
    }
    NEXUS_SurfaceClient_Release(blit_client);
    
    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(recycledEvent);
    for (i=0;i<NUM_SURFACES;i++) {
        NEXUS_Surface_Destroy(surface[i].handle);
    }
    return NULL;
}
