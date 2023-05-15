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
 * $brcm_Workfile: checkpoint_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 6/23/11 3:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/checkpoint_stress.c $
 * 
 * Hydra_Software_Devel/9   6/23/11 3:47p erickson
 * SW7420-1893: polling checkpoint fixed
 * 
 * Hydra_Software_Devel/8   6/3/11 3:32p erickson
 * SW7420-1893: prevent infinite loop
 * 
 * Hydra_Software_Devel/7   5/27/11 4:49p erickson
 * SW7420-1893: pollingCheckpoint can still fail
 * 
 * Hydra_Software_Devel/6   5/27/11 4:43p erickson
 * SW7346-149: randomize fifo size and threshold for greater stress
 * 
 * Hydra_Software_Devel/5   5/24/11 3:45p erickson
 * SW7420-1893: remove pollingCheckpoint test until it can be fixed
 * 
 * Hydra_Software_Devel/4   5/20/11 2:10p erickson
 * SW7420-1893: add polling checkpoint test
 * 
 * Hydra_Software_Devel/3   8/16/10 1:03p erickson
 * SW7405-3671: packet blit tests
 *
 * Hydra_Software_Devel/2   7/12/10 3:43p erickson
 * SW7405-4508: add visual feedback
 *
 * Hydra_Software_Devel/1   7/6/10 12:41p erickson
 * SW7405-4508: create multi-threaded mutliple blitter checkpoint stress
 * test
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

BDBG_MODULE(checkpoint_stress);

#define MAX_THREADS 16

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

/* this data is setup by the main thread, but used by each blit thread */
struct shared_app_data {
    bool done; /* used to terminate threads cleanly */
};

struct private_app_data {
    unsigned id;
    const struct shared_app_data *shared;
    NEXUS_SurfaceHandle surface;
};

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
    struct private_app_data *app_data = context;
    NEXUS_Graphics2DOpenSettings gfxOpenSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle event;
    unsigned id = app_data->id;
    unsigned cnt = 0, checkpoint_cnt = 0;
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceMemory mem;
    NEXUS_Pixel *ptr;
    bool done = false;

    rc = BKNI_CreateEvent(&event);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultOpenSettings(&gfxOpenSettings);
    gfxOpenSettings.packetFifoSize = 5000 + rand()%50000;
    gfxOpenSettings.packetFifoThreshold = rand() % gfxOpenSettings.packetFifoSize;
    gfx = NEXUS_Graphics2D_Open(0, &gfxOpenSettings);
    BDBG_WRN(("opening blitter(%d): %d %d", id, gfxOpenSettings.packetFifoSize, gfxOpenSettings.packetFifoThreshold));

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.pollingCheckpoint = rand() % 2;
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Surface_GetMemory(app_data->surface, &mem);
    NEXUS_Surface_GetCreateSettings(app_data->surface, &createSettings);
    ptr = mem.buffer;
    /* prepare read from the surface to confirm that the blit before the checkpoint is done.
    test the last pixel in the surface */
    ptr += (createSettings.height-1) * (mem.pitch / sizeof(uint32_t)) + (createSettings.width-1);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    if (rand() % 2) {
        /* some threads blit to a large surface */
        createSettings.width = 1280;
        createSettings.height = 720;
    }
    else {
        /* some to small */
        createSettings.width = 50;
        createSettings.height = 50;
    }
    surface = NEXUS_Surface_Create(&createSettings);
    BDBG_ASSERT(surface);

    /* this loop must exit with a checkpoint, otherwise NEXUS_Surface_Destroy might be called on a surface in process */
    do {
        NEXUS_Graphics2DFillSettings fillSettings;

        /* fill the entire surface */
        NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
        fillSettings.surface = surface;
        fillSettings.color = (rand()%0xFFFFFF) | ((rand()%0xFF)<<24);
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        if (rc) continue;

        /* ensure there is a checkpoint before closing */
        done = app_data->shared->done;

        if (done || rand() % 10 == 0) {
            NEXUS_Pixel pixel;

            fillSettings.surface = app_data->surface;
            rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);

            if (gfxSettings.pollingCheckpoint) {
                unsigned cnt = 0;
                do {
                    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
                    BKNI_Sleep(1);
                    /* catch an infinite loop. the extra *100 is needed because BKNI_Sleep(1) must be < 1 millisecond. */
                    BDBG_ASSERT(++cnt < 100*MAX_THREADS*100);
                } while (rc == NEXUS_GRAPHICS2D_BUSY);
                BDBG_ASSERT(!rc);
            }
            else {
                rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
                if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                    rc = BKNI_WaitForEvent(event, 100*MAX_THREADS);
                    BDBG_ASSERT(!rc); /* cannot timeout in this example */
                }
            }
            checkpoint_cnt++;

            /* flush the RAC before CPU access */
            NEXUS_Surface_Flush(app_data->surface);

            /* read from the surface to confirm that the blit before the checkpoint is done.
            test the last pixel in the surface */
            pixel = *ptr;
            if (pixel != fillSettings.color) {
                BDBG_ERR(("blit_thread(%d) %ux%u filled with %#x, read from memory %08x (%s checkpoint)", id, createSettings.width, createSettings.height, fillSettings.color, pixel,
                    gfxSettings.pollingCheckpoint?"polling":"async"));
                BKNI_Fail();
            }
        }

        ++cnt;
        if (cnt % 500 == 0 || done) {
            BKNI_Printf("blit_thread(%d): %d %ux%u fills, %d checkpoints\n", id, cnt, createSettings.width, createSettings.height, checkpoint_cnt);
        }
    } while (!done);


    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Surface_Destroy(surface);
    BKNI_DestroyEvent(event);
    return NULL;
}

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceMemory mem;
    NEXUS_GraphicsSettings graphicsSettings;
    int i;
    pthread_t thread[MAX_THREADS];
    struct shared_app_data app_data;
    struct private_app_data thread_data[MAX_THREADS];

    NEXUS_Error rc;

    srand(time(NULL));
    
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
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS && !NEXUS_DTV_PLATFORM
    if ( platformConfig.outputs.composite[0] ) {
        NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    }
#endif
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = videoFormatInfo.width;
    createSettings.height = videoFormatInfo.height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    surface = NEXUS_Surface_Create(&createSettings);
    BDBG_ASSERT(surface);
    NEXUS_Surface_GetMemory(surface, &mem);

    BKNI_Memset(&app_data, 0, sizeof(app_data));

    /* fill with black */
    BKNI_Memset(mem.buffer, 0, createSettings.height * mem.pitch);
    NEXUS_Surface_Flush(surface);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    /* graphicsSettings.position will default to the display size */
    graphicsSettings.clip.width = createSettings.width;
    graphicsSettings.clip.height = createSettings.height;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Display_SetGraphicsFramebuffer(display, surface);
    BDBG_ASSERT(!rc);

    for (i=0;i<MAX_THREADS;i++) {
        unsigned distance = videoFormatInfo.height/MAX_THREADS;
        thread_data[i].id=i;
        thread_data[i].shared = &app_data;
        createSettings.width = videoFormatInfo.width;
        createSettings.height = distance/2;
        createSettings.pMemory = (uint8_t *)mem.buffer + i*distance*mem.pitch;
        thread_data[i].surface = NEXUS_Surface_Create(&createSettings);
        BDBG_ASSERT(thread_data[i].surface);
    }
    /* fill random boxes from multiple threads */
    for (i=0;i<MAX_THREADS;i++) {
        rc = pthread_create(&thread[i], NULL, blit_thread, &thread_data[i]);
        BDBG_ASSERT(!rc);
        BKNI_Sleep(500); /* ramp up */
    }

    BKNI_Sleep(120 * 1000); /* run for 2 minutes */

    /* take down all the threads */
    app_data.done = true;
    for (i=0;i<MAX_THREADS;i++) {
        BKNI_Sleep(500); /* ramp down */
        pthread_join(thread[i], NULL);
    }
    for (i=0;i<MAX_THREADS;i++) {
        NEXUS_Surface_Destroy(thread_data[i].surface);
    }

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = false;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Surface_Destroy(surface);

    NEXUS_Platform_Uninit();

    return 0;
}
