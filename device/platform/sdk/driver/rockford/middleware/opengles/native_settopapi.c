/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: native_settopapi.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 9/23/08 6:48p $
 *
 * Module Description: Native entry point functions for the Settop API.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/native_settopapi.c $
 * 
 * Hydra_Software_Devel/8   9/23/08 6:48p nissen
 * PR 46638: Optimized rectangle blits.
 * 
 * Hydra_Software_Devel/7   9/8/08 9:12p nissen
 * PR 46638: Added support for waiting for blits.
 * 
 * Hydra_Software_Devel/6   9/8/08 3:18p nissen
 * PR 46638: Added blit function to support 2D rectangle polygon blits.
 * 
 * Hydra_Software_Devel/5   11/15/07 8:35p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/4   11/10/06 12:45p nissen
 * PR 20908: Added support for offscreen back buffer (quadruple
 * buffering).
 * 
 * Hydra_Software_Devel/3   10/27/06 11:32a nissen
 * PR 12814: Added support for building  for 7400.
 * 
 * Hydra_Software_Devel/2   3/21/06 10:30a nissen
 * PR 19915: Changed location of grc module handle.
 * 
 * Hydra_Software_Devel/1   11/17/05 11:08p nissen
 * PR 17423: Native Settop API.
 * 
 ***************************************************************************/

#include "native_private.h"
#include "native_settopapi.h"

/***************************************************************************/
void native_P_InitDisplay( 
	NativeDisplay display,
	BMEM_Handle *phMem,
	BP3D_Handle *phP3d )
{
	BSTD_UNUSED( display );

#if (BCHP_CHIP==7400)
	*phMem = b_board.b7400.mem;
#else
	*phMem = b_board.b7038.mem;
#endif

	*phP3d = b_root.video.p3d;
}

/***************************************************************************/
void native_P_InitWindow( 
	NativeWindow window,
	BSUR_Surface_Handle *phSurface,
	int *piWidth,
	int *piHeight )
{
	bsurface_t bsurface = (bsurface_t) window;
	bsurface_settings bsettings;

	bsurface_get( bsurface, &bsettings );

	*phSurface = bsurface->surface;
	*piWidth = bsettings.position.width;
	*piHeight = bsettings.position.height;
}

/***************************************************************************/
void native_P_InitPixmap( 
	NativePixmap pixmap,
	BSUR_Surface_Handle *phSurface,
	int *piWidth,
	int *piHeight )
{
	bsurface_t bsurface = (bsurface_t) pixmap;
	bsurface_settings bsettings;

	bsurface_get( bsurface, &bsettings );

	*phSurface = bsurface->surface;
	*piWidth = bsettings.position.width;
	*piHeight = bsettings.position.height;
}

/***************************************************************************/
void native_P_CopySurface( 
	NativeDisplay display,
	BSUR_Surface_Handle hDstSurface,
	BSUR_Surface_Handle hSrcSurface )
{
	bgraphics_t bgraphics = (bgraphics_t) display;

	BGRC_ResetState( bgraphics->grc );
	BGRC_Source_SetFilterCoeffs( bgraphics->grc, BGRC_FilterCoeffs_eBilinear, BGRC_FilterCoeffs_eBilinear );
	BGRC_Source_SetSurface( bgraphics->grc, hSrcSurface );
	BGRC_Output_SetSurface( bgraphics->grc, hDstSurface );
	BGRC_Output_SetColorKeySelection( bgraphics->grc, 
		BGRC_Output_ColorKeySelection_eTakeSource, 
		BGRC_Output_ColorKeySelection_eTakeSource, 
		BGRC_Output_ColorKeySelection_eTakeDestination, 
		BGRC_Output_ColorKeySelection_eTakeDestination );
	BGRC_IssueStateAndWait( bgraphics->grc );
}

/***************************************************************************/
void native_P_BlitSurface( 
	EGL_P_Context *pContext,
	NativeDisplay display,
	BSUR_Surface_Handle hDstSurface,
	int iDstX, int iDstY, int iDstWidth, int iDstHeight,
	BSUR_Surface_Handle hSrcSurface,
	int iSrcX, int iSrcY, int iSrcWidth, int iSrcHeight,
	float fPhaseX, float fPhaseY, 
	float fScaleX, float fScaleY, 
	uint32_t ulColor,
	GLboolean bSame )
{
	bgraphics_t bgraphics = (bgraphics_t) display;
	static uint32_t s_ulPrevColor;

	uint32_t ulDstX = (uint32_t) ((iDstWidth < 0) ? iDstX + iDstWidth : iDstX);
	uint32_t ulDstY = ((uint32_t) (iDstHeight < 0) ? iDstY + iDstHeight : iDstY);
	uint32_t ulDstWidth = (uint32_t) ((iDstWidth < 0) ? -iDstWidth : iDstWidth);
	uint32_t ulDstHeight = (uint32_t) ((iDstHeight < 0) ? -iDstHeight : iDstHeight);
 	uint32_t ulSrcX = (uint32_t) ((iSrcWidth < 0) ? iSrcX + iSrcWidth : iSrcX);
	uint32_t ulSrcY = (uint32_t) ((iSrcHeight < 0) ? iSrcY + iSrcHeight : iSrcY);
	uint32_t ulSrcWidth = (uint32_t) ((iSrcWidth < 0) ? -iSrcWidth : iSrcWidth);
	uint32_t ulSrcHeight = (uint32_t) ((iSrcHeight < 0) ? -iSrcHeight : iSrcHeight);

	/* scale */
	if( (iSrcWidth * iDstWidth > 0) && (iSrcHeight * iDstHeight > 0) )
	{
		int iPhaseX = (fPhaseX - iSrcX) * (1 << 10);
		int iPhaseY = (fPhaseY - iSrcY) * (1 << 10);
		int iFactorX = (1 << 10) / fScaleX;
		int iFactorY = (1 << 10) / fScaleY;

		BGRC_Source_SetFilterPhaseAdjustment( bgraphics->grc, iPhaseX, iPhaseY, 10 );
		BGRC_Source_SetFixedScaleFactor( bgraphics->grc, fScaleX * iFactorX, iFactorX, fScaleY * iFactorY, iFactorY );
	}
	else
	{
		BGRC_Source_SetFilterPhaseAdjustment( bgraphics->grc, 0, 0, 10 );
		BGRC_Source_SetFixedScaleFactor( bgraphics->grc, 0, 0, 0, 0 );
	}

	/* fast blit */
	if( bSame && (s_ulPrevColor == ulColor) )
	{
		if( ulDstWidth && ulDstHeight && ((ulSrcWidth && ulSrcHeight) || (hSrcSurface == 0)) )
		{
			if( hSrcSurface )
				BGRC_Source_SetRectangle( bgraphics->grc, ulSrcX, ulSrcY, ulSrcWidth, ulSrcHeight );

			if( pContext->bBlend || pContext->bLogicOp )
				BGRC_Destination_SetRectangle( bgraphics->grc, ulDstX, ulDstY, ulDstWidth, ulDstHeight );

			BGRC_Output_SetRectangle( bgraphics->grc, ulDstX, ulDstY, ulDstWidth, ulDstHeight );
			BGRC_IssueState( bgraphics->grc, 0, 0 );
			pContext->bBlitBusy = GL_TRUE;
		}
		return;
	}
	s_ulPrevColor = ulColor;

	/* source */
	BGRC_Source_SetColor( bgraphics->grc, ulColor );
	BGRC_Source_SetSurface( bgraphics->grc, hSrcSurface );

	if( hSrcSurface )
	{
		/* filter */
		BGRC_FilterCoeffs eFilter = (
			(pContext->pTexture->eTextureMinFilter == GL_LINEAR) || (pContext->pTexture->eTextureMinFilter == GL_NEAREST_MIPMAP_LINEAR) || 
			(pContext->pTexture->eTextureMagFilter == GL_LINEAR) || (pContext->pTexture->eTextureMinFilter == GL_LINEAR_MIPMAP_LINEAR))
			? BGRC_FilterCoeffs_eBilinear : BGRC_FilterCoeffs_ePointSample;
		BGRC_Source_SetFilterCoeffs( bgraphics->grc, eFilter, eFilter );

		/* color matrix */
		if( pContext->eTexEnvMode == GL_MODULATE )
		{
			pContext->aiTextureMatrix[ 0] = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColor, 2) << 2;
			pContext->aiTextureMatrix[ 6] = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColor, 1) << 2;
			pContext->aiTextureMatrix[12] = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColor, 0) << 2;
			pContext->aiTextureMatrix[18] = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColor, 3) << 2;
			BGRC_Source_SetColorMatrix5x4( bgraphics->grc, pContext->aiTextureMatrix, 10 );
			BGRC_Source_ToggleColorMatrix( bgraphics->grc, true );
		}
		else if( pContext->eTexEnvMode == GL_ADD )
		{
			pContext->aiTextureMatrix[ 4] = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColor, 2) << 2;
			pContext->aiTextureMatrix[ 9] = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColor, 1) << 2;
			pContext->aiTextureMatrix[14] = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColor, 0) << 2;
			pContext->aiTextureMatrix[19] = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColor, 3) << 2;
			BGRC_Source_SetColorMatrix5x4( bgraphics->grc, pContext->aiTextureMatrix, 10 );
			BGRC_Source_ToggleColorMatrix( bgraphics->grc, true );
		}
		else
		{
			BGRC_Source_ToggleColorMatrix( bgraphics->grc, false );
		}
	
		/* dimensions */
		if( ulSrcWidth && ulSrcHeight )
			BGRC_Source_SetRectangle( bgraphics->grc, ulSrcX, ulSrcY, ulSrcWidth, ulSrcHeight );
		BGRC_Source_SetDirection( bgraphics->grc, iSrcWidth * iDstWidth < 0, iSrcHeight * iDstHeight < 0 );
	}
	else
	{
		BGRC_Source_ToggleColorMatrix( bgraphics->grc, false );
	}

	/* destination */
	if( pContext->bBlend || pContext->bLogicOp )
	{
		/* blend */
		if( pContext->bBlend )
		{
			BGRC_Blend_SetColorBlend( bgraphics->grc, BGRC_Blend_Source_eSourceColor, pContext->eGRCSrcBlendColor, false,
				BGRC_Blend_Source_eDestinationColor, pContext->eGRCDstBlendColor, false, BGRC_Blend_Source_eZero );
 			BGRC_Blend_SetAlphaBlend( bgraphics->grc, BGRC_Blend_Source_eSourceAlpha, pContext->eGRCSrcBlendAlpha, false,
				BGRC_Blend_Source_eDestinationAlpha, pContext->eGRCDstBlendAlpha, false, BGRC_Blend_Source_eZero );
 			BGRC_Blend_SetColor( bgraphics->grc, ulColor );
		}
		/* rop */
		else if( pContext->bLogicOp )
		{
			BGRC_Pattern_Set( bgraphics->grc, pContext->ucRop3, 0, 0, 0 );
		}

		/* surface and dimensions */
		if( ulDstWidth && ulDstHeight )
			BGRC_Destination_SetRectangle( bgraphics->grc, ulDstX, ulDstY, ulDstWidth, ulDstHeight );
		BGRC_Destination_SetSurface( bgraphics->grc, hDstSurface );
	}
	else
	{
		BGRC_Destination_SetSurface( bgraphics->grc, 0 );
	}

	/* output */
	if( ulDstWidth && ulDstHeight )
		BGRC_Output_SetRectangle( bgraphics->grc, ulDstX, ulDstY, ulDstWidth, ulDstHeight );
	BGRC_Output_SetSurface( bgraphics->grc, hDstSurface );
	BGRC_Output_SetDither( bgraphics->grc, pContext->bDither );

	BGRC_Output_SetColorKeySelection( bgraphics->grc, 
		pContext->bBlend ? BGRC_Output_ColorKeySelection_eTakeBlend : (
		pContext->bLogicOp ? BGRC_Output_ColorKeySelection_eTakePattern : 
		BGRC_Output_ColorKeySelection_eTakeSource), 
		BGRC_Output_ColorKeySelection_eTakeSource, 
		BGRC_Output_ColorKeySelection_eTakeDestination, 
		BGRC_Output_ColorKeySelection_eTakeDestination );

	/* blit */
	if( ulDstWidth && ulDstHeight && ((ulSrcWidth && ulSrcHeight) || (hSrcSurface == 0)) )
	{
		BGRC_IssueState( bgraphics->grc, 0, 0 );
		pContext->bBlitBusy = GL_TRUE;
	}
}

/***************************************************************************/
void native_P_BlitWait( 
	EGL_P_Context *pContext,
	NativeDisplay display,
	BSUR_Surface_Handle hSurface )
{
	bgraphics_t bgraphics = (bgraphics_t) display;

	if( pContext->bBlitBusy )
	{
		BGRC_ResetState( bgraphics->grc );
	 	BGRC_Source_SetSurface( bgraphics->grc, hSurface );
 		BGRC_Output_SetSurface( bgraphics->grc, hSurface );
		BGRC_Source_SetRectangle( bgraphics->grc, 0, 0, 2, 1 );
		BGRC_Output_SetRectangle( bgraphics->grc, 0, 0, 2, 1 );
		BGRC_IssueStateAndWait( bgraphics->grc );
		pContext->bBlitBusy = GL_FALSE;
	}
}

/* End of File */
