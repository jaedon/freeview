/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_enc.c $
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 10/11/12 2:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/src/common/bvbi_enc.c $
 * 
 * Hydra_Software_Devel/23   10/11/12 2:10p darnstein
 * SW7346-909: Add display setting for VBI priority over analog copy
 * protection.
 * 
 * Hydra_Software_Devel/22   4/24/12 12:03p darnstein
 * SW7405-5643: BVBI API change. BVBI_Encode_Create() now includes a
 * settings argument. It is a data structure that includes an option for
 * ARIB style video output.
 * 
 * Hydra_Software_Devel/21   3/22/11 12:57p darnstein
 * SW7420-1693: CGMS for 480P video needs to take "VEC double rate
 * sampling" into account.
 * 
 * Hydra_Software_Devel/20   3/15/11 5:12p darnstein
 * SW7335-1139: "Method 2" for CGMS-B is the default, if hardware can
 * support it.
 * 
 * Hydra_Software_Devel/19   3/2/11 3:38p darnstein
 * SW7422-261: More problems with line numbering for PAL-M and multi-line
 * closed captioning. This time, chipsets whose CCE cores lack "SCTE"
 * extensions were affected.
 * 
 * Hydra_Software_Devel/18   11/16/10 5:12p darnstein
 * SW7405-4995: don't use memcmp on structs. Compare each struct field
 * individually.
 * 
 * Hydra_Software_Devel/17   10/8/10 6:10p darnstein
 * SW7405-4931: there were errors in previous check-in.
 * 
 * Hydra_Software_Devel/16   10/8/10 6:01p darnstein
 * SW7405-4931: Functions BVBI_Encode_Set* check for unsupported hardware
 * feature.
 * 
 * Hydra_Software_Devel/15   10/1/10 2:49p darnstein
 * SW7422-46: Adapt to 7422 and 7425 chipsets.
 * 
 * Hydra_Software_Devel/14   5/25/10 11:54a darnstein
 * SW7405-4393: last check-in was too complete.
 * 
 * Hydra_Software_Devel/13   5/24/10 2:24p darnstein
 * SW7405-4393: last check-in was incomplete.
 * 
 * Hydra_Software_Devel/12   5/24/10 2:02p darnstein
 * SW7405-4393: free a certain pointer when other operations fail. Memory
 * leak.
 * 
 * Hydra_Software_Devel/11   4/9/10 7:45p darnstein
 * SW7125-258,SW7125-259,SW7125-260: When creating decoder object, do all
 * ocations in a different order. This simplifies error-recovery logic
 * for the benefit of Coverity.
 * 
 * Hydra_Software_Devel/10   3/3/10 8:07p darnstein
 * SW7420-598: Adjust #ifdefs that select capabilities CGMS-B, CGMS-Bd.
 * 
 * Hydra_Software_Devel/9   12/10/09 6:46p darnstein
 * SW7420-498: Move memory management from "apply changes" functions to
 * "create" functions. Applies to teletext memory only.
 * 
 * Hydra_Software_Devel/8   11/18/09 3:54p darnstein
 * SW7468-24: Gemstar options now placed in dedicated data structure.
 * 
 * Hydra_Software_Devel/7   6/30/09 5:51p darnstein
 * PR56342: support for 7550 chipset.
 * 
 * Hydra_Software_Devel/6   4/6/09 2:38p darnstein
 * PR53826: simplify the #if statements that indicate chip capabilities.
 * 
 * Hydra_Software_Devel/5   3/30/09 1:26p darnstein
 * PR53666: completely remove automatic VBI enable/disable logic. It was
 * doing almost nothing, and causing this PR.
 * 
 * Hydra_Software_Devel/4   2/9/09 7:58p darnstein
 * PR45819: minor update for 7420 and CGMSAE core: version numbers.
 * 
 * Hydra_Software_Devel/3   12/17/08 8:12p darnstein
 * PR46112: allow CGMS-A and CGMS-B output at the same time.
 * 
 * Hydra_Software_Devel/2   12/8/08 7:36p darnstein
 * PR49987: fix bug in BVBI_Encode_Create(). Affects 656 output.
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

BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

static BERR_Code BVBI_P_Encode_ApplyChanges (
	BVBI_P_Encode_Handle* pVbi_Enc, BVBI_P_Encode_CNState* cnState, bool force);

static BERR_Code BVBI_P_Encode_Set_VBI (
	BVBI_Encode_Handle encodeHandle,
	bool bEnabled,
	uint32_t ulSelect_Standard);

static BERR_Code BVBI_P_Encode_Get_VBI (
	BVBI_Encode_Handle encodeHandle,
	bool* pbEnabled,
	uint32_t ulSelect_Standard);

static BERR_Code BVBI_P_Encode_656_Set_VBI (
	BVBI_Encode_Handle encodeHandle,
	bool bEnabled,
	uint32_t ulSelect_Standard);

static BERR_Code BVBI_P_Encode_656_Get_VBI (
	BVBI_Encode_Handle encodeHandle,
	bool* pbEnabled,
	uint32_t ulSelect_Standard);

static BERR_Code BVBI_P_Encode_XSER_Set_VBI (
	BVBI_Encode_Handle encodeHandle,
	bool bEnabled,
	uint32_t ulSelect_Standard);

static BERR_Code BVBI_P_Encode_XSER_Get_VBI (
	BVBI_Encode_Handle encodeHandle,
	bool* pbEnabled,
	uint32_t ulSelect_Standard);

static bool BVBI_P_Encode_IsDirty (
	BVBI_P_Encode_Handle *pVbi_Enc,
	uint32_t nextActive, uint32_t ulSelect_Standard);

static void BVBI_P_Encode_ChangesApplied (BVBI_P_Encode_Handle *pVbi_Enc);

static void P_UnionAllocatedCores (
	const uint8_t hwCoreIndexA[BVBI_P_EncCoreType_eLAST],
	const uint8_t hwCoreIndexB[BVBI_P_EncCoreType_eLAST],
	uint8_t hwCoreIndexResult[BVBI_P_EncCoreType_eLAST]);

static bool P_GsOptionsEqual (
	const BVBI_GSOptions* p1, const BVBI_GSOptions* p2);
static bool P_XserSettingsEqual (
	const BVBI_XSER_Settings* p1, const BVBI_XSER_Settings* p2);


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/

static const BVBI_Encode_Settings s_Encode_DefaultSettings = 
{
	false,						/* bArib480p          */
	false						/* bVbiPriority       */
};

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_GetDefaultSettings ( 
	BAVC_VbiPath         eDestination,	
	BVBI_Encode_Settings   *pSettings
)
{
	BERR_Code eErr = BERR_SUCCESS;
	BSTD_UNUSED (eDestination);

	if(!pSettings)
	{
		BDBG_ERR(("Invalid parameter\n"));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BVBI_Encode_GetDefaultSettings_Done;
	}

	*pSettings = s_Encode_DefaultSettings;

BVBI_Encode_GetDefaultSettings_Done:
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_Create(BVBI_Handle vbiHandle,
                           BAVC_VbiPath eDest,
						   const BVBI_Encode_Settings *pSettings,
                           BVBI_Encode_Handle *pEncodeHandle)
{
	BVBI_P_EncCoreType eType;
	BVBI_P_Handle *pVbi;
	BVBI_P_TTData* pttDataT = NULL;
	BVBI_P_TTData* pttDataB = NULL;
	BERR_Code eErr = BERR_SUCCESS;
	BVBI_P_Encode_Handle *pVbi_Enc = NULL;
	bool bVbiEncReserved = false;
#if (BVBI_P_NUM_ANCI656_656 > 0) 
	bool bAnci656Reserved = false;
#endif 

	BDBG_ENTER(BVBI_Encode_Create);

	/* check parameters */
	BVBI_P_GET_CONTEXT(vbiHandle, pVbi);
	if((!pVbi) || (!pEncodeHandle))
	{
		BDBG_ERR(("Invalid parameter\n"));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BVBI_Encode_Create_Done;
	}
	if ((eDest != BAVC_VbiPath_eVec0)
#if (BVBI_P_NUM_VEC >= 2)
     && (eDest != BAVC_VbiPath_eVec1)
#endif
#if (BVBI_P_NUM_VEC >= 3)
	 && (eDest != BAVC_VbiPath_eVec2)
#endif
#if (BVBI_P_NUM_PTVEC >= 1)
	 && (eDest != BAVC_VbiPath_eBypass0)
#endif
#if (BVBI_P_NUM_PTVEC >= 2)
	 && (eDest != BAVC_VbiPath_eBypass1)
#endif
	)
	{
		BDBG_ERR(("Invalid parameter\n"));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BVBI_Encode_Create_Done;
	}

	/* Check to see if the caller has already opened an encode handle that
	   controls the same video destination */
	for (pVbi_Enc = BLST_D_FIRST (&pVbi->encode_contexts) ;
		 pVbi_Enc ;
		 pVbi_Enc = BLST_D_NEXT (pVbi_Enc, link))
	{
		if (pVbi_Enc->eDest == eDest)
		{
			BDBG_ERR(("Multiple handles for one VBI core\n"));
			eErr = BERR_TRACE(BVBI_ERR_HW_CONFLICT);
			goto BVBI_Encode_Create_Done;
		}
	}

	/* Alloc the VBI encode handle. */
	pVbi_Enc =
		(BVBI_P_Encode_Handle*)(BKNI_Malloc(sizeof(BVBI_P_Encode_Handle)));
	if(!pVbi_Enc)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Encode_Create_Done;
	}

	/* Options provided by caler? */
	if (pSettings == NULL)
		pSettings = &s_Encode_DefaultSettings;

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVbi_Enc, 0x0, sizeof(BVBI_P_Encode_Handle));
	pVbi_Enc->bDisabled = false;
	pVbi_Enc->eDest     = eDest;
	pVbi_Enc->pVbi      = pVbi;

	/* Set current/next state to defaults */
	pVbi_Enc->curr.eVideoFormat             = BFMT_VideoFmt_eNTSC;
	pVbi_Enc->curr.ePixRep                  = BAVC_HDMI_PixelRepetition_eNone;
	pVbi_Enc->curr.h656options.e656Format   = BVBI_656Fmt_SAA7113;
	pVbi_Enc->curr.h656options.sdid         = BVBI_P_p656_SetEEbits(0);
	pVbi_Enc->curr.ulActive_Standards       = 0x0;
	pVbi_Enc->curr.ulActive_656_Standards   = 0x0;
	pVbi_Enc->curr.gsOptions.bTvg2x         = false;
	pVbi_Enc->curr.gsOptions.baseline_top   = 10;
	pVbi_Enc->curr.gsOptions.linemask_top   = 0x01F;
	pVbi_Enc->curr.gsOptions.baseline_bot   = 273;
	pVbi_Enc->curr.gsOptions.linemask_bot   = 0x01F;
	pVbi_Enc->curr.amolType                 = BVBI_AMOL_Type_I;
	pVbi_Enc->curr.scteType                 = BVBI_SCTE_Type_CCONLY;
	pVbi_Enc->curr.eCsc                     = BVBI_CSC_SCTE_NTSCYIQ;
	pVbi_Enc->curr.eCoCsc                   = BVBI_CSC_NONE;
#if defined(BVBI_P_CGMSAE_VER1) || defined(BVBI_P_CGMSAE_VER2) || \
    defined(BVBI_P_CGMSAE_VER4)
	pVbi_Enc->curr.bCea805dStyle            = false;
#else
	pVbi_Enc->curr.bCea805dStyle            = true;
#endif
	pVbi_Enc->curr.bPR18010_bad_line_number = false;
	pVbi_Enc->curr.xserSettings             = *BVBI_P_GetDefaultXserSettings();

	/* Clear out list of "in use" VEC hardware cores */
	for (eType = 0 ; eType < BVBI_P_EncCoreType_eLAST ; ++eType)
	{
		pVbi_Enc->curr.hwCoreIndex[eType] = 0xFF;
	}

	/* Allocate VBI_ENC core. This is a bit of a kludge. */
	eErr = BVBI_P_Encode_ReserveCore (
		eDest, BVBI_P_SELECT_VBIENC, 
		pVbi->vecHwCoreMask, pVbi->vecHwCoreMask_656,  
		pVbi_Enc->curr.hwCoreIndex);
	if (eErr != BERR_SUCCESS)
	{
		BDBG_ERR(("Hardware allocate failure\n"));
		goto BVBI_Encode_Create_Done;
	}
	bVbiEncReserved = true;

	/* Allocating ANCI656 core is another kludge */
#if (BVBI_P_NUM_ANCI656_656 > 0) /** { **/
	if (BVBI_P_is656 (pVbi_Enc->eDest))
	{
		eErr = BVBI_P_Encode_ReserveCore (
			eDest, BVBI_P_SELECT_ANCI, 
			pVbi->vecHwCoreMask, pVbi->vecHwCoreMask_656,  
			pVbi_Enc->curr.hwCoreIndex);
		if (eErr != BERR_SUCCESS)
		{
			BDBG_ERR(("Hardware allocate failure\n"));
			goto BVBI_Encode_Create_Done;
		}
		bAnci656Reserved = true;
	}
#endif /** } **/

	/* Next state is the same as current state for now. */
	pVbi_Enc->next = pVbi_Enc->curr;

	/* Special initialization for CGMS */
#ifdef P_CGMS_SOFTWARE_CRC
	pVbi_Enc->last_cgms_user = 0;
	pVbi_Enc->last_cgms_formatted =
		BVPI_P_CGMS_format_data (pVbi_Enc->last_cgms_user);
#endif

	/* Initialize teletext pointers for LCO */
	pttDataT = (BVBI_P_TTData*)(BKNI_Malloc(sizeof(BVBI_P_TTData)));
	if(!pttDataT)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Encode_Create_Done;
	}
	BKNI_Memset((void*)pttDataT, 0x0, sizeof(BVBI_P_TTData));
	eErr = BVBI_P_TTData_Alloc (
		pVbi->hMem, BVBI_TT_MAX_LINES, BVBI_TT_MAX_LINELENGTH, pttDataT);
	if (eErr != BERR_SUCCESS)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Encode_Create_Done;
	}

	pttDataB = (BVBI_P_TTData*)(BKNI_Malloc(sizeof(BVBI_P_TTData)));
	if(!pttDataB)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Encode_Create_Done;
	}
	BKNI_Memset((void*)pttDataB, 0x0, sizeof(BVBI_P_TTData));
	eErr = BVBI_P_TTData_Alloc (
		pVbi->hMem, BVBI_TT_MAX_LINES, BVBI_TT_MAX_LINELENGTH, pttDataB);
	if (eErr != BERR_SUCCESS)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Encode_Create_Done;
	}

	/* Allocate empty space for SCTE data */
	eErr = BVBI_P_Encode_AllocScte (
		pVbi->hMem,
		pVbi_Enc->hTopScteNrtv,
		pVbi_Enc->hBotScteNrtv,
		pVbi_Enc->hTopScteMono,
		pVbi_Enc->hBotScteMono,
		&pVbi_Enc->sctePamData
	);
	if (eErr != BERR_SUCCESS)
	{
		eErr = BERR_TRACE (eErr);
		goto BVBI_Encode_Create_Done;
	}
	/* Programming note:
	 * This is the last possible failure in this function. Therefore, there is
	 * no need to undo this action during error handling.
	 */

	/* ARIB option */
	pVbi_Enc->bArib480p = pSettings->bArib480p;

	/* Smart pointers come last. This will make the error handling logic easier
	 * for Coverity to understand. */
	BVBI_P_LCOP_CREATE (pVbi_Enc, topTTDataO, pttDataT, clink);
	BVBI_P_LCOP_CREATE (pVbi_Enc, botTTDataO, pttDataB, clink);

	/* Link encode context into VBI handle's list */
	BLST_D_INSERT_HEAD (&pVbi->encode_contexts, pVbi_Enc, link);

	/* All done. now return the new fresh context to user. */
	*pEncodeHandle = (BVBI_Encode_Handle)pVbi_Enc;

	/* Initialize magic number to the size of the struct */
	pVbi_Enc->ulBlackMagic = sizeof(BVBI_P_Encode_Handle);

  BVBI_Encode_Create_Done:

	BDBG_LEAVE(BVBI_Encode_Create);

	if (BERR_SUCCESS != eErr) 
	{
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
		if (NULL != pVbi_Enc)
		{
#if (BVBI_P_NUM_ANCI656_656 > 0) /** { **/
			if (bAnci656Reserved)
			{
				BVBI_P_Encode_ReleaseCore (
					eDest, BVBI_P_SELECT_ANCI, 0x0,
					pVbi->vecHwCoreMask, pVbi->vecHwCoreMask_656,  
					pVbi_Enc->curr.hwCoreIndex);
			}
#endif /** } **/
			if (bVbiEncReserved)
			{
				BVBI_P_Encode_ReleaseCore (
					eDest, BVBI_P_SELECT_VBIENC, 0x0,
					pVbi->vecHwCoreMask, pVbi->vecHwCoreMask_656,  
					pVbi_Enc->curr.hwCoreIndex);
			}
			pVbi_Enc->ulBlackMagic = 0x0;
			BKNI_Free ((void*)pVbi_Enc);
		}
	}

	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_Destroy(BVBI_Encode_Handle encodeHandle)
{
	uint32_t ulSelect_Standard;
	BVBI_P_Handle *pVbi;
	BVBI_P_Encode_Handle* pVbi_Enc;
	BVBI_P_TTData* pttData;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Encode_Destroy);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Shut down any active cores */
	pVbi_Enc->next.ulActive_Standards = 0x0;
	pVbi_Enc->next.ulActive_656_Standards = 0x0;
	eErr = BVBI_Encode_ApplyChanges( encodeHandle );

	/* Deallocate bulky data if necessary */
	pVbi = pVbi_Enc->pVbi;
	BVBI_P_LCOP_DESTROY (pVbi_Enc, topTTDataO, &pVbi->ttFreelist, clink);
	pttData = BVBI_P_LCOP_GET (pVbi_Enc, topTTDataO);
	BVBI_P_TTData_Alloc (pVbi->hMem, 0, 0, pttData);
	BKNI_Free ((void*)pttData);
	BVBI_P_LCOP_DESTROY (pVbi_Enc, botTTDataO, &pVbi->ttFreelist, clink);
	pttData = BVBI_P_LCOP_GET (pVbi_Enc, botTTDataO);
	BVBI_P_TTData_Alloc (pVbi->hMem, 0, 0, pttData);
	BKNI_Free ((void*)pttData);
	BVBI_P_Encode_FreeScte (
		pVbi->hMem,
		pVbi_Enc->hTopScteNrtv,
		pVbi_Enc->hBotScteNrtv,
		pVbi_Enc->hTopScteMono,
		pVbi_Enc->hBotScteMono,
		&pVbi_Enc->sctePamData
	);

	/* Release hardware cores for re-use */
	for (ulSelect_Standard = 0x1 ; ulSelect_Standard < BVBI_P_SELECT_LAST ; 
		 ulSelect_Standard <<= 1)
	{
		BVBI_P_Encode_ReleaseCore (
			pVbi_Enc->eDest, ulSelect_Standard, 0x0,
			pVbi->vecHwCoreMask, pVbi->vecHwCoreMask_656,  
			pVbi_Enc->next.hwCoreIndex);
	}

	/* Unlink from parent VBI context */
	BLST_D_REMOVE (&pVbi->encode_contexts, pVbi_Enc, link);

	/* The handle is invalid */
	pVbi_Enc->ulBlackMagic = 0;

	/* Release context in system memory */
	BKNI_Free((void*)pVbi_Enc);

	BDBG_LEAVE(BVBI_Encode_Destroy);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_ApplyChanges( BVBI_Encode_Handle encodeHandle )
{
	BVBI_P_Encode_Handle *pVbi_Enc;
	BERR_Code eErr;

	BDBG_ENTER(BVBI_Encode_ApplyChanges);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto done;
	}

	/* Try to apply changes */
	eErr = BVBI_P_Encode_ApplyChanges (pVbi_Enc, &pVbi_Enc->next, false);

	/* On failure, restore state. */
	if (eErr != BERR_SUCCESS)
	{
		(void)BVBI_P_Encode_ApplyChanges (pVbi_Enc, &pVbi_Enc->curr, true);
		goto done;
	}

	/* Hardware should now be fully programmed */
	BVBI_P_Encode_ChangesApplied (pVbi_Enc);

done:
	BDBG_LEAVE(BVBI_Encode_ApplyChanges);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_SetGemstarOptions(
	BVBI_Encode_Handle encodeHandle,
	const BVBI_GSOptions* gsOptions
)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_SetGemstarOptions);

	/* check parameters */
	if (gsOptions == 0x0)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if (!pVbi_Enc)
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
	pVbi_Enc->next.gsOptions = *gsOptions;

	BDBG_LEAVE(BVBI_Encode_SetGemstarOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_GetGemstarOptions(
	BVBI_Encode_Handle encodeHandle,
	BVBI_GSOptions*    gsOptions
)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_GetGemstarOptions);

	/* check parameters */
	if (gsOptions == 0x0)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if (!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return options as requested */
	*gsOptions = pVbi_Enc->curr.gsOptions;

	BDBG_LEAVE(BVBI_Encode_GetGemstarOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_SetAMOLOptions(
	BVBI_Encode_Handle encodeHandle,
	BVBI_AMOL_Type     amolType
)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_SetAMOLOptions);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if (!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	switch (amolType)
	{
	case BVBI_AMOL_Type_I:
	case BVBI_AMOL_Type_II_Lowrate:
	case BVBI_AMOL_Type_II_Highrate:
		break;

	default:
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember options as requested */
	pVbi_Enc->next.amolType = amolType;

	BDBG_LEAVE(BVBI_Encode_SetAMOLOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_SetSCTEOptions(
	BVBI_Encode_Handle encodeHandle,
	BVBI_SCTE_Type     scteType
)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_SetSCTEOptions);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if (!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	switch (scteType)
	{
	case BVBI_SCTE_Type_CCONLY:
	case BVBI_SCTE_Type_CCNRTV:
	case BVBI_SCTE_Type_CCPAM:
	case BVBI_SCTE_Type_CCMONO:
		break;

	default:
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember options as requested */
	pVbi_Enc->next.scteType = scteType;

	BDBG_LEAVE(BVBI_Encode_SetSCTEOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_GetAMOLOptions(
	BVBI_Encode_Handle encodeHandle,
	BVBI_AMOL_Type*    pAmolType
)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_GetAMOLOptions);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if (!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (pAmolType == 0x0)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return options as requested */
	*pAmolType = pVbi_Enc->curr.amolType;

	BDBG_LEAVE(BVBI_Encode_GetAMOLOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_GetSCTEOptions(
	BVBI_Encode_Handle encodeHandle,
	BVBI_SCTE_Type*    pScteType
)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_GetSCTEOptions);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if (!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (pScteType == 0x0)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return options as requested */
	*pScteType = pVbi_Enc->curr.scteType;

	BDBG_LEAVE(BVBI_Encode_GetSCTEOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_SetVideoFormat(BVBI_Encode_Handle encodeHandle,
									 BFMT_VideoFmt eVideoFormat)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_SetVideoFormat);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_Encode_SetVideoFormat);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember format as requested */
	pVbi_Enc->next.eVideoFormat = eVideoFormat;

	BDBG_LEAVE(BVBI_Encode_SetVideoFormat);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_SetHdmiPixelRepetition( 
	BVBI_Encode_Handle encodeHandle,
	BAVC_HDMI_PixelRepetition ePixRep
)
{
#ifdef BVBI_P_HAS_CROSSBAR_VEC

	BDBG_ENTER(BVBI_Encode_SetHdmiPixelRepetition);
	BSTD_UNUSED (encodeHandle);
	BSTD_UNUSED (ePixRep);
#else
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_SetHdmiPixelRepetition);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_Encode_SetHdmiPixelRepetition);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember format as requested */
	pVbi_Enc->next.ePixRep = ePixRep;
#endif

	BDBG_LEAVE(BVBI_Encode_SetHdmiPixelRepetition);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_GetVideoFormat(BVBI_Encode_Handle encodeHandle,
									 BFMT_VideoFmt *peVideoFormat)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_GetVideoFormat);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if((!pVbi_Enc) || (!peVideoFormat))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return format as requested */
	*peVideoFormat = pVbi_Enc->curr.eVideoFormat;

	BDBG_LEAVE(BVBI_Encode_GetVideoFormat);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_GetHdmiPixelRepetition(
	BVBI_Encode_Handle encodeHandle,
	BAVC_HDMI_PixelRepetition* pePixRep)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_GetHdmiPixelRepetition);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if((!pVbi_Enc) || (!pePixRep))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return format as requested */
	*pePixRep = pVbi_Enc->curr.ePixRep;

	BDBG_LEAVE(BVBI_Encode_GetHdmiPixelRepetition);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_SetCSC (
    BVBI_Encode_Handle encodeHandle, BVBI_CSC eCsc, BVBI_CSC eCoCsc)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_SetCSC);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_Encode_SetCSC);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	switch (eCsc)
	{
	case BVBI_CSC_SCTE_MYIQ:
	case BVBI_CSC_SCTE_NTSCYIQ:
	case BVBI_CSC_SCTE_YUV:
	case BVBI_CSC_SCTE_CO:
	case BVBI_CSC_NONE:
		break;
	default:
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_Encode_SetCSC);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}
	switch (eCoCsc)
	{
	case BVBI_CSC_SCTE_MYIQ:
	case BVBI_CSC_SCTE_NTSCYIQ:
	case BVBI_CSC_SCTE_YUV:
	case BVBI_CSC_SCTE_CO:
	case BVBI_CSC_NONE:
		break;
	default:
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_Encode_SetCSC);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	/* Remember format as requested */
	pVbi_Enc->next.eCsc   = eCsc;
	pVbi_Enc->next.eCoCsc = eCsc;

	BDBG_LEAVE(BVBI_Encode_SetCSC);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_GetCSC(
	BVBI_Encode_Handle encodeHandle, BVBI_CSC *peCsc, BVBI_CSC *peCoCsc)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_GetCSC);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if ((!pVbi_Enc) || (!peCsc) || (!peCoCsc))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return format as requested */
	*peCsc   = pVbi_Enc->curr.eCsc;
	*peCoCsc = pVbi_Enc->curr.eCoCsc;

	BDBG_LEAVE(BVBI_Encode_GetCSC);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_SetCGMSBstyle(BVBI_Encode_Handle encodeHandle,  
		                            bool bCea805dStyle)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_SetCGMSBstyle);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_Encode_GetCGMSBstyle);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
#if !defined(BVBI_P_CGMSAE_VER3) && !defined(BVBI_P_CGMSAE_VER5) 
	if (bCea805dStyle)
	{
		BDBG_ERR(("CGMS-B not supported by VEC hardware\n"));
		BDBG_LEAVE(BVBI_Encode_GetCGMSBstyle);
		return BERR_TRACE (BERR_NOT_SUPPORTED);
	}
#endif

	/* Remember format as requested */
	pVbi_Enc->next.bCea805dStyle = bCea805dStyle;

	BDBG_LEAVE(BVBI_Encode_SetCGMSBstyle);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_GetCGMSBstyle(BVBI_Encode_Handle encodeHandle,  
		                            bool* pbCea805dStyle)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_SetCGMSBstyle);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if ((!pVbi_Enc) || (!pbCea805dStyle))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return format as requested */
	*pbCea805dStyle = pVbi_Enc->curr.bCea805dStyle;

	BDBG_LEAVE(BVBI_Encode_SetCGMSBstyle);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_656_SetFormat(BVBI_Encode_Handle encodeHandle,
									BVBI_656Fmt anci656Fmt,
									uint8_t sdid)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_656_SetFormat);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_Encode_SetVideoFormat);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember format as requested */
	switch (anci656Fmt)
	{
	case BVBI_656Fmt_SAA7113:
	case BVBI_656Fmt_Modified_SAA7113:
	case BVBI_656Fmt_SAA7114:
	case BVBI_656Fmt_SAA7115:
	case BVBI_656Fmt_SMPTE291:
		pVbi_Enc->next.h656options.e656Format = anci656Fmt;
		break;
	default:
		BDBG_LEAVE(BVBI_Encode_SetVideoFormat);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Remember other options */
	pVbi_Enc->next.h656options.sdid = BVBI_P_p656_SetEEbits (sdid);

	BDBG_LEAVE(BVBI_Encode_656_SetFormat);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_656_GetFormat(BVBI_Encode_Handle encodeHandle,
									BVBI_656Fmt* pAnci656Fmt,
									uint8_t* psdid)
{
	BVBI_P_Encode_Handle *pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_656_GetFormat);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if ((!pVbi_Enc) || (!pAnci656Fmt) || (!psdid))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return data as requested */
	*pAnci656Fmt = pVbi_Enc->curr.h656options.e656Format;
	*psdid       = pVbi_Enc->curr.h656options.sdid;

	BDBG_LEAVE(BVBI_Encode_656_GetFormat);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_XSER_SetOptions (
	BVBI_Encode_Handle encodeHandle,
	BVBI_XSER_Settings* pSettings)
{
	BVBI_P_Encode_Handle* pVbi_Enc;

	BDBG_ENTER(BVBI_Encode_XSER_SetOptions);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if ((!pVbi_Enc) || (!pSettings))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* TODO: validate contents of caller's pSettings. */

	/* Record change to desired state */
	pVbi_Enc->next.xserSettings = *pSettings;

	/* Success */
	BDBG_LEAVE (BVBI_Encode_XSER_SetOptions);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_XSER_GetOptions (
	BVBI_Encode_Handle encodeHandle,
	BVBI_XSER_Settings* pSettings)
{
	BVBI_P_Encode_Handle* pVbi_Enc;

	BDBG_ENTER(BVBI_P_Encode_XSER_Get_VBI);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if ((!pVbi_Enc) || (!pSettings))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Send info to caller */
	*pSettings = pVbi_Enc->curr.xserSettings;

	/* Success */
	BDBG_LEAVE (BVBI_P_Encode_XSER_Get_VBI);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVBI_Encode_Data_isr( BVBI_Encode_Handle encodeHandle,
								BVBI_Field_Handle fieldHandle,
								BAVC_Polarity polarity )
{
	uint32_t whatActive;
	BVBI_P_Encode_Handle* pVbi_Enc;
	BVBI_P_Field_Handle*  pVbi_Fld;
	BVBI_P_Handle*        pVbi;
	BVBI_P_Encode_CNState* currentState;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Encode_Data_isr);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	if (fieldHandle)
	{
		BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
		if(!pVbi_Fld)
		{
			BDBG_ERR(("Invalid parameter\n"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}
	else
	{
		pVbi_Fld = NULL;
	}
	currentState = &pVbi_Enc->curr;

	/* If user has provided field data to encode */
	if (pVbi_Fld)
	{
		/* For convenience */
		pVbi = pVbi_Enc->pVbi;

		/* Check polarity */
		if (polarity != BAVC_Polarity_eFrame)
		{
			uint32_t mask = (uint32_t)1 << (uint32_t)polarity;
			if ((pVbi_Fld->polarityMask & mask) != mask )
			{
				return BERR_TRACE(BVBI_ERR_INVALID_FIELD_POLARITY);
			}
		}

		/* Encode the various forms of VBI data as configured */
		whatActive =
			currentState->ulActive_Standards |
			currentState->ulActive_656_Standards |
			currentState->ulActive_XSER_Standards;
		if ( (whatActive               & BVBI_P_SELECT_CC           ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_CC           ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_CC_NOENCODE)    )
		{
			(void)BVBI_P_CC_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eCCE],
					polarity,
					pVbi_Fld->usCCData );
		}
		if ( (whatActive               & BVBI_P_SELECT_CGMSA          ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_CGMSA          ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_CGMS_NOENCODE)    )
		{
			/* Avoid a lengthy calculation if possible */
			/* Transform data from user format to encoder format */
			uint32_t ulData = pVbi_Fld->ulCGMSData;
#ifdef P_CGMS_SOFTWARE_CRC
			if (ulData == pVbi_Enc->last_cgms_user)
			{
				ulData = pVbi_Enc->last_cgms_formatted;
			}
			else
			{
				pVbi_Enc->last_cgms_user = ulData;
				if (pVbi_Enc->curr.eVideoFormat != BFMT_VideoFmt_e576p_50Hz)
					ulData = BVPI_P_CGMS_format_data (ulData);
				pVbi_Enc->last_cgms_formatted = ulData;
			}
#else
			if (pVbi_Enc->curr.eVideoFormat != BFMT_VideoFmt_e576p_50Hz)
				ulData = BVPI_P_CGMS_format_data (ulData);
#endif

			(void)BVBI_P_CGMSA_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eCGMSAE],
					polarity,
					ulData );
		}
#if defined(BVBI_P_CGMSAE_VER2) || defined(BVBI_P_CGMSAE_VER3) || \
    defined(BVBI_P_CGMSAE_VER5)  /** { **/
		if ( (whatActive               & BVBI_P_SELECT_CGMSB          ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_CGMSB          ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_CGMS_NOENCODE)    )
		{
			(void)BVBI_P_CGMSB_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eCGMSAE],
					polarity,
					*(pVbi_Fld->pCgmsbDatum));
		}
#endif /** } **/
		if ( (whatActive               & BVBI_P_SELECT_WSS           ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_WSS           ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_WSS_NOENCODE)    )
		{
			(void)BVBI_P_WSS_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eWSE],
					polarity,
					pVbi_Fld->usWSSData );
		}

		if ( (whatActive               & BVBI_P_SELECT_VPS           ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_VPS           ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_VPS_NOENCODE)    )
		{
			(void)BVBI_P_VPS_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eWSE],
					polarity,
					pVbi_Fld->pVPSData );
		}
#if (BVBI_P_NUM_TTE > 0) || (BVBI_P_NUM_TTE_656 > 0) 
		if ( (whatActive               & BVBI_P_SELECT_TT                 ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_TT                 ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_TELETEXT_NOENCODE)    )
		{
			BVBI_P_TTData* pttData;
			if (polarity == BAVC_Polarity_eTopField)
			{
				BVBI_P_LCOP_COPY_isr (
					pVbi_Fld, TTDataO, pVbi_Enc, topTTDataO,
					&pVbi->ttFreelist, clink);
				pttData = BVBI_P_LCOP_GET_isr (pVbi_Enc, topTTDataO);
			}
			else
			{
				BVBI_P_LCOP_COPY_isr (
					pVbi_Fld, TTDataO, pVbi_Enc, botTTDataO,
					&pVbi->ttFreelist, clink);
				pttData = BVBI_P_LCOP_GET_isr (pVbi_Enc, botTTDataO);
			}
			(void)BVBI_P_TT_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					pVbi_Enc->pVbi->hMem,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eTTE],
					currentState->eVideoFormat,
					polarity,
					currentState->bPR18010_bad_line_number,
					pttData);
		}
#endif
#if (BVBI_P_NUM_GSE > 0)
		if ( (whatActive               & BVBI_P_SELECT_GS                ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_GS                ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_GEMSTAR_NOENCODE)    )
		{
			(void)BVBI_P_GS_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eGSE],
					currentState->eVideoFormat,
					polarity,
					pVbi_Fld->pGSData );
		}
#endif 
#if (BVBI_P_NUM_AMOLE > 0)
		if ( (whatActive               & BVBI_P_SELECT_AMOL              ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_AMOL              ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_AMOL_NOENCODE   )    )
		{
			(void)BVBI_P_AMOL_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eAMOLE],
					currentState->eVideoFormat,
					polarity,
					pVbi_Fld->amolType,
					pVbi_Fld->pAmolData );
		}
#endif 
		if ( (whatActive               & BVBI_P_SELECT_MCC              ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_MCC              ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_MCC_NOENCODE   )    )
		{
			(void)BVBI_P_MCC_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eCCE],
					currentState->eVideoFormat,
					pVbi_Enc->bArib480p,
					polarity,
					currentState->bPR18010_bad_line_number,
					pVbi_Fld->pMCCData );
		}
#if (BVBI_P_NUM_SCTEE > 0)  /** { **/
		if ( (whatActive               & BVBI_P_SELECT_SCTE              ) &&
			 (pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_SCTE              ) &&
			 !(pVbi_Fld->ulErrInfo     & BVBI_LINE_ERROR_SCTE_NOENCODE   )    )
		{
			(void)BVBI_P_SCTE_Encode_Data_isr (
					pVbi_Enc->pVbi->hReg,
					pVbi_Enc->pVbi->hMem,
					BVBI_P_is656(pVbi_Enc->eDest),
					pVbi_Enc->curr.hwCoreIndex[BVBI_P_EncCoreType_eSCTE],
					currentState->eVideoFormat,
					polarity,
					currentState->scteType,
					pVbi_Fld->pPScteData,
					pVbi_Enc->hTopScteNrtv,
					pVbi_Enc->hBotScteNrtv,
					pVbi_Enc->hTopScteMono,
					pVbi_Enc->hBotScteMono,
					&pVbi_Enc->sctePamData);
		}
#endif /** } BVBI_P_NUM_SCTEE **/
	}

	BDBG_LEAVE(BVBI_Encode_Data_isr);
	return eErr;
}

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Encode_ApplyChanges (
	BVBI_P_Encode_Handle *pVbi_Enc, BVBI_P_Encode_CNState* cnState, bool force)
{
	BVBI_P_Handle *pVbi;
	uint32_t nextActive;
	bool isActive;
	BERR_Code eErr;
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST];
	BERR_Code firstErr= BERR_SUCCESS;

	BDBG_ENTER(BVBI_P_Encode_ApplyChanges);

	/* Get back pointer to VBI module, for convenience. */
	pVbi = pVbi_Enc->pVbi;

	/* Cannot do closed caption and multi-line closed caption  at the
	 * same time, because the same hardware core is used for both forms.
	 */
	if ((cnState->ulActive_Standards & BVBI_P_SELECT_MCC) &&
	    (cnState->ulActive_Standards & BVBI_P_SELECT_CC)    )
	{
		eErr = BVBI_ERR_CORE_CONFLICT;
		if (firstErr == BERR_SUCCESS)
			firstErr = eErr;
		if (!force)
			goto done;
	}

	/* Form union of cores presently in use, and cores in use after changes are
	 * applied. Thus, we will be able to program cores that will later be
	 * disconnected */
	P_UnionAllocatedCores (
		pVbi_Enc->curr.hwCoreIndex, pVbi_Enc->next.hwCoreIndex, hwCoreIndex);

	/* Connect new encoder cores as required */
	BVBI_P_Encode_ConnectCores (
		pVbi->hReg, pVbi_Enc->eDest, hwCoreIndex);

	/* This step has to be done in advance of the programming of individual
	   encoder cores. Otherwise, the VEC may hang. See PR19719. */
	eErr = BERR_TRACE (BVBI_P_VE_Enc_Program (
		pVbi->hReg,
		BVBI_P_is656(pVbi_Enc->eDest),
		hwCoreIndex[BVBI_P_EncCoreType_eVBIENC],
		cnState->ulActive_Standards,
		cnState->ulActive_656_Standards,
		cnState->eVideoFormat));
	if (eErr != BERR_SUCCESS)
	{
		if (firstErr == BERR_SUCCESS)
			firstErr = eErr;
		if (!force)
			goto done;
	}

#if (BVBI_P_HAS_XSER_TT)
	eErr = BERR_TRACE (BVBI_P_ITU656_Enc_Program (
		pVbi->hReg,
		&cnState->xserSettings,
		cnState->ulActive_XSER_Standards));
	if (eErr != BERR_SUCCESS)
	{
		if (firstErr == BERR_SUCCESS)
			firstErr = eErr;
		if (!force)
			goto done;
	}
#endif

	/* 
	 * Program encoder cores not already done 
	 */

	/* Union of all VBI outputs for next state */
	nextActive =
		cnState->ulActive_Standards |
		cnState->ulActive_656_Standards |
		cnState->ulActive_XSER_Standards;

	isActive = ((nextActive & BVBI_P_SELECT_CC) != 0);
	if (force || BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_CC))
	{
		eErr = BERR_TRACE (BVBI_P_CC_Enc_Program (
			pVbi->hReg,
			BVBI_P_is656(pVbi_Enc->eDest),
			hwCoreIndex[BVBI_P_EncCoreType_eCCE],
			isActive,
			cnState->bPR18010_bad_line_number,
			cnState->eVideoFormat,
			pVbi_Enc->bArib480p));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}
	}
	isActive = ((nextActive & BVBI_P_SELECT_CGMSA) != 0);
	if (force ||
		BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_CGMSA))
	{
		eErr = BERR_TRACE (BVBI_P_CGMSA_Enc_Program (
			pVbi->hReg,
			BVBI_P_is656(pVbi_Enc->eDest),
			hwCoreIndex[BVBI_P_EncCoreType_eCGMSAE],
			isActive,
			cnState->eVideoFormat,
			pVbi_Enc->bArib480p,
			cnState->ePixRep));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}
	}
#if defined(BVBI_P_CGMSAE_VER2) || defined(BVBI_P_CGMSAE_VER3) || \
    defined(BVBI_P_CGMSAE_VER5)  /** { **/
	isActive = ((nextActive & BVBI_P_SELECT_CGMSB) != 0);
	if (force ||
		BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_CGMSB))
	{
		eErr = BERR_TRACE (BVBI_P_CGMSB_Enc_Program (
			pVbi->hReg,
			BVBI_P_is656(pVbi_Enc->eDest),
			hwCoreIndex[BVBI_P_EncCoreType_eCGMSAE],
			isActive,
			cnState->eVideoFormat,
			pVbi_Enc->bArib480p,
			cnState->ePixRep,
			cnState->bCea805dStyle));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}
	}
#endif /** } **/
	isActive = ((nextActive & BVBI_P_SELECT_WSS) != 0);
	if (force ||
		BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_WSS))
	{
		eErr = BERR_TRACE (BVBI_P_WSS_Enc_Program (
			pVbi->hReg,
			BVBI_P_is656(pVbi_Enc->eDest),
			hwCoreIndex[BVBI_P_EncCoreType_eWSE],
			isActive,
			cnState->bPR18010_bad_line_number,
			cnState->eVideoFormat));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}
	}
	isActive = ((nextActive & BVBI_P_SELECT_VPS) != 0);
	if (force ||
		BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_VPS))
	{
		eErr = BERR_TRACE (BVBI_P_VPS_Enc_Program (
			pVbi->hReg,
			BVBI_P_is656(pVbi_Enc->eDest),
			hwCoreIndex[BVBI_P_EncCoreType_eWSE],
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
	isActive = ((nextActive & BVBI_P_SELECT_TT) != 0);
	if (force || BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_TT))
	{
		/* Find DRAM for teletext storage */
		BVBI_P_TTData* ptopData;
		BVBI_P_TTData* pbotData;
		BVBI_P_LCOP_WRITE (
			pVbi_Enc, topTTDataO, &pVbi->ttFreelist, clink);
		ptopData =  BVBI_P_LCOP_GET (pVbi_Enc, topTTDataO);
		BVBI_P_LCOP_WRITE (
			pVbi_Enc, botTTDataO, &pVbi->ttFreelist, clink);
		pbotData =  BVBI_P_LCOP_GET (pVbi_Enc, botTTDataO);

#if (BVBI_P_NUM_TTE > 0) || (BVBI_P_NUM_TTE_656 > 0) 
		/* coverity[var_deref_model: FALSE] */
		eErr = BERR_TRACE (BVBI_P_TT_Enc_Program (
			pVbi->hReg,
			pVbi->hMem,
			BVBI_P_is656(pVbi_Enc->eDest),
			hwCoreIndex[BVBI_P_EncCoreType_eTTE],
			isActive,
			((cnState->ulActive_XSER_Standards & BVBI_P_SELECT_TT) != 0),
			cnState->eVideoFormat,
			pVbi->tteShiftDirMsb2Lsb,
			&cnState->xserSettings,
			ptopData,
			pbotData
		));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}
#endif
	}

#if (BVBI_P_NUM_GSE > 0)
	isActive = ((nextActive & BVBI_P_SELECT_GS) != 0);
	if (force || BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_GS))
	{
		eErr = BERR_TRACE (BVBI_P_GS_Enc_Program (
			pVbi->hReg,
			BVBI_P_is656(pVbi_Enc->eDest),
			hwCoreIndex[BVBI_P_EncCoreType_eGSE],
			isActive,
			cnState->eVideoFormat,
			pVbi_Enc->bArib480p,
			&cnState->gsOptions
		));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}
	}
#endif 
#if (BVBI_P_NUM_AMOLE > 0)
	isActive = ((nextActive & BVBI_P_SELECT_AMOL) != 0);
	if (force ||
		BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_AMOL))
	{
		eErr = BERR_TRACE (BVBI_P_AMOL_Enc_Program (
			pVbi->hReg,
			BVBI_P_is656(pVbi_Enc->eDest),
			hwCoreIndex[BVBI_P_EncCoreType_eAMOLE],
			isActive,
			cnState->eVideoFormat,
			pVbi_Enc->bArib480p,
			cnState->amolType
		));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}
	}
#endif 
	isActive = ((nextActive & BVBI_P_SELECT_MCC) != 0);
	/* Cannot initialize both CC and MCC. It is one or the other. */
	if ((nextActive & BVBI_P_SELECT_CC) == 0)
	{
		if (force ||
			BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_MCC))
		{
			eErr = BERR_TRACE (BVBI_P_MCC_Enc_Program (
				pVbi->hReg,
				BVBI_P_is656(pVbi_Enc->eDest),
				hwCoreIndex[BVBI_P_EncCoreType_eCCE],
				isActive,
				cnState->eVideoFormat,
				pVbi_Enc->bArib480p
			));
			if (eErr != BERR_SUCCESS)
			{
				if (firstErr == BERR_SUCCESS)
					firstErr = eErr;
				if (!force)
					goto done;
			}
		}
	}
#if (BVBI_P_NUM_SCTEE > 0)  /** { **/
	isActive = ((nextActive & BVBI_P_SELECT_SCTE) != 0);
	if (force ||
		BVBI_P_Encode_IsDirty (pVbi_Enc, nextActive, BVBI_P_SELECT_SCTE))
	{
		eErr = BERR_TRACE (BVBI_P_SCTE_Enc_Program (
			pVbi->hReg,
			BVBI_P_is656(pVbi_Enc->eDest),
			hwCoreIndex[BVBI_P_EncCoreType_eSCTE],
			isActive,
			cnState->eVideoFormat,
			pVbi_Enc->bArib480p,
			cnState->scteType,
			cnState->eCsc,
			cnState->eCoCsc
		));
		if (eErr != BERR_SUCCESS)
		{
			if (firstErr == BERR_SUCCESS)
				firstErr = eErr;
			if (!force)
				goto done;
		}
	}
#endif /** } BVBI_P_NUM_SCTEE **/

/*
 * The 7118, 7601 chipsets have no ANCI656 core
 * If modular VEC, there is only one ANCI656 core, associated with the
 * Ancillary VEC path
 * Otherwise, there is one ANCI656 core per VEC path.
 */
#if (BVBI_P_NUM_ANCI656_656 > 0) /** { **/
	if (hwCoreIndex[BVBI_P_EncCoreType_eANCI] != 0xFF)
	{
		if (BVBI_P_is656 (pVbi_Enc->eDest))
		{
			eErr = BERR_TRACE (BVBI_P_A656_Enc_Program (
				pVbi->hReg,
				hwCoreIndex[BVBI_P_EncCoreType_eANCI],
				&cnState->h656options,
				cnState->bPR18010_bad_line_number,
				cnState->eVideoFormat));
			if (eErr != BERR_SUCCESS)
			{
				if (firstErr == BERR_SUCCESS)
					firstErr = eErr;
				if (!force)
					goto done;
			}
		}
	}
#endif /** } BVBI_P_NUM_ANCI656 **/
/* TODO: Does the ANCI656 core need to be connected via the crossbar? */

	/* Disconnect old cores no longer required */
	BVBI_P_Encode_ConnectCores (
		pVbi->hReg, pVbi_Enc->eDest, pVbi_Enc->next.hwCoreIndex);

done:
	BDBG_LEAVE(BVBI_P_Encode_ApplyChanges);
	return firstErr;
}

/***************************************************************************
 *
 */

BERR_Code BVBI_Encode_SetCC(BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_CC);
}
BERR_Code BVBI_Encode_SetCGMSA(BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_CGMSA);
}
BERR_Code BVBI_Encode_SetCGMSB(BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
#if defined(BVBI_P_CGMSAE_VER2) || defined(BVBI_P_CGMSAE_VER3) || \
    defined(BVBI_P_CGMSAE_VER5)  /** { **/
#else
	BSTD_UNUSED(encodeHandle);
	if (bEnabled)
	{
		BDBG_ERR(("No CGMS-B hardware feature"));
		return BERR_TRACE(BVBI_ERR_HW_UNSUPPORTED);
	}
#endif
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_CGMSB);
}
BERR_Code BVBI_Encode_SetWSS(BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_WSS);
}
BERR_Code BVBI_Encode_SetVPS(BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_VPS);
}
BERR_Code BVBI_Encode_SetTeletext(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
#if (BVBI_P_NUM_TTE > 0) || (BVBI_P_NUM_TTE_656 > 0) 
#else
	BSTD_UNUSED(encodeHandle);
	if (bEnabled)
	{
		BDBG_ERR(("No teletext hardware feature"));
		return BERR_TRACE(BVBI_ERR_HW_UNSUPPORTED);
	}
#endif
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_TT);
}
BERR_Code BVBI_Encode_SetGemstar(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
#if (BVBI_P_NUM_GSE > 0)
#else
	BSTD_UNUSED(encodeHandle);
	if (bEnabled)
	{
		BDBG_ERR(("No Gemstar hardware feature"));
		return BERR_TRACE(BVBI_ERR_HW_UNSUPPORTED);
	}
#endif
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_GS);
}
BERR_Code BVBI_Encode_SetAMOL(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
#if (BVBI_P_NUM_AMOLE > 0)
#else
	BSTD_UNUSED(encodeHandle);
	if (bEnabled)
	{
		BDBG_ERR(("No AMOL hardware feature"));
		return BERR_TRACE(BVBI_ERR_HW_UNSUPPORTED);
	}
#endif
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_AMOL);
}
BERR_Code BVBI_Encode_SetMCC(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_MCC);
}
BERR_Code BVBI_Encode_SetSCTE(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
#if (BVBI_P_NUM_SCTEE > 0)  
#else
	BSTD_UNUSED(encodeHandle);
	if (bEnabled)
	{
		BDBG_ERR(("No SCTE hardware feature"));
		return BERR_TRACE(BVBI_ERR_HW_UNSUPPORTED);
	}
#endif
	return BVBI_P_Encode_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_SCTE);
}

BERR_Code BVBI_Encode_GetCC(BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_CC);
}
BERR_Code BVBI_Encode_GetCGMSA(BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_CGMSA);
}
BERR_Code BVBI_Encode_GetCGMSB(BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_CGMSB);
}
BERR_Code BVBI_Encode_GetWSS(BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_WSS);
}
BERR_Code BVBI_Encode_GetVPS(BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_VPS);
}
BERR_Code BVBI_Encode_GetTeletext(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_TT);
}
BERR_Code BVBI_Encode_GetGemstar(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_GS);
}
BERR_Code BVBI_Encode_GetAMOL(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_AMOL);
}
BERR_Code BVBI_Encode_GetMCC(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_MCC);
}
BERR_Code BVBI_Encode_GetSCTE(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_Get_VBI (encodeHandle, pbEnabled, BVBI_P_SELECT_SCTE);
}

BERR_Code BVBI_Encode_656_SetCC(BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_656_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_CC);
}
BERR_Code BVBI_Encode_656_SetWSS(BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_656_Set_VBI (
		encodeHandle, bEnabled, BVBI_P_SELECT_WSS);
}
BERR_Code BVBI_Encode_656_SetTeletext(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_656_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_TT);
}
BERR_Code BVBI_Encode_656_SetGemstar(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_656_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_GS);
}
BERR_Code BVBI_Encode_656_SetAMOL(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_656_Set_VBI (
		encodeHandle, bEnabled, BVBI_P_SELECT_AMOL);
}
BERR_Code BVBI_Encode_656_SetMCC(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_656_Set_VBI (
		encodeHandle, bEnabled, BVBI_P_SELECT_MCC);
}
BERR_Code BVBI_Encode_656_SetSCTE(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return BVBI_P_Encode_656_Set_VBI (
		encodeHandle, bEnabled, BVBI_P_SELECT_SCTE);
}

BERR_Code BVBI_Encode_656_GetCC(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_656_Get_VBI (
		encodeHandle, pbEnabled, BVBI_P_SELECT_CC);
}
BERR_Code BVBI_Encode_656_GetWSS(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_656_Get_VBI (
		encodeHandle, pbEnabled, BVBI_P_SELECT_WSS);
}
BERR_Code BVBI_Encode_656_GetTeletext(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_656_Get_VBI (
		encodeHandle, pbEnabled, BVBI_P_SELECT_TT);
}
BERR_Code BVBI_Encode_656_GetGemstar(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_656_Get_VBI (
		encodeHandle, pbEnabled, BVBI_P_SELECT_GS);
}
BERR_Code BVBI_Encode_656_GetAMOL(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_656_Get_VBI (
		encodeHandle, pbEnabled, BVBI_P_SELECT_AMOL);
}
BERR_Code BVBI_Encode_656_GetMCC(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_656_Get_VBI (
		encodeHandle, pbEnabled, BVBI_P_SELECT_MCC);
}
BERR_Code BVBI_Encode_656_GetSCTE(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_656_Get_VBI (
		encodeHandle, pbEnabled, BVBI_P_SELECT_SCTE);
}

BERR_Code BVBI_Encode_XSER_SetTeletext(
	BVBI_Encode_Handle encodeHandle, bool bEnabled)
{
	return
		BVBI_P_Encode_XSER_Set_VBI (encodeHandle, bEnabled, BVBI_P_SELECT_TT);
}
BERR_Code BVBI_Encode_XSER_GetTeletext(
	BVBI_Encode_Handle encodeHandle, bool* pbEnabled)
{
	return BVBI_P_Encode_XSER_Get_VBI (
		encodeHandle, pbEnabled, BVBI_P_SELECT_TT);
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Encode_Set_VBI(
	BVBI_Encode_Handle encodeHandle,
	bool bEnabled,
	uint32_t ulSelect_Standard)
{
	BVBI_P_Encode_Handle* pVbi_Enc;
	bool bIs656;
	BERR_Code eErr;


	BDBG_ENTER(BVBI_P_Encode_Set_VBI);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Is VEC path for 656 out? */
	bIs656 = BVBI_P_is656 (pVbi_Enc->eDest);

	/* Record change to desired state */
	if (bEnabled)
	{
		/* If path is for 656, we have an error */
		if (bIs656)
		{
			BDBG_ERR(("This VEC path only for 656 output"));
			return BERR_TRACE(BVBI_ERR_HW_UNSUPPORTED);
		}

		/* Reserve the required chip core */
		eErr = BVBI_P_Encode_ReserveCore (
			pVbi_Enc->eDest, ulSelect_Standard, 
			pVbi_Enc->pVbi->vecHwCoreMask, pVbi_Enc->pVbi->vecHwCoreMask_656,  
			pVbi_Enc->next.hwCoreIndex);
		if (eErr != BERR_SUCCESS)
		{
			BDBG_ERR(("Allocate failure\n"));
			return BERR_TRACE(eErr);
		}
		
		/* Hardware configuration comes later */
		pVbi_Enc->next.ulActive_Standards |= ulSelect_Standard;
	}
	else
	{
		/* If path is for 656, we have no work to do */
		if (bIs656)
		{
			BDBG_LEAVE(BVBI_P_Encode_Set_VBI);
			return BERR_SUCCESS;
		}

		/* Hardware configuration comes later */
		pVbi_Enc->next.ulActive_Standards &= ~ulSelect_Standard;

		/* Release hardware core (most of the time) */
		BVBI_P_Encode_ReleaseCore (
			pVbi_Enc->eDest, ulSelect_Standard, 
			pVbi_Enc->next.ulActive_Standards, 
			pVbi_Enc->pVbi->vecHwCoreMask, pVbi_Enc->pVbi->vecHwCoreMask_656,  
			pVbi_Enc->next.hwCoreIndex);
	}

	BDBG_LEAVE(BVBI_P_Encode_Set_VBI);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Encode_Get_VBI(
	BVBI_Encode_Handle encodeHandle,
	bool* pbEnabled,
	uint32_t ulSelect_Standard)
{
	BVBI_P_Encode_Handle* pVbi_Enc;

	BDBG_ENTER(BVBI_P_Encode_Get_VBI);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if((!pVbi_Enc) || (!pbEnabled))
	{
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_P_Encode_Get_VBI);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return info */
	*pbEnabled =
		((pVbi_Enc->curr.ulActive_Standards & ulSelect_Standard) != 0);

	BDBG_LEAVE(BVBI_P_Encode_Get_VBI);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Encode_656_Set_VBI(
	BVBI_Encode_Handle encodeHandle,
	bool bEnabled,
	uint32_t ulSelect_Standard)
{
	BVBI_P_Encode_Handle* pVbi_Enc;
	bool bIs656;
	BERR_Code eErr;

	BDBG_ENTER(BVBI_P_Encode_656_Set_VBI);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Is VEC path for 656 out? */
	bIs656 = BVBI_P_is656 (pVbi_Enc->eDest);

	/* Record change to desired state */
	if (bEnabled)
	{
		/* If path is not for 656, we have an error */
		if (!bIs656)
		{
			BDBG_ERR(("This VEC path not for 656 output"));
			return BERR_TRACE(BVBI_ERR_HW_UNSUPPORTED);
		}

		/* Reserve the required chip core */
		eErr = BVBI_P_Encode_ReserveCore (
			pVbi_Enc->eDest, ulSelect_Standard, 
			pVbi_Enc->pVbi->vecHwCoreMask, pVbi_Enc->pVbi->vecHwCoreMask_656,  
			pVbi_Enc->next.hwCoreIndex);
		if (eErr != BERR_SUCCESS)
		{
			BDBG_ERR(("Allocate failure\n"));
			return BERR_TRACE(eErr);
		}
		
		/* Hardware configuration comes later */
		pVbi_Enc->next.ulActive_656_Standards |= ulSelect_Standard;
	}
	else
	{
		/* If path is not for 656, we have no work to do */
		if (!bIs656)
		{
			BDBG_LEAVE(BVBI_P_Encode_656_Set_VBI);
			return BERR_SUCCESS;
		}

		/* Hardware configuration comes later */
		pVbi_Enc->next.ulActive_656_Standards &= ~ulSelect_Standard;

		/* Release hardware core (most of the time) */
		BVBI_P_Encode_ReleaseCore (
			pVbi_Enc->eDest, ulSelect_Standard,
			pVbi_Enc->next.ulActive_656_Standards, 
			pVbi_Enc->pVbi->vecHwCoreMask, pVbi_Enc->pVbi->vecHwCoreMask_656,  
			pVbi_Enc->next.hwCoreIndex);
	}

	BDBG_LEAVE(BVBI_P_Encode_656_Set_VBI);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Encode_656_Get_VBI(
	BVBI_Encode_Handle encodeHandle,
	bool* pbEnabled,
	uint32_t ulSelect_Standard)
{
	BVBI_P_Encode_Handle* pVbi_Enc;

	BDBG_ENTER(BVBI_P_Encode_656_Get_VBI);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if((!pVbi_Enc) || (!pbEnabled))
	{
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBI_P_Encode_656_Get_VBI);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return info */
	*pbEnabled =
		((pVbi_Enc->curr.ulActive_656_Standards & ulSelect_Standard) != 0);

	BDBG_LEAVE(BVBI_P_Encode_656_Get_VBI);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Encode_XSER_Set_VBI(
	BVBI_Encode_Handle encodeHandle,
	bool bEnabled,
	uint32_t ulSelect_Standard)
{
	BVBI_P_Encode_Handle* pVbi_Enc;

	BDBG_ENTER(BVBI_P_Encode_XSER_Set_VBI);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Record change to desired state */
	if (bEnabled)
	{
		pVbi_Enc->next.ulActive_XSER_Standards |= ulSelect_Standard;
	}
	else
	{
		pVbi_Enc->next.ulActive_XSER_Standards &= ~ulSelect_Standard;
	}

	BDBG_LEAVE(BVBI_P_Encode_XSER_Set_VBI);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVBI_P_Encode_XSER_Get_VBI(
	BVBI_Encode_Handle encodeHandle,
	bool* pbEnabled,
	uint32_t ulSelect_Standard)
{
	BVBI_P_Encode_Handle* pVbi_Enc;

	BDBG_ENTER(BVBI_P_Encode_XSER_Get_VBI);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if((!pVbi_Enc) || (!pbEnabled))
	{
		BDBG_ERR(("Invalid parameter\n"));
	BDBG_LEAVE(BVBI_P_Encode_Get_VBI);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return info */
	*pbEnabled =
		((pVbi_Enc->curr.ulActive_XSER_Standards & ulSelect_Standard) != 0);

	BDBG_LEAVE(BVBI_P_Encode_XSER_Get_VBI);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static bool BVBI_P_Encode_IsDirty (
	BVBI_P_Encode_Handle *pVbi_Enc,
	uint32_t nextActive, uint32_t ulSelect_Standard)
{
	BVBI_P_Encode_CNState* currentState = &pVbi_Enc->curr;
	BVBI_P_Encode_CNState* nextState    = &pVbi_Enc->next;
	uint32_t currentActive =
		currentState->ulActive_Standards |
		currentState->ulActive_656_Standards |
		currentState->ulActive_XSER_Standards;

	bool retval = (
		 (currentActive & ulSelect_Standard) !=
		 (nextActive    & ulSelect_Standard)   );

	if ((nextActive & ulSelect_Standard) != 0)
	{
		retval = 
			retval || (nextState->eVideoFormat != currentState->eVideoFormat);

		if (ulSelect_Standard == BVBI_P_SELECT_AMOL)
		{
			retval = retval || (nextState->amolType != currentState->amolType);
		}
		if (ulSelect_Standard == BVBI_P_SELECT_SCTE)
		{
			retval = retval || (nextState->scteType != currentState->scteType);
			retval = retval || (nextState->eCsc     != currentState->eCsc    );
			retval = retval || (nextState->eCoCsc   != currentState->eCoCsc  );
		}
		if (ulSelect_Standard == BVBI_P_SELECT_CGMSA)
		{
			retval = 
				retval ||
					(nextState->ePixRep != currentState->ePixRep);
		}
		if (ulSelect_Standard == BVBI_P_SELECT_CGMSB)
		{
			retval = 
				retval || 
					(nextState->bCea805dStyle != currentState->bCea805dStyle);
			retval = 
				retval ||
					(nextState->ePixRep != currentState->ePixRep);
		}
		if (ulSelect_Standard == BVBI_P_SELECT_GS)
		{
			retval = retval ||
				!P_GsOptionsEqual (
					&nextState->gsOptions, &currentState->gsOptions);
		}
		if (ulSelect_Standard == BVBI_P_SELECT_TT)
		{
			retval = retval ||
				!P_XserSettingsEqual (
					&nextState->xserSettings, &currentState->xserSettings);
		}
	}

	return retval;
}

/***************************************************************************
 *
 */
static void BVBI_P_Encode_ChangesApplied (BVBI_P_Encode_Handle *pVbi_Enc)
{
	pVbi_Enc->curr = pVbi_Enc->next;
}

/***************************************************************************
 *
 */
bool BVBI_P_is656 (BAVC_VbiPath eDest)
{
	bool bIs656;

	switch (eDest)
	{
	case BAVC_VbiPath_eVec0:
	case BAVC_VbiPath_eVec1:
	case BAVC_VbiPath_eVec2:
		bIs656 = false;
		break;

	case BAVC_VbiPath_eBypass0:
	case BAVC_VbiPath_eBypass1:
		bIs656 = true;
		break;

	default:
		bIs656 = false;
		BDBG_ASSERT (0);
		break;
	}

	return bIs656;
}

/***************************************************************************
 *
 */
void P_UnionAllocatedCores (
	const uint8_t hwCoreIndexA[BVBI_P_EncCoreType_eLAST],
	const uint8_t hwCoreIndexB[BVBI_P_EncCoreType_eLAST],
	uint8_t hwCoreIndexResult[BVBI_P_EncCoreType_eLAST])
{
	BVBI_P_EncCoreType eType;

	for (eType = 0 ; eType < BVBI_P_EncCoreType_eLAST ; ++eType)
	{
		uint8_t indexA = hwCoreIndexA[eType];
		if (indexA == 0xFF)
			hwCoreIndexResult[eType] = hwCoreIndexB[eType];
		else
			hwCoreIndexResult[eType] = indexA;
	}
}


/***************************************************************************
 *
 */
static bool P_GsOptionsEqual (
	const BVBI_GSOptions* p1, const BVBI_GSOptions* p2)
{
	BDBG_ASSERT (BVBI_GSOptions_Version == 1);

	return 
		(p1->bTvg2x       == p2->bTvg2x      ) &&
		(p1->baseline_top == p2->baseline_top) &&
		(p1->linemask_top == p2->linemask_top) &&
		(p1->baseline_bot == p2->baseline_bot) &&
		(p1->linemask_bot == p2->linemask_bot) ;
}

/***************************************************************************
 *
 */
static bool P_XserSettingsEqual (
	const BVBI_XSER_Settings* p1, const BVBI_XSER_Settings* p2)
{
	BDBG_ASSERT (BVBI_XSER_Settings_Version == 1);

	return
		(p1->xsSerialDataContent    == p2->xsSerialDataContent   ) &&
		(p1->ttSerialDataSync       == p2->ttSerialDataSync      ) &&
		(p1->iTTserialDataSyncDelay == p2->iTTserialDataSyncDelay) ;
}


/**********************
* Engineering special *
**********************/

/***************************************************************************
 *
 */
BERR_Code BVBI_P_Encode_SetPR18010_behavior(
	BVBI_Encode_Handle encodeHandle, bool bPR18010_bad_line_number )
{
	BVBI_P_Encode_Handle *pVbi_Enc;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Encode_SetPR18010_behavior);

	/* check parameters */
	BVBI_P_GET_ENCODE_CONTEXT(encodeHandle, pVbi_Enc);
	if(!pVbi_Enc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto done;
	}

	/* Accept user's setting */
	pVbi_Enc->next.bPR18010_bad_line_number = bPR18010_bad_line_number;

done:
	BDBG_LEAVE(BVBI_Encode_SetPR18010_behavior);
	return eErr;

}
