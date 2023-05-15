/***************************************************************************
 *	   Copyright (c) 2004-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_cm3d_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/31 $
 * $brcm_Date: 3/8/12 11:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_cm3d_priv.c $
 * 
 * Hydra_Software_Devel/31   3/8/12 11:50a yuxiaz
 * SW7425-2546: Fixed warning msg.
 * 
 * Hydra_Software_Devel/30   12/1/11 4:31p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 * 
 * Hydra_Software_Devel/SW7425-968/1   11/3/11 2:28p yuxiaz
 * SW7425-968: Use BVDC_P_Rect in VDC.
 * 
 * Hydra_Software_Devel/29   11/11/10 7:27p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 * 
 * Hydra_Software_Devel/28   6/18/10 8:51p rpan
 * SW7400-2763: Fixed compile errors.
 * 
 * Hydra_Software_Devel/27   5/7/10 7:08p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 * 
 * Hydra_Software_Devel/26   4/19/10 10:12p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/25   4/7/10 11:27a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/24   4/5/10 3:59p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/23   12/9/09 12:08p jessem
 * SW3556-868: Enclosed ulRegionId in BDBG_DEBUG_BUILD.
 *
 * Hydra_Software_Devel/22   7/21/09 5:20p jessem
 * PR 39335: Fixed RUL for enabling VIVID in
 * BVDC_P_Cm3d_BuildRegionRul_isr and
 * BVDC_P_Cm3d_BuildOverlapRegionRul_isr.
 *
 * Hydra_Software_Devel/21   5/13/09 12:32a pntruong
 * PR51108: Coverity Defect ID:13602 OVERRUN_STATIC.
 *
 * Hydra_Software_Devel/20   3/13/09 5:28p syang
 * PR 52376, PR 41481, PR 44041:
 * 1). exec user buf returning to hBuffer before shut-down code in
 * writer_isr; 2). clean up mad-delay handling code and add handling to
 * all settings in pic-node that are delayed by mad; 3). 444 SCL no-
 * longer uses BVN_IN to align-up for half-band; 4). add pic-node rects
 * dump feature as setting lbox_0_scratch to 2 (1 still printing mpeg pic
 * info).
 *
 * Hydra_Software_Devel/19   2/17/09 3:18p jessem
 * PR 48992: Added filling of hue/sat chroma histogram array aaaand
 * setting of chroma histogram type when collection is enabled.
 *
 * Hydra_Software_Devel/18   12/3/08 5:04p jessem
 * PR 48992: Added Chroma Histogram API support.
 *
 * Hydra_Software_Devel/17   11/21/08 11:38a jessem
 * PR 49411: Enhanced error message for checking PWL count per region and
 * min and max values.
 *
 * Hydra_Software_Devel/16   10/7/08 11:09a jessem
 * PR 39335: Changed memcpy to simple assignment.
 *
 * Hydra_Software_Devel/15   9/29/08 4:52p jessem
 * PR 46498, PR 46499: Added VIVID_ENA register programming and removed
 * slope references for B0.
 *
 * Hydra_Software_Devel/14   9/9/08 4:38p jessem
 * PR 46499: Removed PWL slope references for B0.
 *
 * Hydra_Software_Devel/13   7/22/08 10:15a jessem
 * PR 39335: Changed number of PWLs to validate  in
 * BVDC_P_Cm3d_ValidateRegion based on BVDC_Cm3dRegion's ulPwlCnt field
 * instead of BVDC_MAX_CM3D_PWL.
 *
 * Hydra_Software_Devel/12   7/21/08 5:33p jessem
 * PR 39335: Changed number of PWLs to configure in
 * BVDC_P_Cm3d_BuildRegionRul_isr based on BVDC_Cm3dRegion's ulPwlCnt
 * field  instead of BVDC_MAX_CM3D_PWL.
 *
 * Hydra_Software_Devel/11   7/9/08 6:12p pntruong
 * PR44306: CM3D and CMP are tied together need to reprogram CM3D when CMP
 * is reset.
 *
 * Hydra_Software_Devel/10   5/23/08 3:48p jessem
 * PR 39335: Rearranged disabling of overlapped region.
 *
 * Hydra_Software_Devel/9   5/12/08 3:03p jessem
 * PR 39335: Added check for region enables. Refactored out building of
 * region and overlap region RULs from BVDC_P_Cm3d_BuildRul_isr.
 *
 * Hydra_Software_Devel/8   5/2/08 10:59a jessem
 * PR 39335: Corrected implementation of  PWL point validation and slope
 * calculation. Added CM3D demo.
 *
 * Hydra_Software_Devel/7   4/4/08 4:51p jessem
 * PR 39335: Removed  bCm3dOvlpRegionChange and bCm3dRegionChange in
 * BVDC_P_Window_Context. Applied setting of dirty bits in "Set"
 * function. Corrected implementation of CM3D RULs.
 *
 * Hydra_Software_Devel/6   3/31/08 5:25p jessem
 * PR 39335: Added conditional compile directives.
 *
 * Hydra_Software_Devel/5   3/31/08 3:29p jessem
 * PR 39335: Removed printfs.
 *
 * Hydra_Software_Devel/4   3/31/08 3:18p jessem
 * PR 39335: Modified implementation.
 *
 * Hydra_Software_Devel/3   3/31/08 2:28p jessem
 * PR 39335: Removed printfs.
 *
 * Hydra_Software_Devel/2   3/31/08 2:25p jessem
 * PR 39335: Added debug support and removed unused defines.
 *
 * Hydra_Software_Devel/1   3/31/08 2:18p jessem
 * PR 39335: Initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "berr.h"

#if BVDC_P_SUPPORT_CM3D
#include "bchp_vivid_cmp_0.h"
#endif

#include "bvdc.h"
#include "bvdc_priv.h"
#include "bvdc_cm3d_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_window_priv.h"


/* constants */
#define BVDC_P_CM3D_FORMAT_POINTS       2 /* Cr/CB or Hue/Sat */

/* Non-overlapped regions */
#define BVDC_P_MIN_HUE                  0
#define BVDC_P_MAX_HUE               1023
#define BVDC_P_MIN_SAT                  0
#define BVDC_P_MAX_SAT               1023

/* Overlapped regions */
#define BVDC_P_OVERLAPPED_MIN_HUE       0
#define BVDC_P_OVERLAPPED_MAX_HUE    1023

#define BVDC_P_CM3D_PWL_X_BIT_MASK  0x3ff

#if (BCHP_VER == BCHP_VER_A0)
#define BVDC_P_CM3D_PWL_SLOPE_FBITS     9
#define BVDC_P_CM3D_PWL_Y_FBITS         8
#endif

BDBG_MODULE(BVDC_CM3D);
BDBG_OBJECT_ID(BVDC_C3D);

#if BVDC_P_SUPPORT_CM3D
/***************************************************************************
 * {private}
 *
 */
static bool BVDC_P_Cm3d_CheckRegionEnables
	( BVDC_P_Cm3d_Handle          hCm3d )
{
	bool bEnable = false;
	uint32_t i;

	BDBG_OBJECT_ASSERT(hCm3d, BVDC_C3D);

	/* Check enables for all 16 regions */
	for (i=0; i < BVDC_MAX_CM3D_REGIONS; i++)
	{
		bEnable |= hCm3d->abRegionEnable[i];
	}

	return bEnable;
}

/***************************************************************************
 * {private}
 *
 */
static bool BVDC_P_Cm3d_CheckOverlapRegionEnable
	( BVDC_P_Cm3d_Handle          hCm3d )
{
	bool bEnable = false;
	uint32_t i;

	BDBG_OBJECT_ASSERT(hCm3d, BVDC_C3D);

	/* Check enables for all non-overlapped regions */
	for (i=0; i < BVDC_MAX_CM3D_OVERLAPPED_REGIONS; i++)
	{
		bEnable |= hCm3d->abOvlpRegionEnable[i];
	}

	return bEnable;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Cm3d_UpdateChromaHistData_isr
 *
 */
static void BVDC_P_Cm3d_UpdateChromaHistData_isr
	( BVDC_P_Cm3d_Handle          hCm3d )
{
	uint32_t id;
	uint32_t ulReg;
	BVDC_ChromaStatus *pStats;
	uint32_t ulBinCount;
	uint32_t aulCrHue[BVDC_CR_CB_HISTOGRAM_COUNT], aulCbSat[BVDC_CR_CB_HISTOGRAM_COUNT];

	BDBG_OBJECT_ASSERT(hCm3d, BVDC_C3D);
	pStats = &hCm3d->stChromaHistData;
	ulBinCount = (hCm3d->eChromaHistType == BVDC_ChromaHistType_eCrCb) ?
		BVDC_CR_CB_HISTOGRAM_COUNT : BVDC_HUE_SAT_HISTOGRAM_COUNT;

	BKNI_Memset((void*)hCm3d->stChromaHistData.stCrCbHist.aulCbHistogram, 0x0,
		sizeof(uint32_t) * BVDC_CR_CB_HISTOGRAM_COUNT);

	BKNI_Memset((void*)hCm3d->stChromaHistData.stCrCbHist.aulCrHistogram, 0x0,
		sizeof(uint32_t) * BVDC_CR_CB_HISTOGRAM_COUNT);

	for(id = 0; id < ulBinCount; id++)
	{
		ulReg = BREG_Read32(hCm3d->hReg, BCHP_VIVID_CMP_0_CR_HUE_BINi_ARRAY_BASE + (id * 4));
		if (BCHP_GET_FIELD_DATA(ulReg, VIVID_CMP_0_CR_HUE_BINi, INVALID) == BCHP_VIVID_CMP_0_CR_HUE_BINi_INVALID_INVALID)
		{
			aulCrHue[id] = 0;
		}
		else
		{
			aulCrHue[id] = BCHP_GET_FIELD_DATA(ulReg, VIVID_CMP_0_CR_HUE_BINi, COUNT);
		}

		ulReg = BREG_Read32(hCm3d->hReg, BCHP_VIVID_CMP_0_CB_SAT_BINi_ARRAY_BASE + (id * 4));
		if (BCHP_GET_FIELD_DATA(ulReg, VIVID_CMP_0_CB_SAT_BINi, INVALID) == BCHP_VIVID_CMP_0_CB_SAT_BINi_INVALID_INVALID)
		{
			aulCbSat[id] = 0;
		}
		else
		{
			aulCbSat[id] = BCHP_GET_FIELD_DATA(ulReg, VIVID_CMP_0_CB_SAT_BINi, COUNT);
		}
	}

	/* copy to array */
	if (hCm3d->eChromaHistType == BVDC_ChromaHistType_eCrCb)
	{
		BKNI_Memcpy((void *)pStats->stCrCbHist.aulCrHistogram, (void *)aulCrHue,
					sizeof(uint32_t) * BVDC_CR_CB_HISTOGRAM_COUNT);

		BKNI_Memcpy((void *)pStats->stCrCbHist.aulCbHistogram, (void *)aulCbSat,
					sizeof(uint32_t) * BVDC_CR_CB_HISTOGRAM_COUNT);

		pStats->stCrCbHist.ulCount = ulBinCount;

		BDBG_MSG(("Collected Chroma Cr/Cb histogram"));
	}
	else if (hCm3d->eChromaHistType == BVDC_ChromaHistType_eHueSat)
	{
		BKNI_Memcpy((void *)pStats->stHueSatHist.aulHueHistogram , (void *)aulCrHue,
					sizeof(uint32_t) * BVDC_HUE_SAT_HISTOGRAM_COUNT);

		BKNI_Memcpy((void *)pStats->stHueSatHist.aulSatHistogram, (void *)aulCbSat,
					sizeof(uint32_t) * BVDC_HUE_SAT_HISTOGRAM_COUNT);

		pStats->stHueSatHist.ulCount = ulBinCount;

		BDBG_MSG(("Collected Chroma Hue/Sat histogram"));
	}
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Cm3d_BuildHistCfgRul_isr
 *
 */
static void BVDC_P_Cm3d_BuildHistCfgRul_isr
	( const BVDC_Window_Handle          hWindow,
	  BVDC_P_ListInfo                  *pList )
{
	const BVDC_P_Window_Info *pCurInfo;
	BVDC_ClipRect stUsrClipRect;
	BVDC_P_Rect stChromaHistoRect;
	bool bInterlaced;
	uint32_t ulWidth, ulHeight;

	BDBG_ENTER(BVDC_P_Cm3d_BuildHistCfgRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pCurInfo = &hWindow->stCurInfo;

	ulWidth = hWindow->stCurInfo.stDstRect.ulWidth;
	ulHeight = hWindow->stCurInfo.stDstRect.ulHeight;
	bInterlaced = hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced;

	stUsrClipRect.ulLeft   = pCurInfo->stChromaRect.stRegion.ulLeft;
	stUsrClipRect.ulRight  = pCurInfo->stChromaRect.stRegion.ulRight;
	stUsrClipRect.ulTop    = pCurInfo->stChromaRect.stRegion.ulTop;
	stUsrClipRect.ulBottom = pCurInfo->stChromaRect.stRegion.ulBottom;

	BVDC_P_CalculateRect_isr(&stUsrClipRect, ulWidth, ulHeight, bInterlaced, &stChromaHistoRect);

	BVDC_P_PRINT_RECT("ChromaHistoRect", &stChromaHistoRect, false);
	BDBG_MSG(("ChromaHistType = %s, Interlaced = %s",
		pCurInfo->stChromaRect.eType ? "HUE_SAT" :"CR_CB",
		bInterlaced ? "true" : "false"));

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_CHIST_WIN_OFFSET);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VIVID_CMP_0_CHIST_WIN_OFFSET, H_OFFSET, stChromaHistoRect.lLeft) |
		BCHP_FIELD_DATA(VIVID_CMP_0_CHIST_WIN_OFFSET, V_OFFSET, stChromaHistoRect.lTop);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_CHIST_WIN_SIZE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VIVID_CMP_0_CHIST_WIN_SIZE, H_SIZE, stChromaHistoRect.ulWidth) |
		BCHP_FIELD_DATA(VIVID_CMP_0_CHIST_WIN_SIZE, V_SIZE, stChromaHistoRect.ulHeight);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_CHIST_SAT_MIN);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VIVID_CMP_0_CHIST_SAT_MIN, SAT_MIN, pCurInfo->stChromaRect.ulSatMin);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_CHIST_HUE_RANGE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VIVID_CMP_0_CHIST_HUE_RANGE, HUE_MAX, pCurInfo->stChromaRect.ulHueMax) |
		BCHP_FIELD_DATA(VIVID_CMP_0_CHIST_HUE_RANGE, HUE_MIN, pCurInfo->stChromaRect.ulHueMin);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_CHIST_TYPE);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VIVID_CMP_0_CHIST_TYPE, TYPE, pCurInfo->stChromaRect.eType);

	BDBG_LEAVE(BVDC_P_Cm3d_BuildHistCfgRul_isr);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Cm3d_BuildHistRul_isr
 *
 * called by BVDC_Cm3d_BuildRul_isr It builds RUL for Chroma
 * Hist HW module.
 *
 */
static void BVDC_P_Cm3d_BuildHistRul_isr
	( BVDC_Window_Handle                hWindow,
	  BVDC_P_ListInfo                  *pList )
{
	BVDC_P_Window_Info      *pCurInfo;
	BVDC_P_Window_DirtyBits *pCurDirty;

	/* handle validation */
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pCurInfo  = &hWindow->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;

	if (pCurInfo->bChromaHistEnable)
	{
		BVDC_P_Cm3d_BuildHistCfgRul_isr(hWindow, pList);
	}

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_CHIST_EN);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VIVID_CMP_0_CHIST_EN, ENABLE, pCurInfo->bChromaHistEnable);

	BDBG_MSG(("Set Chroma Histogram collection to %s", pCurInfo->bChromaHistEnable ? "true" : "false"));

}

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Cm3d_Create
	( BVDC_P_Cm3d_Handle          *phCm3d,
	  const BVDC_P_WindowId        eWinId,
	  const BREG_Handle            hReg )
{
	BVDC_P_Cm3dContext *pCm3d;

	BDBG_ENTER(BVDC_P_Cm3d_Create);

	BDBG_ASSERT(phCm3d);

	/* (1) Alloc the context. */
	pCm3d = (BVDC_P_Cm3dContext*)
		(BKNI_Malloc(sizeof(BVDC_P_Cm3dContext)));
	if(!pCm3d)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pCm3d, 0x0, sizeof(BVDC_P_Cm3dContext));
	BDBG_OBJECT_SET(pCm3d, BVDC_C3D);

	pCm3d->eId             = eWinId;
	pCm3d->hReg            = hReg;

	/* All done. now return the new fresh context to user. */
	*phCm3d = (BVDC_P_Cm3d_Handle)pCm3d;

	BDBG_LEAVE(BVDC_P_Cm3d_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Cm3d_Destroy
	( BVDC_P_Cm3d_Handle        hCm3d )
{
	BDBG_ENTER(BVDC_P_Cm3d_Destroy);
	BDBG_OBJECT_ASSERT(hCm3d, BVDC_C3D);

	BDBG_OBJECT_DESTROY(hCm3d, BVDC_C3D);
	/* [1] Free the context. */
	BKNI_Free((void*)hCm3d);

	BDBG_LEAVE(BVDC_P_Cm3d_Destroy);

	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Cm3d_Init
	( const BVDC_P_Cm3d_Handle  hCm3d )
{
	BDBG_ENTER(BVDC_P_Cm3d_Init);
	BDBG_OBJECT_ASSERT(hCm3d, BVDC_C3D);

	hCm3d->bInitial = true;

	/* reset arrays */
	BKNI_Memset((void*)hCm3d->astRegion, 0x0, sizeof(hCm3d->astRegion));
	BKNI_Memset((void*)hCm3d->abRegionEnable, 0x0, sizeof(hCm3d->abRegionEnable));
	BKNI_Memset((void*)hCm3d->astOvlpRegion, 0x0, sizeof(hCm3d->astOvlpRegion));
	BKNI_Memset((void*)hCm3d->abOvlpRegionEnable, 0x0, sizeof(hCm3d->abOvlpRegionEnable));
	BKNI_Memset((void*)&hCm3d->stChromaHistData, 0x0, sizeof(hCm3d->stChromaHistData));

	hCm3d->stChromaHistData.stCrCbHist.ulCount = 0;
	hCm3d->eChromaHistType                     = BVDC_ChromaHistType_eInvalid;

	BDBG_LEAVE(BVDC_P_Cm3d_Init);
	return;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Cm3d_ValidateRegion
	( const BVDC_Cm3dRegion         *pRegion,
	  uint32_t                       ulRegionId )
{
	BERR_Code err = BERR_SUCCESS;
	uint32_t i, j;

	BDBG_ENTER(BVDC_P_Cm3d_ValidateRegion);

#if (!BDBG_DEBUG_BUILD)
	BSTD_UNUSED(ulRegionId);
#endif

	/* validate number of PWLs */
	if (pRegion->ulPwlCnt > BVDC_MAX_CM3D_PWL)
	{
		BDBG_ERR(("Region %d's number of PWLs (%d) for this region exceeds maximum (%d)",
			ulRegionId, pRegion->ulPwlCnt, BVDC_MAX_CM3D_PWL));
		return BERR_TRACE(BVDC_ERR_CM3D_INVALID_PARAMETER);
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
	if (pRegion->eColorRegionFormat == BVDC_Cm3dFormat_ePolar)
	{
		if ((pRegion->ulMaxX > BVDC_P_MAX_HUE) || (pRegion->ulMaxY > BVDC_P_MAX_SAT))
		{
			BDBG_ERR(("Region %d's min and max values exceed bounds.", ulRegionId));
			return BERR_TRACE(BVDC_ERR_CM3D_INVALID_PARAMETER);
		}
	}

	/* validate region's PWL points */
	for (i=0; i<pRegion->ulPwlCnt; i++)
	{
		for (j=0; j<BVDC_MAX_CM3D_PWL_POINTS; j++)
		{
			/* validate X points */
			if ((j+1) < BVDC_MAX_CM3D_PWL_POINTS)
			{
				if ((pRegion->astPwl[i].astPwlPoint[j].ulX & BVDC_P_CM3D_PWL_X_BIT_MASK) >
					 (pRegion->astPwl[i].astPwlPoint[j+1].ulX & BVDC_P_CM3D_PWL_X_BIT_MASK))
				{
					BDBG_ERR(("Region %d's X PWL point %d must be less than or equal to the X PWL point %d",
						ulRegionId, j, j+1));
					return BERR_TRACE(BVDC_ERR_CM3D_INVALID_PARAMETER);
				}
			}
		}
	}

	BDBG_LEAVE(BVDC_P_Cm3d_ValidateRegion);
	return err;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Cm3d_ValidateOverlappedRegion
	( const BVDC_Cm3dOverlappedRegion   *pRegion,
	  uint32_t                           ulRegionId )
{
	BERR_Code err = BERR_SUCCESS;
	uint32_t i, j;

	BDBG_ENTER(BVDC_P_Cm3d_ValidateOverlappedRegion);

#if (!BDBG_DEBUG_BUILD)
	BSTD_UNUSED(ulRegionId);
#endif

	if (pRegion->ulPwlCnt > BVDC_MAX_CM3D_OVERLAPPED_PWL)
	{
		BDBG_ERR(("Region %d's number of PWLs (%d) for overlapped region exceeds maximum (%d)",
			ulRegionId, pRegion->ulPwlCnt, BVDC_MAX_CM3D_OVERLAPPED_PWL));
		return BERR_TRACE(BVDC_ERR_CM3D_INVALID_PARAMETER);
	}

	/* validate region's PWL points */
	for (i=0; i<BVDC_MAX_CM3D_OVERLAPPED_PWL; i++)
	{
		for (j=0; j<BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS; j++)
		{
			/* validate X points */
			if ((j+1) < BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS)
			{
				if (pRegion->astPwl[i].astPwlPoint[j].ulX > pRegion->astPwl[i].astPwlPoint[j+1].ulX)
				{
					BDBG_ERR(("Region %d's X PWL point %d must be less than the X PWL point %d",
						ulRegionId, j, j+1));
					return BERR_TRACE(BVDC_ERR_CM3D_INVALID_PARAMETER);
				}
			}

			if (pRegion->astPwl[i].astPwlPoint[j].ulX > BVDC_P_OVERLAPPED_MAX_HUE)
			{
				BDBG_ERR(("Region %d's hue %d exceeds is out of range (%d to %d)",
					ulRegionId,	BVDC_P_OVERLAPPED_MIN_HUE, BVDC_P_OVERLAPPED_MAX_HUE));
				return BERR_TRACE(BVDC_ERR_CM3D_INVALID_PARAMETER);
			}
		}

		/* check if number of PWL points exceed max allowed for overlapped region's PWL */
		if (pRegion->astPwl[i].astPwlPoint[j].ilY || pRegion->astPwl[i].astPwlPoint[j].ulX)
		{
			BDBG_WRN(("Region %d's PWL points %d and above are ignored. Only %d PWL points are allowed for overlapped regions",
				ulRegionId, j, BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS));
		}
	}

	BDBG_LEAVE(BVDC_P_Cm3d_ValidateOverlappedRegion);
	return err;
}

#if (BCHP_VER == BCHP_VER_A0)
/***************************************************************************
 * {private}
 *
 */
static int32_t BVDC_P_Cm3d_GetPwlSlope
	( uint32_t         ulX0,  /* U10 */
	  uint32_t         ulX1,  /* U10 */
	  int32_t          ilY0,  /* S1.8 */
	  int32_t          ilY1 ) /* S1.8 */
{
	int32_t ilDeltaY;
	int32_t ilDeltaX;
	int32_t ilSlope; /* S0.9 */

	/* do fixed point arithmetic */
	ilDeltaY = ((ilY1 - ilY0) << BVDC_P_CM3D_PWL_SLOPE_FBITS) >> BVDC_P_CM3D_PWL_Y_FBITS;

	/* Cast to signed int to preserve signed int slope result below. */
	ilDeltaX = (int32_t)(ulX1 - ulX0);

	/* round up/down using uDeltaX/2 */
	if (ilDeltaY >= 0 )
		ilDeltaY += ilDeltaX/2;
	else
		ilDeltaY -= ilDeltaX/2;

	ilSlope = ilDeltaY/ilDeltaX;

	BDBG_MSG(("0(%d, %d); 1(%d, %d): ilDeltaY = %d, ilDeltaX = %d, ilSlope = %d[or 0x%x]",
		ulX0, ilY0, ulX1, ilY1, ilDeltaY, ilDeltaX, ilSlope, ilSlope));

	return ilSlope;
}
#endif

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Cm3dStoreRegionInfo
	( BVDC_Window_Handle               hWindow,
	  const BVDC_Cm3dRegion           *pstRegion,
	  uint32_t                         ulRegionId  )
{
	BVDC_P_Window_Info     *pNewInfo;
	uint32_t                i, j;
	uint32_t                ulRegData, ulPwlRegData = 0;

	BDBG_ENTER(BVDC_P_Cm3dStoreRegionInfo);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurResource.hCm3d, BVDC_C3D);

	pNewInfo = &hWindow->stNewInfo;

	/* store region info in CM3D object for easy retrieval */
	hWindow->stCurResource.hCm3d->astRegion[ulRegionId] = *pstRegion;

	/* Setup region cfg */
	/* X coordinate */
	ulRegData = BCHP_FIELD_DATA(VIVID_CMP_0_REGION0_X_CFG, PR_SEL, pstRegion->eColorRegionFormat) |
				(BCHP_MASK(VIVID_CMP_0_REGION0_X_CFG, X_MAX) & BCHP_FIELD_DATA(VIVID_CMP_0_REGION0_X_CFG, X_MAX, pstRegion->ulMaxX)) |
				(BCHP_MASK(VIVID_CMP_0_REGION0_X_CFG, X_MIN) & BCHP_FIELD_DATA(VIVID_CMP_0_REGION0_X_CFG, X_MIN, pstRegion->ulMinX));

	pNewInfo->stRegionConfig.aulRegionConfig[ulRegionId * BVDC_P_CM3D_FORMAT_POINTS] = ulRegData;

	/* Y coordinate */
	ulRegData = BCHP_FIELD_DATA(VIVID_CMP_0_REGION0_Y_CFG, ENABLE, pstRegion->bEnable) |
				(BCHP_MASK(VIVID_CMP_0_REGION0_Y_CFG, Y_MAX) & BCHP_FIELD_DATA(VIVID_CMP_0_REGION0_Y_CFG, Y_MAX, pstRegion->ulMaxY)) |
				(BCHP_MASK(VIVID_CMP_0_REGION0_Y_CFG, Y_MIN) & BCHP_FIELD_DATA(VIVID_CMP_0_REGION0_Y_CFG, Y_MIN, pstRegion->ulMinY));

	pNewInfo->stRegionConfig.aulRegionConfig[(ulRegionId * BVDC_P_CM3D_FORMAT_POINTS) + 1] = ulRegData;

	/* setup PWLs */
	for (i=0; i<pstRegion->ulPwlCnt; i++)
	{
		uint32_t ulRegionIndex = ulRegionId *  BVDC_MAX_CM3D_PWL * BVDC_MAX_CM3D_PWL_POINTS;

		/* Calculate and store the slope per PWL point. Note that there are
		 * BVDC_MAX_CM3D_PWL_POINTS - 1 valid slopes. */
		for (j=0; j<BVDC_MAX_CM3D_PWL_POINTS; j++)
		{
			uint32_t ulX0;
			int32_t ilY0;
			uint32_t ulPwlIndex;

#if (BCHP_VER == BCHP_VER_A0)
			int32_t ilSlope = 0; /* S0.9 format 2's complement */
			uint32_t ulX1;
			int32_t ilY1;

			ulX1 = pstRegion->astPwl[i].astPwlPoint[j+1].ulX;
			ilY1 = pstRegion->astPwl[i].astPwlPoint[j+1].ilY;
#endif

			ulX0 = pstRegion->astPwl[i].astPwlPoint[j].ulX;
			ilY0 = pstRegion->astPwl[i].astPwlPoint[j].ilY;

#if (BCHP_VER == BCHP_VER_A0)
			ilSlope = BVDC_P_Cm3d_GetPwlSlope(ulX0, ulX1, ilY0, ilY1);

			ulRegData = (BCHP_MASK(VIVID_CMP_0_PWL0_CFGi, PWL_SLOPE) & BCHP_FIELD_DATA(VIVID_CMP_0_PWL0_CFGi, PWL_SLOPE, ilSlope)) |
						(BCHP_MASK(VIVID_CMP_0_PWL0_CFGi, PWL_Y) & BCHP_FIELD_DATA(VIVID_CMP_0_PWL0_CFGi, PWL_Y, ilY0)) |
						(BCHP_MASK(VIVID_CMP_0_PWL0_CFGi, PWL_X) & BCHP_FIELD_DATA(VIVID_CMP_0_PWL0_CFGi, PWL_X, ulX0));
#else
			ulRegData = (BCHP_MASK(VIVID_CMP_0_PWL0_CFGi, PWL_Y) & BCHP_FIELD_DATA(VIVID_CMP_0_PWL0_CFGi, PWL_Y, ilY0)) |
						(BCHP_MASK(VIVID_CMP_0_PWL0_CFGi, PWL_X) & BCHP_FIELD_DATA(VIVID_CMP_0_PWL0_CFGi, PWL_X, ulX0));

#endif

			ulPwlIndex = ulRegionIndex + (i * BVDC_MAX_CM3D_PWL_POINTS) + j;
			pNewInfo->stRegionConfig.aulPwl[ulPwlIndex] = ulRegData;
		}

		/* setup PWL inputs */
		switch (i)
		{
			case 0:
				ulPwlRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_PWL_IN_SELi, PWL0_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
			case 1:
				ulPwlRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_PWL_IN_SELi, PWL1_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
			case 2:
				ulPwlRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_PWL_IN_SELi, PWL2_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
			case 3:
				ulPwlRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_PWL_IN_SELi, PWL3_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
			case 4:
				ulPwlRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_PWL_IN_SELi, PWL4_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
			case 5:
				ulPwlRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_PWL_IN_SELi, PWL5_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
			case 6:
				ulPwlRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_PWL_IN_SELi, PWL6_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
			case 7:
				ulPwlRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_PWL_IN_SELi, PWL7_IN_SEL, pstRegion->astPwl[i].ePwlInput);
				break;
		}
	}

	/* store PWL input info */
	pNewInfo->stRegionConfig.aulPwlInput[ulRegionId] = ulPwlRegData;

	/* setup gain adjustment */
	for (i=0; i<pstRegion->ulGainCnt; i++)
	{
		ulRegData = (BCHP_MASK(VIVID_CMP_0_ADJ_RSLT0_CFGi, PEAK_GAIN) & BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_RSLT0_CFGi, PEAK_GAIN, pstRegion->astGain[i].ilPeakGain)) |
					(BCHP_MASK(VIVID_CMP_0_ADJ_RSLT0_CFGi, PROD_SEL) & BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_RSLT0_CFGi, PROD_SEL, pstRegion->astGain[i].eAlgo));

		for (j=0; j<pstRegion->astGain[i].ulInputCnt; j++)
		{
			switch (j)
			{
				case 0:
					ulRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_RSLT0_CFGi, GAIN0_SEL,
						pstRegion->astGain[i].aeInput[j]);
					break;
				case 1:
					ulRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_RSLT0_CFGi, GAIN1_SEL,
						pstRegion->astGain[i].aeInput[j]);
					break;
				case 2:
					ulRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_RSLT0_CFGi, GAIN2_SEL,
						pstRegion->astGain[i].aeInput[j]);
					break;
				default:
					BDBG_ERR(("No such gain."));
			}

		}

		/* store gain adjust info */
		pNewInfo->stRegionConfig.aulGainAdj[(ulRegionId * BVDC_MAX_CM3D_GAIN_CONFIGS) + i] = ulRegData;
	}

	ulRegData = BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_OUT_CFGi, PR_SEL, pstRegion->eOutputFormat);

	/* setup gain output */
	ulRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_OUT_CFGi, REG_ALPHA_SEL, pstRegion->eAlpha);
	ulRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_OUT_CFGi, Y_GAIN_SEL, pstRegion->eLumaGain);
	ulRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_OUT_CFGi, Y_OFFSET_SEL, pstRegion->eLumaOffset);
	ulRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_OUT_CFGi, SAT_GAIN_SEL, pstRegion->eSatGain);
	ulRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_OUT_CFGi, CB_OFFSET_SEL, pstRegion->eCbOffset);
	ulRegData |= BCHP_FIELD_DATA(VIVID_CMP_0_ADJ_OUT_CFGi, CR_OFFSET_SEL, pstRegion->eCrOffset);

	/* store gain output info */
	pNewInfo->stRegionConfig.aulGainOutput[ulRegionId] = ulRegData;

	/* enable or disable region */
	hWindow->stCurResource.hCm3d->abRegionEnable[ulRegionId] = (pstRegion->bEnable) ? true : false;

	pNewInfo->bCm3dRegionEnable = BVDC_P_Cm3d_CheckRegionEnables(hWindow->stCurResource.hCm3d);

	pNewInfo->stDirty.stBits.bCm3dRegionAdj = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_P_Cm3dStoreRegionInfo);

	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Cm3dStoreOverlappedRegionInfo
	( BVDC_Window_Handle               hWindow,
	  const BVDC_Cm3dOverlappedRegion *pstRegion,
	  uint32_t                         ulRegionId )
{
	BVDC_P_Window_Info     *pNewInfo;
	uint32_t                i, j;
	uint32_t                ulRegData;

	BDBG_ENTER(BVDC_P_Cm3dStoreOverlappedRegionInfo);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurResource.hCm3d, BVDC_C3D);

	pNewInfo = &hWindow->stNewInfo;

	/* store region info in CM3D object for easy retrieval */
	hWindow->stCurResource.hCm3d->astOvlpRegion[ulRegionId] = *pstRegion;

	/* store region information from user */
	ulRegData = (BCHP_MASK(VIVID_CMP_0_BLUE_STR_CFG, CB_PEAK_GAIN ) & BCHP_FIELD_DATA(VIVID_CMP_0_BLUE_STR_CFG, CB_PEAK_GAIN, pstRegion->ilCbPeakGain)) |
				(BCHP_MASK(VIVID_CMP_0_BLUE_STR_CFG, CR_PEAK_GAIN) & BCHP_FIELD_DATA(VIVID_CMP_0_BLUE_STR_CFG, CR_PEAK_GAIN, pstRegion->ilCrPeakGain)) |
				BCHP_FIELD_DATA(VIVID_CMP_0_BLUE_STR_CFG, PWL1_IN_SEL, pstRegion->astPwl[1].ePwlInput) |
				BCHP_FIELD_DATA(VIVID_CMP_0_BLUE_STR_CFG, PWL0_IN_SEL, pstRegion->astPwl[0].ePwlInput) |
				BCHP_FIELD_DATA(VIVID_CMP_0_BLUE_STR_CFG, PWL_ENABLE, (pstRegion->bEnable == true) ? 1 : 0);

	pNewInfo->stOvlpRegionConfig.ulRegionCfg = ulRegData;

	/* Calculate and store the slope per PWL point. Note that there are
	 * BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS - 1 valid slopes. */
	for (i=0;i<BVDC_MAX_CM3D_OVERLAPPED_PWL; i++)
	{
		for (j=0; j<BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS; j++)
		{
			uint32_t ulX0;
			int32_t ilY0;

#if (BCHP_VER == BCHP_VER_A0)
			int32_t ilSlope = 0; /* S0.9 format 2's complement */
			uint32_t ulX1;
			int32_t ilY1;

			ulX1 = pstRegion->astPwl[i].astPwlPoint[j+1].ulX;
			ilY1 = pstRegion->astPwl[i].astPwlPoint[j+1].ilY;
#endif

			ulX0 = pstRegion->astPwl[i].astPwlPoint[j].ulX;
			ilY0 = pstRegion->astPwl[i].astPwlPoint[j].ilY;

#if (BCHP_VER == BCHP_VER_A0)
			/* last PWL point pair does not have a slope */
			if (j == BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS-1)
				ilSlope = 0;
			else
				ilSlope = BVDC_P_Cm3d_GetPwlSlope(ulX0, ulX1, ilY0, ilY1);

 			ulRegData = (BCHP_MASK(VIVID_CMP_0_BL_PWL0_CFGi, PWL_SLOPE) & BCHP_FIELD_DATA(VIVID_CMP_0_BL_PWL0_CFGi, PWL_SLOPE, ilSlope)) |
						(BCHP_MASK(VIVID_CMP_0_BL_PWL0_CFGi, PWL_Y) & BCHP_FIELD_DATA(VIVID_CMP_0_BL_PWL0_CFGi, PWL_Y, ilY0)) |
						(BCHP_MASK(VIVID_CMP_0_BL_PWL0_CFGi, PWL_X) & BCHP_FIELD_DATA(VIVID_CMP_0_BL_PWL0_CFGi, PWL_X, ulX0));
#else
			ulRegData = (BCHP_MASK(VIVID_CMP_0_PWL0_CFGi, PWL_Y) & BCHP_FIELD_DATA(VIVID_CMP_0_PWL0_CFGi, PWL_Y, ilY0)) |
						(BCHP_MASK(VIVID_CMP_0_PWL0_CFGi, PWL_X) & BCHP_FIELD_DATA(VIVID_CMP_0_PWL0_CFGi, PWL_X, ulX0));

#endif

			pNewInfo->stOvlpRegionConfig.aulPwl[(i * BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS) + j] = ulRegData;
		}
	}

	/* enable or disable region */
	hWindow->stCurResource.hCm3d->abOvlpRegionEnable[ulRegionId] = (pstRegion->bEnable) ? true : false;

	pNewInfo->bCm3dOverlapRegionEnable = BVDC_P_Cm3d_CheckOverlapRegionEnable(hWindow->stCurResource.hCm3d);

	pNewInfo->stDirty.stBits.bCm3dOverlapRegionAdj = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_P_Cm3dStoreOverlappedRegionInfo);

	return;
}


/*************************************************************************
 *	{secret}
 *	BVDC_P_Cm3d_BuildRegionRul_isr
 *	Builds CM3D Region
 **************************************************************************/
static void BVDC_P_Cm3d_BuildRegionRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BVDC_P_Window_Info           *pCurInfo;
	uint32_t                      ulRegData;
	uint32_t                      i, j;

	BDBG_ENTER(BVDC_P_Cm3d_BuildRegionRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pCurInfo = &hWindow->stCurInfo;

	/* rebuild all CM3D if compositor initial or window initial */
	BDBG_OBJECT_ASSERT(hWindow->stCurResource.hCm3d, BVDC_C3D);

	/* Disable all regions */
	for (i=0; i< BVDC_MAX_CM3D_REGIONS; i++)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_REGION0_Y_CFG + (i * sizeof(uint32_t) *
											BVDC_P_CM3D_FORMAT_POINTS));
		*pList->pulCurrent++ = BCHP_FIELD_DATA(VIVID_CMP_0_REGION0_Y_CFG, ENABLE, 0);
	}

	if (pCurInfo->bCm3dRegionEnable)
	{
		/* VIVID enable control */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_VIVID_ENA);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(VIVID_CMP_0_VIVID_ENA, AUTO_DISA, Continuous) |
			BCHP_FIELD_ENUM(VIVID_CMP_0_VIVID_ENA, ENABLE, Enable);

		/* Configure PWLs for all desired regions */
		for (i=0; i<BVDC_MAX_CM3D_REGIONS; i++)
		{
			/* check if region is to be enabled and if so include region in RUL */
			if (hWindow->stCurResource.hCm3d->abRegionEnable[i] == true)
			{
				/* See PWL register arrangement */
				for (j=0; j<hWindow->stCurResource.hCm3d->astRegion[i].ulPwlCnt; j++)
				{
					uint32_t ulPwlTableIndex;
					uint32_t ulRegionBase = i * BVDC_MAX_CM3D_PWL * BVDC_MAX_CM3D_PWL_POINTS;
					uint32_t ulPwlRegBase = (j * BVDC_MAX_CM3D_REGIONS + i) * BVDC_MAX_CM3D_PWL_POINTS;

					*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_MAX_CM3D_PWL_POINTS);
					*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_PWL0_CFGi_ARRAY_BASE +
											(ulPwlRegBase * sizeof(uint32_t)));

					ulPwlTableIndex = ulRegionBase + (j * BVDC_MAX_CM3D_PWL_POINTS);
					BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pCurInfo->stRegionConfig.aulPwl[ulPwlTableIndex],
							BVDC_MAX_CM3D_PWL_POINTS * sizeof(uint32_t));
					pList->pulCurrent += BVDC_MAX_CM3D_PWL_POINTS;
				}

				/* set PWL input */
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_PWL_IN_SELi_ARRAY_BASE +
										(i * sizeof(uint32_t)));
				*pList->pulCurrent++ = pCurInfo->stRegionConfig.aulPwlInput[i];


				/* set gain adj */
				for (j=0; j<BVDC_MAX_CM3D_GAIN_CONFIGS; j++)
				{
					uint32_t ulGainAdjTableIndex = j * BVDC_MAX_CM3D_REGIONS + i;

					*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
					*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_ADJ_RSLT0_CFGi_ARRAY_BASE +
											(ulGainAdjTableIndex * sizeof(uint32_t)));
					*pList->pulCurrent++ =
						pCurInfo->stRegionConfig.aulGainAdj[(i * BVDC_MAX_CM3D_GAIN_CONFIGS) + j];
				}

				/* set gain output */
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_ADJ_OUT_CFGi_ARRAY_BASE +
										i * sizeof(uint32_t));
				*pList->pulCurrent++ = pCurInfo->stRegionConfig.aulGainOutput[i];


				/* set region cfg and enable */
				*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CM3D_FORMAT_POINTS);
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_REGION0_X_CFG +
					(i * BVDC_P_CM3D_FORMAT_POINTS) * sizeof(uint32_t));
				BKNI_Memcpy((void*)pList->pulCurrent,
							(void*)&pCurInfo->stRegionConfig.aulRegionConfig[i * BVDC_P_CM3D_FORMAT_POINTS],
							BVDC_P_CM3D_FORMAT_POINTS * sizeof(uint32_t));
				pList->pulCurrent += BVDC_P_CM3D_FORMAT_POINTS;
			}
		}

		/* Allow CM3D'd pixels to flow through */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BYPASS_CTRL);

		ulRegData = ~(BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_CB_OFFSET) |
					 BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_CR_OFFSET) |
					 BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_HUE_OFFSET) |
					 BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_SAT_OFFSET) |
					 BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_SAT_GAIN) |
					 BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_Y_OFFSET) |
					 BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_Y_GAIN) |
					 BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_ALPHA) |
					 BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_VIVID));

		*pList->pulCurrent++ = BRDC_OP_VAR_AND_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
		*pList->pulCurrent++ = ulRegData;

		*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_1);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BYPASS_CTRL);

		ulRegData = 0;

		if (pCurInfo->stSplitScreenSetting.eCm3d != BVDC_SplitScreenMode_eDisable)
		{
			uint32_t ulBoundary = hWindow->stAdjDstRect.ulWidth / 2;
			uint32_t ulDemoSide;
			uint32_t i;

			*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_CTRL);

			for (i=0; i<BVDC_MAX_CM3D_REGIONS; i++)
			{
				if (hWindow->stCurResource.hCm3d->abRegionEnable[i] == true)
				{
					ulRegData |= (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_ALPHA) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_ALPHA, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_CM3D_ALPHA_Demo)) |
								 (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_Y_GAIN) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_Y_GAIN, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_CM3D_Y_GAIN_Demo)) |
								 (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_Y_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_Y_OFFSET, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_CM3D_Y_OFFSET_Demo)) |
								 (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_SAT_GAIN) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_SAT_GAIN, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_CM3D_SAT_GAIN_Demo)) |
								 (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_SAT_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_SAT_OFFSET, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_CM3D_SAT_OFFSET_Demo)) |
								 (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_HUE_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_HUE_OFFSET, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_CM3D_HUE_OFFSET_Demo)) |
								 (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_CR_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_CR_OFFSET, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_CM3D_CR_OFFSET_Demo)) |
								 (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_CB_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_CB_OFFSET, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_CM3D_CB_OFFSET_Demo));
					break;
				}
			}

			*pList->pulCurrent++ = BRDC_OP_VAR_OR_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
			*pList->pulCurrent++ = ulRegData;

			*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_1);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_CTRL);

			ulDemoSide = (pCurInfo->stSplitScreenSetting.eCm3d == BVDC_SplitScreenMode_eLeft) ?
				                   BCHP_VIVID_CMP_0_DEMO_SETTING_DEMO_L_R_LEFT :
				                   BCHP_VIVID_CMP_0_DEMO_SETTING_DEMO_L_R_RIGHT;

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_SETTING);
			*pList->pulCurrent++ = BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_SETTING, DEMO_L_R, ulDemoSide) |
								   BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_SETTING, DEMO_BOUNDARY, ulBoundary);
		}
		else
		{
			*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_CTRL);

			ulRegData = ~( BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_ALPHA) |
						   BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_Y_GAIN) |
						   BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_Y_OFFSET) |
						   BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_SAT_GAIN) |
						   BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_SAT_OFFSET) |
						   BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_HUE_OFFSET) |
						   BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_CR_OFFSET) |
						   BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_CM3D_CB_OFFSET) );

			*pList->pulCurrent++ = BRDC_OP_VAR_AND_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
			*pList->pulCurrent++ = ulRegData;

			*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_1);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_CTRL);
		}
	}
	else
	{
		/* Prevent CM3D'd pixels to flow through */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BYPASS_CTRL);

		ulRegData = (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_CB_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_CB_OFFSET, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_CM3D_CB_OFFSET_Bypass)) |
					 (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_CR_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_CR_OFFSET, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_CM3D_CR_OFFSET_Bypass)) |
					 (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_HUE_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_HUE_OFFSET, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_CM3D_HUE_OFFSET_Bypass)) |
					 (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_SAT_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_SAT_OFFSET, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_CM3D_SAT_OFFSET_Bypass) )|
					 (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_SAT_GAIN) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_SAT_GAIN, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_CM3D_SAT_GAIN_Bypass)) |
					 (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_Y_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_Y_OFFSET, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_CM3D_Y_OFFSET_Bypass)) |
					 (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_Y_GAIN) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_Y_GAIN, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_CM3D_Y_GAIN_Bypass)) |
					 (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_ALPHA) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_CM3D_ALPHA, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_CM3D_ALPHA_Bypass));

		if (pCurInfo->bContrastStretch == false)
			ulRegData |= BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_VIVID) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_VIVID, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_VIVID_Bypass);

		*pList->pulCurrent++ = BRDC_OP_VAR_OR_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
		*pList->pulCurrent++ = ulRegData;

		*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_1);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BYPASS_CTRL);
	}

	BDBG_LEAVE(BVDC_P_Cm3d_BuildRegionRul_isr);

	return;
}

/*************************************************************************
 *	{secret}
 *	BVDC_P_Cm3d_BuildRul_isr
 *	Builds CM3D Overlap Region
 **************************************************************************/
static void BVDC_P_Cm3d_BuildOverlapRegionRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BVDC_P_Window_Info           *pCurInfo;
	uint32_t                      ulRegData;

	BDBG_ENTER(BVDC_P_Cm3d_BuildOverlapRegionRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pCurInfo = &hWindow->stCurInfo;

	/* rebuild all CM3D if compositor initial or window initial */
	BDBG_OBJECT_ASSERT(hWindow->stCurResource.hCm3d, BVDC_C3D);

	/* Disable active region */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BLUE_STR_CFG);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VIVID_CMP_0_BLUE_STR_CFG, PWL_ENABLE, 0);

	if (pCurInfo->bCm3dOverlapRegionEnable)
	{
		/* VIVID enable control */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_VIVID_ENA);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(VIVID_CMP_0_VIVID_ENA, AUTO_DISA, Continuous) |
			BCHP_FIELD_ENUM(VIVID_CMP_0_VIVID_ENA, ENABLE, Enable);

		/* Configure PWLs */
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_MAX_CM3D_OVERLAPPED_PWL * BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BL_PWL0_CFGi_ARRAY_BASE);
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pCurInfo->stOvlpRegionConfig.aulPwl,
				(BVDC_MAX_CM3D_OVERLAPPED_PWL * BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS) * sizeof(uint32_t));
		pList->pulCurrent += (BVDC_MAX_CM3D_OVERLAPPED_PWL * BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS);

		/* Set Chroma Peak gain values and PWL input types and Enable*/
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BLUE_STR_CFG);
		*pList->pulCurrent++ = pCurInfo->stOvlpRegionConfig.ulRegionCfg;

		/* Allow CM3D'd pixels to flow */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BYPASS_CTRL);

		ulRegData = ~(BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_BLS_CR_OFFSET) |
						BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_BLS_CB_OFFSET) |
						BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_VIVID));

		*pList->pulCurrent++ = BRDC_OP_VAR_AND_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
		*pList->pulCurrent++ = ulRegData;

		*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_1);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BYPASS_CTRL);

		ulRegData = 0;

		if (pCurInfo->stSplitScreenSetting.eCm3d != BVDC_SplitScreenMode_eDisable)
		{
			uint32_t ulBoundary = hWindow->stAdjDstRect.ulWidth / 2;
			uint32_t ulDemoSide;

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_CTRL);

			if (hWindow->stCurResource.hCm3d->abOvlpRegionEnable[0]) /* only 1 region */
			{
				ulRegData |= (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_BLS_CR_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_BLS_CR_OFFSET, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_BLS_CR_OFFSET_Demo)) |
							 (BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_BLS_CB_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_BLS_CB_OFFSET, BCHP_VIVID_CMP_0_DEMO_CTRL_DEMO_ENA_BLS_CB_OFFSET_Demo));
			}

			*pList->pulCurrent++ = BRDC_OP_VAR_OR_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
			*pList->pulCurrent++ = ulRegData;

			*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_1);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_CTRL);

			ulDemoSide = (pCurInfo->stSplitScreenSetting.eCm3d == BVDC_SplitScreenMode_eLeft) ?
				                   BCHP_VIVID_CMP_0_DEMO_SETTING_DEMO_L_R_LEFT :
				                   BCHP_VIVID_CMP_0_DEMO_SETTING_DEMO_L_R_RIGHT;

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_SETTING);
			*pList->pulCurrent++ = BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_SETTING, DEMO_L_R, ulDemoSide) |
								   BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_SETTING, DEMO_BOUNDARY, ulBoundary);
		}
		else
		{
			*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_CTRL);

			ulRegData = ~( BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_BLS_CR_OFFSET) |
						   BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_BLS_CB_OFFSET) );

			*pList->pulCurrent++ = BRDC_OP_VAR_AND_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
			*pList->pulCurrent++ = ulRegData;

			*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_1);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_CTRL);
		}
	}
	else
	{
		/* Prevent CM3D'd pixels to flow */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BYPASS_CTRL);

		ulRegData = (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_BLS_CR_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_BLS_CR_OFFSET, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_BLS_CR_OFFSET_Bypass)) |
					 (BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_BLS_CB_OFFSET) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_BLS_CB_OFFSET, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_BLS_CB_OFFSET_Bypass));

		if (pCurInfo->bContrastStretch == false)
		{
			ulRegData |= BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_VIVID) & BCHP_FIELD_DATA(VIVID_CMP_0_BYPASS_CTRL, BYPASS_VIVID, BCHP_VIVID_CMP_0_BYPASS_CTRL_BYPASS_VIVID_Bypass);
		}

		*pList->pulCurrent++ = BRDC_OP_VAR_OR_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
		*pList->pulCurrent++ = ulRegData;

		*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_1);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_BYPASS_CTRL);
	}

	BDBG_LEAVE(BVDC_P_Cm3d_BuildOverlapRegionRul_isr);

	return;
}


/*************************************************************************
 *	{secret}
 *	BVDC_P_Cm3d_BuildRul_isr
 *	Builds CM3D block
 **************************************************************************/
void BVDC_P_Cm3d_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList,
	  bool                         bInitial )
{
	BVDC_P_Window_Info            *pCurInfo;
	BVDC_P_Window_DirtyBits       *pCurDirty;

	BDBG_ENTER(BVDC_P_Cm3d_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurResource.hCm3d, BVDC_C3D);
	BDBG_ASSERT(BVDC_P_WindowId_eComp0_V0 == hWindow->eId);

	/* rebuild all CM3D if compositor initial or window initial */
	pCurDirty = &hWindow->stCurInfo.stDirty;
	pCurInfo = &hWindow->stCurInfo;
	bInitial |= hWindow->stCurResource.hCm3d->bInitial;

	if(pCurDirty->stBits.bCm3dRegionAdj || bInitial)
	{
		BVDC_P_Cm3d_BuildRegionRul_isr(hWindow, pList);
		pCurDirty->stBits.bCm3dRegionAdj = BVDC_P_CLEAN;
	}

	if(pCurDirty->stBits.bCm3dOverlapRegionAdj || bInitial)
	{
		BVDC_P_Cm3d_BuildOverlapRegionRul_isr(hWindow, pList);
		pCurDirty->stBits.bCm3dOverlapRegionAdj = BVDC_P_CLEAN;
	}

	/* Collect Histogram Data */
	if ((pCurDirty->stBits.bChromaHistoRect == BVDC_P_CLEAN) && pCurInfo->bChromaHistEnable)
	{
		uint32_t ulReg;
		ulReg = BREG_Read32(hWindow->stCurResource.hCm3d->hReg, BCHP_VIVID_CMP_0_CHIST_EN);

		/* Ascertain that the enable bit is 0. This means that the chroma
		   histogram collection has stopped */
		if ((BCHP_GET_FIELD_DATA(ulReg, VIVID_CMP_0_CHIST_EN, ENABLE) == BCHP_VIVID_CMP_0_CHIST_EN_ENABLE_DISABLE) &&
			(BCHP_GET_FIELD_DATA(ulReg, VIVID_CMP_0_CHIST_STATUS, INVALID) == BCHP_VIVID_CMP_0_CHIST_STATUS_INVALID_VALID))
		{
			hWindow->stCurResource.hCm3d->eChromaHistType = pCurInfo->stChromaRect.eType;

			BVDC_P_Cm3d_UpdateChromaHistData_isr(hWindow->stCurResource.hCm3d);

			if (pCurInfo->bChromaHistEnable)
				pCurDirty->stBits.bChromaHistoRect = BVDC_P_DIRTY;
		}
	}

	/* Build RUL for chroma histogram collection */
	if (pCurDirty->stBits.bChromaHistoRect || bInitial)
	{
		BVDC_P_Cm3d_BuildHistRul_isr(hWindow, pList);
		pCurDirty->stBits.bChromaHistoRect = BVDC_P_CLEAN;
	}

	hWindow->stCurResource.hCm3d->bInitial = false;

	BDBG_LEAVE(BVDC_P_Cm3d_BuildRul_isr);

	return;
}

/*************************************************************************
 *  {private}
 *
 *
 **************************************************************************/
void BVDC_P_Cm3d_GetChromaHistogramData
	( const BVDC_P_Cm3d_Handle         hCm3d,
	  BVDC_ChromaStatus               *pStatus )
{
	BDBG_ENTER(BVDC_P_Cm3d_GetChromaHistogramData);
	BDBG_OBJECT_ASSERT(hCm3d, BVDC_C3D);

	BKNI_EnterCriticalSection();

	/* populate user struct with latest histogram data */
	*pStatus = hCm3d->stChromaHistData;

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVDC_P_Cm3d_GetChromaHistogramData);

	return;
}

#endif

/* End of File */
