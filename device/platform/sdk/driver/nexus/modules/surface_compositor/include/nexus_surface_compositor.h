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
 * $brcm_Workfile: nexus_surface_compositor.h $
 * $brcm_Revision: 25 $
 * $brcm_Date: 8/6/12 1:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/surface_compositor/include/nexus_surface_compositor.h $
 * 
 * 25   8/6/12 1:30p erickson
 * SW7425-3565: add NEXUS_SurfaceComposition.contentMode
 * 
 * 24   7/31/12 10:40a erickson
 * SW7420-1992: increase NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS
 * 
 * 23   7/30/12 2:57p erickson
 * SW7420-1992: increase NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS
 * 
 * 22   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 21   1/20/12 2:19p erickson
 * SW7420-1992: clarify which NEXUS_GraphicsSettings members are
 *  overridden
 * 
 * 20   11/10/11 4:56p erickson
 * SW7420-1992: change videowindow array to
 *  NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS to support mosaic mode
 * 
 * 19   10/24/11 5:43p vsilyaev
 * SW7420-1992: Updated cursor support
 * 
 * 18   10/21/11 4:39p erickson
 * SW7420-1992: update comments
 * 
 * 17   10/20/11 6:21p erickson
 * SW7420-1992: remove TODO
 * 
 * 16   10/20/11 2:13p vsilyaev
 * SW7420-1992: Added 3D compositing
 * 
 * 15   10/20/11 1:56p erickson
 * SW7420-1992: embed NEXUS_VideoWindowSettings in
 *  NEXUS_SurfaceCompositorDisplaySettings
 * 
 * 14   10/18/11 12:32p vsilyaev
 * SW7420-1992: Moved virtual display into the composition settings
 * 
 * 13   10/13/11 3:33p erickson
 * SW7420-1992: add display format change support
 * 
 * 12   10/4/11 5:52p vsilyaev
 * SW7425-1364: Added ClientSurfaceId
 * 
 * 11   10/3/11 9:25a erickson
 * SW7420-1148: add video window support
 * 
 * 10   9/7/11 7:13p vsilyaev
 * SW7420-1148: Decouple rendering of primary and other displays. Other
 *  displays would get just a copy of result rendered on primary display.
 * 
 * 9   9/1/11 2:07p vsilyaev
 * SW7420-1992: Updated API
 * 
 * 8   8/26/11 5:36p vsilyaev
 * SW7420-1992: Updated API
 * 
 * 7   8/23/11 4:13p erickson
 * SW7420-1992: rename to surface_compositor
 * 
 * 6   8/22/11 4:11p vsilyaev
 * SW7420-1992: Updated implementation of surface compositor
 * 
 * 5   8/5/11 3:58p erickson
 * SW7420-1992: server can set number of framebuffers per display. revise
 *  tunneling api accordingly.
 * 
 * 4   8/5/11 12:05p erickson
 * SW7420-1992: add tunneled mode, remove direct_mode
 * 
 * 3   8/4/11 10:41a erickson
 * SW7420-1992: implement direct mode
 * 
 * 2   8/3/11 9:57a erickson
 * SW7420-1992: add per-display framebuffer heap to
 *  NEXUS_SurfaceCompositorSettings
 * 
 * 1   7/26/11 1:34p erickson
 * SW7420-1992: add surface_compositor module and examples
 * 
 **************************************************************************/
#ifndef NEXUS_SURFACE_COMPOSITOR_H__
#define NEXUS_SURFACE_COMPOSITOR_H__

#include "bstd.h"
#include "nexus_types.h"
#include "nexus_surface.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_surface_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
**/
typedef struct NEXUS_SurfaceCompositor *NEXUS_SurfaceCompositorHandle;

/**
Summary:
**/
NEXUS_SurfaceCompositorHandle NEXUS_SurfaceCompositor_Create( /* attr{destructor=NEXUS_SurfaceCompositor_Destroy} */
    unsigned server_id
    );

/**
Summary:
**/
void NEXUS_SurfaceCompositor_Destroy(
    NEXUS_SurfaceCompositorHandle handle
    );

/**
Summary:
**/
NEXUS_SurfaceClientHandle NEXUS_SurfaceCompositor_CreateClient( /* attr{destructor=NEXUS_SurfaceCompositor_DestroyClient} */
    NEXUS_SurfaceCompositorHandle handle,
    NEXUS_SurfaceCompositorClientId client_id
    );

/**
Summary:
**/
void NEXUS_SurfaceCompositor_DestroyClient(
    NEXUS_SurfaceClientHandle client
    );

/* must support regular and mosaic windows */
#define NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS 16

/**
Summary:
per-display settings used in NEXUS_SurfaceCompositorSettings

Description:
When you pass a NEXUS_DisplayHandle, you give up your ability to call certain nexus functions. They are:
- NEXUS_Display_Close
- NEXUS_Display_SetGraphicsSettings
- NEXUS_Display_SetGraphicsFramebuffer

When you pass a NEXUS_VideoWindowHandle, you give up your ability to call certain nexus functions. They are:
- NEXUS_VideoWindow_Close
- NEXUS_VideoWindow_SetSettings

You cannot change a display handle or framebuffer settings while the surface compositor is active.

Typical values for framebuffer.number are:
1: Single buffering results in maximum performance, but there will be tearing on the screen.
2: Double buffering is the default and standard mode. You can set tunnelCapable = true, but can only have one client.
3: Used if you will have a client with tunnelCapable = true and there are other clients which do not overlap the tunneled region.
4: Used if you will have a client with tunnelCapable = true and there are other clients which overlap the tunneled region.
*/
typedef struct NEXUS_SurfaceCompositorDisplaySettings {
    NEXUS_GraphicsSettings graphicsSettings; /* user can apply new graphics settings while surface compositor is active. however, the following
        settings will be overridden: enabled, clip, position, frameBufferCallback */
    NEXUS_DisplayHandle display;  
    struct {
        unsigned number; /* number of framebuffers. default is 2. see comment above regarding how this relates to NEXUS_SurfaceCompositorClientSettings.tunnelCapable. */
        unsigned width, height; /* desired framebuffer dimensions. if not equal to graphicsSettings.position.width/height GFD upscale is required. if zero, defaults will be used. */
        NEXUS_Pixel backgroundColor; /* used to fill background if there is no fullscreen surface */
        NEXUS_PixelFormat pixelFormat; /* pixel format of the framebuffer surfaces */
        NEXUS_HeapHandle heap;  /* set with NEXUS_Platform_GetFramebufferHeap() */
    } framebuffer;
    struct {
        NEXUS_VideoWindowHandle window; /* see above for nexus API's consumed by this interface */
        NEXUS_VideoWindowSettings settings; /* user can apply new window settings while surface compositor is active. however, the following
            settings will be overridden: position, clipRect, clipBase, visible, contentMode. */
    } window[NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS];
    struct { 
       bool overrideOrientation;           /* used to override orientation of 2D display formats */
       NEXUS_VideoOrientation orientation; /* orientation of the 3D display */
    } display3DSettings;
} NEXUS_SurfaceCompositorDisplaySettings;

/**
Summary:
server-side compositor settings
**/
typedef struct NEXUS_SurfaceCompositorSettings
{
    bool enabled; /* set to false and wait for inactiveCallback before changing display settings */
    NEXUS_CallbackDesc inactiveCallback; /* fired when SurfaceCompositor finishes going from enabled -> disabled. 
                                            After this has fired, you can change display settings. */
    NEXUS_SurfaceCompositorDisplaySettings display[NEXUS_MAX_DISPLAYS]; /* can only be changed if status.active is false. */
    NEXUS_CallbackDesc frameBufferCallback; /* called after all framebuffers have been updated */
} NEXUS_SurfaceCompositorSettings;

/**
Summary:
**/
void NEXUS_SurfaceCompositor_GetSettings(
    NEXUS_SurfaceCompositorHandle handle,
    NEXUS_SurfaceCompositorSettings *pSettings /* [out] */
    );

/**
Summary:
set surface compositor settings
**/
NEXUS_Error NEXUS_SurfaceCompositor_SetSettings(
    NEXUS_SurfaceCompositorHandle handle,
    const NEXUS_SurfaceCompositorSettings *pSettings
    );
    
typedef struct NEXUS_SurfaceCompositorClientSettings
{
    NEXUS_SurfaceComposition composition;
    bool tunnelCapable; /* client can call NEXUS_SurfaceClient_AcquireTunneledSurface */
} NEXUS_SurfaceCompositorClientSettings;

void NEXUS_SurfaceCompositor_GetClientSettings(
    NEXUS_SurfaceCompositorHandle handle,
    NEXUS_SurfaceClientHandle client,
    NEXUS_SurfaceCompositorClientSettings *pSettings /* [out] */
    );

/**
set parent surface settings
the client can set its child surface and video window settings, but it cannot set parent surface settings.
**/
NEXUS_Error NEXUS_SurfaceCompositor_SetClientSettings(
    NEXUS_SurfaceCompositorHandle handle,
    NEXUS_SurfaceClientHandle client,
    const NEXUS_SurfaceCompositorClientSettings *pSettings
    );
    
typedef struct NEXUS_SurfaceCompositorStatus
{
    bool active; /* this goes to false only after enabled is set to false and the inactiveCallback has fired. 
                    if enabled is true or if the inactiveCallback has not fired, active is true and display
                    settings cannot be changed. */
    unsigned numAcquiredTunneledSurfaces;
} NEXUS_SurfaceCompositorStatus;

NEXUS_Error NEXUS_SurfaceCompositor_GetStatus(
    NEXUS_SurfaceCompositorHandle handle,
    NEXUS_SurfaceCompositorStatus *pStatus /* [out] */
    );
    
#ifdef __cplusplus
}
#endif

#endif

