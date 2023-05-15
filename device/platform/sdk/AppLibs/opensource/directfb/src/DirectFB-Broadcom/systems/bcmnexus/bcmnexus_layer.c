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
 * $brcm_Workfile: bcmnexus_layer.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/25 $
 * $brcm_Date: 3/7/12 5:26p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus_layer.c $
 *
 * DirectFB_1_4_15_Port/25   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/24   2/29/12 3:37p apandav
 * SW7552-208: DirectFB: CVBS can't output 576i due to DFB identify
 *  vertical scaling incorrectly on 7552 platform.
 *
 * DirectFB_1_4_15_Port/23   2/23/12 1:32p apandav
 * SW7425-2463: DirectFB: Optimize bcmSetRegion() code in bcmnexus_layer.c
 *
 * DirectFB_1_4_15_Port/22   2/13/12 3:46p mnelis
 * SW7552-208: DirectFB: CVBS can't output 576i due to DFB identify
 *  vertical scaling incorrectly on 7552 platform.
 *
 * DirectFB_1_4_15_Port/21   2/9/12 2:33p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB_1_4_15_Port/20   1/12/12 7:30p apandav
 * SW7125-1207: DirectFB: Support an option to have 960x1080 framebuffer
 *  to display in 1080p mode
 *
 * DirectFB_1_4_15_Port/19   1/9/12 2:24p apandav
 * SW7125-1207: DirectFB: Support an option to have 960x1080 framebuffer
 *  to display in 1080p mode
 *
 * DirectFB_1_4_15_Port/18   12/19/11 5:04p robertwm
 * SW7425-2002: DirectFB: Moving cursor when running windowed app on top
 *  of df_window exposes lower app (flipping).
 * SW7420-1515: [DirectFB] Got black bar when moving mouse in running
 *  df_andi + df_window (DFB Multi-App + SaWMan mode + Kernel Mode)
 *
 * DirectFB_1_4_15_Port/17   12/12/11 5:01p robertwm
 * SW7420-2158: DirectFB: Consider setting pthread mutex robust attribute.
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   12/8/11 3:34p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/16   11/30/11 7:49p scchang
 * SW7420-2158: DirectFB: Consider setting pthread mutex robust attribute
 *
 * DirectFB_1_4_15_Port/15   11/25/11 2:33p christ
 * SW7420-2154: DirectFB: Allow dynamic z offset changes for layers using
 *  blit composited 3D stereoscopic code.
 *
 * DirectFB_1_4_15_Port/14   11/11/11 2:50p christ
 * SW7425-1737: DirectFB: Vertical scaler not initialised correctly
 *
 * DirectFB_1_4_15_Port/13   11/8/11 3:38p christ
 * SW7425-1703: DirectFB: Move NEXUS_Display calls into platform layer
 *
 * DirectFB_1_4_15_Port/10   10/20/11 7:08p christ
 * SW7425-1563: DirectFB: Insignia test failures - Fill Rectangles
 *
 * DirectFB_1_4_15_Port/9   10/14/11 1:56p christ
 * SW7425-1493: DirectFB: Cycling through stereoscopic display modes
 *  causes VDC error
 *
 * DirectFB_1_4_15_Port/8   10/10/11 5:02p christ
 * SW7420-2089: DirectFB: df_andi3d fails to run on 7420
 *
 * DirectFB_1_4_15_Port/7   10/7/11 12:31p robertwm
 * SW7420-2083: DirectFB: Mirroring from HD to SD output results in wrong
 *  scaling.
 * SW7405-5504: DirectFB: Running df_andi with HD to SD mirroring enabled
 *  causes assertion and crash.
 *
 * DirectFB_1_4_15_Port/6   10/5/11 12:18p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_14_Port/5   9/23/11 12:48p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 * DirectFB_1_4_14_Port/4   9/13/11 12:24p robertwm
 * SW7420-2049: DirectFB: Source ID of each layer should default to
 *  independent values.
 * SW7405-5474: deadlock between frameBufferThread() and
 *  bcmSetLayerFrameBuffer() in bcmnexus_layer.c
 * SW7405-5486: DirectFB appears to hang in DFBCreate() in Inaris.
 * SW7420-2053: DirectFB-1.4.5: Hang in ShutdownLayer() function after
 *  <CTRL><C> a DFB application.
 *
 * DirectFB_1_4_14_Port/3   8/19/11 5:04p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_14_Port/2   8/12/11 4:44p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/13   9/12/11 1:03p robertwm
 * SW7405-5474: deadlock between frameBufferThread() and
 *  bcmSetLayerFrameBuffer() in bcmnexus_layer.c
 * SW7405-5486: DirectFB appears to hang in DFBCreate() in Inaris.
 * SW7420-2053: DirectFB-1.4.5: Hang in ShutdownLayer() function after
 *  <CTRL><C> a DFB application.
 *
 * DirectFB_1_4_5_Port/12   9/12/11 12:44p robertwm
 * SW7420-2049: DirectFB: Source ID of each layer should default to
 *  independent values.
 *
 * DirectFB_1_4_5_Port/11   8/18/11 4:29p christ
 * SW7425-1126: DirectFB: Stereoscopic layer Z offset not working on
 *  latest Nexus release.
 *
 * DirectFB_1_4_5_Port/10   8/17/11 11:43a tross
 * Correct source rectangle used when vscaling to correct
 * behavior of IDirectFBDisplayLayer::SetSourceRectangle.
 *
 * DirectFB_1_4_5_Port/9   8/17/11 2:13p christ
 * SW7425-1126: DirectFB: Stereoscopic layer Z offset not working on
 *  latest Nexus release.
 *
 * DirectFB_1_4_5_Port/8   7/21/11 9:42a tross
 * SWDTV-8038: Since DTV chips drive the panel directly (not via HDMI,
 * the framing mode is irrelevant other than to detect 2D vs. 3D.
 *
 * DirectFB_1_4_5_Port/7   7/15/11 12:55p robertwm
 * SW7231-300: DirectFB-1.4.5: Add surface hints to core DFB code.
 *
 * DirectFB_1_4_5_Port/6   7/12/11 5:24p christ
 * SW7425-888: DirectFB: Add src and dest rectangles to dfb_gfx_stretch_to
 *
 * DirectFB_1_4_5_Port/5   7/7/11 6:45p tross
 * SWBLURAY-26478: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/5   7/1/11 2:04p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/5   7/1/11 1:33p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/4   7/1/11 1:32p robertwm
 * SW7425-814: DirectFB: Cannot build DirectFB-1.4.5 Phase 2.0 candidate
 *  with Phase 2.0 refsw.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/3   6/29/11 3:33p robertwm
 * SW7420-1964: Memory allocation failure from DirectFB (df_andi).
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/2   6/16/11 5:11p robertwm
 * SW7405-5363: DirectFB-1.4.1: Cannot build system driver due to missing
 *  Nexus 3DTV functions and structures.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/1   6/13/11 12:54p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/11   5/27/11 5:11p tross
 * Updates to support latest version of stereoscopic API.
 *
 *
 * DirectFB_1_4_Port/52   5/18/11 4:22p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/51   5/17/11 6:51p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/10   5/6/11 9:47a tross
 * Only call SetFrameBuffer if the layer is enabled to prevent access to
 *  NULL
 * regionData.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/9   4/29/11 1:46p tross
 * Retrieve current display settings before each use to keep
 * synchroinized with externasl changes to display.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/8   4/11/11 7:06p tross
 * Merge changes to get STB to compile.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/7   4/11/11 5:25p tross
 * Set the default colorspace for a layer as it is initialized.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/6   4/6/11 3:41p tross
 * Ensure all flags are checked each time bcmSetRegion is called.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/5   3/28/11 11:03a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/52   5/18/11 4:22p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/51   5/17/11 6:51p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/49   3/17/11 1:15p robertwm
 * SW7420-1683: No display output on sawman multi-app with the latest
 *  direct1.4.5-1.5 in glib.
 *
 * DirectFB_1_4_Port/48   3/17/11 12:40p robertwm
 * SW7422-359: DirectFB: Add support for A8 Palette8 (DSPF_ALUT8) support
 *  into DirectFB.
 *
 * DirectFB_1_4_Port/47   3/4/11 5:03p robertwm
 * SW7420-1593: DirectFB-1.4.1: Add ShutdownLayer() API.
 *
 * DirectFB_1_4_Port/46   3/4/11 3:30p robertwm
 * SW7420-1590: DirectFB-1.4.5: Deadlock when closing down multiple
 *  applications.
 *
 * DirectFB_1_4_Port/45   3/1/11 12:01p christ
 * SW7420-1545: DirectFB: Application hang when using multi app DirectFB
 *  with SawMan window manager.
 *
 * DirectFB_1_4_Port/44   2/18/11 11:28a robertwm
 * SW7422-136: Green tint noticed on running any DFB app with pixel format
 *  not containing an alpha component.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/4   3/22/11 2:21p tross
 * Remove unused background region functions.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   3/11/11 3:39p tross
 * Bringup of Bluray background layer.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/6/11 3:59p tross
 * Check for NULL in calls to bcmSetLayerFrameBuffer.
 * Send locks, not handles to bcmFlipRegion.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:14p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/43   2/9/11 4:25p robertwm
 * SW7422-252: DirectFB: Add ABGR pixelformat support to DirectFB.
 *
 * DirectFB_1_4_Port/42   2/8/11 1:31p robertwm
 * SW7420-1471: DirectFB: Replace fusion skirmishes with pthread
 *  semantics.
 *
 * DirectFB_1_4_Port/41   2/8/11 1:12p robertwm
 * SW7420-1467: DirectFB: Multiple apps should NOT share NEXUS Display
 *  handle.
 *
 * DirectFB_1_4_Port/40   1/26/11 1:48p robertwm
 * SW7420-1430: DirectFB: Cannot compile DFB with 7420 Phase 4.1 refsw.
 *
 * DirectFB_1_4_Port/39   1/24/11 8:54p christ
 * SW7420-1411: DirectFB: Merge of features required for DirectFB 1.4.5
 *  phase 1.5 release - Add some encoder support
 *
 * DirectFB_1_4_Port/38   1/24/11 4:32p christ
 *
 * SW7420-1411: DirectFB: Merge of features required for DirectFB 1.4.5
 *  phase 1.5 release - Add some encoder support
 *
 * DirectFB_1_4_Port/37   1/24/11 3:28p robertwm
 * SW7420-1410: DirectFB: Random hangs using SaWMan-1.4.1 after tearing
 *  patches applied.
 *
 * DirectFB_1_4_Port/36   1/6/11 5:25p raywu
 * SW7420-1360: DirectFB: Tearing issue for SaWMan flipping
 *
 * DirectFB_1_4_Port/35   1/3/11 4:39p scchang
 * SW7420-1338: Add A8 and other pixel formats support for FillTrapezoid()
 *  for M2MC core
 *
 * DirectFB_1_4_Port/34   12/27/10 2:43p scchang
 * SW7550-527: Add dual output support on 7550
 *
 * DirectFB_1_4_Port/33   12/16/10 6:45p robertwm
 * SW35230-2598: DirectFB: Remove warning messages from bcmnexus_layer.c
 *  due to secondary layer not being present.
 *
 * DirectFB_1_4_Port/32   12/16/10 6:35p robertwm
 * SW35230-2595: DirectFB: Change default GFD filter co-efficient from
 *  Anisotropic to Bilinear.
 *
 * DirectFB_1_4_Port/31   12/14/10 1:18p robertwm
 * SW7422-140: DirectFB: Support OpenGL ES 2.0 applications on the VC-4
 *  core.
 *
 * DirectFB_1_4_Port/30   10/21/10 5:02a robertwm
 * SW7405-4958: DirectFB-1.4.5: Running df_window results in bcmTestRegion
 *  failures.
 *
 * DirectFB_1_4_Port/29   10/15/10 5:24p robertwm
 * SW7420-1180: DirectFB-1.4.5: Add Layer Shutdown code.
 *
 * DirectFB_1_4_Port/28   10/14/10 1:01p lig
 * SW7405-4939: sprintf corrupt stack and cause app crashes. change to use
 *  snprintf
 *
 * DirectFB_1_4_Port/27   10/12/10 2:54p robertwm
 * SW7550-589: DirectFB: Add LUT4 pixel format support to DirectFB.
 *
 * DirectFB_1_4_Port/26   9/23/10 4:11p robertwm
 * SW7405-4896: DirectFB: Kernel oops after running SaWMan "Penguins"
 *  twice in a row on DirectFB 1.4.0 release.
 * SW7405-4877: DirectFB: Optimise layer handling code when setting the
 *  graphics framebuffer.
 * SW7405-4897: DirectFB: Relaunching testman causes kernel Oops.
 * SW7405-4898: DirectFB: Poor performance when running multiple instances
 *  of df_andi in mirror mode.
 *
 * DirectFB_1_4_Port/25   9/2/10 2:46p robertwm
 * SW7405-4841: DirectFB: SetPowerMode() causes DFB to hang.
 *
 * DirectFB_1_4_Port/24   8/17/10 1:21a robertwm
 * SW7405-4764: DirectFB: Color keying on the primary layer doesn't work.
 *
 * DirectFB_1_4_Port/23   8/16/10 11:19p robertwm
 * SW7405-4758: DirectFB: Seg fault when running dual output apps with the
 *  "force-windowed,autoflip-window" options.
 *
 * DirectFB_1_4_Port/22   8/11/10 2:00a robertwm
 * SW7405-4250: DirectFB: Screen destination rectangle and source
 *  rectangle not saved across contexts.
 * SW7405-4743: DirectFB: Multiple instances of df_andi.c do not get
 *  displayed.
 *
 * DirectFB_1_4_Port/21   7/26/10 5:32p jackf
 * SW35230-771: Added include for shim_nexus.h.
 *
 * DirectFB_1_4_Port/20   6/7/10 4:45p robertwm
 * SW7405-4464: DirectFB: Display blending equations are wrong!
 *
 * DirectFB_1_4_Port/19   5/27/10 9:20p robertwm
 * SW7420-784: DirectFB: SetPalette on a non-layer surface doesn't work.
 *
 * DirectFB_1_4_Port/18   4/19/10 4:46p robertwm
 * SW3556-1100: DirectFB: Remove warnings generated when NUM_DISPLAYS is
 *  1.
 *
 * DirectFB_1_4_Port/17   4/15/10 10:39p robertwm
 * SW7405-4184: Allow independent mode and resolution to be specified.
 *
 * DirectFB_1_4_Port/16   4/15/10 7:51p robertwm
 * SW7405-4184: Allow independent mode and resolution to be specified.
 *
 ***************************************************************************/

#include "bcmnexus.h"
#include "bcmnexus_screen.h"
#include "bcmnexus_layer.h"
#include "bcmnexus_utils.h"
#include "bcmnexus_pool.h"
#include "nexus_core_utils.h"
#include "bdbg.h"

#include <directfb_version.h>

#include <core/core.h>
#include <core/system.h>
#include <core/palette.h>
#include <core/layers_internal.h>
#include <core/layer_control.h>
#include <core/screens_internal.h>
#include <core/screens.h>
#include <fusion/shmalloc.h>
#include <misc/conf.h>
#include <gfx/util.h>

#ifdef __GNUC__
#define UNUSED_ __attribute__ ((unused))
#else
#define UNUSED_
#endif

D_DEBUG_DOMAIN( bcmnexusLayer,  "bcmNexus/Layer",  "Broadcom NEXUS Layer driver" );
D_DEBUG_DOMAIN( bcmnexusLayerX, "bcmNexus/LayerX", "Broadcom NEXUS Layer driver (eXtra debug)" );

#define BCM_DFB_USE_SURFACE_SET_STEREO_EYE ((DIRECTFB_MAJOR_VERSION * 1000000 + \
                                             DIRECTFB_MINOR_VERSION * 1000 + \
                                             DIRECTFB_MICRO_VERSION) < 1006000)

#define BCM_DFB_SURFACE_CREATE_WITH_HINT   ((DIRECTFB_MAJOR_VERSION * 1000000 + \
                                             DIRECTFB_MINOR_VERSION * 1000 + \
                                             DIRECTFB_MICRO_VERSION) < 1006000)


/**********************************************************************************************************************/

bcmLayerData *
bcmGetLayerData(DFBDisplayLayerID layerId)
{
    CoreLayer *layer;

    layer = dfb_layer_at(layerId);

    if (layer)
        return layer->layer_data;
    else
        return NULL;
}

static void
frameBufferCallback( void *context, int param )
{
     bcmLayerData *layerData = context;
     pthread_mutex_t *pLock;
     pthread_cond_t  *pSem;
     CoreSurface       *frameBuffer      = layerData->frameBuffer;
     CoreSurfaceBuffer *flipPendingLeft  = layerData->flipPendingLeft;
     CoreSurfaceBuffer *flipPendingRight = layerData->flipPendingRight;

     D_DEBUG_AT( bcmnexusLayerX, "  -> screen %d gfx layer %d framebuffer callback handled\n", layerData->displayId, param);

     pLock = &layerData->frameBufferSemLock;
     pSem  = &layerData->frameBufferSem;
     PTHREAD_ROBUST_MUTEX_LOCK( pLock );
     layerData->flipPendingLeft  = NULL;
     layerData->flipPendingRight = NULL;
     pthread_cond_broadcast( pSem );
     pthread_mutex_unlock( pLock );

     /* Inform DirectFB that the buffer has been displayed */
     if (flipPendingLeft)
         dfb_surface_notify_display( frameBuffer, flipPendingLeft );

     if (flipPendingRight)
         dfb_surface_notify_display( frameBuffer, flipPendingRight );
}

#if FUSION_BUILD_MULTI
static void *
frameBufferThread( UNUSED_ DirectThread *thread, void *arg )
{
    DFBBCMNEXUS                                *dfb_bcmnexus;
    bcmLayerData                               *layerData;
    NEXUS_DisplayHandle                         hDisplay;
    DFB_Platform_P_DisplayGraphicsFramebuffer3D frameBuffer3D;
    NEXUS_SurfaceHandle  hLeftSurface, hRightSurface;
    DFB_PlatformRect left_rect, right_rect;
    bool single_buffered = false;

    layerData    = arg;
    dfb_bcmnexus = dfb_system_data();

    D_DEBUG_AT( bcmnexusLayer, "%s: screen %d gfx layer %d: Starting...\n", __FUNCTION__, layerData->displayId, layerData->layerId );

    /* Signal to thread which spawned us that we're started */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->frameBufferThreadLock );
    pthread_cond_broadcast( &layerData->frameBufferThreadSem );

    while (!layerData->frameBufferThreadQuit) {
        /* Wait for someone to signal that they want the framebuffer set... */
        pthread_cond_wait( &layerData->frameBufferThreadSem, &layerData->frameBufferThreadLock );

        if (layerData->layerEnabled)
        {
            CoreScreen *screen = dfb_screens_at_translated(layerData->displayId);
            bcmScreenData* screenData = (bcmScreenData*)(screen->screen_data);

            hLeftSurface = layerData->regionData->leftBufferLock.handle;
            hRightSurface = layerData->regionData->rightBufferLock.handle;
            D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Settings Gfx Framebuffer handle=%p...\n",
                        __FUNCTION__, layerData->displayId, layerData->layerId, (void *)hLeftSurface );

            hDisplay = GET_DISPLAY_HND(dfb_bcmnexus, layerData->displayId);
            DFB_Platform_P_GetDefaultDisplayGraphicsFramebuffer3D(&frameBuffer3D);

            frameBuffer3D.main = hLeftSurface;

            if (layerData->regionData->config.options & DLOP_STEREO)
            {
                frameBuffer3D.right = hRightSurface;

                switch (screenData->encoderCfg.framing)
                {
                    case DSEPF_STEREO_SIDE_BY_SIDE_HALF:
                        frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e3D_LeftRight;
                        D_DEBUG_AT(bcmnexusLayer,"Using Left/Right packed stereoscopic graphics buffers\n");
                        break;
                    case DSEPF_STEREO_SIDE_BY_SIDE_FULL:
                    case DSEPF_STEREO_TOP_AND_BOTTOM:
                    case DSEPF_STEREO_FRAME_PACKING:
                        frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e3D_OverUnder;
                        D_DEBUG_AT(bcmnexusLayer,"Using Over/Under packed stereoscopic graphics buffers\n");
                        break;
                    case DSEPF_MONO:
                    default:
                        frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e2D;
                        D_DEBUG_AT(bcmnexusLayer,"Using single graphics buffer\n");
                        break;
                }
            }
            else
            {
                frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e2D;
                frameBuffer3D.right = NULL;
            }

#if (!BCMNEXUS_VDC_SUPPORT_3D_VIDEO || !BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT) || (defined(HUMAX_PLATFORM_BASE))	/* to support 3D surface */
            // If we don't have 3D hardware support we can simulate half res stereoscopic modes with a series
            // of blits. This doesn't provide the complete implementation.
            if (layerData->regionData->config.options & (DLOP_LR_MONO | DLOP_STEREO) )
            {
#if defined(HUMAX_PLATFORM_BASE)	/* to support 3D surface */
				switch (screenData->encoderCfg.framing)
				{
					case DSEPF_STEREO_SIDE_BY_SIDE_HALF:
						frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e3D_LeftRight;
						D_DEBUG_AT(bcmnexusLayer,"Using Left/Right packed stereoscopic graphics buffers\n");
						break;
					case DSEPF_STEREO_SIDE_BY_SIDE_FULL:
					case DSEPF_STEREO_TOP_AND_BOTTOM:
					case DSEPF_STEREO_FRAME_PACKING:
						frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e3D_OverUnder;
						D_DEBUG_AT(bcmnexusLayer,"Using Over/Under packed stereoscopic graphics buffers\n");
						break;
					case DSEPF_MONO:
					default:
						frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e2D;
						D_DEBUG_AT(bcmnexusLayer,"Using single graphics buffer\n");
						break;
				}
#endif
                layerData->flipPendingLeft  = layerData->regionData->leftBufferLock.buffer;
                layerData->flipPendingRight = layerData->regionData->rightBufferLock.buffer;

                layerData->frameBufferThreadResult =
                DFB_Platform_P_BlitComposite3DFramebuffer(GET_GRAPHICS2D_HND(dfb_bcmnexus),
                                                              hDisplay,
                                                          &frameBuffer3D,
                                                          (layerData->regionData->config.options & DLOP_STEREO),
                                                          layerData->z);
            }
            else
#endif
            {
                layerData->flipPendingLeft  = layerData->regionData->leftBufferLock.buffer;
                layerData->flipPendingRight = layerData->regionData->rightBufferLock.buffer;

	            right_rect.height = layerData->regionData->right_update.h;
	            right_rect.width = layerData->regionData->right_update.w;
	            right_rect.x = layerData->regionData->right_update.x;
	            right_rect.y = layerData->regionData->right_update.y;


	            left_rect.height = layerData->regionData->left_update.h;
	            left_rect.width = layerData->regionData->left_update.w;
	            left_rect.x = layerData->regionData->left_update.x;
	            left_rect.y = layerData->regionData->left_update.y;

	            if (layerData->regionData->config.buffermode == DLBM_FRONTONLY)
	            {
	                D_DEBUG_AT( bcmnexusLayer, "%s() Single buffered layer : 0x%08x\n", __FUNCTION__, layerData->regionData->config.buffermode );
	                single_buffered = true;
	            }
	            else
	                single_buffered = false;

#if defined(HUMAX_PLATFORM_BASE)	/* to support 3D surface */
				switch (screenData->encoderCfg.framing)
				{
					case DSEPF_STEREO_SIDE_BY_SIDE_HALF:
						frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e3D_LeftRight;
						D_DEBUG_AT(bcmnexusLayer,"Using Left/Right packed stereoscopic graphics buffers\n");
						break;
					case DSEPF_STEREO_SIDE_BY_SIDE_FULL:
					case DSEPF_STEREO_TOP_AND_BOTTOM:
					case DSEPF_STEREO_FRAME_PACKING:
						frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e3D_OverUnder;
						D_DEBUG_AT(bcmnexusLayer,"Using Over/Under packed stereoscopic graphics buffers\n");
						break;
					case DSEPF_MONO:
					default:
						frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e2D;
						D_DEBUG_AT(bcmnexusLayer,"Using single graphics buffer\n");
						break;
				}
		
				if(frameBuffer3D.orientation != DFB_Platform_P_VideoOrientation_e2D)
				{
#if (!BCMNEXUS_VDC_SUPPORT_3D_VIDEO || !BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT) || (defined(HUMAX_PLATFORM_BASE))	/* to support 3D surface */
					layerData->frameBufferThreadResult =
					DFB_Platform_P_BlitComposite3DFramebuffer(GET_GRAPHICS2D_HND(dfb_bcmnexus),
																  hDisplay,
															  &frameBuffer3D,
															  (layerData->regionData->config.options & DLOP_STEREO),
															  layerData->z);
#endif
					if(layerData->frameBufferThreadResult == DFB_PLATFORM_UNSUPPORTED)
					{
						if ((layerData->regionData->config.options & DLOP_STEREO) == 0)
						{
							frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e2D;
							frameBuffer3D.right = NULL;
						}
						layerData->frameBufferThreadResult =
								(DFB_Platform_P_SetDisplayGraphicsFramebuffer3D(hDisplay, &frameBuffer3D, &left_rect, &right_rect, single_buffered) == NEXUS_SUCCESS) ? DFB_OK : DFB_FAILURE; 					
					}
				}
				else
				{
					layerData->frameBufferThreadResult =
								   (DFB_Platform_P_SetDisplayGraphicsFramebuffer3D(hDisplay, &frameBuffer3D, &left_rect, &right_rect, single_buffered) == NEXUS_SUCCESS) ? DFB_OK : DFB_FAILURE;
				}
#else
	            layerData->frameBufferThreadResult =
	                (DFB_Platform_P_SetDisplayGraphicsFramebuffer3D(hDisplay, &frameBuffer3D, &left_rect, &right_rect, single_buffered) == NEXUS_SUCCESS) ? DFB_OK : DFB_FAILURE;
#endif
            }

        }
        /* Signal that the result of setting the framebuffer is now ready to be read. */
        pthread_cond_broadcast( &layerData->frameBufferThreadSem );
    }
    pthread_mutex_unlock( &layerData->frameBufferThreadLock );

    D_DEBUG_AT( bcmnexusLayer, "%s: screen %d gfx layer %d: Exiting...\n", __FUNCTION__, layerData->displayId, layerData->layerId );

    return NULL;
}
#endif

static void
bcmGetClipRectangle(bcmLayerData *layerData, int dispWidth, int dispHeight, int defaultWidth, int defaultHeight, int* clipWidth, int* clipHeight)
{
    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;

    if(layerData->ibuffer)
    {
        long int ibufArea = brcm_config->ibuffer.width * brcm_config->ibuffer.height;
        long int dstArea = dispWidth * dispHeight;

        if ((layerData->srcRectangle.w > dispWidth ) || (layerData->srcRectangle.h > dispHeight) )
        {
            *clipWidth        = MIN(brcm_config->ibuffer.width, dispWidth);
            *clipHeight       = MIN(brcm_config->ibuffer.height, dispHeight);
        }
        else if((layerData->srcRectangle.w <= dispWidth) || (layerData->srcRectangle.h <= dispHeight) )
        {
            if(dstArea > ibufArea)
            {
                *clipWidth        = MIN(brcm_config->ibuffer.width, dispWidth);
                *clipHeight       = MIN(brcm_config->ibuffer.height, dispHeight);

            }
            else
            {
                *clipWidth        = defaultWidth;
                *clipHeight       = defaultHeight;
            }
        }
        else
        {
            *clipWidth  = defaultWidth;
            *clipHeight = defaultHeight;
        }
    }
    else
    {
        *clipWidth  = defaultWidth;
        *clipHeight = defaultHeight;
    }
}

static DFBResult
bcmGetLayerGraphicsSettings(DFBBCMNEXUS *dfb_bcmnexus, DFBDisplayLayerID layerId, DFB_Platform_P_GraphicsSettings *pGraphicsSettings)
{
    int                    source_height, source_width;
    int                    display_width, display_height;
    int                    canvas_width,  canvas_height;
    bcmLayerData          *layerData;
    CoreLayerRegionConfig *regionConfig;
    DFBScreenID            displayId;
    DFB_PlatformDimension  displaySize;
    int                    width;
    int                    height;

    layerData      = bcmGetLayerData(layerId);
    if (layerData == NULL)
        return DFB_FAILURE;

    regionConfig   = (CoreLayerRegionConfig *)&(layerData->regionData->config);
    displayId      = layerData->displayId;

    /* Get Canvas size */
    DFB_Platform_P_GetDisplaySize(displayId, &displaySize);

    canvas_width   = displaySize.w;
    canvas_height  = displaySize.h;

    /* Get Display Rectangle size */
    display_width  = MIN(layerData->destRectangle.w, canvas_width);
    display_height = MIN(layerData->destRectangle.h, canvas_height);

    /* Get Source Rectangle size */
    source_width   = layerData->srcRectangle.w;
    if (layerData->hasVscaler)
        source_height = layerData->srcRectangle.h;
    else
        source_height = display_height;

    DFB_Platform_P_GetGraphicsSettings(GET_DISPLAY_HND(dfb_bcmnexus, displayId), pGraphicsSettings);

    pGraphicsSettings->enabled           = layerData->layerEnabled;
    pGraphicsSettings->zorder            = layerData->level + BCMNEXUS_Z_ORDER_MAX/2;
    pGraphicsSettings->position.width    = display_width;
    pGraphicsSettings->position.height   = display_height;
    pGraphicsSettings->position.x        = (layerData->destRectangle.x + display_width  > canvas_width)  ? 0 : layerData->destRectangle.x;
    pGraphicsSettings->position.y        = (layerData->destRectangle.y + display_height > canvas_height) ? 0 : layerData->destRectangle.y;
    pGraphicsSettings->clip.x            = layerData->srcRectangle.x;
    pGraphicsSettings->clip.y            = layerData->srcRectangle.y;

    bcmGetClipRectangle(layerData, display_width, display_height, source_width, source_height, &width, &height);

    pGraphicsSettings->clip.width  = (unsigned short) width;
    pGraphicsSettings->clip.height = (unsigned short) height;


    pGraphicsSettings->lowerChromakey.a  = 0x00;
    pGraphicsSettings->lowerChromakey.r  = regionConfig->src_key.r;
    pGraphicsSettings->lowerChromakey.g  = regionConfig->src_key.g;
    pGraphicsSettings->lowerChromakey.b  = regionConfig->src_key.b;

    pGraphicsSettings->upperChromakey.a  = 0xff;
    pGraphicsSettings->upperChromakey.r  = regionConfig->src_key.r;
    pGraphicsSettings->upperChromakey.g  = regionConfig->src_key.g;
    pGraphicsSettings->upperChromakey.b  = regionConfig->src_key.b;

    pGraphicsSettings->chromakeyEnabled  = layerData->srcColorKeyOn;
    pGraphicsSettings->sourceBlendFactor = layerData->srcBlendFunc;
    pGraphicsSettings->destBlendFactor   = layerData->destBlendFunc;
    pGraphicsSettings->alpha             = layerData->alpha;
    pGraphicsSettings->constantAlpha     = layerData->constantAlpha;

    pGraphicsSettings->horizontalFilter  = BCMNEXUS_GFD_SCALE_FILTER_COEFF;

#if defined(BCMNEXUS_GFD0_HAS_VERTICAL_SCALER) || defined(BCMNEXUS_GFD1_HAS_VERTICAL_SCALER)
    pGraphicsSettings->verticalFilter    = BCMNEXUS_GFD_SCALE_FILTER_COEFF;
#endif

    if (layerData->layerEnabled) {
        pGraphicsSettings->frameBufferCallback.callback = layerData->layerCallback;
        pGraphicsSettings->frameBufferCallback.context  = layerData;
        pGraphicsSettings->frameBufferCallback.param    = layerId;
    } else {
        pGraphicsSettings->frameBufferCallback.callback = NULL;
    }

#if BCMNEXUS_VDC_SUPPORT_3D_VIDEO && BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
    CoreScreen *screen = dfb_screens_at_translated(layerData->displayId);
    bcmScreenData* screenData = (bcmScreenData*)(screen->screen_data);

    /* Modify display sizes for half res 3d modes */
    if (screenData->encoderCfg.framing == DSEPF_STEREO_SIDE_BY_SIDE_HALF)
#if defined(HUMAX_PLATFORM_BASE)	/* to support 3D surface */
	{
		pGraphicsSettings->position.width	 = display_width/2;
		pGraphicsSettings->clip.width = pGraphicsSettings->clip.width/2;
	}
#else
		pGraphicsSettings->position.width	 = display_width/2;
#endif


    if (screenData->encoderCfg.framing == DSEPF_STEREO_TOP_AND_BOTTOM)
        pGraphicsSettings->position.height    = display_height/2;

    /* Set Z offset if required */
    if ( regionConfig->options & (DLOP_LR_MONO | DLOP_STEREO) )
    {
        int scaledZ;

        if (screenData->encoderCfg.framing != DSEPF_MONO)
        {

            if (screenData->encoderCfg.framing == DSEPF_STEREO_SIDE_BY_SIDE_HALF)
                scaledZ = layerData->z * display_width / (int)(display_width/2);
            else
                scaledZ = layerData->z;

            pGraphicsSettings->stereoOffset = scaledZ;
            pGraphicsSettings->clip.width -= abs(scaledZ);
            pGraphicsSettings->position.width -= abs(scaledZ);
        }
        else
            pGraphicsSettings->stereoOffset = 0;
    }
    else
            pGraphicsSettings->stereoOffset = 0;

    /* This code is here to support the updating of the stereo depth in a l/r mono surface in
       the situation where we don't have 3D display hardware and are using the blit composited
       3D stereoscopic effects */
#if !BCMNEXUS_VDC_SUPPORT_3D_VIDEO || !BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
    if (regionConfig->options & (DLOP_LR_MONO | DLOP_STEREO) )
    {
        D_DEBUG_AT( bcmnexusLayer, "%s() push GFX updates to FB thread\n",__FUNCTION__);
        /* Indicate to our framebuffer thread that the framebuffer needs to be set... */
        PTHREAD_ROBUST_MUTEX_LOCK( &layerData->frameBufferThreadLock );
        pthread_cond_broadcast( &layerData->frameBufferThreadSem );
        /* Wait for the result... */
        pthread_cond_wait( &layerData->frameBufferThreadSem, &layerData->frameBufferThreadLock );
        pthread_mutex_unlock( &layerData->frameBufferThreadLock );
    }
#endif

#endif /* BCMNEXUS_VDC_SUPPORT_3D_VIDEO */

    return DFB_OK;
}

static DFBResult
bcmSetLayerFrameBuffer(UNUSED_ DFBBCMNEXUS *dfb_bcmnexus, DFBDisplayLayerID layerId)
{
    DFBResult            result = DFB_OK;
    bcmLayerData        *layerData;
    bool                 single_buffered = false;


    layerData = bcmGetLayerData(layerId);
    if (layerData == NULL) {
        result = DFB_FAILURE;
    } else {
        D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d (enabled=%d, master=%c)\n",
                    __FUNCTION__, layerData->displayId, layerId, layerData->layerEnabled, dfb_core_is_master( dfb_bcmnexus_core ) ? 'y' : 'n');

        if (layerData->regionData->config.buffermode == DLBM_FRONTONLY)
        {
            D_DEBUG_AT( bcmnexusLayer, "%s() Single buffered layer : 0x%08x\n", __FUNCTION__, layerData->regionData->config.buffermode );
            single_buffered = true;
        }

        if (layerData->layerEnabled)
        {
#if FUSION_BUILD_MULTI /*Use thread in Multi app to avoid deadlock.*/
            /* Indicate to our framebuffer thread that the framebuffer needs to be set... */
            PTHREAD_ROBUST_MUTEX_LOCK( &layerData->frameBufferThreadLock );
            pthread_cond_broadcast( &layerData->frameBufferThreadSem );

            /* Wait for the result... */
            if (pthread_cond_wait( &layerData->frameBufferThreadSem, &layerData->frameBufferThreadLock ) != 0)
            {
                pthread_mutex_unlock( &layerData->frameBufferThreadLock );
                return DFB_FAILURE;
            }

            result = layerData->frameBufferThreadResult;
            pthread_mutex_unlock( &layerData->frameBufferThreadLock );
#else
            DFB_Platform_P_DisplayGraphicsFramebuffer3D frameBuffer3D;
            NEXUS_DisplayHandle hDisplay;
            NEXUS_SurfaceHandle  hLeftSurface, hRightSurface;

            hLeftSurface = layerData->regionData->leftBufferLock.handle;
            hRightSurface = layerData->regionData->rightBufferLock.handle;


            CoreScreen *screen = dfb_screens_at_translated(layerData->displayId);
            bcmScreenData* screenData = (bcmScreenData*)(screen->screen_data);

            hDisplay = GET_DISPLAY_HND(dfb_bcmnexus, layerData->displayId);
            DFB_Platform_P_GetDefaultDisplayGraphicsFramebuffer3D(&frameBuffer3D);

            frameBuffer3D.main = hLeftSurface;

            if (layerData->regionData->config.options & DLOP_STEREO)
            {

                frameBuffer3D.right = hRightSurface;

                switch (screenData->encoderCfg.framing)
                {

                    case DSEPF_STEREO_SIDE_BY_SIDE_HALF:
                        frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e3D_LeftRight;
                        D_DEBUG_AT(bcmnexusLayer,"Using Left/Right packed stereoscopic graphics buffers\n");
                        break;
                    case DSEPF_STEREO_SIDE_BY_SIDE_FULL:
                    case DSEPF_STEREO_TOP_AND_BOTTOM:
                    case DSEPF_STEREO_FRAME_PACKING:
                        frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e3D_OverUnder;
                        D_DEBUG_AT(bcmnexusLayer,"Using Over/Under packed stereoscopic graphics buffers\n");
                        break;
                    case DSEPF_MONO:
                    default:
                        frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e2D;
                        D_DEBUG_AT(bcmnexusLayer,"Using single graphics buffer\n");
                        break;
                }

            }
            else
            {
                frameBuffer3D.orientation = DFB_Platform_P_VideoOrientation_e2D;
                frameBuffer3D.right = NULL;
            }

#if !BCMNEXUS_VDC_SUPPORT_3D_VIDEO || !BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
            // If we don't have 3D hardware support we can simulate half res stereoscopic modes with a series
            // of blits. This doesn't provide the complete implementation.
            if (layerData->regionData->config.options & (DLOP_LR_MONO | DLOP_STEREO) )
                DFB_Platform_P_BlitComposite3DFramebuffer(GET_GRAPHICS2D_HND(dfb_bcmnexus),
                                                          hDisplay,
                                                          &frameBuffer3D,
                                                          (layerData->regionData->config.options & DLOP_STEREO),
                                                          layerData->z);
#endif

            DFB_PlatformRect left_rect, right_rect;

            right_rect.height = layerData->regionData->right_update.h;
            right_rect.width  = layerData->regionData->right_update.w;
            right_rect.x      = layerData->regionData->right_update.x;
            right_rect.y      = layerData->regionData->right_update.y;

            left_rect.height = layerData->regionData->left_update.h;
            left_rect.width  = layerData->regionData->left_update.w;
            left_rect.x      = layerData->regionData->left_update.x;
            left_rect.y      = layerData->regionData->left_update.y;

            result = (DFB_Platform_P_SetDisplayGraphicsFramebuffer3D(hDisplay, &frameBuffer3D,
                                                                     (left_rect.height  > 0 ? &left_rect  : NULL),
                                                                     (right_rect.height > 0 ? &right_rect : NULL),
                                                                     single_buffered) == DFB_PLATFORM_OK) ? DFB_OK : DFB_FAILURE;
#endif // framebuffer thread
        }
    }

    return result;
}

static DFBResult
bcmSetLayerGraphicsSettings(DFBBCMNEXUS *dfb_bcmnexus, DFBDisplayLayerID layerId)
{
    DFB_Platform_P_GraphicsSettings graphicsSettings;
    bcmLayerData          *layerData;
    NEXUS_DisplayHandle    hDisplay;
    DFBResult              result = DFB_OK;

    layerData = bcmGetLayerData(layerId);
    if (layerData == NULL) {
        result = DFB_FAILURE;
    } else {
        D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d (master=%c)\n",
                __FUNCTION__, layerData->displayId, layerId, dfb_core_is_master( dfb_bcmnexus_core ) ? 'y' : 'n');

        bcmGetLayerGraphicsSettings(dfb_bcmnexus, layerId, &graphicsSettings);

        D_DEBUG_AT( bcmnexusLayer, "-> position (%d, %d, %d x %d), clip (%d, %d, %d x %d), opacity %d\n",
                    graphicsSettings.position.x, graphicsSettings.position.y,
                    graphicsSettings.position.width, graphicsSettings.position.height,
                    graphicsSettings.clip.x, graphicsSettings.clip.y,
                    graphicsSettings.clip.width, graphicsSettings.clip.height,
                    graphicsSettings.alpha );

        hDisplay = GET_DISPLAY_HND(dfb_bcmnexus, layerData->displayId);
        result = (DFB_Platform_P_SetGraphicsSettings(hDisplay, &graphicsSettings) == NEXUS_SUCCESS) ? DFB_OK : DFB_FAILURE;

    }
    return result;
}

DFBResult
bcmSetLayerState(DFBBCMNEXUS *dfb_bcmnexus, DFBDisplayLayerID layerId, bool enable)
{
    DFBResult               result;
    bcmLayerData           *layerData;
    DFBScreenID             displayId = 0xff;

    layerData = bcmGetLayerData(layerId);

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: enable=%d\n", __FUNCTION__, displayId, layerId, enable);

    if (layerData == NULL) {
        result = DFB_FAILURE;
    } else {
        /* Lock access to the layerData */
        PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

        displayId = layerData->displayId;

        /* Get whether the screen/display is on */
        layerData->layerEnabled = (bcmGetPowerMode(dfb_screens_at(displayId)) == DSPM_ON) && enable;

        result = bcmSetLayerGraphicsSettings(dfb_bcmnexus, layerId);

        if (result == DFB_OK)
            result = bcmSetLayerFrameBuffer(dfb_bcmnexus, layerId);

        /* Unlock access to the layerData */
        pthread_mutex_unlock( &layerData->lock );
    }
    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Exit [%d].\n", __FUNCTION__, displayId, layerId, result );
    return result;
}

DFBResult
bcmSetLayerDestRectangle(DFBBCMNEXUS *dfb_bcmnexus, DFBDisplayLayerID layerId, DFBRectangle *destRectangle)
{
    DFBResult               result;
    DFBScreenID             displayId = 0xff;
    bcmLayerData           *layerData;
    bcmRegionData          *regionData;
    DFB_Platform_P_GraphicsSettings  graphicsSettings;

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: rect=%dx%d@%d,%d\n", __FUNCTION__,
                    displayId, layerId, destRectangle->w, destRectangle->h, destRectangle->x, destRectangle->y);

    layerData = bcmGetLayerData(layerId);

    if (layerData == NULL) {
        result = DFB_FAILURE;
    } else {
        /* Lock access to the layerData */
        PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

        displayId = layerData->layerId;

        /* Save new destination rectangle settings */
        layerData->destRectangle = *destRectangle;

        bcmGetLayerGraphicsSettings(dfb_bcmnexus, layerId, &graphicsSettings);

        regionData = layerData->regionData;
        regionData->vscale = false;

        if (graphicsSettings.clip.height != layerData->srcRectangle.h)
        {
            regionData->vscale = true;
        }

        result = bcmSetLayerGraphicsSettings(dfb_bcmnexus, layerId);

        if (result == DFB_OK)
            result = bcmSetLayerFrameBuffer(dfb_bcmnexus, layerId);

        /* Unlock access to the layerData */
        pthread_mutex_unlock( &layerData->lock );
    }
    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Exit [%d].\n", __FUNCTION__, displayId, layerId, result );
    return result;
}

static void
bcmGetLayerDefaultSize(DFBDisplayLayerID layerId, int *pWidth, int *pHeight)
{
    int                   width, height;
    DFBScreenID           displayId;
    DFB_PlatformDimension displaySize;

    displayId = dfb_screen_id_translated(dfb_layer_screen(dfb_layer_at_translated(layerId)));

    /*Secondary layer frame buffer size need to be equal to maximum secondary disp format size. Secondary GFD does not have V-scaler
       IMS ID 528599 */
    if(displayId == SECONDARY_DISPLAY_ID)
    {
        DFB_PlatformDimension dispFormatSize;

        DFB_Platform_P_DFBFormatToDisplaySize(DFB_PlatformVideoFormat_ePalG, &dispFormatSize);
        width  = dispFormatSize.w;
        height = dispFormatSize.h;
    }
    else
    {
        DFB_Platform_P_GetDisplaySize(displayId, &displaySize);
        width  = displaySize.w;
        height = displaySize.h;
    }

    if (dfb_config->layers[layerId].config.flags & DLCONF_WIDTH)
        width = dfb_config->layers[layerId].config.width;

    if (dfb_config->layers[layerId].config.flags & DLCONF_HEIGHT)
        height = dfb_config->layers[layerId].config.height;

    *pWidth = width;
    *pHeight = height;

}

static void
bcmGetLayerDefaultFormat(DFBDisplayLayerID layerId, DFBSurfacePixelFormat *pFormat)
{
    DFBSurfacePixelFormat pixelFormat = DSPF_ARGB;

    if (dfb_config->layers[layerId].config.flags & DLCONF_PIXELFORMAT)
        pixelFormat = dfb_config->layers[layerId].config.pixelformat;

    *pFormat = pixelFormat;
}

static void
bcmGetLayerDefaultBufferMode(DFBDisplayLayerID layerId, DFBDisplayLayerBufferMode *pMode)
{
    DFBDisplayLayerBufferMode bufferMode = DLBM_FRONTONLY;

    if (dfb_config->layers[layerId].config.flags & DLCONF_BUFFERMODE)
        bufferMode = dfb_config->layers[layerId].config.buffermode;

    *pMode = bufferMode;
}

static void
bcmGetLayerDefaultOptions(DFBDisplayLayerID layerId, DFBDisplayLayerOptions *pOptions)
{
    DFBDisplayLayerOptions options = DLOP_OPACITY | DLOP_ALPHACHANNEL;

    if (dfb_config->layers[layerId].config.flags & DLCONF_OPTIONS)
        options = dfb_config->layers[layerId].config.options;

    *pOptions = options;
}

static void
bcmGetLayerDefaultSurfaceCaps(UNUSED_ DFBDisplayLayerID layerId, DFBSurfaceCapabilities *pCaps)
{
    DFBSurfaceCapabilities caps = DSCAPS_PRIMARY | DSCAPS_VIDEOONLY | DSCAPS_PREMULTIPLIED;

    *pCaps = caps;
}

static void
bcmGetLayerDefaultLevel(UNUSED_ DFBDisplayLayerID layerId, int *pLevel)
{
    *pLevel = 0;
}

static int
bcmGetLayerDefaultUpdateSkipCnt(DFBDisplayLayerID layerId)
{
#if ((DIRECTFB_MAJOR_VERSION * 1000000 + \
      DIRECTFB_MINOR_VERSION * 1000 + \
      DIRECTFB_MICRO_VERSION) >= 1006000)
    return direct_config_get_int_value( "layer-update-skip" );
#else

    return dfb_config->layers[layerId].update_skip_cnt;

#endif

}

static int bcmLayerDataSize( void )
{
    return sizeof(bcmLayerData);
}

static int bcmRegionDataSize( void )
{
    return sizeof(bcmRegionData);
}

/********************************************
 * Common functions to setup layers/regions *
 ********************************************/

/* Common function to help initialize the layer data. */
static DFBResult
bcmInitLayer( DFBBCMNEXUS                *dfb_bcmnexus,
                          bcmLayerData               *layerData,
                          DFBDisplayLayerDescription *description,
                          DFBDisplayLayerConfig      *config)
{
    DFBScreenID         displayId;
    DFBDisplayLayerID   layerId;
    pthread_mutexattr_t mutexAttr;
    pthread_condattr_t  condAttr;
#if FUSION_BUILD_MULTI
    char                thread_name[24];
#endif

    layerId   = layerData->layerId;
    displayId = layerData->displayId;

    D_DEBUG_AT( bcmnexusLayer, "%s() %s display graphics layer %d\n", __FUNCTION__,
                (displayId == PRIMARY_DISPLAY_ID) ? "primary" : "secondary", layerId) ;

    /* Save the graphics layer ID for the display */
    SET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, layerId, GET_DISPLAY_NUM_GFX_LAYERS(dfb_bcmnexus, displayId)++, displayId);

    /* semaphore init */
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_init(&condAttr);
    pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&layerData->lock, &mutexAttr);
    pthread_mutex_init(&layerData->frameBufferSemLock, &mutexAttr);
    pthread_cond_init(&layerData->frameBufferSem, &condAttr);

#if FUSION_BUILD_MULTI
    /* Thread init */
    pthread_mutex_init( &layerData->frameBufferThreadLock, &mutexAttr );
    pthread_cond_init( &layerData->frameBufferThreadSem, &condAttr );
#endif

    pthread_mutexattr_destroy(&mutexAttr);
    pthread_condattr_destroy(&condAttr);

#if FUSION_BUILD_MULTI
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->frameBufferThreadLock );
    layerData->frameBufferThreadQuit = false;

    snprintf(thread_name, D_ARRAY_SIZE(thread_name), "BRCM FrameBuffer %d", layerId);
    layerData->frameBufferThread = direct_thread_create( DTT_OUTPUT, frameBufferThread, (void *)layerData, thread_name );

    /* Wait for thread to start and to get into 'ready and waiting state' */
    pthread_cond_wait( &layerData->frameBufferThreadSem, &layerData->frameBufferThreadLock );
    pthread_mutex_unlock( &layerData->frameBufferThreadLock );
#endif

    /* Set type and capabilities */
    description->type = DLTF_GRAPHICS;
    description->caps = DLCAPS_SURFACE         | DLCAPS_OPACITY      | DLCAPS_ALPHACHANNEL |
                        DLCAPS_PREMULTIPLIED   | DLCAPS_SRC_COLORKEY | DLCAPS_LEVELS |
                        DLCAPS_SCREEN_POSITION | DLCAPS_SCREEN_SIZE  | DLCAPS_SOURCES;

    description->caps |= DLCAPS_LR_MONO | DLCAPS_STEREO;

    /* Set the layer name. */
    snprintf( description->name, DFB_DISPLAY_LAYER_DESC_NAME_LENGTH,
              "BRCM %s disp gfx layer %d", (displayId == PRIMARY_DISPLAY_ID) ? "primary" : "secondary", layerId );

    /* fill out the default configuration */
    config->flags = DLCONF_WIDTH | DLCONF_HEIGHT | DLCONF_PIXELFORMAT | DLCONF_BUFFERMODE | DLCONF_OPTIONS |
        DLCONF_SURFACE_CAPS | DLCONF_SOURCE | DLCONF_COLORSPACE;

    /* Set default level, number of regions, sources and clipping regions */
    description->level        = 0;  /* Default level is 0. Negative means below and positive means above */
    description->regions      = 1;  /* We only support a single region per layer context */
    description->sources      = 2;  /* HD and SD graphics sources */
    description->clip_regions = 0;

    /* Set the default level for this layer */
    bcmGetLayerDefaultLevel(layerId, &layerData->level);

    /* Set the default size of this layer */
    bcmGetLayerDefaultSize(layerId, &config->width, &config->height);

    /* Set the default pixel format and colorspace for this layer */
    bcmGetLayerDefaultFormat(layerId, &config->pixelformat);
    config->colorspace = DFB_COLORSPACE_DEFAULT(config->pixelformat);

    /* Set the default buffer mode for this layer */
    bcmGetLayerDefaultBufferMode(layerId, &config->buffermode);

    /* Set the default options for this layer */
    bcmGetLayerDefaultOptions(layerId, &config->options);

    /* Set the default surface capabilities for this layer */
    bcmGetLayerDefaultSurfaceCaps(layerId, &config->surface_caps);

    /* Set the default source for this layer */
    config->source = layerData->sourceId;

    /* Set layer callback... */
    layerData->layerCallback = frameBufferCallback;

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: flags=0x%08x, %dx%d, format=%s, bm=%d, opt=0x%08x, surf.caps=0x%08x\n", __FUNCTION__,
            displayId, layerId, config->flags,
            config->width, config->height, dfb_pixelformat_name(config->pixelformat),
            config->buffermode, config->options, config->surface_caps );

    return DFB_OK;
}

static DFBResult
bcmShutdownLayer( UNUSED_ CoreLayer *layer,
                  UNUSED_ void      *driver_data,
                          void      *layer_data )
{
    DFBBCMNEXUS                     *dfb_bcmnexus;
    DFB_PlatformNexusHandle          hDisplay;
    DFB_Platform_P_GraphicsSettings  graphicsSettings;
    bcmLayerData                    *layerData = layer_data;

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d\n", __FUNCTION__, layerData->displayId, layerData->layerId );

    dfb_bcmnexus = dfb_system_data();
    hDisplay = GET_DISPLAY_HND(dfb_bcmnexus, layerData->displayId);
    DFB_Platform_P_GetGraphicsSettings(hDisplay, &graphicsSettings);
    graphicsSettings.frameBufferCallback.callback = NULL;
    graphicsSettings.frameBufferCallback.context= NULL;
    DFB_Platform_P_SetGraphicsSettings(hDisplay, &graphicsSettings);

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: waiting for framebuffer thread to end...\n", __FUNCTION__,
            layerData->displayId, layerData->layerId );

#if FUSION_BUILD_MULTI
    /* Destroy the framebuffer thread */
    direct_thread_cancel( layerData->frameBufferThread );
    direct_thread_join( layerData->frameBufferThread );
    direct_thread_destroy( layerData->frameBufferThread );

    pthread_mutex_destroy( &layerData->frameBufferThreadLock );
    pthread_cond_destroy( &layerData->frameBufferThreadSem );
#endif

    pthread_mutex_destroy( &layerData->lock );
    pthread_mutex_destroy( &layerData->frameBufferSemLock );
    pthread_cond_destroy( &layerData->frameBufferSem );

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Exit.\n", __FUNCTION__, layerData->displayId, layerData->layerId );
    return DFB_OK;
}

/* Common function to test whether the region can be modified to these settings. */
static DFBResult
bcmTestRegion( UNUSED_ CoreLayer                  *layer,
                       void                       *driver_data,
                       void                       *layer_data,
                       CoreLayerRegionConfig      *config,
                       CoreLayerRegionConfigFlags *failed )
{
    bcmLayerData               *layerData    = layer_data;
    CoreLayerRegionConfigFlags  fail         = CLRCF_NONE;
    int                         source_height, source_width;
    int                         display_width, display_height;
    DFBScreenID                 displayId;
    DFBDisplayLayerID           layerId;
    DFB_PlatformDimension       displaySize;

    D_ASSERT( (DFBBCMNEXUS*)driver_data != NULL );

    displayId  = layerData->displayId;
    layerId    = layerData->layerId;

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d (%dx%d) format=%s, caps=0x%08x, bm=%d,\n"
                               "options=0x%08x, source_id=%d, opacity=%d [%dx%d@%d,%d]=>[%dx%d@%d,%d]\n",
                               __FUNCTION__, displayId, layerId, config->width, config->height, dfb_pixelformat_name(config->format),
                               config->surface_caps, config->buffermode, config->options, config->source_id, config->opacity,
                               config->source.w, config->source.h, config->source.x, config->source.y,
                               config->dest.w, config->dest.h, config->dest.x, config->dest.y);

    /* Get Display Rectangle size */
    DFB_Platform_P_GetDisplaySize(displayId, &displaySize);

    display_width  = MIN(config->dest.w, displaySize.w);
    display_height = MIN(config->dest.h, displaySize.h);

    /* Get Source Rectangle size */
    source_width = config->source.w;

    /* Lock access to the layerData */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    if (layerData->hasVscaler)
        source_height = config->source.h;
    else
        source_height = display_height;

    /* Unlock access to the layerData */
    pthread_mutex_unlock( &layerData->lock );

#if (DIRECTFB_MAJOR_VERSION >= 1) && (DIRECTFB_MINOR_VERSION >= 4) && (DIRECTFB_MICRO_VERSION >= 3)
#else
    /* The horizontal/vertical GFD scaler can upscale to a max factor of 4 and can be used to downscale to max factor of 6.  However,
       downscaling requires additional memory bandwidth, so RTS settings may need to be tweaked. */
    if ((display_width > source_width * 4) || (display_width * 6 < source_width))
         fail |= CLRCF_WIDTH;

    /* Check height */
    if ((display_height > source_height * 4) || (display_height * 6 < source_height))
         fail |= CLRCF_HEIGHT;
#endif

    /* Check format */
    switch (config->format) {
          case DSPF_A8:
          case DSPF_LUT2:
          case DSPF_LUT4:
          case DSPF_LUT8:
          case DSPF_ALUT8:
#ifdef BCMNEXUS_PIXELFORMAT_X4_R4_G4_B4_SUPPORTED
          case DSPF_RGB444:
#endif
#ifdef BCMNEXUS_PIXELFORMAT_X1_R5_G5_B5_SUPPORTED
          case DSPF_RGB555:
#endif
#ifdef BCMNEXUS_PIXELFORMAT_X1_B5_G5_R5_SUPPORTED
          case DSPF_BGR555:
#endif
          case DSPF_RGBA4444:
          case DSPF_ARGB4444:
          case DSPF_ARGB1555:
          case DSPF_RGB16:
#ifdef BCMNEXUS_PIXELFORMAT_R8_G8_B8_SUPPORTED
          case DSPF_RGB24:
#endif
          case DSPF_RGB32:
          case DSPF_ARGB:
#if defined(BCMNEXUS_PIXELFORMAT_A8_B8_G8_R8_SUPPORTED) && defined(PIXEL_ABGR)
          case DSPF_ABGR:
#endif
          case DSPF_YUY2:
          case DSPF_UYVY:
          case DSPF_AYUV:
               break;

          default:
               fail |= CLRCF_FORMAT;
    }

    /* Check options */
    if (!D_FLAGS_ARE_IN( config->options, DLOP_OPACITY | DLOP_ALPHACHANNEL | DLOP_SRC_COLORKEY |
                                          DLOP_LR_MONO | DLOP_STEREO))
         fail |= CLRCF_OPTIONS;

    if (D_FLAGS_ARE_SET( config->options, DLOP_LR_MONO | DLOP_STEREO ))
         fail |= CLRCF_OPTIONS;

    if (failed)
         *failed = fail;

    if (fail)
    {
         D_ERROR("bcmNexus/Layer: %s() screen %d gfx layer %d: Failed (flags = 0x%08x)!\n", __FUNCTION__, displayId, layerId, fail);
         return DFB_UNSUPPORTED;
    }

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Exit.\n", __FUNCTION__, displayId, layerId );
    return DFB_OK;
}

/* Common function to add a region to a layer */
static DFBResult
bcmAddRegion( CoreLayer             *layer,
              bcmLayerData          *layerData,
              bcmRegionData         *regionData,
              CoreLayerRegionConfig *config,
              int                    width,
              int                    height)
{
    DFBResult         res = DFB_OK;
    CoreSurface      *tmp_surface;
    DFBScreenID       displayId;
    DFBDisplayLayerID layerId;

    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;

    displayId = layerData->displayId;
    layerId   = layerData->layerId;

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: %dx%d, format=%s, region=%dx%d, caps=0x%08x,\n"
                               "bm=%d, options=0x%08x, source_id=%d, opacity=%d [%dx%d@%d,%d] -> [%dx%d@%d,%d]\n",
                               __FUNCTION__, displayId, layerId, width, height, dfb_pixelformat_name(config->format),
                               config->width, config->height, config->surface_caps, config->buffermode,
                               config->options, config->source_id, config->opacity,
                               config->source.w, config->source.h, config->source.x, config->source.y,
                               config->dest.w, config->dest.h, config->dest.x, config->dest.y);

    /* Lock access to the layerData */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    /* Save current region pointer in layer data */
    layerData->regionData = regionData;

    /* Ensure the update skip count is 0 initially */
    layerData->updateSkipCnt = 0;

    /* Store new region configuration */
    regionData->config = *config;

    /* Save Layer's source screen/display rectangle if not already set */
    if (layerData->srcRectangle.w == 0)
        layerData->srcRectangle = config->source;

    /* Save Layer's destination screen/display rectangle if not already set */
    if (layerData->destRectangle.w == 0)
        layerData->destRectangle = config->dest;

    if (brcm_config->ibuffer.width && brcm_config->ibuffer.height)
        layerData->ibuffer = true;

    /* If graphics feeder doesn't have a vertical scaler, then we need to create
       a temporary framebuffer to allow vertical scaling to occur */
    if (!layerData->hasVscaler || layerData->ibuffer)
    {
#if BCM_DFB_SURFACE_CREATE_WITH_HINT
        res = dfb_surface_create_simple( layer->core, width, height, config->format, config->colorspace,
                DSCAPS_VIDEOONLY | DSCAPS_STEREO, CSTF_SHARED, layerId, DSHF_LAYER, NULL, &tmp_surface );
#else
        res = dfb_surface_create_simple( layer->core, width, height, config->format, config->colorspace,
                DSCAPS_VIDEOONLY | DSCAPS_STEREO, CSTF_SHARED, layerId, NULL, &tmp_surface );
#endif

        if (res == DFB_OK)
        {
            D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Created framebuffer %p: %dx%d (format=%s).\n", __FUNCTION__,
                        displayId, layerId, (void *)tmp_surface, width, height, dfb_pixelformat_name(config->format) );

            layerData->frameBuffer = tmp_surface;

            /* Make the surface global for all processes to access the reference count */
            res = dfb_surface_globalize( layerData->frameBuffer );
        }
    }
    /* Unlock access to the layerData */
    pthread_mutex_unlock( &layerData->lock );

    D_ASSERT( res == DFB_OK );

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Exit [%d].\n", __FUNCTION__, displayId, layerId, res );
    return res;
}

/* Common function to set the primary region to the new settings. */
static DFBResult
bcmSetRegion( UNUSED_ CoreLayer                  *layer,
                      void                       *driver_data,
                      void                       *layer_data,
                      void                       *region_data,
                      CoreLayerRegionConfig      *config,
                      CoreLayerRegionConfigFlags  updated,
                      CoreSurface                *surface,
                      CorePalette                *palette,
                      CoreSurfaceBufferLock      *left_lock,
                      CoreSurfaceBufferLock      *right_lock )
{
    DFBResult              res;
    DFBBCMNEXUS           *dfb_bcmnexus = driver_data;
    bcmLayerData          *layerData    = layer_data;
    bcmRegionData         *regionData   = region_data;
    CoreSurfaceBufferLock  leftFrameBufferLock, rightFrameBufferLock;
    CoreLayerRegionConfig *regionConfig;
    NEXUS_SurfaceHandle    hLeftFrameBuffer, hRightFrameBuffer;
    DFB_Platform_P_GraphicsSettings graphicsSettings;
    DFBScreenID            displayId;
    DFBDisplayLayerID      layerId;
    DFB_PlatformDimension  displaySize;
    int                    source_width, source_height;
    bool                   reenable_gfx;
    bool                   displayOn;
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
     DFBSurfaceStereoEye    surfaceEye, frameBufferEye;
#endif
    CoreSurfaceBuffer     *buffer;

    D_ASSERT( dfb_bcmnexus != NULL );

    /* Lock access to the layerData */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    displayId    = layerData->displayId;
    layerId      = layerData->layerId;
    regionConfig = &regionData->config;

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: updated flags=0x%08x, surface=%p,\n"
                               "handle=%p, format=%s, region=%dx%d, caps=0x%08x, bm=%d,\n"
                               "options=0x%08x, source_id=%d, opacity=%d [%dx%d@%d,%d] -> [%dx%d@%d,%d]\n",
                               __FUNCTION__, displayId, layerId, updated, (void *)surface, left_lock->handle, dfb_pixelformat_name(config->format),
                               config->width, config->height, config->surface_caps, config->buffermode,
                               config->options, config->source_id, config->opacity,
                               config->source.w, config->source.h, config->source.x, config->source.y,
                               config->dest.w, config->dest.h, config->dest.x, config->dest.y);

    regionData->leftBufferLock = *left_lock;

    if (config->options & DLOP_STEREO)
          regionData->rightBufferLock = *right_lock;

    hLeftFrameBuffer = left_lock->handle;
    hRightFrameBuffer = (config->options & DLOP_STEREO) ? right_lock->handle : NULL;

    if (D_FLAGS_IS_SET(updated, CLRCF_PALETTE) && palette) {
        /* iterate through all connected buffers */
        int i;

        /* Lock the surface prior to modifying the palette */
        if (dfb_surface_lock( surface )) {
            /* Unlock access to the layerData */
            pthread_mutex_unlock( &layerData->lock );
            return DFB_FUSION;
        }

        for( i=0; i<surface->num_buffers; i++ ) {
            CoreSurfaceBufferLock  lock;

            buffer = surface->left_buffers[i];
            res = dfb_surface_buffer_lock( buffer, CSAID_CPU, CSAF_WRITE | CSAF_READ, &lock );
            if ( res == DFB_OK ) {
                copy_palette( surface, lock.handle );
                dfb_surface_buffer_unlock( &lock );
            }
            else
                fprintf(stderr,"FIX THIS!!!\n");

            if (surface->config.caps & DSCAPS_STEREO)
            {
                buffer = surface->right_buffers[i];
                res = dfb_surface_buffer_lock( buffer, CSAID_CPU, CSAF_WRITE | CSAF_READ, &lock );
                if ( res == DFB_OK ) {
                    copy_palette( surface, lock.handle );
                    dfb_surface_buffer_unlock( &lock );
                }
                else
                    fprintf(stderr,"FIX THIS!!!\n");
            }
        }
        dfb_surface_unlock( surface );
    }

    if (!layerData->layerCallback) {
        layerData->layerCallback = frameBufferCallback;
    }

    if (!layerData->frameBuffer) {
        layerData->frameBuffer = surface;
    }

    /* If all flags are set, then don't update anything as it will
       have already been updated in AddRegion */

#if ((DIRECTFB_MAJOR_VERSION * 1000000 + \
      DIRECTFB_MINOR_VERSION * 1000 + \
      DIRECTFB_MICRO_VERSION) >= 1004003)

    if (true)
#else
    if (!D_FLAGS_ARE_SET(updated, CLRCF_ALL))
#endif
    {
        /* Save Region Configuration */
        if (D_FLAGS_IS_SET(updated, CLRCF_WIDTH))
            regionConfig->width = config->width;

        if (D_FLAGS_IS_SET(updated, CLRCF_HEIGHT))
            regionConfig->height = config->height;

        if (D_FLAGS_IS_SET(updated, CLRCF_FORMAT))
            regionConfig->format = config->format;

        if (D_FLAGS_IS_SET(updated, CLRCF_SURFACE_CAPS))
            regionConfig->surface_caps = config->surface_caps;

        if (D_FLAGS_IS_SET(updated, CLRCF_BUFFERMODE))
            regionConfig->buffermode = config->buffermode;

        if (D_FLAGS_IS_SET(updated, CLRCF_OPTIONS))
            regionConfig->options = config->options;

        if (D_FLAGS_IS_SET(updated, CLRCF_SOURCE_ID))
            regionConfig->source_id = config->source_id;

        if (D_FLAGS_IS_SET(updated, CLRCF_SOURCE)) {
            regionConfig->source = config->source;
            layerData->srcRectangle = config->source;
        }

        if (D_FLAGS_IS_SET(updated, CLRCF_DEST)) {
            regionConfig->dest = config->dest;
            layerData->destRectangle = config->dest;
        }

        if (D_FLAGS_IS_SET(updated, CLRCF_CLIPS)) {
            regionConfig->clips     = config->clips;
            regionConfig->num_clips = config->num_clips;
            regionConfig->positive  = config->positive;
        }

        if (D_FLAGS_IS_SET(updated, CLRCF_OPACITY))
            regionConfig->opacity = config->opacity;

        if (D_FLAGS_IS_SET(updated, CLRCF_ALPHA_RAMP)) {
            regionConfig->alpha_ramp[0] = config->alpha_ramp[0];
            regionConfig->alpha_ramp[1] = config->alpha_ramp[1];
            regionConfig->alpha_ramp[2] = config->alpha_ramp[2];
            regionConfig->alpha_ramp[3] = config->alpha_ramp[3];
        }

        if (D_FLAGS_IS_SET(updated, CLRCF_SRCKEY))
            regionConfig->src_key = config->src_key;

        if (D_FLAGS_IS_SET(updated, CLRCF_DSTKEY))
            regionConfig->dst_key = config->dst_key;

        if (D_FLAGS_IS_SET(updated, CLRCF_PARITY))
            regionConfig->parity = config->parity;
    }

    /* Setup Source Configuration */
    layerData->sourceId = config->source_id;

    /* Get whether the screen/display is on */
    displayOn = (bcmGetPowerMode(dfb_screens_at(displayId)) == DSPM_ON);

    /* Check if we need to re-enable the graphics layer */
    reenable_gfx = ((layerData->layerEnabled == false) && displayOn);

    layerData->layerEnabled  = displayOn;
    layerData->srcColorKeyOn = D_FLAGS_IS_SET(regionConfig->options, DLOP_SRC_COLORKEY);

    /* Case 1: Per-pixel alpha blending enabled on a non-premultiplied source surface.
               In this case, we need to use the following blending equation:
               Oc = (Sc * Sa) + (1 - Sa) * Dc

       Case 2: Per-pixel alpha blending enabled on a pre-multiplied source surface.
               In this case, we don't need to multiply the Sc by the Sa as this has
               already been completed.  The Sa needs to be set to 1 by using the constant alpha
               value of 0xff:
               Oc = (Sc * Ca) + (1 - Sa) * Dc
               Ca = 0xff
       Case 3: Global alpha blending enabled on a source surface.
               In this case, we need to use the constant alpha as the "global" alpha value:
               Oc = (Sc * Ca) + (1 - Ca) * Dc
       Case 4: Both per-pixel alpha blending and global alpha value are set on a non-premultiplied source surface.
               In this case, we can use the standard blending equation as well as the independent alpha value:
               Oc = (Sc * Sa) + (1 - Sa) * Dc
       Case 5: Both per-pixel alpha blending and global alpha value are set on a premultiplied source surface.
               In this case, we can use the independent alpha value along with constant source alpha of 0xff:
               Oc = (Sc * Ca) + (1 - Sa) * Dc
               Ca = 0xff
    */

    layerData->srcBlendFunc             = DFB_PlatformBlendFactor_eOne;
    layerData->destBlendFunc            = DFB_PlatformBlendFactor_eZero;
    layerData->alpha                    = 0xFF;
    layerData->constantAlpha            = 0xFF;

    if (regionConfig->options & DLOP_ALPHACHANNEL) {
        layerData->srcBlendFunc         = DFB_PlatformBlendFactor_eSourceAlpha;
        layerData->destBlendFunc        = DFB_PlatformBlendFactor_eInverseSourceAlpha;

        if (surface->config.caps & DSCAPS_PREMULTIPLIED)
        {
            layerData->srcBlendFunc     = DFB_PlatformBlendFactor_eConstantAlpha;
        }

        if (!DFB_PIXELFORMAT_HAS_ALPHA(surface->config.format))
        {
            layerData->srcBlendFunc     = DFB_PlatformBlendFactor_eConstantAlpha;

        }
        layerData->alpha = (regionConfig->options & DLOP_OPACITY) ? regionConfig->opacity : 0xff;
    }
    else if (regionConfig->options & DLOP_OPACITY) {
        layerData->constantAlpha        = regionConfig->opacity;
        layerData->srcBlendFunc         = DFB_PlatformBlendFactor_eConstantAlpha;
        /* There is a bug in the VDC on some chips where setting the dest blend func to (1-Ca) causes
           a green tint.  The temporary solution is to set this to (1-Sa) if the constant alpha
           value is 0xFF. */
        /*constantAlpha needs to be set to 0xFF to avod vdc invalid blend errors*/
        layerData->constantAlpha = 0xFF;
        layerData->alpha = regionConfig->opacity;
        layerData->destBlendFunc    = DFB_PlatformBlendFactor_eInverseSourceAlpha;
    }

    /* Color keying doesn't work if the blending equations use constants
       e.g. Sa=Ca, Da=(1-Ca)  or Sa=1, Da=0 */
    if (layerData->srcColorKeyOn) {
        if(layerData->destBlendFunc != DFB_PlatformBlendFactor_eInverseSourceAlpha)
        {
        layerData->srcBlendFunc         = DFB_PlatformBlendFactor_eSourceAlpha;
        layerData->destBlendFunc        = DFB_PlatformBlendFactor_eInverseSourceAlpha;

        /* Ensure the opacity can still be changed, as this uses the alpha value */
        if (layerData->constantAlpha != 0xFF)
            layerData->alpha = layerData->constantAlpha;
    }

    }

    /* Now get the updated graphics settings */
    bcmGetLayerGraphicsSettings(dfb_bcmnexus, layerId, &graphicsSettings);

    source_width  = graphicsSettings.clip.width;
    source_height = graphicsSettings.clip.height;

    DFB_Platform_P_GetDisplaySize(displayId, &displaySize);

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Requested: %dx%d: src:[%dx%d@%d,%d] => dst:[%dx%d@%d,%d], zorder=%d\n", __FUNCTION__,
                displayId, layerId,
                regionConfig->width, regionConfig->height,
                regionConfig->source.w, regionConfig->source.h, regionConfig->source.x, regionConfig->source.y,
                regionConfig->dest.w, regionConfig->dest.h, regionConfig->dest.x, regionConfig->dest.y,
                graphicsSettings.zorder );

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Actual:    %dx%d: src:[%dx%d@%d,%d] => dst:[%dx%d@%d,%d], zorder=%d\n", __FUNCTION__,
                displayId, layerId,
                displaySize.w, displaySize.h,
                source_width, source_height, graphicsSettings.clip.x, graphicsSettings.clip.y,
                graphicsSettings.position.width, graphicsSettings.position.height, graphicsSettings.position.x, graphicsSettings.position.y,
                graphicsSettings.zorder );

    if (layerData->srcColorKeyOn)
        D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: ColorKey[RGB]=0x%02x,0x%02x,0x%02x\n", __FUNCTION__,
                    displayId, layerId, regionConfig->src_key.r, regionConfig->src_key.g, regionConfig->src_key.b);

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: sourceBlendFactor=%d,destBlendFactor=%d,alpha=%d,constantAlpha=%d\n", __FUNCTION__,
                displayId, layerId, graphicsSettings.sourceBlendFactor, graphicsSettings.destBlendFactor,
                graphicsSettings.alpha, graphicsSettings.constantAlpha);

    regionData->vscale = false;
    if ((graphicsSettings.clip.height != layerData->srcRectangle.h) || layerData->ibuffer)
    {
        DFBRectangle rect;

        D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: using blitter to vscale layer\n", __FUNCTION__, displayId, layerId);

        if (graphicsSettings.clip.height != layerData->srcRectangle.h)
        regionData->vscale = true;

        /* Copy the source surface to the layer's framebuffer */
        D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Going to flush %p...\n", __FUNCTION__, displayId, layerId, (void *)hLeftFrameBuffer );
        NEXUS_Surface_Flush( hLeftFrameBuffer );
        if ( hRightFrameBuffer )
            NEXUS_Surface_Flush( hRightFrameBuffer );

        if (dfb_surface_lock( layerData->frameBuffer )) {
            /* Unlock access to the layerData */
            pthread_mutex_unlock( &layerData->lock );
            return DFB_FUSION;
        }

        /* We use the M2MC to scale only the vertical and the GFD for the horizontal */
        rect.x = 0;
        rect.y = 0;

        bcmGetClipRectangle(layerData, displaySize.w, displaySize.h, source_width, source_height, &rect.w, &rect.h);

#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE

        /* Save stereo eye */
        surfaceEye = dfb_surface_get_stereo_eye( surface );
        frameBufferEye = dfb_surface_get_stereo_eye( layerData->frameBuffer );

        dfb_surface_set_stereo_eye( surface, DSSE_LEFT );
        dfb_surface_set_stereo_eye( layerData->frameBuffer, DSSE_LEFT );
        dfb_gfx_stretch_to( surface, layerData->frameBuffer, &layerData->srcRectangle, &rect, false);
#else
        dfb_gfx_stretch_stereo( surface, DSSE_LEFT, layerData->frameBuffer, DSSE_LEFT, &layerData->srcRectangle, &rect, false);
#endif

        if ( surface->config.caps & DSCAPS_STEREO )
        {
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
            dfb_surface_set_stereo_eye( surface, DSSE_RIGHT );
            dfb_surface_set_stereo_eye( layerData->frameBuffer, DSSE_RIGHT );
            dfb_gfx_stretch_to( surface, layerData->frameBuffer, &layerData->srcRectangle, &rect, false);
#else
            dfb_gfx_stretch_stereo( surface, DSSE_RIGHT, layerData->frameBuffer, DSSE_RIGHT, &layerData->srcRectangle, &rect, false);
#endif
        }

#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
        dfb_surface_set_stereo_eye( surface, surfaceEye );
#endif

        dfb_gfxcard_sync();

        /* Need to instantiate a real surface buffer for the framebuffer */
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
        dfb_surface_set_stereo_eye( layerData->frameBuffer, DSSE_LEFT );
        buffer = dfb_surface_get_buffer( layerData->frameBuffer, CSBR_FRONT );
#else
        buffer = dfb_surface_get_buffer2( layerData->frameBuffer, CSBR_FRONT, DSSE_LEFT );
#endif
        D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
        res = dfb_surface_buffer_lock( buffer, CSAID_GPU, CSAF_READ, &leftFrameBufferLock );
        if (res)
        {
            D_ERROR( "%s() screen %d gfx layer %d: Could NOT lock framebuffer surface! (res=%d)\n", __FUNCTION__, displayId, layerId, res );
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
            dfb_surface_set_stereo_eye( layerData->frameBuffer, frameBufferEye );
#endif
            dfb_surface_unlock( layerData->frameBuffer );
            /* Unlock access to the layerData */
            pthread_mutex_unlock( &layerData->lock );
            return res;
        }
        regionData->leftBufferLock = leftFrameBufferLock;
        hLeftFrameBuffer = leftFrameBufferLock.handle;

        if ( surface->config.caps & DSCAPS_STEREO )
        {
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
            dfb_surface_set_stereo_eye( layerData->frameBuffer, DSSE_RIGHT );
            buffer = dfb_surface_get_buffer( layerData->frameBuffer, CSBR_FRONT );
#else
            buffer = dfb_surface_get_buffer2( layerData->frameBuffer, CSBR_FRONT, DSSE_RIGHT );
#endif
            D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
            res = dfb_surface_buffer_lock( buffer, CSAID_GPU, CSAF_READ, &rightFrameBufferLock );
            if ( res )
            {
                D_ERROR( "%s() screen %d gfx layer %d: Could NOT lock right framebuffer surface! (res=%d)\n", __FUNCTION__, displayId, layerId, res );
                dfb_surface_buffer_unlock( &leftFrameBufferLock );
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                dfb_surface_set_stereo_eye( layerData->frameBuffer, frameBufferEye );
#endif
                dfb_surface_unlock( layerData->frameBuffer );
                /* Unlock access to the layerData */
                pthread_mutex_unlock( &layerData->lock );
                return res;
            }
        }
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
        dfb_surface_set_stereo_eye( layerData->frameBuffer, frameBufferEye );
#endif
    }

    /* Ensure the surface is flushed prior to the graphics feeder reading it. */
    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Going to flush %p...\n", __FUNCTION__, displayId, layerId, (void *)hLeftFrameBuffer );
    NEXUS_Surface_Flush( hLeftFrameBuffer );
    if ( hRightFrameBuffer )
        NEXUS_Surface_Flush( hRightFrameBuffer );

    /* If the graphics was disabled, then we need to call NEXUS_Display_SetGraphicsSettings first before calling
       NEXUS_Display_SetGraphicsFramebuffer.  If not, then we need to make sure we do this in the opposite order
       to prevent a stale surface buffer being used in the call to NEXUS_Display_SetGraphicsSettings! */
    if (reenable_gfx == true)
    {
        bcmSetLayerGraphicsSettings(dfb_bcmnexus, layerId);
        bcmSetLayerFrameBuffer(dfb_bcmnexus, layerId);
    } else {
        bcmSetLayerFrameBuffer(dfb_bcmnexus, layerId);
        bcmSetLayerGraphicsSettings(dfb_bcmnexus, layerId);
    }

    if (regionData->vscale == true)
    {
        dfb_surface_buffer_unlock(&leftFrameBufferLock);
        if (regionConfig->surface_caps & DSCAPS_STEREO)
        {
            dfb_surface_buffer_unlock(&rightFrameBufferLock);
        }

        if (dfb_surface_unlock( layerData->frameBuffer )) {
            /* Unlock access to the layerData */
            pthread_mutex_unlock( &layerData->lock );
            return DFB_FUSION;
        }
    }

    /* Unlock access to the layerData */
    pthread_mutex_unlock( &layerData->lock );

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Exit.\n", __FUNCTION__, displayId, layerId );
    return DFB_OK;
}

/* Common function to remove a region from a layer */
static DFBResult
bcmRemoveRegion( UNUSED_ CoreLayer *layer,
                         void      *driver_data,
                         void      *layer_data,
                 UNUSED_ void      *region_data )
{
    DFBBCMNEXUS            *dfb_bcmnexus = driver_data;
    bcmLayerData           *layerData    = layer_data;
    DFBDisplayLayerID       layerId;
    DFBScreenID             displayId;

    D_ASSERT( dfb_bcmnexus != NULL );

    layerId      = layerData->layerId;
    displayId    = layerData->displayId;

    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d\n", __FUNCTION__, displayId, layerId );

    layerData->layerEnabled = false;
    bcmSetLayerGraphicsSettings(dfb_bcmnexus, layerId);

    if (!layerData->hasVscaler)
    {
        D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Removing framebuffer %p.\n", __FUNCTION__,
                    displayId, layerId, (void *)layerData->frameBuffer);

        /* Decrement the internal reference count and if 0, then really destroy the surface */
        dfb_surface_unlink( &layerData->frameBuffer );
    }
    layerData->frameBuffer = NULL;
    layerData->regionData  = NULL;

    pthread_mutex_unlock( &layerData->lock );

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Exit\n", __FUNCTION__, displayId, layerId );

    return DFB_OK;
}

/* Common function to invalidate a region */
static DFBResult
bcmInvalidateRegion( DFBBCMNEXUS           *dfb_bcmnexus,
                     bcmLayerData          *layerData,
                     bcmRegionData         *regionData,
                     CoreSurface           *surface,
                     DFBSurfaceFlipFlags    flags,
                     CoreSurfaceBufferLock *left_lock,
                     CoreSurfaceBufferLock *right_lock,
                     bool                   flipping)
{
    DFBResult              res = DFB_OK;
    DFBDisplayLayerID      layerId;
    DFBScreenID            displayId;
    DFB_PlatformDimension  displaySize;
    CoreSurfaceBufferLock  leftFrameBufferLock, rightFrameBufferLock;
    CoreSurfaceBufferLock *pLeftFrameBufferLock, *pRightFrameBufferLock;
    DFBRectangle           rect;
    bool                   vscale;
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
    DFBSurfaceStereoEye    surfaceEye, frameBufferEye;
#endif
    CoreSurfaceBuffer     *buffer;

    D_ASSERT( dfb_bcmnexus != NULL );

    layerId   = layerData->layerId;
    displayId = layerData->displayId;

    pLeftFrameBufferLock = left_lock;
    pRightFrameBufferLock = right_lock;

    vscale = regionData->vscale;

    if (((vscale == true) || layerData->ibuffer ) && layerData->frameBuffer)
    {
        D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: using blitter to vscale layer\n", __FUNCTION__, displayId, layerId);
        if (dfb_surface_lock( layerData->frameBuffer )) {
            return DFB_FUSION;
        }

        DFB_Platform_P_GetDisplaySize(displayId, &displaySize);

        /* We use the M2MC to scale only the vertical and the GFD for the horizontal */
        rect.x = 0;
        rect.y = 0;

        bcmGetClipRectangle(layerData, displaySize.w, displaySize.h, layerData->srcRectangle.w, MIN(layerData->destRectangle.h, displaySize.h), &rect.w, &rect.h);

        if (((rect.w == layerData->srcRectangle.w)) && (rect.h == MIN(layerData->destRectangle.h, displaySize.h)))
        {
            if (vscale != true)
            {
                /*No scaling required*/
                goto no_scale;
            }
        }

#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
        surfaceEye = dfb_surface_get_stereo_eye( surface );
        frameBufferEye = dfb_surface_get_stereo_eye( layerData->frameBuffer );

        dfb_surface_set_stereo_eye( surface, DSSE_LEFT );
        dfb_surface_set_stereo_eye( layerData->frameBuffer, DSSE_LEFT );
        dfb_gfx_stretch_to( surface, layerData->frameBuffer, &layerData->srcRectangle, &rect, flipping );
#else
        dfb_gfx_stretch_stereo( surface, DSSE_LEFT, layerData->frameBuffer, DSSE_LEFT, &layerData->srcRectangle, &rect, flipping );
#endif

        if (surface->config.caps & DSCAPS_STEREO)
        {
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
            dfb_surface_set_stereo_eye( surface, DSSE_RIGHT );
            dfb_surface_set_stereo_eye( layerData->frameBuffer, DSSE_RIGHT );
            dfb_gfx_stretch_to( surface, layerData->frameBuffer, &layerData->srcRectangle, &rect, flipping );
#else
        dfb_gfx_stretch_stereo( surface, DSSE_RIGHT, layerData->frameBuffer, DSSE_RIGHT, &layerData->srcRectangle, &rect, flipping );
#endif
        }

#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
        dfb_surface_set_stereo_eye( surface, surfaceEye );
#endif
        dfb_gfxcard_sync();

        pLeftFrameBufferLock = &leftFrameBufferLock;
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
        dfb_surface_set_stereo_eye( layerData->frameBuffer, DSSE_LEFT );
        buffer = dfb_surface_get_buffer( layerData->frameBuffer, CSBR_FRONT );
#else
        buffer = dfb_surface_get_buffer2( layerData->frameBuffer, CSBR_FRONT, DSSE_LEFT );
#endif

        D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
        res = dfb_surface_buffer_lock( buffer, CSAID_GPU, CSAF_READ, pLeftFrameBufferLock );
        if (res)
        {
            D_ERROR( "%s() screen %d gfx layer %d: Could NOT lock framebuffer! (res=%d)\n", __FUNCTION__, displayId, layerId, res );
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
            dfb_surface_set_stereo_eye( layerData->frameBuffer, frameBufferEye );
#endif
            dfb_surface_unlock( layerData->frameBuffer );
            return res;
        }

        if (surface->config.caps & DSCAPS_STEREO)
        {
            pRightFrameBufferLock = &rightFrameBufferLock;
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
            dfb_surface_set_stereo_eye( layerData->frameBuffer, DSSE_RIGHT );
            buffer = dfb_surface_get_buffer( layerData->frameBuffer, CSBR_FRONT );
#else
            buffer = dfb_surface_get_buffer2( layerData->frameBuffer, CSBR_FRONT, DSSE_RIGHT );
#endif
            D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
            res = dfb_surface_buffer_lock( buffer, CSAID_GPU, CSAF_READ, pRightFrameBufferLock );
            if (res)
            {
                D_ERROR( "%s() screen %d gfx layer %d: Could NOT lock right framebuffer! (res=%d)\n", __FUNCTION__, displayId, layerId, res );
                dfb_surface_buffer_unlock( pLeftFrameBufferLock );
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                dfb_surface_set_stereo_eye( layerData->frameBuffer, frameBufferEye );
#endif
                dfb_surface_unlock( layerData->frameBuffer );
                return res;
            }
        }
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
        dfb_surface_set_stereo_eye( layerData->frameBuffer, frameBufferEye );
#endif
    }
no_scale:

    /* Only Set the Graphics Framebuffer if the layer is enabled and the framebuffer has changed. */
    if (layerData->layerEnabled &&
        ((pLeftFrameBufferLock->handle != regionData->leftBufferLock.handle) ||
         (pRightFrameBufferLock && (pRightFrameBufferLock->handle != regionData->rightBufferLock.handle))))
    {
        regionData->leftBufferLock = *pLeftFrameBufferLock;
        if (pRightFrameBufferLock)
            regionData->rightBufferLock = *pRightFrameBufferLock;
        else
            memset(&regionData->rightBufferLock, 0, sizeof(regionData->rightBufferLock));

        if (flags & DSFLIP_WAIT) {
            D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: waiting for framebuffer %p callback...\n", __FUNCTION__,
                    displayId, layerId, (void *)layerData->frameBuffer);
            PTHREAD_ROBUST_MUTEX_LOCK( &layerData->frameBufferSemLock );
        }

        /* Inform DirectFB that the buffers have changed function */
        if (flipping)
            dfb_surface_flip( surface, false );

        bcmSetLayerFrameBuffer(dfb_bcmnexus, layerId);

        if (flags & DSFLIP_WAIT) {
            pthread_cond_wait( &layerData->frameBufferSem, &layerData->frameBufferSemLock );
            pthread_mutex_unlock( &layerData->frameBufferSemLock );

            D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: received callback for framebuffer %p.\n", __FUNCTION__,
                    displayId, layerId, (void *)layerData->frameBuffer);
        }
    }
    else {
        /* Inform DirectFB that the buffers have changed function */
        if (flipping)
            dfb_surface_flip( surface, false );
    }

    if ((vscale == true) && layerData->frameBuffer)
    {
        D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: unlocking framebuffer %p\n", __FUNCTION__,
                    displayId, layerId, (void *)layerData->frameBuffer);

        dfb_surface_buffer_unlock( pLeftFrameBufferLock );
        if (surface->config.caps & DSCAPS_STEREO)
        {
            dfb_surface_buffer_unlock( pRightFrameBufferLock );
        }
        if (dfb_surface_unlock( layerData->frameBuffer )) {
            return DFB_FUSION;
        }
    }
    return res;
}

/* Common function to flip a region */
static DFBResult
bcmFlipRegion( DFBBCMNEXUS           *dfb_bcmnexus,
               bcmLayerData          *layerData,
               bcmRegionData         *regionData,
               CoreSurface           *surface,
               DFBSurfaceFlipFlags    flags,
               CoreSurfaceBufferLock *left_lock,
               CoreSurfaceBufferLock *right_lock)
{
    DFBResult res;

    res = bcmInvalidateRegion(dfb_bcmnexus, layerData, regionData, surface, flags, left_lock, right_lock, true);

    return res;
}

static DFBResult
bcmGetLevel( UNUSED_ CoreLayer *layer,
             UNUSED_ void      *driver_data,
                     void      *layer_data,
                     int       *level )
{
    bcmLayerData *layerData = layer_data;

    /* Lock access to the layerData */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    *level = layerData->level;

    /* Unlock access to the layerData */
    pthread_mutex_unlock( &layerData->lock );

    D_DEBUG_AT(bcmnexusLayer, "%s() screen %d gfx layer %d: level=%d\n", __FUNCTION__, layerData->displayId, layerData->layerId, *level);
    return DFB_OK;
}

/* Set the z-order of this layer.
   0 is the default layer
   +ve are higher than this layer
   -ve are lower than this layer

   Nexus levels go from 0 (lowest) to BCMNEXUS_Z_ORDER_MAX (highest)
   Default level is 2 for graphics (which is 0 level for DFB)
*/
static DFBResult
bcmSetLevel( UNUSED_ CoreLayer *layer,
                     void      *driver_data,
                     void      *layer_data,
                     int        level )
{
    DFBResult              res          = DFB_INVARG;
    DFBBCMNEXUS           *dfb_bcmnexus = driver_data;
    bcmLayerData          *layerData    = layer_data;
    DFBDisplayLayerID      layerId;
    DFBScreenID            displayId;

    layerId   = layerData->layerId;
    displayId = layerData->displayId;

    D_DEBUG_AT(bcmnexusLayer, "%s() screen %d gfx layer %d: level=%d\n", __FUNCTION__, displayId, layerId, level);

    if ((level <= BCMNEXUS_Z_ORDER_MAX/2) && (level >= -BCMNEXUS_Z_ORDER_MAX/2))
    {
        /* Lock access to the layerData */
        PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

        layerData->level = level;
        res = bcmSetLayerGraphicsSettings(dfb_bcmnexus, layerId);
        if (res != DFB_OK)
            D_ERROR("%s() screen %d gfx layer %d: Cannot set layer level to %d!\n", __FUNCTION__,
                    displayId, layerId, level);

        /* Unlock access to the layerData */
        pthread_mutex_unlock( &layerData->lock );
    } else
        D_ERROR("%s() screen %d gfx layer %d: Invalid level %d! It needs to be between %d and -%d.\n", __FUNCTION__,
                displayId, layerId, level, BCMNEXUS_Z_ORDER_MAX/2, BCMNEXUS_Z_ORDER_MAX/2);

    D_DEBUG_AT( bcmnexusLayer, "%s() screen %d gfx layer %d: Exit [%d].\n", __FUNCTION__, displayId, layerId, res );
    return res;
}

/*
 * Set the stereo depth of this layer.
 *
 * The left buffer will be shifted +z and the right buffer
 * will be shifted -z.
 */
static DFBResult
bcmSetStereoDepth( UNUSED_ CoreLayer  *layer,
                    void               *driver_data,
                    void               *layer_data,
                    bool                follow_video,
                    int                 z )
{
    DFBResult              res = DFB_OK;
    DFBBCMNEXUS           *dfb_bcmnexus = driver_data;
    bcmLayerData          *layerData = layer_data;
    DFBScreenID            displayId;
    DFBDisplayLayerID      layerId;

    layerId   = layerData->layerId;
    displayId = layerData->displayId;

    D_DEBUG_AT(bcmnexusLayerX, "%s() screen %d gfx layer %d: %s z (%d)\n", __FUNCTION__, displayId, layerId,
        follow_video ? "metadata" : "fixed", z);

    if (follow_video) {
        D_ERROR("%s() follow_video only supported on BluRay platforms! %d %d\n", __FUNCTION__,
            displayId, layerId);
        return DFB_UNSUPPORTED;
    }

    /* Lock access to the layerData */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    layerData->follow_video = follow_video;
    layerData->z = z;
    if (layerData->layerEnabled) {
        res = bcmSetLayerGraphicsSettings(dfb_bcmnexus, layerId);
        if (res != DFB_OK)
        {
                D_ERROR("%s() screen %d gfx layer %d: Cannot set layer stereo depth (%d)!\n", __FUNCTION__,
                displayId, layerId, z);
        }
    }
    /* Unlock access to the layerData */
    pthread_mutex_unlock( &layerData->lock );

    return res;
}

/**************************************************
 * Setup the Primary Display Primary Layer/Region *
 **************************************************/

/* initialize the primary graphics layer data
 * this function is called for the master fusionee only
 */
static DFBResult
bcmInitPrimaryDisplayPrimaryLayer( CoreLayer                  *layer,
                                   void                       *driver_data,
                                   void                       *layer_data,
                                   DFBDisplayLayerDescription *description,
                                   DFBDisplayLayerConfig      *config,
                           UNUSED_ DFBColorAdjustment         *adjustment )
{
    DFBBCMNEXUS  *dfb_bcmnexus = driver_data;
    bcmLayerData *layerData    = layer_data;

    D_ASSERT( dfb_bcmnexus != NULL );

    layerData->layerId   = DLID_PRIMARY;
    layerData->displayId = dfb_screen_id_translated(dfb_layer_screen(layer));
    layerData->sourceId  = DLSID_SURFACE;
#ifdef BCMNEXUS_GFD0_HAS_VERTICAL_SCALER
    layerData->hasVscaler = true;
#endif

    return bcmInitLayer(dfb_bcmnexus, layerData, description, config);
}

/* Add a primary region to the primary display's primary layer */
static DFBResult
bcmAddPrimaryDisplayPrimaryRegion(         CoreLayer             *layer,
                                   UNUSED_ void                  *driver_data,
                                           void                  *layer_data,
                                           void                  *region_data,
                                           CoreLayerRegionConfig *config )
{
    return bcmAddRegion(layer, layer_data, region_data, config, BCMNEXUS_MAX_HD_DISPLAY_WIDTH, BCMNEXUS_MAX_HD_DISPLAY_HEIGHT);
}

/* Flip the primary region */
static DFBResult
bcmFlipPrimaryDisplayPrimaryRegion( UNUSED_ CoreLayer             *layer,
                                            void                  *driver_data,
                                            void                  *layer_data,
                                            void                  *region_data,
                                            CoreSurface           *surface,
                                            DFBSurfaceFlipFlags    flags,
                                            const DFBRegion       *left_update,
                                            CoreSurfaceBufferLock *left_lock,
                                            const DFBRegion       *right_update,
                                            CoreSurfaceBufferLock *right_lock )
{
    DFBResult             res;
    DFBBCMNEXUS          *dfb_bcmnexus = driver_data;
    bcmLayerData         *layerData    = layer_data;
    DFBDisplayLayerID     layerId;
    DFBScreenID           displayId;
#if (NUM_DISPLAYS > 1)
    bcmLayerData         *layerDataSec;
    bcmRegionData        *regionData;
    DFBRectangle          rect;
    CoreSurfaceBufferLock leftFrameBufferLock, rightFrameBufferLock;
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
    DFBSurfaceStereoEye   surfaceEye, frameBufferEye;
#endif
    CoreSurfaceBuffer    *buffer;
    DFB_PlatformDimension displaySize;
#endif
    DFBRectangle left_update_rect,right_update_rect;


    layerId   = layerData->layerId;
    displayId = layerData->displayId;

    if (left_update)
    {
        dfb_rectangle_from_region(&left_update_rect, left_update);
    }
    else
    {
        left_update_rect.x = layerData->srcRectangle.x;
        left_update_rect.y = layerData->srcRectangle.y;
        left_update_rect.h = layerData->srcRectangle.h;
        left_update_rect.w = layerData->srcRectangle.w;
    }

    if (right_update)
    {
        dfb_rectangle_from_region(&right_update_rect, right_update);
    }
    else
    {
        right_update_rect.x = layerData->srcRectangle.x;
        right_update_rect.y = layerData->srcRectangle.y;
        right_update_rect.h = layerData->srcRectangle.h;
        right_update_rect.w = layerData->srcRectangle.w;
    }

    D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: surface=%p, flags=0x%08x, "
        "left_handle=%p right_handle=%p\n", __FUNCTION__, layerData->displayId, layerData->layerId,
        (void *)surface, flags, (void *)left_lock->handle, right_lock ? (void *)right_lock->handle : NULL );

    /* Lock access to the layerData */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    res = bcmFlipRegion(dfb_bcmnexus, layer_data, region_data, surface, flags, left_lock, right_lock);
    if (res != DFB_OK)
    {
        D_ERROR("%s() screen %d gfx layer %d: Could NOT flip primary region (res=%d)!\n", __FUNCTION__, displayId, layerId, res);
        /* Unlock access to the layerData */
        pthread_mutex_unlock( &layerData->lock );
        return res;
    }

#if (NUM_DISPLAYS > 1)
    /* Check to see whether we need to use the M2MC to blit from the primary graphics source path
       to the secondary path when we are in mirroring mode. */
    layerDataSec = bcmGetLayerData(GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, SECONDARY_DISPLAY_ID));
    if (layerDataSec != NULL)
    {
        regionData = layerDataSec->regionData;

        if (regionData && (layerDataSec->sourceId == DLSID_SURFACE) && layerDataSec->frameBuffer)
        {
            if (dfb_surface_lock( layerDataSec->frameBuffer )) {
                /* Unlock access to the layerData */
                pthread_mutex_unlock( &layerData->lock );
                return DFB_FUSION;
            }

            float mod_h = 0, mod_w = 0;

            /* Use the M2MC to scale the primary gfx layer H size to the H size of the secondary gfx layer.
               Also use the M2MC to scale the primary gfx layer V size to the V size of the secondary gfx layer
               if we have a vertical scaler in the graphics feeder (GFD), otherwise we need to use the M2MC to
               scale it vertically to the display height (which isn't necessarily the canvas height) */
            rect.x = 0;
            rect.y = 0;
            rect.w = regionData->config.width;
            if (layerDataSec->hasVscaler)
                rect.h = regionData->config.height;
            else
            {
                DFB_Platform_P_GetDisplaySize(SECONDARY_DISPLAY_ID, &displaySize);
                rect.h = MIN(layerDataSec->destRectangle.h, displaySize.h);
            }

            D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: About to stretch blit surface %p to framebuffer %p [%dx%d@%d,%d]\n", __FUNCTION__,
                    displayId, layerId, (void *)surface, (void *)layerDataSec->frameBuffer, rect.w, rect.h, rect.x, rect.y);

#if defined(HUMAX_PLATFORM_BASE)
#else
            if (left_update)
            {
                /* layerData->SrcRectangle is the primary layer, regionData->config is the secondary */
                /* Calculate the difference to reduce output by*/
                mod_h = ((float)regionData->config.height / (float) layerData->srcRectangle.h);
                mod_w = ((float)regionData->config.width / (float) layerData->srcRectangle.w);

                /* left_update_rect is the partial source retangle */
                rect.x = (left_update_rect.x * mod_w) + 0.5;
                rect.y = (left_update_rect.y * mod_h) + 0.5;;
                rect.h = (left_update_rect.h * mod_h) ;
                rect.w = (left_update_rect.w * mod_w) ;
            }
#endif
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE

            surfaceEye = dfb_surface_get_stereo_eye( surface );
            frameBufferEye = dfb_surface_get_stereo_eye( layerDataSec->frameBuffer );

            dfb_surface_set_stereo_eye( surface, DSSE_LEFT );
            dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, DSSE_LEFT );
#if defined(HUMAX_PLATFORM_BASE)
            dfb_gfx_stretch_to( surface, layerDataSec->frameBuffer, &layerData->srcRectangle, &rect, false );
#else
            dfb_gfx_stretch_to( surface, layerDataSec->frameBuffer, &left_update_rect, &rect, false );
#endif
#else
            dfb_gfx_stretch_stereo( surface, DSSE_LEFT, layerDataSec->frameBuffer, DSSE_LEFT, &left_update_rect, &rect, false );
#endif

            if (surface->config.caps & DSCAPS_STEREO)
            {
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                dfb_surface_set_stereo_eye( surface, DSSE_RIGHT );
                dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, DSSE_RIGHT );

#if defined(HUMAX_PLATFORM_BASE)
		dfb_gfx_stretch_to( surface, layerDataSec->frameBuffer, &layerData->srcRectangle, &rect, false );
#else
                dfb_gfx_stretch_to( surface, layerDataSec->frameBuffer, &right_update_rect, &rect, false );
#endif
#else
                dfb_gfx_stretch_stereo( surface, DSSE_RIGHT, layerDataSec->frameBuffer, DSSE_RIGHT, &right_update_rect, &rect, false );
#endif
            }
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
            dfb_surface_set_stereo_eye( surface, surfaceEye );
#endif
            dfb_gfxcard_sync();

#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE

            dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, DSSE_LEFT );
            buffer = dfb_surface_get_buffer( layerDataSec->frameBuffer, CSBR_FRONT );
#else
            buffer = dfb_surface_get_buffer2( layerDataSec->frameBuffer, CSBR_FRONT, DSSE_LEFT );
#endif
            D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
            res = dfb_surface_buffer_lock( buffer, CSAID_GPU, CSAF_WRITE, &leftFrameBufferLock );
            if (res)
            {
                D_ERROR( "%s() screen %d gfx layer %d: Could NOT lock framebuffer! (res=%d)\n", __FUNCTION__, displayId, layerId, res );
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                dfb_surface_set_stereo_eye(layerDataSec->frameBuffer, frameBufferEye);
#endif
                dfb_surface_unlock(layerDataSec->frameBuffer);
                /* Unlock access to the layerData */
                pthread_mutex_unlock( &layerData->lock );
                return res;
            }

            memset( &rightFrameBufferLock, 0, sizeof(rightFrameBufferLock) );
            if (surface->config.caps & DSCAPS_STEREO)
            {
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, DSSE_RIGHT );
                buffer = dfb_surface_get_buffer( layerDataSec->frameBuffer, CSBR_FRONT );
#else
                buffer = dfb_surface_get_buffer2( layerDataSec->frameBuffer, CSBR_FRONT, DSSE_RIGHT );
#endif

                D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer);
                res = dfb_surface_buffer_lock( buffer, CSAID_GPU, CSAF_WRITE, &rightFrameBufferLock );
                if (res)
                {
                    D_ERROR( "%s() screen %d gfx layer %d: Could NOT lock right framebuffer! (res=%d)\n", __FUNCTION__, displayId, layerId, res );
                    dfb_surface_buffer_unlock( &leftFrameBufferLock );
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                    dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, frameBufferEye );
#endif
                    dfb_surface_unlock( layerDataSec->frameBuffer );
                    /* Unlock access to the layerData */
                    pthread_mutex_unlock( &layerData->lock );
                    return res;
                }
            }
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
            dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, frameBufferEye );
#endif

            if (layerDataSec->layerEnabled &&
                ((leftFrameBufferLock.handle != regionData->leftBufferLock.handle) ||
                 (rightFrameBufferLock.handle != regionData->rightBufferLock.handle))) {
                regionData->leftBufferLock = leftFrameBufferLock;
                regionData->rightBufferLock = rightFrameBufferLock;

                bcmSetLayerFrameBuffer( dfb_bcmnexus, SECONDARY_GFX_LAYER_INDEX );
            }

            dfb_surface_buffer_unlock( &leftFrameBufferLock );
            if (surface->config.caps & DSCAPS_STEREO)
            {
                dfb_surface_buffer_unlock( &rightFrameBufferLock );
            }

            if (dfb_surface_unlock( layerDataSec->frameBuffer )) {
                /* Unlock access to the layerData */
                pthread_mutex_unlock( &layerData->lock );
                return DFB_FUSION;
            }
        }
    }
#endif
    /* Unlock access to the layerData */
    pthread_mutex_unlock( &layerData->lock );

    D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: Exit.\n", __FUNCTION__, displayId, layerId );
    return DFB_OK;
}

/* Update the primary region */
static DFBResult
bcmUpdatePrimaryDisplayPrimaryRegion( UNUSED_ CoreLayer             *layer,
                                              void                  *driver_data,
                                              void                  *layer_data,
                                              void                  *region_data,
                                              CoreSurface           *surface,
                                              const DFBRegion       *left_update,
                                              CoreSurfaceBufferLock *left_lock,
                                              const DFBRegion       *right_update,
                                              CoreSurfaceBufferLock *right_lock )
{
    DFBResult             res;
    DFBBCMNEXUS          *dfb_bcmnexus = driver_data;
    bcmLayerData         *layerData    = layer_data;
    DFBDisplayLayerID     layerId;
    DFBScreenID           displayId;
    bool                  primaryUpdated;
    bcmRegionData        *regionData = region_data;
#if (NUM_DISPLAYS > 1)
    bcmLayerData         *layerDataSec;
    DFBRectangle          rect;
    CoreSurfaceBufferLock leftFrameBufferLock, rightFrameBufferLock;
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
    DFBSurfaceStereoEye   surfaceEye, frameBufferEye;
#endif
    CoreSurfaceBuffer    *buffer;
    DFB_PlatformDimension displaySize;
#endif

    layerId   = layerData->layerId;
    displayId = layerData->displayId;

    D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: surf=%p, left_handle=%p [%d,%d->%d,%d], right_handle=%p [%d,%d->%d,%d]\n",
        __FUNCTION__, displayId, layerId, (void *)surface,
        (void *)left_lock->handle, left_update->x1, left_update->y1, left_update->x2, left_update->y2,
        right_lock ? (void *)right_lock->handle : NULL, right_update ? right_update->x1 : 0, right_update ? right_update->y1 : 0,
        right_update ? right_update->x2 : 0, right_update ? right_update->y2 : 0);

    /* Lock access to the layerData */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    if(left_update)
    {
        dfb_rectangle_from_region(&regionData->left_update, left_update);
    }

    if(right_update)
    {
        dfb_rectangle_from_region(&regionData->right_update, right_update);
    }

    /* If update skip count is set to a value greater than 0, it means we won't
       update this layer region as often.  This is useful for reducing memory
       bandwidth and improving graphics performance.
    */
    if (layerData->updateSkipCnt) {
        layerData->updateSkipCnt--;
        primaryUpdated = false;
    } else {
#if BCMNEXUS_NSC_SUPPORT
        res = bcmFlipRegion(dfb_bcmnexus, layer_data, region_data, surface, DSFLIP_NONE, left_lock, right_lock);
#else
        res = bcmInvalidateRegion(dfb_bcmnexus, layer_data, region_data, surface, DSFLIP_NONE, left_lock, right_lock, false);
#endif

        if (res != DFB_OK)
        {
            D_ERROR("%s() screen %d gfx layer %d: Could NOT update primary region (res=%d)!\n", __FUNCTION__, displayId, layerId, res);
            /* Unlock access to the layerData */
            pthread_mutex_unlock( &layerData->lock );
            return res;
        }
        layerData->updateSkipCnt = bcmGetLayerDefaultUpdateSkipCnt(layerData->layerId);
        primaryUpdated = true;
    }

#if (NUM_DISPLAYS > 1)
    /* Check to see whether we need to use the M2MC to blit from the primary graphics source path
       to the secondary path when we are in mirroring mode. */
    layerDataSec = bcmGetLayerData(GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, SECONDARY_DISPLAY_ID));
    if (layerDataSec && primaryUpdated)
    {
        regionData = layerDataSec->regionData;

        if (regionData && (layerDataSec->sourceId == DLSID_SURFACE) && layerDataSec->frameBuffer)
        {
            /* If update skip count is set to a value greater than 0, it means we won't
               update the secondary display with the contents of the primary as often.
               This is useful for reducing memory bandwidth and improving graphics
               performance.
            */
            if (layerDataSec->updateSkipCnt) {
                layerDataSec->updateSkipCnt--;
            } else {
                if (dfb_surface_lock( layerDataSec->frameBuffer )) {
                    /* Unlock access to the layerData */
                    pthread_mutex_unlock( &layerData->lock );
                    return DFB_FUSION;
                }

                /* Use the M2MC to scale the primary gfx layer H size to the H size of the secondary gfx layer.
                   Also use the M2MC to scale the primary gfx layer V size to the V size of the secondary gfx layer
                   if we have a vertical scaler in the graphics feeder (GFD), otherwise we need to use the M2MC to
                   scale it vertically to the display height (which isn't necessarily the canvas height) */
                rect.x = 0;
                rect.y = 0;
                rect.w = regionData->config.width;
                if (layerDataSec->hasVscaler)
                    rect.h = regionData->config.height;
                else
                {
                    DFB_Platform_P_GetDisplaySize(SECONDARY_DISPLAY_ID, &displaySize);
                    rect.h = MIN(layerDataSec->destRectangle.h, displaySize.h);
                }

                D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: About to stretch blit surface %p to framebuffer %p [%dx%d@%d,%d]\n",
                            __FUNCTION__, displayId, layerId, (void *)surface, (void *)layerDataSec->frameBuffer,
                            rect.w, rect.h, rect.x, rect.y);

#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                surfaceEye = dfb_surface_get_stereo_eye( surface );
                frameBufferEye = dfb_surface_get_stereo_eye( layerDataSec->frameBuffer );

                dfb_surface_set_stereo_eye( surface, DSSE_LEFT );
                dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, DSSE_LEFT );
                dfb_gfx_stretch_to( surface, layerDataSec->frameBuffer, &layerData->srcRectangle, &rect, false );
#else
                dfb_gfx_stretch_stereo( surface, DSSE_LEFT, layerDataSec->frameBuffer, DSSE_LEFT, &layerData->srcRectangle, &rect, false );
#endif

                if (surface->config.caps & DSCAPS_STEREO)
                {
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                    dfb_surface_set_stereo_eye( surface, DSSE_RIGHT );
                    dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, DSSE_RIGHT );
                    dfb_gfx_stretch_to( surface, layerDataSec->frameBuffer, &layerData->srcRectangle, &rect, false );
#else
                    dfb_gfx_stretch_stereo( surface, DSSE_RIGHT, layerDataSec->frameBuffer, DSSE_RIGHT, &layerData->srcRectangle, &rect, false );
#endif
                }
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                dfb_surface_set_stereo_eye( surface, surfaceEye );
#endif
                dfb_gfxcard_sync();

#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, DSSE_LEFT );
                buffer = dfb_surface_get_buffer( layerDataSec->frameBuffer, CSBR_FRONT );
#else
                buffer = dfb_surface_get_buffer2( layerDataSec->frameBuffer, CSBR_FRONT, DSSE_LEFT );
#endif
                D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );
                res = dfb_surface_buffer_lock( buffer, CSAID_GPU, CSAF_WRITE, &leftFrameBufferLock );
                if (res)
                {
                    D_ERROR( "%s() screen %d gfx layer %d: Could NOT lock framebuffer! (res=%d)\n", __FUNCTION__, displayId, layerId, res );
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                    dfb_surface_set_stereo_eye(layerDataSec->frameBuffer, frameBufferEye);
#endif
                    dfb_surface_unlock(layerDataSec->frameBuffer);
                    /* Unlock access to the layerData */
                    pthread_mutex_unlock( &layerData->lock );
                    return res;
                }

                memset( &rightFrameBufferLock, 0, sizeof(rightFrameBufferLock) );
                if (surface->config.caps & DSCAPS_STEREO)
                {
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                    dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, DSSE_RIGHT );
                    buffer = dfb_surface_get_buffer( layerDataSec->frameBuffer, CSBR_FRONT );
#else
                    buffer = dfb_surface_get_buffer2( layerDataSec->frameBuffer, CSBR_FRONT, DSSE_RIGHT );
#endif
                    D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer);
                    res = dfb_surface_buffer_lock( buffer, CSAID_GPU, CSAF_WRITE, &rightFrameBufferLock );
                    if (res)
                    {
                        D_ERROR( "%s() screen %d gfx layer %d: Could NOT lock right framebuffer! (res=%d)\n", __FUNCTION__, displayId, layerId, res );
                        dfb_surface_buffer_unlock( &leftFrameBufferLock );
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                        dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, frameBufferEye );
#endif
                        dfb_surface_unlock( layerDataSec->frameBuffer );
                        /* Unlock access to the layerData */
                        pthread_mutex_unlock( &layerData->lock );
                        return res;
                    }
                }
#if BCM_DFB_USE_SURFACE_SET_STEREO_EYE
                dfb_surface_set_stereo_eye( layerDataSec->frameBuffer, frameBufferEye );
#endif

                if (layerDataSec->layerEnabled &&
                    ((leftFrameBufferLock.handle != regionData->leftBufferLock.handle) ||
                     (rightFrameBufferLock.handle != regionData->rightBufferLock.handle))) {
                    regionData->leftBufferLock = leftFrameBufferLock;
                    regionData->rightBufferLock = rightFrameBufferLock;

                    bcmSetLayerFrameBuffer( dfb_bcmnexus, SECONDARY_GFX_LAYER_INDEX );
                }

                dfb_surface_buffer_unlock( &leftFrameBufferLock );
                if (surface->config.caps & DSCAPS_STEREO)
                {
                    dfb_surface_buffer_unlock( &rightFrameBufferLock );
                }

                if (dfb_surface_unlock( layerDataSec->frameBuffer )) {
                    /* Unlock access to the layerData */
                    pthread_mutex_unlock( &layerData->lock );
                    return DFB_FUSION;
                }
                layerDataSec->updateSkipCnt = bcmGetLayerDefaultUpdateSkipCnt(layerDataSec->layerId);
            }
        }
    }
#endif
    /* Unlock access to the layerData */
    pthread_mutex_unlock( &layerData->lock );

    D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: Exit.\n", __FUNCTION__, displayId, layerId );
    return DFB_OK;
}

static DFBResult
bcmInitPrimaryMainLayerSource( UNUSED_ CoreLayer                         *layer,
                               UNUSED_ void                              *driver_data,
                                       void                              *layer_data,
                                       int                                source,
                                       DFBDisplayLayerSourceDescription  *description )
{
    bcmLayerData *layerData = layer_data;

    /* We know that this is called for the Primary Main layer */
    description->source_id = (source == 0) ? layerData->sourceId : (layerData->sourceId + 1);
    snprintf(description->name, DFB_DISPLAY_LAYER_DESC_NAME_LENGTH, "BCMNEXUS Gfx Source %d", description->source_id);
    description->caps = DDLSCAPS_SURFACE;

    return DFB_OK;
}


DisplayLayerFuncs bcmnexusPrimaryGfxLayerFuncs = {
    .LayerDataSize         = bcmLayerDataSize,
    .RegionDataSize        = bcmRegionDataSize,
    .InitLayer             = bcmInitPrimaryDisplayPrimaryLayer,
    .ShutdownLayer         = bcmShutdownLayer,
    .TestRegion            = bcmTestRegion,
    .AddRegion             = bcmAddPrimaryDisplayPrimaryRegion,
    .SetRegion             = bcmSetRegion,
    .RemoveRegion          = bcmRemoveRegion,
    .FlipRegion            = bcmFlipPrimaryDisplayPrimaryRegion,
    .UpdateRegion          = bcmUpdatePrimaryDisplayPrimaryRegion,
    .InitSource            = bcmInitPrimaryMainLayerSource,
    .SetLevel              = bcmSetLevel,
    .GetLevel              = bcmGetLevel,
    .SetStereoDepth        = bcmSetStereoDepth
};


/******************************************************
 * Setup the Secondary Display's Primary Layer/Region *
 ******************************************************/

#if (NUM_DISPLAYS > 1)
/* initialize the secondary graphics layer data
 * this function is called for the master fusionee only
 */
static DFBResult
bcmInitSecondaryDisplayPrimaryLayer(         CoreLayer                  *layer,
                                             void                       *driver_data,
                                             void                       *layer_data,
                                             DFBDisplayLayerDescription *description,
                                             DFBDisplayLayerConfig      *config,
                                     UNUSED_ DFBColorAdjustment         *adjustment )
{
    DFBBCMNEXUS  *dfb_bcmnexus = driver_data;
    bcmLayerData *layerData    = layer_data;

    D_ASSERT( dfb_bcmnexus != NULL );

    layerData->layerId   = DLID_PRIMARY+1;
    layerData->displayId = dfb_screen_id_translated(dfb_layer_screen(layer));
    layerData->sourceId  = DLSID_SURFACE+1;

    layerData->hasVscaler = DFB_Platform_P_CheckGfxVScl(SECONDARY_DISPLAY_ID);

    return bcmInitLayer(dfb_bcmnexus, layerData, description, config);
}

/* Add a region to the secondary display's primary layer */
static DFBResult
bcmAddSecondaryDisplayPrimaryRegion(         CoreLayer             *layer,
                                     UNUSED_ void                  *driver_data,
                                             void                  *layer_data,
                                             void                  *region_data,
                                             CoreLayerRegionConfig *config )
{
    return bcmAddRegion(layer, layer_data, region_data, config, BCMNEXUS_MAX_SD_DISPLAY_WIDTH, BCMNEXUS_MAX_SD_DISPLAY_HEIGHT);
}

/* Flip the secondary region */
static DFBResult
bcmFlipSecondaryDisplayPrimaryRegion( UNUSED_ CoreLayer             *layer,
                                              void                  *driver_data,
                                              void                  *layer_data,
                                              void                  *region_data,
                                              CoreSurface           *surface,
                                              DFBSurfaceFlipFlags    flags,
                                      UNUSED_ const DFBRegion       *left_update,
                                              CoreSurfaceBufferLock *left_lock,
                                      UNUSED_ const DFBRegion       *right_update,
                                              CoreSurfaceBufferLock *right_lock )
{
    DFBResult          res;
    bcmLayerData      *layerData = layer_data;
    DFBDisplayLayerID  layerId;
    DFBScreenID        displayId;

    layerId   = layerData->layerId;
    displayId = layerData->displayId;

    D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: surface=%p, flags=0x%08x, left_handle=%p, right_handle=%p\n", __FUNCTION__,
                displayId, layerId, (void *)surface, flags, (void *)left_lock->handle, right_lock ? (void *)right_lock->handle : NULL );

    /* Lock access to the layerData */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    res = bcmFlipRegion(driver_data, layer_data, region_data, surface, flags, left_lock, right_lock);

    /* Unlock access to the layerData */
    pthread_mutex_unlock( &layerData->lock );

    if (res != DFB_OK) {
        D_ERROR("%s() screen %d gfx layer %d: Could NOT flip secondary region (res=%d)!\n", __FUNCTION__,  displayId, layerId, res);
        return res;
    }

    D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: Exit.\n", __FUNCTION__, displayId, layerId );
    return DFB_OK;
}

/* Update the secondary region */
static DFBResult
bcmUpdateSecondaryDisplayPrimaryRegion( UNUSED_ CoreLayer             *layer,
                                                void                  *driver_data,
                                                void                  *layer_data,
                                                void                  *region_data,
                                                CoreSurface           *surface,
                                                const DFBRegion       *left_update,
                                                CoreSurfaceBufferLock *left_lock,
                                                const DFBRegion       *right_update,
                                                CoreSurfaceBufferLock *right_lock )
{
    DFBResult          res       = DFB_OK;
    bcmLayerData      *layerData = layer_data;
    DFBDisplayLayerID  layerId;
    DFBScreenID        displayId;

    layerId   = layerData->layerId;
    displayId = layerData->displayId;

    D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: surf=%p, left_handle=%p [%d,%d->%d,%d], right_handle=%p [%d,%d->%d,%d]\n",
        __FUNCTION__, displayId, layerId, (void *)surface,
        (void *)left_lock->handle, left_update->x1, left_update->y1, left_update->x2, left_update->y2,
        right_lock ? (void *)right_lock->handle : NULL, right_update ? right_update->x1 : 0, right_update ? right_update->y1 : 0,
        right_update ? right_update->x2 : 0, right_update ? right_update->y2 : 0);

    /* Lock access to the layerData */
    PTHREAD_ROBUST_MUTEX_LOCK( &layerData->lock );

    /* If update skip count is set to a value greater than 0, it means we won't
       update this layer region as often.  This is useful for reducing memory
       bandwidth and improving graphics performance.
    */
    if (layerData->updateSkipCnt) {
        layerData->updateSkipCnt--;
    } else {

        res = bcmInvalidateRegion(driver_data, layer_data, region_data, surface, DSFLIP_NONE, left_lock, right_lock, false);

        if (res != DFB_OK)
            D_ERROR("%s() screen %d gfx layer %d: Could NOT update secondary region (res=%d)!\n", __FUNCTION__, displayId, layerId, res);

        layerData->updateSkipCnt = bcmGetLayerDefaultUpdateSkipCnt(layerData->layerId);
    }
    /* Unlock access to the layerData */
    pthread_mutex_unlock( &layerData->lock );

    D_DEBUG_AT( bcmnexusLayerX, "%s() screen %d gfx layer %d: Exit [%d].\n", __FUNCTION__, displayId, layerId, res );
    return res;
}

static DFBResult
bcmInitSecondaryMainLayerSource( UNUSED_ CoreLayer                         *layer,
                                 UNUSED_ void                              *driver_data,
                                         void                              *layer_data,
                                         int                                source,
                                         DFBDisplayLayerSourceDescription  *description )
{
    bcmLayerData *layerData = layer_data;

    /* We know that this is called for the Secondary Main layer */
    description->source_id = (source == 0) ? layerData->sourceId : (layerData->sourceId - 1);
    snprintf(description->name, DFB_DISPLAY_LAYER_DESC_NAME_LENGTH, "BCMNEXUS Gfx Source %d", description->source_id);
    description->caps = DDLSCAPS_SURFACE;

    return DFB_OK;
}

DisplayLayerFuncs bcmnexusSecondaryGfxLayerFuncs = {
    .LayerDataSize         = bcmLayerDataSize,
    .RegionDataSize        = bcmRegionDataSize,
    .InitLayer             = bcmInitSecondaryDisplayPrimaryLayer,
    .ShutdownLayer         = bcmShutdownLayer,
    .TestRegion            = bcmTestRegion,
    .AddRegion             = bcmAddSecondaryDisplayPrimaryRegion,
    .SetRegion             = bcmSetRegion,
    .RemoveRegion          = bcmRemoveRegion,
    .FlipRegion            = bcmFlipSecondaryDisplayPrimaryRegion,
    .UpdateRegion          = bcmUpdateSecondaryDisplayPrimaryRegion,
    .InitSource            = bcmInitSecondaryMainLayerSource,
    .SetLevel              = bcmSetLevel,
    .GetLevel              = bcmGetLevel,
};
#endif
