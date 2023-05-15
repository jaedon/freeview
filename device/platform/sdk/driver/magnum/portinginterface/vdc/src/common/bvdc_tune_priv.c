/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_tune_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/24/09 5:36p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/3548/bvdc_tune_priv.c $
 * 
 * Hydra_Software_Devel/1   11/24/09 5:36p yuxiaz
 * SW7420-462: Added new tuning functions for Tuning Database.
 * 
 ***************************************************************************/
#include "bvdc_tune_priv.h"
#include "bvdc_window_priv.h"

BDBG_MODULE(BVDC_TUNE);


/***************************************************************************
 * Index needs to be consistent with bvdc_tune.h
 *
 */
BERR_Code BVDC_P_Tune_Sharpness_Settings_GetData
	( const BVDC_SharpnessSettings          *pSharpnessSettings,
	  uint32_t                               ulField, 
	  uint32_t                              *pData )
{
	BSTD_UNUSED(pSharpnessSettings);
	BSTD_UNUSED(ulField);
	BSTD_UNUSED(pData);
	return BERR_SUCCESS;
}


/***************************************************************************
 * Index needs to be consistent with bvdc_tune.h
 *
 */
BERR_Code BVDC_P_Tune_Sharpness_Settings_SetData
	( BVDC_SharpnessSettings                *pSharpnessSettings,
	  uint32_t                               ulField, 
	  uint32_t                               ulData )
{
	BSTD_UNUSED(pSharpnessSettings);
	BSTD_UNUSED(ulField);
	BSTD_UNUSED(ulData);
	return BERR_SUCCESS;
}

/* End of File */


