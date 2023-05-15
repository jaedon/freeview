/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_cm3d.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 6/18/10 8:51p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_cm3d.c $
 * 
 * Hydra_Software_Devel/13   6/18/10 8:51p rpan
 * SW7400-2763: Fixed compile errors.
 * 
 * Hydra_Software_Devel/12   4/19/10 10:12p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/11   4/7/10 11:27a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/10   4/5/10 3:59p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/9   5/13/09 12:31a pntruong
 * PR51108: Coverity Defect ID:13602 OVERRUN_STATIC.
 *
 * Hydra_Software_Devel/8   12/3/08 5:04p jessem
 * PR 48992: Added Chroma Histogram API support.
 *
 * Hydra_Software_Devel/7   11/20/08 12:03p jessem
 * PR 49411: Fixed bounds check on number of CM3D regions.
 *
 * Hydra_Software_Devel/6   10/7/08 11:09a jessem
 * PR 39335: Changed memcpy to simple assignment.
 *
 * Hydra_Software_Devel/5   10/7/08 9:57a jessem
 * PR 39335: Enclosed getting of CM3D settings structure in critical
 * section.
 *
 * Hydra_Software_Devel/4   5/2/08 10:55a jessem
 * PR 39335: Corrected hWindow parameter to BVDC_P_Cm3dStoreRegionInfo and
 * BVDC_P_Cm3dStoreOverlappedRegionInfo.
 *
 * Hydra_Software_Devel/3   3/31/08 5:25p jessem
 * PR 39335: Added conditional compile directives.
 *
 * Hydra_Software_Devel/2   3/31/08 3:17p jessem
 * PR 39335: Modified implementation.
 *
 * Hydra_Software_Devel/1   3/31/08 2:17p jessem
 * PR 39335: Initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "berr.h"

#include "bvdc.h"
#include "bvdc_priv.h"
#include "bvdc_cm3d_priv.h"
#include "bvdc_window_priv.h"

BDBG_MODULE(BVDC_CM3D);

BERR_Code BVDC_Window_SetCm3dRegion
	( BVDC_Window_Handle               hWindow,
 	  const BVDC_Cm3dRegion           *pstRegion,
 	  uint32_t                         ulRegionId )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_SetCm3dRegion);

#if (BVDC_P_SUPPORT_CM3D)
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pstRegion);
	BDBG_ASSERT(ulRegionId < BVDC_MAX_CM3D_REGIONS);

	err = BVDC_P_Cm3d_ValidateRegion(pstRegion, ulRegionId);
	if (err != BERR_SUCCESS)
	{
		return err;
	}
	else
	{
		/* only support main display's main window */
		if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
		{
			return BERR_TRACE(BVDC_ERR_CM3D_WINDOW_NOT_SUPPORTED);
		}

		BVDC_P_Cm3dStoreRegionInfo(hWindow, pstRegion, ulRegionId);
	}
#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pstRegion);
	BSTD_UNUSED(ulRegionId);
#endif

	BDBG_LEAVE(BVDC_Window_SetCm3dRegion);
	return err;
}

BERR_Code BVDC_Window_GetCm3dRegion
	( BVDC_Window_Handle               hWindow,
	  BVDC_Cm3dRegion                 *pstRegion,
	  uint32_t                         ulRegionId )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_GetCm3dRegion);

#if (BVDC_P_SUPPORT_CM3D)
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pstRegion);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_CM3D_WINDOW_NOT_SUPPORTED);
	}

	if(ulRegionId < (sizeof(hWindow->stCurResource.hCm3d->astRegion)/sizeof(BVDC_Cm3dRegion)))
	{
		BKNI_EnterCriticalSection();
		*pstRegion = hWindow->stCurResource.hCm3d->astRegion[ulRegionId];
		BKNI_LeaveCriticalSection();
	}
	else
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pstRegion);
	BSTD_UNUSED(ulRegionId);
#endif

	BDBG_LEAVE(BVDC_Window_GetCm3dRegion);
	return err;
}

BERR_Code BVDC_Window_SetCm3dOverlappedRegion
	( BVDC_Window_Handle               hWindow,
	  const BVDC_Cm3dOverlappedRegion *pstRegion,
	  uint32_t                         ulRegionId )
{
	BERR_Code             err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_SetCm3dOverlappedRegion);

#if (BVDC_P_SUPPORT_CM3D)
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pstRegion);
	BDBG_ASSERT(ulRegionId < BVDC_MAX_CM3D_REGIONS);

	err = BVDC_P_Cm3d_ValidateOverlappedRegion(pstRegion, ulRegionId);
	if (err != BERR_SUCCESS)
	{
		return err;
	}
	else
	{
		/* only support main display's main window */
		if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
		{
			return BERR_TRACE(BVDC_ERR_CM3D_WINDOW_NOT_SUPPORTED);
		}

		BVDC_P_Cm3dStoreOverlappedRegionInfo(hWindow, pstRegion, ulRegionId);
	}
#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pstRegion);
	BSTD_UNUSED(ulRegionId);
#endif

	BDBG_LEAVE(BVDC_Window_SetCm3dOverlappedRegion);
	return err;
}

BERR_Code BVDC_Window_GetCm3dOverlappedRegion
	( BVDC_Window_Handle               hWindow,
	  BVDC_Cm3dOverlappedRegion       *pstRegion,
	  uint32_t                         ulRegionId )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_SetCm3dOverlappedRegion);

#if (BVDC_P_SUPPORT_CM3D)
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pstRegion);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_CM3D_WINDOW_NOT_SUPPORTED);
	}

	if(ulRegionId < (sizeof(hWindow->stCurResource.hCm3d->astOvlpRegion)/sizeof(BVDC_Cm3dOverlappedRegion)))
	{
		BKNI_EnterCriticalSection();
		*pstRegion = hWindow->stCurResource.hCm3d->astOvlpRegion[ulRegionId];
		BKNI_LeaveCriticalSection();
	}
	else
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pstRegion);
	BSTD_UNUSED(ulRegionId);
#endif

	BDBG_LEAVE(BVDC_Window_SetCm3dOverlappedRegion);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetChromaStatsConfiguration
	( BVDC_Window_Handle               hWindow,
	  const BVDC_ChromaSettings       *pSettings )
{
	BERR_Code             err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_SetChromaStatsConfiguration);

#if (BVDC_P_SUPPORT_CM3D)
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if((hWindow->eId != BVDC_P_WindowId_eComp0_V0) && (!hWindow->bChromaHistAvail))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	if (!pSettings)
	{
		hWindow->stNewInfo.bChromaHistEnable = false;
	}
	else
	{
		if(pSettings->stRegion.ulLeft   > BVDC_P_CLIPRECT_PERCENT ||
		   pSettings->stRegion.ulRight  > BVDC_P_CLIPRECT_PERCENT ||
		   pSettings->stRegion.ulTop    > BVDC_P_CLIPRECT_PERCENT ||
		   pSettings->stRegion.ulBottom > BVDC_P_CLIPRECT_PERCENT ||
		   pSettings->stRegion.ulLeft + pSettings->stRegion.ulRight  > BVDC_P_CLIPRECT_PERCENT ||
		   pSettings->stRegion.ulTop  + pSettings->stRegion.ulBottom > BVDC_P_CLIPRECT_PERCENT ||
		   pSettings->ulHueMax > BVDC_P_MAX_CHIST_HUE_ANGLE ||
		   pSettings->ulHueMin > BVDC_P_MAX_CHIST_HUE_ANGLE)
		{
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		hWindow->stNewInfo.stChromaRect = *pSettings;
		hWindow->stNewInfo.bChromaHistEnable = true;
	}

#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pSettings);
#endif

	BDBG_LEAVE(BVDC_Window_SetChromaStatsConfiguration);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetChromaStatsConfiguration
	( BVDC_Window_Handle               hWindow,
	  BVDC_ChromaSettings             *pSettings )
{
	BDBG_ENTER(BVDC_Window_GetChromaStatsConfiguration);

#if (BVDC_P_SUPPORT_CM3D)
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if((hWindow->eId != BVDC_P_WindowId_eComp0_V0) && (!hWindow->bChromaHistAvail))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	if (pSettings)
	{
		*pSettings = hWindow->stCurInfo.stChromaRect;
	}

#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pSettings);
#endif

	BDBG_LEAVE(BVDC_Window_GetChromaStatsConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetChromaStatus
	( const BVDC_Window_Handle         hWindow,
	  BVDC_ChromaStatus               *pStatus )
{
	BDBG_ENTER(BVDC_Window_GetChromaStatus);

#if BVDC_P_SUPPORT_CM3D
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if((hWindow->eId != BVDC_P_WindowId_eComp0_V0) && (!hWindow->bChromaHistAvail))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	/* set return value */
	if (pStatus)
	{
		if ((hWindow->bChromaHistAvail) && (hWindow->stCurInfo.bChromaHistEnable))
		{
			BVDC_P_Cm3d_GetChromaHistogramData(hWindow->stCurResource.hCm3d, pStatus);
		}
		else
		{
			BDBG_ERR(("Chroma Histogram collection is not enabled, cannot retrieve histogram data"));
			return BERR_TRACE(BERR_NOT_SUPPORTED);
		}
	}

#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pStatus);
#endif

	BDBG_LEAVE(BVDC_Window_GetChromaStatus);
	return BERR_SUCCESS;
}


/* End of file */
