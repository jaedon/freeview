/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_picture_ctrl.h $
 * $brcm_Revision: 20 $
 * $brcm_Date: 3/2/11 3:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/include/nexus_picture_ctrl.h $
 * 
 * 20   3/2/11 3:13p erickson
 * SW7420-1285: fix null_allowed attr
 *
 * 19   12/14/10 3:18p erickson
 * SW7420-1285: add null_allowed attribute
 *
 * 18   12/9/10 12:00p erickson
 * SW35230-2492: move NEXUS_ClipRect to nexus_types.h
 *
 * 17   3/3/10 2:38p erickson
 * SW7420-565: add fields to NEXUS_PictureCtrlContrastStretch, remove
 *  duplicate fields from NEXUS_PictureCtrlAdvColorSettings
 *
 * 16   6/22/09 3:59p erickson
 * PR56233: remove NEXUS_PictureCtrlAdvColorSettings.monochrome. believed
 *  to be DTV feature.
 *
 * 15   11/14/08 5:43p jgarrett
 * PR 48957: Updating to match new VDC API style
 *
 * 14   9/24/08 11:25a erickson
 * PR47300: update NEXUS_PictureCtrlColorClipSettings
 *
 * 13   9/22/08 12:10p erickson
 * PR47190: added NEXUS_PictureCtrl_SetColorClipSettings
 *
 * 12   9/22/08 10:34a erickson
 * PR46948: merge
 *
 * PR46948/1   9/22/08 2:55p chengs
 * PR46948: Backlight Dimming in Dynamic Contrast code.
 *
 * 11   9/5/08 1:16p erickson
 * PR45897: add dynamic backlight options for panels
 *
 * 10   9/2/08 10:21a erickson
 * PR45181: update hungarian notation change
 *
 * 9   8/13/08 11:47a erickson
 * PR45479: merge
 *
 * PR45479/1   8/12/08 12:25p chengs
 * PR45479: Updated Dynamic Contrast code.
 *
 * 8   6/26/08 10:20a erickson
 * PR44071: prevent overlap of NEXUS_PictureCtrlCommonSettings.colorTemp
 *  and NEXUS_PictureCtrlAdvColorSettings attenuation settings. added
 *  enable booleans.
 *
 * 7   5/9/08 3:47p erickson
 * PR42552: readd Display luma stats functions
 *
 * 6   4/30/08 3:08p erickson
 * PR39453: add 3548/3556 features
 *
 * 5   4/2/08 2:47p erickson
 * PR40950: move PictureCtrl custom data into function params
 *
 * 4   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 3   2/21/08 5:48p erickson
 * PR39003: PQ api change
 *
 * PR38710/1   2/1/08 5:41p chengs
 * PR38710: Rename GreenStretch and BlueStretch to GreenBoost and
 *  BlueBoost, support new BlueStretch.
 *
 * 2   1/28/08 1:07p erickson
 * PR35457: add change notice
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/14   1/11/08 11:04a erickson
 * PR36159: refactored LumaStatistics api
 *
 * Nexus_Devel/13   1/11/08 10:34a erickson
 * PR36159: update comments
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
#ifndef NEXUS_PICTURE_CTRL_H__
#define NEXUS_PICTURE_CTRL_H__

#include "nexus_types.h"
#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*=************************************************************************
The PictureCtrl interface manipulates the backend picture enhancement units.
in future Nexus releases.
**************************************************************************/


/***************************************************************************
Summary:
    Dither settings

Description:
    Used in NEXUS_PictureCtrl_SetDitherSettings
**************************************************************************/
typedef struct NEXUS_PictureCtrlDitherSettings
{
    bool      reduceSmooth;
    bool      smoothEnable;
    uint32_t  smoothLimit;
} NEXUS_PictureCtrlDitherSettings;

/***************************************************************************
Summary:
    CMS parameters used in NEXUS_PictureCtrlCmsSettings
**************************************************************************/
typedef struct NEXUS_PictureCtrlCmsParameters
{
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t cyan;
    uint32_t magenta;
    uint32_t yellow;
} NEXUS_PictureCtrlCmsParameters;

/***************************************************************************
Summary:
    CMS settings

Description:
    Used in NEXUS_PictureCtrl_SetCmsSettings
**************************************************************************/
typedef struct NEXUS_PictureCtrlCmsSettings
{
    NEXUS_PictureCtrlCmsParameters saturationGain;
    NEXUS_PictureCtrlCmsParameters hueGain;
} NEXUS_PictureCtrlCmsSettings;

/***************************************************************************
Summary:
    Sharpness register Settings (TNT_CMP_0_V0)

Description:
    Please refer to the register database (RDB) for details on these settings.
    Used in NEXUS_PictureCtrl_SetSharpnessValue
**************************************************************************/
typedef struct NEXUS_PictureCtrlSharpnessValue
{
    bool        enable;                 /* TOP_CONTROL.ENABLE */
    uint32_t    lumaControlCore;        /* LUMA_CONTROL.CORE */
    uint32_t    lumaControlGain;        /* LUMA_CONTROL.GAIN */
    uint32_t    lumaControlBlur;        /* LUMA_CONTROL.BLUR */
    bool        lumaCtrlSoften;         /* LUMA_CONTROL.SOFTEN */
    bool        lumaCtrlHOnly;          /* LUMA_CONTROL.H_ONLY */
    uint32_t    lumaPeakingHAvoid;      /* LUMA_PEAKING.H_AVOID */
    uint32_t    lumaPeakingVAvoid;      /* LUMA_PEAKING.V_AVOID */
    uint32_t    lumaPeakingPeakLimit;   /* LUMA_PEAKING.PEAK_LIMIT */
    uint32_t    lumaPeakingPeakValue;   /* LUMA_PEAKING.PEAK_VALUE */
    uint32_t    chromaControlCore;      /* CHROMA_CONTROL.CORE */
    bool        chromaCtrlWideChroma;   /* CHROMA_CONTROL.WIDE_CHROMA */
    uint32_t    chromaControlFalseColor;/* CHROMA_CONTROL.FALSE_COLOR */
    uint32_t    chromaControlGain;      /* CHROMA_CONTROL.GAIN */
    bool        chromaCtrlHOnly;        /* CHROMA_CONTROL.H_ONLY */
    uint32_t    wideLumaControlCore;    /* WIDE_LUMA_CONTROL.CORE */
    uint32_t    wideLumaControlMode;    /* WIDE_LUMA_CONTROL.MODE */
} NEXUS_PictureCtrlSharpnessValue;


#define NEXUS_DC_TABLE_ROWS              (15)
#define NEXUS_DC_TABLE_COLS              (17)

/***************************************************************************
Summary:
    This structure describes the settings used the contrast stretch algorithm.

Description:
    The fields are for the dynamic contrast stretch algorithm. These,
    however, do not allow the user direct control of the LAB table.
    The algorithm just uses these fields as basis for calculating
    the contrast stretch.

    Warning: These can change in future chip revisions.
**************************************************************************/
typedef struct NEXUS_PictureCtrlContrastStretch
{
	bool            enabled;                        /* enable contrast stretch. if false, all other values in this structure are ignored. */
    int             gain;                           /* the amount of stretch towards min and max
                                                       (recommended value of 1 in fixed point format) */
    int             gainShift;                      /* fractional number of gain */

    int             filterNum;                      /* unused: the numerator of the min, mid, max low pass filter */
    int             filterDenom;                    /* unused: the denomerator of the min, mid, max low pass filter */
    int             dynamicContrastBlackGain;       /* unused: gain for black stretch side in Dynamic Contrast (in fixed point format) */
    int             dynamicContrastWhiteGain;       /* unused: gain for white stretch side in Dynamic Contrast (in fixed point format) */
    uint16_t        dynamicContrastBlackLimit;      /* unused: limit for black stretch in Dynamic Contrast */
    uint16_t        dynamicContrastWhiteLimit;      /* unused: limit for white stretch in Dynamic Contrast */
    uint16_t        dynamicContrastEstCurMaxPt;     /* unused: point to estimate current max luma in dynamic contrast */
    uint16_t        dynamicContrastEstCurMinPt;     /* unused: point to estimate current min luma in dynamic contrast */
    bool            dynamicBacklightControl;        /* unused */

    int32_t         ireTable[NEXUS_DC_TABLE_COLS -1];
    uint32_t        dcTable1[NEXUS_DC_TABLE_ROWS * NEXUS_DC_TABLE_COLS];
    uint32_t        dcTable2[NEXUS_DC_TABLE_ROWS * NEXUS_DC_TABLE_COLS];
} NEXUS_PictureCtrlContrastStretch;

/***************************************************************************
Summary:
Array sizes used in NEXUS_LumaStatistics
**************************************************************************/
#define NEXUS_LUMA_HISTOGRAM_COUNT  (16)
#define NEXUS_LUMA_HISTOGRAM_LEVELS (4)

/***************************************************************************
Summary:
    This structure describes the luma statistics.

Description:
NEXUS_LumaStatistics contains luma statistics for the current picture coming out of the window.

See NEXUS_PictureCtrl_GetLumaStatistics
***************************************************************************/
typedef struct NEXUS_LumaStatistics
{
    uint32_t average;   /* the average of all luma in the specified region */
    uint32_t min;       /* the min of all luma in the specified region */
    uint32_t max;       /* the min of all luma in the specified region */
    uint32_t histogram[NEXUS_LUMA_HISTOGRAM_COUNT];  /* histogram data of luma */
    uint32_t levelStats[NEXUS_LUMA_HISTOGRAM_LEVELS]; /* only valid on some chips */
} NEXUS_LumaStatistics;

/***************************************************************************
Summary:
    Common picture control settings

Description:
    All int16_t values in this structure range between -32768 and 32767. The default is 0.
    Used in NEXUS_PictureCtrl_SetCommonSettings
**************************************************************************/
typedef struct NEXUS_PictureCtrlCommonSettings
{
    int16_t contrast;
    int16_t saturation;
    int16_t hue;
    int16_t brightness;

    bool    colorTempEnabled; /* If true, set colorTemp. This will be overridden if NEXUS_PictureCtrlAdvColorSettings.attenuationRbgEnabled is true. */
    int16_t colorTemp;

    bool    sharpnessEnable; /* enable or disable sharpness adjustment */
    int16_t sharpness;
} NEXUS_PictureCtrlCommonSettings;

/***************************************************************************
Summary:
    Attenutation base
**************************************************************************/
#define NEXUS_PICTURE_ATTENUATION_BASE  (2048)

/***************************************************************************
Summary:
    Blue stretch settings
**************************************************************************/
typedef struct
{
    uint16_t blueStretchOffset;
    uint16_t blueStretchSlope;
} NEXUS_BlueStretchSettings;

/***************************************************************************
Summary:
    Enhanced picture control color settings

Description:
    See NEXUS_PictureCtrl_GetAdvColorSettings
**************************************************************************/
typedef struct NEXUS_PictureCtrlAdvColorSettings
{
    bool attenuationRbgEnabled; /* If true, apply the following attenuation RGB convert matrix settings.
                                   This will be overridden if NEXUS_PictureCtrlCommonSettings.colorTempEnabled is true. */
    unsigned attenuationR;      /* attenuation_R in convert matrix */
    unsigned attenuationG;      /* attenuation_G in convert matrix */
    unsigned attenuationB;      /* attenuation_B in convert matrix */
    int16_t  offsetR;           /* offset_R in convert matrix */
    int16_t  offsetG;           /* offset_G in convert matrix */
    int16_t  offsetB;           /* offset_B in convert matrix */

    int16_t  fleshTone;          /* value for Flesh Tone */
    int16_t  greenBoost;         /* value for Green Boost */
    int16_t  blueBoost;          /* value for Blue Boost */
    NEXUS_BlueStretchSettings blueStretchSettings;  /* settings for Blue Stretch */
} NEXUS_PictureCtrlAdvColorSettings;

/***************************************************************************
Summary:
    Color Correction Settings

Description:
    See NEXUS_PictureCtrl_SetColorCorrectionTable
**************************************************************************/
typedef struct NEXUS_PictureControlColorCorrectionSettings
{
    bool enabled;
    unsigned gammaId;  /* identifier for gamma  */
    unsigned colorId;  /* identifier for the color temperature */
} NEXUS_PictureControlColorCorrectionSettings;

/***************************************************************************
Summary:
    Get common color settings
****************************************************************************/
void NEXUS_PictureCtrl_GetCommonSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlCommonSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Set common color settings

Description:
    NEXUS_PictureCtrl_SetCommonSettings is used to do the basic picture
    control of contrast, saturation, hue, brightness, color temperature
    and sharpness.

    It sets the color space convertor (CSC) in the compositor (CMP).
    This function will override anything set by NEXUS_VideoWindow_SetColorMatrix.

See Also:
    NEXUS_VideoWindow_SetColorMatrix - more detailed control of the CSC block
    NEXUS_PictureCtrl_SetSharpnessValue - more detailed control of sharpness
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetCommonSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlCommonSettings *pSettings
    );

/***************************************************************************
Summary:
    Get enhanced color settings
****************************************************************************/
void NEXUS_PictureCtrl_GetAdvColorSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlAdvColorSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Set enhanced color settings

Description:
    NEXUS_PictureCtrl_SetAdvColorSettings is used to advanced picture
    control of dynamic contrast, flesh tone, green boost, blue boost
    and monochrome.

See Also:
    NEXUS_PictureCtrl_SetCmsSettings - overrides any fleshTone, greenBoost and blueBoost setting
    NEXUS_PictureCtrl_LoadCabTable - overrides any fleshTone, greenBoost and blueBoost setting
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetAdvColorSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlAdvColorSettings *pSettings
    );

/***************************************************************************
Summary:
    Get CMS control settings
****************************************************************************/
void NEXUS_PictureCtrl_GetCmsSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlCmsSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Set CMS control settings

Description:
    NEXUS_PictureCtrl_SetCmsSettings sets the saturation gain and hue gain
    for six basic colors(red/green/blue/cyan/magenta/yellow). It provides
    the capability to manage each color in the color bar independently.

    Note that NEXUS_PictureCtrl_SetCmsSettings is exclusive with the
    parameters of fleshTone, greenBoost and blueBoost in
    NEXUS_PictureCtrl_SetAdvColorSettings because both are based on CAB
    block and they are implemented through different algorithm internally.

    If the user turns on CMS by NEXUS_PictureCtrl_SetCmsSettings, it is not
    allowed to call NEXUS_PictureCtrl_SetAdvColorSettings to turn on
    flesh tone, green boost or blue boost before user turn off CMS.

    If the user turn on fleshTone, greenBoost or blueBoost by calling
    NEXUS_PictureCtrl_SetAdvColorSettings, it is not allowed to turn on
    CMS by NEXUS_PictureCtrl_SetCmsSettings before user turn off fleshTone,
    greenBoost and blueBoost.

See Also:
    NEXUS_PictureCtrl_SetAdvColorSettings
    NEXUS_PictureCtrl_LoadCabTable
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetCmsSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlCmsSettings *pSettings
    );

/***************************************************************************
Summary:
    Get current window dither settings
****************************************************************************/
void NEXUS_PictureCtrl_GetDitherSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlDitherSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Set new window dither settings
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetDitherSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlDitherSettings *pSettings
    );

/***************************************************************************
Summary:
    Get contrast stretch parameters
****************************************************************************/
void NEXUS_PictureCtrl_GetContrastStretch(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlContrastStretch *pContrast /* [out] histogram data */
    );

/***************************************************************************
Summary:
    Set contrast stretch parameters

Description:
    This is an advanced usage of dynamic contrast feature. User can call this
    API to customize dynamic contrast config.

See Also:
    NEXUS_PictureCtrl_SetAdvColorSettings
    NEXUS_PictureCtrl_LoadLabTable
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetContrastStretch(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlContrastStretch *pContrast /* attr{null_allowed=y} histogram data */
    );

/***************************************************************************
Summary:
    Set contrast stretch parameters along with custom data for LAB processing
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetCustomContrastStretch(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlContrastStretch *pContrast,
    const int16_t *pTable,    /* attr{nelem=numTableEntries} custom LAB parameters */
    unsigned numTableEntries
    );

/***************************************************************************
Summary:
    Get custom sharpness setting registers.
****************************************************************************/
void NEXUS_PictureCtrl_GetSharpnessValue(
    NEXUS_VideoWindowHandle window,
    NEXUS_PictureCtrlSharpnessValue *pData /* [out] */
    );

/***************************************************************************
Summary:
    Set custom sharpness setting registers.

Description:
    NEXUS_PictureCtrl_SetSharpnessValue provides more detail controls for
    advanced user to adjust sharpness.

    NEXUS_PictureCtrl_SetSharpnessValue will override the sharpness
    settings that were set through NEXUS_PictureCtrl_SetCommonSettings.sharpness.

    Call NEXUS_PictureCtrl_SetSharpnessValue with NEXUS_PictureCtrlSharpnessValue.enable=false to
    disable the custom settings.

See Also:
    NEXUS_PictureCtrl_SetCommonSettings
****************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetSharpnessValue(
    NEXUS_VideoWindowHandle window,
    const NEXUS_PictureCtrlSharpnessValue *pData
    );

/***************************************************************************
Summary:
    Load the CAB table

Description:
    Disable or enable CAB block by providing the CAB table pointer.
    A NULL pointer disables CAB. A partial table can be loaded by
    using the offset and size parameters.

    A few caveats follow:

    1. If partial tables are to be loaded, it is required that the entire
    table be loaded first because the uninitialized portions of the table
    cannot be filled in with initial values that are guaranteed to work
    correctly with the partially loaded table.
    2. When loading partial tables, the VDC cannot guarantee that it
    would be error free. It is the responsiblilty of the end user to
    assure its validity and operationability with the rest of the PEP block.
    3. The CAB table is persistent. If disabled, the table will not be
    erased. The window would have to be closed to reset the table.
    4. Once user table is loaded, user cannot select auto flesh, green
    boost and blue boost algorithm until user disable customized
    table by passing in the NULL pointer table.

See Also:
    This function overrides values set by NEXUS_PictureCtrl_SetAdvColorSettings or
    NEXUS_PictureCtrl_SetCmsSettings. See them for details of which features are overridden.
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_LoadCabTable(
    NEXUS_VideoWindowHandle window,
    const uint32_t *pTable,         /* attr{null_allowed=y;nelem=numTableEntries} table data */
    unsigned numTableEntries,       /* size of the CAB table, size 0 is used to disable custom table. */
    unsigned offset                 /* start offset in the table */
    );

/***************************************************************************
Summary:
    Load the LAB table

Description:
    NEXUS_PictureCtrl_LoadLabTable loads the raw LAB table into LAB block.
    It will override the dynamic contrast settings set through
    NEXUS_PictureCtrl_SetAdvColorSettings or NEXUS_PictureCtrl_SetContrastStretch.

See Also:
    This function overrides values set by NEXUS_PictureCtrl_SetAdvColorSettings or
    NEXUS_PictureCtrl_SetContrastStretch. See them for details of which features are overridden.
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_LoadLabTable(
    NEXUS_VideoWindowHandle window,
    const uint32_t *pTable,         /* attr{null_allowed=y;nelem=numTableEntries} table data */
    unsigned numTableEntries,       /* size of the LAB table, size 0 is used to disable custom table. */
    unsigned offset                 /* start offset in the table */
    );

/***************************************************************************
Summary:
    Set luma average calculation region for the display
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_ConfigDisplayLumaStatistics(
    NEXUS_DisplayHandle display,
    const NEXUS_ClipRect *pRect /* Luma average calculation region. */
    );

/***************************************************************************
Summary:
    Get luma average value of the display
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_GetDisplayLumaStatistics(
    NEXUS_DisplayHandle display,
    NEXUS_LumaStatistics *pLumaStat /* [out] */
    );


/***************************************************************************
Summary:
    Set luma average calculation region for the video window
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_ConfigWindowLumaStatistics(
    NEXUS_VideoWindowHandle window,
    const NEXUS_ClipRect *pRect /* Luma average calculation region. */
    );

/***************************************************************************
Summary:
    Get luma average value of video window's surface
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_GetWindowLumaStatistics(
    NEXUS_VideoWindowHandle window,
    NEXUS_LumaStatistics *pLumaStat /* [out] */
    );

/***************************************************************************
Summary:
    Load color correction table for white balance
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetColorCorrectionTable(
    NEXUS_DisplayHandle display,
    const NEXUS_PictureControlColorCorrectionSettings *pSettings
    );

/***************************************************************************
Summary:
    Load custom color correction table for white balance
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetCustomColorCorrectionTable(
    NEXUS_DisplayHandle display,
    const uint32_t *pTable,           /* attr{null_allowed=y;nelem=numTableEntries} */
    unsigned numTableEntries
    );

/***************************************************************************
Summary:
Color clip modes used in NEXUS_PictureCtrlColorClipSettings
***************************************************************************/
typedef enum NEXUS_ColorClipMode
{
    NEXUS_ColorClipMode_eNone = 0,     /* Disable color clip */
    NEXUS_ColorClipMode_eWhite,        /* Enable color clip for white */
    NEXUS_ColorClipMode_eBlack,        /* Enable color clip for black */
    NEXUS_ColorClipMode_eBoth,         /* Enable color clip for both */
    NEXUS_ColorClipMode_eMax
} NEXUS_ColorClipMode;

/***************************************************************************
Summary:
    This structure describes Color Clip slopes/joints configurations.

Description:
    The job of the color clip is to gracefully remap out of range RGB values in a
    graceful way before hardware does the clipping to 0 or 1023.  What gets clipped
    is purely a function of the matrix which converts display-colorspace YCbCr to
    display-colorspace RGB.  This matrix is completely determined by the display's
    primaries and white point.
    It only depends on the primaries and white point of the display, which is fixed.
    Therefore, the slopes and joints can be computed offline and stored for each
    TV model or individual unit.

    Note: slopes are unsigned and the derivation should follow the provided
    programming guide.
***************************************************************************/
typedef struct NEXUS_PictureCtrlColorClipSettings
{
    uint32_t            crYSlopeA; /* Chroma versus (219-Luma) Slope; in U16.16 fixed point format. */
    uint32_t            crYSlopeB; /* Chroma versus (219-Luma) Slope; in U16.16 fixed point format. */
    uint32_t            cbYSlopeA; /* Chroma versus (219-Luma) Slope; in U16.16 fixed point format. */
    uint32_t            cbYSlopeB; /* Chroma versus (219-Luma) Slope; in U16.16 fixed point format. */
    uint32_t            crJoint;   /* Joint of Cr Slope_A and Slope_B. */
    uint32_t            cbJoint;   /* Joint of Cb Slope_A and Slope_B. */
    bool                extendedWhite;
    bool                extendedBlack;
    NEXUS_ColorClipMode colorClipMode;
} NEXUS_PictureCtrlColorClipSettings;

/***************************************************************************
Summary:
Get color clip settings
**************************************************************************/
void NEXUS_PictureCtrl_GetColorClipSettings(
    NEXUS_DisplayHandle display,
    NEXUS_PictureCtrlColorClipSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
Set color clip settings
**************************************************************************/
NEXUS_Error NEXUS_PictureCtrl_SetColorClipSettings(
    NEXUS_DisplayHandle display,
    const NEXUS_PictureCtrlColorClipSettings *pSettings /* attr{null_allowed=y} */
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_PICTURE_CTRL_H__ */

