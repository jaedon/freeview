/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_mcc.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 12/4/08 6:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_mcc.c $
 * 
 * Hydra_Software_Devel/3   12/4/08 6:12p darnstein
 * PR45819: remove some obsolete "alias central" logic.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:57p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/3   11/7/07 8:22p darnstein
 * PR35983: Fix a Coverity error with some #ifdefs.
 * 
 * Hydra_Software_Devel/1   6/5/07 11:25p pntruong
 * PR30411: Add support for multiple CC lines.
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


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_GetMCCData_isr (
	BVBI_Field_Handle fieldHandle, 
	BVBI_MCCData* pMCCData
)
{
	BVBI_P_Field_Handle* pVbi_Fld;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_GetMCCData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pMCCData))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Verify that data is present on this field handle */
	if (!(pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_MCC))
		return (BVBI_ERR_FIELD_NODATA);
	else if (pVbi_Fld->ulErrInfo & BVBI_LINE_ERROR_MCC_NOENCODE)
		eErr = (BVBI_ERR_FIELD_BADDATA);

	/* Check that field handle was properly sized */
	if (!(pVbi_Fld->pMCCData))
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Return data as requested */
	*pMCCData = *(pVbi_Fld->pMCCData);

	BDBG_LEAVE(BVBI_Field_GetMCCData_isr);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetMCCData_isr( 
	BVBI_Field_Handle fieldHandle, 
	BVBI_MCCData*     pMCCData
)
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_SetMCCData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pMCCData))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Check that field handle was properly sized */
	if (!(pVbi_Fld->pMCCData))
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Store data as requested */
    *(pVbi_Fld->pMCCData) = *pMCCData;

	/* Indicate valid data is present */
	pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_MCC;

	BDBG_LEAVE(BVBI_SetMCCData_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Implementation supporting closed caption functions that are not in API
***************************************************************************/


/***************************************************************************
* Static (private) functions
***************************************************************************/

/* End of file */
