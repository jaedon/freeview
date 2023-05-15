/***************************************************************************
 *     (c)2011-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_surface_client_video.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 9/14/12 2:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface_compositor/src/nexus_surface_client_video.c $
 * 
 * 10   9/14/12 2:58p erickson
 * SW7420-1992: fix NEXUS_SurfaceClient_ReleaseVideoWindow
 * 
 * 9   8/6/12 1:30p erickson
 * SW7425-3565: add NEXUS_SurfaceComposition.contentMode
 * 
 * 8   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 7   7/16/12 1:01p vsilyaev
 * SW7420-2324: Use atomic verify and acquire for handle verification
 * 
 * 6   7/13/12 10:38a erickson
 * SW7231-890: single single-process warning
 * 
 * 5   7/11/12 3:34p erickson
 * SW7231-890: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 4   4/13/12 11:00a erickson
 * SW7346-748: default state of VideoWindow is visible=true. if no video,
 *  VDC will mute.
 * 
 * 3   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 2   1/10/12 3:38p vsilyaev
 * SW7420-2195: Added clipping for the client surface, currently supported
 *  for video surfaces only
 * 
 * 1   11/11/11 4:43p erickson
 * SW7420-1992: refactor video client code to not require compositor
 *  shutdown
 * 
 **************************************************************************/
#include "nexus_surface_compositor_module.h"
#include "nexus_surface_compositor_impl.h"
#include "priv/nexus_display_priv.h"
#include "priv/nexus_video_window_priv.h"
#include "nexus_class_verification.h" /* allows acquire/release tunneled surface to register with objdb */

BDBG_MODULE(nexus_surface_client_video);

NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_AcquireVideoWindow( NEXUS_SurfaceClientHandle parent_handle, unsigned window_id )
{
    NEXUS_SurfaceClientHandle client;
    int rc;

    BDBG_OBJECT_ASSERT(parent_handle, NEXUS_SurfaceClient);
    if (window_id >= NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS) {
        BDBG_WRN(("video window %d not supported", window_id));
        return NULL;
    }
    if (parent_handle->server->windowClient[window_id]) {
        BDBG_WRN(("video window %d not available", window_id));
        return NULL;
    }
    
    client = BKNI_Malloc(sizeof(*client));
    if (!client) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_SurfaceClient, client);
    client->server = parent_handle->server;
    client->type = NEXUS_SurfaceClient_eVideoWindow;
    client->client_id = window_id;
    client->parent = parent_handle;
    parent_handle->server->windowClient[window_id] = client;

    rc = NEXUS_OBJECT_REGISTER(NEXUS_SurfaceClient, client, Create);
    if (rc) {rc = BERR_TRACE(rc); return NULL;}

    client->acquired = true;
    client->parent = parent_handle;
    BLST_S_INSERT_HEAD(&client->parent->children, client, child_link);
    return client;
}

void NEXUS_SurfaceClient_ReleaseVideoWindow( NEXUS_SurfaceClientHandle client )
{
    NEXUS_OBJECT_UNREGISTER(NEXUS_SurfaceClient, client, Destroy);
    NEXUS_OBJECT_RELEASE(client, NEXUS_SurfaceClient, client);
    return;
}

void NEXUS_SurfaceClient_P_VideoWindowFinalizer( NEXUS_SurfaceClientHandle client )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SurfaceClient, client);
    BDBG_ASSERT(client->type == NEXUS_SurfaceClient_eVideoWindow);
    BDBG_ASSERT(client->server->windowClient[client->client_id] == client);
    client->acquired = false;
    client->server->windowClient[client->client_id] = NULL;
    nexus_surfaceclient_p_resetvideo(client);
    BLST_S_REMOVE(&client->parent->children, client, NEXUS_SurfaceClient, child_link);
    client->parent = NULL;
    
    NEXUS_OBJECT_DESTROY(NEXUS_SurfaceClient, client);
    BKNI_Free(client);
}

void nexus_surfaceclient_p_resetvideo( NEXUS_SurfaceClientHandle client )
{
    unsigned i;
    NEXUS_DisplayUpdateMode updateMode;

    if (!g_NEXUS_SurfaceCompositorModuleSettings.modules.display) return;
    NEXUS_Module_Lock(g_NEXUS_SurfaceCompositorModuleSettings.modules.display);
    /* one BVDC_ApplyChanges for all windows */
    NEXUS_DisplayModule_SetUpdateMode_priv(NEXUS_DisplayUpdateMode_eManual, &updateMode);

    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        NEXUS_VideoWindowSettings *pWindowSettings;
        NEXUS_VideoWindowHandle window;
        int rc;

        window = client->server->settings.display[i].window[client->client_id].window;
        if (!window) continue;

        pWindowSettings = &client->server->settings.display[i].window[client->client_id].settings;

        pWindowSettings->position.x = 0;
        pWindowSettings->position.y = 0;
        pWindowSettings->position.width = client->server->display[i].formatInfo.canvas.width;
        pWindowSettings->position.height = client->server->display[i].formatInfo.canvas.height;
        pWindowSettings->visible = true;
        pWindowSettings->clipRect = pWindowSettings->position;
        pWindowSettings->clipBase = pWindowSettings->position;
        pWindowSettings->contentMode = NEXUS_VideoWindowContentMode_eBox;
        rc = NEXUS_VideoWindow_SetSettings_priv(window, pWindowSettings);
        if (rc) {rc = BERR_TRACE(rc); goto done;}
    }
done:
    NEXUS_DisplayModule_SetUpdateMode_priv(updateMode, NULL);
    NEXUS_Module_Unlock(g_NEXUS_SurfaceCompositorModuleSettings.modules.display);
}

static void nexus_surfaceclient_p_combine_clipping(const NEXUS_Rect *clipBase1, const NEXUS_Rect *clipRect1, const NEXUS_SurfaceRegion *clipBase2, const NEXUS_Rect *clipRect2, NEXUS_Rect *clipRect)
{
    clipRect->width = (clipRect1->width * clipRect2->width) / clipBase2->width;
    clipRect->x = ((clipRect2->x*clipBase1->width) + (clipRect1->x*clipRect2->width))/clipBase2->width;
    clipRect->height = (clipRect1->height * clipRect2->height)/ clipBase2->height;
    clipRect->y = ((clipRect2->y*clipBase1->height) + (clipRect1->y*clipRect2->height))/clipBase2->height;
    BDBG_MSG(("combine_clipping: %u,%u,%u,%u -> %u,%u,%u,%u", clipRect1->x, clipRect1->y, clipRect1->width, clipRect1->height, clipRect->x, clipRect->y, clipRect->width, clipRect->height));
    return;
}

NEXUS_Error nexus_surfaceclient_p_setvideo( NEXUS_SurfaceClientHandle client )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    unsigned i;
    const NEXUS_SurfaceClientSettings *pSettings = &client->settings;
    NEXUS_DisplayUpdateMode updateMode;

    BDBG_ASSERT(client->server);
    if(client->parent==NULL) {
        return NEXUS_SUCCESS;
    }
    if (client->server->pending_update) {
        client->server->pending_setvideo = true;
        /* resolved in nexus_surface_compositor_p_compose */
        return 0;
    }

    if (!g_NEXUS_SurfaceCompositorModuleSettings.modules.display) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    NEXUS_Module_Lock(g_NEXUS_SurfaceCompositorModuleSettings.modules.display);
    /* one BVDC_ApplyChanges for all windows */
    NEXUS_DisplayModule_SetUpdateMode_priv(NEXUS_DisplayUpdateMode_eManual, &updateMode);

    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        NEXUS_VideoWindowSettings *pWindowSettings;
        NEXUS_VideoWindowHandle window;
        bool visible = client->settings.composition.visible && client->parent->serverSettings.composition.visible;
        /* XXX also verify that parent is on the top */

        window = client->server->settings.display[i].window[client->client_id].window;
        if (!window) continue;

        pWindowSettings = &client->server->settings.display[i].window[client->client_id].settings;


        /* unlike graphics, video window's target is actual display coordinates.
        this algorithm will set window position and source clipping.
        two passes are required:
        1) scale and clip for the client
        2) scale and clip for the display
        the source clip must be done after both stages are complete.
        */

        if(visible) {
            NEXUS_Rect windowScreenRect; /* unclipped client rectangle */
            NEXUS_SurfaceRegion displayScale;
            NEXUS_SurfaceRegion clientScale;
            NEXUS_Rect display;
            NEXUS_Rect temp, temp2;

            /* set artificial source coordinates */
            pWindowSettings->clipBase.x = 0;
            pWindowSettings->clipBase.y = 0;
            pWindowSettings->clipBase.width = 720;
            pWindowSettings->clipBase.height = 480;

            /* convert pSettings->composition.position into client's top-level coordiantes */
            clientScale.width = client->parent->serverSettings.composition.position.width;
            clientScale.height = client->parent->serverSettings.composition.position.height;
            nexus_surfacemp_p_convert_coord(
                &client->parent->settings.composition.virtualDisplay, /* from client coordinates */
                &clientScale, /* to the client's top-level position, in virtual display coordinates */
                &pSettings->composition.position,
                &temp
            );

            /* apply origin */
            temp.x += client->parent->serverSettings.composition.position.x;
            temp.y += client->parent->serverSettings.composition.position.y;

            /* clip in client box */
            nexus_surfacemp_p_clip_rect(&client->parent->serverSettings.composition.position, &temp, &pWindowSettings->position);

            displayScale.width = display.width = client->server->display[i].formatInfo.canvas.width;
            displayScale.height = display.height = client->server->display[i].formatInfo.canvas.height;
            display.x = display.y = 0;

            /* scale into the display coordinates */
            nexus_surfacemp_p_convert_coord(&client->parent->serverSettings.composition.virtualDisplay, &displayScale, &temp, &windowScreenRect); /* unclipped window */
            nexus_surfacemp_p_convert_coord(&client->parent->serverSettings.composition.virtualDisplay, &displayScale, &pWindowSettings->position, &temp); /* clipped window */
            nexus_surfacemp_p_clip_rect(&windowScreenRect, &temp, &temp2); /* must clip because rounding could be different for the two scales */
            /* clip into the screen box */
            nexus_surfacemp_p_clip_rect(&display, &temp, &pWindowSettings->position);

            /* find clipped source */
            nexus_surfacemp_scale_clipped_rect(&windowScreenRect, &temp2, &pWindowSettings->clipBase, &pWindowSettings->clipRect);

            if(pSettings->composition.clipRect.x || pSettings->composition.clipRect.y || pSettings->composition.clipRect.width || pSettings->composition.clipRect.height) {
                /* add user requested clipping */
                if(pSettings->composition.virtualDisplay.width && pSettings->composition.virtualDisplay.height) {
                    nexus_surfaceclient_p_combine_clipping(&pWindowSettings->clipBase, &pWindowSettings->clipRect, &pSettings->composition.virtualDisplay, &pSettings->composition.clipRect,  &temp);
                    pWindowSettings->clipRect = temp;
                }
            }


            /* 2x2 is VDC's enforced minimum, but use larger-than-minimum threshold because of BVN bandwidth */
            if(pWindowSettings->position.width < 10 || pWindowSettings->position.height < 10) {
                visible = false;
                pWindowSettings->position = display;
                pWindowSettings->clipRect = pWindowSettings->clipBase;
            }
            BDBG_MSG(("setvideo %d,%d,%d,%d => from:%dx%d to:%dx%d ==> %d,%d,%d,%d (%d,%d,%d,%d)",
                /* specified position */
                pSettings->composition.position.x, pSettings->composition.position.y, pSettings->composition.position.width, pSettings->composition.position.height,
                /* transform */
                client->parent->settings.composition.virtualDisplay.width, client->parent->settings.composition.virtualDisplay.height,
                display.width,
                display.height,
                /* actual position */
                pWindowSettings->position.x, pWindowSettings->position.y, pWindowSettings->position.width, pWindowSettings->position.height,
                pWindowSettings->clipRect.x, pWindowSettings->clipRect.y, pWindowSettings->clipRect.width, pWindowSettings->clipRect.height));


        }

        pWindowSettings->visible = visible;
        pWindowSettings->contentMode = pSettings->composition.contentMode;
        /* TODO: blend equation */

        rc = NEXUS_VideoWindow_SetSettings_priv(window, pWindowSettings);
        if (rc) {rc = BERR_TRACE(rc); goto done;}
    }

done:
    NEXUS_DisplayModule_SetUpdateMode_priv(updateMode, NULL);
    NEXUS_Module_Unlock(g_NEXUS_SurfaceCompositorModuleSettings.modules.display);
    return rc;
}

