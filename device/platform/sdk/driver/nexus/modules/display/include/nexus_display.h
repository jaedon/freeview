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
 * $brcm_Workfile: nexus_display.h $
 * $brcm_Revision: 62 $
 * $brcm_Date: 10/11/12 12:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/include/nexus_display.h $
 * 
 * 62   10/11/12 12:25p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
 *  added object verification
 * 
 * 61   9/13/12 4:23p erickson
 * SW7425-3896: deprecate NEXUS_GraphicsFramebuffer3D.rightAlpha
 * 
 * 60   7/30/12 11:19a erickson
 * SW7425-3573: add specific error codes fro NEXUS_Display_AddOutput and
 *  RemoveOutput
 * 
 * 59   7/12/12 4:03p erickson
 * SW7405-5488: add NEXUS_GetDisplayCapabilities
 * 
 * 58   4/13/12 11:38a erickson
 * SW7425-1581: fix comment
 * 
 * 57   1/27/12 11:55a erickson
 * SW7425-2139: extend NEXUS_DisplayStatus
 *
 * 56   1/12/12 12:02p jtna
 * SW7425-1875: merge NEXUS_DisplaySettings.display3DSettings.sourceBuffer
 *
 * 55   10/27/11 3:32p erickson
 * SW7425-1581: move crcQueueSize to NEXUS_DisplaySettings
 *
 * 54   9/29/11 12:37p erickson
 * SW7425-1320: change comment for NEXUS_DisplayTimingGenerator_eEncoder
 *
 * 53   9/8/11 2:18p erickson
 * SW7425-1267: add NEXUS_GraphicsSettings.horizontalCoeffIndex and
 *  .verticalCoeffIndex
 *
 * 52   7/19/11 11:14a erickson
 * SW7408-291: move vf filter code to video_output
 *
 * 51   6/30/11 2:18p jtna
 * SW7425-630: change .rightViewOffset from unsigned to int since
 *  BVDC_Window_SetDstRightRect() prototype changed
 *
 * 50   6/28/11 9:47a erickson
 * SW7405-5366: add NEXUS_Display_GetStatus
 *
 * 49   6/23/11 3:36p erickson
 * SW7408-291: add NEXUS_Display_SetVfFilter
 *
 * 48   5/31/11 9:19a erickson
 * SW7425-652: update comments on NEXUS_DisplayStgSettings
 *
 * 47   5/27/11 2:42p erickson
 * SW7425-652: add NEXUS_DisplayStgSettings
 *
 * 46   4/22/11 3:15p vsilyaev
 * SW7425-416: Updated 3D display API
 *
 * 45   4/19/11 1:54p erickson
 * SW7405-5224: add NEXUS_VideoWindowSettings.preferSyncLock and
 *  NEXUS_VideoWindowStatus.isSyncLocked
 *
 * 44   4/5/11 9:46a erickson
 * SWDEPRECATED-2425: update comments
 *
 * 43   3/11/11 4:13p erickson
 * SW7346-103: add support for CMP CRC capture
 *
 * 42   2/14/11 5:00p erickson
 * SW7425-64: move NEXUS_Display_SetCustomFormatSettings to
 *  nexus_display_custom.h
 *
 * 41   12/14/10 3:25p erickson
 * SW7420-1285: add null_allowed attribute
 *
 * 40   12/1/10 4:27p erickson
 * SW7550-577: add NEXUS_DisplayTimingGenerator
 *
 * 39   11/9/10 2:49p erickson
 * SW35230-1899: add NEXUS_GraphicsSettings.visible
 *
 * 38   8/26/10 9:32a erickson
 * SW3548-3069: clarify NEXUS_Display_SetCustomFormatSettings usage
 *
 * 37   8/9/10 5:42a erickson
 * SW35230-896: add NEXUS_GraphicsSettings.verticalFilter
 *
 * 36   6/25/10 12:12p erickson
 * SW7405-4515: add NEXUS_Display_SetGraphicsColorSettings. allow
 *  NEXUS_Display_SetGraphicsColorMatrix with NULL to unset matrix.
 *
 * 35   6/1/10 3:57p erickson
 * SW3548-2962: add NEXUS_DisplayModule_SetConfigurationId
 *
 * 34   5/13/10 10:01a erickson
 * SW7405-4326: deprecate NEXUS_GraphicsSettings.antiflutterFilter
 *
 * 33   1/12/10 5:28p erickson
 * SWDEPRECATED-2425: improve comments re:
 *  NEXUS_Display_SetGraphicsFramebuffer and enabling/disabling graphics
 *
 * 32   11/17/09 3:54p erickson
 * SW3556-852: add NEXUS_DisplayModule_SetAutomaticPictureQuality
 *
 * 31   11/5/09 3:54p erickson
 * SWDEPRECATED-2425: update comments on
 *  NEXUS_Display_SetGraphicsFramebuffer
 *
 * 30   8/6/09 2:38p erickson
 * PR56417: removed NEXUS_GraphicsSettings.gammaCorrection. feature should
 *  not be used.
 *
 * 29   7/16/09 11:50a erickson
 * PR56417: added NEXUS_GraphicsSettings.gammaCorrection
 *
 * 28   6/15/09 12:18p erickson
 * PR55928: added NEXUS_DisplaySettings.vecIndex
 *
 * 27   3/23/09 1:44p erickson
 * PR53425: add NEXUS_DisplaySettings.vsyncCallback
 *
 * 26   3/19/09 2:32p erickson
 * PR53151: clarify usage of NEXUS_Display_SetCustomFormatSettings
 *
 * 25   3/6/09 4:30p erickson
 * PR52839: added NEXUS_GraphicsFilterCoeffs to NEXUS_GraphicsSettings for
 *  controlling GFD scaling coefficients
 *
 * 24   2/23/09 3:03p erickson
 * PR52406: fix deep copy in NEXUS_DisplayCustomFormatSettings
 *
 * 23   2/23/09 1:36p erickson
 * PR35457: improve comments
 *
 * 22   1/12/09 12:29p erickson
 * PR50979: rename NEXUS_Display_ConnectVideoInput, clarify comments
 *
 * 21   1/6/09 7:39p erickson
 * PR50749: added NEXUS_Display_DisconnectVideoDecoder for tearing down
 *  NEXUS_Display_DriveVideoDecoder
 *
 * 20   11/3/08 3:40p erickson
 * PR46063: added NEXUS_DisplayCustomFormatSettings.interlaced
 *
 * 19   10/6/08 11:38p erickson
 * PR44801: merge
 *
 * PR44801/1   9/29/08 9:00p bandrews
 * PR44801: Added support for display VEC alignment
 *
 * 18   9/26/08 1:22p erickson
 * PR35457: added comment about sync-locked displays
 *
 * 17   9/19/08 5:19p erickson
 * PR46063: added NEXUS_Display_SetCustomFormatSettings
 *
 * 16   9/17/08 12:29p erickson
 * PR47014: expose compositor blend equation in NEXUS_VideoWindowSettings
 *  and NEXUS_GraphicsSettings
 *
 * 15   9/8/08 3:03p erickson
 * PR46527: intial checkin of test code to route from AVD->M2MC->GFD
 *
 * 14   6/25/08 11:44a erickson
 * PR39453: moved xvYccEnabled to NEXUS_DisplaySettings
 *
 * 13   5/6/08 2:40p erickson
 * PR42352: update comments
 *
 * 12   5/5/08 4:48p erickson
 * PR42445: provide API's for better display frame rate control
 *
 * 11   4/4/08 4:06p erickson
 * PR39257: add NEXUS_DisplayType_eBypass for DTV
 *
 * 10   3/24/08 5:35p vsilyaev
 * PR 40755: Added implementation for colorkeying
 *
 * 9   3/11/08 4:28p erickson
 * PR36159: clarified comment
 *
 * 8   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 7   2/21/08 1:57p erickson
 * PR39786: clarify comments
 *
 * 6   2/6/08 12:42p vsilyaev
 * PR 38682: Added attributes for the linux/kernel proxy mode
 *
 * 5   2/1/08 3:20p erickson
 * PR38679: add vbi support
 *
 * 4   1/23/08 9:22p vobadm
 * PR35457: update docs
 *
 * 3   1/23/08 2:39p erickson
 * PR35457: update docs
 *
 * 2   1/21/08 3:50p erickson
 * PR38862: added NEXUS_DisplayType_eLvds which is the default. use
 * DisplayType to select DVO or LVDS.
 *
 * 1   1/18/08 2:16p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#ifndef NEXUS_DISPLAY_H__
#define NEXUS_DISPLAY_H__

#include "nexus_types.h"
#include "nexus_surface.h"
#include "nexus_display_types.h"
#include "nexus_display_custom.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=*
A Display provides control of a video compositor.

Each display has one or more video windows.
For all displays except NEXUS_DisplayType_eBypass types, there is one graphics framebuffer.
The compositor combines these video sources into one display.

For NEXUS_DisplayType_eAuto types, there is a VEC associated with the compositor for analog output.

NEXUS_VideoInput tokens are connected to NEXUS_VideoWindowHandle's to route from various video sources.
See NEXUS_VideoWindow_AddInput.

NEXUS_VideoOutput tokens are added to NEXUS_Display to route to various video outputs.
See NEXUS_Display_AddOutput.
**/

/**
Summary:
NEXUS_DisplayType is used when opening a Display
**/
typedef enum NEXUS_DisplayType
{
    NEXUS_DisplayType_eAuto = 0, /* Configure display for VEC output. The VEC output is used to drive analog outputs like
                                    component, composite, svideo as well as HDMI output.
                                    This is called eAuto because the index of the VEC (e.g. primary or secondary VEC) is automatically chosen based on internal configuration. */
    NEXUS_DisplayType_eLvds,     /* Configure display for LVDS output by means of the DVO port. This is used to drive an LCD/PDP panel directly.
                                    This is only valid for some chips and Display indexes. */
    NEXUS_DisplayType_eDvo,      /* Configure display for DVO output. This is used to drive an LCD/PDP panel with an external LVDS daughter card.
                                    This is only valid for some chips and Display indexes. */
    NEXUS_DisplayType_eBypass,   /* Configure display for bypass output to CCIR656 or composite output. No graphics. */
    NEXUS_DisplayType_eMax
} NEXUS_DisplayType;

/**
Summary:
Display timing generator selection
**/
typedef enum NEXUS_DisplayTimingGenerator
{
    NEXUS_DisplayTimingGenerator_ePrimaryInput,
    NEXUS_DisplayTimingGenerator_eSecondaryInput,
    NEXUS_DisplayTimingGenerator_eTertiaryInput,
    NEXUS_DisplayTimingGenerator_eHdmiDvo, /* used for HDMI output master mode */
    NEXUS_DisplayTimingGenerator_e656Output,
    NEXUS_DisplayTimingGenerator_eEncoder, /* optional. maps to STG (simple timing generator). also see NEXUS_DisplayStgSettings.
                                              if you open with eAuto and use a display index dedicated to the encoder (like NEXUS_ENCODER_DISPLAY_IDX),
                                              it will select the appropriate STG for the encoder. */
    NEXUS_DisplayTimingGenerator_eAuto, /* default */
    NEXUS_DisplayTimingGenerator_eMax
} NEXUS_DisplayTimingGenerator;

/**
Summary:
3D source buffer selection.

Description:
In 3DTV, the default configuration dictates that the left buffer from source is routed
to the left buffer of the display and the right buffer from source is routed to the
right buffer of the display.
This default can be overriden such that either the left or right source buffer is
routed to both display buffers.

This enum is only effective when both the source and display are configured in 3D.
**/
typedef enum NEXUS_Display3DSourceBuffer {
    NEXUS_Display3DSourceBuffer_eDefault = 0, /* left source buffer for left display buffer, right source buffer for right display buffer */
    NEXUS_Display3DSourceBuffer_eLeft,        /* left source buffer for both display buffers */
    NEXUS_Display3DSourceBuffer_eRight,       /* right source buffer for both display buffers */
    NEXUS_Display3DSourceBuffer_eMax
} NEXUS_Display3DSourceBuffer;

/**
Summary:
Display settings
**/
typedef struct NEXUS_DisplaySettings
{
    NEXUS_DisplayType displayType;         /* The type of display to open. Cannot be changed after Open. */
    NEXUS_DisplayTimingGenerator timingGenerator;
    int vecIndex;                          /* If displayType is NEXUS_DisplayType_eAuto, this selects the VEC for this display.
                                              The default value is -1, which allows Nexus to select the mapping.
                                              Nexus will use the NEXUS_DisplayModuleSettings.vecSwap setting only if vecIndex is -1. */

    NEXUS_VideoFormat format;              /* Output format of video display. If an output is connected which can't support this format the SetSettings call will fail.
                                              Full-screen windows will be automatically resized.
                                              Graphics will be disabled. You must re-enable graphics, usually after setting the newly sized framebuffer. */
    NEXUS_DisplayAspectRatio aspectRatio;  /* Aspect ratio of display, if applicable */
    NEXUS_Timebase timebase;               /* Timebase that will drive the outputs */
    NEXUS_VideoInputHandle frameRateMaster;      /* VideoInput that provides the frame rate master to the VEC, which determines if the VEC will
                                              use a drop-frame or non-drop-frame frame rate (if applicable).
                                              If NULL, Nexus will look for NEXUS_VideoWindowSettings.autoMaster.
                                              If no autoMaster is set, Nexus will look at NEXUS_DisplayModuleSettings.dropFrame. */
    NEXUS_Pixel background;                /* Background color in NEXUS_PixelFormat_eA8_R8_G8_B8 colorspace */
    bool xvYccEnabled;                     /* If true, Nexus will check the HdmiInput for xvYCC colorspace and, if detected, will output xvYcc to an HdmiOutput or PanelOutput. */
    NEXUS_DisplayHandle alignmentTarget;   /* If non-NULL, Nexus will ask VDC to align this display's timing to the display
                                              specified as the target.  This only works when both displays are the same frame
                                              rate and frameRateMaster is NULL and NEXUS_VideoWindowSettings.autoMaster is false.
                                              If NULL, disables display alignment for this display. */
    NEXUS_CallbackDesc vsyncCallback;      /* Callback fires on every output vsync. Nexus does not deliver this callback with any minimum latency or during
                                              the blanking interval. It should be used for general timing only. */
    struct {
       bool  overrideOrientation;           /* used to override orientation of 2D display formats */
       NEXUS_VideoOrientation  orientation; /* orientation of the 3D display */
       NEXUS_Display3DSourceBuffer sourceBuffer;
    } display3DSettings;

    unsigned crcQueueSize;              /* if non-zero, CRC capture is enabled. use NEXUS_Display_GetCrcData to retrieve data. */
} NEXUS_DisplaySettings;

/**
Summary:
This type describes possible update modes for the display module

Description:
When updating the display, it's often necessary to call multiple functions to get the
desired setting. Instead of having each function applied immediately to hardware, the user can instruct Nexus
to accumulate changes, then apply them all at once.

By default Nexus operates in NEXUS_DisplayUpdateMode_eAuto mode. In this mode, all changes
are applied immediately and do not accumulate.

If you set Nexus into NEXUS_DisplayUpdateMode_eManual mode, changes will accumulate.
Call NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eNow) to apply the
acculumated changes. Nexus will remain in NEXUS_DisplayUpdateMode_eManual mode.

Be aware that some changes cannot be accumulated, even in NEXUS_DisplayUpdateMode_eManual mode.
These will be applied immediately, as if it was in NEXUS_DisplayUpdateMode_eAuto mode.
This mainly applies to creation and destruction of resources.
**/
typedef enum NEXUS_DisplayUpdateMode
{
    NEXUS_DisplayUpdateMode_eAuto,   /* Update the display settings immediately. */
    NEXUS_DisplayUpdateMode_eManual, /* Accumulate changes until user calls NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eNow) */
    NEXUS_DisplayUpdateMode_eNow     /* Apply all changes accumulated in NEXUS_DisplayUpdateMode_eManual mode. */
} NEXUS_DisplayUpdateMode;

/**
Summary:
This function is used to control when changes in display settings take effect.

Description:
Most display module functions take effect immediately (e.g. resizing of a window).
NEXUS_DisplayModule_SetUpdateMode allows the user to accumulate changes and commit them as a single transaction.
This can reduce execution time and can eliminate transition effects.

For example to change size of two windows simultaneously user would need use this code:

     NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eManual);
     NEXUS_VideoWindow_GetSettings(window0, &windowSettings);
     windowSettings.position.height /= 2;
     windowSettings.position.width /= 2;
     NEXUS_VideoWindow_SetSettings(window0, &windowSettings);
     NEXUS_VideoWindow_GetSettings(window1, &windowSettings);
     windowSettings.position.height *= 2;
     windowSettings.position.width *= 2;
     NEXUS_VideoWindow_SetSettings(window1, &windowSettings);
     NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eAuto);
**/
NEXUS_Error NEXUS_DisplayModule_SetUpdateMode(
    NEXUS_DisplayUpdateMode updateMode
    );

/**
Summary:
Get default NEXUS_DisplaySettings before calling NEXUS_Display_Open.
**/
void NEXUS_Display_GetDefaultSettings(
    NEXUS_DisplaySettings *pSettings    /* [out] */
    );

/**
Summary:
Open a new Display.
**/
NEXUS_DisplayHandle NEXUS_Display_Open( /* attr{destructor=NEXUS_Display_Close}  */
    unsigned displayIndex,
    const NEXUS_DisplaySettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
Close a Display
**/
void NEXUS_Display_Close(
    NEXUS_DisplayHandle display
    );

/**
Summary:
Get the current NEXUS_DisplaySettings from the Display.
**/
void NEXUS_Display_GetSettings(
    NEXUS_DisplayHandle display,
    NEXUS_DisplaySettings *pSettings      /* [out] */
    );

/**
Summary:
Set new NEXUS_DisplaySettings to the Display.
**/
NEXUS_Error NEXUS_Display_SetSettings(
    NEXUS_DisplayHandle display,
    const NEXUS_DisplaySettings *pSettings
    );

#define NEXUS_DISPLAY_REMOVE_OUTPUT_WRONG_CONNECTION    NEXUS_MAKE_ERR_CODE(0x102, 0)
#define NEXUS_DISPLAY_REMOVE_OUTPUT_NOT_CONNECTED       NEXUS_MAKE_ERR_CODE(0x102, 1)
#define NEXUS_DISPLAY_ADD_OUTPUT_ALREADY_CONNECTED      NEXUS_MAKE_ERR_CODE(0x102, 2)

/**
Summary:
Adds unique VideoOutput to the display.

Description:
This causes the content for this display to be routed to this output.
The output must support the Display's current NEXUS_VideoFormat, otherwise the function fails.
**/
NEXUS_Error NEXUS_Display_AddOutput(
    NEXUS_DisplayHandle display,
    NEXUS_VideoOutput output
    );

/**
Summary:
Removes connected VideoOutput from the display
**/
NEXUS_Error NEXUS_Display_RemoveOutput(
    NEXUS_DisplayHandle display,
    NEXUS_VideoOutput output
    );

/**
Summary:
Removes all VideoOutputs connected to the display
**/
void NEXUS_Display_RemoveAllOutputs(
    NEXUS_DisplayHandle display
    );

/**
Summary:
Status information returned by NEXUS_Display_GetStatus
**/
typedef struct NEXUS_DisplayStatus
{
    unsigned refreshRate; /* actual frame rate in 1/1000 Hz (e.g. 29.97 = 29970).
                             This is determined by the framerate of NEXUS_DisplaySettings.format and
                             the drop-frame/non-drop-frame characteristics of the sync-locked source. */
    unsigned numWindows; /* number of video windows supported by this display. may be less than NEXUS_NUM_VIDEO_WINDOWS. */
    bool graphicsSupported; /* true if graphics is supported */
} NEXUS_DisplayStatus;

/**
Summary:
Get status information about the display
**/
NEXUS_Error NEXUS_Display_GetStatus(
    NEXUS_DisplayHandle display,
    NEXUS_DisplayStatus *pStatus /* [out] */
    );

/*********************************
* Graphics
********/

/**
Summary:
Graphics feeder (GFD) scaler filtering options used in NEXUS_GraphicsSettings
**/
typedef enum NEXUS_GraphicsFilterCoeffs
{
    NEXUS_GraphicsFilterCoeffs_ePointSample,      /* Point sampled filtering */
    NEXUS_GraphicsFilterCoeffs_eBilinear,         /* Bilinear filtering */
    NEXUS_GraphicsFilterCoeffs_eAnisotropic,      /* Anisotropic filtering */
    NEXUS_GraphicsFilterCoeffs_eSharp,            /* Tabled sin(x)/x filtering. Also, see comments for horizontalCoeffIndex and verticalCoeffIndex */
    NEXUS_GraphicsFilterCoeffs_eMax
} NEXUS_GraphicsFilterCoeffs;

/*
Summary:
Graphics compositor settings which instruct the Display how to composite a graphics framebuffer with VideoWindows.

Description:
Graphics scaling is calculated by comparing the source width/height
with the destination width/height.

Instead of assuming the destination width/height is the width/height of the
display format, the application must specify it independently. This is because
the real issue is not the width/height of the display forward, but the x/y/width/height
of the viewable area of the TV. This can vary depending on the application and
target platform.

The source width/height are used to define the scale factor used by graphic
surfaces.
*/
typedef struct NEXUS_GraphicsSettings
{
    bool enabled; /* Enable graphics on the display.
                     When disabled, nexus will forget about any surface set with NEXUS_Display_SetGraphicsFramebuffer and
                     will destroy any internal state related to graphics.
                     After enabling, you must call NEXUS_Display_SetGraphicsFramebuffer to set the framebuffer. */
    bool visible; /* If false, nexus will hide graphics but will remember the framebuffer and will do minimal
                     reconfiguration of the display.
                     Both 'enabled' and 'visible' must be true to see graphics. 'visible' defaults to true. */
    bool antiflutterFilter; /* deprecated. use NEXUS_Graphics2DSettings.verticalFilter instead. */
    uint8_t alpha; /* alpha blending of framebuffer with video where 0xFF is opaque and 0x00 is transparent.  */
    unsigned zorder; /* Z-order of the graphics plane relative to the video windows' zorder. 0 is on bottom. */

    bool chromakeyEnabled; /* should the chromakey values be applied? */
    NEXUS_Pixel lowerChromakey; /* lower bound of chromakey range in NEXUS_PixelFormat_eA8_R8_G8_B8 colorspace */
    NEXUS_Pixel upperChromakey; /* upper bound of chromakey range in NEXUS_PixelFormat_eA8_R8_G8_B8 colorspace */

    NEXUS_Rect position; /* The area within the display that surface should displayed */
    NEXUS_Rect clip; /* The area within the surface that should be shown, relative to 0,0.
        If you want to show the entire surface, cliprect should be 0,0,position.width,position.height.
        If you specify a cliprect which is smaller, only that portition will be shown
        and it will be scaled to fit the rectangle of size position.width and position.height.
        At this time only horizontal upscaling is supported. Vertical scaling and horizontal downscaling are not supported.
        The default value of clip.width is less than position.width for HD resolutions. Typically, HD graphics use horizontal upscaling to converse memory bandwidth. */

    NEXUS_CallbackDesc frameBufferCallback;  /* Callback called when framebuffer has been set. After the callback fires,
        it's possible to use the previous framebuffer without any visible artifacting. */

    NEXUS_CompositorBlendFactor sourceBlendFactor; /* Source refers to the graphics. */
    NEXUS_CompositorBlendFactor destBlendFactor;   /* Dest referes to whatever the graphics is being blended with. */
    uint8_t constantAlpha;                         /* constantAlpha is used if either sourceBlendFactor or destBlendFactor specify it */

    NEXUS_GraphicsFilterCoeffs horizontalFilter;   /* GFD horizontal upscaler coefficients */
    NEXUS_GraphicsFilterCoeffs verticalFilter;     /* GFD vertical upscaler coefficients */
    unsigned horizontalCoeffIndex;                 /* if horizontalFilter == eSharp, then this index is used for table-driven coefficients. */
    unsigned verticalCoeffIndex;                   /* if verticalFilter == eSharp, then this index is used for table-driven coefficients. */

    struct {
        int rightViewOffset; /* offset of the right view */
    } graphics3DSettings;
} NEXUS_GraphicsSettings;

/**
Summary:
Get current graphics compositing settings.
**/
void NEXUS_Display_GetGraphicsSettings(
    NEXUS_DisplayHandle display,
    NEXUS_GraphicsSettings *pSettings /* [out] */
    );

/**
Summary:
Set graphics compositing settings.
**/
NEXUS_Error NEXUS_Display_SetGraphicsSettings(
    NEXUS_DisplayHandle display,
    const NEXUS_GraphicsSettings *pSettings
    );

/*
Summary:
Set which surface should be used as the framebuffer.

Description:
There is no implicit double-buffering. The user is responsible to provide any double or triple buffering logic by cycling the framebuffer between two or three surfaces.

If you are doing asynchronous blits into the new framebuffer, to avoid tearing your app must wait for those blits to be completed before calling
NEXUS_Display_SetGraphicsFramebuffer. See NEXUS_Graphics2D_Checkpoint.

The framebuffer will be switched on the next vsync. NEXUS_GraphicsSettings.frameBufferCallback will be fired after that switch happens.
If you are double buffering, to avoid tearing you must not write into the outgoing framebuffer until after frameBufferCallback is fired.
If you want to start updating the next framebuffer before waiting for frameBufferCallback, use triple buffering. You can achieve a sustained 60 fps
graphics update (or 50 fps for 50Hz systems), if your application can prepare the next framebuffer in less than the 16 msec vsync time minus a small amount
of time (e.g. 1-2 msec) to process the frameBufferCallback.

If you call NEXUS_Display_SetGraphicsFramebuffer a second time, before the first call was able to be applied (i.e. before the next vsync), your first call
will be overwritten. NEXUS_Display_SetGraphicsFramebuffer calls are not queued.

Framebuffers are always placed at coordinate 0,0.

The difference between this surface's width and height and the NEXUS_GraphicsSettings destinationWidth and destinationHeight will
result in graphics feeder scaling, if the scaling feature exists.

When you set NEXUS_GraphicsSettings.enabled = false, nexus will forget about any framebuffer that was set. The application can then
safely delete the surface. To re-enable, you must set NEXUS_GraphicsSettings.enabled = true and call NEXUS_Display_SetGraphicsFramebuffer with a new surface.
*/
NEXUS_Error NEXUS_Display_SetGraphicsFramebuffer(
    NEXUS_DisplayHandle display,
    NEXUS_SurfaceHandle frameBuffer
    );

/**
Summary:
Get current color space convertor matrix for the graphics feeder (GFD)
**/
void NEXUS_Display_GetGraphicsColorMatrix(
    NEXUS_DisplayHandle display,
    NEXUS_ColorMatrix *pColorMatrix /* [out] */
    );

/**
Summary:
Set new color space convertor matrix for the graphics feeder (GFD)

Description:
This will override anything set by NEXUS_Display_SetGraphicsColorSettings.
**/
NEXUS_Error NEXUS_Display_SetGraphicsColorMatrix(
    NEXUS_DisplayHandle display,
    const NEXUS_ColorMatrix *pColorMatrix /* attr{null_allowed=y} */
    );

/**
Summary:
Set color control settings for graphics feeder (GFD).

Description:
This will override anything set by NEXUS_Display_SetGraphicsColorMatrix.

int16_t values range between -32768 and 32767.
**/
typedef struct NEXUS_GraphicsColorSettings
{
    int16_t contrast;
    int16_t saturation;
    int16_t hue;
    int16_t brightness;
} NEXUS_GraphicsColorSettings;

/**
Summary:
**/
void NEXUS_Display_GetGraphicsColorSettings(
    NEXUS_DisplayHandle display,
    NEXUS_GraphicsColorSettings *pSettings /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_Display_SetGraphicsColorSettings(
    NEXUS_DisplayHandle display,
    const NEXUS_GraphicsColorSettings *pSettings
    );

/**
Summary:
Connect a Display to a VideoInput without a VideoWindow.

Description:
Some usage modes allow video to be processed with no video window.
In these cases, a VideoInput might still need interrupts or some other internal connection from the Display.

For example, the VideoDecoder has NEXUS_VideoDecoder_CreateStripedSurface which allows an application
to capture decoded pictures for graphics processing. The application can then display those decoded
pictures using a graphics framebuffer. However, the VideoDecoder still needs interrupts from the Display
to drive its internal heartbeat. This connection makes that possible.
**/
NEXUS_Error NEXUS_Display_ConnectVideoInput(
    NEXUS_DisplayHandle display,
    NEXUS_VideoInputHandle videoInput
    );

/**
Summary:
Disconnect a VideoInput connected with NEXUS_Display_ConnectVideoInput
**/
void NEXUS_Display_DisconnectVideoInput(
    NEXUS_DisplayHandle display,
    NEXUS_VideoInputHandle videoInput
    );

/**
Summary:
Enable a default set of picture quality features including MAD, ANR, and DNR.

Description:
By default, Nexus starts with picture quality features disabled. Users must enable each feature and can control each feature's settings.

NEXUS_DisplayModule_SetAutomaticPictureQuality performs a one-time automatic enabling of picture quality features for all displays and windows in the system.
After this function finishes, the user can call PQ functions and learn or modify these settings.

This is a one-time setting. No on-going automatic picture quality changes will be made.
**/
NEXUS_Error NEXUS_DisplayModule_SetAutomaticPictureQuality(void);

/**
Summary:
Update the NEXUS_DisplayModuleSettings.configurationId for dynamic RTS.

Description:
See NEXUS_DisplayModuleSettings.configurationId for the meaning of this ID.
**/
NEXUS_Error NEXUS_DisplayModule_SetConfigurationId(
    unsigned configurationId
    );

/**
Summary:
Display CRC data
**/
typedef struct NEXUS_DisplayCrcData
{
    struct {
        uint32_t luma;
        uint32_t cb;
        uint32_t cr;
    } cmp; /* compositor (CMP) CRC */
} NEXUS_DisplayCrcData;

/**
Summary:
Get compositor CRC data

Description:
You must set NEXUS_DisplaySettings.crcQueueSize to a non-zero value (for example, 30) to capture data.
**/
NEXUS_Error NEXUS_Display_GetCrcData(
    NEXUS_DisplayHandle display,
    NEXUS_DisplayCrcData *pData, /* attr{nelem=numEntries;nelem_out=pNumEntriesReturned} [out] array of crc data structures */
    unsigned numEntries,
    unsigned *pNumEntriesReturned /* [out] */
    );

/*
Summary:
Description of the stereo (Left/Right) framebuffer

**/

typedef struct NEXUS_GraphicsFramebuffer3D {
    NEXUS_VideoOrientation  orientation; /* orientation of the graphics framebuffer */
    NEXUS_SurfaceHandle main; /* mandatory, main surface */
    NEXUS_SurfaceHandle alpha; /* optional, alpha surface */
    NEXUS_SurfaceHandle right; /* optional, right surface */
    NEXUS_SurfaceHandle rightAlpha; /* deprecated */
    uint32_t alphaW0; /* used as pixel alpha when pixel's W is 0 */
    uint32_t alphaW1; /* used as pixel alpha when pixel's W is 1 */
} NEXUS_GraphicsFramebuffer3D;

/**
Summary:
Get default values for NEXUS_DisplayCustomFormatSettings
**/
void NEXUS_Graphics_GetDefaultFramebuffer3D(
    NEXUS_GraphicsFramebuffer3D *pFrameBuffer3D /* [out] */
    );

/*
Summary:
Sets description of surfaces used as 3d(Left/Right)  framebuffer

Description:
This function is  the same as NEXUS_Display_SetGraphicsFramebuffer with sole exception
that it could provides information about composition of 3D framebuffer

See Also:
    NEXUS_Display_SetGraphicsFramebuffer
    NEXUS_Graphics_GetDefaultFramebuffer3D
*/
NEXUS_Error NEXUS_Display_SetGraphicsFramebuffer3D(
    NEXUS_DisplayHandle display,
    const NEXUS_GraphicsFramebuffer3D *pFrameBuffer3D
    );

/**
Summary:
STG (simple timing generator) settings.

Description:
See BVDC_Display_SetStgConfiguration for more detailed documentation.
**/
typedef struct NEXUS_DisplayStgSettings
{
    bool enabled; /* If NEXUS_DisplayTimingGenerator_eEncoder is set (the STG master), this must be true; Nexus will reject a false setting.
                     When master TG of the display is not STG, if enabled = true then the STG output is slaved on that display;
                     else STG output would be detached from that display. */
    bool nonRealTime; /* configure STG trigger as real-time or non-real-time */
} NEXUS_DisplayStgSettings;

/**
Summary:
Get STG settings
**/
void NEXUS_Display_GetStgSettings(
    NEXUS_DisplayHandle display,
    NEXUS_DisplayStgSettings *pSettings  /* [out] */
    );

/**
Summary:
Set STG settings
**/
NEXUS_Error NEXUS_Display_SetStgSettings(
    NEXUS_DisplayHandle display,
    const NEXUS_DisplayStgSettings *pSettings
    );

/**
Summary:
display module capabilities
**/
typedef struct NEXUS_DisplayCapabilities
{
    bool displayFormatSupported[NEXUS_VideoFormat_eMax]; /* is NEXUS_DisplaySettings.format supported by any display in the system? */
} NEXUS_DisplayCapabilities;

/**
Summary:
get display module capabilities
**/
void NEXUS_GetDisplayCapabilities(
    NEXUS_DisplayCapabilities *pCapabilities /* [out] */
    );
    
#ifdef __cplusplus
}
#endif

#endif /* NEXUS_DISPLAY_H__ */
