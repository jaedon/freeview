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
 * $brcm_Workfile: bcmnexus.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/19 $
 *
 * $brcm_Date: 3/7/12 5:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus.c $
 *
 * DirectFB_1_4_15_Port/19   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/18   3/1/12 7:58p christ
 * SW7420-2273: DirectFB-1.4.15: Use correct Nexus heap for secondary
 *  display frame buffers
 *
 * DirectFB_1_4_15_Port/17   2/9/12 2:33p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB_1_4_15_Port/16   1/3/12 4:45p robertwm
 * SW7425-2047: DirectFB: System driver registers second display when not
 *  initialised.
 *
 * DirectFB_1_4_15_Port/15   12/22/11 4:37p christ
 * SW7425-2047: DirectFB: System driver registers second display when not
 *  initialised
 *
 * DirectFB_1_4_15_Port/14   12/12/11 11:05p robertwm
 * SW7420-2192: DirectFB-1.4.15: Cannot run any test on the video layer.
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   12/8/11 3:35p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/13   12/2/11 4:40p scchang
 * SW7420-2158: DirectFB: Consider setting pthread mutex robust attribute
 *
 * DirectFB_1_4_15_Port/11   11/8/11 3:35p christ
 * SW7425-1703: DirectFB: Move NEXUS_Display calls into platform layer
 *
 * DirectFB_1_4_15_Port/9   11/4/11 12:30p robertwm
 * SW7425-1664: DirectFB: Generic IPC for hardware accelerated image
 *  providers.
 *
 * DirectFB_1_4_15_Port/8   10/11/11 12:38p christ
 * SW7425-1465: DirectFB: Disable HW image provider when using secure
 *  fusion mode
 *
 * DirectFB_1_4_15_Port/7   10/6/11 4:09p christ
 * SW7425-1420: DirectFB: Access master surface pool handle from slave
 *  application
 *
 * DirectFB_1_4_15_Port/6   10/5/11 12:17p robertwm
 * SWDTV-8225: [35233: 4.0] Need H/W Cursor support in DFB1.4.5 for DTV4.0
 *  233.
 * SW7231-382: DirectFB: Support Creating DFB Surfaces for OpenGL
 *  rendering on the VC-4.
 *
 * DirectFB_1_4_14_Port/5   9/23/11 12:47p christ
 * SW7420-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 * DirectFB_1_4_14_Port/4   9/13/11 4:46p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 * DirectFB_1_4_14_Port/3   9/6/11 5:12p robertwm
 * SW7420-2047: DirectFB: Closing down DFB slave application causes kernel
 *  oops.
 *
 * DirectFB_1_4_14_Port/2   8/12/11 4:44p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/5   7/12/11 1:05p christ
 * SW7425-887: DirectFB: System driver updates to support later versions
 *  of DirectFB (1.4.13 and later)
 *
 * DirectFB_1_4_5_Port/4   7/11/11 5:30p robertwm
 * SW7231-292: DirectFB-1.4.5: Ensure that outputs are not connected to
 *  the display twice.
 *
 * DirectFB_1_4_5_Port/3   7/7/11 6:45p tross
 * SWBLURAY-26478: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_5_Port/2   6/27/11 6:25p christ
 * SW7425-782: DirectFB: Latest DirectFB 1.4.5 tree fails to shutdown when
 *  running testman / testrun.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:05p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/30   6/10/11 5:17p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/29   6/2/11 4:34p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/28   5/17/11 6:50p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/27   5/13/11 11:00p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 * SW7405-5327: DirectFB: Remove "LD_PRELOAD" of libinit.so.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/4   3/28/11 11:03a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/26   3/30/11 6:20p christ
 * SW7425-254: DirectFB: Improve performance by replacing linux-fusion
 *  calls with pthreads
 *
 * DirectFB_1_4_Port/25   3/17/11 12:42p robertwm
 * SW7422-378: DirectFB: Destroying a Palettised surface in a client
 *  application causes NEXUS assertion during free.
 * SW7422-383: DirectFB: Need to serialise access to SID to prevent
 *  assertions.
 *
 * DirectFB_1_4_Port/24   3/11/11 5:05p christ
 * SW7468-370: DirectFB: 1.4.1 release fails to compile for platforms
 *  without component output.
 *
 * DirectFB_1_4_Port/23   3/1/11 8:50p robertwm
 * SW7420-1559: DirectFB: NEXUS surface assertion in multi-app mode when
 *  closing down a DFB client.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   3/18/11 5:07p tross
 * Merge in latest from DirectFB & Nexus.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/14/11 1:24p tross
 * Call to SetEncoderConfig to 720p from df_andi was causing errors
 * in bdvd_gfx_window_set_framebuffer().
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:14p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/22   2/15/11 6:09p apandav
 * SW35230-3034: Mainline DirectFB build fails in PCL 2k11 source tree
 *
 * DirectFB_1_4_Port/21   2/8/11 12:44p robertwm
 * SW7420-1467: DirectFB: Multiple apps should NOT share NEXUS Display
 *  handle.
 *
 * DirectFB_1_4_Port/20   2/3/11 6:21p christ
 * SW7420-1458: DirectFB: Rework internal handling of nexus heaps. -
 *  remove second nexus join.
 *
 * DirectFB_1_4_Port/19   2/3/11 6:01p christ
 * SW7420-1458: DirectFB: Rework internal handling of nexus heaps.
 *
 * DirectFB_1_4_Port/18   1/19/11 12:25p scchang
 * SW7420-1394: video layer support in DFB
 *
 * DirectFB_1_4_Port/17   12/3/10 8:39p raywu
 * SW7550-629: DirectFB: Integrated ZSP code to DFB packet buffer branch
 *
 * DirectFB_1_4_Port/16   12/1/10 3:47p christ
 * SW7420-1283: DirectFB: Add support for extra Nexus heap on dual MEMC
 *  parts
 *
 * DirectFB_1_4_Port/15   8/20/10 3:38p robertwm
 * SW7405-4791: DirectFB: DirectFB-1.4.1 Phase 2.5 fails to build with
 *  Nexus Phase 5.0 reference software.
 *
 * DirectFB_1_4_Port/14   8/11/10 3:24a robertwm
 * SW3556-1177: DirectFB: system_initialize segfault with DirectFB-1.4.1
 *  Phase 2.5 PreRel.
 *
 * DirectFB_1_4_Port/13   8/11/10 2:01a robertwm
 * SW7405-4250: DirectFB: Screen destination rectangle and source
 *  rectangle not saved across contexts.
 * SW7405-4743: DirectFB: Multiple instances of df_andi.c do not get
 *  displayed.
 *
 * DirectFB_1_4_Port/12   7/26/10 5:32p jackf
 * SW35230-771: Added include for shim_nexus.h.
 *
 * DirectFB_1_4_Port/11   4/19/10 2:12p robertwm
 * SW3556-1099: DirectFB: SID should not be enabled if there is
 *  insufficient driver memory to initialise it.
 *
 * DirectFB_1_4_Port/10   4/15/10 10:36p robertwm
 * SW7405-4214: DirectFB: Multi-Process DirectFB fails to run.
 *
 * DirectFB_1_4_Port/9   4/15/10 7:52p robertwm
 * SW7405-4184: Allow independent mode and resolution to be specified.
 *
 * DirectFB_1_4_Port/8   4/9/10 4:24p robertwm
 * SW7405-4181: DirectFB: Allow the graphics and system module versions to
 *  be specified in the build system.
 * SW7405-4180: DirectFB: Require independent setting of graphics
 *  resolution on multiple output paths.
 *
 * DirectFB_1_4_Port/7   12/23/09 2:22p robertwm
 * SW7550-51: Support DirectFB on 7550.
 *
 * DirectFB_1_4_Port/SW7550-51/1   12/22/09 3:22p mphillip
 * SW7550-51: Add 7550 build support (on branch)
 *
 * DirectFB_1_4_Port/6   12/4/09 11:00a robertwm
 * SW7405-3539: DirectFB: Running df_andi (penguins) using default window
 *  manager in exclusive mode locks up.
 *
 * DirectFB_1_4_Port/5   11/16/09 1:27p robertwm
 * SW7405-3399: WaitForSync() function is unimplemented.
 * (Fixed graphics freeze issue).
 *
 * DirectFB_1_4_Port/4   11/13/09 7:10p robertwm
 * SW7405-3399: WaitForSync() function is unimplemented.
 *
 * DirectFB_1_4_Port/3   8/27/09 4:36p robertwm
 * SW3548-2427: DirectFB: Support 3549 panel types and fix compilation
 *  bugs.
 *
 * DirectFB_1_4_Port/2   8/21/09 11:12a robertwm
 * PR57917: DirectFB: testman and testrun don't run anymore in 1.4.1
 *  (SaWMan)
 *
 * DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
 * PR56658: Add DirectFB 1.4.1 broadcom_files
 *
 * 13   3/12/09 11:54a gmohile
 * PR 53153 : Set memory alignment settings
 *
 * 12   2/10/09 10:25a gmohile
 * PR 45342 : Free graphics memory during shutdown
 *
 * 11   11/3/08 4:05p gmohile
 * PR 25109 : DO not shut down outputs before removing them
 *
 * 10   11/3/08 1:57p gmohile
 * PR 45243 : Default all DTV to panel output
 *
 * 9   10/24/08 12:42p gmohile
 * PR 46490 : Add hw picture decode support
 *
 * 8   10/15/08 3:22p gmohile
 * PR 45342 : No need to destroy primary surface at shutdown
 *
 *
 ***************************************************************************/

#include "bcmnexus.h"
#include "bcmnexus_screen.h"
#include "bcmnexus_layer.h"
#include "bcmnexus_pool.h"
#include "bcmnexus_video.h"
#ifdef BCMNEXUS_HW_CURSOR_SUPPORT
#include "bcmnexus_hwcursor.h"
#endif
#include "bdbg.h"

#include <directfb_version.h>

#include <misc/conf.h>
#include <core/core.h>
#include <core/coredefs.h>
#include <core/coretypes.h>
#include <core/layers_internal.h>
#include <core/layer_control.h>
#include <core/screens_internal.h>
#include <core/screens.h>

#include <core/core_system.h>

#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/thread.h>
#include <direct/conf.h>

#include <fusion/conf.h>
#include <fusion/arena.h>
#include <fusion/shmalloc.h>

D_DEBUG_DOMAIN( bcmnexusSystem,  "bcmNexus/System",  "Broadcom NEXUS System module" );
D_DEBUG_DOMAIN( bcmnexusSystemX, "bcmNexus/SystemX", "Broadcom NEXUS System module eXtended debugging" );

DFB_CORE_SYSTEM( bcmnexus )

CoreDFB *dfb_bcmnexus_core = NULL;

#ifdef __GNUC__
#define UNUSED_ __attribute__ ((unused))
#else
#define UNUSED_
#endif

/** public **/
static void
system_get_info( CoreSystemInfo *info )
{
    D_DEBUG_AT( bcmnexusSystem, "%s()\n", __FUNCTION__ );

    info->version.major = BCMNEXUS_SYS_MAJOR_VERSION;
    info->version.minor = BCMNEXUS_SYS_MINOR_VERSION;
    info->type = CORE_ANY;
    info->caps = CSCAPS_ACCELERATION;

    snprintf( info->name, DFB_CORE_SYSTEM_INFO_NAME_LENGTH, "BCMNEXUS" );
    snprintf( info->vendor, DFB_CORE_SYSTEM_INFO_VENDOR_LENGTH, "Broadcom Corporation" );
    snprintf( info->url, DFB_CORE_SYSTEM_INFO_URL_LENGTH, "http://www.broadcom.com" );
    snprintf( info->license, DFB_CORE_SYSTEM_INFO_LICENSE_LENGTH, "proprietary" );
}

static void
register_screens_layers( DFBBCMNEXUS *dfb_bcmnexus )
{
    CoreScreen *primary_screen;
#if (NUM_DISPLAYS > 1)
    CoreScreen *secondary_screen;
    DFB_PlatformSettings  platformSettings;
#endif

    /* Register primary screen functions */
    primary_screen = dfb_screens_register( dfb_gfxcard_get_primary(), dfb_bcmnexus, &bcmnexusPrimaryScreenFuncs );

    if (primary_screen == NULL)
    {
        D_ERROR("Cannot register primary screen!");
        return;
    }

    /* Always register primary graphics layer functions FIRST */
    dfb_layers_register( primary_screen, dfb_bcmnexus, &bcmnexusPrimaryGfxLayerFuncs );

#if (NUM_DISPLAYS > 1)
    /* Get the DirectFB Platform settings */
    DFB_Platform_GetSettings(&platformSettings);

    /* Check if our secondary display has been enabled */
    if (platformSettings.display[1].handle != NULL )
    {
        D_DEBUG_AT( bcmnexusSystem, "%s() Enable secondary display\n",__FUNCTION__);

    /* Register secondary screen functions */
    secondary_screen = dfb_screens_register( dfb_gfxcard_get_primary(), dfb_bcmnexus, &bcmnexusSecondaryScreenFuncs );

    if (secondary_screen == NULL)
    {
        D_ERROR("Cannot register secondary screen!");
        return;
    }

    /* Always register secondary graphics layer functions SECOND */
    dfb_layers_register( secondary_screen, dfb_bcmnexus, &bcmnexusSecondaryGfxLayerFuncs );
    }
    else
        D_DEBUG_AT( bcmnexusSystem, "%s() secondary display not initialised - not enabling\n",__FUNCTION__);

#endif

    /* register video layer functions */
    dfb_layers_register( primary_screen, dfb_bcmnexus, &bcmnexusVideoFuncs );

#ifdef BCMNEXUS_HW_CURSOR_SUPPORT

/*Register hardware cursor functions*/
dfb_layers_register( primary_screen, dfb_bcmnexus, &bcmnexusHwCursorFuncs );

#endif

}

static void
unregister_screens_layers( DFBBCMNEXUS *dfb_bcmnexus )
{
#if !BCMNEXUS_NSC_SUPPORT
    NEXUS_StopCallbacks(GET_DISPLAY_HND(dfb_bcmnexus, PRIMARY_DISPLAY_ID));
#if (NUM_DISPLAYS > 1)
    NEXUS_StopCallbacks(GET_DISPLAY_HND(dfb_bcmnexus, SECONDARY_DISPLAY_ID));
#endif
    pthread_mutex_destroy( &GET_DISPLAY_VSYNC_MUTEX(dfb_bcmnexus, PRIMARY_DISPLAY_ID) );
    pthread_cond_destroy( &GET_DISPLAY_VSYNC_SEM(dfb_bcmnexus, PRIMARY_DISPLAY_ID) );
    SET_DISPLAY_VSYNC_CB(dfb_bcmnexus, NULL, PRIMARY_DISPLAY_ID);
#if (NUM_DISPLAYS > 1)
    pthread_mutex_destroy( &GET_DISPLAY_VSYNC_MUTEX(dfb_bcmnexus, SECONDARY_DISPLAY_ID));
    pthread_cond_destroy( &GET_DISPLAY_VSYNC_SEM(dfb_bcmnexus, SECONDARY_DISPLAY_ID) );
    SET_DISPLAY_VSYNC_CB(dfb_bcmnexus, NULL, SECONDARY_DISPLAY_ID);
#endif
#else
    (void)dfb_bcmnexus;
#endif
}

void
option_get_string(const char * name, char **dest)
{
    char *values[10];
    int num;

    D_DEBUG_AT( bcmnexusSystem, "%s() Getting value for %s\n", __FUNCTION__, name);

    if (direct_config_get(name, values, 10, &num) == DR_OK)
    {
        if (*dest)
        {
            D_FREE( *dest );
        }

        if (num > 0)
            *dest = D_STRDUP( values[num - 1] );
        else
            *dest = D_STRDUP( "" );

        D_DEBUG_AT( bcmnexusSystem, "%s() Value for %s is $%s$\n", __FUNCTION__, name, *dest);

    }
    else
    {
        D_DEBUG_AT( bcmnexusSystem, "%s() Failed to get value for %s\n", __FUNCTION__, name);
    }

}

void
system_intiailize_options(BCMNEXUS_Options *options)
{

    char *temp = NULL;

    options->sw_picture_decode = true;

#ifdef NEXUS_HAS_PICTURE_DECODER
    /* For chips with SID always default to h/w decode */
    /*But run-time config can allow switching to s/w decode */
    if(!getenv("sw_picture_decode")) {
        options->sw_picture_decode = false;
    }
#endif

    options->bcmnexus_ir_timeout = direct_config_get_int_value("bcmnexus-ir-timeout");
    if (options->bcmnexus_ir_timeout == 0)
    {
        /*Default value if unset*/
        options->bcmnexus_ir_timeout = 120;
    }

    options->bcmnexus_key_timeout = direct_config_get_int_value("bcmnexus-key-timeout");
    if (options->bcmnexus_key_timeout == 0)
    {
        /*Default value if unset*/
        options->bcmnexus_key_timeout = 150;
    }

    options->bcmnexus_ir_repeat_time = direct_config_get_int_value("bcmnexus-ir-repeat-time");
    options->bcmnexus_key_repeat_time = direct_config_get_int_value("bcmnexus-key-repeat-time");


    options->bcmnexus_key_debounce = direct_config_get_int_value("bcmnexus-key-debounce");

    options->bcmnexus_ir_repeat_skip = direct_config_get_int_value("bcmnexus-ir-repeat-skip");
    options->bcmnexus_key_repeat_skip = direct_config_get_int_value("bcmnexus-key-repeat-skip");

    option_get_string("bcmnexus-ir-protocol", &options->bcmnexus_ir_protocol);
    option_get_string("bcmnexus-ir-keycodes", &options->bcmnexus_ir_keycodes);

    option_get_string("res", &options->res);

    if (options->res) {
        if ((strcmp(options->res, "1080p"  ) != 0)  &&
            (strcmp(options->res, "1080p60") != 0)  &&
            (strcmp(options->res, "1080p50") != 0)  &&
            (strcmp(options->res, "1080p30") != 0)  &&
            (strcmp(options->res, "1080p25") != 0)  &&
            (strcmp(options->res, "1080p24") != 0)  &&
            (strcmp(options->res, "1080i"  ) != 0)  &&
            (strcmp(options->res, "1080i30") != 0)  &&
            (strcmp(options->res, "1080i25") != 0)  &&
            (strcmp(options->res, "720p"   ) != 0)  &&
            (strcmp(options->res, "720ps"  ) != 0)  &&
            (strcmp(options->res, "720p60" ) != 0)  &&
            (strcmp(options->res, "720p50" ) != 0)  &&
            (strcmp(options->res, "720p50s") != 0)  &&
            (strcmp(options->res, "720p30" ) != 0)  &&
            (strcmp(options->res, "720p25" ) != 0)  &&
            (strcmp(options->res, "720p24" ) != 0)  &&
            (strcmp(options->res, "720p24s") != 0)  &&
            (strcmp(options->res, "576p"   ) != 0)  &&
            (strcmp(options->res, "576i"   ) != 0)  &&
            (strcmp(options->res, "480p"   ) != 0)  &&
            (strcmp(options->res, "480i"   ) != 0)  &&
            (strcmp(options->res, "VGA"    ) != 0)  &&
            (strcmp(options->res, "SVGA"   ) != 0)  &&
            (strcmp(options->res, "XGA"    ) != 0))
        {
            D_ERROR("%s: 'res': unsupported resolution specified! $%s$ %d \n", __FUNCTION__, options->res, strcmp(options->res, "720p"   ));
            D_FREE( options->res );
            options->res = D_STRDUP(" ");
        }
    }
    else
    {
            D_ERROR("%s: 'res': unspecified!\n", __FUNCTION__);
            options->res = D_STRDUP(" ");
    }

    option_get_string("ibuffer", &temp);
    if (temp)
    {
        if (sscanf( temp, "%dx%d", &options->ibuffer.width, &options->ibuffer.height ) < 2) {
            D_ERROR("DirectFB/Config '%s': Could not parse width and height!\n", temp);
        }

        D_FREE(temp);
    }

}

static DFBResult
system_initialize( CoreDFB *core, void **data )
{
    DFBResult            ret;
    DFBBCMNEXUS *dfb_bcmnexus = NULL;
    FusionSHMPoolShared *pool;
    DFB_PlatformSettings platformSettings;

    D_DEBUG_AT( bcmnexusSystem, "%s() core=%p\n", __FUNCTION__, (void *)core );
    pthread_mutexattr_t mutexAttr;

    DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eMasterNexusUninitialized, &platformSettings);

    /* Call to initialise DirectFB Platform code */
    ret = DFB_Platform_Init(&platformSettings);
    if (ret) {
        return ret;
    }

    /* Allocate shared memory state object */
    pool = dfb_core_shmpool( core );
    dfb_bcmnexus = SHCALLOC( pool, 1, sizeof(DFBBCMNEXUS) );
    if (!dfb_bcmnexus) {
        return D_OOSHM();
    }

    /* Save the per-process core handle */
    dfb_bcmnexus_core = core;

    system_intiailize_options(&dfb_bcmnexus->options);

    /* Get the DirectFB Platform settings */
    DFB_Platform_GetSettings(&platformSettings);

    SET_DISPLAY_HEAP_HND(dfb_bcmnexus, platformSettings.displayHeapHandle);
    D_DEBUG_AT( bcmnexusSystem, "%s() display heap handle=%p\n", __FUNCTION__, (void *)GET_DISPLAY_HEAP_HND(dfb_bcmnexus) );
    SET_2ND_DISPLAY_HEAP_HND(dfb_bcmnexus, platformSettings.secondaryDisplayHeapHandle);
    D_DEBUG_AT( bcmnexusSystem, "%s() secondary display heap handle=%p\n", __FUNCTION__, (void *)GET_2ND_DISPLAY_HEAP_HND(dfb_bcmnexus) );
    SET_OFFSCREEN_HEAP_HND(dfb_bcmnexus, platformSettings.offscreenHeapHandle);
    D_DEBUG_AT( bcmnexusSystem, "%s() offscreen heap handle=%p\n", __FUNCTION__, (void *)GET_OFFSCREEN_HEAP_HND(dfb_bcmnexus) );
    SET_PALETTE_HEAP_HND(dfb_bcmnexus, platformSettings.paletteHeapHandle);
    D_DEBUG_AT( bcmnexusSystem, "%s() palette heap handle=%p\n", __FUNCTION__, (void *)GET_PALETTE_HEAP_HND(dfb_bcmnexus) );
    SET_VIDEO_HEAP_HND(dfb_bcmnexus, platformSettings.videoHeapHandle);
    D_DEBUG_AT( bcmnexusSystem, "%s() video heap handle=%p\n", __FUNCTION__, (void *)GET_VIDEO_HEAP_HND(dfb_bcmnexus) );
    SET_GRAPHICS2D_HND(dfb_bcmnexus, platformSettings.graphics2d.handle);
    D_DEBUG_AT( bcmnexusSystem, "%s() graphics2d handle=%p\n", __FUNCTION__, (void *)GET_GRAPHICS2D_HND(dfb_bcmnexus) );
    SET_DISPLAY_HND(dfb_bcmnexus, platformSettings.display[0].handle, PRIMARY_DISPLAY_ID);
    D_DEBUG_AT( bcmnexusSystem, "%s() display0 handle=%p\n", __FUNCTION__, (void *)GET_DISPLAY_HND(dfb_bcmnexus, PRIMARY_DISPLAY_ID) );
#if (NUM_DISPLAYS > 1)
    SET_DISPLAY_HND(dfb_bcmnexus, platformSettings.display[1].handle, SECONDARY_DISPLAY_ID);
    D_DEBUG_AT( bcmnexusSystem, "%s() display1 handle=%p\n", __FUNCTION__, (void *)GET_DISPLAY_HND(dfb_bcmnexus, SECONDARY_DISPLAY_ID) );
#endif
#if (NUM_DISPLAYS > 2)
    SET_DISPLAY_HND(dfb_bcmnexus, platformSettings.display[2].handle, TERTIARY_DISPLAY_ID);
    D_DEBUG_AT( bcmnexusSystem, "%s() display2 handle=%p\n", __FUNCTION__, (void *)GET_DISPLAY_HND(dfb_bcmnexus, TERTIARY_DISPLAY_ID) );
#endif

    /* register screens and layers */
    register_screens_layers( dfb_bcmnexus );

    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);

#if NEXUS_HAS_PICTURE_DECODER
    if (platformSettings.pictureDecoder.handle == NULL)
        dfb_bcmnexus->options.sw_picture_decode = true;
    else
        SET_PICTURE_DECODER_HND(dfb_bcmnexus, platformSettings.pictureDecoder.handle);

    pthread_mutex_init( &GET_PICTURE_DECODER_LOCK(dfb_bcmnexus), &mutexAttr);

    D_DEBUG_AT( bcmnexusSystem, "%s() picture decoder handle=%p\n", __FUNCTION__, (void *)GET_PICTURE_DECODER_HND(dfb_bcmnexus) );
#endif

#if NEXUS_HAS_BCC
        SET_BCC_HND(dfb_bcmnexus, platformSettings.blockCopyCompositor.handle);
#endif

    /* make data accessible via dfb_system_data(); */
    /* must be set before initializing the pools! */
    *data = dfb_bcmnexus;

    dfb_surface_pool_initialize( core, &bcmnexusPoolFunctions,           &GET_POOL_HND(dfb_bcmnexus) );
#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_V3D)
    dfb_surface_pool_initialize( core, &bcmnexusGraphics3DPoolFunctions, &GET_GFX3D_POOL_HND(dfb_bcmnexus) );
#endif

    /* Initialise Primary Screen VSYNC callbacks */
    pthread_mutex_init( &GET_DISPLAY_VSYNC_MUTEX(dfb_bcmnexus, PRIMARY_DISPLAY_ID), &mutexAttr);
    pthread_cond_init( &GET_DISPLAY_VSYNC_SEM(dfb_bcmnexus, PRIMARY_DISPLAY_ID), NULL);

    SET_DISPLAY_VSYNC_CB(dfb_bcmnexus, NULL, PRIMARY_DISPLAY_ID);

#if (NUM_DISPLAYS > 1)
    /* Initialise Secondary Screen VSYNC callbacks */
    pthread_mutex_init( &GET_DISPLAY_VSYNC_MUTEX(dfb_bcmnexus, SECONDARY_DISPLAY_ID), &mutexAttr);
    pthread_cond_init( &GET_DISPLAY_VSYNC_SEM(dfb_bcmnexus, SECONDARY_DISPLAY_ID), NULL);
    SET_DISPLAY_VSYNC_CB(dfb_bcmnexus, NULL, SECONDARY_DISPLAY_ID);
#endif

    pthread_mutexattr_destroy(&mutexAttr);

    DFB_Platform_P_SetSharedSettings(DFB_PlatformSharedSetting_eSurfacePool,
                                     (void*)&GET_POOL_HND(dfb_bcmnexus));

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_V3D)
    DFB_Platform_P_SetSharedSettings(DFB_PlatformSharedSetting_e3DGraphicsPool,
                                     (void*)&GET_GFX3D_POOL_HND(dfb_bcmnexus));
#endif

    D_DEBUG_AT( bcmnexusSystem, "%s() Exit(%d)\n", __FUNCTION__, DFB_OK);
    return DFB_OK;
}

static DFBResult
system_join( CoreDFB *core, void **data )
{
    DFBResult         ret;
    DFBBCMNEXUS      *dfb_bcmnexus;
    DFB_PlatformSettings platformSettings;
    CoreSurfacePool* pool;

    D_DEBUG_AT( bcmnexusSystem, "%s() core=%p\n", __FUNCTION__, (void *)core );

    DFB_Platform_GetDefaultSettings(DFB_PlatformClientType_eSlaveNexusUninitialized, &platformSettings);

    /* Call to initialise DirectFB Platform code */
    ret = DFB_Platform_Init(&platformSettings);
    if (ret) {
        return ret;
    }

    /* Save the per-process core handle */
    dfb_bcmnexus_core = core;


    dfb_bcmnexus = D_CALLOC( 1, sizeof(DFBBCMNEXUS) );
    if (!dfb_bcmnexus) {
        return D_OOM();
    }

    system_intiailize_options(&dfb_bcmnexus->options);

    SET_OFFSCREEN_HEAP_HND(dfb_bcmnexus, platformSettings.offscreenHeapHandle);
    D_DEBUG_AT( bcmnexusSystem, "%s() offscreen heap handle=%p\n", __FUNCTION__, (void *)GET_OFFSCREEN_HEAP_HND(dfb_bcmnexus) );

    DFB_Platform_P_GetSharedSettings(DFB_PlatformSharedSetting_eSurfacePool,(void*)&pool);
    SET_POOL_HND(dfb_bcmnexus,pool);
    D_DEBUG_AT( bcmnexusSystem, "%s() system pool handle=%p\n", __FUNCTION__, (void *)GET_POOL_HND(dfb_bcmnexus ));

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_V3D)
    DFB_Platform_P_GetSharedSettings(DFB_PlatformSharedSetting_e3DGraphicsPool,(void*)&pool);
    SET_GFX3D_POOL_HND(dfb_bcmnexus,pool);
    D_DEBUG_AT( bcmnexusSystem, "%s() 3D GFX pool handle=%p\n", __FUNCTION__, (void *)GET_GFX3D_POOL_HND(dfb_bcmnexus ));
#endif

    /* register screens and layers */
    register_screens_layers( dfb_bcmnexus );

    /* set this first before pool join */
    *data = dfb_bcmnexus;

    if (GET_POOL_HND(dfb_bcmnexus))
        dfb_surface_pool_join( core, GET_POOL_HND(dfb_bcmnexus), &bcmnexusPoolFunctions );

    if (GET_GFX3D_POOL_HND(dfb_bcmnexus))
        dfb_surface_pool_join( core, GET_GFX3D_POOL_HND(dfb_bcmnexus), &bcmnexusGraphics3DPoolFunctions );

    return DFB_OK;
}

static DFBResult
system_shutdown( UNUSED_ bool emergency )
{
    DFBBCMNEXUS *dfb_bcmnexus = (DFBBCMNEXUS *) dfb_system_data();

    FusionSHMPoolShared *pool;

    D_DEBUG_AT( bcmnexusSystem, "%s()\n", __FUNCTION__ );

    D_ASSERT( dfb_bcmnexus != NULL );

#if NEXUS_HAS_PICTURE_DECODER
    pthread_mutex_destroy( &GET_PICTURE_DECODER_LOCK(dfb_bcmnexus) );
#endif

    /* Unregister screens */
    unregister_screens_layers(dfb_bcmnexus);

    /* Destroy the surface pool(s) used by Nexus */
    if (GET_GFX3D_POOL_HND(dfb_bcmnexus))
        dfb_surface_pool_destroy( GET_GFX3D_POOL_HND(dfb_bcmnexus) );

    if (GET_POOL_HND(dfb_bcmnexus))
        dfb_surface_pool_destroy( GET_POOL_HND(dfb_bcmnexus) );

    pool = dfb_core_shmpool( dfb_bcmnexus_core );
    SHFREE( pool, dfb_bcmnexus );

    dfb_bcmnexus_core = NULL;

    /* Call to uninitialise platform code */
    DFB_Platform_Uninit();

    return DFB_OK;
}

static DFBResult
system_leave( UNUSED_ bool emergency )
{
    DFBBCMNEXUS *dfb_bcmnexus = (DFBBCMNEXUS *) dfb_system_data();

    D_DEBUG_AT( bcmnexusSystem, "%s()\n", __FUNCTION__ );

    /* Leave the surface pool(s) used by Nexus */
    if (GET_GFX3D_POOL_HND(dfb_bcmnexus))
        dfb_surface_pool_leave( GET_GFX3D_POOL_HND(dfb_bcmnexus) );

    if (GET_POOL_HND(dfb_bcmnexus))
        dfb_surface_pool_leave( GET_POOL_HND(dfb_bcmnexus) );

    D_FREE(dfb_bcmnexus);

    /* Call to uninitialise platform code */
    DFB_Platform_Uninit();

    return DFB_OK;
}

static DFBResult
system_suspend()
{
    D_DEBUG_AT( bcmnexusSystem, "%s()\n", __FUNCTION__ );
    return DFB_UNIMPLEMENTED;
}

static DFBResult
system_resume()
{
    D_DEBUG_AT( bcmnexusSystem, "%s()\n", __FUNCTION__ );
    return DFB_UNIMPLEMENTED;
}

/******************************************************************************/

static volatile void *
system_map_mmio( UNUSED_ unsigned int    offset,
                 UNUSED_ int             length )
{
    return NULL;
}

static void
system_unmap_mmio( UNUSED_ volatile void  *addr,
                   UNUSED_ int             length )
{
    return;
}

static int
system_get_accelerator()
{
    return -1;
}

static VideoMode *
system_get_modes()
{
    /* TODO: Generate and return a list of all video modes
     * possible.  The application will select amongst the mode
     * returned to set the display resolution, etc. */
    return NULL;
}

static VideoMode *
system_get_current_mode()
{
    return NULL;
}

static DFBResult
system_thread_init()
{
        return DFB_UNIMPLEMENTED;
}

static bool
system_input_filter( UNUSED_ CoreInputDevice *device,
                     UNUSED_ DFBInputEvent   *event )
{
    return false;
}

static unsigned long
system_video_memory_physical( UNUSED_ unsigned int offset )
{
    return 0;
}

static void *
system_video_memory_virtual( unsigned int offset )
{
    (void)offset;

    /* "unused" in DirectFB 1.3.x */
    return 0;
}

static unsigned int
system_videoram_length()
{
    unsigned int size = 0;
    NEXUS_HeapHandle heap;
    NEXUS_MemoryStatus status;
    DFBBCMNEXUS *dfb_bcmnexus;

    D_DEBUG_AT( bcmnexusSystem, "%s() Enter\n", __FUNCTION__ );

    dfb_bcmnexus = (DFBBCMNEXUS *) dfb_system_data();


    heap = GET_DISPLAY_HEAP_HND(dfb_bcmnexus);

    if (heap)
    {
        if (NEXUS_Heap_GetStatus(heap, &status) == NEXUS_SUCCESS)
            size = status.size;
    }

    D_DEBUG_AT( bcmnexusSystem, "%s() Exit (size=%d)\n", __FUNCTION__, size );

    return size;
}

static unsigned long
system_aux_memory_physical( UNUSED_ unsigned int offset )
{
    return 0;
}

static void *
system_aux_memory_virtual( UNUSED_ unsigned int offset )
{
    return NULL;
}

static unsigned int
system_auxram_length()
{
    return 0;
}

static void
system_get_busid( int *ret_bus,
                  int *ret_dev,
                  int *ret_func )
{
    *ret_bus  = 0;
    *ret_dev  = 0;
    *ret_func = 0;
}

static void
system_get_deviceid( unsigned int *ret_vendor_id,
                     unsigned int *ret_device_id )
{
    *ret_vendor_id = 0;
    *ret_device_id = 0;
}

#if ((DIRECTFB_MAJOR_VERSION * 1000000 + \
      DIRECTFB_MINOR_VERSION * 1000 + \
      DIRECTFB_MICRO_VERSION) >= 1004013)


static int
system_surface_data_size( void )
{
     /* Return zero because shared surface data is unneeded. */
     return 0;
}

static void
system_surface_data_init( CoreSurface *surface, void *data )
{
     /* Ignore since unneeded. */
    (void)surface;
    (void)data;
     return;
}

static void
system_surface_data_destroy( CoreSurface *surface, void *data )
{
     /* Ignore since unneeded. */
    (void)surface;
    (void)data;
     return;
}
#endif
