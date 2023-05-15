/***************************************************************************
 *     (c)2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_surface_client.h $
 * $brcm_Revision: 13 $
 * $brcm_Date: 11/28/11 2:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface_compositor/include/nexus_surface_client.h $
 * 
 * 13   11/28/11 2:00p erickson
 * SW7420-2148: update comments on recycled and displayed callbacks
 * 
 * 12   11/16/11 2:56p erickson
 * SW7420-1992: update comments
 * 
 * 11   10/24/11 5:43p vsilyaev
 * SW7420-1992: Updated cursor support
 * 
 * 10   10/20/11 2:13p vsilyaev
 * SW7420-1992: Added 3D compositing
 * 
 * 9   10/13/11 3:33p erickson
 * SW7420-1992: add display format change support
 *
 * 8   10/4/11 5:52p vsilyaev
 * SW7425-1364: Added ClientSurfaceId
 *
 * 7   8/26/11 5:36p vsilyaev
 * SW7420-1992: Updated API
 *
 * 6   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 *
 * 5   8/22/11 4:11p vsilyaev
 * SW7420-1992: Updated implementation of surface compositor
 *
 * 4   8/11/11 3:31p erickson
 * SW7420-1992: update
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
#ifndef NEXUS_SURFACE_CLIENT_H__
#define NEXUS_SURFACE_CLIENT_H__

#include "bstd.h"
#include "nexus_types.h"
#include "nexus_surface.h"
#include "nexus_surface_compositor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Acquire a client handle from the server

Description:
Only the server can create clients and control their top-level composition.
If the client wants to request this, it must be done with app-level IPC.
**/
NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_Acquire( /* attr{release=NEXUS_SurfaceClient_Release} */
    NEXUS_SurfaceCompositorClientId client_id
    );

/**
Summary:
Release a client that was acquired
**/
void NEXUS_SurfaceClient_Release(
    NEXUS_SurfaceClientHandle client
    );

/**
Summary:
TODO: Create a child window within an acquired parent window
**/
NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_CreateChild(
    NEXUS_SurfaceClientHandle parent_handle
    );

/**
Summary:
Destroy a child window
**/
void NEXUS_SurfaceClient_DestroyChild(
    NEXUS_SurfaceClientHandle child_handle
    );

/**
Summary:
Acquire a video window as a child of an acquired parent window

Description:
this allows the client to resize and reposition the window along with its GUI changes.
you cannot submit surfaces to a video window NEXUS_SurfaceClientHandle.
you can call NEXUS_SurfaceClient_SetSettings to control composition settings.
**/
NEXUS_SurfaceClientHandle NEXUS_SurfaceClient_AcquireVideoWindow(
    NEXUS_SurfaceClientHandle parent_handle,
    unsigned window_id
    );

/**
Summary:
**/
void NEXUS_SurfaceClient_ReleaseVideoWindow(
    NEXUS_SurfaceClientHandle window_handle
    );

/**
Summary:
Client settings

Description:
There are three different kinds of clients. The following settings apply in this way:
1) top-level clients - all settings exception composition apply
2) child video window - only composition applies
3) child surfaces - not implemented
**/
typedef struct NEXUS_SurfaceClientSettings
{
    NEXUS_SurfaceComposition composition; /* This currently applies only to video child windows. 
        It currently has no effect on top-level composition of the client. */
    NEXUS_CallbackDesc recycled; /* one or more surfaces have been recycled. they are no longer in use by the server or hardware.
        client should call RecycleSurface to retrieve the surface(s).
        recycling happens after PushSurface if the new surface causes an old surface to become unused.
        recycling also happens after SetSurface/UpdateSurface when the client's surface has been copied to the server's memory. */
    NEXUS_CallbackDesc displayed; /* called when the contents of the last SetSurface/UpdateSurface/PushSurface has made it to the framebuffer.
        you should wait on this callback only if you want your client to be paced by the display's frame rate. 
        it will result in slower updates than just using the recycled callback. */
    NEXUS_CallbackDesc displayStatusChanged; /* NEXUS_SurfaceClientStatus.display has changed */

    NEXUS_VideoOrientation orientation; /* orientation of the client's surface.
        if display is 3D:
            if e3D_LeftRight, then the left half of the surface is used for left eye, right half for right eye.
            if e3D_OverUnder, then the top half of the surface is used for left eye, bottom half for right eye.
            if e2D, then the whole surface is used for both eyes.
        if display is 2D:
            if e3D_LeftRight, then only left half will be used.
            if e3D_OverUnder, then only top half will be used.
            if 2D, then the whole surface is used. */
} NEXUS_SurfaceClientSettings;

/**
Summary:
set a new surface (incremental mode)

Example scenario:
1) client creates a surface
2) client renders the whole surface
3) client calls NEXUS_SurfaceClient_SetSurface.
4) client must wait for the recycled or displayed callback before making an update to the surface.
5) client can update a region of the surface and call NEXUS_SurfaceClient_UpdateSurface.
6) client must wait for the recycled callback before making another update to the surface.
**/
NEXUS_Error NEXUS_SurfaceClient_SetSurface(
    NEXUS_SurfaceClientHandle handle,
    NEXUS_SurfaceHandle surface
    );

/**
Summary:
update a surface that was set with NEXUS_SurfaceClient_SetSurface

Description:
Client is responsible for waiting for the recycled or displayed callback before calling UpdateSurface after a previous SetSurface or UpdateSurface.
If it is called prematurely, the function will fail.
**/
NEXUS_Error NEXUS_SurfaceClient_UpdateSurface(
    NEXUS_SurfaceClientHandle handle,
    const NEXUS_Rect *pUpdateRect /* attr{null_allowed=y} NULL for whole surface */
    );

/**
Summary:
push a new surface (multibuffered mode)

Example scenario:
1) client creates surface[0] and [1].
2) client renders surface[0] then calls NEXUS_SurfaceClient_PushSurface. the function is non-blocking and no copy will be made.
   this surface will be composited and displayed by the server.
3) client renders into surface[1] then calls NEXUS_SurfaceClient_PushSurface.
4) pushing surface[1] will cause surface[0] to eventually become unused. when that occurs, the recycled callback will fire.
5) the client should wait for the recycled or displayed callback, then call RecycleSurface to retrieve the surface.
6) once the surface has been retrieved, the client can redraw into it or destroy it. it is no longer used by the surface compositor.
**/
NEXUS_Error NEXUS_SurfaceClient_PushSurface(
    NEXUS_SurfaceClientHandle handle,
    NEXUS_SurfaceHandle surface,
    const NEXUS_Rect *pUpdateRect, /* attr{null_allowed=y} NULL for whole surface */
    bool infront /* in front is used to push surface in front of any other queued surfaces, if there are any other surfaces queued they would get immediately recycled */
    );

/**
Summary:
recycle a surface that was previously pushed with NEXUS_SurfaceClient_PushSurface

Description:
after receiving a recycled callback, you should call NEXUS_SurfaceClient_RecycleSurface to retrieve the surfaces from the surface compositor.
more than one surface may be recycled for a single callback. you should continue to call RecycleSurface until num_returned < num_entries.
**/
NEXUS_Error NEXUS_SurfaceClient_RecycleSurface(
    NEXUS_SurfaceClientHandle handle,
    NEXUS_SurfaceHandle *recycled, /* attr{nelem=num_entries;nelem_out=num_returned} */
    size_t num_entries,
    size_t *num_returned
    );

/**
Summary:
Remove all surfaces from the surface compositor immediately

Description:
This clears surfaces in all modes.
This does not cause surfaces to be recycled. They are immediately available to the client, either to reuse or destroy.

NEXUS_SurfaceClient_Clear does not clear in-flight callbacks. So, a pending recycled callback or a surface that is in the recycle queue because of previous
actions will still have to be recycled.
**/
void NEXUS_SurfaceClient_Clear(
    NEXUS_SurfaceClientHandle handle
    );

/**
Summary:
Get client settings
**/
void NEXUS_SurfaceClient_GetSettings(
    NEXUS_SurfaceClientHandle handle,
    NEXUS_SurfaceClientSettings *pSettings /* [out] */
    );

/**
Summary:
Set client settings

Description:
See NEXUS_SurfaceClientSettings for which settings apply to the client
**/
NEXUS_Error NEXUS_SurfaceClient_SetSettings(
    NEXUS_SurfaceClientHandle handle,
    const NEXUS_SurfaceClientSettings *pSettings
    );

/**
Summary:
Server and display status for the client
**/
typedef struct NEXUS_SurfaceClientStatus
{
    NEXUS_Rect position; /* client position and size in framebuffer coordinates */
    struct {
        bool enabled; /* display is enabled. if not, client should release all tunneled surfaces. */
        bool enabled3d; /* if display is in a 3D mode */
        NEXUS_SurfaceRegion framebuffer; /* framebuffer size in pixels. */
        NEXUS_SurfaceRegion screen; /* display size */
    } display;    
} NEXUS_SurfaceClientStatus;

/**
Summary:
Get server and display status
**/
NEXUS_Error NEXUS_SurfaceClient_GetStatus(
    NEXUS_SurfaceClientHandle handle,
    NEXUS_SurfaceClientStatus *pStatus /* [out] */
    );

/**
Summary:
Acquire a tunneled surface (tunneled mode)

Description:
server must set tunnelCapable = true for this client first.
currently, there can only be one tunneled client.
currently, we only support tunneling to display[0].

Client is responsible for waiting for the displayed callback before calling GetTunneledSurface again.
If it is called prematurely, the function will fail.
**/
NEXUS_Error NEXUS_SurfaceClient_AcquireTunneledSurface(
    NEXUS_SurfaceClientHandle handle,
    NEXUS_SurfaceHandle *pSurface /* [out] */
    );

/**
Summary:
Release a tunneled surface
**/
void NEXUS_SurfaceClient_ReleaseTunneledSurface(
    NEXUS_SurfaceClientHandle handle,
    NEXUS_SurfaceHandle pSurface
    );

/**
Summary:
Push a tunneled surface

Description:
the client has finished rendering into the tunneled surface obtained from GetTunneledSurface. it can be displayed immediately.
the client is responsible that surface is completed before submitting.
completion of a surface may include calling NEXUS_Graphics2D_Checkpoint and/or NEXUS_Surface_Flush.
**/
NEXUS_Error NEXUS_SurfaceClient_PushTunneledSurface(
    NEXUS_SurfaceClientHandle handle,
    NEXUS_SurfaceHandle pSurface,
    const NEXUS_Rect *pUpdateRect, /* attr{null_allowed=y} NULL for whole surface */
    bool infront /* in front is used to submit surface in front of any other submitted surfaces, if there are any other surfaces submitted they would get immediately recycled */
    );

/**
Summary:
Recycle a tunneled surface

Description:
See NEXUS_SurfaceClient_RecycleSurface for a description of how recycling works
*/
NEXUS_Error NEXUS_SurfaceClient_RecycleTunneledSurface(
    NEXUS_SurfaceClientHandle handle,
    NEXUS_SurfaceHandle *recycled, /* attr{nelem=num_entries;nelem_out=num_returned} */
    size_t num_entries,
    size_t *num_returned
    );


/**
Summary:
tell surface_compositor that for whatever surface is current (either last SetSurface, last PushSurface or last surface returned by GetTunneledSurface)
the compositor should re-render that surface on every vsync without any further notification from the client.

Description:
the feature cannot be implemented without possible tearing, so it is not recommended.
it is useful for porting libraries that expect direct access to the framebuffer.

to release the published surface, you must either call SetSurface or PushSurface (with another surface, same surface or NULL),
or call PushTunneledSurface to release the tunneled surface.
**/
NEXUS_Error NEXUS_SurfaceClient_PublishSurface(
    NEXUS_SurfaceClientHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif
