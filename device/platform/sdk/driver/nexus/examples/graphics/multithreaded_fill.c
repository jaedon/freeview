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
 * $brcm_Workfile: multithreaded_fill.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 1/25/12 5:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/graphics/multithreaded_fill.c $
 * 
 * 11   1/25/12 5:22p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 10   12/13/11 3:00p erickson
 * SW7231-536: add outputs
 * 
 * 9   12/8/11 5:15p erickson
 * SW7405-3671: add required checkpoint
 * 
 * 8   9/7/11 12:50p erickson
 * SW7420-1009: use NEXUS_ANY_ID
 * 
 * 7   11/29/10 2:57p erickson
 * SW7420-1078: add NEXUS_Platform_GetFramebufferHeap
 * 
 * 6   8/6/10 1:11p erickson
 * SW7420-703: remove unnecessary BCHP_CHIP and SCART logic
 *
 * 5   6/18/10 4:19p erickson
 * SW7405-4508: rework example so each thread uses its own instance of
 *  NEXUS_Graphics2D
 *
 * 4   2/24/10 9:43p mphillip
 * SW7550-277: Guard graphics examples for platforms lacking graphics2d
 *
 * 3   11/10/09 3:20p nickh
 * SW7420-166: Explicitly pass in heap1 for allocated surfaces on 7420 as
 *  nexus defaults to heap0.
 *
 * 2   5/13/09 9:24a erickson
 * PR54982: fix range error on fill
 *
 * 1   5/11/09 11:46a erickson
 * PR54954: added multithreaded_fill
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
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
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
    BKNI_EventHandle event;
    unsigned id = pthread_self();
    unsigned cnt = 0;

    rc = BKNI_CreateEvent(&event);
    BDBG_ASSERT(!rc);
    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    while (!app_data->done) {
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

        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        if (rc) continue;

        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            BKNI_WaitForEvent(event, 0xFFFFFFFF);
        }
    }
    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(event);
    return NULL;
}
#endif

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
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
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
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
		}
    }
#endif

    NEXUS_Surface_GetDefaultCreateSettings(&app_data.createSettings);
    app_data.createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    app_data.createSettings.width = 720;
    app_data.createSettings.height = 480;
    app_data.createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    app_data.surface = NEXUS_Surface_Create(&app_data.createSettings);

    /* fill with black. this instance of the blitter is not used in the threads. */
    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.pollingCheckpoint = true;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = app_data.surface;
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
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, app_data.surface);
    BDBG_ASSERT(!rc);

    /* fill random boxes from multiple threads*/
    for (i=0;i<MAX_THREADS;i++) {
        rc = pthread_create(&thread[i], NULL, blit_thread, &app_data);
        BDBG_ASSERT(!rc);
    }

    printf("press ENTER to quit\n");
    getchar();

    /* take down all the threads */
    app_data.done = true;
    for (i=0;i<MAX_THREADS;i++) {
        pthread_join(thread[i], NULL);
    }

    /* take down the system */
    NEXUS_Display_Close(display);
    NEXUS_Surface_Destroy(app_data.surface);
    NEXUS_Platform_Uninit();
#else
	printf("This application is not supported on this platform!\n");
#endif

    return 0;
}
