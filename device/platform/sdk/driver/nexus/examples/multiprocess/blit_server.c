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
 * $brcm_Workfile: blit_server.c $
 * $brcm_Revision: 35 $
 * $brcm_Date: 8/9/12 3:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/blit_server.c $
 * 
 * 35   8/9/12 3:58p erickson
 * SW7425-2734: stop callbacks so we can destroy clients
 * 
 * 34   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 33   6/6/12 1:34p erickson
 * SW7435-27: alloc 8MB untrusted heap from same region as heap[0]
 * 
 * 32   4/26/12 4:34p erickson
 * SW7425-2882: standardize fully mapped heap
 *
 * 31   1/30/12 3:10p rjlewis
 * SW7425-1136: Added HDMI Support.
 *
 * 30   12/22/11 2:47p erickson
 * SW7425-1795: use eFull heap for graphics2d packet fifo or playpump fifo
 *
 * 29   11/18/11 1:24p erickson
 * SW7420-1148: add -timeout for automation
 *
 * 28   11/18/11 10:20a erickson
 * SW7422-445: use
 *  NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE) for client
 *  heap so that server code will work across a variety of platforms
 *
 * 27   11/15/11 10:58a erickson
 * SW7420-2130: shutdown clients cleanly
 *
 * 26   11/2/11 1:04p erickson
 * SW7231-438: standardize cmdline options
 *
 * 25   10/18/11 12:32p vsilyaev
 * SW7420-1992: Moved virtual display into the composition settings
 *
 * 24   9/13/11 1:23p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 *
 * 23   9/7/11 7:12p vsilyaev
 * SW7420-1148: Decouple rendering of primary and other displays. Other
 *  displays would get just a copy of result rendered on primary display.
 *
 * 22   9/7/11 12:55p erickson
 * SW7420-1009: use NEXUS_ANY_ID for NEXUS_Graphics2D_Open
 *
 * 21   9/2/11 9:55a erickson
 * SW7420-1992: run tunneled_client in untrusted mode
 *
 * 20   8/26/11 5:36p vsilyaev
 * SW7420-1992: Updated API
 *
 * 19   8/25/11 2:01p erickson
 * SW7420-1148: add -trust option for tunneled_client
 *
 * 18   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 *
 * 17   8/22/11 4:36p vsilyaev
 * SW7420-1992: Updated implementation of surface compositor
 *
 * 16   8/22/11 2:15p erickson
 * SW7420-1148: run blit_server clients as untrusted clients
 *
 * 15   8/12/11 2:06p erickson
 * SW7420-1992: add hdmi output
 *
 * 14   8/10/11 3:15p erickson
 * SW7420-1992: allow apps to build without surface_compositor
 *
 * 13   8/10/11 9:19a erickson
 * SW7420-1992: only one display for TUNNEL_MODE
 *
 * 12   8/5/11 3:58p erickson
 * SW7420-1992: server can set number of framebuffers per display. revise
 *  tunneling api accordingly.
 *
 * 11   8/5/11 12:05p erickson
 * SW7420-1992: add tunneled mode, remove direct_mode
 *
 * 10   8/4/11 10:41a erickson
 * SW7420-1992: implement direct mode
 *
 * 8   7/26/11 1:33p erickson
 * SW7420-1992: add surface_compositor module and examples
 *
 * 7   3/7/11 12:17p erickson
 * SW7420-1123: rely on memoryType default
 *
 * 6   3/7/11 11:46a erickson
 * SW7420-1123: use [NEXUS_MAX_HEAPS-1] instead of [3]
 *
 * 5   1/21/11 10:50a erickson
 * SW7422-220: mkfifo with mode 0777
 *
 * 4   1/19/11 5:08p erickson
 * SW7422-220: allow heap[] array to be sparse
 *
 * 3   1/19/11 2:03p erickson
 * SW7420-1123: socket-based usermode IPC
 *
 * 2   12/10/10 3:13p erickson
 * SW7420-1148: rename
 *
 * 1   12/7/10 5:47p erickson
 * SW7420-1148: refactor kernel mode proxy for process isolation
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_base_mmap.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#if NEXUS_HAS_SURFACE_COMPOSITOR
#include "nexus_surface_compositor.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

BDBG_MODULE(blit_server);

#if NEXUS_HAS_SURFACE_COMPOSITOR
static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void checkpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle event)
{
    int rc;
    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        BKNI_WaitForEvent(event, 0xffffffff);
    }
}

/*
application options
*/
#define TUNNEL_MODE 1
#define SINGLEBUFFER_MODE 0
#if TUNNEL_MODE
#define TUNNEL_SPLIT 0
#endif

#if SINGLEBUFFER_MODE && TUNNEL_MODE
/* single buffered tunneled mode can only have 1 client */
#define NUM_CLIENTS 1
#else
#define NUM_CLIENTS 4
#endif

static const NEXUS_Rect screen = {0,0,720, 480};
#if TUNNEL_SPLIT
static const NEXUS_Rect sidebar = {480,0,240, 480};
#else
#define sidebar  screen
#endif

struct client_info {
    unsigned width, height;
    int inc_x, inc_y;
    int inc_width, inc_height;
    bool tunnelCapable;
    bool displayCache;
    const NEXUS_Rect *bounds;
    /* TODO: alpha */
} g_client_transform[NUM_CLIENTS] = {
    {
#if TUNNEL_SPLIT
        480, 480,
#else
        720, 480,
#endif
        0, 0, 0, 0, TUNNEL_MODE,  false, &screen}, /* stationary full screen */
#if NUM_CLIENTS > 1
    {100, 100, 1, 1, 0, 0, false, false, &sidebar },
    {150, 150,-1, 2, 3, 3, false, false, &sidebar },
    {250, 250,-3,-2, 0, 0, false, true, &sidebar}  /* display cache test */
#endif
};

/* global data for callbacks */
NEXUS_SurfaceCompositorHandle surface_compositor;
NEXUS_SurfaceClientHandle blit_client[NUM_CLIENTS];

void framebuffer_callback(void *context, int param)
{
    unsigned i;
    NEXUS_SurfaceCompositorClientSettings client_settings;
    NEXUS_Error rc;

    BSTD_UNUSED(context);
    BSTD_UNUSED(param);

    for (i=0;i<NUM_CLIENTS;i++) {
        struct client_info *client = &g_client_transform[i];
        if (!blit_client[i]) continue;

        if (!client->inc_x &&
            !client->inc_y &&
            !client->inc_width &&
            !client->inc_height) continue;

        NEXUS_SurfaceCompositor_GetClientSettings(surface_compositor, blit_client[i], &client_settings);
        client_settings.composition.position.x += client->inc_x;
        client_settings.composition.position.y += client->inc_y;

        /* slide clients so that offscreen coordinates are tested */
        if (client_settings.composition.position.x >= (int)client_settings.composition.virtualDisplay.width) {
            client_settings.composition.position.x = -1 * (int)client_settings.composition.position.width;
        }
        else if (client_settings.composition.position.x < -1 * (int)client_settings.composition.position.width) {
            client_settings.composition.position.x = client_settings.composition.virtualDisplay.width;
        }
        if (client_settings.composition.position.y >= (int)client_settings.composition.virtualDisplay.height) {
            client_settings.composition.position.y = -1 * (int)client_settings.composition.position.height;
        }
        else if (client_settings.composition.position.y < -1 * (int)client_settings.composition.position.height) {
            client_settings.composition.position.y = client_settings.composition.virtualDisplay.height;
        }
#if TUNNEL_SPLIT
        if(client_settings.composition.position.x < client->bounds->x) {
            client_settings.composition.position.x = client->bounds->x;
        }
        if(client_settings.composition.position.y < client->bounds->y) {
            client_settings.composition.position.y = client->bounds->y;
        }
#endif

        /* grow/shrink to test clipping combinations */
        if (client->inc_width) {
            client_settings.composition.position.width += client->inc_width;
            if ((client->inc_width > 0 && client_settings.composition.position.width >= 300) ||
                (client->inc_width < 0 && client_settings.composition.position.width < 50))
            {
                client->inc_width *= -1;
            }
        }
        if (client->inc_height) {
            client_settings.composition.position.height += client->inc_height;
            if ((client->inc_height > 0 && client_settings.composition.position.height >= 300) ||
                (client->inc_height < 0 && client_settings.composition.position.height < 50))
            {
                client->inc_height *= -1;
            }
        }

        rc = NEXUS_SurfaceCompositor_SetClientSettings(surface_compositor, blit_client[i], &client_settings);
        BDBG_ASSERT(!rc);
    }
}

static void print_usage(void)
{
    printf(
    "usage: nexus blit_server [--help|-h] [-unprotected]\n"
    "options:\n"
    "  --help|-h      print this help screen\n"
    "  -unprotected   run clients in unprotected mode\n"
    "  -timeout X     exit after X seconds. default is to prompt for user.\n"
    );
}

int main(int argc, char **argv)
{
    NEXUS_SurfaceHandle desktop;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display[2];
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle event;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformStartServerSettings serverSettings;
    NEXUS_SurfaceCompositorSettings surface_compositor_settings;
    NEXUS_SurfaceClientHandle desktop_client;
    NEXUS_SurfaceCompositorClientSettings client_settings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    NEXUS_Error rc;
    unsigned i;
    int curarg = 1;
    bool unprotected_clients = false;
    unsigned timeout = 0;

    while (curarg < argc) {
        if (!strcmp(argv[curarg], "--help") || !strcmp(argv[curarg], "-h")) {
            print_usage();
            return 0;
        }
        else if (!strcmp(argv[curarg], "-timeout") && argc>curarg+1) {
            timeout = strtoul(argv[++curarg], NULL, 0);
        }
        else if (!strcmp(argv[curarg], "-unprotected")) {
            /* NOTE: unprotected clients cannot crash */
            unprotected_clients = true;
        }
        curarg++;
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    /* use default heaps. for realworld apps, the server app may want to create a special heap
    to isolate untrusted clients. */

    /* create 8MB eFull heap for untrusted clients to do packet blit and playpump. realworld apps may want
    a pool of heaps so untrusted clients don't share. 
    use the memcIndex/subIndex from heap[0] so it's adaptable (that is, steal the 8MB from heap[0]). */
    platformSettings.heap[NEXUS_MAX_HEAPS-1].size = 8 * 1024 * 1024;
    platformSettings.heap[NEXUS_MAX_HEAPS-1].memcIndex = platformSettings.heap[0].memcIndex;
    platformSettings.heap[NEXUS_MAX_HEAPS-1].subIndex = platformSettings.heap[0].subIndex;
    platformSettings.heap[NEXUS_MAX_HEAPS-1].guardBanding = false; /* corruptions shouldn't cause server failures */
    platformSettings.heap[NEXUS_MAX_HEAPS-1].memoryType = NEXUS_MemoryType_eFull; /* requires for packet blit and playpump */
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);
    BKNI_Memset(blit_client, 0, sizeof(blit_client));

    NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
    serverSettings.allowUnauthenticatedClients = true;
    serverSettings.unauthenticatedConfiguration.mode = unprotected_clients ? NEXUS_ClientMode_eUnprotected : NEXUS_ClientMode_eUntrusted;
    serverSettings.unauthenticatedConfiguration.heap[0] = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE); /* default heap for client. VC4 usable, eApplication mapping. */
    serverSettings.unauthenticatedConfiguration.heap[NEXUS_MAX_HEAPS-1] = platformConfig.heap[NEXUS_MAX_HEAPS-1]; /* for packet blit and playpump */
    /* blit_server allows client to use any, but allow blit_client to default to 1 */
    serverSettings.unauthenticatedConfiguration.resources.surfaceClient.id[0] = 1; /* id[0] is default for blit_client */
    serverSettings.unauthenticatedConfiguration.resources.surfaceClient.id[1] = 0;
    serverSettings.unauthenticatedConfiguration.resources.surfaceClient.id[2] = 2;
    serverSettings.unauthenticatedConfiguration.resources.surfaceClient.id[3] = 3;
    serverSettings.unauthenticatedConfiguration.resources.surfaceClient.total = 4;
    rc = NEXUS_Platform_StartServer(&serverSettings);
    BDBG_ASSERT(!rc);

    display[1] = NULL;
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display[0] = NEXUS_Display_Open(0, &displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display[1] = NEXUS_Display_Open(1, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display[0], NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if(display[1]) {
        NEXUS_Display_AddOutput(display[1], NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    }
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display[0], NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format.
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display[0], &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display[0], &displaySettings);
        }
    }
#endif

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    /* create desktop surface for background */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = 480;
    desktop = NEXUS_Surface_Create(&createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.color = 0xFF000000; /* black background */
    fillSettings.surface = desktop;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    checkpoint(gfx, event);

    /* create surface compositor server */
    surface_compositor = NEXUS_SurfaceCompositor_Create(0);
    NEXUS_SurfaceCompositor_GetSettings(surface_compositor, &surface_compositor_settings);
    NEXUS_Display_GetGraphicsSettings(display[0], &surface_compositor_settings.display[0].graphicsSettings);
    surface_compositor_settings.display[0].graphicsSettings.enabled = true;
    surface_compositor_settings.display[0].display = display[0];
#if SINGLEBUFFER_MODE
    surface_compositor_settings.display[0].framebuffer.number = 1;
#elif TUNNEL_MODE
    surface_compositor_settings.display[0].framebuffer.number = 7;
#else
    surface_compositor_settings.display[0].framebuffer.number = 2;
#endif
    surface_compositor_settings.display[0].framebuffer.width = 1280;
    surface_compositor_settings.display[0].framebuffer.height = 720;
    surface_compositor_settings.display[0].framebuffer.backgroundColor = 0xFF000000; /* black background */
    surface_compositor_settings.display[0].framebuffer.heap = NEXUS_Platform_GetFramebufferHeap(0);
    if (display[1]) {
        NEXUS_Display_GetGraphicsSettings(display[1], &surface_compositor_settings.display[1].graphicsSettings);
        surface_compositor_settings.display[1].graphicsSettings.enabled = true;
        surface_compositor_settings.display[1].display = display[1];
        surface_compositor_settings.display[1].framebuffer.width = 720;
        surface_compositor_settings.display[1].framebuffer.height = 480;
        surface_compositor_settings.display[1].framebuffer.backgroundColor = 0xFF000000; /* black background */
        surface_compositor_settings.display[1].framebuffer.heap = NEXUS_Platform_GetFramebufferHeap(1);
    }
    surface_compositor_settings.frameBufferCallback.callback = framebuffer_callback;
    surface_compositor_settings.frameBufferCallback.context = surface_compositor;
    rc = NEXUS_SurfaceCompositor_SetSettings(surface_compositor, &surface_compositor_settings);
    BDBG_ASSERT(!rc);

#if USE_BACKGROUND_SURFACE
    /* create desktop_client for the server */
    desktop_client = NEXUS_SurfaceCompositor_CreateClient(surface_compositor, 100);
    rc = NEXUS_SurfaceClient_SetSurface(desktop_client, desktop);
    BDBG_ASSERT(!rc);
    /* default settings make it fullscreen, zorder=0 */
#else
    BSTD_UNUSED(desktop_client);
#endif

    /* create surface for client app. it will be acquired by the client using id 1. */
    for (i=0;i<NUM_CLIENTS;i++) {
        struct client_info *client = &g_client_transform[i];

        blit_client[i] = NEXUS_SurfaceCompositor_CreateClient(surface_compositor, i);
        NEXUS_SurfaceCompositor_GetClientSettings(surface_compositor, blit_client[i], &client_settings);
        client_settings.composition.position.x = client->bounds->x;
        client_settings.composition.position.y = client->bounds->y;
        client_settings.composition.position.width = client->width;
        client_settings.composition.position.height = client->height;
        client_settings.composition.zorder = i+1; /* above desktop_client */
        client_settings.composition.displayCache = g_client_transform[i].displayCache;
        client_settings.tunnelCapable = client->tunnelCapable;
        client_settings.composition.virtualDisplay.width = 720;
        client_settings.composition.virtualDisplay.height = 480;
        rc = NEXUS_SurfaceCompositor_SetClientSettings(surface_compositor, blit_client[i], &client_settings);
        BDBG_ASSERT(!rc);
    }

    if (!timeout) {
        printf("Press ENTER to shutdown blit_server\n");
        getchar();
    }
    else {
        /* auto close */
        BKNI_Sleep(timeout*1000);
    }

    /* must stop server before destroying clients (which could be acquired by a client) */
    NEXUS_Platform_StopServer();

    NEXUS_StopCallbacks(surface_compositor);
    for (i=0;i<NUM_CLIENTS;i++) {
        NEXUS_SurfaceClientHandle client = blit_client[i];
        blit_client[i] = NULL;
        NEXUS_SurfaceCompositor_DestroyClient(client);
    }
#if USE_BACKGROUND_SURFACE
    NEXUS_SurfaceCompositor_DestroyClient(desktop_client);
#endif
    NEXUS_Surface_Destroy(desktop);
    NEXUS_SurfaceCompositor_Destroy(surface_compositor);

    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(event);
    NEXUS_Display_Close(display[0]);
    if (display[1]) {
        NEXUS_Display_Close(display[1]);
    }
    NEXUS_Platform_Uninit();

    return 0;
}

#else
int main(void)
{
    printf("ERROR: you must include surface_compositor.inc in platform_modules.inc\n");
    return -1;
}
#endif /*NEXUS_HAS_SURFACE_COMPOSITOR*/

