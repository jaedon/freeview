/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 2/10/12 5:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/bchp.c $
 * 
 * Hydra_Software_Devel/9   2/10/12 5:25p rjlewis
 * SW7425-2218: removed AvsLock/Unlock and added get handle.
 * 
 * Hydra_Software_Devel/8   2/1/12 7:03p rjlewis
 * SW7425-2216: Adding support for AVS Lock/Unlock.
 * 
 * Hydra_Software_Devel/7   8/16/11 11:32a rjlewis
 * SW7346-117: Added support for new chip specific Enter/Exit Stand-by
 * mode.
 * 
 * Hydra_Software_Devel/6   8/3/11 11:46a rjlewis
 * SW7346-117: added support for GetAvsData function.
 * 
 * Hydra_Software_Devel/5   1/20/11 1:40p gmohile
 * SW7408-212 : Add support for avs settings
 * 
 * Hydra_Software_Devel/4   6/10/04 5:43p jasonh
 * PR 11257: Added initial support of BCHP_GetFeature for 7038 chipsets.
 * 
 * Hydra_Software_Devel/3   11/25/03 10:27a hongtaoz
 * Removed compile warning.
 * 
 * Hydra_Software_Devel/2   9/26/03 8:49a dlwin
 * Updated to return 16 bits for Chip ID and Chip Rev.
 * 
 * Hydra_Software_Devel/1   9/23/03 9:47p dlwin
 * Initial version
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "bchp.h"
#include "bchp_priv.h"


BDBG_MODULE(BCHP);


BERR_Code BCHP_Close(
	BCHP_Handle hChip				/* [in] Chip handle */
	)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER( BCHP_Close );
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( hChip->pCloseFunc != NULL );
	rc = hChip->pCloseFunc( hChip );
	BDBG_LEAVE( BCHP_Close );
	return( rc );
}

BERR_Code BCHP_GetChipInfo(
	const BCHP_Handle hChip,			/* [in] Chip handle */
	uint16_t *pChipId,					/* [out] Chip Id */
	uint16_t *pChipRev					/* [out] Chip Rev. */
	)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER( BCHP_GetChipInfo );
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( hChip->pGetChipInfoFunc != NULL );
	rc = hChip->pGetChipInfoFunc( hChip, pChipId, pChipRev );
	BDBG_LEAVE( BCHP_GetChipInfo );
	return( rc );
}

BERR_Code BCHP_GetFeature(
	const BCHP_Handle hChip,			/* [in] Chip handle */
	const BCHP_Feature eFeature,		/* [in] Feature to be queried. */
	void *pFeatureValue					/* [out] Feature value. */
	)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER( BCHP_GetFeature );
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( pFeatureValue );

	if (hChip->pGetFeatureFunc)
		rc = hChip->pGetFeatureFunc( hChip, eFeature, pFeatureValue );
	else
		rc = BERR_TRACE(BERR_UNKNOWN);

	BDBG_LEAVE( BCHP_GetFeature );
	return( rc );
}

void BCHP_MonitorPvt( 
	BCHP_Handle hChip,              /* [in] Chip handle */
	BCHP_AvsSettings *pSettings     /* [in] AVS settings. */
	)
{
	BDBG_ENTER( BCHP_MonitorPvt );
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( pSettings );

	if (hChip->pMonitorPvtFunc) 
		hChip->pMonitorPvtFunc(hChip, pSettings);

	BDBG_LEAVE( BCHP_MonitorPvt );
}

BERR_Code BCHP_GetAvsData(
	BCHP_Handle hChip,   /* [in] Chip handle */
	BCHP_AvsData *pData) /* [out] pointer to location to return data */
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER( BCHP_GetAvsData );
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( pData );

	if (hChip->pGetAvsDataFunc) 
		rc = hChip->pGetAvsDataFunc(hChip, pData);
	else
		rc = BERR_TRACE(BERR_UNKNOWN);

	BDBG_LEAVE( BCHP_GetAvsData );
	return( rc );
}

BERR_Code BCHP_Standby(
    BCHP_Handle hChip)   /* [in] Chip handle */
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER( BCHP_Standby );
	BDBG_ASSERT( hChip );

	if (hChip->pStandbyModeFunc) 
		rc = hChip->pStandbyModeFunc(hChip, true);

	BDBG_LEAVE( BCHP_Standby );
	return( rc );
}

BERR_Code BCHP_Resume(
    BCHP_Handle hChip)  /* [in] Chip handle */
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER( BCHP_Resume );
	BDBG_ASSERT( hChip );

	if (hChip->pStandbyModeFunc) 
		rc = hChip->pStandbyModeFunc(hChip, false);

	BDBG_LEAVE( BCHP_Resume );
	return( rc );
}

/* For testing purposes only */
void *BCHP_GetAvsHandle(
    BCHP_Handle hChip)   /* [in] Chip handle */
{
	void *result;

	BDBG_ENTER( BCHP_GetAvsHandle );
	BDBG_ASSERT( hChip );

	result = (void*)hChip->avsHandle; /* will be NULL for platforms without AVS support */

	BDBG_LEAVE( BCHP_GetAvsHandle );
	return( result );
}

/* end of file */
