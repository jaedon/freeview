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
 * $brcm_Workfile: nexus_surface_compositor.c $
 * $brcm_Revision: 73 $
 * $brcm_Date: 10/23/12 2:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface_compositor/src/nexus_surface_compositor.c $
 * 
 * 73   10/23/12 2:34p erickson
 * SW7425-4068: fix right buffer calculation
 * 
 * 72   10/22/12 10:02a erickson
 * SW7435-429: add counts to proc
 * 
 * 71   10/17/12 5:11p erickson
 * SW7435-429: add proc
 * 
 * 70   8/8/12 4:58p vsilyaev
 * SW7435-296: Simplified handling of 'published' clients
 * 
 * 69   7/5/12 2:24p vsilyaev
 * SW7425-3392: Don't clear update flags when marking 'hidden' clients
 * 
 * 68   7/5/12 2:20p vsilyaev
 * SW7425-3392: Fixed typo
 * 
 * 67   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 66   4/17/12 3:38p vsilyaev
 * SW7425-2874: Added more tests to verify invariants, clear
 *  tunnel.visible when last surface removed from the emulated tunnel
 *  client
 * 
 * 65   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 64   1/26/12 5:22p erickson
 * SW7425-2116: add NEXUS_SurfaceComposition.colorMatrix
 * 
 * 63   1/6/12 12:57p vsilyaev
 * SW7420-2209: Calclulate and verify CRC also for 'real' tunneled clients
 * 
 * 62   1/5/12 5:55p vsilyaev
 * SW7420-2209: Added profiling traces and uses sparse CRC to reduce CPU
 *  load
 * 
 * 61   1/5/12 3:31p vsilyaev
 * SW7420-2209: Added option that would calculate and compare CRC when
 *  surfaces goes through the compositor pipe-line
 * 
 * 60   12/27/11 12:58p vsilyaev
 * SW7420-2119: Use better opaque test for blendequation and accoout
 *  opacity in test for ommiting screen clear (fill)
 * 
 * 59   12/15/11 3:00p vsilyaev
 * SW7420-1992: Use proper surface for compositing
 * 
 * 58   12/9/11 4:29p erickson
 * SW7420-1992: update dirty clients even if hidden
 * 
 * 57   11/23/11 2:55p vsilyaev
 * SW7420-2148: Reworked queue management for the push type clients
 * 
 * 56   11/22/11 1:39p vsilyaev
 * SW7420-2148: Use faster path for Set/Update clients, better handle out-
 *  of-phase displayed callback
 * 
 * 55   11/17/11 3:41p erickson
 * SW7125-1164: do positive check for illegal change to avoid stack
 *  blowout
 * 
 * 54   11/11/11 4:43p erickson
 * SW7420-1992: refactor video client code to not require compositor
 *  shutdown
 * 
 * 53   11/11/11 11:27a erickson
 * SW7125-1149: fix coverity warning
 * 
 * 52   11/10/11 4:56p erickson
 * SW7420-1992: change videowindow array to
 *  NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS to support mosaic mode
 * 
 * 51   11/9/11 10:14a erickson
 * SW7420-2119: fix release build warning
 * 
 * 50   11/7/11 2:34p erickson
 * SW7420-1992: don't make surfacecmp inactive until tunneled surfaces are
 *  released
 *
 * 49   11/7/11 10:58a vsilyaev
 * SW7420-2119: Refactored tunnel mode support
 *
 * 48   11/4/11 2:18p vsilyaev
 * SW7231-439: Really fixed stop/start callbacks
 *
 * 47   11/4/11 1:00p vsilyaev
 * SW7231-439: Removed missleading comment
 *
 * 46   11/4/11 11:48a erickson
 * SW7231-439: add stop/start callbacks on display before calling
 *  NEXUS_CallbackHandler_Shutdown(display->frameBufferCallback);
 *
 * 45   11/3/11 9:17p vsilyaev
 * SW7420-2119: Removed unnecessary state
 *
 * 44   11/3/11 9:15p vsilyaev
 * SW7420-2119: Improved handling of 'real' tunnel clients
 *
 * 43   11/1/11 5:26p vsilyaev
 * SW7420-2119: Returned back support for single frame buffer rendering
 *
 * 42   11/1/11 1:03p vsilyaev
 * SW7420-2114: Added code to handle full resolution 3D display formats
 *
 * 41   10/31/11 4:38p vsilyaev
 * SW7420-2114: Added support for native 3D graphics
 *
 * 40   10/28/11 5:01p vsilyaev
 * SW7420-2114: Fixed memory aliasing of different buffers
 *
 * 39   10/27/11 4:24p vsilyaev
 * SW7420-2110: Added support  for incremental (without redrawing of
 *  entire framebuffer) cursor drawing
 *
 * 38   10/26/11 3:52p erickson
 * SW7420-1992: allow window handles and settings to change while enabled
 *
 * 37   10/25/11 7:38p vsilyaev
 * SW7420-1992: Properly handle creation of new client
 *
 * 36   10/25/11 4:40p vsilyaev
 * SW7420-1992: Use single instance of Graphics2D
 *
 * 35   10/24/11 5:43p vsilyaev
 * SW7420-1992: Updated cursor support
 *
 * 34   10/24/11 2:24p erickson
 * SW7420-1992: don't try to compose idle clients
 *
 * 33   10/20/11 3:35p erickson
 * SW7420-1992: don't call NEXUS_VideoWindow_GetSettings_priv
 *
 * 32   10/20/11 2:13p vsilyaev
 * SW7420-1992: Added 3D compositing
 *
 * 31   10/19/11 4:44p erickson
 * SW7420-1992: fix rounding error for video window
 *
 * 30   10/18/11 6:07p vsilyaev
 * SW7420-1992: Use double linked queue for the client surfaces and trim
 *  masked surfaces
 *
 * 29   10/18/11 12:33p vsilyaev
 * SW7420-1992: More work on separation of backend and frontend
 *
 * 28   10/13/11 5:05p vsilyaev
 * SW7420-1992: Moved surface release to separate function
 *
 * 27   10/13/11 4:54p vsilyaev
 * SW7420-1992: Fixed shutdown sequemce for video window
 *
 * 26   10/13/11 3:33p erickson
 * SW7420-1992: add display format change support
 *
 * 25   10/12/11 12:49p vsilyaev
 * SW7420-1992: Moved handling of client side changes to single function
 *
 * 24   10/10/11 10:07a vsilyaev
 * SW7420-1992,SW7420-2085: Use reference counting to extend life cycle of
 *  surfaces
 *
 * 23   10/6/11 7:37p vsilyaev
 * SW7420-1992: Improved management of tunnel client
 *
 * 22   10/6/11 7:06p vsilyaev
 * SW7420-1992: Removed unneded state
 *
 * 21   10/6/11 7:02p vsilyaev
 * SW7420-1992: Added render list
 *
 * 20   10/5/11 3:50p vsilyaev
 * SW7420-1992: Split compositor(frontend) and renderer(backend) portions
 *
 * 19   10/5/11 2:05p vsilyaev
 * SW7420-1992: Improved shutdown sequence
 *
 * 18   10/5/11 1:06p erickson
 * SW7420-1992: fix HD-only video window
 *
 * 17   10/5/11 10:40a erickson
 * SW7420-1992: move calcblit into compose to avoid race conditions, check
 *  if source is clipped to nothing
 *
 * 16   10/4/11 4:38p erickson
 * SW7420-1992: fix non-tunneled clients that are tunnel-capable, release
 *  tunneled surfaces, recalc video from server-side changes, reduce churn
 *  if invisible clients are reconfigured, fix background repaint if
 *  tunneled client is destroyed
 *
 * 15   10/3/11 9:25a erickson
 * SW7420-1148: add video window support
 *
 * 14   9/7/11 7:13p vsilyaev
 * SW7420-1148: Decouple rendering of primary and other displays. Other
 *  displays would get just a copy of result rendered on primary display.
 *
 * 13   9/1/11 2:07p vsilyaev
 * SW7420-1992: Updated API
 *
 * 12   8/31/11 3:56p erickson
 * SW7420-1992: allow more than one surface compositor
 *
 * 11   8/26/11 5:35p vsilyaev
 * SW7420-1992: Updated API
 *
 * 10   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 *
 * 9   8/22/11 4:11p vsilyaev
 * SW7420-1992: Updated implementation of surface compositor
 *
 * 8   8/11/11 3:31p erickson
 * SW7420-1992: update
 *
 * 7   8/10/11 9:20a erickson
 * SW7420-1992: fix kernel stack overflow on auto-close
 *
 * 6   8/5/11 3:58p erickson
 * SW7420-1992: server can set number of framebuffers per display. revise
 *  tunneling api accordingly.
 *
 * 5   8/5/11 12:05p erickson
 * SW7420-1992: add tunneled mode, remove direct_mode
 *
 * 4   8/4/11 10:41a erickson
 * SW7420-1992: implement direct mode
 *
 * 3   8/3/11 12:37p erickson
 * SW7420-1992: determine if a client is fullscreen, then short circuit
 *  all blits below that
 *
 * 2   8/3/11 9:57a erickson
 * SW7420-1992: add per-display framebuffer heap to
 *  NEXUS_SurfaceCompositorSettings
 *
 * 1   7/26/11 1:34p erickson
 * SW7420-1992: add surface_compositor module and examples
 *
 **************************************************************************/
#include "nexus_surface_compositor_module.h"
#include "nexus_surface_compositor_impl.h"
#include "priv/nexus_surface_priv.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_surface_compositor);
BTRC_MODULE(surface_compositor_composite,ENABLE);

struct nexus_surfacecompositor_list g_nexus_surfacecompositor_list;

#define BDBG_MSG_TRACE(X) BDBG_MSG(X)
#define BDBG_MSG_DISABLE(X)

void NEXUS_SurfaceCompositorModule_P_Print(void)
{
    NEXUS_SurfaceCompositorHandle server;
    for (server = BLST_S_FIRST(&g_nexus_surfacecompositor_list); server; server = BLST_S_NEXT(server, link)) {
        NEXUS_SurfaceClientHandle client;
        unsigned i;
        BDBG_LOG(("SurfaceCompositor %p", server));
        for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
            struct NEXUS_SurfaceCompositorDisplay *display = &server->display[i];
            if (display->display) {
                BDBG_LOG(("display %d: %p, %dx%d", display->index, display->display, display->formatInfo.canvas.width, display->formatInfo.canvas.height));
            }
        }
        for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {        
            BDBG_LOG(("client %p: %d,%d,%d,%d; visible %c; zorder %d", client,
                client->serverSettings.composition.position.x, 
                client->serverSettings.composition.position.y, 
                client->serverSettings.composition.position.width, 
                client->serverSettings.composition.position.height, 
                client->serverSettings.composition.visible?'y':'n',
                client->serverSettings.composition.zorder));
            switch (client->state.client_type) {
            case client_type_set: 
                BDBG_LOG(("  set %p, total %d", client->set.surface, client->set.cnt));
                break;
            case client_type_push:
                {
                    unsigned push_count = 0, recycle_count = 0;
                    NEXUS_SurfaceCompositor_P_PushElement *e;
                    for (e = BLST_SQ_FIRST(&client->queue.push); e; e = BLST_SQ_NEXT(e, link)) push_count++;
                    for (e = BLST_SQ_FIRST(&client->queue.recycle); e; e = BLST_SQ_NEXT(e, link)) recycle_count++;
                    BDBG_LOG(("  pushed %d, recycled %d, total %d", push_count, recycle_count, client->queue.cnt));
                }
                break;
            case client_type_tunnel:
                BDBG_LOG(("  tunneled"));
                break;
            default:
                break;    
            }    
        }
    }
}

static NEXUS_Error nexus_surfaceclient_p_update_display_cache(NEXUS_SurfaceClientHandle client, bool force);
static void nexus_surface_compositor_p_dealloc_displays(NEXUS_SurfaceCompositorHandle server);
static void nexus_p_surface_compositor_update_all_clients(NEXUS_SurfaceCompositorHandle server, unsigned flags);
static void nexus_surface_compositor_p_update_dirty_client(NEXUS_SurfaceClientHandle client);

static void nexus_surface_compositor_p_initialize_compositor_display(struct NEXUS_SurfaceCompositorDisplay *cmpDisplay)
{
    unsigned i;

    cmpDisplay->compositing = NULL;
    cmpDisplay->composited = NULL;
    cmpDisplay->submitted = NULL;
    cmpDisplay->displaying = NULL;
    cmpDisplay->master_framebuffer = NULL;
    cmpDisplay->generation = 1;
    BLST_SQ_INIT(&cmpDisplay->available);
    BLST_Q_INIT(&cmpDisplay->tunnel.submitted);
    BLST_Q_INIT(&cmpDisplay->tunnel.available);
    for (i=0;i<NEXUS_SURFACECMP_MAX_FRAMEBUFFERS;i++) {
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = &cmpDisplay->framebuffer[i];
        /* framebuffer->surface points to allocated memory. do not clear. */
        framebuffer->full_render = true;
        framebuffer->scene.elements.count = 0;
        framebuffer->scene.dirty.x = 0;
        framebuffer->scene.dirty.y = 0;
        framebuffer->scene.dirty.width = 0;
        framebuffer->scene.dirty.height = 0;
        framebuffer->ref_cnt = 0;
        framebuffer->generation = 0;
        framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eAvailable;
        framebuffer->tunnel.acquired = false;
        framebuffer->tunnel.pristine = false;
        NEXUS_SURFACECLIENT_P_SURFACE_INIT(&framebuffer->tunnel.surface);
    }
    return;
}

static void nexus_surface_compositor_p_clear_blitter_state(NEXUS_SurfaceCompositorHandle server)
{
    server->state.blitter.active = 0;
    server->state.blitter.delayed = 0;
    return;
}

NEXUS_SurfaceCompositorHandle NEXUS_SurfaceCompositor_Create( unsigned server_id )
{
    NEXUS_SurfaceCompositorHandle server;
    NEXUS_Error rc;
    unsigned i;
    NEXUS_Graphics2DSettings gfxSettings;

    server = BKNI_Malloc(sizeof(*server));
    if (!server) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_SurfaceCompositor, server);
    server->server_id = server_id;
    BLST_Q_INIT(&server->clients);
    BLST_D_INIT(&server->cursors);
    server->settings.enabled = true;

    BLST_S_INSERT_HEAD(&g_nexus_surfacecompositor_list, server, link);

    server->frameBufferCallback = NEXUS_TaskCallback_Create(server, NULL);
    if (!server->frameBufferCallback) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    server->inactiveCallback = NEXUS_TaskCallback_Create(server, NULL);
    if (!server->inactiveCallback) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }

    server->gfx = NEXUS_Graphics2D_Open(0, NULL);
    if (!server->gfx) {
        BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }
    server->state.update_flags = 0;
    nexus_surface_compositor_p_clear_blitter_state(server);
    NEXUS_Graphics2D_GetSettings(server->gfx, &gfxSettings);

    NEXUS_CallbackHandler_Init(server->packetSpaceAvailableCallback, nexus_surface_compositor_p_packetspaceavailable, server);
    NEXUS_CallbackHandler_Init(server->checkpointCallback, nexus_surface_compositor_p_checkpoint, server);

    NEXUS_CallbackHandler_PrepareCallback(server->checkpointCallback, gfxSettings.checkpointCallback);
    NEXUS_CallbackHandler_PrepareCallback(server->packetSpaceAvailableCallback, gfxSettings.packetSpaceAvailable);


    rc = NEXUS_Graphics2D_SetSettings(server->gfx, &gfxSettings);
    if (rc) {
        rc = BERR_TRACE(rc);
        goto error;
    }

    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        struct NEXUS_SurfaceCompositorDisplay *display = &server->display[i];

        nexus_surface_compositor_p_initialize_compositor_display(display);
        display->canvas.width = 720;
        display->canvas.height = 480;


        NEXUS_CallbackHandler_Init(display->frameBufferCallback, nexus_surface_compositor_p_framebuffer, display);
        display->server = server;

        /* default settings per display */
        server->settings.display[i].framebuffer.number = 2;
        server->settings.display[i].framebuffer.width = 720;
        server->settings.display[i].framebuffer.width = 480;
        server->settings.display[i].framebuffer.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        server->settings.display[i].framebuffer.backgroundColor = 0xFF000000;
        server->settings.display[i].display3DSettings.overrideOrientation = false;
        server->settings.display[i].display3DSettings.orientation = NEXUS_VideoOrientation_e2D;
    }


    return server;

error:
    NEXUS_SurfaceCompositor_Destroy(server);
    return NULL;
}

static void NEXUS_SurfaceCompositor_P_Finalizer( NEXUS_SurfaceCompositorHandle server )
{
    NEXUS_SurfaceCursorHandle cursor;
    NEXUS_SurfaceClientHandle client;

    NEXUS_OBJECT_ASSERT(NEXUS_SurfaceCompositor, server);

    BLST_S_REMOVE(&g_nexus_surfacecompositor_list, server, NEXUS_SurfaceCompositor, link);
    
    /* coverity[use_after_free] */
    while(NULL!=(cursor=BLST_D_FIRST(&server->cursors))) {
        BDBG_WRN(("%p:leaked cursor %p", server, cursor));
        NEXUS_SurfaceCursor_Destroy(cursor);
    }

    /* coverity[use_after_free] */
    while(NULL!=(client=BLST_Q_FIRST(&server->clients))) {
        BDBG_WRN(("%p:leaked client %p", server, client));
        NEXUS_SurfaceCompositor_DestroyClient(client);
    }

    /* we are immediately inactive, not by waiting, but by destroying */
    nexus_surface_compositor_p_dealloc_displays(server);


    if(server->gfx) {
        NEXUS_Graphics2D_Close(server->gfx);
    }
    NEXUS_CallbackHandler_Shutdown(server->packetSpaceAvailableCallback);
    NEXUS_CallbackHandler_Shutdown(server->checkpointCallback);

    if (server->frameBufferCallback) {
        NEXUS_TaskCallback_Destroy(server->frameBufferCallback);
    }
    if (server->inactiveCallback) {
        NEXUS_TaskCallback_Destroy(server->inactiveCallback);
    }

    /* release held surfaces */
    nexus_surface_compositor_p_release_surfaces(server);

#if 0
    BDBG_OBJECT_UNSET(server, NEXUS_SurfaceCompositor);
#else
    NEXUS_OBJECT_DESTROY(NEXUS_SurfaceCompositor, server);
#endif
#if NEXUS_SURFACE_COMPOSITOR_P_CHECK_IMMUTABLE 
    NEXUS_BTRC_REPORT(surface_compositor_crc);
#endif
    NEXUS_BTRC_REPORT(surface_compositor_composite);
    NEXUS_BTRC_REPORT(surface_compositor_render);
    BKNI_Free(server);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_SurfaceCompositor, NEXUS_SurfaceCompositor_Destroy);

void NEXUS_SurfaceCompositor_GetSettings( NEXUS_SurfaceCompositorHandle server, NEXUS_SurfaceCompositorSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);
    *pSettings = server->settings;
}

static void nexus_surfacemp_p_release_display_framebuffers(NEXUS_SurfaceCompositorHandle server, struct NEXUS_SurfaceCompositorDisplay *cmpDisplay)
{
    unsigned j;
    BSTD_UNUSED(server);

    for (j=0;j<NEXUS_SURFACECMP_MAX_FRAMEBUFFERS;j++) {
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = &cmpDisplay->framebuffer[j];
        /* checkpoint required before deleting. but we cannot block here.
        so, move surface into a queue for deletion in the checkpoint. */
        if (framebuffer->surface) {
            /* this function should only be called after it ensured that there is no any outstanding activity in compositor or display */
            NEXUS_Surface_Destroy(framebuffer->surface);
        }
        if(framebuffer->view3D.left) {
            NEXUS_Surface_Destroy(framebuffer->view3D.left);
        }
        if(framebuffer->view3D.right) {
            NEXUS_Surface_Destroy(framebuffer->view3D.right);
        }

        framebuffer->surface = NULL;
        framebuffer->view3D.left = NULL;
        framebuffer->view3D.right = NULL;
    }
    return;
}

static void nexus_surface_compositor_p_dealloc_displays(NEXUS_SurfaceCompositorHandle server)
{
    unsigned i;
    /* it's only handled from the NEXUS_SurfaceCompositor_Destroy */

    NEXUS_SurfaceCursor_P_Clear(server);

    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        struct NEXUS_SurfaceCompositorDisplay *cmpDisplay = &server->display[i];
        if (cmpDisplay->display) {
            NEXUS_Error rc;
            NEXUS_GraphicsSettings graphicsSettings;
            /* disable graphics and stop callbacks */
            NEXUS_Display_GetGraphicsSettings(cmpDisplay->display, &graphicsSettings);
            NEXUS_CALLBACKDESC_INIT(&graphicsSettings.frameBufferCallback);
            graphicsSettings.enabled = false;
            rc = NEXUS_Display_SetGraphicsSettings(cmpDisplay->display, &graphicsSettings);
            if (rc) rc = BERR_TRACE(rc);
            NEXUS_StopCallbacks(cmpDisplay->display); /* clear callbacks that are 'in-flight' */
            NEXUS_CallbackHandler_Shutdown(cmpDisplay->frameBufferCallback);
            NEXUS_StartCallbacks(cmpDisplay->display); /* enable further use of callbacks from the display */
        }

        nexus_surfacemp_p_release_display_framebuffers(server, cmpDisplay);
    }
    return;
}

/* disable without stopping callbacks */
static void nexus_surfacemp_p_disable_display(NEXUS_SurfaceCompositorHandle server, struct NEXUS_SurfaceCompositorDisplay *cmpDisplay)
{
    NEXUS_Error rc;
    NEXUS_GraphicsSettings graphicsSettings;
    BSTD_UNUSED(server);
    NEXUS_Display_GetGraphicsSettings(cmpDisplay->display, &graphicsSettings);
    graphicsSettings.enabled = false;
    rc = NEXUS_Display_SetGraphicsSettings(cmpDisplay->display, &graphicsSettings);
    if (rc) rc = BERR_TRACE(rc);
    return;
}

static NEXUS_Error nexus_surface_compositor_p_update_display( NEXUS_SurfaceCompositorHandle server, const NEXUS_SurfaceCompositorSettings *pSettings )
{
    unsigned i, j;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Error rc;

    BDBG_ASSERT(!server->state.active);

    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        struct NEXUS_SurfaceCompositorDisplay *cmpDisplay = &server->display[i];
        NEXUS_VideoFormatInfo formatInfo;
        NEXUS_DisplaySettings displaySettings;
        bool defaultClip = false;

        nexus_surface_compositor_p_initialize_compositor_display(cmpDisplay);

        if (pSettings->display[i].display) {
            if (pSettings->display[i].framebuffer.number > NEXUS_SURFACECMP_MAX_FRAMEBUFFERS ||
                pSettings->display[i].framebuffer.number < 1)
            {
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
            server->display[i].num_framebuffers = pSettings->display[i].framebuffer.number;

            if (server->settings.display[i].display) {
                /* this display already registered, so check if framebuffer must be recreated */
                if (pSettings->display[i].framebuffer.width != server->settings.display[i].framebuffer.width ||
                    pSettings->display[i].framebuffer.height != server->settings.display[i].framebuffer.height)
                {
                    nexus_surfacemp_p_release_display_framebuffers(server, cmpDisplay);
                }
            }

            NEXUS_Display_GetSettings(pSettings->display[i].display, &displaySettings);
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);
            if(formatInfo.height==482) {
                formatInfo.height = 480; /* FIX for NTSC 482 vs 480 */
            }
            cmpDisplay->index = i;
            cmpDisplay->display = pSettings->display[i].display;
            cmpDisplay->backgroundColor = pSettings->display[i].framebuffer.backgroundColor;

            graphicsSettings = pSettings->display[i].graphicsSettings;
            graphicsSettings.enabled = true;
            /* at this time only full screen graphics is supported */
            graphicsSettings.clip.x = graphicsSettings.clip.y = 0;
            graphicsSettings.position.x = graphicsSettings.position.y = 0;
            graphicsSettings.position.width = formatInfo.width;
            graphicsSettings.position.height = formatInfo.height;
            if(graphicsSettings.clip.width==0 || graphicsSettings.clip.height==0) {
                graphicsSettings.clip.width = pSettings->display[i].framebuffer.width;
                graphicsSettings.clip.height = pSettings->display[i].framebuffer.height;
                defaultClip = false;
            }
            cmpDisplay->canvas.width = graphicsSettings.clip.width;
            cmpDisplay->canvas.height = graphicsSettings.clip.height;
            cmpDisplay->formatInfo.canvas.width = formatInfo.width;
            cmpDisplay->formatInfo.canvas.height = formatInfo.height;
            cmpDisplay->formatInfo.orientation = NEXUS_VideoOrientation_e2D;
            cmpDisplay->formatInfo.native3D = false;
            cmpDisplay->offset3DRight.x = cmpDisplay->offset3DRight.y = 0;

            if(pSettings->display[i].display3DSettings.overrideOrientation || displaySettings.display3DSettings.overrideOrientation || formatInfo.isFullRes3d) {
                if(pSettings->display[i].display3DSettings.overrideOrientation) {
                    cmpDisplay->formatInfo.orientation = pSettings->display[i].display3DSettings.orientation;
                } else if (displaySettings.display3DSettings.overrideOrientation) {
                    cmpDisplay->formatInfo.orientation = displaySettings.display3DSettings.orientation;
                    cmpDisplay->formatInfo.native3D = cmpDisplay->formatInfo.orientation != NEXUS_VideoOrientation_e2D;
                } else {
                    BDBG_ASSERT(formatInfo.isFullRes3d);
                    defaultClip = true; /* have to divide canvas and clip */
                    cmpDisplay->formatInfo.orientation = NEXUS_VideoOrientation_e3D_OverUnder;
                    cmpDisplay->formatInfo.native3D = true;
                    cmpDisplay->formatInfo.canvas.height *= 2; /* it would get divided latter */
                    /* cmpDisplay->canvas.height  and graphicsSettings.clip.height left as is, and would get divided latter, effectevely size of the framebuffer in pixels is controlled by an application */
                }
                switch(cmpDisplay->formatInfo.orientation) {
                case NEXUS_VideoOrientation_e3D_LeftRight:
                    cmpDisplay->formatInfo.canvas.width /= 2;
                    if(!cmpDisplay->formatInfo.native3D || defaultClip) {
                        if(cmpDisplay->canvas.width%2) {
                            BDBG_WRN(("%p:display:%a iinvalid framebuffer width:%u", server, i, cmpDisplay->canvas.width));
                        }
                        cmpDisplay->canvas.width /= 2; /* with native3D and  default clip, canvas has to be halved */
                    }
                    cmpDisplay->offset3DRight.x = cmpDisplay->canvas.width;
                    if(cmpDisplay->formatInfo.native3D) {
                        graphicsSettings.position.width /= 2;
                        if(defaultClip) {
                            graphicsSettings.clip.width /= 2;
                        }
                    }
                    break;
                case NEXUS_VideoOrientation_e3D_OverUnder:
                    cmpDisplay->formatInfo.canvas.height /= 2;
                    if(!cmpDisplay->formatInfo.native3D || defaultClip) {
                        if(cmpDisplay->canvas.height%2) {
                            BDBG_WRN(("%p:display:%a iinvalid framebuffer height%u", server, i, cmpDisplay->canvas.height));
                        }
                        cmpDisplay->canvas.height /= 2; /* with native3D and  default clip, canvas has to be halved */
                    }
                    cmpDisplay->offset3DRight.y = cmpDisplay->canvas.height;
                    if(cmpDisplay->formatInfo.native3D) {
                        graphicsSettings.position.height /= 2;
                        if(defaultClip) {
                            graphicsSettings.clip.height /= 2;
                        }
                    }
                    break;
                default:
                    break;
                }
            }

            BDBG_MSG_TRACE(("%p: display:%u format:%u(%ux%u) display.canvas:%ux%u framebuffer.canvas:%ux%u clip:%ux%u %s%s", server, i, displaySettings.format, formatInfo.width, formatInfo.height, cmpDisplay->formatInfo.canvas.width, cmpDisplay->formatInfo.canvas.height,  cmpDisplay->canvas.width, cmpDisplay->canvas.height, graphicsSettings.clip.width, graphicsSettings.clip.height, cmpDisplay->formatInfo.native3D?"native ":"", cmpDisplay->formatInfo.orientation==NEXUS_VideoOrientation_e2D?"2D":"3D"));
            rc = NEXUS_SurfaceCursor_P_UpdateDisplay(server); /* update cursor state and verify if new configuration compatible with cursor configuration */
            if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto cursor_error;}

            NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
            /* create new frambuffers if needed */
            for (j=0;j<cmpDisplay->num_framebuffers;j++) {
                struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = &cmpDisplay->framebuffer[j];

                /* already initialized above */
                framebuffer->scene.dirty.width = pSettings->display[i].framebuffer.width;
                framebuffer->scene.dirty.height = pSettings->display[i].framebuffer.height;
                /* allocate framebuffer surface */
                if (!framebuffer->surface) {
                    NEXUS_SurfaceMemory memory;
                    const NEXUS_PixelFormatInfo *pixelInfo;

                    createSettings.width = pSettings->display[i].framebuffer.width;
                    createSettings.height = pSettings->display[i].framebuffer.height;
                    createSettings.heap = pSettings->display[i].framebuffer.heap;
                    createSettings.pixelFormat = pSettings->display[i].framebuffer.pixelFormat;
                    createSettings.pMemory = NULL;
                    framebuffer->surface = NEXUS_Surface_Create(&createSettings);
                    if (!framebuffer->surface) {
                        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
                        goto create_error;
                    }
                    framebuffer->view3D.left = NULL;
                    framebuffer->view3D.right = NULL;
                    if(cmpDisplay->formatInfo.orientation==NEXUS_VideoOrientation_e3D_LeftRight || cmpDisplay->formatInfo.orientation==NEXUS_VideoOrientation_e3D_OverUnder) {
                        NEXUS_Surface_GetMemory(framebuffer->surface, &memory);
                        pixelInfo = NEXUS_PixelFormat_GetInfo(createSettings.pixelFormat);
                        createSettings.width = cmpDisplay->canvas.width;
                        createSettings.height = cmpDisplay->canvas.height;
                        createSettings.pitch = memory.pitch;
                        createSettings.pMemory = memory.buffer;
                        framebuffer->view3D.left = NEXUS_Surface_Create(&createSettings);
                        if (!framebuffer->view3D.left) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto create_error; }
                        createSettings.pMemory = (void *)((uint8_t *)memory.buffer + pixelInfo->bpp/8 * cmpDisplay->offset3DRight.x + memory.pitch * cmpDisplay->offset3DRight.y);
                        framebuffer->view3D.right = NEXUS_Surface_Create(&createSettings);
                        if (!framebuffer->view3D.right) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto create_error; }
                    }
                }
                BLST_SQ_INSERT_TAIL(&cmpDisplay->available, framebuffer, link);
            }

            NEXUS_CallbackHandler_PrepareCallback(cmpDisplay->frameBufferCallback, graphicsSettings.frameBufferCallback);
            rc = NEXUS_Display_SetGraphicsSettings(pSettings->display[i].display, &graphicsSettings);
            if (rc) {
                rc = BERR_TRACE(rc);
                goto set_error;
            }
        }
        else {
            /* display being unregistered */
            if (cmpDisplay->display) {
                nexus_surfacemp_p_disable_display(server, cmpDisplay);
                cmpDisplay->display = NULL;
            }
            nexus_surfacemp_p_release_display_framebuffers(server, cmpDisplay);
        }
    }

    server->settings = *pSettings;

    rc = nexus_surface_compositor_p_verify_tunnel(server);
    if (rc) return BERR_TRACE(rc);

    nexus_p_surface_compositor_update_all_clients(server, NEXUS_P_SURFACECLIENT_UPDATE_DISPLAY);

    return 0;

cursor_error:
create_error:
    /* destroy all framebuffers that were created */
    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        for (j=0;j<NEXUS_SURFACECMP_MAX_FRAMEBUFFERS;j++) {
            if (server->display[i].framebuffer[j].surface) {
                NEXUS_Surface_Destroy(server->display[i].framebuffer[j].surface); /* immediate is ok */
            }
            if(server->display[i].framebuffer[j].view3D.left) {
                NEXUS_Surface_Destroy(server->display[i].framebuffer[j].view3D.left);
            }
            if(server->display[i].framebuffer[j].view3D.right) {
                NEXUS_Surface_Destroy(server->display[i].framebuffer[j].view3D.right);
            }
            server->display[i].framebuffer[j].surface = NULL;
            server->display[i].framebuffer[j].view3D.right = NULL;
            server->display[i].framebuffer[j].view3D.left = NULL;
        }
    }
set_error:
    return rc;
}

static void nexus_surface_compositor_inactive_timer(void *context)
{
    NEXUS_SurfaceCompositorHandle server = context;
    NEXUS_SurfaceClientHandle client;

    BDBG_MSG(("inactive_timer enabled=%d", server->settings.enabled));
    server->inactiveTimer = NULL;

    if (!server->settings.enabled) {
        /* fire displayed events to all clients who have submitted */
        for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {
            if (client->pending_displayed_callback) {
                client->pending_displayed_callback = false;
                NEXUS_TaskCallback_Fire(client->displayedCallback);
            }
        }

        server->inactiveTimer = NEXUS_ScheduleTimer(50, nexus_surface_compositor_inactive_timer, server);
    }
}

NEXUS_Error NEXUS_SurfaceCompositor_SetSettings( NEXUS_SurfaceCompositorHandle server, const NEXUS_SurfaceCompositorSettings *pSettings )
{
    NEXUS_Error rc = 0;
    bool enable_changed;
    unsigned i;

    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);

    if (server->state.active) {
        for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
            if ((pSettings->display[i].display != server->settings.display[i].display) ||
                BKNI_Memcmp(&pSettings->display[i].framebuffer, &server->settings.display[i].framebuffer, sizeof(pSettings->display[i].framebuffer)) ||
                BKNI_Memcmp(&pSettings->display[i].display3DSettings, &server->settings.display[i].display3DSettings, sizeof(pSettings->display[i].display3DSettings)))
            {
                BDBG_ERR(("you must disable the surface compositor and wait for the inactiveCallback before changing display settings."));
                return BERR_TRACE(NEXUS_NOT_AVAILABLE);
            }
        }
    }

    /* these settings can be set regardless of enabled/active state */
    NEXUS_TaskCallback_Set(server->frameBufferCallback, &pSettings->frameBufferCallback);
    NEXUS_TaskCallback_Set(server->inactiveCallback, &pSettings->inactiveCallback);
    enable_changed = pSettings->enabled != server->settings.enabled;

    if (!server->state.active) {
        rc = nexus_surface_compositor_p_update_display(server, pSettings);
        if (rc) return BERR_TRACE(rc);

        server->state.active = pSettings->enabled;
        if (server->state.active) {
            BDBG_WRN(("resume composition after enabling"));
            nexus_surface_compositor_p_compose(server);
        }
    }
    else {
        /* if active, the only possible change is the enabled flag. */
        server->settings = *pSettings;

        if (enable_changed && !pSettings->enabled) {
            /* starting disabled -> !active process */
            nexus_p_surface_compositor_check_inactive(server);
            if (!server->inactiveTimer) {
                /* kick start timer that keeps clients alive if fmt change takes a long time */
                nexus_surface_compositor_inactive_timer(server);
            }
        }
    }

    if (enable_changed) {
        /* we only have to check enabled flag for client displayStatusChanged because
        no display setting can change while enabled, and no display change matters while disabled */
        NEXUS_SurfaceClientHandle client;
        for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {
            NEXUS_TaskCallback_Fire(client->displayStatusChangedCallback);
        }
    }

    return rc;
}

void NEXUS_SurfaceCompositor_GetClientSettings( NEXUS_SurfaceCompositorHandle server, NEXUS_SurfaceClientHandle client, NEXUS_SurfaceCompositorClientSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);
    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    *pSettings = client->serverSettings;
}

#if 0
static bool nexus_rect_is_not_intersected(const NEXUS_Rect *a, const NEXUS_Rect *b)
{
    return
    (a->x + a->width <= b->x) ||  /* a completely to the left */
    (a->x >= b->x + b->width) ||  /* a completely to the right */
    (a->y + a->height <= b->y) || /* a completely to the top */
    (a->y >= b->y + b->height);    /* a completely to the bottom */
}
#endif

NEXUS_Error NEXUS_SurfaceCompositor_SetClientSettings( NEXUS_SurfaceCompositorHandle server, NEXUS_SurfaceClientHandle client, const NEXUS_SurfaceCompositorClientSettings *pSettings )
{
    NEXUS_SurfaceClientHandle temp, prev = NULL;
    bool resize, move;
    unsigned flags = 0;

    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);
    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);

    if (pSettings->tunnelCapable && server->tunnel.client && server->tunnel.client != client) {
        /* currently, there can only be one tunneled client. it's conceivable that we could support
        more than one, if they are non-overlapping, but it would be complicated. */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* sort server->clients by pSettings->composition.zorder.
    TODO: currently O(1) search, but likely can be optimized. */
    if (client->serverSettings.composition.zorder != pSettings->composition.zorder) {
        flags = NEXUS_P_SURFACECLIENT_UPDATE_ZORDER;
        BLST_Q_REMOVE(&client->server->clients, client,  link);
        for (temp = BLST_Q_FIRST(&client->server->clients); temp; temp = BLST_Q_NEXT(temp, link)) {
            BDBG_ASSERT(temp != client);
            if (temp->serverSettings.composition.zorder >= pSettings->composition.zorder) {
                if (prev) {
                    BLST_Q_INSERT_AFTER(&client->server->clients, prev, client, link);
                }
                else {
                    BLST_Q_INSERT_HEAD(&client->server->clients, client, link);
                }
                break;
            }
            prev = temp;
        }
        if (!temp) {
            if (prev) {
                BLST_Q_INSERT_AFTER(&client->server->clients, prev, client, link);
            }
            else {
                BLST_Q_INSERT_HEAD(&client->server->clients, client, link);
            }
        }
    }

    resize = pSettings->composition.position.width != client->serverSettings.composition.position.width ||
             pSettings->composition.position.height != client->serverSettings.composition.position.height;
    move =   pSettings->composition.position.x != client->serverSettings.composition.position.x ||
             pSettings->composition.position.y != client->serverSettings.composition.position.y;

    /* can't move or resize active tunneled client */
    if( client->state.client_type == client_type_tunnel && (resize || move)) { return BERR_TRACE(NEXUS_NOT_SUPPORTED); }

    flags |= client->serverSettings.composition.visible != pSettings->composition.visible ? NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE : 0;
    if(client->serverSettings.composition.virtualDisplay.width != pSettings->composition.virtualDisplay.width || client->serverSettings.composition.virtualDisplay.height != pSettings->composition.virtualDisplay.height) {
        flags |= NEXUS_P_SURFACECLIENT_UPDATE_CLIENT;
        resize = true;
        move = true;
    }

    /* save settings */
    client->serverSettings = *pSettings;

    nexus_p_surface_compositor_update_client(client, flags | (resize?NEXUS_P_SURFACECLIENT_UPDATE_SIZE:0)|(move?NEXUS_P_SURFACECLIENT_UPDATE_POSITION:0));

    return NEXUS_SUCCESS;
}

static void nexus_surface_compositor_p_setvideo(NEXUS_SurfaceCompositorHandle server)
{
    unsigned i;
    for (i=0;i<NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS;i++) {
        NEXUS_SurfaceClientHandle client = server->windowClient[i];
        if (client) {
            nexus_surfaceclient_p_setvideo(client);
        }
    }
}

/*
pInputCoord = input coordinates
pOutputCoord = output coordinates
pRect = [in/out] param to be converted from inputCoordinates to outputCoordinates
*/
void nexus_surfacemp_p_convert_coord(const NEXUS_SurfaceRegion *pInputCoord, const NEXUS_SurfaceRegion *pOutputCoord, const NEXUS_Rect *pSrcRect, NEXUS_Rect *pDstRect)
{
    BDBG_ASSERT((const void *)pSrcRect != (const void *)pDstRect);
    pDstRect->x = pSrcRect->x * (int)pOutputCoord->width / (int)pInputCoord->width;
    pDstRect->width = pSrcRect->width * pOutputCoord->width / pInputCoord->width;
    pDstRect->y = pSrcRect->y * (int)pOutputCoord->height / (int)pInputCoord->height;
    pDstRect->height = pSrcRect->height * pOutputCoord->height / pInputCoord->height;
    return;
}

void nexus_surfacemp_p_clip_rect(const NEXUS_Rect *pBound, const NEXUS_Rect *pSrcRect, NEXUS_Rect *pDstRect)
{
    int inc;
    BDBG_ASSERT((const void *)pSrcRect != (const void *)pDstRect);

    if (pSrcRect->x >= pBound->x + pBound->width ||
        pSrcRect->y >= pBound->y + pBound->height ||
        pSrcRect->x + pSrcRect->width < pBound->x ||
        pSrcRect->y + pSrcRect->height < pBound->y)
    {
        pDstRect->y = pBound->y;
        pDstRect->x = pBound->x;
        pDstRect->width = pDstRect->height = 0;
        return;
    }

    inc = pSrcRect->x < pBound->x ? (pBound->x - pSrcRect->x) : 0;
    pDstRect->width = pSrcRect->width - inc;
    pDstRect->x = pSrcRect->x + inc;

    inc = pSrcRect->y < pBound->y ? (pBound->y - pSrcRect->y) : 0;
    pDstRect->height = pSrcRect->height - inc;
    pDstRect->y = pSrcRect->y + inc;

    if (pDstRect->x + pDstRect->width > pBound->x + pBound->width) {
        pDstRect->width = pBound->x + pBound->width - pDstRect->x;
    }
    if (pDstRect->y + pDstRect->height > pBound->y + pBound->height) {
        pDstRect->height = pBound->y + pBound->height - pDstRect->y;
    }
    return;
}

/* SrcClipRect is the clipped version of pSrcRect , finds pDstClipRect such that proportions between pDstClipRect and pSrcClipRect are the same as between pSrcRect and pDstRect */
void nexus_surfacemp_scale_clipped_rect(const NEXUS_Rect *pSrcRect, const NEXUS_Rect *pSrcClipRect, const NEXUS_Rect *pDstRect, NEXUS_Rect *pDstClipRect)
{
    int inc;
    BDBG_ASSERT((const void *)pDstRect != (const void *)pDstClipRect);

    /* pSrcClipRect must be within pSrcRect */
    BDBG_ASSERT(pSrcClipRect->x >= pSrcRect->x);
    BDBG_ASSERT(pSrcClipRect->x + pSrcClipRect->width <= pSrcRect->x + pSrcRect->width);
    BDBG_ASSERT(pSrcClipRect->y >= pSrcRect->y);
    BDBG_ASSERT(pSrcClipRect->y + pSrcClipRect->height <= pSrcRect->y + pSrcRect->height);

    inc = pSrcClipRect->x - pSrcRect->x;
    pDstClipRect->x = pDstRect->x + inc * pDstRect->width / pSrcRect->width;
    pDstClipRect->width = pSrcClipRect->width * pDstRect->width / pSrcRect->width;

    inc = pSrcClipRect->y - pSrcRect->y;
    pDstClipRect->y = pDstRect->y + inc * pDstRect->height / pSrcRect->height;
    pDstClipRect->height = pSrcClipRect->height * pDstRect->height / pSrcRect->height;

    return;
}

static void nexus_surfaceclient_p_realloc_display_cache(NEXUS_SurfaceClientHandle client)
{
    struct NEXUS_SurfaceCompositorDisplay *cmpDisplay;
    bool required;
    NEXUS_PixelFormat framebufferFormat;

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    BDBG_OBJECT_ASSERT(client->server, NEXUS_SurfaceCompositor);

    cmpDisplay = &client->server->display[0];
    framebufferFormat = client->server->settings.display[0].framebuffer.pixelFormat;
    required = cmpDisplay->display && client->serverSettings.composition.displayCache && client->state.client_type == client_type_set;
    if(client->state.cache.surface) {
        if(!required ||  !(client->state.framebufferRect.width == client->state.cache.width) || (client->state.framebufferRect.height == client->state.cache.height) || (client->state.cache.format == framebufferFormat)) {
            NEXUS_Surface_Destroy(client->state.cache.surface);
            client->state.cache.surface = NULL;
        }
    }
    if(required && client->state.cache.surface==NULL) {
        NEXUS_SurfaceCreateSettings createSettings;
        NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
        createSettings.width = client->state.framebufferRect.width;
        createSettings.height = client->state.framebufferRect.height;
        createSettings.pixelFormat = framebufferFormat;

        BDBG_MSG_TRACE(("create display cache [%p] %dx%d -> %dx%d", client,
            client->set.serverCreateSettings.width, client->set.serverCreateSettings.height,
            createSettings.width, createSettings.height));
        client->state.cache.surface = NEXUS_Surface_Create(&createSettings);
        client->state.cache.needs_update = true;
    }
    return ;
}

static NEXUS_Error nexus_surfaceclient_p_update_display_cache(NEXUS_SurfaceClientHandle client, bool force)
{
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(client, NEXUS_SurfaceClient);
    if (!client->serverSettings.composition.displayCache || !client->set.serverSurface) {
        return 0;
    }

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    if (client->state.cache.surface) {
        if (force || client->state.cache.needs_update) {
            blitSettings.source.surface = client->set.serverSurface;
            blitSettings.output.surface = client->state.cache.surface;
            /* always a full-surface blit to update the display cache */
            BDBG_MSG_TRACE(("blit [%p] to display.cache.surface %p->%p", client,
                blitSettings.source.surface, blitSettings.output.surface));
            rc = NEXUS_Graphics2D_Blit(client->server->gfx, &blitSettings);
            if (rc) return BERR_TRACE(rc);

            client->state.cache.needs_update = false;
        }
    }
    return NEXUS_SUCCESS;
}

NEXUS_Error nexus_surface_compositor_p_verify_tunnel(NEXUS_SurfaceCompositorHandle server)
{
    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);

    if (server->tunnel.client) {
        /* disable test for now, tunnel could work with only 2 framebuffers,
         * but it would be limited to the single client, only. 3 buffers would
         * allow multiclient - but with stall in a pipeline
         * Perhaps test should be excercised when  */
#if 0
        NEXUS_SurfaceClientHandle temp;
        unsigned i;


        temp = BLST_S_FIRST(&server->clients);
        BDBG_ASSERT(temp);
        if (BLST_S_NEXT(temp, link) && server->display[0].num_framebuffers < 3) {
            BDBG_ERR(("tunneling with multiple clients requires 3 or 4 framebuffers"));
            return BERR_TRACE(NEXUS_NOT_AVAILABLE);
        }
#endif
    }

    return NEXUS_SUCCESS;
}

static void nexus_surface_compositor_add_render_element(NEXUS_SurfaceCompositorHandle server, NEXUS_P_SurfaceCompositorRenderElements *elements, NEXUS_SurfaceClientHandle client, bool left)
{
    BSTD_UNUSED(server);
    BDBG_MSG_TRACE(("add_render_element:%p [%u] client:%p surface:%p", elements->count, server, client, client->state.current));
    if(elements->count < NEXUS_P_SURFACECMP_MAX_SURFACES) {
        NEXUS_P_SurfaceCompositorRenderElement *data = elements->data + elements->count;
        elements->count++;
        data->sourceSurface = client->state.current.surface;
        data->colorBlend = client->serverSettings.composition.colorBlend;
        data->alphaBlend = client->serverSettings.composition.alphaBlend;
        data->constantColor = client->serverSettings.composition.constantColor;
        data->colorMatrixEnabled = client->serverSettings.composition.colorMatrixEnabled;
        if (data->colorMatrixEnabled) {
            data->colorMatrix = client->serverSettings.composition.colorMatrix;
        }
        if(left) {
            data->outputRect = client->state.left.outputRect;
            data->sourceRect = client->state.left.sourceRect;
            if(client->state.client_type==client_type_push || client->state.client_type == client_type_tunnel_emulated) {
                NEXUS_SURFACECLIENT_P_SURFACE_VERIFY(client, &client->state.current);
            }
        } else {
            data->outputRect = client->state.right.outputRect;
            data->sourceRect = client->state.right.sourceRect;
        }
    } else {
        if(elements->count == NEXUS_P_SURFACECMP_MAX_SURFACES) { /* print it only once */
            elements->count++;
            BDBG_WRN(("nexus_surface_compositor_add_render_element:%#lx overflow of render elements table(%u)", (unsigned long)elements, NEXUS_P_SURFACECMP_MAX_SURFACES));
        }
    }
    return;
}

static void nexus_surface_compositor_p_compose_framebuffer(NEXUS_SurfaceCompositorHandle server, struct NEXUS_SurfaceCompositorDisplay *cmpDisplay, struct NEXUS_SurfaceCompositorFramebuffer *framebuffer)
{
    NEXUS_SurfaceClientHandle client;

    BDBG_ASSERT(cmpDisplay->display);

    /* if a surface is full screen and non-opaque, we can skip rendering anything below it, including the background.
    the general principle is that we can skip rendering any surface that is fully concealed. */

    BDBG_MSG_TRACE(("compose_framebuffer:%#lx %#lx", (unsigned long)server, (unsigned long)cmpDisplay));

    framebuffer->generation = cmpDisplay->generation;

    for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {
        BDBG_MSG_TRACE(("compose:%#lx client:%#lx", (unsigned long)server, (unsigned long)client));
        BDBG_MSG_TRACE(("compose_framebuffer:%#lx %#lx client:%#lx:%u:%u %s (%d,%d,%u,%u)", (unsigned long)server, (unsigned long)cmpDisplay, (unsigned long)client, client->client_id, client->state.client_type, client->state.left.visible==0?"OFFSCREEN":"", client->state.left.outputRect.x, client->state.left.outputRect.y, client->state.left.outputRect.width, client->state.left.outputRect.height));
        client->process_pending_displayed_callback = client->pending_displayed_callback;

        if(client->state.client_type==client_type_push || client->state.client_type == client_type_tunnel_emulated) {
            client->queue.compositing = true;
        }
        
        if(client->set.dirty) {
            /* even if hidden, we must blit */
            nexus_surface_compositor_p_update_dirty_client(client);
        }
        
        if(client->state.left.hidden && client->state.right.hidden) {
            continue;
        }
        if(client->state.client_type == client_type_tunnel) {
            BDBG_ASSERT(server->tunnel.client == client);
            if( ((client->state.current.surface == framebuffer->tunnel.surface.surface) /* don't composite to itself */ ||
               (framebuffer->tunnel.pristine && !server->tunnel.overlapped) /* already rendered */)) {
                BDBG_MSG_TRACE(("compose:%p reusing framebuffer:%p for tunnel client:%p ", server, framebuffer, client));
                server->renderState.tunnel.active = true;
                server->renderState.tunnel.rect = client->state.left.sourceRect;
                    /* do nothing */
            } else {
                BDBG_MSG_TRACE(("compose:%p copying surface:%p to framebuffer:%p for tunnel client:%p ", server, client->state.current, framebuffer, client));
                client->state.left.outputRect = client->state.framebufferRect;
                switch(client->settings.orientation) {
                default:
                case NEXUS_VideoOrientation_e2D:
                    break;
                case NEXUS_VideoOrientation_e3D_LeftRight:
                    client->state.left.outputRect.width *= 2;
                    break;
                case NEXUS_VideoOrientation_e3D_OverUnder:
                    client->state.left.outputRect.height *= 2;
                    break;
                }
                client->state.left.sourceRect.width = client->state.left.outputRect.width;
                client->state.left.sourceRect.height = client->state.left.outputRect.height;
                client->state.left.sourceRect.x = client->state.left.sourceRect.y = 0;
                nexus_surface_compositor_add_render_element(server, &server->renderState.elements, client, true);
            }
           continue;
        } else if(client->state.update_flags) { /* there were some changes, update current surface and sourceRect */
            NEXUS_Surface_P_ClientSurface surface; 
            NEXUS_Rect sourceRect;
            unsigned right_x_offset = 0;
            unsigned right_y_offset = 0;

            BDBG_ASSERT(client->state.left.visible || client->state.right.visible);
            NEXUS_SURFACECLIENT_P_SURFACE_INIT(&surface);

            client->state.update_flags = 0;
            if(client->published.surface) {
                surface.surface = client->published.surface;
            } else {
                switch(client->state.client_type) {
                case client_type_set:
                    BDBG_ASSERT(client->set.serverSurface);
                    surface.surface = client->set.serverSurface;
                    break;
                case client_type_tunnel_emulated:
                case client_type_push:
                    {
                        NEXUS_SurfaceCreateSettings createSettings;
                        const NEXUS_SurfaceCompositor_P_PushElement *first_node = BLST_SQ_FIRST(&client->queue.push);

                        BDBG_ASSERT(first_node); /* for push clients, queue should be unempty, for tunnel_emulated clients, if queue empty then visible is false */
                        surface = first_node->surface;
                        NEXUS_Surface_GetCreateSettings(surface.surface, &createSettings);
                        client->state.clientRegion.width = createSettings.width;
                        client->state.clientRegion.height = createSettings.height;
                    }
                    break;
                case client_type_tunnel:
                default:
                    BDBG_ASSERT(0);
                    surface.surface = NULL;
                    break;
                }
            }
            BDBG_ASSERT(surface.surface);
            client->state.current = surface;
            sourceRect.x = sourceRect.y = 0;
            sourceRect.width = client->state.clientRegion.width;
            sourceRect.height = client->state.clientRegion.height;
            switch(client->settings.orientation) {
            default:
            case NEXUS_VideoOrientation_e2D:
                break;
            case NEXUS_VideoOrientation_e3D_LeftRight:
                if(sourceRect.width%2) {
                    BDBG_WRN(("client:%p:%u unsupported 3D LeftRight width:%u", client, client->client_id, sourceRect.width));
                }
                sourceRect.width/=2;
                right_x_offset = sourceRect.width;
                break;
            case NEXUS_VideoOrientation_e3D_OverUnder:
                if(sourceRect.height%2) {
                    BDBG_WRN(("client:%p:%u unsupported 3D OverUnder height:%u", client, client->client_id, sourceRect.height));
                }
                sourceRect.height/=2;
                right_y_offset = sourceRect.height;
                break;
            }
            if(!client->state.left.hidden) {
                nexus_surfacemp_scale_clipped_rect(&client->state.framebufferRect, &client->state.left.outputRect, &sourceRect, &client->state.left.sourceRect);
                BDBG_MSG_TRACE(("%p: client:%p:%u (%d,%d,%u,%u)[left]", server, client, client->client_id, client->state.client_type, client->state.left.sourceRect.x, client->state.left.sourceRect.y, client->state.left.sourceRect.width, client->state.left.sourceRect.height));
            }
            if(!client->state.right.hidden && (cmpDisplay->formatInfo.orientation==NEXUS_VideoOrientation_e3D_LeftRight || NEXUS_VideoOrientation_e3D_OverUnder)) {
                NEXUS_Rect outputRect; /* we need to shift outputRect to 0 */
                NEXUS_Rect framebufferRect; /* we need to account for rightViewOffset */
                BDBG_ASSERT(client->state.right.outputRect.x >= cmpDisplay->offset3DRight.x);
                BDBG_ASSERT(client->state.right.outputRect.y >= cmpDisplay->offset3DRight.y);
                outputRect.width = client->state.right.outputRect.width;
                outputRect.height = client->state.right.outputRect.height;
                outputRect.x = client->state.right.outputRect.x - cmpDisplay->offset3DRight.x;
                outputRect.y = client->state.right.outputRect.y - cmpDisplay->offset3DRight.y;
                framebufferRect.width = client->state.framebufferRect.width;
                framebufferRect.height = client->state.framebufferRect.height;
                framebufferRect.y = client->state.framebufferRect.y;
                framebufferRect.x = client->state.framebufferRect.x + client->state.rightViewOffset;
                nexus_surfacemp_scale_clipped_rect(&framebufferRect, &outputRect, &sourceRect, &client->state.right.sourceRect);
                /* and shift client rectangle */
                client->state.right.sourceRect.x += right_x_offset;
                client->state.right.sourceRect.y += right_y_offset;
                BDBG_MSG_TRACE(("%p:client:%p:%u (%d,%d,%u,%u)[right]", server,  client, client->client_id, client->state.client_type, client->state.left.sourceRect.x, client->state.right.sourceRect.y, client->state.right.sourceRect.width, client->state.right.sourceRect.height));
            }
        }
        BDBG_ASSERT(client->state.current.surface);
        if(!client->state.left.hidden) {
            nexus_surface_compositor_add_render_element(server, &server->renderState.elements, client, true);
        }
        if(!client->state.right.hidden) {
            nexus_surface_compositor_add_render_element(server, &server->renderState.elements, client, false);
        }
    }
    framebuffer->tunnel.pristine = !server->tunnel.overlapped;
    return;
}

/* returm true if b is completely within a */
static bool nexus_p_surface_rect_is_bound(const NEXUS_Rect *a, const NEXUS_Rect *b)
{
    return
    (b->x >= a->x) && (b->x + b->width <= a->x + a->width) &&
    (b->y >= a->y) && (b->y + b->height <= a->y + a->height);
}


/* return true if client is opaque, e.g. it would override (not blend) anything that drawn behind it */
static bool nexus_p_surface_client_opaque(NEXUS_SurfaceClientHandle client)
{
    return nexus_p_surface_composition_opaque(&client->serverSettings.composition.colorBlend, &client->serverSettings.composition.alphaBlend);
}

static void nexus_surface_compositor_p_update_hidden_clients(NEXUS_SurfaceCompositorHandle server)
{
    NEXUS_SurfaceClientHandle client;
    const struct NEXUS_SurfaceCompositorDisplay *cmpDisplay = &server->display[0];

    if(server->state.update_flags & (NEXUS_P_SURFACECLIENT_UPDATE_DISPLAY | NEXUS_P_SURFACECLIENT_UPDATE_SIZE | NEXUS_P_SURFACECLIENT_UPDATE_POSITION | NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE | NEXUS_P_SURFACECLIENT_UPDATE_CLIENT)) {
        BDBG_MSG_TRACE(("update_hidden_clients:%p looking for hidden clients", server));
        /* update hidden flag for all clients */
        for (client=BLST_Q_LAST(&server->clients); client; client = BLST_Q_PREV(client, link)) {
            NEXUS_SurfaceClientHandle prev;

            client->state.left.hidden = true;
            client->state.right.hidden = true;
            if(client->state.left.visible) {
                client->state.left.hidden = false;
                /* then look for all clients that are on top and verify that they aren't completely masking current client */
                for(prev=BLST_Q_NEXT(client,link); prev ; prev=BLST_Q_NEXT(prev, link)) {
                    if(prev->state.left.hidden) {
                        continue;
                    }
                    if(nexus_p_surface_rect_is_bound(&prev->state.left.outputRect, &client->state.left.outputRect) && nexus_p_surface_client_opaque(prev)) {
                        BDBG_MSG_TRACE(("compose:%p client %p (%d,%d,%u,%u) hides client %p (%d,%d,%u,%u) left", server, prev, prev->state.left.outputRect.x, prev->state.left.outputRect.y, prev->state.left.outputRect.width, prev->state.left.outputRect.height, client, client->state.left.outputRect.x, client->state.left.outputRect.y, client->state.left.outputRect.width, client->state.left.outputRect.height));
                        client->state.left.hidden = true;
                        break;
                    }
                }
            }
            if(client->state.right.visible && (cmpDisplay->formatInfo.orientation==NEXUS_VideoOrientation_e3D_LeftRight || NEXUS_VideoOrientation_e3D_OverUnder)) {
                client->state.right.hidden = false;
                /* then look for all clients that are on top and verify that they aren't completely masking current client */
                for(prev=BLST_Q_NEXT(client,link); prev ; prev=BLST_Q_NEXT(prev, link)) {
                    if(prev->state.right.hidden) {
                        continue;
                    }
                    if(nexus_p_surface_rect_is_bound(&prev->state.right.outputRect, &client->state.right.outputRect) && nexus_p_surface_client_opaque(prev)) {
                        BDBG_MSG_TRACE(("compose:%p client %p (%d,%d,%u,%u) hides client %p (%d,%d,%u,%u)", server, prev, prev->state.right.outputRect.x, prev->state.right.outputRect.y, prev->state.right.outputRect.width, prev->state.right.outputRect.height, client, client->state.right.outputRect.x, client->state.right.outputRect.y, client->state.right.outputRect.width, client->state.right.outputRect.height));
                        client->state.right.hidden = true;
                        break;
                    }
                }
            }
        }
        if(server->tunnel.client) {
            /* find first rendered client that is not tunnel */
            for (client=BLST_Q_LAST(&server->clients); client; client = BLST_Q_PREV(client, link)) {
                if(client == server->tunnel.client) {
                    continue;
                }
                if( !client->state.right.hidden || !client->state.left.hidden) {
                    break;
                }
            }
            server->tunnel.overlapped = client != NULL;
            if(server->tunnel.overlapped) {
                BDBG_MSG_TRACE(("update_hidden_clients:%p tunnel client:%p(%u) overlapped by client:%p(%u)", server, server->tunnel.client, server->tunnel.client->client_id, client, client->client_id));
            }
        }
    }
    return;
}

/* compose all framebuffers */
void nexus_surface_compositor_p_compose(NEXUS_SurfaceCompositorHandle server)
{
    struct NEXUS_SurfaceCompositorDisplay *cmpDisplay = &server->display[0];

    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);

    if (!server->settings.enabled) {
        /* short circuit composition triggered by client activity */
        return;
    }

    BDBG_MSG_TRACE(("compose:%#lx", (unsigned long)server));
    if(cmpDisplay->display==NULL || server->settings.display[0].display==NULL) {
        return;
    }
    if(cmpDisplay->num_framebuffers==1) { /* if there is a single framebuffer, then rendering to currently displayed buffer is allowed */
        nexus_surface_compositor_p_recycle_displayed_framebuffer(cmpDisplay);
    }
    /* 1. Check if all steps are completed */
    if (cmpDisplay->compositing || cmpDisplay->composited || cmpDisplay->submitted) {
        /* framebuffer callback is pending, so we can update then */
        BDBG_MSG_TRACE(("compose:%#lx -> compositing:%#lx composited:%#lx submitted:%#lx", (unsigned long)server, (unsigned long)cmpDisplay->compositing, (unsigned long)cmpDisplay->composited, (unsigned long)cmpDisplay->submitted));
        server->pending_update = true;
        return;
    }
    if(!nexus_surface_compositor_p_blitter_acquire(server, NEXUS_P_SURFACEBLITTER_COMPOSITOR)) {
        BDBG_MSG_TRACE(("compose:%p blitter busy", server));
        return;
    }

    server->pending_update = false;

    if (server->pending_setvideo) {
        nexus_surface_compositor_p_setvideo(server);
        server->pending_setvideo = false;
    }

    if(server->state.update_flags) {
        nexus_surface_compositor_p_update_hidden_clients(server);
        server->state.update_flags = 0;
    }

    server->renderState.tunnel.rect.width = 0;
    server->renderState.tunnel.rect.height = 0;
    server->renderState.tunnel.active = false;
    server->renderState.tunnel.tunnelSource = NULL;

    /* 2.  select framebuffer to draw */
    {
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer;
        bool tunnelActive = server->tunnel.client && !server->tunnel.client->state.left.hidden;
        struct NEXUS_SurfaceCompositorFramebuffer *tunnelSource = NULL;
        struct NEXUS_SurfaceCompositorFramebuffer *tunnelDest = NULL;

        tunnelSource = framebuffer = BLST_Q_FIRST(&cmpDisplay->tunnel.submitted);
        if(tunnelActive && framebuffer && !server->tunnel.overlapped) {
            /* render into client provided framebuffer */
            BDBG_MSG_TRACE(("compose:%#lx rendering into tunnel framebuffer:%#lx", (unsigned long)server, (unsigned long)framebuffer));
            tunnelDest = framebuffer;
            BLST_Q_REMOVE_HEAD(&cmpDisplay->tunnel.submitted, tunnelLink); /* would be added  into available by nexus_surface_compositor_p_recycle_displayed_framebuffer */
        } else {
            framebuffer = BLST_SQ_FIRST(&cmpDisplay->available);
            if(framebuffer) {
                BDBG_MSG_TRACE(("compose:%p rendering into framebuffer:%p", server, framebuffer));
                BLST_SQ_REMOVE_HEAD(&cmpDisplay->available, link);
                if(tunnelActive) {
                    if(tunnelSource ) {
                        BLST_Q_REMOVE_HEAD(&cmpDisplay->tunnel.submitted, tunnelLink); /* would be added  into available by nexus_surface_compositor_compositing_completed */
                        server->renderState.tunnel.tunnelSource = tunnelSource;
                    } else {
                        if(tunnelActive && !server->tunnel.overlapped) { /* if no new tunnel buffer, just skip rendering */
                            BDBG_MSG_TRACE(("compose:%p skip rendering due to idle tunnel client %p", server, server->tunnel.client));
                            BLST_SQ_INSERT_HEAD(&cmpDisplay->available, framebuffer, link); /* push framebuffer back */
                            framebuffer = NULL;
                            goto framebuffer_done;
                        }
                        /* look in currently displayed framebuffer */
                        if(cmpDisplay->displaying && cmpDisplay->displaying->tunnel.pristine) {
                            tunnelSource = cmpDisplay->displaying;
                        } else {
                            /* and then in list of avaliable surfaces */
                            for(tunnelSource=BLST_Q_FIRST(&cmpDisplay->tunnel.available);tunnelSource;tunnelSource=BLST_Q_NEXT(tunnelSource, tunnelLink)) {
                                if(tunnelSource->tunnel.pristine) {
                                    break;
                                }
                            }
                        }
                    }
                    BDBG_MSG_TRACE(("compose:%p rendering into framebuffer:%p from tunnel:%p", server, framebuffer, tunnelSource));
                }
            }
        }
framebuffer_done:
        if(framebuffer) {
            cmpDisplay->compositing = framebuffer;
            framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eCompositing;
        } else {
            /* undo */
            cmpDisplay->compositing = NULL;
            server->pending_update = true;
            nexus_surface_compositor_p_blitter_release(server, NEXUS_P_SURFACEBLITTER_COMPOSITOR);
            return;
        }
        if(server->tunnel.client) {
            BDBG_MSG_TRACE(("compose:%p: tunnelSource:%p(%p,%p) dest:%p", server, tunnelSource, BLST_Q_FIRST(&cmpDisplay->tunnel.submitted), server->renderState.tunnel.tunnelSource, framebuffer));
            NEXUS_SURFACECLIENT_P_SURFACE_INIT(&server->tunnel.client->state.current);
            if(tunnelSource) {
                NEXUS_SURFACECLIENT_P_SURFACE_VERIFY(server->tunnel.client, &tunnelSource->tunnel.surface);
                server->tunnel.client->state.current.surface = tunnelSource->tunnel.surface.surface;
            } else {
                server->tunnel.client->state.current.surface = NULL;
            }
        }
   }

    /* 3. Composite master display */
    BTRC_TRACE(surface_compositor_composite,START);
    {
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = cmpDisplay->compositing;
        BDBG_ASSERT(framebuffer);
        framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eCompositing;
        server->renderState.elements.count = 0;
        server->renderState.step = taskInit;
        nexus_surface_compositor_p_compose_framebuffer(server, cmpDisplay, framebuffer);
        cmpDisplay->generation++;
        /* 4. render framebuffer for master display */
        nexus_surface_compositor_p_render_framebuffer(server, cmpDisplay, framebuffer);
    }
    BTRC_TRACE(surface_compositor_composite,STOP);
    return;
}


static void nexus_p_surface_compositor_update(NEXUS_SurfaceCompositorHandle server, unsigned flags)
{
    server->state.update_flags |= flags;
    nexus_surface_compositor_p_compose(server);
    return;
}


void nexus_p_surface_compositor_update_virtual_display(const struct NEXUS_SurfaceCompositorDisplay *display, const NEXUS_SurfaceComposition *composition, NEXUS_SurfaceRegion *virtualDisplay)
{
    virtualDisplay->width = composition->virtualDisplay.width ? composition->virtualDisplay.width: display->canvas.width;
    virtualDisplay->height = composition->virtualDisplay.height ? composition->virtualDisplay.height : display->canvas.height;
    return;
}

static void nexus_surface_compositor_p_update_dirty_client(NEXUS_SurfaceClientHandle client)
{
    NEXUS_Error rc;
    NEXUS_Graphics2DBlitSettings blitSettings;
    BDBG_ASSERT(client->state.client_type == client_type_set);
    BDBG_ASSERT(client->set.dirty);
    BDBG_ASSERT(client->set.surface.surface);
    BDBG_ASSERT(client->set.serverSurface);
    BDBG_ASSERT(!client->set.updating); /* can't have another transaction in flight */

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = client->set.surface.surface;
    blitSettings.output.surface = client->set.serverSurface;
    blitSettings.source.rect = client->set.dirtyRect;
    blitSettings.output.rect = client->set.dirtyRect;
    NEXUS_SURFACECLIENT_P_SURFACE_VERIFY(client, &client->set.surface);
    rc = NEXUS_Graphics2D_Blit(client->server->gfx, &blitSettings);
    /* TODO: if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) {
        cannot block for packetSpaceAvailable.
        must keep track of which blits are completed.
        in the packetSpaceAvailable callback, resume work.
        }
   */
    rc = nexus_surfaceclient_p_update_display_cache(client, false);
    client->set.updating = true;
    return;
}

void nexus_surface_compositor_p_update_dirty_clients(NEXUS_SurfaceCompositorHandle server, NEXUS_SurfaceClientHandle client)
{
    if(!nexus_surface_compositor_p_blitter_acquire(server, NEXUS_P_SURFACEBLITTER_CLIENT)) {
        return;
    }

    if(client!=NULL) {
        /* update this client */
        nexus_surface_compositor_p_update_dirty_client(client);
    } else {
        /* update all clients */
        for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {
            if(client->state.client_type == client_type_set && client->set.dirty) {
                nexus_surface_compositor_p_update_dirty_client(client);
            }
        }
    }
    nexus_surface_compositor_p_blitter_start(server, NEXUS_P_SURFACEBLITTER_CLIENT);
    return;
}

void nexus_surface_compositor_p_update_dirty_clients_done(NEXUS_SurfaceCompositorHandle server)
{
    NEXUS_SurfaceClientHandle client;
    for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {
        if(client->state.client_type == client_type_set && client->set.updating) {
            BDBG_ASSERT(client->set.dirty); /* dirty should be set */
            client->set.dirty = false;
            client->set.updating = false;
            NEXUS_TaskCallback_Fire(client->recycledCallback);
        }
    }
}

static void nexus_p_surface_compositor_update_single_client(NEXUS_SurfaceClientHandle client, unsigned flags)
{
    NEXUS_SurfaceCompositorHandle server = client->server;

    BDBG_MSG_TRACE(("update_client:%#lx:%u:%u %s%s%s%s%s%s%s%s",  (unsigned long)client, client->client_id, client->state.client_type, flags&NEXUS_P_SURFACECLIENT_UPDATE_ZORDER?"zorder ":"", flags&NEXUS_P_SURFACECLIENT_UPDATE_SIZE?"size ":"", flags&NEXUS_P_SURFACECLIENT_UPDATE_POSITION?"position ":"", flags&NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE?"visible ":"", flags&NEXUS_P_SURFACECLIENT_UPDATE_SOURCE?"source ":"", flags&NEXUS_P_SURFACECLIENT_UPDATE_COMPOSITION?"composition ":"", flags&NEXUS_P_SURFACECLIENT_UPDATE_DISPLAY?"display ":"", flags&NEXUS_P_SURFACECLIENT_UPDATE_PUBLISH?"publish":""));
    client->state.update_flags |= flags;
    if(flags & (NEXUS_P_SURFACECLIENT_UPDATE_DISPLAY | NEXUS_P_SURFACECLIENT_UPDATE_CLIENT)) {
        nexus_p_surface_compositor_update_virtual_display(&server->display[0], &client->serverSettings.composition, &client->state.virtualDisplay);
    }

    if(flags & (NEXUS_P_SURFACECLIENT_UPDATE_SIZE | NEXUS_P_SURFACECLIENT_UPDATE_DISPLAY | NEXUS_P_SURFACECLIENT_UPDATE_CLIENT)) {
    /* update client cache */
    }

    if(flags & (NEXUS_P_SURFACECLIENT_UPDATE_DISPLAY | NEXUS_P_SURFACECLIENT_UPDATE_SIZE | NEXUS_P_SURFACECLIENT_UPDATE_POSITION | NEXUS_P_SURFACECLIENT_UPDATE_CLIENT)) {
        NEXUS_SurfaceClientHandle child;
        NEXUS_Rect framebuffer;
        /* verify if window has to be repositioned */
        for (child = BLST_S_FIRST(&client->children); child; child = BLST_S_NEXT(child, child_link)) {
            if (child->type == NEXUS_SurfaceClient_eVideoWindow) {
                server->pending_setvideo = true; /* will be resolved in nexus_surface_compositor_p_compose */
                break;
            }
        }

        framebuffer.x = framebuffer.y = 0;
        framebuffer.width = server->display[0].canvas.width;
        framebuffer.height = server->display[0].canvas.height;
        nexus_surfacemp_p_convert_coord(&client->state.virtualDisplay, &server->display[0].canvas, &client->serverSettings.composition.position, &client->state.framebufferRect);
        nexus_surfacemp_p_clip_rect(&framebuffer, &client->state.framebufferRect, &client->state.left.outputRect);
        BDBG_MSG_TRACE(("update_client:%#lx:%u:%u (%u,%u) (%d,%d,%u,%u) (%d,%d,%u,%u) left", (unsigned long)client, client->client_id, client->state.client_type, client->state.virtualDisplay.width, client->state.virtualDisplay.height, client->serverSettings.composition.position.x, client->serverSettings.composition.position.y, client->serverSettings.composition.position.width, client->serverSettings.composition.position.height, client->state.left.outputRect.x, client->state.left.outputRect.y, client->state.left.outputRect.width, client->state.left.outputRect.height));
        if(server->display[0].formatInfo.orientation==NEXUS_VideoOrientation_e3D_LeftRight || server->display[0].formatInfo.orientation==NEXUS_VideoOrientation_e3D_OverUnder) {
            NEXUS_Rect framebufferRect; /* we need to account for rightViewOffset */
            client->state.rightViewOffset = (client->serverSettings.composition.rightViewOffset * client->state.virtualDisplay.width) /  server->display[0].canvas.width;
            /* moove framebufferRect to according to the rightViewOffset */
            framebufferRect.width = client->state.framebufferRect.width;
            framebufferRect.height = client->state.framebufferRect.height;
            framebufferRect.y = client->state.framebufferRect.y;
            framebufferRect.x = client->state.framebufferRect.x + client->state.rightViewOffset;
            /* clip it to the framebufferRect */
            nexus_surfacemp_p_clip_rect(&framebuffer, &framebufferRect, &client->state.right.outputRect);
            client->state.right.outputRect.x += server->display[0].offset3DRight.x;
            client->state.right.outputRect.y += server->display[0].offset3DRight.y;
            BDBG_MSG_TRACE(("update_client:%#lx:%u:%u (%u,%u) (%d,%d,%u,%u) (%d/%d) (%d,%d,%u,%u) right", (unsigned long)client, client->client_id, client->state.client_type, client->state.virtualDisplay.width, client->state.virtualDisplay.height, client->serverSettings.composition.position.x, client->serverSettings.composition.position.y, client->serverSettings.composition.position.width, client->serverSettings.composition.position.height, client->serverSettings.composition.rightViewOffset, client->state.rightViewOffset, client->state.right.outputRect.x, client->state.left.outputRect.y, client->state.right.outputRect.width, client->state.right.outputRect.height));
        } else {
            client->state.right.outputRect.width = client->state.right.outputRect.height = 0;
        }
    }

    if(flags & (NEXUS_P_SURFACECLIENT_UPDATE_DISPLAY | NEXUS_P_SURFACECLIENT_UPDATE_SIZE | NEXUS_P_SURFACECLIENT_UPDATE_POSITION | NEXUS_P_SURFACECLIENT_UPDATE_VISIBLE | NEXUS_P_SURFACECLIENT_UPDATE_CLIENT)) {
        nexus_surfaceclient_p_realloc_display_cache(client);
        if(!client->serverSettings.composition.visible || client->state.client_type == client_type_idle ||
                ((client->state.client_type == client_type_tunnel || client->state.client_type == client_type_tunnel_emulated)&& !client->tunnel.visible) ) {
            client->state.left.visible = false;
            client->state.right.visible = false;
        } else {
            client->state.left.visible = client->state.left.outputRect.width != 0 && client->state.left.outputRect.height != 0;
            client->state.right.visible = client->state.right.outputRect.width != 0 && client->state.right.outputRect.height != 0;
        }
        BDBG_MSG_TRACE(("visible:%#p:%u %s %s", client, client->client_id, client->state.left.visible?"left":"", client->state.right.visible?"right":""));
        client->state.current.surface = NULL;
    }

    if(flags & NEXUS_P_SURFACECLIENT_UPDATE_SOURCE) {
        BDBG_ASSERT(!(flags & NEXUS_P_SURFACECLIENT_UPDATE_PUBLISH)); /* SOURCE and PUBLISH can't be set simultaneously */
        client->published.surface = NULL;
        client->pending_displayed_callback = true;
        switch(client->state.client_type) {
        case client_type_set:
            client->set.dirty = true;
            BDBG_ASSERT(!client->set.updating);
            client->set.dirtyRect = client->update_info.updateRect;
            nexus_surface_compositor_p_update_dirty_clients(client->server, client);
            break;
        case client_type_push:
        case client_type_tunnel:
        case client_type_tunnel_emulated:
        default:
            break;
        }
    }
    switch(client->state.client_type) {
    case client_type_push: 
            BDBG_ASSERT(BLST_SQ_FIRST(&client->queue.push));
            break;
    case client_type_tunnel_emulated:
        if(client->state.left.visible || client->state.right.visible) {
            BDBG_ASSERT(BLST_SQ_FIRST(&client->queue.push));
        }
        break;
    default:
        break;
    }
    return;
}

void nexus_p_surface_compositor_update_client(NEXUS_SurfaceClientHandle client, unsigned flags)
{
    if(flags) {
        nexus_p_surface_compositor_update_single_client(client, flags);
        nexus_p_surface_compositor_update(client->server, flags);
        if(client->state.client_type == client_type_tunnel) {
            /* needed in order to set server->tunnel.overlapped which in turn used to allocate tunnel surfaces */
            nexus_surface_compositor_p_update_hidden_clients(client->server);
        }
    }
    return;
}

static void nexus_p_surface_compositor_update_all_clients(NEXUS_SurfaceCompositorHandle server, unsigned flags)
{
    NEXUS_SurfaceClientHandle client;
    for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {
        nexus_p_surface_compositor_update_single_client(client, flags);
    }
    nexus_p_surface_compositor_update(server, flags);
    return;
}

NEXUS_Error NEXUS_SurfaceCompositor_GetStatus( NEXUS_SurfaceCompositorHandle server, NEXUS_SurfaceCompositorStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->active = server->state.active;

    if (server->tunnel.client) {
        unsigned i;
        NEXUS_SurfaceClient_TunnelSurface *tunnelSurface;
        for(tunnelSurface=NULL,i=0;i<sizeof(server->tunnel.client->tunnel.surfaces)/sizeof(server->tunnel.client->tunnel.surfaces[0]);i++) {
            if(server->tunnel.client->tunnel.surfaces[i].surface) {
                pStatus->numAcquiredTunneledSurfaces++;
            }
        }
    }

    return 0;
}

void nexus_p_surface_compositor_check_inactive(NEXUS_SurfaceCompositorHandle server)
{
    unsigned i;
    NEXUS_Error rc;
    NEXUS_SurfaceCompositorStatus status;

    BDBG_ASSERT(server->state.active); /* if inactive, the condition should never arise to call this */
    BDBG_ASSERT(!server->settings.enabled); /* if enabled, it should not be called */

    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        BDBG_MSG(("check_inactive[%d]: %p %p", i, server->display[i].compositing, server->display[i].submitted));
        if (server->display[i].compositing || server->display[i].submitted) return;
    }

    rc = NEXUS_SurfaceCompositor_GetStatus(server, &status);
    if (rc || status.numAcquiredTunneledSurfaces) return;

    /* nothing is pending. we are inactive. */
    BDBG_WRN(("inactive"));
    nexus_surface_compositor_p_release_surfaces(server);
    /* last fb callbacks received, so disable displays */
    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        struct NEXUS_SurfaceCompositorDisplay *cmpDisplay = &server->display[i];
        if (cmpDisplay->display) {
            nexus_surfacemp_p_disable_display(server, cmpDisplay);
        }
    }

    server->state.active = false;
    nexus_surface_compositor_p_clear_blitter_state(server);
    NEXUS_TaskCallback_Fire(server->inactiveCallback);
    return;
}

