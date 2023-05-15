/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: egl_surface.c $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 4/18/12 1:02p $
 *
 * Module Description: Surface entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/egl_surface.c $
 * 
 * Hydra_Software_Devel/22   4/18/12 1:02p nissen
 * SW7125-1283: Fixed depth surface support using nexus planes.
 * 
 * Hydra_Software_Devel/21   1/17/12 3:34p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/20   10/28/09 2:00p nissen
 * SW7405-2856: Fixed build error.
 * 
 * Hydra_Software_Devel/19   8/12/09 1:46p nissen
 * PR 56587: Fixed build warning.
 * 
 * Hydra_Software_Devel/18   7/4/09 5:13p nissen
 * PR 56587: Added depth buffer allocation for OpenGL conformance.
 * 
 * Hydra_Software_Devel/17   5/27/09 2:32p nissen
 * PR 47303: Fixed return value in pixmap creation function.
 * 
 * Hydra_Software_Devel/16   10/6/08 12:46p nissen
 * PR 47303: Added support for calling new SUR surface create function.
 * 
 * Hydra_Software_Devel/15   11/16/07 11:02a nissen
 * PR 35855: Fixed problem with calling native function.
 * 
 * Hydra_Software_Devel/14   11/15/07 8:05p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/13   4/30/07 12:19p nissen
 * PR 30062: Added code to check for a valid vdc source.
 * 
 * Hydra_Software_Devel/12   11/10/06 12:39p nissen
 * PR 20908: Added support for offscreen back buffer (quadruple
 * buffering).
 * 
 * Hydra_Software_Devel/11   1/12/06 1:18p nissen
 * PR 18979: Allowing depth buffer creation to fail without returning
 * error.
 * 
 * Hydra_Software_Devel/10   11/11/05 3:46p nissen
 * PR 17423: Added support for using Settop API.
 * 
 * Hydra_Software_Devel/9   11/9/05 8:36p nissen
 * PR 17656: Added call to apply changes after installing display
 * callback.
 * 
 * Hydra_Software_Devel/8   10/26/05 2:36p nissen
 * PR 17656: Fixed page flipping problem.
 * 
 * Hydra_Software_Devel/7   8/2/05 4:21p nissen
 * PR 16475: Removed use of GRC to clear display surfaces.
 * 
 * Hydra_Software_Devel/6   3/9/05 3:01p nissen
 * PR 12818: Fixed support for triple buffer flipping.
 * 
 * Hydra_Software_Devel/5   9/15/04 2:59p nissen
 * PR 9132: Fixed initial buffer settings when creating a windowed
 * surface.
 * 
 * Hydra_Software_Devel/4   9/7/04 4:34p nissen
 * PR 9132: Added code to display a surface when creating a windowed
 * surface.
 * 
 * Hydra_Software_Devel/3   9/7/04 3:34p nissen
 * PR 9132: Added buffer swapping and logic op rendering support.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:40p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "native_private.h"

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
BPXL_Format EGL_P_ConvertFormat_EGLtoP3D( 
	EGLConfig config )
{
	switch( EGL_P_GREEN_SIZE(config) )
	{
	case 4: return BPXL_eA4_R4_G4_B4; break;
	case 5: return BPXL_eA1_R5_G5_B5; break;
	case 6: return BPXL_eR5_G6_B5; break;
	case 8: return BPXL_eA8_R8_G8_B8; break;
	}

	return 0;
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI EGLSurface APIENTRY eglCreateWindowSurface( 
	EGLDisplay dpy, 
	EGLConfig config, 
	NativeWindow window, 
	const EGLint *attrib_list )
{
	EGL_P_Surface *pSurface = NULL;
	EGLint iDepthSize = 0;
	BPXL_Format eFormat;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return (EGLSurface) EGL_NO_SURFACE;

	if( attrib_list && (attrib_list[0] != EGL_NONE) )
	{
		EGL_P_SET_ERROR(EGL_BAD_ATTRIBUTE);
		return (EGLSurface) EGL_NO_SURFACE;
	}

	eFormat = EGL_P_ConvertFormat_EGLtoP3D( config );
	if( eFormat == 0 )
	{
		EGL_P_SET_ERROR(EGL_BAD_CONFIG);
		return (EGLSurface) EGL_NO_SURFACE;
	}

	pSurface = BKNI_Malloc( sizeof (EGL_P_Surface) );
	if( pSurface == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_ALLOC);
		return (EGLSurface) EGL_NO_SURFACE;
	}

	BKNI_Memset( pSurface, 0, sizeof (EGL_P_Surface) );

	native_P_InitWindow( window, &pSurface->hRenderSurface, &pSurface->uiFormat, 
		&pSurface->uiOffset, &pSurface->pvMemory, &pSurface->iPitch, &pSurface->iWidth, &pSurface->iHeight );

	eglGetConfigAttrib( dpy, config, EGL_DEPTH_SIZE, &iDepthSize );

#ifdef EGL_P_CONFORMANT
	if( iDepthSize )
	{
		GLuint uiFormat;
		EGLint iWidth;
		EGLint iHeight;

#ifdef BSUR_SURFACE_CREATE_HAS_SETTINGS
		BERR_Code err = BSUR_Surface_Create( pDisplay->hMem, pSurface->iWidth, pSurface->iHeight, 0, NULL, BPXL_eZ16, 0, 0, 0, &pSurface->hDepthSurface );
#else
		BERR_Code err = BSUR_Surface_Create( pDisplay->hMem, pSurface->iWidth, pSurface->iHeight, 0, NULL, BPXL_eZ16, 0, 0, &pSurface->hDepthSurface );
#endif
		if( err != BERR_SUCCESS )
		{
			BKNI_Free( pSurface );
			EGL_P_SET_ERROR(EGL_BAD_ALLOC);
			return (EGLSurface) EGL_NO_SURFACE;
		}
		native_P_InitWindow( window, &pSurface->hDepthSurface, &uiFormat, 
			&pSurface->uiDepthOffset, &pSurface->pvDepthMemory, &pSurface->iDepthPitch, &iWidth, &iHeight );
	}
#endif

	pSurface->nativeWindow = window;
	pSurface->config  = config;
	pSurface->eFormat = eFormat;

	return (EGLSurface) pSurface;
}

/***************************************************************************/
GLAPI EGLSurface APIENTRY eglCreatePixmapSurface( 
	EGLDisplay dpy, 
	EGLConfig config, 
	NativePixmap pixmap, 
	const EGLint *attrib_list )
{
	EGL_P_Surface *pSurface = NULL;
	BPXL_Format eFormat;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return (EGLSurface) EGL_NO_SURFACE;

	if( attrib_list && (attrib_list[0] != EGL_NONE) )
	{
		EGL_P_SET_ERROR(EGL_BAD_ATTRIBUTE);
		return (EGLSurface) EGL_NO_SURFACE;
	}

	eFormat = EGL_P_ConvertFormat_EGLtoP3D( config );
	if( eFormat == 0 )
	{
		EGL_P_SET_ERROR(EGL_BAD_CONFIG);
		return (EGLSurface) EGL_NO_SURFACE;
	}

	pSurface = BKNI_Malloc( sizeof (EGL_P_Surface) );
	if( pSurface == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_ALLOC);
		return (EGLSurface) EGL_NO_SURFACE;
	}

	BKNI_Memset( pSurface, 0, sizeof (EGL_P_Surface) );

	native_P_InitWindow( pixmap, &pSurface->hRenderSurface, &pSurface->uiFormat, 
		&pSurface->uiOffset, &pSurface->pvMemory, &pSurface->iPitch, &pSurface->iWidth, &pSurface->iHeight );

	pSurface->nativePixmap = pixmap;
	pSurface->config = config;
	pSurface->eFormat = eFormat;

	return (EGLSurface) pSurface;
}

/***************************************************************************/
GLAPI EGLSurface APIENTRY eglCreatePbufferSurface( 
	EGLDisplay dpy, 
	EGLConfig config, 
	const EGLint *attrib_list )
{
	EGL_P_Surface *pSurface = NULL;
	EGLint attribute;
	BPXL_Format eFormat;
	BERR_Code err;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return (EGLSurface) EGL_NO_SURFACE;

	eFormat = EGL_P_ConvertFormat_EGLtoP3D( config );
	if( eFormat == 0 )
	{
		EGL_P_SET_ERROR(EGL_BAD_CONFIG);
		return (EGLSurface) EGL_NO_SURFACE;
	}

	pSurface = BKNI_Malloc( sizeof (EGL_P_Surface) );
	if( pSurface == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_ALLOC);
		return (EGLSurface) EGL_NO_SURFACE;
	}

	BKNI_Memset( pSurface, 0, sizeof (EGL_P_Surface) );

	pSurface->config = config;
	pSurface->eFormat = eFormat;

	attribute = *attrib_list++;
	while( attribute != EGL_NONE )
	{
		switch( attribute )
		{
		case EGL_WIDTH:           pSurface->iWidth = *attrib_list++; break;
		case EGL_HEIGHT:          pSurface->iHeight = *attrib_list++; break;
		case EGL_LARGEST_PBUFFER: pSurface->iLargestBuffer = *attrib_list++; break;
		default: attrib_list++;
		}
		attribute = *attrib_list++;
	}

	if( (pSurface->iWidth == 0) || (pSurface->iHeight) == 0 )
	{
		BKNI_Free( pSurface );
		EGL_P_SET_ERROR(EGL_BAD_ATTRIBUTE);
		return (EGLSurface) EGL_NO_SURFACE;
	}

#ifdef BSUR_SURFACE_CREATE_HAS_SETTINGS
	err = BSUR_Surface_Create( pDisplay->hMem, pSurface->iWidth, pSurface->iHeight, 0, NULL, eFormat, 0, 0, 0, &pSurface->hRenderSurface );
#else
	err = BSUR_Surface_Create( pDisplay->hMem, pSurface->iWidth, pSurface->iHeight, 0, NULL, eFormat, 0, 0, &pSurface->hRenderSurface );
#endif
	if( err != BERR_SUCCESS )
	{
		BKNI_Free( pSurface );
		EGL_P_SET_ERROR(EGL_BAD_ALLOC);
		return (EGLSurface) EGL_NO_SURFACE;
	}

	pSurface->bColorOwned = GL_TRUE;

	return (EGLSurface) pSurface;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglDestroySurface( 
	EGLDisplay dpy, 
	EGLSurface surface )
{
	EGL_P_Surface *pSurface;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	pSurface = (EGL_P_Surface *) surface;
	if( pSurface == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_SURFACE);
		return EGL_FALSE;
	}

	if( pSurface->bColorOwned )
		BSUR_Surface_Destroy( pSurface->hRenderSurface );

	if( pSurface->hDepthSurface )
		BSUR_Surface_Destroy( pSurface->hDepthSurface );

	BKNI_Free( pSurface );

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglQuerySurface( 
	EGLDisplay dpy, 
	EGLSurface surface, 
	EGLint attribute, 
	EGLint *value )
{
	EGL_P_Surface *pSurface;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	pSurface = (EGL_P_Surface *) surface;
	if( pSurface == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_SURFACE);
		return EGL_FALSE;
	}

	switch( attribute )
	{
	case EGL_CONFIG_ID:       *value = (EGLint) pSurface->config; break;
	case EGL_WIDTH:           *value = pSurface->iWidth; break;
	case EGL_HEIGHT:          *value = pSurface->iHeight; break;
	case EGL_LARGEST_PBUFFER: *value = pSurface->iLargestBuffer; break;
	default: EGL_P_SET_ERROR(EGL_BAD_ATTRIBUTE); return EGL_FALSE;
	}

	return EGL_TRUE;
}

/* End of File */
