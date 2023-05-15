/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_amol.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 2/10/11 5:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_amol.c $
 * 
 * Hydra_Software_Devel/4   2/10/11 5:35p darnstein
 * SW7422-255: The two set/get functions for AMOL data now have a length
 * argument.
 * 
 * Hydra_Software_Devel/3   11/24/09 4:35p darnstein
 * SW35230-16: first cut at 35230 support.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:56p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/16   6/6/08 5:36p darnstein
 * PR38956: compile in support for SCTE and AMOL in 93548.
 * 
 * Hydra_Software_Devel/15   3/19/08 7:38p darnstein
 * PR40770: change Pedestal gain value for AMOL to 0x4d. Per Lin Ma.
 * 
 * Hydra_Software_Devel/14   2/15/08 7:05p darnstein
 * PR20429: The AMOL waveforms were going out on the wrong video lines.
 * 
 * Hydra_Software_Devel/13   12/4/07 1:19p darnstein
 * PR36897: VBI software is ready for testing with 97335 chipset.
 * 
 * Hydra_Software_Devel/12   10/31/07 3:51p darnstein
 * PR34528: BVBI is ready for testing on 7325.
 * 
 * Hydra_Software_Devel/11   9/17/07 2:47p darnstein
 * PR29723: The fact that only primary VEC has an AMOLE core was not
 * handled correctly.
 * 
 * Hydra_Software_Devel/10   4/20/07 3:36p darnstein
 * PR29723: Compilation for 7405 chipset.
 * 
 * Hydra_Software_Devel/9   12/15/06 2:38p darnstein
 * PR25990: Improve method for switching among VEC paths.
 * 
 * Hydra_Software_Devel/8   11/1/06 5:17p darnstein
 * PR24979: Kin Fan Ho scoped the waveform and picked the correct
 * horizontal offset.
 * 
 * Hydra_Software_Devel/7   7/20/06 6:21p darnstein
 * PR21688: Fix typo in previous check-in.
 * 
 * Hydra_Software_Devel/6   7/20/06 2:33p darnstein
 * PR21688: Use the new hardware soft reset scheme for later model chips.
 * 
 * Hydra_Software_Devel/5   5/26/06 4:58p darnstein
 * PR20612: adapt to new register names in the AMOL core.
 * 
 * Hydra_Software_Devel/4   4/25/06 4:42p darnstein
 * PR20949: Adapt to fixed up names in RDB for NULL_ENABLE register
 * fields.
 * 
 * Hydra_Software_Devel/3   4/18/06 1:49p darnstein
 * PR18010: Silence some compiler warnings encountered by David Lwin.
 * 
 * Hydra_Software_Devel/2   4/12/06 6:17p darnstein
 * PR18010: Don't try to program encoders for don't exist for 656 output.
 * 
 * Hydra_Software_Devel/1   4/11/06 7:25p darnstein
 * PR20429: First cut at AMOL encoder software.
 * 
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bkni.h"			/* For critical sections */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */

BDBG_MODULE(BVBI);

/***************************************************************************
* Private data
***************************************************************************/

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

static int P_size_by_type (BVBI_AMOL_Type type);


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/


/***************************************************************************
 *
 */
BERR_Code BVBI_Field_GetAMOLData_isr (
	BVBI_Field_Handle fieldHandle,	
	BVBI_AMOL_Type      *pAmolType,  
	uint8_t            *pAMOLData,
	unsigned int*         pLength
)
{
	int count;
	int size;
	uint8_t* fdata;
	BVBI_P_Field_Handle* pVbi_Fld;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_GetAMOLData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pAMOLData) || (!pAmolType) || (!pLength))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Verify that data is present on this field handle */
	if (!(pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_AMOL))
		return (BVBI_ERR_FIELD_NODATA);
	else if (pVbi_Fld->ulErrInfo & BVBI_LINE_ERROR_AMOL_NOENCODE)
		eErr = (BVBI_ERR_FIELD_BADDATA);

	/* Check that field handle was properly sized */
	fdata = pVbi_Fld->pAmolData;
	if (!fdata)
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Return data as requested */
	BDBG_ASSERT (pVbi_Fld->amolType != BVBI_AMOL_Type_None);
	size = P_size_by_type (pVbi_Fld->amolType);
	BDBG_ASSERT (size >= 0);
	*pAmolType = pVbi_Fld->amolType;
	for (count = 0 ; count < size ; ++count)
		*pAMOLData++ = *fdata++;
	*pLength = P_size_by_type (*pAmolType);

	BDBG_LEAVE(BVBI_Field_GetAMOLData_isr);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetAMOLData_isr(
	BVBI_Field_Handle fieldHandle,	
	BVBI_AMOL_Type        amolType,
	uint8_t             *pAMOLData,
	unsigned int            length
)
{
	int count;
	int size;
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_Field_SetAMOLData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pAMOLData))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	/* TODO: Check amolType more carefully? */
	if (amolType == BVBI_AMOL_Type_None)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	size = P_size_by_type (amolType);
	if (size < 0)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	if ((unsigned int)size > length)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Check that field handle was properly sized */
	if (!pVbi_Fld->pAmolData)
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}
	if (pVbi_Fld->amolSize < size)
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Store data as requested */
	pVbi_Fld->amolType = amolType;
	for (count = 0 ; count < size ; ++count)
		pVbi_Fld->pAmolData[count] = *pAMOLData++;

	/* Indicate valid data is present */
	pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_AMOL;

	BDBG_LEAVE(BVBI_Field_SetAMOLData_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Implementation of supporting AMOL functions that are not in API
***************************************************************************/

BERR_Code BVBI_P_AMOL_Init( BVBI_P_Handle *pVbi )
{
#if (BVBI_P_NUM_AMOLE > 0) || (BVBI_P_NUM_AMOLE_656 > 0)
	uint8_t hwIndex;
#endif

	BDBG_ENTER(BVBI_P_AMOL_Init);

#if (BVBI_P_NUM_AMOLE == 0) && (BVBI_P_NUM_AMOLE_656 == 0)
	BSTD_UNUSED (pVbi);
#endif

#if (BVBI_P_NUM_AMOLE > 0)
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_AMOLE ; ++hwIndex)
		BVBI_P_AMOL_Enc_Init (pVbi->hReg, false, hwIndex);
#endif
#if (BVBI_P_NUM_AMOLE_656 > 0)
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_AMOLE_656 ; ++hwIndex)
		BVBI_P_AMOL_Enc_Init (pVbi->hReg, true, hwIndex);
#endif

	BDBG_LEAVE(BVBI_P_AMOL_Init);
	return BERR_SUCCESS;
}


/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static int P_size_by_type (BVBI_AMOL_Type type)
{
	int size;

	switch (type)
	{
	case BVBI_AMOL_Type_None:
		size = 0;
		break;
	case BVBI_AMOL_Type_I:
		size = 6;
		break;
	case BVBI_AMOL_Type_II_Lowrate:
		size = 12;
		break;
	case BVBI_AMOL_Type_II_Highrate:
		size = 24;
		break;
	default:
		size = -1;
		break;
	}

	return size;
}

/* End of file */
