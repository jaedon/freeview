/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_fog.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 8/14/10 2:28p $
 *
 * Module Description: Fog entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_fog.c $
 * 
 * Hydra_Software_Devel/6   8/14/10 2:28p nissen
 * SW7400-2378: Fixed problems with bad input.
 * 
 * Hydra_Software_Devel/5   4/27/07 3:17p nissen
 * PR 30062: Changed exponential fog to pass conformance.
 * 
 * Hydra_Software_Devel/4   9/20/06 1:32p nissen
 * PR 22858: Fixed fog.
 * 
 * Hydra_Software_Devel/3   8/10/04 2:57p nissen
 * PR 9132: Fixed fog by switching from table to vertex.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:22p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
void GL_P_SetVertexFogIndex(
	EGL_P_Context *pContext, 
	BP3D_Vertex *pVertexIn,
	BP3D_Vertex *pVertexOut )
{
	GLfloat fFogExp;
	GLfloat fFogIndex;
	GLfloat fFogClamp = 0.0;
	GLfloat fZ;

	switch( pContext->eFogMode )
	{
	case GL_LINEAR:
		fZ = (pVertexIn->fZ < 0.0) ? -pVertexIn->fZ : pVertexIn->fZ;
		if( pContext->fTransFogEnd != pContext->fTransFogStart )
		{
			fFogIndex = (pContext->fTransFogEnd - fZ) / (pContext->fTransFogEnd - pContext->fTransFogStart);
			fFogClamp = GL_P_MIN(GL_P_MAX(fFogIndex, 0.0), 1.0) * 255.0;
		}
		break;
	case GL_EXP:
/*		fFogExp   = pContext->fFogDensity * pVertexIn->fZ * 8;
*/		fFogExp   = pContext->fFogDensity * pVertexIn->fZ;
		fFogIndex = exp( -fFogExp );
		fFogClamp = GL_P_MIN(GL_P_MAX(fFogIndex, 0.0), 1.0) * 255.0;
		break;
	case GL_EXP2:
/*		fFogExp   = pContext->fFogDensity * pVertexIn->fZ * 8;
*/		fFogExp   = pContext->fFogDensity * pVertexIn->fZ;
		fFogIndex = exp( -fFogExp * fFogExp );
		fFogClamp = GL_P_MIN(GL_P_MAX(fFogIndex, 0.0), 1.0) * 255.0;
		break;
	default:
		break;
	}
	
	pVertexOut->ulSpecular |= ((uint32_t) fFogClamp) << 24;
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glFogf( 
	GLenum pname, 
	GLfloat param )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	switch( pname )
	{
	case GL_FOG_MODE:
		if( ((GLenum) param != GL_LINEAR) && ((GLenum) param != GL_EXP) && ((GLenum) param != GL_EXP2) )
		{
			GL_P_SET_ERROR(GL_INVALID_VALUE);
			return;
		}
		pContext->eFogMode = (GLenum) param;
		BP3D_SetFog( pDisplay->hP3d, pContext->bFog ? BP3D_Fog_Vertex : BP3D_Fog_None );
		break;

	case GL_FOG_DENSITY:
		if( param < 0.0 )
		{
			GL_P_SET_ERROR(GL_INVALID_VALUE);
			return;
		}
		pContext->fFogDensity = param;
		break;

	case GL_FOG_START:
		pContext->fFogStart = param;
		break;

	case GL_FOG_END:
		pContext->fFogEnd = param;
		break;

	default: 
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}
}

/***************************************************************************/
GLAPI void APIENTRY glFogfv( 
	GLenum pname, 
	const GLfloat *params )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( pname == GL_FOG_COLOR )
	{
		pContext->ulFogColor = GL_P_CONVERT_COLOR_FtoI(params[3], params[0], params[1], params[2]);
		BP3D_SetFogColor( pDisplay->hP3d, pContext->ulFogColor );
	}
	else
	{
		glFogf( pname, *params );
	}
}

/***************************************************************************/
GLAPI void APIENTRY glFogx( 
	GLenum pname, 
	GLfixed param )
{
	glFogf( pname, (pname == GL_FOG_MODE) ? (GLfloat) param : GL_P_CONVERT_XtoF(param) );
}

/***************************************************************************/
GLAPI void APIENTRY glFogxv( 
	GLenum pname, 
	const GLfixed *params )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( pname == GL_FOG_COLOR )
	{
		pContext->ulFogColor = GL_P_CONVERT_COLOR_XtoI(params[3], params[0], params[1], params[2]);
		BP3D_SetFogColor( pDisplay->hP3d, pContext->ulFogColor );
	}
	else
	{
		glFogx( pname, *params );
	}
}

/* End of File */
