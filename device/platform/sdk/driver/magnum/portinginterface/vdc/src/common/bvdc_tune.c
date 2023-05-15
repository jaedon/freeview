/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_tune.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/24/09 5:35p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/3548/bvdc_tune.c $
 * 
 * Hydra_Software_Devel/1   11/24/09 5:35p yuxiaz
 * SW7420-462: Added new tuning functions for Tuning Database.
 * 
 ***************************************************************************/
#include "bvdc_tune_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_tnt.h"

BDBG_MODULE(BVDC_TUNE);

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetTuneParam
	( BVDC_Window_Handle               hWindow,
	  const uint32_t                   aFields[], 
	  const uint32_t                   aValues[], 
	  int                              num )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(aFields);
	BSTD_UNUSED(aValues);
	BSTD_UNUSED(num);

	BDBG_WRN(("BVDC_Window_SetTuneParam is not supported on this chip"));
	return BERR_TRACE(BVDC_ERR_TUNE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetTuneParam
	( const BVDC_Window_Handle         hWindow,
	  const uint32_t                   aFields[],
	  uint32_t                         aValues[],
	  int                              num )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(aFields);
	BSTD_UNUSED(aValues);
	BSTD_UNUSED(num);

	BDBG_WRN(("BVDC_Window_GetTuneParam is not supported on this chip"));
	return BERR_TRACE(BVDC_ERR_TUNE_NOT_SUPPORTED);
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_ResetTuneParam
	( BVDC_Window_Handle         hWindow )
{
	BSTD_UNUSED(hWindow);

	BDBG_WRN(("BVDC_Window_ResetTuneParam is not supported on this chip"));
	return BERR_TRACE(BVDC_ERR_TUNE_NOT_SUPPORTED);
}


/* End of File */


