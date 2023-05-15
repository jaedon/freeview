/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_tnt.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 11/24/09 9:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/3548/bvdc_tnt.h $
 * 
 * Hydra_Software_Devel/3   11/24/09 9:43p pntruong
 * SW7420-462: Fixed build errors.
 *
 * Hydra_Software_Devel/2   12/2/08 5:25p pntruong
 * PR49863: Added B1 support.
 *
 * Hydra_Software_Devel/1   9/30/08 3:09p jessem
 * PR 46489: Initial version.
 *
 *
 ***************************************************************************/
#ifndef BVDC_TNT_H__
#define BVDC_TNT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Newer hw version of TNT (aka TN2T) */
#if(((BCHP_CHIP == 3548) && (BCHP_VER >= BCHP_VER_B0)) || \
	((BCHP_CHIP == 3556) && (BCHP_VER >= BCHP_VER_B0)))

/***************************************************************************
Description:
	The following describes maximums for regions in TN2T CM3D
***************************************************************************/
#define BVDC_MAX_CHROMA_SHARPNESS_REGIONS               4
#define BVDC_MAX_CHROMA_SHARPNESS_PWL                   3
#define BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS            4

/***************************************************************************
Description:
	The following describes maximums for the TN2T Peaking, LTI, and CTI blocks
***************************************************************************/
#define BVDC_MAX_LUMA_PEAKING_FREQ_BANDS               14
#define BVDC_MAX_LUMA_OUTPUT_CORING_BANDS               3
#define BVDC_MAX_CORING_GAIN_OFFSETS                    4
#define BVDC_MAX_CORING_THRESHOLDS                      2

/***************************************************************************
Summary:
	This enumerates 2 types of formats that can be used by a region.
	BVDC_ChromaSharpnessFormat_ePolar is pie-shaped and uses hue and sat elements to
	describe its form and coordinates. BVDC_ChromaSharpnessFormat_eRectangular
	is rectangular and uses CR and CB elements to describe its form and
	coordinates.

Description:
***************************************************************************/
typedef enum
{
	BVDC_ChromaSharpnessFormat_eRectangular = 0,
	BVDC_ChromaSharpnessFormat_ePolar
} BVDC_ChromaSharpnessFormat;

/***************************************************************************
Summary:
	This enumerates 5 inputs to a PWL

Description:
***************************************************************************/
typedef enum
{
	BVDC_ChromaSharpnessPwlInput_eCr = 0,
	BVDC_ChromaSharpnessPwlInput_eCb,
	BVDC_ChromaSharpnessPwlInput_eLuma,
	BVDC_ChromaSharpnessPwlInput_eHue,
	BVDC_ChromaSharpnessPwlInput_eSat
} BVDC_ChromaSharpnessPwlInput;

/***************************************************************************
Summary:
	This enumerates 2 algorithms used for computation.

Description:

***************************************************************************/
typedef enum
{
	BVDC_ChromaSharpnessGainAlgorithm_eAbsMin = 0,
	BVDC_ChromaSharpnessGainAlgorithm_eProduct
} BVDC_ChromaSharpnessGainAlgorithm;

/***************************************************************************
Summary:
	This structure describes a PWL point.

Description:
	The slope for 2 adjacent PWL points is calculated using the following.

	ulX		- The X coordinate.
	iY 		- The Y coordinate. The user has to provide a value left
			  shifted by 6 to preserve the precision.

Example:

	iY = -1 << 6     - value of -1
	iY = (1 << 6)/3  - value of 1/3

See Also:
***************************************************************************/
typedef struct
{
	uint32_t           ulX;
	int32_t            ilY; /* user has to provide right shifted by 6 value to
							   preserve the precision */
} BVDC_ChromaSharpnessPwlPoint;

/***************************************************************************
Summary:
	This structure describes a PWL (piece-wise linear)

Description:

	ePwlInput		- The input to the PWL
	astPwlPoint		- Array of PWLs

See Also:
	BVDC_ChromaSharpnessPwlInput
	BVDC_ChromaSharpnessPwlPoint
***************************************************************************/
typedef struct
{
	BVDC_ChromaSharpnessPwlInput                ePwlInput;
	BVDC_ChromaSharpnessPwlPoint                astPwlPoint[BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS];
} BVDC_ChromaSharpnessPwl;

/***************************************************************************
Summary:
	This enumerates the Luma Peaking Output Coring modes.

Description:

***************************************************************************/
typedef enum
{
	BVDC_Sharpness_LumaPeakingOutputCoringMode_eDirectional = 0,
	BVDC_Sharpness_LumaPeakingOutputCoringMode_eLowHigh,
	BVDC_Sharpness_LumaPeakingOutputCoringMode_eTotal
} BVDC_Sharpness_LumaPeakingOutputCoringMode;

/***************************************************************************
Summary:
	This enumerates vertical filter types.

Description:

***************************************************************************/
typedef enum
{
	BVDC_Sharpness_Vertical_Filter_e5Tap = 0,
	BVDC_Sharpness_Vertical_Filter_e7Tap,
	BVDC_Sharpness_Vertical_Filter_eBypass
} BVDC_Sharpness_Vertical_Filter;

/***************************************************************************
Summary:
	This enumerates horizontal filter types.

Description:

***************************************************************************/
typedef enum
{
	BVDC_Sharpness_Horizontal_Filter_e5Tap = 0,
	BVDC_Sharpness_Horizontal_Filter_e7Tap,
	BVDC_Sharpness_Horizontal_Filter_e9Tap
} BVDC_Sharpness_Horizontal_Filter;

/***************************************************************************
Summary:
	This enumerates luma statistic windows horizontal size (in pixels).

Description:

***************************************************************************/
typedef enum
{
	BVDC_Sharpness_Luma_Stat_Win_HSize_e11 = 0,
	BVDC_Sharpness_Luma_Stat_Win_HSize_e9,
	BVDC_Sharpness_Luma_Stat_Win_HSize_e7
} BVDC_Sharpness_Luma_Stat_Window_HSize;

/***************************************************************************
Summary:
	This structure describes the peaking gains for a given frequency band.

Description:
	usOvershootGain		- the overshoot gain
	usUndershootGain	- the undershoot gain

See Also:
	BVDC_Window_SetSharpnessConfig
	BVDC_Window_GetSharpnessConfig
***************************************************************************/
typedef struct
{
	uint32_t   ulOvershootGain;    /* U6 format */
	uint32_t   ulUndershootGain;   /* U6 format */
} BVDC_Sharpness_LumaPeakingGain;

/***************************************************************************
Summary:
	This structure describes a chroma sharpness region

Description:
	Regions cannot overlap other regions.

	eColorRegionFormat	- can be polar or rectangular

	ulMaxX				- If eRegionType is Polar, this is max hue else max Cr. In U10 format.
	ulMinX				- If eRegionType is Polar, this is min hue else min Cr. In U10 format.
						- Hue min to Hue max have the following characteristics:
							 - Range is from 0 to 1023.
							 - The LSB is half a degree.
							 - The circular hue range is from 0 to 719.
							 - Max hue of 720 to 1023 is used when the range crosses 719, ie.,
							   more than 360 degrees.
							 - Range 0 to 303 is equivalent to range 720 to 1023.
							 - Hue min < Hue max

	ulMaxY				- If eRegionType is Polar, this is max sat else max Cb. In U10 format.
	ulMinY 				- If eRegionType is Polar, this is min sat else min Cb. In U10 format.
						- Sat min to Sat max have the following characteristics:
							 - Range is from 0 to 725.
							 - Sat min < Sat max

	ulPwlCnt			- number of PWLs (piece-wise linear) for this region
	astPwl				- an array of PWLs for this region
						- PWL_X[i] < PWL_X[i+1]
						- When PWL_X[0] > ulMinX (or ulMinY), the normalized PWL gain between the
						  first PWL point and ulMinX (or ulMinY) is PWL_Y[0]
						- When PWL_X[3] <= ulMaxX (or ulMaxY), the normalized PWL gain between the
						  first PWL point and ulMaxX (or ulMaxY) is PWL_Y[3]

	eAlgo				- use product or absolute minimum for computation

	bEnable				- enables or disables the region

	sLtiGain            - the LTI gain of the region
	sPeakGainOffset     - the Peak gain offset of the region
	sPeakCoreOffset		- the prak coring offset of the region

See Also:
	BVDC_ChromaSharpnessPwl
	BVDC_ChromaSharpnessFormat
	BVDC_Window_SetChromaSharpnessRegion
	BVDC_Window_GetChromaSharpnessRegion
***************************************************************************/
typedef struct
{
	BVDC_ChromaSharpnessFormat          eChromaRegionFormat;
	uint32_t                            ulMaxX;
	uint32_t                            ulMinX;
	uint32_t                            ulMaxY;
	uint32_t                            ulMinY;

	uint32_t                            ulPwlCnt;
	BVDC_ChromaSharpnessPwl             astPwl[BVDC_MAX_CHROMA_SHARPNESS_PWL];

	BVDC_ChromaSharpnessGainAlgorithm   eAlgo;
	bool                                bEnable;

	int32_t                             ilLtiGainOffset;
	int32_t                             ilPeakGainOffset;
	int32_t						        ilPeakCoreOffset;
} BVDC_ChromaSharpnessRegion;

/***************************************************************************
Summary:
	This structure describes the sharpness settings. This includes
	Luma Peaking, LTI, CTI, and Chroma Sharpness(CM3D) blocks.

Description:
	stLumaPeaking.bInputCoringEnable				- enables input coring for Luma Peaking
	stLumaPeaking.astGain                  			- an array of peaking gains for the Luma
									  				  Peaking block's frequency bands

	stLumaPeaking.eOutputCoringMode					- Luma Peaking output coring mode
	stLumaPeaking.aulOutputCoringBands				- the coring bands associated with Luma
									  				  Peaking output coring mode

	stLumaPeaking.stInputCoringLoBand.ailOffset		- array of Luma Peaking Input Low band gain offsets
	stLumaPeaking.stInputCoringLoBand.aulThreshold	- array of Luma Peaking Input Low band thresholds

	stLumaPeaking.stInputCoringHiBand.ailGainOffset	- array of Luma Peaking Input High band gain offset
	stLumaPeaking.stInputCoringHiBand.aulThreshold	- array of Luma Peaking Input High band thresholds


	stLumaPeaking.bEnableClipAvoidance				- enables Luma Peaking clip avoidance
	stLumaPeaking.stClipAvoidUpper.ulSlope			- Clip Avoidance upper slope
	stLumaPeaking.stClipAvoidUpper.ulThreshold		- Clip Avoidance upper threshold
	stLumaPeaking.stClipAvoidLower.ulSlope			- Clip Avoidance lower slope
	stLumaPeaking.stClipAvoidLower.ulThreshold		- Clip Avoidance upper threshold


	stLti.bInputCoringEnable						- LTI input coring enable
	stLti.ilGainOffset								- LTI input coring gain offset
	stLti.ulThreshold								- LTI input coring thresholds

	stLti.ulGain									- LTI gain

	stLti.bBlurFilterEnable							- LTI blur filter enable
	stLti.eVerFilter								- LTI vertical filter
	stLti.eHorFilter								- LTI horizontal filter


	stLti.ulHAvoid									- suppresses LTI horizontal aliasing
	stLti.ulVAvoid									- suppresses LTI vertical aliasing

	stLti.ulOutputCoringLevel						- LTI output coring level

	stLti.ilCurveControl							- LTI curve control (Sharp request)


	stCtiHorFilter.ulGain							- CTI horizontal gain
	stCtiHorFilter.ulCoringLevel					- CTI horizontal coring level
	stCtiHorFilter.eFilter							- CTI horizontal filter

	stCtiHorFilter.ulGain							- CTI vertical gain
	stCtiHorFilter.ulCoringLevel					- CTI vertical coring level
	stCtiHorFilter.eVerFilter						- CTI vertical filter


	bChromaSharpnessEnable							- enables Chroma region correction
	astChromaRegion									- an array of chroma sharpness regions

	eLumaStatWin                    				- the horizontal size of the luma stat window

See Also:
	BVDC_Window_SetSharpnessConfig
	BVDC_Window_GetSharpnessConfig
***************************************************************************/
typedef struct
{
	/* Luma Peaking */
	struct
	{
		BVDC_Sharpness_LumaPeakingGain               astGain[BVDC_MAX_LUMA_PEAKING_FREQ_BANDS];

		BVDC_Sharpness_LumaPeakingOutputCoringMode   eOutputCoringMode;
		uint32_t                                     aulOutputCoringBands[BVDC_MAX_LUMA_OUTPUT_CORING_BANDS]; /* U8 format */

		bool                                         bInputCoringEnable;
		struct
		{
			int32_t                                  ailGainOffset[BVDC_MAX_CORING_GAIN_OFFSETS];   /* S6 format */
			uint32_t                                 aulThreshold[BVDC_MAX_CORING_THRESHOLDS];      /* U10 format */
		} stInputCoringLoBand;

		struct
		{
			int32_t                                  ailGainOffset[BVDC_MAX_CORING_GAIN_OFFSETS];   /* S6 format */
			uint32_t                                 aulThreshold[BVDC_MAX_CORING_THRESHOLDS];      /* U10 format */
		} stInputCoringHiBand;


		bool                                         bEnableClipAvoidance;
		struct
		{
			uint32_t                                 ulSlope;       /* U2 format */
			uint32_t                                 ulThreshold;   /* U10 format */
		} stClipAvoidUpper;

		struct
		{
			uint32_t                                 ulSlope;       /* U2 format */
			uint32_t                                 ulThreshold;   /* U10 format */
		} stClipAvoidLower;
	} stLumaPeaking;

	/* LTI */
	struct
	{
		bool                                         bInputCoringEnable;
		int32_t                                      ailGainOffset[BVDC_MAX_CORING_GAIN_OFFSETS];   /* S6 format */
		uint32_t                                     aulThreshold[BVDC_MAX_CORING_THRESHOLDS];      /* U10 format */

		uint32_t                                     ulGain; /* U6 format */

		bool                                         bBlurFilterEnable;
		BVDC_Sharpness_Vertical_Filter               eVerFilter;
		BVDC_Sharpness_Horizontal_Filter             eHorFilter;

		uint32_t                                     ulHAvoid; /* U6 format */
		uint32_t                                     ulVAvoid; /* U6 format */

		uint32_t                                     ulOutputCoringLevel; /* U8 format */

		int32_t                                      ilCurveControl;      /* S4 format */
	} stLti;

	/* CTI */
	struct
	{
		uint32_t                                     ulGain;        /* U6 format */
		uint32_t                                     ulCoringLevel; /* U8 format */
		BVDC_Sharpness_Horizontal_Filter             eFilter;
	} stCtiHorFilter;

	struct
	{
		uint32_t                                     ulGain;        /* U6 format */
		uint32_t                                     ulCoringLevel; /* U8 format */
		BVDC_Sharpness_Vertical_Filter               eFilter;
	} stCtiVerFilter;

	/* Chroma-based filtering */
	bool                                             bChromaSharpnessEnable;
	BVDC_ChromaSharpnessRegion                       astChromaRegion[BVDC_MAX_CHROMA_SHARPNESS_REGIONS];

	/* Luma Histogram */
	BVDC_Sharpness_Luma_Stat_Window_HSize            eLumaStatWin;
} BVDC_SharpnessSettings;

#else

/***************************************************************************
Summary:
	This structure describes the sharpness settings.

Description:
	BVDC_SharpnessSettings is a structure containing
	custom luma and chroma settings used with TNT.

	Warning: This may change in future chip revisions.

	ulLumaCtrlCore         - Larger values prevent sharpening of low-level
	textures and noise. Values greater than 32 (or less than -32) are not
	commonly used. This is a signed number S7.0. Legal range: -128 ~ 127.

	ulLumaCtrlGain         - Larger values increase luma sharpness (or
	increase softness if "LUMA_CTRL_SOFTEN" is set). Zero disables luma
	sharpness (or softness). Values greater than 32 are usually too harsh
	for normal viewing. This is an unsigned number U6.0. Legal range: 0 ~ 63.

	ulLumaCtrlBlur         - Large values reduce sharpness for very high
	frequence texture and prevent sharpening of noise. Legal range: 0 ~ 2.

	bLumaCtrlSoften        - If enable, TNT will soften luma data and we
	recommend to set "LUMA_CTRL_CORE" to be 0. If disable, TNT will sharpen
	luma data.

	bLumaCtrlHOnly         - When enabled, TNT will only sharpen luma data
	horizontally, when disabled, TNT will sharpen luma data in both
	(horizontal and vertical) direction.

	ulLumaPeakingHAvoid    - Luma vertical sharp edge avoidance control.
	Larger values avoid luma sharpening of strong vertical edges. This is an
	unsigned number U6.0. Legal range: 0 ~ 63.

	ulLumaPeakingVAvoid    - Luma horizontal sharp edge avoidance. Larger
	values avoid luma sharpening of strong horizontal edges. This is an
	unsigned number U6.0. Legal range: 0 ~ 63.

	ulLumaPeakingPeakLimit - This value sets a hard limit on luma overshoot.
	This is an unsigned number U7.0. Legal range: 0 ~ 127.

	ulLumaPeakingPeakValue - Larger values reduce luma overshoot. This is an
	unsigned number U5.0. Legal range: 0 ~ 31.

	ulChromaCtrlCore       - Larger values prevent sharpening of low-level
	textures and noise. This is an unsigned number U8.0. Legal range: 0 ~ 255.

	bChromaCtrlWideChroma  - When enabled, TNT will use wider range for
	chroma peaking. When disabled, TNT will use normal range for chroma
	peaking.

	ulChromaCtrlFalseColor - This is to avoid false color when sharpening
	sharp edges of chroma data. Larger values will reduce false colors, at
	the cost of some chroma sharpness. This is an unsigned number U3.0.
	Legal range: 0 ~ 7.

	ulChromaCtrlGain       - Larger values increase chroma sharpness. Zero
	disables chroma. sharpness. Values greater than 32 are usually too harsh
	for normal viewing. This is an unsigned number U6.0. Legal range: 0 ~ 63.

	bChromaCtrlHOnly       - When enabled, TNT will only sharpen chroma data
	horizontally, when disabled, TNT will sharpen chroma data in both
	(horizontal and vertical) direction.

	ulWideLumaCtrlCore     - Larger values prevent sharpening of low-level
	textures and noise. Values greater than 32 (or less than -32) are not
	commonly used. This is a signed number S7.0. Legal range: -128 ~ 127.

	ulWideLumaCtrlMode     - Wide luma mode control.
	2'b00: luma peaking is kept narrow for tight, crisp peaking.
	2'b01: luma peaking use a wider range for more dramatic, intense peaking.
	2'b10: luma peaking use the widest range for dramatic, intense peaking.

See Also:
	BVDC_Window_SetSharpnessConfig, BVDC_Window_GetSharpnessConfig
***************************************************************************/
typedef struct
{
	uint32_t                                         ulLumaCtrlCore;
	uint32_t                                         ulLumaCtrlGain;
	uint32_t                                         ulLumaCtrlBlur;
	bool                                             bLumaCtrlSoften;
	bool                                             bLumaCtrlHOnly;
	uint32_t                                         ulLumaPeakingHAvoid;
	uint32_t                                         ulLumaPeakingVAvoid;
	uint32_t                                         ulLumaPeakingPeakLimit;
	uint32_t                                         ulLumaPeakingPeakValue;
	uint32_t                                         ulChromaCtrlCore;
	bool                                             bChromaCtrlWideChroma;
	uint32_t                                         ulChromaCtrlFalseColor;
	uint32_t                                         ulChromaCtrlGain;
	bool                                             bChromaCtrlHOnly;
	uint32_t                                         ulWideLumaCtrlCore;
	uint32_t                                         ulWideLumaCtrlMode;
	uint32_t                                         ulSimpleLumaCtrlCore;
	bool                                             bSimpleLumaCtrlMode;
} BVDC_SharpnessSettings;
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_TNT_H__ */

/* End of File */

