/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_vps.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/3/08 7:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_vps.c $
 * 
 * Hydra_Software_Devel/2   12/3/08 7:58p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/29   10/1/08 2:53p darnstein
 * PR46356: Check error condition: pointer argument is NULL.
 * 
 * Hydra_Software_Devel/28   9/15/08 8:58p darnstein
 * PR46356: VPS decoder software in BVBI is ready for test.
 * 
 * Hydra_Software_Devel/27   5/13/08 12:02p darnstein
 * PR42416: adjust decoder video line number when the frame comb work-
 * around is in place.
 * 
 * Hydra_Software_Devel/26   2/20/07 10:44a darnstein
 * PR27521: Apply software workaround. The TTE core is reset, at every
 * video field. Also, the encoder output for the field NOT in use is
 * disabled, at every video field.
 * 
 * Hydra_Software_Devel/25   1/2/07 4:19p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 * 
 * Hydra_Software_Devel/24   12/14/06 7:19p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 * 
 * Hydra_Software_Devel/23   8/31/06 2:09p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 * 
 * Hydra_Software_Devel/22   5/3/06 2:31p darnstein
 * PR18010: Remove extraneous call to BVBI_P_CC_ConfigForOthers() that
 * might change the video format of closed caption encoder core.
 * 
 * Hydra_Software_Devel/21   4/12/06 7:25p darnstein
 * PR18010: Don't try to program encoders for don't exist for 656 output.
 * 
 * Hydra_Software_Devel/20   4/10/06 6:06p darnstein
 * PR20611: Fix Clearcase header (in comments).
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
BERR_Code BVBI_Field_GetVPSData_isr( BVBI_Field_Handle fieldHandle, 
								     BVBI_VPSData *pVPSData )
{
	BVBI_P_Field_Handle* pVbi_Fld;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_GetVPSData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pVPSData))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Verify that data is present on this field handle */
	if (!(pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_VPS))
		return (BVBI_ERR_FIELD_NODATA);
	if (pVbi_Fld->ulErrInfo & BVBI_LINE_ERROR_VPS_NOENCODE)
		eErr = (BVBI_ERR_FIELD_BADDATA);

	/* Check that field handle was properly sized */
	if (!pVbi_Fld->pVPSData)
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Return data as requested */
    *pVPSData = *pVbi_Fld->pVPSData;

	BDBG_LEAVE(BVBI_Field_GetVPSData_isr);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetVPSData_isr( BVBI_Field_Handle fieldHandle, 
								     BVBI_VPSData *pVPSData )
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_Field_SetVPSData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pVPSData))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Check that field handle was properly sized */
	if (!pVbi_Fld->pVPSData)
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Store data as requested */
    *pVbi_Fld->pVPSData = *pVPSData;

	/* Indicate valid data is present */
	pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_VPS;

	BDBG_LEAVE(BVBI_Field_SetVPSData_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Implementation of supporting VPS functions that are not in API
***************************************************************************/


BERR_Code BVBI_P_VPS_Init( BVBI_P_Handle *pVbi )
{
	uint8_t hwIndex;
#if defined(BVBI_P_HAS_VPSD)
	uint32_t ulCoreOffset;
#endif
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_P_VPS_Init);

	/* Initialize VPS encoders */
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_WSE ; ++hwIndex)
		BVBI_P_VPS_Enc_Init (pVbi->hReg, hwIndex);

#if defined(BVBI_P_HAS_VPSD)
	if ((eErr = BVBI_P_VDTOP_Dec_Reset (
		pVbi->hReg, BAVC_SourceId_eVdec0, BVBI_P_SELECT_VPS)) != BERR_SUCCESS)
	{
		BDBG_LEAVE(BVBI_P_VPS_Init);
		return eErr;
	}
	ulCoreOffset = 0x0;
	BVBI_P_VPS_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif

	BDBG_LEAVE(BVBI_P_VPS_Init);
	return eErr;
}


/***************************************************************************
* Static (private) functions
***************************************************************************/

/* End of file */
