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
 * $brcm_Workfile: tunneled_client.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 11/27/12 1:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/tunneled_client.c $
 * 
 * 17   11/27/12 1:40p erickson
 * SW7435-421: undo previous change. IPC comes up before nexus join and
 *  comes down after nexus uninit.
 * 
 * 16   11/6/12 10:14a erickson
 * SW7425-1364: improve error messages and client options
 * 
 * 15   10/16/12 4:42p erickson
 * SW7435-421: fix memory leak
 * 
 * 14   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 13   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 * 
 * 12   11/7/11 10:58a vsilyaev
 * SW7420-2119: Refactored tunnel mode support
 * 
 * 11   10/5/11 11:49a vsilyaev
 * SW7425-1364: Added support for reference server
 * 
 * 10   9/7/11 12:55p erickson
 * SW7420-1009: use NEXUS_ANY_ID for NEXUS_Graphics2D_Open
 * 
 * 9   9/2/11 9:54a erickson
 * SW7420-1992: run tunneled_client in untrusted mode
 * 
 * 8   8/26/11 5:36p vsilyaev
 * SW7420-1992: Updated API
 * 
 * 7   8/25/11 2:01p erickson
 * SW7420-1148: add -trust option for tunneled_client
 * 
 * 6   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 * 
 * 5   8/22/11 4:36p vsilyaev
 * SW7420-1992: Updated implementation of surface compositor
 * 
 * 4   8/10/11 3:15p erickson
 * SW7420-1992: allow apps to build without surfacecmp
 * 
 * 3   8/10/11 3:03p erickson
 * SW7420-1123: move NEXUS_Platform_Uninit to nexus_platform_client.h so
 *  clients don't have to #include nexus_platform.h
 * 
 * 2   8/5/11 3:58p erickson
 * SW7420-1992: server can set number of framebuffers per display. revise
 *  tunneling api accordingly.
 * 
 * 1   8/5/11 12:05p erickson
 * SW7420-1992: add tunneled mode, remove direct_mode
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

BDBG_MODULE(tunneled_client);

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

int main(int argc, const char * const argv[])
{
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_SurfaceHandle surfaces[2];
    bool  busy[2];
    unsigned nsurfaces=2;
    unsigned current;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, displayedEvent;
    unsigned i;
    NEXUS_Error rc;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_SurfaceClientHandle tunnel_client;
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
    BKNI_CreateEvent(&displayedEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Platform_GetClientConfiguration(&clientConfig);
    if (gfx_client_id == -1 && clientConfig.resources.surfaceClient.total) {
        gfx_client_id = clientConfig.resources.surfaceClient.id[0];
    }
    tunnel_client = NEXUS_SurfaceClient_Acquire(gfx_client_id);
    if (!tunnel_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed. run client with '-client X' to change ids.", gfx_client_id));
        return -1;
    }
    
    NEXUS_SurfaceClient_GetSettings(tunnel_client, &client_settings);
    client_settings.displayed.callback = complete;
    client_settings.displayed.context = displayedEvent;
    rc = NEXUS_SurfaceClient_SetSettings(tunnel_client, &client_settings);
    BDBG_ASSERT(!rc);

    /* fill with black */
    for(i=0;i<nsurfaces;i++) {
        busy[i] = false;
        rc = NEXUS_SurfaceClient_AcquireTunneledSurface(tunnel_client, &surfaces[i]);
        if(rc!=NEXUS_SUCCESS) {
            break;
        }
    }
    if(i==0) {
        BDBG_ERR(("not supported number of surfaces %u", i));
        return -1;
    }
    nsurfaces = i;
    current = 0;
    NEXUS_Surface_GetCreateSettings(surfaces[current], &createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surfaces[current];
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    checkpoint(gfx, checkpointEvent);
    rc = NEXUS_SurfaceClient_PushTunneledSurface(tunnel_client, surfaces[current], NULL, false);
    BDBG_ASSERT(!rc);
    rc = BKNI_WaitForEvent(displayedEvent, 5000);
    BDBG_ASSERT(!rc);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings); /* don't get defaults more often than necessary */
    for(i=0;;) {
        unsigned j;
       
        current = (current+1)%nsurfaces;
        if(busy[current] && nsurfaces>1 ) {
            unsigned j;
            for(j=0;j<nsurfaces;j++) {
                if(!busy[j]) {
                    current = j;
                    break;
                }
            }
            if(j>=nsurfaces) {
                for(j=0;j<1000;j++) {
                    NEXUS_SurfaceHandle surface;
                    size_t n;
                    rc = NEXUS_SurfaceClient_RecycleTunneledSurface(tunnel_client, &surface, 1, &n);
                    BDBG_ASSERT(rc==0);
                    if(n==0) {
                        BDBG_ASSERT(j<100);
                        BKNI_Sleep(10);
                        continue;
                    }
                    for(j=0;j<nsurfaces;j++) {
                        if(surfaces[j]==surface) {
                            busy[j] = false;
                            break;
                        }
                    }
                    BDBG_ASSERT(j!=nsurfaces);
                    break;
                }
                continue;
            }
        }
        NEXUS_Surface_GetCreateSettings(surfaces[current], &createSettings); /* we could rely on the cache if we know display size has not changed */
        
        /* if server has only 1 framebuffer, we can make incremental updates in tunnel mode.
        if server is multibuffering, we must update the whole framebuffer in tunnel mode. */
        if(nsurfaces>1) {
            fillSettings.surface = surfaces[current];
            fillSettings.rect.x = 0;
            fillSettings.rect.y = 0;
            fillSettings.rect.width = createSettings.width;
            fillSettings.rect.height = createSettings.height;
            fillSettings.color = 0;
            rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
            BDBG_ASSERT(!rc);
        }
            
        for (j=0;j<1000;j++) {
            fillSettings.surface = surfaces[current];
            fillSettings.rect.x = rand() % (createSettings.width-20);
            fillSettings.rect.y = rand() % (createSettings.height-20);
            fillSettings.rect.width = 20;
            fillSettings.rect.height = 20;
            fillSettings.color = (rand()%0xFFFFFF) | ((i/200)%0xFF<<24);
    
            while (1) {
                rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
                if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    /* blit can fail because an internal queue is full. wait for space to open up, 
                    then resubmit the blit. */
                    BKNI_WaitForEvent(packetSpaceAvailableEvent, BKNI_INFINITE);
                }
                else {
                    break;
                }
            }
            i++;
        }

        /* must do checkpoint. server uses different blitter fifo. */
        checkpoint(gfx, checkpointEvent);

        busy[current] = true;
        rc = NEXUS_SurfaceClient_PushTunneledSurface(tunnel_client, surfaces[current], NULL, false);
        BDBG_ASSERT(!rc);
        
        rc = BKNI_WaitForEvent(displayedEvent, 5000);
        BDBG_ASSERT(!rc);
        
        if (i && i%50000==0) {
            BDBG_WRN(("%u fills completed", i));
        }
    }
    for(i=0;i<nsurfaces;i++) {
        NEXUS_SurfaceClient_ReleaseTunneledSurface(tunnel_client, surfaces[i]);
    }

    NEXUS_SurfaceClient_Release(tunnel_client);
    BKNI_DestroyEvent(displayedEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Platform_Uninit();
    simple_client_shutdown();

    return 0;
}
#else
int main(void)
{
    printf("ERROR: you must include surface_compositor.inc in platform_modules.inc\n");
    return -1;
}
#endif
