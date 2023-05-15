/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_tnt_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/30 $
 * $brcm_Date: 8/13/12 2:31p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_tnt_priv.c $
 * 
 * Hydra_Software_Devel/30   8/13/12 2:31p tdo
 * SW7425-3679: sync up TNT programming with the programming guide
 *
 * Hydra_Software_Devel/29   8/6/12 3:09p tdo
 * SW7425-3679: sync up TNT programming with the programming guide
 *
 * Hydra_Software_Devel/28   2/23/11 3:51p yuxiaz
 * SW7346-85, SW7422-51: Treat ED formats as SD for TNT settings.
 *
 * Hydra_Software_Devel/27   2/16/11 5:15p tdo
 * SW7346-85, SW7422-51 : Software TN2TH Setting
 *
 * Hydra_Software_Devel/26   11/11/10 7:31p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/25   6/18/10 4:24p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/24   4/19/10 10:22p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/23   4/7/10 11:35a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/22   4/5/10 4:14p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/21   12/23/09 2:03p tdo
 * SW7408-26: Add new TNT support
 *
 * Hydra_Software_Devel/20   12/9/09 12:07p jessem
 * SW3556-868: Enclosed ulRegionId in BDBG_DEBUG_BUILD.
 *
 * Hydra_Software_Devel/19   11/24/09 5:47p yuxiaz
 * SW7420-462: Added new tuning functions for Tuning Database.
 *
 * Hydra_Software_Devel/18   11/11/09 12:58p tdo
 * SW7400-2574: Handling gain calculation based on format width instead of
 * format type
 *
 * Hydra_Software_Devel/17   11/10/09 4:32p tdo
 * SW7400-2574: Adjust TNT based on input and output format
 *
 * Hydra_Software_Devel/16   11/9/09 4:02p tdo
 * SW7400-2574: Update TNT default settings
 *
 * Hydra_Software_Devel/15   11/9/09 3:58p tdo
 * SW7400-2574: Update TNT default settings
 *
 * Hydra_Software_Devel/14   5/13/09 12:01p pntruong
 * PR51108: Another attemp to address coverity.
 *
 * Hydra_Software_Devel/13   5/13/09 9:06a pntruong
 * PR51108: More Coverity Defect ID:13602 OVERRUN_STATIC.
 *
 * Hydra_Software_Devel/12   5/13/09 12:32a pntruong
 * PR51108: Coverity Defect ID:13602 OVERRUN_STATIC.
 *
 * Hydra_Software_Devel/11   11/21/08 11:38a jessem
 * PR 49384: Enhanced error message for checking PWL count per region.
 *
 * Hydra_Software_Devel/10   11/20/08 12:26p jessem
 * PR 49384: Corrected bounds check on the number of chroma sharpness
 * regions.
 *
 * Hydra_Software_Devel/9   10/18/08 12:06p jessem
 * PR 46489: Rearranged programming of top control and fixed RUL
 * programming of CTI filters.
 *
 * Hydra_Software_Devel/8   10/17/08 7:40p jessem
 * PR 46489: Fixed scale factor calculation. Fixed assignment of values
 * into registers (RUL). Fixed enabling of CM3D regions.
 *
 * Hydra_Software_Devel/7   10/15/08 1:23p jessem
 * PR 46489: Fixed bug in storing interpolated sharpness. Changed
 * BVDC_P_Tnt_StoreInterpolatedSharpnessValues to
 * BVDC_P_Tnt_InterpolateSharpness.
 *
 * Hydra_Software_Devel/6   10/7/08 11:10a jessem
 * PR 46489: Changed memcpy to simple assignment.
 *
 * Hydra_Software_Devel/5   9/30/08 12:46p jessem
 * PR 46489: Enclosed #include "bchp_tnt_cmp_0_v0.h"
 * withBVDC_P_SUPPORT_TNT_VER >= 5
 *
 * Hydra_Software_Devel/4   9/30/08 12:34p jessem
 * PR 46489: Re-added TN2T support using tighter compiler directive to
 * compile only when BVDC_P_SUPPORT_TNT_VER >= 5.
 *
 * Hydra_Software_Devel/3   9/29/08 6:38p pntruong
 * PR 46489: Rollbacked all the build errors introduced.  Todo: need to
 * properly factored out the code for tnt and tnt new.
 *
 * Hydra_Software_Devel/1   9/17/08 8:36p pntruong
 * PR47013: B0: Initial vdc support.
 *
 **************************************************************************/
#include "bvdc_compositor_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_pep_priv.h"
#include "bvdc_hist_priv.h"
#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_4)
#include "bchp_vivid_cmp_0.h"
#endif
#if (BVDC_P_SUPPORT_TNT_VER >= 5)
#include "bchp_tnt_cmp_0_v0.h"
#endif
#include "bvdc_tnt.h"
#include "bvdc_tnt_priv.h"

BDBG_MODULE(BVDC_TNT);

#if (BVDC_P_SUPPORT_TNT)
#if (BVDC_P_SUPPORT_TNT_VER == 5)            /* TNT2 HW base */

/* Chroma sharpness regions */
#define BVDC_P_MIN_HUE                              0
#define BVDC_P_MAX_HUE                           1023
#define BVDC_P_MIN_SAT                              0
#define BVDC_P_MAX_SAT                           1023

#define BVDC_P_CHROMA_SHARPNESS_PWL_Y_FBITS         6
#define BVDC_P_CHROMA_SHARPNESS_PWL_X_BIT_MASK  0x3ff

#define BVDC_P_MAX_CORING_THRESHOLD              1023
#define BVDC_P_SCALE_FACTOR_SHIFT                  16
#define BVDC_P_SCALE_FACTOR_DIVISOR             (1 << BVDC_P_SCALE_FACTOR_SHIFT)

/***************************************************************************
 * {private}
 *
 */
static uint32_t BVDC_P_Tnt_CalculateThresholdScaleFactor
	( uint32_t ulIndex,
	  uint32_t aulThreshold[BVDC_MAX_CORING_THRESHOLDS] )
{
	uint32_t ulScaleFactor = 0;
	uint32_t ulDivisor = BVDC_P_SCALE_FACTOR_DIVISOR;

	switch(ulIndex)
	{
		case 0:
			/* divide by zero or qoutient == 1 */
			if ((aulThreshold[0] == 0) || (aulThreshold[0] == 1))
				ulScaleFactor = 0;
			else
			{
				/* round up */
				ulDivisor += aulThreshold[0]/2;
				ulScaleFactor = ulDivisor / (aulThreshold[0]);
			}
			break;
		case 1:
			/* divide by zero or qoutient == 1 */
			if (((aulThreshold[1] - aulThreshold[0]) == 0) || ((aulThreshold[1] - aulThreshold[0]) == 1))
				ulScaleFactor = 0;
			else
			{
				/* round up */
				ulDivisor += (aulThreshold[1] - aulThreshold[0])/2;
				ulScaleFactor = ulDivisor /(aulThreshold[1] - aulThreshold[0]);
			}
			break;
		case 2:
			/* divide by zero or qoutient == 1 */
			if ((aulThreshold[1] == BVDC_P_MAX_CORING_THRESHOLD) ||
				(BVDC_P_MAX_CORING_THRESHOLD - aulThreshold[1]) == 1)
				ulScaleFactor = 0;
			else
			{
				/* round up */
				ulDivisor += (BVDC_P_MAX_CORING_THRESHOLD - aulThreshold[1])/2;
				ulScaleFactor = ulDivisor / (BVDC_P_MAX_CORING_THRESHOLD - aulThreshold[1]);
			}
			break;
	}
	return ulScaleFactor;
}

/***************************************************************************
 * {private}
 *
 */
static BERR_Code BVDC_P_Tnt_ValidateChromaSharpnessRegion
	( const BVDC_ChromaSharpnessRegion         *pRegion,
	  uint32_t                                  ulRegionId )
{
	BERR_Code err = BERR_SUCCESS;
	uint32_t i, j;

	BDBG_ENTER(BVDC_P_Tnt_ValidateChromaSharpnessRegion);

#if (!BDBG_DEBUG_BUILD)
	BSTD_UNUSED(ulRegionId);
#endif

	/* validate number of PWLs */
	if (pRegion->ulPwlCnt > BVDC_MAX_CHROMA_SHARPNESS_PWL)
	{
		BDBG_ERR(("Region %d's number of PWLs (%d) for this region exceeds maximum (%d)",
			ulRegionId, pRegion->ulPwlCnt,  BVDC_MAX_CHROMA_SHARPNESS_PWL));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* validate region's min and max values */
	if (pRegion->ulMinX > pRegion->ulMaxX)
	{
		BDBG_ERR(("Region %d's min X (0x%x) value are greater than max X (0x%x) value!",
			ulRegionId, pRegion->ulMinX, pRegion->ulMaxX));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (pRegion->ulMinY > pRegion->ulMaxY)
	{
		BDBG_ERR(("Region %d's min Y value (0x%x) are greater than max Y (0x%x) value!",
			ulRegionId, pRegion->ulMinY, pRegion->ulMaxY));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* validate region's hue and sat values */
	if (pRegion->eChromaRegionFormat == BVDC_ChromaSharpnessFormat_ePolar)
	{
		if ((pRegion->ulMaxX > BVDC_P_MAX_HUE) || (pRegion->ulMaxY > BVDC_P_MAX_SAT))
		{
			BDBG_ERR(("Region %d's min and max values exceed bounds.", ulRegionId));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}

	/* validate region's PWL points */
	for (i=0; i<pRegion->ulPwlCnt; i++)
	{
		for (j=0; j<BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS; j++)
		{
			/* validate X points */
			if ((j+1) < BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS)
			{
				if ((pRegion->astPwl[i].astPwlPoint[j].ulX & BVDC_P_CHROMA_SHARPNESS_PWL_X_BIT_MASK) >
					 (pRegion->astPwl[i].astPwlPoint[j+1].ulX & BVDC_P_CHROMA_SHARPNESS_PWL_X_BIT_MASK))
				{
					BDBG_ERR(("Region %d's X PWL point %d must be less than or equal to the X PWL point %d",
						ulRegionId, j, j+1));
					return BERR_TRACE(BERR_INVALID_PARAMETER);
				}
			}
		}
	}

	BDBG_LEAVE(BVDC_P_Tnt_ValidateChromaSharpnessRegion);
	return err;
}

/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Tnt_StoreChromaSharpnessRegionInfo
	( BVDC_Window_Handle                  hWindow,
	  const BVDC_ChromaSharpnessRegion   *pstRegion,
	  uint32_t                            ulRegionId  )
{
	BVDC_P_Window_Info     *pNewInfo;
	uint32_t                i, j;
	uint32_t                ulRegData, ulPwlRegData = 0;

	BDBG_ENTER(BVDC_P_Tnt_StoreChromaSharpnessRegionInfo);

	if(ulRegionId >= BVDC_MAX_CHROMA_SHARPNESS_REGIONS)
	{
		BDBG_ASSERT(0);
		return;
	}

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pNewInfo = &hWindow->stNewInfo;

	/* Setup region cfg */
	/* X coordinate */
	ulRegData = BCHP_FIELD_DATA(TNT_CMP_0_V0_REGION0_X_CFG, PR_SEL, pstRegion->eChromaRegionFormat) |
				(BCHP_MASK(TNT_CMP_0_V0_REGION0_X_CFG, X_MAX) & BCHP_FIELD_DATA(TNT_CMP_0_V0_REGION0_X_CFG, X_MAX, pstRegion->ulMaxX)) |
				(BCHP_MASK(TNT_CMP_0_V0_REGION0_X_CFG, X_MIN) & BCHP_FIELD_DATA(TNT_CMP_0_V0_REGION0_X_CFG, X_MIN, pstRegion->ulMinX));

	pNewInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulRegionConfig[ulRegionId * BVDC_P_CHROMA_SHARPNESS_FORMAT_POINTS] = ulRegData;

	/* Y coordinate */
	ulRegData = BCHP_FIELD_DATA(TNT_CMP_0_V0_REGION0_Y_CFG, ENABLE, pstRegion->bEnable) |
				(BCHP_MASK(TNT_CMP_0_V0_REGION0_Y_CFG, Y_MAX) & BCHP_FIELD_DATA(TNT_CMP_0_V0_REGION0_Y_CFG, Y_MAX, pstRegion->ulMaxY)) |
				(BCHP_MASK(TNT_CMP_0_V0_REGION0_Y_CFG, Y_MIN) & BCHP_FIELD_DATA(TNT_CMP_0_V0_REGION0_Y_CFG, Y_MIN, pstRegion->ulMinY));

	pNewInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulRegionConfig[(ulRegionId * BVDC_P_CHROMA_SHARPNESS_FORMAT_POINTS) + 1] = ulRegData;

	/* setup PWLs */
	for (i=0; i<pstRegion->ulPwlCnt; i++)
	{
		uint32_t ulRegionIndex = ulRegionId *  BVDC_MAX_CHROMA_SHARPNESS_PWL * BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS;

		/* Calculate and store the slope per PWL point. Note that there are
		 * BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS - 1 valid slopes. */
		for (j=0; j<BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS; j++)
		{
			uint32_t ulX0;
			int32_t ilY0;
			uint32_t ulPwlIndex;

			ulX0 = pstRegion->astPwl[i].astPwlPoint[j].ulX;
			ilY0 = pstRegion->astPwl[i].astPwlPoint[j].ilY;

			ulRegData = (BCHP_MASK(TNT_CMP_0_V0_PWL0_CFGi, PWL_Y) & BCHP_FIELD_DATA(TNT_CMP_0_V0_PWL0_CFGi, PWL_Y, ilY0)) |
						(BCHP_MASK(TNT_CMP_0_V0_PWL0_CFGi, PWL_X) & BCHP_FIELD_DATA(TNT_CMP_0_V0_PWL0_CFGi, PWL_X, ulX0));

			ulPwlIndex = ulRegionIndex + (i * BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS) + j;
			pNewInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulPwl[ulPwlIndex] = ulRegData;
		}

		/* setup PWL inputs */
		switch (i)
		{
			case 0:
				ulPwlRegData |= BCHP_FIELD_DATA(TNT_CMP_0_V0_PWL_IN_SELi, PWL0_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
			case 1:
				ulPwlRegData |= BCHP_FIELD_DATA(TNT_CMP_0_V0_PWL_IN_SELi, PWL1_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
			case 2:
				ulPwlRegData |= BCHP_FIELD_DATA(TNT_CMP_0_V0_PWL_IN_SELi, PWL2_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
		}
	}

	/* store PWL input info */
	pNewInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulPwlInput[ulRegionId] = ulPwlRegData;

	/* setup gain adjustment */
	ulRegData = BCHP_MASK(TNT_CMP_0_V0_ADJ_RSLT0_CFGi, PROD_SEL) & BCHP_FIELD_DATA(TNT_CMP_0_V0_ADJ_RSLT0_CFGi, PROD_SEL, pstRegion->eAlgo);

	/* store gain adjust info */
	pNewInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulGainAdj[ulRegionId] = ulRegData;

	/* setup Chroma Region Gain/Coring Correction Offsets */
	ulRegData = (BCHP_MASK(TNT_CMP_0_V0_COLOR_OFFSETi, LTI_GAIN_OFFSET) & BCHP_FIELD_DATA(TNT_CMP_0_V0_COLOR_OFFSETi, LTI_GAIN_OFFSET, pstRegion->ilLtiGainOffset)) |
				(BCHP_MASK(TNT_CMP_0_V0_COLOR_OFFSETi, PEAK_GAIN_OFFSET) & BCHP_FIELD_DATA(TNT_CMP_0_V0_COLOR_OFFSETi, PEAK_GAIN_OFFSET, pstRegion->ilPeakGainOffset)) |
				(BCHP_MASK(TNT_CMP_0_V0_COLOR_OFFSETi, PEAK_CORE_OFFSET) & BCHP_FIELD_DATA(TNT_CMP_0_V0_COLOR_OFFSETi, PEAK_CORE_OFFSET, pstRegion->ilPeakCoreOffset));

	/* store Chroma Region Gain/Coring Correction Offsets */
	pNewInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulColorOffset[ulRegionId] = ulRegData;

	/* store status of chroma region */
	pNewInfo->stSharpnessPrivData.abChromaSharpnessRegionEnable[ulRegionId] = (pstRegion->bEnable) ? true : false;


	pNewInfo->stSharpnessPrivData.bChromaRegionCorrectionEnable = pNewInfo->stSharpnessConfig.bChromaSharpnessEnable;

	BDBG_LEAVE(BVDC_P_Tnt_StoreChromaSharpnessRegionInfo);

	return;
}

/*************************************************************************
 *	{secret}
 *	BVDC_P_Tnt_BuildChromaSharpnessRegionRul_isr
 *	Builds TNT Chroma Sharpness Region
 **************************************************************************/
static void BVDC_P_Tnt_BuildChromaSharpnessRegionRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BVDC_P_Window_Info           *pCurInfo;
	uint32_t                      i, j;

	BDBG_ENTER(BVDC_P_Tnt_BuildChromaSharpnessRegionRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pCurInfo = &hWindow->stCurInfo;

	/* Disable all regions */
	for (i=0; i< BVDC_MAX_CHROMA_SHARPNESS_REGIONS; i++)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_REGION0_Y_CFG + (i * sizeof(uint32_t) *
											BVDC_P_CHROMA_SHARPNESS_FORMAT_POINTS));
		*pList->pulCurrent++ = BCHP_FIELD_DATA(TNT_CMP_0_V0_REGION0_Y_CFG, ENABLE, 0);
	}

	/* Configure PWLs for all desired regions */
	for (i=0; i<BVDC_MAX_CHROMA_SHARPNESS_REGIONS; i++)
	{
		/* check if region is to be enabled and if so include region in RUL */
		if (pCurInfo->stSharpnessPrivData.abChromaSharpnessRegionEnable[i] == true)
		{
			/* See PWL register arrangement */
			for (j=0; j<pCurInfo->stSharpnessConfig.astChromaRegion[i].ulPwlCnt; j++)
			{
				uint32_t ulPwlTableIndex;
				uint32_t ulRegionBase = i * BVDC_MAX_CHROMA_SHARPNESS_PWL * BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS;
				uint32_t ulPwlRegBase = (j * BVDC_MAX_CHROMA_SHARPNESS_REGIONS + i) * BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS;

				*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS);
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_PWL0_CFGi_ARRAY_BASE +
										(ulPwlRegBase * sizeof(uint32_t)));

				ulPwlTableIndex = ulRegionBase + (j * BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS);
				BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pCurInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulPwl[ulPwlTableIndex],
						BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS * sizeof(uint32_t));
				pList->pulCurrent += BVDC_MAX_CHROMA_SHARPNESS_PWL_POINTS;
			}

			/* set PWL input */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_PWL_IN_SELi_ARRAY_BASE +
									(i * sizeof(uint32_t)));
			*pList->pulCurrent++ = pCurInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulPwlInput[i];

			/* set gain adj */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_ADJ_RSLT0_CFGi_ARRAY_BASE +
									(i * sizeof(uint32_t)));
			*pList->pulCurrent++ = pCurInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulGainAdj[i];

			/* set Chroma Region Gain/Coring Correction Offsets */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_COLOR_OFFSETi_ARRAY_BASE +
									(i * sizeof(uint32_t)));
			*pList->pulCurrent++ = pCurInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulColorOffset[i];

			/* set region cfg and enable */
			*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CHROMA_SHARPNESS_FORMAT_POINTS);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_REGION0_X_CFG +
				(i * BVDC_P_CHROMA_SHARPNESS_FORMAT_POINTS) * sizeof(uint32_t));

			BKNI_Memcpy((void*)pList->pulCurrent,
						(void*)&pCurInfo->stSharpnessPrivData.stChromaSharpnessRegionConfig.aulRegionConfig[i * BVDC_P_CHROMA_SHARPNESS_FORMAT_POINTS],
						BVDC_P_CHROMA_SHARPNESS_FORMAT_POINTS * sizeof(uint32_t));
			pList->pulCurrent += BVDC_P_CHROMA_SHARPNESS_FORMAT_POINTS;
		}
	}


	BDBG_LEAVE(BVDC_P_Tnt_BuildChromaSharpnessRegionRul_isr);

	return;
}

/*************************************************************************
 *	{secret}
 *	BVDC_P_Tnt_BuildSharpnessRul_isr
 *	Builds Luma Peaking, LTI, CTI
 **************************************************************************/
static void BVDC_P_Tnt_BuildSharpnessRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BVDC_P_Window_Info           *pCurInfo;
	uint32_t                      ulRegData;

	BDBG_ENTER(BVDC_P_Tnt_BuildChromaSharpnessRegionRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pCurInfo = &hWindow->stCurInfo;

	/* LC Align control */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL);
	*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_LC_ALIGN_CONTROL, ALIGN_CR_CB, SEL_CB) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LC_ALIGN_CONTROL, RESET_COUNTER, 0x0);

	/* Luma Peaking Gain */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_MAX_LUMA_PEAKING_FREQ_BANDS);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_GAINSi_ARRAY_BASE);
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pCurInfo->stSharpnessPrivData.aulLumaPeakingGain,
			BVDC_MAX_LUMA_PEAKING_FREQ_BANDS * sizeof(uint32_t));
	pList->pulCurrent += BVDC_MAX_LUMA_PEAKING_FREQ_BANDS;

	/* Luma Peaking Input Coring Low band Gain offset*/
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET1) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET1,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringLoBand.ailGainOffset[0])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET2) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET2,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringLoBand.ailGainOffset[1])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET3) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET3,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringLoBand.ailGainOffset[2])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET4) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET4,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringLoBand.ailGainOffset[3]));

	*pList->pulCurrent++ = ulRegData;

	/* Luma Peaking Input Coring Low band Threshold */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_THR_LOW);

	 ulRegData = BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_LOW, T1,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringLoBand.aulThreshold[0]) |
		 		BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_LOW, T2,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringLoBand.aulThreshold[1]);

	*pList->pulCurrent++ = ulRegData;

	/* Luma Peaking Input Coring Low Band Scale Factors */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_DIV_LOWi_ARRAY_BASE);
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pCurInfo->stSharpnessPrivData.aulLoBandScaleFactor,
			BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS;

	/* Luma Peaking Input Coring High band */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET1) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET1,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringHiBand.ailGainOffset[0])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET2) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET2,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringHiBand.ailGainOffset[1])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET3) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET3,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringHiBand.ailGainOffset[2])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET4) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET4,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringHiBand.ailGainOffset[3]));

	*pList->pulCurrent++ = ulRegData;

	/* Luma Peaking Input Coring High band Threshold */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH, T1) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH, T1,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringHiBand.aulThreshold[0])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH, T2) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH, T2,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stInputCoringHiBand.aulThreshold[1]));

	*pList->pulCurrent++ = ulRegData;

	/* Luma Peaking Input Coring High band Scale Factors */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_DIV_HIGHi_ARRAY_BASE);
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pCurInfo->stSharpnessPrivData.aulHiBandScaleFactor,
			BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS;

	/* Luma Peaking Output Coring Mode */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_OUT_CORE);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LPEAK_OUT_CORE, MODE) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_OUT_CORE, MODE,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.eOutputCoringMode)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_OUT_CORE, LCORE_BAND1) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_OUT_CORE, LCORE_BAND1,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.aulOutputCoringBands[0])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_OUT_CORE, LCORE_BAND2) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_OUT_CORE, LCORE_BAND2,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.aulOutputCoringBands[1])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_OUT_CORE, LCORE_BAND3) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_OUT_CORE, LCORE_BAND3,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.aulOutputCoringBands[2]));

	*pList->pulCurrent++ = ulRegData;

	/* Luma Peaking Clip Avoidance */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_CLIP_AVOID);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, CLIPAVOID_EN) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, CLIPAVOID_EN,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.bEnableClipAvoidance)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, CLIPVAL1) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, CLIPVAL1,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stClipAvoidLower.ulThreshold)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, SLOPE1) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, SLOPE1,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stClipAvoidLower.ulSlope)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, CLIPVAL2) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, CLIPVAL2,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stClipAvoidUpper.ulThreshold)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, SLOPE2) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, SLOPE2,
		 			pCurInfo->stSharpnessConfig.stLumaPeaking.stClipAvoidUpper.ulSlope));

	*pList->pulCurrent++ = ulRegData;

	/* LTI Control */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_CONTROL);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LTI_CONTROL, LTI_INCORE_EN) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_CONTROL, LTI_INCORE_EN,
		 			pCurInfo->stSharpnessConfig.stLti.bInputCoringEnable)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_CONTROL, SHIFT) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_CONTROL, SHIFT,
		 			pCurInfo->stSharpnessConfig.stLti.ilCurveControl)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_CONTROL, GAIN) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_CONTROL, GAIN,
		 			pCurInfo->stSharpnessConfig.stLti.ulGain)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_CONTROL, HAVOID) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_CONTROL, HAVOID,
		 			pCurInfo->stSharpnessConfig.stLti.ulHAvoid)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_CONTROL, VAVOID) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_CONTROL, VAVOID,
		 			pCurInfo->stSharpnessConfig.stLti.ulVAvoid)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_CONTROL, CORE_LEVEL) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_CONTROL, CORE_LEVEL,
		 			pCurInfo->stSharpnessConfig.stLti.ulOutputCoringLevel));

	*pList->pulCurrent++ = ulRegData;

	/* LTI Filter */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_FILTER);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LTI_FILTER, BLUR_EN) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_FILTER, BLUR_EN,
		 			pCurInfo->stSharpnessConfig.stLti.bBlurFilterEnable)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_FILTER, V_FILT_SEL) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_FILTER, V_FILT_SEL,
		 			pCurInfo->stSharpnessConfig.stLti.eVerFilter)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_FILTER, H_FILT_SEL) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_FILTER, H_FILT_SEL,
		 			pCurInfo->stSharpnessConfig.stLti.eHorFilter));

	*pList->pulCurrent++ = ulRegData;

	/* LTI Input Coring Threshold */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_INCORE_THR);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LTI_INCORE_THR, T2) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_THR, T2,
		 			pCurInfo->stSharpnessConfig.stLti.aulThreshold[1])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_INCORE_THR, T1) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_THR, T1,
		 			pCurInfo->stSharpnessConfig.stLti.aulThreshold[0]));

	*pList->pulCurrent++ = ulRegData;

	/* LTI Input Coring Threshold Scale Factors */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_INCORE_DIVi_ARRAY_BASE);
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pCurInfo->stSharpnessPrivData.aulLtiScaleFactor,
			BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS;

	/* LTI Input Coring Gain offset*/
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_INCORE_GOFF);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET1) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET1,
		 			pCurInfo->stSharpnessConfig.stLti.ailGainOffset[0])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET2) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET2,
		 			pCurInfo->stSharpnessConfig.stLti.ailGainOffset[1])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET3) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET3,
		 			pCurInfo->stSharpnessConfig.stLti.ailGainOffset[2])) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET4) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET4,
		 			pCurInfo->stSharpnessConfig.stLti.ailGainOffset[3]));

	*pList->pulCurrent++ = ulRegData;

	/* CTI Horizontal Filter Controls*/
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_CTI_H);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_CTI_H, FILT_SEL) & BCHP_FIELD_DATA(TNT_CMP_0_V0_CTI_H, FILT_SEL,
		 			pCurInfo->stSharpnessConfig.stCtiHorFilter.eFilter)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_CTI_H, GAIN) & BCHP_FIELD_DATA(TNT_CMP_0_V0_CTI_H, GAIN,
		 			pCurInfo->stSharpnessConfig.stCtiHorFilter.ulGain)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_CTI_H, CORE_LEVEL) & BCHP_FIELD_DATA(TNT_CMP_0_V0_CTI_H, CORE_LEVEL,
		 			pCurInfo->stSharpnessConfig.stCtiHorFilter.ulCoringLevel));

	*pList->pulCurrent++ = ulRegData;

	/* CTI Vertical Filter Controls*/
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_CTI_V);

	 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_CTI_V, FILT_SEL) & BCHP_FIELD_DATA(TNT_CMP_0_V0_CTI_V, FILT_SEL,
		 			pCurInfo->stSharpnessConfig.stCtiVerFilter.eFilter)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_CTI_V, GAIN) & BCHP_FIELD_DATA(TNT_CMP_0_V0_CTI_V, GAIN,
		 			pCurInfo->stSharpnessConfig.stCtiVerFilter.ulGain)) |
		 		 (BCHP_MASK(TNT_CMP_0_V0_CTI_V, CORE_LEVEL) & BCHP_FIELD_DATA(TNT_CMP_0_V0_CTI_V, CORE_LEVEL,
		 			pCurInfo->stSharpnessConfig.stCtiVerFilter.ulCoringLevel));

	*pList->pulCurrent++ = ulRegData;

	/* Demo */
	if(pCurInfo->stSplitScreenSetting.eSharpness != BVDC_SplitScreenMode_eDisable)
	{
		uint32_t ulBoundary = hWindow->stAdjDstRect.ulWidth / 2;
		uint32_t ulDemoSide = (pCurInfo->stSplitScreenSetting.eSharpness == BVDC_SplitScreenMode_eLeft) ?
		                       BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_LEFT :
		                       BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_RIGHT;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_DEMO_SETTING);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(TNT_CMP_0_V0_DEMO_SETTING, DEMO_L_R, ulDemoSide) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_DEMO_SETTING, DEMO_BOUNDARY, ulBoundary) ;

		BDBG_MSG(("TNT Demo Mode: L_R = %s, BOUNDARY = %d",
		          ((ulDemoSide == BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_LEFT) ? "L" : "R"),
		          ulBoundary));
	}

	BDBG_LEAVE(BVDC_P_Tnt_BuildChromaSharpnessRegionRul_isr);

}

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Tnt_ValidateSharpnessSettings
	( const BVDC_SharpnessSettings       *pstSettings )
{
	BERR_Code err = BERR_SUCCESS;
	uint32_t i;

	BDBG_ENTER(BVDC_P_Tnt_ValidateSharpnessSettings);

	/* Luma peaking coring low band/high band thresholds and LTI coring thresholds */
	for (i=0; i<BVDC_MAX_CORING_THRESHOLDS; i++)
	{
		if (pstSettings->stLumaPeaking.stInputCoringLoBand.aulThreshold[0] >
			pstSettings->stLumaPeaking.stInputCoringLoBand.aulThreshold[1])
		{
			BDBG_ERR(("Luma Peaking input coring low band threshold T1[%d] > T2[%d]",
				pstSettings->stLumaPeaking.stInputCoringLoBand.aulThreshold[0],
				pstSettings->stLumaPeaking.stInputCoringLoBand.aulThreshold[1]));
			err = BERR_INVALID_PARAMETER;
		}
		if (pstSettings->stLumaPeaking.stInputCoringHiBand.aulThreshold[0] >
			pstSettings->stLumaPeaking.stInputCoringHiBand.aulThreshold[1])
		{
			BDBG_ERR(("Luma Peaking input coring high band threshold T1[%d] > T2[%d]",
				pstSettings->stLumaPeaking.stInputCoringHiBand.aulThreshold[0],
				pstSettings->stLumaPeaking.stInputCoringHiBand.aulThreshold[1]));
			err = BERR_INVALID_PARAMETER;
		}
		if (pstSettings->stLti.aulThreshold[0] > pstSettings->stLti.aulThreshold[1])
		{
			BDBG_ERR(("LTI input coring threshold T1[%d] > T2[%d]",
				pstSettings->stLti.aulThreshold[0],
				pstSettings->stLti.aulThreshold[1]));
			err = BERR_INVALID_PARAMETER;
		}
	}

	for (i=0; i<BVDC_MAX_CHROMA_SHARPNESS_REGIONS; i++)
	{
		err = BVDC_P_Tnt_ValidateChromaSharpnessRegion(&pstSettings->astChromaRegion[i], i);
	}

	BDBG_LEAVE(BVDC_P_Tnt_ValidateSharpnessSettings);
	return err;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Tnt_StoreSharpnessSettings
	( BVDC_Window_Handle                  hWindow,
 	  const BVDC_SharpnessSettings       *pstSettings )
{
	BVDC_P_Window_Info     *pNewInfo;
	uint32_t                i;
	uint32_t                ulRegData;

	BDBG_ENTER(BVDC_P_Tnt_StoreSharpnessSettings);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pNewInfo = &hWindow->stNewInfo;
	pNewInfo->stSharpnessConfig = *pstSettings;

	/* Luma peaking */
	/* store luma peaking gains per frequency band */
	for (i=0; i<BVDC_MAX_LUMA_PEAKING_FREQ_BANDS; i++)
	{
		 ulRegData = (BCHP_MASK(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_NEG) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_NEG, pstSettings->stLumaPeaking.astGain[i].ulUndershootGain)) |
					 (BCHP_MASK(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_POS) & BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_POS, pstSettings->stLumaPeaking.astGain[i].ulOvershootGain));
		 pNewInfo->stSharpnessPrivData.aulLumaPeakingGain[i] = ulRegData;
	}

	/* store coring threshold scale factors */
	for (i=0; i<BVDC_P_MAX_CORING_THRESHOLD_SCALE_FACTORS; i++)
	{
		/* Lo Band */
		pNewInfo->stSharpnessPrivData.aulLoBandScaleFactor[i] = BVDC_P_Tnt_CalculateThresholdScaleFactor(i,
					pNewInfo->stSharpnessConfig.stLumaPeaking.stInputCoringLoBand.aulThreshold);

		/* Hi Band */
		pNewInfo->stSharpnessPrivData.aulHiBandScaleFactor[i] = BVDC_P_Tnt_CalculateThresholdScaleFactor(i,
					pNewInfo->stSharpnessConfig.stLumaPeaking.stInputCoringHiBand.aulThreshold);

		/* Lti */
		pNewInfo->stSharpnessPrivData.aulLtiScaleFactor[i] = BVDC_P_Tnt_CalculateThresholdScaleFactor(i,
					pNewInfo->stSharpnessConfig.stLti.aulThreshold);
	}

	/* Chroma sharpness regions */
	for (i=0; i<BVDC_MAX_CHROMA_SHARPNESS_REGIONS; i++)
	{
		BVDC_P_Tnt_StoreChromaSharpnessRegionInfo(hWindow, &pstSettings->astChromaRegion[i], i);
	}

	BDBG_LEAVE(BVDC_P_Tnt_StoreSharpnessSettings);

}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Tnt_BuildInit_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pList);
}

void BVDC_P_Tnt_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BVDC_P_Window_Info           *pCurInfo;
	BVDC_P_Window_DirtyBits      *pCurDirty;

	BDBG_ENTER(BVDC_P_Tnt_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(BVDC_P_WindowId_eComp0_V0 == hWindow->eId);

	/* rebuild all TN2T if compositor initial or window initial */
	pCurDirty = &hWindow->stCurInfo.stDirty;
	pCurInfo = &hWindow->stCurInfo;

	if(pCurInfo->bSharpnessEnable)
	{
		BDBG_MSG(("Building TNT RUL"));

		/* TOP Control */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_TOP_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, UPDATE_SEL, NORMAL) |
			((pCurInfo->stSplitScreenSetting.eSharpness == BVDC_SplitScreenMode_eDisable) ?
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, DEMO_MODE, DISABLE) :
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, DEMO_MODE, ENABLE)) |
			((pCurInfo->stSharpnessPrivData.bChromaRegionCorrectionEnable == false) ?
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, CHROMA_REGION_EN, DISABLE) :
			 BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, CHROMA_REGION_EN, ENABLE)) |
			((pCurInfo->stSharpnessConfig.stLumaPeaking.bInputCoringEnable == false) ?
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, LPEAK_INCORE_EN, DISABLE) :
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, LPEAK_INCORE_EN, ENABLE)) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_TOP_CONTROL, H_WINDOW,
				pCurInfo->stSharpnessConfig.eLumaStatWin) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, MASTER_EN, ENABLE);

		/* TNT's CM3D */
		if (pCurInfo->stSharpnessPrivData.bChromaRegionCorrectionEnable)
		{
			BVDC_P_Tnt_BuildChromaSharpnessRegionRul_isr(hWindow, pList);
		}

		/* Peaking, LTI, CTI */
		BVDC_P_Tnt_BuildSharpnessRul_isr(hWindow, pList);
	}
	else
	{
		/* Bypass TNT */
		BDBG_MSG(("Bypass TNT"));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_TOP_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, UPDATE_SEL, NORMAL) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, DEMO_MODE, DISABLE) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, MASTER_EN, DISABLE);
	}

	BDBG_LEAVE(BVDC_P_Tnt_BuildRul_isr);

	return;
}

void BVDC_P_Tnt_BuildVysncRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pList);
}
#elif(BVDC_P_SUPPORT_TNT_VER >= 6)
void BVDC_P_Tnt_BuildInit_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BDBG_ENTER(BVDC_P_Tnt_BuildInit_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(3);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_DIV_HIGHi_ARRAY_BASE);
	*pList->pulCurrent++ = 0x51e;
	*pList->pulCurrent++ = 0x1b4;
	*pList->pulCurrent++ = 0x4f;

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_OUT_CORE);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(TNT_CMP_0_V0_LPEAK_OUT_CORE, MODE,        TOTAL) |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_OUT_CORE, LCORE_BAND2, 0x8  ) |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_OUT_CORE, LCORE_BAND1, 0x8  );

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_CLIP_AVOID);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, CLIPAVOID_EN, ENABLE) |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, SLOPE2,       0x1)    |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, SLOPE1,       0x1)    |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, CLIPVAL2,     0x334)  |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_CLIP_AVOID, CLIPVAL1,     0xc8);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_FILTER);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(TNT_CMP_0_V0_LTI_FILTER, BLUR_EN,    ENABLE) |
		BCHP_FIELD_ENUM(TNT_CMP_0_V0_LTI_FILTER, H_FILT_SEL, TAP9);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_INCORE_THR);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_THR, T2, 0xc8) |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_THR, T1, 0x32);

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(3);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_INCORE_DIVi_ARRAY_BASE);
	*pList->pulCurrent++ = 0x51e;
	*pList->pulCurrent++ = 0x1b4;
	*pList->pulCurrent++ = 0x4f;

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_INCORE_GOFF);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET4, 0x0) |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET3, 0x0) |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET2, 0xfffffffc) |
		BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_INCORE_GOFF, OFFSET1, 0xfffffffc);

	BSTD_UNUSED(hWindow);
	BDBG_LEAVE(BVDC_P_Tnt_BuildInit_isr);
	return;
}

void BVDC_P_Tnt_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BVDC_P_Window_Info    *pCurInfo;

	BDBG_ENTER(BVDC_P_Tnt_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/* Get current pointer to RUL */
	pCurInfo = &hWindow->stCurInfo;

	if(pCurInfo->bSharpnessEnable)
	{
		bool bIsInputSd;
		bool bIsOutputSd;
		uint32_t aulLPeakGain[4];
		uint32_t ulLPeakGoffLow;
		uint32_t ulLPeakGoffHigh;
		uint32_t ulLPeakIncoreThrLow;
		uint32_t aulPeakIncoreThrDivLow[3];
		uint32_t ulLPeakIncoreThrHigh;
		uint32_t ulCoreLevel;
		uint32_t ulGainA, ulGainB;
		uint32_t ulLtiGain;
		uint32_t ulCtiHGain;

		/* Treat both SD and ED as SD for TNT settings */
		bIsInputSd = VIDEO_FORMAT_IS_SD(pCurInfo->hSource->stCurInfo.pFmtInfo->eVideoFmt) ||
			VIDEO_FORMAT_IS_ED(pCurInfo->hSource->stCurInfo.pFmtInfo->eVideoFmt);
		bIsOutputSd = VIDEO_FORMAT_IS_SD(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt) ||
			VIDEO_FORMAT_IS_ED(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt);
		BDBG_MSG(("Building TNT RUL %s In %s Out",
			bIsInputSd ? "SD" : "HD", bIsOutputSd ? "SD" : "HD"));

		/* Setting up the demo mode register */
		if(pCurInfo->stSplitScreenSetting.eSharpness != BVDC_SplitScreenMode_eDisable)
		{
			uint32_t ulBoundary = hWindow->stAdjDstRect.ulWidth / 2;
			uint32_t ulDemoSide = (pCurInfo->stSplitScreenSetting.eSharpness == BVDC_SplitScreenMode_eLeft) ?
			                       BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_LEFT :
			                       BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_RIGHT;

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_DEMO_SETTING);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_DEMO_SETTING, DEMO_L_R, ulDemoSide) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_DEMO_SETTING, DEMO_BOUNDARY, ulBoundary) ;

			BDBG_MSG(("TNT Demo Mode: L_R = %s, BOUNDARY = %d",
			          ((ulDemoSide == BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_LEFT) ? "L" : "R"),
			          ulBoundary));
		}

		if(bIsInputSd && !bIsOutputSd)
		{
			/* SD in HD out => SD mode */
			BVDC_P_Sharpness_Calculate_Gain_Value(pCurInfo->sSharpness, 4, 12, 20,
			                                      &ulGainA);
			BVDC_P_Sharpness_Calculate_Gain_Value(pCurInfo->sSharpness, 4, 12, 20,
			                                      &ulGainB);
			BVDC_P_Sharpness_Calculate_Gain_Value(pCurInfo->sSharpness, 4, 12, 20,
			                                      &ulLtiGain);
			BVDC_P_Sharpness_Calculate_Gain_Value(pCurInfo->sSharpness, 0, 10, 20,
			                                      &ulCtiHGain);
			aulLPeakGain[0] = 0;
			aulLPeakGain[1] =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_NEG, ulGainA) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_POS, ulGainA) ;
			aulLPeakGain[2] =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_NEG, ulGainB) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_POS, ulGainB) ;
			aulLPeakGain[3] = 0;

			ulLPeakGoffLow =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET4, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET3, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET2, 0xfffffffc) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET1, 0xfffffffc);

			ulLPeakGoffHigh =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET4, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET3, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET2, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET1, 0x0 );

			ulLPeakIncoreThrLow =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_LOW, T2, 0xc8) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_LOW, T1, 0x32);

			aulPeakIncoreThrDivLow[0] = 0x51e;
			aulPeakIncoreThrDivLow[1] = 0x1b4;
			aulPeakIncoreThrDivLow[2] = 0x4f;

			ulLPeakIncoreThrHigh =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH, T2, 0x0) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH, T1, 0x0);

			ulCoreLevel = 0x4;
		}
		else
		{
			/* SD in SD out => HD mode */
			/* HD in SD out => HD mode */
			/* HD in HD out => HD mode */
			BVDC_P_Sharpness_Calculate_Gain_Value(pCurInfo->sSharpness, 4, 12, 20,
			                                      &ulGainA);
			BVDC_P_Sharpness_Calculate_Gain_Value(pCurInfo->sSharpness, 4, 12, 20,
			                                      &ulGainB);
			BVDC_P_Sharpness_Calculate_Gain_Value(pCurInfo->sSharpness, 4, 12, 20,
			                                      &ulLtiGain);
			BVDC_P_Sharpness_Calculate_Gain_Value(pCurInfo->sSharpness, 0, 10, 20,
			                                      &ulCtiHGain);
			aulLPeakGain[0] = 0;
			aulLPeakGain[1] = 0;
			aulLPeakGain[2] =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_NEG, ulGainA) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_POS, ulGainA) ;
			aulLPeakGain[3] =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_NEG, ulGainB) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_GAINSi, GAIN_POS, ulGainB) ;

			ulLPeakGoffLow =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET4, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET3, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET2, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW, OFFSET1, 0x0 );

			ulLPeakGoffHigh =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET4, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET3, 0x0 ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET2, 0xfffffffc ) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH, OFFSET1, 0xfffffffc );

			ulLPeakIncoreThrLow =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_LOW, T2, 0x0) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_LOW, T1, 0x0);

			aulPeakIncoreThrDivLow[0] = 0x0;
			aulPeakIncoreThrDivLow[1] = 0x0;
			aulPeakIncoreThrDivLow[2] = 0x0;

			ulLPeakIncoreThrHigh =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH, T2, 0xc8) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH, T1, 0x32);

			ulCoreLevel = 0x4;
		}

		BDBG_MSG(("GainA=0x%x, GainB=0x%x", ulGainA, ulGainB));
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(4);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_GAINSi_ARRAY_BASE);
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)&aulLPeakGain, 4 * sizeof(uint32_t));
		pList->pulCurrent += 4;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_GOFF_LOW);
		*pList->pulCurrent++ = ulLPeakGoffLow;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_GOFF_HIGH);
		*pList->pulCurrent++ = ulLPeakGoffHigh;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_THR_LOW);
		*pList->pulCurrent++ = ulLPeakIncoreThrLow;

		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(3);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_DIV_LOWi_ARRAY_BASE);
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)&aulPeakIncoreThrDivLow, 3 * sizeof(uint32_t));
		pList->pulCurrent += 3;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LPEAK_INCORE_THR_HIGH);
		*pList->pulCurrent++ = ulLPeakIncoreThrHigh;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LTI_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_LTI_CONTROL, LTI_INCORE_EN, ENABLE)|
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_CONTROL, GAIN,          ulLtiGain)  |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_CONTROL, HAVOID,        0xc)   |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LTI_CONTROL, CORE_LEVEL,    ulCoreLevel);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_CTI_H);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_CTI_H, FILT_SEL,   TAP7) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_CTI_H, GAIN,       ulCtiHGain) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_CTI_H, CORE_LEVEL, 0xa);
	}

	BDBG_LEAVE(BVDC_P_Tnt_BuildRul_isr);
	return;
}

void BVDC_P_Tnt_BuildVysncRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BVDC_P_Window_Info    *pCurInfo;

	BDBG_ENTER(BVDC_P_Tnt_BuildVysncRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/* Get current pointer to RUL */
	pCurInfo = &hWindow->stCurInfo;

	if(pCurInfo->bSharpnessEnable)
	{
		/* TOP Control */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_TOP_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, UPDATE_SEL, NORMAL) |
			((pCurInfo->stSplitScreenSetting.eSharpness == BVDC_SplitScreenMode_eDisable) ?
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, DEMO_MODE, DISABLE) :
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, DEMO_MODE, ENABLE)) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, LPEAK_INCORE_EN, ENABLE) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_TOP_CONTROL, H_WINDOW, 2) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, MASTER_EN, ENABLE);
	}
	else
	{
		/* Bypass TNT */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_TOP_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, LPEAK_INCORE_EN, DISABLE) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, DEMO_MODE,       DISABLE) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_TOP_CONTROL, H_WINDOW,        2      ) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, UPDATE_SEL,      NORMAL ) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, MASTER_EN,       DISABLE);
	}

	BDBG_LEAVE(BVDC_P_Tnt_BuildVysncRul_isr);
	return;
}
#else  /* TNT1 HW base */
void BVDC_P_Tnt_BuildInit_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pList);
}

void BVDC_P_Tnt_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BVDC_P_Window_Info    *pCurInfo;

	BDBG_ENTER(BVDC_P_Tnt_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/* Get current pointer to RUL */
	pCurInfo = &hWindow->stCurInfo;

	if(pCurInfo->bSharpnessEnable)
	{
		if(!pCurInfo->bUserSharpnessConfig)
		{
			bool bIsSd = VIDEO_FORMAT_IS_SD(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt) ||
			             VIDEO_FORMAT_IS_SD(pCurInfo->hSource->stCurInfo.pFmtInfo->eVideoFmt);
			int16_t sCenterGain = (bIsSd) ? 0x1f : 0xf;
			BVDC_P_Sharpness_Calculate_Gain_Value(pCurInfo->sSharpness, 0, sCenterGain, 0x3F,
			                                      &hWindow->stCurResource.hPep->ulLumaChromaGain);

			pCurInfo->stSharpnessConfig.ulLumaCtrlGain   = hWindow->stCurResource.hPep->ulLumaChromaGain;
			pCurInfo->stSharpnessConfig.ulChromaCtrlGain = hWindow->stCurResource.hPep->ulLumaChromaGain;
			pCurInfo->stSharpnessConfig.bLumaCtrlHOnly   =
				(hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced) ? true : false;
			pCurInfo->stSharpnessConfig.bChromaCtrlHOnly =
				(hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced) ? true : false;
		}

		/* Build RUL */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_TOP_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, UPDATE_SEL, EOP) |
			((pCurInfo->stSplitScreenSetting.eSharpness == BVDC_SplitScreenMode_eDisable) ?
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, DEMO_MODE, DISABLE) :
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, DEMO_MODE, ENABLE)) |
#if (BVDC_P_SUPPORT_TNT_VER == 2) || (BVDC_P_SUPPORT_TNT_VER >= 4)
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, RING_SUPPION_MODE, NORMAL) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, RING_SUPPION, DISABLE) |
#endif
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, ENABLE, ENABLE);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LUMA_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_CONTROL, SCRATCH, 0x0) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_CONTROL, CORE,
				pCurInfo->stSharpnessConfig.ulLumaCtrlCore ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_CONTROL, GAIN,
				pCurInfo->stSharpnessConfig.ulLumaCtrlGain ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_CONTROL, BLUR,
				pCurInfo->stSharpnessConfig.ulLumaCtrlBlur )|
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_CONTROL, SOFTEN,
				pCurInfo->stSharpnessConfig.bLumaCtrlSoften )|
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_CONTROL, H_ONLY,
				pCurInfo->stSharpnessConfig.bLumaCtrlHOnly );

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LUMA_PEAKING);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_PEAKING, H_AVOID,
				pCurInfo->stSharpnessConfig.ulLumaPeakingHAvoid ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_PEAKING, V_AVOID,
				pCurInfo->stSharpnessConfig.ulLumaPeakingVAvoid ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_PEAKING, PEAK_LIMIT,
				pCurInfo->stSharpnessConfig.ulLumaPeakingPeakLimit ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LUMA_PEAKING, PEAK_VALUE,
				pCurInfo->stSharpnessConfig.ulLumaPeakingPeakValue ) ;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_CHROMA_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(TNT_CMP_0_V0_CHROMA_CONTROL, SCRATCH, 0x0) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_CHROMA_CONTROL, CORE,
				pCurInfo->stSharpnessConfig.ulChromaCtrlCore ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_CHROMA_CONTROL, WIDE_CHROMA,
				pCurInfo->stSharpnessConfig.bChromaCtrlWideChroma ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_CHROMA_CONTROL, FALSE_COLOR,
				pCurInfo->stSharpnessConfig.ulChromaCtrlFalseColor ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_CHROMA_CONTROL, GAIN,
				pCurInfo->stSharpnessConfig.ulChromaCtrlGain ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_CHROMA_CONTROL, H_ONLY,
				pCurInfo->stSharpnessConfig.bChromaCtrlHOnly );

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_LC_ALIGN_CONTROL, ALIGN_CR_CB, SEL_CB) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_LC_ALIGN_CONTROL, RESET_COUNTER, 0x0);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL );
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(TNT_CMP_0_V0_WIDE_LUMA_CONTROL, CORE,
				pCurInfo->stSharpnessConfig.ulWideLumaCtrlCore ) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_WIDE_LUMA_CONTROL, MODE,
				pCurInfo->stSharpnessConfig.ulWideLumaCtrlMode ) ;
#if (BVDC_P_SUPPORT_TNT_VER >= 3)
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL , CORE,
				pCurInfo->stSharpnessConfig.ulSimpleLumaCtrlCore) |
			BCHP_FIELD_DATA(TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL , MODE,
				pCurInfo->stSharpnessConfig.bSimpleLumaCtrlMode);
#endif

		/* Setting up the demo mode register */
		if(pCurInfo->stSplitScreenSetting.eSharpness != BVDC_SplitScreenMode_eDisable)
		{
			uint32_t ulBoundary = hWindow->stAdjDstRect.ulWidth / 2;
			uint32_t ulDemoSide = (pCurInfo->stSplitScreenSetting.eSharpness == BVDC_SplitScreenMode_eLeft) ?
			                       BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_LEFT :
			                       BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_RIGHT;

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_DEMO_SETTING);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_DEMO_SETTING, DEMO_L_R, ulDemoSide) |
				BCHP_FIELD_DATA(TNT_CMP_0_V0_DEMO_SETTING, DEMO_BOUNDARY, ulBoundary) ;

			BDBG_MSG(("TNT Demo Mode: L_R = %s, BOUNDARY = %d",
			          ((ulDemoSide == BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_LEFT) ? "L" : "R"),
			          ulBoundary));
		}
	}
	else
	{
		/* Bypass TNT */
		BDBG_MSG(("Bypass TNT"));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_TOP_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, UPDATE_SEL, NORMAL) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, DEMO_MODE, DISABLE) |
#if (BVDC_P_SUPPORT_TNT_VER == 2) || (BVDC_P_SUPPORT_TNT_VER >= 4)
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, RING_SUPPION_MODE, NORMAL) |
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, RING_SUPPION, DISABLE) |
#endif
			BCHP_FIELD_ENUM(TNT_CMP_0_V0_TOP_CONTROL, ENABLE, BYPASS);

#if (BVDC_P_SUPPORT_TNT_VER >= 3)
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL , CORE, 0x0	 ) |
				BCHP_FIELD_ENUM(TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL , MODE, DISABLE);
#endif
	}

	BDBG_LEAVE(BVDC_P_Tnt_BuildRul_isr);
	return;
}

void BVDC_P_Tnt_BuildVysncRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pList);
}
#endif /* TNT1 HW base */
#else  /* BVDC_P_SUPPORT_TNT == 0*/
void BVDC_P_Tnt_BuildInit_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pList);
}
void BVDC_P_Tnt_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pList);
}

void BVDC_P_Tnt_BuildVysncRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pList);
}
#endif /* BVDC_P_SUPPORT_TNT */

/* End of File */
