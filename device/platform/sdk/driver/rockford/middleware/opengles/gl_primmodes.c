/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_primmodes.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 9/20/06 1:52p $
 *
 * Module Description: Prim Mode entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_primmodes.c $
 * 
 * Hydra_Software_Devel/4   9/20/06 1:52p nissen
 * PR 22858: Fixed polygon offset.
 * 
 * Hydra_Software_Devel/3   10/17/05 8:51p nissen
 * PR 17656: Fixed gouraud shading.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:56p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glCullFace( 
	GLenum mode )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	pContext->eCullMode = mode;

	if( pContext->bCullFace )
	{
		switch( mode )
		{
		case GL_FRONT:
			BP3D_SetCull( pDisplay->hP3d, (pContext->eFrontFace == GL_CW) ? 
				BP3D_Cull_Clockwise : BP3D_Cull_CounterClockwise );
			break;
		case GL_BACK:
			BP3D_SetCull( pDisplay->hP3d, (pContext->eFrontFace == GL_CCW) ? 
				BP3D_Cull_Clockwise : BP3D_Cull_CounterClockwise );
			break;
		case GL_FRONT_AND_BACK:
			BP3D_SetCull( pDisplay->hP3d, BP3D_Cull_None );
			break;
		default: 
			GL_P_SET_ERROR(GL_INVALID_ENUM);
			return;
		}
	}
	else
	{
		BP3D_SetCull( pDisplay->hP3d, BP3D_Cull_None );
	}
}

/***************************************************************************/
GLAPI void APIENTRY glFrontFace( 
	GLenum mode )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (mode != GL_CW) && (mode != GL_CCW) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pContext->eFrontFace = mode;
	glCullFace( pContext->eCullMode );
}

/***************************************************************************/
GLAPI void APIENTRY glShadeModel( 
	GLenum mode )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (mode != GL_FLAT) && (mode != GL_SMOOTH) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pContext->eShadeModel = mode;

	BP3D_SetShade( pDisplay->hP3d, (mode == GL_SMOOTH) ? BP3D_Shade_Gouraud : BP3D_Shade_Flat );
}

/***************************************************************************/
GLAPI void APIENTRY glPolygonOffset( 
	GLfloat factor, 
	GLfloat units )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	pContext->fDepthFactor = factor;
	pContext->fDepthUnits = units;
	pContext->fDepthOffset = pContext->bPolygonOffsetFill ? units / 65536.0 : 0.0;

	BP3D_SetDepthFactorScale( pDisplay->hP3d, pContext->bPolygonOffsetFill ? factor : 0.0 );
}

/***************************************************************************/
GLAPI void APIENTRY glPolygonOffsetx( 
	GLfixed factor, 
	GLfixed units )
{
	glPolygonOffset( GL_P_CONVERT_XtoF(factor), GL_P_CONVERT_XtoF(units) );
}

/* End of File */
