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
 * $brcm_Workfile: nexus_display_types.h $
 * $brcm_Revision: 17 $
 * $brcm_Date: 5/18/12 10:22a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/include/nexus_display_types.h $
 * 
 * 17   5/18/12 10:22a erickson
 * SW7435-165: merge
 * 
 * SW7435-165/1   5/17/12 3:25p mward
 * SW7435-165:  define NEXUS_MAX_VIDEO_ENCODERS
 * 
 * 16   3/15/12 5:11p erickson
 * SW7425-2607: increase for 7435
 * 
 * 15   3/13/12 5:16p erickson
 * SW7425-2607: increase NEXUS_MAX_DISPLAYS
 * 
 * 14   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 13   10/13/11 3:45p erickson
 * SWDEPRECATED-2425: clarify comments on NEXUS_VideoWindowContentMode
 * 
 * 12   7/8/11 4:13p erickson
 * SW7425-592: add NEXUS_DisplayAspectRatio_eSar for custom display
 *  formats
 * 
 * 11   3/4/11 1:22p erickson
 * SW7125-833: add NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection
 *
 * 10   10/5/10 10:53a erickson
 * SW7420-1135: move NEXUS_VideoWindowContentMode for external use
 *
 * 9   9/17/09 9:38a erickson
 * SW7405-3024: introduce NEXUS_DisplayAspectRatio_eAuto so that users can
 *  set 4x3 720p/1080i if desired.
 *
 * 8   10/24/08 12:49p erickson
 * PR48023: move NEXUS_AttenuationRgbSettings to nexus_display_types.h
 *
 * 7   9/18/08 11:02a erickson
 * PR47014: update blend equation enums
 *
 * 6   9/17/08 12:29p erickson
 * PR47014: expose compositor blend equation in NEXUS_VideoWindowSettings
 *  and NEXUS_GraphicsSettings
 *
 * 5   4/11/08 3:37p vsilyaev
 * PR 40921: Added SCART input
 *
 * 4   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 3   1/23/08 9:22p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 2:39p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:16p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/8   12/18/07 1:34p erickson
 * PR38213: analog video input rework
 *
 * Nexus_Devel/7   11/30/07 10:52a erickson
 * PR35457: api update
 5
 * Nexus_Devel/6   11/26/07 12:26p vsilyaev
 * PR36159: Merged 3563 code
 *
 * Nexus_Devel/PR36159/1   11/26/07 12:46a gezhang
 * PR36159: Bring up Nexus on 93563 platform
 *
 * Nexus_Devel/5   11/2/07 4:42p vsilyaev
 * PR 36696: Used connector model for VideoInput's and VideoOutput's
 *
 * Nexus_Devel/4   9/28/07 11:55a vsilyaev
 * PR 34662: Fixed API after review
 *
 * Nexus_Devel/3   9/17/07 6:48p vsilyaev
 * PR 34662: Coded more implementation
 *
 * Nexus_Devel/2   9/12/07 1:52p vsilyaev
 * PR 34419: Updated display API
 *
 * Nexus_Devel/1   9/10/07 4:34p vsilyaev
 * PR 34662: Common types for Display module
 *
 **************************************************************************/
#ifndef NEXUS_DISPLAY_TYPES_H__
#define NEXUS_DISPLAY_TYPES_H__

#include "nexus_base_types.h"
#include "nexus_video_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NEXUS_MAX_VIDEO_DACS 7
#define NEXUS_MAX_DISPLAYS 6
#define NEXUS_MAX_VIDEO_WINDOWS 2
#define NEXUS_MAX_VIDEO_ENCODERS 4

/**
Summary:
Types of the display aspect ratio
**/
typedef enum NEXUS_DisplayAspectRatio
{
    NEXUS_DisplayAspectRatio_eAuto, /* 4x3 for SD and 480p, 16x9 for HD (including 720p, 1080i, etc.) */
    NEXUS_DisplayAspectRatio_e4x3,
    NEXUS_DisplayAspectRatio_e16x9,
    NEXUS_DisplayAspectRatio_eSar,  /* sample aspect ratio. requires separate x/y. see NEXUS_DisplayCustomFormatSettings. */
    NEXUS_DisplayAspectRatio_eMax
} NEXUS_DisplayAspectRatio;

/**
Summary:
DAC's avaliable for the analog outputs
**/
typedef enum NEXUS_VideoDac
{
    NEXUS_VideoDac_eNone, /* disconnected */
    NEXUS_VideoDac_e0,
    NEXUS_VideoDac_e1,
    NEXUS_VideoDac_e2,
    NEXUS_VideoDac_e3,
    NEXUS_VideoDac_e4,
    NEXUS_VideoDac_e5,
    NEXUS_VideoDac_e6,
    NEXUS_VideoDac_eMax
} NEXUS_VideoDac;

/**
Summary:
Display handle obtained from NEXUS_Display_Open
**/
typedef struct NEXUS_Display *NEXUS_DisplayHandle;

/**
Summary:
Video window handle obtained from NEXUS_Display_GetWindow
**/
typedef struct NEXUS_VideoWindow *NEXUS_VideoWindowHandle;

/**
Summary:
Index for analog video color channel arrays.

Description:
The NEXUS_AnalogVideoDecoder interface processes video by a maximum 3 channels.

Composite uses one channel.
Svideo uses two channels.
Component uses three channels which can be configured for either YPrPb or RGB.

Use this enum as your array index to avoid hardcoding 0, 1 or 2 into your application.

Currently, the assigment of color channel to NEXUS_AnalogVideoDecoder index is not programmable. If it ever
became programmable, this enum could be replaced with a user-programmable integer index.
**/
typedef enum NEXUS_AnalogVideoChannel
{
    NEXUS_AnalogVideoChannel_eY         = 0,
    NEXUS_AnalogVideoChannel_ePr        = 1,
    NEXUS_AnalogVideoChannel_ePb        = 2,

    NEXUS_AnalogVideoChannel_eR         = 0,
    NEXUS_AnalogVideoChannel_eG         = 1,
    NEXUS_AnalogVideoChannel_eB         = 2,

    NEXUS_AnalogVideoChannel_eLuma      = 0,
    NEXUS_AnalogVideoChannel_eChroma    = 1,

    NEXUS_AnalogVideoChannel_eComposite = 0,

    NEXUS_AnalogVideoChannel_eScartR    = 0,
    NEXUS_AnalogVideoChannel_eScartG    = 1,
    NEXUS_AnalogVideoChannel_eScartB    = 2,
    NEXUS_AnalogVideoChannel_eScartY    = 0,
    NEXUS_AnalogVideoChannel_eScartPr   = 1,
    NEXUS_AnalogVideoChannel_eScartPb   = 2,
    NEXUS_AnalogVideoChannel_eScartSync = 3,
    NEXUS_AnalogVideoChannel_eScartComposite = 3,
    NEXUS_AnalogVideoChannel_eScartLuma  = 3,
    NEXUS_AnalogVideoChannel_eScartChroma = 1
} NEXUS_AnalogVideoChannel;

/**
Summary:
Factors used to blend video windows and graphics in the compositor

Description:
Used in NEXUS_VideoWindowSettings and NEXUS_GraphicsSettings.

See BVDC_Window_SetBlendFactor for a thorough explanation of video and graphics blending equations.

Compare with NEXUS_BlendFactor in nexus_graphics2d.h. There are similar, but map to separate blocks of hardware.
**/
typedef enum NEXUS_CompositorBlendFactor
{
    NEXUS_CompositorBlendFactor_eZero = 0,                /* 0 */
    NEXUS_CompositorBlendFactor_eOne,                     /* 1 */
    NEXUS_CompositorBlendFactor_eSourceAlpha,             /* alpha source */
    NEXUS_CompositorBlendFactor_eInverseSourceAlpha,      /* 1 - (alpha source) */
    NEXUS_CompositorBlendFactor_eConstantAlpha,           /* constant alpha */
    NEXUS_CompositorBlendFactor_eInverseConstantAlpha,    /* 1 - (constant alpha) */
    NEXUS_CompositorBlendFactor_eMax
} NEXUS_CompositorBlendFactor;

/***************************************************************************
Summary:
Conversion matrix settings

Description:
Parameters in the convert matrix are in RGB domain in 20.11 signed fixed point.
For example, 1 is represented by 0x800 and -1 is 0xffffffff.

Used by NEXUS_VideoWindow_SetAttenuationRgb and NEXUS_PanelOutput_SetAttenuationRgb

**************************************************************************/
typedef struct NEXUS_AttenuationRgbSettings
{
    int32_t attenuationR;      /* attenuation_R in convert matrix */
    int32_t attenuationG;      /* attenuation_G in convert matrix */
    int32_t attenuationB;      /* attenuation_B in convert matrix */
    int32_t offsetR;           /* offset_R in convert matrix */
    int32_t offsetG;           /* offset_G in convert matrix */
    int32_t offsetB;           /* offset_B in convert matrix */
} NEXUS_AttenuationRgbSettings;

/**
Summary:
Options for converting the source to the video window rectangle based on both of their aspect ratios (A/R).

Description:
The behavior of this enum depends on the aspect ratio of the video window and the source.
If the aspect ratio of the display and content match, this enum has no effect.
**/
typedef enum NEXUS_VideoWindowContentMode
{
    NEXUS_VideoWindowContentMode_eZoom,      /* cut off content to preserve aspect ratio.
        If you want to achieve different levels of zoom, you will need to use eFull and set your own manual source clipping
        using NEXUS_VideoWindowSettings. This eZoom enum implements only one flavor of zoom, which is minimal clipping. */
    NEXUS_VideoWindowContentMode_eBox,       /* add either a window box (also known as pillar box or sidebars) or a letter box to preserve aspect ratio.
        The type of box depends on the aspect ratio of the source and display.
        If source is 4x3 and display is 16x9, eBox will result in window box.
        If source is 16x9 and display is 4x3, eBox will result in letter box. */
    NEXUS_VideoWindowContentMode_ePanScan,   /* Used for 16x9 source with pan scan vectors on 4x3 display only, otherwise same as zoom.
                                                Additional A/R correct is applied after pan scan vectors. */
    NEXUS_VideoWindowContentMode_eFull,      /* distort aspect ratio but see all the content and no windowbox or letterbox. */
    NEXUS_VideoWindowContentMode_eFullNonLinear,/* Non-linear upscaling to full screen where
                                                  the edge of the content will have more distorted aspect ratio.
                                                  See NEXUS_VideoWindow_SetNonLinearScalerSettings (for some chips) for an enable boolean. */
    NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection, /* Same as ePanScan, but don't apply additional A/R correction afterwards. */
    NEXUS_VideoWindowContentMode_eMax
} NEXUS_VideoWindowContentMode;

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VIDEO_DISPLAY_H__ */
