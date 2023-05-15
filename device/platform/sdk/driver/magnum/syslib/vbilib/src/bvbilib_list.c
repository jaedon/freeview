/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbilib_list.c $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 9/10/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/vbilib/src/bvbilib_list.c $
 * 
 * Hydra_Software_Devel/19   9/10/12 4:18p darnstein
 * SW7552-344: change some Coverity conventional comments.
 * 
 * Hydra_Software_Devel/18   10/31/08 5:49p darnstein
 * PR41250,PR41252: port over conventional comments from 7401 branch.
 * 
 * Hydra_Software_Devel/17   6/6/08 5:38p darnstein
 * PR38956: allocate space for CGMS-B data in field handles.
 * 
 * Hydra_Software_Devel/16   4/3/08 5:49p darnstein
 * PR38956,PR40709: New API elements supporting CGMS-B encoding.
 * 
 * Hydra_Software_Devel/15   11/13/07 12:46p darnstein
 * PR35092: Fix the logic error discovered by Coverity in version
 * Refsw_Dedicated_vbi_7401/1.
 * 
 * Hydra_Software_Devel/14   9/11/07 5:17p darnstein
 * PR25708: First release of SCTE encoder software.
 * 
 * Hydra_Software_Devel/13   6/6/07 12:50p darnstein
 * PR30411: multi-line closed caption encoding is ready for test.
 * 
 * Hydra_Software_Devel/12   1/18/07 3:16p darnstein
 * PR26464: correctly handle teletext output to multiple VECs
 * 
 * Hydra_Software_Devel/12   1/18/07 3:13p darnstein
 * PR26464: correctly handle teletext output to multiple VECs
 * 
 * Hydra_Software_Devel/11   4/11/06 7:26p darnstein
 * PR20429: first cut at AMOL encoder software.
 * 
 * Hydra_Software_Devel/10   10/21/05 7:46p darnstein
 * PR17732: Gemstar encoder can output a waveform, but I can't make it
 * change yet.
 * 
 * Hydra_Software_Devel/9   12/13/04 7:41p darnstein
 * PR 13255: When recycling a field handle, clear out all its attributes.
 * 
 * Hydra_Software_Devel/8   11/16/04 1:01p shyam
 * PR 8365 : Added BERR_TRACE for error returns
 * 
 * Hydra_Software_Devel/7   7/27/04 11:11a darnstein
 * PR 9080: don't use private functions to access field handle usage count
 * in BVBI porting interface.  It is against the rules.
 * 
 * Hydra_Software_Devel/6   5/11/04 6:58p darnstein
 * PR 9080: fix a stupid typo.
 * 
 * Hydra_Software_Devel/5   5/11/04 6:54p darnstein
 * PR 9080: add support for user processing of VBI data, simultaneously
 * with hardware encoding of same VBI data.
 * 
 * Hydra_Software_Devel/4   2/27/04 6:17p darnstein
 * PR 9080: fix misuse of BDBG_ASSERT macro.
 * 
 * Hydra_Software_Devel/3   11/19/03 7:45p darnstein
 * Fix mis-spelling of BVBIlib_List_Create, another minor typo too.
 * 
 * Hydra_Software_Devel/2   10/21/03 3:01p darnstein
 * Fix usage of BDBG_ENTER(), BDBG_LEAVE().
 * 
 * Hydra_Software_Devel/1   10/14/03 11:32a darnstein
 * Initial version
 * 
 ***************************************************************************/
#include "bstd.h"
#include "berr.h"
#include "berr_ids.h"
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */
#include "bvbi.h"		 /* VBI p.i. API */
#include "bvbi_prot.h"           /* VBI p.i. protected data structures */
#include "bvbilib.h"             /* This module. */
#include "bvbilib_priv.h"        /* VBI lib internal data structures */

BDBG_MODULE(BVBIlib);

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

/***************************************************************************
* Local (private) data 
***************************************************************************/
static const BVBIlib_List_Settings sDefaultSettings = {
	false, false, false, false, false, false, 0, 0, 0, 0
};

/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BVBIlib_List_GetDefaultSettings (BVBIlib_List_Settings*  pSettings)
{
	if (!pSettings)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	*pSettings = sDefaultSettings;

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBIlib_List_Create(
	BVBIlib_List_Handle *pVbillHandle,	
	BVBI_Handle             vbiHandle,
	int                      nHandles,
	const BVBIlib_List_Settings*  pSettings
)
{
	BVBIlib_P_List_Handle *pVbilibl;
	int iHandle;
	BVBIlib_P_FieldHanger *hanger;
	BVBI_Field_Handle hField;
	BERR_Code eErr;

	BDBG_ENTER(BVBIlib_List_Create);

	if((!pVbillHandle) ||
	   (nHandles <= 0)   )
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Alloc the main VBI context. */
	pVbilibl = 
		(BVBIlib_P_List_Handle*)(BKNI_Malloc(sizeof(BVBIlib_P_List_Handle)));

	if(!pVbilibl)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVbilibl, 0x0, sizeof(BVBIlib_P_List_Handle));

	/* Initialize magic number to the size of the struct */
	pVbilibl->ulBlackMagic = sizeof(BVBIlib_P_List_Handle);

	/* Store memory handle to allocate teletext data as necessary */
	pVbilibl->hVbi = vbiHandle;

	/* Store various settings */
	if (!pSettings)
		pSettings = &sDefaultSettings;
	pVbilibl->settings = *pSettings;
	/* Programming note: these settings are not really needed. But in a future
	   ----------------  version of the module, they might be? */

	/* Initialize empty lists of field handle hangers */
	BLST_Q_INIT(&pVbilibl->field_contexts);
	BLST_Q_INIT(&pVbilibl->empty_hangers);

	/* Loop over handles to be created */
	for (iHandle = 0 ; iHandle < nHandles ; ++iHandle)
	{
		/* Create a field handle */
		if ((eErr = BVBI_Field_Create (&hField, vbiHandle)) != BERR_SUCCESS)
		{
			BVBIlib_List_Destroy ((BVBIlib_List_Handle)pVbilibl);
			BDBG_LEAVE(BVBIlib_List_Create);
			return eErr;
		}

		/* Create a field handle hanger to hold the above handle */
		hanger = 
			(BVBIlib_P_FieldHanger*)(BKNI_Malloc(
				sizeof(BVBIlib_P_FieldHanger)));
		if(!hanger)
		{
			BVBI_Field_Destroy (hField);
			BVBIlib_List_Destroy ((BVBIlib_List_Handle)pVbilibl);
			BDBG_LEAVE(BVBIlib_List_Create);
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}
		hanger->hField = hField;

		/* Add field/hanger assembly to the free list */
		BLST_Q_INSERT_HEAD (&pVbilibl->field_contexts, hanger, link);

		/* Allocate for teletext if necessary */
		if (pSettings->bAllowTeletext)
		{
			if ((eErr = 
				BVBI_Field_TT_Allocate (
					hField, BVBI_TT_MAX_LINES, BVBI_TT_MAX_LINELENGTH)) !=
				BERR_SUCCESS)
			{
				BVBIlib_List_Destroy ((BVBIlib_List_Handle)pVbilibl);
				BDBG_LEAVE(BVBIlib_List_Create);
				return eErr;
			}
		}

		/* Allocate for VPS if necessary */
		if (pSettings->bAllowVPS)
		{
			if ((eErr = BVBI_Field_VPS_Allocate (hField, true)) != BERR_SUCCESS)
			{
				BVBIlib_List_Destroy ((BVBIlib_List_Handle)pVbilibl);
				BDBG_LEAVE(BVBIlib_List_Create);
				return eErr;
			}
		}

		/* Allocate for Gemstar if necessary */
		if (pSettings->bAllowGemstar)
		{
			if ((eErr = BVBI_Field_GS_Allocate (hField, true)) != BERR_SUCCESS)
			{
				BVBIlib_List_Destroy ((BVBIlib_List_Handle)pVbilibl);
				BDBG_LEAVE(BVBIlib_List_Create);
				return eErr;
			}
		}

		/* Allocate for AMOL if necessary */
		if (pSettings->bAllowAmol)
		{
			if ((eErr = 
				BVBI_Field_AMOL_Allocate (hField, BVBI_AMOL_Type_II_Highrate)) 
				!= BERR_SUCCESS)
			{
				BVBIlib_List_Destroy ((BVBIlib_List_Handle)pVbilibl);
				BDBG_LEAVE(BVBIlib_List_Create);
				return eErr;
			}
		}

		/* Allocate for multi-line closed caption, if necessary */
		if (pSettings->bAllowMcc)
		{
			if ((eErr = BVBI_Field_MCC_Allocate (hField, true)) != BERR_SUCCESS)
			{
				BVBIlib_List_Destroy ((BVBIlib_List_Handle)pVbilibl);
				BDBG_LEAVE(BVBIlib_List_Create);
				return eErr;
			}
		}

		/* Allocate for CGMS-B, if necessary */
		if (pSettings->bAllowCgmsB)
		{
			if ((eErr = BVBI_Field_CGMSB_Allocate (hField, true)) != 
				BERR_SUCCESS)
			{
				BVBIlib_List_Destroy ((BVBIlib_List_Handle)pVbilibl);
				BDBG_LEAVE(BVBIlib_List_Create);
				return eErr;
			}
		}

		/* Allocate for SCTE if necessary.  Not space efficient. */
		if ((pSettings->scteCcSize  != 0) || (pSettings->scteNrtvSize != 0) ||
			(pSettings->sctePamSize != 0) || (pSettings->scteMonoSize != 0)   )
		{
			if ((eErr = 
				BVBI_Field_SCTE_Allocate (hField, 
					pSettings->scteCcSize,  pSettings->scteNrtvSize, 
					pSettings->sctePamSize, pSettings->scteMonoSize)) 
				!= BERR_SUCCESS)
			{
				BVBIlib_List_Destroy ((BVBIlib_List_Handle)pVbilibl);
				BDBG_LEAVE(BVBIlib_List_Create);
				return eErr;
			}
		}
	}

	/* Initialize status counters */
	pVbilibl->nAllocated = nHandles;
	pVbilibl->nInUse     =        0;

	/* All done. now return the new fresh context to user. */
	*pVbillHandle = (BVBIlib_List_Handle)pVbilibl;

	BDBG_LEAVE(BVBIlib_List_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_List_Destroy( BVBIlib_List_Handle listHandle )
{
	BVBIlib_P_List_Handle *pVbill;
	BVBIlib_P_FieldHanger* hanger;
	BVBI_Field_Handle      hField;
	BERR_Code eErr      = BERR_SUCCESS;
	BERR_Code eErrFirst = BERR_SUCCESS;

	BDBG_ENTER(BVBIlib_List_Destroy);

	/* check parameters */
	BVBILIB_P_GET_LIST_CONTEXT(listHandle, pVbill);
	if(!pVbill)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Determine if user has failed to turn in all handles */
	if (pVbill->nInUse != 0)
	{
		BDBG_ERR(("Field handle leak\n"));
		return BERR_TRACE(BVBIlib_USER_LEAK);
	}

	/* Clear out list of free handles */
	/* coverity[alias] */
	/* coverity[USE_AFTER_FREE] */
	while ((hanger = BLST_Q_FIRST (&pVbill->field_contexts)) != NULL)
	{
		BLST_Q_REMOVE (&pVbill->field_contexts, hanger, link);
		hField = hanger->hField;
		if ((eErr = BVBI_Field_Destroy (hField)) != BERR_SUCCESS)
		{
			if (eErrFirst == BERR_SUCCESS)
			{
				eErrFirst = eErr;
			}
		}
		BKNI_Free ((void*)hanger);
	}

	/* Clear out list of unused field handle hangers */
	/* coverity[alias] */
	/* coverity[USE_AFTER_FREE] */
	while ((hanger = BLST_Q_FIRST (&pVbill->empty_hangers)) != NULL)
	{
		BLST_Q_REMOVE (&pVbill->empty_hangers, hanger, link);
		/* coverity[freed_arg] */
		BKNI_Free ((void*)hanger);
	}

	/* The handle is now invalid */
	pVbill->ulBlackMagic = 0;

	/* Release context in system memory */
	BKNI_Free((void*)pVbill);

	BDBG_LEAVE(BVBIlib_List_Destroy);
	return eErrFirst;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_List_Obtain_isr(
	BVBIlib_List_Handle   vbillHandle,
	BVBI_Field_Handle *  pFieldHandle
)
{
	BVBIlib_P_List_Handle *pVbill;
	BVBIlib_P_FieldHanger* hanger;
	BERR_Code eErr      = BERR_SUCCESS;

	BDBG_ENTER(BVBIlib_List_Obtain_isr);

	/* check parameters */
	BVBILIB_P_GET_LIST_CONTEXT(vbillHandle, pVbill);
	if(!pVbill)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* If there is a handle available */
	if ((hanger = BLST_Q_FIRST (&pVbill->field_contexts)) != NULL)
	{
		/* Take it out of the free list */
		BLST_Q_REMOVE (&pVbill->field_contexts, hanger, link);

		/* Adjust attributes */
		BVBI_Field_Zero_UsageCount_isr (hanger->hField);
		BVBI_Field_ClearState_isr      (hanger->hField);

		/* Give it to the caller */
		*pFieldHandle = hanger->hField;

		/* Hold on to the empty hanger for use later */
		hanger->hField = NULL;
		BLST_Q_INSERT_TAIL (&pVbill->empty_hangers, hanger, link);

		/* Adjust statistic */
		++(pVbill->nInUse);
	}
	else
	{
		/* Complain */
		eErr = BERR_TRACE(BVBIlib_NO_HANDLES);
	}

	BDBG_LEAVE(BVBIlib_List_Obtain_isr);
	return eErr;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_List_Return_isr(
	BVBIlib_List_Handle    vbillHandle,
	BVBI_Field_Handle     fieldHandle
)
{
	BVBIlib_P_List_Handle *pVbill;
	BVBIlib_P_FieldHanger *hanger;

	BDBG_ENTER(BVBIlib_List_Return_isr);

	/* check parameters */
	BVBILIB_P_GET_LIST_CONTEXT(vbillHandle, pVbill);
	if(!pVbill)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Usage check */
	if (BVBI_Field_Get_UsageCount_isr (fieldHandle) != 0)
		return BVBIlib_USER_LEAK;

	/* If there are no empty hangers left, then the list is corrupted */
	hanger = BLST_Q_FIRST (&pVbill->empty_hangers);
	BDBG_ASSERT (hanger != NULL);
	BLST_Q_REMOVE (&pVbill->empty_hangers, hanger, link);

	/* Hook up the user's field hanger and file it */
	hanger->hField = fieldHandle;
	BLST_Q_INSERT_HEAD (&pVbill->field_contexts, hanger, link);

	/* Adjust statistic */
	--(pVbill->nInUse);

	BDBG_LEAVE(BVBIlib_List_Return_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
void BVBIlib_List_IncrementUsage_isr(
	BVBI_Field_Handle     fieldHandle
)
{
	BDBG_ENTER(BVBIlib_List_IncrementUsage_isr);

	/* Just increment usage count */
	BVBI_Field_Increment_UsageCount_isr (fieldHandle);

	BDBG_LEAVE(BVBIlib_List_IncrementUsage_isr);
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_List_DecrementUsage_isr(
	BVBIlib_List_Handle    vbillHandle,
	BVBI_Field_Handle     fieldHandle
)
{
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVBIlib_List_DecrementUsage_isr);

	/* Decrement usage count */
	BVBI_Field_Decrement_UsageCount_isr (fieldHandle);

	/* Recycle if usage count is zero */
	if (BVBI_Field_Get_UsageCount_isr (fieldHandle) == 0)
	{
		eStatus = BVBIlib_List_Return_isr (vbillHandle, fieldHandle);
	}

	BDBG_LEAVE(BVBIlib_List_DecrementUsage_isr);
	return eStatus;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_List_DecrementUsage(
	BVBIlib_List_Handle    vbillHandle,
	BVBI_Field_Handle     fieldHandle
)
{
	BERR_Code eStatus;

	BDBG_ENTER(BVBIlib_List_DecrementUsage);

	BKNI_EnterCriticalSection();
	eStatus = BVBIlib_List_DecrementUsage_isr (vbillHandle, fieldHandle);
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBIlib_List_DecrementUsage);
	return eStatus;
}

/* End of File */
