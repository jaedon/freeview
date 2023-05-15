/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_hist_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/35 $
 * $brcm_Date: 12/1/11 4:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_hist_priv.c $
 * 
 * Hydra_Software_Devel/35   12/1/11 4:32p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 * 
 * Hydra_Software_Devel/SW7425-968/1   11/3/11 2:28p yuxiaz
 * SW7425-968: Use BVDC_P_Rect in VDC.
 * 
 * Hydra_Software_Devel/34   11/11/10 7:28p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 * 
 * Hydra_Software_Devel/33   6/18/10 4:20p rpan
 * SW7400-2763: New BVN resource management.
 * 
 * Hydra_Software_Devel/32   5/7/10 7:11p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 * 
 * Hydra_Software_Devel/31   4/19/10 10:16p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/30   4/7/10 11:31a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/29   4/5/10 4:07p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/28   3/13/09 5:29p syang
 * PR 52376, PR 41481, PR 44041:
 * 1). exec user buf returning to hBuffer before shut-down code in
 * writer_isr; 2). clean up mad-delay handling code and add handling to
 * all settings in pic-node that are delayed by mad; 3). 444 SCL no-
 * longer uses BVN_IN to align-up for half-band; 4). add pic-node rects
 * dump feature as setting lbox_0_scratch to 2 (1 still printing mpeg pic
 * info).
 *
 * Hydra_Software_Devel/27   3/9/09 2:58p tdo
 * PR51106: Fix overflow problem in calculating histogram threshold
 *
 * Hydra_Software_Devel/26   3/4/09 4:11p tdo
 * PR51106: Support table base dynamic contrast algorithm.  Change
 * precision of level threshold for histogram data.
 *
 * Hydra_Software_Devel/25   1/16/09 4:19p tdo
 * PR46529, PR51020: Use correct current writer node to store histogram
 * and completely freeze APL when source mute
 *
 * Hydra_Software_Devel/24   1/15/09 1:37p tdo
 * PR46529, PR51020: Fix compiling error
 *
 * Hydra_Software_Devel/23   1/15/09 1:15p tdo
 * PR46529, PR51020: Implement const delay for histogram for dynamic
 * contrast use, and avoid histogram update when source freezed
 *
 * Hydra_Software_Devel/22   1/5/09 6:54p tdo
 * PR48642: Correctly set threshold size for interlace case
 *
 * Hydra_Software_Devel/21   11/19/08 4:01p pntruong
 * PR49309: Need to handle for vdec oversample size.
 *
 * Hydra_Software_Devel/20   11/12/08 3:13p tdo
 * PR48642: Provide clipping rect for histo luma region
 *
 * Hydra_Software_Devel/19   11/4/08 3:50p tdo
 * PR48643: return correct pixel count to nexus for APL calculatioin
 *
 * Hydra_Software_Devel/18   10/27/08 5:37p syang
 * PR 46477: back off SetEnable in in BuildRul_isr according to GetOps (it
 * causes window to disapper on 7400 D0)
 *
 * Hydra_Software_Devel/17   10/24/08 4:07p syang
 * PR 46477: in BuildRul_isr only disable in shadow regs  according to
 * GetOps (some SetEnable_isr does HW reset)
 *
 * Hydra_Software_Devel/16   10/24/08 11:05a syang
 * PR 46477: in BuildRul_isr reset enable bit according to GetOps
 *
 * Hydra_Software_Devel/15   7/30/08 11:35a tdo
 * PR45181: Implement New Dynamic Contrast Algorithm
 *
 * Hydra_Software_Devel/14   6/30/08 3:57p tdo
 * PR39333: Set HIST V_SIZE and OFFSET correct number of lines input to
 * the block
 *
 * Hydra_Software_Devel/13   6/23/08 3:43p tdo
 * PR43672: clear dirty bit and defer HIST config until vnet mode is
 * active to avoid app killed when switching input src
 *
 * Hydra_Software_Devel/12   6/18/08 4:36p tdo
 * PR39333: Use correct rectangle for HIST when connect to CMP side
 *
 * Hydra_Software_Devel/11   6/17/08 5:28p tdo
 * PR39333: Connect HIST at reader side for VDEC source as a work around
 *
 * Hydra_Software_Devel/10   6/9/08 8:17p tdo
 * PR39333: Hist bringup
 *
 * Hydra_Software_Devel/9   4/25/08 1:50p pntruong
 * PR39333: Updated with feedbacks from DavidE.
 *
 * Hydra_Software_Devel/8   4/14/08 10:54a tdo
 * PR39333: Add missing intruction to build HIST rul
 *
 * Hydra_Software_Devel/7   4/11/08 2:11p tdo
 * PR39333: Properly handle Hist in vnet
 *
 * Hydra_Software_Devel/6   4/2/08 9:10p tdo
 * PR39333: Use static const table
 *
 * Hydra_Software_Devel/5   4/2/08 4:31p tdo
 * PR39333: Correctly programming HIST core
 *
 * Hydra_Software_Devel/4   3/24/08 4:29p tdo
 * PR39333: Passing Hist handle instead of its pointer to follow the
 * standard.  Using the interlace flag from source instead of compositor
 * when setting the size.
 *
 * Hydra_Software_Devel/3   3/21/08 5:08p tdo
 * PR39333: More Hist implementation for 3548
 *
 * Hydra_Software_Devel/2   3/21/08 12:10p tdo
 * PR39333: More implementation of HIST block
 *
 * Hydra_Software_Devel/1   3/19/08 5:29p tdo
 * PR39333: Implement HIST core
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "brdc.h"
#include "bvdc.h"
#include "bchp_vnet_b.h"
#include "bvdc_resource_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_compositor_priv.h"

#if (BVDC_P_SUPPORT_HIST_VER >= BVDC_P_SUPPORT_HIST_VER_2)
#include "bchp_hist.h"

BDBG_MODULE(BVDC_HIST);
BDBG_OBJECT_ID(BVDC_HST);

/* This table mapped from VDC bin selects to HW numbins */
static const BVDC_P_Hist_NumBins s_aHistNumBins[] =
{
	{ BCHP_HIST_NUMBINS_NUMBINS_BIN_16, 16 },
	{ BCHP_HIST_NUMBINS_NUMBINS_BIN_32, 32 },
	{ BCHP_HIST_NUMBINS_NUMBINS_BIN_64, 64 },
};

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_Create
 *
 * called by BVDC_Open only
 */
BERR_Code BVDC_P_Hist_Create
	( BVDC_P_Hist_Handle               *phHist,
	  BVDC_P_HistId                     eHistId,
	  BREG_Handle                       hRegister,
	  BVDC_P_Resource_Handle            hResource )
{
	BVDC_P_HistContext *pHist;
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Hist_Create);

	/* in case creation failed */
	BDBG_ASSERT(phHist);
	*phHist = NULL;

	pHist = (BVDC_P_HistContext *)
		(BKNI_Malloc(sizeof(BVDC_P_HistContext)));
	if( pHist )
	{
		/* init the context */
		BKNI_Memset((void*)pHist, 0x0, sizeof(BVDC_P_HistContext));
		BDBG_OBJECT_SET(pHist, BVDC_HST);
		pHist->eId          = eHistId;
		pHist->hRegister    = hRegister;
		pHist->ulRegOffset  = 0;
		pHist->hWindow      = NULL;
		pHist->bInitial     = true;

		/* init the SubRul sub-module */
		BVDC_P_SubRul_Init(&(pHist->SubRul), BVDC_P_Hist_MuxAddr(pHist),
			0, BVDC_P_DrainMode_eNone, 0, hResource);

		*phHist = pHist;
	}

	BDBG_LEAVE(BVDC_P_Hist_Create);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_Destroy
 *
 * called by BVDC_Close only
 */
BERR_Code BVDC_P_Hist_Destroy
	( BVDC_P_Hist_Handle                hHist )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Hist_Destroy);
	BDBG_OBJECT_ASSERT(hHist, BVDC_HST);

	BDBG_OBJECT_DESTROY(hHist, BVDC_HST);
	/* it is gone afterwards !!! */
	BKNI_Free((void*)hHist);

	BDBG_LEAVE(BVDC_P_Hist_Destroy);
	return BERR_TRACE(eResult);
}


/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_AcquireConnect
 *
 * It is called by BVDC_Window_Validate after changing from diableBox to
 * enablingBox .
 */
BERR_Code BVDC_P_Hist_AcquireConnect
	( BVDC_P_Hist_Handle                hHist,
	  BVDC_Window_Handle                hWindow)
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Hist_AcquireConnect);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hHist->hWindow = hWindow;

	BDBG_LEAVE(BVDC_P_Hist_AcquireConnect);
	return BERR_TRACE(eResult);
}


/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_ReleaseConnect_isr
 *
 * It is called after window decided that hist is no-longer used by HW in
 * its vnet mode (i.e. it is really shut down and teared off from vnet).
 */
BERR_Code BVDC_P_Hist_ReleaseConnect_isr
	( BVDC_P_Hist_Handle               *phHist )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Hist_ReleaseConnect_isr);

	/* handle validation */
	BDBG_OBJECT_ASSERT(*phHist, BVDC_HST);

	BVDC_P_SubRul_UnsetVnet_isr(&((*phHist)->SubRul));
	BVDC_P_Resource_ReleaseHandle_isr(
		BVDC_P_SubRul_GetResourceHandle_isr(&(*phHist)->SubRul),
		BVDC_P_ResourceType_eHist, (void *)(*phHist));

	/* this makes win to stop calling hist code */
	*phHist = NULL;

	BDBG_LEAVE(BVDC_P_Hist_ReleaseConnect_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_UpdateHistData_isr
 *
 * called by BVDC_Window_BuildRul_isr at every src vsync. It reads histogram
 * data from HW into hist structure
 */
void BVDC_P_Hist_UpdateHistData_isr
	( BVDC_P_Hist_Handle                hHist )
{
	uint32_t id;
	uint32_t ulReg;
	BVDC_LumaStatus *pStats = &hHist->stHistData;

	BDBG_OBJECT_ASSERT(hHist, BVDC_HST);

	ulReg = BCHP_FIELD_ENUM(HIST_SW_READ, READING, READ);
	BREG_Write32(hHist->hRegister, BCHP_HIST_SW_READ, ulReg);

	ulReg = BREG_Read32(hHist->hRegister, BCHP_HIST_RD_MIN);
	if((BCHP_GET_FIELD_DATA(ulReg, HIST_RD_MIN, INVALID)  == BCHP_HIST_RD_MIN_INVALID_INVALID) ||
	   (BCHP_GET_FIELD_DATA(ulReg, HIST_RD_MIN, NOTFOUND) == BCHP_HIST_RD_MIN_NOTFOUND_NF))
	{
		pStats->ulMin = 0;
	}
	else
	{
		pStats->ulMin = BCHP_GET_FIELD_DATA(ulReg, HIST_RD_MIN, MIN);
	}

	ulReg = BREG_Read32(hHist->hRegister, BCHP_HIST_RD_MAX);
	if((BCHP_GET_FIELD_DATA(ulReg, HIST_RD_MAX, INVALID)  == BCHP_HIST_RD_MAX_INVALID_INVALID) ||
	   (BCHP_GET_FIELD_DATA(ulReg, HIST_RD_MAX, NOTFOUND) == BCHP_HIST_RD_MAX_NOTFOUND_NF))
	{
		pStats->ulMax = 0;
	}
	else
	{
		pStats->ulMax = BCHP_GET_FIELD_DATA(ulReg, HIST_RD_MAX, MAX);
	}

	ulReg = BREG_Read32(hHist->hRegister, BCHP_HIST_STATUS);
	if(BCHP_GET_FIELD_DATA(ulReg, HIST_STATUS, INVALID) == BCHP_HIST_STATUS_INVALID_VALID)
	{
		pStats->ulSum = BREG_Read32(hHist->hRegister, BCHP_HIST_RD_APL);
	}
	else
	{
		pStats->ulSum = 0;
	}

	for(id = 0; id < BVDC_LUMA_HISTOGRAM_COUNT; id++)
	{
		ulReg = BREG_Read32(hHist->hRegister, BCHP_HIST_RD_BINi_ARRAY_BASE + (id * 4));
		if((BCHP_GET_FIELD_DATA(ulReg, HIST_RD_BINi, INVALID) == BCHP_HIST_RD_BINi_INVALID_INVALID) ||
		   (BCHP_GET_FIELD_DATA(ulReg, HIST_RD_BINi, NOT_USED) == BCHP_HIST_RD_BINi_NOT_USED_NOT_USED))
		{
			pStats->aulHistogram[id] = 0;
		}
		else
		{
			pStats->aulHistogram[id] = BCHP_GET_FIELD_DATA(ulReg, HIST_RD_BINi, COUNT);
		}
	}

	for(id = 0; id < BVDC_LUMA_HISTOGRAM_LEVELS; id++)
	{
		ulReg = BREG_Read32(hHist->hRegister, BCHP_HIST_RD_LEVELi_ARRAY_BASE + (id * 4));
		if((BCHP_GET_FIELD_DATA(ulReg, HIST_RD_LEVELi, INVALID) == BCHP_HIST_RD_LEVELi_INVALID_INVALID) ||
		   (BCHP_GET_FIELD_DATA(ulReg, HIST_RD_LEVELi, NOTFOUND) == BCHP_HIST_RD_LEVELi_NOTFOUND_NF))
		{
			pStats->aulLevelStats[id] = 0;
		}
		else
		{
			pStats->aulLevelStats[id] = BCHP_GET_FIELD_DATA(ulReg, HIST_RD_LEVELi, LEVEL);
		}
	}

	ulReg = BCHP_FIELD_ENUM(HIST_SW_READ, READING, IDLE);
	BREG_Write32(hHist->hRegister, BCHP_HIST_SW_READ, ulReg);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_P_Hist_SetEnable_isr
	( BVDC_P_ListInfo                  *pList,
	  bool                              bEnable )
{
	BDBG_ENTER(BVDC_P_Hist_SetEnable_isr);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HIST_EN);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(HIST_EN, ENABLE, bEnable);

	BDBG_MSG(("Set Hist %s", bEnable ? "true" : "false"));

	BDBG_LEAVE(BVDC_P_Hist_SetEnable_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_BuildCfgRul_isr
 *
 * called by BVDC_Window_BuildRul_isr once dirty bit is set. It builds RUL for
 * Hist HW module.
 */
static void BVDC_P_Hist_BuildCfgRul_isr
	( BVDC_P_Hist_Handle                hHist,
	  const BVDC_Window_Handle          hWindow,
	  BVDC_P_ListInfo                  *pList )
{
	const BVDC_P_Window_Info *pCurInfo;
	uint32_t ulRulOffset;
	const BVDC_P_Hist_NumBins *pNumBin;
	BVDC_ClipRect stUsrClipRect;
	BVDC_P_Rect stHistoRect;
	bool bInterlaced;
	uint32_t aulLevelThres[BVDC_LUMA_HISTOGRAM_LEVELS];
	uint32_t id;
	uint32_t ulWidth, ulHeight;

	BDBG_ENTER(BVDC_P_Hist_BuildCfgRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hHist, BVDC_HST);
	pCurInfo = &hWindow->stCurInfo;
	BDBG_ASSERT(pCurInfo->stLumaRect.eNumBins < (sizeof(s_aHistNumBins) / sizeof(BVDC_P_Hist_NumBins)));

	pNumBin = &s_aHistNumBins[pCurInfo->stLumaRect.eNumBins];

	ulRulOffset = hHist->ulRegOffset;

	if(hWindow->stCurInfo.bHistAtSrc)
	{
		ulWidth  = BVDC_P_OVER_SAMPLE(hWindow->stCurInfo.hSource->ulSampleFactor,
			hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->ulWidth);
		ulHeight = hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->ulHeight;
		bInterlaced = hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced;
	}
	else
	{
		ulWidth = hWindow->stCurInfo.stDstRect.ulWidth;
		ulHeight = hWindow->stCurInfo.stDstRect.ulHeight;
		bInterlaced = hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced;
	}

	if(hWindow->stCurInfo.bLumaRectUserSet)
	{
		stUsrClipRect = pCurInfo->stLumaRect.stRegion;
	}
	else
	{
		/* If usr region is not set, HIST can be enabled by dynamic contrast */
		/* therefore, use default clipping of 0, meaning full source size */
		stUsrClipRect.ulLeft   = 0;
		stUsrClipRect.ulRight  = 0;
		stUsrClipRect.ulTop    = 0;
		stUsrClipRect.ulBottom = 0;
	}
	BVDC_P_CalculateRect_isr(&stUsrClipRect, ulWidth, ulHeight, bInterlaced, &stHistoRect);

	for(id = 0; id < BVDC_LUMA_HISTOGRAM_LEVELS; id++)
	{
		aulLevelThres[id] = ((pCurInfo->stLumaRect.aulLevelThres[id] *
			stHistoRect.ulWidth /100) * stHistoRect.ulHeight / 100);
	}

	if(hWindow->stCurInfo.bLumaRectUserSet)
	{
		BDBG_MSG(("ClipRect: (%4d, %4d, %4d, %4d)", stUsrClipRect.ulLeft,
			stUsrClipRect.ulRight, stUsrClipRect.ulTop, stUsrClipRect.ulBottom));
	}
	BVDC_P_PRINT_RECT("HistoRect", &stHistoRect, false);
	BDBG_MSG(("HistEnable = %s, UserCfg = %s, HistAtSrc = %s, Hist Size: %d, Interlace = %s",
		pCurInfo->bHistEnable ? "true" :"false",
		hWindow->stCurInfo.bLumaRectUserSet ? "true" : "false",
		hWindow->stCurInfo.bHistAtSrc ? "true" : "false",
		pNumBin->ulHistSize,
		bInterlaced ? "true" : "false"));
	BDBG_MSG(("Threshold[(%d/100)%%] = %d", pCurInfo->stLumaRect.aulLevelThres[0], aulLevelThres[0]));
	BDBG_MSG(("Threshold[(%d/100)%%] = %d", pCurInfo->stLumaRect.aulLevelThres[1], aulLevelThres[1]));
	BDBG_MSG(("Threshold[(%d/100)%%] = %d", pCurInfo->stLumaRect.aulLevelThres[2], aulLevelThres[2]));
	BDBG_MSG(("Threshold[(%d/100)%%] = %d", pCurInfo->stLumaRect.aulLevelThres[3], aulLevelThres[3]));

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HIST_WIN_OFFSET);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HIST_WIN_OFFSET, H_OFFSET, stHistoRect.lLeft) |
		BCHP_FIELD_DATA(HIST_WIN_OFFSET, V_OFFSET, stHistoRect.lTop);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HIST_WIN_SIZE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HIST_WIN_SIZE, H_SIZE, stHistoRect.ulWidth) |
		BCHP_FIELD_DATA(HIST_WIN_SIZE, V_SIZE, stHistoRect.ulHeight);

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_LUMA_HISTOGRAM_LEVELS);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HIST_LVL_THRESHi_ARRAY_BASE);
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)&aulLevelThres[0], BVDC_LUMA_HISTOGRAM_LEVELS * sizeof(uint32_t));
	pList->pulCurrent += BVDC_LUMA_HISTOGRAM_LEVELS;

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HIST_NUMBINS);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HIST_NUMBINS, NUMBINS, pNumBin->ulHwNumBin);

	BVDC_P_Hist_SetEnable_isr(pList, pCurInfo->bHistEnable);
	hHist->ulHistSize = pNumBin->ulHistSize;
	hHist->stHistData.ulPixelCnt = stHistoRect.ulWidth * stHistoRect.ulHeight;
	BDBG_LEAVE(BVDC_P_Hist_BuildCfgRul_isr);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_BuildVsyncRul_isr
 *
 * called by BVDC_Window_BuildRul_isr every vsync
 */
static void BVDC_P_Hist_BuildVsyncRul_isr
	( BVDC_P_ListInfo                  *pList )
{
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HIST_POP);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(HIST_POP, POP, POP);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_BuildRul_isr
 *
 * called by BVDC_Window_BuildRul_isr at every src vsync. It builds RUL for
 * Hist HW module.
 *
 * It will reset *phHist to NULL if the HW module is no longer used by
 * any window.
 */
void BVDC_P_Hist_BuildRul_isr
	( BVDC_P_Hist_Handle                hHist,
	  BVDC_P_ListInfo                  *pList,
	  BVDC_P_State                      eVnetState,
	  BVDC_P_PicComRulInfo             *pPicComRulInfo )
{
	uint32_t                 ulRulOpsFlags;
	BVDC_P_Window_Info      *pCurInfo;
	BVDC_P_Window_DirtyBits *pCurDirty;

	/* handle validation */
	BDBG_OBJECT_ASSERT(hHist, BVDC_HST);
	BDBG_OBJECT_ASSERT(hHist->hWindow, BVDC_WIN);

	pCurInfo  = &hHist->hWindow->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;


	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hHist->SubRul), pPicComRulInfo->eWin, eVnetState, pList->bLastExecuted);

	if ((0 == ulRulOpsFlags) || (ulRulOpsFlags & BVDC_P_RulOp_eReleaseHandle))
	{
		if(pCurDirty->stBits.bHistoRect)
		{
			/* Defer setting hist configuration until vnet stage is active */
			hHist->bInitial = true;
			pCurDirty->stBits.bHistoRect = BVDC_P_CLEAN;
		}
		return;
	}

	if(!pList->bLastExecuted || pCurDirty->stBits.bHistoRect || hHist->bInitial)
	{
		BVDC_P_Hist_BuildCfgRul_isr(hHist, hHist->hWindow, pList);

		hHist->bInitial = false;
		pCurDirty->stBits.bHistoRect = BVDC_P_CLEAN;
	}

	if(pCurInfo->bHistEnable)
	{
		BVDC_P_Hist_BuildVsyncRul_isr(pList);
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eEnable)
	{
		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			BVDC_P_SubRul_JoinInVnet_isr(&(hHist->SubRul), pList);
		}
	}
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_SubRul_DropOffVnet_isr(&(hHist->SubRul), pList);
		BVDC_P_Hist_SetEnable_isr(pList, false);
		hHist->bInitial = true;
	}
}

#else
/***************************************************************************/
/* No support for any hist block */

#include "bvdc_errors.h"

BDBG_MODULE(BVDC_HIST);
BDBG_OBJECT_ID(BVDC_HST);

BERR_Code BVDC_P_Hist_Create
	( BVDC_P_Hist_Handle *              phHist,
	  BVDC_P_HistId                     eHistId,
	  BREG_Handle                       hRegister,
	  BVDC_P_Resource_Handle            hResource )
{
	BDBG_ASSERT(phHist);
	*phHist = NULL;
	BSTD_UNUSED(eHistId);
	BSTD_UNUSED(hRegister);
	BSTD_UNUSED(hResource);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Hist_Destroy
	( BVDC_P_Hist_Handle                hHist )
{
	BSTD_UNUSED(hHist);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Hist_AcquireConnect
	( BVDC_P_Hist_Handle                hHist,
	  BVDC_Window_Handle                hWindow)
{
	BSTD_UNUSED(hHist);
	BSTD_UNUSED(hWindow);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Hist_ReleaseConnect_isr
	( BVDC_P_Hist_Handle               *phHist )
{
	BSTD_UNUSED(phHist);
	return BERR_SUCCESS;
}

void BVDC_P_Hist_BuildRul_isr
	( BVDC_P_Hist_Handle                hHist,
 	  BVDC_P_ListInfo                  *pList,
	  BVDC_P_State                      eVnetState,
	  BVDC_P_PicComRulInfo             *pPicComRulInfo )
{
	BSTD_UNUSED(hHist);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eVnetState);
	BSTD_UNUSED(pPicComRulInfo);
	return;
}

void BVDC_P_Hist_UpdateHistData_isr
	( BVDC_P_Hist_Handle                hHist )
{
	BSTD_UNUSED(hHist);
	return;
}

#endif  /* #if (BVDC_P_SUPPORT_HIST_VER >= BVDC_P_SUPPORT_HIST_VER_2) */

void BVDC_P_Hist_GetHistogramData
	( const BVDC_Window_Handle          hWindow,
	  BVDC_LumaStatus                  *pLumaStatus )
{
	BVDC_Source_Handle            hSource;

	BDBG_ENTER(BVDC_P_Hist_GetHistogramData);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurResource.hHist, BVDC_HST);
	hSource = hWindow->stCurInfo.hSource;
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	BKNI_EnterCriticalSection();

	if(pLumaStatus)
	{
		if(hSource->stCurInfo.eMuteMode == BVDC_MuteMode_eDisable)
		{
			*pLumaStatus = hWindow->stCurResource.hHist->stHistData;
		}
		else
		{
			*pLumaStatus = hWindow->stCurResource.hHist->stFreezedHistData;
		}
	}

	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BVDC_P_Hist_GetHistogramData);

	return;
}

/* End of file. */
