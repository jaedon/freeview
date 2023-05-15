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
 * $brcm_Workfile: animation_client.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 11/27/12 1:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/animation_client.c $
 * 
 * 14   11/27/12 1:40p erickson
 * SW7435-421: undo previous change. IPC comes up before nexus join and
 *  comes down after nexus uninit.
 * 
 * 13   11/6/12 10:14a erickson
 * SW7425-1364: improve error messages and client options
 * 
 * 12   10/16/12 4:42p erickson
 * SW7435-421: fix memory leak
 * 
 * 11   10/10/12 2:46p erickson
 * SW7420-1992: fix race condition
 * 
 * 10   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 9   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 * 
 * 8   11/3/11 9:14p vsilyaev
 * SW7420-2119: Better use of NEXUS_SurfaceClient_RecycleSurface
 * 
 * 7   10/5/11 11:49a vsilyaev
 * SW7425-1364: Added support for reference server
 * 
 * 6   9/7/11 12:55p erickson
 * SW7420-1009: use NEXUS_ANY_ID for NEXUS_Graphics2D_Open
 * 
 * 5   8/26/11 5:36p vsilyaev
 * SW7420-1992: Updated API
 * 
 * 4   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 * 
 * 3   8/10/11 3:15p erickson
 * SW7420-1992: allow apps to build without surfacecmp
 * 
 * 2   8/10/11 3:03p erickson
 * SW7420-1123: move NEXUS_Platform_Uninit to nexus_platform_client.h so
 *  clients don't have to #include nexus_platform.h
 * 
 * 1   7/26/11 1:34p erickson
 * SW7420-1992: add surfacecmp module and examples
 * 
 **************************************************************************/
#include "nexus_platform_client.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_memory.h"
#include "nexus_base_mmap.h"
#if NEXUS_HAS_SURFACE_COMPOSITOR
#include "nexus_surface_client.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

BDBG_MODULE(animation_client);

#include "refsw_session_simple_client.h"

#if NEXUS_HAS_SURFACE_COMPOSITOR
static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void checkpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle checkpointEvent)
{
    int rc;
    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
    }
}

#define NUM_SURFACES 50
struct {
    NEXUS_SurfaceHandle handle;
    bool submitted;
} g_surface[NUM_SURFACES];
unsigned submit_ptr, recycle_ptr;

static void recycle_next(NEXUS_SurfaceClientHandle blit_client);

int main(int argc, const char * const argv[])
{
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, recycledEvent;
    int i;
    NEXUS_Error rc;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    int gfx_client_id = -1;
    int curarg = 1;
    
    while (argc > curarg) {
        if (!strcmp(argv[curarg], "-client") && argc>curarg+1) {
            gfx_client_id = atoi(argv[++curarg]);
        }
        curarg++;
    }
    
    rc = simple_client_init(argv[0], &authSettings);
    printf("using certificate:'%s'\n", (char *)authSettings.certificate.data);
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        printf("cannot join: %d\n", rc);
        return -1;
    }

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&packetSpaceAvailableEvent);
    BKNI_CreateEvent(&recycledEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 200;
    createSettings.height = 200;
    for (i=0;i<NUM_SURFACES;i++) {
        g_surface[i].handle = NEXUS_Surface_Create(&createSettings);
        
        /* green background */
        fillSettings.surface = g_surface[i].handle;
        fillSettings.rect.width = 0;
        fillSettings.rect.height = 0;
        fillSettings.color = 0xFF208020;
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
        
        /* gray white box */
        fillSettings.surface = g_surface[i].handle;
        fillSettings.rect.x = i*(createSettings.width - 20)/NUM_SURFACES;
        fillSettings.rect.y = 0;
        fillSettings.rect.width = 20;
        fillSettings.rect.height = createSettings.height;
        fillSettings.color = 0xFF333355;
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
    }
    checkpoint(gfx, checkpointEvent);

    NEXUS_Platform_GetClientConfiguration(&clientConfig);
    if (gfx_client_id == -1 && clientConfig.resources.surfaceClient.total) {
        gfx_client_id = clientConfig.resources.surfaceClient.id[0];
    }
    blit_client = NEXUS_SurfaceClient_Acquire(gfx_client_id);
    if (!blit_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed. run client with '-client X' to change ids.", gfx_client_id));
        return -1;
    }
    
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.recycled.callback = complete;
    client_settings.recycled.context = recycledEvent;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);

    /* push already-rendered surfaces in as fast as possible. allow display vsync to flow control. */
    for(i=0;;) {
        if (g_surface[submit_ptr].submitted) {
            recycle_next(blit_client);
            rc = BKNI_WaitForEvent(recycledEvent, BKNI_INFINITE);
            BDBG_ASSERT(!rc);
            continue;
        }
        
        NEXUS_SurfaceClient_PushSurface(blit_client, g_surface[submit_ptr].handle, NULL, false);
        BDBG_ASSERT(!rc);
        g_surface[submit_ptr].submitted = true;
        if (++submit_ptr == NUM_SURFACES) {
            submit_ptr = 0;
        }
        recycle_next(blit_client);
    }
    
    NEXUS_SurfaceClient_Clear(blit_client);
    for (i=0;i<NUM_SURFACES;i++) {
        NEXUS_Surface_Destroy(g_surface[i].handle);
    } 
    NEXUS_SurfaceClient_Release(blit_client);
    BKNI_DestroyEvent(recycledEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Platform_Uninit();
    simple_client_shutdown();

    return 0;
}

static void recycle_next(NEXUS_SurfaceClientHandle blit_client)
{
    unsigned num;
    do {
        NEXUS_SurfaceHandle surface;
        NEXUS_Error rc;
        rc = NEXUS_SurfaceClient_RecycleSurface(blit_client, &surface, 1, &num);
        BDBG_ASSERT(!rc);
        if (num) {
            /* they should be recycled in the order submitted */
            BDBG_ASSERT(g_surface[recycle_ptr].handle == surface);
            g_surface[recycle_ptr].submitted = false;
            if (++recycle_ptr == NUM_SURFACES) {
                recycle_ptr = 0;
            }
        }
    } while (num);
}

#else
int main(void)
{
    printf("ERROR: you must include surface_compositor.inc in platform_modules.inc\n");
    return -1;
}
#endif
