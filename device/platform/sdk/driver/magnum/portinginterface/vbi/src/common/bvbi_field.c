/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_field.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 1/20/09 12:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7335/bvbi_field.c $
 * 
 * Hydra_Software_Devel/2   1/20/09 12:12p darnstein
 * PR51074: For the problem with 15 lines or more teletext (output), apply
 * the same software workaround that was used on 7401-C0. Note that this
 * workaround itself causes problems for some chipsets, such as 3563-C0.
 * So the workaround is controlled by an #ifdef.
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

static BERR_Code
BVBI_P_Field_ClearFlagData_isr (
	BVBI_Field_Handle fieldHandle,
	uint32_t flagBit);


/***************************************************************************
 *
 */
BERR_Code BVBI_Field_Create( BVBI_Field_Handle* pFieldHandle,
					 		 BVBI_Handle vbiHandle )
{
	BVBI_P_Handle *pVbi;
	BVBI_P_Field_Handle *pVbi_Fld = NULL;
	BVBI_P_TTData* pttData;
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_Create);

	/* check parameters */
	BVBI_P_GET_CONTEXT(vbiHandle, pVbi);
	if ((!pFieldHandle) || (!pVbi))
	{
		BDBG_ERR(("Invalid parameter\n"));
		eStatus = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BVBI_Field_Create_Done;
	}

	/* Alloc the VBI field handle. */
	pVbi_Fld =
		(BVBI_P_Field_Handle*)(BKNI_Malloc(sizeof(BVBI_P_Field_Handle)));
	if(!pVbi_Fld)
	{
		eStatus = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Field_Create_Done;
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVbi_Fld, 0x0, sizeof(BVBI_P_Field_Handle));
	pVbi_Fld->usCCData = BVBI_P_SetCCParityBits_isr(0x0);
	pVbi_Fld->ulErrInfo = 0x0;

	/* Remember VBI main handle for subsequent allocations */
	pVbi_Fld->pVbi = pVbi;

	/* Indicate no room for teletext, VPS, Gemstar, AMOL, or MCC  data */
	pttData = (BVBI_P_TTData*)(BKNI_Malloc(sizeof(BVBI_P_TTData)));
	if(!pttData)
	{
		eStatus = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Field_Create_Done;
	}
	BKNI_Memset((void*)pttData, 0x0, sizeof(BVBI_P_TTData));
	BVBI_P_TTData_Alloc (pVbi_Fld->pVbi->hMem, 0, 0, pttData);
	BVBI_P_LCOP_CREATE (pVbi_Fld, TTDataO, pttData, clink);
	pVbi_Fld->pVPSData = 0x0;
	pVbi_Fld->pGSData = 0x0;
	pVbi_Fld->amolType = BVBI_AMOL_Type_None;
	pVbi_Fld->pAmolData = 0x0;
	pVbi_Fld->amolSize = 0;
	pVbi_Fld->pMCCData = 0x0;
	pVbi_Fld->pPScteData = 0x0;
	pVbi_Fld->pCgmsbDatum = 0x0;

	/* Indicate no valid VBI data yet */
	pVbi_Fld->ulWhichPresent = 0x0;

	/* We don't know what the polarity is */
	pVbi_Fld->polarityMask = 0x0;

	/* Initialize protected attributes */
	pVbi_Fld->inUseCount = 0;

	/* Initialize magic number to the size of the struct */
	pVbi_Fld->ulBlackMagic = sizeof(BVBI_P_Field_Handle);

	/* All done. now return the new fresh context to user. */
	*pFieldHandle = (BVBI_Field_Handle)pVbi_Fld;

  BVBI_Field_Create_Done:

	BDBG_LEAVE(BVBI_Field_Create);

	if ((BERR_SUCCESS != eStatus) && (NULL != pVbi_Fld))
		BKNI_Free((void*)pVbi_Fld);

	/* coverity[leaked_storage: FALSE] */
	return eStatus;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_TT_Allocate(
				BVBI_Field_Handle fieldHandle,
				uint8_t ucMaxLines,
				uint8_t ucLineSize )
{
	BVBI_P_Field_Handle* pVbi_Fld;
	BVBI_P_TTData* pttData;
	BERR_Code eErr;

	BDBG_ENTER(BVBI_Field_TT_Allocate);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	if ((ucMaxLines != 0) && (ucLineSize == 0))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

#ifdef BVBI_P_TTE_WA15
	if (ucMaxLines !=0)
		ucMaxLines = BVBI_TT_MAX_LINES;
#endif

	/* (Re)allocate TT data as necessary */
	BVBI_P_LCOP_WRITE (pVbi_Fld, TTDataO, &pVbi_Fld->pVbi->ttFreelist, clink);
	pttData = BVBI_P_LCOP_GET (pVbi_Fld, TTDataO);
	eErr = BERR_TRACE (BVBI_P_TTData_Alloc (
		pVbi_Fld->pVbi->hMem, ucMaxLines, ucLineSize, pttData));
	if (eErr != BERR_SUCCESS)
	{
		BDBG_LEAVE(BVBI_Field_TT_Allocate);
		return eErr;
	}

	BDBG_LEAVE(BVBI_Field_TT_Allocate);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_VPS_Allocate(
				BVBI_Field_Handle fieldHandle,
				bool bEnable )
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_Field_VPS_Allocate);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Lose any VPS data that may exist already */
	if (pVbi_Fld->pVPSData)
	{
		BKNI_Free((void*)pVbi_Fld->pVPSData);
		pVbi_Fld->pVPSData = 0x0;
	}

	/* If user wants to hold any VPS data */
	if (bEnable)
	{
		pVbi_Fld->pVPSData =
			(BVBI_VPSData*)(BKNI_Malloc(sizeof(BVBI_VPSData)));
		if(!pVbi_Fld->pVPSData)
		{
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}
	}

	BDBG_LEAVE(BVBI_Field_VPS_Allocate);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_GS_Allocate(
				BVBI_Field_Handle fieldHandle,
				bool bEnable )
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_Field_GS_Allocate);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Lose any GS data that may exist already */
	if (pVbi_Fld->pGSData)
	{
		BKNI_Free((void*)pVbi_Fld->pGSData);
		pVbi_Fld->pGSData = 0x0;
	}

	/* If user wants to hold any GS data */
	if (bEnable)
	{
		pVbi_Fld->pGSData =
			(BVBI_GSData*)(BKNI_Malloc(sizeof(BVBI_GSData)));
		if(!pVbi_Fld->pGSData)
		{
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}
	}

	BDBG_LEAVE(BVBI_Field_GS_Allocate);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_AMOL_Allocate(
				BVBI_Field_Handle fieldHandle,
				BVBI_AMOL_Type amolType)
{
	BVBI_P_Field_Handle* pVbi_Fld;
	int dataSize;

	BDBG_ENTER(BVBI_Field_AMOL_Allocate);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Lose any AMOL data that may exist already */
	if (pVbi_Fld->pAmolData)
	{
		BKNI_Free((void*)pVbi_Fld->pAmolData);
		pVbi_Fld->pAmolData = 0x0;
		pVbi_Fld->amolSize = 0;
	}

	switch (amolType)
	{
	case BVBI_AMOL_Type_None:
		dataSize = 0;
		break;
	case BVBI_AMOL_Type_I:
		dataSize = 6;
		break;
	case BVBI_AMOL_Type_II_Lowrate:
		dataSize = 12;
		break;
	case BVBI_AMOL_Type_II_Highrate:
		dataSize = 24;
		break;
	default:
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	if (dataSize != 0)
	{
		pVbi_Fld->pAmolData =
			(uint8_t*)(BKNI_Malloc(dataSize));
		if(!pVbi_Fld->pAmolData)
		{
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}
		pVbi_Fld->amolSize = dataSize;
	}

	BDBG_LEAVE(BVBI_Field_AMOL_Allocate);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_MCC_Allocate(
				BVBI_Field_Handle fieldHandle,
				bool alloc)
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_Field_MCC_Allocate);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Lose any MCC data that may exist already */
	if (pVbi_Fld->pMCCData)
	{
		BKNI_Free((void*)pVbi_Fld->pMCCData);
		pVbi_Fld->pMCCData = 0x0;
	}

	if (alloc)
	{
		pVbi_Fld->pMCCData = (BVBI_MCCData*)(BKNI_Malloc(sizeof(BVBI_MCCData)));
		if(!pVbi_Fld->pMCCData)
		{
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}
	}
	else
	{
		if (pVbi_Fld->pMCCData)
		{
			BKNI_Free ((void*)(pVbi_Fld->pMCCData));
			pVbi_Fld->pMCCData = 0x0;
		}
	}

	BDBG_LEAVE(BVBI_Field_MCC_Allocate);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SCTE_Allocate(
	BVBI_Field_Handle fieldHandle,
	size_t cc_size, bool scteEnableNrtv, size_t pam_size, bool scteEnableMono)
{
	BVBI_P_Field_Handle* pVbi_Fld;
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_SCTE_Allocate);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* If any data is requested */
	if (cc_size || scteEnableNrtv || pam_size || scteEnableMono)
	{
		/* Allocate the internal struct if necessary */
		if (!(pVbi_Fld->pPScteData))
		{
			pVbi_Fld->pPScteData = BKNI_Malloc (sizeof (BVBI_P_SCTE_Data));
			if (!(pVbi_Fld->pPScteData))
			{
				return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
			}
			pVbi_Fld->pPScteData->field_number     = 0;
			pVbi_Fld->pPScteData->line_size        = 0;
			pVbi_Fld->pPScteData->nrtv_data[0]     = 0;
			pVbi_Fld->pPScteData->nrtv_data[1]     = 0;
			pVbi_Fld->pPScteData->pam_data_size    = 0;
			pVbi_Fld->pPScteData->mono_data[0]     = 0;
			pVbi_Fld->pPScteData->mono_data[1]     = 0;
		}
		/* Internal standard for SCTE data allocation */
		eStatus =
			BVBI_P_SCTEData_Alloc (
				pVbi_Fld->pVbi->hMem, cc_size, scteEnableNrtv, pam_size,
				scteEnableMono, pVbi_Fld->pPScteData);
		}
	else /* no data is requested */
	{
		/* If internal data has been allocated */
		if (pVbi_Fld->pPScteData)
		{
			/* Internal standard for SCTE data de-allocation */
			eStatus =
				BVBI_P_SCTEData_Alloc (
					pVbi_Fld->pVbi->hMem, 0, false, 0, false,
					pVbi_Fld->pPScteData);
			if (eStatus != BERR_SUCCESS)
			{
				return BERR_TRACE(eStatus);
			}
			/* Clear out the internal data structure itself */
			BKNI_Free ((void*)((pVbi_Fld->pPScteData)));
			pVbi_Fld->pPScteData = 0x0;
		}
	}

	BDBG_LEAVE(BVBI_Field_SCTE_Allocate);
	return eStatus;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_CGMSB_Allocate(
				BVBI_Field_Handle fieldHandle,
				bool bEnable)
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_Field_CGMSB_Allocate);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Lose any CGMSB data that may exist already */
	if (pVbi_Fld->pCgmsbDatum)
	{
		BKNI_Free((void*)pVbi_Fld->pCgmsbDatum);
		pVbi_Fld->pCgmsbDatum = 0x0;
	}

	if (bEnable)
	{
		pVbi_Fld->pCgmsbDatum = 
			(BVBI_CGMSB_Datum*)(BKNI_Malloc(sizeof(BVBI_CGMSB_Datum)));
		if(!pVbi_Fld->pCgmsbDatum)
		{
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}
	}
	else
	{
		if (pVbi_Fld->pCgmsbDatum)
		{
			BKNI_Free ((void*)(pVbi_Fld->pCgmsbDatum));
			pVbi_Fld->pCgmsbDatum = 0x0;
		}
	}

	BDBG_LEAVE(BVBI_Field_CGMSB_Allocate);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_Destroy(BVBI_Field_Handle fieldHandle)
{
	BVBI_P_Field_Handle* pVbi_Fld;
	BVBI_P_TTData* pttData;

	BDBG_ENTER(BVBI_Field_Destroy);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Deallocate bulky data if necessary */
	BVBI_P_LCOP_DESTROY (pVbi_Fld, TTDataO, &pVbi_Fld->pVbi->ttFreelist, clink);
	pttData = BVBI_P_LCOP_GET (pVbi_Fld, TTDataO);
	BVBI_P_TTData_Alloc (pVbi_Fld->pVbi->hMem, 0, 0, pttData);
	BKNI_Free ((void*)pttData);
	BVBI_Field_MCC_Allocate (fieldHandle, false);
	BVBI_Field_AMOL_Allocate(fieldHandle, BVBI_AMOL_Type_None);
	BVBI_Field_GS_Allocate (fieldHandle, false);
	BVBI_Field_VPS_Allocate (fieldHandle, false);
	BVBI_Field_CGMSB_Allocate (fieldHandle, false);
	BVBI_Field_SCTE_Allocate (fieldHandle, 0, false, 0, false);

	/* The handle is invalid */
	pVbi_Fld->ulBlackMagic = 0;

	/* Release context in system memory */
	BKNI_Free((void*)pVbi_Fld);

	BDBG_LEAVE(BVBI_Field_Destroy);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetPolarity_isr(
    BVBI_Field_Handle fieldHandle,
    uint32_t         polarityMask
)
{
	BVBI_P_Field_Handle *pVbi_Fld;

	BDBG_ENTER(BVBI_Field_SetPolarity_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if ((polarityMask != 0x0                         ) &&
		(polarityMask != (1<<BAVC_Polarity_eTopField)) &&
	    (polarityMask != (1<<BAVC_Polarity_eBotField)) &&
	    (polarityMask !=
			((1<<BAVC_Polarity_eTopField) | (1<<BAVC_Polarity_eBotField)))
	   )
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Set attribute as requested */
	pVbi_Fld->polarityMask = polarityMask;

	BDBG_LEAVE(BVBI_Field_SetPolarity_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_GetPolarity_isr(
    BVBI_Field_Handle fieldHandle,
    uint32_t *      pPolarityMask
)
{
	BVBI_P_Field_Handle *pVbi_Fld;

	BDBG_ENTER(BVBI_Field_GetPolarity_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pPolarityMask))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Get attribute as requested */
	*pPolarityMask = pVbi_Fld->polarityMask;

	BDBG_LEAVE(BVBI_Field_GetPolarity_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_GetErrorInfo_isr(
	BVBI_Field_Handle fieldHandle, uint32_t *pErrInfo )
{
	BVBI_P_Field_Handle *pVbi_Fld;

	BDBG_ENTER(BVBI_Field_GetErrorInfo_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pErrInfo))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Return info as requested */
	*pErrInfo = pVbi_Fld->ulErrInfo;

	BDBG_LEAVE(BVBI_Field_GetErrorInfo_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_ClearCCData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_CC));
}
BERR_Code BVBI_Field_ClearTTData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_TT));
}
BERR_Code BVBI_Field_ClearCGMSAData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_CGMSA));
}
BERR_Code BVBI_Field_ClearCGMSBData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_CGMSB));
}
BERR_Code BVBI_Field_ClearWSSData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_WSS));
}
BERR_Code BVBI_Field_ClearVPSData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_VPS));
}
BERR_Code BVBI_Field_ClearGSData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_GS));
}
BERR_Code BVBI_Field_ClearAMOLData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_AMOL));
}
BERR_Code BVBI_Field_ClearMCCData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_MCC));
}
BERR_Code BVBI_Field_ClearSCTEData_isr (BVBI_Field_Handle fieldHandle)
{
	return BERR_TRACE (BVBI_P_Field_ClearFlagData_isr (
		fieldHandle, BVBI_P_SELECT_SCTE));
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static BERR_Code
BVBI_P_Field_ClearFlagData_isr (
	BVBI_Field_Handle fieldHandle,
	uint32_t flagBit)
{
	BVBI_P_Field_Handle *pVbi_Fld;

	BDBG_ENTER(BVBI_P_Field_ClearFlagData_isr);

	/* check parameter */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Clear the data as requested */
	pVbi_Fld->ulWhichPresent &= ~flagBit;

	BDBG_LEAVE(BVBI_P_Field_ClearFlagData_isr);
	return BERR_SUCCESS;
}


#if 0
/***************************************************************************
 *
 */
BERR_Code BVBI_P_Field_Dump( BVBI_Field_Handle fieldHandle,
							 BAVC_Polarity polarity,
							 char* printme, int printmetoo )
{
	BVBI_P_Field_Handle* pVbi_Fld;
	char* polarityString;
	uint8_t* pucData;
	uint32_t mask;
	char* p1;
	char* p2;

	BDBG_ENTER(BVBI_P_Field_Dump);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	switch (polarity)
	{
	case BAVC_Polarity_eTopField:
		polarityString = "Top";
		break;
	case BAVC_Polarity_eBotField:
		polarityString = "Bottom";
		break;
	case BAVC_Polarity_eFrame:
		polarityString = "Frame";
		break;
	default:
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	pucData = pVbi_Fld->TTDataO.pucData;
	mask = *(uint32_t*)pucData;
	p1 = pucData + 4;
	p2 = pucData + (4 + 34);
	if (mask)
	{
		printf ("%s %d\n", printme, printmetoo);
		printf ("Polarity %s, fpm %x\n",
			polarityString, pVbi_Fld->polarityMask);
		printf ("Data %08x  \"%s\"  \"%s\"\n", mask, p1, p2);
	}

	BDBG_LEAVE(BVBI_P_Field_Dump);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
void BVBI_Field_Check(BVBI_Field_Handle fieldHandle)
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_P_Field_Check);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	BDBG_ASSERT (pVbi_Fld);
	BDBG_ASSERT (pVbi_Fld->TTDataO.ucDataSize);
	BDBG_ASSERT (pVbi_Fld->TTDataO.pucData);

	BDBG_LEAVE(BVBI_P_Field_Check);
}
#endif /* 0 */
