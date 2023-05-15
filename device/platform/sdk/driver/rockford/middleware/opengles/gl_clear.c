/***************************************************************************
 *     Copyright (c) 2004-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_clear.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 9/23/08 6:23p $
 *
 * Module Description: Clear entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_clear.c $
 * 
 * Hydra_Software_Devel/7   9/23/08 6:23p nissen
 * PR 46638: Optimized rectangle blits.
 * 
 * Hydra_Software_Devel/6   9/8/08 12:35p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/5   9/17/04 3:23p nissen
 * PR 9132: Fixed logic op rendering.
 * 
 * Hydra_Software_Devel/4   9/7/04 4:08p nissen
 * PR 9132: Added support for logic op rendering.
 * 
 * Hydra_Software_Devel/3   8/10/04 3:03p nissen
 * PR 9132: Added support for clearing depth buffer to specified value.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:51p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "native_private.h"

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glClear( 
	GLbitfield mask )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( mask & (~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( pContext->bRectangleBlit2D )
	{
		native_P_BlitSurface( pContext, pDisplay->nativeDisplay, pDisplay->pDrawSurface->hRenderSurface, 
			pContext->iViewportOffsetX, pContext->iViewportOffsetY, pContext->iViewportWidth, 
			pContext->iViewportHeight, 0, 0, 0, 0, 0, 0, 0, 1, 1, pContext->ulClearColor, GL_FALSE );
	}
	else
	{
		if( mask & GL_COLOR_BUFFER_BIT )
			BP3D_ClearColor( pDisplay->hP3d, pContext->ulClearColor );
	
		if( mask & GL_DEPTH_BUFFER_BIT )
			BP3D_ClearDepth( pDisplay->hP3d, pContext->fClearDepth );
	}

	pContext->bfClearMask = mask;
}

/***************************************************************************/
GLAPI void APIENTRY glClearColor( 
	GLclampf red, 
	GLclampf green, 
	GLclampf blue, 
	GLclampf alpha )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	pContext->ulClearColor = GL_P_CONVERT_COLOR_FtoI(alpha, red, green, blue);
}

/***************************************************************************/
GLAPI void APIENTRY glClearColorx( 
	GLclampx red, 
	GLclampx green, 
	GLclampx blue, 
	GLclampx alpha )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	pContext->ulClearColor = GL_P_CONVERT_COLOR_XtoI(alpha, red, green, blue);
}

/***************************************************************************/
GLAPI void APIENTRY glClearDepthf( 
	GLclampf depth )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	pContext->fClearDepth = depth;
}

/***************************************************************************/
GLAPI void APIENTRY glClearDepthx( 
	GLclampx depth )
{
	glClearDepthf( GL_P_CONVERT_XtoF(depth) );
}

/***************************************************************************/
GLAPI void APIENTRY glClearStencil( 
	GLint s )
{
	BSTD_UNUSED( s );
}

/* End of File */
