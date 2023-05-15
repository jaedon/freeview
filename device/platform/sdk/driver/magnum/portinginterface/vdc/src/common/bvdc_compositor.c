/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_compositor.c $
 * $brcm_Revision: Hydra_Software_Devel/44 $
 * $brcm_Date: 8/13/12 4:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_compositor.c $
 * 
 * Hydra_Software_Devel/44   8/13/12 4:01p tdo
 * SW7425-3734: Fix coverity issues
 *
 * Hydra_Software_Devel/43   3/2/12 2:24p yuxiaz
 * SW7425-2526: Added runtime query capabilities for compositor, window
 * and display.
 *
 * Hydra_Software_Devel/42   8/8/11 1:58p albertl
 * SW7425-745: Fixed coverity error.
 *
 * Hydra_Software_Devel/41   8/5/11 8:24p albertl
 * SW7425-745: Added blanking to compositor color for all outputs and all
 * colorspaces.
 *
 * Hydra_Software_Devel/40   1/21/11 11:15a tdo
 * SW7125-792: Fix compiling error
 *
 * Hydra_Software_Devel/39   1/20/11 4:59p tdo
 * SW7125-792: Properly handle HDMI sync only
 *
 * Hydra_Software_Devel/38   5/7/10 7:08p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/37   4/19/10 10:13p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/36   4/7/10 11:27a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/35   4/5/10 4:00p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/34   6/7/08 8:25p albertl
 * PR39336, PR39163:  Fixed bugs in second compositor support.  Added
 * dirty bits mechanism to compositor for colorclip.
 *
 * Hydra_Software_Devel/33   5/21/08 3:54p albertl
 * PR39163:  Moved colorclip from window to compositor and changed
 * BVDC_Window_Set/GetColorClip to BVDC_Compositor_Set/GetColorClip.
 *
 * Hydra_Software_Devel/32   4/1/08 3:06p rpan
 * PR39337: Implemented CMP OSD configuration.
 *
 * Hydra_Software_Devel/31   3/19/08 5:43p tdo
 * PR39333: Implement HIST core
 *
 * Hydra_Software_Devel/30   11/28/07 2:01p tdo
 * PR37413: Provide new Api to set offset and size of histogram window and
 * read corresponding parameters from specific window
 *
 * Hydra_Software_Devel/29   6/11/07 1:07p tdo
 * PR30613: Modify dynamic contrast stretch prototype to meet customer
 * requirement
 *
 * Hydra_Software_Devel/28   4/5/07 6:12p tdo
 * PR25462: add support for luma average reporting in the PEP
 *
 * Hydra_Software_Devel/27   4/5/07 2:00p pntruong
 * PR28538, PR28539, PR28540, PR25462: Average pixel luma and customized
 * deinterlacer support.
 *
 * Hydra_Software_Devel/26   3/29/07 4:50p pntruong
 * PR25462: Updated the average pixel luma descriptions, and removed old
 * duplicated api.
 *
 * Hydra_Software_Devel/PR28538/1   4/3/07 9:31p xhuang
 * PR25462: Support APL setting
 *
 * Hydra_Software_Devel/25   1/4/07 12:03p yuxiaz
 * PR 26890: Added VDC_Source_GetDefaultSettings,
 * BVDC_Window_GetDefaultSettings and BVDC_Compositor_GetDefaultSettings.
 *
 * Hydra_Software_Devel/24   12/18/06 11:24p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   12/6/06 5:40p pntruong
 * PR25462: Need to support luma average reporting in compostior.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/19/06 6:58p hongtaoz
 * PR23260: add dvo master mode support; add support for cmp/display
 * connection at create time; restructured display code to be scalable
 * for 7400B support;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/19/06 4:35p pntruong
 * PR23222, PR23225: VDEC - ATSC & PC input support.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/19/06 1:46p pntruong
 * PR23222, PR23225: VDEC - ATSC & PC input support.
 *
 * Hydra_Software_Devel/23   3/18/05 6:30p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/22   10/11/04 11:00a pntruong
 * PR12966: VDC Background color: Get does not match Set.
 *
 * Hydra_Software_Devel/21   8/13/04 7:22p syang
 * PR 12231: added window id in error message when destroying comp with
 * window connected.
 *
 * Hydra_Software_Devel/20   7/7/04 5:42p pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 * Hydra_Software_Devel/19   7/6/04 8:28a pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 ***************************************************************************/
#include "bstd.h"                     /* standard types */
#include "bpxl.h"                     /* Pixel format */
#include "bkni.h"                     /* For malloc */
#include "bvdc.h"                     /* Video display */
#include "bvdc_compositor_priv.h"
#include "bvdc_priv.h"


BDBG_MODULE(BVDC_CMP);


/***************************************************************************
 *
 */
BERR_Code BVDC_Compositor_GetDefaultSettings
	( BVDC_CompositorId                eCompositorId,
	  BVDC_Compositor_Settings        *pDefSettings )
{
	BSTD_UNUSED(eCompositorId);
	BSTD_UNUSED(pDefSettings);

	return BERR_SUCCESS;
}

/***************************************************************************
 * Create a compositor.
 *
 * If eCompositorId is not create this function will create an compositor
 * handle and pass it back to user.  It also keep track of it in hVdc.
 */
BERR_Code BVDC_Compositor_Create
	( BVDC_Handle                      hVdc,
	  BVDC_Compositor_Handle          *phCompositor,
	  BVDC_CompositorId                eCompositorId,
	  const BVDC_Compositor_Settings  *pDefSettings )
{
	BDBG_ENTER(BVDC_Compositor_Create);
	BSTD_UNUSED(pDefSettings);
	BDBG_ASSERT(phCompositor);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	if(!hVdc->pFeatures->abAvailCmp[eCompositorId])
	{
		BDBG_ERR(("Compositor[%d] not supported on this chipset.", eCompositorId));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	BDBG_OBJECT_ASSERT(hVdc->ahCompositor[eCompositorId], BVDC_CMP);
	/* Check if compositor is created or not. */
	if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahCompositor[eCompositorId]) ||
	   BVDC_P_STATE_IS_CREATE(hVdc->ahCompositor[eCompositorId]) ||
	   BVDC_P_STATE_IS_DESTROY(hVdc->ahCompositor[eCompositorId]))
	{
		return BERR_TRACE(BVDC_ERR_COMPOSITOR_ALREADY_CREATED);
	}

	BDBG_MSG(("Creating compositor%d", hVdc->ahCompositor[eCompositorId]->eId));
	BDBG_ASSERT(BVDC_P_STATE_IS_INACTIVE(hVdc->ahCompositor[eCompositorId]));

	/* Reinitialize context.  But not make it active until apply. */
	*phCompositor = hVdc->ahCompositor[eCompositorId];
	BVDC_P_Compositor_Init(*phCompositor);

	/* Mark as create, awating for apply. */
	hVdc->ahCompositor[eCompositorId]->eState = BVDC_P_State_eCreate;

	BDBG_LEAVE(BVDC_Compositor_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Compositor_Destroy
	( BVDC_Compositor_Handle           hCompositor )
{
	uint32_t i;

	BDBG_ENTER(BVDC_Compositor_Destroy);

	/* Return if trying to free a NULL handle. */
	if(!hCompositor)
	{
		goto done;
	}

	/* check parameters */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* Check to see if there are any active windows. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hCompositor->ahWindow[i]))
		{
			BDBG_ERR(("Active window %d has not been released.",
					  hCompositor->ahWindow[i]->eId));
			BDBG_ERR(("The window state is %d.",
					  hCompositor->ahWindow[i]->eState));
			return BERR_TRACE(BERR_LEAKED_RESOURCE);
		}
	}

	if(BVDC_P_STATE_IS_DESTROY(hCompositor) ||
	   BVDC_P_STATE_IS_INACTIVE(hCompositor))
	{
		goto done;
	}

	if(BVDC_P_STATE_IS_ACTIVE(hCompositor))
	{
		hCompositor->eState = BVDC_P_State_eDestroy;
	}

	if(BVDC_P_STATE_IS_CREATE(hCompositor))
	{
		hCompositor->eState = BVDC_P_State_eInactive;
	}

done:
	BDBG_LEAVE(BVDC_Compositor_Destroy);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Compositor_GetMaxWindowCount
	( const BVDC_Compositor_Handle     hCompositor,
	  uint32_t                        *pulVideoWindowCount,
	  uint32_t                        *pulGfxWindowCount )
{
	BDBG_ENTER(BVDC_Compositor_GetMaxWindowCount);
	/* check parameters */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* set return value */
	if(pulVideoWindowCount)
	{
		*pulVideoWindowCount = hCompositor->pFeatures->ulMaxVideoWindow;
	}

	if(pulGfxWindowCount)
	{
		*pulGfxWindowCount = hCompositor->pFeatures->ulMaxGfxWindow;
	}

	BDBG_LEAVE(BVDC_Compositor_GetMaxWindowCount);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Compositor_SetBackgroundColor
	( BVDC_Compositor_Handle           hCompositor,
	  uint8_t                          ucRed,
	  uint8_t                          ucGreen,
	  uint8_t                          ucBlue )
{
	unsigned int uiARGB;

	BDBG_ENTER(BVDC_Compositor_SetBackgroundColor);
	/* check parameters */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* set new value */
	hCompositor->stNewInfo.ucRed   = ucRed;
	hCompositor->stNewInfo.ucGreen = ucGreen;
	hCompositor->stNewInfo.ucBlue  = ucBlue;
	uiARGB = BPXL_MAKE_PIXEL(BPXL_eA8_R8_G8_B8,
		0x00, ucRed, ucGreen, ucBlue);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	/* Convert base on current colorspace output */
	if((BVDC_P_CmpColorSpace_eHdYCrCb != hCompositor->eCmpColorSpace) &&
	   (BVDC_P_CmpColorSpace_eSmpte_240M != hCompositor->eCmpColorSpace))
	{
		BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
			uiARGB, (unsigned int*)&hCompositor->stNewInfo.ulBgColorYCrCb);
	}
	else
	{
		BPXL_ConvertPixel_RGBtoHdYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
			uiARGB, (unsigned int*)&hCompositor->stNewInfo.ulBgColorYCrCb);
	}
#else
	BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
		uiARGB, (unsigned int*)&hCompositor->stNewInfo.ulBgColorYCrCb);
#endif
	if(hCompositor->hDisplay)
	{
#if BVDC_P_ORTHOGONAL_VEC
	/* Trigger HDMI sync only and mute dirty bits */
		if(hCompositor->hDisplay->stNewInfo.bEnableHdmi)
		{
			hCompositor->hDisplay->stNewInfo.stDirty.stBits.bHdmiSyncOnly = BVDC_P_DIRTY;
		}
#endif
		hCompositor->hDisplay->stNewInfo.stDirty.stBits.bOutputMute = BVDC_P_DIRTY;
	}


	BDBG_LEAVE(BVDC_Compositor_SetBackgroundColor);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Compositor_GetBackgroundColor
	( const BVDC_Compositor_Handle     hCompositor,
	  uint8_t                         *pucRed,
	  uint8_t                         *pucGreen,
	  uint8_t                         *pucBlue )
{
	BDBG_ENTER(BVDC_Compositor_GetBackgroundColor);
	/* check parameters */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* set return value */
	if(pucRed)
	{
		*pucRed   = hCompositor->stCurInfo.ucRed;
	}

	if(pucGreen)
	{
		*pucGreen = hCompositor->stCurInfo.ucGreen;
	}

	if(pucBlue)
	{
		*pucBlue  = hCompositor->stCurInfo.ucBlue;
	}

	BDBG_LEAVE(BVDC_Compositor_GetBackgroundColor);
	return BERR_SUCCESS;
}

/***************************************************************************
*
**************************************************************************/
BERR_Code BVDC_Compositor_SetLumaStatsConfiguration
	( BVDC_Compositor_Handle           hCompositor,
	  const BVDC_LumaSettings         *pLumaSettings )
{
	BERR_Code             err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Compositor_SetLumaStatsConfiguration);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* TODO: support LUMA AVG for CMP_1 */

	if(hCompositor->eId == BVDC_CompositorId_eCompositor0)
	{
		if(pLumaSettings)
		{
			hCompositor->stNewInfo.stLumaRect = *pLumaSettings;
			hCompositor->stNewInfo.bLumaRectUserSet = true;
		}
		else
		{
			hCompositor->stNewInfo.bLumaRectUserSet = false;
		}
	}
	else
	{
		err = BERR_TRACE(BERR_NOT_SUPPORTED);
		BDBG_ERR(("Luma Rect set is not supported for compositor[%d]", hCompositor->eId));
	}

	BDBG_LEAVE(BVDC_Compositor_SetLumaStatsConfiguration);
	return err;
}


/***************************************************************************
*
**************************************************************************/
BERR_Code BVDC_Compositor_GetLumaStatsConfiguration
	( const BVDC_Compositor_Handle     hCompositor,
	  BVDC_LumaSettings               *pLumaSettings )
{
	BDBG_ENTER(BVDC_Compositor_GetLumaStatsConfiguration);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	if(pLumaSettings)
	{
		*pLumaSettings = hCompositor->stCurInfo.stLumaRect;
	}

	BDBG_LEAVE(BVDC_Compositor_GetLumaStatsConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
*
**************************************************************************/
BERR_Code BVDC_Compositor_GetLumaStatus
	( const BVDC_Compositor_Handle     hCompositor,
	  BVDC_LumaStatus                 *pLumaStatus )
{
	BDBG_ENTER(BVDC_Compositor_GetLumaStatus);

	/* TODO: support LUMA AVG for CMP_1 */

	/* check parameters */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	if(hCompositor->eId != BVDC_CompositorId_eCompositor0)
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	/* set return value */
	if(pLumaStatus)
	{
#if (BVDC_P_SUPPORT_CMP_LUMA_AVG_VER > 0)
		uint32_t i;
		uint32_t ulReg;
		BREG_Handle hReg = hCompositor->hVdc->hRegister;

		/* Read luma suma from hw and return it to user. */
		ulReg = BREG_Read32(hReg, BCHP_CMP_0_CMP_LUMA_SUM);
		pLumaStatus->ulSum =
			BCHP_GET_FIELD_DATA(ulReg, CMP_0_CMP_LUMA_SUM, LUMA_SUM);

		ulReg = BREG_Read32(hReg, BCHP_CMP_0_CMP_PIXEL_COUNT);
		pLumaStatus->ulPixelCnt =
			BCHP_GET_FIELD_DATA(ulReg, CMP_0_CMP_PIXEL_COUNT, PIXEL_COUNT);

		/* Compositor does not min/max/histogram. */
		pLumaStatus->ulMax = 0;
		pLumaStatus->ulMin = 0;
		for(i = 0; i < BVDC_LUMA_HISTOGRAM_COUNT; i++)
		{
			pLumaStatus->aulHistogram[i] = 0;
		}
#endif
	}

	BDBG_LEAVE(BVDC_Compositor_GetLumaStatus);
	return BERR_SUCCESS;
}


/***************************************************************************
**************************************************************************/
BERR_Code BVDC_Compositor_SetOsdConfiguration
	( BVDC_Compositor_Handle     hCompositor,
	  const BVDC_OsdSettings     *pOsdSettings )
{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	BSTD_UNUSED(hCompositor);
	BSTD_UNUSED(pOsdSettings);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#else
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Compositor_SetOsdConfiguration);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	if (hCompositor->eId == BVDC_CompositorId_eCompositor0)
	{
#if (!BVDC_P_SUPPORT_CMP_OSD)
		err = BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
	}
	else
	{
		/* OSD configuration is only supported by compositor 0 of
		 * certain chipsets.
		 */
		err = BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	if (err)
	{
		BDBG_ERR(("OSD set is not supported for compositor[%d]", hCompositor->eId));
		BDBG_LEAVE(BVDC_Compositor_SetOsdConfiguration);
		return err;
	}

	hCompositor->stNewInfo.stOsd = *pOsdSettings;

	BDBG_LEAVE(BVDC_Compositor_SetOsdConfiguration);
	return err;
#endif
}


/***************************************************************************
**************************************************************************/
BERR_Code BVDC_Compositor_GetOsdConfiguration
	( BVDC_Compositor_Handle     hCompositor,
	  BVDC_OsdSettings           *pOsdSettings )
{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	BSTD_UNUSED(hCompositor);
	BSTD_UNUSED(pOsdSettings);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#else
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Compositor_GetOsdConfiguration);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	if (hCompositor->eId == BVDC_CompositorId_eCompositor0)
	{
#if (!BVDC_P_SUPPORT_CMP_OSD)
		err = BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
	}
	else
	{
		/* OSD configuration is only supported by compositor 0 of
		 * certain chipsets.
		 */
		err = BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	if (err)
	{
		BDBG_ERR(("OSD get is not supported for compositor[%d]", hCompositor->eId));
		BDBG_LEAVE(BVDC_Compositor_GetOsdConfiguration);
		return err;
	}

	*pOsdSettings = hCompositor->stCurInfo.stOsd;

	BDBG_LEAVE(BVDC_Compositor_GetOsdConfiguration);
	return err;
#endif
}


/***************************************************************************
**************************************************************************/
BERR_Code BVDC_Compositor_SetColorClip
	( BVDC_Compositor_Handle          hCompositor,
	  const BVDC_ColorClipSettings   *pstColorClipSettings )
{
	BDBG_ENTER(BVDC_Compositor_SetColorClip);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* only support compositor 0 */
	if(hCompositor->eId != BVDC_CompositorId_eCompositor0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pstColorClipSettings->eColorClipMode > BVDC_ColorClipMode_Both)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hCompositor->stNewInfo.stColorClipSettings = *pstColorClipSettings;

	hCompositor->stNewInfo.stDirty.stBits.bColorClip = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Compositor_SetColorClip);
	return BERR_SUCCESS;
}


/***************************************************************************
**************************************************************************/
BERR_Code BVDC_Compositor_GetColorClip
	( BVDC_Compositor_Handle          hCompositor,
	  BVDC_ColorClipSettings         *pstColorClipSettings )
{
	BDBG_ENTER(BVDC_Compositor_GetColorClip);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	if(pstColorClipSettings)
	{
		*pstColorClipSettings = hCompositor->stCurInfo.stColorClipSettings;
	}

	BDBG_LEAVE(BVDC_Compositor_GetColorClip);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Compositor_GetCapabilities
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_Compositor_Capabilities    *pCapabilities )
{
	BSTD_UNUSED(hCompositor);
	BSTD_UNUSED(pCapabilities);

	return BERR_SUCCESS;
}

/* End of File */
