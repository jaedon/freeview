/******************************************
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
 * $brcm_Workfile: nexus_graphics2d.h $
 * $brcm_Revision: 57 $
 * $brcm_Date: 7/30/12 11:34a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics2d/include/nexus_graphics2d.h $
 * 
 * 57   7/30/12 11:34a erickson
 * SW7435-279: move hwFifoSize from NEXUS_Graphics2DOpenSettings to
 *  NEXUS_Graphics2DModuleSettings.core[]
 * 
 * 56   2/17/12 11:28a erickson
 * SW7405-5427: add separate booleans for output mirroring, change dest
 *  mirroring booleans to read control, not read & write control
 * 
 * 55   1/26/12 5:20p erickson
 * SW7425-2116: add NEXUS_Graphics2D_SetAlphaColorMatrix, fix
 *  documentation of NEXUS_Graphics2DColorMatrix.shift
 *
 * 54   12/8/11 10:58a erickson
 * SW7425-1921: fix comment
 *
 * 53   12/8/11 9:48a erickson
 * SW7420-2155: clarify use of mirror booleans. document new auto-mirror
 *  for overlapped blits.
 *
 * 52   7/21/11 5:29p erickson
 * SW7420-1974: add option for separate dest and source constant color
 *
 * 51   6/16/11 3:07p erickson
 * SW7405-5356: update comments re: requirement that
 *  NEXUS_Graphics2D_Checkpoint be called if packetFifoThreshold is non-
 *  zero
 *
 * 50   5/26/11 1:13p erickson
 * SW7420-1894: deprecate preAllocPacketMemory and maxOperations from
 *  NEXUS_Graphics2DOpenSettings
 *
 * 49   5/20/11 5:03p erickson
 * SW7420-1894: add NEXUS_Graphics2DOpenSettings.packetFifoThreshold
 *
 * 48   2/11/11 5:15p erickson
 * SW7420-1491: add attr{memory=cached} to NEXUS_Graphics2D_Memset32
 *
 * 47   1/20/11 6:12p erickson
 * SW7405-5063: add NEXUS_FillOp_eUseBlendEquation
 *
 * 46   12/7/10 5:44p erickson
 * SW7420-1138: add boundsHeap
 *
 * 45   9/22/10 5:11p erickson
 * SW7405-4882: add NEXUS_Graphics2D_ConvertFilter and
 *  NEXUS_Graphics2D_ConvertColorMatrix
 *
 * 44   9/15/10 3:55p erickson
 * SW7420-1081: add attr hint to NEXUS_Graphics2D_GetPacketBuffer
 *
 * 43   8/16/10 11:08a erickson
 * SW7405-3671: add packet blit API. switch to packet-based GRC
 *  implementation.
 *
 * 42   6/23/10 12:14p erickson
 * SW7550-453: refactor to use nexus_striped_surface.h interface
 *
 * 41   6/18/10 4:30p erickson
 * SW7405-4508: virtualize NEXUS_Graphics2D_Open so that each user can
 *  have their own blitter and their own checkpoint callback
 *
 * 40   6/17/10 6:25p erickson
 * SWDEPRECATED-2425: add comment
 *
 * 39   3/10/10 2:53p vsilyaev
 * SW7405-3671: Added device offsets
 *
 * 38   2/16/10 1:22p vsilyaev
 * SW7405-3671: Added NEXUS_Graphics2D_BatchBlit
 *
 * 37   12/30/09 12:23p erickson
 * SWDEPRECATED-2425: add comments for NEXUS_Graphics2DColorMatrix
 *
 * 36   11/4/09 9:47a erickson
 * SW7405-3328: add NEXUS_Graphics2D_PorterDuffFill
 *
 * DirectFB_1_4_Port/1   10/30/09 2:07p robertwm
 * SW7405-3328: Support Porter-Duff Fill Compositions.
 *
 * 35   9/29/09 2:53p erickson
 * SW7405-3021: add
 *  NEXUS_Graphics2DBlitSettings.alphaPremultiplySourceEnabled
 *
 * 34   9/23/09 11:45a erickson
 * SW7405-3062: update comments for NEXUS_Graphics2D_Memset32
 *
 * 33   8/10/09 12:57p vsilyaev
 * PR 57469: Return proper error code when queue is full and try to handle
 *  condition when BGRC_WaitForOperationsComplete returns out of memory
 *  error
 *
 * 32   8/5/09 1:37p erickson
 * PR54954: clarify comments re: checkpoint
 *
 * 31   5/11/09 11:48a erickson
 * PR54954: added comment about multithreaded graphics code
 *
 * 30   4/22/09 9:13a erickson
 * PR53698: added comment to NEXUS_Graphics2DBlitSettings about palettized
 *  destination surfaces
 *
 * 29   4/20/09 9:23a erickson
 * PR53701: added blit reversal flags to NEXUS_Graphics2DBlitSettings
 *
 * 28   4/3/09 4:39p erickson
 * PR53701: added NEXUS_Graphics2DScalingControlSettings to
 *  NEXUS_Graphics2DBlitSettings
 *
 * 27   3/19/09 11:12a erickson
 * PR53116: add support for raster operations
 *
 * 26   2/26/09 9:45a erickson
 * PR52506: updated comments
 *
 * 25   2/25/09 4:44p vsilyaev
 * PR 52506: Updated code to move completionCallback as part of the
 *  graphics settings. Print warning if two methods are used
 *  simultaneously
 *
 * 24   1/21/09 1:46p vsilyaev
 * PR 51396: Use the NEXUS_Pixel datatype for pixels values
 *
 * 23   12/9/08 10:36a erickson
 * PR50088: fix comment
 *
 * 22   11/7/08 9:55a erickson
 * PR46300: fix comment
 *
 * 21   10/23/08 11:53a erickson
 * PR48190: added NEXUS_Graphics2DOpenSettings.maxOperations and
 *  deprecated NEXUS_Graphics2DOpenSettings
 *
 * 20   10/20/08 9:57a erickson
 * PR46527: extend NEXUS_Graphics2D_DestripeToSurface with optional
 *  pTargetRect
 *
 * 19   9/18/08 11:02a erickson
 * PR47014: update blend equation enums
 *
 * 18   9/17/08 11:35a erickson
 * PR47014: clarify default values for dest.rect.width and .height
 *
 * 17   9/17/08 10:34a erickson
 * PR47014: add NEXUS_BlendEquation and new color and alpha ops to use it
 *
 * 16   9/12/08 1:00p erickson
 * PR46844: added NEXUS_Graphics2D_Memset32
 *
 * 15   9/9/08 4:59p erickson
 * PR46683: fix NEXUS_Graphics2DBlitSettings.conversionMatrix
 *
 * 14   9/8/08 4:12p erickson
 * PR46430: allow destripe to a larger supplied surface
 *
 * 13   9/4/08 12:36p erickson
 * PR46430: added NEXUS_Graphics2D_DestripeToSurface
 *
 * 12   9/3/08 11:49a erickson
 * PR46287: clarify meaning of standard color format. handle palette index
 *  the same for both color format types.
 *
 * 11   8/21/08 5:14p erickson
 * PR45941: clarified comment
 *
 * 10   8/21/08 2:16p erickson
 * PR44792: added missing attr to Open
 *
 * 9   8/19/08 4:19p erickson
 * PR45866: added NEXUS_Graphics2DOpenSettings to NEXUS_Graphics2D_Open
 *
 * 8   5/6/08 4:05p erickson
 * PR41738: remove NEXUS_Graphics2DColorKeyMode
 *
 * 7   4/14/08 3:12p erickson
 * PR41738: added dest color key, brutus_copy_rect, filter enums. added
 *  FastBlit.
 *
 * 6   3/26/08 1:33p erickson
 * PR40948: remove alternateConst
 *
 * 5   2/26/08 8:54p erickson
 * PR36275: readd NEXUS_BlitAlphaOp_eEmulateTransparentVideo
 *
 * 4   2/25/08 2:59p erickson
 * PR39786: added NEXUS_Graphics2D_Destripe
 *
 * 3   1/25/08 3:57p erickson
 * PR38924: added comment about downscale limits
 *
 * 2   1/22/08 2:41p erickson
 * PR36159: clarify blit enums. removed unimplemented enums.
 *
 * Nexus_Devel/10   1/8/08 1:19p erickson
 * PR36159: added autoFlush and blockedSync
 *
 * Nexus_Devel/PR36159/1   1/4/08 3:32p gezhang
 * PR36159: Add autoFlush and blockedSync configurations to graphics and
 * surface settings
 *
 * Nexus_Devel/9   11/30/07 11:12a erickson
 * PR35457: api update
 *
 * Nexus_Devel/8   11/28/07 2:11p erickson
 * PR35457: doc update
 *
 *****************************************/
#ifndef NEXUS_GRAPHICS2D_H__
#define NEXUS_GRAPHICS2D_H__

/*=***********************************
The Graphics2D Interface provides functions to blit from one surface to another surface and to
fill a surface with a color.

The main functions are NEXUS_Graphics2D_Blit and NEXUS_Graphics2D_Fill.

NEXUS_Graphics2D_PorterDuffBlit is provided as a convenience. However all porter-duff blits can also be
accomplished using NEXUS_Graphics2D_Blit along with NEXUS_BlitColorOp_eUseBlendEquation.

The Graphics2D module has a non-blocking API. By default functions will not wait for an internal event to complete.
If you need to wait for a flil or blit to complete, you can request a callback or poll.
****/

#include "nexus_surface.h"
#include "nexus_striped_surface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
Summary:
Handle for the Graphics2D interface.
**/
typedef struct NEXUS_Graphics2D *NEXUS_Graphics2DHandle;

/**
Summary:
Settings used for NEXUS_Graphics2D_Open
**/
typedef struct NEXUS_Graphics2DOpenSettings
{
    unsigned packetFifoSize;    /* Size of SW fifo for packet-based blitting. Each NEXUS_Graphics2D has its own SW fifo. */
    unsigned packetFifoThreshold; /* Threshold to submit packets for blitting. A higher threshold will result in greater throughput but reduced latency.
                                     Packets will be submitted when threshold is reached or when NEXUS_Graphics2D_Checkpoint is called.
                                     Default is packetFifoThreshold == packetFifoSize for maximum throughput; therefore, NEXUS_Graphics2D_Checkpoint is required
                                     to execute a blit or fill. */
    NEXUS_HeapHandle heap;      /* optional heap */
    NEXUS_HeapHandle boundsHeap; /* optional heap to bounds check all blits */

    unsigned maxOperations;     /* deprecated. currently only used for ZSP graphics. */
    bool preAllocPacketMemory;  /* deprecated and unused. */
} NEXUS_Graphics2DOpenSettings;

/**
Summary:
This error code is returned by NEXUS_Graphics2D_Checkpoint if the graphics hardware is busy
and an asynchronous callback is scheduled to execute when HW will become available.
**/
#define NEXUS_GRAPHICS2D_QUEUED NEXUS_MAKE_ERR_CODE(0x101, 2)

/**
Summary:
This error code is returned by NEXUS_Graphics2D_Checkpoint if the graphics hardware is busy
and NEXUS_Graphics2DSettings.pollingCheckpoint is set to true.
**/
#define NEXUS_GRAPHICS2D_BUSY NEXUS_MAKE_ERR_CODE(0x101, 3)

/**
Summary:
This error code is returned by NEXUS_Graphics2D_Blit, NEXUS_Graphics2D_Fill, NEXUS_Graphics2D_PorterDuffFill,
NEXUS_Graphics2D_FastBlit or NEXUS_Graphics2D_PorterDuffBlit functions when there is no space in the hardware queue;
user then could call NEXUS_Graphics2D_Checkpoint function to wait for queue to dry out.
**/
#define NEXUS_GRAPHICS2D_QUEUE_FULL NEXUS_MAKE_ERR_CODE(0x101, 4)

/**
Summary:
Identifies the pixel format for certain color parameters.

Description:
The term "standard color format" refers to a 32 bit color value which is
either ARGB8888 (for RGB surfaces) or AYCbCr8888 (for YCbCr surfaces).

For blit constant color or a blit color key value, only the standard color format is allowed.

For fill, the user has an option to use either the standard color format (NEXUS_ColorFormatType_eStandardFormat)
or the same pixel format as the surface being filled (NEXUS_ColorFormatType_eSameAsSurface).

For both types, a palette color value is simply the palette index. In lower level software (e.g. GRC), a bit shift of the
palette index is required in some cases. Nexus handles this automatically in all cases.

Used in NEXUS_Graphics2DSettings.
**/
typedef enum NEXUS_ColorFormatType
{
    NEXUS_ColorFormatType_eSameAsSurface, /* The associated color parameter has the same format as a related surface.
                                             For example, for an ARGB1555 surface, the fill color will be ARGB1555. */
    NEXUS_ColorFormatType_eStandardFormat /* The associated color parameter has the standard color format.
                                             For example, for an ARGB1555 surface, the fill color will be ARGB8888. */
} NEXUS_ColorFormatType;

/**
Summary:
Settings for the Graphics2D object.

Description:
Used in NEXUS_Graphics2D_GetSettings and NEXUS_Graphics2D_SetSettings
**/
typedef struct NEXUS_Graphics2DSettings
{
    NEXUS_ColorFormatType colorFormatType;   /* Specifies how to interpret the NEXUS_Graphics2DFillSettings.color value. */
    bool blockedSync;       /* deprecated */
    bool pollingCheckpoint; /* If true, then NEXUS_Graphics2D_Checkpoint will never wait or schedule an asynchronous checkpointCallback.
                               Instead, it will return either NEXUS_SUCCESS or NEXUS_GRAPHICS2D_BUSY. */
    unsigned completionTimeout; /* timeout, in microseconds, on how long NEXUS_Graphics2D_Checkpoint should busy loop prior to scheduling an
                               asynchronous checkpointCallback notification. This is ignored if pollingCheckpoint is true. */
    NEXUS_CallbackDesc checkpointCallback; /* Callback that is used for asynchronous notification by the NEXUS_Graphics2D_Checkpoint function.
                               Only used if pollingCheckpoint is false.
                               This will be overwritten if you call NEXUS_Graphics2D_Checkpoint with a non-NULL callback parameter. */
    NEXUS_CallbackDesc packetSpaceAvailable; /* if a blit fails with NEXUS_GRAPHICS2D_QUEUE_FULL, packetSpaceAvailable will be fired
                               when space becomes available. */
} NEXUS_Graphics2DSettings;

/**
Summary:
Get default settings for NEXUS_Graphics2D_Open
**/
void NEXUS_Graphics2D_GetDefaultOpenSettings(
   NEXUS_Graphics2DOpenSettings *pSettings /* [out] */
   );

/**
Summary:
Open a Graphics2D interface for blit and fill operations.

Description:
You can open more than one instance of the Graphics2D interface with the same index.
Each instance will use the same HW blitter, but each will have its own SW handle and checkpoint.
Only the first instance will have its NEXUS_Graphics2DOpenSettings determine the memory allocation.
**/
NEXUS_Graphics2DHandle NEXUS_Graphics2D_Open(   /* attr{destructor=NEXUS_Graphics2D_Close}  */
    unsigned index,
    const NEXUS_Graphics2DOpenSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
Close a Graphics2D interface.
**/
void NEXUS_Graphics2D_Close(
    NEXUS_Graphics2DHandle handle
    );

/**
Summary:
Get current settings.
**/
void NEXUS_Graphics2D_GetSettings(
    NEXUS_Graphics2DHandle    handle,
    NEXUS_Graphics2DSettings *pSettings /* [out] */
    );

/**
Summary:
Apply new settings.

Description:
These fill take effect on the next blit/flit.
**/
NEXUS_Error NEXUS_Graphics2D_SetSettings(
    NEXUS_Graphics2DHandle          handle,
    const NEXUS_Graphics2DSettings *pSettings
    );

/***************************************************************************
Summary:
Blending equation factors used in NEXUS_BlendEquation
****************************************************************************/
typedef enum NEXUS_BlendFactor
{
    NEXUS_BlendFactor_eZero,                    /* Zero */
    NEXUS_BlendFactor_eHalf,                    /* 1/2 */
    NEXUS_BlendFactor_eOne,                     /* One */
    NEXUS_BlendFactor_eSourceColor,             /* Color from source */
    NEXUS_BlendFactor_eInverseSourceColor,      /* 1-color from source */
    NEXUS_BlendFactor_eSourceAlpha,             /* Alpha from source */
    NEXUS_BlendFactor_eInverseSourceAlpha,      /* 1-alpha from source */
    NEXUS_BlendFactor_eDestinationColor,        /* Color from destination */
    NEXUS_BlendFactor_eInverseDestinationColor, /* 1-color from destination */
    NEXUS_BlendFactor_eDestinationAlpha,        /* Alpha from destination */
    NEXUS_BlendFactor_eInverseDestinationAlpha, /* 1-alpha from destination */
    NEXUS_BlendFactor_eConstantColor,           /* Color from blend color. See NEXUS_Graphics2DBlitSettings.constantColor. */
    NEXUS_BlendFactor_eInverseConstantColor,    /* 1-color from blend color */
    NEXUS_BlendFactor_eConstantAlpha,           /* Alpha from blend color. See NEXUS_Graphics2DBlitSettings.constantColor. */
    NEXUS_BlendFactor_eInverseConstantAlpha,    /* 1-alpha from blend color */
    NEXUS_BlendFactor_eMax
} NEXUS_BlendFactor;

/*****************************************************************************
Summary:
Blend equation

Description:
This gives low-level control of the alpha and color blend equation
used in NEXUS_Graphics2D_Blit. Used in NEXUS_Graphics2DBlitSettings.

Equation is: a*b +/- c*d +/- e

*****************************************************************************/
typedef struct NEXUS_BlendEquation
{
    NEXUS_BlendFactor a;
    NEXUS_BlendFactor b;
    bool              subtract_cd;
    NEXUS_BlendFactor c;
    NEXUS_BlendFactor d;
    bool              subtract_e;
    NEXUS_BlendFactor e;
} NEXUS_BlendEquation;

/**
Summary:
How to fill the surface with the given color

Description:
Used in NEXUS_Graphics2DFillSettings
**/
typedef enum NEXUS_FillOp
{
    NEXUS_FillOp_eIgnore = 0,   /* Do not change the values for the channel (either color or alpha) */
    NEXUS_FillOp_eCopy,         /* Copy the value of the channel (color or alpha) from the constant color to the surface */
    NEXUS_FillOp_eBlend,        /* For color channel, blend surface color with constant color using constant alpha.
                                   This operation is not support for the alpha channel. */
    NEXUS_FillOp_eUseBlendEquation, /* Use NEXUS_BlendEquation */
    NEXUS_FillOp_eMax
} NEXUS_FillOp;

/**
Summary:
Parameters for NEXUS_Graphics2D_Fill

Description:
Used in NEXUS_Graphics2D_GetDefaultFillSettings and NEXUS_Graphics2D_Fill
**/
typedef struct NEXUS_Graphics2DFillSettings
{
    NEXUS_SurfaceHandle   surface;
    NEXUS_Pixel           color;           /* Color to fill. See NEXUS_Graphics2DSettings.colorFormatType for the pixel format of this color value. */
    NEXUS_Rect            rect;            /* Area of surface to fill. width,height of 0,0 fills the entire surface */
    NEXUS_FillOp          colorOp;         /* Operation to take on the color channel */
    NEXUS_FillOp          alphaOp;         /* Operation to take on the alpha channel */
    NEXUS_BlendEquation   colorBlend;      /* Used only if colorOp = NEXUS_FillOp_eUseBlendEquation */
    NEXUS_BlendEquation   alphaBlend;      /* Used only if alphaOp = NEXUS_FillOp_eUseBlendEquation */
} NEXUS_Graphics2DFillSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.
**/
void NEXUS_Graphics2D_GetDefaultFillSettings(
    NEXUS_Graphics2DFillSettings *pSettings /* [out] */
    );

/**
Summary:
Fill, tint or otherwise modify the pixels of an area of a surface using a constant value

Description:
This routine modifies the color channels and/or the alpha channel of the
pixels of a surface using a constant value.
This function is non-blocking. It returns immediately without waiting for the fill to actually happen.

If packetFifoThreshold is non-zero, NEXUS_Graphics2D_Checkpoint is required to submit queue blits to hardware.
The NEXUS_Graphics2D_Checkpoint callback is also used to determine when this blit is done.
**/
NEXUS_Error NEXUS_Graphics2D_Fill(
    NEXUS_Graphics2DHandle handle,
    const NEXUS_Graphics2DFillSettings *pSettings
    );

/**
Summary:
Color operations available in NEXUS_Graphics2DBlitSettings

Description:
If you want to make no change to the output color, then "source" must be "output".

The NEXUS_BlitColorOp is orthogonal with the NEXUS_BlitAlphaOp.
Used in NEXUS_Graphics2DBlitSettings
**/
typedef enum NEXUS_BlitColorOp
{
    NEXUS_BlitColorOp_eCopySource = 0,            /* Copy the source color with no blending. */
    NEXUS_BlitColorOp_eUseConstantAlpha,          /* Blend the source and dest colors using the alpha from the constantColor param */
    NEXUS_BlitColorOp_eUseSourceAlpha,            /* Blend the source and dest colors using the alpha from the source pixels */
    NEXUS_BlitColorOp_eUseDestAlpha,              /* Blend the source and dest colors using the alpha from the dest pixels */
    NEXUS_BlitColorOp_eSelectPaletteWithColorkey, /* Combine two palettized surfaces using colorkey to select per pixel.
                                                     Use the colorKeyLower and colorKeyUpper fields of NEXUS_Graphics2DBlitSettings */
    NEXUS_BlitColorOp_eAdd,                       /* Oc = Sc + Dc. Adds source and destination color.
                                                     Can be used for brightening and over-saturation effects. */
    NEXUS_BlitColorOp_eUseBlendEquation,          /* If true, use colorBlend equation */
    NEXUS_BlitColorOp_eMax
} NEXUS_BlitColorOp;

/**
Summary:
Alpha operations available in NEXUS_Graphics2DBlitSettings

Description:
If you want to make no change to the output alpha, then either "source" or "dest" must be the same surface as "output".

Using NEXUS_BlitColorOp_eUseConstantAlpha and NEXUS_BlitAlphaOp_eCopyConstant at the same time is not
supported. There is only one constantColor and it would apply to both operations. If you want to do both
of these operations with different alpha values, you will need to perform two blits.

The NEXUS_BlitAlphaOp is orthogonal with the NEXUS_BlitColorOp.
Used in NEXUS_Graphics2DBlitSettings

**/
typedef enum NEXUS_BlitAlphaOp
{
    NEXUS_BlitAlphaOp_eCopySource = 0,            /* Oa = Sa. Copy the source alpha to the output alpha. */
    NEXUS_BlitAlphaOp_eCopyDest,                  /* Oa = Da. Copy the dest alpha to the output alpha */
    NEXUS_BlitAlphaOp_eCopyConstant,              /* Oa = Ca (where Ca is constantColor>>24). Copy the constantColor parameter for the output alpha */
    NEXUS_BlitAlphaOp_eCombine,                   /* Oa = Sa*Sa + Da*(1-Sa). Combine source and dest alpha. */
    NEXUS_BlitAlphaOp_eEmulateTransparentVideo,   /* Calculate alpha values so that video window, when placed behind the
                                                     graphics window, appears to be in front of the graphics, with an
                                                     alpha of Ca. (Oa = 1 - (1-Sa)*Ca) */
    NEXUS_BlitAlphaOp_eAdd,                       /* Oa = Sa + Da. Adds source and destination alpha. */
    NEXUS_BlitAlphaOp_eUseBlendEquation,          /* If true, use alphaBlend equation */
    NEXUS_BlitAlphaOp_eMax
} NEXUS_BlitAlphaOp;

/**
Summary:
Graphics filtering options used in NEXUS_Graphics2DBlitSettings
**/
typedef enum NEXUS_Graphics2DFilterCoeffs
{
    NEXUS_Graphics2DFilterCoeffs_ePointSample,      /* Point sampled filtering */
    NEXUS_Graphics2DFilterCoeffs_eBilinear,         /* Bilinear filtering */
    NEXUS_Graphics2DFilterCoeffs_eAnisotropic,      /* Anisotropic filtering */
    NEXUS_Graphics2DFilterCoeffs_eSharp,            /* Tabled sin(x)/x filtering */
    NEXUS_Graphics2DFilterCoeffs_eSharper,          /* Sharper upscale */
    NEXUS_Graphics2DFilterCoeffs_eBlurry,           /* 3-point filtering */
    NEXUS_Graphics2DFilterCoeffs_eAntiFlutter,      /* Anti-Flutter filtering */
    NEXUS_Graphics2DFilterCoeffs_eAntiFlutterBlurry,/* Blurry Anti-Flutter filtering */
    NEXUS_Graphics2DFilterCoeffs_eAntiFlutterSharp, /* Sharp Anti-Flutter filtering */
    NEXUS_Graphics2DFilterCoeffs_eMax
} NEXUS_Graphics2DFilterCoeffs;

#define NEXUS_GRAPHICS2D_COLOR_MATRIX_COEFF_COUNT           (20)

/***************************************************************************
Summary:
5x4 coefficient matrix for blitter color space convertor

Description:
Used in NEXUS_Graphics2DBlitSettings to modify incoming source values.

The 5x4 matrix is applied as follows:

    [C2out]   [M[0]  M[1]  M[2]  M[3]  M[4] ]   [C2in]
    [C1out]   [M[5]  M[6]  M[7]  M[8]  M[9] ]   [C1in]
    [C0out] = [M[10] M[11] M[12] M[13] M[14]] * [C0in]
    [Aout ]   [M[15] M[16] M[17] M[18] M[19]]   [Ain ]
    [ 1   ]                                     [ 1  ]

    C2out =  M[0]*C2in +  M[1]*C1in +  M[2]*C0in +  M[3]*Ain +  M[4]
    C1out =  M[5]*C2in +  M[6]*C1in +  M[7]*C0in +  M[8]*Ain +  M[9]
    C0out = M[10]*C2in + M[11]*C1in + M[12]*C0in + M[13]*Ain + M[14]
    Aout  = M[15]*C2in + M[16]*C1in + M[17]*C0in + M[18]*Ain + M[19]

This is an identity matrix: M[0] = M[6] = M[12] = M[18] = 1; shift = 0;

An alpha premultiply can be accomplished with:

    uint8_t alpha;
    M[0] = M[6] = M[12] = alpha;
    M[18] = 256;
    shift = 8; // 2^8 == 256

See BGRC_Source_SetColorMatrix5x4 for more detailed comments
****************************************************************************/
typedef struct NEXUS_Graphics2DColorMatrix
{
    unsigned shift; /* number of LSB bits that are the fractional part of the fixed point coeffMatrix[] values  */
    int32_t  coeffMatrix[NEXUS_GRAPHICS2D_COLOR_MATRIX_COEFF_COUNT];
} NEXUS_Graphics2DColorMatrix;


/*****************************************************************************
Summary:
Pattern (raster operations) settings used in NEXUS_Graphics2DBlitSettings
*****************************************************************************/
typedef struct NEXUS_Graphics2DPatternSettings
{
    uint8_t  ropVector;     /* 8 bit ROP vector. */
    uint8_t  pattern[64];   /* 8x8 bit ROP pattern. */
    uint32_t backColor;     /* ROP background color. */
    uint32_t foreColor;     /* ROP foreground color. */
} NEXUS_Graphics2DPatternSettings;

/*****************************************************************************
Summary:
Scaling control block for blit
*****************************************************************************/
typedef struct NEXUS_Graphics2DScalingControlSettings
{
    /* See BGRC_Source_SetFilterPhaseAdjustment for additional documentation  */
    bool     setFilterPhaseAdj;       /* true: Set the filter phase adjustment */
    int32_t  horizontalPhase;         /* Horizontal initial phase. */
    int32_t  verticalPhase;           /* Vertical initial phase. */
    uint32_t phaseFixedPointShift;    /* Fixed-point shift, in bits, to allow for fractional values. */

    /* BGRC_Source_SetFixedScaleFactor for additional documentation */
    bool     setFixedScaleFactor;     /* true: Set the fixed scale factor */
    uint32_t horizontalNumerator;     /* Horizontal/Source numerator */
    uint32_t horizontalDenominator;   /* Horizontal/Source denominator */
    uint32_t verticalNumerator;       /* Vertical/Output numerator */
    uint32_t verticalDenominator;     /* Vertical/Output denominator */
} NEXUS_Graphics2DScalingControlSettings;

/**
Summary:
Parameters for NEXUS_Graphics2D_Blit

Description:
The HW blitter has a maximum vertical and horizontal downscale (current 15x). If you exceed
this limit, you will get an error from the low-level software under Nexus. If you need a
greater downscale, you will need to perform multiple passes using an intermediate buffer.

Note regarding palettized destination surfaces: the M2MC hardware cannot perform palette lookups
on a destination surface. Therefore, any blend operation with a palettized destination surface
is going to be a blend of the index, not a blend of the color referenced by the index.
This has some usefulness, but only in a some narrow usage cases. For instance, a palette can be constructed
where the 8 bits are 3R, 2G, 2B. This would have a meaningful blend.
If you have not set up your destination palette index to have a meaningful blend, you will get strange results
if you attempt a blend.

Used in NEXUS_Graphics2D_Blit
**/
typedef struct NEXUS_Graphics2DBlitSettings
{
    struct
    {
        NEXUS_SurfaceHandle surface;  /* May be NULL for some operations. */
        NEXUS_Rect rect;              /* width,height of 0,0 for source or output indicates the entire surface.
                                         dest's width,height are ignored because, by definition, they must be the same as output's. */
    } source, dest, output;

    NEXUS_BlitColorOp       colorOp;         /* Specify how to blit the color component */
    NEXUS_BlitAlphaOp       alphaOp;         /* Specify how to blit the alpha component */

    NEXUS_BlendEquation     colorBlend;      /* Used only if colorOp = NEXUS_BlitColorOp_eUseBlendEquation */
    NEXUS_BlendEquation     alphaBlend;      /* Used only if alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation */

    NEXUS_Pixel             constantColor;       /*  Constant alpha and color. Only standard color format supported (see NEXUS_ColorFormatType). Used by some operations. */
    bool                    useDestAndSourceConstantColor; /* if false, constantColor is used for BLEND, SRC and DEST blocks.
                                                              if true, destConstantColor and sourceConstantColor are used. */
    NEXUS_Pixel             destConstantColor;   /* color for DEST block, if useDestAndSourceConstantColors is true. Used if DEST surface has no color. */
    NEXUS_Pixel             sourceConstantColor; /* color for SOURCE block, if useDestAndSourceConstantColors is true. Used if SOURCE surface has no color. */

    struct {
        struct {
            bool enabled;     /* Enable colorkey. If disabled, the following values are ignored. */
            NEXUS_Pixel lower;   /* Lower colorkey value. Only standard color format supported (see NEXUS_ColorFormatType). */
            NEXUS_Pixel upper;   /* Upper colorkey value. Only standard color format supported (see NEXUS_ColorFormatType). */
            NEXUS_Pixel mask;    /* Mask to use when comparing color key to source color */
            NEXUS_Pixel replace; /* Replacement color */
            NEXUS_Pixel replaceMask; /*  Mask for replacement color */
        } source, dest;
    } colorKey;

    bool conversionMatrixEnabled;   /* if true, use conversionMatrix. no automatic YCrCb/RGB conversion will take place. */
    NEXUS_Graphics2DColorMatrix conversionMatrix; /* modifying incoming source values before any blend */

    bool patternSettingsEnabled; /* if true, use patternSettings */
    NEXUS_Graphics2DPatternSettings patternSettings;

    NEXUS_Graphics2DFilterCoeffs horizontalFilter;
    NEXUS_Graphics2DFilterCoeffs verticalFilter;

    bool scalingControlSettingsEnabled; /* if true, use scalingControlSettings */
    NEXUS_Graphics2DScalingControlSettings scalingControlSettings;

    /* Blit direction reversal flags. Default is false which is top to bottom, left to right.
    If you mirror source, dest and output at the same time, you will get non-mirrored output. This is a useful technique for a memmove-style blit
    where the output overlaps the source in the same surface. This is done by default if mirror booleans are left at their default false values. */
    bool mirrorSourceVertically; /* If true, read source bottom to top. */
    bool mirrorSourceHorizontally; /* If true, read source right to left. */
    bool mirrorDestVertically; /* If true, read dest bottom to top. */
    bool mirrorDestHorizontally; /* If true, read dest left to right. */
    bool mirrorOutputVertically; /* If true, write output bottom to top. */
    bool mirrorOutputHorizontally; /* If true, write output left to right. */

    /* Enable/Disable Alpha Premultiplication */
    bool alphaPremultiplySourceEnabled; /* If true, then each color component in the source surface will be modulated with the
                                           source's alpha component before scaling. */
} NEXUS_Graphics2DBlitSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.
**/
void NEXUS_Graphics2D_GetDefaultBlitSettings(
    NEXUS_Graphics2DBlitSettings *pSettings /* [out] */
    );

/**
Summary:
Perform a block transfer (blit) from one or two surfaces to a destination.

Description:
This function is non-blocking. It returns immediately without waiting for the fill to actually happen.

Example:
A simple copy of a whole surface to another surface can be performed as follows:

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = source_surface;
    blitSettings.output.surface = output_surface;
    NEXUS_Graphics2D_Blit(handle, &blitSettings);

If packetFifoThreshold is non-zero, NEXUS_Graphics2D_Checkpoint is required to submit queue blits to hardware.
The NEXUS_Graphics2D_Checkpoint callback is also used to determine when this blit is done.
**/
NEXUS_Error NEXUS_Graphics2D_Blit(
    NEXUS_Graphics2DHandle              handle,
    const NEXUS_Graphics2DBlitSettings *pSettings
    );

/**
Summary:
Perform a block transfer (blit) from one surface to a destination.

Description:
This is a faster, less feature-rich version of NEXUS_Graphics2D_Blit.

Example:
A simple copy of a whole surface to another surface can be performed as follows:

    NEXUS_Graphics2D_FastBlit(handle,
        output_surface, NULL,
        source_surface, NULL,
        NEXUS_BlitColorOp_eCopySource,
        NEXUS_BlitAlphaOp_eCopySource,
        0);

Only the following ops are supported:
    NEXUS_BlitColorOp_eCopySource
    NEXUS_BlitAlphaOp_eCopySource
    NEXUS_BlitAlphaOp_eCopyConstant

This blit does not support color keying, dest surface, matrix conversion or YCrCb/RGB conversion.

If packetFifoThreshold is non-zero, NEXUS_Graphics2D_Checkpoint is required to submit queue blits to hardware.
The NEXUS_Graphics2D_Checkpoint callback is also used to determine when this blit is done.
**/
NEXUS_Error NEXUS_Graphics2D_FastBlit(
    NEXUS_Graphics2DHandle handle,
    NEXUS_SurfaceHandle outputSurface,
    const NEXUS_Rect *pOutputRect,          /* attr{null_allowed=y} if NULL, blit to whole surface */
    NEXUS_SurfaceHandle sourceSurface,
    const NEXUS_Rect *pSourceRect,          /* attr{null_allowed=y} if NULL, blit from whole surface */
    NEXUS_BlitColorOp colorOp,
    NEXUS_BlitAlphaOp alphaOp,
    NEXUS_Pixel constantColor
    );

/**
Summary:
PorterDuff Operations used in NEXUS_Graphics2D_PorterDuffFill and NEXUS_Graphics2D_PorterDuffBlit

Description:
Used in NEXUS_Graphics2DPorterDuffFillSettings and NEXUS_Graphics2DPorterDuffBlitSettings
**/
typedef enum NEXUS_PorterDuffOp
{
    NEXUS_PorterDuffOp_eClear = 0,   /* Erase output  */
    NEXUS_PorterDuffOp_eSrc,         /* Copy source to output */
    NEXUS_PorterDuffOp_eDst,         /* Copy dest to output */
    NEXUS_PorterDuffOp_eSrcOver,     /* Source over dest */
    NEXUS_PorterDuffOp_eDstOver,     /* Dest over source */
    NEXUS_PorterDuffOp_eSrcIn,       /* Part of source within dest */
    NEXUS_PorterDuffOp_eDstIn,       /* Part of dest within source */
    NEXUS_PorterDuffOp_eSrcOut,      /* Part of source not in dest */
    NEXUS_PorterDuffOp_eDstOut,      /* Part of dest not in source */
    NEXUS_PorterDuffOp_eSrcAtop,     /* Part of source in dest and remainder is dest */
    NEXUS_PorterDuffOp_eDstAtop,     /* Part of dest in source and remainder is source */
    NEXUS_PorterDuffOp_eXor,         /* Part of src not in dest and part of dest not in source */
    NEXUS_PorterDuffOp_eMax
} NEXUS_PorterDuffOp;

/**
Summary:
Parameters for NEXUS_Graphics2D_PorterDuffFill

Description:
Used in NEXUS_Graphics2D_GetDefaultPorterDuffFillSettings and NEXUS_Graphics2D_PorterDuffFill
**/
typedef struct NEXUS_Graphics2DPorterDuffFillSettings
{
    NEXUS_PorterDuffOp    operation;       /*  A Porter-Duff operation */
    NEXUS_SurfaceHandle   surface;
    NEXUS_Pixel           color;           /* Color to fill. See NEXUS_Graphics2DSettings.colorFormatType for the pixel format of this color value. */
    NEXUS_Rect            rect;            /* Area of surface to fill. width,height of 0,0 fills the entire surface */
} NEXUS_Graphics2DPorterDuffFillSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.
**/
void NEXUS_Graphics2D_GetDefaultPorterDuffFillSettings(
    NEXUS_Graphics2DPorterDuffFillSettings *pSettings /* [out] */
    );

/**
Summary:
Fill, tint or otherwise modify the pixels of an area of a surface using a constant value and
Porter-Duff composition rules.

Description:
This routine modifies the color channels and/or the alpha channel of the
pixels of a surface using a combination of constant value and Porter-Duff composition rules.
This function is non-blocking. It returns immediately without waiting for the fill to actually happen.

If packetFifoThreshold is non-zero, NEXUS_Graphics2D_Checkpoint is required to submit queue blits to hardware.
The NEXUS_Graphics2D_Checkpoint callback is also used to determine when this blit is done.
**/
NEXUS_Error NEXUS_Graphics2D_PorterDuffFill(
    NEXUS_Graphics2DHandle handle,
    const NEXUS_Graphics2DPorterDuffFillSettings *pSettings
    );

/**
Summary:
Parameters for a Porter-Duff blit.

Description:
Used in NEXUS_Graphics2D_PorterDuffBlit
**/
typedef struct NEXUS_Graphics2DPorterDuffBlitSettings
{
    NEXUS_PorterDuffOp   operation;        /*  A Porter-Duff operation */
    NEXUS_SurfaceHandle  sourceSurface;
    NEXUS_Rect           sourceRect;       /*  Area of sourceSurface to blit from. width,height of 0,0 blits from the entire surface */
    NEXUS_SurfaceHandle  destSurface;
    NEXUS_Rect           destRect;         /*  Area of destSurface to blit from. width,height of 0,0 blits from the entire surface */
    NEXUS_SurfaceHandle  outSurface;
    NEXUS_Rect           outRect;          /*  Area of outSurface to blit from. width,height of 0,0 blits to the entire surface */
} NEXUS_Graphics2DPorterDuffBlitSettings;

/**
Summary:
Get default settings for the structure.

Description:
;his is required in order to make application code resilient to the addition of new structure members in the future.
**/
void NEXUS_Graphics2D_GetDefaultPorterDuffBlitSettings(
    NEXUS_Graphics2DPorterDuffBlitSettings *pSettings /* [out] */
    );

/**
Summary:
Combine two surfaces into a third using Porter-Duff composition rules

Description:
This routine combines an area of the source surface and an area of the
destination surface into an area of the output surface using Porter-Duff
composition rules. No color space conversion or color keying is performed.

If the source surface is palette-based and the output surface is not
paletted-based, then the palette entries in the source pixels will be
converted to colors in the output pixels

This function is non-blocking. It returns immediately without waiting for the fill to actually happen.

If packetFifoThreshold is non-zero, NEXUS_Graphics2D_Checkpoint is required to submit queue blits to hardware.
The NEXUS_Graphics2D_Checkpoint callback is also used to determine when this blit is done.
**/
NEXUS_Error NEXUS_Graphics2D_PorterDuffBlit(
    NEXUS_Graphics2DHandle handle,
    const NEXUS_Graphics2DPorterDuffBlitSettings *pSettings
    );

/**
Summary:
Submit queue blits to hardware and check status of the graphics hardware for completion of all blit and fill operations.

Description:
If packetFifoThreshold is non-zero, NEXUS_Graphics2D_Checkpoint is required to submit queue blits to hardware.

Prior to the CPU accessing the surface memory (see NEXUS_Surface_GetMemory)
or before setting a surface as the current framebuffer (see NEXUS_Display_SetGraphicsFramebuffer)
you should check that the graphics hardware has completed all operations.
For maximum throughput, you should only call NEXUS_Graphics2D_Checkpoint when one of these boundary conditions occurs.

NEXUS_Graphics2D_Checkpoint will notify the caller when all blits prior it being called have completed.
NEXUS_Graphics2D_Checkpoint can either return immediately with those results or return immediately after
scheduling a checkpointCallback.
NEXUS_Graphics2D_Checkpoint never blocks.

Return values if pLegacyCallback is NULL:
    NEXUS_SUCCESS - there are no outstanding blit and fill operations in the hardware.
    NEXUS_GRAPHICS2D_QUEUED - there are some outstanding blit and/or fill operations in the hardware queue. when these blits are completed NEXUS_Graphics2DSettings.checkpointCallback is fired.
    NEXUS_GRAPHICS2D_BUSY - there are some outstanding blit and/or fill operations in the hardware queue. This can only occur if NEXUS_Graphics2DSettings.pollingCheckpoint is true.

Return values if pLegacyCallback is not NULL (not recommended):
    NEXUS_SUCCESS - the pLegacyCallback has been scheduled. wait on that callback to learn when all blits prior to NEXUS_Graphics2D_Checkpoint have completed.
    Non-zero value - a system error has occured

Be aware that if NEXUS_Graphics2D_Checkpoint schedules a checkpointCallback, it is possible for other blits to be issued after NEXUS_Graphics2D_Checkpoint is called
but before the checkpointCallback arrives. When the checkpointCallback arrives, it only means that all blits prior to NEXUS_Graphics2D_Checkpoint have completed.

NEXUS_Graphics2D_Checkpoint should not be called from multiple threads with the same handle.
Instead, each thread should open its own instance of NEXUS_Graphics2D.
See nexus/examples/graphics/multithreaded_fill for an example of how to write multi-threaded graphics code.

See Also:
NEXUS_Graphics2D_Blit
NEXUS_Graphics2D_Fill
NEXUS_Graphics2D_PorterDuffFill
NEXUS_Graphics2D_PorterDuffBlit
NEXUS_Surface_GetMemory
NEXUS_Display_SetGraphicsFramebuffer
NEXUS_Graphics2DSettings
**/
NEXUS_Error NEXUS_Graphics2D_Checkpoint(
    NEXUS_Graphics2DHandle handle,
    const NEXUS_CallbackDesc *pLegacyCallback /* attr{null_allowed=y} Legacy argument. New code should pass NULL here and use NEXUS_Graphics2DSettings.checkpointCallback instead. */
    );

/**
Summary:
Create a new surface by copying from a striped surface

Description:
A striped surface is the output of NEXUS_StillDecoder_GetStripedSurface
When the user is done with the returned surface, the user must destroy it by calling NEXUS_Surface_Destroy.
**/
NEXUS_SurfaceHandle NEXUS_Graphics2D_Destripe(
    NEXUS_Graphics2DHandle handle,
    NEXUS_StripedSurfaceHandle stripedSurface
    );

/**
Summary:
Destripe a surface into another surface which has already been created

Description:
Call NEXUS_StripedSurface_GetStatus to get the width and height before destriping.

A striped surface is the output of NEXUS_StillDecoder_GetStripedSurface.

This function is preferred over NEXUS_Graphics2D_Destripe in a memory constrained system because the application
can minimize heap allocations.

If the striped surface is smaller than the target surface (in both width and height), then the resulting destriped pixels
will be placed in the upper left corner of the target surface. If the striped surface is larger than the target surface,
this function will fail.
**/
NEXUS_Error NEXUS_Graphics2D_DestripeToSurface(
    NEXUS_Graphics2DHandle handle,
    NEXUS_StripedSurfaceHandle stripedSurface,
    NEXUS_SurfaceHandle targetSurface,
    const NEXUS_Rect *pTargetRect /* attr{null_allowed=y} optional */
    );

/**
Summary:
Perform a 32 bit memset using M2MC hardware

Description:
This call is asynchronous, just like NEXUS_Graphics2D_Blit. Use NEXUS_Graphics2D_Checkpoint.

Some caveats:
- The count parameter is the number of uint32_t's, not the number of bytes.
- The count parameter must be a multiple of 1024, which means NEXUS_Graphics2D_Memset32 works in multiples of 4096 bytes.
- Because NEXUS_Graphics2D_Memset32 uses the M2MC hardware, you may need to flush the cache (NEXUS_Memory_FlushCache) before calling it.
**/
NEXUS_Error NEXUS_Graphics2D_Memset32(
    NEXUS_Graphics2DHandle handle,
    void *address,  /* attr{memory=cached} address to buffer which was allocated from the heap (e.g. NEXUS_Memory_Allocate). */
    uint32_t data,  /* 32 value to write to the buffer */
    unsigned count  /* number of uint32_t's to write (not number of bytes). because of constraints of M2MC hardware, count must be between 1K and 16M and
                       must be evenly divisible by 1K. */
    );

/**
Summary:
Deprecated. Use packet blit instead.
**/
typedef struct NEXUS_Graphics2DBatchBlitSettings
{
    NEXUS_SurfaceHandle outSurface;   /* output surface (always set) */
    const uint32_t     *pOutOffsets;  /* attr{memory=cached} pointer to output device offsets (replaces address in output surface) */
    const uint16_t     *pOutPoints;   /* attr{memory=cached} pointer to output x and y buffer (always set) */
    const uint16_t     *pOutSizes;    /* attr{memory=cached} pointer to output width and height buffer (always set) */
    NEXUS_SurfaceHandle srcSurface;   /* source surface */
    const uint32_t     *pSrcOffsets;  /* attr{memory=cached} pointer to source device offsets (replaces address in source surface) */
    const uint16_t     *pSrcPoints;   /* attr{memory=cached} pointer to source x and y buffer */
    const uint16_t     *pSrcSizes;    /* attr{memory=cached} pointer to source width and height buffer (only set when scaling) */
    NEXUS_SurfaceHandle dstSurface;   /* destination surface (only set with blends, rops, or colorkey) */
    const uint32_t     *pDstOffsets;  /* attr{memory=cached} pointer to destination device offsets (replaces address in destination surface) */
    const uint16_t     *pDstPoints;   /* attr{memory=cached} pointer to destination x and y buffer */
    const NEXUS_Pixel  *pColors;      /* attr{memory=cached} pointer to constant color buffer (32bpp ARGB or AYCbCr)*/
} NEXUS_Graphics2DBatchBlitSettings;

/**
Summary:
Deprecated. Use packet blit instead.
**/
void NEXUS_Graphics2D_GetDefaultBatchBlitSettings(
    NEXUS_Graphics2DBatchBlitSettings *pSettings /* [out] */
    );

/**
Summary:
Deprecated. Use packet blit instead.
**/
NEXUS_Error NEXUS_Graphics2D_BatchBlit(
    NEXUS_Graphics2DHandle handle,
    size_t count,     /* number of blits in batch */
    size_t index,     /* index of first blit */
    size_t *next,     /* [out] index of first blit not submitted due to lack of memory. */
                      /* All blits are complete when *next=count after returning. */
    const NEXUS_Graphics2DBatchBlitSettings *pBatchSettings,   /* surfaces and pointers to point, size, and color buffers */
    const NEXUS_Graphics2DBlitSettings *pBlitSettings          /* blit state */
    );

/**
Summary:
Get memory for packet-based blitting

Description:
See bm2mc_packet.h and nexus/examples/graphics/packet_* for packet API documentation.
**/
NEXUS_Error NEXUS_Graphics2D_GetPacketBuffer(
    NEXUS_Graphics2DHandle handle,
    void **pBuffer, /* [out] attr{memory=cached} */
    size_t *pSize, /* [out] */
    size_t minSize
    );

/**
Summary:
Submit packet blits
**/
NEXUS_Error NEXUS_Graphics2D_PacketWriteComplete(
    NEXUS_Graphics2DHandle handle,
    size_t size
    );

/**
Summary:
Applications can #if NEXUS_GRAPHICS2D_PACKET_SUPPORT if they are running older versions
which do not have the packet api.
**/
#define NEXUS_GRAPHICS2D_PACKET_SUPPORT

#include "bm2mc_packet.h"

/**
Summary:
Convert NEXUS_Graphics2DFilterCoeffs to BM2MC_PACKET_FilterCoeffs
**/
NEXUS_Error NEXUS_Graphics2D_ConvertFilter(
    NEXUS_Graphics2DFilterCoeffs filter,
    size_t srcSize,                        /* source width or height */
    size_t outSize,                        /* output width or height */
    BM2MC_PACKET_FilterCoeffs *pCoeffs     /* [out] packet filter coeffs */
    );

/**
Summary:
Convert NEXUS_Graphics2DColorMatrix to BM2MC_PACKET_ColorMatrix
**/
NEXUS_Error NEXUS_Graphics2D_ConvertColorMatrix(
    const NEXUS_Graphics2DColorMatrix *pMatrixIn,
    BM2MC_PACKET_ColorMatrix *pMatrixOut /* [out] */
    );

/**
Summary:
Set up a matrix to do alpha blend
**/
void NEXUS_Graphics2D_SetAlphaColorMatrix(
    uint8_t alpha,
    NEXUS_Graphics2DColorMatrix *pMatrix /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif
