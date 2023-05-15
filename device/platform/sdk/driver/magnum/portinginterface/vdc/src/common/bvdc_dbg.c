/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_dbg.c $
 * $brcm_Revision: Hydra_Software_Devel/51 $
 * $brcm_Date: 8/16/12 3:00p $
 *
 * Module Description:
 *   This module print out helpful information to debug BVN
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_dbg.c $
 * 
 * Hydra_Software_Devel/51   8/16/12 3:00p vanessah
 * SW7425-2571: change on BVDC_Dbg_MaskBvnErrorCb
 * 
 * Hydra_Software_Devel/50   8/15/12 6:22p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/49   8/13/12 4:03p tdo
 * SW7425-3734: Fix coverity issues
 *
 * Hydra_Software_Devel/48   4/26/12 11:34a tdo
 * SW7425-2571: Add VDC API to query BVN module error
 *
 * Hydra_Software_Devel/47   11/23/11 4:25p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/46   8/15/11 4:21p tdo
 * SW7420-2017: Add debug function to get SCL status register
 *
 * Hydra_Software_Devel/45   7/22/11 11:13p pntruong
 * SW7425-983: Corrected deinterlacer's hscaler threshold.
 *
 * Hydra_Software_Devel/44   3/15/11 4:22p yuxiaz
 * SW7400-2882: Added support to enable / disable multi-buffering logging
 * for a specific window.
 *
 * Hydra_Software_Devel/43   2/9/11 3:59p yuxiaz
 * SW7400-2882: Ported multi-buffing event logging scheme to VDC. Move
 * related APIs to bvdc_dbg.h
 *
 * Hydra_Software_Devel/42   10/4/10 5:24p hongtaoz
 * SW7425-33: added the 4th compositor support;
 *
 * Hydra_Software_Devel/41   9/30/10 3:18p tdo
 * SW7422-47: VDC: Need to handle re-arragnement of core level interrupts
 *
 * Hydra_Software_Devel/40   9/13/10 4:35p yuxiaz
 * SW7358-4: Added initial VDC support for 7358.
 *
 * Hydra_Software_Devel/39   8/30/10 4:15p tdo
 * SW7425-11, SW7425-13: Add 7425 support for VDC
 *
 * Hydra_Software_Devel/38   8/26/10 5:25p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/37   6/14/10 3:36p hongtaoz
 * SW3548-2976: soft init GFD when DCX switches from ON to OFF, and when
 * GFD source is created; added GFD BVB error monitoring to debug build;
 *
 * Hydra_Software_Devel/36   4/19/10 10:13p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/35   4/7/10 11:28a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/34   4/5/10 4:04p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/33   2/9/10 2:26p tdo
 * SW7420-575: Name change MCDI_* -> MDI_*
 *
 * Hydra_Software_Devel/32   3/9/09 1:23p rpan
 * PR52203: Fixed the inconsistency caused by various flags used for
 * compilation checking.
 *
 * Hydra_Software_Devel/31   3/9/09 11:18a rpan
 * PR52203: More checks before turn on MCVP error checking.
 *
 * Hydra_Software_Devel/30   3/9/09 10:05a rpan
 * PR52203: Add error bit handling for newly added cores.
 *
 * Hydra_Software_Devel/27   9/5/08 4:46p tdo
 * PR46484: Bringup appframework for7420
 *
 * Hydra_Software_Devel/26   6/24/08 6:57p hongtaoz
 * PR40499: added ANR error debug callback;
 *
 * Hydra_Software_Devel/25   5/16/08 9:35a yuxiaz
 * PR42732: Coverity Defect ID:7846 FORWARD_NULL bvdc_dbg.c
 *
 * Hydra_Software_Devel/24   2/12/08 8:18p pntruong
 * PR38929: Make more files compile.
 *
 * Hydra_Software_Devel/23   1/29/08 6:08p pntruong
 * PR34870: Added get handle debug functions for video tool library.
 *
 * Hydra_Software_Devel/22   1/18/08 12:08p hongtaoz
 * PR38756: added MAD to the BVN error debug handling;
 *
 * Hydra_Software_Devel/21   9/21/07 3:59p hongtaoz
 * PR34955: added Progressive Segmented Frame(PsF) 1080p support; fixed
 * 1080p pass-through condition; fixed bvdc_dbg register access error;
 * fixed general progressive capture interlaced playback feeder
 * programming problem;
 *
 * Hydra_Software_Devel/20   4/4/07 3:44p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 * Hydra_Software_Devel/19   2/16/07 4:32p pntruong
 * PR15284, PR27951: Enabled error printing for all errors, and correct
 * data corruption.
 *
 * Hydra_Software_Devel/18   2/16/07 11:36a pntruong
 * PR15284, PR27951: Fixed build error for bvn debug disabled.
 *
 * Hydra_Software_Devel/17   2/16/07 9:37a pntruong
 * PR15284, PR27951: Graphics shimmering on HD path when video is scaled
 * down (PIG).  HW fixed.  Removed software work-around to avoid bvb
 * error interrupts from window surface.  Make bvdc dbg more portable.
 *
 * Hydra_Software_Devel/16   2/12/07 1:49p pntruong
 * PR27821:  Remove unused lagacy and work-around code (7038) from
 * mainline.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bvdc.h"
#include "brdc_dbg.h"
#include "bvdc_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_compositor_priv.h"

BDBG_MODULE(BVDC_DBG);


/***************************************************************************
 * Generic error recovery handler for BVN blocks.
 */
void BVDC_P_BvnErrorHandler_isr
	( void                            *pvhVdc,
	  int                              iIdx )
{
	BVDC_Handle hVdc = (BVDC_Handle)pvhVdc;
	const BVDC_P_IntCbTbl *pIntCb;

	/* log the interrupt count */
	hVdc->aulBvnErrCnt[iIdx]++;

	if(!hVdc->abBvnErrMask[iIdx])
	{
		pIntCb = BVDC_P_GetBvnErrorCb(iIdx);
		if(pIntCb != NULL)
		{
			BDBG_ERR(("ASSERT INT %s(%d): %d", pIntCb->pchInterruptName, iIdx, hVdc->aulBvnErrCnt[iIdx]));
			BDBG_ASSERT(0);
		}
	}
	return;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Dbg_CreateBvnErrorCb
	( BVDC_Handle                  hVdc )
{
	uint32_t i;
	BERR_Code eStatus = BERR_SUCCESS;
	const BVDC_P_IntCbTbl *pIntCb;

	for(i = 0; i < BVDC_BvnError_eMaxCount; i++)
	{
		pIntCb = BVDC_P_GetBvnErrorCb(i);
		if(pIntCb == NULL)
			continue;

		BDBG_MSG(("Install IntId %s(%d) L2Reg=0x%x Mask=0x%x",
			pIntCb->pchInterruptName, i, pIntCb->ulL2ClearReg, pIntCb->ulL2ClearMask));
		eStatus = BINT_CreateCallback(&hVdc->ahBvnErrHandlerCb[i], hVdc->hInterrupt,
			pIntCb->ErrIntId,
			pIntCb->pfCallback,
			(void*)hVdc, i);
		if(BERR_SUCCESS != eStatus)
		{
			return BERR_TRACE(eStatus);
		}

		eStatus = BINT_ClearCallback(hVdc->ahBvnErrHandlerCb[i]);
		if(BERR_SUCCESS != eStatus)
		{
			return BERR_TRACE(eStatus);
		}

		eStatus = BINT_EnableCallback(hVdc->ahBvnErrHandlerCb[i]);
		if(BERR_SUCCESS != eStatus)
		{
			return BERR_TRACE(eStatus);
		}
	}

	return eStatus;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Dbg_DestroyBvnErrorCb
	( BVDC_Handle                  hVdc )
{
	uint32_t i;
	BERR_Code eStatus = BERR_SUCCESS;

	for(i = 0; i < BVDC_BvnError_eMaxCount; i++)
	{
		if(hVdc->ahBvnErrHandlerCb[i] == NULL)
			continue;

		eStatus = BINT_DisableCallback(hVdc->ahBvnErrHandlerCb[i]);
		if(BERR_SUCCESS != eStatus)
		{
			return BERR_TRACE(eStatus);
		}

		eStatus = BINT_DestroyCallback(hVdc->ahBvnErrHandlerCb[i]);
		if(BERR_SUCCESS != eStatus)
		{
			return BERR_TRACE(eStatus);
		}
	}

	return eStatus;
}


/***************************************************************************
 *
 */
void BVDC_Dbg_ClearBvnError
	( BVDC_Handle                  hVdc )
{
	uint32_t i;
	const BVDC_P_IntCbTbl *pIntCb;

	for(i = 0; i < BVDC_BvnError_eMaxCount; i++)
	{
		pIntCb = BVDC_P_GetBvnErrorCb(i);
		if(pIntCb == NULL)
			continue;

		BDBG_MSG(("Clear IntId %s(%d) L2Reg=0x%x Mask=0x%x",
			pIntCb->pchInterruptName, i, pIntCb->ulL2ClearReg, pIntCb->ulL2ClearMask));
		BREG_Write32(hVdc->hRegister, pIntCb->ulL2ClearReg, pIntCb->ulL2ClearMask);

		/* Clear interrupt count */
		hVdc->aulBvnErrCnt[i] = 0;
	}

	return;
}


/***************************************************************************
 *
 */
void BVDC_Dbg_GetBvnErrorStatus
	( BVDC_Handle                  hVdc )
{
	uint32_t i;
	const BVDC_P_IntCbTbl *pIntCb;

	for(i = 0; i < BVDC_BvnError_eMaxCount; i++)
	{
		if(hVdc->aulBvnErrCnt[i] != 0)
		{
			pIntCb = BVDC_P_GetBvnErrorCb(i);
			if(pIntCb != NULL)
			{
				BDBG_ERR(("%s(%d): %d", pIntCb->pchInterruptName, i, hVdc->aulBvnErrCnt[i]));
			}
			else
			{
				BDBG_ASSERT(pIntCb != NULL);
			}
		}
	}
	return;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Dbg_MaskBvnErrorCb
	( BVDC_Handle                  hVdc,
	  BVDC_BvnModule               eBvnModule,
	  uint32_t                     ulModuleIdx,
	  bool                         bMaskOff )
{
	BVDC_BvnError       eBvnErrModule;
	BERR_Code eStatus = BERR_SUCCESS;

	if(BVDC_Bvn_eMaxCount <= eBvnModule)
	{
		eStatus = BERR_INVALID_PARAMETER;
		return (eStatus);
	}

	switch (eBvnModule)
	{
		case BVDC_Bvn_eRdc:
			eBvnErrModule = BVDC_BvnError_eRdc; 
			BSTD_UNUSED(ulModuleIdx);
			break;
			
		case BVDC_Bvn_eMfd:
			if(ulModuleIdx >=(BVDC_BvnError_eVfd_0 - BVDC_BvnError_eMfd_0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eMfd_0 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eVfd:
			if(ulModuleIdx >=(BVDC_BvnError_eScl_0 - BVDC_BvnError_eVfd_0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eVfd_0 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eScl:
			if(ulModuleIdx >=(BVDC_BvnError_eDnr_0 - BVDC_BvnError_eScl_0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eScl_0 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eDnr:
			if(ulModuleIdx >=(BVDC_BvnError_eMad_0 - BVDC_BvnError_eDnr_0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eDnr_0 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eMad:
			eBvnErrModule = BVDC_BvnError_eMad_0;
			BSTD_UNUSED(ulModuleIdx);
			break;
			
		case BVDC_Bvn_eMvp:
			if(ulModuleIdx >=(BVDC_BvnError_eMcdi_0 - BVDC_BvnError_eMvp_0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eMvp_0 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eMcdi:
			if(ulModuleIdx >=(BVDC_BvnError_eMctf_0 - BVDC_BvnError_eMcdi_0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eMcdi_0 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eMctf:
			eBvnErrModule = BVDC_BvnError_eMctf_0;
			BSTD_UNUSED(ulModuleIdx);
			break;
			
		case BVDC_Bvn_eHscl:
			if(ulModuleIdx >=(BVDC_BvnError_eCap_0 - BVDC_BvnError_eHscl_0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eHscl_0 + ulModuleIdx;
			break;
		case BVDC_Bvn_eCap:
			if(ulModuleIdx >=(BVDC_BvnError_eGfd_0 - BVDC_BvnError_eCap_0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eCap_0 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eGfd:
			if(ulModuleIdx >=(BVDC_BvnError_eCmp_0_V0 - BVDC_BvnError_eGfd_0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eGfd_0 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eCmp_V0:
			if(ulModuleIdx >=(BVDC_BvnError_eCmp_0_V1 - BVDC_BvnError_eCmp_0_V0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eCmp_0_V0 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eCmp_V1:
			if(ulModuleIdx >=(BVDC_BvnError_eCmp_0_G0 - BVDC_BvnError_eCmp_0_V1))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eCmp_0_V1 + ulModuleIdx;
			break;
			
		case BVDC_Bvn_eCmp_G0:
			if(ulModuleIdx >=(BVDC_BvnError_eMaxCount - BVDC_BvnError_eCmp_0_G0))
			{
				eStatus = BERR_INVALID_PARAMETER;
				return (eStatus);
			}
			
			eBvnErrModule = BVDC_BvnError_eCmp_0_G0 + ulModuleIdx;
			break;
		default:
			eStatus = BERR_INVALID_PARAMETER;
			return (eStatus);
	}

	hVdc->abBvnErrMask[eBvnErrModule] = bMaskOff;
	return eStatus;
}


/***************************************************************************
 *
 */
BVDC_Source_Handle BVDC_Dbg_GetSourceHandle
	( const BVDC_Handle                hVdc,
	  BAVC_SourceId                    eSourceId )
{
	BVDC_Source_Handle hSrc = NULL;

	/* Sanity checks */
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahSource[eSourceId]))
	{
		hSrc = hVdc->ahSource[eSourceId];
	}

	return hSrc;
}


/***************************************************************************
 *
 */
BVDC_Compositor_Handle BVDC_Dbg_GetCompositorHandle
	( const BVDC_Handle                hVdc,
	  BVDC_CompositorId                eCompositorId )
{
	BVDC_Compositor_Handle hCmp = NULL;

	/* Sanity checks */
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahCompositor[eCompositorId]))
	{
		hCmp = hVdc->ahCompositor[eCompositorId];
	}

	return hCmp;
}


/***************************************************************************
 *
 */
BVDC_Window_Handle BVDC_Dbg_GetWindowHandle
	( const BVDC_Handle                hVdc,
	  BVDC_CompositorId                eCompositorId,
	  BVDC_WindowId                    eWindowId )
{
	BVDC_Window_Handle hWin = NULL;
	BVDC_Compositor_Handle hCmp = NULL;

	/* Sanity checks */
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* Get the compositor */
	hCmp = BVDC_Dbg_GetCompositorHandle(hVdc, eCompositorId);

	/* Do we have valid compositor */
	if(hCmp)
	{
		BVDC_P_WindowId eWinId;
		BDBG_OBJECT_ASSERT(hCmp, BVDC_CMP);
		switch(eCompositorId)
		{
		case BVDC_CompositorId_eCompositor0:
			switch(eWindowId)
			{
			case BVDC_WindowId_eVideo0:
				eWinId = BVDC_P_WindowId_eComp0_V0;
				break;

			case BVDC_WindowId_eVideo1:
				eWinId = BVDC_P_WindowId_eComp0_V1;
				break;

			case BVDC_WindowId_eGfx0:
				eWinId = BVDC_P_WindowId_eComp0_G0;
				break;

			case BVDC_WindowId_eGfx1:
				eWinId = BVDC_P_WindowId_eComp0_G1;
				break;

			case BVDC_WindowId_eGfx2:
				eWinId = BVDC_P_WindowId_eComp0_G2;
				break;

			default:
				return NULL;
			}
			break;

		case BVDC_CompositorId_eCompositor1:
			switch(eWindowId)
			{
			case BVDC_WindowId_eVideo0:
				eWinId = BVDC_P_WindowId_eComp1_V0;
				break;

			case BVDC_WindowId_eVideo1:
				eWinId = BVDC_P_WindowId_eComp1_V1;
				break;

			case BVDC_WindowId_eGfx0:
				eWinId = BVDC_P_WindowId_eComp1_G0;
				break;

			default:
				return NULL;
			}
			break;

		case BVDC_CompositorId_eCompositor2:
			switch(eWindowId)
			{
			case BVDC_WindowId_eVideo0:
				eWinId = BVDC_P_WindowId_eComp2_V0;
				break;

			case BVDC_WindowId_eGfx0:
				eWinId = BVDC_P_WindowId_eComp2_G0;
				break;

			default:
				return NULL;
			}
			break;

		case BVDC_CompositorId_eCompositor3:
			switch(eWindowId)
			{
			case BVDC_WindowId_eVideo0:
				eWinId = BVDC_P_WindowId_eComp3_V0;
				break;

			case BVDC_WindowId_eGfx0:
				eWinId = BVDC_P_WindowId_eComp3_G0;
				break;

			default:
				return NULL;
			}
			break;

		case BVDC_CompositorId_eCompositor4:
			switch(eWindowId)
			{
			case BVDC_WindowId_eVideo0:
				eWinId = BVDC_P_WindowId_eComp4_V0;
				break;

			case BVDC_WindowId_eGfx0:
				eWinId = BVDC_P_WindowId_eComp4_G0;
				break;

			default:
				return NULL;
			}
			break;

		case BVDC_CompositorId_eCompositor5:
			switch(eWindowId)
			{
			case BVDC_WindowId_eVideo0:
				eWinId = BVDC_P_WindowId_eComp5_V0;
				break;

			case BVDC_WindowId_eGfx0:
				eWinId = BVDC_P_WindowId_eComp5_G0;
				break;

			default:
				return NULL;
			}
			break;

		case BVDC_CompositorId_eCompositor6:
			switch(eWindowId)
			{
			case BVDC_WindowId_eVideo0:
				eWinId = BVDC_P_WindowId_eComp6_V0;
				break;

			case BVDC_WindowId_eGfx0:
				eWinId = BVDC_P_WindowId_eComp6_G0;
				break;

			default:
				return NULL;
			}
			break;

		default:
			return NULL;
		}

		if(BVDC_P_STATE_IS_ACTIVE(hCmp->ahWindow[eWinId]))
		{
			hWin = hCmp->ahWindow[eWinId];
		}
	}

	return hWin;
}


/***************************************************************************
 *
 */
BVDC_Source_Handle BVDC_Dbg_Window_GetSourceHandle
	( const BVDC_Window_Handle         hWindow )
{
	BVDC_Source_Handle hSrc = NULL;

	/* Sanity checks */
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(BVDC_P_STATE_IS_ACTIVE(hWindow) &&
	   BVDC_P_STATE_IS_ACTIVE(hWindow->stCurInfo.hSource))
	{
		hSrc = hWindow->stCurInfo.hSource;
	}

	return hSrc;
}


/***************************************************************************
 *
 */
uint32_t BVDC_Dbg_Window_GetScalerStatus
	( const BVDC_Window_Handle         hWindow )
{
	BVDC_P_Scaler_Handle hScaler;

	/* Sanity checks */
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(BVDC_P_STATE_IS_ACTIVE(hWindow))
	{
		hScaler = hWindow->stCurResource.hScaler;
		return BREG_Read32(hScaler->hReg, BCHP_SCL_0_BVB_IN_STATUS + hScaler->ulRegOffset);
	}

	return 0;
}


/***************************************************************************
 *
 */
BVDC_Compositor_Handle BVDC_Dbg_Window_GetCompositorHandle
	( const BVDC_Window_Handle         hWindow )
{
	BVDC_Compositor_Handle hCmp = NULL;

	/* Sanity checks */
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(BVDC_P_STATE_IS_ACTIVE(hWindow) &&
	   BVDC_P_STATE_IS_ACTIVE(hWindow->hCompositor))
	{
		hCmp = hWindow->hCompositor;
	}

	return hCmp;
}


/***************************************************************************
 *
 */
BVDC_Display_Handle BVDC_Dbg_Compositor_GetDisplayHandle
	( const BVDC_Compositor_Handle     hCompositor )
{
	BVDC_Display_Handle hDis = NULL;

	/* Sanity checks */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	if(BVDC_P_STATE_IS_ACTIVE(hCompositor) &&
	   BVDC_P_STATE_IS_ACTIVE(hCompositor->hDisplay))
	{
		hDis = hCompositor->hDisplay;
	}

	return hDis;
}


/***************************************************************************
 * Main Debug Entry Point!
 *
 */
BERR_Code BVDC_Dbg_Dump
	( BVDC_Handle                      hVdc )
{
	BSTD_UNUSED(hVdc);
	BDBG_MSG(("Not supported!"));
	return BERR_SUCCESS;
}

#if (BVDC_BUF_LOG == 1)
/***************************************************************************
 * BVDC_SetBufLogStateAndDumpTrigger
 *
 * Set when to start logging multi-buffering events and how to notify user
 * the log can be dumped.
 */
BERR_Code BVDC_SetBufLogStateAndDumpTrigger
	( BVDC_BufLogState                 eLogState,
  	  const BVDC_CallbackFunc_isr 	   pfCallback,
	  void							   *pvParm1,
	  int 							   iParm2 )
{
	if (eLogState > BVDC_BufLogState_eAutomaticReduced)
	{
		BDBG_ERR(("Invalid log state %d ", (int) eLogState));
		return BERR_INVALID_PARAMETER;
	}

	if (((BVDC_BufLogState_eAutomatic == eLogState) || (BVDC_BufLogState_eAutomaticReduced == eLogState))
		&& (NULL == pfCallback))
	{
		BDBG_ERR(("Callback function must be registered for eAutomatic or eAutomaticReduced mode"));
		return BERR_INVALID_PARAMETER;
	}

	BVDC_P_Buffer_SetLogStateAndDumpTrigger(eLogState, pfCallback, pvParm1, iParm2);

	return BERR_SUCCESS;
}

/***************************************************************************
 * BVDC_DumpBufLog
 *
 * Print out the captured multi-buffering events log.
 *
 */
void BVDC_DumpBufLog(void)
{
	BVDC_P_Buffer_DumpLog();
}

/***************************************************************************
 * BVDC_SetupManualTrigger
 *
 * Prepares the manual trigger of the multi-buffering event log
 *
 */
void BVDC_SetBufLogManualTrigger(void)
{
    BVDC_P_Buffer_SetManualTrigger();
}

BERR_Code BVDC_Window_EnableBufLog
	( const BVDC_Window_Handle         hWindow,
	  bool                             bEnable )
{
	if(!BVDC_P_WIN_IS_VIDEO_WINDOW(hWindow->eId))
		return BERR_TRACE(BERR_INVALID_PARAMETER);

	BVDC_P_Buffer_EnableBufLog(hWindow->eId, bEnable);
	return BERR_SUCCESS;
}
#endif


/* end of file */
