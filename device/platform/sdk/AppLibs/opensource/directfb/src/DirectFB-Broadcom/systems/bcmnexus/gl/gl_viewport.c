/***************************************************************************
 *     (c)2004-2012 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: gl_viewport.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Viewport entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_viewport.c $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 4:45p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/1   8/6/10 8:09p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
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

#include <direct/debug.h>

#include "gl_private.h"

D_DEBUG_DOMAIN( P3DGL_API, "P3DGL/API", "Broadcom P3D GL API" );

/***************************************************************************/
void GL_P_SetViewport(
	p3dOpenGLContextData *pContext )
{
	GLfloat fLeft   = pContext->Viewport.fX;
	GLfloat fTop    = ((GLfloat) pContext->size.h - pContext->Viewport.fHeight) - pContext->Viewport.fY;
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
		GLfloat fScissorTop     = (GLfloat) pContext->size.h - pContext->Scissor.fY - pContext->Scissor.fHeight;
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

	BP3D_SetViewport( pContext->hP3d, pContext->iViewportX, pContext->iViewportY, 
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
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	D_DEBUG_AT( P3DGL_API, "%s( %d,%d %dx%d )\n", __FUNCTION__, x, y, width, height );

	if( (width < 0) || (height < 0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( (x != (GLint) pContext->Viewport.fX) || (width != (GLsizei) pContext->Viewport.fWidth) ||
		(y != (GLint) pContext->Viewport.fY) || (height != (GLsizei) pContext->Viewport.fHeight) )
	{
		GL_P_InitiateRender( pContext, GL_TRUE );

		pContext->Viewport.fX = (GLfloat) x;
		pContext->Viewport.fY = (GLfloat) y;
		pContext->Viewport.fWidth = (GLfloat) width;
		pContext->Viewport.fHeight = (GLfloat) height;

		GL_P_SetViewport( pContext );
	}
}

/***************************************************************************/
GLAPI void APIENTRY glScissor( 
	GLint x, 
	GLint y, 
	GLsizei width, 
	GLsizei height )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( (width < 0) || (height < 0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( pContext->bScissorTestChanged || 
		(x != (GLint) pContext->Scissor.fX) || (width != (GLsizei) pContext->Scissor.fWidth) ||
		(y != (GLint) pContext->Scissor.fY) || (height != (GLsizei) pContext->Scissor.fHeight) )
	{
		GL_P_InitiateRender( pContext, GL_TRUE );

		pContext->bScissorTestChanged = GL_FALSE;
		pContext->Scissor.fX = (GLfloat) x;
		pContext->Scissor.fY = (GLfloat) y;
		pContext->Scissor.fWidth = (GLfloat) width;
		pContext->Scissor.fHeight = (GLfloat) height;

		GL_P_SetViewport( pContext );
	}
}

/* End of File */
