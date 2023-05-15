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
 * $brcm_Workfile: gl_texture.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Texture entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_texture.c $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 4:45p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/1   8/6/10 8:08p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
 * 
 * Hydra_Software_Devel/17   3/16/09 5:05p nissen
 * PR 41194: Fixed texture deallocation.
 * 
 * Hydra_Software_Devel/16   11/3/08 5:55p mphillip
 * PR47441: Texture allocation for kernel mode adjustments for 3548/3556
 * 
 * Hydra_Software_Devel/15   9/23/08 6:35p nissen
 * PR 46638: Optimized rectangle blits.
 * 
 * Hydra_Software_Devel/14   9/8/08 1:10p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/13   1/23/08 3:19p mphillip
 * PR38923: mipmap memory needs to be freed in a matching manner for
 * kernel/proxy mode, or it will crash
 * 
 * Hydra_Software_Devel/12   9/14/06 4:39p nissen
 * PR 22858: Fixed problem with glBindTexture() returning error.
 * 
 * Hydra_Software_Devel/11   9/14/06 3:27p nissen
 * PR 22858: Fixed problem with setting mipmap parameters.
 * 
 * Hydra_Software_Devel/10   6/5/06 12:26p nissen
 * PR 20908: Added support for texture swizzling.
 * 
 * Hydra_Software_Devel/9   1/6/06 9:54a nissen
 * PR 17656: Added support for using Settop API video captured textures.
 * 
 * Hydra_Software_Devel/8   11/11/05 4:11p nissen
 * PR 17423: Added video texture support.
 * 
 * Hydra_Software_Devel/7   10/26/05 2:40p nissen
 * PR 17656: Added support for setting mipmap lod max.
 * 
 * Hydra_Software_Devel/6   10/17/05 9:24p nissen
 * PR 17656: Made texture parameters texture states instead of context
 * states.
 * 
 * Hydra_Software_Devel/5   9/17/04 2:54p nissen
 * PR 9132: Changed parameters for calling initiate render function.
 * 
 * Hydra_Software_Devel/4   9/7/04 3:58p nissen
 * PR 9132: Added buffer swapping support.
 * 
 * Hydra_Software_Devel/3   8/10/04 2:03p nissen
 * PR 9132: Added code to initiate render when deleting textures.
 * 
 * Hydra_Software_Devel/2   7/28/04 2:19p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "gl_private.h"

#if B_SYSTEM_proxy
#define BMEM_TEXTURE_ALLOCATION 1
#endif /* B_SYSTEM_proxy */
#ifdef NEXUS_HAS_3D
#define BMEM_TEXTURE_ALLOCATION 1
#endif /* NEXUS_HAS_3D */

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glBindTexture( 
	GLenum target, 
	GLuint texture )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( ((target != GL_TEXTURE_2D) && (target != GL_TEXTURE_2D_SURFACE_DIRECTFB) && (target != GL_TEXTURE_SWIZZLE)) || 
		(texture >= pContext->uiFreeTexCount) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( pContext->apTextures[texture] == NULL )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( pContext->apTextures[texture] == (GL_P_Texture *) 1 )
	{
		pContext->apTextures[texture] = (GL_P_Texture *) BKNI_Malloc( sizeof (GL_P_Texture) );
		if( pContext->apTextures[texture] == NULL )
		{
			GL_P_SET_ERROR(GL_OUT_OF_MEMORY);
			return;
		}

		BKNI_Memset( pContext->apTextures[texture], 0, sizeof (GL_P_Texture) );

		pContext->apTextures[texture]->eTextureMinFilter = GL_NEAREST_MIPMAP_LINEAR;
		pContext->apTextures[texture]->eTextureMagFilter = GL_LINEAR;
		pContext->apTextures[texture]->eTextureWrapS = GL_REPEAT;
		pContext->apTextures[texture]->eTextureWrapT = GL_REPEAT;
	}

	pContext->pTexture = pContext->apTextures[texture];

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pContext->pTexture->eTextureMinFilter );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pContext->pTexture->eTextureMagFilter );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, pContext->pTexture->eTextureWrapS );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, pContext->pTexture->eTextureWrapT );
	BP3D_SetTextureSwizzle( pContext->hP3d, pContext->pTexture->bSwizzle ? true : false );
}

/***************************************************************************/
GLAPI void APIENTRY glDeleteTextures( 
	GLsizei n, 
	const GLuint *textures )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();
	GLuint uiMinTexIndex = pContext->uiFreeTexIndex;
	GLsizei ii, jj;

	if( (n == 0) || (textures == NULL) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	/* TODO: Only initiate if a render is pending that uses this texture */
	GL_P_InitiateRender( pContext, GL_TRUE );

	for( ii = 0; ii < n; ++ii )
	{
		GL_P_Texture *pTexture = pContext->apTextures[textures[ii]];
		if( (GLuint) pTexture > 1 )
		{
			if( pTexture == pContext->pTexture )
				glBindTexture( GL_TEXTURE_2D, 0 );

			for( jj = 0; jj < GL_P_TEXTURE_LEVEL_MAX; ++jj )
			{
				if( pTexture->aLevels[jj].pvMemory )
#if BMEM_TEXTURE_ALLOCATION
				{
					if (ii > 0)
						BMEM_Free( pContext->hMem, pTexture->aLevels[jj].pvMemory );
					else
						BKNI_Free( pTexture->aLevels[jj].pvMemory );
				}
#else /* !proxy */
					BKNI_Free( pTexture->aLevels[jj].pvMemory );
#endif /* BMEM_TEXTURE_ALLOCATION */

				if( jj && pTexture->aLevels[jj].hSurface )
				{
					BSUR_Surface_Destroy( pTexture->aLevels[jj].hSurface );
				}
			}

			if( pTexture->hSurface && !pTexture->pSurface )
				BSUR_Surface_Destroy( pTexture->hSurface );

			if( pTexture->pvMemory )
				BMEM_Free( pContext->hMem, pTexture->pvMemory );

			BKNI_Free( (void *) pTexture );
		}

		pContext->apTextures[textures[ii]] = 0;
		uiMinTexIndex = GL_P_MIN(uiMinTexIndex, textures[ii]);
	}

	pContext->uiFreeTexCount += n;
	pContext->uiFreeTexIndex = GL_P_MIN(pContext->uiFreeTexIndex, uiMinTexIndex);
}

/***************************************************************************/
GLAPI void APIENTRY glGenTextures( 
	GLsizei n, 
	GLuint *textures )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();
	GLsizei ii;

	if( (n == 0) || (textures == NULL) || (n > (GLsizei) pContext->uiFreeTexCount) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	for( ii = 0; ii < n; ++ii )
	{
		while( pContext->apTextures[pContext->uiFreeTexIndex] )
			pContext->uiFreeTexIndex++;

		pContext->apTextures[pContext->uiFreeTexIndex] = (GL_P_Texture *) 1;
		textures[ii] = pContext->uiFreeTexIndex;
	}

	pContext->uiFreeTexCount -= n;
}

/***************************************************************************/
GLAPI void APIENTRY glTexEnvf( 
	GLenum target, 
	GLenum pname, 
	GLfloat param )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( (target != GL_TEXTURE_ENV) && (pname != GL_TEXTURE_ENV_MODE) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	switch( (GLenum) param )
	{
	case GL_MODULATE:
		BP3D_SetSpecular( pContext->hP3d, pContext->bLighting );
		BP3D_SetTextureBlend( pContext->hP3d, BP3D_TextureBlend_ModulateAlpha );
		break;
	case GL_DECAL:
		BP3D_SetSpecular( pContext->hP3d, pContext->bLighting );
		BP3D_SetTextureBlend( pContext->hP3d, BP3D_TextureBlend_DecalAlpha );
		break;
	case GL_BLEND:
	case GL_REPLACE:
		BP3D_SetSpecular( pContext->hP3d, pContext->bLighting && (!pContext->bTexture2D) );
		BP3D_SetTextureBlend( pContext->hP3d, BP3D_TextureBlend_Decal );
		break;
	case GL_ADD:
		BP3D_SetSpecular( pContext->hP3d, pContext->bLighting );
		BP3D_SetTextureBlend( pContext->hP3d, BP3D_TextureBlend_Add );
		break;
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pContext->eTexEnvMode = (GLenum) param;
	BKNI_Memset( pContext->aiTextureMatrix, 0, sizeof (int32_t) * 20 );
}

/***************************************************************************/
GLAPI void APIENTRY glTexEnvfv( 
	GLenum target, 
	GLenum pname, 
	const GLfloat *params )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( pname == GL_TEXTURE_ENV_COLOR )
		pContext->ulTextureColor = GL_P_CONVERT_COLOR_FtoI(params[3], params[0], params[1], params[2]);
	else
		glTexEnvf( target, pname, *params );
}

/***************************************************************************/
GLAPI void APIENTRY glTexEnvx( 
	GLenum target, 
	GLenum pname, 
	GLfixed param )
{
	glTexEnvf( target, pname, (GLfloat) param );
}

/***************************************************************************/
GLAPI void APIENTRY glTexEnvxv( 
	GLenum target, 
	GLenum pname, 
	const GLfixed *params )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( pname == GL_TEXTURE_ENV_COLOR )
		pContext->ulTextureColor = GL_P_CONVERT_COLOR_XtoI(params[3], params[0], params[1], params[2]);
	else
		glTexEnvx( target, pname, *params );
}

/***************************************************************************/
GLAPI void APIENTRY glTexParameterf( 
	GLenum target, 
	GLenum pname, 
	GLfloat param )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();
	GL_P_Texture *pTexture = pContext->pTexture;

	if( target != GL_TEXTURE_2D )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( pTexture == NULL )
		return;

	switch( pname )
	{
	case GL_TEXTURE_MIN_FILTER:
		switch( (GLenum) param )
		{
		case GL_NEAREST:
			pTexture->bMipmap = GL_FALSE;
			BP3D_SetTextureMipmap( pContext->hP3d, (bool) pTexture->bMipmap, pTexture->iLevelCount - 1, 0.0 );
			BP3D_SetTextureFilter( pContext->hP3d, (pTexture->eTextureMagFilter == GL_NEAREST) ? 
				BP3D_TextureFilter_Point : BP3D_TextureFilter_Bilinear );
			break;
		case GL_LINEAR:
			pTexture->bMipmap = GL_FALSE;
			BP3D_SetTextureMipmap( pContext->hP3d, (bool) pTexture->bMipmap, pTexture->iLevelCount - 1, 0.0 );
			BP3D_SetTextureFilter( pContext->hP3d, BP3D_TextureFilter_Bilinear );
			break;
		case GL_NEAREST_MIPMAP_NEAREST:
		case GL_NEAREST_MIPMAP_LINEAR:
			pTexture->bMipmap = GL_TRUE;
			BP3D_SetTextureMipmap( pContext->hP3d, (bool) pTexture->bMipmap, pTexture->iLevelCount - 1, 0.0 );
			BP3D_SetTextureFilter( pContext->hP3d, (pTexture->eTextureMagFilter == GL_NEAREST) ? 
				BP3D_TextureFilter_Point : BP3D_TextureFilter_Bilinear );
			break;
		case GL_LINEAR_MIPMAP_NEAREST:
		case GL_LINEAR_MIPMAP_LINEAR:
			pTexture->bMipmap = GL_TRUE;
			BP3D_SetTextureMipmap( pContext->hP3d, (bool) pTexture->bMipmap, pTexture->iLevelCount - 1, 0.0 );
			BP3D_SetTextureFilter( pContext->hP3d, BP3D_TextureFilter_Bilinear );
			break;
		default:
			GL_P_SET_ERROR(GL_INVALID_ENUM);
			return;
		}
		pTexture->eTextureMinFilter = (GLenum) param;
		break;

	case GL_TEXTURE_MAG_FILTER:
		switch( (GLenum) param )
		{
		case GL_NEAREST:
			BP3D_SetTextureFilter( pContext->hP3d, ((pTexture->eTextureMinFilter & 1) == 0) ? 
				BP3D_TextureFilter_Point : BP3D_TextureFilter_Bilinear );
			break;
		case GL_LINEAR:
			BP3D_SetTextureFilter( pContext->hP3d, BP3D_TextureFilter_Bilinear );
			break;
		default:
			GL_P_SET_ERROR(GL_INVALID_ENUM);
			return;
		}
		pTexture->eTextureMagFilter = (GLenum) param;
		break;

	case GL_TEXTURE_WRAP_S:
		switch( (GLenum) param )
		{
		case GL_CLAMP_TO_EDGE:
			BP3D_SetTextureWrap( pContext->hP3d, BP3D_TextureWrap_Clamp );
			break;
		case GL_REPEAT:
			BP3D_SetTextureWrap( pContext->hP3d, (pTexture->eTextureWrapT == GL_REPEAT) ? 
				BP3D_TextureWrap_Tile : BP3D_TextureWrap_Clamp );
			break;
		default:
			GL_P_SET_ERROR(GL_INVALID_ENUM);
			return;
		}
		pTexture->eTextureWrapS = (GLenum) param;
		break;

	case GL_TEXTURE_WRAP_T:
		switch( (GLenum) param )
		{
		case GL_CLAMP_TO_EDGE:
			BP3D_SetTextureWrap( pContext->hP3d, BP3D_TextureWrap_Clamp );
			break;
		case GL_REPEAT:
			BP3D_SetTextureWrap( pContext->hP3d, (pTexture->eTextureWrapS == GL_REPEAT) ? 
				BP3D_TextureWrap_Tile : BP3D_TextureWrap_Clamp );
			break;
		default:
			GL_P_SET_ERROR(GL_INVALID_ENUM);
			return;
		}
		pTexture->eTextureWrapT = (GLenum) param;
		break;

	case GL_GENERATE_MIPMAP:
		pTexture->bGenerateMipmaps = (GLboolean) param;
		break;

	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}
}

/***************************************************************************/
GLAPI void APIENTRY glTexParameterx( 
	GLenum target, 
	GLenum pname, 
	GLfixed param )
{
	glTexParameterf( target, pname, (GLfloat) param );
}

/* End of File */
