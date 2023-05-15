/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_texture.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 8/6/10 4:06p $
 *
 * Module Description: Texture entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_texture.c $
 * 
 * Hydra_Software_Devel/20   8/6/10 4:06p nissen
 * SW7400-2378: Fixed warnings.
 * 
 * Hydra_Software_Devel/19   8/6/10 1:25p nissen
 * SW7400-2378: Added empty functions for glTexParameteriv/xv.
 * 
 * Hydra_Software_Devel/18   7/23/09 12:59p nissen
 * PR 56059: Removed BMEM texture allocations for Kernel Mode proxy
 * version.
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

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glBindTexture( 
	GLenum target, 
	GLuint texture )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( ((target != GL_TEXTURE_2D) && (target != GL_TEXTURE_VIDEO) && (target != GL_TEXTURE_SWIZZLE)) || 
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
	BP3D_SetTextureSwizzle( pDisplay->hP3d, pContext->pTexture->bSwizzle ? true : false );
}

/***************************************************************************/
GLAPI void APIENTRY glDeleteTextures( 
	GLsizei n, 
	const GLuint *textures )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();
	GLuint uiMinTexIndex = pContext->uiFreeTexIndex;
	GLsizei ii, jj;

	if( (n == 0) || (textures == NULL) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	/* TODO: Only initiate if a render is pending that uses this texture */
	EGL_P_InitiateRender( pDisplay, pContext, GL_TRUE );

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
					BKNI_Free( pTexture->aLevels[jj].pvMemory );

				if( jj && pTexture->aLevels[jj].hSurface )
					BSUR_Surface_Destroy( pTexture->aLevels[jj].hSurface );
			}

			if( pTexture->hSurface && (!pTexture->bVideo) )
				BSUR_Surface_Destroy( pTexture->hSurface );

			if( pTexture->pvMemory )
				BMEM_Free( pDisplay->hMem, pTexture->pvMemory );

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
	EGL_P_Context *pContext = EGL_P_GetContext();
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
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (target != GL_TEXTURE_ENV) && (pname != GL_TEXTURE_ENV_MODE) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	switch( (GLenum) param )
	{
	case GL_MODULATE:
		BP3D_SetSpecular( pDisplay->hP3d, pContext->bLighting );
		BP3D_SetTextureBlend( pDisplay->hP3d, BP3D_TextureBlend_ModulateAlpha );
		break;
	case GL_DECAL:
		BP3D_SetSpecular( pDisplay->hP3d, pContext->bLighting );
		BP3D_SetTextureBlend( pDisplay->hP3d, BP3D_TextureBlend_DecalAlpha );
		break;
	case GL_BLEND:
	case GL_REPLACE:
		BP3D_SetSpecular( pDisplay->hP3d, pContext->bLighting && (!pContext->bTexture2D) );
		BP3D_SetTextureBlend( pDisplay->hP3d, BP3D_TextureBlend_Decal );
		break;
	case GL_ADD:
		BP3D_SetSpecular( pDisplay->hP3d, pContext->bLighting );
		BP3D_SetTextureBlend( pDisplay->hP3d, BP3D_TextureBlend_Add );
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
	EGL_P_Context *pContext = EGL_P_GetContext();

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
	EGL_P_Context *pContext = EGL_P_GetContext();

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
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();
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
			BP3D_SetTextureMipmap( pDisplay->hP3d, (bool) pTexture->bMipmap, pTexture->iLevelCount - 1, 0.0 );
			BP3D_SetTextureFilter( pDisplay->hP3d, (pTexture->eTextureMagFilter == GL_NEAREST) ? 
				BP3D_TextureFilter_Point : BP3D_TextureFilter_Bilinear );
			break;
		case GL_LINEAR:
			pTexture->bMipmap = GL_FALSE;
			BP3D_SetTextureMipmap( pDisplay->hP3d, (bool) pTexture->bMipmap, pTexture->iLevelCount - 1, 0.0 );
			BP3D_SetTextureFilter( pDisplay->hP3d, BP3D_TextureFilter_Bilinear );
			break;
		case GL_NEAREST_MIPMAP_NEAREST:
		case GL_NEAREST_MIPMAP_LINEAR:
			pTexture->bMipmap = GL_TRUE;
			BP3D_SetTextureMipmap( pDisplay->hP3d, (bool) pTexture->bMipmap, pTexture->iLevelCount - 1, 0.0 );
			BP3D_SetTextureFilter( pDisplay->hP3d, (pTexture->eTextureMagFilter == GL_NEAREST) ? 
				BP3D_TextureFilter_Point : BP3D_TextureFilter_Bilinear );
			break;
		case GL_LINEAR_MIPMAP_NEAREST:
		case GL_LINEAR_MIPMAP_LINEAR:
			pTexture->bMipmap = GL_TRUE;
			BP3D_SetTextureMipmap( pDisplay->hP3d, (bool) pTexture->bMipmap, pTexture->iLevelCount - 1, 0.0 );
			BP3D_SetTextureFilter( pDisplay->hP3d, BP3D_TextureFilter_Bilinear );
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
			BP3D_SetTextureFilter( pDisplay->hP3d, ((pTexture->eTextureMinFilter & 1) == 0) ? 
				BP3D_TextureFilter_Point : BP3D_TextureFilter_Bilinear );
			break;
		case GL_LINEAR:
			BP3D_SetTextureFilter( pDisplay->hP3d, BP3D_TextureFilter_Bilinear );
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
			BP3D_SetTextureWrap( pDisplay->hP3d, BP3D_TextureWrap_Clamp );
			break;
		case GL_REPEAT:
			BP3D_SetTextureWrap( pDisplay->hP3d, (pTexture->eTextureWrapT == GL_REPEAT) ? 
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
			BP3D_SetTextureWrap( pDisplay->hP3d, BP3D_TextureWrap_Clamp );
			break;
		case GL_REPEAT:
			BP3D_SetTextureWrap( pDisplay->hP3d, (pTexture->eTextureWrapS == GL_REPEAT) ? 
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

/***************************************************************************/
GLAPI void APIENTRY glTexParameteriv( 
	GLenum target, 
	GLenum pname, 
	GLint *param )
{
	BSTD_UNUSED( target );
	BSTD_UNUSED( pname );
	BSTD_UNUSED( param );
}

/***************************************************************************/
GLAPI void APIENTRY glTexParameterxv( 
	GLenum target, 
	GLenum pname, 
	GLfixed *param )
{
	BSTD_UNUSED( target );
	BSTD_UNUSED( pname );
	BSTD_UNUSED( param );
}

/* End of File */
