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
 * $brcm_Workfile: nexus_video_adj.h $
 * $brcm_Revision: 26 $
 * $brcm_Date: 6/15/12 12:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/include/nexus_video_adj.h $
 * 
 * 26   6/15/12 12:45p erickson
 * SW7425-3202: add NEXUS_VideoWindow_SetCompressionSettings
 * 
 * 25   4/12/12 12:58p erickson
 * SW7420-2286: add NEXUS_VideoWindowMadSettings.pqEnhancement, default
 *  eOff
 *
 * 24   3/15/12 11:42a erickson
 * SW7420-2278: clarify that enable32Pulldown is actually enabling reverse
 *  3:2 pulldown. default is now false.
 *
 * 23   3/22/11 9:29a erickson
 * SW7405-5201: update comments on NEXUS_ScalerCaptureBias
 *
 * 22   3/2/11 3:13p erickson
 * SW7420-1285: fix null_allowed attr
 *
 * 21   12/14/10 3:18p erickson
 * SW7420-1285: add null_allowed attribute
 *
 * 20   8/27/10 8:59a erickson
 * SW7420-965: sync up comments with 3548 nexus_video_adj.h
 *
 * 19   8/27/10 8:53a erickson
 * SW7420-965: add two more NEXUS_VideoWindowGameMode enums
 *
 * 18   8/26/10 3:32p erickson
 * SW7420-965: add NEXUS_VideoWindowGameMode_e4Fields_2Delay
 *
 * 17   5/7/10 4:06p erickson
 * SWDEPRECATED-2425: fix comments on
 *  NEXUS_VideoWindowFilterSettings.level
 *
 * 16   4/1/10 2:59p erickson
 * SW7405-4074: add usage comments for NEXUS_VideoWindowGameMode
 *
 * 15   2/10/10 2:34p erickson
 * SW7405-3829: add NEXUS_VideoWindowMadSettings.shrinkWidth
 *
 * 14   10/29/08 11:38a erickson
 * PR48028: added deringing and dejagging options to
 *  NEXUS_VideoWindowScalerSettings
 *
 * 13   10/22/08 5:24p erickson
 * PR48023: update comments
 *
 * 12   9/25/08 9:45a erickson
 * PR47113: replace NEXUS_VideoWindow_SetScalerCoefficientIndex with
 *  NEXUS_VideoWindow_SetCoefficientIndexSettings
 *
 * 11   9/16/08 5:17p erickson
 * PR46967: fix naming of NEXUS_VideoWindowScalerSettings
 *
 * 10   7/17/08 9:34a erickson
 * PR44764: update for BVDC_Window_SetBandwidthEquationParams api change
 *
 * 9   6/18/08 10:04a erickson
 * PR43617: extended NEXUS_VideoWindowScalerSettings with bandwidth equation
 *  params
 *
 * 8   3/27/08 9:49a erickson
 * PR40950: remove unused variables
 *
 * 7   3/26/08 2:36p erickson
 * PR40950: set custom ANR/DNR data in custom function
 *
 * 6   3/26/08 12:54p erickson
 * PR40567: add MAD and ANR pixel format
 *
 * PR40567/1   3/26/08 4:19p xhuang
 * PR40567:add pixel format set in Nexus
 *
 * 5   1/28/08 1:07p erickson
 * PR35457: add change notice
 *
 * 4   1/25/08 9:42a erickson
 * PR39003: adapt to latest VDC
 *
 * PR39003/1   1/25/08 3:06p xhuang
 * PR39003:update MAD game mode define
 *
 * 3   1/23/08 8:37p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 5:16p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/13   1/8/08 3:52p erickson
 * PR36159: rename DNR/ANR/MAD functions to be VideoWindow
 *
 * Nexus_Devel/12   1/8/08 10:57a erickson
 * PR36159: refactor some 3563 Display API's
 *
 * Nexus_Devel/11   12/18/07 1:35p erickson
 * PR38213: analog video input rework
 *
 * Nexus_Devel/10   11/30/07 10:52a erickson
 * PR35457: api update
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_ADJUST_H__
#define NEXUS_VIDEO_ADJUST_H__

#include "nexus_types.h"
#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=*
The Video Adjustment interface allows manipulation of BVN-middle processing units.

NOTE: This API will be revised for BCM3548. The functions and structures will change
in future Nexus releases.
**/

/**
Summary:
Mode used in NEXUS_VideoWindowFilterSettings
**/
typedef enum NEXUS_VideoWindowFilterMode
{
    NEXUS_VideoWindowFilterMode_eDisable = 0,
    NEXUS_VideoWindowFilterMode_eBypass,
    NEXUS_VideoWindowFilterMode_eEnable,
    NEXUS_VideoWindowFilterMode_eMax
} NEXUS_VideoWindowFilterMode;

/**
Summary:
Filter settings used in NEXUS_VideoWindowDnrSettings and NEXUS_VideoWindowAnrSettings.
**/
typedef struct NEXUS_VideoWindowFilterSettings
{
    NEXUS_VideoWindowFilterMode mode; /* mode of noise reduction */
    int level; /* level of noise reduction, valid range -100 ... 2^31 */
} NEXUS_VideoWindowFilterSettings;

/**
Summary:
Digital Noise Reduction (DNR) settings

Description:
See BVDC_Dnr_Settings for detailed information
**/
typedef struct NEXUS_VideoWindowDnrSettings
{
    NEXUS_VideoWindowFilterSettings mnr; /* Mosquito Noise Reduction. See BVDC_Dnr_Settings for mnr.level range. */
    NEXUS_VideoWindowFilterSettings bnr; /* Block Noise Reduction. See BVDC_Dnr_Settings for bnr.level range. */
    NEXUS_VideoWindowFilterSettings dcr; /* Digital Contour Reduction. See BVDC_Dnr_Settings for dcr.level range. */
    unsigned qp; /* non zero value is used to force constant QP (0 is default and used to derive QP  from the decoded stream) */
} NEXUS_VideoWindowDnrSettings;

/**
Summary:
Analog Noise Reduction (ANR) settings

Description:
anr.level is used for user adjustment to S/N db number.
If S/N is around 60 db, the video signal is very clean and ANR
will be configured to perform very little filtering. When S/N db
number become smaller, the video signal is more noisy and ANR will
be configured to perform stronger filtering. As it reaches to
about 25 db, ANR filtering reaches the strongest.
**/
typedef struct NEXUS_VideoWindowAnrSettings
{
    NEXUS_VideoWindowFilterSettings anr; /* Analog Noise Reduction */

    NEXUS_PixelFormat pixelFormat;       /* pixel format of ANR buffer */
} NEXUS_VideoWindowAnrSettings;

/**
Summary:
MAD game mode options

Description:
Used in NEXUS_VideoWindowMadSettings. Not related to NEXUS_VideoWindowGameModeDelay.

For pause or frame advance on multi-display systems, the delay introduced by MAD can cause noticeably different pictures
to appear on each display. To eliminate this, choose one of the "ForceSpatial" options below.
By default, nexus will configure MAD to allocate 5 fields for MAD (worst case), so we recommend you use
NEXUS_VideoWindowGameMode_e5Fields_ForceSpatial.
**/
typedef enum NEXUS_VideoWindowGameMode
{
    NEXUS_VideoWindowGameMode_eOff,                     /* Disable MAD Game Mode */
	NEXUS_VideoWindowGameMode_e5Fields_2Delay,          /* MAD Game Mode 5 Active Pixel Fields, 2 field delay */
    NEXUS_VideoWindowGameMode_e5Fields_1Delay,          /* MAD Game Mode 5 Active Pixel Fields, 1 field delay */
    NEXUS_VideoWindowGameMode_e5Fields_0Delay,          /* Mad Game Mode 5 Active Pixel Fields, 0 Field Delay */
    NEXUS_VideoWindowGameMode_e5Fields_ForceSpatial,    /* Mad Game Mode 5 Active Pixel Fields, 0 Field Delay, Spatial Only */
    NEXUS_VideoWindowGameMode_e4Fields_2Delay,          /* Mad Game Mode 4 Active Pixel Fields, 2 Field Delay */
    NEXUS_VideoWindowGameMode_e4Fields_1Delay,          /* Mad Game Mode 4 Active Pixel Fields, 1 Field Delay */
    NEXUS_VideoWindowGameMode_e4Fields_0Delay,          /* Mad Game Mode 4 Active Pixel Fields, 0 Field Delay */
    NEXUS_VideoWindowGameMode_e4Fields_ForceSpatial,    /* Mad Game Mode 4 Active Pixel Fields, 0 Field Delay, Spatial Only */
	NEXUS_VideoWindowGameMode_e3Fields_2Delay,          /* Mad Game Mode 3 Active Pixel Fields, 2 Field Delay */
    NEXUS_VideoWindowGameMode_e3Fields_1Delay,          /* Mad Game Mode 3 Active Pixel Fields, 1 Field Delay */
    NEXUS_VideoWindowGameMode_e3Fields_0Delay,          /* Mad Game Mode 3 Active Pixel Fields, 0 Field Delay */
    NEXUS_VideoWindowGameMode_e3Fields_ForceSpatial,    /* Mad Game Mode 3 Active Pixel Fields, 0 Field Delay, Spatial Only */
    NEXUS_VideoWindowGameMode_eMax
} NEXUS_VideoWindowGameMode;

/**
Summary:
Multi-buffering game delay mode settings

Description:
Used in NEXUS_VideoWindow_SetGameModeDelay

The game mode delay control can place the sync-slipped window's multi-
buffer playback pointer to a target offset relative to its capture
pointer to achieve smaller multi-buffer delay and faster game response.

Note, to make sure the game mode delay control converges and stablize,
before enabling game mode delay control function, user should make sure
the followings:

1) make the window's display track a DPCR timebase;
2) program the DPCR timebase to lock the window's source rate;

Also note,

- This function is currently meant for the DTV application with
  DVO panel output; analog output might cause TV rolling or loss of sync
  during adjustment; analog and digital outputs of the same display cannot
  simultaneously turn on if a window enables game mode;
- A display can only have one window with game mode delay
  control enabled.
- Only sync-slipped window can have this feature.

This is not related to MAD game mode. See NEXUS_VideoWindowMadSettings.gameMode for MAD game mode.
**/
typedef struct NEXUS_VideoWindowGameModeDelay
{
    bool enable; /* Enable the feature; default OFF; if there is significant
                    rate gap between source and display, like 50->60, VDC will
                    internally disable it */
    bool forceCoarseTrack; /* Force coarse tracking of non-standard source refresh
                    rate for game mode display;
                    when this flag is turned on, display can track the source
                    rate within +-0.3% variance of the nominal value.
                    When this flag is turned off, VDC may internally switch
                    between fine track and coarse track: if source
                    format is PC format, or window master frame rate tracking
                    is off, VDC will still use coarse adjustment anyhow. So this
                    flag is mainly an override for TV input rate way off from
                    its nominal value; default OFF */
    uint32_t bufferDelayTarget; /* bufferDelayTarget    - This is the desired difference (in usecs) between
                    source VSYNC and the display VSYNC at the capture buffer*/
    uint32_t bufferDelayTolerance; /* This is the tolerance from the center as defined by
                    bufferDelayTarget. This is in usecs.*/
} NEXUS_VideoWindowGameModeDelay;

/**
Summary:
Enabled general picture quality enhancement features for deinterlacer
**/
typedef enum NEXUS_MadPqEnhancement
{
    NEXUS_MadPqEnhancement_eAuto, /* internal decision */
    NEXUS_MadPqEnhancement_eOff,  /* force off */
    NEXUS_MadPqEnhancement_eOn,   /* force on */
    NEXUS_MadPqEnhancement_eMax
} NEXUS_MadPqEnhancement;

/**
Summary:
Motion Adaptive Deinterlacer (MAD) settings
**/
typedef struct NEXUS_VideoWindowMadSettings
{
    bool deinterlace;                   /* If true, MAD is allowed to be used if available and appropriate. */
    bool enable32Pulldown;              /* enable reverse 3:2 pulldown. defaults to false.
                                           this feature enables the deinterlacer's detection/recovery of progressive picture from 3:2 pulled-down contents. */
    bool enable22Pulldown;              /* enable reverse 2:2 pulldown. defaults to false.
                                           this feature enables the deinterlacer's detection/recovery of progressive picture from 2:2 pulled-down contents. */
    NEXUS_VideoWindowGameMode gameMode; /* Include game mode */
    NEXUS_PixelFormat pixelFormat;      /* pixel format of MAD buffer */
    bool shrinkWidth;                   /* Internally scale down the width of the input source so that we can deinterlace a source whose width exceeds
                                           the MAD HW limitation. This sacrifices horizontal resolution and requires additional memory allocation and bandwidth.
                                           Ideally, this setting should be applied before calling NEXUS_VideoWindow_AddInput. Otherwise, it will cause a momentary black frame. */
    NEXUS_MadPqEnhancement pqEnhancement;
} NEXUS_VideoWindowMadSettings;

/**
Summary:
SCL/CAP bias for NEXUS_VideoWindowScalerSettings.bandwidthEquationParams

Description:
When configuring the BVN, VDC will often require both a scaler (SCL) and capture (CAP) block.
BVN bandwidth considerations require a certain order, either CAP->SCL or SCL->CAP.
In eAuto mode, this is calculated automatically.

When doing a continuous resize of a video window, you will see a "black flash" when the eAuto calculation
switches from one mode to another. This black flash is caused by a BVN reconfig, which is needed to switch
the order.

If you want to avoid the black flash, you can fix the bias to be always either SCL->CAP or CAP->SCL.
However, this selection will require an RTS analysis and possibly new RTS settings for the BVN.
Unless you perform this analysis, you should leave the bias at eAuto.

See BVDC_Window_SetBandwidthEquationParams for more documentation.
**/
typedef enum NEXUS_ScalerCaptureBias
{
    NEXUS_ScalerCaptureBias_eAuto = 0,            /* dynamically switch between SCL->CAP and CAP->SCL */
    NEXUS_ScalerCaptureBias_eScalerBeforeCapture, /* SCL->CAP */
    NEXUS_ScalerCaptureBias_eScalerAfterCapture   /* CAP->SCL */
} NEXUS_ScalerCaptureBias;


/**
Summary:
Scaler settings
**/
typedef struct NEXUS_VideoWindowScalerSettings
{
    bool        nonLinearScaling;           /* Set true to enable non-linear scaling. */
    uint32_t    nonLinearSourceWidth;       /* Applied if nonLinearScaling is true */
    uint32_t    nonLinearScalerOutWidth;    /* Applied if nonLinearScaling is true */

    /* RTS settings - applies even for linear scaling */
    struct {
        NEXUS_ScalerCaptureBias bias;
        uint32_t delta; /* tolerance for NEXUS_ScalerCaptureBias, in units of 1/10% */
    } bandwidthEquationParams;

    bool verticalDejagging;         /* set true to enable, defaults false */
    bool horizontalLumaDeringing;   /* set true to enable, defaults true */
    bool verticalLumaDeringing;     /* set true to enable, defaults true */
    bool horizontalChromaDeringing; /* set true to enable, defaults true */
    bool verticalChromaDeringing;   /* set true to enable, defaults true */
} NEXUS_VideoWindowScalerSettings;

/* Deprecated name */
#define NEXUS_VideoWindowSclSettings NEXUS_VideoWindowScalerSettings

/**
Summary:
Get current DNR settings
**/
void NEXUS_VideoWindow_GetDnrSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowDnrSettings *pSettings /* [out] */
    );

/**
Summary:
Set new DNR settings

Description:
This function maps to BVDC_Source_SetDnrConfiguration.
You can find additional documentation in bvdc.h for this function.
**/
NEXUS_Error NEXUS_VideoWindow_SetDnrSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowDnrSettings *pSettings
    );

/**
Summary:
Set new DNR settings along with custom data for DNR processing

Description:
The format of the additional data is private.
**/
NEXUS_Error NEXUS_VideoWindow_SetCustomDnrSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowDnrSettings *pSettings,
    const uint8_t *pCustomData,   /* attr{null_allowed=y;nelem=numEntries} additional data */
    unsigned numEntries
    );

/**
Summary:
Get current ANR settings
**/
void NEXUS_VideoWindow_GetAnrSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowAnrSettings *pSettings /* [out] */
    );

/**
Summary:
Set new ANR settings

Description:
This function maps to BVDC_Source_SetAnrConfiguration.
You can find additional documentation in bvdc.h for this function.
**/
NEXUS_Error NEXUS_VideoWindow_SetAnrSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowAnrSettings *pSettings
    );

/**
Summary:
Set new ANR settings along with custom data for ANR processing

Description:
The format of the additional data is private.
**/
NEXUS_Error NEXUS_VideoWindow_SetCustomAnrSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowAnrSettings *pSettings,
    const uint8_t *pCustomData,   /* attr{null_allowed=y;nelem=numEntries} additional data */
    unsigned numEntries /* size of pCustomData in bytes */

    );

/**
Summary:
Get current MAD settings
**/
void NEXUS_VideoWindow_GetMadSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowMadSettings *pSettings /* [out] */
    );

/**
Summary:
Set new MAD settings

Description:
This function maps to BVDC_Window_SetDeinterlaceConfiguration.
You can find additional documentation in bvdc.h for this function.
**/
NEXUS_Error NEXUS_VideoWindow_SetMadSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowMadSettings *pSettings
    );

/**
Summary:
Get scaler settings
**/
void NEXUS_VideoWindow_GetScalerSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowScalerSettings *pSettings /* [out] */
    );

/**
Summary:
Set new scaler settings

Description:
This function maps to BVDC_Window_SetScalerConfiguration.
You can find additional documentation in bvdc.h for this function.
**/
NEXUS_Error NEXUS_VideoWindow_SetScalerSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowScalerSettings *pSettings
    );

/**
Summary:
Set the window game mode buffer delay.

Description:
    The game mode delay control can place the sync-slipped window's multi-
    buffer playback pointer to a target offset relative to its capture
    pointer to achieve smaller multi-buffer delay and faster game response.

    Note, to make sure the game mode delay control converges and stablize,
    before enabling game mode delay control function, user should make sure
    the followings:

    1) make the window's display track a DPCR timebase;
    2) program the DPCR timebase to lock the window's source rate;

    Also note,

    - This function is currently meant for the DTV application with
      DVO panel output; analog output might cause TV rolling or loss of sync
      during adjustment; analog and digital outputs of the same display cannot
      simultaneously turn on if a window enables game mode;
    - A display can only have one window with game mode delay
      control enabled.
    - Only sync-slipped window can have this feature.

This function maps to BVDC_Window_SetGameModeDelay.
You can find additional documentation in bvdc.h for this function.
**/
NEXUS_Error NEXUS_VideoWindow_SetGameModeDelay(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowGameModeDelay *pSettings
    );

/**
Summary:
Get the window game mode buffer delay settings.

Description:
See Also:
NEXUS_VideoWindow_SetGameModeDelay
**/
void NEXUS_VideoWindow_GetGameModeDelay(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowGameModeDelay *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Set indices to select filter coefficients from customizable internal tables

Description:
    This structure allows the user to select customized filter coefficients for scaler, hscaler or mad from the built-in
    or user replacable tables in bvdc_coeffs_priv.c.
    Each value in this structure is used as an index into those tables.
    The default for all indices is 0.

Used in NEXUS_VideoWindow_SetCoefficientIndexSettings.
***************************************************************************/
typedef struct NEXUS_VideoWindowCoefficientIndexSettings
{
    uint32_t sclVertLuma;   /* scaler vertical luma filter index */
    uint32_t sclHorzLuma;   /* scaler horizontal luma filter index */
    uint32_t sclVertChroma; /* scaler vertical chroma filter index */
    uint32_t sclHorzChroma; /* scaler horizontal chroma filter index */

    uint32_t hsclHorzLuma;  /* hscaler horizontal luma filter index */
    uint32_t hsclHorzChroma;/* hscaler horizontal chroma filter index */

    uint32_t madHorzLuma;   /* MAD horizontal luma filter index */
    uint32_t madHorzChroma; /* MAD horizontal chroma filter index */
} NEXUS_VideoWindowCoefficientIndexSettings;

/**
Summary:
Get current coefficient index settings.
**/
void NEXUS_VideoWindow_GetCoefficientIndexSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowCoefficientIndexSettings *pSettings /* [out] */
    );

/**
Summary:
Set new coefficient index settings.

Description:
This function maps to BVDC_Window_SetCoefficientIndex.
You can find additional documentation in bvdc.h for this function.
**/
NEXUS_Error NEXUS_VideoWindow_SetCoefficientIndexSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowCoefficientIndexSettings *pSettings
    );

/**
Summary:
Compression settings
**/
typedef struct NEXUS_VideoWindowCompressionSettings
{
    struct {
        bool enabled;
        unsigned compressionAmount; /* units of half bits per pixel. for example, a value of 10 leads to compression of 5 bits per pixel */
    } mad, anr, cap;
} NEXUS_VideoWindowCompressionSettings;

/**
Summary:
Compression settings
**/
void NEXUS_VideoWindow_GetCompressionSettings(
    NEXUS_VideoWindowHandle handle,
    NEXUS_VideoWindowCompressionSettings *pSettings
    );

/**
Summary:
Compression settings
**/
NEXUS_Error NEXUS_VideoWindow_SetCompressionSettings(
    NEXUS_VideoWindowHandle handle,
    const NEXUS_VideoWindowCompressionSettings *pSettings
    );
    
#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VIDEO_ADJUST_H__ */

