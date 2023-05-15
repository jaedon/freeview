/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_downsample_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 8/13/12 3:07p $
 *
 * Module Description:
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_downsample_priv.c $
 * 
 * Hydra_Software_Devel/13   8/13/12 3:07p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 * 
 * Hydra_Software_Devel/12   11/11/10 7:28p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 * 
 * Hydra_Software_Devel/11   4/7/10 11:29a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/10   4/5/10 4:05p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/9   5/28/09 2:18p yuxiaz
 * PR55364: Disable dithering when any testfeature1 is enabled
 *
 * Hydra_Software_Devel/8   4/17/09 11:57a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/7   3/25/08 3:17p syang
 * PR 40431: add complete assert for critical section protection among src
 * _isr, dsp _isr, and ApplyChanges
 *
 * Hydra_Software_Devel/6   2/21/08 5:00p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/5   10/30/07 7:16p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/4   8/1/07 7:20p albertl
 * PR33017:  Rul now written to both slots, fixing progressive to
 * interlaced transitions where sometimes the rul wouldn't be executed.
 *
 * Hydra_Software_Devel/3   7/12/07 6:11p albertl
 * PR32593:  Added missing case for BAVC_SourceId_eDs0 and added fixes to
 * 24 to 60 Hz conversion requiring an extra buffer and a wider gap
 * between reader and writer.
 *
 * Hydra_Software_Devel/2   5/10/07 3:22p syang
 * PR 30617: add bStartFeed to src for muting writer
 *
 * Hydra_Software_Devel/1   12/18/06 11:42p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   11/15/06 6:17p albertl
 * PR24587:  Added downsample source for 7440.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"
#include "bvdc_priv.h"

#include "bvdc_downsample_priv.h"

BDBG_MODULE(BVDC_DS);
BDBG_OBJECT_ID(BVDC_DWS);

#if BVDC_P_SUPPORT_DOWNSAMPLE
/***************************************************************************/
/* Has some support for at least one VDEC? */

#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_bufferheap_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_compositor_priv.h"

#include "bchp_ds_0.h"


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_DownSample_SetVideoFormat
	( BVDC_P_DownSample_Handle         hDownSample,
	  const BFMT_VideoInfo            *pFmtInfo )
{
	BVDC_P_Source_Info *pNewInfo;
	BVDC_P_Source_DirtyBits *pNewDirty;

	BDBG_ENTER(BVDC_P_DownSample_SetVideoFormat);
	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);
	BDBG_OBJECT_ASSERT(hDownSample->hSource, BVDC_SRC);

	pNewInfo  = &hDownSample->hSource->stNewInfo;
	pNewDirty = &hDownSample->hSource->stDirty;

	/* set new info and dirty bits. */
	pNewInfo->pFmtInfo = pFmtInfo;

	/* Dirty bit set */
	pNewDirty->bInputFormat = BVDC_P_DIRTY;

	/* Update source freq */
	hDownSample->hSource->ulVertFreq = pNewInfo->pFmtInfo->ulVertFreq;

	BVDC_P_Source_ApplyChanges(hDownSample->hSource);

	BDBG_LEAVE(BVDC_P_DownSample_SetVideoFormat);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_DownSample_BuildFormatChange_isr
	( const BVDC_P_DownSample_Handle   hDownSample,
	  BVDC_P_ListInfo                 *pList)
{
	const BFMT_VideoInfo *pFmtInfo;
	uint32_t ulOffset;
	uint32_t ulHSize;
	uint32_t ulVSize;

	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);
	BDBG_OBJECT_ASSERT(hDownSample->hSource, BVDC_SRC);

	ulOffset = hDownSample->ulOffset;

	/* Current settings */
	pFmtInfo = hDownSample->hSource->stCurInfo.pFmtInfo;

	ulHSize = pFmtInfo->ulWidth;
	ulVSize = pFmtInfo->bInterlaced ? (pFmtInfo->ulHeight / 2) : pFmtInfo->ulHeight;

	/* Set new input format. */
	/* DS_0_BVB_IN_SIZE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_BVB_IN_SIZE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DS_0_BVB_IN_SIZE, HSIZE, ulHSize ) |
		BCHP_FIELD_DATA(DS_0_BVB_IN_SIZE, VSIZE, ulVSize );

	/* Turn on format detection. */
	/* DS_0_FORMAT_DETECT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_FORMAT_DETECT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(DS_0_FORMAT_DETECT, FORMAT_DETECT, ENABLE );

	BDBG_MSG(("New entries added by DS BuildFormatChange"));
	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_DownSample_BuildOutputRect_isr
	( const BVDC_P_DownSample_Handle   hDownSample,
	  BVDC_P_ListInfo                 *pList)
{
	const BVDC_P_Rect *pScanOut;
	const BFMT_VideoInfo *pFmtInfo;
	uint32_t ulHSize;
	uint32_t ulVSize;
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);
	BDBG_OBJECT_ASSERT(hDownSample->hSource, BVDC_SRC);

	ulOffset = hDownSample->ulOffset;

	/* Current settings */
	pScanOut = &(hDownSample->hSource->stScanOut);
	pFmtInfo = hDownSample->hSource->stCurInfo.pFmtInfo;

	ulHSize = pScanOut->ulWidth;
	ulVSize = pFmtInfo->bInterlaced ? (pFmtInfo->ulHeight / 2) : pFmtInfo->ulHeight;

	/* DS_0_PIC_SIZE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_PIC_SIZE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DS_0_PIC_SIZE, HSIZE, ulHSize ) |
		BCHP_FIELD_DATA(DS_0_PIC_SIZE, VSIZE, ulVSize );

	/* DS_0_PIC_OFFSET (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_PIC_OFFSET + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DS_0_PIC_OFFSET, HSIZE, pScanOut->lLeft ) |
		BCHP_FIELD_DATA(DS_0_PIC_OFFSET, VSIZE, pScanOut->lTop );

	BDBG_MSG(("New entries added by DS BuildOutputRect"));
	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_DownSample_BuildDrain_isr
	( BVDC_P_DownSample_Handle         hDownSample,
	  BVDC_P_ListInfo                 *pList,
	  bool                             bDrain)
{
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);

	ulOffset = hDownSample->ulOffset;

	/* DS_0_CONFIGURATION (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_CONFIGURATION + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DS_0_CONFIGURATION, SCRATCH,          0                    ) |
		(bDrain ? BCHP_FIELD_ENUM(DS_0_CONFIGURATION, INPUT_DRAIN,      ENABLE     )
		        : BCHP_FIELD_ENUM(DS_0_CONFIGURATION, INPUT_DRAIN,      DISABLE    ))|
		BCHP_FIELD_ENUM(DS_0_CONFIGURATION, RX_APPEND,        APPEND_DISABLE       ) |
		BCHP_FIELD_ENUM(DS_0_CONFIGURATION, FILTER_TYPE,      FILTER_TYPE_STANDARD ) |
		BCHP_FIELD_ENUM(DS_0_CONFIGURATION, RING_SUPPRESSION, DISABLE              );

	return;
}

/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_DownSample_BuildOutput_isr
	( BVDC_P_DownSample_Handle         hDownSample,
	  BVDC_P_ListInfo                 *pList,
	  bool                             bOutput)
{
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);

	ulOffset = hDownSample->ulOffset;

	/* DS_0_CONTROL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_CONTROL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(DS_0_CONTROL, ENABLE_CTRL, STOP_ON_FIELD_COMPLETION ) |
		BCHP_FIELD_DATA(DS_0_CONTROL, ENABLE,      bOutput                  );

	return;
}


/***************************************************************************
 * Build RUL related to input format: size, triggers etc
 *
 * We need to hold reset anytime programming the input control and other
 * hd_dvi related settings, and release when done.
 */
static void BVDC_P_DownSample_BuildVsyncRul_isr
	( BVDC_P_DownSample_Handle         hDownSample,
	  BVDC_P_ListInfo                 *pList)
{
	BVDC_P_Source_Info *pCurInfo;

	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);
	BDBG_OBJECT_ASSERT(hDownSample->hSource, BVDC_SRC);

	pCurInfo = &hDownSample->hSource->stCurInfo;

	if (hDownSample->hSource->ulConnectedWindow && pCurInfo->ulWindows)
	{
		BVDC_P_DownSample_BuildOutput_isr(hDownSample, pList, true);
	}

	BDBG_LEAVE(BVDC_P_DownSample_BuildVsyncRul_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_DownSample_Create
	( BVDC_P_DownSample_Handle        *phDownSample,
	  BVDC_P_DownSampleId              eDownSampleId,
	  BREG_Handle                      hReg,
	  BVDC_Source_Handle               hSource )
{
	BVDC_P_DownSampleContext   *pDownSample;

	BDBG_ENTER(BVDC_P_DownSample_Create);
	BDBG_ASSERT(phDownSample);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* The handle will be NULL if create fails. */
	*phDownSample = NULL;

	/* (1) Alloc the context. */
	pDownSample = (BVDC_P_DownSampleContext*)
		(BKNI_Malloc(sizeof(BVDC_P_DownSampleContext)));
	if(!pDownSample)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pDownSample, 0x0, sizeof(BVDC_P_DownSampleContext));
	BDBG_OBJECT_SET(pDownSample, BVDC_DWS);

	/* Store the id & hRegister for activating the triggers. */
	pDownSample->eId                   = eDownSampleId;
	pDownSample->hReg                  = hReg;
	pDownSample->hSource               = hSource;


	/* (2) All done. now return the new fresh context to user. */
	*phDownSample = (BVDC_P_DownSample_Handle)pDownSample;

	BDBG_LEAVE(BVDC_P_DownSample_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_DownSample_Destroy
	( BVDC_P_DownSample_Handle               hDownSample )
{
	BDBG_ENTER(BVDC_P_DownSample_Destroy);
	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);

	BDBG_OBJECT_DESTROY(hDownSample, BVDC_DWS);
	/* [1] Release context in system memory */
	BKNI_Free((void*)hDownSample);

	BDBG_LEAVE(BVDC_P_DownSample_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_DownSample_Init
	( BVDC_P_DownSample_Handle               hDownSample )
{
	BDBG_ENTER(BVDC_P_DownSample_Init);

	/* Sanity check and get context */
	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);
	BDBG_OBJECT_ASSERT(hDownSample->hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hDownSample->hSource->hVdc, BVDC_VDC);

	/* Downsample states */
	hDownSample->bVideoDetected    = false;
	hDownSample->hSrcCompositor    = hDownSample->hSource->hVdc->ahCompositor[hDownSample->hSource->eDsSrcCompId];

	BDBG_ASSERT(hDownSample->hSrcCompositor);
	BDBG_OBJECT_ASSERT(hDownSample->hSrcCompositor, BVDC_CMP);
	hDownSample->hSrcCompositor->hDownSample     = hDownSample;

	/* Downsample triggers are determined after source creation */
	hDownSample->hSource->aeTrigger[0] = BVDC_P_CMP_GET_DISP_TOP_TRIGGER(hDownSample->hSrcCompositor);
	hDownSample->hSource->aeTrigger[1] = BVDC_P_CMP_GET_DISP_BOT_TRIGGER(hDownSample->hSrcCompositor);

	BVDC_P_DownSample_SetVideoFormat(hDownSample,
		hDownSample->hSrcCompositor->hDisplay->stCurInfo.pFmtInfo);

	BDBG_LEAVE(BVDC_P_DownSample_Init);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_DownSample_Disconnect
	( BVDC_P_DownSample_Handle               hDownSample )
{
	BDBG_ENTER(BVDC_P_DownSample_Disconnect);
	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);
	BDBG_OBJECT_ASSERT(hDownSample->hSrcCompositor, BVDC_CMP);

	hDownSample->hSrcCompositor->hDownSample     = NULL;
	hDownSample->hSrcCompositor     = NULL;

	BDBG_LEAVE(BVDC_P_DownSample_Disconnect);
	return;
}


/***************************************************************************
 * {private}
 *
 * Update Vdec status.
 */
void BVDC_P_DownSample_UpdateStatus_isr
	( BVDC_P_DownSample_Handle               hDownSample )
{
	BVDC_P_Source_Info *pCurInfo;
	BVDC_P_Source_DirtyBits *pCurDirty;
	uint32_t ulReg;
	uint32_t ulOffset;
	bool bVideoDetected = false;

	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);
	BDBG_OBJECT_ASSERT(hDownSample->hSource, BVDC_SRC);
	pCurInfo  = &hDownSample->hSource->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;
	ulOffset  = hDownSample->ulOffset;

	/* check if format is valid */
	ulReg    = BREG_Read32(hDownSample->hReg, BCHP_DS_0_CUR_BVB_IN_SIZE + ulOffset);
	bVideoDetected = !(BCHP_GET_FIELD_DATA(ulReg, DS_0_CUR_BVB_IN_SIZE, H_SIZE_MISMATCH));

	if(hDownSample->bVideoDetected != bVideoDetected)
	{
		hDownSample->bVideoDetected = bVideoDetected;
		pCurDirty->bVideoDetected = BVDC_P_DIRTY;
		hDownSample->hSource->bStartFeed = bVideoDetected;
	}

	return;
}

/***************************************************************************
 * {private}
 *
 * Get Vdec status.
 */
void BVDC_P_DownSample_GetStatus_isr
	( const BVDC_P_DownSample_Handle   hDownSample,
	  bool                            *pbVideoDetected )
{
	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);

	/* (1) Downsample input video detected? */
	if(pbVideoDetected)
	{
		*pbVideoDetected = hDownSample->bVideoDetected;
	}

	return;
}


/***************************************************************************
 * {private}
 *
 * Build vdec RUL.
 */
void BVDC_P_DownSample_BuildRul_isr
	( BVDC_P_DownSample_Handle         hDownSample,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId )
{
	BVDC_P_Source_Info *pCurInfo;
	BVDC_P_Source_Info *pNewInfo;
	BVDC_P_Source_DirtyBits *pCurDirty, *pOldDirty;

	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);
	BDBG_OBJECT_ASSERT(hDownSample->hSource, BVDC_SRC);

	pOldDirty = &hDownSample->hSource->astOldDirty[eFieldId];
	pCurInfo  = &hDownSample->hSource->stCurInfo;
	pNewInfo  = &hDownSample->hSource->stNewInfo;
	pCurDirty = &pCurInfo->stDirty;

	/* Clear old dirty bits. */
	if(BVDC_P_IS_DIRTY(pOldDirty))
	{
		if(!pList->bLastExecuted)
		{
			BVDC_P_OR_ALL_DIRTY(pCurDirty, pOldDirty);
		}
		else
		{
			BVDC_P_CLEAN_ALL_DIRTY(pOldDirty);
		}
	}

	/* Let's see what we need to build. */
	if(BVDC_P_IS_DIRTY(pCurDirty))
	{
		BDBG_MSG(("eVideoFmt      = %s, dirty = %d",
			pCurInfo->pFmtInfo->pchFormatStr, pCurDirty->bInputFormat));
		BDBG_MSG(("bVideoDetected = %d, dirty = %d",
			pDownSample->bVideoDetected, pCurDirty->bVideoDetected));
		BDBG_MSG(("ulWindows = %d, dirty = %d",
			pCurInfo->ulWindows, pCurDirty->bWindowNum));
		BDBG_MSG(("------------------------------intP%d", eFieldId));

		if(pCurDirty->bInputFormat)
		{
			/* Drain data and disable output while setting up format change. */
			BVDC_P_DownSample_BuildDrain_isr(hDownSample, pList, true);

			BVDC_P_DownSample_BuildOutput_isr(hDownSample, pList, false);

			BVDC_P_DownSample_BuildFormatChange_isr(hDownSample, pList);

			if (hDownSample->hSource->ulConnectedWindow)
			{
				BVDC_P_DownSample_BuildDrain_isr(hDownSample, pList, false);
			}
		}

		if(pCurDirty->bWindowNum)
		{
			/* we need to drain before all windows actually disconnect
			 * from the DS or the DS will stall, so we read from ulWindows
			 */
			if (pCurInfo->ulWindows == 0)
			{
				BVDC_P_DownSample_BuildDrain_isr(hDownSample, pList, true);
				BVDC_P_DownSample_BuildOutput_isr(hDownSample, pList, false);
			}
			/* disable drain when a window is actually connected. */
			else if (hDownSample->hSource->ulConnectedWindow)
			{
				BVDC_P_DownSample_BuildDrain_isr(hDownSample, pList, false);
			}
			else
			{
				/* shouldn't get here. ulWindows should never be non-zero when
				 * ulConnectedWindow is zero, because ulConnectedWindow is
				 * updated immediately after a window using this source is opened.
				 * It is however updated on a vsync after a window is closed.
				 */
				BDBG_ASSERT(false);
			}
		}

		if(pCurDirty->bRecAdjust)
		{
			BVDC_P_DownSample_BuildOutputRect_isr(hDownSample, pList);
		}

		/* Clear dirty bits. */
		*pOldDirty = *pCurDirty;
		BVDC_P_CLEAN_ALL_DIRTY(pCurDirty);
	}

	/* This get build every vsync. */
	BVDC_P_DownSample_BuildVsyncRul_isr(hDownSample, pList);
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_DownSample_Bringup_isr
	( const BVDC_P_DownSample_Handle         hDownSample )
{
	uint32_t i;
	BRDC_List_Handle hList;
	BRDC_Slot_Handle hSlot;
	BVDC_P_ListInfo stList;
	BVDC_P_ListInfo *pList = &stList;
	uint32_t ulOffset;

	/* Sanity check and get context */
	BDBG_OBJECT_ASSERT(hDownSample, BVDC_DWS);
	BDBG_OBJECT_ASSERT(hDownSample->hSource, BVDC_SRC);

	ulOffset = hDownSample->ulOffset;

	BVDC_P_SRC_NEXT_RUL(hDownSample->hSource, BAVC_Polarity_eTopField);
	hSlot = BVDC_P_SRC_GET_SLOT(hDownSample->hSource, BAVC_Polarity_eTopField);
	hList = BVDC_P_SRC_GET_LIST(hDownSample->hSource, BAVC_Polarity_eTopField);

	for(i = 0; i < hDownSample->hSource->ulSlotUsed; i++)
	{
		BRDC_Slot_ExecuteOnTrigger_isr(hDownSample->hSource->ahSlot[i],
			hDownSample->hSource->aeTrigger[i], true);
	}

	BVDC_P_ReadListInfo_isr(pList, hList);

	/* DS_0_REVISION (RO) */

	/* DS_0_SRC_SELECT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_SRC_SELECT + ulOffset);

	if (hDownSample->hSource->eDsSrcCompId == BVDC_CompositorId_eCompositor0)
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(DS_0_SRC_SELECT, CMP_SELECT, CMP_0 );
	}
	else if (hDownSample->hSource->eDsSrcCompId == BVDC_CompositorId_eCompositor1)
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(DS_0_SRC_SELECT, CMP_SELECT, CMP_1 );
	}
	else
	{
		/* only compositors 0 and 1 can be a DS source */
		BDBG_ASSERT(false);
	}

	/* DS_0_SCRATCH_REG (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_SCRATCH_REG + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DS_0_SCRATCH_REG, VALUE, 0 );

	/* DS_0_DEBUG_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_DEBUG_CTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DS_0_DEBUG_CTRL, HV_CNT_SEL,    0 ) |
		BCHP_FIELD_DATA(DS_0_DEBUG_CTRL, FORCE_PIC_END, 0 );

	/* DS_0_CUR_BVB_IN_SIZE (RO) */
	/* DS_0_BVB_IN_STATUS_CLEAR (WO) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DS_0_BVB_IN_STATUS_CLEAR + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DS_0_BVB_IN_STATUS_CLEAR, MISSING_SYNC, 0 ) |
		BCHP_FIELD_DATA(DS_0_BVB_IN_STATUS_CLEAR, LONG_SOURCE,  0 ) |
		BCHP_FIELD_DATA(DS_0_BVB_IN_STATUS_CLEAR, SHORT_SOURCE, 0 ) |
		BCHP_FIELD_DATA(DS_0_BVB_IN_STATUS_CLEAR, LONG_LINE,    0 ) |
		BCHP_FIELD_DATA(DS_0_BVB_IN_STATUS_CLEAR, SHORT_LINE,   0 );

	/* DS_0_BVB_IN_STATUS (RO) */
	/* DS_0_DEBUG_HV_CNT (RO) */

	BVDC_P_DownSample_BuildDrain_isr(hDownSample, pList, true);
	BVDC_P_WriteListInfo_isr(pList, hList);
	BRDC_Slot_SetList_isr(hSlot, hList);
	BRDC_Slot_Execute_isr(hSlot);

	BDBG_MSG(("Bringup source[%d].", hDownSample->hSource->eId));

}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Source_DownSampleDataReady_isr
	( void                            *pvSourceHandle,
	  int                              iParam2 )
{
	int i;
	BVDC_P_ListInfo stList;
	BVDC_Source_Handle hSource;
	BVDC_P_Source_Info *pCurInfo;
	BRDC_Slot_Handle hSlot, hOtherSlot;
	BRDC_List_Handle hList;
	const BFMT_VideoInfo *pFmtInfo;

	BDBG_ENTER(BVDC_P_Source_DownSampleDataReady_isr);

	/* Get Source context */
	hSource = (BVDC_Source_Handle)pvSourceHandle;
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hSource->hDownSample, BVDC_DWS);

	/* We should not even get the Gfx or mpeg source here. */
	BDBG_ASSERT(BVDC_P_SRC_IS_DS(hSource->eId));

	/* Make sure the BKNI enter/leave critical section works. */
	BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
	hSource->hVdc->ulInsideCs ++;

	/* Update source user info */
	BVDC_P_Source_UpdateSrcState_isr(hSource);

	/* Update source info */
	BVDC_P_DownSample_UpdateStatus_isr(hSource->hDownSample);

	/* Get current settings */
	pCurInfo = &hSource->stCurInfo;
	pFmtInfo = pCurInfo->pFmtInfo;

	/* for progressive src format, we always expect Top slot interrupt */
	hSource->eNextFieldIntP = iParam2;

	/* this is actually the current slot */
	hSource->eNextFieldId   = pFmtInfo->bInterlaced
		? BVDC_P_NEXT_POLARITY(iParam2) : BAVC_Polarity_eFrame;

	/* Get the approriate slot/list for building RUL. */
	BVDC_P_SRC_NEXT_RUL(hSource, hSource->eNextFieldIntP);
	hSlot = BVDC_P_SRC_GET_SLOT(hSource, hSource->eNextFieldIntP);
	hList = BVDC_P_SRC_GET_LIST(hSource, hSource->eNextFieldIntP);

	/* Update the status of last executed RUL. */
	BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, true);

	/* Assert: list is not connected to any slot */
	BRDC_List_SetNumEntries_isr(hList, 0);

	/* Get current pointer to RUL and info. */
	BVDC_P_ReadListInfo_isr(&stList, hList);
	stList.bMasterList = true;

	/* update windows that connected to this source, including user info,
	 * destroy related state and disconnecting from source */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hSource->ahWindow[i] ||
		   BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
		   BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
		{
			continue;
		}

		BVDC_P_Window_AdjustRectangles_isr(hSource->ahWindow[i], NULL, NULL, 0);
	}

	/* Get the source scan out rectangle. Combine the user pan-scan info
	 * from all the window that uses this source;
	 * Note: pFieldData = NULL for analog video source. */
	if(pCurInfo->stDirty.bRecAdjust)
	{
		BVDC_P_Source_GetScanOutRect_isr(hSource, NULL, NULL, &hSource->stScanOut);
	}

	/* For each window using this source do the following. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hSource->ahWindow[i] ||
		   BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
		   BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
		{
			continue;
		}

		BVDC_P_Window_Writer_isr(hSource->ahWindow[i], NULL, NULL,
			hSource->eNextFieldId, &stList, 0);

		BVDC_P_Window_BuildRul_isr(hSource->ahWindow[i], &stList, hSource->eNextFieldId,
			true,  /* writer*/
			false  /* reader */);
	}

	if(pCurInfo->stDirty.bRecAdjust || hSource->bWait4ReconfigVnet)
	{
		/* Gather window information after vnetmode is determined */
		BVDC_P_Source_GetWindowVnetmodeInfo_isr(pSource);
	}

	/* Now build the DS block. */
	BVDC_P_DownSample_BuildRul_isr(hSource->hDownSample, &stList, hSource->eNextFieldIntP);

	/* UPdate current pictures, the eSourcePolarity must update every field. */
	if(hSource->bPictureChanged)
	{
		hSource->bPictureChanged = false;
	}

	if(hSource->bRasterChanged)
	{
		hSource->bRasterChanged = false;
	}

	/* Update entries count */
	BVDC_P_WriteListInfo_isr(&stList, hList);

	/* This programs the slot after next. This is needed in conjunction with
	 * the programming of the next slot above and for accommodating same field
	 * polarity sources. Moreover, this is so to make the 4 buffer algorithm
	 * work correctly. */
	hOtherSlot = BVDC_P_SRC_GET_SLOT(hSource,
		BVDC_P_NEXT_POLARITY(hSource->eNextFieldIntP));

	/* Always assign single RUL to two slots to avoid uncovered transition
	 * from dual to one; */
	/* Note: to flush the cached RUL only once, call the Dual function
	   instead of two individual slot functions; */
	BRDC_Slot_SetListDual_isr(hSlot, hOtherSlot, hList);

	hSource->hVdc->ulInsideCs --;
	BDBG_LEAVE(BVDC_P_Source_DownSampleDataReady_isr);
	return;
}

#else

void BVDC_P_Source_DownSampleDataReady_isr
	( void                            *pvSourceHandle,
	  int                              iParam2 )
{
	BSTD_UNUSED(pvSourceHandle);
	BSTD_UNUSED(iParam2);

	return;
}

#endif

/* End of File */
