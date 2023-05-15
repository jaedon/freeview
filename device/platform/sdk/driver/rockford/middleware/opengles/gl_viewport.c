/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_viewport.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 8/14/10 2:36p $
 *
 * Module Description: Viewport entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_viewport.c $
 * 
 * Hydra_Software_Devel/8   8/14/10 2:36p nissen
 * SW7400-2378: Fixed problems with bad input.
 * 
 * Hydra_Software_Devel/7   9/8/08 1:22p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/6   3/10/08 11:39a nissen
 * PR 40078: Fixed glScissor.
 * 
 * Hydra_Software_Devel/5   9/14/07 12:26p nissen
 * PR 30062: Fixed scissors.
 * 
 * Hydra_Software_Devel/4   9/17/04 2:48p nissen
 * PR 9132: Changed parameters for calling initiate render function.
 * 
 * Hydra_Software_Devel/3   9/7/04 4:02p nissen
 * PR 9132: Added buffer swapping support.
 * 
 * Hydra_Software_Devel/2   7/28/04 2:34p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
void GL_P_SetViewport(
	EGL_P_Display *pDisplay,
	EGL_P_Context *pContext )
{
	GLfloat fLeft   = pContext->Viewport.fX;
	GLfloat fTop    = ((GLfloat) pDisplay->pDrawSurface->iHeight - pContext->Viewport.fHeight) - pContext->Viewport.fY;
	GLfloat fRight  = fLeft + pContext->Viewport.fWidth;
	GLfloat fBottom = fTop + pContext->Viewport.fHeight;
	GLfloat fWidth  = fRight - fLeft;
	GLfloat fHeight = fBottom - fTop;

	GLfloat fClipLeft   = fLeft;
	GLfloat fClipTop    = fTop;
	GLfloat fClipRight  = fRight;
	GLfloat fClipBottom = fBottom;

	GLfloat fGuardEpsilon = ((GLfloat) GL_P_RENDER_SIZE_MAX) / ((GLfloat) ((fWidth > fHeight) ? fWidth : fHeight));
	GLfloat fGuardWidth   = (fWidth > fHeight) ? (GLfloat) GL_P_RENDER_SIZE_MAX : (fGuardEpsilon * fWidth);
	GLfloat fGuardHeight  = (fWidth > fHeight) ? (fGuardEpsilon * fHeight) : ((GLfloat) GL_P_RENDER_SIZE_MAX);

	if( pContext->bScissorTest )
	{
		GLfloat fScissorLeft    = pContext->Scissor.fX;
		GLfloat fScissorTop     = (GLfloat) pDisplay->pDrawSurface->iHeight - pContext->Scissor.fY - pContext->Scissor.fHeight;
		GLfloat fScissorRight   = fScissorLeft + pContext->Scissor.fWidth;
		GLfloat fScissorBottom  = fScissorTop + pContext->Scissor.fHeight;

		fClipLeft   = GL_P_MAX(fScissorLeft, fLeft);
		fClipTop    = GL_P_MAX(fScissorTop, fTop);
		fClipRight  = GL_P_MIN(fScissorRight, fRight);
		fClipBottom = GL_P_MIN(fScissorBottom, fBottom);
	}

	fRight  = fClipRight - fLeft + (fGuardWidth - pContext->Viewport.fWidth) / 2;
	fBottom = fClipBottom - fTop + (fGuardHeight - pContext->Viewport.fHeight) / 2;
	fLeft   = fClipLeft - fLeft + (fGuardWidth - pContext->Viewport.fWidth) / 2;
	fTop    = fClipTop - fTop + (fGuardHeight - pContext->Viewport.fHeight) / 2;

	pContext->iViewportX = (int) (fLeft + 0.5);
	pContext->iViewportY = (int) (fTop + 0.5);
	pContext->iViewportWidth = (int) (fRight - fLeft);
	pContext->iViewportHeight = (int) (fBottom - fTop);
	pContext->iViewportOffsetX = (int) fClipLeft;
	pContext->iViewportOffsetY = (int) fClipTop;

	BP3D_SetViewport( pDisplay->hP3d, pContext->iViewportX, pContext->iViewportY, 
		pContext->iViewportWidth, pContext->iViewportHeight, pContext->iViewportOffsetX, pContext->iViewportOffsetY );

	pContext->Viewport.fGBEpsilon = fGuardEpsilon;
	pContext->Viewport.fGBInvEpsilon = (fGuardEpsilon != 0.0) ? 1.0 / fGuardEpsilon : 0.0;
	pContext->Viewport.fGBWidth = fGuardWidth;
	pContext->Viewport.fGBHeight = fGuardHeight;
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glViewport( 
	GLint x, 
	GLint y, 
	GLsizei width, 
	GLsizei height )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (width < 0) || (height < 0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( width && height && (
		(x != (GLint) pContext->Viewport.fX) || (width != (GLsizei) pContext->Viewport.fWidth) ||
		(y != (GLint) pContext->Viewport.fY) || (height != (GLsizei) pContext->Viewport.fHeight)) )
	{
		EGL_P_InitiateRender( pDisplay, pContext, GL_TRUE );

		pContext->Viewport.fX = (GLfloat) x;
		pContext->Viewport.fY = (GLfloat) y;
		pContext->Viewport.fWidth = (GLfloat) width;
		pContext->Viewport.fHeight = (GLfloat) height;

		GL_P_SetViewport( pDisplay, pContext );
	}
}

/***************************************************************************/
GLAPI void APIENTRY glScissor( 
	GLint x, 
	GLint y, 
	GLsizei width, 
	GLsizei height )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (width < 0) || (height < 0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( pContext->bScissorTestChanged || 
		(x != (GLint) pContext->Scissor.fX) || (width != (GLsizei) pContext->Scissor.fWidth) ||
		(y != (GLint) pContext->Scissor.fY) || (height != (GLsizei) pContext->Scissor.fHeight) )
	{
		EGL_P_InitiateRender( pDisplay, pContext, GL_TRUE );

		pContext->bScissorTestChanged = GL_FALSE;
		pContext->Scissor.fX = (GLfloat) x;
		pContext->Scissor.fY = (GLfloat) y;
		pContext->Scissor.fWidth = (GLfloat) width;
		pContext->Scissor.fHeight = (GLfloat) height;

		GL_P_SetViewport( pDisplay, pContext );
	}
}

/* End of File */
