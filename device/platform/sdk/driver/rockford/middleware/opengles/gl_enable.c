/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_enable.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 7/4/09 5:22p $
 *
 * Module Description: Enable entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_enable.c $
 * 
 * Hydra_Software_Devel/6   7/4/09 5:22p nissen
 * PR 56587: Changed LogicOp to using blending for OpenGL conformance.
 * 
 * Hydra_Software_Devel/5   9/8/08 12:47p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/4   10/17/05 8:56p nissen
 * PR 17656: Switched texture parameters from context to texture states.
 * 
 * Hydra_Software_Devel/3   9/17/04 3:19p nissen
 * PR 9132: Fixed logic op rendering.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:06p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
void GL_P_SetEnable(
	EGL_P_Display *pDisplay,
	EGL_P_Context *pContext,
	GLenum cap,
	GLint bEnable )
{
	switch( cap )
	{
	case GL_ALPHA_TEST:
		pContext->bAlphaTest = bEnable;
		glAlphaFunc( pContext->eAlphaFunc, pContext->fAlphaRef );
		break;

	case GL_BLEND:
		pContext->bBlend = bEnable;
		glBlendFunc( pContext->eSrcBlendFactor, pContext->eDstBlendFactor );
		if( (!pContext->bBlend) && pContext->bLogicOp )
			glLogicOp( pContext->eLogicOp );
		break;

	case GL_COLOR_LOGIC_OP:
		pContext->bLogicOp = bEnable;
		if( pContext->bLogicOp )
			glLogicOp( pContext->eLogicOp );
		else
			glBlendFunc( pContext->eSrcBlendFactor, pContext->eDstBlendFactor );
		break;

	case GL_COLOR_MATERIAL:
		pContext->bColorMaterial = bEnable;
		break;

	case GL_CULL_FACE:
		pContext->bCullFace = bEnable;
		glCullFace( pContext->eCullMode );
		glFrontFace( pContext->eFrontFace );
		break;

	case GL_DEPTH_TEST:
		pContext->bDepthTest = bEnable;
		glDepthFunc( pContext->eDepthFunc );
		glDepthMask( pContext->bDepthMask );
		break;

	case GL_DITHER:
		pContext->bDither = bEnable;
		BP3D_SetDither( pDisplay->hP3d, (bool) bEnable );
		break;

	case GL_FOG:
		pContext->bFog = bEnable;
		glFogf( GL_FOG_MODE, (GLfloat) pContext->eFogMode );
		glFogf( GL_FOG_DENSITY, pContext->fFogDensity );
		glFogf( GL_FOG_START, pContext->fFogStart );
		glFogf( GL_FOG_END, pContext->fFogEnd );
		BP3D_SetFogColor( pDisplay->hP3d, pContext->ulFogColor );
		break;

	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
		pContext->bLight[cap - GL_LIGHT0] = bEnable;
		break;

	case GL_LIGHTING:
		pContext->bLighting = bEnable;
		glShadeModel( pContext->eShadeModel );
		if( pContext->pTexture )
			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (GLfloat) pContext->eTexEnvMode );
		break;

	case GL_NORMALIZE:
		pContext->bNormalize = bEnable;
		break;

	case GL_POLYGON_OFFSET_FILL:
		pContext->bPolygonOffsetFill = bEnable;
		glPolygonOffset( pContext->fDepthFactor, pContext->fDepthUnits );
		break;

	case GL_RESCALE_NORMAL:
		pContext->bRescaleNormal = bEnable;
		break;

	case GL_SCISSOR_TEST:
		pContext->bScissorTestChanged = (pContext->bScissorTest == bEnable) ? GL_FALSE : GL_TRUE;
		pContext->bScissorTest = bEnable;
		glScissor( pContext->Scissor.fX, pContext->Scissor.fY, pContext->Scissor.fWidth, pContext->Scissor.fHeight );
		break;

	case GL_TEXTURE_2D:
		pContext->bTexture2D = bEnable;
		if( pContext->pTexture )
		{
			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (GLfloat) pContext->eTexEnvMode );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat) pContext->pTexture->eTextureMinFilter );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat) pContext->pTexture->eTextureMagFilter );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLfloat) pContext->pTexture->eTextureWrapS );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLfloat) pContext->pTexture->eTextureWrapT );
		}
		break;

	case GL_LINE_SMOOTH:
	case GL_MULTISAMPLE:
	case GL_POINT_SMOOTH:
	case GL_SAMPLE_ALPHA_TO_COVERAGE:
	case GL_SAMPLE_ALPHA_TO_ONE:
	case GL_SAMPLE_COVERAGE:
	case GL_STENCIL_TEST:
		break;

	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}
}

/***************************************************************************/
void GL_P_SetClientStateEnable(
	EGL_P_Display *pDisplay,
	EGL_P_Context *pContext,
	GLenum array,
	GLint bEnable )
{
	BSTD_UNUSED( pDisplay );

	switch( array )
	{
	case GL_COLOR_ARRAY:
		pContext->bColorArray = bEnable;
		break;
	case GL_NORMAL_ARRAY:
		pContext->bNormalArray = bEnable;
		break;
	case GL_TEXTURE_COORD_ARRAY:
		pContext->bTexCoordArray = bEnable;
		break;
	case GL_VERTEX_ARRAY:
		pContext->bVertexArray = bEnable;
		break;
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glEnable(
	GLenum cap )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	GL_P_SetEnable( pDisplay, pContext, cap, GL_TRUE );
}

/***************************************************************************/
GLAPI void APIENTRY glDisable( 
	GLenum cap )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	GL_P_SetEnable( pDisplay, pContext, cap, GL_FALSE );
}

/***************************************************************************/
GLAPI void APIENTRY glEnableClientState( 
	GLenum array )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	GL_P_SetClientStateEnable( pDisplay, pContext, array, GL_TRUE );
}

/***************************************************************************/
GLAPI void APIENTRY glDisableClientState( 
	GLenum array )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	GL_P_SetClientStateEnable( pDisplay, pContext, array, GL_FALSE );
}

/* End of File */
