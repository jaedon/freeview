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
 * $brcm_Workfile: multithreaded_fill.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/20/11 5:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/multithreaded_fill.c $
 * 
 * Hydra_Software_Devel/2   12/20/11 5:35p erickson
 * SW7231-540: remove workaround
 * 
 * Hydra_Software_Devel/1   12/13/11 2:56p erickson
 * SW7231-540: add test
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

#if NEXUS_HAS_GRAPHICS2D
/* this data is setup by the main thread, but used by each blit thread */
struct app_data {
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceHandle surface;
    bool done; /* used to terminate threads cleanly */
};

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void rand_rect(unsigned width, unsigned height, NEXUS_Rect *pRect)
{
    pRect->width = 1+(rand()%width);
    pRect->height = 1+(rand()%height);
    if (width-pRect->width) {
        pRect->x = rand() % (width-pRect->width);
    }
    else {
        pRect->x = 0;
    }
    if (height-pRect->height) {
        pRect->y = rand() % (height-pRect->height);
    }
    else {
        pRect->y = 0;
    }
}

/* continually blit from a dedicated thread.
each thread gets its own NEXUS_Graphics2D instance and event */
void *blit_thread(void *context)
{
    NEXUS_Error rc;
    struct app_data *app_data = context;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    BKNI_EventHandle event;
    unsigned id = pthread_self();
    unsigned cnt = 0;
    NEXUS_SurfaceHandle temp;

    rc = BKNI_CreateEvent(&event);
    BDBG_ASSERT(!rc);
    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);

    temp = NEXUS_Surface_Create(&app_data->createSettings);
    fillSettings.surface = temp;
    fillSettings.color = 0;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);

    while (!app_data->done) {

        if (++cnt % 10000 == 0) BKNI_Printf("blit_thread(%#x): %d\n", id, cnt);

        switch (rand() % 2) {
        case 0:
            fillSettings.surface = app_data->surface;
            rand_rect(app_data->createSettings.width, app_data->createSettings.height, &fillSettings.rect);
            fillSettings.color = (rand()%0xFFFFFF) | ((rand()%0xFF)<<24);
            NEXUS_Graphics2D_Fill(gfx, &fillSettings);
            break;
        case 1:
            blitSettings.source.surface = rand()%2?temp:app_data->surface;
            blitSettings.output.surface = rand()%2?temp:app_data->surface;
            rand_rect(app_data->createSettings.width, app_data->createSettings.height, &blitSettings.source.rect);
            rand_rect(app_data->createSettings.width, app_data->createSettings.height, &blitSettings.output.rect);
            /* TODO: temporarily prevent too large downscale */
            if (blitSettings.source.rect.width/12>=blitSettings.output.rect.width) {
                blitSettings.output.rect.width = 0;
            }
            if (blitSettings.source.rect.height/7>=blitSettings.output.rect.height) {
                blitSettings.output.rect.height = 0;
            }
            NEXUS_Graphics2D_Blit(gfx, &blitSettings);
            break;
        }
        if (rand()%10 == 0) {
            rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
            if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                BKNI_WaitForEvent(event, 0xFFFFFFFF);
            }
        }
    }
    NEXUS_Surface_Destroy(temp);
    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(event);
    return NULL;
}
#endif

#include <sys/time.h>
static unsigned b_get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

int main(void)
{
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceHandle surface;
#define MAX_THREADS 4
    pthread_t thread[MAX_THREADS];
    struct app_data app_data[MAX_THREADS];
    NEXUS_Error rc;
    unsigned starttime, timeout = 120;
    unsigned i;

    srand(time(NULL));

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_Memset(&app_data, 0, sizeof(app_data));
    BKNI_Memset(&thread, 0, sizeof(thread));

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = 480;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    surface = NEXUS_Surface_Create(&createSettings);

    /* fill with black. this instance of the blitter is not used in the threads. */
    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.pollingCheckpoint = true;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    do {
        BKNI_Sleep(1);
        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    } while (rc == NEXUS_GRAPHICS2D_BUSY);
    NEXUS_Graphics2D_Close(gfx);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, surface);
    BDBG_ASSERT(!rc);

    starttime = b_get_time();
    while (timeout && b_get_time() - starttime < timeout*1000) {
        unsigned i = rand() % MAX_THREADS;
        switch (rand() % 3) {
        case 0:
            if (!thread[i]) {
                app_data[i].createSettings = createSettings;
                app_data[i].surface = surface;
                app_data[i].done = false;
                rc = pthread_create(&thread[i], NULL, blit_thread, &app_data[i]);
                BDBG_ASSERT(!rc);
            }
            break;
        case 1:
            if (thread[i] && rand()%20==0) {
                app_data[i].done = true;
                pthread_join(thread[i], NULL);
                thread[i] = 0;
            }
            break;
        case 2:
            BKNI_Sleep(rand() % 100);
            break;
        }
    }
    for (i=0;i<MAX_THREADS;i++) {
        if (thread[i]) {
            app_data[i].done = true;
            pthread_join(thread[i], NULL);
            thread[i] = 0;
        }
    }

    /* take down the system */
    NEXUS_Display_Close(display);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}
