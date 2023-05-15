/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: egl_buffer.c $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 1/17/12 3:18p $
 *
 * Module Description: Buffer entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/egl_buffer.c $
 * 
 * Hydra_Software_Devel/22   1/17/12 3:18p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/21   6/16/10 12:13p nissen
 * SW7405-3631: Added flush to eglSwapBuffers for conformance.
 * 
 * Hydra_Software_Devel/20   12/11/09 2:33p nissen
 * SW7405-3549: Removed current render prepare when waiting for previous
 * render.
 * 
 * Hydra_Software_Devel/19   8/12/09 1:37p nissen
 * PR 56587: Using ReadSurface as a background source.
 * 
 * Hydra_Software_Devel/18   7/4/09 5:10p nissen
 * PR 56587: Fixed eglSwapBuffers for OpenGL conformance.
 * 
 * Hydra_Software_Devel/17   11/7/08 4:24p nissen
 * PR 46638: Updated eglWaitGL to prepare next render before flushing
 * previous one.
 * 
 * Hydra_Software_Devel/16   9/24/08 9:16p nissen
 * PR 46638: Implemented eglWaitGL function.
 * 
 * Hydra_Software_Devel/15   9/8/08 11:59a nissen
 * PR 46638: Added code to store surface when swapping buffers.
 * 
 * Hydra_Software_Devel/14   12/4/07 8:44p nissen
 * PR 35856: Added support for rendering to a surface that is not the
 * frame buffer.
 * 
 * Hydra_Software_Devel/13   11/15/07 7:52p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/12   11/10/06 12:34p nissen
 * PR 20908: Added support for offscreen back buffer (quadruple
 * buffering).
 * 
 * Hydra_Software_Devel/11   6/12/06 10:35a nissen
 * PR 21896: Added support for timing various operations.
 * 
 * Hydra_Software_Devel/10   6/7/06 10:52p nissen
 * PR 20908: Added support for triple buffering to Settop API verision.
 * 
 * Hydra_Software_Devel/9   4/28/06 11:01a nissen
 * PR 20908: Moved render flush to before flipping instead of after render
 * for Settop API version.
 * 
 * Hydra_Software_Devel/8   11/11/05 3:32p nissen
 * PR 17423: Added support for using Settop API.
 * 
 * Hydra_Software_Devel/7   10/26/05 2:30p nissen
 * PR 17656: Fixed page flipping problem.
 * 
 * Hydra_Software_Devel/6   3/9/05 3:10p nissen
 * PR 12818: Fixed support for triple buffer flipping.
 * 
 * Hydra_Software_Devel/5   9/17/04 4:11p nissen
 * PR 9132: Fixed logic op rendering.
 * 
 * Hydra_Software_Devel/4   9/15/04 3:07p nissen
 * PR 9132: Add temporary code to clear the back buffer to red when
 * surface swapping.
 * 
 * Hydra_Software_Devel/3   9/7/04 3:18p nissen
 * PR 9132: Added buffer swapping and logic op rendering support.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:37p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "native_private.h"

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglWaitGL( void )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();
	if( pContext == NULL )
		return EGL_FALSE;

#if 0
	BP3D_SetDstColorSurface( pDisplay->hP3d, pDrawSurface->hRenderSurface );
	BP3D_SetSrcColorSurface( pDisplay->hP3d, pReadSurface->hRenderSurface );
#else
	BP3D_SetDstColorPlane( pDisplay->hP3d, pDisplay->pDrawSurface->uiFormat, pDisplay->pDrawSurface->uiOffset, pDisplay->pDrawSurface->iPitch );
	BP3D_SetSrcColorPlane( pDisplay->hP3d, pDisplay->pReadSurface->uiFormat, pDisplay->pReadSurface->uiOffset, pDisplay->pReadSurface->iPitch );
#endif

/*	BP3D_PrepareRender( pDisplay->hP3d );*/

	BP3D_FlushRender( pDisplay->hP3d );
	native_P_BlitWait( pContext, pDisplay->nativeDisplay, pDisplay->pDrawSurface->hRenderSurface );

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglWaitNative( 
	EGLint engine )
{
	BSTD_UNUSED( engine );

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglSwapBuffers( 
	EGLDisplay dpy, 
	EGLSurface draw
#ifndef EGL_P_CONFORMANT
	, NativeWindow window
#endif
	)
{
	EGL_P_Surface *pSurface;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	pSurface = (EGL_P_Surface *) draw;
	if( pSurface == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_SURFACE);
		return EGL_FALSE;
	}

#ifdef EGL_P_CONFORMANT
	glFlush();
#endif

	pDisplay->pDrawSurface = pSurface;
	pDisplay->pReadSurface = pSurface;

#ifndef EGL_P_CONFORMANT
	pSurface->nativeWindow = window;
	native_P_InitWindow( pSurface->nativeWindow, &pSurface->hRenderSurface, &pSurface->uiFormat, 
		&pSurface->uiOffset, &pSurface->pvMemory, &pSurface->iPitch, &pSurface->iWidth, &pSurface->iHeight );
#endif

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglCopyBuffers( 
	EGLDisplay dpy, 
	EGLSurface surface, 
	NativePixmap target )
{
	BSTD_UNUSED( dpy );
	BSTD_UNUSED( surface );
	BSTD_UNUSED( target );

	return EGL_TRUE;
}

/* End of File */
