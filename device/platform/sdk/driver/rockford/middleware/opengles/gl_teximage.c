/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_teximage.c $
 * $brcm_Revision: Hydra_Software_Devel/35 $
 * $brcm_Date: 4/27/12 3:27p $
 *
 * Module Description: Texture Image entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_teximage.c $
 * 
 * Hydra_Software_Devel/35   4/27/12 3:27p nissen
 * SW7125-1283: Switched to using planes for video textures.
 * 
 * Hydra_Software_Devel/34   1/17/12 3:45p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/33   7/23/09 1:05p nissen
 * PR 56059: Removed BMEM texture allocations for Kernel Mode proxy
 * version.
 * 
 * Hydra_Software_Devel/32   7/4/09 5:25p nissen
 * PR 56587: Added support for palette textures. Fixed textures smaller
 * than 8x8 with mipmapping.
 * 
 * Hydra_Software_Devel/31   6/8/09 1:36p nissen
 * PR 54208: Fixed texture pointer dereference.
 * 
 * Hydra_Software_Devel/30   6/8/09 1:24p nissen
 * PR 54209: Fixed level parameter verification in glTexImage2D and
 * glCopyTexSubImage2D.
 * 
 * Hydra_Software_Devel/29   6/8/09 1:18p nissen
 * PR 54209: Fixed level parameter verification in glTexSubImage2D.
 * 
 * Hydra_Software_Devel/28   2/6/09 3:00p nissen
 * PR 41194: Fixed swizzling textues for 7420.
 * 
 * Hydra_Software_Devel/27   1/13/09 4:39p nissen
 * PR 41194: Fixed swizzling textues for 7405.
 * 
 * Hydra_Software_Devel/26   1/13/09 4:24p nissen
 * PR 41194: Fixed support for swizzling textues.
 * 
 * Hydra_Software_Devel/25   11/3/08 5:55p mphillip
 * PR47441: Texture allocation for kernel mode adjustments for 3548/3556
 * 
 * Hydra_Software_Devel/24   10/29/08 5:13p nissen
 * PR 41194: Fixed support for swizzling textues on 3548 B0.
 * 
 * Hydra_Software_Devel/23   10/6/08 12:53p nissen
 * PR 47303: Added support for calling new SUR surface create function.
 * 
 * Hydra_Software_Devel/22   9/8/08 1:04p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/21   8/19/08 9:58p nissen
 * PR 41194: Added support for swizzling textues on 3548/3556.
 * 
 * Hydra_Software_Devel/20   1/7/08 7:43p mphillip
 * PR35854: Mipmap texture needs to be allocated from the shared heap
 * 
 * Hydra_Software_Devel/19   11/15/07 8:18p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/18   9/10/07 11:48a nissen
 * PR 30062: Fixed mipmap problem when using video textures.
 * 
 * Hydra_Software_Devel/17   5/19/07 7:18a nissen
 * PR 30062: Fixed problem with auto generated mipmap levels.
 * 
 * Hydra_Software_Devel/16   5/17/07 12:33p nissen
 * PR 30062: Fixed problem with auto generated mipmap levels.
 * 
 * Hydra_Software_Devel/15   12/11/06 1:53p nissen
 * PR 20908: Added support for GL_ARGB textures.
 * 
 * Hydra_Software_Devel/14   12/8/06 2:51p nissen
 * PR 20908: Added support for copying only the dirty portion of a texture
 * to device memory.
 * 
 * Hydra_Software_Devel/13   12/7/06 11:55a nissen
 * PR 20908: Added support for using cached memory for internal textures
 * for faster copies.
 * 
 * Hydra_Software_Devel/12   9/14/06 3:30p nissen
 * PR 22858: Fixed problem with setting mipmap parameters.
 * 
 * Hydra_Software_Devel/11   6/5/06 12:23p nissen
 * PR 20908: Added support for texture swizzling.
 * 
 * Hydra_Software_Devel/10   1/6/06 9:42a nissen
 * PR 17656: Added support for using Settop API video captured textures.
 * 
 * Hydra_Software_Devel/9   11/18/05 1:43p nissen
 * PR 17423: Fixed texture memory allocation alignment.
 * 
 * Hydra_Software_Devel/8   11/11/05 4:07p nissen
 * PR 17423: Added video texture support.
 * 
 * Hydra_Software_Devel/7   10/17/05 9:18p nissen
 * PR 17656: Added support for generating mipmap lods.
 * 
 * Hydra_Software_Devel/6   3/9/05 3:14p nissen
 * PR 12813: Added support for more render and texture formats.
 * 
 * Hydra_Software_Devel/5   9/17/04 2:59p nissen
 * PR 9132: Changed parameters for calling initiate render function.
 * 
 * Hydra_Software_Devel/4   9/7/04 3:54p nissen
 * PR 9132: Added buffer swapping support.
 * 
 * Hydra_Software_Devel/3   8/10/04 2:13p nissen
 * PR 9132: Fixed spacing in code.
 * 
 * Hydra_Software_Devel/2   7/28/04 2:15p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "native_private.h"

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
BPXL_Format GL_P_ConvertFormat_GLtoP3D( 
	GLenum format, 
	GLenum type )
{
	switch( type )
	{
	case GL_UNSIGNED_BYTE:
		switch( format )
		{
		case GL_RGB:
			return BPXL_eR5_G6_B5;
		case GL_RGBA:
		case GL_ARGB:
			return BPXL_eA8_R8_G8_B8;
		case GL_LUMINANCE:
			return BPXL_eL8;
		case GL_ALPHA:
		case GL_LUMINANCE_ALPHA:
			return BPXL_eL8_A8;
		case GL_PALETTE4_RGB8_OES:
		case GL_PALETTE4_RGBA8_OES:
		case GL_PALETTE8_RGB8_OES:
		case GL_PALETTE8_RGBA8_OES:
			return BPXL_eA8_R8_G8_B8;
		case GL_PALETTE4_R5_G6_B5_OES:
		case GL_PALETTE8_R5_G6_B5_OES:
			return BPXL_eR5_G6_B5;
		case GL_PALETTE4_RGBA4_OES:
		case GL_PALETTE8_RGBA4_OES:
			return BPXL_eA4_R4_G4_B4;
		case GL_PALETTE4_RGB5_A1_OES:
		case GL_PALETTE8_RGB5_A1_OES:
			return BPXL_eA1_R5_G5_B5;
		default: 
			GL_P_SET_ERROR(GL_INVALID_ENUM);
		}
		break;

	case GL_UNSIGNED_SHORT_5_6_5:
		if( format != GL_RGB )
		{
			GL_P_SET_ERROR(GL_INVALID_OPERATION);
			return 0;
		}
		return BPXL_eR5_G6_B5;

	case GL_UNSIGNED_SHORT_4_4_4_4:
		if( format != GL_RGBA )
		{
			GL_P_SET_ERROR(GL_INVALID_OPERATION);
			return 0;
		}
		return BPXL_eA4_R4_G4_B4;

	case GL_UNSIGNED_SHORT_5_5_5_1:
		if( format != GL_RGBA )
		{
			GL_P_SET_ERROR(GL_INVALID_OPERATION);
			return 0;
		}
		return BPXL_eA1_R5_G5_B5;

	default: 
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}

	return 0;
}

/***************************************************************************/
void GL_P_ConvertPixels_GLtoP3D( 
	GLvoid *pvDst, 
	GLvoid *pvSrc, 
	GLenum format, 
	GLenum type, 
	GLint xoffset, 
	GLint yoffset, 
	GLsizei width, 
	GLsizei height,
	GLsizei pitch )
{
	GLsizei ii, jj;

	switch( type )
	{
	case GL_UNSIGNED_BYTE:
		switch( format )
		{
		case GL_RGB:
			/* GL_RGB to BPXL_eR5_G6_B5 */
			for( jj = 0; jj < height; ++jj )
			{
				uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + jj * width * 3;
				for( ii = 0; ii < width; ++ii )
					pusDst[ii] = 
						((((uint16_t) pucSrc[ii * 3 + 0]) & 0xF8) << 8) | 
						((((uint16_t) pucSrc[ii * 3 + 1]) & 0xFC) << 3) |
						((((uint16_t) pucSrc[ii * 3 + 2]) & 0xF8) >> 3);
			}
			break;

		case GL_RGBA:
			/* GL_RGBA to BPXL_eA8_R8_G8_B8 */
			for( jj = 0; jj < height; ++jj )
			{
				uint32_t *pulDst = (uint32_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + jj * width * 4;
				for( ii = 0; ii < width; ++ii )
					pulDst[ii] = 
						(((uint32_t) pucSrc[ii * 4 + 0]) << 16) |
						(((uint32_t) pucSrc[ii * 4 + 1]) << 8) |
						(((uint32_t) pucSrc[ii * 4 + 2]) << 0) |
						(((uint32_t) pucSrc[ii * 4 + 3]) << 24);
			}
			break;

		case GL_ARGB:
			/* GL_ARGB to BPXL_eA8_R8_G8_B8 */
			for( jj = 0; jj < height; ++jj )
			{
				uint32_t *pulDst = (uint32_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint32_t *pulSrc = (uint32_t *) pvSrc + jj * width;
				for( ii = 0; ii < width; ++ii )
					pulDst[ii] = pulSrc[ii];
			}
			break;

		case GL_ALPHA:
			/* GL_ALPHA to BPXL_eL8_A8 */
			for( jj = 0; jj < height; ++jj )
			{
				uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + jj * width;
				for( ii = 0; ii < width; ++ii )
					pusDst[ii] = (uint16_t) pucSrc[ii] | 0xFF00;
			}
			break;

		case GL_LUMINANCE:
			/* GL_LUMINANCE to BPXL_eL8 */
			for( jj = 0; jj < height; ++jj )
			{
				uint8_t *pucDst = (uint8_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + jj * width;
				BKNI_Memcpy( pucDst, pucSrc, width );
			}
			break;

		case GL_LUMINANCE_ALPHA:
			/* GL_LUMINANCE_ALPHA to BPXL_eL8_A8 */
			for( jj = 0; jj < height; ++jj )
			{
				uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint16_t *pusSrc = (uint16_t *) pvSrc + jj * width;
				BKNI_Memcpy( pusDst, pusSrc, width * 2 );
			}
			break;

		case GL_PALETTE4_RGB8_OES:
		{
			/* GL_PALETTE4_RGB8_OES to BPXL_eA8_R8_G8_B8 */
			uint32_t ulPaletteSize = 3 * 16;
			uint8_t *pucPal = (uint8_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint32_t *pulDst = (uint32_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width / 2;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = (pucSrc[ii / 2] >> (((~ii) & 1) * 4)) & 0xF;
					uint32_t ulR = (uint32_t) pucPal[ucIndex * 3];
					uint32_t ulG = (uint32_t) pucPal[ucIndex * 3 + 1];
					uint32_t ulB = (uint32_t) pucPal[ucIndex * 3 + 2];
					pulDst[ii] = (ulR << 16) | (ulG << 8) | ulB | 0xFF000000;
				}
			}
			break;
		}
		case GL_PALETTE4_RGBA8_OES:
		{
			/* GL_PALETTE4_RGBA8_OES to BPXL_eA8_R8_G8_B8 */
			uint32_t ulPaletteSize = 4 * 16;
			uint32_t *pulPal = (uint32_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint32_t *pulDst = (uint32_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width / 2;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = (pucSrc[ii / 2] >> (((~ii) & 1) * 4)) & 0xF;
					uint32_t ulSrc = pulPal[ucIndex];
					pulDst[ii] = (ulSrc & 0xFF00FF00) | ((ulSrc >> 16) & 0xFF) | ((ulSrc & 0xFF) << 16);
				}
			}
			break;
		}
		case GL_PALETTE8_RGB8_OES:
		{
			/* GL_PALETTE8_RGB8_OES to BPXL_eA8_R8_G8_B8 */
			uint32_t ulPaletteSize = 3 * 256;
			uint8_t *pucPal = (uint8_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint32_t *pulDst = (uint32_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = pucSrc[ii];
					uint32_t ulR = (uint32_t) pucPal[ucIndex * 3];
					uint32_t ulG = (uint32_t) pucPal[ucIndex * 3 + 1];
					uint32_t ulB = (uint32_t) pucPal[ucIndex * 3 + 2];
					pulDst[ii] = (ulR << 16) | (ulG << 8) | ulB | 0xFF000000;
				}
			}
			break;
		}
		case GL_PALETTE8_RGBA8_OES:
		{
			/* GL_PALETTE8_RGBA8_OES to BPXL_eA8_R8_G8_B8 */
			uint32_t ulPaletteSize = 4 * 256;
			uint32_t *pulPal = (uint32_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint32_t *pulDst = (uint32_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = pucSrc[ii];
					uint32_t ulSrc = pulPal[ucIndex];
					pulDst[ii] = (ulSrc & 0xFF00FF00) | ((ulSrc >> 16) & 0xFF) | ((ulSrc & 0xFF) << 16);
				}
			}
			break;
		}
		case GL_PALETTE4_R5_G6_B5_OES:
		{
			/* GL_PALETTE4_R5_G6_B5_OES to BPXL_eR5_G6_B5 */
			uint32_t ulPaletteSize = 2 * 16;
			uint16_t *pusPal = (uint16_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width / 2;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = (pucSrc[ii / 2] >> (((~ii) & 1) * 4)) & 0xF;
					uint16_t usSrc = pusPal[ucIndex];
					pusDst[ii] = usSrc;
				}
			}
			break;
		}
		case GL_PALETTE8_R5_G6_B5_OES:
		{
			/* GL_PALETTE8_R5_G6_B5_OES to BPXL_eR5_G6_B5 */
			uint32_t ulPaletteSize = 2 * 256;
			uint16_t *pusPal = (uint16_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = pucSrc[ii];
					uint16_t usSrc = pusPal[ucIndex];
					pusDst[ii] = usSrc;
				}
			}
			break;
		}
		case GL_PALETTE4_RGBA4_OES:
		{
			/* GL_PALETTE4_RGBA4_OES to BPXL_eA4_R4_G4_B4 */
			uint32_t ulPaletteSize = 2 * 16;
			uint16_t *pusPal = (uint16_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width / 2;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = (pucSrc[ii / 2] >> (((~ii) & 1) * 4)) & 0xF;
					uint16_t usSrc = pusPal[ucIndex];
					pusDst[ii] = (usSrc >> 4) | (usSrc << 12);
				}
			}
			break;
		}
		case GL_PALETTE8_RGBA4_OES:
		{
			/* GL_PALETTE8_RGBA4_OES to BPXL_eA4_R4_G4_B4 */
			uint32_t ulPaletteSize = 2 * 256;
			uint16_t *pusPal = (uint16_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = pucSrc[ii];
					uint16_t usSrc = pusPal[ucIndex];
					pusDst[ii] = (usSrc >> 4) | (usSrc << 12);
				}
			}
			break;
		}
		case GL_PALETTE4_RGB5_A1_OES:
		{
			/* GL_PALETTE4_RGB5_A1_OES to BPXL_eA4_R4_G4_B4 */
			uint32_t ulPaletteSize = 2 * 16;
			uint16_t *pusPal = (uint16_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width / 2;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = (pucSrc[ii / 2] >> (((~ii) & 1) * 4)) & 0xF;
					uint16_t usSrc = pusPal[ucIndex];
					pusDst[ii] = (usSrc >> 1) | (usSrc << 15);
				}
			}
			break;
		}
		case GL_PALETTE8_RGB5_A1_OES:
		{
			/* GL_PALETTE8_RGB5_A1_OES to BPXL_eA4_R4_G4_B4 */
			uint32_t ulPaletteSize = 2 * 256;
			uint16_t *pusPal = (uint16_t *) pvSrc;
			for( jj = 0; jj < height; ++jj )
			{
				uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
				uint8_t *pucSrc = (uint8_t *) pvSrc + ulPaletteSize + jj * width;
				for( ii = 0; ii < width; ++ii )
				{
					uint8_t ucIndex = pucSrc[ii];
					uint16_t usSrc = pusPal[ucIndex];
					pusDst[ii] = (usSrc >> 1) | (usSrc << 15);
				}
			}
			break;
		}
		}
		break;

	case GL_UNSIGNED_SHORT_5_6_5:
		/* GL_UNSIGNED_SHORT_5_6_5 to BPXL_eR5_G6_B5 */
		for( jj = 0; jj < height; ++jj )
		{
			uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
			uint16_t *pusSrc = (uint16_t *) pvSrc + jj * width;
			for( ii = 0; ii < width; ++ii )
				pusDst[ii] = pusSrc[ii];
		}
		break;

	case GL_UNSIGNED_SHORT_4_4_4_4:
		/* GL_UNSIGNED_SHORT_4_4_4_4 to BPXL_eA4_R4_G4_B4 */
		for( jj = 0; jj < height; ++jj )
		{
			uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
			uint16_t *pusSrc = (uint16_t *) pvSrc + jj * width;
			for( ii = 0; ii < width; ++ii )
				pusDst[ii] = (pusSrc[ii] >> 4) | (pusSrc[ii] << 12);
		}
		break;

	case GL_UNSIGNED_SHORT_5_5_5_1:
		/* GL_UNSIGNED_SHORT_5_5_5_1 to BPXL_eA1_R5_G5_B5 */
		for( jj = 0; jj < height; ++jj )
		{
			uint16_t *pusDst = (uint16_t *) pvDst + xoffset + (yoffset + jj) * pitch;
			uint16_t *pusSrc = (uint16_t *) pvSrc + jj * width;
			for( ii = 0; ii < width; ++ii )
				pusDst[ii] = (pusSrc[ii] >> 1) | (pusSrc[ii] << 15);
		}
		break;
	}
}

/***************************************************************************/
GLboolean GL_P_SwizzleTexture( 
	GLvoid *pvDst,
	GLvoid *pvSrc,
	GLint iWidth,
	GLint iHeight,
	GLint iBpp )
{
	GLubyte *pucDst = (GLubyte *) pvDst;
	GLubyte *pucSrc = (GLubyte *) pvSrc;
	GLushort *pusDst = (GLushort *) pvDst;
	GLushort *pusSrc = (GLushort *) pvSrc;
	GLuint *puiDst = (GLuint *) pvDst;
	GLuint *puiSrc = (GLuint *) pvSrc;

	GLuint uiWidthShift = 0;
	GLint iTmpWidth = iWidth;
	GLint ii, jj;

#if ((BCHP_CHIP==7400) || (BCHP_CHIP==7038))
	if( (iWidth < 16) || (iHeight < 8) )
		return GL_FALSE;
#else
	if( (iWidth < 64) || (iHeight < 32) )
		return GL_FALSE;
#endif

	while( iTmpWidth > 1 )
	{
		uiWidthShift++;
		iTmpWidth = iTmpWidth >> 1;
	}

	for( jj = 0; jj < iHeight; ++jj )
	{
		for( ii = 0; ii < iWidth; ++ii )
		{
#if ((BCHP_CHIP==7400) || (BCHP_CHIP==7038))
			GLuint uiSA = (ii & 0x007);
			GLuint uiSB = (iBpp == 2) ? (ii & 0x008) << 4 : ((ii & 0xFF8) << 4);
			GLuint uiSC = (iBpp == 2) ? (ii & 0xFF0) << 4 : 0;
			GLuint uiTA = (jj & 0x00E) << 2;
			GLuint uiTB = (jj & 0x001) << 6;
			GLuint uiTC = (jj & 0xFF0) << uiWidthShift;

			GLuint uiSrcIndex = (jj * iWidth) + ii;
			GLuint uiDstIndex = uiSA | uiSB | uiSC | uiTA | uiTB | uiTC;

			switch( iBpp )
			{
			case 1: pucDst[uiDstIndex] = pucSrc[uiSrcIndex]; break;
			case 2: pusDst[uiDstIndex] = pusSrc[uiSrcIndex]; break;
			case 4: puiDst[uiDstIndex] = puiSrc[uiSrcIndex]; break;
			}

#elif ((BCHP_CHIP==3548) && (BCHP_VER == BCHP_VER_A0)) || \
	  ((BCHP_CHIP==3556) && (BCHP_VER == BCHP_VER_A0)) || \
	  (BCHP_CHIP==7405)
			GLuint uiA = ((jj & (~0x1F)) >> 5) << (8 + uiWidthShift - 3);
			GLuint uiB = ((ii & (~0x07)) >> 3) << 8;
			GLuint uiC = ((jj & ( 0x01)) >> 0) << 7;
			GLuint uiD = ((jj & ( 0x1E)) >> 1) << 3;
			GLuint uiE = ((ii & ( 0x07)) >> 0) << 0;

			GLuint uiSrcIndex = (jj * iWidth) + ii;
			GLuint uiDstIndex = uiA | uiB | uiC | uiD | uiE;

			switch( iBpp )
			{
			case 1: pucDst[uiDstIndex] = pucSrc[uiSrcIndex]; break;
			case 2: pusDst[uiDstIndex] = pusSrc[uiSrcIndex]; break;
			case 4: puiDst[uiDstIndex] = puiSrc[uiSrcIndex]; break;
			}

#else
			GLuint uiA = ((jj & (~0x1F)) >> 5) << (9 + uiWidthShift - 4);
			GLuint uiB = ((ii & (~0x0F)) >> 4) << 9;
			GLuint uiC = ((jj & ( 0x01)) >> 0) << 8;
			GLuint uiD = ((jj & ( 0x1E)) >> 1) << 4;
			GLuint uiE = ((ii & ( 0x0F)) >> 0) << 0;

			GLuint uiSrcIndex = (jj * iWidth) + ii;
			GLuint uiDstIndex = uiA | uiB | uiC | uiD | uiE;

			switch( iBpp )
			{
			case 1: pucDst[uiDstIndex] = pucSrc[uiSrcIndex]; break;
			case 2: pusDst[uiDstIndex] = pusSrc[uiSrcIndex]; break;
			case 4: puiDst[uiDstIndex] = puiSrc[uiSrcIndex]; break;
			}
#endif
		}
	}
	return GL_TRUE;
}

/***************************************************************************/
GLboolean GL_P_LoadTexture( 
	EGL_P_Display *pDisplay,
	EGL_P_Context *pContext,
	GL_P_Texture *pTexture )
{
	BERR_Code err;
	GLint iWidth;
	GLint iHeight;
	GLint iBpp;
	GLint iLevelCount = 0;
	GLint iTextureSize = 0;
	GLint iLevelOffset = 0;
	GLint iLodOffset = 0;
	GLint ii;

	if( (pContext->bTexture2D == GL_FALSE) || (pTexture == NULL) )
	{
		BP3D_SetTextureSurface( pDisplay->hP3d, 0 );
		return GL_TRUE;
	}

	iWidth = pTexture->iWidth;
	iHeight = pTexture->iHeight;
	iBpp = BPXL_BITS_PER_PIXEL(pTexture->eFormat) / 8;

	if( pTexture->bVideo )
	{
		BSUR_Surface_Handle hSurface;
		unsigned int uiFormat;
		unsigned int uiOffset;
		void *pvMemory;
		int iPitch;
		int iWidth;
		int iHeight;

		native_P_InitPixmap( (NativePixmap) pTexture->hSurface, &hSurface, &uiFormat, &uiOffset, &pvMemory, &iPitch, &iWidth, &iHeight );
		BP3D_SetTexturePlane( pDisplay->hP3d, uiFormat, uiOffset, iPitch, iWidth, iHeight );
		BP3D_SetTextureMipmap( pDisplay->hP3d, false, 0, 0.0 );
		return GL_TRUE;
	}

	/* calculate texture surface memory size */
	for( ii = 0; ii < GL_P_TEXTURE_LEVEL_MAX; ++ii )
	{
		GL_P_MipmapLevel *pLevel = &pTexture->aLevels[ii];
		GLint iNextWidth = GL_P_MAX(iWidth / 2, 1);
		GLint iNextHeight = GL_P_MAX(iHeight / 2, 1);

		if( (pLevel->pvMemory == NULL) && ((pDisplay->bGenerateMipmaps == GL_FALSE) || (pTexture->bGenerateMipmaps == GL_FALSE)) )
			break;

		pLevel->iOffset = iTextureSize;
		pLevel->iWidth = iWidth;
		pLevel->iHeight = iHeight;
		pLevel->iSize = iWidth * iHeight * iBpp;

		iTextureSize += pLevel->iSize;
		iLevelCount++;

		if( (iWidth == iNextWidth) && (iHeight == iNextHeight) )
			break;

		iWidth = iNextWidth;
		iHeight = iNextHeight;
	}

	if( iTextureSize == 0 )
		return GL_FALSE;

	/* add extra level memory for mipmap textures less than 8x8 */
	if( (iLevelCount > 1) && ((pTexture->iWidth < 8) || (pTexture->iHeight < 8)) )
	{
		iWidth = pTexture->iWidth;
		iHeight = pTexture->iHeight;

		while( (iWidth < 8) || (iHeight < 8) )
		{
			iWidth *= 2;
			iHeight *= 2;
			iTextureSize += iWidth * iHeight * iBpp;
			iLevelOffset += iWidth * iHeight * iBpp;
			iLodOffset++;
		}
	}
	else
	{
		/* set minimum texture size of 8x8 for textures that are not mipmapped */
		iWidth = ((iLevelCount == 1) && (pTexture->iWidth < 8)) ? 8 : pTexture->iWidth;
		iHeight = ((iLevelCount == 1) && (pTexture->iHeight < 8)) ? 8 : pTexture->iHeight;
		iTextureSize = GL_P_MAX(iTextureSize, iWidth * iHeight * iBpp );
	}

	/* free previously allocated texture surface and memory if unsuitable */
	if( (iTextureSize != pTexture->iSize) || (iLevelCount != pTexture->iLevelCount) )
	{
		pTexture->iSize = iTextureSize;
		pTexture->iLevelCount = iLevelCount;

		for( ii = 1; ii < pTexture->iLevelCount; ++ii )
		{
			if( pTexture->aLevels[ii].hSurface )
			{
				BSUR_Surface_Destroy( pTexture->aLevels[ii].hSurface );
				pTexture->aLevels[ii].hSurface = 0;
			}
		}

		if( pTexture->hSurface )
		{
			BSUR_Surface_Destroy( pTexture->hSurface );
			pTexture->hSurface = 0;
		}

		if( pTexture->pvMemory )
		{
			BMEM_Free( pDisplay->hMem, pTexture->pvMemory );
			pTexture->pvMemory = NULL;
		}
	}

	/* allocate surface and its memory */
	if( pTexture->hSurface == 0 )
	{
		pTexture->pvMemory = BMEM_AllocAligned( pDisplay->hMem, iTextureSize, 3, 0 );
		if( pTexture->pvMemory == NULL )
			return GL_FALSE;

		if( BMEM_ConvertAddressToCached( pDisplay->hMem, pTexture->pvMemory, &pTexture->pvCached ) != BERR_SUCCESS )
		{
			BMEM_Free( pDisplay->hMem, pTexture->pvMemory );
			pTexture->pvMemory = NULL;
			return GL_FALSE;
		}

#ifdef BSUR_SURFACE_CREATE_HAS_SETTINGS
		err = BSUR_Surface_Create( pDisplay->hMem, iWidth, iHeight, iWidth * iBpp, pTexture->pvMemory, pTexture->eFormat, 
			0, BSUR_CONSTRAINT_WIDTH_POWER_2 | BSUR_CONSTRAINT_HEIGHT_POWER_2, 0, &pTexture->hSurface );
#else
		err = BSUR_Surface_Create( pDisplay->hMem, iWidth, iHeight, iWidth * iBpp, pTexture->pvMemory, pTexture->eFormat, 
			0, BSUR_CONSTRAINT_WIDTH_POWER_2 | BSUR_CONSTRAINT_HEIGHT_POWER_2, &pTexture->hSurface );
#endif
		if( err != BERR_SUCCESS )
		{
			BMEM_Free( pDisplay->hMem, pTexture->pvMemory );
			pTexture->pvMemory = NULL;
			return GL_FALSE;
		}

		pTexture->aLevels[0].hSurface = pTexture->hSurface;
		for( ii = 1; ii < pTexture->iLevelCount; ++ii )
		{
			GL_P_MipmapLevel *pLevel = &pTexture->aLevels[ii];
			uint32_t ulTextureAddress = (uint32_t) pTexture->pvMemory + pLevel->iOffset;

#ifdef BSUR_SURFACE_CREATE_HAS_SETTINGS
			BSUR_Surface_Create( pDisplay->hMem, pLevel->iWidth, pLevel->iHeight, 
				pLevel->iWidth * iBpp, (void *) ulTextureAddress, pTexture->eFormat, 
				0, BSUR_CONSTRAINT_WIDTH_POWER_2 | BSUR_CONSTRAINT_HEIGHT_POWER_2, 0, &pLevel->hSurface );
#else
			BSUR_Surface_Create( pDisplay->hMem, pLevel->iWidth, pLevel->iHeight, 
				pLevel->iWidth * iBpp, (void *) ulTextureAddress, pTexture->eFormat, 
				0, BSUR_CONSTRAINT_WIDTH_POWER_2 | BSUR_CONSTRAINT_HEIGHT_POWER_2, &pLevel->hSurface );
#endif
		}

		pTexture->DirtyBounds.iMinX = 0;
		pTexture->DirtyBounds.iMinY = 0;
		pTexture->DirtyBounds.iMaxX = (GLint) iWidth;
		pTexture->DirtyBounds.iMaxY = (GLint) iHeight;
		pTexture->bDirty = GL_TRUE;
		pTexture->bAllDirty = GL_TRUE;
	}

	/* copy texture if dirty */
	if( pTexture->bDirty )
	{
		GLubyte *pucMem = (GLubyte *) pTexture->pvCached + iLevelOffset;
		GLsizei ii, nn;
		GLint xx, xc, yy, yc;

		for( ii = 0; ii < pTexture->iLevelCount; ++ii )
		{
			GL_P_MipmapLevel *pLevel = &pTexture->aLevels[ii];
			if( pLevel->pvMemory )
			{
				/* scale textures that are too small (less than 8 pixels) */
				if( (iLevelCount == 1) && (ii == 0) && ((pTexture->iWidth != iWidth) || (pTexture->iHeight != iHeight)) )
				{
					GLint iXCount = iWidth / pTexture->iWidth;
					GLint iYCount = iHeight / pTexture->iHeight;
					for( yy = 0; yy < pTexture->iHeight; ++yy )
						for( yc = 0; yc < iYCount; ++yc )
							for( xx = 0; xx < pTexture->iWidth; ++xx )
								switch( iBpp )
								{
								case 1: {
									GLubyte *pucDst = (GLubyte *) pucMem + (yy * iYCount + yc) * iWidth + xx * iXCount;
									GLubyte *pucSrc = (GLubyte *) pLevel->pvMemory + yy * pTexture->iWidth + xx;
									for( xc = 0; xc < iXCount; ++xc )
										pucDst[xc] = pucSrc[0];
									break; }
								case 2: {
									GLushort *pusDst = (GLushort *) pucMem + (yy * iYCount + yc) * iWidth + xx * iXCount;
									GLushort *pusSrc = (GLushort *) pLevel->pvMemory + yy * pTexture->iWidth + xx;
									for( xc = 0; xc < iXCount; ++xc )
										pusDst[xc] = pusSrc[0];
									break; }
								case 4: {
									GLuint *pulDst = (GLuint *) pucMem + (yy * iYCount + yc) * iWidth + xx * iXCount;
									GLuint *pulSrc = (GLuint *) pLevel->pvMemory + yy * pTexture->iWidth + xx;
									for( xc = 0; xc < iXCount; ++xc )
										pulDst[xc] = pulSrc[0];
									break; }
								}
				}
				else
				{
					GLboolean bSwizzled = GL_FALSE;

					/* swizzle texture */
					if( pTexture->bSwizzle )
						bSwizzled = GL_P_SwizzleTexture( pucMem, pLevel->pvMemory, pLevel->iWidth, pLevel->iHeight, iBpp );

					if( bSwizzled == GL_FALSE )
					{
						/* copy texture */
						if( pTexture->bAllDirty )
						{
							/* copy whole texture */
							if( pLevel->iSize >= 4 )
							{
								for( nn = 0; nn < pLevel->iSize / 4; ++nn )
									((uint32_t *) pucMem)[nn] = ((uint32_t *) pLevel->pvMemory)[nn];
							}
							else
							{
								pucMem[0] = ((uint8_t *) pLevel->pvMemory)[0];
								pucMem[1] = ((uint8_t *) pLevel->pvMemory)[1];
							}
						}
						else
						{
							/* copy dirty part of texture */
							GLint iDirtyX = pTexture->DirtyBounds.iMinX;
							GLint iDirtyY = pTexture->DirtyBounds.iMinY;
							GLint iDirtyWidth = pTexture->DirtyBounds.iMaxX - pTexture->DirtyBounds.iMinX;
							GLint iDirtyHeight = pTexture->DirtyBounds.iMaxY - pTexture->DirtyBounds.iMinY;
							GLint iDirtyCount = (iDirtyWidth * iBpp) / 4;

							for( yy = 0; yy < iDirtyHeight; ++yy )
							{
								GLint iOffset = ((yy + iDirtyY) * pTexture->iWidth + iDirtyX) * iBpp;
								GLuint *puiDst = (GLuint *) ((GLubyte *) pucMem + iOffset);
								GLuint *puiSrc = (GLuint *) ((GLubyte *) pLevel->pvMemory + iOffset);

								for( xx = 0; xx < iDirtyCount; ++xx )
									puiDst[xx] = puiSrc[xx];
							}
						}
					}
				}
			}

			BMEM_FlushCache( pDisplay->hMem, pucMem, pLevel->iSize );
			pucMem += pLevel->iSize;
		}

		pTexture->bDirty = GL_FALSE;
		pTexture->bAllDirty = GL_FALSE;
	
		/* generate mipmap levels of detail */
		if( pDisplay->bGenerateMipmaps && pTexture->bGenerateMipmaps )
		{
			for( ii = 1; ii < pTexture->iLevelCount; ++ii )
				native_P_CopySurface( pDisplay->nativeDisplay, pTexture->aLevels[ii].hSurface, pTexture->aLevels[ii - 1].hSurface );
		}
	}

	BP3D_SetTextureSurface( pDisplay->hP3d, pTexture->hSurface );
	BP3D_SetTextureMipmap( pDisplay->hP3d, (bool) pContext->pTexture->bMipmap, pTexture->iLevelCount + iLodOffset - 1, 0 );

	return GL_TRUE;
}

/***************************************************************************/
GLenum GL_P_GetSceneTextureType( 
	EGL_P_Display *pDisplay,
	GLint internalformat )
{
	EGL_P_Surface *pSurface = pDisplay->pDrawSurface;
	BSUR_Surface_Handle hSurface = pSurface->hRenderSurface;
	BPXL_Format eFormat;

	BSUR_Surface_GetFormat( hSurface, &eFormat );

	switch( eFormat )
	{
	case BPXL_eR5_G6_B5:
		switch( internalformat )
		{
		case GL_RGB:       return GL_UNSIGNED_SHORT_5_6_5;
		case GL_LUMINANCE: return GL_UNSIGNED_BYTE;
		default:           return 0;
		}
		break;

	case BPXL_eA8_R8_G8_B8:
		switch( internalformat )
		{
		case GL_RGBA:            return GL_UNSIGNED_BYTE;
		case GL_RGB:             return GL_UNSIGNED_SHORT_5_6_5;
		case GL_LUMINANCE:       return GL_UNSIGNED_BYTE;
		case GL_LUMINANCE_ALPHA: return GL_UNSIGNED_BYTE;
		case GL_ALPHA:           return GL_UNSIGNED_BYTE;
		default:                 return 0;
		}
		break;

	case BPXL_eA4_R4_G4_B4:
		switch( internalformat )
		{
		case GL_RGBA:            return GL_UNSIGNED_SHORT_4_4_4_4;
		case GL_RGB:             return GL_UNSIGNED_SHORT_5_6_5;
		case GL_LUMINANCE:       return GL_UNSIGNED_BYTE;
		case GL_LUMINANCE_ALPHA: return GL_UNSIGNED_BYTE;
		case GL_ALPHA:           return GL_UNSIGNED_BYTE;
		default:                 return 0;
		}
		break;

	case BPXL_eA1_R5_G5_B5:
		switch( internalformat )
		{
		case GL_RGBA:            return GL_UNSIGNED_SHORT_5_5_5_1;
		case GL_RGB:             return GL_UNSIGNED_SHORT_5_6_5;
		case GL_LUMINANCE:       return GL_UNSIGNED_BYTE;
		case GL_LUMINANCE_ALPHA: return GL_UNSIGNED_BYTE;
		case GL_ALPHA:           return GL_UNSIGNED_BYTE;
		default:                 return 0;
		}
		break;

	default:
		break;
	}

	return 0;
}

/***************************************************************************/
void GL_P_CopySceneToTexture( 
	EGL_P_Display *pDisplay,
	EGL_P_Context *pContext,
	GL_P_MipmapLevel *pLevel,
	GLint internalformat, 
	GLint iTexX, 
	GLint iTexY, 
	GLint iSceneX, 
	GLint iSceneY, 
	GLsizei iWidth, 
	GLsizei iHeight )
{
	EGL_P_Surface *pSurface = pDisplay->pDrawSurface;
	BSUR_Surface_Handle hSurface = pSurface->hRenderSurface;
	BPXL_Format eFormat;
	void *pvMemory;
	GLuint uiSceneWidth;
	GLuint uiSceneHeight;
	GLuint uiPitch;
	GLuint uiBpp;
	GLsizei ii, jj;

	EGL_P_InitiateRender( pDisplay, pContext, GL_TRUE );

	BSUR_Surface_GetFormat( hSurface, &eFormat );
	BSUR_Surface_GetAddress( hSurface, &pvMemory, &uiPitch );
	BSUR_Surface_GetDimensions( hSurface, &uiSceneWidth, &uiSceneHeight );
	uiBpp = BPXL_BITS_PER_PIXEL(eFormat) / 8;

	switch( eFormat )
	{
	case BPXL_eR5_G6_B5:
		switch( internalformat )
		{
		case GL_RGB:
			/* RGB565 to RGB565 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				void *pvDst = (void *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * uiBpp);
				void *pvSrc = (void *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				BKNI_Memcpy( pvDst, pvSrc, iWidth * uiBpp );
			}
			break;
		case GL_LUMINANCE:
			/* RGB565 to L8 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLubyte *pucDst = (GLubyte *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * 1);
				GLushort *pusSrc = (GLushort *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pucDst[ii] = ((pusSrc[ii] & 0xF800) >> 8) | ((pusSrc[ii] & 0xE000) >> 13);
			}
			break;
		default:
			break;
		}
		break;

	case BPXL_eA8_R8_G8_B8:
		switch( internalformat )
		{
		case GL_RGBA:
			/* ARGB8888 to ARGB8888 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				void *pvDst = (void *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * uiBpp);
				void *pvSrc = (void *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				BKNI_Memcpy( pvDst, pvSrc, iWidth * uiBpp );
			}
			break;
		case GL_RGB:
			/* ARGB8888 to RGB565 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLushort *pusDst = (GLushort *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * 2);
				GLuint *pulSrc = (GLuint *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pusDst[ii] = ((pulSrc[ii] >> 8) & 0xF800) | ((pulSrc[ii] >> 5) & 0x07E0) | ((pulSrc[ii] >> 3) & 0x1F);
			}
			break;
		case GL_LUMINANCE:
			/* ARGB8888 to L8 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLubyte *pucDst = (GLubyte *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * 1);
				GLuint *pulSrc = (GLuint *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pucDst[ii] = (pulSrc[ii] & 0xFF0000) >> 16;
			}
			break;
		case GL_LUMINANCE_ALPHA:
			/* ARGB8888 to LA88 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLushort *pusDst = (GLushort *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * 2);
				GLuint *pulSrc = (GLuint *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pusDst[ii] = ((pulSrc[ii] >> 24) & 0xFF) | ((pulSrc[ii] >> 8) & 0xFF00);
			}
			break;
		case GL_ALPHA:
			/* ARGB8888 to LA88 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLushort *pusDst = (GLushort *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * 2);
				GLuint *pulSrc = (GLuint *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pusDst[ii] = ((pulSrc[ii] >> 24) & 0xFF) | 0xFF00;
			}
			break;
		default:
			break;
		}
		break;

	case BPXL_eA4_R4_G4_B4:
		switch( internalformat )
		{
		case GL_RGBA:
			/* ARGB4444 to ARGB4444 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				void *pvDst = (void *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * uiBpp);
				void *pvSrc = (void *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				BKNI_Memcpy( pvDst, pvSrc, iWidth * uiBpp );
			}
			break;
		case GL_RGB:
			/* ARGB4444 to RGB565 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLushort *pusDst = (GLushort *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * 2);
				GLushort *pusSrc = (GLushort *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pusDst[ii] = ((pusSrc[ii] & 0x0F00) << 4) | (pusSrc[ii] & 0x0800) |
						((pusSrc[ii] & 0x00F0) << 3) | ((pusSrc[ii] & 0x00C0) >> 1) |
						((pusSrc[ii] & 0x000F) << 1) | ((pusSrc[ii] & 0x0008) >> 3);
			}
			break;
		case GL_LUMINANCE:
			/* ARGB4444 to L8 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLubyte *pucDst = (GLubyte *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * 1);
				GLushort *pusSrc = (GLushort *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pucDst[ii] = ((pusSrc[ii] & 0x0F00) >> 4) | ((pusSrc[ii] & 0x0F00) >> 8);
			}
			break;
		case GL_LUMINANCE_ALPHA:
			/* ARGB4444 to LA88 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLushort *pusDst = (GLushort *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * 2);
				GLushort *pusSrc = (GLushort *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pusDst[ii] = ((pusSrc[ii] & 0x0F00) << 4) | (pusSrc[ii] & 0x0F00) |
						((pusSrc[ii] & 0xF000) >> 8) | ((pusSrc[ii] & 0xF000) >> 12);
			}
			break;
		case GL_ALPHA:
			/* ARGB4444 to LA88 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLushort *pusDst = (GLushort *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * 2);
				GLushort *pusSrc = (GLushort *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pusDst[ii] = ((pusSrc[ii] >> 8) & 0xF0) | ((pusSrc[ii] >> 12) & 0xF) | 0xFF00;
			}
			break;
		default:
			break;
		}
		break;

	case BPXL_eA1_R5_G5_B5:
		switch( internalformat )
		{
		case GL_RGBA:
			/* ARGB1555 to ARGB1555 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				void *pvDst = (void *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * uiBpp);
				void *pvSrc = (void *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				BKNI_Memcpy( pvDst, pvSrc, iWidth * uiBpp );
			}
			break;
		case GL_RGB:
			/* ARGB1555 to RGB565 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLushort *pusDst = (GLushort *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * uiBpp);
				GLushort *pusSrc = (GLushort *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pusDst[ii] = ((pusSrc[ii] & 0x7C00) << 1) | (pusSrc[ii] & 0x001F) |
						((pusSrc[ii] & 0x03E0) << 1) | ((pusSrc[ii] & 0x0200) >> 4);
			}
			break;
		case GL_LUMINANCE:
			/* ARGB1555 to L8 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLubyte *pucDst = (GLubyte *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * uiBpp);
				GLushort *pusSrc = (GLushort *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pucDst[ii] = ((pusSrc[ii] & 0x7C00) >> 7) | ((pusSrc[ii] & 0x7000) >> 12);
			}
			break;
		case GL_LUMINANCE_ALPHA:
			/* ARGB1555 to LA88 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLushort *pusDst = (GLushort *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * uiBpp);
				GLushort *pusSrc = (GLushort *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pusDst[ii] = ((pusSrc[ii] & 0x7C00) << 1) | ((pusSrc[ii] & 0x7000) >> 4) |
						((pusSrc[ii] & 0x8000) ? 0xFF : 0);
			}
			break;
		case GL_ALPHA:
			/* ARGB1555 to AXXX1555 */
			for( jj = 0; jj < iHeight; ++jj )
			{
				GLushort *pusDst = (GLushort *) ((GLubyte *) pLevel->pvMemory + ((jj + iTexY) * pLevel->iWidth + iTexX) * uiBpp);
				GLushort *pusSrc = (GLushort *) ((GLubyte *) pvMemory + (uiSceneHeight - iSceneY - jj - 1) * uiPitch + iSceneX * uiBpp);
				for( ii = 0; ii < iWidth; ++ii )
					pusDst[ii] = pusSrc[ii] | 0x7FFF;
			}
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
}

/***************************************************************************/
void GL_P_SetTextureDirty(
	GL_P_Texture *pTexture, 
	GLint iX,
	GLint iY,
	GLint iWidth,
	GLint iHeight )
{
	if( !pTexture->bAllDirty )
	{
		if( pTexture->bDirty )
		{
			pTexture->DirtyBounds.iMinX = (GLint) GL_P_MIN(pTexture->DirtyBounds.iMinX, iX);
			pTexture->DirtyBounds.iMinY = (GLint) GL_P_MIN(pTexture->DirtyBounds.iMinY, iY);
			pTexture->DirtyBounds.iMaxX = (GLint) GL_P_MAX(pTexture->DirtyBounds.iMaxX, iX + iWidth);
			pTexture->DirtyBounds.iMaxY = (GLint) GL_P_MAX(pTexture->DirtyBounds.iMaxY, iY + iHeight);
		}
		else
		{
			pTexture->DirtyBounds.iMinX = (GLint) iX;
			pTexture->DirtyBounds.iMinY = (GLint) iY;
			pTexture->DirtyBounds.iMaxX = (GLint) iX + iWidth;
			pTexture->DirtyBounds.iMaxY = (GLint) iY + iHeight;
			pTexture->bDirty = GL_TRUE;
		}
	}
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glTexImage2D( 
	GLenum target, 
	GLint level, 
	GLint internalformat, 
	GLsizei width, 
	GLsizei height, 
	GLint border, 
	GLenum format, 
	GLenum type, 
	const GLvoid *pixels )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Texture *pTexture = pContext->pTexture;
	GLsizei iTextureSize;
	BPXL_Format eFormat;

	if( (target != GL_TEXTURE_2D) && (target != GL_TEXTURE_VIDEO) && (target != GL_TEXTURE_SWIZZLE)  )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( (width < 0) || (width > GL_P_TEXTURE_SIZE_MAX) || 
		(height < 0) || (height > GL_P_TEXTURE_SIZE_MAX) || 
		((width & (width - 1)) != 0) || ((height & (height - 1)) != 0) ||
		(level < 0) || (level >= GL_P_TEXTURE_LEVEL_MAX) || (border != 0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( internalformat != (GLint) format )
	{
		GL_P_SET_ERROR(GL_INVALID_OPERATION);
		return;
	}

	if( (pTexture == NULL) || (width == 0) || (height == 0) )
	{
		return;
	}

	if( target == GL_TEXTURE_VIDEO )
	{
#if 1
		pTexture->hSurface = (BSUR_Surface_Handle) pixels;
#else
#if 0
		native_P_InitPixmap( (NativePixmap) pixels, &pTexture->hSurface, &pTexture->iWidth, &pTexture->iHeight );
#else
		unsigned int uiFormat;
		unsigned int uiOffset;
		void *pvMemory;
		int iPitch;

		native_P_InitPixmap( (NativePixmap) pixels, &pTexture->hSurface, &uiFormat, &uiOffset, 
			&pvMemory, &iPitch, &pTexture->iWidth, &pTexture->iHeight );

		if( pTexture->hSurface == NULL )
		{
			BSUR_Surface_Create( pDisplay->hMem, pTexture->iWidth, pTexture->iHeight, 
				iPitch, pvMemory, uiFormat, 0, 0, 0, &pTexture->hSurface );
		}
#endif
#endif
		pTexture->bVideo = true;
		return;
	}

	pTexture->bSwizzle = (target == GL_TEXTURE_SWIZZLE) ? GL_TRUE: GL_FALSE;

	/* get texture format and size */
	eFormat = GL_P_ConvertFormat_GLtoP3D( format, type );
	if( eFormat == 0 )
		return;

	iTextureSize = width * height * BPXL_BITS_PER_PIXEL(eFormat) / 8;

	/* free previous texture level memory if changing */
	if( pTexture->aLevels[level].pvMemory && (iTextureSize != pTexture->aLevels[level].iSize) )
	{
		BKNI_Free( pTexture->aLevels[level].pvMemory );
		pTexture->aLevels[level].pvMemory = NULL;
	}

	/* allocate texture level memory */
	if( pTexture->aLevels[level].pvMemory == NULL )
	{
		pTexture->aLevels[level].pvMemory = BKNI_Malloc( iTextureSize );
		if( pTexture->aLevels[level].pvMemory == NULL )
		{
			GL_P_SET_ERROR(GL_OUT_OF_MEMORY);
			return;
		}
	}

	/* free previous surface if changing */
	if( (eFormat != pTexture->eFormat) || (format != pTexture->eInternalFormat ) || 
		((level == 0) && ((width != pTexture->iWidth) || (height != pTexture->iHeight))) )
	{
		if( pTexture->hSurface )
		{
			BSUR_Surface_Destroy( pTexture->hSurface );
			pTexture->hSurface = 0;
		}

		if( pTexture->pvMemory )
		{
			BMEM_Free( pDisplay->hMem, pTexture->pvMemory );
			pTexture->pvMemory = NULL;
		}
	}

	/* set texture structure */
	if( level == 0 )
	{
		pTexture->iWidth = width;
		pTexture->iHeight = height;
	}

	pTexture->eFormat = eFormat;
	pTexture->eInternalFormat = internalformat;
	pTexture->aLevels[level].iWidth = width;
	pTexture->aLevels[level].iHeight = height;
	pTexture->aLevels[level].iSize = iTextureSize;

	/* copy texture image */
	if( pixels )
	{
		GL_P_ConvertPixels_GLtoP3D( pTexture->aLevels[level].pvMemory, (GLvoid *) pixels, 
			format, type, 0, 0, width, height, width );

		pTexture->DirtyBounds.iMinX = 0;
		pTexture->DirtyBounds.iMinY = 0;
		pTexture->DirtyBounds.iMaxX = (GLint) width;
		pTexture->DirtyBounds.iMaxY = (GLint) height;
		pTexture->bDirty = GL_TRUE;
		pTexture->bAllDirty = GL_TRUE;
	}
}

/***************************************************************************/
GLAPI void APIENTRY glTexSubImage2D(
	GLenum target, 
	GLint level, 
	GLint xoffset, 
	GLint yoffset, 
	GLsizei width, 
	GLsizei height, 
	GLenum format, 
	GLenum type, 
	const GLvoid *pixels )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Texture *pTexture = pContext->pTexture;

	if( target != GL_TEXTURE_2D )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( (width < 0) || (width > GL_P_TEXTURE_SIZE_MAX) || 
		(height < 0) || (height > GL_P_TEXTURE_SIZE_MAX) || 
		(level < 0) || (level >= GL_P_TEXTURE_LEVEL_MAX) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( (pTexture == NULL) || (pTexture->aLevels[level].pvMemory == NULL) )
		return;

	if( (pixels == NULL) || (width == 0) || (height == 0) )
		return;

	GL_P_ConvertPixels_GLtoP3D( pTexture->aLevels[level].pvMemory, (GLvoid *) pixels, 
		format, type, xoffset, yoffset, width, height, pTexture->aLevels[level].iWidth );
	GL_P_SetTextureDirty( pTexture, xoffset, yoffset, width, height );
}

/***************************************************************************/
GLAPI void APIENTRY glCopyTexImage2D(
	GLenum target, 
	GLint level, 
	GLenum internalformat, 
	GLint x, 
	GLint y, 
	GLsizei width, 
	GLsizei height, 
	GLint border )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Texture *pTexture = pContext->pTexture;

	GLenum type = GL_P_GetSceneTextureType( pDisplay, internalformat );
	if( type == 0 )
	{
		GL_P_SET_ERROR(GL_INVALID_OPERATION); 
		return;
	}

	glTexImage2D( target, level, internalformat, width, height, border, internalformat, type, NULL );
	if( pTexture->aLevels[level].pvMemory == NULL )
	{
		GL_P_SET_ERROR(GL_INVALID_OPERATION);
		return;
	}

	GL_P_CopySceneToTexture( pDisplay, pContext, &pTexture->aLevels[level], internalformat, 0, 0, x, y, width, height );
	GL_P_SetTextureDirty( pTexture, 0, 0, width, height );
}

/***************************************************************************/
GLAPI void APIENTRY glCopyTexSubImage2D( 
	GLenum target, 
	GLint level, 
	GLint xoffset, 
	GLint yoffset, 
	GLint x, 
	GLint y, 
	GLsizei width, 
	GLsizei height )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Texture *pTexture = pContext->pTexture;
	GLenum type;

	if( target != GL_TEXTURE_2D )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( (width < 0) || (width > GL_P_TEXTURE_SIZE_MAX) || 
		(height < 0) || (height > GL_P_TEXTURE_SIZE_MAX) || 
		(level < 0) || (level >= GL_P_TEXTURE_LEVEL_MAX) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( (pTexture == NULL) || (pTexture->aLevels[level].pvMemory == NULL) )
	{
		GL_P_SET_ERROR(GL_INVALID_OPERATION);
		return;
	}

	if( (width == 0) || (height == 0) )
		return;

	type = GL_P_GetSceneTextureType( pDisplay, pTexture->eInternalFormat );
	if( type == 0 )
	{
		GL_P_SET_ERROR(GL_INVALID_OPERATION); 
		return;
	}

	GL_P_CopySceneToTexture( pDisplay, pContext, &pTexture->aLevels[level], pTexture->eInternalFormat, 
		xoffset, yoffset, x, y, width, height );
	GL_P_SetTextureDirty( pTexture, xoffset, yoffset, width, height );
}

/***************************************************************************/
GLAPI void APIENTRY glCompressedTexImage2D( 
	GLenum target, 
	GLint level, 
	GLenum internalformat, 
	GLsizei width, 
	GLsizei height, 
	GLint border, 
	GLsizei imageSize, 
	const GLvoid *data )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Texture *pTexture = pContext->pTexture;

	BSTD_UNUSED( imageSize );

	glTexImage2D( target, level, internalformat, width, height, border, internalformat, GL_UNSIGNED_BYTE, NULL );

	/* copy texture image */
	if( pTexture->aLevels[level].pvMemory && data )
	{
		GL_P_ConvertPixels_GLtoP3D( pTexture->aLevels[level].pvMemory, (GLvoid *) data, 
			internalformat, GL_UNSIGNED_BYTE, 0, 0, width, height, width );

		pTexture->DirtyBounds.iMinX = 0;
		pTexture->DirtyBounds.iMinY = 0;
		pTexture->DirtyBounds.iMaxX = (GLint) width;
		pTexture->DirtyBounds.iMaxY = (GLint) height;
		pTexture->bDirty = GL_TRUE;
		pTexture->bAllDirty = GL_TRUE;
	}
}

/***************************************************************************/
GLAPI void APIENTRY glCompressedTexSubImage2D( 
	GLenum target, 
	GLint level, 
	GLint xoffset, 
	GLint yoffset, 
	GLsizei width, 
	GLsizei height, 
	GLenum format, 
	GLsizei imageSize, 
	const GLvoid *data )
{
	BSTD_UNUSED( target );
	BSTD_UNUSED( level );
	BSTD_UNUSED( xoffset );
	BSTD_UNUSED( yoffset );
	BSTD_UNUSED( width );
	BSTD_UNUSED( height );
	BSTD_UNUSED( format );
	BSTD_UNUSED( imageSize );
	BSTD_UNUSED( data );
}

/* End of File */
