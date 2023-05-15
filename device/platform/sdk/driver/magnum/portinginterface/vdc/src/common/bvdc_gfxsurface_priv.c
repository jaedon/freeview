/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_gfxsurface_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 6/14/12 3:43p $
 *
 * Module Description:
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_gfxsurface_priv.c $
 * 
 * Hydra_Software_Devel/7   6/14/12 3:43p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 * 
 * Hydra_Software_Devel/6   6/8/12 2:53p syang
 * SW7435-227: P0 / A0 format gfx surface OK to have width/pitch 0
 * 
 * Hydra_Software_Devel/5   5/23/12 6:46p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 * 
 * Hydra_Software_Devel/4   5/9/12 2:55p syang
 * SW7425-2093:  fix compile error for diff chips
 * 
 * Hydra_Software_Devel/1   5/9/12 10:48a syang
 * SW7425-2093: abstract out gfx surface managing code into separate
 * submodule, to be shared by gfd and vfd
 * 
 ***************************************************************************/

#include "bvdc_gfxsurface_priv.h"


BDBG_MODULE(BVDC_GFXSUR);
BDBG_OBJECT_ID(BVDC_GFXSUR);


#define BVDC_P_SUR_RETURN_IF_ERR(result) \
	if ( BERR_SUCCESS != (result)) \
{\
	return BERR_TRACE(result);  \
}

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxSurface_AllocShadowRegs
 *
 * Called by GfxFeeder or VfdFeeder to allocate surface address shadow
 * registers from scratch pool, 
 */
BERR_Code BVDC_P_GfxSurface_AllocShadowRegs
	( BVDC_P_GfxSurfaceContext        *pGfxSurface,
	  BRDC_Handle                      hRdc,
	  bool                             b3dSrc )
{
	BDBG_ASSERT(NULL != pGfxSurface);
	BDBG_ASSERT(NULL != hRdc);

	pGfxSurface->b3dSrc = b3dSrc;

	if (0 == pGfxSurface->ulSurAddrReg[0])
	{
		BDBG_ASSERT(0 == pGfxSurface->ulSurAddrReg[1] &&
					0 == pGfxSurface->ulRSurAddrReg[0] &&
					0 == pGfxSurface->ulRSurAddrReg[1] &&
					0 == pGfxSurface->ulRegIdxReg &&
					0 == pGfxSurface->ulVsyncCntrReg);
					
		pGfxSurface->ulVsyncCntrReg = BRDC_AllocScratchReg(hRdc);
		pGfxSurface->ulSurAddrReg[0] = BRDC_AllocScratchReg(hRdc);
		if (b3dSrc)
		{
			pGfxSurface->ulSurAddrReg[1] = BRDC_AllocScratchReg(hRdc);
			pGfxSurface->ulRSurAddrReg[0] = BRDC_AllocScratchReg(hRdc);
			pGfxSurface->ulRSurAddrReg[1] = BRDC_AllocScratchReg(hRdc);
			pGfxSurface->ulRegIdxReg = BRDC_AllocScratchReg(hRdc);
		}
	}

	if ((!b3dSrc && (!pGfxSurface->ulSurAddrReg[0] || !pGfxSurface->ulVsyncCntrReg)) ||
		( b3dSrc && (!pGfxSurface->ulSurAddrReg[0] || !pGfxSurface->ulSurAddrReg[1] ||
					!pGfxSurface->ulRSurAddrReg[0] || !pGfxSurface->ulRSurAddrReg[1] ||
					!pGfxSurface->ulRegIdxReg || !pGfxSurface->ulVsyncCntrReg)))
	{
		BDBG_ERR(("Not enough scratch registers for gfx surface feeder[%d]",
				  pGfxSurface->eSrcId));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	return BERR_SUCCESS;
}

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxSurface_FreeShadowRegs
 *
 * Called to fee surface address shadow registers back to scratch pool, 
 * when the shadow registers are no longer needed.
 */
BERR_Code BVDC_P_GfxSurface_FreeShadowRegs
	( BVDC_P_GfxSurfaceContext        *pGfxSurface,
	  BRDC_Handle                      hRdc )
{
	if (pGfxSurface->ulSurAddrReg[0])
	{
		BDBG_ASSERT(0 != pGfxSurface->ulVsyncCntrReg);
		BRDC_FreeScratchReg(hRdc, pGfxSurface->ulSurAddrReg[0]);
		BRDC_FreeScratchReg(hRdc, pGfxSurface->ulVsyncCntrReg);

		if (pGfxSurface->b3dSrc)
		{
			BDBG_ASSERT(0 != pGfxSurface->ulSurAddrReg[1] &&
						0 != pGfxSurface->ulRSurAddrReg[0] &&
						0 != pGfxSurface->ulRSurAddrReg[1] &&
						0 != pGfxSurface->ulRegIdxReg );		
			BRDC_FreeScratchReg(hRdc, pGfxSurface->ulSurAddrReg[1]);
			BRDC_FreeScratchReg(hRdc, pGfxSurface->ulRSurAddrReg[0]);
			BRDC_FreeScratchReg(hRdc, pGfxSurface->ulRSurAddrReg[1]);
			BRDC_FreeScratchReg(hRdc, pGfxSurface->ulRegIdxReg);
		}
		
		pGfxSurface->ulSurAddrReg[0] = 0;
		pGfxSurface->ulSurAddrReg[1] = 0;
		pGfxSurface->ulRSurAddrReg[0] = 0;
		pGfxSurface->ulRSurAddrReg[1] = 0;
		pGfxSurface->ulRegIdxReg = 0;
		pGfxSurface->ulVsyncCntrReg = 0;
	}
	return BERR_SUCCESS;
}

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxSurface_Init
 *
 * Called by BVDC_P_GfxFeeder_Init or BVDC_P_Feeder_Init when a source
 * handle is created with BVDC_Source_Created
 */
BERR_Code BVDC_P_GfxSurface_Init
	( BVDC_P_GfxSurfaceContext        *pGfxSurface )
{
	BDBG_ASSERT(NULL != pGfxSurface);

	BDBG_ASSERT(0 != pGfxSurface->ulSurAddrReg[0] &&
				0 != pGfxSurface->ulVsyncCntrReg);
	BREG_Write32(pGfxSurface->hRegister, pGfxSurface->ulSurAddrReg[0],  0);
	BREG_Write32(pGfxSurface->hRegister, pGfxSurface->ulVsyncCntrReg,   1);

	if (pGfxSurface->b3dSrc)
	{
		BDBG_ASSERT(0 != pGfxSurface->ulSurAddrReg[1] &&
					0 != pGfxSurface->ulRSurAddrReg[0] &&
					0 != pGfxSurface->ulRSurAddrReg[1] &&
					0 != pGfxSurface->ulRegIdxReg );
		BREG_Write32(pGfxSurface->hRegister, pGfxSurface->ulSurAddrReg[1],  0);
		BREG_Write32(pGfxSurface->hRegister, pGfxSurface->ulRSurAddrReg[0], 0);
		BREG_Write32(pGfxSurface->hRegister, pGfxSurface->ulRSurAddrReg[1], 0);
		BREG_Write32(pGfxSurface->hRegister, pGfxSurface->ulRegIdxReg,      0);
	}	

	BKNI_Memset((void*)&pGfxSurface->stIsrSurInfo, 0x0, sizeof(BVDC_P_SurfaceInfo));
	BKNI_Memset((void*)&pGfxSurface->stNewSurInfo, 0x0, sizeof(BVDC_P_SurfaceInfo));
	BKNI_Memset((void*)&pGfxSurface->stCurSurInfo, 0x0, sizeof(BVDC_P_SurfaceInfo));
	
	pGfxSurface->ucNodeIdx = 0;
	BKNI_Memset((void*)&pGfxSurface->stSurNode[0], 0x0, 4 * sizeof(BVDC_P_GfxSurNode));
	
	return BERR_SUCCESS;
}

/*------------------------------------------------------------------------
 *  {secret}
 *  BVDC_P_GfxSurface_SetSurface_isr
 *
 *  It will check the BAVC_Gfx_Picture struct pointed by pAvcGfxPic to ensure
 *  that there is no conflict inside itself. It will then program the surface
 *  address shadow registers if size / pitch and format match the current. 
 *  Otherwise it will mark the surface as "changed" so that ApplyChange will 
 *  validate it, and then the next rul build will program it into RUL
 */
BERR_Code BVDC_P_GfxSurface_SetSurface_isr 
	( BVDC_P_GfxSurfaceContext        *pGfxSurface,
	  BVDC_P_SurfaceInfo              *pSurInfo,
	  const BAVC_Gfx_Picture          *pAvcGfxPic )
{
	BERR_Code  eResult;
	BSUR_Surface_Handle  hSurface;
	void                *pvAddress;
	uint32_t             ulSurOffset;
	uint32_t             ulPitch;
	uint32_t             ulWidth;
	uint32_t             ulHeight;
	BPXL_Format          ePxlFmt;
	uint32_t             ulRSurOffset;
#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
	uint32_t             ulAlphaOffset;
	uint32_t             ulAlphaPitch;
	BSUR_Surface_Handle  hAlphaSurface;
	uint32_t             ulAlphaWidth;
	uint32_t             ulAlphaHeight;
	BPXL_Format          eAlphaPxlFmt;
#elif (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_4)
	BSUR_Surface_Handle  hRSurface;
	uint32_t             ulRPitch;
	uint32_t             ulRWidth;
	uint32_t             ulRHeight;
	BPXL_Format          eRPxlFmt;
#endif
	uint32_t             ulPltOffset;
	BSUR_Palette_Handle  hPalette;
	uint32_t             ulPaletteNumEntries;
	BPXL_Format          ePaletteEntryFormat;
	
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_GFD)
	BSUR_TestFeature1_Settings  stTestFeature;
#endif
	
	BVDC_P_SurfaceInfo  *pCurSurInfo = &pGfxSurface->stCurSurInfo;
		  
	/* get info from main surface */
	hSurface = pAvcGfxPic->hSurface;
	eResult = BSUR_Surface_GetAddress( hSurface, &pvAddress, &ulPitch );
	eResult |= BSUR_Surface_GetDimensions( hSurface, &ulWidth, &ulHeight );
	eResult |= BSUR_Surface_GetOffset( hSurface, &ulSurOffset );
	eResult |= BSUR_Surface_GetFormat( hSurface, &ePxlFmt );
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_GFD)
	eResult |= BSUR_Surface_GetTestFeature1( hSurface, &stTestFeature );
#endif
	BVDC_P_SUR_RETURN_IF_ERR( eResult );

	if (BPXL_eP0==ePxlFmt || BPXL_eA0==ePxlFmt)
	{
		/* to make later logic simple */
		ulPitch = (ulPitch)? ulPitch : 1;
		pvAddress = (pvAddress)? pvAddress : (void *)0x1;
		ulSurOffset = (ulSurOffset)? ulSurOffset : 0x1;
	}
		
	if (0==ulWidth || 0==ulHeight || 0==ulPitch || NULL==pvAddress)
	{
		BDBG_ERR(("ulWidth %d, ulHeight %d, ulPitch %d, pvAddress 0x%x",
				  ulWidth, ulHeight, ulPitch, pvAddress));
		return BERR_TRACE(BVDC_ERR_GFX_SUR_SIZE_MISMATCH);
	}
	
#if (BVDC_P_SUPPORT_GFD_VER_0 == BVDC_P_SUPPORT_GFD1_VER)
	if ((BAVC_SourceId_eGfx1 == pGfxSurface->eSrcId) &&
		(!BPXL_IS_YCbCr_FORMAT(ePxlFmt)) &&
		(BPXL_eP1 != ePxlFmt) && (BPXL_eP2 != ePxlFmt) && (BPXL_eP4 != ePxlFmt))
	{
		return BERR_TRACE(BVDC_ERR_GFX_SUR_FMT_MISMATCH);
	}
#endif

	ulRSurOffset = 0; /* mark for 2D case */

#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
	ulAlphaOffset = 0; /* mark for no alpha surface case */
	ulAlphaPitch = 0;
	
	/* check alpha surface, OK to have NULL hAlphaSurface */
	if (pAvcGfxPic->hAlphaSurface && BVDC_P_SRC_IS_GFX(pGfxSurface->eSrcId))
	{
		hAlphaSurface = pAvcGfxPic->hAlphaSurface;
		eResult = BSUR_Surface_GetAddress( hAlphaSurface, &pvAddress, &ulAlphaPitch );
		eResult |= BSUR_Surface_GetOffset( hAlphaSurface, &ulAlphaOffset );
		eResult |= BSUR_Surface_GetDimensions( hAlphaSurface, &ulAlphaWidth, &ulAlphaHeight );
		if (ulWidth != ulAlphaWidth || ulHeight != ulAlphaHeight) 
		{
			return BERR_TRACE(BVDC_ERR_GFX_SUR_SIZE_MISMATCH);
		}

		eResult |= BSUR_Surface_GetFormat( hAlphaSurface, &eAlphaPxlFmt );
		BVDC_P_SUR_RETURN_IF_ERR( eResult );
		if ((BPXL_eW1 != eAlphaPxlFmt) ||
			(BPXL_eB5_G6_R5 != ePxlFmt && BPXL_eR5_G6_B5 != ePxlFmt))
		{
			return BERR_TRACE(BVDC_ERR_GFX_SUR_FMT_MISMATCH);
		}
	}

#elif (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_4)
	/* check right surface */
	if (BVDC_P_ORIENTATION_IS_3D(pAvcGfxPic->eInOrientation))
	{
		/* since orientation is 3D, hRSurface must be valid */
		hRSurface = pAvcGfxPic->hRSurface;
		eResult = BSUR_Surface_GetAddress( hRSurface, &pvAddress, &ulRPitch );	
		eResult |= BSUR_Surface_GetDimensions( hRSurface, &ulRWidth, &ulRHeight );
		eResult |= BSUR_Surface_GetOffset( hRSurface, &ulRSurOffset );
		eResult |= BSUR_Surface_GetFormat( hRSurface, &eRPxlFmt );
		BVDC_P_SUR_RETURN_IF_ERR( eResult );

		if (ulPitch != ulRPitch ||
			ulWidth != ulRWidth ||
			ulHeight != ulRHeight ||
			ePxlFmt != eRPxlFmt)
		{
			return BERR_INVALID_PARAMETER;
		}
	}
#endif
	
	/* handle palette format */
	if ( BPXL_IS_PALETTE_FORMAT(ePxlFmt) )
	{
		/* if it is palette input format, but hPalette is NULL, we assume
		 * the palette does not change. This allow double buffering palette
		 * surface without loading palette for every buffer swiching.
		 */
		eResult = BSUR_Surface_GetPalette( hSurface, &hPalette );
		BVDC_P_SUR_RETURN_IF_ERR( eResult );
		if ( NULL != hPalette )
		{
			eResult = BSUR_Palette_GetOffset( hPalette, &ulPltOffset );
			eResult |= BSUR_Palette_GetNumEntries ( hPalette, &ulPaletteNumEntries );
			eResult |= BSUR_Palette_GetFormat( hPalette, &ePaletteEntryFormat );
			BVDC_P_SUR_RETURN_IF_ERR( eResult );

			/* no more error check */
			pSurInfo->ulPaletteAddress = ulPltOffset;
			pSurInfo->ulPaletteNumEntries = ulPaletteNumEntries;
			pSurInfo->eActivePxlFmt = ePaletteEntryFormat;
			pSurInfo->bChangePaletteTable = true;
		}
		else
		{
			/* no palette table change */
			pSurInfo->eActivePxlFmt = pCurSurInfo->eActivePxlFmt;
			pSurInfo->bChangePaletteTable = false; 
		}
	}
	else if (false == (BPXL_IS_ALPHA_ONLY_FORMAT(ePxlFmt)))
	{
		pSurInfo->eActivePxlFmt = ePxlFmt;
		pSurInfo->bChangePaletteTable = false; 
	}
	else /* alpha only format */
	{
		pSurInfo->eActivePxlFmt = BPXL_eA8_R8_G8_B8;
		pSurInfo->bChangePaletteTable = false; 
	}

	/* store surface info
	 */
	pSurInfo->stAvcPic = *pAvcGfxPic;
	pSurInfo->ulAddress = ulSurOffset;
	pSurInfo->ulPitch   = ulPitch;
	pSurInfo->ulWidth   = ulWidth;
	pSurInfo->ulHeight  = ulHeight;
	pSurInfo->eInputPxlFmt = ePxlFmt;
	pSurInfo->ulRAddress = ulRSurOffset;
#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
	pSurInfo->ulAlphaAddress = ulAlphaOffset;
	pSurInfo->ulAlphaPitch   = ulAlphaPitch;
#endif
	
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_GFD)
	pSurInfo->stTestFeature1 = stTestFeature;
#endif
	
	pSurInfo->bChangeSurface = true; /* inform ApplyChange */

	/* set this surface setting to shadow register if we can
	 */
	if ((ulWidth == pCurSurInfo->ulWidth) &&
		(ulHeight == pCurSurInfo->ulHeight) &&
		(ulPitch == pCurSurInfo->ulPitch) &&
		(ePxlFmt == pCurSurInfo->eInputPxlFmt) &&
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_GFD)
		(stTestFeature.bEnable == pCurSurInfo->stTestFeature1.bEnable) &&
		(stTestFeature.ulBitsPerPixel == pCurSurInfo->stTestFeature1.ulBitsPerPixel) &&
		(stTestFeature.ulPredictionMode == pCurSurInfo->stTestFeature1.ulPredictionMode) &&
#endif
		(!pSurInfo->bChangePaletteTable))
	{
		BVDC_P_GfxSurface_SetShadowRegs_isr(pGfxSurface, pSurInfo);

		/* surface is already in use */
		pSurInfo->bChangeSurface = false; 
		*pCurSurInfo = *pSurInfo;
	}

	return eResult;
}

/*-------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxSurface_SetShadowRegs_isr
 *
 * set the surface addr to the shadow registers.
 * Called from BVDC_Source_SetSurface or BuildRul after ApplyChange
 */
void BVDC_P_GfxSurface_SetShadowRegs_isr
	( BVDC_P_GfxSurfaceContext        *pGfxSurface,
	  BVDC_P_SurfaceInfo              *pSurInfo )
{
	BREG_Handle  hRegister;
	BVDC_P_GfxSurNode  *pNode;	
	uint32_t  ulVsyncCntr1, ulVsyncCntr2;
	int iNodeIdx;
	uint32_t  ulSurAddr, ulRSurAddr, ulRegIdx;

	hRegister = pGfxSurface->hRegister;

	/* pre-read of ulVsyncCntrReg before setting shadow registers.
	 * note: when RUL is executed it will increase the value in ulVsyncCntrReg by 1
	 */
	ulVsyncCntr1 = BREG_Read32_isr(hRegister, pGfxSurface->ulVsyncCntrReg);
	if (0 == ulVsyncCntr1)
	{
		/* we could see this once after 828.5 days of running */
		BDBG_MSG(("We see Vsync cntr rapped back to 0"));
	}

	/* set surface addr to shadow registers
	 * note: RUL will add pitch for bottom field
	 */
	ulSurAddr = pSurInfo->ulAddress + pGfxSurface->ulMainByteOffset;
	if (pGfxSurface->b3dSrc)
	{
#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
		ulRSurAddr = pSurInfo->ulAlphaAddress + pGfxSurface->ulAlphaByteOffset;
#else
		ulRSurAddr = pSurInfo->ulRAddress + pGfxSurface->ulMainByteOffset;		
#endif
		ulRegIdx = ~ pGfxSurface->ulRegIdx;
		BREG_Write32_isr(hRegister, pGfxSurface->ulSurAddrReg[ulRegIdx & 1], ulSurAddr);
		BREG_Write32_isr(hRegister, pGfxSurface->ulRSurAddrReg[ulRegIdx & 1], ulRSurAddr);
		BREG_Write32_isr(hRegister, pGfxSurface->ulRegIdxReg, ulRegIdx);
		pGfxSurface->ulRegIdx = ulRegIdx;
	}
	else
	{
		BREG_Write32_isr(hRegister, pGfxSurface->ulSurAddrReg[0], ulSurAddr);
		ulRSurAddr = 0;
	}
	
	/* post-read of ulVsyncCntrReg after setting shadow registers.
	 */
	ulVsyncCntr2 = BREG_Read32_isr(hRegister, pGfxSurface->ulVsyncCntrReg);
	   
	/* choose a node to record this sur setting */
	iNodeIdx = pGfxSurface->ucNodeIdx;
	if ((pGfxSurface->stSurNode[iNodeIdx].ulVsyncCntr != ulVsyncCntr1) ||
		(pGfxSurface->stSurNode[iNodeIdx].bExeDuringSet))
	{
		/* current node might have be executed, so we use next node */
		iNodeIdx = (iNodeIdx + 1) & 0x3;
		pGfxSurface->ucNodeIdx = iNodeIdx;
	}
	pNode = &pGfxSurface->stSurNode[iNodeIdx];

	/* record this setting to node */
	pNode->stAvcPic = pSurInfo->stAvcPic;
	pNode->ulAddr = ulSurAddr;
	pNode->ulPitch = pSurInfo->ulPitch;
	pNode->ulRAddr = ulRSurAddr;
	pNode->ulVsyncCntr = ulVsyncCntr2;  /* post read */
	pNode->bExeDuringSet = (ulVsyncCntr2 != ulVsyncCntr1);

	return;
}

/*------------------------------------------------------------------------
 *  {private}
 *	BVDC_P_GfxSurface_GetSurfaceInHw_isr
 *
 *  Read HW registers to decide which picture node the HW is using.
 */
BAVC_Gfx_Picture *BVDC_P_GfxSurface_GetSurfaceInHw_isr
	( BVDC_P_GfxSurfaceContext        *pGfxSurface )
{
	BVDC_P_GfxSurNode  *pNode;	
	uint32_t  ulVsyncCntr, ulAddr;
	int ii, iNodeIdx;

	/* when RUL is executed it will increase the value in ulVsyncCntrReg by 1 */
	ulVsyncCntr = BREG_Read32_isr(pGfxSurface->hRegister, pGfxSurface->ulVsyncCntrReg);
	
	iNodeIdx = pGfxSurface->ucNodeIdx;
	for (ii=0; ii<4; ii++)
	{
		/* "&& pNode->ulAddr" here is for the first 4 surface setting after start */
		pNode = &pGfxSurface->stSurNode[iNodeIdx];
		if (ulVsyncCntr != pNode->ulVsyncCntr && pNode->ulAddr)
		{
			/* at least one vsync passed after the setting of this node */
			return &pNode->stAvcPic;
		}
		else 
		{
			if (pNode->bExeDuringSet)
			{
				ulAddr = BREG_Read32_isr(pGfxSurface->hRegister, pGfxSurface->ulHwMainSurAddrReg);
				if ((0 != ulAddr) &&
					(ulAddr == pNode->ulAddr || /* top field or frame */
					 ulAddr == pNode->ulAddr + pNode->ulPitch || /* bottom field */
					 ulAddr == pNode->ulRAddr || /* left/right swapped top field or frame */
					 ulAddr == pNode->ulRAddr + pNode->ulPitch)) /* left/right swapped bottom */
				{
					/* left sur and right sur always update together, so we only
					 * need to check main sur addr */
					return &pNode->stAvcPic;
				}
			}
			/* else this node is definately not used by HW yet */
		}
		
		iNodeIdx = (iNodeIdx + 4 - 1) & 0x3;
	}

	/* we might be here at beginning only. return the current sur setting
	 * if no surface is set yet, the AvcPic returned could have all NULL surface handles */
	BDBG_MSG(("no node found, ulVsyncCntr=%d, srcId = %d", ulVsyncCntr, pGfxSurface->eSrcId));
	return &(pGfxSurface->stSurNode[pGfxSurface->ucNodeIdx].stAvcPic);
}

/* End of File */
