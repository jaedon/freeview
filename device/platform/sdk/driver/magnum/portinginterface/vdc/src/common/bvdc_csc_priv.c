/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_csc_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/63 $
 * $brcm_Date: 3/8/12 11:50a $
 *
 * Module Description:
 *   VDEC/HD_DVI (or similiar hw) CSC matrix coefficients.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_csc_priv.c $
 * 
 * Hydra_Software_Devel/63   3/8/12 11:50a yuxiaz
 * SW7425-2546: Fixed warning msg.
 * 
 * Hydra_Software_Devel/62   8/5/11 8:24p albertl
 * SW7425-745: Added blanking to compositor color for all outputs and all
 * colorspaces.
 * 
 * Hydra_Software_Devel/61   6/24/11 2:58p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 * 
 * Hydra_Software_Devel/60   8/27/10 3:00p albertl
 * SW7405-4556: Fix for coverity warning.
 * 
 * Hydra_Software_Devel/59   8/26/10 7:39p albertl
 * SW7405-4556, SW7405-4515: Changed RGB attenuation to use inverse of
 * user matrix C.
 * 
 * Hydra_Software_Devel/58   3/12/10 2:42a albertl
 * SW3548-2700: Fixed build error.
 * 
 * Hydra_Software_Devel/57   3/11/10 5:34p albertl
 * SW3548-2700: Added float debug messages.  Fixed incorrect value for Mc
 * when doing RGB attenuation with user matrices.
 * 
 * Hydra_Software_Devel/56   1/12/10 8:41p albertl
 * SW3548-2700: Changed RGB attenuation to use DVO matrix as YCbCr->RGB
 * matrix and Matrix C as RGB->YCbCr matrix.
 * 
 * Hydra_Software_Devel/55   5/15/09 2:27p pntruong
 * PR51338: Updated for the rgb input signal.
 *
 * Hydra_Software_Devel/54   5/12/09 10:45p pntruong
 * PR51338: Added support for adc fullrange raw 11-bit.
 *
 * Hydra_Software_Devel/53   4/30/09 3:03p albertl
 * PR54593: Fixed Set and GetDvoMatrix to take and return matrices in
 * YCbCr->RGB and YCbCr->YCbCr format and convert to hardware format
 * internally.
 *
 * Hydra_Software_Devel/52   4/1/09 7:41p albertl
 * PR51825: Fixed build errors.
 *
 * Hydra_Software_Devel/51   4/1/09 5:19p albertl
 * PR51825: Changed negative maximum range to -32768.  Changed linearly
 * interpolate values to go from -32767 to 32767, with -32768 capped to -
 * 32767.
 *
 * Hydra_Software_Devel/50   3/25/09 5:54p pntruong
 * PR52524: Used the index determined by upstream block.
 *
 * Hydra_Software_Devel/49   3/20/09 6:02p pntruong
 * PR52524: Fixed typo.
 *
 * Hydra_Software_Devel/48   3/9/09 1:28p pntruong
 * PR52524: Added support for hdmi rgb quantization of full range and
 * limited range.
 *
 * Hydra_Software_Devel/47   2/26/09 3:51p albertl
 * PR52515: Changed pRgbToYCbCr and pYCbCrToRgb to non-static.
 *
 * Hydra_Software_Devel/46   10/24/08 2:44a albertl
 * PR48023:  Added BVDC_Display_SetDvoAttenuationRGB and
 * BVDC_Display_GetDvoAttenuationRGB, enabling CSC adjustment on DVO
 * path.  Further cleaned up CSC matrix multiplication code.
 *
 * Hydra_Software_Devel/45   10/23/08 5:45p albertl
 * PR47318, PR47814: Fixed overflow when calculating RGB attenuation
 * offsets.  Rewrote portions of RGB attenuation code and fixed various
 * offset related issues.
 *
 * Hydra_Software_Devel/44   10/9/08 5:27p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/43   10/2/08 7:26p albertl
 * PR47318: Changed color temp adjusts to apply to offsets.
 *
 * Hydra_Software_Devel/42   10/2/08 5:51p albertl
 * PR45808: Updated CSC tables and code to use unified 8-bit to 8-bit
 * coefficients and offsets.  Added support for custom panels.
 *
 * Hydra_Software_Devel/41   8/14/08 1:39p pntruong
 * PR45021: Corrected the csc for vdec secam input.
 *
 * Hydra_Software_Devel/40   8/11/08 8:57p pntruong
 * PR45333: Set correct default pedestal settings according programming
 * spreadsheet.  Default sync on master channel instead of all-channel
 * for non-pc input.
 *
 * Hydra_Software_Devel/39   8/4/08 1:39p pntruong
 * PR45360: Updated csc for pc input.
 *
 * Hydra_Software_Devel/38   7/30/08 12:27p pntruong
 * PR45021:  Correct color space for secam and PAL format.
 *
 * Hydra_Software_Devel/PR45021/1   7/29/08 9:52a bcao
 * PR45021: correct color space for secam and PAL format
 *
 * Hydra_Software_Devel/37   6/17/08 3:52p albertl
 * PR35135:  Fixed incorrect black offsets causing incorrect contrast
 * adjustments.
 *
 * Hydra_Software_Devel/36   6/5/08 8:43p pntruong
 * PR41315: Initial secam bringup.
 *
 * Hydra_Software_Devel/35   5/2/08 9:14p albertl
 * PR42395:  Fixed fixed point MTH overflow in
 * BVDC_P_Csc_ColorTempToAttenuationRGB.
 *
 * Hydra_Software_Devel/34   4/16/08 5:33p albertl
 * PR37121:  Fixed CbAlpha nd CrAlpha overflow on 7325 GFD.
 *
 * Hydra_Software_Devel/33   1/13/08 3:27a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/32   12/6/07 7:27p albertl
 * PR37722:  Moved matrix initialization out of isr.  Fixed float to fix
 * conversion macros that allowed fload instructions to creep in.
 *
 * Hydra_Software_Devel/31   10/23/07 3:28p pntruong
 * PR36138: Video change suddenly(too large) if adjust Colortemp value
 * from 0 to +/-1(it's OK for other value step).
 *
 * Hydra_Software_Devel/PR36138/1   10/19/07 1:19p chengs
 * PR36138: Video change suddenly(too large) if adjust Colortemp value
 * from 0 to +/-1(it's OK for other value step)
 *
 * Hydra_Software_Devel/30   10/15/07 3:10p pntruong
 * PR35135: Removed debug printf.
 *
 * Hydra_Software_Devel/29   10/15/07 2:57p albertl
 * PR35135:  Fixed overflow when multiplying offsets that occurs with
 * 3563.
 *
 * Hydra_Software_Devel/28   10/11/07 5:20p albertl
 * PR35135:  Fixed bug assigning bUseAlpha.
 *
 * Hydra_Software_Devel/27   10/3/07 11:54a pntruong
 * PR35411: Extend support for user specify gfd csc.
 *
 * Hydra_Software_Devel/26   10/2/07 11:21a pntruong
 * PR35136: Some cleanup of unused code.
 *
 * Hydra_Software_Devel/25   10/1/07 5:51p pntruong
 * PR35460: Fixed precision for hddvi and vdec.
 *
 * Hydra_Software_Devel/24   9/28/07 1:31p pntruong
 * PR34674: Finetune PAL video qulity.
 *
 * Hydra_Software_Devel/23   9/27/07 7:13p albertl
 * PR35135:  Fixed missing shift for Y Alphas.  Removed obsolete macros.
 *
 * Hydra_Software_Devel/22   9/26/07 8:00p albertl
 * PR35135:  Updated csc calculations to properly handle alpha values.
 *
 * Hydra_Software_Devel/21   9/20/07 1:13a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/20   9/17/07 7:11p pntruong
 * PR30702: Updated dynamics contrast to reflect saturation range
 * expansion, and default Cb/Cr off for contrast adjust ment.
 *
 * Hydra_Software_Devel/19   7/23/07 6:36p pntruong
 * PR31134: Fixed the maxtrix swap for s_SdYPrPb_To_SdYCrCb.
 *
 * Hydra_Software_Devel/18   7/23/07 6:26p albertl
 * PR30292:  Updated CSC for 10-bit video for 3563.
 *
 * Hydra_Software_Devel/16   7/17/07 7:01p albertl
 * PR31134:  Updated CSC tables.
 *
 * Hydra_Software_Devel/15   7/2/07 11:42a pntruong
 * PR32673: [VDEC] Add NTSC_J support.
 *
 * Hydra_Software_Devel/14   6/28/07 5:00p tdo
 * PR32549: Handle the corner case for identity color temperature
 * attenuation and offset
 *
 * Hydra_Software_Devel/13   6/18/07 6:30p albertl
 * PR30702:  Expanded hue and saturation ranges, and reworked saturation
 * algorithm to support one slope from 0 to 1, and another for 1 to
 * KaMax.
 *
 * Hydra_Software_Devel/12   6/12/07 11:19a tdo
 * PR30613: Move CSC calculations into bvdc_csc_priv.c
 *
 * Hydra_Software_Devel/11   6/1/07 1:51p yuxiaz
 * PR31713: Turn off warning messages for 7400B0.
 *
 * Hydra_Software_Devel/10   5/22/07 7:18p yuxiaz
 * PR29980: Updated HD_DVI CSC settings.
 *
 * Hydra_Software_Devel/9   4/9/07 11:24a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/8   3/28/07 6:17p tdo
 * PR28392, PR28282: Implement get color matrix function
 *
 * Hydra_Software_Devel/7   3/28/07 5:28p tdo
 * PR28392: Implement get color matrix function
 *
 * Hydra_Software_Devel/6   3/23/07 11:19a tdo
 * PR 28392: Provide API to customize source CSC
 *
 * Hydra_Software_Devel/5   2/8/07 3:32p maivu
 * PR 27558: Fixed warnings for release build
 *
 * Hydra_Software_Devel/4   1/22/07 7:51p pntruong
 * PR22579: Bringup of HD_DVI input (dual core).  Fixed colorspace.
 *
 * Hydra_Software_Devel/3   1/11/07 9:29p pntruong
 * PR23222:  Bringup up svideo, yprpb, and all input with ntsc.
 *
 * Hydra_Software_Devel/2   1/11/07 12:32p tdo
 * PR 23222:  Update CSC matrices
 *
 * Hydra_Software_Devel/1   12/18/06 11:40p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   10/18/06 12:15p pntruong
 * PR23222: VDEC - ATSC input support.  Separate out colorspace conversion
 * code.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   10/18/06 12:05p pntruong
 * PR23222: VDEC - ATSC input support.  Separate out colorspace conversion
 * code.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bvdc_csc_priv.h"
#include "bvdc_displayfmt_priv.h"

BDBG_MODULE(BVDC_CSC);

/* PR30702: Need to enlarge the CSC range for contrast/brightness/saturation/hue". */
#ifndef BVDC_SUPPORT_CONTRAST_WITH_CBCR
#define BVDC_SUPPORT_CONTRAST_WITH_CBCR          (0)
#endif

/* Populate with all necessary input <-> output colorspace conversions.
 * And any special matrices.  Should also include basic common CSC matrices
 * operations. */
static const BVDC_P_CscCoeffs s_Vdec_Identity = BVDC_P_MAKE_VDEC_CSC
	(  1.0000,  0.0000,  0.0000,   16.0000,
	   0.0000,  0.0000,  1.0000,  128.0000,
	   0.0000,  1.0000,  0.0000,  128.0000 );

static const BVDC_P_CscCoeffs s_Yuv_To_SdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  1.6909,  0.0000,  0.0000,   16.0000,
	   0.0000,  1.9839,  0.0000,  128.0000,
	   0.0000,  0.0000,  1.4058,  128.0000 );

static const BVDC_P_CscCoeffs s_Yuv_To_HdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  1.5659, -0.3823, -0.1504,  -15.0938,
	   0.0000,  1.8828,  0.0962,  137.5156,
	  -0.0015,  0.2114,  1.3267,  140.8281 );

static const BVDC_P_CscCoeffs s_SdRgb_To_SdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  0.4688,  0.9219,  0.1797,   16.0000,
	  -0.2695, -0.5313,  0.8008,  128.0000,
	   0.8008, -0.6719, -0.1289,  128.0000 );

static const BVDC_P_CscCoeffs s_SdRgb_To_SdYCrCb_FullRange = BVDC_P_MAKE_VDEC_CSC
	(  0.4688,  0.9219,  0.1797,   16.0000,
	  -0.2695, -0.5313,  0.8008,  128.0000,
	   0.8008, -0.6719, -0.1289,  128.0000 );

static const BVDC_P_CscCoeffs s_SdRgb_To_HdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  0.4331,  1.1094,  0.0269,  -15.0938,
	  -0.2178, -0.5923,  0.8125,  137.5156,
	   0.7856, -0.7471, -0.0400,  140.8281 );

static const BVDC_P_CscCoeffs s_HdRgb_To_SdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  0.3774,  0.9277,  0.2593,   16.0000,
	  -0.2383, -0.5537,  0.7915,  128.0000,
	   0.8120, -0.6499, -0.1606,  128.0000 );

static const BVDC_P_CscCoeffs s_HdRgb_To_HdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  0.3410,  1.1430,  0.1150,   16.0000,
	  -0.1880, -0.6290,  0.8170,  128.0000,
	   0.8170, -0.7430, -0.0740,  128.0000 );

static const BVDC_P_CscCoeffs s_SdYPrPb_To_SdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  1.5956,  0.0000,  0.0000,   16.0000,
	   0.0000,  0.0000,  1.6350,  128.0000,
	   0.0000,  1.6350,  0.0000,  128.0000 );

static const BVDC_P_CscCoeffs s_SdYPrPb_To_HdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  1.5693, -0.3262,  0.1812,   32.2500,
	   0.0000,  1.6450,  0.1201,  121.1406,
	   0.0015,  0.1851,  1.6343,  123.2188 );

static const BVDC_P_CscCoeffs s_HdYPrPb_To_SdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  1.5664,  0.3086,  0.1602,  -43.6719,
	   0.0000,  1.5742, -0.1133,  -58.6719,
	   0.0000, -0.1758,  1.5820,  -51.8750 );

static const BVDC_P_CscCoeffs s_HdYPrPb_To_HdYCrCb = BVDC_P_MAKE_VDEC_CSC
	(  1.5664,  0.0000,  0.0000,   16.0000,
	   0.0000,  1.5977,  0.0000,  -76.6719,
	   0.0000,  0.0000,  1.5977,  -76.6719 );

/* Special CSC for NTSC_J */
static const BVDC_P_CscCoeffs s_Yuv_To_SdYCrCb_NTSC_J = BVDC_P_MAKE_VDEC_CSC
	(  1.5649,  0.0000,  0.0000,   16.0000,
	   0.0000,  1.8350,  0.0000,  128.0000,
	   0.0000,  0.0000,  1.3004,  128.0000 );

/* Special CSC for PAL D/Nc*/
static const BVDC_P_CscCoeffs s_Yuv_To_SdYCrCb_PAL_D_NC = BVDC_P_MAKE_VDEC_CSC
	(  1.5990,  0.0000,  0.0000,   16.0000,
	   0.0000,  1.8740,  0.0000,  128.0000,
	   0.0000,  0.0000,  1.3290,  128.0000 );

/* Special CSC for SECAM*/
static const BVDC_P_CscCoeffs s_Yuv_To_SdYCrCb_SECAM = BVDC_P_MAKE_VDEC_CSC
	(  1.5990,  0.0000,  0.0000,   16.0000,
	   0.0000, -1.3600,  0.0000,  128.0000,
	   0.0000,  0.0000,  1.3600,  128.0000 );

/* HD_DVI, MFD, and Csc */
static const BVDC_P_CscCoeffs s_HdDvi_Identity = BVDC_P_MAKE_HDDVI_CSC
	( 1.0000,  0.0000,  0.0000,   0.0000,
	  0.0000,  1.0000,  0.0000,   0.0000,
	  0.0000,  0.0000,  1.0000,   0.0000 );

/* The following matrix converts
   from [Limited Range RGB BT.601 / SMPTE 170M / xvYCC 601]
   to   [YCbCr BT.601 / SMPTE 170M / xvYCC 601 ] */
static const BVDC_P_CscCoeffs s_HdDvi_SdRgb_To_SdYCrCb = BVDC_P_MAKE_HDDVI_CSC
	( 0.298939,  0.586625,  0.114436,   0.000000,
	 -0.172638, -0.338777,  0.511416, 128.000000,
	  0.511416, -0.427936, -0.083479, 128.000000 );

/* The following matrix converts
   from [Limited Range RGB BT.709 / xvYCC 709]
   to   [YCbCr BT.709 / xvYCC 709 ] */
static const BVDC_P_CscCoeffs s_HdDvi_HdRgb_To_HdYCrCb = BVDC_P_MAKE_HDDVI_CSC
	( 0.212639,  0.715169,  0.072192,   0.000000,
	 -0.117208, -0.394207,  0.511416, 128.000000,
	  0.511416, -0.464524, -0.046891, 128.000000 );

/* The following matrix converts
   from [Full Range RGB BT.601 / SMPTE 170M / xvYCC 601]
   to   [YCbCr BT.601 / SMPTE 170M / xvYCC 601 ] */
static const BVDC_P_CscCoeffs s_HdDvi_SdRgb_To_SdYCrCb_FullRange = BVDC_P_MAKE_HDDVI_CSC
	( 0.255733,  0.501839,  0.097896,  16.000000,
	 -0.147686, -0.289814,  0.437500, 128.000000,
	  0.437500, -0.366086, -0.071414, 128.000000 );

/* The following matrix converts
   from [Full Range RGB BT.709 / xvYCC 709]
   to   [YCbCr BT.709 / xvYCC 709 ] */
static const BVDC_P_CscCoeffs s_HdDvi_HdRgb_To_HdYCrCb_FullRange = BVDC_P_MAKE_HDDVI_CSC
	( 0.181906,  0.611804,  0.061758,  16.000000,
	 -0.100268, -0.337232,  0.437500, 128.000000,
	  0.437500, -0.397386, -0.040114, 128.000000 );

/* Color Temperature - Piecewise Linear Model Parameters - (Assuming Temp is in 100s of K) */
/* Low Temp Model - Under 6500 K
    Atten_R = (0 * Temp + 100)/100
    Atten_G = (0.62695 * Temp + 59.223)/100
    Atten_B = (1.33301 * Temp + 13.333)/100      */
#define BVDC_P_MAKE_CLRTEMP_LMODEL_PARAML(f_bits)              \
	BVDC_P_MAKE_CLRTEMP_LMODEL                                 \
		(  0.00000, 100.000,                                   \
		   0.62695,  59.223,                                   \
		   1.33301,  13.333,                                   \
		   BVDC_P_FIX_MAX_BITS - f_bits, f_bits)

/* High Temp Model - Over 6500 K
	Atten_R = (-0.57422 * Temp + 137.328)/100
	Atten_G = (-0.44727 * Temp + 129.134)/100
	Atten_B = (0 * Temp + 100)/100      */
#define BVDC_P_MAKE_CLRTEMP_LMODEL_PARAMH(f_bits)             \
	BVDC_P_MAKE_CLRTEMP_LMODEL                                \
		( -0.57422, 137.328,                                  \
		  -0.44727, 129.134,                                  \
		   0.00000, 100.000,                                  \
		   BVDC_P_FIX_MAX_BITS - f_bits, f_bits)

static const int32_t s_ClrTemp_LModel_ParamL_Cmp[3][2] =
	BVDC_P_MAKE_CLRTEMP_LMODEL_PARAML(BVDC_P_CSC_CMP_CX_F_BITS);

static const int32_t s_ClrTemp_LModel_ParamH_Cmp[3][2] =
	BVDC_P_MAKE_CLRTEMP_LMODEL_PARAMH(BVDC_P_CSC_CMP_CX_F_BITS);

static const int32_t s_ClrTemp_LModel_ParamL_Gfd[3][2] =
	BVDC_P_MAKE_CLRTEMP_LMODEL_PARAML(BVDC_P_CSC_GFD_CX_F_BITS);

static const int32_t s_ClrTemp_LModel_ParamH_Gfd[3][2] =
	BVDC_P_MAKE_CLRTEMP_LMODEL_PARAMH(BVDC_P_CSC_GFD_CX_F_BITS);


/* macros to select additive offset from alpha component or offset.*/
#define BVDC_P_CSC_USE_ALPHA(matrix)\
	((matrix->usYAlpha || matrix->usCbAlpha || matrix->usCrAlpha) ? true : false)

#define BVDC_P_CSC_GET_YO(matrix) \
	(BVDC_P_CSC_USE_ALPHA(matrix) ? matrix->usYAlpha : \
		                            matrix->usYOffset)

#define BVDC_P_CSC_GET_CBO(matrix) \
	(BVDC_P_CSC_USE_ALPHA(matrix) ? matrix->usCbAlpha : \
	                                matrix->usCbOffset)

#define BVDC_P_CSC_GET_CRO(matrix) \
	(BVDC_P_CSC_USE_ALPHA(matrix) ? matrix->usCrAlpha : \
	                                matrix->usCrOffset)

/* alphas are stored as fractionals and are shifted to convert them to
   integers for calcuations */

#define BVDC_P_CSC_YOTOFIX(matrix) \
	(BVDC_P_CSC_USE_ALPHA(matrix) ? BVDC_P_CSC_CXTOFIX(BVDC_P_CSC_GET_YO(matrix)) << BVDC_P_CSC_VIDEO_DATA_BITS : \
	                                BVDC_P_CSC_COTOFIX(BVDC_P_CSC_GET_YO(matrix)))

#define BVDC_P_CSC_CBOTOFIX(matrix) \
	(BVDC_P_CSC_USE_ALPHA(matrix) ? BVDC_P_CSC_CXTOFIX(BVDC_P_CSC_GET_CBO(matrix)) << BVDC_P_CSC_VIDEO_DATA_BITS : \
	                                BVDC_P_CSC_COTOFIX(BVDC_P_CSC_GET_CBO(matrix)))

#define BVDC_P_CSC_CROTOFIX(matrix) \
	(BVDC_P_CSC_USE_ALPHA(matrix) ? BVDC_P_CSC_CXTOFIX(BVDC_P_CSC_GET_CRO(matrix)) << BVDC_P_CSC_VIDEO_DATA_BITS : \
	                                BVDC_P_CSC_COTOFIX(BVDC_P_CSC_GET_CRO(matrix)))


#define BVDC_P_CSC_FIXTOYO(offset, matrix) \
	if (BVDC_P_CSC_USE_ALPHA(matrix)) \
		 matrix->usYAlpha  = BVDC_P_CSC_FIXTOCX(offset >> BVDC_P_CSC_VIDEO_DATA_BITS); \
	else matrix->usYOffset = BVDC_P_CSC_FIXTOCO(offset);

#define BVDC_P_CSC_FIXTOCBO(offset, matrix) \
	if (BVDC_P_CSC_USE_ALPHA(matrix)) \
	     matrix->usCbAlpha  = BVDC_P_CSC_FIXTOCX(offset >> BVDC_P_CSC_VIDEO_DATA_BITS); \
	else matrix->usCbOffset = BVDC_P_CSC_FIXTOCO(offset);

#define BVDC_P_CSC_FIXTOCRO(offset, matrix) \
	if (BVDC_P_CSC_USE_ALPHA(matrix)) \
	     matrix->usCrAlpha  = BVDC_P_CSC_FIXTOCX(offset >> BVDC_P_CSC_VIDEO_DATA_BITS); \
	else matrix->usCrOffset = BVDC_P_CSC_FIXTOCO(offset);


/***************************************************************************
 * Print out what the CSC look like.
 */
void BVDC_P_Csc_Print_isr
	( const BVDC_P_CscCoeffs          *pCscCoeffs )
{
#if BDBG_DEBUG_BUILD
	/* Let's what they look like */
	BDBG_MSG(("[cx_i=%d, cx_f=%d, co_i=%d, co_f=%d",
		pCscCoeffs->usCxIntBits, pCscCoeffs->usCxFractBits, pCscCoeffs->usCoIntBits, pCscCoeffs->usCoFractBits));
	BDBG_MSG(("[[0x%04x 0x%04x 0x%04x 0x%04x 0x%04x]",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset));
	BDBG_MSG((" [0x%04x 0x%04x 0x%04x 0x%04x 0x%04x]",
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset));
	BDBG_MSG((" [0x%04x 0x%04x 0x%04x 0x%04x 0x%04x]]",
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

/* uses floats, build only for debugging purposes */
#if 0
	BDBG_MSG(("[cx_i=%d, cx_f=%d, co_i=%d, co_f=%d",
		pCscCoeffs->usCxIntBits, pCscCoeffs->usCxFractBits, pCscCoeffs->usCoIntBits, pCscCoeffs->usCoFractBits));
	BDBG_MSG(("[[%0f %04f %04f %04f %04f]",
		BVDC_P_CSC_CXTOF(pCscCoeffs->usY0), BVDC_P_CSC_CXTOF(pCscCoeffs->usY1), BVDC_P_CSC_CXTOF(pCscCoeffs->usY2), BVDC_P_CSC_CXTOF(pCscCoeffs->usYAlpha), BVDC_P_CSC_COTOF(pCscCoeffs->usYOffset)));
	BDBG_MSG((" [%04f %04f %04f %04f %04f]",
		BVDC_P_CSC_CXTOF(pCscCoeffs->usCb0), BVDC_P_CSC_CXTOF(pCscCoeffs->usCb1), BVDC_P_CSC_CXTOF(pCscCoeffs->usCb2), BVDC_P_CSC_CXTOF(pCscCoeffs->usCbAlpha), BVDC_P_CSC_COTOF(pCscCoeffs->usCbOffset)));
	BDBG_MSG((" [%04f %04f %04f %04f %04f]]",
		BVDC_P_CSC_CXTOF(pCscCoeffs->usCr0), BVDC_P_CSC_CXTOF(pCscCoeffs->usCr1), BVDC_P_CSC_CXTOF(pCscCoeffs->usCr2), BVDC_P_CSC_CXTOF(pCscCoeffs->usCrAlpha), BVDC_P_CSC_COTOF(pCscCoeffs->usCrOffset)));
#endif

#else
	BSTD_UNUSED(pCscCoeffs);
#endif
	return;
}


/***************************************************************************
 * Multiplies two csc matrices set up in 4x4 format.  pCscCoeffs contains
 * bit shift settings and alpha usage information.
 */
void BVDC_P_Csc_Mult4X4_isr
	( int32_t                          aalMatrixRet[4][4],
	  int32_t                          aalMatrix1[4][4],
	  int32_t                          aalMatrix2[4][4],
	  BVDC_P_CscCoeffs                *pCscCoeffs)
{
	int i, j, k;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			aalMatrixRet[i][j] = 0;
			for (k = 0; k < 4; k++)
			{
				if (j == 3)
				{
					aalMatrixRet[i][j] += BVDC_P_CSC_FIX_MUL_OFFSET(aalMatrix1[i][k], aalMatrix2[k][j], pCscCoeffs);
				}
				else
				{
					aalMatrixRet[i][j] += BVDC_P_CSC_FIX_MUL(aalMatrix1[i][k], aalMatrix2[k][j]);
				}
			}
		}
	}
}


/***************************************************************************
 * Return the desired color space coverstion for CSC in vdec.
 */
void BVDC_P_Csc_GetVdecTable_isr
	( BVDC_P_CscCoeffs                *pCsc,
	  BVDC_P_VdecColorSpace            eInColorSpace,
	  BVDC_P_CmpColorSpace             eOuColorSpace,
	  const BFMT_VideoInfo            *pFmtInfo,
	  bool                             bFullRange )
{
	if(bFullRange)
	{
		if((BVDC_P_VdecColorSpace_eSdRgb == eInColorSpace) ||
		   (BVDC_P_VdecColorSpace_eHdRgb == eInColorSpace))
		{
			*pCsc = s_SdRgb_To_SdYCrCb_FullRange;
			BDBG_MSG(("Selecting: %s", "s_SdRgb_To_SdYCrCb_FullRange"));
		}
		else
		{
			*pCsc = s_Vdec_Identity;
			BDBG_MSG(("Selecting: %s", "s_Vdec_Identity"));
		}
	}
	else if(BVDC_P_VdecColorSpace_eYuv == eInColorSpace)
	{
		if(BFMT_VideoFmt_eNTSC_J == pFmtInfo->eVideoFmt)
		{
			*pCsc = (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
				? s_Yuv_To_SdYCrCb_NTSC_J : s_Yuv_To_HdYCrCb;
			BDBG_MSG(("Selecting: %s", (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
				? "s_Yuv_To_SdYCrCb_NTSC_J" : "s_Yuv_To_HdYCrCb"));
		}
		else if((BFMT_VideoFmt_ePAL_NC== pFmtInfo->eVideoFmt) ||
		        (BFMT_VideoFmt_ePAL_G== pFmtInfo->eVideoFmt) ||
		        (BFMT_VideoFmt_ePAL_60== pFmtInfo->eVideoFmt))
		{
			*pCsc = (BVDC_P_CmpColorSpace_ePalSdYCrCb == eOuColorSpace)
				? s_Yuv_To_SdYCrCb_PAL_D_NC: s_Yuv_To_HdYCrCb;
			BDBG_MSG(("Selecting: %s", (BVDC_P_CmpColorSpace_ePalSdYCrCb == eOuColorSpace)
				? "s_Yuv_To_SdYCrCb_PAL_D_NC" : "s_Yuv_To_HdYCrCb"));
		}
		else if(VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt))
		{  /* check for correctness */
			*pCsc = (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
				? s_Yuv_To_SdYCrCb_SECAM : s_Yuv_To_HdYCrCb;
			BDBG_MSG(("Selecting: %s", (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
				? "s_Yuv_To_SdYCrCb" : "s_Yuv_To_HdYCrCb"));
		}
		else
		{
			*pCsc = (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
				? s_Yuv_To_SdYCrCb : s_Yuv_To_HdYCrCb;
			BDBG_MSG(("Selecting: %s", (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
				? "s_Yuv_To_SdYCrCb" : "s_Yuv_To_HdYCrCb"));
		}
	}
	else if(BVDC_P_VdecColorSpace_eSdRgb == eInColorSpace)
	{
		*pCsc = (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
			? s_SdRgb_To_SdYCrCb : s_SdRgb_To_HdYCrCb;
		BDBG_MSG(("Selecting: %s", (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
			? "s_SdRgb_To_SdYCrCb" : "s_SdRgb_To_HdYCrCb"));
	}
	else if(BVDC_P_VdecColorSpace_eSdYPrPb == eInColorSpace)
	{
		*pCsc = (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
			? s_SdYPrPb_To_SdYCrCb : s_SdYPrPb_To_HdYCrCb;
		BDBG_MSG(("Selecting: %s", (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
			? "s_SdYPrPb_To_SdYCrCb" : "s_SdYPrPb_To_HdYCrCb"));
	}
	else if(BVDC_P_VdecColorSpace_eHdRgb == eInColorSpace)
	{
		*pCsc = (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
			? s_HdRgb_To_SdYCrCb : s_HdRgb_To_HdYCrCb;
		BDBG_MSG(("Selecting: %s", (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
			? "s_HdRgb_To_SdYCrCb" : "s_HdRgb_To_HdYCrCb"));
	}
	else if(BVDC_P_VdecColorSpace_eHdYPrPb == eInColorSpace)
	{
		*pCsc = (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
			? s_HdYPrPb_To_SdYCrCb : s_HdYPrPb_To_HdYCrCb;
		BDBG_MSG(("Selecting: %s", (BVDC_P_CmpColorSpace_eNtscSdYCrCb == eOuColorSpace)
			? "s_HdYPrPb_To_SdYCrCb" : "s_HdYPrPb_To_HdYCrCb"));
	}
	else
	{
		BDBG_MSG(("Unknown colorspace conversion use identity"));
		*pCsc = s_Vdec_Identity;
	}

	/* Let's what they look like */
	BVDC_P_Csc_Print_isr(pCsc);

	return;
}


/***************************************************************************
 * Return the desired color space coverstion for CSC in HdDvi.
 */
void BVDC_P_Csc_GetHdDviTable_isr
	( BVDC_P_CscCoeffs                *pCsc,
	  BAVC_CscMode                     eCscMode )
{
	switch(eCscMode)
	{
	case BAVC_CscMode_e709RgbFullRange:
		*pCsc = s_HdDvi_HdRgb_To_HdYCrCb_FullRange;
		break;

	case BAVC_CscMode_e709RgbLimitedRange:
		*pCsc = s_HdDvi_HdRgb_To_HdYCrCb;
		break;

	case BAVC_CscMode_e601RgbFullRange:
		*pCsc = s_HdDvi_SdRgb_To_SdYCrCb_FullRange;
		break;

	case BAVC_CscMode_e601RgbLimitedRange:
		*pCsc = s_HdDvi_SdRgb_To_SdYCrCb;
		break;

	case BAVC_CscMode_e709YCbCr:
	case BAVC_CscMode_e601YCbCr:
	case BAVC_CscMode_eMax:
	default:
		BDBG_MSG(("Unknown colorspace conversion use identity: %d", eCscMode));
		*pCsc = s_HdDvi_Identity;
	}

	/* Let's what they look like */
	BDBG_MSG(("Hd Dvi Matrix:"));
	BVDC_P_Csc_Print_isr(pCsc);

	return;
}

/***************************************************************************
 * Return the color space converstion for CSC in HDDVI or CMP from user matrix.
 */
void BVDC_P_Csc_FromMatrix_isr
	( BVDC_P_CscCoeffs                *pCsc,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift )
{
	pCsc->usY0       = BVDC_P_FR_USR_MATRIX(pl32_Matrix[0],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usY1       = BVDC_P_FR_USR_MATRIX(pl32_Matrix[1],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usY2       = BVDC_P_FR_USR_MATRIX(pl32_Matrix[2],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usYAlpha   = BVDC_P_FR_USR_MATRIX(pl32_Matrix[3],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usYOffset  = BVDC_P_FR_USR_MATRIX(pl32_Matrix[4],  ulShift, pCsc->usCoIntBits, pCsc->usCoFractBits);

	pCsc->usCb0      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[5],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usCb1      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[6],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usCb2      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[7],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usCbAlpha  = BVDC_P_FR_USR_MATRIX(pl32_Matrix[8],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usCbOffset = BVDC_P_FR_USR_MATRIX(pl32_Matrix[9],  ulShift, pCsc->usCoIntBits, pCsc->usCoFractBits);

	pCsc->usCr0      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[10], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usCr1      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[11], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usCr2      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[12], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usCrAlpha  = BVDC_P_FR_USR_MATRIX(pl32_Matrix[13], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
	pCsc->usCrOffset = BVDC_P_FR_USR_MATRIX(pl32_Matrix[14], ulShift, pCsc->usCoIntBits, pCsc->usCoFractBits);

	/* Let's what they look like */
	BDBG_MSG(("User Matrix:"));
	BVDC_P_Csc_Print_isr(pCsc);

	return;
}


/***************************************************************************
 * Return the user matrix from vdec color space coverstion table.
 */
void BVDC_P_Csc_ToMatrix_isr
	( int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  const BVDC_P_CscCoeffs          *pCsc,
	  uint32_t                         ulShift )
{
	pl32_Matrix[0]  = BVDC_P_TO_USR_MATRIX(pCsc->usY0,       pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[1]  = BVDC_P_TO_USR_MATRIX(pCsc->usY1,       pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[2]  = BVDC_P_TO_USR_MATRIX(pCsc->usY2,       pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[3]  = BVDC_P_TO_USR_MATRIX(pCsc->usYAlpha,   pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[4]  = BVDC_P_TO_USR_MATRIX(pCsc->usYOffset,  pCsc->usCoIntBits, pCsc->usCoFractBits, ulShift);

	pl32_Matrix[5]  = BVDC_P_TO_USR_MATRIX(pCsc->usCb0,      pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[6]  = BVDC_P_TO_USR_MATRIX(pCsc->usCb1,      pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[7]  = BVDC_P_TO_USR_MATRIX(pCsc->usCb2,      pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[8]  = BVDC_P_TO_USR_MATRIX(pCsc->usCbAlpha,  pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[9]  = BVDC_P_TO_USR_MATRIX(pCsc->usCbOffset, pCsc->usCoIntBits, pCsc->usCoFractBits, ulShift);

	pl32_Matrix[10] = BVDC_P_TO_USR_MATRIX(pCsc->usCr0,      pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[11] = BVDC_P_TO_USR_MATRIX(pCsc->usCr1,      pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[12] = BVDC_P_TO_USR_MATRIX(pCsc->usCr2,      pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[13] = BVDC_P_TO_USR_MATRIX(pCsc->usCrAlpha,  pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
	pl32_Matrix[14] = BVDC_P_TO_USR_MATRIX(pCsc->usCrOffset, pCsc->usCoIntBits, pCsc->usCoFractBits, ulShift);

	return;
}


/***************************************************************************
 * Return the color space converstion for CSC in HDDVI or CMP from user matrix.
 * User matrices are converted from user YCbCr->RGB format to Dvo hardware
 * csc format of CbYCr->GBR
 */
void BVDC_P_Csc_FromMatrixDvo_isr
	( BVDC_P_CscCoeffs                *pCsc,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift,
	  bool                             bRgb )
{
	if (bRgb)
	{
		/* R */
		pCsc->usCr0      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[1], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCr1      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[0], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCr2      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[2], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCrAlpha  = BVDC_P_FR_USR_MATRIX(pl32_Matrix[3], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCrOffset = BVDC_P_FR_USR_MATRIX(pl32_Matrix[4], ulShift, pCsc->usCoIntBits, pCsc->usCoFractBits);

		/* G */
		pCsc->usY0       = BVDC_P_FR_USR_MATRIX(pl32_Matrix[6],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usY1       = BVDC_P_FR_USR_MATRIX(pl32_Matrix[5],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usY2       = BVDC_P_FR_USR_MATRIX(pl32_Matrix[7],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usYAlpha   = BVDC_P_FR_USR_MATRIX(pl32_Matrix[8],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usYOffset  = BVDC_P_FR_USR_MATRIX(pl32_Matrix[9],  ulShift, pCsc->usCoIntBits, pCsc->usCoFractBits);

		/* B */
		pCsc->usCb0      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[11],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCb1      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[10],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCb2      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[12],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCbAlpha  = BVDC_P_FR_USR_MATRIX(pl32_Matrix[13],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCbOffset = BVDC_P_FR_USR_MATRIX(pl32_Matrix[14],  ulShift, pCsc->usCoIntBits, pCsc->usCoFractBits);
	}
	else
	{
		pCsc->usY0       = BVDC_P_FR_USR_MATRIX(pl32_Matrix[1],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usY1       = BVDC_P_FR_USR_MATRIX(pl32_Matrix[0],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usY2       = BVDC_P_FR_USR_MATRIX(pl32_Matrix[2],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usYAlpha   = BVDC_P_FR_USR_MATRIX(pl32_Matrix[3],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usYOffset  = BVDC_P_FR_USR_MATRIX(pl32_Matrix[4],  ulShift, pCsc->usCoIntBits, pCsc->usCoFractBits);

		pCsc->usCb0      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[6],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCb1      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[5],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCb2      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[7],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCbAlpha  = BVDC_P_FR_USR_MATRIX(pl32_Matrix[8],  ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCbOffset = BVDC_P_FR_USR_MATRIX(pl32_Matrix[9],  ulShift, pCsc->usCoIntBits, pCsc->usCoFractBits);

		pCsc->usCr0      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[11], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCr1      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[10], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCr2      = BVDC_P_FR_USR_MATRIX(pl32_Matrix[12], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCrAlpha  = BVDC_P_FR_USR_MATRIX(pl32_Matrix[13], ulShift, pCsc->usCxIntBits, pCsc->usCxFractBits);
		pCsc->usCrOffset = BVDC_P_FR_USR_MATRIX(pl32_Matrix[14], ulShift, pCsc->usCoIntBits, pCsc->usCoFractBits);
	}

	/* Let's what they look like */
	BDBG_MSG(("User DVO Matrix:"));
	BVDC_P_Csc_Print_isr(pCsc);

	return;
}


/***************************************************************************
 * Return the user matrix from vdec color space coverstion table.
 */
void BVDC_P_Csc_ToMatrixDvo_isr
	( int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  const BVDC_P_CscCoeffs          *pCsc,
	  uint32_t                         ulShift,
	  bool                             bRgb )
{
	if (bRgb)
	{
		/* R */
		pl32_Matrix[0] = BVDC_P_TO_USR_MATRIX(pCsc->usCr1,        pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[1] = BVDC_P_TO_USR_MATRIX(pCsc->usCr0,        pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[2] = BVDC_P_TO_USR_MATRIX(pCsc->usCr2,        pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[3] = BVDC_P_TO_USR_MATRIX(pCsc->usCrAlpha,    pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[4] = BVDC_P_TO_USR_MATRIX(pCsc->usCrOffset,   pCsc->usCoIntBits, pCsc->usCoFractBits, ulShift);

		/* G */
		pl32_Matrix[5]  = BVDC_P_TO_USR_MATRIX(pCsc->usY1,        pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[6]  = BVDC_P_TO_USR_MATRIX(pCsc->usY0,        pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[7]  = BVDC_P_TO_USR_MATRIX(pCsc->usY2,        pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[8]  = BVDC_P_TO_USR_MATRIX(pCsc->usYAlpha,    pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[9]  = BVDC_P_TO_USR_MATRIX(pCsc->usYOffset,   pCsc->usCoIntBits, pCsc->usCoFractBits, ulShift);

		/* B */
		pl32_Matrix[10]  = BVDC_P_TO_USR_MATRIX(pCsc->usCb1,      pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[11]  = BVDC_P_TO_USR_MATRIX(pCsc->usCb0,      pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[12]  = BVDC_P_TO_USR_MATRIX(pCsc->usCb2,      pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[13]  = BVDC_P_TO_USR_MATRIX(pCsc->usCbAlpha,  pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[14]  = BVDC_P_TO_USR_MATRIX(pCsc->usCbOffset, pCsc->usCoIntBits, pCsc->usCoFractBits, ulShift);
	}
	else
	{
		pl32_Matrix[0]  = BVDC_P_TO_USR_MATRIX(pCsc->usY1,        pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[1]  = BVDC_P_TO_USR_MATRIX(pCsc->usY0,        pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[2]  = BVDC_P_TO_USR_MATRIX(pCsc->usY2,        pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[3]  = BVDC_P_TO_USR_MATRIX(pCsc->usYAlpha,    pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[4]  = BVDC_P_TO_USR_MATRIX(pCsc->usYOffset,   pCsc->usCoIntBits, pCsc->usCoFractBits, ulShift);

		pl32_Matrix[5]  = BVDC_P_TO_USR_MATRIX(pCsc->usCb1,       pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[6]  = BVDC_P_TO_USR_MATRIX(pCsc->usCb0,       pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[7]  = BVDC_P_TO_USR_MATRIX(pCsc->usCb2,       pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[8]  = BVDC_P_TO_USR_MATRIX(pCsc->usCbAlpha,   pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[9]  = BVDC_P_TO_USR_MATRIX(pCsc->usCbOffset,  pCsc->usCoIntBits, pCsc->usCoFractBits, ulShift);

		pl32_Matrix[10] = BVDC_P_TO_USR_MATRIX(pCsc->usCr1,       pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[11] = BVDC_P_TO_USR_MATRIX(pCsc->usCr0,       pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[12] = BVDC_P_TO_USR_MATRIX(pCsc->usCr2,       pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[13] = BVDC_P_TO_USR_MATRIX(pCsc->usCrAlpha,   pCsc->usCxIntBits, pCsc->usCxFractBits, ulShift);
		pl32_Matrix[14] = BVDC_P_TO_USR_MATRIX(pCsc->usCrOffset,  pCsc->usCoIntBits, pCsc->usCoFractBits, ulShift);
	}

	return;
}

/***************************************************************************
 * Apply the contrast calculation to the color matrix
 */
void BVDC_P_Csc_ApplyContrast_isr
	( int16_t                          sContrast,
	  BVDC_P_CscCoeffs                *pCscCoeffs )
{
	int32_t lFixK;
	int32_t lFixKMin = BVDC_P_CONTRAST_FIX_K_MIN;
	int32_t lFixKMax = BVDC_P_CONTRAST_FIX_K_MAX;
	int32_t lFixOne = BVDC_P_CSC_ITOFIX(1);

	int32_t lFixYOffset;
	int32_t lFixCbOffset;
	int32_t lFixCrOffset;

	BDBG_MSG(("Apply contrast = %d:", sContrast));
	BDBG_MSG(("Input CSC : %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

	/* K of 1.0 is no contrast adjustment.
	 * K changes linearly from Kmin to 1 with input contrast from -32768 to 0
	 */
	if (sContrast <= 0)
	{
		lFixK = (((lFixOne - lFixKMin) * (sContrast - BVDC_P_CONTRAST_VAL_MIN)) /
		         -BVDC_P_CONTRAST_VAL_MIN
				)
				+ lFixKMin;
	}
	/* K changes linearly from slightly greater than 1.0 to KMax with input contrast from 1 to 32767 */
	else
	{
		lFixK = (((lFixKMax - lFixOne) * sContrast) /
				 BVDC_P_CONTRAST_VAL_MAX
				)
				+ lFixOne;
	}

	lFixYOffset  = BVDC_P_CSC_FIX_MUL_OFFSET(lFixK, BVDC_P_CSC_YOTOFIX(pCscCoeffs), pCscCoeffs) +
				   (BVDC_P_LUMA_BLACK_OFFSET * (lFixOne - lFixK));

	lFixCbOffset = BVDC_P_CSC_FIX_MUL_OFFSET(lFixK, BVDC_P_CSC_CBOTOFIX(pCscCoeffs), pCscCoeffs) +
				   (BVDC_P_CHROMA_BLACK_OFFSET * (lFixOne - lFixK));

	lFixCrOffset = BVDC_P_CSC_FIX_MUL_OFFSET(lFixK, BVDC_P_CSC_CROTOFIX(pCscCoeffs), pCscCoeffs) +
				   (BVDC_P_CHROMA_BLACK_OFFSET * (lFixOne - lFixK));

	/* Y */
	pCscCoeffs->usY0 = BVDC_P_CSC_FIXTOCX(
		                         BVDC_P_CSC_FIX_MUL(lFixK, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usY0)));
	pCscCoeffs->usY1 = BVDC_P_CSC_FIXTOCX(
		                         BVDC_P_CSC_FIX_MUL(lFixK, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usY1)));
	pCscCoeffs->usY2 = BVDC_P_CSC_FIXTOCX(
		                         BVDC_P_CSC_FIX_MUL(lFixK, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usY2)));

	BVDC_P_CSC_FIXTOYO(lFixYOffset, pCscCoeffs);

#if (BVDC_SUPPORT_CONTRAST_WITH_CBCR)
	/* Cb */
	pCscCoeffs->usCb0 = BVDC_P_CSC_FIXTOCX(
		                         BVDC_P_CSC_FIX_MUL(lFixK, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb0)));
	pCscCoeffs->usCb1 = BVDC_P_CSC_FIXTOCX(
		                         BVDC_P_CSC_FIX_MUL(lFixK, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb1)));
	pCscCoeffs->usCb2 = BVDC_P_CSC_FIXTOCX(
		                         BVDC_P_CSC_FIX_MUL(lFixK, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb2)));

	BVDC_P_CSC_FIXTOCBO(lFixCbOffset, pCscCoeffs);

	/* Cr */
	pCscCoeffs->usCr0 = BVDC_P_CSC_FIXTOCX(
		                         BVDC_P_CSC_FIX_MUL(lFixK, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr0)));
	pCscCoeffs->usCr1 = BVDC_P_CSC_FIXTOCX(
		                         BVDC_P_CSC_FIX_MUL(lFixK, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr1)));
	pCscCoeffs->usCr2 = BVDC_P_CSC_FIXTOCX(
		                         BVDC_P_CSC_FIX_MUL(lFixK, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr2)));

	BVDC_P_CSC_FIXTOCRO(lFixCrOffset, pCscCoeffs);
#endif

	BDBG_MSG(("Output CSC: %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

	return;
}


/***************************************************************************
 * Apply the Saturation and Hue calculation to color matrix
 */
void BVDC_P_Csc_ApplySaturationAndHue_isr
	( int16_t                          sSaturation,
	  int16_t                          sHue,
	  BVDC_P_CscCoeffs                *pCscCoeffs )
{
	int32_t lTmpCb0;
	int32_t lTmpCb1;
	int32_t lTmpCb2;
	int32_t lTmpCbOffset;

	int32_t lTmpCr0;
	int32_t lTmpCr1;
	int32_t lTmpCr2;
	int32_t lTmpCrOffset;

	int32_t lFixKa;
	int32_t lFixKt;

	int32_t lFixKaMax = BVDC_P_SATURATION_FIX_KA_MAX;
	int32_t lFixKhMax = BVDC_P_HUE_FIX_KH_MAX;

	int32_t lFixKSinKt;
	int32_t lFixKCosKt;
	int32_t lFixC0;
	int32_t lFixC1;

	int32_t lFixOne = BVDC_P_CSC_ITOFIX(1);

	BDBG_MSG(("Apply sat = %d, hue = %d:", sSaturation, sHue));
	BDBG_MSG(("Input CSC : %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

	/**
	 * Ka of 1.0 is no saturation adjustment.
	 * Ka changes linearly with input saturation value, from 0 to 1.0 when saturation
	 * is negative, and from 1.0 to 4 when saturation is positive.
	 *
	 * With KaMax = 4, minimum saturation = -32768, maximum saturation = 32767:
	 * -32768 input saturation equals Ka of 0
	 *      0 input saturation equals Ka of 1
	 *  32767 input saturation equals Ka of 4
	 */

	if (sSaturation <= 0)
	{
		lFixKa = (lFixOne * (sSaturation - BVDC_P_CONTRAST_VAL_MIN)) /
			     -BVDC_P_SATURATION_VAL_MIN;
	}
	else
	{
		lFixKa = ((((lFixKaMax - lFixOne) * sSaturation) /
		           BVDC_P_SATURATION_VAL_MAX))
				 + lFixOne;
	}

	/**
	 * Kt of 0 is no hue adjustment.
	 * Kt changes linearly with input hue value, bounded by +/- Khmax.
	 * hue of -32768 is clamped to -32767.
	 *
	 * With KhMax = pi, minimum hue = -32767, maximum hue = 32767:
	 * -32767 input hue equals Kt of -pi
	 *      0 input hue equals Kt of 0
	 *  32767 input hue equals Kt of pi
	 */
	if (sHue < -BVDC_P_HUE_VAL_MAX)
	{
		sHue = -BVDC_P_HUE_VAL_MAX;
	}

	lFixKt = (lFixKhMax * sHue) / BVDC_P_HUE_VAL_MAX;

	lFixKSinKt = BVDC_P_CSC_FIX_MUL(lFixKa , BVDC_P_CSC_FIX_SIN(lFixKt));
	lFixKCosKt = BVDC_P_CSC_FIX_MUL(lFixKa , BVDC_P_CSC_FIX_COS(lFixKt));

	/* offset coefficient is stored in integer format */
	lFixC0 = ((1 << (BVDC_P_CSC_VIDEO_DATA_BITS - 1)) * (lFixKSinKt - lFixKCosKt + lFixOne));
	lFixC1 = ((1 << (BVDC_P_CSC_VIDEO_DATA_BITS - 1)) * (-lFixKSinKt - lFixKCosKt + lFixOne));

	lTmpCb0      = BVDC_P_CSC_FIX_MUL(lFixKCosKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb0)) -
				   BVDC_P_CSC_FIX_MUL(lFixKSinKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr0));
	lTmpCb1      = BVDC_P_CSC_FIX_MUL(lFixKCosKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb1)) -
				   BVDC_P_CSC_FIX_MUL(lFixKSinKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr1));
	lTmpCb2      = BVDC_P_CSC_FIX_MUL(lFixKCosKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb2)) -
				   BVDC_P_CSC_FIX_MUL(lFixKSinKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr2));
	lTmpCbOffset = BVDC_P_CSC_FIX_MUL_OFFSET(lFixKCosKt, BVDC_P_CSC_CBOTOFIX(pCscCoeffs), pCscCoeffs) -
		           BVDC_P_CSC_FIX_MUL_OFFSET(lFixKSinKt, BVDC_P_CSC_CROTOFIX(pCscCoeffs), pCscCoeffs) +
				   lFixC0;

	lTmpCr0      = BVDC_P_CSC_FIX_MUL(lFixKSinKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb0)) +
				   BVDC_P_CSC_FIX_MUL(lFixKCosKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr0));
	lTmpCr1      = BVDC_P_CSC_FIX_MUL(lFixKSinKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb1)) +
				   BVDC_P_CSC_FIX_MUL(lFixKCosKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr1));
	lTmpCr2      = BVDC_P_CSC_FIX_MUL(lFixKSinKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb2)) +
				   BVDC_P_CSC_FIX_MUL(lFixKCosKt, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr2));
	lTmpCrOffset = BVDC_P_CSC_FIX_MUL_OFFSET(lFixKSinKt, BVDC_P_CSC_CBOTOFIX(pCscCoeffs), pCscCoeffs) +
		           BVDC_P_CSC_FIX_MUL_OFFSET(lFixKCosKt, BVDC_P_CSC_CROTOFIX(pCscCoeffs), pCscCoeffs) +
				   lFixC1;

	pCscCoeffs->usCb0      = BVDC_P_CSC_FIXTOCX(lTmpCb0);
	pCscCoeffs->usCb1      = BVDC_P_CSC_FIXTOCX(lTmpCb1);
	pCscCoeffs->usCb2      = BVDC_P_CSC_FIXTOCX(lTmpCb2);
	BVDC_P_CSC_FIXTOCBO(lTmpCbOffset, pCscCoeffs);

	pCscCoeffs->usCr0      = BVDC_P_CSC_FIXTOCX(lTmpCr0);
	pCscCoeffs->usCr1      = BVDC_P_CSC_FIXTOCX(lTmpCr1);
	pCscCoeffs->usCr2      = BVDC_P_CSC_FIXTOCX(lTmpCr2);
	BVDC_P_CSC_FIXTOCRO(lTmpCrOffset, pCscCoeffs);

	BDBG_MSG(("Output CSC: %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

	return;
}


/***************************************************************************
 * Apply brightness calculation to color matrix
 */
void BVDC_P_Csc_ApplyBrightness_isr
	( int16_t                          sBrightness,
	  BVDC_P_CscCoeffs                *pCscCoeffs )
{
	int16_t sK;

	BDBG_MSG(("Apply brightness = %d:", sBrightness));
	BDBG_MSG(("Input CSC : %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

	/* brightness of -32768 clamped to -32767. */
	if (sBrightness < -BVDC_P_BRIGHTNESS_VAL_MAX)
	{
		sBrightness = -BVDC_P_BRIGHTNESS_VAL_MAX;
	}

	/* sK varies linearly from -KMax to KMax based on input brightness */
	if (BVDC_P_CSC_USE_ALPHA(pCscCoeffs))
	{
		sK = ((sBrightness << (BVDC_P_CX_FRACTION_BITS - BVDC_P_CSC_VIDEO_DATA_BITS)) *
			  BVDC_P_BRIGHTNESS_K_MAX) / BVDC_P_BRIGHTNESS_VAL_MAX;

		pCscCoeffs->usYAlpha += sK;
		pCscCoeffs->usYAlpha &= BVDC_P_CX_MASK;
	}
	else
	{
		sK = ((sBrightness << BVDC_P_CO_FRACTION_BITS) *
			  BVDC_P_BRIGHTNESS_K_MAX) / BVDC_P_BRIGHTNESS_VAL_MAX;

		pCscCoeffs->usYOffset += sK;
		pCscCoeffs->usYOffset &= BVDC_P_CO_MASK;
	}

	BDBG_MSG(("Output CSC: %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

	return;
}


/***************************************************************************
 * Apply RGB attenuation calculation to color matrix
 */
void BVDC_P_Csc_ApplyAttenuationRGB_isr
	( int32_t                          lAttenuationR,
	  int32_t                          lAttenuationG,
	  int32_t                          lAttenuationB,
	  int32_t                          lOffsetR,
	  int32_t                          lOffsetG,
	  int32_t                          lOffsetB,
	  BVDC_P_CscCoeffs                *pCscCoeffs,
	  const BVDC_P_CscCoeffs          *pYCbCrToRGB,
	  const BVDC_P_CscCoeffs          *pRGBToYCbCr,
	  bool                             bUserCsc,
	  bool                             bUserCscC )
{
	int32_t M0[4][4];
	int32_t M1[4][4];
	int32_t M2[4][4];
	int32_t MTmp[4][4];

	BDBG_ASSERT(pCscCoeffs);
	BDBG_ASSERT(pYCbCrToRGB);
	BDBG_ASSERT(pRGBToYCbCr);

	BDBG_MSG(("Apply RGB Attenuation"));
	BDBG_MSG(("Attenuation R=%d, G=%d, B=%d, Offset R=%d, G=%d, B=%d:",
		lAttenuationR, lAttenuationG, lAttenuationB, lOffsetR, lOffsetG, lOffsetB));
	BDBG_MSG(("YCbCrToRGB matrix:"));
	BVDC_P_Csc_Print_isr(pYCbCrToRGB);
	BDBG_MSG(("RGBToYCbCr matrix:"));
	BVDC_P_Csc_Print_isr(pRGBToYCbCr);
	BDBG_MSG(("Input CSC : %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));
	BVDC_P_Csc_Print_isr(pCscCoeffs);

	if(lAttenuationB == (int32_t)BVDC_P_CSC_ITOFIX(1) &&
	   lAttenuationG == (int32_t)BVDC_P_CSC_ITOFIX(1) &&
	   lAttenuationR == (int32_t)BVDC_P_CSC_ITOFIX(1) &&
	   lOffsetB == 0 &&
	   lOffsetG == 0 &&
	   lOffsetR == 0)
	{
		BDBG_MSG(("Output CSC: %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
			pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
			pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
			pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

		BVDC_P_Csc_Print_isr(pCscCoeffs);

		return;
	}
	else if (bUserCsc && !bUserCscC)
	{
		BDBG_ERR(("Color Temp or RGB Attenuation adjustment requires BVDC_Window_SetColorMatrixNonLinearC to be set when BVDC_Window_SetColorMatrix is used."));
		return;
	}

	/* M0 = Original CSC Matrix */
	BVDC_P_CSC_MAKE4X4(M0, pCscCoeffs);

	/* M1 = YCrCb to RGB Matrix */
	BVDC_P_CSC_MAKE4X4(M1, pYCbCrToRGB);

	M1[0][0] = BVDC_P_CSC_FIX_MUL(lAttenuationR, M1[0][0]);
	M1[0][1] = BVDC_P_CSC_FIX_MUL(lAttenuationR, M1[0][1]);
	M1[0][2] = BVDC_P_CSC_FIX_MUL(lAttenuationR, M1[0][2]);
	M1[0][3] = BVDC_P_CSC_FIX_MUL_OFFSET(lAttenuationR, M1[0][3], pCscCoeffs) + lOffsetR ;

	M1[1][0] = BVDC_P_CSC_FIX_MUL(lAttenuationG, M1[1][0]);
	M1[1][1] = BVDC_P_CSC_FIX_MUL(lAttenuationG, M1[1][1]);
	M1[1][2] = BVDC_P_CSC_FIX_MUL(lAttenuationG, M1[1][2]);
	M1[1][3] = BVDC_P_CSC_FIX_MUL_OFFSET(lAttenuationG, M1[1][3], pCscCoeffs) + lOffsetG ;

	M1[2][0] = BVDC_P_CSC_FIX_MUL(lAttenuationB, M1[2][0]);
	M1[2][1] = BVDC_P_CSC_FIX_MUL(lAttenuationB, M1[2][1]);
	M1[2][2] = BVDC_P_CSC_FIX_MUL(lAttenuationB, M1[2][2]);
	M1[2][3] = BVDC_P_CSC_FIX_MUL_OFFSET(lAttenuationB, M1[2][3], pCscCoeffs) + lOffsetB ;

	/* M2 = RGB to YCrCb Matrix */
	BVDC_P_CSC_MAKE4X4(M2, pRGBToYCbCr);

	/* Multiply M2*M1 -> Store in MTmp  */
	BVDC_P_Csc_Mult4X4_isr(MTmp, M2, M1, pCscCoeffs);

	/* Multiply MTmp*M0 -> store in M1 */
	BVDC_P_Csc_Mult4X4_isr(M1, MTmp, M0, pCscCoeffs);

	pCscCoeffs->usY0  = BVDC_P_CSC_FIXTOCX(M1[0][0]);
	pCscCoeffs->usY1  = BVDC_P_CSC_FIXTOCX(M1[0][1]);
	pCscCoeffs->usY2  = BVDC_P_CSC_FIXTOCX(M1[0][2]);
	BVDC_P_CSC_FIXTOYO(M1[0][3], pCscCoeffs);

	pCscCoeffs->usCb0 = BVDC_P_CSC_FIXTOCX(M1[1][0]);
	pCscCoeffs->usCb1 = BVDC_P_CSC_FIXTOCX(M1[1][1]);
	pCscCoeffs->usCb2 = BVDC_P_CSC_FIXTOCX(M1[1][2]);
	BVDC_P_CSC_FIXTOCBO(M1[1][3], pCscCoeffs);

	pCscCoeffs->usCr0 = BVDC_P_CSC_FIXTOCX(M1[2][0]);
	pCscCoeffs->usCr1 = BVDC_P_CSC_FIXTOCX(M1[2][1]);
	pCscCoeffs->usCr2 = BVDC_P_CSC_FIXTOCX(M1[2][2]);
	BVDC_P_CSC_FIXTOCRO(M1[2][3], pCscCoeffs);

	BDBG_MSG(("Output CSC: %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

	BVDC_P_Csc_Print_isr(pCscCoeffs);

	return;
}


/***************************************************************************
 * Convert color temperature to attenuation RGB
 */
BERR_Code BVDC_P_Csc_ColorTempToAttenuationRGB
	( int16_t                          sColorTemp,
	  int32_t                         *plAttenuationR,
	  int32_t                         *plAttenuationG,
	  int32_t                         *plAttenuationB,
  	  BVDC_P_CscCoeffs                *pCscCoeffs )

{
	int32_t lSlope, lKelvin;
	int32_t lAttenuationR;
	int32_t lAttenuationG;
	int32_t lAttenuationB;

	/* Maximum and Minimum Values of Brightness Slider
	  (Presumably these would be BVDC_P_BRIGHTNESS_VAL_MAX and BVDC_P_BRIGHTNESS_VAL_MIN,
	   however this is not the case at the moment) */

	int32_t lColorTempMax = BVDC_P_COLORTEMP_VAL_MAX;
	int32_t lColorTempMin = BVDC_P_COLORTEMP_VAL_MIN;
	int32_t lColorTempCenter;

	/* Maximum, Minimum, and Center Values of Color Temperature in 100s of Kelvin */
	int32_t lKelvinMax    = BVDC_P_COLORTEMP_KELVIN_MAX;
	int32_t lKelvinMin    = BVDC_P_COLORTEMP_KELVIN_MIN;
	int32_t lKelvinCenter = BVDC_P_COLORTEMP_KELVIN_CENTER;


	/* Color Temperature - Piecewise Linear Model Parameters - (Assuming Temp is in 100s of K) */
	int32_t lModel_ParamL[3][2];
	int32_t lModel_ParamH[3][2];
	int i, j;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 2; j++)
		{
			if ((BVDC_P_CX_FRACTION_BITS == BVDC_P_CSC_CMP_CX_F_BITS) &&
				(BVDC_P_CX_INT_BITS == BVDC_P_CSC_CMP_CX_I_BITS))
			{
				lModel_ParamL[i][j] = s_ClrTemp_LModel_ParamL_Cmp[i][j];
				lModel_ParamH[i][j] = s_ClrTemp_LModel_ParamH_Cmp[i][j];
			}
			else if ((BVDC_P_CX_FRACTION_BITS == BVDC_P_CSC_GFD_CX_F_BITS) &&
					 (BVDC_P_CX_INT_BITS == BVDC_P_CSC_GFD_CX_I_BITS))
			{
				lModel_ParamL[i][j] = s_ClrTemp_LModel_ParamL_Gfd[i][j];
				lModel_ParamH[i][j] = s_ClrTemp_LModel_ParamH_Gfd[i][j];
			}
			else
			{
				BDBG_MSG(("Colortemp only supported for CMP and GFD."));
				return BERR_INVALID_PARAMETER;
			}
		}
	}

	BDBG_ENTER(BVDC_P_Csc_ColorTempToAttenuationRGB);

	lColorTempCenter = (lColorTempMin + lColorTempMax)/2;

	if(sColorTemp < lColorTempCenter) {
		lSlope = (lColorTempCenter - lColorTempMin)/(lKelvinCenter - lKelvinMin);
	}
	else {
		lSlope = (BVDC_P_COLORTEMP_VAL_MAX - lColorTempCenter)/(lKelvinMax - lKelvinCenter);
	}

	lKelvin = sColorTemp/lSlope - lColorTempCenter/lSlope + lKelvinCenter;

	/* Determine Attenuation Factors Using Piecewise Linear Model of Color Temperature */
	if(lKelvin < lKelvinCenter) {
		lAttenuationR = ((lModel_ParamL[0][0] * lKelvin) + lModel_ParamL[0][1]) / 100;
		lAttenuationG = ((lModel_ParamL[1][0] * lKelvin) + lModel_ParamL[1][1]) / 100;
		lAttenuationB = ((lModel_ParamL[2][0] * lKelvin) + lModel_ParamL[2][1]) / 100;
	}
	else
	{
		lAttenuationR = ((lModel_ParamH[0][0] * lKelvin) + lModel_ParamH[0][1]) / 100;
		lAttenuationG = ((lModel_ParamH[1][0] * lKelvin) + lModel_ParamH[1][1]) / 100;
		lAttenuationB = ((lModel_ParamH[2][0] * lKelvin) + lModel_ParamH[2][1]) / 100;
	}

	/* Ensure Attenuation Factors are Between 0 and 1 */
	lAttenuationR = (lAttenuationR < 0) ? 0 : (lAttenuationR > (int32_t)BVDC_P_CSC_ITOFIX(1)) ? (int32_t)BVDC_P_CSC_ITOFIX(1) : lAttenuationR;
	lAttenuationG = (lAttenuationG < 0) ? 0 : (lAttenuationG > (int32_t)BVDC_P_CSC_ITOFIX(1)) ? (int32_t)BVDC_P_CSC_ITOFIX(1) : lAttenuationG;
	lAttenuationB = (lAttenuationB < 0) ? 0 : (lAttenuationB > (int32_t)BVDC_P_CSC_ITOFIX(1)) ? (int32_t)BVDC_P_CSC_ITOFIX(1) : lAttenuationB;

	*plAttenuationR = lAttenuationR;
	*plAttenuationG = lAttenuationG;
	*plAttenuationB = lAttenuationB;

	BDBG_LEAVE(BVDC_P_Csc_ColorTempToAttenuationRGB);
	return BERR_SUCCESS;
}


/***************************************************************************
 * Apply RGB attenuation calculation to color matrix, performed in RGB
 * colorspace
 */
void BVDC_P_Csc_DvoApplyAttenuationRGB_isr
	( int32_t                          lAttenuationR,
	  int32_t                          lAttenuationG,
	  int32_t                          lAttenuationB,
	  int32_t                          lOffsetR,
	  int32_t                          lOffsetG,
	  int32_t                          lOffsetB,
	  BVDC_P_CscCoeffs                *pCscCoeffs )
{
	int32_t lNewOffsetR;
	int32_t lNewOffsetG;
	int32_t lNewOffsetB;

	BDBG_MSG(("Apply Dvo RGB Attenuation"));
	BDBG_MSG(("Attenuation R=%d, G=%d, B=%d, Offset R=%d, G=%d, B=%d:",
		lAttenuationR, lAttenuationG, lAttenuationB, lOffsetR, lOffsetG, lOffsetB));
	BDBG_MSG(("Input CSC : %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

	if(lAttenuationB == (int32_t)BVDC_P_CSC_ITOFIX(1) &&
	   lAttenuationG == (int32_t)BVDC_P_CSC_ITOFIX(1) &&
	   lAttenuationR == (int32_t)BVDC_P_CSC_ITOFIX(1) &&
	   lOffsetB == 0 &&
	   lOffsetG == 0 &&
	   lOffsetR == 0)
	{
		BDBG_MSG(("Output CSC: %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
			pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
			pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
			pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

		return;
	}

	lNewOffsetR = BVDC_P_CSC_FIX_MUL_OFFSET(lAttenuationR, BVDC_P_CSC_CROTOFIX(pCscCoeffs), pCscCoeffs) + lOffsetR;
	lNewOffsetG = BVDC_P_CSC_FIX_MUL_OFFSET(lAttenuationG, BVDC_P_CSC_YOTOFIX(pCscCoeffs),  pCscCoeffs) + lOffsetG;
	lNewOffsetB = BVDC_P_CSC_FIX_MUL_OFFSET(lAttenuationB, BVDC_P_CSC_CBOTOFIX(pCscCoeffs), pCscCoeffs) + lOffsetB;

	/* R */
	pCscCoeffs->usCr0 = BVDC_P_CSC_FIXTOCX(BVDC_P_CSC_FIX_MUL(lAttenuationR, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr0)));
	pCscCoeffs->usCr1 = BVDC_P_CSC_FIXTOCX(BVDC_P_CSC_FIX_MUL(lAttenuationR, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr1)));
	pCscCoeffs->usCr2 = BVDC_P_CSC_FIXTOCX(BVDC_P_CSC_FIX_MUL(lAttenuationR, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCr2)));
	BVDC_P_CSC_FIXTOCRO(lNewOffsetR, pCscCoeffs);

	/* G */
	pCscCoeffs->usY0  = BVDC_P_CSC_FIXTOCX(BVDC_P_CSC_FIX_MUL(lAttenuationG, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usY0)));
	pCscCoeffs->usY1  = BVDC_P_CSC_FIXTOCX(BVDC_P_CSC_FIX_MUL(lAttenuationG, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usY1)));
	pCscCoeffs->usY2  = BVDC_P_CSC_FIXTOCX(BVDC_P_CSC_FIX_MUL(lAttenuationG, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usY2)));
	BVDC_P_CSC_FIXTOYO(lNewOffsetG, pCscCoeffs);

	/* B */
	pCscCoeffs->usCb0 = BVDC_P_CSC_FIXTOCX(BVDC_P_CSC_FIX_MUL(lAttenuationB, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb0)));
	pCscCoeffs->usCb1 = BVDC_P_CSC_FIXTOCX(BVDC_P_CSC_FIX_MUL(lAttenuationB, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb1)));
	pCscCoeffs->usCb2 = BVDC_P_CSC_FIXTOCX(BVDC_P_CSC_FIX_MUL(lAttenuationB, BVDC_P_CSC_CXTOFIX(pCscCoeffs->usCb2)));
	BVDC_P_CSC_FIXTOCBO(lNewOffsetB, pCscCoeffs);

	BDBG_MSG(("Output CSC: %x %x %x %x %x, %x %x %x %x %x, %x %x %x %x %x",
		pCscCoeffs->usY0, pCscCoeffs->usY1, pCscCoeffs->usY2, pCscCoeffs->usYAlpha, pCscCoeffs->usYOffset,
		pCscCoeffs->usCb0, pCscCoeffs->usCb1, pCscCoeffs->usCb2, pCscCoeffs->usCbAlpha, pCscCoeffs->usCbOffset,
		pCscCoeffs->usCr0, pCscCoeffs->usCr1, pCscCoeffs->usCr2, pCscCoeffs->usCrAlpha, pCscCoeffs->usCrOffset));

	return;
}


/***************************************************************************
 * Calculate inverse of matrix
 */
void BVDC_P_Csc_MatrixInverse
	( BVDC_P_CscCoeffs                *pCscCoeffs,
	  BVDC_P_CscCoeffs                *pRetInvCscCoeffs )
{
	BMTH_FIX_Matrix stMatrix;
	BMTH_FIX_Matrix stInvMatrix;

	BDBG_MSG(("Calculate Inverse Matrix"));
	BDBG_MSG(("Input Matrix:"));
	BVDC_P_Csc_Print_isr(pCscCoeffs);

	/* Convert matrices to BMTH_FIX_Matrix format */
	BVDC_P_CSC_MAKE4X4_MTH(stMatrix, pCscCoeffs);

	stInvMatrix.ulSize = 4;
	stInvMatrix.ulFractBits = BVDC_P_CX_FRACTION_BITS;
	BKNI_Memset(stInvMatrix.data, 0, sizeof(stInvMatrix.data));

	BMTH_FIX_Matrix_Inverse(&stMatrix, &stInvMatrix);

	/* Convert result to csc coeff matrix format */
    pRetInvCscCoeffs->usY0  = BVDC_P_CSC_FIXTOCX(stInvMatrix.data[0][0]);
    pRetInvCscCoeffs->usY1  = BVDC_P_CSC_FIXTOCX(stInvMatrix.data[0][1]);
    pRetInvCscCoeffs->usY2  = BVDC_P_CSC_FIXTOCX(stInvMatrix.data[0][2]);
	BVDC_P_CSC_FIXTOYO(stInvMatrix.data[0][3], pRetInvCscCoeffs);

    pRetInvCscCoeffs->usCb0 = BVDC_P_CSC_FIXTOCX(stInvMatrix.data[1][0]);
    pRetInvCscCoeffs->usCb1 = BVDC_P_CSC_FIXTOCX(stInvMatrix.data[1][1]);
    pRetInvCscCoeffs->usCb2 = BVDC_P_CSC_FIXTOCX(stInvMatrix.data[1][2]);
	BVDC_P_CSC_FIXTOCBO(stInvMatrix.data[1][3], pRetInvCscCoeffs);

    pRetInvCscCoeffs->usCr0 = BVDC_P_CSC_FIXTOCX(stInvMatrix.data[2][0]);
    pRetInvCscCoeffs->usCr1 = BVDC_P_CSC_FIXTOCX(stInvMatrix.data[2][1]);
    pRetInvCscCoeffs->usCr2 = BVDC_P_CSC_FIXTOCX(stInvMatrix.data[2][2]);
	BVDC_P_CSC_FIXTOCRO(stInvMatrix.data[2][3], pRetInvCscCoeffs);

	BDBG_MSG(("Inverse Matrix:"));
	BVDC_P_Csc_Print_isr(pRetInvCscCoeffs);

	return;
}

void BVDC_P_Csc_MatrixMultVector_isr
	( BVDC_P_CscCoeffs                *pCscCoeffs,
	  uint32_t                         aVector[4],
	  uint32_t                         aRetVector[4])
{
	BMTH_FIX_Vector stVector;
	BMTH_FIX_Vector stRetVector;
	BMTH_FIX_Matrix stMatrix;
	uint32_t i;

	BVDC_P_CSC_MAKE4X4_MTH(stMatrix, pCscCoeffs);

	stVector.ulSize = 4;
	stVector.ulFractBits = BVDC_P_CX_FRACTION_BITS;

	for (i = 0; i < 4; i++)
	{
		stVector.data[i] = aVector[i];
	}

	BMTH_FIX_Matrix_MultVector(&stMatrix, &stVector, &stRetVector);

	for (i=0; i < 4; i++)
	{
		aRetVector[i] = stRetVector.data[i];
	}
}


/***************************************************************************
 * Set a matrix to output specified color in its original colorspace.
 */
void BVDC_P_Csc_ApplyYCbCrColor
	( BVDC_P_CscCoeffs                *pCscCoeffs,
	  uint32_t                         ulColor0,
	  uint32_t                         ulColor1,
	  uint32_t                         ulColor2 )
{
  	uint32_t aulColorVector[4];
  	uint32_t aulRetColorVector[4];

	BDBG_MSG(("Apply YCbCr Color"));
	BDBG_MSG(("ulColor0=%d, ulColor1=%d, ulColor2=%d", ulColor0, ulColor1, ulColor2));
	BDBG_MSG(("Input Matrix:"));
	BVDC_P_Csc_Print_isr(pCscCoeffs);

	aulColorVector[0] = ulColor0;
	aulColorVector[1] = ulColor1;
	aulColorVector[2] = ulColor2;
	aulColorVector[3] = 1;

	/* multiply before we shift to fract bits to avoid overflow */
	BVDC_P_Csc_MatrixMultVector_isr(pCscCoeffs, aulColorVector, aulRetColorVector);

	pCscCoeffs->usY0 = 0;
	pCscCoeffs->usY1 = 0;
	pCscCoeffs->usY2 = 0;
	pCscCoeffs->usYOffset = aulRetColorVector[0] << pCscCoeffs->usCoFractBits;
	pCscCoeffs->usCb0 = 0;
	pCscCoeffs->usCb1 = 0;
	pCscCoeffs->usCb2 = 0;
	pCscCoeffs->usCbOffset = aulRetColorVector[1] << pCscCoeffs->usCoFractBits;
	pCscCoeffs->usCr0 = 0;
	pCscCoeffs->usCr1 = 0;
	pCscCoeffs->usCr2 = 0;
	pCscCoeffs->usCrOffset = aulRetColorVector[2] << pCscCoeffs->usCoFractBits;

	BDBG_MSG(("Output Matrix:"));
	BVDC_P_Csc_Print_isr(pCscCoeffs);
}


/* End of file */
