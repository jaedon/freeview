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
 * $brcm_Workfile: nexus_surface_renderer.c $
 * $brcm_Revision: 30 $
 * $brcm_Date: 8/14/12 10:41a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface_compositor/src/nexus_surface_renderer.c $
 * 
 * 30   8/14/12 10:41a vsilyaev
 * SW7435-301: Added surface reference counting for push/recycle clients
 * 
 * 29   8/8/12 4:58p vsilyaev
 * SW7435-296: Simplified handling of 'published' clients
 * 
 * 28   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 27   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 26   1/26/12 5:22p erickson
 * SW7425-2116: add NEXUS_SurfaceComposition.colorMatrix
 * 
 * 25   1/5/12 5:55p vsilyaev
 * SW7420-2209: Added profiling traces and uses sparse CRC to reduce CPU
 *  load
 * 
 * 24   12/27/11 12:58p vsilyaev
 * SW7420-2119: Use better opaque test for blendequation and accoout
 *  opacity in test for ommiting screen clear (fill)
 * 
 * 23   11/23/11 2:55p vsilyaev
 * SW7420-2148: Reworked queue management for the push type clients
 * 
 * 22   11/22/11 1:39p vsilyaev
 * SW7420-2148: Use faster path for Set/Update clients, better handle out-
 *  of-phase displayed callback
 * 
 * 21   11/15/11 11:01a erickson
 * SW7420-2130: move blit settings into dynamically allocated memory to
 *  avoid stack blowout in linux kernel mode
 * 
 * 20   11/7/11 10:58a vsilyaev
 * SW7420-2119: Refactored tunnel mode support
 * 
 * 19   11/4/11 12:59p vsilyaev
 * SW7231-439: Don't clear state prematurely
 * 
 * 18   11/3/11 9:15p vsilyaev
 * SW7420-2119: Improved handling of 'real' tunnel clients
 * 
 * 17   11/1/11 5:26p vsilyaev
 * SW7420-2119: Returned back support for single frame buffer rendering
 * 
 * 16   10/31/11 4:38p vsilyaev
 * SW7420-2114: Added support for native 3D graphics
 * 
 * 15   10/28/11 5:01p vsilyaev
 * SW7420-2114: Fixed memory aliasing of different buffers
 * 
 * 14   10/28/11 1:33p vsilyaev
 * SW7420-2110: Use better blend equation for cursor rendering
 * 
 * 13   10/27/11 4:24p vsilyaev
 * SW7420-2110: Added support  for incremental (without redrawing of
 *  entire framebuffer) cursor drawing
 * 
 * 12   10/25/11 4:40p vsilyaev
 * SW7420-1992: Use single instance of Graphics2D
 * 
 * 11   10/24/11 5:43p vsilyaev
 * SW7420-1992: Updated cursor support
 * 
 * 10   10/20/11 2:13p vsilyaev
 * SW7420-1992: Added 3D compositing
 * 
 * 9   10/18/11 6:06p vsilyaev
 * SW7420-1992: Use double linked queue for the client surfaces and trim
 *  masked surfaces
 * 
 * 8   10/18/11 12:33p vsilyaev
 * SW7420-1992: More work on separation of backend and frontend
 * 
 * 7   10/13/11 5:05p vsilyaev
 * SW7420-1992: Moved surface release to separate function
 * 
 * 6   10/13/11 3:33p erickson
 * SW7420-1992: add display format change support
 * 
 * 5   10/11/11 8:51a erickson
 * SW7420-1992: change == to =
 * 
 * 4   10/10/11 10:07a vsilyaev
 * SW7420-1992,SW7420-2085: Use reference counting to extend life cycle of
 *  surfaces
 * 
 * 3   10/10/11 9:49a erickson
 * SW7420-1992: change == to =
 * 
 * 2   10/6/11 7:02p vsilyaev
 * SW7420-1992: Added render list
 * 
 * 1   10/5/11 3:47p vsilyaev
 * SW7425-1364: Surface compositor renderer portion
 * 
 **************************************************************************/
#include "nexus_surface_compositor_module.h"
#include "nexus_surface_compositor_impl.h"
#include "priv/nexus_surface_priv.h"

#define BDBG_MSG_TRACE(X) BDBG_MSG(X)

BDBG_MODULE(nexus_surface_renderer);
BTRC_MODULE(surface_compositor_render,ENABLE);

static bool nexus_surface_compositor_p_try_submitframebuffer(struct NEXUS_SurfaceCompositorDisplay *display);


static void nexus_surface_compositor_p_submitframebuffer(struct NEXUS_SurfaceCompositorDisplay *display, struct NEXUS_SurfaceCompositorFramebuffer *framebuffer)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(display->server, NEXUS_SurfaceCompositor);
    BDBG_ASSERT(display->display);
    BDBG_MSG_TRACE(("setfb display[%u] fb[%p->%p]", display->index, display->displaying, framebuffer));
    framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eSubmitted;
    display->submitted = framebuffer;
    if(!display->formatInfo.native3D) {
        rc = NEXUS_Display_SetGraphicsFramebuffer(display->display, framebuffer->surface);
        if (rc) BERR_TRACE(rc); /* fall through */
    } else {
        NEXUS_GraphicsFramebuffer3D  framebuffer3D;
        BDBG_ASSERT(framebuffer->view3D.left);
        BDBG_ASSERT(framebuffer->view3D.right);
        NEXUS_Graphics_GetDefaultFramebuffer3D(&framebuffer3D);
        framebuffer3D.main = framebuffer->view3D.left;
        framebuffer3D.right = framebuffer->view3D.right;
        framebuffer3D.orientation = display->formatInfo.orientation; /* XXX unused */
        BDBG_MSG_TRACE(("setfb display[%u] fb[%#lx->%#lx] left:%p right:%p orientation:%u", display->index, (unsigned long)display->displaying, (unsigned long)framebuffer, framebuffer3D.main, framebuffer3D.right, framebuffer3D.orientation));
        rc = NEXUS_Display_SetGraphicsFramebuffer3D(display->display, &framebuffer3D);
        if (rc) BERR_TRACE(rc); /* fall through */
    }

    return;
}

static void nexus_surface_compositor_slave_compositing_completed(struct NEXUS_SurfaceCompositorDisplay *slaveDisplay)
{
    unsigned new_ref_cnt;

    BDBG_MSG_TRACE(("slave_compositing_completed:%#lx %#lx:%#lx", (unsigned long)slaveDisplay, (unsigned long)slaveDisplay->compositing, (unsigned long)slaveDisplay->master_framebuffer));
    BDBG_ASSERT(slaveDisplay->master_framebuffer);
    BDBG_ASSERT(slaveDisplay->master_framebuffer->ref_cnt>0);
    BDBG_ASSERT(slaveDisplay->compositing);

    slaveDisplay->master_framebuffer->ref_cnt--;
    new_ref_cnt = slaveDisplay->master_framebuffer->ref_cnt;
    slaveDisplay->master_framebuffer = NULL;
    slaveDisplay->composited =  slaveDisplay->compositing;
    slaveDisplay->compositing = NULL;
    nexus_surface_compositor_p_try_submitframebuffer(slaveDisplay);

    return;
}

static void nexus_surface_compositor_render_slave(NEXUS_SurfaceCompositorHandle server, const struct NEXUS_SurfaceCompositorDisplay *masterDisplay, struct NEXUS_SurfaceCompositorDisplay *slaveDisplay, const NEXUS_SurfaceCompositorDisplaySettings *slaveSettings, struct NEXUS_SurfaceCompositorFramebuffer *slaveFramebuffer, const NEXUS_SurfaceCompositorDisplaySettings *masterSettings, struct NEXUS_SurfaceCompositorFramebuffer *masterFramebuffer)
{
    NEXUS_Error rc;

    NEXUS_Graphics2D_GetDefaultBlitSettings(&server->renderState.blitSettings);
    BDBG_MSG_TRACE(("render_slave:%#lx  %#lx->%#lx", (unsigned long)slaveDisplay, (unsigned long)masterFramebuffer, (unsigned long)slaveFramebuffer));

    if(masterDisplay->formatInfo.orientation == slaveDisplay->formatInfo.orientation) {
        server->renderState.blitSettings.output.rect = slaveSettings->graphicsSettings.clip;
        server->renderState.blitSettings.source.rect = masterSettings->graphicsSettings.clip;
        server->renderState.blitSettings.output.surface = slaveFramebuffer->surface;
        server->renderState.blitSettings.source.surface = masterFramebuffer->surface;
    } else if(masterDisplay->formatInfo.orientation == NEXUS_VideoOrientation_e3D_LeftRight || masterDisplay->formatInfo.orientation == NEXUS_VideoOrientation_e3D_OverUnder) {
        BDBG_ASSERT(masterFramebuffer->view3D.right);
        BDBG_ASSERT(masterFramebuffer->view3D.left);
        if(slaveDisplay->formatInfo.orientation == NEXUS_VideoOrientation_e2D) {
            /* 3D -> 2D */
            server->renderState.blitSettings.source.surface = masterFramebuffer->view3D.left;
            server->renderState.blitSettings.output.rect = slaveSettings->graphicsSettings.clip;
            server->renderState.blitSettings.output.surface = slaveFramebuffer->surface;
        } else {
            /* 3D -> 3D */
            BDBG_ASSERT(slaveFramebuffer->view3D.left);
            BDBG_ASSERT(slaveFramebuffer->view3D.right);
            server->renderState.blitSettings.source.surface = masterFramebuffer->view3D.right;
            server->renderState.blitSettings.output.surface = slaveFramebuffer->view3D.right;
            rc = NEXUS_Graphics2D_Blit(server->gfx, &server->renderState.blitSettings);
            BERR_TRACE(rc);
            server->renderState.blitSettings.source.surface = masterFramebuffer->view3D.left;
            server->renderState.blitSettings.output.surface = slaveFramebuffer->view3D.left;
        }
    } else if(slaveDisplay->formatInfo.orientation == NEXUS_VideoOrientation_e3D_LeftRight || slaveDisplay->formatInfo.orientation == NEXUS_VideoOrientation_e3D_OverUnder) {
        /* 2D -> 3D */
        server->renderState.blitSettings.source.rect = masterSettings->graphicsSettings.clip;
        server->renderState.blitSettings.source.surface = masterFramebuffer->surface;
        server->renderState.blitSettings.output.surface = slaveFramebuffer->view3D.right;
        rc = NEXUS_Graphics2D_Blit(server->gfx, &server->renderState.blitSettings);
        BERR_TRACE(rc);
        server->renderState.blitSettings.output.surface = slaveFramebuffer->view3D.left;
    } else {
        BDBG_ASSERT(0);
    }
    slaveFramebuffer->generation = masterFramebuffer->generation;
    slaveDisplay->master_framebuffer = masterFramebuffer;
    slaveDisplay->compositing = slaveFramebuffer;
    masterFramebuffer->ref_cnt++;
    slaveFramebuffer->generation = masterFramebuffer->generation;

    rc = NEXUS_Graphics2D_Blit(server->gfx, &server->renderState.blitSettings);
    BERR_TRACE(rc);

    NEXUS_SurfaceCursor_P_ReleaseCursors(server, slaveDisplay, slaveFramebuffer);
    NEXUS_SurfaceCursor_P_RenderCursors(server, slaveDisplay, slaveFramebuffer);
    return;
}

static void nexus_surface_compositor_update_slave(NEXUS_SurfaceCompositorHandle server, struct NEXUS_SurfaceCompositorDisplay *masterDisplay, struct NEXUS_SurfaceCompositorFramebuffer *masterFramebuffer, struct NEXUS_SurfaceCompositorDisplay *slaveDisplay)
{
    BDBG_MSG_TRACE(("update_slave:%#lx %#lx:%#lx -> %#lx(%#lx,%#lx,%#lx)", (unsigned long)server, (unsigned long)masterDisplay, (unsigned long)masterFramebuffer, (unsigned long)slaveDisplay, (unsigned long)slaveDisplay->compositing, (unsigned long)slaveDisplay->composited, (unsigned long)slaveDisplay->submitted));
    if(slaveDisplay->compositing==NULL && slaveDisplay->composited==NULL && slaveDisplay->submitted==NULL) {
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = slaveDisplay->displaying;
        if(framebuffer) {
            BDBG_MSG_TRACE(("update_slave:%#lx %#lx:%#lx -> %#lx:%#lx (%u,%u)", (unsigned long)server, (unsigned long)masterDisplay, (unsigned long)masterFramebuffer, (unsigned long)slaveDisplay, (unsigned long)framebuffer, framebuffer->generation, masterFramebuffer->generation));
            if( ((int)(framebuffer->generation - masterFramebuffer->generation))>=0) {
                goto done;/* newer frame already displayed */
            }
        }
        if(slaveDisplay->num_framebuffers==1) {
            nexus_surface_compositor_p_recycle_displayed_framebuffer(slaveDisplay);
        }
        framebuffer = BLST_SQ_FIRST(&slaveDisplay->available);
        BDBG_MSG_TRACE(("update_slave:%#lx %#lx:%#lx -> %#lx:%#lx", (unsigned long)server, (unsigned long)masterDisplay, (unsigned long)masterFramebuffer, (unsigned long)slaveDisplay, (unsigned long)framebuffer));
        if(framebuffer) {
            BLST_SQ_REMOVE_HEAD(&slaveDisplay->available, link);
            nexus_surface_compositor_render_slave(server, masterDisplay, slaveDisplay, &server->settings.display[slaveDisplay->index], framebuffer, &server->settings.display[masterDisplay->index], masterFramebuffer);
        }
    }
done:
    return;
}

static void nexus_surface_compositor_update_slaves(NEXUS_SurfaceCompositorHandle server, struct NEXUS_SurfaceCompositorDisplay *masterDisplay, struct NEXUS_SurfaceCompositorFramebuffer *masterFramebuffer)
{
    unsigned i;
    for(i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        struct NEXUS_SurfaceCompositorDisplay *slaveDisplay = &server->display[i];
        if(masterDisplay->index==i) {
            continue;
        }
        if(slaveDisplay->display==NULL) {
            continue;
        }
        nexus_surface_compositor_update_slave(server, masterDisplay, masterFramebuffer, slaveDisplay);
    }
    return;
}

void nexus_surface_compositor_p_release_surfaces(NEXUS_SurfaceCompositorHandle server)
{
    unsigned i;

    for(i=0;i<server->renderState.elements.count;i++) {
        const NEXUS_P_SurfaceCompositorRenderElement *data = server->renderState.elements.data+i;
        NEXUS_OBJECT_RELEASE(server, NEXUS_Surface, data->sourceSurface);
    }
    /* clear state, so it's safe to call this function multiple times */
    server->renderState.elements.count = 0;
    server->renderState.tunnel.active = false;
    server->renderState.tunnel.tunnelSource = NULL;
    return;
}

static void nexus_surface_compositor_p_return_tunnel_framebuffer(struct NEXUS_SurfaceCompositorDisplay *display, struct NEXUS_SurfaceCompositorFramebuffer *framebuffer)
{
    if(!framebuffer->tunnel.acquired) {
        BDBG_MSG_TRACE(("return_tunnel_framebuffer:%p %p to display", display, framebuffer));
        framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eAvailable;
        BLST_SQ_INSERT_TAIL(&display->available, framebuffer, link);
    } else {
        BDBG_MSG_TRACE(("return_tunnel_framebuffer:%p %p to tunnel", display, framebuffer));
        framebuffer->tunnel.composited = true;
        framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eAvailable;
        BLST_Q_INSERT_TAIL(&display->tunnel.available, framebuffer , tunnelLink);
        BDBG_ASSERT(display->server->tunnel.client);
        NEXUS_TaskCallback_Fire(display->server->tunnel.client->recycledCallback);
    }
    return;
}

static void nexus_surface_compositor_p_compose_release_clients(NEXUS_SurfaceCompositorHandle server)
{
    NEXUS_SurfaceClientHandle client;

    /* after compositing completed, buffers for push type clients could be immediatly recycled */
    for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {
        if(client->state.client_type==client_type_push || client->state.client_type == client_type_tunnel_emulated) {
            if(client->queue.compositing) {
                NEXUS_SurfaceCompositor_P_PushElement *first_node = BLST_SQ_FIRST(&client->queue.push);
                client->queue.compositing = false;
                if(first_node) {
                    NEXUS_SurfaceCompositor_P_PushElement *next_node = BLST_SQ_NEXT(first_node, link);
                    client->queue.last = next_node == NULL;
                    if(next_node) {
                        BDBG_MSG_TRACE(("nexus_surface_compositor_p_compose_release_clients:%p queue recycle %p", client, first_node->surface));
                        NEXUS_OBJECT_RELEASE(client, NEXUS_Surface, first_node->surface.surface);
                        BLST_SQ_REMOVE_HEAD(&client->queue.push, link);
                        BLST_SQ_INSERT_TAIL(&client->queue.recycle, first_node, link);
                        NEXUS_TaskCallback_Fire(client->recycledCallback);
                        client->state.update_flags |= NEXUS_P_SURFACECLIENT_UPDATE_SOURCE;
                    }
                }
            }
        }  
    }
    return;
}


static void nexus_surface_compositor_compositing_completed(NEXUS_SurfaceCompositorHandle server)
{
    struct NEXUS_SurfaceCompositorDisplay *display = &server->display[0];
    unsigned i;

    BDBG_MSG_TRACE(("compositing_completed:%#lx %p", (unsigned long)server, display->compositing));
    
    nexus_surface_compositor_p_blitter_release(server, NEXUS_P_SURFACEBLITTER_COMPOSITOR);
    nexus_surface_compositor_p_update_dirty_clients_done(server); /* check if for set clients server side copy was updated */

    if(display->compositing) {
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = display->compositing;
        display->compositing = NULL;
        server->renderState.step = taskCompleted;
        server->renderState.current = 0;
        framebuffer->scene.elements.count = server->renderState.elements.count;
        for(i=0;i<server->renderState.elements.count;i++) {
            const NEXUS_P_SurfaceCompositorRenderElement *data = server->renderState.elements.data+i;
            framebuffer->scene.elements.data[i] = *data;
        }
        framebuffer->scene.dirty = server->renderState.tunnel.rect;
        display->composited = framebuffer;
        framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eComposited;
        if(server->renderState.tunnel.tunnelSource && server->renderState.tunnel.tunnelSource->state == NEXUS_SurfaceCompositorFramebufferState_eTunnelSubmitted) {
            nexus_surface_compositor_p_return_tunnel_framebuffer(display,server->renderState.tunnel.tunnelSource);
        }
        nexus_surface_compositor_p_release_surfaces(server);
        nexus_surface_compositor_p_compose_release_clients(server);
        nexus_surface_compositor_p_try_submitframebuffer(display);
    }

    for(i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        struct NEXUS_SurfaceCompositorDisplay *slaveDisplay = &server->display[i];
        if(display->index==i) {
            continue;
        }
        if(slaveDisplay->compositing) {
            nexus_surface_compositor_slave_compositing_completed(slaveDisplay);
        }
    }
    BTRC_TRACE(surface_compositor_render,STOP);
    return;
}

static void nexus_surface_compositor_p_blitter_complete(NEXUS_SurfaceCompositorHandle compositor)
{
    BDBG_MSG_TRACE(("%p:blitter_complete %#x", compositor, compositor->state.blitter.active));
    switch(compositor->state.blitter.active) {
    case NEXUS_P_SURFACEBLITTER_COMPOSITOR:
        nexus_surface_compositor_compositing_completed(compositor);
        break;
    case NEXUS_P_SURFACEBLITTER_CURSOR:
        nexus_surface_compositor_p_blitter_release(compositor, NEXUS_P_SURFACEBLITTER_CURSOR);
        break;
    case NEXUS_P_SURFACEBLITTER_CLIENT:
        nexus_surface_compositor_p_blitter_release(compositor, NEXUS_P_SURFACEBLITTER_CLIENT);
        nexus_surface_compositor_p_update_dirty_clients_done(compositor);
        break;    
    default:
        BDBG_ASSERT(0);
    }
    BDBG_MSG_TRACE(("%p:blitter_complete done %#x", compositor, compositor->state.blitter.active));
    BDBG_ASSERT(compositor->state.blitter.active==0);
    return;
}

void nexus_surface_compositor_p_checkpoint(void *context)
{
    NEXUS_SurfaceCompositorHandle compositor = context;

    BDBG_OBJECT_ASSERT(compositor, NEXUS_SurfaceCompositor);
  
    if (!compositor->settings.enabled) {
        unsigned i;
        for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
            struct NEXUS_SurfaceCompositorDisplay *display = &compositor->display[i];
            display->compositing = NULL;
        }
        nexus_p_surface_compositor_check_inactive(compositor);
        return;
    }
    nexus_surface_compositor_p_blitter_complete(compositor);

    if(compositor->state.blitter.delayed & NEXUS_P_SURFACEBLITTER_COMPOSITOR) {
        compositor->state.blitter.delayed &= ~(NEXUS_P_SURFACEBLITTER_CURSOR | NEXUS_P_SURFACEBLITTER_COMPOSITOR | NEXUS_P_SURFACEBLITTER_CLIENT);
        nexus_surface_compositor_p_compose(compositor); /* compose would render cursors at new framebuffer */
    } else if(compositor->state.blitter.delayed & NEXUS_P_SURFACEBLITTER_CURSOR) {
        compositor->state.blitter.delayed &= ~(NEXUS_P_SURFACEBLITTER_CURSOR);
        NEXUS_SurfaceCursor_P_UpdateCursor(compositor);
    } else if(compositor->state.blitter.delayed & NEXUS_P_SURFACEBLITTER_CLIENT) {
        compositor->state.blitter.delayed &= ~(NEXUS_P_SURFACEBLITTER_CLIENT);
        nexus_surface_compositor_p_update_dirty_clients(compositor, NULL /* update all clients */);
    }
    return;
}




void nexus_surface_compositor_p_packetspaceavailable(void *context)
{
    NEXUS_SurfaceCompositorHandle server = context;
    /* TODO */
    BSTD_UNUSED(server);
}

static bool nexus_surface_compositor_p_try_submitframebuffer(struct NEXUS_SurfaceCompositorDisplay *display)
{
    BDBG_MSG_TRACE(("try_sumbitframebuffer:%#lx %#lx[%u] submitted:%#lx composited:%#lx", (unsigned long)display->server, (unsigned long)display, display->index, display->submitted, display->composited));

    if(display->submitted==NULL && display->composited) {
        struct NEXUS_SurfaceCompositorFramebuffer *framebuffer = display->composited;
        display->composited = NULL;
        nexus_surface_compositor_p_submitframebuffer(display, framebuffer);
        return true;
    } else {
        return false;
    }
}

void nexus_surface_compositor_p_recycle_displayed_framebuffer(struct NEXUS_SurfaceCompositorDisplay *display)
{
    struct NEXUS_SurfaceCompositorFramebuffer *framebuffer;
    framebuffer = display->displaying;
    if(framebuffer) {
        /* BDBG_ASSERT(framebuffer->state == NEXUS_SurfaceCompositorFramebufferState_eDisplaying); */
        display->displaying = NULL;
        BDBG_MSG_TRACE(("recycle_displayed_framebuffer:%#lx recycling:%#lx(%u) %s %s", (unsigned long)display->server, (unsigned long)framebuffer, framebuffer->ref_cnt, display->index?"master":"slave", framebuffer->tunnel.acquired?"tunnel":""));
        nexus_surface_compositor_p_return_tunnel_framebuffer(display,framebuffer);
    }
    return;
}


static void nexus_surface_compositor_p_slave_framebuffer_applied(struct NEXUS_SurfaceCompositorDisplay *display)
{
    struct NEXUS_SurfaceCompositorFramebuffer *framebuffer;
    NEXUS_SurfaceCompositorHandle server = display->server;

    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);

    framebuffer = display->displaying;
    BDBG_MSG_TRACE(("slave_framebuffer:%#lx %#lx<->%#lx", (unsigned long)server, (unsigned long)framebuffer, (unsigned long)display->submitted));
    nexus_surface_compositor_p_recycle_displayed_framebuffer(display);
    BDBG_ASSERT(display->submitted);
    framebuffer = display->submitted;
    display->submitted = NULL;
    display->displaying = framebuffer;
    framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eDisplaying;
    if(!nexus_surface_compositor_p_try_submitframebuffer(display)) {
#if 0
        struct NEXUS_SurfaceCompositorDisplay *masterDisplay = &server->display[0];
        if(masterDisplay->displaying) {
            nexus_surface_compositor_update_slave(server, masterDisplay, masterDisplay->displaying, display);
        }
#endif
    }
    return;
}

static void nexus_surface_compositor_p_master_framebuffer_applied(struct NEXUS_SurfaceCompositorDisplay *display)
{
    NEXUS_SurfaceCompositorHandle server = display->server;
    struct NEXUS_SurfaceCompositorFramebuffer *framebuffer;

    BDBG_OBJECT_ASSERT(server, NEXUS_SurfaceCompositor);

    nexus_surface_compositor_p_recycle_displayed_framebuffer(display);

    BDBG_ASSERT(display->submitted);
    framebuffer = display->submitted;
    display->submitted = NULL;
    display->displaying = framebuffer;
    framebuffer->state = NEXUS_SurfaceCompositorFramebufferState_eDisplaying;
    BDBG_MSG_TRACE(("master_framebuffer_applied:%#lx %#lx[%u] displaying:%#lx", (unsigned long)server, (unsigned long)display, display->index, framebuffer));

    nexus_surface_compositor_p_try_submitframebuffer(display);

    {
        NEXUS_SurfaceClientHandle client;
        bool recompose = server->pending_update;

        
        /* framebuffers have been set. */
        NEXUS_TaskCallback_Fire(server->frameBufferCallback);
        for (client = BLST_Q_FIRST(&server->clients); client; client = BLST_Q_NEXT(client, link)) {
            if (client->process_pending_displayed_callback) {
                client->process_pending_displayed_callback = false;
                client->pending_displayed_callback = false;
                BDBG_MSG_TRACE(("master_framebuffer_callback:%p client %p(%u) displayed", server, client, client->client_id));
                NEXUS_TaskCallback_Fire(client->displayedCallback);
            }
            if(!recompose) {
                if(client->published.surface) { /* if one client is published, we must rerender for the next vsync */
                    recompose = true;
                } else if(client->state.client_type==client_type_push || client->state.client_type == client_type_tunnel_emulated) {
                    if (!client->queue.last) {
                        recompose = true; /* need to compose with new surface */
                        BDBG_MSG_TRACE(("master_framebuffer_callback:%#lx client %#x recompose", (unsigned long)server, (unsigned long)client));
                    }
                }
            }
        }

        if (recompose) {
            BDBG_MSG_TRACE(("master_framebuffer_callback:%#lx recompose", server));
            nexus_surface_compositor_p_compose(server);
        }
    }
    return;
}

void nexus_surface_compositor_p_framebuffer(void *context)
{
    struct NEXUS_SurfaceCompositorDisplay *display = context;
    BDBG_MSG_TRACE(("framebuffer_callback:%#lx %#lx[%u] new:%#lx old:%#lx", (unsigned long)display->server, (unsigned long)display, display->index, display->submitted, display->displaying));

    if (!display->server->settings.enabled) {
        display->submitted = NULL;
        nexus_p_surface_compositor_check_inactive(display->server);
        return;
    }

    if(display->index==0) {
        nexus_surface_compositor_p_master_framebuffer_applied(display);
    } else {
        nexus_surface_compositor_p_slave_framebuffer_applied(display);
    }
}

/* return true if composition is opaque, e.g. it would override (not blend) anything that drawn behind it */
bool nexus_p_surface_composition_opaque(const NEXUS_BlendEquation *colorBlend, const NEXUS_BlendEquation *alphaBlend)
{
    BSTD_UNUSED(alphaBlend);
    return colorBlend->c == NEXUS_BlendFactor_eZero && colorBlend->d == NEXUS_BlendFactor_eZero;
}

void nexus_surface_compositor_p_render_framebuffer(NEXUS_SurfaceCompositorHandle server, struct NEXUS_SurfaceCompositorDisplay *cmpDisplay, struct NEXUS_SurfaceCompositorFramebuffer *framebuffer)
{
    unsigned i;
    NEXUS_Error rc; const NEXUS_P_SurfaceCompositorRenderElement *data;

    BDBG_MSG_TRACE(("render_framebuffer:%p: %p:%p[%u]", server, framebuffer, framebuffer->surface, framebuffer->ref_cnt));
    /* this function could be interrupted at any call to Graphics2D call, at which point it would save it's state and be resumed (by calling function over) some later time */
    if(server->renderState.step == taskInit) {
        BTRC_TRACE(surface_compositor_render,START);
        for(i=0;i<server->renderState.elements.count;i++) {
            data = server->renderState.elements.data + i;
            NEXUS_OBJECT_ACQUIRE(server, NEXUS_Surface, data->sourceSurface);
        }
        server->renderState.current = 0;
        server->renderState.step = taskFill;
    }
    if(server->renderState.step == taskFill) {
        if(server->renderState.current==0) {
            /* verify whether there are any 'full screen' elements thus making fill step unnecessary */
            for(i=0;i<server->renderState.elements.count;i++) {
                data = server->renderState.elements.data + i;
                if(data->outputRect.width == cmpDisplay->canvas.width && data->outputRect.height == cmpDisplay->canvas.height && nexus_p_surface_composition_opaque(&data->colorBlend, &data->alphaBlend)) {
                    NEXUS_SurfaceCursor_P_ReleaseCursors(server, cmpDisplay, framebuffer);
                    goto blit_step;
                }
            }
        }
        NEXUS_SurfaceCursor_P_ClearCursors(server, cmpDisplay, framebuffer);
        NEXUS_Graphics2D_GetDefaultFillSettings(&server->renderState.fillSettings);

        server->renderState.fillSettings.color = cmpDisplay->backgroundColor;
        server->renderState.fillSettings.surface = framebuffer->surface;
        if(!server->renderState.tunnel.active) { /* we don't support incremental fill of tunneled client */
            if(framebuffer->scene.dirty.width && framebuffer->scene.dirty.height) {
                server->renderState.fillSettings.rect = framebuffer->scene.dirty; 
                BDBG_MSG_TRACE(("clear dirty:%#lx:%#lx [%u,%u %ux%u]", (unsigned long)server, (unsigned long)cmpDisplay, server->renderState.fillSettings.rect.x, server->renderState.fillSettings.rect.y, server->renderState.fillSettings.rect.width, server->renderState.fillSettings.rect.height));
                rc = NEXUS_Graphics2D_Fill(server->gfx, &server->renderState.fillSettings);
                /* TODO: if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) */
                if (rc) rc = BERR_TRACE(rc);
            }
        }

        for(;server->renderState.current<framebuffer->scene.elements.count;server->renderState.current++) {
            data = framebuffer->scene.elements.data + server->renderState.current;
            server->renderState.fillSettings.rect = data->outputRect;
            BDBG_MSG_TRACE(("clear prev:%#lx :%#lx:%#lx [%u,%u %ux%u]", (unsigned long)server, (unsigned long)cmpDisplay, (unsigned long)data->sourceSurface, server->renderState.fillSettings.rect.x, server->renderState.fillSettings.rect.y, server->renderState.fillSettings.rect.width, server->renderState.fillSettings.rect.height));
            rc = NEXUS_Graphics2D_Fill(server->gfx, &server->renderState.fillSettings);
            /* TODO: if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) */
            if (rc) rc = BERR_TRACE(rc);
        }
blit_step:
        server->renderState.step = taskBlit;
        server->renderState.current = 0;
    }

    if(server->renderState.step == taskBlit) {
        NEXUS_Graphics2D_GetDefaultBlitSettings(&server->renderState.blitSettings);
        server->renderState.blitSettings.output.surface = framebuffer->surface;
        server->renderState.blitSettings.dest.surface = framebuffer->surface;
        server->renderState.blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
        server->renderState.blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;


        for(;server->renderState.current<server->renderState.elements.count;server->renderState.current++) {
            data = server->renderState.elements.data + server->renderState.current;
            server->renderState.blitSettings.source.surface = data->sourceSurface;
            server->renderState.blitSettings.colorBlend = data->colorBlend;
            server->renderState.blitSettings.alphaBlend = data->alphaBlend;
            server->renderState.blitSettings.constantColor = data->constantColor;
            server->renderState.blitSettings.output.rect = data->outputRect;
            server->renderState.blitSettings.dest.rect = data->outputRect;
            server->renderState.blitSettings.source.rect = data->sourceRect;
            server->renderState.blitSettings.conversionMatrixEnabled = data->colorMatrixEnabled;
            if (data->colorMatrixEnabled) {
                server->renderState.blitSettings.conversionMatrix = data->colorMatrix;
            }
            rc = NEXUS_Graphics2D_Blit(server->gfx, &server->renderState.blitSettings);
            BDBG_MSG_TRACE(("blit [%d:%p] [%u,%u %ux%u]", cmpDisplay->index, server->renderState.blitSettings.source.surface,
            server->renderState.blitSettings.output.rect.x, server->renderState.blitSettings.output.rect.y,
            server->renderState.blitSettings.output.rect.width, server->renderState.blitSettings.output.rect.height));
            /* TODO: if (rc == NEXUS_GRAPHICS2D_QUEUE_FULL) */
            if (rc) rc = BERR_TRACE(rc);
        }
        server->renderState.step = taskCompleted;

        nexus_surface_compositor_update_slaves(server, cmpDisplay, framebuffer); /* render slaves without cursor */
        NEXUS_SurfaceCursor_P_RenderCursors(server, cmpDisplay, framebuffer);

        nexus_surface_compositor_p_blitter_start(server, NEXUS_P_SURFACEBLITTER_COMPOSITOR);
    }

    return;
}


bool nexus_surface_compositor_p_blitter_acquire(NEXUS_SurfaceCompositorHandle compositor, unsigned client)
{
    BDBG_ASSERT(client==NEXUS_P_SURFACEBLITTER_COMPOSITOR || client==NEXUS_P_SURFACEBLITTER_CURSOR || client==NEXUS_P_SURFACEBLITTER_CLIENT);
    if(compositor->state.blitter.active==0) {
        BDBG_MSG_TRACE(("%p:blitter_acquire %#x ", compositor, client));
        compositor->state.blitter.active = client;
        return true;
    } else {
        BDBG_MSG_TRACE(("%p:blitter busy %#x delayed (%#x,%#x)", compositor, compositor->state.blitter.active, client, compositor->state.blitter.delayed));
        compositor->state.blitter.delayed |= client;
        return false;
    }
}


void nexus_surface_compositor_p_blitter_release(NEXUS_SurfaceCompositorHandle compositor, unsigned client)
{
    BDBG_ASSERT(compositor->state.blitter.active == client);
    BDBG_MSG_TRACE(("%p:blitter_release %#x", compositor, client));
    BSTD_UNUSED(client);
    compositor->state.blitter.active = 0;
    return;
}

void nexus_surface_compositor_p_blitter_start(NEXUS_SurfaceCompositorHandle compositor, unsigned client)
{
    NEXUS_Error rc;
    BDBG_ASSERT(compositor->state.blitter.active == client);
    BSTD_UNUSED(client);
    rc = NEXUS_Graphics2D_Checkpoint(compositor->gfx, NULL);
    BDBG_MSG_TRACE(("%p:blitter_start %#x->%u", compositor, client, rc));
    if(rc==NEXUS_SUCCESS) {
        nexus_surface_compositor_p_blitter_complete(compositor);
        BDBG_ASSERT(compositor->state.blitter.delayed==0); /* XXX we don't support delayed without checkpoint, they can't be called directly since it could cause an endless recursion, and if it has to be handled then, perhaps via 0 time callout */
    } else if (rc == NEXUS_GRAPHICS2D_QUEUED) {
       /* nexus_surface_compositor_p_checkpoint would call nexus_surface_compositor_p_blitter_complete */
    } else {
        rc = BERR_TRACE(rc);
    }
    return;

}
