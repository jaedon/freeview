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
 * $brcm_Workfile: published_tunneled_client.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/11 12:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/published_tunneled_client.c $
 * 
 * 2   9/7/11 12:55p erickson
 * SW7420-1009: use NEXUS_ANY_ID for NEXUS_Graphics2D_Open
 * 
 * 1   8/11/11 3:31p erickson
 * SW7420-1992: add example
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
#if NEXUS_HAS_SURFACECMP
#include "nexus_surfacecmp_client.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

BDBG_MODULE(blit_tunneled_client);

#if NEXUS_HAS_SURFACECMP
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

int main(int argc, char **argv)
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, displayedEvent;
    int i;
    NEXUS_Error rc;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    NEXUS_SurfaceClientStatus surfaceClientStatus;
    unsigned client_id = 0;
    
    if (argc > 1) {
        client_id = atoi(argv[1]);
    }

    NEXUS_Platform_GetDefaultClientAuthenticationSettings(&authSettings);
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

    blit_client = NEXUS_SurfaceClient_Acquire(client_id);
    if (!blit_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed, client_id"));
        return -1;
    }
    
    /* this app assumes status won't change */
    rc = NEXUS_SurfaceClient_GetStatus(blit_client, &surfaceClientStatus);
    BDBG_ASSERT(!rc);
    
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.displayed.callback = complete;
    client_settings.displayed.context = displayedEvent;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);

    /* fill with black */
    rc = NEXUS_SurfaceClient_GetTunneledSurface(blit_client, &surface);
    BDBG_ASSERT(!rc);
    NEXUS_Surface_GetCreateSettings(surface, &createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    checkpoint(gfx, checkpointEvent);
    
    NEXUS_Surface_GetCreateSettings(surface, &createSettings);
    
    rc = NEXUS_SurfaceClient_PublishSurface(blit_client);
    BDBG_ASSERT(!rc);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings); /* don't get defaults more often than necessary */
    BDBG_WRN(("start"));
    for(i=0;;) {
        unsigned j;
        
        /* if server has only 1 framebuffer, we can make incremental updates in tunnel mode.
        if server is multibuffering, we must update the whole framebuffer in tunnel mode. */
        if (surfaceClientStatus.display[0].numFramebuffers > 1) {
            fillSettings.surface = surface;
            fillSettings.rect.x = 0;
            fillSettings.rect.y = 0;
            fillSettings.rect.width = createSettings.width;
            fillSettings.rect.height = createSettings.height;
            fillSettings.color = 0;
            rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
            BDBG_ASSERT(!rc);
        }
            
        for (j=0;j<1000;j++) {
            fillSettings.surface = surface;
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
        
        /* don't submit. just keep rendering */

        if (i && i%50000==0) {
            /* only have to checkpoint before making accurate count of # of blits */
            checkpoint(gfx, checkpointEvent);
            
            BDBG_WRN(("%d fills completed", i));
        }
    }

    NEXUS_SurfaceClient_Release(blit_client);
    BKNI_DestroyEvent(displayedEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Platform_Uninit();

    return 0;
}
#else
int main(void)
{
    printf("ERROR: you must include surfacecmp.inc in platform_modules.inc\n");
    return -1;
}
#endif
