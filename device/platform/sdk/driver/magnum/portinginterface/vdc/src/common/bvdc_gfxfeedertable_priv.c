/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_gfxfeedertable_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/39 $
 * $brcm_Date: 5/9/12 10:53a $
 *
 * Module Description:
 *   Contains tables for Display settings.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/core/bvdc_gfxfeedertable_priv.c $
 * 
 * Hydra_Software_Devel/39   5/9/12 10:53a syang
 * SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
 * error fix with GetSurface; use shared gfxsurface to manage gfx surface
 * 
 * Hydra_Software_Devel/38   6/24/11 3:49p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 * 
 * Hydra_Software_Devel/37   11/10/10 5:14p albertl
 * SW7550-610: Fixed build error.
 * 
 * Hydra_Software_Devel/35   11/10/10 4:36p albertl
 * SW7550-610: Extended BVDC_Window_SetCoefficientIndex to work for
 * graphics windows.
 * 
 * Hydra_Software_Devel/34   8/26/10 7:41p albertl
 * SW7405-4556, SW7405-4515: Changed RGB attenuation to use inverse of
 * user matrix C.
 * 
 * Hydra_Software_Devel/33   2/4/10 6:26p albertl
 * SW3548-2700: BVDC_P_GfxFeeder_DecideColorMatrix_isr now returns
 * identity if pixel format is not RGB.
 * 
 * Hydra_Software_Devel/32   1/12/10 8:42p albertl
 * SW3548-2700: Changed RGB attenuation to use DVO matrix as YCbCr->RGB
 * matrix and Matrix C as RGB->YCbCr matrix.
 * 
 * Hydra_Software_Devel/31   9/11/09 4:35p tdo
 * SW7420-338: Need to init correct GFD with correct capability
 *
 * Hydra_Software_Devel/30   6/19/09 4:18p tdo
 * PR45796: Update the correct VSCL coeff
 *
 * Hydra_Software_Devel/29   5/19/09 11:12a tdo
 * PR45796: Update VSCL coeff
 *
 * Hydra_Software_Devel/28   3/16/09 10:47p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/6   3/5/09 11:46a syang
 * PR 45796:  use VSCL coeffs from Rahul Sansi
 *
 * Hydra_Software_Devel/7420_mcvp/5   2/20/09 4:07p syang
 * PR 45796: temporarily force to use bilinear coeffs for VSCL
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/18/09 3:40p syang
 * PR 45796: fix 7400 compiling
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/13/09 5:55p syang
 * PR 45796:  added some vscl coeffs, GFD VSCL does show gfx, but with
 * some gabage at top, and some non-linear effect.
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/13/09 10:02a syang
 * PR 45796: first time check in for GFD VSCL support
 *
 * Hydra_Software_Devel/7420_mcvp/1   2/11/09 11:31a tdo
 * PR 45785, PR 45789: merge from main branch on 2/11/09
 *
 * Hydra_Software_Devel/27   2/10/09 11:16a yuxiaz
 * PR51614: VDC global symbol naming convention violations
 *
 * Hydra_Software_Devel/26   10/9/08 5:29p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/25   10/12/07 12:57p syang
 * PR 35067: clean up
 *
 * Hydra_Software_Devel/24   10/11/07 6:19p syang
 * PR 35067: coverity (OVERRUN_STATIC) fix
 *
 * Hydra_Software_Devel/23   9/27/07 7:12p albertl
 * PR35135:  Fixed missing shift for Y Alphas.  Removed obsolete macros.
 *
 * Hydra_Software_Devel/22   9/26/07 8:02p albertl
 * PR35135:  Updated csc calculations to properly handle alpha values.
 *
 * Hydra_Software_Devel/21   9/20/07 1:19a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/20   8/30/07 4:25p darnstein
 * PR33711: declare static tables to be const. This is required by
 * Broadcom's programming standards.
 *
 * Hydra_Software_Devel/19   1/22/07 2:33p syang
 * PR 27262: fixed compile warning with
 * BVDC_P_GetFilterCoefficients_PointSample
 *
 * Hydra_Software_Devel/18   1/22/07 1:18p darnstein
 * PR26619: silence compiler warnings.
 *
 * Hydra_Software_Devel/17   8/10/06 9:57a syang
 * PR 22567: added support for 3563
 *
 * Hydra_Software_Devel/16   8/7/06 5:30p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/15   6/30/06 5:28p syang
 * PR 22413: added pointsample support for gfx horizontal scaling to
 * 7401B0, 7400B0, and 7118
 *
 * Hydra_Software_Devel/14   6/14/06 1:40p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/13   3/7/06 4:14p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/12   1/17/06 4:18p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/11   12/21/05 1:33p syang
 * PR 15191: upadated gfd color matrix according to main video win's color
 * matrix
 *
 * Hydra_Software_Devel/10   8/1/05 5:34p syang
 * PR 16163: updated 7401 gfx horizontal scale coff in gfx feeder
 *
 * Hydra_Software_Devel/9   6/28/05 11:03a jasonh
 * PR 16012: Adding support for 7401
 *
 * Hydra_Software_Devel/8   3/17/05 6:39p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/7   3/15/05 12:24p syang
 * PR 14087: added 3560 support (CM_*4 has now S9_4 format)
 *
 * Hydra_Software_Devel/6   5/24/04 5:10p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/3   5/17/04 5:33p syang
 * PR 9750: fixed a bug in color conv matrix with constant blending
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   5/12/04 5:08p syang
 * PR 11088: Reverse tap posion of filter coeff inside each phase becase
 * GFD HW implement SUM(Coeff[i] * Tap[i]), while the original tables
 * copied from M2MC implement SUM(Coeff[7-i] * Tap[i]).
 *
 ***************************************************************************/
#include "bvdc_gfxfeeder_priv.h"
#include "bvdc_window_priv.h"
#include "bchp.h"
#include "bchp_gfd_0.h"

BDBG_MODULE(BVDC_GFX);

/****************************************************************
 *  Color Conversion Matrix
 ****************************************************************/

/*--------------------------------------------------------------------*/
static const BVDC_P_CscCoeffs  s_RGBA_to_SdYCrCb_AlphaBlend = BVDC_P_MAKE_GFD_CSC
	(  0.2570,  0.5040,  0.0980,  16.0000,  0.0000,
	  -0.1480, -0.2910,  0.4390, 128.0000,  0.0000,
	   0.4390, -0.3680, -0.0710, 128.0000,  0.0000 );

/*--------------------------------------------------------------------*/
static const BVDC_P_CscCoeffs  s_RGBA_to_HdYCrCb_AlphaBlend = BVDC_P_MAKE_GFD_CSC
	(  0.1830,  0.6140,  0.0620,  16.0000,  0.0000,
	  -0.1010, -0.3380,  0.4390, 128.0000,  0.0000,
	   0.4390, -0.3990, -0.0400, 128.0000,  0.0000 );

/*--------------------------------------------------------------------*/
static const BVDC_P_CscCoeffs s_SdYCbCr_to_RGBA_AlphaBlend = BVDC_P_MAKE_GFD_CSC
	(  1.168950,  0.000000,  1.602425, -223.813572,  0.0000,
	   1.168950, -0.394860, -0.816582,  136.361359,  0.0000,
	   1.168950,  2.024147, -0.000000, -277.794001,  0.0000 );

/*--------------------------------------------------------------------*/
static const BVDC_P_CscCoeffs s_HdYCbCr_to_RGBA_AlphaBlend = BVDC_P_MAKE_GFD_CSC
	(  1.168950,  0.000000,  1.799682, -249.062527,  0.0000,
	   1.168950, -0.214073, -0.535094,   77.190243,  0.0000,
	   1.168950,  2.120703,  0.000000, -290.153216,  0.0000 );

/*--------------------------------------------------------------------*/
static const BVDC_P_CscCoeffs  s_RGBA_to_SdYCrCb_ConstBlend = BVDC_P_MAKE_GFD_CSC
	(  0.2570,  0.5040,  0.0980,   0.0000,  16.0000,
	  -0.1480, -0.2910,  0.4390,   0.0000, 128.0000,
	   0.4390, -0.3680, -0.0710,   0.0000, 128.0000 );

/*--------------------------------------------------------------------*/
static const BVDC_P_CscCoeffs  s_RGBA_to_HdYCrCb_ConstBlend = BVDC_P_MAKE_GFD_CSC
	(  0.1830,  0.6140,  0.0620,   0.0000,  16.0000,
	  -0.1010, -0.3380,  0.4390,   0.0000, 128.0000,
	   0.4390, -0.3990, -0.0400,   0.0000, 128.0000 );

/*--------------------------------------------------------------------*/
static const BVDC_P_CscCoeffs s_SdYCbCr_to_RGBA_ConstBlend = BVDC_P_MAKE_GFD_CSC
	(  1.168950,  0.000000,  1.602425,  0.0000, -223.813572,
	   1.168950, -0.394860, -0.816582,  0.0000,  136.361359,
	   1.168950,  2.024147, -0.000000,  0.0000, -277.794001 );

/*--------------------------------------------------------------------*/
static const BVDC_P_CscCoeffs s_HdYCbCr_to_RGBA_ConstBlend = BVDC_P_MAKE_GFD_CSC
	(  1.168950,  0.000000,  1.799682,  0.0000, -249.062527,
	   1.168950, -0.214073, -0.535094,  0.0000,   77.190243,
	   1.168950,  2.120703,  0.000000,  0.0000, -290.153216 );

/*--------------------------------------------------------------------*/
static const BVDC_P_CscCoeffs s_Identity = BVDC_P_MAKE_GFD_CSC
	(  1.000000,  0.000000,  0.000000,  0.000000,  0.000000,
	   0.000000,  1.000000,  0.000000,  0.000000,  0.000000,
	   0.000000,  0.000000,  1.000000,  0.000000,  0.000000 );

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_DecideColorMatrix_isr
 *
 * output: color matrix to convert from active pixel format to output
 *         color primary (main video window's color primary)
 *
 * Note: Because of gamma effect, of not knowing how user treated alpha
 * when the src gfx surface was created, and of diff between Bt601 and
 * Bt709 is not very noticable for gfx, we decide to use idendity matrix
 * to convert between Bt601 and Bt709 (i.e. not conv).
 */
BERR_Code BVDC_P_GfxFeeder_DecideColorMatrix_isr
	( BPXL_Format                  eActivePxlFmt,
	  BVDC_P_GfxFeeder_Handle      hGfxFeeder,
	  bool                         bConstantBlend,
	  BVDC_P_CscCoeffs            *paulClrMatrix,
	  const BVDC_P_CscCoeffs     **ppaulRGBToYCbCr,
	  const BVDC_P_CscCoeffs     **ppaulYCbCrToRGB )
{
	BVDC_P_CmpColorSpace  eDestColorSpace;

	/* TODO: in the future, we should distinquish current hd/sd standard
	 * from legacy hd/sd, and NTSC sd from PAL sd */
	BVDC_P_Window_GetCurrentDestColorSpace( hGfxFeeder->hWindow, &eDestColorSpace );

	if( hGfxFeeder->hWindow->stCurInfo.bUserCscC &&
	    hGfxFeeder->hWindow->stCurInfo.bUserCsc )
	{
		BDBG_MSG(("Using User WIN CSC for GFX RGBToYCbCr Matrix"));
		BDBG_MSG(("Using Inverse User WIN CSC for GFX YCbCrToRGB Matrix"));

		*ppaulYCbCrToRGB = &hGfxFeeder->hWindow->stInvUserCscC;
		*ppaulRGBToYCbCr = paulClrMatrix;
	}
	else
	{
		if (hGfxFeeder->hWindow->stCurInfo.bUserCscC)
		{
			BDBG_WRN(("User GFX CSC Matrix C set without GFX CSC Main Matrix being set."));
			BDBG_WRN(("User GFX CSC Matrix C will be ignored in Color Temp/RGB attenuation."));
		}
		
		BDBG_MSG(("Using hardcoded CSCs for GFX YCbCrToRGB and RGBToYCbCr Matrix"));

		if ( (BVDC_P_CmpColorSpace_eHdYCrCb   == eDestColorSpace) ||
		     (BVDC_P_CmpColorSpace_eSmpte_240M == eDestColorSpace) )
		{

			*ppaulRGBToYCbCr = (bConstantBlend)?
				&s_RGBA_to_HdYCrCb_ConstBlend:
				&s_RGBA_to_HdYCrCb_AlphaBlend;

			*ppaulYCbCrToRGB = (bConstantBlend)?
				&s_HdYCbCr_to_RGBA_ConstBlend:
				&s_HdYCbCr_to_RGBA_AlphaBlend;
		}
		else
		{
			*ppaulRGBToYCbCr = (bConstantBlend)?
				&s_RGBA_to_SdYCrCb_ConstBlend:
				&s_RGBA_to_SdYCrCb_AlphaBlend;

			*ppaulYCbCrToRGB = (bConstantBlend)?
				&s_SdYCbCr_to_RGBA_ConstBlend:
				&s_SdYCbCr_to_RGBA_AlphaBlend;
		}
	}

	/* fill in paulClrMatrix with correct matrix. */
	/* GFX matrix is a single matrix based on pixel format, and never a non-linear matrix like Matrix C. */
	if( hGfxFeeder->hWindow->stCurInfo.bUserCsc )
	{
		BDBG_MSG(("Using User WIN CSC for GFX CSC Matrix"));

		if( hGfxFeeder->stCurCfgInfo.stDirty.stBits.bCsc )
		{
			BVDC_P_Csc_FromMatrix_isr( paulClrMatrix,
				hGfxFeeder->hWindow->stCurInfo.pl32_Matrix,
				hGfxFeeder->hWindow->stCurInfo.ulUserShift);
		}
	}
	else if ( true == BPXL_IS_RGB_FORMAT(eActivePxlFmt) )
	{
		*paulClrMatrix = **ppaulRGBToYCbCr;
	}
	else
	{
		*paulClrMatrix = s_Identity;
	}

	return BERR_SUCCESS;
}


/****************************************************************
 *  GFD HSCL coefficients
 ****************************************************************/

static const uint32_t s_aulFirCoeff_PointSample[] =
{
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x0000 ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_02,    COEFF_2, 0x0100 ),

	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_1, 0x0000 ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_02,    COEFF_2, 0x0000 ),
};

static const uint32_t s_aulFirCoeff_Bilinear[] =
{
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x0000 ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_02,    COEFF_2, 0x0100 ),

	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_1, 0x0000 ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_02,    COEFF_2, 0x0080 ),
};

static const uint32_t s_aulFirCoeff_Sharp_1toN[] =
{
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x0000 ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_02,    COEFF_2, 0x0100 ),

	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_0, 0x000e ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_1, 0xffd7 ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_02,    COEFF_2, 0x009b ),
};

static const uint32_t s_aulFirCoeff_Smooth_1toN[] =
{
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x0000 ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_02,    COEFF_2, 0x0100 ),

	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_0, 0x0004 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_1, 0xffe5 ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_02,    COEFF_2, 0x0097 ),
};

static const uint32_t s_aulFirCoeff_Sharp_16to9[] =
{
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_0, 0xfff0 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x0044 ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_02,    COEFF_2, 0x0098 ),

	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_0, 0xfff0 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_1, 0x000c ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_02,    COEFF_2, 0x0084 ),
};

static const uint32_t s_aulFirCoeff_Smooth_16to9[] =
{
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_0, 0xfffc ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x003d ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_02,    COEFF_2, 0x008e ),

	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_0, 0xfffc ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_1, 0x000e ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_02,    COEFF_2, 0x0076 ),
};

static const uint32_t s_aulFirCoeff_Sharp_3to1[] =
{
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_0, 0x0017 ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x003f ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_02,    COEFF_2, 0x0054 ),

	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_0, 0x000a ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_1, 0x002a ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_02,    COEFF_2, 0x004c ),
};

static const uint32_t s_aulFirCoeff_Smooth_3to1[] =
{
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_0, 0x001d ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x003d ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE0_02,    COEFF_2, 0x004c ),

	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_0, 0x000e ) |
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_00_01, COEFF_1, 0x002c ),
	BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_COEFF_PHASE1_02,    COEFF_2, 0x0046 ),
};


#define BVDC_P_GFX_FIRCOEFF_IDX_START (101)
#define BVDC_P_GFX_FIRCOEFF_IDX_END   (126)

/****************************************************************
 *  Indexed GFD HSCL coefficients
 ****************************************************************/
static const uint32_t s_paulFirCoeffTbl[][4] =
{
	{ 0x01000400, 0x06000000, 0x00400260, 0x05600000 },  /* 1 */
	{ 0x00B00400, 0x06A00000, 0x00200210, 0x05D00000 },  /* 2 */
	{ 0x00800400, 0x07000000, 0x000001E0, 0x06200000 },  /* 3 */
	{ 0x00500400, 0x07600000, 0x7FF001A0, 0x06700000 },  /* 4 */
	{ 0x002003F0, 0x07E00000, 0x7FE00160, 0x06C00000 },  /* 5 */
	{ 0x7FF003E0, 0x08600000, 0x7FE00120, 0x07000000 },  /* 6 */
	{ 0x7FC003C0, 0x09000000, 0x7FE000D0, 0x07500000 },  /* 7 */
	{ 0x7FA003A0, 0x09800000, 0x7FD00090, 0x07A00000 },  /* 8 */
	{ 0x7F800370, 0x0A200000, 0x7FE00040, 0x07E00000 },  /* 9 */
	{ 0x7F700330, 0x0AC00000, 0x7FE07FF0, 0x08300000 },  /* 10 */
	{ 0x7F6002F0, 0x0B600000, 0x7FE07FA0, 0x08800000 },  /* 11 */
	{ 0x7F6002A0, 0x0C000000, 0x7FF07F50, 0x08C00000 },  /* 12 */
	{ 0x7F600240, 0x0CC00000, 0x00007F00, 0x09000000 },  /* 13 */
	{ 0x7F7001D0, 0x0D800000, 0x00107ED0, 0x09200000 },  /* 14 */
	{ 0x7F900170, 0x0E000000, 0x00107E90, 0x09600000 },  /* 15 */
	{ 0x7FB000F0, 0x0EC00000, 0x00207E70, 0x09700000 },  /* 16 */
	{ 0x7FD00090, 0x0F400000, 0x00207E60, 0x09800000 },  /* 17 */
	{ 0x00000010, 0x0FE00000, 0x00307E50, 0x09800000 },  /* 18 */
	{ 0x00207FA0, 0x10800000, 0x00307E50, 0x09800000 },  /* 19 */
	{ 0x00507F10, 0x11400000, 0x00207E70, 0x09700000 },  /* 20 */
	{ 0x00707EB0, 0x11C00000, 0x00207E90, 0x09500000 },  /* 21 */
	{ 0x00907E40, 0x12600000, 0x00107EC0, 0x09300000 },  /* 22 */
	{ 0x00A07DE0, 0x13000000, 0x00007EF0, 0x09100000 },  /* 23 */
	{ 0x00A07D70, 0x13E00000, 0x7FF07F40, 0x08D00000 },  /* 24 */
	{ 0x00A07D20, 0x14800000, 0x7FF07F90, 0x08800000 },  /* 25 */
	{ 0x00A07CD0, 0x15200000, 0x7FE07FE0, 0x08400000 },  /* 26 */
};

/*--------------------------------------------------------------------*/
static uint32_t *BVDC_P_GetFilterCoefficients_PointSample(
	uint32_t ulSrcSize,
	uint32_t ulOutSize )
{
	BSTD_UNUSED(ulSrcSize);
	BSTD_UNUSED(ulOutSize);
	return (uint32_t *) s_aulFirCoeff_PointSample;
}

/*--------------------------------------------------------------------*/
static uint32_t *BVDC_P_GetFilterCoefficients_Bilinear(
	uint32_t ulSrcSize,
	uint32_t ulOutSize )
{
	BSTD_UNUSED(ulSrcSize);
	BSTD_UNUSED(ulOutSize);
	return (uint32_t *) s_aulFirCoeff_Bilinear;
}

/*--------------------------------------------------------------------*/
static uint32_t *BVDC_P_GetFilterCoefficients_Sharp(
	uint32_t ulSrcSize,
	uint32_t ulOutSize )
{
	if( ulOutSize == ulSrcSize )
		return (uint32_t *) s_aulFirCoeff_PointSample;
	else if( ulOutSize >= ulSrcSize )
		return (uint32_t *) s_aulFirCoeff_Sharp_1toN;
	else if( ulOutSize * 12 >= ulSrcSize * 16 )  /* rounding to 9/16 */
		return (uint32_t *) s_aulFirCoeff_Sharp_16to9;
	else
		return (uint32_t *) s_aulFirCoeff_Sharp_3to1;
}

/*--------------------------------------------------------------------*/
static uint32_t *BVDC_P_GetFilterCoefficients_Smooth(
	uint32_t ulSrcSize,
	uint32_t ulOutSize )
{
	if( ulOutSize == ulSrcSize )
		return (uint32_t *) s_aulFirCoeff_PointSample;
	else if( ulOutSize >= ulSrcSize )
		return (uint32_t *) s_aulFirCoeff_Smooth_1toN;
	else if( ulOutSize * 12 >= ulSrcSize * 16 )  /* rounding to 9/16 */
		return (uint32_t *) s_aulFirCoeff_Smooth_16to9;
	else
		return (uint32_t *) s_aulFirCoeff_Smooth_3to1;
}

/*--------------------------------------------------------------------
 * {private}
 * Description:
 * This is the private type for coefficient functions.
*--------------------------------------------------------------------*/
typedef uint32_t *(* BVDC_P_GetFilterCoefFunc)( uint32_t ulSrcSize, uint32_t ulOutSize );

/*--------------------------------------------------------------------
 * {private}
 * Note: The order has to match the def of BVDC_FilterCoeffs
 *-------------------------------------------------------------------*/
static const BVDC_P_GetFilterCoefFunc s_pfnGetFilterCoefficients[] =
{
	BVDC_P_GetFilterCoefficients_PointSample,
	BVDC_P_GetFilterCoefficients_Bilinear,
	BVDC_P_GetFilterCoefficients_Smooth,
	BVDC_P_GetFilterCoefficients_Sharp
};

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_DecideFilterCoeff_isr
 *
 * output: Hscl filter coeff
 */
BERR_Code BVDC_P_GfxFeeder_DecideFilterCoeff_isr
	( BVDC_FilterCoeffs     eCoeffs,
	  uint32_t              ulCtIndex,
	  uint32_t              ulSrcSize,
	  uint32_t              ulOutSize,
	  uint32_t **           paulCoeff )
{
	if ((eCoeffs <= BVDC_FilterCoeffs_eSharp) && (NULL != paulCoeff))
	{
		if (eCoeffs == BVDC_FilterCoeffs_eSharp && ulCtIndex != 0)
		{
			if (ulCtIndex < BVDC_P_GFX_FIRCOEFF_IDX_START || ulCtIndex > BVDC_P_GFX_FIRCOEFF_IDX_END)
			{
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}

			*paulCoeff = (uint32_t *)s_paulFirCoeffTbl[ulCtIndex - BVDC_P_GFX_FIRCOEFF_IDX_START];
		}
		else
		{
			*paulCoeff = (* s_pfnGetFilterCoefficients[eCoeffs])(ulSrcSize, ulOutSize);
		}
		return BERR_SUCCESS;
	}
	else
	{
		BDBG_ASSERT( NULL != paulCoeff );
		BDBG_ASSERT( eCoeffs <= BVDC_FilterCoeffs_eSharp );
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
}

#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
/****************************************************************
 *  GFD VSCL coefficients
 ****************************************************************/

static const uint32_t s_aulVsclFirCoeff_PointSample[] =
{
	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE0_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x0100 ),

	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE1_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE1_00_01, COEFF_1, 0x0000 ),
	BVDC_P_SCL_LAST
};

static const uint32_t s_aulVsclFirCoeff_Bilinear[] =
{
	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE0_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x0100 ),

	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE1_00_01, COEFF_0, 0x0000 ) |
	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE1_00_01, COEFF_1, 0x0080 ),
	BVDC_P_SCL_LAST
};

static const uint32_t s_aulVsclFirCoeff_Sharp_1toN[] =
{
	0x09200370, 0x076000A0,
	BVDC_P_SCL_LAST
};

static const uint32_t s_aulVsclFirCoeff_Sharp_16to9[] =
{
	0x08E00390, 0x075000B0,
	BVDC_P_SCL_LAST
};

static const uint32_t s_aulVsclFirCoeff_Sharp_3to1[] =
{
	0x08E00390, 0x075000B0,
	BVDC_P_SCL_LAST
};


static const uint32_t s_aulVsclFirCoeff_Smooth_1toN[] =
{
	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE0_00_01, COEFF_0, 0x0037 ) |
	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE0_00_01, COEFF_1, 0x0092 ),

	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE1_00_01, COEFF_0, 0x0009 ) |
	BCHP_FIELD_DATA( GFD_0_VERT_FIR_COEFF_PHASE1_00_01, COEFF_1, 0x0077 ),
	BVDC_P_SCL_LAST
};

static const uint32_t s_aulVsclFirCoeff_Smooth_16to9[] =
{
	0x05800540, 0x042003E0,
	BVDC_P_SCL_LAST
};

static const uint32_t s_aulVsclFirCoeff_Smooth_3to1[] =
{
	0x05600550, 0x041003F0,
	BVDC_P_SCL_LAST
};


/****************************************************************
 *  Indexed GFD VSCL coefficients
 ****************************************************************/
static const uint32_t s_paulVsclFirCoeffTbl[][4] =
{
	{ 0x0A000300, 0x07A00060 },  /* 1 */
	{ 0x0A6002D0, 0x07B00050 },  /* 2 */
	{ 0x0AA002B0, 0x07C00040 },  /* 3 */
	{ 0x0B000280, 0x07D00030 },  /* 4 */
	{ 0x0B400260, 0x07E00020 },  /* 5 */
	{ 0x0BA00230, 0x07F00010 },  /* 6 */
	{ 0x0C000200, 0x08000000 },  /* 7 */
	{ 0x0C6001D0, 0x08107FF0 },  /* 8 */
	{ 0x0CE00190, 0x08207FE0 },  /* 9 */
	{ 0x0D600150, 0x08307FD0 },  /* 10 */
	{ 0x0DE00110, 0x08307FD0 },  /* 11 */
	{ 0x0E6000D0, 0x08407FC0 },  /* 12 */
	{ 0x0EE00090, 0x08407FC0 },  /* 13 */
	{ 0x0F600050, 0x08407FC0 },  /* 14 */
	{ 0x10000000, 0x08407FC0 },  /* 15 */
	{ 0x10607FD0, 0x08407FC0 },  /* 16 */
	{ 0x11007F80, 0x08407FC0 },  /* 17 */
	{ 0x11607F50, 0x08407FC0 },  /* 18 */
	{ 0x11E07F10, 0x08307FD0 },  /* 19 */
	{ 0x12407EE0, 0x08307FD0 },  /* 20 */
	{ 0x12C07EA0, 0x08207FE0 },  /* 21 */
	{ 0x13007E80, 0x08107FF0 },  /* 22 */
	{ 0x13407E60, 0x08000000 },  /* 23 */
	{ 0x13607E50, 0x08000000 },  /* 24 */
	{ 0x13807E40, 0x07F00010 },  /* 25 */
	{ 0x13807E40, 0x07E00020 },  /* 26 */
};


/*--------------------------------------------------------------------*/
static uint32_t *BVDC_P_GetVsclFilterCoefficients_PointSample(
	uint32_t ulSrcSize,
	uint32_t ulOutSize )
{
	BSTD_UNUSED(ulSrcSize);
	BSTD_UNUSED(ulOutSize);
	return (uint32_t *) s_aulVsclFirCoeff_PointSample;
}

/*--------------------------------------------------------------------*/
static uint32_t *BVDC_P_GetVsclFilterCoefficients_Bilinear(
	uint32_t ulSrcSize,
	uint32_t ulOutSize )
{
	BSTD_UNUSED(ulSrcSize);
	BSTD_UNUSED(ulOutSize);
	return (uint32_t *) s_aulVsclFirCoeff_Bilinear;
}

/*--------------------------------------------------------------------*/
static uint32_t *BVDC_P_GetVsclFilterCoefficients_Sharp(
	uint32_t ulSrcSize,
	uint32_t ulOutSize )
{
	if( ulOutSize == ulSrcSize )
		return (uint32_t *) s_aulVsclFirCoeff_PointSample;
	else if( ulOutSize >= ulSrcSize )
		return (uint32_t *) s_aulVsclFirCoeff_Sharp_1toN;
	else if( ulOutSize * 12 >= ulSrcSize * 16 )  /* rounding to 9/16 */
		return (uint32_t *) s_aulVsclFirCoeff_Sharp_16to9;
	else
		return (uint32_t *) s_aulVsclFirCoeff_Sharp_3to1;
}

/*--------------------------------------------------------------------*/
static uint32_t *BVDC_P_GetVsclFilterCoefficients_Smooth(
	uint32_t ulSrcSize,
	uint32_t ulOutSize )
{
	if( ulOutSize == ulSrcSize )
		return (uint32_t *) s_aulVsclFirCoeff_PointSample;
	else if( ulOutSize >= ulSrcSize )
		return (uint32_t *) s_aulVsclFirCoeff_Smooth_1toN;
	else if( ulOutSize * 12 >= ulSrcSize * 16 )  /* rounding to 9/16 */
		return (uint32_t *) s_aulVsclFirCoeff_Smooth_16to9;
	else
		return (uint32_t *) s_aulVsclFirCoeff_Smooth_3to1;
}

/*--------------------------------------------------------------------
 * {private}
 * Description:
 * This is the private type for coefficient functions.
*--------------------------------------------------------------------*/
typedef uint32_t *(* BVDC_P_GetVsclFilterCoefFunc)( uint32_t ulSrcSize, uint32_t ulOutSize );

/*--------------------------------------------------------------------
 * {private}
 * Note: The order has to match the def of BVDC_FilterCoeffs
 *-------------------------------------------------------------------*/
static const BVDC_P_GetVsclFilterCoefFunc s_pfnGetVsclFirCoefficients[] =
{
	BVDC_P_GetVsclFilterCoefficients_PointSample,
	BVDC_P_GetVsclFilterCoefficients_Bilinear,
	BVDC_P_GetVsclFilterCoefficients_Smooth,
	BVDC_P_GetVsclFilterCoefficients_Sharp
};

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_DecideVsclFirCoeff_isr
 *
 * output: filter coeff
 *
 * Note: This implementation is originally copied from bgrc, we should watch
 * bgrc's update and update this code accordingly.
 */
BERR_Code BVDC_P_GfxFeeder_DecideVsclFirCoeff_isr
	( BVDC_FilterCoeffs     eCoeffs,
	  uint32_t              ulCtIndex,
	  uint32_t              ulSrcSize,
	  uint32_t              ulOutSize,
	  uint32_t **           paulCoeff )
{
	if ((eCoeffs <= BVDC_FilterCoeffs_eSharp) && (NULL != paulCoeff))
	{
		if (eCoeffs == BVDC_FilterCoeffs_eSharp && ulCtIndex != 0)
		{
			if (ulCtIndex < BVDC_P_GFX_FIRCOEFF_IDX_START || ulCtIndex > BVDC_P_GFX_FIRCOEFF_IDX_END)
			{
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}

			*paulCoeff = (uint32_t *)s_paulVsclFirCoeffTbl[ulCtIndex - BVDC_P_GFX_FIRCOEFF_IDX_START];
		}
		else
		{
			*paulCoeff = (* s_pfnGetVsclFirCoefficients[eCoeffs])(ulSrcSize, ulOutSize);
		}
		return BERR_SUCCESS;
	}
	else
	{
		BDBG_ASSERT( NULL != paulCoeff );
		BDBG_ASSERT( eCoeffs <= BVDC_FilterCoeffs_eSharp );
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
}
#endif  /* #if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3) */

/* End of File */
