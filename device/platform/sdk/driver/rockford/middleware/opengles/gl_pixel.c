/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_pixel.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 7/4/09 5:24p $
 *
 * Module Description: Pixel entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_pixel.c $
 * 
 * Hydra_Software_Devel/8   7/4/09 5:24p nissen
 * PR 56587: Added support for reading packed formats.
 * 
 * Hydra_Software_Devel/7   11/15/07 8:12p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/6   11/11/05 3:49p nissen
 * PR 17423: Added support for using Settop API.
 * 
 * Hydra_Software_Devel/5   3/9/05 3:17p nissen
 * PR 12813: Added support for more render and texture formats.
 * 
 * Hydra_Software_Devel/4   9/17/04 3:04p nissen
 * PR 9132: Changed parameters for calling initiate render function.
 * 
 * Hydra_Software_Devel/3   9/7/04 3:51p nissen
 * PR 9132: Added buffer swapping support.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:53p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glPixelStorei( 
	GLenum pname, 
	GLint param )
{
	BSTD_UNUSED( pname );
	BSTD_UNUSED( param );
}

/***************************************************************************/
GLAPI void APIENTRY glReadPixels( 
	GLint x, 
	GLint y, 
	GLsizei width, 
	GLsizei height, 
	GLenum format, 
	GLenum type, 
	GLvoid *pixels )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();
	EGL_P_Surface *pSurface = pDisplay->pDrawSurface;
	BSUR_Surface_Handle hSurface = pSurface->hRenderSurface;

	if( (format != GL_RGBA) && (format != GL_RGB) && (format != GL_LUMINANCE) && 
		(format != GL_LUMINANCE_ALPHA) && (format != GL_ALPHA) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( (type != GL_UNSIGNED_BYTE) && (type != GL_UNSIGNED_SHORT_5_6_5) && 
		(type != GL_UNSIGNED_SHORT_5_5_5_1) && (type != GL_UNSIGNED_SHORT_4_4_4_4) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( (x < 0) || (y < 0) || (width < 0) || (height < 0) || (pixels == NULL) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( (width == 0) || (height == 0) )
		return;

	if( pDisplay->pDrawSurface && hSurface )
	{
		BPXL_Format eFormat;
		void *pvMemory;
		GLuint ulPitch;
		GLuint ulWidth;
		GLuint ulHeight;
		GLuint *pulDst;
		GLuint *pulSrc;
		GLushort *pusDst;
		GLushort *pusSrc;
		GLint ii, jj;

		EGL_P_InitiateRender( pDisplay, pContext, GL_TRUE );

		BSUR_Surface_GetFormat( hSurface, &eFormat );
		BSUR_Surface_GetAddress( hSurface, &pvMemory, &ulPitch );
		BSUR_Surface_GetDimensions( hSurface, &ulWidth, &ulHeight );

		pulDst = (GLuint *) pixels;
		pusDst = (GLushort *) pixels;
		pulSrc = (GLuint *) ((GLubyte *) pvMemory + (ulHeight - y - 1) * ulPitch) + x;
		pusSrc = (GLushort *) ((GLubyte *) pvMemory + (ulHeight - y - 1) * ulPitch) + x;

		if( type == GL_UNSIGNED_BYTE )
		{
			/* convert to RGBA8888 */
			switch( eFormat )
			{
			case BPXL_eA8_R8_G8_B8:
				for( jj = 0; jj < height; ++jj )
				{
					for( ii = 0; ii < width; ++ii )
					{
						GLuint ulSrc = pulSrc[ii];
						pulDst[ii] = (ulSrc & 0xFF00FF00) | ((ulSrc >> 16) & 0xFF) | ((ulSrc & 0xFF) << 16);
					}
					pulDst += width;
					pulSrc -= ulPitch / 4;
				}
				break;

			case BPXL_eR5_G6_B5:
				for( jj = 0; jj < height; ++jj )
				{
					for( ii = 0; ii < width; ++ii )
					{
						GLuint ulSrc = (GLuint) pusSrc[ii];
						pulDst[ii] = 0xFF000000 | 
							((ulSrc & 0xF800) >>  8) | ((ulSrc & 0xE000) >> 13) | 
							((ulSrc & 0x07E0) <<  5) | ((ulSrc & 0x0600) >>  1) | 
							((ulSrc & 0x001F) << 19) | ((ulSrc & 0x001C) << 14);
					}
					pulDst += width;
					pusSrc -= ulPitch / 2;
				}
				break;

			case BPXL_eA4_R4_G4_B4:
				for( jj = 0; jj < height; ++jj )
				{
					for( ii = 0; ii < width; ++ii )
					{
						GLuint ulSrc = (GLuint) pusSrc[ii];
						pulDst[ii] = 
							((ulSrc & 0xF000) << 16) | ((ulSrc & 0xF000) << 12) | 
							((ulSrc & 0x0F00) >>  4) | ((ulSrc & 0x0F00) >>  8) | 
							((ulSrc & 0x00F0) <<  8) | ((ulSrc & 0x00F0) <<  4) | 
							((ulSrc & 0x000F) << 20) | ((ulSrc & 0x000F) << 16);
					}
					pulDst += width;
					pusSrc -= ulPitch / 2;
				}
				break;

			case BPXL_eA1_R5_G5_B5:
				for( jj = 0; jj < height; ++jj )
				{
					for( ii = 0; ii < width; ++ii )
					{
						GLuint ulSrc = (GLuint) pusSrc[ii];
						pulDst[ii] = ((ulSrc & 0x8000) ? 0xFF000000 : 0) | 
							((ulSrc & 0x7C00) >>  7) | ((ulSrc & 0x7000) >> 12) | 
							((ulSrc & 0x03E0) <<  6) | ((ulSrc & 0x0380) <<  1) | 
							((ulSrc & 0x001F) << 19) | ((ulSrc & 0x001C) << 14);
					}
					pulDst += width;
					pusSrc -= ulPitch / 2;
				}
				break;

			default:
				break;
			}
		}
		else if( type == GL_UNSIGNED_SHORT_4_4_4_4 )
		{
			/* convert from ARGB4444 to RGBA4444 */
			for( jj = 0; jj < height; ++jj )
			{
				for( ii = 0; ii < width; ++ii )
				{
					GLushort usSrc = pusSrc[ii];
					pusDst[ii] = ((usSrc & 0xF000) >> 12) | ((usSrc & 0x0FFF) << 4);
				}
				pusDst += width;
				pusSrc -= ulPitch / 2;
			}
		}
		else if( type == GL_UNSIGNED_SHORT_5_5_5_1 )
		{
			/* convert from ARGB1555 to RGBA5551 */
			for( jj = 0; jj < height; ++jj )
			{
				for( ii = 0; ii < width; ++ii )
				{
					GLushort usSrc = pusSrc[ii];
					pusDst[ii] = ((usSrc & 0x8000) >> 15) | ((usSrc & 0x7FFF) << 1);
				}
				pusDst += width;
				pusSrc -= ulPitch / 2;
			}
		}
		else/* if( type == GL_UNSIGNED_SHORT_5_6_5 )*/
		{
			/* copy from RGB565 to RGB565 */
			for( jj = 0; jj < height; ++jj )
			{
				for( ii = 0; ii < width; ++ii )
					pusDst[ii] = pusSrc[ii];
				pusDst += width;
				pusSrc -= ulPitch / 2;
			}
		}
	}
}

/* End of File */
