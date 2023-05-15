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
 * $brcm_Workfile: bcmnexus_layer.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/6 $
 * $brcm_Date: 3/7/12 5:26p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus_layer.h $
 * 
 * DirectFB_1_4_15_Port/6   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 * 
 * DirectFB_1_4_15_Port/5   1/12/12 7:31p apandav
 * SW7125-1207: DirectFB: Support an option to have 960x1080 framebuffer
 *  to display in 1080p mode
 * 
 * DirectFB_1_4_15_Port/4   12/19/11 5:04p robertwm
 * SW7425-2002: DirectFB: Moving cursor when running windowed app on top
 *  of df_window exposes lower app (flipping).
 * SW7420-1515: [DirectFB] Got black bar when moving mouse in running
 *  df_andi + df_window (DFB Multi-App + SaWMan mode + Kernel Mode)
 * 
 * DirectFB_1_4_15_Port/3   11/8/11 3:35p christ
 * SW7425-1703: DirectFB: Move NEXUS_Display calls into platform layer
 *
 * DirectFB_1_4_14_Port/2   9/13/11 12:26p robertwm
 * SW7405-5474: deadlock between frameBufferThread() and
 *  bcmSetLayerFrameBuffer() in bcmnexus_layer.c
 * SW7405-5486: DirectFB appears to hang in DFBCreate() in Inaris.
 * SW7420-2053: DirectFB-1.4.5: Hang in ShutdownLayer() function after
 *  <CTRL><C> a DFB application.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:05p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/12   6/10/11 5:17p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   4/5/11 2:41p tross
 * Remove unnecessary bcmRemoveRegion prototype.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/28/11 11:03a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/11   4/13/11 11:48a apandav
 * SWDTV-6602: DirectFB: Adding Vsync Patch from Denis(DOK)
 *
 * DirectFB_1_4_Port/10   3/4/11 5:04p robertwm
 * SW7420-1593: DirectFB-1.4.1: Add ShutdownLayer() API.
 *
 * DirectFB_1_4_Port/9   3/4/11 3:30p robertwm
 * SW7420-1590: DirectFB-1.4.5: Deadlock when closing down multiple
 *  applications.
 *
 * DirectFB_1_4_Port/8   3/1/11 12:01p christ
 * SW7420-1545: DirectFB: Application hang when using multi app DirectFB
 *  with SawMan window manager.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:14p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/7   2/8/11 1:31p robertwm
 * SW7420-1471: DirectFB: Replace fusion skirmishes with pthread
 *  semantics.
 *
 * DirectFB_1_4_Port/6   2/8/11 1:13p robertwm
 * SW7420-1467: DirectFB: Multiple apps should NOT share NEXUS Display
 *  handle.
 *
 * DirectFB_1_4_Port/5   1/24/11 4:32p christ
 * SW7420-1411: DirectFB: Merge of features required for DirectFB 1.4.5
 *  phase 1.5 release - Add some encoder support
 *
 * DirectFB_1_4_Port/4   8/11/10 2:00a robertwm
 * SW7405-4250: DirectFB: Screen destination rectangle and source
 *  rectangle not saved across contexts.
 * SW7405-4743: DirectFB: Multiple instances of df_andi.c do not get
 *  displayed.
 *
 * DirectFB_1_4_Port/3   4/9/10 4:31p robertwm
 * SW7405-4179: DirectFB: Support setting the z-order (level) of a layer.
 * SW7405-4180: DirectFB: Require independent setting of graphics
 *  resolution on multiple output paths.
 *
 * DirectFB_1_4_Port/2   12/16/09 9:49p robertwm
 * SW7400-2643: DirectFB: Add support for setting/getting video modes.
 *
 ***************************************************************************/

#ifndef __CORE__BCMNEXUS__LAYER_H__
#define __CORE__BCMNEXUS__LAYER_H__

#include <directfb_version.h>
#include <core/layers.h>
#include <direct/thread.h>

/******************************************************************************/

typedef struct {
     CoreLayerRegionConfig          config;             /* Current Region Configuration */
     bool                           vscale;             /* Set to true when a vertical scale using blitter is required */
     CoreSurfaceBufferLock          leftBufferLock;     /* Remember the last surface buffer lock */
     CoreSurfaceBufferLock          rightBufferLock;    /* Remember the last surface buffer lock */
     DFBRectangle                   left_update;        /* Partial update region */
     DFBRectangle                   right_update;       /* Partial update region */
} bcmRegionData;

typedef struct {
     bcmRegionData                 *regionData;         /* Pointer to region data */
     DFBDisplayLayerID              layerId;            /* Graphics layer ID */
     DFBScreenID                    displayId;          /* Primary or secondary display/screen */
     DFBDisplayLayerSourceID        sourceId;           /* Graphics source Id */
     bool                           layerEnabled;       /* true if this layer is enabled */
     int                            level;              /* z-order of this layer (default is 0 for primary, +ve means above and -ve means below) */
     CoreSurface                   *frameBuffer;        /* Used when there is no vertical scaler in the GFD */
     pthread_cond_t                 frameBufferSem;     /* Framebuffer callback semaphore */
     pthread_mutex_t                frameBufferSemLock; /* Framebuffer callback protective mutex */
     DFBRectangle                   srcRectangle;       /* Source rectangle for the display/screen */
     DFBRectangle                   destRectangle;      /* Destination rectangle for the display/screen */
     DFB_PlatformBlendFactor        srcBlendFunc;       /* Source blend function */
     DFB_PlatformBlendFactor        destBlendFunc;      /* Destination blend function */
     int                            alpha;              /* FF = fully opaque, 00 = fully transparent */
     int                            constantAlpha;      /* FF = fully opaque, 00 = fully transparent */
     bool                           srcColorKeyOn;      /* true if source color keying is enabled */
     bool                           hasVscaler;         /* Set to true if graphics feeder has vertical scaler */
     bool                           ibuffer;            /* Set if intermediate buffer is defined in direcfbrc for scaling*/
     int                            updateSkipCnt;      /* Set > 0, mean how many times we can skip updating this layer */
     pthread_mutex_t                lock;               /* Lock access to this structure */
     void                         (*layerCallback)(void *context, int param);
#if FUSION_BUILD_MULTI
     DirectThread                  *frameBufferThread;  /* Framebuffer thread */
     pthread_mutex_t                frameBufferThreadLock;
     pthread_cond_t                 frameBufferThreadSem;
     DFBResult                      frameBufferThreadResult;
     bool                           frameBufferThreadQuit;
#endif
     bool                           follow_video;       /* Stereo offset comes from video metadata. */
     int                            z;                  /* Stereo depth. */
     CoreSurfaceBuffer             *flipPendingLeft;
     CoreSurfaceBuffer             *flipPendingRight;
} bcmLayerData;

typedef struct {
    DFBDisplayLayerID   layerId;
    NEXUS_SurfaceHandle hLeftSurface;
    NEXUS_SurfaceHandle hRightSurface;
} bcmLayerCallSetGfxFrameBuffer;

typedef struct {
    DFBDisplayLayerID      layerId;
    NEXUS_GraphicsSettings gfxSettings;
} bcmLayerCallSetGfxSettings;

/**********************************************************************************************************************/

/* Layers */

extern DFBResult bcmSetLayerState(DFBBCMNEXUS *dfb_bcmnexus, DFBDisplayLayerID layerId, bool enable);
extern DFBResult bcmSetLayerDestRectangle(DFBBCMNEXUS *dfb_bcmnexus, DFBDisplayLayerID layerId, DFBRectangle *destRectangle);
extern DisplayLayerFuncs bcmnexusPrimaryGfxLayerFuncs;
extern DisplayLayerFuncs bcmnexusSecondaryGfxLayerFuncs;
extern DisplayLayerFuncs bcmnexusBackgroundGfxLayerFuncs;

bcmLayerData *bcmGetLayerData(DFBDisplayLayerID layerId);

#endif
