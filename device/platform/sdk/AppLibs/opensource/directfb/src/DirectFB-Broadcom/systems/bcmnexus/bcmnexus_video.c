/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: bcmnexus_video.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/5 $
 * $brcm_Date: 3/7/12 5:27p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus_video.c $
 *
 * DirectFB_1_4_15_Port/5   3/7/12 5:27p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/4   11/30/11 7:49p scchang
 * SW7420-2158: DirectFB: Consider setting pthread mutex robust attribute
 *
 * DirectFB_1_4_15_Port/3   10/5/11 12:26p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_14_Port/2   8/19/11 5:04p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/5   9/19/11 3:15p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_5_Port/4   8/15/11 12:50p robertwm
 * SW7420-2033: DirectFB: need to expose Nexus VideoWindow handle in dfb
 *  platform code.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/3   7/7/11 6:46p tross
 * SWBLURAY-26478: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_5_Port/2   6/27/11 6:25p christ
 * SW7425-782: DirectFB: Latest DirectFB 1.4.5 tree fails to shutdown when
 *  running testman / testrun.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:05p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/8   6/10/11 5:16p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/4   4/11/11 7:06p tross
 * Merge changes to get STB to compile.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   4/7/11 8:18p tross
 * Merge from latest.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/28/11 11:03a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:14p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/7   5/18/11 4:42p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/6   5/17/11 6:52p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/5   3/30/11 6:20p christ
 * SW7425-254: DirectFB: Improve performance by replacing linux-fusion
 *  calls with pthreads
 *
 * DirectFB_1_4_Port/4   3/1/11 11:30a robertwm
 * SW7420-1554: DirectFB: Fusion skirmish dismiss input/output error in
 *  Video Layer Code.
 *
 * DirectFB_1_4_Port/3   2/20/11 7:32p robertwm
 * SW7420-1394: video layer support in DFB.
 * SW7422-275: DirectFB: Enabling video layer causes Segmentation fault.
 *
 * DirectFB_1_4_Port/2   1/24/11 4:25p christ
 * SW7420-1411: DirectFB: Merge of features required for DirectFB 1.4.5
 *  phase 1.5 release - rename conflicting structure
 *
 * DirectFB_1_4_Port/1   1/19/11 12:32p scchang
 * SW7420-1394: video layer support in DFB
 *
 ***************************************************************************/


#include "bcmnexus.h"

#include "bcmnexus_screen.h"
#include "bcmnexus_layer.h"
#include "bcmnexus_utils.h"
#include "nexus_video_window.h"
#include "nexus_video_image_input.h"
#include "nexus_video_input.h"
#include "bdbg.h"

#include <core/core.h>
#include <core/system.h>
#include <core/palette.h>
#include <core/layers_internal.h>
#include <fusion/shmalloc.h>
#include <misc/conf.h>

#ifdef __GNUC__
#define UNUSED_ __attribute__ ((unused))
#else
#define UNUSED_
#endif

D_DEBUG_DOMAIN( bcmnexusVideoLayer,  "bcmNexus/VideoLayer",  "Broadcom NEXUS Video Layer" );
D_DEBUG_DOMAIN( bcmnexusVideoLayerX, "bcmNexus/VideoLayerX", "Broadcom NEXUS Video Layer eXtended debugging" );

/**********************************************************************************************************************/

typedef struct {
     CoreSurfaceBufferLock      bufferLock;     /* Remember the last surface buffer lock */
} bcmVideoRegionData;

typedef struct {
    bcmVideoRegionData         *regionData;     /* Pointer to region data */
    DFBDisplayLayerID           layerId;        /* Graphics layer ID */
    DFBScreenID                 displayId;      /* Primary or secondary display/screen */
    DFBDisplayLayerSourceID     sourceId;       /* Graphics source Id */
    bool                        layerEnabled;   /* true if this layer is enabled */

    pthread_mutex_t             lock;           /* for framebuffer change synchronisation */
    pthread_cond_t              cond;           /* for framebuffer change synchronisation */

    NEXUS_VideoWindowHandle     videoWindow;
    NEXUS_VideoImageInputHandle imageInput;
} bcmVideoLayerData;

/**********************************************************************************************************************/

static void
frameBufferCallback( void *context, int param )
{
    bcmVideoLayerData *layerData = context;

    D_DEBUG_AT( bcmnexusVideoLayerX, "  -> screen %d video layer %d framebuffer callback handled\n", layerData->displayId, param);

    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );
    pthread_cond_broadcast( &layerData->cond );
    pthread_mutex_unlock( &layerData->lock );
}

/**********************************************************************************************************************/

static int
bcmVideoLayerDataSize( void )
{
    return sizeof(bcmVideoLayerData);
}

static int
bcmVideoRegionDataSize( void )
{
    return sizeof(bcmVideoRegionData);
}

/* initialize the layer data
 * this function is called for the master fusionee only
 */
static DFBResult
bcmVideoInitLayer(         CoreLayer                  *layer,
                   UNUSED_ void                       *driver_data,
                           void                       *layer_data,
                           DFBDisplayLayerDescription *description,
                           DFBDisplayLayerConfig      *config,
                   UNUSED_ DFBColorAdjustment         *adjustment )
{
    bcmVideoLayerData    *layerData    = layer_data;
    DFBScreenID           displayId;
    DFBDisplayLayerID     layerId;
    DFB_PlatformDimension displaySize;
    pthread_mutexattr_t   mutexAttr;
    pthread_condattr_t    condAttr;

    layerId   = layerData->layerId   = DLID_PRIMARY + 2;
    displayId = layerData->displayId = dfb_screen_id_translated(dfb_layer_screen(layer));
    layerData->sourceId  = DLSID_SURFACE;

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() %s display video layer %d\n", __FUNCTION__,
                (displayId == PRIMARY_DISPLAY_ID) ? "primary" : "secondary", layerId) ;

    /* semaphore init */
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_init(&condAttr);
    pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init( &layerData->lock, &mutexAttr);
    pthread_cond_init( &(layerData->cond), &condAttr);

    pthread_mutexattr_destroy(&mutexAttr);
    pthread_condattr_destroy(&condAttr);

    /* Set type and capabilities */
    description->type = DLTF_VIDEO;
    description->caps = DLCAPS_SURFACE         | DLCAPS_OPACITY     |
                        DLCAPS_SCREEN_POSITION | DLCAPS_SCREEN_SIZE;

    /* Set the screen name. */
    snprintf( description->name, DFB_DISPLAY_LAYER_DESC_NAME_LENGTH,
              "BRCM %s disp video layer %d", (layerId == (DLID_PRIMARY + 2)) ? "primary" : "secondary", layerId );

    DFB_Platform_P_GetDisplaySize(displayId, &displaySize);

    /* fill out the default configuration */
    config->flags       = DLCONF_WIDTH | DLCONF_HEIGHT | DLCONF_PIXELFORMAT | DLCONF_BUFFERMODE | DLCONF_OPTIONS;
    config->width       = displaySize.w;
    config->height      = displaySize.h;
    config->pixelformat = DSPF_YUY2;
    config->buffermode  = DLBM_FRONTONLY;
    config->options     = DLOP_OPACITY;

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: flags=0x%08x, %dx%d, format=%s, bm=%d, opt=0x%08x, surf.caps=0x%08x\n", __FUNCTION__,
                displayId, layerId, config->flags, config->width, config->height, dfb_pixelformat_name(config->pixelformat),
                config->buffermode, config->options, config->surface_caps );

    return DFB_OK;
}


static DFBResult
bcmVideoShutdownLayer( UNUSED_ CoreLayer *layer,
                       UNUSED_ void      *driver_data,
                               void      *layer_data )
{
    bcmVideoLayerData    *layerData    = layer_data;
    DFBScreenID           displayId;
    DFBDisplayLayerID     layerId;

    layerId   = layerData->layerId   = DLID_PRIMARY + 2;
    displayId = layerData->displayId = dfb_screen_id_translated(dfb_layer_screen(layer));

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() %s display video layer %d\n", __FUNCTION__,
                (displayId == PRIMARY_DISPLAY_ID) ? "primary" : "secondary", layerId) ;

    pthread_mutex_destroy( &layerData->lock );
    pthread_cond_destroy( &layerData->cond );

    return DFB_OK;
}

static DFBResult
bcmVideoTestRegion( UNUSED_ CoreLayer                  *layer,
                    UNUSED_ void                       *driver_data,
                            void                       *layer_data,
                            CoreLayerRegionConfig      *config,
                            CoreLayerRegionConfigFlags *failed )
{
    bcmVideoLayerData          *layerData    = layer_data;
    CoreLayerRegionConfigFlags  fail         = CLRCF_NONE;
    DFBScreenID                 displayId;
    DFBDisplayLayerID           layerId;
    DFB_PlatformDimension       displaySize;

    displayId  = layerData->displayId;
    layerId    = layerData->layerId;

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d (%dx%d) format=%s, caps=0x%08x, bm=%d,\n"
                               "options=0x%08x, source_id=%d, opacity=%d [%dx%d@%d,%d]=>[%dx%d@%d,%d]\n",
                               __FUNCTION__, displayId, layerId, config->width, config->height, dfb_pixelformat_name(config->format),
                               config->surface_caps, config->buffermode, config->options, config->source_id, config->opacity,
                               config->source.w, config->source.h, config->source.x, config->source.y,
                               config->dest.w, config->dest.h, config->dest.x, config->dest.y);

    DFB_Platform_P_GetDisplaySize(displayId, &displaySize);

    /* Check width */
    if (config->width > displaySize.w)
        fail |= CLRCF_WIDTH;

    /* Check height */
    if (config->height > displaySize.h)
        fail |= CLRCF_HEIGHT;

    /* Check format */
    switch (config->format) {
        case DSPF_YUY2:
            break;

        default:
            fail |= CLRCF_FORMAT;
    }

    /* Check options */
    if (!D_FLAGS_ARE_IN( config->options, DLOP_OPACITY ))
        fail |= CLRCF_OPTIONS;

    if (failed)
        *failed = fail;

    if (fail)
    {
         D_ERROR("bcmNexus/VideoLayer: %s() screen %d video layer %d: Failed (flags = 0x%08x)!\n", __FUNCTION__, displayId, layerId, fail);
         return DFB_UNSUPPORTED;
    }

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: Exit.\n", __FUNCTION__, displayId, layerId );
    return DFB_OK;
}

static DFBResult
bcmVideoAddRegion( UNUSED_ CoreLayer             *layer,
                           void                  *driver_data,
                           void                  *layer_data,
                           void                  *region_data,
                   UNUSED_ CoreLayerRegionConfig *config )
{
    DFBResult                      res          = DFB_OK;
    DFBBCMNEXUS                   *dfb_bcmnexus = driver_data;
    bcmVideoLayerData             *layerData    = layer_data;
    NEXUS_VideoImageInputSettings  imageInputSetting;
    NEXUS_VideoWindowSettings      windowCfg;
    DFBScreenID                    displayId;
    DFBDisplayLayerID              layerId;
    DFB_PlatformNexusHandle        windowHandle;

    D_ASSERT( dfb_bcmnexus != NULL );

    displayId = layerData->displayId;
    layerId   = layerData->layerId;

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: format=%s, region=%dx%d, caps=0x%08x,\n"
                                    "bm=%d, options=0x%08x, source_id=%d, opacity=%d [%dx%d@%d,%d] -> [%dx%d@%d,%d]\n",
                                     __FUNCTION__, displayId, layerId, dfb_pixelformat_name(config->format),
                                     config->width, config->height, config->surface_caps, config->buffermode,
                                     config->options, config->source_id, config->opacity,
                                     config->source.w, config->source.h, config->source.x, config->source.y,
                                     config->dest.w, config->dest.h, config->dest.x, config->dest.y);

    /* Save current region pointer in layer data */
    layerData->regionData = region_data;

    /* Video window 0 is already opened by platform code normally */
    res = DFB_Platform_GetDisplayWindow(GET_DISPLAY_HND(dfb_bcmnexus, displayId), 0, &windowHandle);

    if (!windowHandle || res != DFB_OK) {
        D_ERROR( "Broadcom/VideoLayer: %s() screen %d video layer %d: Could not open video window 0!\n", __FUNCTION__, displayId, layerId );
        res = DFB_FAILURE;
    } else {
        layerData->videoWindow = windowHandle;

        D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: Going to set video window settings for window handle %p\n",
                    __FUNCTION__, displayId, layerId, (void *)layerData->videoWindow );

        /* Should set video window settings before adding image input to video window */
        NEXUS_VideoWindow_GetSettings(layerData->videoWindow, &windowCfg);
        windowCfg.position.x = 0;
        windowCfg.position.y = 0;
        windowCfg.position.width = config->width;
        windowCfg.position.height = config->height;

        if (NEXUS_VideoWindow_SetSettings(layerData->videoWindow, &windowCfg) != NEXUS_SUCCESS) {
            D_ERROR( "Broadcom/VideoLayer: %s() screen %d video layer %d: Could not get video window 0 settings!\n", __FUNCTION__, displayId, layerId );
            res = DFB_FAILURE;
        } else {
            layerData->imageInput = NEXUS_VideoImageInput_Open( 0, NULL );

            if (!layerData->imageInput) {
                D_ERROR( "Broadcom/VideoLayer: %s() screen %d video layer %d: Could not open video image input 0!\n",  __FUNCTION__, displayId, layerId );
                res = DFB_FAILURE;
            } else {
                NEXUS_VideoImageInput_GetSettings( layerData->imageInput, &imageInputSetting );
                imageInputSetting.imageCallback.callback = frameBufferCallback;
                imageInputSetting.imageCallback.context  = layerData;
                imageInputSetting.imageCallback.param    = layerId;

                if (NEXUS_VideoImageInput_SetSettings( layerData->imageInput, &imageInputSetting ) != NEXUS_SUCCESS) {
                    D_ERROR( "Broadcom/VideoLayer: %s() screen %d video layer %d: Could not get video image input 0 settings!\n", __FUNCTION__, displayId, layerId );
                    res = DFB_FAILURE;
                } else {
                    if (NEXUS_VideoWindow_AddInput( layerData->videoWindow, NEXUS_VideoImageInput_GetConnector(layerData->imageInput) ) != NEXUS_SUCCESS) {
                        D_ERROR( "Broadcom/VideoLayer: %s() screen %d video layer %d: Could not add video image input 0 to video window 0!\n", __FUNCTION__, displayId, layerId );
                        res = DFB_FAILURE;
                    }
                }
            }
        }
    }
    D_ASSERT( res == DFB_OK );

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: Exit [%d].\n", __FUNCTION__, displayId, layerId, res );
    return res;
}

static void
setVideoLayerBuffer( bcmVideoLayerData     *layerData,
                     bcmVideoRegionData    *regionData,
                     CoreSurface           *surface,
                     CoreSurfaceBufferLock *lock,
                     bool                   wait )
{
    DFBScreenID         displayId;
    DFBDisplayLayerID   layerId;
    NEXUS_SurfaceHandle hSurface;
    struct timespec     timeout;

    displayId = layerData->displayId;
    layerId   = layerData->layerId;
    hSurface  = lock->handle;

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: enabled=%d surface=%p [handle=%p, oldhandle=%p]\n",
                 __FUNCTION__, displayId, layerId, layerData->layerEnabled, (void *)surface, (void *)hSurface, (void *)regionData->bufferLock.handle );

    if (layerData->layerEnabled && (hSurface != regionData->bufferLock.handle))
    {
        PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

        /* Save video framebuffer */
        regionData->bufferLock = *lock;

        NEXUS_VideoImageInput_SetSurface( layerData->imageInput, hSurface );

        if (wait) {
            D_DEBUG_AT( bcmnexusVideoLayerX, "%s() screen %d video layer %d: waiting for framebuffer %p callback...\n", __FUNCTION__,
                        displayId, layerId, (void *)surface );

            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_nsec += 100000000; // 0.1sec in nanosec

            pthread_cond_timedwait(&layerData->cond,
                                   &layerData->lock,
                                   &timeout);

            D_DEBUG_AT( bcmnexusVideoLayerX, "%s() screen %d video layer %d: received callback for framebuffer %p.\n", __FUNCTION__,
                        displayId, layerId, (void *)surface );
        }
        pthread_mutex_unlock( &layerData->lock );
    }
}

static DFBResult
bcmVideoSetRegion( UNUSED_ CoreLayer                  *layer,
                   UNUSED_ void                       *driver_data,
                           void                       *layer_data,
                           void                       *region_data,
                           CoreLayerRegionConfig      *config,
                           CoreLayerRegionConfigFlags  updated,
                           CoreSurface                *surface,
                   UNUSED_ CorePalette                *palette,
                           CoreSurfaceBufferLock      *left_lock,
                   UNUSED_ CoreSurfaceBufferLock      *right_lock )

{
    bcmVideoLayerData         *layerData    = layer_data;
    bcmVideoRegionData        *regionData   = region_data;
    NEXUS_VideoWindowSettings  windowCfg;
    DFBScreenID                displayId;
    DFBDisplayLayerID          layerId;

    D_ASSERT( (DFBBCMNEXUS*)driver_data != NULL );

    displayId    = layerData->displayId;
    layerId      = layerData->layerId;

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: updated flags=0x%08x, surface=%p,\n"
                                    "handle=%p, format=%s, region=%dx%d, caps=0x%08x, bm=%d,\n"
                                    "options=0x%08x, source_id=%d, opacity=%d [%dx%d@%d,%d] -> [%dx%d@%d,%d]\n",
                                    __FUNCTION__, displayId, layerId, updated, (void *)surface, left_lock->handle, dfb_pixelformat_name(config->format),
                                    config->width, config->height, config->surface_caps, config->buffermode,
                                    config->options, config->source_id, config->opacity,
                                    config->source.w, config->source.h, config->source.x, config->source.y,
                                    config->dest.w, config->dest.h, config->dest.x, config->dest.y);

    /* Get whether the screen/display is on */
    layerData->layerEnabled = (bcmGetPowerMode(dfb_screens_at(displayId)) == DSPM_ON);

    /* Set Surface to Source */
    NEXUS_VideoWindow_GetSettings( layerData->videoWindow, &windowCfg );

    windowCfg.visible         = true;
    windowCfg.clipBase.x      = 0;
    windowCfg.clipBase.y      = 0;
    windowCfg.clipBase.width  = config->width;
    windowCfg.clipBase.height = config->height;
    windowCfg.position.x      = config->dest.x;
    windowCfg.position.y      = config->dest.y;
    /* window position has to be inside display boundary for now */
    if ((config->dest.x + config->dest.w) > config->width)
        windowCfg.position.width  = config->width*2 - (config->dest.x + config->dest.w);
    else
        windowCfg.position.width  = config->dest.w;
    /* window position has to be inside display boundary for now */
    if ((config->dest.y + config->dest.h) > config->height)
        windowCfg.position.height = config->height*2 - (config->dest.y + config->dest.h);
    else
        windowCfg.position.height = config->dest.h;

    windowCfg.clipRect.x      = config->source.x;
    windowCfg.clipRect.y      = config->source.y;
    windowCfg.clipRect.width  = config->source.w;
    windowCfg.clipRect.height = config->source.h;

    if (config->options & DLOP_OPACITY) {
        windowCfg.sourceBlendFactor = NEXUS_CompositorBlendFactor_eConstantAlpha;
        windowCfg.destBlendFactor   = NEXUS_CompositorBlendFactor_eInverseConstantAlpha;
    }
    else {
        windowCfg.sourceBlendFactor = NEXUS_CompositorBlendFactor_eOne;
        windowCfg.destBlendFactor   = NEXUS_CompositorBlendFactor_eZero;
    }

    windowCfg.alpha         =
    windowCfg.constantAlpha = config->opacity;

    windowCfg.pixelFormat = bcmnexus_getNexusPixelFormat( config->format );

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: clip=%dx%d@%d,%d,position=%dx%d@%d,%d,sourceBlendFactor=%d,destBlendFactor=%d,"
                                    "alpha=%d,constantAlpha=%d\n", __FUNCTION__, displayId, layerId,
                                    windowCfg.clipRect.width, windowCfg.clipRect.height, windowCfg.clipRect.x, windowCfg.clipRect.y,
                                    windowCfg.position.width, windowCfg.position.height, windowCfg.position.x, windowCfg.position.y,
                                    windowCfg.sourceBlendFactor, windowCfg.destBlendFactor, windowCfg.alpha, windowCfg.constantAlpha );

    NEXUS_VideoWindow_SetSettings( layerData->videoWindow, &windowCfg );

    setVideoLayerBuffer( layerData, regionData, surface, left_lock, false );

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: Exit.\n", __FUNCTION__, displayId, layerId );
    return DFB_OK;
}

static DFBResult
bcmVideoRemoveRegion( UNUSED_ CoreLayer *layer,
                      UNUSED_ void      *driver_data,
                              void      *layer_data,
                      UNUSED_ void      *region_data )
{
    bcmVideoLayerData *layerData    = layer_data;

    D_ASSERT( (DFBBCMNEXUS*)driver_data != NULL );

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d\n", __FUNCTION__, layerData->displayId, layerData->layerId );

    NEXUS_VideoImageInput_SetSurface( layerData->imageInput, NULL );
    NEXUS_VideoWindow_RemoveInput( layerData->videoWindow, NEXUS_VideoImageInput_GetConnector(layerData->imageInput) );
    NEXUS_VideoInput_Shutdown( NEXUS_VideoImageInput_GetConnector(layerData->imageInput) );
    NEXUS_VideoImageInput_Close( layerData->imageInput );

    /* NEXUS Video Window is automatically closed by platform layer now. */

    layerData->regionData = NULL;
    return DFB_OK;
}

static DFBResult
bcmVideoFlipRegion( UNUSED_ CoreLayer             *layer,
                    UNUSED_ void                  *driver_data,
                            void                  *layer_data,
                            void                  *region_data,
                            CoreSurface           *surface,
                            DFBSurfaceFlipFlags    flags,
                    UNUSED_ const DFBRegion       *left_update,
                            CoreSurfaceBufferLock *left_lock,
                    UNUSED_ const DFBRegion       *right_update,
                    UNUSED_ CoreSurfaceBufferLock *right_lock )
{
    bcmVideoLayerData *layerData    = layer_data;

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: surface=%p, flags=0x%08x, handle=%p\n", __FUNCTION__,
                layerData->displayId, layerData->layerId, (void *)surface, flags, (void *)left_lock->handle );

    setVideoLayerBuffer( layerData, region_data, surface, left_lock, (flags & DSFLIP_WAIT) );

    /* Inform DirectFB that the buffers have changed function */
    dfb_surface_flip( surface, false );

    return DFB_OK;
}

static DFBResult
bcmVideoUpdateRegion( UNUSED_ CoreLayer             *layer,
                      UNUSED_ void                  *driver_data,
                              void                  *layer_data,
                              void                  *region_data,
                              CoreSurface           *surface,
                              const DFBRegion       *left_update,
                              CoreSurfaceBufferLock *left_lock,
                      UNUSED_ const DFBRegion       *right_update,
                      UNUSED_ CoreSurfaceBufferLock *right_lock )
{
    bcmVideoLayerData *layerData    = layer_data;

    D_DEBUG_AT( bcmnexusVideoLayer, "%s() screen %d video layer %d: surf=%p, handle=%p [%d,%d->%d,%d]\n", __FUNCTION__,
                layerData->displayId, layerData->layerId, (void *)surface, (void *)left_lock->handle,
                left_update->x1, left_update->y1, left_update->x2, left_update->y2 );

    setVideoLayerBuffer( layerData, region_data, surface, left_lock, false );

    return DFB_OK;
}

DisplayLayerFuncs bcmnexusVideoFuncs = {
    .LayerDataSize         = bcmVideoLayerDataSize,
    .RegionDataSize        = bcmVideoRegionDataSize,
    .InitLayer             = bcmVideoInitLayer,
    .ShutdownLayer         = bcmVideoShutdownLayer,
    .TestRegion            = bcmVideoTestRegion,
    .AddRegion             = bcmVideoAddRegion,
    .SetRegion             = bcmVideoSetRegion,
    .RemoveRegion          = bcmVideoRemoveRegion,
    .FlipRegion            = bcmVideoFlipRegion,
    .UpdateRegion          = bcmVideoUpdateRegion,
};
