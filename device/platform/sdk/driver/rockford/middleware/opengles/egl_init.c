/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: egl_init.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 5/11/12 1:30p $
 *
 * Module Description: Initialization entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/egl_init.c $
 * 
 * Hydra_Software_Devel/13   5/11/12 1:30p nissen
 * SW7125-1283: Fixed P3D memory leak when closing.
 * 
 * Hydra_Software_Devel/12   9/19/11 2:51p nissen
 * SW7125-1085: Fixed eglTerminate.
 * 
 * Hydra_Software_Devel/11   2/27/09 3:06p nissen
 * PR 42507: Added support for dual display.
 * 
 * Hydra_Software_Devel/10   8/18/08 5:15p nissen
 * PR 41194: Added function for configuring memory blockout for 3D device.
 * 
 * Hydra_Software_Devel/9   7/18/08 8:37a nissen
 * PR 41229: Fixed warnings.
 * 
 * Hydra_Software_Devel/8   7/15/08 2:05p nissen
 * PR 41229: Added support for configuring device tiles.
 * 
 * Hydra_Software_Devel/7   11/15/07 7:58p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/6   6/12/06 10:42a nissen
 * PR 21896: Added support for timing various operations.
 * 
 * Hydra_Software_Devel/5   11/11/05 3:39p nissen
 * PR 17423: Added support for using Settop API.
 * 
 * Hydra_Software_Devel/4   10/17/05 9:09p nissen
 * PR 17656: Added support for generating mipmap lods.
 * 
 * Hydra_Software_Devel/3   9/7/04 3:26p nissen
 * PR 9132: Added buffer swapping and logic op rendering support.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:32p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "native_private.h"

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
EGL_P_Display g_EGL_P_Display[2] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 } };
EGLint g_EGL_P_iDisplayIndex = 0;

/***************************************************************************/
EGL_P_Display *EGL_P_ValidateDisplay( 
	EGLDisplay dpy )
{
	if( dpy == (EGLDisplay) (&g_EGL_P_Display[0]) )
		return (EGL_P_Display *) (&g_EGL_P_Display[0]);
	else if( dpy == (EGLDisplay) (&g_EGL_P_Display[1]) )
		return (EGL_P_Display *) (&g_EGL_P_Display[1]);

	EGL_P_SET_ERROR(EGL_BAD_DISPLAY);
	return NULL;
}

/***************************************************************************/
void EGL_P_SetDisplay( 
	EGLDisplay dpy )
{
	g_EGL_P_iDisplayIndex = (dpy == (EGLDisplay) (&g_EGL_P_Display[0])) ? 0 : 1;
}

/***************************************************************************/
EGL_P_Display *EGL_P_GetDisplay( void )
{
	return &g_EGL_P_Display[g_EGL_P_iDisplayIndex];
}

/***************************************************************************/
EGL_P_Context *EGL_P_GetContext( void )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	return pDisplay->pContext;
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI EGLDisplay APIENTRY eglGetDisplay( 
	NativeDisplay display )
{
	EGL_P_Display *pDisplay = &g_EGL_P_Display[0];
	if( pDisplay->nativeDisplay )
	{
		pDisplay = &g_EGL_P_Display[1];
		if( pDisplay->nativeDisplay )
		{
			EGL_P_SET_ERROR(EGL_BAD_DISPLAY);
			return 0;
		}
	}

	BKNI_Memset( pDisplay, 0, sizeof (EGL_P_Display) );

	native_P_InitDisplay( display, &pDisplay->hMem, &pDisplay->hP3d );

	pDisplay->nativeDisplay = display;
	pDisplay->iError = EGL_SUCCESS;

	return (EGLDisplay) pDisplay;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglInitialize( 
	EGLDisplay dpy, 
	EGLint *major, 
	EGLint *minor )
{
	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	/* set major and minor versions */
	if( major )
		*major = EGL_P_MAJOR_VERSION;
	if( minor )
		*minor = EGL_P_MINOR_VERSION;

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglTerminate( 
	EGLDisplay dpy )
{
	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	pDisplay->iError = 0;
	pDisplay->pContext = NULL;
	pDisplay->pDrawSurface = NULL;
	pDisplay->pReadSurface = NULL;
	pDisplay->bGenerateMipmaps = EGL_FALSE;

	native_P_TermDisplay( pDisplay->nativeDisplay );

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI const char * APIENTRY eglQueryString( 
	EGLDisplay dpy, 
	EGLint name )
{
	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return NULL;

	switch( name )
	{
	case EGL_VENDOR:
		return EGL_P_VENDOR_STRING;
	case EGL_VERSION:
		return EGL_P_VERSION_STRING;
	case EGL_EXTENSIONS:
		return EGL_P_EXTENSIONS_STRING;
	}

	EGL_P_SET_ERROR(EGL_BAD_PARAMETER);
	return NULL;
}

/***************************************************************************/
GLAPI EGLContext APIENTRY eglGetCurrentContext( void )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	return (EGLContext) pDisplay->pContext;
}

/***************************************************************************/
GLAPI EGLSurface APIENTRY eglGetCurrentSurface( 
	EGLint readdraw )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	return (EGLSurface) ((readdraw == EGL_DRAW) ? 
		pDisplay->pDrawSurface : pDisplay->pReadSurface);
}

/***************************************************************************/
GLAPI EGLDisplay APIENTRY eglGetCurrentDisplay( void )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	if( pDisplay->pContext == NULL )
		return (EGLDisplay) EGL_BAD_DISPLAY;

	return (EGLDisplay) pDisplay;
}

/***************************************************************************/
GLAPI void (* APIENTRY eglGetProcAddress (const char *procname))(void)
{
	BSTD_UNUSED( procname );
	return NULL;
}

/***************************************************************************/
GLAPI void APIENTRY eglConfigureDevice( 
	EGLDisplay dpy,
	GLuint uiTileWidth,
	GLuint uiTileHeight,
	GLboolean bTileMergeHorizontal,
	GLboolean bTileMergeVertical )
{
	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return;

	BP3D_ConfigureTiles( pDisplay->hP3d, uiTileWidth, uiTileHeight, bTileMergeHorizontal, bTileMergeVertical );
}

/***************************************************************************/
GLAPI GLint APIENTRY eglConfigureMemoryBlockout( 
	EGLDisplay dpy,
	GLint iMemoryBlockout )
{
	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return 0;

	return BP3D_ConfigureMemoryBlockout( pDisplay->hP3d, iMemoryBlockout );
}

/* End of File */
