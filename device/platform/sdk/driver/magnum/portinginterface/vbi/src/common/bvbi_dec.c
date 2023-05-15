/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_dec.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 4/9/10 7:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_dec.c $
 * 
 * Hydra_Software_Devel/6   4/9/10 7:37p darnstein
 * SW7125-256,SW7125-257: When creating decoder object, do all  ocations
 * in a different order. This simplifies error-recovery logic for the
 * benefit of Coverity.
 * 
 * Hydra_Software_Devel/5   1/8/10 5:26p darnstein
 * SW7408-15: compiling for chipsets without VDEC or IN656 results in
 *      ed in warnings about unused variables and labels.
 * 
 * Hydra_Software_Devel/4   12/10/09 6:46p darnstein
 * SW7420-498: Move memory management from "apply changes" functions to
 * "create" functions. Applies to teletext memory only.
 * 
 * Hydra_Software_Devel/3   11/18/09 3:54p darnstein
 * SW7468-24: Gemstar options now placed in dedicated data structure.
 * 
 * Hydra_Software_Devel/2   2/20/09 12:37p darnstein
 * PR49987: Add a missing piece of logic for the bBrokenDataCount option.
 * 
 * Hydra_Software_Devel/1   12/3/08 8:01p darnstein
 * PR45819:
 * 
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bkni.h"			/* For critical sections */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */
#ifdef BVBI_P_TTD_SCB2_ERROR
#include "bmem.h"
#endif

BDBG_MODULE(BVBI);

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

static BERR_Code BVBI_P_Decode_ApplyChanges (
	BVBI_P_Decode_Handle* pVbi_Dec, BVBI_P_Decode_CNState* cnState, bool force);

static BERR_Code BVBI_P_Decode_Set_VBI(
	BVBI_Decode_Handle decodeHandle,
	bool bEnabled,
	uint32_t ulSelect_Standard);

static BERR_Code BVBI_P_Decode_Get_VBI(
	BVBI_Decode_Handle decodeHandle,
	bool* pbEnabled,
	uint32_t ulSelect_Standard);

#if (BVBI_P_NUM_VDEC >= 1)
static bool BVBI_P_Decode_IsDirty (
	BVBI_P_Decode_Handle *pVbi_Dec, bool isActive, uint32_t ulSelect_Standard);
#endif

static void BVBI_P_Decode_ChangesApplied (BVBI_P_Decode_Handle *pVbi_Dec);


/***************************************************************************
* Implementation of API functions
***************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_Create(BVBI_Handle vbiHandle,
                             BAVC_SourceId eSource,
                             BVBI_Decode_Handle *pDecodeHandle)
{
	BVBI_P_Handle *pVbi;
	BVBI_P_Decode_Handle *pVbi_Dec = NULL;
	BVBI_P_TTData* pttDataT = NULL;
	BVBI_P_TTData* pttDataB = NULL;
#ifdef BVBI_P_TTD_SCB2_ERROR
	BVBI_P_TTData* pttDataTs = NULL;
	BVBI_P_TTData* pttDataBs = NULL;
#endif
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Decode_Create);

	/* check parameters */
	BVBI_P_GET_CONTEXT(vbiHandle, pVbi);
	if((!pVbi) || (!pDecodeHandle))
	{
		BDBG_ERR(("Invalid parameter\n"));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BVBI_Decode_Create_Done;
	}

	/* This check is important.  I don't have to do this check again when
	decoding VBI data.  It saves time and simplifies the software. */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		break;
#endif
	case BAVC_SourceId_e656In0:
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_e656In1:
		break;
#endif
	default:
		BDBG_ERR(("Invalid parameter\n"));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BVBI_Decode_Create_Done;
	}

	/* Check to see if the caller has already opened a decode handle that
	   controls the same video source */
	for (pVbi_Dec = BLST_D_FIRST (&pVbi->decode_contexts) ;
		 pVbi_Dec ;
		 pVbi_Dec = BLST_D_NEXT (pVbi_Dec, link))
	{
		if (pVbi_Dec->eSource == eSource)
		{
			BDBG_ERR(("Multiple handles for one VBI core\n"));
			eErr = BERR_TRACE(BVBI_ERR_HW_CONFLICT);
			goto BVBI_Decode_Create_Done;
		}
	}

	/* Alloc the VBI decode handle. */
	pVbi_Dec =
		(BVBI_P_Decode_Handle*)(BKNI_Malloc(sizeof(BVBI_P_Decode_Handle)));
	if(!pVbi_Dec)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Decode_Create_Done;
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVbi_Dec, 0x0, sizeof(BVBI_P_Decode_Handle));
	pVbi_Dec->gsOptionsChanged = true;

	/* Initialize current/next state */
	pVbi_Dec->curr.eVideoFormat                      = BFMT_VideoFmt_eNTSC;
	pVbi_Dec->curr.ulActive_Standards                = 0x0;
	pVbi_Dec->curr.e656Format                        = BVBI_656Fmt_SAA7113;
	pVbi_Dec->curr.SMPTE291Moptions.fParseCb         =
										BVBI_Decode_656_SMPTE291MbrcmCb_isr;
	pVbi_Dec->curr.SMPTE291Moptions.fParseCbArg0     = 0x0;
	pVbi_Dec->curr.SMPTE291Moptions.bLongHeader      = false;
	pVbi_Dec->curr.SMPTE291Moptions.bBrokenDataCount = true;
	pVbi_Dec->curr.wssVline576i             = 23;
	pVbi_Dec->curr.gsOptions.baseline_top   = 10;
	pVbi_Dec->curr.gsOptions.linemask_top   = 0x01F;
	pVbi_Dec->curr.gsOptions.baseline_bot   = 273;
	pVbi_Dec->curr.gsOptions.linemask_bot   = 0x01F;
	pVbi_Dec->next = pVbi_Dec->curr;

	/* Save back pointer to VBI handle */
	pVbi_Dec->pVbi = pVbi;

	/* Remember where to decode from */
	pVbi_Dec->eSource = eSource;

	/* Initialize teletext pointers for LCO */
	pttDataT = (BVBI_P_TTData*)(BKNI_Malloc(sizeof(BVBI_P_TTData)));
	if(!pttDataT)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Decode_Create_Done;
	}
	BKNI_Memset((void*)pttDataT, 0x0, sizeof(BVBI_P_TTData));
	eErr = BVBI_P_TTData_Alloc (
		pVbi->hMem, BVBI_TT_MAX_LINES, BVBI_TT_MAX_LINELENGTH, pttDataT);
	if (eErr != BERR_SUCCESS)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Decode_Create_Done;
	}

	pttDataB = (BVBI_P_TTData*)(BKNI_Malloc(sizeof(BVBI_P_TTData)));
	if(!pttDataB)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Decode_Create_Done;
	}
	BKNI_Memset((void*)pttDataB, 0x0, sizeof(BVBI_P_TTData));
	eErr = BVBI_P_TTData_Alloc (
		pVbi->hMem, BVBI_TT_MAX_LINES, BVBI_TT_MAX_LINELENGTH, pttDataB);
	if (eErr != BERR_SUCCESS)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Decode_Create_Done;
	}

#ifdef BVBI_P_TTD_SCB2_ERROR
	pttDataTs = (uint8_t*)BMEM_AllocAligned (pVbi->hMem, 13*512, 10, 0);
	if (!pttDataTs)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Decode_Create_Done;
	}
	pVbi_Dec->topTTscratch = pttDataTs;
	BKNI_Memset ((void*)(pVbi_Dec->topTTscratch), 0x0, 13*512);

	pttDataBs = (uint8_t*)BMEM_AllocAligned (pVbi->hMem, 13*512, 10, 0);
	if (!pttDataBs)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Decode_Create_Done;
	}
	pVbi_Dec->botTTscratch = pttDataBs;
	BKNI_Memset ((void*)(pVbi_Dec->botTTscratch), 0x0, 13*512);
#endif

	/* Initialize 656 ancillary data parser (software) */
	if ((eSource == BAVC_SourceId_e656In0) ||
	    (eSource == BAVC_SourceId_e656In1))
	{
		if ((eErr = BERR_TRACE (BVBI_P_P656_Init (pVbi_Dec))) != 
			BERR_SUCCESS)
		{
			goto BVBI_Decode_Create_Done;
		}
	}
	
	/* Smart pointers come last. This will make the error handling logic easier
	 * for Coverity to understand. */
	BVBI_P_LCOP_CREATE (pVbi_Dec, topTTDataO, pttDataT, clink);
	BVBI_P_LCOP_CREATE (pVbi_Dec, botTTDataO, pttDataB, clink);

	/* Link decode context into VBI handle's list */
	BLST_D_INSERT_HEAD (&pVbi->decode_contexts, pVbi_Dec, link);

	/* Initialize magic number to the size of the struct */
	pVbi_Dec->ulBlackMagic = sizeof(BVBI_P_Decode_Handle);

	/* All done. now return the new fresh context to user. */
	*pDecodeHandle = (BVBI_Decode_Handle)pVbi_Dec;

BVBI_Decode_Create_Done:

	/* In case of failure, take care of partial allocations */
	if (BERR_SUCCESS != eErr) 
	{
#ifdef BVBI_P_TTD_SCB2_ERROR
		if (NULL != pttDataBs)
			BMEM_Free (pVbi->hMem, (void*)pttDataBs);
		if (NULL != pttDataTs)
			BMEM_Free (pVbi->hMem, (void*)pttDataTs);
#endif
		if (NULL != pttDataB)
		{
			BVBI_P_TTData_Alloc (pVbi->hMem, 0, 0, pttDataB);
			BKNI_Free ((void*)pttDataB); 
		}
		if (NULL != pttDataT)
		{
			BVBI_P_TTData_Alloc (pVbi->hMem, 0, 0, pttDataT);
			BKNI_Free ((void*)pttDataT); 
		}
		if (NULL != pVbi_Dec)
			BKNI_Free((void*)pVbi_Dec);
	}

	BDBG_LEAVE(BVBI_Decode_Create);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_Destroy(BVBI_Decode_Handle decodeHandle)
{
	BVBI_P_Handle *pVbi;
	BVBI_P_Decode_Handle* pVbi_Dec;
	BVBI_P_TTData* pttData;

	BDBG_ENTER(BVBI_Decode_Destroy);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if(!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Shut down any active cores */
	pVbi_Dec->next.ulActive_Standards = 0x0;
	BVBI_Decode_ApplyChanges( decodeHandle );

	/* The handle will be invalid */
	pVbi_Dec->ulBlackMagic = 0;

	/* Deallocate bulky data storage if necessary */
	pVbi = pVbi_Dec->pVbi;
	BVBI_P_LCOP_DESTROY (pVbi_Dec, topTTDataO, &pVbi->ttFreelist, clink);
	pttData = BVBI_P_LCOP_GET (pVbi_Dec, topTTDataO);
	BVBI_P_TTData_Alloc (pVbi->hMem, 0, 0, pttData);
	BKNI_Free ((void*)pttData);
	BVBI_P_LCOP_DESTROY (pVbi_Dec, botTTDataO, &pVbi->ttFreelist, clink);
	pttData = BVBI_P_LCOP_GET (pVbi_Dec, botTTDataO);
	BVBI_P_TTData_Alloc (pVbi->hMem, 0, 0, pttData);
	BKNI_Free ((void*)pttData);
#ifdef BVBI_P_TTD_SCB2_ERROR
	BMEM_Free (pVbi->hMem, (void*)pVbi_Dec->topTTscratch);
	BMEM_Free (pVbi->hMem, (void*)pVbi_Dec->botTTscratch);
#endif

	/* Shut down 656 ancillary data parser (software) */
	if ((pVbi_Dec->eSource == BAVC_SourceId_e656In0) ||
	    (pVbi_Dec->eSource == BAVC_SourceId_e656In1))
	{
		BVBI_P_P656_DeInit (pVbi_Dec);
	}

	/* Unlink from parent VBI context */
	BLST_D_REMOVE (&pVbi->decode_contexts, pVbi_Dec, link);

	/* Release context in system memory */
	BKNI_Free((void*)pVbi_Dec);

	BDBG_LEAVE(BVBI_Decode_Destroy);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_ApplyChanges( BVBI_Decode_Handle decodeHandle )
{
	BVBI_P_Decode_Handle *pVbi_Dec;
	BERR_Code eErr;

	BDBG_ENTER(BVBI_Decode_ApplyChanges);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if(!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Try to apply changes */
	eErr = BVBI_P_Decode_ApplyChanges (pVbi_Dec, &pVbi_Dec->next, false);

	/* On failure, restore state */
	if (eErr != BERR_SUCCESS)
	{
		(void)BVBI_P_Decode_ApplyChanges (pVbi_Dec, &pVbi_Dec->curr, true);
		goto done;
	}

	/* Hardware should now be fully programmed */
	BVBI_P_Decode_ChangesApplied (pVbi_Dec);

done:
	BDBG_LEAVE(BVBI_Decode_ApplyChanges);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_SetVideoFormat(BVBI_Decode_Handle decodeHandle,
									 BFMT_VideoFmt eVideoFormat)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_Decode_SetVideoFormat);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if(!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember format as requested */
	pVbi_Dec->next.eVideoFormat = eVideoFormat;

	BDBG_LEAVE(BVBI_Decode_SetVideoFormat);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_GetVideoFormat(BVBI_Decode_Handle decodeHandle,
									 BFMT_VideoFmt *peVideoFormat)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_Decode_GetVideoFormat);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if((!pVbi_Dec) || (!peVideoFormat))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return format as requested */
	*peVideoFormat = pVbi_Dec->curr.eVideoFormat;

	BDBG_LEAVE(BVBI_Decode_GetVideoFormat);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_656_SetFormat(BVBI_Decode_Handle decodeHandle,
									BVBI_656Fmt anci656Fmt)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_Decode_656_SetFormat);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if(!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_Decode_SetVideoFormat);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember format as requested */
	switch (anci656Fmt)
	{
	case BVBI_656Fmt_SAA7113:
	case BVBI_656Fmt_SAA7114A:
	case BVBI_656Fmt_SAA7114B:
	case BVBI_656Fmt_SAA7114C:
	case BVBI_656Fmt_SAA7115:
	case BVBI_656Fmt_SMPTE291:
		pVbi_Dec->next.e656Format = anci656Fmt;
		break;
	default:
		BDBG_LEAVE(BVBI_Decode_SetVideoFormat);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_LEAVE(BVBI_Decode_656_SetFormat);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_656_GetFormat(BVBI_Decode_Handle decodeHandle,
									BVBI_656Fmt* pAnci656Fmt)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_Decode_656_GetFormat);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if((!pVbi_Dec) || (!pAnci656Fmt))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return format as requested */
	*pAnci656Fmt = pVbi_Dec->curr.e656Format;

	BDBG_LEAVE(BVBI_Decode_656_GetFormat);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_656_SetSMPTE291Moptions (
	BVBI_Decode_Handle decodeHandle,
	BVBI_Decode_656_SMPTE291M_Cb fParserCb_isr,
	void* arg0,
	bool bLongHeader,
	bool bBrokenDataCount
)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_Decode_656_SetSMPTE291Moptions);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if ((!pVbi_Dec) || (!fParserCb_isr))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember options as requested */
	pVbi_Dec->next.SMPTE291Moptions.fParseCb         = fParserCb_isr;
	pVbi_Dec->next.SMPTE291Moptions.fParseCbArg0     = arg0;
	pVbi_Dec->next.SMPTE291Moptions.bLongHeader      = bLongHeader;
	pVbi_Dec->next.SMPTE291Moptions.bBrokenDataCount = bBrokenDataCount;

	BDBG_LEAVE(BVBI_Decode_656_SetSMPTE291Moptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_SetGemstarOptions(
	BVBI_Decode_Handle decodeHandle,
	const BVBI_GSOptions* gsOptions
)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_Decode_SetGemstarOptions);

	/* check parameters */
	if (gsOptions == 0x0)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if (!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (gsOptions->baseline_bot <= 256)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember options as requested */
	pVbi_Dec->next.gsOptions = *gsOptions;
	pVbi_Dec->gsOptionsChanged = true;

	BDBG_LEAVE(BVBI_Decode_SetGemstarOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_GetGemstarOptions(
	BVBI_Decode_Handle decodeHandle,
	BVBI_GSOptions*    gsOptions
)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_Decode_GetGemstarOptions);

	/* check parameters */
	if (gsOptions == 0x0)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if (!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return options as requested */
	*gsOptions = pVbi_Dec->curr.gsOptions;

	BDBG_LEAVE(BVBI_Decode_GetGemstarOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_SetWssOptions(
	BVBI_Decode_Handle decodeHandle,
	uint16_t              vline576i
)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_Decode_SetWssOptions);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if (!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (vline576i > 255)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember options as requested */
	pVbi_Dec->next.wssVline576i = vline576i;

	BDBG_LEAVE(BVBI_Decode_SetWssOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_GetWssOptions(
	BVBI_Decode_Handle decodeHandle,
	uint16_t*            pVline576i
)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_Decode_GetWssOptions);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if (!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (pVline576i == 0x0) 
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return options as requested */
	*pVline576i = pVbi_Dec->curr.wssVline576i;

	BDBG_LEAVE(BVBI_Decode_GetWssOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_Data_isr( BVBI_Decode_Handle decodeHandle,
                            	BVBI_Field_Handle  fieldHandle,
								BAVC_Polarity polarity )
{
	BVBI_P_Handle* pVbi;
	BVBI_P_Decode_Handle* pVbi_Dec;
	BVBI_P_Field_Handle*  pVbi_Fld;
	BVBI_P_Decode_CNState* currentState;
	uint32_t whatActive;
#if (BVBI_P_NUM_VDEC >= 1)
	uint32_t ulErrInfo = 0;
#endif

	BDBG_ENTER(BVBI_Decode_Data_isr);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if(!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	pVbi = pVbi_Dec->pVbi;
	currentState = &pVbi_Dec->curr;
	whatActive = currentState->ulActive_Standards;
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Clear out the error bits.  The next set of function calls may
	   set them again. */
	pVbi_Fld->ulErrInfo = 0x0;
	pVbi_Fld->ulWhichPresent = 0x0;

	/* Decode the various forms of analog VBI data as configured */
	if ((pVbi_Dec->eSource == BAVC_SourceId_eVdec0) ||
	    (pVbi_Dec->eSource == BAVC_SourceId_eVdec1))
	{
#if (BVBI_P_NUM_VDEC >= 1) /** { **/
		if (whatActive & BVBI_P_SELECT_CC)
		{
			ulErrInfo = BVBI_P_CC_Decode_Data_isr (
							pVbi->hReg,
							pVbi_Dec->eSource,
							polarity,
							&pVbi_Fld->usCCData );
			if ( (ulErrInfo & BVBI_LINE_ERROR_CC_NODATA) == 0)
				pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_CC;
			pVbi_Fld->ulErrInfo |= ulErrInfo;
		}
		if (whatActive & BVBI_P_SELECT_CGMSA)
		{
			ulErrInfo = BVBI_P_CGMS_Decode_Data_isr (
							pVbi->hReg,
							pVbi_Dec->eSource,
							polarity,
							&pVbi_Fld->ulCGMSData );
			if ( (ulErrInfo & BVBI_LINE_ERROR_CGMS_NODATA) == 0)
				pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_CGMSA;
			pVbi_Fld->ulErrInfo |= ulErrInfo;
		}
		if (whatActive & BVBI_P_SELECT_WSS)
		{
			ulErrInfo = BVBI_P_WSS_Decode_Data_isr (
							pVbi->hReg,
							pVbi_Dec->eSource,
							polarity,
							&pVbi_Fld->usWSSData );
			if ( (ulErrInfo & BVBI_LINE_ERROR_WSS_NODATA) == 0)
				pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_WSS;
			pVbi_Fld->ulErrInfo |= ulErrInfo;
		}
#ifdef BVBI_P_HAS_VPSD
		if (whatActive & BVBI_P_SELECT_VPS)
		{
			ulErrInfo = BVBI_P_VPS_Decode_Data_isr (
							pVbi->hReg,
							pVbi_Dec->eSource,
							polarity,
							pVbi_Fld->pVPSData );
			if ( (ulErrInfo & BVBI_LINE_ERROR_VPS_NODATA) == 0)
				pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_VPS;
			pVbi_Fld->ulErrInfo |= ulErrInfo;
		}
#endif
#ifdef BVBI_P_HAS_GSD
		if (whatActive & BVBI_P_SELECT_GS)
		{
			ulErrInfo = BVBI_P_GS_Decode_Data_isr (
							pVbi->hReg,
							pVbi_Dec->eSource,
							currentState->eVideoFormat,
							polarity,
							pVbi_Fld->pGSData );
			if ( (ulErrInfo & BVBI_LINE_ERROR_GEMSTAR_NODATA) == 0)
				pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_GS;
			pVbi_Fld->ulErrInfo |= ulErrInfo;
		}
#endif
		if (whatActive & BVBI_P_SELECT_TT)
		{
			/* Debug code
			*(uint32_t*)(pVbi_Fld->TTDataO.pucData) = 0xDEADBEEF;
			*/
			BVBI_P_TTData* pttData;
			#ifdef BVBI_P_TTD_SCB2_ERROR
			uint8_t* ttScratch = 0x0;
			uint32_t scratchMask;
			int iBit;
			#endif
			if (polarity == BAVC_Polarity_eTopField)
			{
				BVBI_P_LCOP_TRANSFER_isr (
					pVbi_Dec, topTTDataO, pVbi_Fld, TTDataO,
					&pVbi->ttFreelist, clink);
				pttData = BVBI_P_LCOP_GET_isr (pVbi_Dec, topTTDataO);
				#ifdef BVBI_P_TTD_SCB2_ERROR
				ttScratch = pVbi_Dec->topTTscratch;
				#endif
			}
			else
			{
				BVBI_P_LCOP_TRANSFER_isr (
					pVbi_Dec, botTTDataO, pVbi_Fld, TTDataO,
					&pVbi->ttFreelist, clink);
				pttData = BVBI_P_LCOP_GET_isr (pVbi_Dec, botTTDataO);
				#ifdef BVBI_P_TTD_SCB2_ERROR
				ttScratch = pVbi_Dec->botTTscratch;
				#endif
			}
			#ifdef BVBI_P_TTD_SCB2_ERROR
			BKNI_Memcpy (pttData->pucData, ttScratch, 47);
			scratchMask = *(uint32_t*)(pttData->pucData);
			for (iBit = 0 ; iBit < 32 ; ++iBit)
			{
				uint32_t wiper = (1 << iBit);
				if ((scratchMask & wiper) != 0x0)
				{
					*(uint32_t*)(pttData->pucData) = wiper;
					break;
				}
			}
			#endif
			ulErrInfo = BVBI_P_TT_Decode_Data_isr (
							pVbi->hReg,
							pVbi->hMem,
							pVbi_Dec->eSource,
							currentState->eVideoFormat,
							polarity,
							pttData );
			/* Debug code 
			*(uint32_t*)(pttData->pucData) = 0x0;
			*/
			if ( (ulErrInfo & BVBI_LINE_ERROR_TELETEXT_NODATA) == 0)
			{
				int iLine;
				BVBI_P_TTData* capData = 
					BVBI_P_LCOP_GET_isr (pVbi_Fld, TTDataO);
				uint32_t lineMask = *(uint32_t*)(capData->pucData);
				for (iLine = 0 ; iLine < capData->ucLines ; ++iLine)
				{
					if ((lineMask & (1 << iLine)) != 0)
						break;
				}
				capData->firstLine = iLine;
				capData->lineMask = (lineMask >> iLine);
				*(uint32_t*)(capData->pucData) = 0xFFFFFFFF;
				pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_TT;
			}
			pVbi_Fld->ulErrInfo |= ulErrInfo;
		}
#endif /** } (BVBI_P_NUM_VDEC >= 1) **/
	}
#if (BVBI_P_NUM_IN656 > 0) /** { **/
	else /* Decode the various forms of ITU-R 656 VBI data as configured */
	{
		bool bDataFound;
		BVBI_P_IN656_Decode_Data_isr (
							pVbi_Dec->pVbi->hReg,
							pVbi_Dec->eSource,
							polarity,
							&bDataFound);
		if (bDataFound)
			BVBI_P_P656_Process_Data_isr (polarity, pVbi_Dec, pVbi_Fld);
	}
#endif /** } (BVBI_P_NUM_IN656 > 0) **/

	/* Record the data's field polarity */
	if (polarity == BAVC_Polarity_eFrame)
		pVbi_Fld->polarityMask = (1<<BAVC_Polarity_eTopField);
	else
		pVbi_Fld->polarityMask = (1<<polarity);

	BDBG_LEAVE(BVBI_Decode_Data_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Decode_ApplyChanges (
	BVBI_P_Decode_Handle* pVbi_Dec, BVBI_P_Decode_CNState* cnState, bool force)
{
	BVBI_P_Handle *pVbi;
	uint32_t whatActive;
#if (BVBI_P_NUM_VDEC > 0) || (BVBI_P_NUM_IN656 > 0) 
	bool isActive;
	BERR_Code eErr;
#endif
	BERR_Code firstErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_P_Decode_ApplyChanges);

#if (BVBI_P_NUM_VDEC == 0) && (BVBI_P_NUM_IN656 == 0) 
	BSTD_UNUSED(force);
#endif

	/* Get back pointer to VBI module, for convenience. */
	pVbi = pVbi_Dec->pVbi;

	/* More convenience */
	whatActive = cnState->ulActive_Standards;

	/* Program analog decoder cores not already done */
	if ((pVbi_Dec->eSource == BAVC_SourceId_eVdec0) ||
	    (pVbi_Dec->eSource == BAVC_SourceId_eVdec1))
	{
#if (BVBI_P_NUM_VDEC > 0) /** { **/
		isActive = ((whatActive & BVBI_P_SELECT_CC) != 0);
		if (force ||
			BVBI_P_Decode_IsDirty (pVbi_Dec, isActive, BVBI_P_SELECT_CC))
		{
			eErr = BERR_TRACE (BVBI_P_CC_Dec_Program (
				pVbi->hReg,
				pVbi_Dec->eSource,
				isActive,
				cnState->eVideoFormat));
			if (eErr != BERR_SUCCESS)
			{
				if (firstErr == BERR_SUCCESS)
					firstErr = eErr;
				if (!force)
					goto done;
			}
		}
		isActive = ((whatActive & BVBI_P_SELECT_CGMSA) != 0);
		if (force ||
			BVBI_P_Decode_IsDirty (pVbi_Dec, isActive, BVBI_P_SELECT_CGMSA))
		{
			eErr = BERR_TRACE (BVBI_P_CGMS_Dec_Program (
				pVbi->hReg, pVbi_Dec->eSource,
				isActive,
				cnState->eVideoFormat));
			if (eErr != BERR_SUCCESS)
			{
				if (firstErr == BERR_SUCCESS)
					firstErr = eErr;
				if (!force)
					goto done;
			}
		}
		isActive = ((whatActive & BVBI_P_SELECT_WSS) != 0);
		if (force ||
			BVBI_P_Decode_IsDirty (pVbi_Dec, isActive, BVBI_P_SELECT_WSS))
		{
			eErr = BERR_TRACE (BVBI_P_WSS_Dec_Program (
				pVbi->hReg,
				pVbi_Dec->eSource,
				isActive,
				cnState->eVideoFormat,
				cnState->wssVline576i));
			if (eErr != BERR_SUCCESS)
			{
				if (firstErr == BERR_SUCCESS)
					firstErr = eErr;
				if (!force)
					goto done;
			}
		}
#ifdef BVBI_P_HAS_VPSD
		isActive = ((whatActive & BVBI_P_SELECT_VPS) != 0);
		if (force ||
			BVBI_P_Decode_IsDirty (pVbi_Dec, isActive, BVBI_P_SELECT_VPS))
		{
			eErr = BERR_TRACE (BVBI_P_VPS_Dec_Program (
				pVbi->hReg,
				pVbi_Dec->eSource,
				isActive,
				cnState->eVideoFormat));
			if (eErr != BERR_SUCCESS)
			{
				if (firstErr == BERR_SUCCESS)
					firstErr = eErr;
				if (!force)
					goto done;
			}
		}
#endif
#ifdef BVBI_P_HAS_GSD
		isActive = ((whatActive & BVBI_P_SELECT_GS) != 0);
		if (force ||
			BVBI_P_Decode_IsDirty (pVbi_Dec, isActive, BVBI_P_SELECT_GS))
		{
			eErr = BERR_TRACE (BVBI_P_GS_Dec_Program (
				pVbi->hReg,
				pVbi_Dec->eSource,
				isActive,
				cnState->eVideoFormat,
				&cnState->gsOptions));
			if (eErr != BERR_SUCCESS)
			{
				if (firstErr == BERR_SUCCESS)
					firstErr = eErr;
				if (!force)
					goto done;
			}
		}
#endif
		isActive = ((whatActive & BVBI_P_SELECT_TT) != 0);
		if (force ||
			BVBI_P_Decode_IsDirty (pVbi_Dec, isActive, BVBI_P_SELECT_TT))
		{
			BVBI_P_TTData* ptopData = 0x0;
			BVBI_P_TTData* pbotData = 0x0;

			/* Allocate space for double buffering TT lines */
			if (isActive)
			{
				BVBI_P_LCOP_WRITE (
					pVbi_Dec, topTTDataO, &pVbi->ttFreelist, clink);
				ptopData = BVBI_P_LCOP_GET (pVbi_Dec, topTTDataO);
				BVBI_P_LCOP_WRITE (
					pVbi_Dec, botTTDataO, &pVbi->ttFreelist, clink);
				pbotData = BVBI_P_LCOP_GET (pVbi_Dec, botTTDataO);
			}

			/* Debug code
			*(uint32_t*)(pVbi_Dec->topTTDataO.pucData) = 0xDEADBEEF;
			*(uint32_t*)(pVbi_Dec->botTTDataO.pucData) = 0xDEADBEEF;
			*/

#ifdef BVBI_P_TTD_SCB2_ERROR
			eErr = BERR_TRACE (BVBI_P_TT_Dec_Program (
				pVbi->hReg,
				pVbi->hMem,
				pVbi_Dec->eSource,
				isActive,
				cnState->eVideoFormat,
				pVbi_Dec->topTTscratch,
				pVbi_Dec->botTTscratch,
				ptopData,
				pbotData));
#else
			eErr = BERR_TRACE (BVBI_P_TT_Dec_Program (
				pVbi->hReg,
				pVbi->hMem,
				pVbi_Dec->eSource,
				isActive,
				cnState->eVideoFormat,
				ptopData,
				pbotData));
#endif
			if (eErr != BERR_SUCCESS)
			{
				if (firstErr == BERR_SUCCESS)
					firstErr = eErr;
				if (!force)
					goto done;
			}
		}

		/* Program lines for interrupts, analog case */
		eErr = BERR_TRACE (BVBI_P_VDTOP_Dec_Program (
			pVbi->hReg,
			pVbi_Dec->eSource,
			cnState->eVideoFormat));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}

#endif /** } (BVBI_P_NUM_VDEC > 0) **/
	}
	else /* Program 656 input cores */
	{
#if (BVBI_P_NUM_IN656 > 0) /** { **/

		isActive = (whatActive != 0);

		/* Program the IN656 core */
		eErr = BERR_TRACE (BVBI_P_IN656_Dec_Program (
			pVbi->hReg,
			pVbi->hMem,
			pVbi_Dec->eSource,
			isActive,
			cnState->e656Format,
			&(cnState->SMPTE291Moptions),
			cnState->eVideoFormat,
			pVbi_Dec->top656Data,
			pVbi_Dec->bot656Data));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}

#if (BVBI_P_NUM_VDEC > 0)
		/* Program lines for interrupts, 656 case. */
		eErr = BERR_TRACE (BVBI_P_VDTOP_656_Dec_Program (
			pVbi->hReg,
			pVbi_Dec->eSource,
			cnState->eVideoFormat));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}
#endif
#endif /** } (BVBI_P_NUM_IN656 >= 1) **/
	}

#if (BVBI_P_NUM_VDEC > 0) || (BVBI_P_NUM_IN656 > 0) 
done:
#endif
	BDBG_LEAVE(BVBI_P_Decode_ApplyChanges);
	return firstErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Decode_SetCC(BVBI_Decode_Handle decodeHandle, bool bEnabled)
{
	return BVBI_P_Decode_Set_VBI (decodeHandle, bEnabled, BVBI_P_SELECT_CC);
}
BERR_Code BVBI_Decode_SetCGMS(BVBI_Decode_Handle decodeHandle, bool bEnabled)
{
	return BVBI_P_Decode_Set_VBI (decodeHandle, bEnabled, BVBI_P_SELECT_CGMSA);
}
BERR_Code BVBI_Decode_SetWSS(BVBI_Decode_Handle decodeHandle, bool bEnabled)
{
	return BVBI_P_Decode_Set_VBI (decodeHandle, bEnabled, BVBI_P_SELECT_WSS);
}
BERR_Code BVBI_Decode_SetVPS(BVBI_Decode_Handle decodeHandle, bool bEnabled)
{
	return BVBI_P_Decode_Set_VBI (decodeHandle, bEnabled, BVBI_P_SELECT_VPS);
}
BERR_Code BVBI_Decode_SetTeletext(
	BVBI_Decode_Handle decodeHandle, bool bEnabled)
{
	return BVBI_P_Decode_Set_VBI (decodeHandle, bEnabled, BVBI_P_SELECT_TT);
}
BERR_Code BVBI_Decode_SetGemstar(BVBI_Decode_Handle decodeHandle, bool bEnabled)
{
	return BVBI_P_Decode_Set_VBI (decodeHandle, bEnabled, BVBI_P_SELECT_GS);
}

BERR_Code BVBI_Decode_GetCC(BVBI_Decode_Handle decodeHandle, bool* pbEnabled)
{
	return BVBI_P_Decode_Get_VBI (decodeHandle, pbEnabled, BVBI_P_SELECT_CC);
}
BERR_Code BVBI_Decode_GetCGMS(BVBI_Decode_Handle decodeHandle, bool* pbEnabled)
{
	return BVBI_P_Decode_Get_VBI (decodeHandle, pbEnabled, BVBI_P_SELECT_CGMSA);
}
BERR_Code BVBI_Decode_GetWSS(BVBI_Decode_Handle decodeHandle, bool* pbEnabled)
{
	return BVBI_P_Decode_Get_VBI (decodeHandle, pbEnabled, BVBI_P_SELECT_WSS);
}
BERR_Code BVBI_Decode_GetVPS(BVBI_Decode_Handle decodeHandle, bool* pbEnabled)
{
	return BVBI_P_Decode_Get_VBI (decodeHandle, pbEnabled, BVBI_P_SELECT_VPS);
}
BERR_Code BVBI_Decode_GetTeletext(
	BVBI_Decode_Handle decodeHandle, bool* pbEnabled)
{
	return BVBI_P_Decode_Get_VBI (decodeHandle, pbEnabled, BVBI_P_SELECT_TT);
}
BERR_Code BVBI_Decode_GetGemstar(
	BVBI_Decode_Handle decodeHandle, bool* pbEnabled)
{
	return BVBI_P_Decode_Get_VBI (decodeHandle, pbEnabled, BVBI_P_SELECT_GS);
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Decode_Set_VBI(
	BVBI_Decode_Handle decodeHandle,
	bool bEnabled,
	uint32_t ulSelect_Standard)
{
	BVBI_P_Decode_Handle *pVbi_Dec;

	BDBG_ENTER(BVBI_P_Decode_Set_VBI);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if(!pVbi_Dec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Record change to desired state */
	if (bEnabled)
	{
		pVbi_Dec->next.ulActive_Standards |= ulSelect_Standard;
	}
	else
	{
		pVbi_Dec->next.ulActive_Standards &= ~ulSelect_Standard;
	}

	BDBG_LEAVE(BVBI_P_Decode_Set_VBI);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Decode_Get_VBI(
	BVBI_Decode_Handle decodeHandle,
	bool* pbEnabled,
	uint32_t ulSelect_Standard)
{
	BVBI_P_Decode_Handle* pVbi_Dec;

	BDBG_ENTER(BVBI_P_Decode_Get_VBI);

	/* check parameters */
	BVBI_P_GET_DECODE_CONTEXT(decodeHandle, pVbi_Dec);
	if((!pVbi_Dec) || (!pbEnabled))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return info */
	*pbEnabled =
		((pVbi_Dec->curr.ulActive_Standards & ulSelect_Standard) != 0);

	BDBG_LEAVE(BVBI_P_Decode_Get_VBI);
	return BERR_SUCCESS;
}

#if (BVBI_P_NUM_VDEC >= 1) /** { **/
/***************************************************************************
 *
 */
static bool BVBI_P_Decode_IsDirty (
	BVBI_P_Decode_Handle *pVbi_Dec, bool isActive, uint32_t ulSelect_Standard)
{
	BVBI_P_Decode_CNState* curr = &pVbi_Dec->curr;
	BVBI_P_Decode_CNState* next = &pVbi_Dec->next;
	bool retval = (
		((next->eVideoFormat != curr->eVideoFormat) &&
		isActive)
		||
		((curr->ulActive_Standards & ulSelect_Standard) !=
		 (next->ulActive_Standards & ulSelect_Standard) )
	  );

	if (ulSelect_Standard == BVBI_P_SELECT_GS)
		retval = retval || pVbi_Dec->gsOptionsChanged;

	else if (ulSelect_Standard == BVBI_P_SELECT_WSS)
		retval = retval || (curr->wssVline576i != next->wssVline576i);

	return retval;
}
#endif /** } (BVBI_P_NUM_VDEC >= 1) **/

/***************************************************************************
 *
 */
static void BVBI_P_Decode_ChangesApplied (BVBI_P_Decode_Handle *pVbi_Dec)
{
	pVbi_Dec->curr = pVbi_Dec->next;
	pVbi_Dec->gsOptionsChanged = false;
}
