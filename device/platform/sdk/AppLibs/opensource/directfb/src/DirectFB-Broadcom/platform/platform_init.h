/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: platform_init.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/18 $
 * $brcm_Date: 3/7/12 5:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/platform_init.h $
 *
 * DirectFB_1_4_15_Port/18   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/17   2/29/12 3:36p apandav
 * SW7552-208: DirectFB: CVBS can't output 576i due to DFB identify
 *  vertical scaling incorrectly on 7552 platform.
 *
 * DirectFB_1_4_15_Port/16   2/13/12 3:46p mnelis
 * SW7552-208: DirectFB: CVBS can't output 576i due to DFB identify
 *  vertical scaling incorrectly on 7552 platform.
 *
 * DirectFB_1_4_15_Port/15   2/9/12 2:34p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB_1_4_15_Port/14   11/25/11 2:42p robertwm
 * SW7125-1167: DirectFB-1.4.15: Cannot compile platform code if 3DTV
 *  support is not available.
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/2   12/8/11 3:00p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   11/22/11 5:19p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/13   11/21/11 5:06p robertwm
 * SW7425-1815: DirectFB-1.4.15: Cannot run DFB in multi-app mode with
 *  Phase 3.5 reference software.
 *
 * DirectFB_1_4_15_Port/12   11/15/11 1:51a christ
 * SW7346-556: DirectFB: Display fails to open on 7346 B1 using Phase 4.5
 *  refsw
 *
 * DirectFB_1_4_15_Port/11   11/8/11 3:35p christ
 * SW7425-1703: DirectFB: Move NEXUS_Display calls into platform layer
 *
 * DirectFB_1_4_15_Port/10   11/2/11 2:27p christ
 * SW7420-2121: DirectFB: Refactor input drivers (ir + keypad) to move
 *  Nexus usage into platform layer
 *
 * DirectFB_1_4_15_Port/9   10/20/11 5:55p robertwm
 * SW7231-398: DirectFB-1.4.15: Locking a pre-allocated surface does not
 *  return same virtual address.
 *
 * DirectFB_1_4_15_Port/8   10/17/11 5:28p robertwm
 * SW7231-398: DirectFB-1.4.15: Locking a pre-allocated surface does not
 *  return same virtual address.
 *
 * DirectFB_1_4_15_Port/7   10/10/11 5:01p christ
 * SW7420-2089: DirectFB: df_andi3d fails to run on 7420
 *
 * DirectFB_1_4_15_Port/6   10/6/11 4:10p christ
 * SW7425-1420: DirectFB: Access master surface pool handle from slave
 *  application
 *
 * DirectFB_1_4_15_Port/5   10/5/11 12:11p robertwm
 * SW7231-382: DirectFB: Support Creating DFB Surfaces for OpenGL
 *  rendering on the VC-4.
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_14_Port/4   9/22/11 4:44p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 * DirectFB_1_4_14_Port/3   9/13/11 4:46p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 * DirectFB_1_4_5_Port/6   9/13/11 4:02p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_5_Port/5   8/15/11 12:45p robertwm
 * SW7420-2031: DirectFB-1.4.5: Segfault when running DFB applications
 *  from limited build in different NEXUS_MODE.
 * SW7420-2033: DirectFB: need to expose Nexus VideoWindow handle in dfb
 *  platform code.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:24a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/4   7/8/11 5:01p robertwm
 * SW7630-105: DirectFB-1.4.5: dfb_platform.h has incorrect init settings
 *  for BCC.
 *
 * DirectFB_1_4_5_Port/3   7/7/11 6:43p tross
 * SWBLURAY-26478: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_5_Port/2   6/16/11 6:19p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:12p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/3   7/1/11 12:03p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/2   6/16/11 5:08p robertwm
 * SW7405-5363: DirectFB-1.4.1: Cannot build system driver due to missing
 *  Nexus 3DTV functions and structures.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/1   6/10/11 5:08p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/24   6/2/11 4:30p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/23   5/18/11 4:20p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/22   5/17/11 6:47p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/21   5/13/11 10:55p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/20   3/11/11 5:05p christ
 * SW7468-370: DirectFB: 1.4.1 release fails to compile for platforms
 *  without component output.
 *
 * DirectFB_1_4_Port/19   2/25/11 6:23p apandav
 * SW3520-44: BCM935230: Error while compiling DirectFB_1_4_Port
 *
 * DirectFB_1_4_Port/18   2/18/11 2:35p robertwm
 * SW7468-366: DirectFB: Platforms without component output should not
 *  call component functions.
 *
 * DirectFB_1_4_Port/17   2/4/11 8:40p robertwm
 * SW7400-3002: DirectFB: Support non-DFB master application in
 *  platform_init code.
 * SW7400-3000: DirectFB: Support non-DFB master application in
 *  platform_init code.
 *
 * DirectFB_1_4_Port/16   2/3/11 6:01p christ
 * SW7420-1458: DirectFB: Rework internal handling of nexus heaps.
 *
 * DirectFB_1_4_Port/15   1/25/11 8:11p robertwm
 * SW7420-1424: DirectFB: Refactor platform_init code to separate DTV and
 *  STB parts and to support NEXUS heap m'ment.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/4   1/18/11 1:26p christ
 * Fix DTV compile problems.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/3   1/17/11 5:19p christ
 * Better fix for compile issue.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/2   1/17/11 2:58p christ
 * libinit changes.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/1   12/13/10 5:23p christ
 * Initial stereo 3d commit
 *
 * DirectFB_1_4_Port/12   10/21/10 2:06p robertwm
 * SW35230-1873: DirectFB: Platform Initialisation code fails to build on
 *  DTV platforms.
 *
 * DirectFB_1_4_Port/11   10/20/10 5:11p robertwm
 * SW7420-1195: DirectFB: Move the platform_init shared handles definition
 *  to a header file for portability.
 *
 * DirectFB_1_4_Port/10   8/20/10 3:36p robertwm
 * SW7405-4791: DirectFB: DirectFB-1.4.1 Phase 2.5 fails to build with
 *  Nexus Phase 5.0 reference software.
 *
 * DirectFB_1_4_Port/9   8/11/10 4:58p robertwm
 * SW35230-957: DirectFB: platform_init fails to build on 35230 with
 *  recent 3D changes.
 *
 * DirectFB_1_4_Port/8   8/11/10 3:24a robertwm
 * SW3556-1177: DirectFB: system_initialize segfault with DirectFB-1.4.1
 *  Phase 2.5 PreRel.
 *
 * DirectFB_1_4_Port/7   8/11/10 1:58a robertwm
 * SW7405-4250: DirectFB: Screen destination rectangle and source
 *  rectangle not saved across contexts.
 * SW7405-4743: DirectFB: Multiple instances of df_andi.c do not get
 *  displayed.
 *
 * DirectFB_1_4_Port/6   8/6/10 8:04p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
 *
 * DirectFB_1_4_Port/5   4/9/10 4:55p robertwm
 * SW7405-4183: DirectFB: Add support for Texture Triangles in the
 *  graphics driver.
 *
 * DirectFB_1_4_Port/4   12/23/09 2:18p robertwm
 * SW7550-51: Support DirectFB on 7550.
 *
 * DirectFB_1_4_Port/SW7550-51/1   12/22/09 3:22p mphillip
 * SW7550-51: Add 7550 build support (on branch)
 *
 * DirectFB_1_4_Port/3   12/17/09 5:24p robertwm
 * SW3556-944: DirectFB: Increase size of graphics heap to 16MB for DTV
 *  platforms.
 *
 * DirectFB_1_4_Port/2   8/7/09 3:43p robertwm
 * PR56902: Require single graphics memory heap to avoid fragmentation.
 * PR56721: Add HDMI output support.
 *
 * DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
 * PR56658: Add DirectFB 1.4.1 broadcom_files
 *
 * 6   11/21/08 8:06p vsilyaev
 * PR 49524: Added graphics2d_sync routine that could be used by different
 *  users of Nexus_Graphics2D
 *
 * 5   11/17/08 3:30p gmohile
 * PR 48730 : Expose surface handle to app
 *
 * 4   10/24/08 12:42p gmohile
 * PR 46490 : Add hw picture decode support
 *
 * 3   10/14/08 3:53p gmohile
 * PR 45243 : Fix nexus handles
 *
 * 2   9/15/08 11:49a gmohile
 * PR 46734 : Add dual output support
 *
 * 1   8/29/08 2:35p gmohile
 * PR 45243 : Do nexus platform init and display open outside directfb
 *
 *****************************************************************************/
#ifndef __BCMNEXUS__PLATFORM_INIT_H__
#define __BCMNEXUS__PLATFORM_INIT_H__

#include "gfxdrivers/bcmnexus/bcmnexus_gfx_defines.h"
#include "systems/bcmnexus/bcmnexus_sys_defines.h"
#include "dfb_platform.h"

#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_memory.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#ifdef NEXUS_HAS_PICTURE_DECODER
#include "nexus_picture_decoder.h"
#endif
#if NEXUS_HAS_GRAPHICS2D || NEXUS_HAS_ZSP_GRAPHICS
#include "nexus_graphics2d.h"
#endif
#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
#if NEXUS_GRAPHICS3D_MODE_PROXY
#include "p3d_nexus_proxy.h"
#endif
#include "nexus_graphics3d.h"
#include <priv/nexus_core.h>
#include <bp3d.h>
#include <nexus_graphics3d_init.h>
#include <nexus_graphics3d_priv.h>
#define NEXUS_THUNK_LAYER
#include <nexus_graphics2d_module.h>
#include <nexus_surface_priv.h>
#endif

#if NEXUS_HAS_SURFACE_COMPOSITOR
#include <nexus_surface_compositor.h>
#endif

#include "platform/bcmnexus_ir_inputmode.h"

#include <string.h>

/* Nexus surface compositor mode definitions */
#define BCMNEXUS_NSC_MULTIBUFFER_MODE 1
#define BCMNEXUS_NSC_TUNNELLED_MODE   2

#define PLATFORM_SETTINGS_VALID 0xADFBFEED

typedef void (*output_callback_t)(NEXUS_DisplayHandle display, int output, bool connected);

#define BDBG_MSG_TRACE(x)   BDBG_MSG(x)

#if (NEXUS_NUM_DISPLAYS <= 1) || BCMNEXUS_NSC_SUPPORT
#define NUM_DISPLAYS 1
#else
#define NUM_DISPLAYS DFB_PLATFORM_NUM_DISPLAYS_MAX
#endif

#define DEFAULT_GFX_HEAP_SIZE (16 * 1024 * 1024)

/* Includes graphics window and video windows */
#define NUM_GFX_LAYERS_PER_DISPLAY  1
#define NUM_VID_LAYERS_PER_DISPLAY  NEXUS_NUM_VIDEO_WINDOWS
#define NUM_LAYERS_PER_DISPLAY      (NUM_GFX_LAYERS + NUM_VID_LAYERS)
#define NUM_LAYERS                  (NUM_LAYERS_PER_DISPLAY * NUM_DISPLAYS)

#define BCMNEXUS_DFB_SHMEM_KEY      5678

/**
Summary:
Function prototype used by DFB_PlatformCallbackDesc.
**/
typedef void (*DFB_PlatformCallback)(void *context, int param);

/*
Summary:
Standard definition of a callback in DFB Platform code.
*/
typedef struct DFB_PlatformCallbackDesc {
    DFB_PlatformCallback            callback;       /* Function pointer */
    void                           *context;        /* First parameter to callback function. */
    int                             param;          /* Second parameter to callback function. */
} DFB_PlatformCallbackDesc;

/* Ensure that the structure begins the same as DFB_PlatformDisplaySettings! */
typedef struct DFB_Platform_P_DisplaySettings
{
    DFB_PlatformVideoFormat         format;         /* Output format of video display */
    DFB_PlatformDisplayAspectRatio  aspectRatio;    /* Aspect ratio of display, if applicable */
    DFB_PlatformStereoscopicMode    stereoMode;     /* Stereoscopic display mode */
    DFB_PlatformColor               background;     /* Background color in DFB DFB_PLATFORM_PIXELFORMAT_ARGB colorspace */

    DFB_PlatformCallbackDesc        vsyncCallback;  /* Vsync callback */
} DFB_Platform_P_DisplaySettings;

/* Shared in memory */
typedef struct DFB_P_DisplayStatus
{
    DFB_PlatformNexusHandle         displayHandle;
    DFB_Platform_P_DisplaySettings  displaySettings;
    DFB_PlatformDimension           displaySize;
} DFB_P_DisplayStatus;

/* Shared in memory */
typedef struct DFB_P_DisplayOutputStatus
{
    DFB_PlatformNexusHandle         outputHandle;
    DFB_PlatformDisplayOutputType   outputType;
    int                             outputIndex;
    int                             displayIndex;
} DFB_P_DisplayOutputStatus;

/* Shared in memory */
typedef struct DFB_P_DisplayWindowStatus
{
    DFB_PlatformNexusHandle         windowHandle;
    int                             windowIndex;
    int                             displayIndex;
} DFB_P_DisplayWindowStatus;

/* Shared in memory */
typedef struct DFB_P_DisplaySettings
{
    DFB_P_DisplayStatus             display[NUM_DISPLAYS];
    DFB_P_DisplayOutputStatus       output[DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX];
    DFB_P_DisplayWindowStatus       window[DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX];
} DFB_P_DisplaySettings;

/* Not shared in memory */
typedef struct DFB_P_DeviceStatus
{
    bool                            IsDisplayOpened[NUM_DISPLAYS];
    bool                            IsWindowOpened[DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX];
    bool                            IsGraphics2DOpened;
    bool                            IsGraphics3DOpened;
    bool                            IsPictureDecoderOpened;
} DFB_P_DeviceStatus;

/**
Summary:
Orientation of video
**/
typedef enum DFB_Platform_P_VideoOrientation {
        DFB_Platform_P_VideoOrientation_e2D,            /* 2D */
        DFB_Platform_P_VideoOrientation_e3D_LeftRight,  /* 3D left right */
        DFB_Platform_P_VideoOrientation_e3D_OverUnder,  /* 3D over under */
        DFB_Platform_P_VideoOrientation_e3D_Left,       /* 3D left */
        DFB_Platform_P_VideoOrientation_e3D_Right,      /* 3D right */
        DFB_Platform_P_VideoOrientation_eMax
} DFB_Platform_P_VideoOrientation;

/*
Summary:
Description of the stereo (Left/Right) framebuffer
**/
typedef struct DFB_Platform_P_DisplayGraphicsFramebuffer3D {
    DFB_Platform_P_VideoOrientation orientation;    /* orientation of the graphics framebuffer */
    DFB_PlatformNexusHandle         main;           /* mandatory, main surface */
    DFB_PlatformNexusHandle         alpha;          /* optional, alpha surface */
    DFB_PlatformNexusHandle         right;          /* optional, right surface */
    DFB_PlatformNexusHandle         rightAlpha;     /* optional, right alpha surface */
    uint32_t                        alphaW0;        /* used as pixel alpha when pixel's W is 0 */
    uint32_t                        alphaW1;        /* used as pixel alpha when pixel's W is 1 */
} DFB_Platform_P_DisplayGraphicsFramebuffer3D;


/**
Summary:
General purpose rectangle
**/
typedef struct DFB_PlatformRect
{
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} DFB_PlatformRect;

typedef enum DFB_PlatformBlendFactor
{
    DFB_PlatformBlendFactor_eZero = 0,                /* 0 */
    DFB_PlatformBlendFactor_eOne,                     /* 1 */
    DFB_PlatformBlendFactor_eSourceAlpha,             /* alpha source */
    DFB_PlatformBlendFactor_eInverseSourceAlpha,      /* 1 - (alpha source) */
    DFB_PlatformBlendFactor_eConstantAlpha,           /* constant alpha */
    DFB_PlatformBlendFactor_eInverseConstantAlpha,    /* 1 - (constant alpha) */
    DFB_PlatformBlendFactor_eMax
} DFB_PlatformBlendFactor;

typedef enum DFB_PlatformFilterCoeffs
{
    DFB_PlatformFilterCoeffs_ePointSample,      /* Point sampled filtering */
    DFB_PlatformFilterCoeffs_eBilinear,         /* Bilinear filtering */
    DFB_PlatformFilterCoeffs_eAnisotropic,      /* Anisotropic filtering */
    DFB_PlatformFilterCoeffs_eMax
} DFB_PlatformFilterCoeffs;

/*
Summary:
Description of the graphics settings
**/
typedef struct DFB_Platform_P_GraphicsSettings
{
    bool enabled;
    uint8_t alpha;
    unsigned zorder;

    bool chromakeyEnabled;
    DFB_PlatformColor lowerChromakey;
    DFB_PlatformColor upperChromakey;

    DFB_PlatformRect position;
    DFB_PlatformRect clip;

    DFB_PlatformCallbackDesc frameBufferCallback;
    DFB_PlatformBlendFactor  sourceBlendFactor;
    DFB_PlatformBlendFactor  destBlendFactor;
    uint8_t constantAlpha;

    DFB_PlatformFilterCoeffs horizontalFilter;
    DFB_PlatformFilterCoeffs verticalFilter;

    int stereoOffset;
} DFB_Platform_P_GraphicsSettings;

/*
Summary:
Pixel formats supported by the platform layer.
**/
typedef enum {
     /* unknown or unspecified format */
     DFB_PLATFORM_PIXELFORMAT_UNKNOWN = 0x00000000,

     /* 16 bit  ARGB (2 byte, alpha 1@15, red 5@10, green 5@5, blue 5@0) */
     DFB_PLATFORM_PIXELFORMAT_ARGB1555,

     /* 16 bit   RGB (2 byte, red 5@11, green 6@5, blue 5@0) */
     DFB_PLATFORM_PIXELFORMAT_RGB16,

     /* 24 bit   RGB (3 byte, red 8@16, green 8@8, blue 8@0) */
     DFB_PLATFORM_PIXELFORMAT_RGB24,

     /* 24 bit   RGB (4 byte, nothing@24, red 8@16, green 8@8, blue 8@0) */
     DFB_PLATFORM_PIXELFORMAT_RGB32,

     /* 32 bit  ARGB (4 byte, alpha 8@24, red 8@16, green 8@8, blue 8@0) */
     DFB_PLATFORM_PIXELFORMAT_ARGB,

     /*  8 bit alpha (1 byte, alpha 8@0), e.g. anti-aliased glyphs */
     DFB_PLATFORM_PIXELFORMAT_A8,

     /* 16 bit   YUV (4 byte/ 2 pixel, macropixel contains CbYCrY [31:0]) */
     DFB_PLATFORM_PIXELFORMAT_YUY2,

     /*  8 bit   RGB (1 byte, red 3@5, green 3@2, blue 2@0) */
     DFB_PLATFORM_PIXELFORMAT_RGB332,

     /* 16 bit   YUV (4 byte/ 2 pixel, macropixel contains YCbYCr [31:0]) */
     DFB_PLATFORM_PIXELFORMAT_UYVY,

     /* 12 bit   YUV (8 bit Y plane followed by 8 bit quarter size U/V planes) */
     DFB_PLATFORM_PIXELFORMAT_I420,

     /* 12 bit   YUV (8 bit Y plane followed by 8 bit quarter size V/U planes) */
     DFB_PLATFORM_PIXELFORMAT_YV12,

     /*  8 bit   LUT (8 bit color and alpha lookup from palette) */
     DFB_PLATFORM_PIXELFORMAT_LUT8,

     /*  8 bit  ALUT (1 byte, alpha 4@4, color lookup 4@0) */
     DFB_PLATFORM_PIXELFORMAT_ALUT44,

     /* 32 bit  ARGB (4 byte, inv. alpha 8@24, red 8@16, green 8@8, blue 8@0) */
     DFB_PLATFORM_PIXELFORMAT_AiRGB,

     /*  1 bit alpha (1 byte/ 8 pixel, most significant bit used first) */
     DFB_PLATFORM_PIXELFORMAT_A1,

     /* 12 bit   YUV (8 bit Y plane followed by one 16 bit quarter size Cb|Cr [7:0|7:0] plane) */
     DFB_PLATFORM_PIXELFORMAT_NV12,

     /* 16 bit   YUV (8 bit Y plane followed by one 16 bit half width Cb|Cr [7:0|7:0] plane) */
     DFB_PLATFORM_PIXELFORMAT_NV16,

     /* 16 bit  ARGB (2 byte, alpha 2@14, red 5@9, green 5@4, blue 4@0) */
     DFB_PLATFORM_PIXELFORMAT_ARGB2554,

     /* 16 bit  ARGB (2 byte, alpha 4@12, red 4@8, green 4@4, blue 4@0) */
     DFB_PLATFORM_PIXELFORMAT_ARGB4444,

     /* 16 bit  RGBA (2 byte, red 4@12, green 4@8, blue 4@4, alpha 4@0) */
     DFB_PLATFORM_PIXELFORMAT_RGBA4444,

     /* 12 bit   YUV (8 bit Y plane followed by one 16 bit quarter size Cr|Cb [7:0|7:0] plane) */
     DFB_PLATFORM_PIXELFORMAT_NV21,

     /* 32 bit  AYUV (4 byte, alpha 8@24, Y 8@16, Cb 8@8, Cr 8@0) */
     DFB_PLATFORM_PIXELFORMAT_AYUV,

     /*  4 bit alpha (1 byte/ 2 pixel, more significant nibble used first) */
     DFB_PLATFORM_PIXELFORMAT_A4,

     /*  1 bit alpha (3 byte/  alpha 1@18, red 6@12, green 6@6, blue 6@0) */
     DFB_PLATFORM_PIXELFORMAT_ARGB1666,

     /*  6 bit alpha (3 byte/  alpha 6@18, red 6@12, green 6@6, blue 6@0) */
     DFB_PLATFORM_PIXELFORMAT_ARGB6666,

     /*  6 bit   RGB (3 byte/   red 6@12, green 6@6, blue 6@0) */
     DFB_PLATFORM_PIXELFORMAT_RGB18,

     /*  2 bit   LUT (1 byte/ 4 pixel, 2 bit color and alpha lookup from palette) */
     DFB_PLATFORM_PIXELFORMAT_LUT2,

     /* 16 bit   RGB (2 byte, nothing @12, red 4@8, green 4@4, blue 4@0) */
     DFB_PLATFORM_PIXELFORMAT_RGB444,

     /* 16 bit   RGB (2 byte, nothing @15, red 5@10, green 5@5, blue 5@0) */
     DFB_PLATFORM_PIXELFORMAT_RGB555,

     /* 16 bit   BGR (2 byte, nothing @15, blue 5@10, green 5@5, red 5@0) */
     DFB_PLATFORM_PIXELFORMAT_BGR555,

     /*  4 bit   LUT (1 byte/ 2 pixel, 4 bit color and alpha lookup from palette) */
     DFB_PLATFORM_PIXELFORMAT_LUT4,

     /* 16 bit  RGBA (2 byte, red 5@11, green 5@6, blue 5@1, alpha 1@0) */
     DFB_PLATFORM_PIXELFORMAT_RGBA5551,

     /* 24 bit full YUV planar (8 bit Y plane followed by an 8 bit Cb and an
        8 bit Cr plane) */
     DFB_PLATFORM_PIXELFORMAT_YUV444P,

     /* 24 bit  ARGB (3 byte, alpha 8@16, red 5@11, green 6@5, blue 5@0) */
     DFB_PLATFORM_PIXELFORMAT_ARGB8565,

     /* 32 bit  AVYU 4:4:4 (4 byte, alpha 8@24, Cr 8@16, Y 8@8, Cb 8@0) */
     DFB_PLATFORM_PIXELFORMAT_AVYU,

     /* 24 bit   VYU 4:4:4 (3 byte, Cr 8@16, Y 8@8, Cb 8@0) */
     DFB_PLATFORM_PIXELFORMAT_VYU,

     /*  1 bit alpha (1 byte/ 8 pixel, LEAST significant bit used first) */
     DFB_PLATFORM_PIXELFORMAT_A1_LSB,

     /* 16 bit   YUV (8 bit Y plane followed by 8 bit 2x1 subsampled V/U planes) */
     DFB_PLATFORM_PIXELFORMAT_YV16,

     /* 32 bit  ABGR (4 byte, alpha 8@24, blue 8@16, green 8@8, red 8@0) */
     DFB_PLATFORM_PIXELFORMAT_ABGR,

     /*  16 bit   LUT (1 byte alpha and 8 bit color lookup from palette) */
     DFB_PLATFORM_PIXELFORMAT_ALUT8,

} DFB_PlatformPixelFormat;

typedef void *DFB_Platform_P_PoolHandle;

/*
Summary:
Description of data types accessable from the platform layer shared memory
**/
typedef enum DFB_PlatformSharedSetting {
    DFB_PlatformSharedSetting_eSurfacePool,
    DFB_PlatformSharedSetting_e3DGraphicsPool,
    DFB_PlatformSharedSetting_eMax
} DFB_PlatformSharedSetting;

typedef struct DFB_P_PlatformSharedData{
    unsigned int surfacePool;
    unsigned int graphics3DPool;
} DFB_P_PlatformSharedData;

/*
Summary:
Description of the input device settings
**/
typedef struct DFB_PlatformInputDeviceSettings {
    unsigned repeatFilterTime;              /* Key Repeat filter time (in ms) -- keys less than this amount apart will be sent as repeats
                                               If 0, repeats will be filtered in hardware only (if available) */
    unsigned eventQueueSize;                /* Max number of events that can be queued before an overflow */
    DFB_PlatformCallbackDesc dataReady;     /* Data ready callback */

    union
    {
        struct
        { /* IR specific */
            DFB_PlatformIrInputMode mode;   /* IR Mode */
            unsigned channel_number;        /* IR Channel Number. Defaults to 0. To open up multiple CIRC devices you must
                                               select two different IR channels. */
        } ir;

        struct
        { /* Keypad specific */
            uint32_t prescale;              /* 27mhz clock prescale value */
            uint8_t debounce;               /* debounce count */
        } keypad;

    } device;

} DFB_PlatformInputDeviceSettings;

typedef struct DFB_PlatformInputDeviceEvent
{
    bool repeat;        /* true if this is a repeated key */
    uint32_t code;      /* code bits 0-31 from the receiver */
    uint32_t codeHigh;  /* code bits 32-47 from the receiver, if available. */
    unsigned interval;  /* Time interval between SW ISR for this event and the previous event, in units of milliseconds */
    bool  down;
    unsigned  x;
    unsigned  y;
    uint32_t button;
} DFB_PlatformInputDeviceEvent;

typedef enum DFB_PlatformInputDeviceType
{
    DFB_PlatformInputDevice_eInfraRed,
    DFB_PlatformInputDevice_eKeypad,
    DFB_PlatformInputDevice_eKeyboard,
    DFB_PlatformInputDevice_ePointer,
    DFB_PlatformInputDevice_eMax

} DFB_PlatformInputDeviceType;

typedef enum InputEventType
{
    EVENT_KEY = 0,
    EVENT_ABS,
    EVENT_REL,
    EVENT_MAX

}InputEventType;

typedef struct DFB_PlatformShared
{
    DFB_PlatformSettings dfb_platform_shared;
    DFB_PlatformStatus dfb_platform_status_shared;
    DFB_P_DisplaySettings dfb_p_display_shared;
    DFB_P_PlatformSharedData dfb_p_platform_shared_data;
} DFB_PlatformShared;

/** Helper functions ***/
extern const NEXUS_VideoFormat
DFB_Platform_P_DFBToNexusVideoFormatTable[];

extern const NEXUS_VideoFormat
DFB_Platform_P_NexusToDFBVideoFormatTable[];

extern const NEXUS_DisplayType
DFB_Platform_P_DFBToNexusDisplayTypeTable[];

extern const DFB_PlatformDisplayType
DFB_Platform_P_NexusToDFBDisplayTypeTable[];

extern const NEXUS_Timebase
DFB_Platform_P_DFBToNexusTimebaseTable[];

extern const NEXUS_AspectRatio
DFB_Platform_P_DFBToNexusDisplayAspectRatioTable[];

extern const DFB_PlatformDisplayAspectRatio
DFB_Platform_P_NexusToDFBDisplayAspectRatioTable[];

extern DFB_PlatformColor
DFB_Platform_P_NexusToDFB_PlatformColor( uint32_t color );

extern uint32_t
DFB_Platform_P_DFBToNexusColor( DFB_PlatformColor color );

extern NEXUS_DisplayType
DFB_Platform_P_DFBToNexusDisplayType( DFB_PlatformDisplayType type );

extern DFB_PlatformDisplayType
DFB_Platform_P_NexusToDFBDisplayType( NEXUS_DisplayType type );

extern NEXUS_VideoFormat
DFB_Platform_P_DFBToNexusVideoFormat( DFB_PlatformVideoFormat format );

extern DFB_PlatformVideoFormat
DFB_Platform_P_NexusToDFBVideoFormat( NEXUS_VideoFormat format );

extern NEXUS_AspectRatio
DFB_Platform_P_DFBToNexusDisplayAspectRatio( DFB_PlatformDisplayAspectRatio ratio );

extern DFB_PlatformDisplayAspectRatio
DFB_Platform_P_NexusToDFBDisplayAspectRatio( NEXUS_AspectRatio ratio );

extern NEXUS_Timebase
DFB_Platform_P_DFBToNexusTimebase ( DFB_PlatformTimebase) ;

extern int
DFB_Platform_P_GetSharedMemSize( void );

extern int
DFB_Platform_P_GetDisplayIndex( DFB_PlatformNexusHandle displayHandle );

extern DFB_PlatformNexusHandle
DFB_Platform_P_GetDisplayHandle( int displayIndex);

extern DFB_PlatformResult
DFB_Platform_P_GetDisplaySize( int displayIndex, DFB_PlatformDimension *pSize);

extern void
DFB_Platform_P_DFBFormatToDisplaySize( DFB_PlatformVideoFormat format, DFB_PlatformDimension *pSize );

extern DFB_PlatformResult
DFB_Platform_P_GetDisplayOutputStatus( DFB_PlatformDisplayOutputType outputType, int outputIndex);

extern DFB_PlatformNexusHandle
DFB_Platform_P_GetDisplayOutputHandle( DFB_PlatformNexusHandle displayHandle, DFB_PlatformDisplayOutputType outputType, int outputIndex );

extern DFB_PlatformResult
DFB_Platform_P_SetDisplayOutputHandle( DFB_PlatformNexusHandle displayHandle, DFB_PlatformDisplayOutputType outputType,
                                       int outputIndex, DFB_PlatformNexusHandle outputHandle);

extern int
DFB_Platform_P_GetDisplayWindowSlot( DFB_PlatformNexusHandle windowHandle );

extern DFB_PlatformNexusHandle
DFB_Platform_P_GetHeap( void *pMemory, bool isMaster );

extern void
DFB_Platform_P_DisplayHeaps( DFB_PlatformSettings *pSettings );

extern unsigned long
DFB_Platform_P_GetGraphicsHeapSize(void);

/*** Private functions ***/
extern DFB_PlatformResult
DFB_Platform_P_GetStatus( DFB_PlatformStatus *pStatus );

extern DFB_PlatformResult
DFB_Platform_P_GetSettings( DFB_PlatformSettings *pSettings );

extern DFB_PlatformNexusHandle
DFB_Platform_P_GetGraphicsHeap( void *pMemory );

extern bool
DFB_Platform_P_Display_GetDefaultInitSettings( unsigned displayIndex, DFB_PlatformDisplayInitSettings *pSettings );

extern DFB_PlatformResult
DFB_Platform_P_Display_Init(unsigned displayIndex, DFB_PlatformDisplayInitSettings *pDisplaySettings, DFB_PlatformNexusHandle *pHandle);

extern void
DFB_Platform_P_Display_Uninit( DFB_PlatformNexusHandle handle );

extern void DFB_Platform_P_FreeLocalSurface(void);

#if (!BCMNEXUS_VDC_SUPPORT_3D_VIDEO || !BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT) || (defined(HUMAX_PLATFORM_BASE))	/* to support 3D surface */
extern DFB_PlatformResult
DFB_Platform_P_BlitComposite3DFramebuffer( DFB_PlatformNexusHandle graphics2dHandle, DFB_PlatformNexusHandle display, DFB_Platform_P_DisplayGraphicsFramebuffer3D *frameBuffer3D, bool stereoLayer, int z);
#endif

extern void
DFB_Platform_P_GetDefaultDisplayGraphicsFramebuffer3D( DFB_Platform_P_DisplayGraphicsFramebuffer3D *pFrameBuffer3D );

extern DFB_PlatformResult
DFB_Platform_P_SetDisplayGraphicsFramebuffer3D( DFB_PlatformNexusHandle display, const DFB_Platform_P_DisplayGraphicsFramebuffer3D *pFrameBuffer3D, const DFB_PlatformRect *pLeftUpdateRect, const DFB_PlatformRect *pRightUpdateRect, bool singleBuffered);

extern DFB_PlatformResult
DFB_Platform_P_ClearDisplayGraphicsFramebuffer3D( DFB_PlatformNexusHandle display);

extern bool
DFB_Platform_P_DisplayWindow_GetDefaultInitSettings( unsigned windowSlot, DFB_PlatformDisplayWindowInitSettings *pSettings );

extern DFB_PlatformResult
DFB_Platform_P_DisplayWindow_Init( DFB_PlatformDisplayWindowInitSettings *pSettings, DFB_PlatformNexusHandle *pHandle );

extern void
DFB_Platform_P_DisplayWindow_Uninit( DFB_PlatformNexusHandle handle );

extern bool
DFB_Platform_P_DisplayOutput_GetDefaultInitSettings( unsigned outputSlot, DFB_PlatformDisplayOutputInitSettings *pSettings );

extern DFB_PlatformResult
DFB_Platform_P_DisplayOutput_Init(DFB_PlatformDisplayOutputInitSettings *pSettings, DFB_PlatformNexusHandle *pHandle);

extern bool
DFB_Platform_P_Graphics2D_GetDefaultInitSettings( DFB_PlatformGraphics2DInitSettings *pSettings );

extern DFB_PlatformResult
DFB_Platform_P_Graphics2D_Init( DFB_PlatformGraphics2DInitSettings *pSettings, DFB_PlatformNexusHandle *pHandle );

extern void
DFB_Platform_P_Graphics2D_Uninit( DFB_PlatformNexusHandle handle );

extern bool
DFB_Platform_P_Graphics3D_GetDefaultInitSettings( DFB_PlatformGraphics3DInitSettings *pSettings );

extern DFB_PlatformResult
DFB_Platform_P_Graphics3D_Init( DFB_PlatformGraphics3DInitSettings *pSettings, DFB_PlatformNexusHandle *pHandle );

extern void
DFB_Platform_P_Graphics3D_Uninit( DFB_PlatformNexusHandle handle );

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
extern BP3D_Handle
DFB_Platform_P_Graphics3D_GetP3D(void);

extern BMEM_Handle
DFB_Platform_P_Graphics3D_GetBMEM(void);

extern void
DFB_Platform_P_Graphics3D_GetBSUR( DFB_PlatformNexusHandle hSurface, BSUR_Surface_Handle *phSur );

extern void
DFB_Platform_P_Graphics3D_OpenP3D( BP3D_Handle *phP3d );

extern void
DFB_Platform_P_Graphics3D_CloseP3D( BP3D_Handle hP3d );
#endif

extern DFB_PlatformResult
DFB_Platform_P_Graphics3D_PoolInit( DFB_Platform_P_PoolHandle *pHandle );

extern void
DFB_Platform_P_Graphics3D_PoolUninit( DFB_Platform_P_PoolHandle handle );

extern DFB_PlatformResult
DFB_Platform_P_Graphics3D_GetBufferSize( DFB_Platform_P_PoolHandle poolHandle, int width, int height,
                                         DFB_PlatformPixelFormat pixelformat, int  *pitch, int *size );

extern bool
DFB_Platform_P_PictureDecoder_GetDefaultInitSettings( DFB_PlatformPictureDecoderInitSettings *pSettings );

extern DFB_PlatformResult
DFB_Platform_P_PictureDecoder_Init( DFB_PlatformPictureDecoderInitSettings *pSettings, DFB_PlatformNexusHandle *pHandle );

extern void
DFB_Platform_P_PictureDecoder_Uninit( DFB_PlatformNexusHandle handle );

/*** API implementation functions ***/
extern void
DFB_Platform_P_SetHeapHandles( DFB_PlatformSettings *pSettings );

extern void
DFB_Platform_P_GetDefaultSettings( DFB_PlatformSettings *pSettings );

extern DFB_PlatformResult
DFB_Platform_P_GetDisplaySettings( DFB_PlatformNexusHandle displayHandle, DFB_Platform_P_DisplaySettings *pSettings);

extern DFB_PlatformResult
DFB_Platform_P_SetDisplaySettings( DFB_PlatformNexusHandle displayHandle, DFB_Platform_P_DisplaySettings *pSettings);

extern DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput(DFB_PlatformNexusHandle displayHandle, DFB_PlatformDisplayOutputType outputType, int outputIndex);

extern DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput(DFB_PlatformNexusHandle displayHandle, DFB_PlatformDisplayOutputType outputType, int outputIndex);

extern DFB_PlatformResult
DFB_Platform_P_GetDisplayOutput(DFB_PlatformNexusHandle displayHandle, DFB_PlatformDisplayOutputType outputType,
                                int outputIndex, DFB_PlatformNexusHandle *pHandle);

extern DFB_PlatformResult
DFB_Platform_P_SetDisplayOutput(DFB_PlatformNexusHandle displayHandle, DFB_PlatformDisplayOutputType outputType,
                                int outputIndex, DFB_PlatformNexusHandle handle);

extern DFB_PlatformResult
DFB_Platform_P_HdmiHotplugHandler(DFB_PlatformNexusHandle displayHandle, DFB_PlatformNexusHandle hdmiHandle);

extern DFB_PlatformResult DFB_Platform_P_Init(DFB_PlatformSettings *pSettings, DFB_P_DisplaySettings* pPrivSettings);

extern DFB_PlatformResult DFB_Platform_P_Uninit(void );

extern DFB_PlatformResult DFB_Platform_P_GetSharedSettings(DFB_PlatformSharedSetting setting, void* pValue);

extern DFB_PlatformResult DFB_Platform_P_SetSharedSettings(DFB_PlatformSharedSetting setting, void* pValue);

extern DFB_PlatformResult DFB_PlatformInputDevice_GetDefaultSettings(
    DFB_PlatformInputDeviceType      device,
    DFB_PlatformInputDeviceSettings *pSettings
    );

extern DFB_PlatformNexusHandle DFB_PlatformInputDevice_Open(
    DFB_PlatformInputDeviceType              device,
    unsigned int                             index,
    const DFB_PlatformInputDeviceSettings   *pSettings
    );

extern DFB_PlatformResult DFB_PlatformInputDevice_Close(
    DFB_PlatformInputDeviceType device,
    DFB_PlatformNexusHandle handle
    );

extern DFB_PlatformResult DFB_PlatformInputDevice_GetEvents(
    DFB_PlatformInputDeviceType   device,
    DFB_PlatformNexusHandle       handle,
    DFB_PlatformInputDeviceEvent *pEvents,        /* [out] attr{nelem=numEvents;nelem_out=pNumEventsRead;reserved=4} Pointer to an array of events */
    unsigned int                  numEvents,      /* Size of the event array */
    unsigned int                 *pNumEventsRead, /* [out] Number of events actually read */
    bool                         *pOverflow       /* [out] Has an overflow occurred? */
    );

extern DFB_PlatformResult DFB_Platform_P_StartIpcServer(void);

extern void DFB_Platform_P_StopIpcServer(void);

extern DFB_PlatformResult DFB_Platform_P_GetShared(DFB_PlatformShared *shm);

/**
Summary:
Get current graphics compositing settings.
**/
extern DFB_PlatformResult DFB_Platform_P_GetGraphicsSettings(
    DFB_PlatformNexusHandle display,
    DFB_Platform_P_GraphicsSettings *pSettings /* [out] */
    );

/**
Summary:
Set graphics compositing settings.
**/
extern DFB_PlatformResult DFB_Platform_P_SetGraphicsSettings(
    DFB_PlatformNexusHandle display,
    const DFB_Platform_P_GraphicsSettings *pSettings
    );

extern bool DFB_Platform_P_CheckGfxVScl( int displayIndex );

#endif
