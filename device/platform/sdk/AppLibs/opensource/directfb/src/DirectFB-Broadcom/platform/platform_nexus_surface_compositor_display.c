/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: platform_nexus_surface_compositor_display.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/5 $
 * $brcm_Date: 3/29/12 11:58a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/platform_nexus_surface_compositor_display.c $
 *
 * DirectFB_1_4_15_Port/5   3/29/12 11:58a mnelis
 * SW7425-2286: DirectFB-XS: Push / Recycle logic in NSC multi buffered
 *  mode incorrect
 *
 * DirectFB_1_4_15_Port/4   3/13/12 4:02p robertwm
 * SW7425-2616: DirectFB-1.4.15: Cannot run Nexus or OGL Nexus
 *  applications in DFB-XS mode.
 *
 * DirectFB_1_4_15_Port/3   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/2   2/29/12 3:41p apandav
 * SW7552-208: DirectFB: CVBS can't output 576i due to DFB identify
 *  vertical scaling incorrectly on 7552 platform.
 *
 * DirectFB_1_4_15_Port/1   2/9/12 2:33p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB-XS_1_4_15_Port/4   1/27/12 8:24p christ
 * SW745-2286: DirectFB-XS: Push / Recycle logic in NSC multi buffered
 *  mode incorrect
 *
 * DirectFB-XS_1_4_15_Port/3   12/9/11 11:07a robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB-XS_1_4_15_Port/2   12/8/11 3:04p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB-XS_1_4_15_Port/1   11/22/11 5:14p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 *****************************************************************************/
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#include "platform_nexus_init.h"

#include "platform_display.h"

#if NEXUS_HAS_SURFACE_COMPOSITOR
#include <nexus_surface_compositor.h>
#endif

DFB_P_DisplaySettings *dfb_p_displaysettings;

// Copy rectangle contents from src to dest
#define SET_RECT(dest,src) { (dest).x = (src).x;          \
                             (dest).y = (src).y;          \
                             (dest).width = (src).width;  \
                             (dest).height = (src).height; }

BDBG_MODULE(nexus_surface_compositor_init);

static uint32_t surfaceSubmitCount = 0;
static BKNI_EventHandle recycledEvent = NULL;
#if BCMNEXUS_NSC_SUPPORT != BCMNEXUS_NSC_TUNNELLED_MODE
static bool pushRecycleMode = true;
static void *lastFramebufferSet = NULL;
#endif
static DFB_PlatformCallbackDesc recycledCallbackContext = { NULL, NULL, 0 };

static void DFB_Platform_P_RecycledCallbackFunction(void *data, int unused)
{
    DFB_PlatformCallbackDesc *pCtx = (DFB_PlatformCallbackDesc *)data;

    BDBG_MSG(("------>%s: callback=%p >>>", __FUNCTION__, pCtx->callback));
    BSTD_UNUSED(unused);
    BKNI_SetEvent(recycledEvent);

    if (pCtx->callback)
    {
        (*pCtx->callback)(pCtx->context, pCtx->param);
    }
}

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_MULTIBUFFER_MODE
static void recycle_all(NEXUS_SurfaceClientHandle blit_client)
{
    NEXUS_Error err;
    unsigned num;

    do {
        NEXUS_SurfaceHandle surface;
        err = NEXUS_SurfaceClient_RecycleSurface(blit_client, &surface, 1, &num);
        surfaceSubmitCount -= num;
        BDBG_ASSERT(!err);
        /* Supposing double buffering */
        if (surfaceSubmitCount > 1) {
            BDBG_MSG(("%s: BKNI_WaitForEvent...", __FUNCTION__));
            err = BKNI_WaitForEvent(recycledEvent, BKNI_INFINITE);
            BDBG_ASSERT(!err);
        }
    } while (num);
}
#endif

void
DFB_Platform_P_HDMI_Hotplug_Callback( void *pParam,
                                      int   iParam )
{
    BSTD_UNUSED(pParam);
    BSTD_UNUSED(iParam);
}

DFB_PlatformResult
DFB_Platform_P_HdmiHotplugHandler( DFB_PlatformNexusHandle displayHandle,
                                   DFB_PlatformNexusHandle hdmiHandle )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(hdmiHandle);
    return DFB_PLATFORM_OK;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Hdmi( DFB_PlatformNexusHandle displayHandle,
                                      int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Hdmi( DFB_PlatformNexusHandle displayHandle,
                                         int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Component( DFB_PlatformNexusHandle displayHandle,
                                           int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Component( DFB_PlatformNexusHandle displayHandle,
                                              int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Composite( DFB_PlatformNexusHandle displayHandle,
                                           int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Composite( DFB_PlatformNexusHandle displayHandle,
                                              int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Svideo( DFB_PlatformNexusHandle displayHandle,
                                        int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Svideo( DFB_PlatformNexusHandle displayHandle,
                                           int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Scart( DFB_PlatformNexusHandle displayHandle,
                                       int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Scart( DFB_PlatformNexusHandle displayHandle,
                                          int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Rfm( DFB_PlatformNexusHandle displayHandle,
                                     int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Rfm( DFB_PlatformNexusHandle displayHandle,
                                        int                     outputIndex )
{
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
    return DFB_PLATFORM_UNSUPPORTED;
}

void
DFB_Platform_P_DFBFormatToDisplaySize( DFB_PlatformVideoFormat format,
                                       DFB_PlatformDimension  *pSize )
{
    NEXUS_VideoFormatInfo info;

    if (format < DFB_PlatformVideoFormat_eMax)
    {
        NEXUS_VideoFormat_GetInfo(DFB_Platform_P_DFBToNexusVideoFormat(format), &info);
        pSize->w = info.width;
        pSize->h = info.height;
    }
}

DFB_PlatformResult
DFB_Platform_P_SetDisplaySettings( DFB_PlatformNexusHandle         displayHandle,
                                   DFB_Platform_P_DisplaySettings *pSettings )
{

    DFB_PlatformResult res;
    int displayIndex;

    displayIndex = DFB_Platform_P_GetDisplayIndex(displayHandle);
    if (displayIndex < 0)
    {
        res = DFB_PLATFORM_INVARG;
    }

    DFB_Platform_P_DFBFormatToDisplaySize(pSettings->format, &dfb_p_displaysettings->display[displayIndex].displaySize);

    return res;
}

/**
Summary:
Get current graphics compositing settings.
**/
DFB_PlatformResult DFB_Platform_P_GetGraphicsSettings(
    DFB_PlatformNexusHandle display,
    DFB_Platform_P_GraphicsSettings *pSettings /* [out] */
    )
{
    NEXUS_SurfaceClientSettings nexusSettings;

    NEXUS_SurfaceClient_GetSettings(display, &nexusSettings);

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    pSettings->enabled = nexusSettings.composition.visible;
    pSettings->alpha   = 0xFF;
    pSettings->zorder  = nexusSettings.composition.zorder;

    pSettings->chromakeyEnabled = false;

    SET_RECT(pSettings->position, nexusSettings.composition.position);
    SET_RECT(pSettings->clip, nexusSettings.composition.position);

    pSettings->frameBufferCallback.callback = recycledCallbackContext.callback;
    pSettings->frameBufferCallback.context  = recycledCallbackContext.context;
    pSettings->frameBufferCallback.param    = recycledCallbackContext.param;

    pSettings->sourceBlendFactor = DFB_PlatformBlendFactor_eSourceAlpha;
    pSettings->destBlendFactor = DFB_PlatformBlendFactor_eInverseSourceAlpha;

    pSettings->constantAlpha = nexusSettings.composition.constantColor;

    pSettings->stereoOffset = nexusSettings.composition.rightViewOffset;

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));

    return DFB_PLATFORM_OK;
}

/**
Summary:
Set graphics compositing settings.
**/
DFB_PlatformResult DFB_Platform_P_SetGraphicsSettings(
    DFB_PlatformNexusHandle display,
    const DFB_Platform_P_GraphicsSettings *pSettings
    )
{
    NEXUS_Error rc;
    NEXUS_SurfaceClientSettings nexusSettings;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    if (recycledEvent == NULL)
    {
        BKNI_CreateEvent(&recycledEvent);
    }

    NEXUS_SurfaceClient_GetSettings(display, &nexusSettings);

    recycledCallbackContext.callback = pSettings->frameBufferCallback.callback;
    recycledCallbackContext.context  = pSettings->frameBufferCallback.context;
    recycledCallbackContext.param    = pSettings->frameBufferCallback.param;

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_MULTIBUFFER_MODE
    /*Need the recycled callback in all cases to signal to DFB that somthing's been flipped.*/
    if (pushRecycleMode)
    {
        nexusSettings.recycled.callback = DFB_Platform_P_RecycledCallbackFunction;
        nexusSettings.recycled.context  = &recycledCallbackContext;
    }
    else
    {
        nexusSettings.displayed.callback = DFB_Platform_P_RecycledCallbackFunction;
        nexusSettings.displayed.context  = &recycledCallbackContext;
    }

   // nexusSettings.orientation = NEXUS_VideoOrientation_e3D_LeftRight;

#else
    /* Tunnelled. */
    nexusSettings.displayed.callback = DFB_Platform_P_RecycledCallbackFunction;
    nexusSettings.displayed.context  = &recycledCallbackContext;
#endif

    rc = NEXUS_SurfaceClient_SetSettings(display, &nexusSettings);

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));

    return (rc == NEXUS_SUCCESS) ? DFB_PLATFORM_OK : DFB_PLATFORM_FAILURE;
}

void
DFB_Platform_P_GetDefaultDisplayGraphicsFramebuffer3D( DFB_Platform_P_DisplayGraphicsFramebuffer3D *pFrameBuffer3D )
{
    BSTD_UNUSED(pFrameBuffer3D);
}

DFB_PlatformResult
DFB_Platform_P_SetDisplayGraphicsFramebuffer3D(       DFB_PlatformNexusHandle                      display,
                                                const DFB_Platform_P_DisplayGraphicsFramebuffer3D *pFrameBuffer3D,
                                                const DFB_PlatformRect *pLeftUpdateRect,
                                                const DFB_PlatformRect *pRightUpdateRect,
                                                bool  singleBuffered)
{
    NEXUS_Error err;

    /*Until we do 3D NSC*/
    BSTD_UNUSED(*pRightUpdateRect);

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_TUNNELLED_MODE
    BSTD_UNUSED(singleBuffered);
    BDBG_MSG(("%s: NEXUS_SurfaceClient_PushTunnelledSurface %p ...", __FUNCTION__, (void *)pFrameBuffer3D->main));
    err = NEXUS_SurfaceClient_PushTunneledSurface(display ,pFrameBuffer3D->main, (NEXUS_Rect *)pLeftUpdateRect, false);
#else

    BDBG_MSG(("%s: NEXUS_SurfaceClient_Push/Set/UpdateSurface %p (surfaceSubmitCount=%d)", __FUNCTION__, (void *)pFrameBuffer3D->main, surfaceSubmitCount));

   if (!surfaceSubmitCount)
    {
        /* Event can be signaled from the Clear
           so reset it so that we get true events.*/
        BKNI_ResetEvent(recycledEvent);
    }


    if (pLeftUpdateRect)
        BDBG_MSG(("%s: Updating rectangle %d x %d @ %d x %d",__FUNCTION__,
                  pLeftUpdateRect->width,pLeftUpdateRect->height,pLeftUpdateRect->x,pLeftUpdateRect->y));
    else
        BDBG_MSG(("%s: No update rectangle",__FUNCTION__));


    if (pushRecycleMode) {
        err = NEXUS_SurfaceClient_PushSurface(display, pFrameBuffer3D->main, (NEXUS_Rect *) pLeftUpdateRect, false);
        surfaceSubmitCount++;

        BDBG_ASSERT(!err);
        recycle_all(display);
    }
    else {
        BDBG_MSG(("%s: NewSurface 0x%08x OldSurface 0x%08x", __FUNCTION__, pFrameBuffer3D->main, lastFramebufferSet));

        /*Must wait for this because it means that the old buffer has been copied out*/
        if (surfaceSubmitCount)
        {
            BKNI_WaitForEvent(recycledEvent, BKNI_INFINITE);
            surfaceSubmitCount--;
        }

        if (lastFramebufferSet != &pFrameBuffer3D->main) {
            err = NEXUS_SurfaceClient_SetSurface(display, pFrameBuffer3D->main);
            lastFramebufferSet = (void *) pFrameBuffer3D->main;
            surfaceSubmitCount++;
        }
        else {
            err = NEXUS_SurfaceClient_UpdateSurface(display, (NEXUS_Rect *) pLeftUpdateRect);
            surfaceSubmitCount++;
        }
    }

    if (singleBuffered){
            /* Applications that draw into the framebuffer
               without flipping can't be seen without this */
            err = NEXUS_SurfaceClient_PublishSurface(display);
        }
#endif

    return (err == NEXUS_SUCCESS) ? DFB_PLATFORM_OK : DFB_PLATFORM_FAILURE;
}

DFB_PlatformResult
DFB_Platform_P_ClearDisplayGraphicsFramebuffer3D(DFB_PlatformNexusHandle                      display)
{

    BDBG_MSG(("%s: Clearing.", __FUNCTION__));

    NEXUS_SurfaceClient_Clear(display);

    surfaceSubmitCount = 0;
    if (recycledEvent)
    {
        /* Release threads waiting
           for the recycled event. */
        BKNI_SetEvent(recycledEvent);
    }

    return DFB_PLATFORM_OK;
}

bool
DFB_Platform_P_DisplayWindow_GetDefaultInitSettings( unsigned                               windowSlot,
                                                     DFB_PlatformDisplayWindowInitSettings *pSettings )
{
    BSTD_UNUSED(windowSlot);
    BSTD_UNUSED(pSettings);
    return false;
}

DFB_PlatformResult
DFB_Platform_P_DisplayWindow_Init( DFB_PlatformDisplayWindowInitSettings *pSettings,
                                   DFB_PlatformNexusHandle               *pHandle )
{
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(pHandle);
    return DFB_PLATFORM_UNSUPPORTED;
}

void
DFB_Platform_P_DisplayWindow_Uninit( DFB_PlatformNexusHandle handle)
{
    BSTD_UNUSED(handle);
}

DFB_PlatformResult
DFB_Platform_P_DisplayOutput_Init(DFB_PlatformDisplayOutputInitSettings *pSettings,
                                  DFB_PlatformNexusHandle               *pHandle )
{
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(pHandle);
    return DFB_PLATFORM_UNSUPPORTED;
}

/*Dummy function to check GFD vscale support*/
bool
DFB_Platform_P_CheckGfxVScl( int displayIndex )
{
    BSTD_UNUSED(displayIndex);
    return false;
}

DFB_PlatformResult
DFB_Platform_P_Display_Init( unsigned                         displayIndex,
                             DFB_PlatformDisplayInitSettings *pSettings,
                             DFB_PlatformNexusHandle         *pHandle)
{
    DFB_PlatformResult             res;
    int   clientId;
    char *pClientId;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pHandle);

    if (displayIndex != 0)
        return DFB_PLATFORM_OK;

    clientId = displayIndex;

    if ((pClientId = getenv("dfb_clientid")) != NULL)
    {
        clientId = strtoul(pClientId, NULL, 10);
    }
    BDBG_MSG(("%s: Acquiring surface client id=%d", __FUNCTION__, clientId));

    *pHandle = NEXUS_SurfaceClient_Acquire(clientId);

    if (*pHandle == NULL)
    {
        BDBG_ERR(("%s: Cannot open surface compositor for display %d!", __FUNCTION__, displayIndex));
        res = DFB_PLATFORM_FAILURE;
    }
    else
    {
        BDBG_MSG(("%s: Display %d Handle=%p", __FUNCTION__, displayIndex, *pHandle));
        dfb_p_displaysettings->display[displayIndex].displayHandle               = *pHandle;
        res = DFB_PLATFORM_OK;
    }

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_TUNNELLED_MODE
    BDBG_MSG(("%s: Display %d: Running NSC in Tunnelled mode", __FUNCTION__, displayIndex));
#else
    if (getenv("dfb_nexus_compositor_set"))
    {
        BDBG_MSG(("%s: Display %d: Running NSC in Set/Update mode", __FUNCTION__, displayIndex));
        pushRecycleMode = false;
    }
    else
    {
        BDBG_MSG(("%s: Display %d: Running NSC in Push/Recycle mode", __FUNCTION__, displayIndex));
        pushRecycleMode = true;
    }
#endif

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

void
DFB_Platform_P_Display_Uninit( DFB_PlatformNexusHandle handle)
{
    int displayIndex = 0;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(handle);

    NEXUS_SurfaceClient_Release(handle);

    if (recycledEvent)
    {
        BKNI_DestroyEvent(recycledEvent);
        recycledEvent = NULL;
    }

    dfb_p_displaysettings->display[displayIndex].displayHandle = NULL;

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

bool
DFB_Platform_P_DisplayOutput_GetDefaultInitSettings( unsigned                               outputSlot,
                                                     DFB_PlatformDisplayOutputInitSettings *pSettings )
{
    bool init = false;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BSTD_UNUSED(outputSlot);
    BSTD_UNUSED(pSettings);

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return init;
}
