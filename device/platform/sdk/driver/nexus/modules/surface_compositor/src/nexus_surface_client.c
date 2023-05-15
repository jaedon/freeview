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
 * $brcm_Workfile: nexus_surface_client.c $
 * $brcm_Revision: 77 $
 * $brcm_Date: 10/25/12 12:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface_compositor/src/nexus_surface_client.c $
 * 
 * 77   10/25/12 12:05p erickson
 * SW7435-459: clear callbacks on release
 * 
 * 76   10/23/12 2:34p erickson
 * SW7425-4068: fix right buffer calculation
 * 
 * 75   10/22/12 10:02a erickson
 * SW7435-429: add counts to proc
 * 
 * 74   10/17/12 5:13p erickson
 * SW7435-416: fix warning
 * 
 * 73   10/17/12 10:30a erickson
 * SW7435-416: surfaces are already registered
 * 
 * 72   9/17/12 3:05p erickson
 * SW7435-344: remove manual NEXUS_OBJECT_REGISTER/UNREGISTER
 * 
 * 71   9/17/12 2:19p erickson
 * SW7420-1992: default surface client to full screen
 * 
 * 70   9/14/12 2:58p erickson
 * SW7420-1992: fix NEXUS_SurfaceClient_ReleaseVideoWindow
 * 
 * 69   9/6/12 9:43a vsilyaev
 * SW7552-338: Fixed typo. Take 2
 * 
 * 68   9/5/12 3:26p vsilyaev
 * SW7552-338: Fixed typo
 * 
 * 67   8/16/12 2:24p erickson
 * SW7425-3595: support GetStatus for video window clients
 * 
 * 66   8/14/12 10:41a vsilyaev
 * SW7435-301: Added surface reference counting for push/recycle clients
 * 
 * 65   8/9/12 10:15a vsilyaev
 * SW7435-296: Mark proper surface for 'publish' in the set/update client
 * 
 * 64   8/8/12 4:58p vsilyaev
 * SW7435-296: Simplified handling of 'published' clients
 * 
 * 63   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 62   7/31/12 1:38p vsilyaev
 * SW7425-3656,SW7425-3428: Fixed bounds checking
 * 
 * 61   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 60   7/13/12 11:27a vsilyaev
 * SW7425-3428: Verify that update region in inside the surface
 * 
 * 59   7/11/12 3:34p erickson
 * SW7231-890: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 58   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 57   4/17/12 3:38p vsilyaev
 * SW7425-2874: Added more tests to verify invariants, clear
 *  tunnel.visible when last surface removed from the emulated tunnel
 *  client
 * 
 * 56   3/7/12 12:39p erickson
 * SW7231-693: default hidden flags to true on nexus_p_surface_client_init
 * 
 * 55   1/10/12 3:38p vsilyaev
 * SW7420-2195: Added clipping for the client surface, currently supported
 *  for video surfaces only
 * 
 * 54   1/6/12 12:57p vsilyaev
 * SW7420-2209: Calclulate and verify CRC also for 'real' tunneled clients
 * 
 * 53   1/5/12 5:55p vsilyaev
 * SW7420-2209: Added profiling traces and uses sparse CRC to reduce CPU
 *  load
 * 
 * 52   1/5/12 3:31p vsilyaev
 * SW7420-2209: Added option that would calculate and compare CRC when
 *  surfaces goes through the compositor pipe-line
 * 
 * 51   12/15/11 3:01p vsilyaev
 * SW7420-1992: Use reference counting to control lifecycle of the surface
 *  passed by the client
 * 
 * 50   11/23/11 2:55p vsilyaev
 * SW7420-2148: Reworked queue management for the push type clients
 * 
 * 49   11/22/11 1:39p vsilyaev
 * SW7420-2148: Use faster path for Set/Update clients, better handle out-
 *  of-phase displayed callback
 * 
 * 48   11/17/11 3:39p erickson
 * SW7420-1992: protect call to check_inactive
 * 
 * 47   11/17/11 1:13p vsilyaev
 * SW7420-2119: Fixed handling of Push/Recycle method with just two
 *  surfaces
 * 
 * 46   11/14/11 12:18p erickson
 * SW7125-1154: fix coverity warning caused by thunk
 * 
 * 45   11/11/11 4:43p erickson
 * SW7420-1992: refactor video client code to not require compositor
 *  shutdown
 * 
 * 44   11/10/11 4:56p erickson
 * SW7420-1992: change videowindow array to
 *  NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS to support mosaic mode
 * 
 * 43   11/7/11 2:34p erickson
 * SW7420-1992: don't make surfacecmp inactive until tunneled surfaces are
 *  released
 *
 * 42   11/7/11 10:58a vsilyaev
 * SW7420-2119: Refactored tunnel mode support
 *
 * 41   11/3/11 9:17p vsilyaev
 * SW7420-2119: Removed unnecessary state
 *
 * 40   11/3/11 9:15p vsilyaev
 * SW7420-2119: Improved handling of 'real' tunnel clients
 *
 * 39   10/28/11 10:55a erickson
 * SW7405-5520: backout workaround
 *
 * 38   10/24/11 5:43p vsilyaev
 * SW7420-1992: Updated cursor support
 *
 * 37   10/21/11 9:23a erickson
 * SW7420-1992: set NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE on first set/push
 *
 * 36   10/20/11 3:35p erickson
 * SW7420-1992: don't call NEXUS_VideoWindow_GetSettings_priv
 *
 * 35   10/20/11 2:13p vsilyaev
 * SW7420-1992: Added 3D compositing
 *
 * 34   10/20/11 1:56p erickson
 * SW7420-1992: embed NEXUS_VideoWindowSettings in
 *  NEXUS_SurfaceCompositorDisplaySettings
 *
 * 33   10/20/11 9:22a erickson
 * SW7405-5520: apply work-around for SD display full-screen flash
 *
 * 32   10/19/11 4:44p erickson
 * SW7420-1992: fix rounding error for video window
 *
 * 31   10/18/11 6:07p vsilyaev
 * SW7420-1992: Use double linked queue for the client surfaces and trim
 *  masked surfaces
 *
 * 30   10/18/11 12:33p vsilyaev
 * SW7420-1992: More work on separation of backend and frontend
 *
 * 29   10/13/11 4:54p vsilyaev
 * SW7420-1992: Fixed shutdown sequemce for video window
 *
 * 28   10/13/11 3:33p erickson
 * SW7420-1992: add display format change support
 *
 * 27   10/12/11 12:49p vsilyaev
 * SW7420-1992: Moved handling of client side changes to single function
 *
 * 26   10/10/11 10:07a vsilyaev
 * SW7420-1992,SW7420-2085: Use reference counting to extend life cycle of
 *  surfaces
 *
 * 25   10/6/11 7:37p vsilyaev
 * SW7420-1992: Improved management of tunnel client
 *
 * 24   10/5/11 2:30p erickson
 * SW7420-1992: reset video window to full screen and invisible on
 *  release, use client->client_id as window id in setvideo().
 *
 * 23   10/5/11 1:59p erickson
 * SW7420-1992: fix warning
 *
 * 22   10/5/11 1:51p erickson
 * SW7420-1148: add explicit register/unregister
 *
 * 21   10/5/11 1:05p erickson
 * SW7420-1992: improve debug
 *
 * 20   10/5/11 12:40p erickson
 * SW7420-1148: add explicit register/unregister when server
 *  creates/destroys clients
 *
 * 19   10/5/11 10:44a erickson
 * SW7420-1992: move calcblit into compose to avoid race conditions, check
 *  if source is clipped to nothing
 *
 * 18   10/4/11 5:52p vsilyaev
 * SW7425-1364: Added ClientSurfaceId
 *
 * 17   10/4/11 4:38p erickson
 * SW7420-1992: fix non-tunneled clients that are tunnel-capable, release
 *  tunneled surfaces, recalc video from server-side changes, reduce churn
 *  if invisible clients are reconfigured, fix background repaint if
 *  tunneled client is destroyed
 *
 * 16   10/3/11 9:51a erickson
 * SW7420-1148: minimum window size for all displays is 2x2
 *
 * 15   10/3/11 9:25a erickson
 * SW7420-1148: add video window support
 *
 * 14   9/7/11 7:13p vsilyaev
 * SW7420-1148: Decouple rendering of primary and other displays. Other
 *  displays would get just a copy of result rendered on primary display.
 *
 * 13   9/2/11 12:40p erickson
 * SW7420-1148: rename "get client" function
 *
 * 12   9/2/11 10:03a erickson
 * SW7420-1148: allow nexus_register/unregister to objdb from nexus
 *  module. enables more complex resource management.
 *
 * 11   9/1/11 2:07p vsilyaev
 * SW7420-1992: Updated API
 *
 * 10   8/31/11 3:56p erickson
 * SW7420-1992: allow more than one surface compositor
 *
 * 9   8/26/11 5:36p vsilyaev
 * SW7420-1992: Updated API
 *
 * 8   8/25/11 2:01p erickson
 * SW7420-1992: remove false ERR
 *
 * 7   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 *
 * 6   8/22/11 4:11p vsilyaev
 * SW7420-1992: Updated implementation of surface compositor
 *
 * 5   8/11/11 3:31p erickson
 * SW7420-1992: update
 *
 * 4   8/10/11 9:20a erickson
 * SW7420-1992: clean server->tunnel.client on destroy
 *
 * 3   8/5/11 3:58p erickson
 * SW7420-1992: server can set number of framebuffers per display. revise
 *  tunneling api accordingly.
 *
 * 2   8/5/11 12:05p erickson
 * SW7420-1992: add tunneled mode, remove direct_mode
 *
 * 1   7/26/11 1:34p erickson
 * SW7420-1992: add surface_compositor module and examples
 *
 **************************************************************************/
#include "nexus_surface_compositor_module.h"
#include "nexus_surface_compositor_impl.h"
#include "priv/nexus_core.h"
#include "priv/nexus_display_priv.h"
#include "nexus_class_verification.h" /* allows acquire/release tunneled surface to register with objdb */
#include "priv/nexus_surface_priv.h"
#include "nexus_client_resources.h"

BDBG_MODULE(nexus_surface_client);

#define BDBG_MSG_TRACE(X) BDBG_MSG(X)
static const NEXUS_BlendEquation NEXUS_SurfaceCompositor_P_ColorCopySource = {
        NEXUS_BlendFactor_eSourceColor, NEXUS_BlendFactor_eOne, false, NEXUS_BlendFactor_eZero, NEXUS_BlendFactor_eZero, false, NEXUS_BlendFactor_eZero
};

static const NEXUS_BlendEquation NEXUS_SurfaceCompositor_P_AlphaCopySource = {
        NEXUS_BlendFactor_eSourceAlpha, NEXUS_BlendFactor_eOne, false, NEXUS_BlendFactor_eZero, NEXUS_BlendFactor_eZero, false, NEXUS_BlendFactor_eZero
};

void nexus_p_surface_composition_init(NEXUS_SurfaceComposition *composition)
{
    BKNI_Memset(composition, 0, sizeof(*composition));
    composition->visible = false;
    composition->position.x = 0;
    composition->position.y = 0;
    composition->position.width = 1920;
    composition->position.height = 1080;
    composition->clipRect.x = 0;
    composition->clipRect.y = 0;
    composition->clipRect.width = 0;
    composition->clipRect.height = 0;
    composition->displayCache = false;
    composition->visible = true;
    composition->constantColor = 0xFF000000;
    composition->colorBlend = NEXUS_SurfaceCompositor_P_ColorCopySource;
    composition->alphaBlend = NEXUS_SurfaceCompositor_P_AlphaCopySource;
    composition->virtualDisplay.width = 1920;
    composition->virtualDisplay.height = 1080;
    composition->rightViewOffset = 0;
    return;
}

static void nexus_p_surface_client_init(NEXUS_SurfaceClientHandle client)
{
    unsigned i;

    BLST_SQ_INIT(&client->queue.push);
    BLST_SQ_INIT(&client->queue.recycle);
    BLST_SQ_INIT(&client->queue.free);
    client->queue.compositing = false;
    client->queue.last = false;
    client->state.client_type = client_type_idle;
    client->state.update_flags = 0;
    NEXUS_SURFACECLIENT_P_SURFACE_INIT(&client->state.current);
    client->state.left.visible = false;
    client->state.left.hidden = true;
    client->state.right.visible = false;
    client->state.right.hidden = true;
    client->set.dirty = false;
    client->set.updating = false;
    client->pending_displayed_callback = false;
    client->process_pending_displayed_callback = false;
    for(i=0;i<NEXUS_P_SURFACECMP_MAX_PUSH_SURFACES;i++) {
        NEXUS_SurfaceCompositor_P_PushElement *node = client->queue.elements + i;
        BLST_SQ_INSERT_TAIL(&client->queue.free, node, link);
    }
    client->settings.orientation = NEXUS_VideoOrientation_e2D;
    nexus_p_surface_composition_init(&client->settings.composition);

    return ;
}

NEXUS_SurfaceClientHandle NEXUS_SurfaceCompositor_CreateClient( NEXUS_SurfaceCompositorHandle server, NEXUS_SurfaceCompositorClientId client_id )
{
    NEXUS_SurfaceClientHandle client;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);

    BDBG_MSG(("create client %d", client_id));
    client = BKNI_Malloc(sizeof(*client));
    if (!client) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_SurfaceClient, client);
    client->server = server;
    BLST_Q_INSERT_HEAD(&client->server->clients, client, link); /* defaults to zorder=0, so goes to head */
    client->client_id = client_id;
    client->type = NEXUS_SurfaceClient_eTopLevel;
    nexus_p_surface_composition_init(&client->serverSettings.composition);
    nexus_p_surface_client_init(client);

    client->displayedCallback = NEXUS_TaskCallback_Create(client, NULL);
    if (!client->displayedCallback) {
        goto error;
    }
    client->recycledCallback = NEXUS_TaskCallback_Create(client, NULL);
    if (!client->recycledCallback) {
        goto error;
    }
    client->displayStatusChangedCallback = NEXUS_TaskCallback_Create(client, NULL);
    if (!client->displayStatusChangedCallback) {
        goto error;
    }

    rc = nexus_surface_compositor_p_verify_tunnel(server);
    if (rc) {
        rc = BERR_TRACE(rc);
        goto error;
    }

    nexus_p_surface_compositor_update_client(client, NEXUS_P_SURFACECLIENT_UPDATE_CLIENT);
    return client;

error:
    NEXUS_SurfaceCompositor_DestroyClient(client);
    return NULL;
}

static void NEXUS_SurfaceClient_P_Release( NEXUS_SurfaceClientHandle client );
static void NEXUS_SurfaceClient_P_Finalizer( NEXUS_SurfaceClientHandle client );

static void NEXUS_SurfaceClient_P_ParentRelease(NEXUS_SurfaceClientHandle client)
{
    NEXUS_OBJECT_ASSERT(NEXUS_SurfaceClient, client);
    return;
}

static void NEXUS_SurfaceClient_P_ParentFinalizer( NEXUS_SurfaceClientHandle client )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SurfaceClient, client);
    
    NEXUS_SurfaceClient_Clear(client);

    BLST_Q_REMOVE(&client->server->clients, client, link);

    if (client->displayedCallback) {
        NEXUS_TaskCallback_Destroy(client->displayedCallback);
    }
    if (client->recycledCallback) {
        NEXUS_TaskCallback_Destroy(client->recycledCallback);
    }
    if (client->displayStatusChangedCallback) {
        NEXUS_TaskCallback_Destroy(client->displayStatusChangedCallback);
    }

    NEXUS_OBJECT_DESTROY(NEXUS_SurfaceClient, client);
    BKNI_Free(client);
    return;
}

static void NEXUS_SurfaceClient_P_Release( NEXUS_SurfaceClientHandle client )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SurfaceClient, client);
    switch (client->type) {
    case NEXUS_SurfaceClient_eVideoWindow:
    case NEXUS_SurfaceClient_eChild:
        /* no release needed. public Release API is an internal destroy. */
        break;
    default:
        NEXUS_SurfaceClient_P_ParentRelease(client);
        break;
    }
}
    
static void NEXUS_SurfaceClient_P_Finalizer( NEXUS_SurfaceClientHandle client )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SurfaceClient, client);
    switch (client->type) {
    case NEXUS_SurfaceClient_eVideoWindow:
        NEXUS_SurfaceClient_P_VideoWindowFinalizer(client);
        break;
    /* TODO: case NEXUS_SurfaceClient_eChild: */    
    default:    
        NEXUS_SurfaceClient_P_ParentFinalizer( client);
        break;
    }
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_SurfaceClient, NEXUS_SurfaceCompositor_DestroyClient);

NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_Acquire( NEXUS_SurfaceCompositorClientId client_id )
{
    NEXUS_SurfaceCompositorHandle server;
    int rc;
    
    rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(surfaceClient,IdList,client_id);
    if (rc) { rc = BERR_TRACE(rc); return NULL; }
    
    for (server = BLST_S_FIRST(&g_nexus_surfacecompositor_list); server; server = BLST_S_NEXT(server, link)) {
        NEXUS_SurfaceClientHandle client;
        BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);
        for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {
            BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
            if (client->client_id == client_id) {
                if (!client->acquired) {
                    nexus_p_surface_compositor_update_client(client, NEXUS_P_SURFACECLIENT_UPDATE_CLIENT);
                    client->acquired = true;
                    return client;
                }
                else {
                    BDBG_ERR(("client_id %d already acquired", client_id));
                    BERR_TRACE(NEXUS_NOT_AVAILABLE);
                    break;
                }
            }
        }
    }
    
    BDBG_ERR(("client_id %d not created", client_id));
    NEXUS_CLIENT_RESOURCES_RELEASE(surfaceClient,IdList,client_id);
    BERR_TRACE(NEXUS_NOT_AVAILABLE);
    return NULL;
}

void NEXUS_SurfaceClient_Release( NEXUS_SurfaceClientHandle client )
{
    BDBG_MSG_TRACE(("NEXUS_SurfaceClient_Release:%p", client));
    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    NEXUS_TaskCallback_Set(client->displayedCallback, NULL);
    NEXUS_TaskCallback_Set(client->recycledCallback, NULL);
    NEXUS_TaskCallback_Set(client->displayStatusChangedCallback, NULL);
    if (client->type == NEXUS_SurfaceClient_eChild) {
        NEXUS_SurfaceClient_DestroyChild(client);
        return;
    }
    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        NEXUS_SurfaceClient_ReleaseVideoWindow(client);
        return;
    }
    client->acquired = false;
    NEXUS_CLIENT_RESOURCES_RELEASE(surfaceClient,IdList,client->client_id);
    NEXUS_SurfaceClient_Clear(client);
    nexus_p_surface_client_init(client);
    return;
}

NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_CreateChild( NEXUS_SurfaceClientHandle parent_handle )
{
    NEXUS_SurfaceClientHandle client;
    BDBG_OBJECT_ASSERT(parent_handle, NEXUS_SurfaceClient);
    if (parent_handle->type != NEXUS_SurfaceClient_eTopLevel) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    client = NULL; /* TODO */
    return client;
}

void NEXUS_SurfaceClient_DestroyChild( NEXUS_SurfaceClientHandle client )
{
    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    if (client->type != NEXUS_SurfaceClient_eChild) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
}

#if NEXUS_SURFACE_COMPOSITOR_P_CHECK_IMMUTABLE 
/* use spares CRC to reduce computation complexity */
/* do rcr on 32 bytes and then skip X_STEP*32 bytes */
#define NEXUS_SURFACE_COMPOSITOR_P_CRC_X_STEP       8
/* do crc on every Y_STEP line */
#define NEXUS_SURFACE_COMPOSITOR_P_CRC_Y_STEP       8

BTRC_MODULE(surface_compositor_crc,ENABLE);

BSTD_INLINE unsigned
b_crc_update_uint8(unsigned seed, unsigned data)
{
    unsigned temp;
    unsigned result;

    /* x16 + x12 + x5 + 1 (CRC-16-CCITT) */
    temp = data ^ (seed >> 8);
    temp ^= (temp >> 4);
    result = (seed << 8) ^ temp ^ (temp << 5) ^ (temp << 12);
    return result&0xFFFF;
}

BSTD_INLINE unsigned
b_crc_update_uint16(unsigned seed, unsigned data)
{
    seed = b_crc_update_uint8(seed, data>>8);
    seed = b_crc_update_uint8(seed, data&0xFF);
    return seed;
}

BSTD_INLINE unsigned
b_crc_update_uint32(unsigned seed, unsigned data)
{
    seed = b_crc_update_uint16(seed, data>>16);
    seed = b_crc_update_uint16(seed, data&0xFFFF);
    return seed;
}

static unsigned NEXUS_SurfaceCompositor_P_SurfaceCrc(const NEXUS_Surface_P_ClientSurface *clientSurface,  NEXUS_SurfaceCreateSettings *createSettings)
{
    unsigned y;
    unsigned seed = 0xFFFF;
    unsigned xsteps;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceHandle surface = clientSurface->surface;

    BTRC_TRACE(surface_compositor_crc, START);

    NEXUS_Surface_GetMemory(surface, &mem);
    NEXUS_Surface_Flush(surface); 
    xsteps = (clientSurface->bpp*createSettings->width+4*(1-NEXUS_SURFACE_COMPOSITOR_P_CRC_X_STEP))/(4*NEXUS_SURFACE_COMPOSITOR_P_CRC_X_STEP*8*sizeof(NEXUS_Pixel));
    for(y=0;y<createSettings->height;y+=NEXUS_SURFACE_COMPOSITOR_P_CRC_Y_STEP) {
        unsigned x;
        const NEXUS_Pixel *pixels = (NEXUS_Pixel *)((uint8_t *)mem.buffer + y*mem.pitch);

        for(x=0;x<xsteps;x++,pixels+=NEXUS_SURFACE_COMPOSITOR_P_CRC_X_STEP*4) { 
            /* unroll loop to process one cache line at a time */ 
            seed = b_crc_update_uint32(seed, pixels[0]);
            seed = b_crc_update_uint32(seed, pixels[1]);
            seed = b_crc_update_uint32(seed, pixels[2]);
            seed = b_crc_update_uint32(seed, pixels[3]);
        }
    }
    BDBG_MSG_TRACE(("surface %p(%p) %ux%u crc %#x", clientSurface->surface, mem.buffer, createSettings->width, createSettings->height, seed));
    BTRC_TRACE(surface_compositor_crc, STOP);
    return seed;
}

static void NEXUS_SURFACECLIENT_P_ASSIGN_SURFACE(NEXUS_SurfaceClientHandle client, NEXUS_Surface_P_ClientSurface *clientSurface, NEXUS_SurfaceHandle appSurface) 
{
    const NEXUS_PixelFormatInfo *pixelInfo;
    NEXUS_SurfaceCreateSettings createSettings;
    BSTD_UNUSED(client);

    clientSurface->surface = appSurface;
    clientSurface->verify = true;
    NEXUS_Surface_GetCreateSettings(appSurface, &createSettings);
    pixelInfo = NEXUS_PixelFormat_GetInfo(createSettings.pixelFormat);
    BDBG_ASSERT(pixelInfo);
    clientSurface->bpp = pixelInfo->bpp;
    clientSurface->crc = NEXUS_SurfaceCompositor_P_SurfaceCrc(clientSurface, &createSettings);
    BDBG_MSG_TRACE(("NEXUS_SURFACECLIENT_P_ASSIGN_SURFACE client (%p,%u) surface:%p crc:%#x", client, client->client_id, clientSurface->surface, clientSurface->crc));
    return;
}

void nexus_surfaceclient_p_verify_surface(NEXUS_SurfaceClientHandle client, const NEXUS_Surface_P_ClientSurface *clientSurface, const char *filename, unsigned lineno)
{
    BSTD_UNUSED(client);
    if(clientSurface->verify && clientSurface->surface) {
        NEXUS_SurfaceCreateSettings createSettings;
        unsigned crc;

        NEXUS_Surface_GetCreateSettings(clientSurface->surface, &createSettings);
        crc = NEXUS_SurfaceCompositor_P_SurfaceCrc(clientSurface, &createSettings);
        if(crc != clientSurface->crc) {
            BDBG_WRN(("NEXUS_SURFACECLIENT_P_SURFACE_VERIFY failed, client (%p,%u) surface:%p crc:%#x,%#x %s:%u", client, client->client_id, clientSurface->surface, crc, clientSurface->crc, filename, lineno));
        }
    }
    return;
}

#else
#define NEXUS_SURFACECLIENT_P_ASSIGN_SURFACE(client, clientSurface, appSurface) (clientSurface)->surface = appSurface
#endif

NEXUS_Error NEXUS_SurfaceClient_SetSurface( NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle surface )
{
    NEXUS_SurfaceCreateSettings createSettings;
    unsigned flags = NEXUS_P_SURFACECLIENT_UPDATE_SOURCE;

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    BDBG_ASSERT(surface);

    BDBG_MSG_TRACE(("SetSurface:%#p %u %p", client, client->client_id, surface));
    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if(client->state.client_type != client_type_set && client->state.client_type != client_type_idle) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
     /* if the previous blit is not complete (if client->set.serverSurface is being read by M2MC HW), we need to fail this call.
    app must wait for recycle event. */
    if (client->set.dirty) {
        BDBG_ERR(("app must wait for recycled callback to avoid tearing. NEXUS_SurfaceClient_SetSurface rejected."));
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    NEXUS_Surface_GetCreateSettings(surface, &createSettings);
    client->state.clientRegion.width = createSettings.width;
    client->state.clientRegion.height = createSettings.height;

    /* if different, destroy and recreate */
    if (client->state.client_type == client_type_set && BKNI_Memcmp(&client->set.serverCreateSettings, &createSettings, sizeof(createSettings)!=0)) {
        BDBG_ASSERT(client->set.serverSurface);
        NEXUS_Surface_Destroy(client->set.serverSurface);
        client->set.serverSurface = NULL;
    }
    if(client->set.serverSurface==NULL) {
        client->state.client_type = client_type_set;
        client->set.serverCreateSettings = createSettings;
        client->set.serverSurface = NEXUS_Surface_Create(&client->set.serverCreateSettings);
        if (!client->set.serverSurface) {
            return BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);
        }
        flags |= NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE;
    }
    BDBG_ASSERT(client->state.client_type == client_type_set);
    if(client->set.surface.surface!=surface) {
        if(client->set.surface.surface) {
            NEXUS_OBJECT_RELEASE(client, NEXUS_Surface, client->set.surface.surface);
        } 
        NEXUS_OBJECT_ACQUIRE(client, NEXUS_Surface, surface);
        NEXUS_SURFACECLIENT_P_ASSIGN_SURFACE(client, &client->set.surface, surface);
    }
    client->update_info.updateRect.x = 0;
    client->update_info.updateRect.y = 0;
    client->update_info.updateRect.width = client->state.clientRegion.width;
    client->update_info.updateRect.height = client->state.clientRegion.height;
    nexus_p_surface_compositor_update_client(client, flags);
    client->set.cnt++;
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_SurfaceClient_UpdateSurface( NEXUS_SurfaceClientHandle client, const NEXUS_Rect *pUpdateRect )
{
    BDBG_MSG_TRACE(("UpdateSurface:%#p %u", client, client->client_id));
    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if(client->state.client_type != client_type_set) {
        BDBG_ERR(("must call NEXUS_SurfaceClient_SetSurface first"));
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    /* if the previous blit is not complete (if client->set.serverSurface is being read by M2MC HW), we need to fail this call.
    app must wait for recycle event. */
    if (client->set.dirty) {
        BDBG_ERR(("app must wait for recycle callback to avoid tearing. NEXUS_SurfaceClient_UpdateSurface rejected."));
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    if(pUpdateRect==NULL) {
        client->update_info.updateRect.x = 0;
        client->update_info.updateRect.y = 0;
        client->update_info.updateRect.width = client->state.clientRegion.width;
        client->update_info.updateRect.height = client->state.clientRegion.height;
    } else {
        if(pUpdateRect->x < 0 || pUpdateRect->y < 0 || (pUpdateRect->x + pUpdateRect->width) > client->state.clientRegion.width || (pUpdateRect->y + pUpdateRect->height) > client->state.clientRegion.height) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        client->update_info.updateRect = *pUpdateRect;
    }
    NEXUS_SURFACECLIENT_P_ASSIGN_SURFACE(client, &client->set.surface, client->set.surface.surface);
    nexus_p_surface_compositor_update_client(client, NEXUS_P_SURFACECLIENT_UPDATE_SOURCE);
    client->set.cnt++;
    return NEXUS_SUCCESS;
}

static void NEXUS_SurfaceClient_P_ClearRecycleQueue(NEXUS_SurfaceClientHandle client)
{
    struct NEXUS_SurfaceCompositor_P_PushQueue *queue = &client->queue.recycle;
    NEXUS_SurfaceCompositor_P_PushElement *node;
    while ((node = BLST_SQ_FIRST(queue))) {
        BLST_SQ_REMOVE_HEAD(queue, link);
        BLST_SQ_INSERT_TAIL(&client->queue.free, node, link);
    }
    return;
}

static void NEXUS_SurfaceClient_P_ClearPushQueue(NEXUS_SurfaceClientHandle client)
{
    NEXUS_SurfaceCompositor_P_PushElement *node;
    struct NEXUS_SurfaceCompositor_P_PushQueue *queue = &client->queue.push;
    while ((node = BLST_SQ_FIRST(queue))) {
        NEXUS_OBJECT_RELEASE(client, NEXUS_Surface, node->surface.surface);
        BLST_SQ_REMOVE_HEAD(queue, link);
        BLST_SQ_INSERT_TAIL(&client->queue.free, node, link);
    }
    return;
}

static void NEXUS_SurfaceClient_P_RecycleOld(NEXUS_SurfaceClientHandle client, bool infront)
{
    NEXUS_SurfaceCompositor_P_PushElement *node;
    bool recycled = false;

    if(!infront) {
        if(client->queue.last && !client->queue.compositing) {
            NEXUS_SurfaceCompositor_P_PushElement *node = BLST_SQ_FIRST(&client->queue.push);
            if(node) {
                recycled = true;
                BLST_SQ_REMOVE_HEAD(&client->queue.push, link);
                BLST_SQ_INSERT_TAIL(&client->queue.recycle, node, link);
                NEXUS_OBJECT_RELEASE(client, NEXUS_Surface, node->surface.surface);
                BDBG_MSG_TRACE(("RecycleOld:%#lx recycle %#lx ...", (unsigned long)client, (unsigned long)node));
            }
        }
    } else {
        NEXUS_SurfaceCompositor_P_PushElement *first_node=NULL;
        if(client->queue.compositing) {
            first_node = BLST_SQ_FIRST(&client->queue.push);
            if(first_node) {
                BLST_SQ_REMOVE_HEAD(&client->queue.push, link);
            }
        }
        while ((node = BLST_SQ_FIRST(&client->queue.push))) {
            recycled = true;
            BLST_SQ_REMOVE_HEAD(&client->queue.push, link);
            BLST_SQ_INSERT_TAIL(&client->queue.recycle, node, link);
            NEXUS_OBJECT_RELEASE(client, NEXUS_Surface, node->surface.surface);
            BDBG_MSG_TRACE(("RecycleOld:%#lx recycle %#lx ...", (unsigned long)client, (unsigned long)node));
        }
        if(first_node) {
            BLST_SQ_INSERT_HEAD(&client->queue.push, first_node, link);
        }
    }
    if(recycled) {
        NEXUS_TaskCallback_Fire(client->recycledCallback);
    }
    return;
}


static bool NEXUS_SurfaceClient_P_RemoveFromQueue(NEXUS_SurfaceClientHandle client, struct NEXUS_SurfaceCompositor_P_PushQueue *queue, NEXUS_SurfaceHandle surface)
{
    NEXUS_SurfaceCompositor_P_PushElement *node;
    for(node = BLST_SQ_FIRST(queue); node; node = BLST_SQ_NEXT(node, link)) {
        if(node->surface.surface == surface) {
            if(queue == &client->queue.push) {
                NEXUS_OBJECT_RELEASE(client, NEXUS_Surface, node->surface.surface);
            }
            BLST_SQ_REMOVE(queue, node, NEXUS_SurfaceCompositor_P_PushElement, link);
            BLST_SQ_INSERT_HEAD(&client->queue.free, node, link);
            return true;
        }
    }
    return false;
}

static void NEXUS_SurfaceClient_P_InsertPushQueue(NEXUS_SurfaceClientHandle client, NEXUS_SurfaceCompositor_P_PushElement *node, NEXUS_SurfaceHandle surface)
{
    NEXUS_OBJECT_ACQUIRE(client, NEXUS_Surface, surface);
    NEXUS_SURFACECLIENT_P_ASSIGN_SURFACE(client, &node->surface, surface);
    BLST_SQ_REMOVE_HEAD(&client->queue.free, link);
    BLST_SQ_INSERT_TAIL(&client->queue.push, node, link);
    client->queue.last = false;
    return;
}



NEXUS_Error NEXUS_SurfaceClient_PushSurface( NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle surface, const NEXUS_Rect *pUpdateRect, bool infront )
{
    NEXUS_SurfaceCompositor_P_PushElement *node;
    NEXUS_SurfaceCreateSettings createSettings;
    unsigned flags = NEXUS_P_SURFACECLIENT_UPDATE_SOURCE;

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    if(client->state.client_type != client_type_push && client->state.client_type != client_type_idle) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    BDBG_MSG_TRACE(("PushSurface:%#lx %#lx %s", (unsigned long)client, (unsigned long)surface, infront?"infront":""));
    client->state.client_type = client_type_push;

    NEXUS_Surface_GetCreateSettings(surface, &createSettings);
    if(pUpdateRect) {
        if(pUpdateRect->x < 0 || pUpdateRect->y < 0 || (pUpdateRect->x + pUpdateRect->width) > createSettings.width || (pUpdateRect->y + pUpdateRect->height) > createSettings.height) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }

    if (BLST_SQ_FIRST(&client->queue.push)==NULL) {
        flags |= NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE;
    }
    NEXUS_SurfaceClient_P_RecycleOld(client, infront);

    node = BLST_SQ_FIRST(&client->queue.free);
    if (!node) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    NEXUS_SurfaceClient_P_InsertPushQueue(client, node, surface);

    client->state.clientRegion.width = createSettings.width;
    client->state.clientRegion.height = createSettings.height;
    if(pUpdateRect==NULL) {
        client->update_info.updateRect.x = 0;
        client->update_info.updateRect.y = 0;
        client->update_info.updateRect.width = createSettings.width;
        client->update_info.updateRect.height = createSettings.height;
    } else {
        client->update_info.updateRect = *pUpdateRect;
    }
    nexus_p_surface_compositor_update_client(client, flags);
    client->queue.cnt++;
    return NEXUS_SUCCESS;
}

static void NEXUS_SurfaceClient_P_RecycleFromQueue(NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle *recycled, size_t num_entries, size_t *num_returned)
{
    unsigned i;
    NEXUS_SurfaceCompositor_P_PushElement *node;
    for (i=0;i<num_entries;i++) {
        node = BLST_SQ_FIRST(&client->queue.recycle);
        if (!node) break;
        NEXUS_SURFACECLIENT_P_SURFACE_VERIFY(client, &node->surface);
        recycled[i] = node->surface.surface;
        BLST_SQ_REMOVE_HEAD(&client->queue.recycle, link);
        NEXUS_SURFACECLIENT_P_SURFACE_INIT(&node->surface);
        BLST_SQ_INSERT_HEAD(&client->queue.free, node, link);
    }
    *num_returned = i;
    return;
}

NEXUS_Error NEXUS_SurfaceClient_RecycleSurface( NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle *recycled, size_t num_entries, size_t *num_returned )
{
    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);

    BDBG_MSG_TRACE(("NEXUS_SurfaceClient_RecycleSurface:%p", client));
    BDBG_ASSERT(num_returned);
    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if(client->state.client_type!=client_type_push) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    NEXUS_SurfaceClient_P_RecycleFromQueue(client, recycled, num_entries, num_returned);
    BDBG_MSG_TRACE(("NEXUS_SurfaceClient_RecycleSurface:%p -> %u", client, *num_returned));
    return NEXUS_SUCCESS;
}

void NEXUS_SurfaceClient_Clear( NEXUS_SurfaceClientHandle client )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    switch(client->state.client_type) {
    case client_type_set:
        BDBG_ASSERT(client->set.serverSurface);
        BDBG_ASSERT(client->set.surface.surface);
        NEXUS_OBJECT_RELEASE(client, NEXUS_Surface, client->set.surface.surface);
        NEXUS_Surface_Destroy(client->set.serverSurface);
        client->set.surface.surface = NULL;
        client->set.serverSurface = NULL;
        client->set.dirty = false;
        client->set.updating = false;
        if(client->state.cache.surface) {
            NEXUS_Surface_Destroy(client->state.cache.surface);
            client->state.cache.surface = NULL;
        }
        break;
    case client_type_tunnel:
    case client_type_tunnel_emulated:
        /* we must explicitly release tunneled surfaces here. objdb can't do it because they are registered as surfaces and will be destroyed */
        if(client->state.client_type == client_type_tunnel) {
            /* if it's really tunnel client, then server should point to this client  */
            BDBG_ASSERT(client->server->tunnel.client == client);
        }
        for(i=0;i<sizeof(client->tunnel.surfaces)/sizeof(client->tunnel.surfaces[0]);i++) {
            if(client->tunnel.surfaces[i].surface) {
                NEXUS_SurfaceClient_ReleaseTunneledSurface(client, client->tunnel.surfaces[i].surface);
            }
        }
        /* release should clear type and tunnel.client */
        BDBG_ASSERT(client->state.client_type == client_type_idle);
        BDBG_ASSERT(!client->tunnel.visible);
        break;
    case client_type_push:
        /* recycle multibuffering queue */
        NEXUS_SurfaceClient_P_ClearRecycleQueue(client);
        NEXUS_SurfaceClient_P_ClearPushQueue(client);
        client->queue.compositing = false;
        client->queue.last = false;
        break;
    default:
        break;
    }

    client->state.client_type = client_type_idle;

    nexus_p_surface_compositor_update_client(client, NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE);
    client->set.cnt = 0;
    client->queue.cnt = 0;

    return;
}

void NEXUS_SurfaceClient_GetSettings( NEXUS_SurfaceClientHandle client, NEXUS_SurfaceClientSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    *pSettings = client->settings;
}

NEXUS_Error NEXUS_SurfaceClient_SetSettings( NEXUS_SurfaceClientHandle client, const NEXUS_SurfaceClientSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    BDBG_ASSERT(pSettings);
    if(client->state.client_type != client_type_idle) {
        /* certain configuration can't be changed when rendering is active */
        if(pSettings->orientation != client->settings.orientation) {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
    }
    if (client->type == NEXUS_SurfaceClient_eTopLevel) {
        NEXUS_TaskCallback_Set(client->displayedCallback, &pSettings->displayed);
        NEXUS_TaskCallback_Set(client->recycledCallback, &pSettings->recycled);
        NEXUS_TaskCallback_Set(client->displayStatusChangedCallback, &pSettings->displayStatusChanged);
    }
    if(client->state.client_type == client_type_tunnel) {
        /* can't change orientation of active tunneled client */
        if(pSettings->orientation != client->settings.orientation) { return BERR_TRACE(NEXUS_NOT_SUPPORTED); }
    }

    client->settings = *pSettings;

    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        /* call after settings applied because it may be deferred */
        nexus_surfaceclient_p_setvideo(client);
    }
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_SurfaceClient_GetStatus( NEXUS_SurfaceClientHandle client, NEXUS_SurfaceClientStatus *pStatus )
{
    const struct NEXUS_SurfaceCompositorDisplay *display;

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    display = &client->server->display[0];

    if(display->display) {
        pStatus->display.framebuffer = display->canvas;
        pStatus->display.screen = display->formatInfo.canvas;
        pStatus->display.enabled3d = display->formatInfo.orientation != NEXUS_VideoOrientation_e2D;
    }

    pStatus->display.enabled = client->server->settings.enabled;
    
    switch (client->type) {
    case NEXUS_SurfaceClient_eVideoWindow:
        /* window position in display coordinates, not framebuffer coordinates */
        pStatus->position = client->server->settings.display[0].window[client->client_id].settings.position;
        break;
    /* TODO: case NEXUS_SurfaceClient_eChild: */    
    default:
        pStatus->position = client->state.framebufferRect; /* not clipped */
        break;
    }

    return 0;
}

NEXUS_Error NEXUS_SurfaceClient_AcquireTunneledSurface( NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle *pSurface )
{
    unsigned i;
    struct NEXUS_SurfaceCompositorDisplay *display;
    NEXUS_SurfaceCompositorHandle server;
    const unsigned tunnelDisplay = 0;
    NEXUS_SurfaceCreateSettings tunnelSettings;
    NEXUS_Rect tunnelRect;
    NEXUS_SurfaceClient_TunnelSurface *tunnelSurface;
    unsigned flags = 0;

    BDBG_MSG_TRACE(("AcquireTunneledSurface:%p", client));

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    if (client->type != NEXUS_SurfaceClient_eTopLevel) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if(client->state.client_type != client_type_tunnel && client->state.client_type != client_type_tunnel_emulated && client->state.client_type != client_type_idle) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    if (!client->server->state.active) { return BERR_TRACE(NEXUS_NOT_AVAILABLE); }

    server = client->server;
    display = &server->display[tunnelDisplay];
    tunnelRect = client->state.framebufferRect; /* unclipped rectangle */
    for(tunnelSurface=NULL,i=0;i<sizeof(client->tunnel.surfaces)/sizeof(client->tunnel.surfaces[0]);i++) {
        if(!client->tunnel.surfaces[i].acquired) {
            tunnelSurface = &client->tunnel.surfaces[i];
            break;
        }
    }
    if(!tunnelSurface) { return BERR_TRACE(NEXUS_NOT_AVAILABLE);}

    if (client->serverSettings.tunnelCapable) {
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = BLST_SQ_FIRST(&display->available);
        NEXUS_SurfaceMemory memory;
        const NEXUS_PixelFormatInfo *pixelInfo;

        if(!server->tunnel.overlapped) {
            BDBG_MSG_TRACE(("AcquireTunneledSurface:%p try to allocate from displaying:%p submitted:%p ", client, display->displaying, display->submitted));
            if(display->displaying && !display->displaying->tunnel.acquired) {
                framebuffer = display->displaying;
            } else if(display->submitted && !display->submitted->tunnel.acquired) {
                framebuffer = display->submitted;
            } else if(display->compositing && !display->compositing->tunnel.acquired) {
                framebuffer = display->compositing;
            }
        }

        if(framebuffer==NULL) { return BERR_TRACE(NEXUS_NOT_AVAILABLE); }

        /* client and server 3D settings should match */
        if(client->settings.orientation != display->formatInfo.orientation || client->serverSettings.composition.rightViewOffset != 0) { return BERR_TRACE(NEXUS_NOT_SUPPORTED);}
        if(client->settings.orientation == NEXUS_VideoOrientation_e2D) {
            /* 2D client should be just on screen */
            if(tunnelRect.x < 0 || tunnelRect.y < 0) { return BERR_TRACE(NEXUS_NOT_SUPPORTED);}
            if(tunnelRect.x + tunnelRect.width > display->canvas.width || tunnelRect.y + tunnelRect.height > display->canvas.height) { return BERR_TRACE(NEXUS_NOT_SUPPORTED);}
        } else {
            /* otherwise it's has to be full screen */
            if(tunnelRect.x != 0 || tunnelRect.y != 0 || tunnelRect.width != display->canvas.width || tunnelRect.height != display->canvas.height) { return BERR_TRACE(NEXUS_NOT_SUPPORTED);}
            /* increase rectangle to account for 3D */
            tunnelRect.y += display->offset3DRight.x;
            tunnelRect.y += display->offset3DRight.y;
        }


        NEXUS_Surface_GetCreateSettings(framebuffer->surface, &tunnelSettings);
        pixelInfo = NEXUS_PixelFormat_GetInfo(tunnelSettings.pixelFormat);
        if(pixelInfo==NULL) {return BERR_TRACE(NEXUS_NOT_SUPPORTED);}
        NEXUS_Surface_GetMemory(framebuffer->surface, &memory);
        tunnelSettings.pMemory = (uint8_t *)memory.buffer + tunnelRect.y * memory.pitch  + tunnelRect.x * pixelInfo->bpp/8;
        tunnelSettings.pPaletteMemory = memory.palette;
        tunnelSettings.pitch = memory.pitch;
        tunnelSurface->framebuffer = framebuffer;
        framebuffer->tunnel.rect = tunnelRect;
        if(client->state.client_type == client_type_idle) {
            client->state.client_type = client_type_tunnel;
            flags = NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE;
            BDBG_ASSERT(client->server->tunnel.client == NULL);
            client->server->tunnel.client = client;
        }
        BDBG_ASSERT(client->state.client_type == client_type_tunnel);
    } else {
        NEXUS_SurfaceHandle framebuffer = display->framebuffer[0].surface;
        NEXUS_Surface_GetCreateSettings(framebuffer, &tunnelSettings);
        tunnelSurface->framebuffer = NULL;
        if(client->state.client_type == client_type_idle) {
            client->state.client_type = client_type_tunnel_emulated;
            flags = NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE;
        }
        BDBG_ASSERT(client->state.client_type == client_type_tunnel_emulated);
        client->state.client_type = client_type_tunnel_emulated;
    }
    tunnelSettings.width = tunnelRect.width;
    tunnelSettings.height = tunnelRect.height;
    client->state.clientRegion.width = tunnelRect.width;
    client->state.clientRegion.height = tunnelRect.height;
    tunnelSurface->surface = NEXUS_Surface_Create(&tunnelSettings);
    if(tunnelSurface->surface==NULL) {return BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);}
    tunnelSurface->acquired = true;
    if(tunnelSurface->framebuffer) {
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = tunnelSurface->framebuffer;

        if(framebuffer==BLST_SQ_FIRST(&display->available)) {
            BLST_SQ_REMOVE_HEAD(&display->available, link);
            BLST_Q_INSERT_HEAD(&display->tunnel.available, framebuffer, tunnelLink);
        }
        framebuffer->tunnel.acquired = true;
        framebuffer->tunnel.composited = false;
        NEXUS_SURFACECLIENT_P_SURFACE_INIT(&framebuffer->tunnel.surface);
        framebuffer->tunnel.surface.surface =  tunnelSurface->surface;
    } else {
        BDBG_ASSERT(client->state.client_type == client_type_tunnel_emulated);
    }

    *pSurface = tunnelSurface->surface;
    nexus_p_surface_compositor_update_client(client, flags);
    BDBG_MSG_TRACE(("AcquireTunneledSurface:%p surface:%p framebuffer:%p", client, tunnelSurface->surface, tunnelSurface->framebuffer));
    return NEXUS_SUCCESS;
}


void NEXUS_SurfaceClient_ReleaseTunneledSurface( NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle surface )
{
    unsigned i;
    unsigned flags = 0;
    NEXUS_SurfaceClient_TunnelSurface *tunnelSurface;

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    BDBG_ASSERT(surface);
    if (client->type != NEXUS_SurfaceClient_eTopLevel) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    if(client->state.client_type != client_type_tunnel && client->state.client_type != client_type_tunnel_emulated) {
        (void) BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return;
    }

    for(tunnelSurface=NULL,i=0;i<sizeof(client->tunnel.surfaces)/sizeof(client->tunnel.surfaces[0]);i++) {
        if(client->tunnel.surfaces[i].surface == surface) {
            tunnelSurface = &client->tunnel.surfaces[i];
            break;
        }
    }

    if (tunnelSurface == NULL) {
        (void)BERR_TRACE(NEXUS_INVALID_PARAMETER);return;
    }
    BDBG_MSG_TRACE(("ReleaseTunnelSurface:%#lx surface:%#lx framebuffer:%d", (unsigned long)client, (unsigned long)tunnelSurface->surface, tunnelSurface->framebuffer));
    NEXUS_Surface_Destroy(tunnelSurface->surface);
    BDBG_ASSERT(tunnelSurface->acquired);
    if(tunnelSurface->framebuffer) {
        const unsigned tunnelDisplay = 0;
        struct NEXUS_SurfaceCompositorDisplay *display = &client->server->display[tunnelDisplay];
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = tunnelSurface->framebuffer;

        BDBG_ASSERT(client->state.client_type == client_type_tunnel);

        switch(framebuffer->state) {
        case NEXUS_SurfaceCompositorFramebufferState_eTunnelSubmitted:
            BLST_Q_REMOVE(&display->tunnel.submitted, framebuffer, tunnelLink);
            if(BLST_Q_FIRST(&display->tunnel.submitted)==NULL) {
                client->tunnel.visible = false;
                flags |= NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE;
            }
            break;
        case NEXUS_SurfaceCompositorFramebufferState_eAvailable:
            BLST_Q_REMOVE(&display->tunnel.available, framebuffer, tunnelLink);
            break;
        default:
            break;
        }
        NEXUS_SURFACECLIENT_P_SURFACE_INIT(&framebuffer->tunnel.surface);
        framebuffer->tunnel.acquired = false;
        tunnelSurface->framebuffer = NULL;
        BLST_SQ_INSERT_TAIL(&display->available, framebuffer, link);
    } else {
        BDBG_ASSERT(client->state.client_type == client_type_tunnel_emulated);
        if(client->queue.compositing && BLST_SQ_FIRST(&client->queue.push) && BLST_SQ_FIRST(&client->queue.push)->surface.surface == tunnelSurface->surface) {
            client->queue.compositing = false;
        }
        if(NEXUS_SurfaceClient_P_RemoveFromQueue(client, &client->queue.push, tunnelSurface->surface)) {
            if(BLST_SQ_FIRST(&client->queue.push)==NULL) {
                client->tunnel.visible = false;
                flags |= NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE;
            }
        } else {
            NEXUS_SurfaceClient_P_RemoveFromQueue(client, &client->queue.recycle, tunnelSurface->surface);
        }
    }
    tunnelSurface->surface = NULL;
    tunnelSurface->acquired = false;
    for(i=0;i<sizeof(client->tunnel.surfaces)/sizeof(client->tunnel.surfaces[0]);i++) {
        if(client->tunnel.surfaces[i].acquired) {
            break;
        }
    }
    if(i==sizeof(client->tunnel.surfaces)/sizeof(client->tunnel.surfaces[0])) {
        /* all tunnel surfaces are released mark client as idle */
        if(client->state.client_type == client_type_tunnel) {
            BDBG_ASSERT(client->server->tunnel.client == client);
            client->server->tunnel.client = NULL;
        }
        client->state.client_type = client_type_idle;
        flags |= NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE;
    }
    if(surface==client->state.current.surface) {
        client->state.current.surface = NULL;
        flags |= NEXUS_P_SURFACECLIENT_UPDATE_SOURCE;
    }
    nexus_p_surface_compositor_update_client(client, flags);

    if (!client->server->settings.enabled) {
        nexus_p_surface_compositor_check_inactive(client->server);
    }

    return;
}

#if 0
/* debug code */
void nexus_surfacecmp_p_print_tunnel(NEXUS_SurfaceCompositorHandle server)
{
    struct NEXUS_SurfaceCompositorDisplay *display = &server->display[0];
    struct NEXUS_SurfaceCompositorFramebuffer *temp;
    BKNI_Printf("tunnel submitted: ");
    for (temp = BLST_Q_FIRST(&display->tunnel.submitted); temp; temp = BLST_Q_NEXT(temp, tunnelLink)) {
        BKNI_Printf("%p(%d) ", temp, temp->state);
    }
    BKNI_Printf("; available: ");
    for (temp = BLST_Q_FIRST(&display->tunnel.available); temp; temp = BLST_Q_NEXT(temp, tunnelLink)) {
        BKNI_Printf("%p(%d) ", temp, temp->state);
    }
    BKNI_Printf("\n");
}
#endif

NEXUS_Error NEXUS_SurfaceClient_PushTunneledSurface( NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle surface, const NEXUS_Rect *pUpdateRect, bool infront)
{
    unsigned i;
    NEXUS_SurfaceClient_TunnelSurface *tunnelSurface;
    const unsigned tunnelDisplay = 0;
    unsigned flags = NEXUS_P_SURFACECLIENT_UPDATE_SOURCE;

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    BDBG_MSG_TRACE(("PushTunneledSurface:%#lx %#lx %s", (unsigned long)client, (unsigned long)surface, infront?"infront":""));
    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if(client->state.client_type != client_type_tunnel && client->state.client_type != client_type_tunnel_emulated) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if(pUpdateRect) {
        if(pUpdateRect->x < 0 || pUpdateRect->y < 0 || (pUpdateRect->x + pUpdateRect->width) > client->state.clientRegion.width || (pUpdateRect->y + pUpdateRect->height) > client->state.clientRegion.height) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }

    for(tunnelSurface=NULL,i=0;i<sizeof(client->tunnel.surfaces)/sizeof(client->tunnel.surfaces[0]);i++) {
        if(client->tunnel.surfaces[i].surface == surface) {
            tunnelSurface = &client->tunnel.surfaces[i];
            break;
        }
    }
    if(!tunnelSurface) { return BERR_TRACE(NEXUS_INVALID_PARAMETER);}
    BDBG_MSG_TRACE(("PushTunneledSurface:%#lx surface:%#lx framebuffer:%#lx", (unsigned long)client, (unsigned long)tunnelSurface->surface, tunnelSurface->framebuffer));
    if(tunnelSurface->framebuffer) {
        struct NEXUS_SurfaceCompositorDisplay *display = &client->server->display[tunnelDisplay];
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = tunnelSurface->framebuffer;
        bool first = BLST_Q_FIRST(&display->tunnel.submitted) == NULL;

        BDBG_MSG_TRACE(("PushTunneledSurface:%#lx tunnel framebuffer:%#lx:%lx(%u) %s", (unsigned long)client, (unsigned long)tunnelSurface->surface, framebuffer, framebuffer->state, first?"first":""));
        BDBG_ASSERT(client->state.client_type == client_type_tunnel);
        if (!client->server->settings.enabled) {
            /* tunneled clients should check enabled state in order to release tunneled
            surfaces when required for display format change, etc. */
            return BERR_TRACE(NEXUS_NOT_AVAILABLE);
        }
        client->tunnel.visible = true;
        if(first) {
            flags |= NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE;
        }

#if 0
        nexus_surfacecmp_p_print_tunnel(client->server);
#endif
        NEXUS_SURFACECLIENT_P_ASSIGN_SURFACE(client, &framebuffer->tunnel.surface, tunnelSurface->surface);

        switch(framebuffer->state) {
        default:
            /* no-op framebuffer either already queued or used for compositing/display */
            /* fire the displayed callback just in case app is waiting on it. it is unnecessary though. */
            NEXUS_TaskCallback_Fire(client->displayedCallback);
            BDBG_MSG_TRACE(("PushTunneledSurface:%#lx surface:%#lx framebuffer:%#lx already displayed", (unsigned long)client, (unsigned long)tunnelSurface->surface, tunnelSurface->framebuffer));
            return BERR_SUCCESS;
        case NEXUS_SurfaceCompositorFramebufferState_eAvailable:
            for(i=0;i<NEXUS_SURFACECMP_MAX_FRAMEBUFFERS;i++) {
                display->framebuffer[i].tunnel.pristine = false;
            }
            framebuffer->tunnel.pristine = true;
            framebuffer->tunnel.composited = false;
            BLST_Q_REMOVE(&display->tunnel.available, framebuffer, tunnelLink);
            BLST_Q_INSERT_TAIL(&display->tunnel.submitted, framebuffer, tunnelLink);
            framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eTunnelSubmitted;
            break;
        }
    } else { /* simulate tunnel surface with push/recycle */
        NEXUS_SurfaceCompositor_P_PushElement *node;

        if(client->state.client_type != client_type_tunnel_emulated) {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
#if 0
        if(!infront) {
            for(node=BLST_SQ_FIRST(&client->queue.push);node;node=BLST_SQ_NEXT(node,link)) {
                if(node->surface == surface) {
                    /* framebuffer either already queued  */
                    BDBG_MSG_TRACE(("PushTunneledSurface:%#lx surface:%#lx already displayed", (unsigned long)client, (unsigned long)surface));
                    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
                }
            }
        } else {
            if(client->queued.compositing) {
                BDBG_ASSERT(node);
                if(node->surface == surface) {
                    BDBG_MSG_TRACE(("PushTunneledSurface:%#lx surface:%#lx already displayed", (unsigned long)client, (unsigned long)surface));
                    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
                }
            }
        }
#endif
        if(BLST_SQ_FIRST(&client->queue.push)==NULL) {
            flags |= NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE;
        }
        NEXUS_SurfaceClient_P_RecycleOld(client, infront);

        if(BLST_SQ_FIRST(&client->queue.free)==NULL) {
            BDBG_MSG_TRACE(("PushTunneledSurface:%#p emulate recycle %p", client, BLST_SQ_FIRST(&client->queue.recycle)));
            NEXUS_SurfaceClient_P_ClearRecycleQueue(client);
        }
        node = BLST_SQ_FIRST(&client->queue.free);
        BDBG_ASSERT(node);
        client->tunnel.visible = true;
        NEXUS_SurfaceClient_P_InsertPushQueue(client, node, surface);
        BDBG_MSG_TRACE(("PushTunneledSurface:%#p emulate insert %p", client, node->surface));
    }
    if(pUpdateRect==NULL) {
        client->update_info.updateRect.x = 0;
        client->update_info.updateRect.y = 0;
        client->update_info.updateRect.width = client->state.clientRegion.width;
        client->update_info.updateRect.height = client->state.clientRegion.height;
    } else {
        client->update_info.updateRect = *pUpdateRect;
    }
    nexus_p_surface_compositor_update_client(client, flags);

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_SurfaceClient_RecycleTunneledSurface( NEXUS_SurfaceClientHandle client, NEXUS_SurfaceHandle *recycled, size_t num_entries, size_t *num_returned )
{
    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    BSTD_UNUSED(num_entries);
    BDBG_ASSERT(num_returned);

    if (!recycled) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    switch(client->state.client_type) {
    case client_type_tunnel_emulated:
        NEXUS_SurfaceClient_P_RecycleFromQueue(client, recycled, num_entries, num_returned);
        break;
    case client_type_tunnel:
        {
            const unsigned tunnelDisplay = 0;
            struct NEXUS_SurfaceCompositorDisplay *display = &client->server->display[tunnelDisplay];
            struct NEXUS_SurfaceCompositorFramebuffer *framebuffer;
            unsigned i;

            for(i=0,framebuffer=BLST_Q_FIRST(&display->tunnel.available);i<num_entries && framebuffer;framebuffer=BLST_Q_NEXT(framebuffer, tunnelLink)) {
                BDBG_MSG_TRACE(("RecycleTunneledSurface:%p framebuffer:%p:%s %s", client, framebuffer, framebuffer->tunnel.pristine?"pristine":"", framebuffer->tunnel.composited?"composited":""));
                if(!framebuffer->tunnel.pristine /* used for 'next' compositing */ ) {
                    framebuffer->tunnel.composited = false;
                    recycled[i] = framebuffer->tunnel.surface.surface;
                    NEXUS_SURFACECLIENT_P_SURFACE_VERIFY(client, &framebuffer->tunnel.surface);
                    i++;
                    continue;
                }
            }
            *num_returned = i;
            BDBG_MSG_TRACE(("RecycleTunneledSurface:%p -> %u", client, i));
        }
        break;
    default:
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    return NEXUS_SUCCESS;
}

/**
if tunneled mode
    if single buffering (which must be single client), nothing need be done.
    if double buffering (which must be single client in the general case), a copy must be done on every 2nd framebuffer.
        note: if double buffering and non-overlapped, we can be multiclient w/ no performance hit on the tunnel.
    if triple buffering (possibly multiclient), the server cannot use the last submitted copy of the tunneled surface. it must use the client's current surface.
if not in tunneled mode (SetSurface or PushSurface)
    a copy from the last SetSurface/PushSurface will be done on each vsync.
**/
NEXUS_Error NEXUS_SurfaceClient_PublishSurface( NEXUS_SurfaceClientHandle client )
{
    NEXUS_SurfaceHandle surface = NULL;

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    if (client->type == NEXUS_SurfaceClient_eVideoWindow) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    /* determine which surface can be published */
    switch(client->state.client_type) {
    case client_type_idle:
#if XXX
    case client_type_tunnel:
#endif
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        break;
    case client_type_set:
        surface = client->set.surface.surface;
        break;
    case client_type_push:
        {
            NEXUS_SurfaceCompositor_P_PushElement *node;
            NEXUS_SurfaceCreateSettings createSettings;

            node = BLST_SQ_FIRST(&client->queue.push);
            if (node==NULL) {
                return BERR_TRACE(NEXUS_NOT_SUPPORTED);
            }
            surface = node->surface.surface;
            NEXUS_Surface_GetCreateSettings(surface, &createSettings);
            client->state.clientRegion.width = createSettings.width;
            client->state.clientRegion.height = createSettings.height;
            break;
        }
    default:
        break;
    }
    if(!surface) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    client->published.surface = surface;
    client->update_info.updateRect.x = 0;
    client->update_info.updateRect.y = 0;
    client->update_info.updateRect.width = client->state.clientRegion.width;
    client->update_info.updateRect.height = client->state.clientRegion.height;
    nexus_p_surface_compositor_update_client(client, NEXUS_P_SURFACECLIENT_UPDATE_PUBLISH);

    return NEXUS_SUCCESS;
}

