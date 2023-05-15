/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: native_magnum.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 5/11/12 2:38p $
 *
 * Module Description: Native entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/native_magnum.c $
 * 
 * Hydra_Software_Devel/10   5/11/12 2:38p nissen
 * SW7125-1283: Fixed P3D memory leak when closing.
 * 
 * Hydra_Software_Devel/9   1/17/12 3:56p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/8   8/14/10 3:03p nissen
 * SW7400-2378: Added open of P3D and GRC modules during init.
 * 
 * Hydra_Software_Devel/7   7/4/09 5:27p nissen
 * PR 56587: Added blit functions definitions.
 * 
 * Hydra_Software_Devel/6   11/15/07 8:28p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/5   11/11/05 4:23p nissen
 * PR 17423: Moved VDC code from native to app.
 * 
 * Hydra_Software_Devel/4   9/22/05 6:44p nissen
 * PR 17194: Updated FrameWork support.
 * 
 * Hydra_Software_Devel/3   9/20/05 10:14a jasonh
 * PR 17194: To work with new framework.
 * 
 * Hydra_Software_Devel/2   7/28/04 2:41p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "native_private.h"
#include "native_magnum.h"

/***************************************************************************/
BGRC_Handle g_hGrc;
BP3D_Handle g_hP3d;

/***************************************************************************/
void native_P_InitDisplay( 
	NativeDisplay display,
	BMEM_Handle *phMem,
	BP3D_Handle *phP3d )
{
	BFramework_Info *pFramework = (BFramework_Info *) display;
	BERR_Code err = BGRC_Open( &g_hGrc, pFramework->hChp, pFramework->hReg, pFramework->hMem, pFramework->hInt, NULL );
	if( err != BERR_SUCCESS )
	{
		BKNI_Printf( "BGRC_Open failed\n" );
		return;
	}
	err = BP3D_Open( &g_hP3d, pFramework->hChp, pFramework->hReg, pFramework->hMem, pFramework->hInt );
	if( err != BERR_SUCCESS )
	{
		BKNI_Printf( "BP3D_Open failed\n" );
		return;
	}

	*phMem = pFramework->hMem;
	*phP3d = g_hP3d;
}

/***************************************************************************/
void native_P_TermDisplay( 
	NativeDisplay display )
{
	BSTD_UNUSED( display );
	BP3D_Close(g_hP3d);
}

/***************************************************************************/
void native_P_InitWindow( 
	NativeWindow window,
	BSUR_Surface_Handle *phSurface,
	unsigned int *puiFormat,
	unsigned int *puiOffset,
	void **ppvMemory,
	int *piPitch,
	int *piWidth,
	int *piHeight )
{
	BSUR_Surface_Handle hSurface = (BSUR_Surface_Handle) window;

	BSUR_Surface_GetFormat( hSurface, (BPXL_Format *) puiFormat );
	BSUR_Surface_GetOffset( hSurface, puiOffset );
	BSUR_Surface_GetAddress( hSurface, ppvMemory, piPitch );
	BSUR_Surface_GetDimensions( hSurface, (uint32_t *) piWidth, (uint32_t *) piHeight );
	*phSurface = hSurface;
}

/***************************************************************************/
void native_P_InitPixmap( 
	NativePixmap pixmap,
	BSUR_Surface_Handle *phSurface,
	unsigned int *puiFormat,
	unsigned int *puiOffset,
	void **ppvMemory,
	int *piPitch,
	int *piWidth,
	int *piHeight )
{
	BSUR_Surface_Handle hSurface = (BSUR_Surface_Handle) pixmap;

	BSUR_Surface_GetFormat( hSurface, (BPXL_Format *) puiFormat );
	BSUR_Surface_GetOffset( hSurface, puiOffset );
	BSUR_Surface_GetAddress( hSurface, ppvMemory, piPitch );
	BSUR_Surface_GetDimensions( hSurface, (uint32_t *) piWidth, (uint32_t *) piHeight );
	*phSurface = hSurface;
}

/***************************************************************************/
void native_P_CopySurface( 
	NativeDisplay display,
	BSUR_Surface_Handle hDstSurface,
	BSUR_Surface_Handle hSrcSurface )
{
	BGRC_ResetState( g_hGrc );
	BGRC_Source_SetFilterCoeffs( g_hGrc, BGRC_FilterCoeffs_eBilinear, BGRC_FilterCoeffs_eBilinear );
	BGRC_Source_SetSurface( g_hGrc, hSrcSurface );
	BGRC_Output_SetSurface( g_hGrc, hDstSurface );
	BGRC_IssueStateAndWait( g_hGrc );
}

/***************************************************************************/
void native_P_BlitSurface( 
	EGL_P_Context *pContext,
	NativeDisplay display,
	BSUR_Surface_Handle hDstSurface,
	int iDstX, int iDstY, int iDstWidth, int iDstHeight,
	BSUR_Surface_Handle hSrcSurface,
	int iSrcX, int ifSrcY, int iSrcWidth, int iSrcHeight,
	float fScaleX, float fScaleY, 
	float fPhaseX, float fPhaseY, 
	uint32_t ulColor,
	GLboolean bSame )
{
}

/***************************************************************************/
void native_P_BlitWait( 
	EGL_P_Context *pContext,
	NativeDisplay display,
	BSUR_Surface_Handle hSurface )
{
}

/* End of File */
