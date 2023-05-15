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
 * $brcm_Workfile: bcmnexus_screen.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/12 $
 * $brcm_Date: 3/7/12 5:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus_screen.c $
 * 
 * DirectFB_1_4_15_Port/12   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 * 
 * DirectFB_1_4_15_Port/11   2/9/12 2:33p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 * 
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   12/8/11 3:33p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 * 
 * DirectFB_1_4_15_Port/10   11/30/11 7:49p scchang
 * SW7420-2158: DirectFB: Consider setting pthread mutex robust attribute
 * 
 * DirectFB_1_4_15_Port/9   11/8/11 3:38p christ
 * SW7425-1703: DirectFB: Move NEXUS_Display calls into platform layer
 *
 * DirectFB_1_4_15_Port/7   10/28/11 12:40p scchang
 * SW7420-2112: DirectFB: Cannot change output format of secondary display
 *  to PAL
 *
 * DirectFB_1_4_15_Port/6   10/21/11 3:28p apandav
 * SW7420-2105: DirectFB: Add support for setting encoder aspect ratio
 *
 * DirectFB_1_4_15_Port/5   10/14/11 1:56p christ
 * SW7425-1493: DirectFB: Cycling through stereoscopic display modes
 *  causes VDC error
 *
 * DirectFB_1_4_15_Port/4   10/5/11 12:26p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_14_Port/3   9/23/11 12:46p christ
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file
 *
 * DirectFB_1_4_14_Port/2   8/19/11 5:04p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/10   9/19/11 3:15p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_5_Port/9   9/13/11 4:05p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_5_Port/8   8/16/11 10:02p tross
 * Need to disable secondary layer & set layer dest rect
 * when reconfiguring encoder for proper operation of
 * encoder configuration.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/7   7/13/11 1:43p apandav
 * SWDTV-7885: DirectFB: DirectFB app fails to start due to invalid
 *  Encoder settings on DTV platforms
 *
 * DirectFB_1_4_5_Port/6   7/11/11 5:32p robertwm
 * SW7231-292: DirectFB-1.4.5: Ensure that outputs are not connected to
 *  the display twice.
 *
 * DirectFB_1_4_5_Port/5   7/8/11 5:02p robertwm
 * SW7231-289: DirectFB-1.4.5: Default resolution cannot be changed at
 *  run-time.
 *
 * DirectFB_1_4_5_Port/4   7/7/11 6:46p tross
 * SWBLURAY-26478: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/4   7/1/11 2:30p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/4   7/1/11 2:05p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/3   7/1/11 1:36p robertwm
 * SW7425-814: DirectFB: Cannot build DirectFB-1.4.5 Phase 2.0 candidate
 *  with Phase 2.0 refsw.
 * SW7425-818: DirectFB: Cannot set 3DTV stereoscopic video formats using
 *  "res=" run-time option.
 * SW7425-819: DirectFB: bcmSetEncoderConfig() does not return
 *  DFB_UNSUPPORTED if the video format is unsupported.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/2   6/16/11 5:30p robertwm
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
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/10   4/29/11 1:48p tross
 * Retrieve current display settings before each use to ensure
 * synchronization with external changes.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/9   4/14/11 3:38p tross
 * Change default display output to component.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/8   4/11/11 7:06p tross
 * Merge changes to get STB to compile.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/7   4/7/11 8:18p tross
 * Merge from latest.
 *
 * DirectFB_1_4_Port/31   6/2/11 6:43p christ
 * SW7420-1914: DirectFB: Add support for setting the background color
 *  using the screen mixer API.
 *
 * DirectFB_1_4_Port/30   6/2/11 6:31p christ
 * SW7420-1914: DirectFB: Add support for setting the background color
 *  using the screen mixer API.
 *
 * DirectFB_1_4_Port/29   6/2/11 4:35p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/28   5/18/11 4:22p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/27   5/17/11 6:52p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/26   3/30/11 6:20p christ
 * SW7425-254: DirectFB: Improve performance by replacing linux-fusion
 *  calls with pthreads
 *
 * DirectFB_1_4_Port/25   3/15/11 10:58a robertwm
 * SW3556-1226: DirectFB: Cannot run DFB on 3556 due to VDB errors.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/5   3/28/11 11:03a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/4   3/14/11 4:40p tross
 * Change bcmInitEncoder to only ensure display handle is available
 * and specify encoder config to set, not actually touch the hardware.
 * All hardware configuration is now done in bcmSetEncoderConfig.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   3/14/11 1:24p tross
 * Call to SetEncoderConfig to 720p from df_andi was causing errors
 * in bdvd_gfx_window_set_framebuffer().
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/11/11 3:41p tross
 * Save Bluray screen settings when configuring encoder.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:14p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/24   3/4/11 12:17p christ
 * SW7405-5174: DirectFB: Setting 1080p50/60 modes on SOCs which don't
 *  support these modes crashes Magnum.
 *
 * DirectFB_1_4_Port/23   3/1/11 12:01p christ
 * SW7420-1545: DirectFB: Application hang when using multi app DirectFB
 *  with SawMan window manager.
 *
 * DirectFB_1_4_Port/22   2/18/11 4:30p robertwm
 * SW7468-366: DirectFB: Platforms without component output should not
 *  call component functions.
 *
 * DirectFB_1_4_Port/21   2/8/11 1:25p robertwm
 * SW7420-1467: DirectFB: Multiple apps should NOT share NEXUS Display
 *  handle.
 * SW7420-1469: DirectFB: Should check return code when trying to add an
 *  output to the display.
 *
 * DirectFB_1_4_Port/20   1/31/11 5:43p robertwm
 * SW7420-1448: DirectFB: bcmSetEncoderConfig() should not enable graphics
 *  if layer has not already been initialised.
 *
 * DirectFB_1_4_Port/19   1/28/11 5:20p robertwm
 * SW7420-1443: DirectFB: Add support for changing display/screen
 *  frequency using ScreenEncoder.
 *
 * DirectFB_1_4_Port/18   1/24/11 4:27p christ
 * SW7420-1411: DirectFB: Merge of features required for DirectFB 1.4.5
 *  phase 1.5 release - Add some encoder support
 *
 * DirectFB_1_4_Port/17   8/20/10 3:40p robertwm
 * SW7405-4791: DirectFB: DirectFB-1.4.1 Phase 2.5 fails to build with
 *  Nexus Phase 5.0 reference software.
 *
 * DirectFB_1_4_Port/16   8/11/10 1:59a robertwm
 * SW7405-4250: DirectFB: Screen destination rectangle and source
 *  rectangle not saved across contexts.
 * SW7405-4743: DirectFB: Multiple instances of df_andi.c do not get
 *  displayed.
 *
 * DirectFB_1_4_Port/15   7/26/10 5:32p jackf
 * SW35230-771: Added include for shim_nexus.h.
 *
 * DirectFB_1_4_Port/14   5/5/10 12:49p robertwm
 * SW7405-4295: DirectFB: Reference to NEXUS_NUM_HDMI_OUTPUTS should check
 *  if > 0.
 *
 * DirectFB_1_4_Port/13   4/19/10 4:47p robertwm
 * SW3556-1100: DirectFB: Remove warnings generated when NUM_DISPLAYS is
 *  1.
 *
 * DirectFB_1_4_Port/12   4/9/10 4:54p robertwm
 * SW7405-4180: DirectFB: Require independent setting of graphics
 *  resolution on multiple output paths.
 * SW7405-4184: Allow independent mode and resolution to be specified.
 *
 * DirectFB_1_4_Port/11   2/25/10 11:50a robertwm
 * SW7405-3961: DirectFB: Add support for 576p and 480p video display
 *  modes.
 *
 * DirectFB_1_4_Port/10   1/25/10 10:25a robertwm
 * SW7405-3827: DirectFB: system module not including
 *  bcmnexus_gfx_defines.h.
 *
 * DirectFB_1_4_Port/9   1/18/10 8:11p robertwm
 * SW7420-551: DirectFB: Add support for Screen "SetPowerMode()" API Call.
 *
 * DirectFB_1_4_Port/8   1/4/10 6:00p robertwm
 * SW7420-523: DirectFB: Require 1080p resolution support.
 *
 * DirectFB_1_4_Port/7   12/23/09 2:23p robertwm
 * SW7550-51: Support DirectFB on 7550.
 *
 * DirectFB_1_4_Port/SW7550-51/1   12/22/09 3:22p mphillip
 * SW7550-51: Add 7550 build support (on branch)
 *
 * DirectFB_1_4_Port/6   12/16/09 9:50p robertwm
 * SW7400-2643: DirectFB: Add support for setting/getting video modes.
 *
 * DirectFB_1_4_Port/5   11/16/09 1:27p robertwm
 * SW7405-3399: WaitForSync() function is unimplemented.
 * (Fixed graphics freeze issue).
 *
 * DirectFB_1_4_Port/4   11/13/09 7:10p robertwm
 * SW7405-3399: WaitForSync() function is unimplemented.
 *
 * DirectFB_1_4_Port/3   8/7/09 3:46p robertwm
 * PR56721: Add HDMI output support.
 *
 * DirectFB_1_4_Port/2   7/9/09 7:45p robertwm
 * PR56691: Require ability to set European display output modes (e.g.
 *  576i, 720p/50, 1080i/25).
 *
 * DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
 * PR56658: Add DirectFB 1.4.1 broadcom_files
 *
 * 8   11/21/08 8:06p vsilyaev
 * PR 49524: Added graphics2d_sync routine that could be used by different
 *  users of Nexus_Graphics2D
 *
 * 7   11/3/08 1:57p gmohile
 * PR 45243 : Default all DTV to panel output
 *
 * ***************************************************************************/

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <core/layers.h>
#include <core/layer_control.h>
#include <core/screens_internal.h>
#include <core/system.h>
#include <fusion/shmalloc.h>
#include "bcmnexus_screen.h"
#include "bcmnexus_layer.h"
#include <directfb_strings.h>

#include "bstd.h"
#include <misc/conf.h>

#ifdef __GNUC__
#define UNUSED_ __attribute__ ((unused))
#else
#define UNUSED_
#endif

D_DEBUG_DOMAIN( bcmnexusScreen, "bcmNexus/Screen",   "Broadcom NEXUS Screen driver" );
D_DEBUG_DOMAIN( bcmnexusScreenX, "bcmNexus/ScreenX", "Broadcom NEXUS Screen Xtra debug" );

/* Strings for Screen are defined here */
const DirectFBScreenOutputConnectorsNames  ( dfb_screen_output_connectors_names    )
const DirectFBScreenOutputResolutionNames  ( dfb_screen_output_resolution_names    )
const DirectFBScreenEncoderTVStandardsNames( dfb_screen_encoder_tv_standards_names )
const DirectFBScreenEncoderScanModeNames   ( dfb_screen_encoder_scan_mode_names    )
const DirectFBScreenEncoderFrequencyNames  ( dfb_screen_encoder_frequency_names    )


static int
bcmScreenDataSize( void )
{
    return sizeof(bcmScreenData);
}

#if defined(BCMNEXUS_DISPLAY_HAS_VSYNC_CALLBACK) && !BCMNEXUS_NSC_SUPPORT
static void
bcmVsyncCallback( void *context,
                  int   param )
{
    DFBBCMNEXUS *dfb_bcmnexus = (DFBBCMNEXUS*)context;

    pthread_mutex_t *pLock = &GET_DISPLAY_VSYNC_MUTEX(dfb_bcmnexus, param);
    pthread_cond_t  *pSem =  &GET_DISPLAY_VSYNC_SEM(dfb_bcmnexus, param);

    D_DEBUG_AT( bcmnexusScreenX, "  -> VSync[%d] callback handled\n", param );

    PTHREAD_ROBUST_MUTEX_LOCK( pLock );
    pthread_cond_signal( pSem );
    pthread_mutex_unlock( pLock );

}

static void
bcmInstallVSyncCallback( DFBBCMNEXUS                    *dfb_bcmnexus,
                         DFBScreenID                     screen,
                         DFB_Platform_P_DisplaySettings *pDisplayCfg )
{
    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d\n", __FUNCTION__, screen );

    if (GET_DISPLAY_VSYNC_CB(dfb_bcmnexus, screen) == NULL)
    {
        SET_DISPLAY_VSYNC_CB(dfb_bcmnexus, bcmVsyncCallback, screen);
        pDisplayCfg->vsyncCallback.callback = GET_DISPLAY_VSYNC_CB(dfb_bcmnexus, screen);
        pDisplayCfg->vsyncCallback.context  = dfb_bcmnexus;
        pDisplayCfg->vsyncCallback.param    = screen;
    }
}
#endif

static DFBResult
bcmSetDisplayOutput( UNUSED_ DFBBCMNEXUS                  *dfb_bcmnexus,
                     UNUSED_ DFBScreenID                   screen,
                     UNUSED_ DFB_PlatformDisplayOutputType outputType,
                     UNUSED_ int                           outputIndex,
                     UNUSED_ bool                          enable )
{
    DFBResult               res = DFB_OK;

#if !BCMNEXUS_NSC_SUPPORT
    DFB_PlatformResult      err;
    DFB_PlatformNexusHandle display;

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, output type=%d, output index=%d, enable=%d\n",
                __FUNCTION__, screen, outputType, outputIndex, enable );

    display = GET_DISPLAY_HND(dfb_bcmnexus, screen);

    if (enable)
        err = DFB_Platform_AddDisplayOutput(display, outputType, outputIndex);
    else
        err = DFB_Platform_RemoveDisplayOutput(display, outputType, outputIndex);

    res = (err == DFB_PLATFORM_OK) ? DFB_OK : DFB_FAILURE;

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, output type=%d, output index=%d, enable=%d Exit[%d]\n",
                __FUNCTION__, screen, outputType, outputIndex, enable, res );

#endif
    return res;
}

static DFBResult
setVideoOutput( UNUSED_ DFBBCMNEXUS                   *dfb_bcmnexus,
                UNUSED_ bcmScreenData                 *sd,
                UNUSED_ int                            output,
                UNUSED_ DFB_PlatformVideoFormat        format,
                UNUSED_ DFBScreenEncoderPictureFraming framing,
                UNUSED_ DFBScreenEncoderConfigFlags    aspect_ratio)
{
    DFBResult                      res    = DFB_OK;
    DFBScreenID                    screen = sd->id;
    DFB_PlatformNexusHandle        display;
    DFB_Platform_P_DisplaySettings displaySettings;

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, output=%d, format=%d\n", __FUNCTION__, screen, output, format );

    display = GET_DISPLAY_HND(dfb_bcmnexus, screen);

    DFB_Platform_P_GetDisplaySettings(display, &displaySettings);

#if (BCMNEXUS_VEC_HAS_1080P_60HZ_SUPPORT == 0)

    if (format == DFB_PlatformVideoFormat_e1080p60hz)
    {
        D_ERROR("%s() Unsupported video_format %d - defaulting to 1080i60\n", __FUNCTION__, displaySettings.format);
        format = DFB_PlatformVideoFormat_e1080i;
    }
    else if (format == DFB_PlatformVideoFormat_e1080p50hz)
    {
        D_ERROR("%s() Unsupported video_format %d - defaulting to 1080i50\n", __FUNCTION__, displaySettings.format);
        format = DFB_PlatformVideoFormat_e1080i50hz;
    }
#endif

#if NEXUS_NUM_HDMI_OUTPUTS && NEXUS_NUM_COMPONENT_OUTPUTS
    if (screen == PRIMARY_DISPLAY_ID)
    {
        DFB_PlatformNexusHandle        componentHandle;

        if (DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eComponent, output, &componentHandle) == DFB_PLATFORM_OK)
        {
            /* Disconnect the component output is the video format cannot be handled by the component output... */
            if (componentHandle)
            {
                if (format >= DFB_PlatformVideoFormat_eVesa640x480p60hz)
            {
                res = bcmSetDisplayOutput(dfb_bcmnexus, screen, DFB_PlatformDisplayOutputType_eComponent, output, false);
                    sd->componentDisconnected = true;
                }
            }
            /* Connect the component output is the video format can be handled by the component output and the component should
               be re-connected... */
            else
            {
                if (format < DFB_PlatformVideoFormat_eVesa640x480p60hz && sd->componentDisconnected)
                {
                    res = bcmSetDisplayOutput(dfb_bcmnexus, screen, DFB_PlatformDisplayOutputType_eComponent, output, true);
                    sd->componentDisconnected = false;
                }
            }
        }
    }
#endif

    if (res == DFB_OK)
    {
#if defined(BCMNEXUS_DISPLAY_HAS_VSYNC_CALLBACK) && !BCMNEXUS_NSC_SUPPORT
        bcmInstallVSyncCallback(dfb_bcmnexus, screen, &displaySettings);
#endif
        /* Update the video format here.
           TODO: Add support for setting the aspect ratio too */
        displaySettings.format = format;
        displaySettings.aspectRatio= aspect_ratio;

        switch (framing)
        {
            case DSEPF_STEREO_SIDE_BY_SIDE_HALF:
                displaySettings.stereoMode = DFB_PlatformStereoscopicMode_eSideBySideHalf;
                break;
            case DSEPF_STEREO_TOP_AND_BOTTOM:
                displaySettings.stereoMode = DFB_PlatformStereoscopicMode_eTopBottom;
                break;
            case DSEPF_STEREO_FRAME_PACKING:
                displaySettings.stereoMode = DFB_PlatformStereoscopicMode_eFramePacking;
                break;
            case DSEPF_STEREO_SIDE_BY_SIDE_FULL:
                displaySettings.stereoMode = DFB_PlatformStereoscopicMode_eSideBySideFull;
                break;
            case DSEPF_MONO:
            default:
                displaySettings.stereoMode = DFB_PlatformStereoscopicMode_e2d;
                break;
        }

        res = (DFB_Platform_P_SetDisplaySettings( display, &displaySettings) == DFB_PLATFORM_OK) ? DFB_OK : DFB_FAILURE;

    }
    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, output=%d, format=%d Exit[%d]\n", __FUNCTION__, screen, output, format, res );

    return res;
}

static void
getDisplayOutputCfg( DFB_PlatformNexusHandle  displayHandle,
                     DFBScreenEncoderConfig  *config )
{
    DFB_PlatformNexusHandle outputHandle;

#if !BCMNEXUS_NSC_SUPPORT
    if (!displayHandle)
#endif
        return;

    config->out_connectors  = DSOC_UNKNOWN;

    if ((DFB_Platform_GetDisplayOutput(displayHandle, DFB_PlatformDisplayOutputType_eComposite, 0, &outputHandle) == DFB_PLATFORM_OK) &&
        (outputHandle != NULL))
    {
        config->out_connectors  |= DSOC_CVBS;
        config->flags           |= DSECONF_CONNECTORS;
    }
    if ((DFB_Platform_GetDisplayOutput(displayHandle, DFB_PlatformDisplayOutputType_eSvideo, 0, &outputHandle) == DFB_PLATFORM_OK) &&
        (outputHandle != NULL))
    {
        config->out_connectors  |= DSOC_YC;
        config->flags           |= DSECONF_CONNECTORS;
    }
    if ((DFB_Platform_GetDisplayOutput(displayHandle, DFB_PlatformDisplayOutputType_eScart, 0, &outputHandle) == DFB_PLATFORM_OK) &&
        (outputHandle != NULL))
    {
        config->out_connectors  |= DSOC_SCART;
        config->flags           |= DSECONF_CONNECTORS;
    }
    if ((DFB_Platform_GetDisplayOutput(displayHandle, DFB_PlatformDisplayOutputType_eScart, 1, &outputHandle) == DFB_PLATFORM_OK) &&
        (outputHandle != NULL))
    {
        config->out_connectors  |= DSOC_SCART2;
        config->flags           |= DSECONF_CONNECTORS;
    }
    if ((DFB_Platform_GetDisplayOutput(displayHandle, DFB_PlatformDisplayOutputType_eComponent, 0, &outputHandle) == DFB_PLATFORM_OK) &&
        (outputHandle != NULL))
    {
        config->out_connectors  |= DSOC_COMPONENT;
        config->flags           |= DSECONF_CONNECTORS;
    }
    if ((DFB_Platform_GetDisplayOutput(displayHandle, DFB_PlatformDisplayOutputType_eHdmi, 0, &outputHandle) == DFB_PLATFORM_OK) &&
        (outputHandle != NULL))
    {
        config->out_connectors  |= DSOC_HDMI;
        config->flags           |= DSECONF_CONNECTORS;
    }
}


static void
getDefaultEncoderCfg( DFB_PlatformNexusHandle  displayHandle,
                      DFBScreenEncoderConfig  *config )
{
    D_ASSERT(config != NULL);

    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;

    memset(config, 0, sizeof(DFBScreenEncoderConfig));
    config->flags = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_SCANMODE |
                                                  DSECONF_FREQUENCY | DSECONF_CONNECTORS |
                                                  DSECONF_RESOLUTION | DSECONF_FRAMING | DSECONF_ASPECT_RATIO);
    config->tv_standard     = DSETV_DIGITAL;
    config->framing         = DSEPF_MONO;
    config->aspect_ratio    = DFB_PlatformDisplayAspectRatio_eAuto;

    if (strcmp(brcm_config->res,"480i") == 0) {
        config->tv_standard     = DSETV_NTSC;
        config->resolution      = DSOR_720_480;
        config->frequency       = DSEF_29_97HZ;
        config->scanmode        = DSESM_INTERLACED;
    }
    else if (strcmp(brcm_config->res,"480p") == 0) {
        config->resolution      = DSOR_720_480;
        config->frequency       = DSEF_59_94HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else if (strcmp(brcm_config->res,"576i") == 0) {
        config->tv_standard     = DSETV_PAL;
        config->resolution      = DSOR_720_576;
        config->frequency       = DSEF_25HZ;
        config->scanmode        = DSESM_INTERLACED;
    }
    else if (strcmp(brcm_config->res,"576p") == 0) {
        config->resolution      = DSOR_720_576;
        config->frequency       = DSEF_50HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else if (strcmp(brcm_config->res,"720p") == 0 || strcmp(brcm_config->res,"720p60") == 0) {
        config->resolution      = DSOR_1280_720;
        config->frequency       = DSEF_60HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else if (strcmp(brcm_config->res,"720ps") == 0 || strcmp(brcm_config->res,"720p60s") == 0) {
        config->resolution      = DSOR_1280_720;
        config->frequency       = DSEF_60HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
        config->framing         = DSEPF_STEREO_FRAME_PACKING;
    }
    else if (strcmp(brcm_config->res,"720p50") == 0) {
        config->resolution      = DSOR_1280_720;
        config->frequency       = DSEF_50HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else if (strcmp(brcm_config->res,"720p50s") == 0) {
        config->resolution      = DSOR_1280_720;
        config->frequency       = DSEF_50HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
        config->framing         = DSEPF_STEREO_FRAME_PACKING;
    }
    else if (strcmp(brcm_config->res,"1080i") == 0 || strcmp(brcm_config->res,"1080i30") == 0) {
        config->resolution      = DSOR_1920_1080;
        config->frequency       = DSEF_30HZ;
        config->scanmode        = DSESM_INTERLACED;
    }
    else if(strcmp(brcm_config->res,"1080i25") == 0) {
        config->resolution      = DSOR_1920_1080;
        config->frequency       = DSEF_25HZ;
        config->scanmode        = DSESM_INTERLACED;
    }
    else if (strcmp(brcm_config->res,"1080p") == 0 || strcmp(brcm_config->res,"1080p60") == 0) {
#if BCMNEXUS_VEC_HAS_1080P_60HZ_SUPPORT
        config->resolution      = DSOR_1920_1080;
        config->frequency       = DSEF_60HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
#else
        D_WARN("%s: Cannot support 1080p/60Hz Output.\n", __FUNCTION__);
#endif
    }
    else if (strcmp(brcm_config->res,"1080p50") == 0) {
#if BCMNEXUS_VEC_HAS_1080P_60HZ_SUPPORT
        config->resolution      = DSOR_1920_1080;
        config->frequency       = DSEF_50HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
#else
        D_WARN("%s: Cannot support 1080p/60Hz Output.\n", __FUNCTION__);
#endif
    }
    else if (strcmp(brcm_config->res,"1080p24") == 0) {
        config->resolution      = DSOR_1920_1080;
        config->frequency       = DSEF_24HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else if (strcmp(brcm_config->res,"1080p24s") == 0) {
        config->resolution      = DSOR_1920_1080;
        config->frequency       = DSEF_24HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
        config->framing         = DSEPF_STEREO_FRAME_PACKING;
    }
    else if (strcmp(brcm_config->res,"1080p25") == 0) {
        config->resolution      = DSOR_1920_1080;
        config->frequency       = DSEF_25HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else if (strcmp(brcm_config->res,"1080p30") == 0) {
        config->resolution      = DSOR_1920_1080;
        config->frequency       = DSEF_30HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else if (strcmp(brcm_config->res,"VGA") == 0) {
        config->resolution      = DSOR_640_480;
        config->frequency       = DSEF_60HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else if (strcmp(brcm_config->res,"SVGA") == 0) {
        config->resolution      = DSOR_800_600;
        config->frequency       = DSEF_60HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else if (strcmp(brcm_config->res,"XGA") == 0) {
        config->resolution      = DSOR_1024_768;
        config->frequency       = DSEF_60HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }
    else {
        /* Default is 720p/60Hz */
        config->resolution      = DSOR_1280_720;
        config->frequency       = DSEF_60HZ;
        config->scanmode        = DSESM_PROGRESSIVE;
    }

    getDisplayOutputCfg(displayHandle, config);
}

static void
getEncoderCfgFromDFBDisplayCfg( DFB_PlatformNexusHandle             displayHandle,
                                const DFB_PlatformDisplaySettings  *displayCfg,
                                DFBScreenEncoderConfig             *config )
{
    D_ASSERT(displayCfg != NULL);
    D_ASSERT(config != NULL);

    memset(config, 0, sizeof(DFBScreenEncoderConfig));
    config->flags = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_SCANMODE |
                                                  DSECONF_FREQUENCY | DSECONF_RESOLUTION |
                                                  DSECONF_FRAMING);
    config->framing = DSEPF_MONO;

    switch (displayCfg->format)
    {
    case DFB_PlatformVideoFormat_eNtsc:
        config->tv_standard = DSETV_NTSC;
        config->resolution  = DSOR_720_480;
        config->frequency   = DSEF_29_97HZ;
        config->scanmode    = DSESM_INTERLACED;
        break;

    case DFB_PlatformVideoFormat_eNtscJapan:
        config->tv_standard = DSETV_NTSC_M_JPN;
        config->resolution  = DSOR_720_480;
        config->frequency   = DSEF_29_97HZ;
        config->scanmode    = DSESM_INTERLACED;
        break;

    case DFB_PlatformVideoFormat_ePalG:
    case DFB_PlatformVideoFormat_ePalH:
    case DFB_PlatformVideoFormat_ePalI:
    case DFB_PlatformVideoFormat_ePalK:
        config->tv_standard = DSETV_PAL;
        config->resolution  = DSOR_720_576;
        config->frequency   = DSEF_25HZ;
        config->scanmode    = DSESM_INTERLACED;
        break;

    case DFB_PlatformVideoFormat_ePalM:
        config->tv_standard = DSETV_PAL_M;
        config->resolution  = DSOR_720_480;
        config->frequency   = DSEF_30HZ;
        break;

    case DFB_PlatformVideoFormat_ePalN:
        config->tv_standard = DSETV_PAL_N;
        config->resolution  = DSOR_720_576;
        config->frequency   = DSEF_25HZ;
        break;

    case DFB_PlatformVideoFormat_ePalNc:
        config->tv_standard = DSETV_PAL_NC;
        config->resolution  = DSOR_720_576;
        config->frequency   = DSEF_25HZ;
        break;

    case DFB_PlatformVideoFormat_eSecamL:
        config->tv_standard = DSETV_SECAM;
        config->resolution  = DSOR_720_576;
        config->frequency   = DSEF_25HZ;
        break;

    case DFB_PlatformVideoFormat_e480p:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_720_480;
        config->frequency   = DSEF_60HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e576p:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_720_576;
        config->frequency   = DSEF_50HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e720p24hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1280_720;
        config->frequency   = DSEF_24HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e720p25hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1280_720;
        config->frequency   = DSEF_25HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e720p30hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1280_720;
        config->frequency   = DSEF_30HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e720p50hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1280_720;
        config->frequency   = DSEF_50HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e720p50hz_3DOU_AS:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1280_720;
        config->frequency   = DSEF_50HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        config->framing     = DSEPF_STEREO_FRAME_PACKING;
        break;

    case DFB_PlatformVideoFormat_e720p:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1280_720;
        config->frequency   = DSEF_60HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e720p_3DOU_AS:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1280_720;
        config->frequency   = DSEF_60HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        config->framing     = DSEPF_STEREO_FRAME_PACKING;
        break;

    case DFB_PlatformVideoFormat_e1080i50hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1920_1080;
        config->frequency   = DSEF_50HZ;
        config->scanmode    = DSESM_INTERLACED;
        break;

    case DFB_PlatformVideoFormat_e1080i:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1920_1080;
        config->frequency   = DSEF_60HZ;
        config->scanmode    = DSESM_INTERLACED;
        break;

    case DFB_PlatformVideoFormat_e1080p24hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1920_1080;
        config->frequency   = DSEF_24HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e1080p24hz_3DOU_AS:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1920_1080;
        config->frequency   = DSEF_24HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        config->framing     = DSEPF_STEREO_FRAME_PACKING;
        break;
/* removed ?? CJT
    case DFB_PlatformVideoFormat_1080p25hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1920_1080;
        config->frequency   = DSEF_25HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;
*/
    case DFB_PlatformVideoFormat_e1080p30hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1920_1080;
        config->frequency   = DSEF_30HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e1080p50hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1920_1080;
        config->frequency   = DSEF_50HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_e1080p60hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1920_1080;
        config->frequency   = DSEF_60HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_eVesa640x480p60hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_640_480;
        config->frequency   = DSEF_60HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_eVesa800x600p60hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_800_600;
        config->frequency   = DSEF_60HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    case DFB_PlatformVideoFormat_eVesa1024x768p60hz:
        config->tv_standard = DSETV_DIGITAL;
        config->resolution  = DSOR_1024_768;
        config->frequency   = DSEF_60HZ;
        config->scanmode    = DSESM_PROGRESSIVE;
        break;

    default:
        D_ERROR("%s() Unknown video_format enumeration, %d.\n", __FUNCTION__, displayCfg->format);
        config->tv_standard     = DSETV_UNKNOWN;
        config->resolution      = DSOR_UNKNOWN;
        config->frequency       = DSEF_UNKNOWN;
        config->scanmode        = DSESM_UNKNOWN;
        config->flags           = DSECONF_NONE;
        config->framing         = DSEPF_UNKNOWN;
        break;
    }

    getDisplayOutputCfg(displayHandle, config);
}

static DFBResult
getDFBDisplayCfgFromEncoderCfg( const DFBScreenEncoderConfig   *config,
                                DFB_PlatformDisplaySettings    *displayCfg,
                                DFBScreenEncoderConfigFlags    *failFlags )
{
    D_ASSERT(displayCfg != NULL);
    D_ASSERT(config != NULL);

    memset(displayCfg, 0, sizeof(displayCfg));

    if (config->flags & DSECONF_TV_STANDARD)
    {
        switch (config->tv_standard)
        {
            case DSETV_NTSC:
                displayCfg->format = DFB_PlatformVideoFormat_eNtsc;
                displayCfg->aspectRatio = DFB_PlatformDisplayAspectRatio_e4x3;
                break;

            case DSETV_NTSC_M_JPN:
                displayCfg->format = DFB_PlatformVideoFormat_eNtscJapan;
                displayCfg->aspectRatio = DFB_PlatformDisplayAspectRatio_e4x3;
                break;

            case DSETV_PAL:
                displayCfg->format = DFB_PlatformVideoFormat_ePalG;
                displayCfg->aspectRatio = DFB_PlatformDisplayAspectRatio_e4x3;
                break;

            case DSETV_PAL_M:
                displayCfg->format = DFB_PlatformVideoFormat_ePalM;
                displayCfg->aspectRatio = DFB_PlatformDisplayAspectRatio_e4x3;
                break;

            case DSETV_PAL_N:
                displayCfg->format = DFB_PlatformVideoFormat_ePalN;
                displayCfg->aspectRatio = DFB_PlatformDisplayAspectRatio_e4x3;
                break;

            case DSETV_PAL_NC:
                displayCfg->format = DFB_PlatformVideoFormat_ePalNc;
                displayCfg->aspectRatio = DFB_PlatformDisplayAspectRatio_e4x3;
                break;

            case DSETV_SECAM:
                displayCfg->format = DFB_PlatformVideoFormat_eSecamL;
                displayCfg->aspectRatio = DFB_PlatformDisplayAspectRatio_e4x3;
                break;

            case DSETV_DIGITAL:
                if (!(config->flags & DSECONF_RESOLUTION) ||
                    !(config->flags & DSECONF_FREQUENCY) ||
                    !(config->flags & DSECONF_SCANMODE))
                {
                    D_ERROR("%s() Must specify resolution, frequency, and scanmode for DSETV_DIGITAL "
                            "TV standard\n", __FUNCTION__);
                    *failFlags = (DFBScreenEncoderConfigFlags)(DSECONF_RESOLUTION | DSECONF_FREQUENCY |
                                                               DSECONF_SCANMODE | DSECONF_TV_STANDARD);
                    return DFB_INVARG;
                }

                displayCfg->format = -1;
                displayCfg->aspectRatio = DFB_PlatformDisplayAspectRatio_e16x9;

                switch (config->frequency)
                {
                    case DSEF_23_976HZ:
                    case DSEF_24HZ:
                        if (config->scanmode == DSESM_PROGRESSIVE)
                        {
                            if (config->resolution == DSOR_1280_720)
                                displayCfg->format = DFB_PlatformVideoFormat_e720p24hz;
                            else if (config->resolution == DSOR_1920_1080)
                            {
                                if (config->framing == DSEPF_STEREO_FRAME_PACKING)
                                    displayCfg->format = DFB_PlatformVideoFormat_e1080p24hz_3DOU_AS;
                                else
                                    displayCfg->format = DFB_PlatformVideoFormat_e1080p24hz;
                            }
                        }
                        break;

                    case DSEF_25HZ:
                        if (config->scanmode == DSESM_PROGRESSIVE)
                        {
                            if (config->resolution == DSOR_1280_720)
                                displayCfg->format = DFB_PlatformVideoFormat_e720p25hz;
                            else if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080p25hz;
                        }
                        else
                        {
                            if (config->resolution == DSOR_720_576)
                                displayCfg->format = DFB_PlatformVideoFormat_ePalG;
                            else if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080i50hz;
                        }
                        break;

                    case DSEF_29_97HZ:
                        if (config->scanmode == DSESM_PROGRESSIVE)
                        {
                            if (config->resolution == DSOR_1280_720)
                                displayCfg->format = DFB_PlatformVideoFormat_e720p30hz;
                            else if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080p30hz;
                        }
                        else
                        {
                            if (config->resolution == DSOR_720_480)
                                displayCfg->format = DFB_PlatformVideoFormat_eNtsc;
                            else if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080i;
                        }
                        break;

                    case DSEF_30HZ:
                        if (config->scanmode == DSESM_PROGRESSIVE)
                        {
                            if (config->resolution == DSOR_1280_720)
                                displayCfg->format = DFB_PlatformVideoFormat_e720p30hz;
                            else if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080p30hz;
                        }
                        else
                        {
                            if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080i;
                        }
                        break;

                    case DSEF_50HZ:
                        if (config->scanmode == DSESM_PROGRESSIVE)
                        {
                            if (config->resolution == DSOR_720_576)
                                displayCfg->format = DFB_PlatformVideoFormat_e576p;
                            else if (config->resolution == DSOR_1280_720)
                                if (config->framing == DSEPF_STEREO_FRAME_PACKING)
                                    displayCfg->format = DFB_PlatformVideoFormat_e720p50hz_3DOU_AS;
                                else
                                    displayCfg->format = DFB_PlatformVideoFormat_e720p50hz;
                            else if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080p50hz;
                        }
                        else
                        {
                            if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080i50hz;
                            else if (config->resolution == DSOR_720_576)
                                displayCfg->format = DFB_PlatformVideoFormat_ePalI;
                        }
                        break;

                    case DSEF_59_94HZ:
                        if (config->scanmode == DSESM_PROGRESSIVE)
                        {
                            if (config->resolution == DSOR_720_480)
                                displayCfg->format = DFB_PlatformVideoFormat_e480p;
                            else if (config->resolution == DSOR_1280_720)
                                if (config->framing == DSEPF_STEREO_FRAME_PACKING)
                                    displayCfg->format = DFB_PlatformVideoFormat_e720p_3DOU_AS;
                                else
                                    displayCfg->format = DFB_PlatformVideoFormat_e720p;
                            else if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080p60hz;
                        }
                        break;

                    case DSEF_60HZ:
                        if (config->scanmode == DSESM_PROGRESSIVE)
                        {
                            if (config->resolution == DSOR_640_480)
                                displayCfg->format = DFB_PlatformVideoFormat_eVesa640x480p60hz;
                            else if (config->resolution == DSOR_720_480)
                                displayCfg->format = DFB_PlatformVideoFormat_e480p;
                            else if (config->resolution == DSOR_800_600)
                                displayCfg->format = DFB_PlatformVideoFormat_eVesa800x600p60hz;
                            else if (config->resolution == DSOR_1280_720)
                            {
                                if (config->framing == DSEPF_STEREO_FRAME_PACKING)
                                    displayCfg->format = DFB_PlatformVideoFormat_e720p_3DOU_AS;
                                else
                                    displayCfg->format = DFB_PlatformVideoFormat_e720p;
                            }
                            else if (config->resolution == DSOR_1024_768)
                                displayCfg->format = DFB_PlatformVideoFormat_eVesa1024x768p60hz;
                            else if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080p60hz;

                        }
                        else
                        {
                            if (config->resolution == DSOR_1920_1080)
                                displayCfg->format = DFB_PlatformVideoFormat_e1080i;
                            else if (config->resolution == DSOR_720_480)
                                displayCfg->format = DFB_PlatformVideoFormat_eNtsc;
                        }
                        break;

                    default:
                        displayCfg->format = DFB_PlatformVideoFormat_eUnknown;
                        break;
                }

                if (displayCfg->format == DFB_PlatformVideoFormat_eUnknown)
                {
                    D_ERROR("%s() Unsupported resolution/frequency/scanmode specified %d/%d/%d.\n",
                            __FUNCTION__, config->resolution, config->frequency, config->scanmode);
                    *failFlags = (DFBScreenEncoderConfigFlags)(DSECONF_RESOLUTION | DSECONF_FREQUENCY |
                                                               DSECONF_SCANMODE | DSECONF_TV_STANDARD);
                    return DFB_UNSUPPORTED;
                }
                break;

            default:
                D_ERROR("%s() Unsupported TV standard specified %d.\n", __FUNCTION__, config->tv_standard);
                *failFlags = DSECONF_TV_STANDARD;
                return DFB_UNSUPPORTED;
        }
    }
    else
    {
        if (config->flags & DSECONF_ASPECT_RATIO)
        {
            displayCfg->aspectRatio = (DFB_PlatformDisplayAspectRatio)(config->aspect_ratio);
        }
    }

    if (config->flags & DSECONF_CONNECTORS)
    {
        if (config->out_connectors & ~(DSOC_YC | DSOC_CVBS | DSOC_COMPONENT | DSOC_HDMI | DSOC_SCART | DSOC_SCART2))
        {
            D_ERROR("%s() Unsupported output connector.\n", __FUNCTION__);
            *failFlags = DSECONF_CONNECTORS;
            return DFB_FAILURE;
        }

        if ((config->out_connectors & DSOC_CVBS) && (config->resolution > DSOR_800_600))
        {
            D_ERROR("%s() Can't connect such a high resolution to CVBS, res %x\n", __FUNCTION__, config->resolution);
            *failFlags = DSECONF_CONNECTORS;
            return DFB_UNSUPPORTED;
        }
    }

    return DFB_OK;
}

static DFBResult
primaryInitScreen( UNUSED_ CoreScreen         *screen,
                   UNUSED_ CoreGraphicsDevice *device,
                   void                       *driver_data,
                   void                       *screen_data,
                   DFBScreenDescription       *description )
{
    DFBBCMNEXUS                *dfb_bcmnexus = driver_data;
    bcmScreenData              *sd           = screen_data;
    DFB_PlatformDisplaySettings displaySettings;
    DFBScreenEncoderConfig      encoderCfg;
    DFBScreenEncoderConfigFlags failFlags;

    D_DEBUG_AT( bcmnexusScreen, "%s()\n", __FUNCTION__ );

    D_ASSERT( dfb_bcmnexus != NULL );

    /* Screen ID = 0 => Primary output ON */
    sd->id = PRIMARY_DISPLAY_ID;
    sd->powerMode = DSPM_ON;

    /* Set the screen capabilities. */
    description->caps       = DSCCAPS_ENCODERS;
    description->encoders   = 1;

    description->caps |= DSCCAPS_VSYNC | DSCCAPS_MIXERS;
    description->mixers  = 1;

    /* Set the screen name. */
    snprintf( description->name, DFB_SCREEN_DESC_NAME_LENGTH, "BCMNEXUS Primary Screen" );

    D_DEBUG_AT( bcmnexusScreen, "%s(): display0 handle=%p\n", __FUNCTION__, (void *)GET_DISPLAY_HND(dfb_bcmnexus, PRIMARY_DISPLAY_ID));

    getDefaultEncoderCfg(GET_DISPLAY_HND(dfb_bcmnexus, sd->id), &encoderCfg);
    getDFBDisplayCfgFromEncoderCfg(&encoderCfg, &displaySettings, &failFlags);
#if BCMNEXUS_NSC_SUPPORT
    sd->fixedResolution = true;
#else
    sd->fixedResolution = false;
#endif

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, format=%d Exit\n", __FUNCTION__, sd->id, displaySettings.format );

    return DFB_OK;
}

static DFBResult
bcmInitEncoder ( UNUSED_ CoreScreen                   *screen,
                         void                         *driver_data,
                         void                         *screen_data,
                         int                           encoder,
                         DFBScreenEncoderDescription  *description,
                         DFBScreenEncoderConfig       *config )
{
    DFBResult                       res = DFB_OK;
    DFBBCMNEXUS                    *dfb_bcmnexus = driver_data;
    bcmScreenData                  *sd  = screen_data;
    DFB_PlatformNexusHandle         display;
    DFB_PlatformDisplaySettings     displayCfg;
    DFBBCMNEXUS                 *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options            *brcm_config = &pBrcmDfb->options;

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, encoder=%d, flags=0x%08x\n", __FUNCTION__, sd->id, encoder, config->flags );

    if (encoder > 0)
    {
        D_ERROR("%s() Unsupported encoder.\n", __FUNCTION__);
        return DFB_UNSUPPORTED;
    }

    /* Set the encoder capabilities. */
    description->caps           = (DFBScreenEncoderCapabilities)(DSECAPS_TV_STANDARDS | DSECAPS_SCANMODE   |
                                                                 DSECAPS_FREQUENCY    | DSECAPS_CONNECTORS |
                                                                 DSECAPS_RESOLUTION   | DSECAPS_FRAMING | DSECAPS_ASPECT_RATIO);
    description->type           = (DFBScreenEncoderType)(DSET_TV | DSET_DIGITAL);
    description->tv_standards   = (DFBScreenEncoderTVStandards)(DSETV_NTSC  | DSETV_NTSC_M_JPN | DSETV_PAL    |
                                                                DSETV_PAL_M | DSETV_PAL_N      | DSETV_PAL_NC |
                                                                DSETV_SECAM | DSETV_DIGITAL);
    description->all_connectors = (DFBScreenOutputConnectors)(
#if NEXUS_NUM_COMPOSITE_OUTPUTS
                                    DSOC_CVBS      |
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
                                    DSOC_COMPONENT |
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
                                    DSOC_HDMI      |
#endif
#if NEXUS_NUM_SCART_INPUTS
                                    DSOC_SCART     |
#endif
#if NEXUS_NUM_SCART_INPUTS > 1
                                    DSOC_SCART2    |
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS
                                    DSOC_YC        |
#endif
                                    DSOC_UNKNOWN);

    description->all_resolutions= (DFBScreenOutputResolution)(DSOR_640_480  | DSOR_720_480  | DSOR_720_576   | DSOR_800_600 |
                                                              DSOR_1024_768 | DSOR_1280_720 | DSOR_1920_1080);

#if BCMNEXUS_VDC_SUPPORT_3D_VIDEO && BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
    description->all_framing    = DSEPF_ALL;
#else
    description->all_framing    = DSEPF_MONO | DSEPF_STEREO_SIDE_BY_SIDE_HALF | DSEPF_STEREO_TOP_AND_BOTTOM;
#endif
    description->all_aspect_ratio= DFB_ASPECT_RATIO_eAuto | DFB_ASPECT_RATIO_e4x3 | DFB_ASPECT_RATIO_e16x9;
    snprintf( description->name, DFB_SCREEN_ENCODER_DESC_NAME_LENGTH, "BCMNEXUS Pri. Encoder" );

    /* Set configuration. */
    display = GET_DISPLAY_HND(dfb_bcmnexus, sd->id);
#if BCMNEXUS_NSC_SUPPORT
    if (0)
#else
    if (!display)
#endif
    {
        DFB_PlatformDisplayInitSettings displayInitSettings;

        D_DEBUG_AT( bcmnexusScreen, "%s() Using default encoder configuration\n", __FUNCTION__ );

        /* Display hasn't been configured yet, so specify default configuration. */
        DFB_Platform_P_Display_GetDefaultInitSettings(sd->id, &displayInitSettings);

        res = (DFB_Platform_P_Display_Init( sd->id, &displayInitSettings, &display) == DFB_PLATFORM_OK) ? DFB_OK : DFB_FAILURE;
        if (res != DFB_OK)
        {
            D_ERROR("%s{} Error opening primary display%d!\n", __FUNCTION__, sd->id);
            return DFB_FAILURE;
        }
        SET_DISPLAY_HND(dfb_bcmnexus, display, sd->id);

        /* Get the default format (or overide if specified). */
        getDefaultEncoderCfg(display, config);
    }
    else if (strcmp(brcm_config->res, " "))
    {
        /* Get the default format (or overide if specified). */
        if (sd->id == 0)
        {
            D_DEBUG_AT( bcmnexusScreen, "%s() Using encoder configuration set externally by DirectFB \"res=%s\" option.\n", __FUNCTION__, brcm_config->res );
            getDefaultEncoderCfg(display, config);
        }
        else
        {

            /*dfb_config->res doesn't reflect how the secondary display was setup.*/
            DFB_Platform_GetDisplaySettings(display, &displayCfg);

            D_DEBUG_AT( bcmnexusScreen, "%s() Using default configuration for screen %d (format=%d).\n", __FUNCTION__, sd->id, displayCfg.format );

            getEncoderCfgFromDFBDisplayCfg(display, &displayCfg, config);
            description->caps           = (DFBScreenEncoderCapabilities) DSECAPS_CONNECTORS;
        }
    }
    else
    {
        DFB_Platform_GetDisplaySettings(display, &displayCfg);

        D_DEBUG_AT( bcmnexusScreen, "%s() Using encoder configuration set externally by application (format=%d).\n", __FUNCTION__, displayCfg.format );

        getEncoderCfgFromDFBDisplayCfg(display, &displayCfg, config);
    }

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d defaults: outputs=0x%08x, std=%s, res=%s, freq=%s, scan=%s\n", __FUNCTION__, sd->id,
                config->out_connectors,
                dfb_screen_encoder_tv_standards_names[D_BITn32(config->tv_standard)].name,
                dfb_screen_output_resolution_names[D_BITn32(config->resolution)].name,
                dfb_screen_encoder_frequency_names[D_BITn32(config->frequency)].name,
                dfb_screen_encoder_scan_mode_names[D_BITn32(config->scanmode)].name
              );

    return res;
}

static DFBResult
bcmTestEncoderConfig( UNUSED_ CoreScreen                   *screen,
                      UNUSED_ void                         *driver_data,
                      UNUSED_ void                         *screen_data,
                              int                           encoder,
                              const DFBScreenEncoderConfig *config,
                              DFBScreenEncoderConfigFlags  *failed )
{
    DFBResult                   rc  = DFB_OK;
    DFB_PlatformDisplaySettings displayCfg;

    D_DEBUG_AT( bcmnexusScreen, "%s()\n", __FUNCTION__ );

    D_ASSERT((DFBBCMNEXUS*)driver_data != NULL);
    D_ASSERT((bcmScreenData*)screen_data != NULL);
    D_ASSERT(encoder == 0);
    D_ASSERT(config != NULL);
    D_ASSERT(failed != NULL);

    if (encoder != 0)
    {
        D_ERROR("%s() Invalid encoder ID %d\n", __FUNCTION__, encoder);
        return DFB_UNSUPPORTED;
    }

    *failed = DSECONF_NONE;
    rc = getDFBDisplayCfgFromEncoderCfg(config, &displayCfg, failed);
    if (rc != DFB_OK)
    {
        D_ERROR("%s() Couldn't determine DFB display config from desired encoder config\n", __FUNCTION__);
    }

    return rc;
}

static DFBResult
bcmSetEncoderConfig ( UNUSED_ CoreScreen                   *screen,
                              void                         *driver_data,
                              void                         *screen_data,
                              int                           encoder,
                              const DFBScreenEncoderConfig *config )
{
    DFBResult                     rc = DFB_OK;
    DFBBCMNEXUS                  *dfb_bcmnexus = driver_data;
    bcmScreenData                *sd  = screen_data;
    bcmLayerData                 *layerData = NULL;
    DFB_PlatformNexusHandle       display;
    DFB_PlatformDisplaySettings   displayCfg;
    DFB_PlatformNexusHandle       outputHandle;
    DFBScreenEncoderConfigFlags   failFlags;
    DFB_PlatformDimension         displaySize;
    bool                          primaryDisabled;

    D_DEBUG_AT( bcmnexusScreen, "%s()\n", __FUNCTION__ );

    D_ASSERT(dfb_bcmnexus != NULL);
    D_ASSERT(sd != NULL);
    D_ASSERT(encoder == 0);
    D_ASSERT(config != NULL);

    if (encoder != 0)
    {
        D_ERROR("%s() Invalid encoder ID %d\n", __FUNCTION__, encoder);
        return DFB_UNSUPPORTED;
    }

    if (!(config->flags & bcmnexus_sys_SUPPORTED_SCREEN_ENCODER_FLAGS))
    {
        D_ERROR("%s() Unsupported Option!\n", __FUNCTION__);
        return DFB_UNSUPPORTED;
    }

    if (!memcmp(config, &sd->encoderCfg, sizeof(DFBScreenEncoderConfig)))
    {
        return DFB_OK;
    }

    display = GET_DISPLAY_HND(dfb_bcmnexus, sd->id);

    rc = getDFBDisplayCfgFromEncoderCfg(config, &displayCfg, &failFlags);
    if (rc != DFB_OK)
    {
        D_ERROR("%s() Couldn't determine DFB display config from desired encoder config\n", __FUNCTION__);
        return rc;
    }

#if BCMNEXUS_NSC_SUPPORT
    if (0)
#else
    if (config->flags & DSECONF_CONNECTORS)
#endif
    {
        if (config->out_connectors & ~(DSOC_YC | DSOC_CVBS | DSOC_COMPONENT | DSOC_HDMI | DSOC_SCART | DSOC_SCART2))
        {
            D_ERROR("%s() Cannot support setting output connector(s) 0x%08x for display%d!\n", __FUNCTION__, config->out_connectors, sd->id);
            return DFB_UNSUPPORTED;
        }

        /* Composite output */
        if (config->out_connectors & DSOC_CVBS)
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eComposite, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle == NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eComposite, 0, true);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't add composite%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }
        else
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eComposite, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle != NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eComposite, 0, false);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't remove composite%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }
        /* Scart output */
        if (config->out_connectors & DSOC_SCART)
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eScart, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle == NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eScart, 0, true);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't add scart%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }
        else
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eScart, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle != NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eScart, 0, false);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't remove scart%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }

        /* Second Scart output */
        if (config->out_connectors & DSOC_SCART2)
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eScart, 1, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle == NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eScart, 1, true);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't add scart%d output to display%d!\n", __FUNCTION__, 1, sd->id);
                    return rc;
                }
            }
        }
        else
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eScart, 1, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle != NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eScart, 1, false);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't remove scart%d output to display%d!\n", __FUNCTION__, 1, sd->id);
                    return rc;
                }
            }
        }

        /* Svideo output */
        if (config->out_connectors & DSOC_YC)
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eSvideo, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle == NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eSvideo, 0, true);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't add svideo%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }
        else
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eSvideo, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle != NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eSvideo, 0, false);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't remove svideo%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }

        /* Component output */
        if (config->out_connectors & DSOC_COMPONENT)
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eComponent, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle == NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eComponent, 0, true);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't add component%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }
        else
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eComponent, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle != NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eComponent, 0, false);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't remove component%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }
#if !defined(HUMAX_PLATFORM_BASE)		
        /* HDMI output */
        if (config->out_connectors & DSOC_HDMI)
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eHdmi, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle == NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eHdmi, 0, true);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't add hdmi%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }
        else
        {
            if ((DFB_Platform_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eHdmi, 0, &outputHandle) == DFB_PLATFORM_OK) &&
                (outputHandle != NULL))
            {
                rc = bcmSetDisplayOutput( dfb_bcmnexus, sd->id, DFB_PlatformDisplayOutputType_eHdmi, 0, false);
                if (rc != DFB_OK)
                {
                    D_ERROR("%s() Couldn't remove hdmi%d output to display%d!\n", __FUNCTION__, 0, sd->id);
                    return rc;
                }
            }
        }
#endif
    }

    if (rc != DFB_OK)
    {
        D_ERROR("%s() Couldn't determine DFB display config from desired encoder config\n", __FUNCTION__);
        return rc;
    }

    /*
     * We need to have a nasty little test here ...
     * If the region hasn't been realised yet there won't be any region data available inside the layer data
     * which will cause the layer calls below to barf.
     */

    layerData = bcmGetLayerData(GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, sd->id));

    if ( layerData == NULL || layerData->regionData == NULL)
        primaryDisabled = true;
    else
        primaryDisabled = false;

    /* Disable graphics first to help prevent unwanted artefacts */
    if (!primaryDisabled)
        rc = bcmSetLayerState(dfb_bcmnexus, GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, sd->id), false);

    if (rc == DFB_OK)
    {
        usleep(20*1000);    /* Wait 20ms */

        rc = setVideoOutput(dfb_bcmnexus, sd, 0, displayCfg.format, config->framing, config->aspect_ratio);

        if (rc == DFB_OK)
        {
            /* Save new settings */
            sd->encoderCfg = *config;
        }

        if (!primaryDisabled)
        {
            DFBRectangle       destRectangle;

            DFB_Platform_P_GetDisplaySize(sd->id, &displaySize);

            destRectangle.x = 0;
            destRectangle.y = 0;
            destRectangle.w = displaySize.w;
            destRectangle.h = displaySize.h;

            D_DEBUG_AT( bcmnexusScreen, "%s Setting display rect (%d,%d) %d x %d\n", __FUNCTION__,
                        destRectangle.x,destRectangle.y,destRectangle.w,destRectangle.h );

            /* Now set the new destination rectangle and framebuffer */
            rc = bcmSetLayerDestRectangle(dfb_bcmnexus, GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, sd->id), &destRectangle);
        }
    }

    usleep(20*1000);    /* Wait 20ms */

    /* Now re-enable graphics */
    if (!primaryDisabled)
        bcmSetLayerState(dfb_bcmnexus, GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, sd->id), true);

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, format=%d aspect_ratio=%d Exit [%d]\n", __FUNCTION__, sd->id, displayCfg.format, displayCfg.aspectRatio, rc );

    return rc;
}

DFBResult bcmInitMixer( UNUSED_ CoreScreen                *screen,
                        UNUSED_ void                      *driver_data,
                                void                      *screen_data,
                                int                        mixer,
                                DFBScreenMixerDescription *description,
                                DFBScreenMixerConfig      *config )
{
    DFBResult                result       = DFB_OK;

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, mixer=%d, flags=0x%08x\n", __FUNCTION__, ((bcmScreenData*)screen_data)->id, mixer, config->flags );

    if (mixer > 0)
    {
        D_ERROR("%s() Unsupported mixer.\n", __FUNCTION__);
        return DFB_UNSUPPORTED;
    }

    /* Set the mixer capabilities. */
    description->caps = (DFBScreenMixerCapabilities)(DSMCAPS_BACKGROUND);

    snprintf( description->name, DFB_SCREEN_MIXER_DESC_NAME_LENGTH, "BCMNEXUS Pri. Mixer" );

    config->flags = DSMCONF_BACKGROUND;
    config->background.a = 0;
    config->background.r = 0;
    config->background.g = 0;
    config->background.b = 0;

    return result;
}

DFBResult bcmTestMixerConfig( UNUSED_ CoreScreen                 *screen,
                              UNUSED_ void                       *driver_data,
                              UNUSED_ void                       *screen_data,
                                      int                         mixer,
                                      const DFBScreenMixerConfig *config,
                                      DFBScreenMixerConfigFlags  *failed )
{
    D_DEBUG_AT( bcmnexusScreen, "%s()\n", __FUNCTION__ );

    *failed = DSECONF_NONE;

    if (mixer > 0)
    {
        D_ERROR("%s() Unsupported mixer.\n", __FUNCTION__);
        return DFB_UNSUPPORTED;
    }

    if (config->flags | (DSMCONF_TREE & DSMCONF_LEVEL & DSMCONF_LAYERS) )
    {
        *failed = ( config->flags | (DSMCONF_TREE & DSMCONF_LEVEL & DSMCONF_LAYERS) );
        return DFB_UNSUPPORTED;
    }

    if (config->flags & DSMCONF_BACKGROUND)
    {
        /* check the background colour is valid ? */
    }


    return DFB_OK;
}

/*
 * Set new configuration.
 */
DFBResult bcmSetMixerConfig( UNUSED_ CoreScreen                 *screen,
                             UNUSED_ void                       *driver_data,
                             UNUSED_ void                       *screen_data,
                                     int                         mixer,
                             UNUSED_ const DFBScreenMixerConfig *config )
{
    DFBResult               result = DFB_UNSUPPORTED;
#if !BCMNEXUS_NSC_SUPPORT
    DFB_PlatformNexusHandle        display;
    DFB_Platform_P_DisplaySettings displaySettings;
    DFBBCMNEXUS         *dfb_bcmnexus = driver_data;
    bcmScreenData       *sd           = screen_data;
#endif
    D_DEBUG_AT( bcmnexusScreen, "%s()\n", __FUNCTION__ );

    if (mixer > 0)
    {
        D_ERROR("%s() Unsupported mixer.\n", __FUNCTION__);
        return DFB_UNSUPPORTED;
    }

#if !BCMNEXUS_NSC_SUPPORT
    if (config->flags & DSMCONF_BACKGROUND)
    {
        display = GET_DISPLAY_HND(dfb_bcmnexus, sd->id);

        DFB_Platform_P_GetDisplaySettings(display, &displaySettings);

        displaySettings.background.a = config->background.a;
        displaySettings.background.r = config->background.r;
        displaySettings.background.g = config->background.g;
        displaySettings.background.b = config->background.b;

        result = (DFB_Platform_P_SetDisplaySettings( display, &displaySettings) == DFB_PLATFORM_OK) ? DFB_OK : DFB_FAILURE;
    }
#endif

    return result;
}

DFBResult bcmGetMixerState( UNUSED_ CoreScreen     *screen,
                            UNUSED_ void           *driver_data,
                            UNUSED_ void           *screen_data,
                            UNUSED_ int             mixer,
                            UNUSED_ CoreMixerState *ret_state )
{
    DFBResult                result = DFB_UNSUPPORTED;

    D_DEBUG_AT( bcmnexusScreen, "%s()\n", __FUNCTION__ );

    return result;
}


#if (NUM_DISPLAYS > 1)
static DFB_PlatformVideoFormat
getDFBVideoFormatFromOutputRes( DFBScreenOutputResolution outRes,
                                DFBScreenID screen )
{
    DFB_PlatformVideoFormat format;

    if (screen == PRIMARY_DISPLAY_ID)
    {
        switch (outRes)
        {
        case DSOR_640_480:
            format = DFB_PlatformVideoFormat_eVesa640x480p60hz;
            break;
        case DSOR_720_480:
            format = DFB_PlatformVideoFormat_eNtsc;
            break;
        case DSOR_720_576:
            format = DFB_PlatformVideoFormat_ePalG;
            break;
        case DSOR_800_600:
            format = DFB_PlatformVideoFormat_eVesa800x600p60hz;
            break;
        case DSOR_1024_768:
            format = DFB_PlatformVideoFormat_eVesa1024x768p60hz;
            break;
        case DSOR_1152_864:
            format = DFB_PlatformVideoFormat_eVesa1152x864p75hz;
            break;
        case DSOR_1280_720:
            format = DFB_PlatformVideoFormat_e720p;
            break;
        case DSOR_1280_768:
            format = DFB_PlatformVideoFormat_eVesa1280x768p60hz;
            break;
        case DSOR_1280_960:
            format = DFB_PlatformVideoFormat_eVesa1280x960p60hz;
            break;
        case DSOR_1280_1024:
            format = DFB_PlatformVideoFormat_eVesa1280x1024p60hz;
            break;
        case DSOR_1400_1050:
            format = DFB_PlatformVideoFormat_eVesa1400x1050p60hz;
            break;
        case DSOR_1600_1200:
            format = DFB_PlatformVideoFormat_eVesa1600x1200p60hz;
            break;
        case DSOR_1920_1080:
            format = DFB_PlatformVideoFormat_e1080i;
            break;
        default:
            format = DFB_PlatformVideoFormat_eUnknown;
        }
    }
    else
    {
        switch (outRes)
        {
        case DSOR_720_480:
            format = DFB_PlatformVideoFormat_eNtsc;
            break;
        case DSOR_720_576:
            format = DFB_PlatformVideoFormat_ePalG;
            break;
        default:
            format = DFB_PlatformVideoFormat_eUnknown;
        }
    }
    return format;
}

static DFBResult
secondaryInitScreen( UNUSED_ CoreScreen           *screen,
                     UNUSED_ CoreGraphicsDevice   *device,
                             void                 *driver_data,
                             void                 *screen_data,
                             DFBScreenDescription *description )
{
    DFBResult                    result;
    DFBBCMNEXUS                 *dfb_bcmnexus = driver_data;
    bcmScreenData               *sd           = screen_data;
    DFB_PlatformVideoFormat      secondaryFormat;
    DFB_PlatformDisplaySettings  displaySettings;

    D_DEBUG_AT( bcmnexusScreen, "%s()\n", __FUNCTION__ );

    D_ASSERT( dfb_bcmnexus != NULL );

    /* Screen ID = 1 => Secondary output ON */
    sd->id = SECONDARY_DISPLAY_ID;
    sd->powerMode = DSPM_ON;

    /* Set the screen capabilities. */
    description->caps = DSCCAPS_OUTPUTS | DSCCAPS_VSYNC | DSCCAPS_ENCODERS;
    description->outputs = 1;
    description->encoders = 1;

    /* Set the screen name. */
    snprintf( description->name, DFB_SCREEN_DESC_NAME_LENGTH, "BCMNEXUS Secondary Screen" );

#if BCMNEXUS_NSC_SUPPORT
    secondaryFormat = DFB_PlatformVideoFormat_eNtsc;
#else
    /* Read back the video format of the primary display and slave the SD output to it
       in terms of display resolution (and frequency) */
    DFB_Platform_GetDisplaySettings( GET_DISPLAY_HND(dfb_bcmnexus, SECONDARY_DISPLAY_ID), &displaySettings);

    switch (displaySettings.format)
    {
    case DFB_PlatformVideoFormat_e1080p50hz:
    case DFB_PlatformVideoFormat_e1080i50hz:
    case DFB_PlatformVideoFormat_e1250i50hz:
    case DFB_PlatformVideoFormat_e720p50hz:
    case DFB_PlatformVideoFormat_e720p50hz_3DOU_AS:
    case DFB_PlatformVideoFormat_eCustom_3D_720p_50hz:
    case DFB_PlatformVideoFormat_e576p:
    case DFB_PlatformVideoFormat_e1440x576p50hz:
    case DFB_PlatformVideoFormat_eCustomer1366x768p50hz:
    case DFB_PlatformVideoFormat_ePalG:
    case DFB_PlatformVideoFormat_e288p50hz:
        secondaryFormat = DFB_PlatformVideoFormat_ePalG;
        break;
#if defined(HUMAX_PLATFORM_BASE)
	case DFB_PlatformVideoFormat_eNtscJapan:
		secondaryFormat = DFB_PlatformVideoFormat_eNtscJapan;
		break;
#endif		
    default:
        secondaryFormat = DFB_PlatformVideoFormat_eNtsc;
    }
#endif

    result = setVideoOutput(dfb_bcmnexus, sd, 0, secondaryFormat, DSEPF_MONO, displaySettings.aspectRatio);
    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d format=%d Exit [%d].\n", __FUNCTION__, sd->id, secondaryFormat, result );
    return result;
}

static DFBResult
bcmInitOutput( UNUSED_ CoreScreen                 *screen,
               UNUSED_ void                       *driver_data,
               UNUSED_ void                       *screen_data,
               UNUSED_ int                         output,
               UNUSED_ DFBScreenOutputDescription *description,
               UNUSED_ DFBScreenOutputConfig      *config )
{
    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, output=%d, flags=0x%08x\n", __FUNCTION__, ((bcmScreenData*)screen_data)->id, output, config->flags );

    /* Set the output capabilities. */
    description->caps = DSOCONF_RESOLUTION;

    /* Set the configuration */
    config->flags |= DSOCONF_RESOLUTION;
    config->resolution = DSOR_UNKNOWN;

    return DFB_OK;
}

static DFBResult
bcmTestOutputConfig( UNUSED_ CoreScreen          *screen,
                     void                        *driver_data,
                     void                        *screen_data,
                     int                          output,
                     const DFBScreenOutputConfig *config,
                     DFBScreenOutputConfigFlags  *failed )
{
    bcmScreenData *sd = screen_data;

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, output=%d, flags=0x%08x\n", __FUNCTION__, sd->id, output, config->flags );

    D_ASSERT( (DFBBCMNEXUS*)driver_data != NULL );

    if (output >= NUM_DISPLAYS)
    {
        *failed |= DSOCONF_CONNECTORS;
    }

    if (config->flags & DSOCONF_RESOLUTION)
    {
        if (getDFBVideoFormatFromOutputRes(config->resolution, sd->id) == DFB_PlatformVideoFormat_eUnknown)
        {
            *failed |= DSOCONF_RESOLUTION;
        }
    }

    if (*failed)
    {
        D_DEBUG_AT( bcmnexusScreen, "%s() screen %d failed flags = 0x%08x\n", __FUNCTION__, sd->id, *failed );
        return DFB_UNSUPPORTED;
    }
    return DFB_OK;
}

/* NOTE: We only support this API for the secondary output, not the primary.
         Applications must use the "SetEncoderConfig" API instead to setup
         the resolution on the primary display.
*/
static DFBResult
bcmSetOutputConfig( UNUSED_ CoreScreen                  *screen,
                            void                        *driver_data,
                            void                        *screen_data,
                            int                          output,
                            const DFBScreenOutputConfig *config )
{
    DFBResult                result       = DFB_OK;
    DFBBCMNEXUS             *dfb_bcmnexus = driver_data;
    bcmScreenData           *sd           = screen_data;
    DFB_PlatformVideoFormat  format;
    DFBRectangle             destRectangle;
    DFB_PlatformDimension    displaySize;

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d, output=%d, flags=0x%08x\n", __FUNCTION__, sd->id, output, config->flags );

    D_ASSERT( dfb_bcmnexus != NULL );

    if (output >= NUM_DISPLAYS)
    {
        result = DFB_UNSUPPORTED;
    }
    else if (config->flags & DSOCONF_RESOLUTION)
    {
        format = getDFBVideoFormatFromOutputRes(config->resolution, sd->id);

        if ((format != DFB_PlatformVideoFormat_eUnknown) && (sd->fixedResolution == false))
        {
            D_DEBUG_AT( bcmnexusScreen, "%s() screen %d res=0x%08x, format=%d\n", __FUNCTION__, sd->id, config->resolution, format );

            /* Disable graphics first to help prevent unwanted artefacts */
            result = bcmSetLayerState(dfb_bcmnexus, GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, sd->id), false);

            if (result == DFB_OK)
            {
                usleep(20*1000);    /* Wait 20ms */
                result = setVideoOutput(dfb_bcmnexus, sd, 0, format, DSEPF_MONO, DFB_PlatformDisplayAspectRatio_eAuto);

                if (result == DFB_OK)
                {
                    DFB_Platform_P_GetDisplaySize(sd->id, &displaySize);

                    destRectangle.x = 0;
                    destRectangle.y = 0;
                    destRectangle.w = displaySize.w;
                    destRectangle.h = displaySize.h;

                    /* Now set the new destination rectangle and framebuffer */
                    result = bcmSetLayerDestRectangle(dfb_bcmnexus, GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, sd->id), &destRectangle);
                }
            }
            usleep(20*1000);    /* Wait 20ms */
            /* Now re-enable graphics */
            bcmSetLayerState(dfb_bcmnexus, GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, sd->id), true);
        } else {
            result = DFB_UNSUPPORTED;
        }
    }
    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d Exit [%d].\n", __FUNCTION__, sd->id, result );
    return result;
}
#endif

static DFBResult
bcmGetScreenSize( UNUSED_ CoreScreen *screen,
                          void       *driver_data,
                          void       *screen_data,
                          int        *ret_width,
                          int        *ret_height )
{
    bcmScreenData         *sd = screen_data;
    DFB_PlatformDimension  displaySize;

    D_ASSERT( (DFBBCMNEXUS*)driver_data != NULL );

    DFB_Platform_P_GetDisplaySize(sd->id, &displaySize);

    *ret_width  = displaySize.w;
    *ret_height = displaySize.h;

    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d: %dx%d\n", __FUNCTION__, sd->id, *ret_width, *ret_height );

    return DFB_OK;
}

#if defined(BCMNEXUS_DISPLAY_HAS_VSYNC_CALLBACK) && !BCMNEXUS_NSC_SUPPORT
static DFBResult
bcmWaitForVSync( UNUSED_ CoreScreen *screen,
                         void       *driver_data,
                         void       *screen_data )
{
    DFBBCMNEXUS    *dfb_bcmnexus = driver_data;
    bcmScreenData  *sd           = screen_data;
    pthread_mutex_t *mutex;
    pthread_cond_t  *sem;

    D_ASSERT( dfb_bcmnexus != NULL );

    D_DEBUG_AT( bcmnexusScreenX, "Wait for VSync[%d]\n", sd->id );

    mutex = &GET_DISPLAY_VSYNC_MUTEX(dfb_bcmnexus, sd->id);
    sem = &GET_DISPLAY_VSYNC_SEM(dfb_bcmnexus, sd->id);

    PTHREAD_ROBUST_MUTEX_LOCK(mutex);
    pthread_cond_wait(sem, mutex);
    pthread_mutex_unlock(mutex);

    D_DEBUG_AT( bcmnexusScreenX, "Received VSync[%d]\n", sd->id );

    return DFB_OK;
}
#endif

static DFBResult
bcmSetPowerMode( UNUSED_ CoreScreen         *screen,
                         void               *driver_data,
                         void               *screen_data,
                         DFBScreenPowerMode  mode )
{
    DFBResult      res = DFB_OK;
    DFBBCMNEXUS   *dfb_bcmnexus = driver_data;
    bcmScreenData *sd           = screen_data;

    D_ASSERT( dfb_bcmnexus != NULL );

    D_DEBUG_AT( bcmnexusScreen, "%s: screen %d power mode=%d\n", __FUNCTION__, sd->id, mode );

    if (mode != sd->powerMode) {
        sd->powerMode = mode;
        res = bcmSetLayerState(dfb_bcmnexus, GET_DISPLAY_GFX_LAYER_ID(dfb_bcmnexus, PRIMARY_GFX_LAYER_INDEX, sd->id), mode==DSPM_ON);
    }
    D_DEBUG_AT( bcmnexusScreen, "%s() screen %d Exit [%d].\n", __FUNCTION__, sd->id, res );
    return res;
}

DFBScreenPowerMode
bcmGetPowerMode( CoreScreen *screen )
{
    bcmScreenData *sd = screen->screen_data;
    return sd->powerMode;
}

ScreenFuncs bcmnexusPrimaryScreenFuncs = {
    .ScreenDataSize =    bcmScreenDataSize,
    .InitScreen =        primaryInitScreen,
    .InitEncoder =       bcmInitEncoder,
    .TestEncoderConfig = bcmTestEncoderConfig,
    .SetEncoderConfig =  bcmSetEncoderConfig,
    .GetScreenSize =     bcmGetScreenSize,
#if defined(BCMNEXUS_DISPLAY_HAS_VSYNC_CALLBACK) && !BCMNEXUS_NSC_SUPPORT
    .WaitVSync =         bcmWaitForVSync,
#endif
    .SetPowerMode =      bcmSetPowerMode,
    .InitMixer =         bcmInitMixer,
    .TestMixerConfig =   bcmTestMixerConfig,
    .SetMixerConfig =    bcmSetMixerConfig,
    .GetMixerState =     bcmGetMixerState};


#if (NUM_DISPLAYS > 1)
ScreenFuncs bcmnexusSecondaryScreenFuncs = {
    .ScreenDataSize =    bcmScreenDataSize,
    .InitEncoder =       bcmInitEncoder,
    .TestEncoderConfig = bcmTestEncoderConfig,
    .SetEncoderConfig =  bcmSetEncoderConfig,
    .InitScreen =        secondaryInitScreen,
    .InitOutput =        bcmInitOutput,
    .TestOutputConfig =  bcmTestOutputConfig,
    .SetOutputConfig =   bcmSetOutputConfig,
    .GetScreenSize =     bcmGetScreenSize,
#if defined(BCMNEXUS_DISPLAY_HAS_VSYNC_CALLBACK) && !BCMNEXUS_NSC_SUPPORT
    .WaitVSync =         bcmWaitForVSync,
#endif
    .SetPowerMode =      bcmSetPowerMode
};
#endif
