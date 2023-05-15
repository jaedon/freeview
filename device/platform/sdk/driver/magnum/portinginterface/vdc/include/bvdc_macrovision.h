/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_macrovision.h $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 11/7/08 6:39p $
 *
 * Module Description:
 *   Header file for Macrovision support
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_macrovision.h $
 * 
 * Hydra_Software_Devel/15   11/7/08 6:39p darnstein
 * PR46484: move new diagnostic commands into bvdc_dbg.h, where they
 * belong.
 * 
 * Hydra_Software_Devel/14   11/7/08 5:34p darnstein
 * PR46484: Add functions to print out contents of some important register
 * value tables. Must be activated with #define BVDC_P_DISPLAY_DUMP.
 * 
 * Hydra_Software_Devel/13   11/3/04 5:00p hongtaoz
 * PR13164: CPC/CPS tables are run-time variable type; also removed
 * private function from the public header file;
 * 
 * Hydra_Software_Devel/12   9/2/04 11:24a hongtaoz
 * PR12552: adding 576p_50Hz support; and removed global MV tables from
 * public header file;
 * 
 * Hydra_Software_Devel/11   7/7/04 2:36p hongtaoz
 * PR7726: added support for customer specified Macrovision table to
 * standard modes mapping;
 * 
 * Hydra_Software_Devel/10   7/2/04 11:08a hongtaoz
 * PR11596: added enums for Macrovision certification test; added RGB
 * output Macrovision on/off support;
 * PR11561: added error checking for Macrovision api;
 * 
 * Hydra_Software_Devel/9   6/25/04 2:33p hongtaoz
 * PR7661: add macrovision support for PAL; fixed macrovision sync-level
 * reduction problem; incorporated 480p macrovision microcode;
 * 
 * Hydra_Software_Devel/8   5/24/04 5:10p jasonh
 * PR 11189: Merge down from B0 to main-line
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   5/11/04 11:11a maivu
 * PR 11032: Added const to static tables
 * 
 * Hydra_Software_Devel/7   4/9/04 2:35p maivu
 * PR 10136: Update pattern ram using latest scripts
 * 
 * Hydra_Software_Devel/6   12/10/03 10:07a maivu
 * PR 7661: add support for 480p macrovision
 * 
 * Hydra_Software_Devel/5   11/20/03 2:57p maivu
 * Added RamTbl for 480i support
 * 
 * Hydra_Software_Devel/4   11/14/03 11:56a maivu
 * Added RamBVBInput_NTSC_MacrovisionTbl
 * 
 * Hydra_Software_Devel/3   10/10/03 9:20a maivu
 * Moved BVDC_MacrovisionType enum to bvdc.h, so private functions can
 * reference it
 * 
 * Hydra_Software_Devel/2   9/22/03 2:22p maivu
 * Added BVDC_Display_SetMacrovisionTable
 * 
 * Hydra_Software_Devel/1   8/29/03 1:31p maivu
 * Initial revision
 *
 ***************************************************************************/
#ifndef BVDC_MACROVISION_H__
#define BVDC_MACROVISION_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BVDC_CPC_COUNT     2
#define BVDC_CPS_COUNT     33
typedef uint8_t      BVDC_CpcTable[BVDC_CPC_COUNT];
typedef uint8_t      BVDC_CpsTable[BVDC_CPS_COUNT];


/***************************************************************************
Summary:
	This function sets the Macrovision type

Description:
	Sets the macrovision type associated with a Display	handle. 
	Returns an error if the macrovision type is invalid, or the display
	output does not support macrovision. 656, and DVI do not support
	Macrovision.

	Does not take immediate effect. Requires an ApplyChanges() call.

Input:
	hDisplay - Display handle created earlier with BVDC_Display_Create.
	eMacrovisionType - macrovision type

Output:

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Function succeed

See Also:
	BVDC_Display_GetMacrovisionType
	Note that HDCP, content protection for DVI is supported in the DVI PI.
**************************************************************************/
BERR_Code BVDC_Display_SetMacrovisionType
	( BVDC_Display_Handle              hDisplay,
	  BVDC_MacrovisionType             eMacrovisionType );

/***************************************************************************
Summary:
	This function queries the Macrovision type applied

Description:
	Returns the macrovision type associated with a Display
	handle.

Input:
	hDisplay - Display handle created earlier with BVDC_Display_Create.

Output:
	peMacrovisionType - pointer to macrovision type

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Function succeed

See Also:
	BVDC_Display_SetMacrovisionType
**************************************************************************/
BERR_Code BVDC_Display_GetMacrovisionType
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_MacrovisionType            *peMacrovisionType );

/***************************************************************************
Summary:
	Provide custom Macrovision CPC/CPS values to use, instead of the
	pre-defined Macrovision types.

Description:
	This function programs the Macrovision settings with the CPC/CPS
	provided by the user. Applications are required to call
	BVDC_Display_SetMacrovisionType with BVDC_MacrovisionType_eCustomized.

Input:
	hDisplay   - Display handle
	pCpcTable  - pointer to CPC table (CPC0,CPC1)
	pCpsTable  - pointer to CPS table (CPS0..CPS32)

Output:

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Function succeed

See Also:
	BVDC_Display_SetMacrovisionType
**************************************************************************/
BERR_Code BVDC_Display_SetMacrovisionTable
	( BVDC_Display_Handle            hDisplay,
	  const BVDC_CpcTable            pCpcTable,
	  const BVDC_CpsTable            pCpsTable );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_MACROVISION_H__ */
/* End of file. */
