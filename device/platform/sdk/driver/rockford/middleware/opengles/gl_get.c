/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_get.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 7/4/09 5:20p $
 *
 * Module Description: Get entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_get.c $
 * 
 * Hydra_Software_Devel/6   7/4/09 5:20p nissen
 * PR 56587: Added support for palette textures for OpenGL conformance.
 * 
 * Hydra_Software_Devel/5   3/9/05 2:58p nissen
 * PR 12818: Fixed support for triple buffer flipping.
 * 
 * Hydra_Software_Devel/4   9/7/04 3:47p nissen
 * PR 9132: Added buffer swapping support.
 * 
 * Hydra_Software_Devel/3   8/10/04 2:31p nissen
 * PR 9132: Increased number of compressed texture formats returned in get
 * function.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:26p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glGetIntegerv(
	GLenum pname, 
	GLint *params )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();
	EGL_P_Surface *pSurface = pDisplay->pDrawSurface;

	switch( pname )
	{
	case GL_ALPHA_BITS:
		params[0] = EGL_P_ALPHA_SIZE(pContext->config);
		break;
	case GL_RED_BITS:
		params[0] = EGL_P_RED_SIZE(pContext->config);
		break;
	case GL_GREEN_BITS:
		params[0] = EGL_P_GREEN_SIZE(pContext->config);
		break;
	case GL_BLUE_BITS:
		params[0] = EGL_P_BLUE_SIZE(pContext->config);
		break;
	case GL_DEPTH_BITS:
		params[0] = EGL_P_DEPTH_SIZE(pContext->config);
		break;
	case GL_STENCIL_BITS:
		params[0] = 0;
		break;
	case GL_ALIASED_POINT_SIZE_RANGE:
	case GL_ALIASED_LINE_WIDTH_RANGE:
	case GL_SMOOTH_POINT_SIZE_RANGE:
	case GL_SMOOTH_LINE_WIDTH_RANGE:
		params[0] = 1;
		params[1] = 1;
		break;
	case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
		params[0] = EGL_P_ALPHA_SIZE(pContext->config) ? GL_RGBA : GL_RGB;
		break;
	case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
		switch( EGL_P_GREEN_SIZE(pContext->config) )
		{
		case 4: params[0] = GL_UNSIGNED_SHORT_4_4_4_4; break;
		case 5: params[0] = GL_UNSIGNED_SHORT_5_5_5_1; break;
		case 6: params[0] = GL_UNSIGNED_SHORT_5_6_5; break;
		case 8: params[0] = GL_UNSIGNED_BYTE; break;
		}
		break;
	case GL_MAX_ELEMENTS_INDICES:
		params[0] = GL_P_VERTEX_MAX;
		break;
	case GL_MAX_ELEMENTS_VERTICES:
		params[0] = GL_P_VERTEX_MAX;
		break;
	case GL_MAX_LIGHTS:
		params[0] = GL_P_LIGHT_MAX;
		break;
	case GL_MAX_MODELVIEW_STACK_DEPTH:
		params[0] = GL_P_MATRIX_STACK_DEPTH;
		break;
	case GL_MAX_PROJECTION_STACK_DEPTH:
		params[0] = GL_P_MATRIX_STACK_DEPTH;
		break;
	case GL_MAX_TEXTURE_STACK_DEPTH:
		params[0] = GL_P_MATRIX_STACK_DEPTH;
		break;
	case GL_MAX_TEXTURE_SIZE:
		params[0] = GL_P_TEXTURE_SIZE_MAX;
		break;
	case GL_MAX_TEXTURE_UNITS:
		params[0] = GL_P_TEXTURE_UNIT_MAX;
		break;
	case GL_MAX_VIEWPORT_DIMS:
		{
			BERR_Code err = BSUR_Surface_GetDimensions( pSurface->hRenderSurface,
				(uint32_t *) (&params[0]), (uint32_t *) (&params[1]) );
			if( err != BERR_SUCCESS )
				GL_P_SET_ERROR(GL_INVALID_ENUM);
		}
		break;
	case GL_COMPRESSED_TEXTURE_FORMATS:
		params[0] = GL_PALETTE4_RGB8_OES;
		params[1] = GL_PALETTE4_RGBA8_OES;
		params[2] = GL_PALETTE4_R5_G6_B5_OES;
		params[3] = GL_PALETTE4_RGBA4_OES;
		params[4] = GL_PALETTE4_RGB5_A1_OES;
		params[5] = GL_PALETTE8_RGB8_OES;
		params[6] = GL_PALETTE8_RGBA8_OES;
		params[7] = GL_PALETTE8_R5_G6_B5_OES;
		params[8] = GL_PALETTE8_RGBA4_OES;
		params[9] = GL_PALETTE8_RGB5_A1_OES;
		break;
	case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
		params[0] = 10;
		break;
	case GL_SUBPIXEL_BITS:
		params[0] = 0;
		break;
	default: 
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}
}

/***************************************************************************/
GLAPI const GLubyte * APIENTRY glGetString( 
	GLenum name )
{
	switch( name )
	{
	case GL_VENDOR:     return (GLubyte *) GL_P_VENDOR;
	case GL_RENDERER:   return (GLubyte *) GL_P_RENDERER;
	case GL_VERSION:    return (GLubyte *) GL_P_VERSION;
	case GL_EXTENSIONS: return (GLubyte *) GL_P_EXTENSIONS;
	default: 
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}

	return NULL;
}

/* End of File */
