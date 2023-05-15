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
 * $brcm_Workfile: gl_enable.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Enable entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_enable.c $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 4:45p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/1   8/6/10 8:07p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
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

#include "gl_private.h"

/***************************************************************************/
void GL_P_SetEnable(
	p3dOpenGLContextData *pContext,
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
		BP3D_SetDither( pContext->hP3d, (bool) bEnable );
		break;
	case GL_FOG:
		pContext->bFog = bEnable;
		glFogf( GL_FOG_MODE, (GLfloat) pContext->eFogMode );
		glFogf( GL_FOG_DENSITY, pContext->fFogDensity );
		glFogf( GL_FOG_START, pContext->fFogStart );
		glFogf( GL_FOG_END, pContext->fFogEnd );
		BP3D_SetFogColor( pContext->hP3d, pContext->ulFogColor );
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
	case GL_COLOR_LOGIC_OP:
/*		GL_P_SetLogicOp( pContext, bEnable, pContext->eLogicOp );*/
		pContext->bLogicOp = bEnable;
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
	p3dOpenGLContextData *pContext,
	GLenum array,
	GLint bEnable )
{
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
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	GL_P_SetEnable( pContext, cap, GL_TRUE );
}

/***************************************************************************/
GLAPI void APIENTRY glDisable( 
	GLenum cap )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	GL_P_SetEnable( pContext, cap, GL_FALSE );
}

/***************************************************************************/
GLAPI void APIENTRY glEnableClientState( 
	GLenum array )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	GL_P_SetClientStateEnable( pContext, array, GL_TRUE );
}

/***************************************************************************/
GLAPI void APIENTRY glDisableClientState( 
	GLenum array )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	GL_P_SetClientStateEnable( pContext, array, GL_FALSE );
}

/* End of File */
