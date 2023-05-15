/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_gs.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 11/24/09 4:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_gs.c $
 * 
 * Hydra_Software_Devel/3   11/24/09 4:35p darnstein
 * SW35230-16: first cut at 35230 support.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:56p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/28   10/1/08 2:50p darnstein
 * PR47070: The data structure for Gemstar data changes to include line
 * number information and line-by-line parity error information. This is
 * an API change, so applications software will need to adapt.
 * 
 * Hydra_Software_Devel/27   9/29/08 3:06p darnstein
 * PR40702: When reading Gemstar data bitmask, AND in the control bitmask.
 * This knocks out the extraneous bits that the hardware seems to report.
 * 
 * Hydra_Software_Devel/26   6/18/08 12:07p darnstein
 * PR38956: reverse BYTE_ORDER. Decoding now works.
 * 
 * Hydra_Software_Devel/25   5/28/08 5:35p darnstein
 * PR23220: Some implementation of Gemstar decoding was missing.
 * 
 * Hydra_Software_Devel/24   3/21/08 7:02p darnstein
 * PR40830: change VEC GAIN register value to 0x48, per Lin Ma.
 * 
 * Hydra_Software_Devel/23   3/14/08 3:18p darnstein
 * PR23220: Gemstar decoding almost works. Cannot process video line 11
 * for some reason.
 * 
 * Hydra_Software_Devel/22   10/10/07 6:08p darnstein
 * PR35440: Some RDB adaptations were not correct.
 * 
 * Hydra_Software_Devel/21   3/6/07 5:44p darnstein
 * PR28456: Support third VEC.
 * 
 * Hydra_Software_Devel/20   12/15/06 3:06p darnstein
 * PR25990: Fix another minor error.
 * 
 * Hydra_Software_Devel/19   12/15/06 2:54p darnstein
 * PR25990: Fix a stupid typo in previous checkin.
 * 
 * Hydra_Software_Devel/18   12/15/06 2:39p darnstein
 * PR25990: Improve method for switching among VEC paths.
 * 
 * Hydra_Software_Devel/17   10/19/06 5:49p darnstein
 * PR24979: change horizontal offset of waveform when sending NTSC.
 * Theory: VEC microcode changed in such a way that the register values
 * are interpreted differently.
 * 
 * Hydra_Software_Devel/16   10/2/06 5:05p darnstein
 * PR21688: Silence two compiler warnings.
 * 
 * Hydra_Software_Devel/15   8/31/06 2:31p darnstein
 * PR23220: manually merge in some preliminary code from the 3560 branch.
 * 
 * Hydra_Software_Devel/14   7/20/06 2:33p darnstein
 * PR21688: Use the new hardware soft reset scheme for later model chips.
 * 
 * Hydra_Software_Devel/13   4/25/06 1:22p darnstein
 * PR18010: silence compiler warnings for some chipsets. Reported by
 * DLwin.
 * 
 * Hydra_Software_Devel/12   4/12/06 6:16p darnstein
 * PR18010: Don't try to program encoders for don't exist for 656 output.
 * 
 * Hydra_Software_Devel/11   4/3/06 5:45p darnstein
 * PR20611: Useless code removed.
 * 
 * Hydra_Software_Devel/10   2/16/06 3:29p darnstein
 * PR19716: Adapt to new names for "null" control registers.
 * 
 * Hydra_Software_Devel/9   2/7/06 3:35p darnstein
 * PR17616: use register settings tuned by Kin Fan Ho. Switch to automatic
 * parity bit generation.
 * 
 * Hydra_Software_Devel/8   1/5/06 6:04p darnstein
 * PR18688: Fixed bad logic errors in BVBI_P_GS_Encode_Data_isr().
 * 
 * Hydra_Software_Devel/7   11/15/05 1:54p darnstein
 * PR17732: Comment out or remove debugging code. Fix compile error on
 * chips that do not have a Gemstar encoder.
 * 
 * Hydra_Software_Devel/6   11/14/05 7:20p darnstein
 * PR17732: Bug fixes mean this version basically works. Still left to do:
 * Get rid of debugging code. Get rid of incorrect calls to BERR_TRACE().
 * Tune analog waveform parameters.
 * 
 * Hydra_Software_Devel/5   10/28/05 3:02p darnstein
 * PR17732: Switch to manual parity in hardware. Still doesn't work.
 * 
 * Hydra_Software_Devel/4   10/21/05 8:17p darnstein
 * PR17732: Fix a counting bug. Affected programming user's data values
 * into hardware.
 *
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bkni.h"			/* For critical sections */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */

BDBG_MODULE(BVBI);


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/


/***************************************************************************
 *
 */
BERR_Code BVBI_Field_GetGSData_isr( BVBI_Field_Handle fieldHandle, 
	BVBI_GSData* pGSData)
{
	BVBI_P_Field_Handle* pVbi_Fld;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_GetGSData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pGSData))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Verify that data is present on this field handle */
	if (!(pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_GS))
		return (BVBI_ERR_FIELD_NODATA);
	else if (pVbi_Fld->ulErrInfo & BVBI_LINE_ERROR_GEMSTAR_NOENCODE)
		eErr = (BVBI_ERR_FIELD_BADDATA);

	/* Check that field handle was properly sized */
	if (!pVbi_Fld->pGSData)
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Return data as requested */
	*pGSData = *(pVbi_Fld->pGSData);

	BDBG_LEAVE(BVBI_Field_GetGSData_isr);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetGSData_isr( BVBI_Field_Handle fieldHandle, 
	BVBI_GSData* pGSData)
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_Field_SetGSData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pGSData))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Check that field handle was properly sized */
	if (!pVbi_Fld->pGSData)
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Store data as requested */
	*(pVbi_Fld->pGSData) = *pGSData;

	/* Indicate valid data is present */
	pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_GS;

	BDBG_LEAVE(BVBI_Field_SetGSData_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Implementation of supporting GS functions that are not in API
***************************************************************************/

BERR_Code BVBI_P_GS_Init( BVBI_P_Handle *pVbi )
{
#if (BVBI_P_NUM_GSE > 0) || (BVBI_P_NUM_GSE_656 > 0)
	uint8_t hwIndex;
#endif

	BDBG_ENTER(BVBI_P_GS_Init);

#if (BVBI_P_NUM_GSE == 0) && (BVBI_P_NUM_GSE_656 == 0)
	BSTD_UNUSED (pVbi);
#endif

	/* Initialize CC encoders */
#if (BVBI_P_NUM_GSE > 0)
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_GSE ; ++hwIndex)
		BVBI_P_GS_Enc_Init (pVbi->hReg, false, hwIndex);
#endif
#if (BVBI_P_NUM_GSE_656 > 0)
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_GSE_656 ; ++hwIndex)
		BVBI_P_GS_Enc_Init (pVbi->hReg, true, hwIndex);
#endif

	/* Initialize GS decoders */
#ifdef BVBI_P_HAS_GSD /** { **/
	BVBI_P_GS_Dec_Init(pVbi->hReg, 0x0);
#endif /** } **/

	BDBG_LEAVE(BVBI_P_GS_Init);
	return BERR_SUCCESS;
}

/* End of file */
