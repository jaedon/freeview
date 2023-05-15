/***************************************************************************
 *     Copyright (c) 2008-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cmd_graphics.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 8/25/11 10:45a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/cmd_graphics.c $
 * 
 * Hydra_Software_Devel/7   8/25/11 10:45a tdo
 * SW7420-2030: Add apply changes error tracking capability
 *
 * Hydra_Software_Devel/6   11/24/10 12:56p pntruong
 * SW7552-17: Added initial 7552 support.
 *
 * Hydra_Software_Devel/5   10/29/10 4:59p vanessah
 * SW7422-43:  3D graphics render, API change
 *
 * Hydra_Software_Devel/4   7/10/09 5:53p rpan
 * PR55188: Convert scanf() to BTST_P_Scanf().
 *
 * Hydra_Software_Devel/3   2/26/09 4:37p nissen
 * PR 46310: Added prediction mode.
 *
 * Hydra_Software_Devel/2   10/27/08 11:28a yuxiaz
 * PR46309: GFD testfeature1 bringup.
 *
 * Hydra_Software_Devel/1   10/22/08 10:22p nissen
 * PR 46310: GRC and P3D tests.
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "test_util.h"
#include "commands.h"

#include "bdbg.h"
#include "bstd_defs.h"

#if (BTST_P_SUPPORT_GRC)
/***************************************************************************/
void BTST_P_OpenGrc(
	BTST_P_Context *pContext )
{
	BERR_Code err;

	if( pContext->hGrc )
	{
		printf( "ERROR: GRC already opened\n" );
		return;
	}

	err = BGRC_Open( &pContext->hGrc, pContext->hChp, pContext->hReg,
		pContext->astMemInfo[0].hMem, pContext->hInt, NULL );
	if( err != BERR_SUCCESS )
	{
		printf( "ERROR: BGRC_Open\n" );
		return;
	}

	printf( "GRC Open\n" );
}

/***************************************************************************/
void BTST_P_CloseGrc(
	BTST_P_Context *pContext )
{
	if( pContext->hGrc == 0 )
	{
		printf( "ERROR: GRC not opened\n" );
		return;
	}

	BGRC_Close( pContext->hGrc );
	printf( "GRC Close\n" );
}

/***************************************************************************/
void BTST_P_GrcCompressDCE(
	BTST_P_Context *pContext )
{
	BSUR_Surface_Settings SurfaceSettings;
	BSUR_Surface_Handle hSourceSurface = 0;
	BSUR_Surface_Handle hCompressSurface = 0;
	BPXL_Format eFormat = BPXL_eA8_R8_G8_B8;
	BERR_Code err;
	uint32_t ulBitsPerPixel, ulWidth, ulHeight, ulFormat, ulPredictionMode;
	BVDC_Window_Handle hGfxWin = NULL;
	BVDC_Source_Handle hGfxSrc = NULL;
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
	BAVC_Gfx_Picture           stGFXPicSetting;
	int iSelectedWin = 2;
	char  c = '0';

	if( pContext->hGrc == 0 )
	{
		printf( "ERROR: GRC not opened\n" );
		return;
	}

	/* get surface information */
	printf( "Bits per pixel: " );
	BTST_P_Scanf(pContext,  "%u", &ulBitsPerPixel );
	printf( "Pixel format (0.ARGB8888, 1.AYCbCr8888): " );
	BTST_P_Scanf(pContext,  "%u", &ulFormat );
	printf( "Prediction mode (0.Normal, 1.NOLEFT, 2.NO2LEFT): " );
	BTST_P_Scanf(pContext,  "%u", &ulPredictionMode );
	printf( "Width: " );
	BTST_P_Scanf(pContext,  "%u", &ulWidth );
	printf( "Height: " );
	BTST_P_Scanf(pContext,  "%u", &ulHeight );

	switch( ulFormat )
	{
	case 0: eFormat = BPXL_eA8_R8_G8_B8; break;
	case 1: eFormat = BPXL_eA8_Y8_Cb8_Cr8; break;
	}

	/* create source surface */
	err = BSUR_Surface_Create( pContext->astMemInfo[0].hMem, ulWidth, ulHeight,
		0, NULL, eFormat, 0, 0, NULL, &hSourceSurface );
	if( err != BERR_SUCCESS )
	{
		printf( "ERROR: BSUR_Surface_Create - source surface\n" );
		goto done;
	}

	BGFX_MakeImageARGB8888( hSourceSurface );

	/* create compressed surface */
	BSUR_Surface_GetDefaultSettings( &SurfaceSettings );
	SurfaceSettings.stTestFeature1Settings.bEnable = true;
	SurfaceSettings.stTestFeature1Settings.ulBitsPerPixel = ulBitsPerPixel;
	SurfaceSettings.stTestFeature1Settings.ulPredictionMode = ulPredictionMode;

	err = BSUR_Surface_Create( pContext->astMemInfo[0].hMem, ulWidth, ulHeight, 0, NULL,
		eFormat, 0, BSUR_CONSTRAINT_DCX_TESTFEATURE1_FORMAT, &SurfaceSettings, &hCompressSurface );
	if( err != BERR_SUCCESS )
	{
		printf( "ERROR: BSUR_Surface_Create - compressed surface\n" );
		goto done;
	}

	/* compression blit */
	BGRC_ResetState( pContext->hGrc );
	BGRC_Source_SetSurface( pContext->hGrc, hSourceSurface );
	BGRC_Output_SetSurface( pContext->hGrc, hCompressSurface );
	BGRC_IssueStateAndWait( pContext->hGrc );
	printf( "Done with M2MC blit\n" );

	BVDC_Source_Create(pContext->hVdc, &hGfxSrc, BAVC_SourceId_eGfx0, NULL);
	stGFXPicSetting.hSurface = hCompressSurface;
	stGFXPicSetting.eInOrientation = BFMT_Orientation_e2D;

	BVDC_Source_SetSurface(hGfxSrc, &stGFXPicSetting);

	BVDC_Window_Create(hCompositor, &hGfxWin, BVDC_WindowId_eGfx0, hGfxSrc, NULL);
	BVDC_Window_SetAlpha(hGfxWin, 0x80);
	BVDC_Window_SetBlendFactor (hGfxWin, BVDC_BlendFactor_eConstantAlpha,
		BVDC_BlendFactor_eOneMinusSrcAlpha, BVDC_ALPHA_MAX);
	BVDC_Window_SetZOrder(hGfxWin, iSelectedWin);
	BVDC_Window_SetVisibility(hGfxWin, true);
	BVDC_Window_SetScalerOutput(hGfxWin, 0, 0, ulWidth, ulHeight);
	BVDC_Window_SetDstRect(hGfxWin, 0, 0, ulWidth, ulHeight);
	BTST_P_ApplyChanges(pContext);

	/* Wait for user */
	printf( "Hit key to exit\n" );
	while(c != 'q')
	{
		c = getchar();
	}

done:
	BVDC_Window_Destroy(hGfxWin);
	BVDC_Source_Destroy(hGfxSrc);
	BTST_P_ApplyChanges(pContext);

	BSUR_Surface_Destroy( hCompressSurface );
	BSUR_Surface_Destroy( hSourceSurface );
}
#endif

#if (BTST_P_SUPPORT_P3D)
/***************************************************************************/
void BTST_P_OpenP3d(
	BTST_P_Context *pContext )
{
	BERR_Code err;

	if( pContext->hP3d )
	{
		printf( "ERROR: P3D already opened\n" );
		return;
	}

	err = BP3D_Open( &pContext->hP3d, pContext->hChp, pContext->hReg,
		pContext->astMemInfo[0].hMem, pContext->hInt );
	if( err != BERR_SUCCESS )
	{
		printf( "ERROR: BP3D_Open\n" );
		return;
	}

	err = BP3D_Context_Create( pContext->hP3d, &pContext->hP3dContext );
 	if( err != BERR_SUCCESS )
	{
		printf( "ERROR: BP3D_Context_Create\n" );
		return;
	}

	BP3D_Context_SetCurrent( pContext->hP3d, pContext->hP3dContext );

	printf( "P3D Open\n" );
}

/***************************************************************************/
void BTST_P_CloseP3d(
	BTST_P_Context *pContext )
{
	if( pContext->hP3d == 0 )
	{
		printf( "ERROR: P3D not opened\n" );
		return;
	}

	BP3D_Context_Destroy( pContext->hP3d, pContext->hP3dContext );
	BP3D_Close( pContext->hP3d );
	printf( "P3D Close\n" );
}

/***************************************************************************/
#define BTST_P_P3D_RENDER_WIDTH    720
#define BTST_P_P3D_RENDER_HEIGHT   480
#define BTST_P_P3D_TEXTURE_WIDTH   1024
#define BTST_P_P3D_TEXTURE_HEIGHT  1024

#define BTST_P_P3D_X0      0
#define BTST_P_P3D_X1      BTST_P_P3D_RENDER_WIDTH
#define BTST_P_P3D_Y0      0
#define BTST_P_P3D_Y1      BTST_P_P3D_RENDER_HEIGHT

#define BTST_P_P3D_S0      0
#define BTST_P_P3D_S1      ((float) BTST_P_P3D_RENDER_WIDTH / BTST_P_P3D_TEXTURE_WIDTH)
#define BTST_P_P3D_T0      0
#define BTST_P_P3D_T1      ((float) BTST_P_P3D_RENDER_HEIGHT / BTST_P_P3D_TEXTURE_HEIGHT)

/***************************************************************************/
void BTST_P_P3dRenderTest(
	BTST_P_Context *pContext )
{
	BSUR_Surface_Handle hRenderSurface = 0;
	BSUR_Surface_Handle hTextureSurface = 0;
	BERR_Code err;
	int ii;

	BP3D_Vertex vertices[4] = {
		{ BTST_P_P3D_X0, BTST_P_P3D_Y0, 0, 1, 0xFFFFFFFF, 0, BTST_P_P3D_S0, BTST_P_P3D_T0, 1, BTST_P_P3D_X0, BTST_P_P3D_Y0 },
		{ BTST_P_P3D_X0, BTST_P_P3D_Y1, 0, 1, 0xFFFFFFFF, 0, BTST_P_P3D_S0, BTST_P_P3D_T1, 1, BTST_P_P3D_X0, BTST_P_P3D_Y1 },
		{ BTST_P_P3D_X1, BTST_P_P3D_Y0, 0, 1, 0xFFFFFFFF, 0, BTST_P_P3D_S1, BTST_P_P3D_T0, 1, BTST_P_P3D_X1, BTST_P_P3D_Y0 },
		{ BTST_P_P3D_X1, BTST_P_P3D_Y1, 0, 1, 0xFFFFFFFF, 0, BTST_P_P3D_S1, BTST_P_P3D_T1, 1, BTST_P_P3D_X1, BTST_P_P3D_Y1 } };

	if( pContext->hP3d == 0 )
	{
		printf( "ERROR: P3D not opened\n" );
		return;
	}

	/* create render surface */
	err = BSUR_Surface_Create( pContext->astMemInfo[0].hMem, BTST_P_P3D_RENDER_WIDTH,
		BTST_P_P3D_RENDER_HEIGHT, 0, NULL, BPXL_eA8_R8_G8_B8, 0, 0, NULL, &hRenderSurface );
	if( err != BERR_SUCCESS )
	{
		printf( "ERROR: BSUR_Surface_Create - render surface\n" );
		goto done;
	}

	/* create texture surface */
	err = BSUR_Surface_Create( pContext->astMemInfo[0].hMem, BTST_P_P3D_TEXTURE_WIDTH,
		BTST_P_P3D_TEXTURE_HEIGHT, 0, NULL, BPXL_eA8_R8_G8_B8, 0, 0, NULL, &hTextureSurface );
	if( err != BERR_SUCCESS )
	{
		printf( "ERROR: BSUR_Surface_Create - texture surface\n" );
		goto done;
	}

	BGFX_MakeImageARGB8888( hTextureSurface );

BP3D_ConfigureMemoryBlockout( pContext->hP3d, -1 );

	/* set render state */
	BP3D_ClearColor( pContext->hP3d, 0 );
	BP3D_ClearDepth( pContext->hP3d, 1 );
	BP3D_SetViewport( pContext->hP3d, 0, 0, BTST_P_P3D_RENDER_WIDTH, BTST_P_P3D_RENDER_HEIGHT, 0, 0 );
	BP3D_SetDstColorSurface( pContext->hP3d, hRenderSurface );
	BP3D_SetTextureSurface( pContext->hP3d, hTextureSurface );

	/* draw 2 triangles */
	printf( "2 triangles ... " );
	BP3D_DrawTriangleList( pContext->hP3d, vertices, 2 );
	BP3D_IssueRenderAndWait( pContext->hP3d );
	printf( "DONE\n" );

	/* draw 64 triangles */
	printf( "64 triangles ... " );
	for( ii = 0; ii < 32; ++ii )
	{
		vertices[ii].fY += ii;
		BP3D_DrawTriangleList( pContext->hP3d, vertices, 2 );
	}
	BP3D_IssueRenderAndWait( pContext->hP3d );
	printf( "DONE\n" );

	/* draw 1024 triangles */
	printf( "1024 triangles ... " );
	for( ii = 0; ii < 512; ++ii )
		BP3D_DrawTriangleList( pContext->hP3d, vertices, 2 );
	BP3D_IssueRenderAndWait( pContext->hP3d );
	printf( "DONE\n" );

done:
	BSUR_Surface_Destroy( hTextureSurface );
	BSUR_Surface_Destroy( hRenderSurface );
}
#endif

