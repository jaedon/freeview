/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_gfxfeeder.c $
* $brcm_Revision: Hydra_Software_Devel/55 $
* $brcm_Date: 8/16/12 2:41p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_gfxfeeder.c $
* 
* Hydra_Software_Devel/55   8/16/12 2:41p yuxiaz
* SW7425-3619: Add const to read only settings.
* 
* Hydra_Software_Devel/54   6/12/12 6:49p darnstein
* SW7425-3226: fix Coverity errors involving suspicious tests for NULL
* pointer.
* 
* Hydra_Software_Devel/53   5/9/12 12:12p syang
* SW7425-2093:  compile warning msg clean up for diff chips
* 
* Hydra_Software_Devel/52   5/9/12 10:53a syang
* SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
* error fix with GetSurface; use shared gfxsurface to manage gfx surface
* 
* Hydra_Software_Devel/51   7/20/11 1:56p vanessah
* SW7346-212: function prototype reformat
* 
* Hydra_Software_Devel/50   7/20/11 12:44p vanessah
* SW7346-212: pair ulInside isr value
* 
* Hydra_Software_Devel/49   4/28/11 7:15p vanessah
* SW7422-118: fix double buffer setting error
* 
* Hydra_Software_Devel/48   4/27/11 4:59p vanessah
* SW7425-404: enable HSCL and VSCL for 3D due to HW bug
* 
* Hydra_Software_Devel/47   4/1/11 7:38a vanessah
* SW7422-118: double buffer for GFD PI support
* 
* Hydra_Software_Devel/46   11/1/10 4:24p jessem
* SW7422-82: Added stereo 3D support to VFD as source feature.
* 
* Hydra_Software_Devel/45   11/1/10 1:13p vanessah
* SW7422-43:  merge 2D into 3D with a unified interface, change input
* data structure as const
* 
* Hydra_Software_Devel/44   10/29/10 5:06p vanessah
* SW7422-43:  3D graphics render,merge 2D into 3D with a unified
* interface
* 
* Hydra_Software_Devel/43   10/25/10 7:18p vanessah
* SW7422-43:  Handle GFD programming for 3D.
* 
* Hydra_Software_Devel/42   10/11/10 12:34p jessem
* SW7420-173: Added support for VDC as source feature.
* 
* Hydra_Software_Devel/41   6/14/10 3:36p hongtaoz
* SW3548-2976: soft init GFD when DCX switches from ON to OFF, and when
* GFD source is created; added GFD BVB error monitoring to debug build;
* 
* Hydra_Software_Devel/40   5/7/10 7:09p albertl
* SW7125-364: Changed dirty bits to use union structure to avoid type-pun
* warnings.
* 
* Hydra_Software_Devel/39   4/19/10 10:14p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic. Move
* BDBG_OBJECT_ID_DECLARE private header files instead of .c.
*
* Hydra_Software_Devel/38   4/7/10 11:30a tdo
* SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
*
* Hydra_Software_Devel/37   4/5/10 4:06p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic
*
* Hydra_Software_Devel/36   3/18/10 3:59p syang
* SW7550-271: added error check for unsupported 7550 GFD1 features
*
* Hydra_Software_Devel/35   10/27/09 6:23p syang
* SW7420-338: add SetSurface_isr to VDC critical section protection
* assert group
*
* Hydra_Software_Devel/34   9/11/09 4:34p tdo
* SW7420-338: Need to init correct GFD with correct capability
*
* Hydra_Software_Devel/33   8/6/09 2:52p syang
* PR 56506: clean up gamma table format desc
*
* Hydra_Software_Devel/32   7/16/09 12:23p syang
* PR 56417: corrected a typo with EnableGammaCorrection para validation
*
* Hydra_Software_Devel/31   10/9/08 5:28p syang
* PR 46891: add _isr to the name of func used in _isr context
*
* Hydra_Software_Devel/30   10/3/08 9:24a yuxiaz
* PR46305, PR46307, PR46309: More Video TestFeature1 support.
*
* Hydra_Software_Devel/29   10/1/08 5:15p yuxiaz
* PR47303, PR46309: Add video TestFeature1 support for GFD on 3548 B0.
*
* Hydra_Software_Devel/28   9/22/08 4:52p yuxiaz
* PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
*
* Hydra_Software_Devel/27   5/8/08 1:23p syang
* PR 39206: remove alpha surface for 3548 and newer chips
*
* Hydra_Software_Devel/26   1/24/07 9:06p albertl
* PR22237:  Updated BMEM calls to use new BMEM_Heap functions.
*
* Hydra_Software_Devel/25   5/26/06 3:40p pntruong
* PR20642: Refactored handling of hList.
*
* Hydra_Software_Devel/24   11/10/05 3:18p syang
* PR 12244: move alpha-only pixel constant color setting from BSUR to
* BVDC
*
* Hydra_Software_Devel/23   5/5/05 3:55p syang
* PR 15202: re-organize for better readability and debugability
*
* Hydra_Software_Devel/22   3/4/05 3:05p syang
* PR 13282: init pNextFrame to NULL before call callback_func; and  add
* some msg
*
* Hydra_Software_Devel/21   1/20/05 4:01p syang
* PR 13282: added api func to query which surface is currently fetched by
* hardware
*
* Hydra_Software_Devel/20   1/19/05 4:09p syang
* PR 13282: new gfx surface setting is now sent to HW as soon as
* possible: with ApplyChanges in user mode, and immediately in _isr mode
*
* Hydra_Software_Devel/19   10/14/04 6:50p syang
* PR 10756: Re-organize code so that all window ajustments are done in a
* central place for better readability and debugablibilty; Added scale
* factor rounding routine.
*
* Hydra_Software_Devel/18   8/31/04 3:40p syang
* PR 11266: added BVDC_Source_SetSurface_isr back because it might still
* be needed
*
* Hydra_Software_Devel/17   8/26/04 6:36p syang
* PR 11266: added src picture call back func for gfx;  added src state to
* call back func for XVD sync;
*
* Hydra_Software_Devel/16   5/24/04 5:09p jasonh
* PR 11189: Merge down from B0 to main-line
*
* Hydra_Software_Devel/Refsw_Devel_7038_B0/1   5/17/04 2:47p syang
* PR 9750: Eliminated BVDC_Source_EnableAlphaPreMultiply and
* BVDC_Source_DisableAlphaPreMultiply and changed to gfd hw alpha pre-
* multiply automatically basing blend factor setting; updated the para
* of BVDC_Window_SetBlendFactor for gfx window accordingly; updated the
* desc of  BVDC_Window_SetAlpha, BVDC_Window_SetBlendFactor,
* BVDC_Source_EnableColorKey, BVDC_Source_DisableColorKey according to
* the change; added gfx blending setting validation with ValidateChanges
* and added blending setting adjust before RUL building for gfx window.
*
* Hydra_Software_Devel/15   4/5/04 12:39p syang
* PR 10477: Changed to copy ApplyChange activated user setting into
* current inside ApplyChange, and to copy intr surface context into
* current  at the beginning of RUL building; 2) Use Dirty bit field to
* indicate change between new context and current context, and change
* between current context and las RUL; Use Flag bits field to replace
* bool to save size and copying time.
*
* Hydra_Software_Devel/14   2/12/04 10:33a syang
* PR 9712: PR 9609: 1). changed to use window alpha to set the HW
* "default key alpha" register; 2). added W1Alpha and W0Alpha parameters
* to BVDC_Source_SetAlphaSurface; 3).
* BVDC_Source_SetDefaultAlphaForKeying is removed from API; 4). the desc
* of BVDC_Window_SetAlpha / SetBlendFactor and
* BVDC_Source_EnableColorKey are also updated accordingly.
*
* Hydra_Software_Devel/13   12/17/03 4:58p syang
* PR 9025: Change MNO to C2C1C0,  described default alpha of 0xff for
* format without alpha,   mentioned defaultAlphaForKey  usage when color
* is disabled in related places.
*
* Hydra_Software_Devel/12   12/10/03 3:53p syang
* PR 8914: ensure DefaultKeyAlpha and other register inited, fixed
* prroblem with GFD_0_SRC_START in the case of bottom field
*
* Hydra_Software_Devel/11   11/12/03 3:52p syang
* added real gfx scale filter coeff and color space conv matrix
*
* Hydra_Software_Devel/10   11/7/03 3:58p syang
* 1). added converting of bmem buf addr from virtual to device offset
* before sending to HW, hMemory added to gfx context; 2). splitted
* BVDC_P_GfxFeeder_ApplyChanges from BVDC_P_GfxFeeder_BuildRul_isr; 3).
* splitted alpha surface from main surface so that alpha surface change
* could be applied even if main sur does not change.
*
* Hydra_Software_Devel/9   10/28/03 2:03p syang
* added BVDC_Source_SetSourface_isr and BVDC_Source_SetAlphaSurface_isr,
* and corrected the usage of BDBG_ENTER / LEAVE
*
* Hydra_Software_Devel/8   9/19/03 4:37p pntruong
* Moved old & cur user's info into context to be consistent with other
* modules.
*
* Hydra_Software_Devel/7   9/8/03 5:07p syang
* change "unsigned int" to "uint32_t" for
* BVDC_Source_EnableGammaCorrection
*
* Hydra_Software_Devel/6   8/26/03 7:00p syang
* re-organize private data structure and code in order to support both
* user setting and multi buffer switching by M2MC interrupt handler
*
* Hydra_Software_Devel/5   8/25/03 11:36a pntruong
* Use handle.
*
* Hydra_Software_Devel/4   8/19/03 2:34p syang
* clean up variable and error code names for readability, and updating
* eA8_Y8_Cr8_Cb8 to eA8_Y8_Cb8_Cr8
*
* Hydra_Software_Devel/4   8/19/03 2:28p syang
* clean up the variable names and error code names for readability
*
* Hydra_Software_Devel/3   8/12/03 4:22p syang
* First time check in after real gfx feeder implementation is added
*
* Hydra_Software_Devel/2   7/22/03 5:35p pntruong
* Added BDBG_MODULE for debugging.
*
* Hydra_Software_Devel/1   7/22/03 4:30p syang
* init version
*
***************************************************************************/
#include "bstd.h"              /* standard types */
#include "bvdc.h"              /* Video display */
#include "bvdc_source_priv.h"
#include "bvdc_gfxfeeder_priv.h"
#include "bvdc_feeder_priv.h"

BDBG_MODULE(BVDC);

/***************************************************************************
*
*/
BERR_Code BVDC_Source_SetChromaExpansion
	( BVDC_Source_Handle    hSource,
	  BVDC_ChromaExpansion  eChromaExpansion )
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Source_SetChromaExpansion);

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hSource->hGfxFeeder, BVDC_GFX);

	if( eChromaExpansion <= BVDC_ChromaExpansion_eLinearInterpolate )
	{
		hSource->hGfxFeeder->stNewCfgInfo.stDirty.stBits.bChromaExpan = 1;
		hSource->hGfxFeeder->stNewCfgInfo.eChromaExpansion = eChromaExpansion;
	}
	else
	{
		BDBG_ERR(("Bad eChromaExpansion."));
		eStatus = BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_LEAVE(BVDC_Source_SetChromaExpansion);
	return eStatus;
}


/***************************************************************************
*
*/
BERR_Code BVDC_Source_EnableColorKey
	( BVDC_Source_Handle  hSource,
	  uint32_t            ulMinAC2C1C0,
	  uint32_t            ulMaxAC2C1C0,
	  uint32_t            ulMaskAC2C1C0,
	  uint8_t             ucKeyedAlpha )
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Source_EnableColorKey);

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hSource->hGfxFeeder, BVDC_GFX);

	hSource->hGfxFeeder->stNewCfgInfo.stDirty.stBits.bKey = 1;
	hSource->hGfxFeeder->stNewCfgInfo.stFlags.bEnableKey = 1;
	hSource->hGfxFeeder->stNewCfgInfo.ulKeyMinAMNO = ulMinAC2C1C0;
	hSource->hGfxFeeder->stNewCfgInfo.ulKeyMaxAMNO = ulMaxAC2C1C0;
	hSource->hGfxFeeder->stNewCfgInfo.ulKeyMaskAMNO = ulMaskAC2C1C0;
	hSource->hGfxFeeder->stNewCfgInfo.ucKeyedAlpha = ucKeyedAlpha;

	BDBG_LEAVE(BVDC_Source_EnableColorKey);
	return eStatus;
}


/***************************************************************************
*
*/
BERR_Code BVDC_Source_DisableColorKey
	( BVDC_Source_Handle  hSource )
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Source_DisableColorKey);

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hSource->hGfxFeeder, BVDC_GFX);

	hSource->hGfxFeeder->stNewCfgInfo.stFlags.bEnableKey = 0;

	BDBG_LEAVE(BVDC_Source_DisableColorKey);
	return eStatus;
}


/***************************************************************************
*
*/
BERR_Code BVDC_Source_SetHorizontalScaleCoeffs
	( BVDC_Source_Handle               hSource,
	  BVDC_FilterCoeffs                eCoeffs )
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Source_SetHorizontalScaleCoeffs);

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hSource->hGfxFeeder, BVDC_GFX);
	if( eCoeffs <= BVDC_FilterCoeffs_eSharp )
	{
		hSource->hGfxFeeder->stNewCfgInfo.stDirty.stBits.bScaleCoeffs = 1;
		hSource->hGfxFeeder->stNewCfgInfo.eScaleCoeffs = eCoeffs;
	}
	else
	{
		BDBG_ERR(("Bad eCoeffs."));
		eStatus = BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_LEAVE(BVDC_Source_SetHorizontalScaleCoeffs);
	return eStatus;
}

#define  GFD_MAX_NUM_GAMMA_T_ENTR     256

/***************************************************************************
*
*/
BERR_Code BVDC_Source_EnableGammaCorrection
	( BVDC_Source_Handle               hSource,
	  uint32_t                         ulNumEntries,
	  const uint32_t                  *pulGammaTable )
{
	BERR_Code  eStatus = BERR_SUCCESS;
	uint32_t  ulClutOffset;

	BDBG_ENTER(BVDC_Source_EnableGammaCorrection);

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if( 256 == ulNumEntries )
	{
		BDBG_OBJECT_ASSERT(hSource->hGfxFeeder, BVDC_GFX);

#if (BVDC_P_SUPPORT_GFD_VER_0 == BVDC_P_SUPPORT_GFD1_VER)
		if (BAVC_SourceId_eGfx1 == hSource->hGfxFeeder->eId)
		{
			return BERR_TRACE(BVDC_ERR_GFX_UNSUPPORTED_GAMMATABLE);
		}
#endif

		if( ulNumEntries <= GFD_MAX_NUM_GAMMA_T_ENTR )
		{
			eStatus = BMEM_Heap_ConvertAddressToOffset( hSource->hGfxFeeder->hMemory,
				(void *) pulGammaTable,
				&ulClutOffset );
			if ( BERR_SUCCESS == eStatus )
			{
				/* user might use the same clut buf, but change the content */
				hSource->hGfxFeeder->stNewCfgInfo.stDirty.stBits.bGammaTable = 1;
				hSource->hGfxFeeder->stNewCfgInfo.stFlags.bEnableGammaCorrection = 1;
				hSource->hGfxFeeder->stNewCfgInfo.ulNumGammaClutEntries  = ulNumEntries;
				hSource->hGfxFeeder->stNewCfgInfo.ulGammaClutAddress     = ulClutOffset;
			}
			else
			{
				BDBG_ERR(("Gamma table buf not Alloced by BMEM."));
				eStatus = BERR_TRACE(BERR_INVALID_PARAMETER);
			}
		}
		else
		{
			BDBG_ERR(("Bad ulNumEntries (1)."));
			eStatus = BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}
	else
	{
		BDBG_ERR(("Bad ulNumEntries (2)."));
		eStatus = BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_LEAVE(BVDC_Source_EnableGammaCorrection);
	return eStatus;
}


/***************************************************************************
*
*/
BERR_Code BVDC_Source_DisableGammaCorrection
	( BVDC_Source_Handle               hSource )
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Source_DisableGammaCorrection);

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hSource->hGfxFeeder, BVDC_GFX);

	hSource->hGfxFeeder->stNewCfgInfo.stFlags.bEnableGammaCorrection = 0;

	BDBG_LEAVE(BVDC_Source_DisableGammaCorrection);
	return eStatus;
}


/***************************************************************************
*
*/
BERR_Code BVDC_Source_SetConstantColor
	( BVDC_Source_Handle               hSource,
	  uint8_t                          ucRed,
	  uint8_t                          ucGreen,
	  uint8_t                          ucBlue )
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Source_SetConstantColor);

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hSource->hGfxFeeder, BVDC_GFX);

	hSource->hGfxFeeder->stNewCfgInfo.stDirty.stBits.bConstantColor = 1;
	hSource->hGfxFeeder->stNewCfgInfo.ulConstantColor =
		(ucRed << 16) | (ucGreen << 8) | (ucBlue);

	BDBG_LEAVE(BVDC_Source_SetConstantColor);
	return eStatus;
}

#ifdef BVDC_P_SUPPORT_OLD_SET_ALPHA_SUR		

/***************************************************************************
*
*/
BERR_Code BVDC_Source_SetAlphaSurface
	( BVDC_Source_Handle   hSource,
	  BSUR_Surface_Handle  hSurface,
	  uint8_t              ucW0Alpha,
	  uint8_t              ucW1Alpha )
{
	BERR_Code  eStatus = BERR_SUCCESS;
	BAVC_Gfx_Picture  *pAvcGfxPic;

	BDBG_ENTER(BVDC_Source_SetAlphaSurface);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pAvcGfxPic = &hSource->hGfxFeeder->stTmpNewAvcPic;
	pAvcGfxPic->hAlphaSurface = hSurface;
	pAvcGfxPic->ucW0Alpha = ucW0Alpha;
	pAvcGfxPic->ucW1Alpha = ucW1Alpha;

	BKNI_EnterCriticalSection();
	BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
	hSource->hVdc->ulInsideCs ++;

	if (NULL != hSource->hGfxFeeder)
	{
		BVDC_P_GfxSurface_SetSurface_isr(
			&hSource->hGfxFeeder->stGfxSurface,
			&hSource->hGfxFeeder->stGfxSurface.stNewSurInfo, pAvcGfxPic);
	}
	else
	{
		BDBG_ERR(("Invalid source handle to set gfx surface."));
		eStatus = BERR_INVALID_PARAMETER;
	}

	hSource->hVdc->ulInsideCs --;
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVDC_Source_SetAlphaSurface);
	return BERR_TRACE(eStatus);
}

/***************************************************************************
*
*/
BERR_Code BVDC_Source_SetAlphaSurface_isr
	( BVDC_Source_Handle   hSource,
	  BSUR_Surface_Handle  hSurface,
	  uint8_t              ucW0Alpha,
	  uint8_t              ucW1Alpha )
{
	BERR_Code  eStatus = BERR_SUCCESS;
	BAVC_Gfx_Picture  *pAvcGfxPic;

	BDBG_ENTER(BVDC_Source_SetAlphaSurface_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pAvcGfxPic = &hSource->hGfxFeeder->stTmpIsrAvcPic;
	pAvcGfxPic->hAlphaSurface = hSurface;
	pAvcGfxPic->ucW0Alpha = ucW0Alpha;
	pAvcGfxPic->ucW1Alpha = ucW1Alpha;

	BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
	hSource->hVdc->ulInsideCs ++;

	if (NULL != hSource->hGfxFeeder)
	{
		BVDC_P_GfxSurface_SetSurface_isr(
			&hSource->hGfxFeeder->stGfxSurface,
			&hSource->hGfxFeeder->stGfxSurface.stIsrSurInfo, pAvcGfxPic);
	}
	else
	{
		BDBG_ERR(("Invalid source handle to set gfx surface."));
		eStatus = BERR_INVALID_PARAMETER;
	}

	hSource->hVdc->ulInsideCs --;

	BDBG_LEAVE(BVDC_Source_SetAlphaSurface_isr);
	return BERR_TRACE(eStatus);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetAlphaSurface
	( BVDC_Source_Handle    hSource,
	  BSUR_Surface_Handle  *phSurface )
{
	BERR_Code  eStatus = BERR_SUCCESS;
	BAVC_Gfx_Picture  *pGfxPic;

	BDBG_ENTER(BVDC_Source_GetAlphaSurface);

	if(NULL != phSurface)
	{
		BKNI_EnterCriticalSection();
		BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
		hSource->hVdc->ulInsideCs ++;

		if (NULL != hSource->hGfxFeeder)
		{
			pGfxPic = BVDC_P_GfxSurface_GetSurfaceInHw_isr(
				&hSource->hGfxFeeder->stGfxSurface);
			*phSurface = pGfxPic->hAlphaSurface;
		}
		else
		{
			BDBG_ERR(("Invalid source handle to get gfx surface."));
			eStatus = BERR_INVALID_PARAMETER;
		}

		hSource->hVdc->ulInsideCs--;
		BKNI_LeaveCriticalSection();		
	}

	BDBG_LEAVE(BVDC_Source_GetAlphaSurface);
	return BERR_TRACE(eStatus);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetAlphaSurface_isr
	( BVDC_Source_Handle    hSource,
	  BSUR_Surface_Handle  *phSurface )
{
	BERR_Code  eStatus = BERR_SUCCESS;
	BAVC_Gfx_Picture  *pGfxPic;

	BDBG_ENTER(BVDC_Source_GetAlphaSurface_isr);

	if(NULL != phSurface)
	{
		BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
		hSource->hVdc->ulInsideCs ++;

		if (NULL != hSource->hGfxFeeder)
		{
			pGfxPic = BVDC_P_GfxSurface_GetSurfaceInHw_isr(
				&hSource->hGfxFeeder->stGfxSurface);
			*phSurface = pGfxPic->hAlphaSurface;
		}
		else
		{
			BDBG_ERR(("Invalid source handle to get gfx surface."));
			eStatus = BERR_INVALID_PARAMETER;
		}

		hSource->hVdc->ulInsideCs--;
	}

	BDBG_LEAVE(BVDC_Source_GetAlphaSurface_isr);
	return BERR_TRACE(eStatus);
}

#endif /* #ifdef BVDC_P_SUPPORT_OLD_SET_ALPHA_SUR */

/* End of File */
