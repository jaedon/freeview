/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_material.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/28/04 1:41p $
 *
 * Module Description: Material entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_material.c $
 * 
 * Hydra_Software_Devel/2   7/28/04 1:41p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glMaterialf( 
	GLenum face, 
	GLenum pname, 
	GLfloat param )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (face != GL_FRONT_AND_BACK) || (pname != GL_SHININESS) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( (param < 0.0) || (param > 128.0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	pContext->Material.fSpecularExponent = param;
}

/***************************************************************************/
GLAPI void APIENTRY glMaterialfv( 
	GLenum face, 
	GLenum pname, 
	const GLfloat *params )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( face != GL_FRONT_AND_BACK )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	switch( pname )
	{
	case GL_SHININESS:
		glMaterialf( face, pname, params[0] );
		break;
	case GL_AMBIENT:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.AmbientColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_DIFFUSE:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.DiffuseColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_SPECULAR:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.SpecularColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_EMISSION:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.EmissionColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_AMBIENT_AND_DIFFUSE:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.AmbientColor, params[0], params[1], params[2], params[3] );
		GL_P_SET_LIGHT_COLOR( &pContext->Material.DiffuseColor, params[0], params[1], params[2], params[3] );
		break;
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}
}

/***************************************************************************/
GLAPI void APIENTRY glMaterialx( 
	GLenum face, 
	GLenum pname, 
	GLfixed param )
{
	glMaterialf( face, pname, GL_P_CONVERT_XtoF(param) );
}

/***************************************************************************/
GLAPI void APIENTRY glMaterialxv( 
	GLenum face, 
	GLenum pname, 
	const GLfixed *params )
{
	if( pname == GL_SHININESS )
	{
		glMaterialf( face, pname, GL_P_CONVERT_XtoF(params[0]) );
	}
	else
	{
		GLfloat afParams[4];
		afParams[0] = GL_P_CONVERT_XtoF(params[0]);
		afParams[1] = GL_P_CONVERT_XtoF(params[1]);
		afParams[2] = GL_P_CONVERT_XtoF(params[2]);
		afParams[3] = GL_P_CONVERT_XtoF(params[3]);
		glMaterialfv( face, pname, afParams );
	}
}

/* End of File */
