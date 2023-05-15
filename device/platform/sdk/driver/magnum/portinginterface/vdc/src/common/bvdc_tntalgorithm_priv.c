/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_tntalgorithm_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 4/19/10 10:22p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_tntalgorithm_priv.c $
 * 
 * Hydra_Software_Devel/17   4/19/10 10:22p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/16   4/7/10 11:35a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/15   4/5/10 4:15p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/14   12/23/09 2:03p tdo
 * SW7408-26: Add new TNT support
 *
 * Hydra_Software_Devel/13   6/29/09 2:41p jessem
 * PR 56434: Have ED formats (480p and 576p) use SD sharpness settings.
 *
 * Hydra_Software_Devel/12   1/8/09 3:51p jessem
 * PR 50316: Added stringent conditional for interpolating sharpness.
 *
 * Hydra_Software_Devel/11   12/15/08 11:07a jessem
 * PR 50316: Updated TNT sharpness  interpolation tables.
 *
 * Hydra_Software_Devel/10   11/20/08 11:20a jessem
 * PR 48529: Changed BDBG_ERR to BDBG_MSG.
 *
 * Hydra_Software_Devel/9   11/20/08 8:58a jessem
 * PR 48529: Updated sharpness interpolation tables.
 *
 * Hydra_Software_Devel/8   11/18/08 2:48p jessem
 * PR 48529: Revised interpolation of sharpness settings based on slider-
 * bar mechanism.
 *
 * Hydra_Software_Devel/7   10/18/08 12:07p jessem
 * PR 46489: Fixed bug in programming interpolated sharpness values.
 *
 * Hydra_Software_Devel/6   10/16/08 4:22p jessem
 * PR 46489: Reorganized luma peaking sharpness equation to avoid losing
 * the siginifcant digit due to unsigned long arithmetic.
 *
 * Hydra_Software_Devel/5   10/16/08 2:57p jessem
 * PR 46489: Corrected equation to calculate luma peakig gains for
 * interpolated sharpness.
 *
 * Hydra_Software_Devel/4   10/15/08 2:14p jessem
 * PR 46489: Added check to new Sharpness setting when interpolating
 * sharpness.
 *
 * Hydra_Software_Devel/3   10/15/08 1:24p jessem
 * PR 46489: Fixed bug in storing interpolated sharpness. Changed
 * BVDC_P_Tnt_StoreInterpolatedSharpnessValues to
 * BVDC_P_Tnt_InterpolateSharpness.
 *
 * Hydra_Software_Devel/2   10/7/08 11:10a jessem
 * PR 46489: Changed memcpy to simple assignment.
 *
 * Hydra_Software_Devel/1   10/7/08 9:52a jessem
 * PR 47304: Added TNT interpolation table and private functions.
 *
 ***************************************************************************/
#include "bvdc_compositor_priv.h"
#include "bvdc_tnt.h"
#include "bvdc_tnt_priv.h"
#include "bvdc_source_priv.h"

BDBG_MODULE(BVDC_TNT);

#if (BVDC_P_SUPPORT_TNT_VER == 5)            /* TNT2 HW base */
#define BVDC_P_TNT_VAL_MIN                 INT16_MIN
#define BVDC_P_TNT_VAL_MAX                 INT16_MAX
#define BVDC_P_TNT_VAL_MID_1               INT16_MIN + (INT16_MAX - INT16_MIN)/3
#define BVDC_P_TNT_VAL_MID_2               INT16_MAX - (INT16_MAX - INT16_MIN)/3

#define BVDC_P_TNT_MAX_INDEX               2

/***************************************************************************
 * {private}
 * This section is for TNT algorithms
 */

static BVDC_SharpnessSettings astSharpness[BVDC_P_TNT_MAX_INDEX] =
{
	{/* SD sharpness */
		/* Luma Peaking */
		{
			{{0x0, 0x0},
			 {0x20, 0x20},{0x20, 0x20}, {0x0, 0x0}, {0x20, 0x20}, {0x20, 0x20}, {0x20, 0x20}, {0x20, 0x20}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}},
			0x0,
			{0x5, 0x5, 0x5},
			0x0,
			{{0x70, 0x70, 0x78, 0x0}, {0xc8, 0x190}},
			{{0x0, 0x0, 0x0, 0x0}, {0xc8, 0x190}},
			0x0,
			{0x2, 0x384},
			{0x2, 0x32}
		},
		/* LTI */
		{
			0x0,
			{0x0, 0x0, 0x0, 0x0},
			{0xc8, 0x190},
			0x10, 0x1, 0x1, 0x2, 0x8, 0x8, 0x4, 0x0
		},
		/* CTI */
			{0x10, 0x10, 0x1},
			{0x10, 0x10, 0x1},
		/* Chroma-based filtering */
		0x1,
		{
			{
				0x1, 0x12a, 0xb5, 0x90, 0x2d, 0x3,
				{
					{0x2, {{0x270, 0x40}, {0x2d5, 0x40}, {0x373, 0x1a}, {0x3dd, 0x0}}},
					{0x3, {{0xbd, 0x0}, {0xce, 0x40}, {0x114, 0x1a}, {0x127, 0x0}}},
					{0x4, {{0x39, 0x0}, {0x3f, 0x40}, {0x7a, 0x40}, {0x88, 0x0}}}
				},
				0x1, 0x1, 0x0, 0xfffffff0, 0x10
			},
			{
				0x1, 0x1fe, 0x15e, 0x1f6, 0xa, 0x3,
				{
					{0x2, {{0x53, 0x0}, {0xd2, 0x40}, {0x373, 0x40}, {0x3d5, 0x0}}},
					{0x3, {{0x15f, 0x0}, {0x160, 0x40}, {0x161, 0x40}, {0x1da, 0x0}}},
					{0x4, {{0x16, 0x0}, {0xa6, 0x2d}, {0x176, 0x40}, {0x1e1, 0x0}}}
				},
				0x1, 0x1, 0x0, 0x10, 0xfffffff0
			},
			{
				0x1, 0x2ec, 0x253, 0x1fa, 0x12, 0x3,
				{
					{0x2, {{0x6a, 0x40}, {0x1eb, 0x40}, {0x314, 0x40}, {0x353, 0x0}}},
					{0x3, {{0x254, 0x0}, {0x255, 0x40}, {0x256, 0x20}, {0x257, 0x0}}},
					{0x4, {{0x2d, 0x26}, {0x54, 0x40}, {0x175, 0x26}, {0x1ed, 0x0}}}
				},
				0x1, 0x1, 0x0, 0x0, 0x0
			},
			{
				0x1, 0x12a, 0xbc, 0x209, 0x93, 0x3,
				{
					{0x2, {{0x5d, 0x40}, {0x155, 0x40}, {0x23a, 0x40}, {0x2ac, 0x0}}},
					{0x3, {{0xbd, 0x0}, {0xca, 0x40}, {0xeb, 0x40}, {0x112, 0xffffffff}}},
					{0x4, {{0xa4, 0x0}, {0xfa, 0x40}, {0x1bd, 0x29}, {0x1f3, 0x0}}}
				},
				0x1, 0x1, 0x0, 0xa, 0x0
			}
		},
		/* Luma Histogram */
		0x0
	},

	{/* HD sharpness */
		/* Luma Peaking */
		{
			{{0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x10, 0x10}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x18, 0x18}, {0x18, 0x18}, {0x18, 0x18}, {0x18, 0x18}, {0x18, 0x18}, {0x18, 0x18}},
			0x0,
			{0x8, 0x8, 0x8},
			0x0,
			{{0x0, 0x0, 0x0, 0x0}, {0xc8, 0x190}},
			{{0x70, 0x70, 0x78, 0x0}, {0xc8, 0x190}},
			0x0,
			{0x2, 0x384},
			{0x2, 0x32}
		},
		/* LTI */
		{
			0x0,
			{0x0, 0x0, 0x0, 0x0},
			{0xc8, 0x190},
			0x10, 0x1, 0x1, 0x2, 0x0, 0x0, 0x4, 0x0
		},
		/* CTI */
			{0x10, 0x10, 0x1},
			{0x10, 0x10, 0x1},
		/* Chroma-based filtering */
		0x1,
		{
			{
				0x1, 0x12a, 0xb5, 0x90, 0x2d, 0x3,
				{
					{0x2, {{0x270, 0x40}, {0x2d5, 0x40}, {0x373, 0x1a}, {0x3dd, 0x0}}},
					{0x3, {{0xbd, 0x0}, {0xce, 0x40}, {0x114, 0x1a}, {0x127, 0x0}}},
					{0x4, {{0x39, 0xffffffff}, {0x3f, 0x40}, {0x7a, 0x40}, {0x88, 0x0}}}
				},
				0x1, 0x1, 0x0, 0xfffffff0, 0x10
			},
			{
				0x1, 0x1fe, 0x15e, 0x1f6, 0xa, 0x3,
				{
					{0x2, {{0x53, 0x0}, {0xd2, 0x40}, {0x373, 0x40}, {0x3d5, 0x0}}},
					{0x3, {{0x15f, 0x0}, {0x160, 0x40}, {0x161, 0x40}, {0x1da, 0x0}}},
					{0x4, {{0x16, 0x0}, {0xa6, 0x2d}, {0x176, 0x40}, {0x1e1, 0x0}}}
				},
				0x1, 0x1, 0x0, 0x10, 0xfffffff0
			},
			{
				0x1, 0x2ec, 0x253, 0x1fa, 0x12, 0x3,
				{
					{0x2, {{0x6a, 0x40}, {0x1eb, 0x40}, {0x314, 0x40}, {0x353, 0x0}}},
					{0x3, {{0x254, 0x0}, {0x255, 0x40}, {0x256, 0x20}, {0x257, 0x0}}},
					{0x4, {{0x2d, 0x26}, {0x54, 0x40}, {0x175, 0x26}, {0x1ed, 0x0}}}
				},
				0x1, 0x1, 0x0, 0x0, 0x0
			},
			{
				0x1, 0x12a, 0xbc, 0x209, 0x93, 0x3,
				{
					{0x2, {{0x5d, 0x40}, {0x155, 0x40}, {0x23a, 0x40}, {0x2ac, 0x0}}},
					{0x3, {{0xbd, 0x0}, {0xca, 0x40}, {0xeb, 0x40}, {0x112, 0xffffffff}}},
					{0x4, {{0xa4, 0x0}, {0xfa, 0x40}, {0x1bd, 0x29}, {0x1f3, 0x0}}}
				},
				0x1, 0x1, 0x0, 0xa, 0x0
			}
		},
		/* Luma Histogram */
		0x0
	}

};

/*************************************************************************
 *  {secret}
 *	BVDC_P_Tnt_Calculate_LumaPeakingGain
 *	Calculate Luma peaking gain values from sSharpness value
 *
 **************************************************************************/
static void BVDC_P_Tnt_Calculate_LumaPeakingGain
	( const int16_t                      sSharpness,
	  uint32_t                           ulTblIndex,
	  BVDC_Sharpness_LumaPeakingGain     aulPeakingGain[BVDC_MAX_LUMA_PEAKING_FREQ_BANDS] )
{
	uint32_t i;

	for (i=0; i<BVDC_MAX_LUMA_PEAKING_FREQ_BANDS; i++)
	{
		aulPeakingGain[i].ulOvershootGain =
				(sSharpness + (BVDC_P_TNT_VAL_MAX + 1)) *
				astSharpness[ulTblIndex].stLumaPeaking.astGain[i].ulOvershootGain/(UINT16_MAX + 1);
		aulPeakingGain[i].ulUndershootGain =
				(sSharpness + (BVDC_P_TNT_VAL_MAX + 1)) *
				astSharpness[ulTblIndex].stLumaPeaking.astGain[i].ulUndershootGain/(UINT16_MAX + 1);
		BDBG_MSG(("Inteprolated peaking gains[%d] = %d,%d",
			i, aulPeakingGain[i].ulUndershootGain, aulPeakingGain[i].ulOvershootGain));
	}

}

BERR_Code BVDC_P_Tnt_InterpolateSharpness
	( BVDC_Window_Handle         hWindow,
	  const int16_t              sSharpness)
{
	BERR_Code                    err = BERR_SUCCESS;
	BVDC_P_Window_Info          *pNewInfo, *pCurInfo;
	BVDC_SharpnessSettings       stSharpnessSettings;
	uint32_t                     ulIndex = 0;

	BDBG_ENTER(BVDC_P_Tnt_InterpolateSharpness);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pNewInfo = &hWindow->stNewInfo;
	pCurInfo = &hWindow->stCurInfo;

	BDBG_OBJECT_ASSERT(pCurInfo->hSource, BVDC_SRC);

	/* Determine if source is SD/ED or HD */
	ulIndex = pCurInfo->hSource->stCurInfo.pVdcFmt->bHd ? 1 : 0;

	stSharpnessSettings = astSharpness[ulIndex];

	/* Luma Peaking Gain */
	BVDC_P_Tnt_Calculate_LumaPeakingGain(sSharpness, ulIndex, stSharpnessSettings.stLumaPeaking.astGain);

	err = BVDC_P_Tnt_ValidateSharpnessSettings(&stSharpnessSettings);
	if (err == BERR_SUCCESS)
	{
		/* set new value */
		pNewInfo->stSharpnessConfig = stSharpnessSettings;

		BVDC_P_Tnt_StoreSharpnessSettings(hWindow, &stSharpnessSettings);
	}

	BDBG_LEAVE(BVDC_P_Tnt_InterpolateSharpness);
	return err;
}

#endif /* BVDC_P_SUPPORT_TNT_VER >= 5 */
