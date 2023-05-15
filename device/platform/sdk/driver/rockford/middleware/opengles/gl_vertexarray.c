/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_vertexarray.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 7/29/10 6:32p $
 *
 * Module Description: Array entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_vertexarray.c $
 * 
 * Hydra_Software_Devel/10   7/29/10 6:32p nissen
 * SW7405-4657: Added support for using color array with lighting enabled.
 * 
 * Hydra_Software_Devel/9   7/6/09 4:48p nissen
 * PR 56587: Added limited support for glColorMask.
 * 
 * Hydra_Software_Devel/8   9/8/08 1:16p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/7   11/15/07 8:22p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/6   5/19/07 7:22a nissen
 * PR 30062: Added support for drawing screen aligned rectangles.
 * 
 * Hydra_Software_Devel/5   9/20/06 1:25p nissen
 * PR 22858: Fixed fog.
 * 
 * Hydra_Software_Devel/4   6/12/06 10:48a nissen
 * PR 21896: Added support for timing various operations.
 * 
 * Hydra_Software_Devel/3   8/10/04 12:43p nissen
 * PR 9132: Fixed vertex strides. Added code to reject primitives if
 * viewport or scissor size is zero.
 * 
 * Hydra_Software_Devel/2   7/28/04 2:30p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
void GL_P_DrawVertices( 
	EGL_P_Display *pDisplay,
	EGL_P_Context *pContext, 
	GLenum mode, 
	GLint iFirst, 
	GLsizei iCount,
	GLenum eIndexType, 
	const GLvoid *pvIndices )
{
	GL_P_Texture *pTexture = pContext->pTexture;
	GL_P_Matrix *pModelViewMatrix = &pContext->ModelViewMatrixStack.aMatrices[pContext->ModelViewMatrixStack.uiTop];
	GL_P_Matrix *pProjectionMatrix = &pContext->ProjectionMatrixStack.aMatrices[pContext->ProjectionMatrixStack.uiTop];

	if( iCount > pContext->iVertexCount )
	{
		if( pContext->pVertices )
			BKNI_Free( pContext->pVertices );

		pContext->pVertices = BKNI_Malloc( iCount * sizeof (BP3D_Vertex) );
		if( pContext->pVertices == NULL ) 
		{
			pContext->iVertexCount = 0;
			GL_P_SET_ERROR(GL_OUT_OF_MEMORY);
			return;
		}
		
		pContext->iVertexCount = iCount;
	}

	if( GL_P_LoadTexture( pDisplay, pContext, pTexture ) == GL_FALSE )
	{
		GL_P_SET_ERROR(GL_OUT_OF_MEMORY);
		return;
	}

	if( pContext->ModelViewMatrixStack.bDirty || pContext->ProjectionMatrixStack.bDirty )
	{
		GL_P_SetCombinedMatrix( &pContext->CombinedMatrix, pProjectionMatrix, pModelViewMatrix );
		pContext->bNormalMatrixDirty = GL_TRUE;

		if( pContext->bFog )
		{
			pContext->fTransFogStart = -pContext->fFogStart * pContext->CombinedMatrix.a[10];
			pContext->fTransFogEnd = -pContext->fFogEnd * pContext->CombinedMatrix.a[10];
		}
	}

	if( pContext->bLighting )
	{
		if( pContext->bNormalMatrixDirty )
		{
			GL_P_SetNormalMatrix( &pContext->NormalMatrix, pModelViewMatrix );
			pContext->bNormalMatrixDirty = GL_FALSE;
		}

		GL_P_TransformEyeCoordArray( pContext, iFirst, iCount, eIndexType, pvIndices );
		GL_P_TransformNormalArray( pContext, iFirst, iCount, eIndexType, pvIndices );
		GL_P_TransformColorArray( pContext, iFirst, iCount, eIndexType, pvIndices );
		GL_P_CalculateLightValues( pContext, iCount );
	}
	else
	{
		GL_P_CopyColorArray( pContext, iFirst, iCount, eIndexType, pvIndices );
	}

	GL_P_TransformVertexArray( pContext, iFirst, iCount, eIndexType, pvIndices );

	if( pContext->bTexture2D && pTexture )
		GL_P_TransformTexCoordArray( pContext, iFirst, iCount, eIndexType, pvIndices );

	pContext->ModelViewMatrixStack.bDirty = GL_FALSE;
	pContext->ProjectionMatrixStack.bDirty = GL_FALSE;

	if( pContext->ulVertexRejectCode == 0 )
	{
		if( pContext->ulVertexOutCode == 0 )
			GL_P_TransformVertices( pContext, iCount );

		switch( mode )
		{
		case GL_POINTS:
			GL_P_ClipAndRenderPoints( pDisplay, pContext, iCount );
			break;
		case GL_LINE_STRIP:
			GL_P_ClipAndRenderLineStrip( pDisplay, pContext, iCount );
			break;
		case GL_LINE_LOOP:
			GL_P_ClipAndRenderLineLoop( pDisplay, pContext, iCount );
			break;
		case GL_LINES:
			GL_P_ClipAndRenderLines( pDisplay, pContext, iCount );
			break;
		case GL_TRIANGLE_STRIP:
			GL_P_ClipAndRenderTriangleStrip( pDisplay, pContext, iCount );
			break;
		case GL_TRIANGLE_FAN:
			GL_P_ClipAndRenderTriangleFan( pDisplay, pContext, iCount );
			break;
		case GL_TRIANGLES:
			GL_P_ClipAndRenderTriangles( pDisplay, pContext, iCount );
			break;
		case GL_RECTANGLES:
			GL_P_ClipAndRenderRectangles( pDisplay, pContext, iCount );
			break;
		default: 
			GL_P_SET_ERROR(GL_INVALID_ENUM);
			return;
		}
	}
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glVertexPointer( 
	GLint size, 
	GLenum type, 
	GLsizei stride, 
	const GLvoid *pointer )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (size < 2) || (size > 4) || (stride < 0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( (type != GL_UNSIGNED_BYTE) && (type != GL_BYTE) && 
		(type != GL_UNSIGNED_SHORT) && (type != GL_SHORT) && 
		(type != GL_FIXED) && (type != GL_FLOAT) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pContext->VertexArray.iSize = size;
	pContext->VertexArray.eType = type;
	pContext->VertexArray.iStride = stride;
	pContext->VertexArray.pvMemory = pointer;
}

/***************************************************************************/
GLAPI void APIENTRY glNormalPointer( 
	GLenum type, 
	GLsizei stride, 
	const GLvoid *pointer )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( stride < 0 )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( (type != GL_UNSIGNED_BYTE) && (type != GL_BYTE) && 
		(type != GL_UNSIGNED_SHORT) && (type != GL_SHORT) && 
		(type != GL_FIXED) && (type != GL_FLOAT) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pContext->NormalArray.iSize = 3;
	pContext->NormalArray.eType = type;
	pContext->NormalArray.iStride = stride;
	pContext->NormalArray.pvMemory = pointer;
}

/***************************************************************************/
GLAPI void APIENTRY glColorPointer( 
	GLint size, 
	GLenum type, 
	GLsizei stride, 
	const GLvoid *pointer )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (size != 4) || (stride < 0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( (type != GL_UNSIGNED_BYTE) && (type != GL_BYTE) && 
		(type != GL_UNSIGNED_SHORT) && (type != GL_SHORT) && 
		(type != GL_FIXED) && (type != GL_FLOAT) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pContext->ColorArray.iSize = size;
	pContext->ColorArray.eType = type;
	pContext->ColorArray.iStride = stride;
	pContext->ColorArray.pvMemory = pointer;
}

/***************************************************************************/
GLAPI void APIENTRY glTexCoordPointer(
	GLint size, 
	GLenum type, 
	GLsizei stride, 
	const GLvoid *pointer )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (size < 2) || (size > 4) || (stride < 0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( (type != GL_UNSIGNED_BYTE) && (type != GL_BYTE) && 
		(type != GL_UNSIGNED_SHORT) && (type != GL_SHORT) && 
		(type != GL_FIXED) && (type != GL_FLOAT) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pContext->TexCoordArray.iSize = size;
	pContext->TexCoordArray.eType = type;
	pContext->TexCoordArray.iStride = stride;
	pContext->TexCoordArray.pvMemory = pointer;
}

/***************************************************************************/
GLAPI void APIENTRY glDrawArrays( 
	GLenum mode, 
	GLint first, 
	GLsizei count )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( count < 0 )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( count == 0 )
		return;

	if( (pContext->bVertexArray == GL_FALSE) || (pContext->VertexArray.pvMemory == NULL) )
		return;

	if( (pContext->Viewport.fWidth == 0.0) || (pContext->Viewport.fHeight == 0.0) )
		return;

	if( pContext->bScissorTest && ((pContext->Scissor.fWidth == 0.0) || (pContext->Scissor.fHeight == 0.0)) )
		return;

	if( (!pContext->bMaskRed) && (!pContext->bMaskBlue) && (!pContext->bMaskGreen) && (!pContext->bMaskAlpha) )
		return;

	if( (!pContext->bMaskRed) || (!pContext->bMaskBlue) || (!pContext->bMaskGreen) || (!pContext->bMaskAlpha) )
	{
		GL_P_SetColorMaskState_Pass0( pContext );
		GL_P_DrawVertices( pDisplay, pContext, mode, first, count, 0, NULL );

		GL_P_SetColorMaskState_Pass1( pContext );
		GL_P_DrawVertices( pDisplay, pContext, mode, first, count, 0, NULL );

		GL_P_SetColorMaskState_Done( pContext );
	}
	else
	{
		GL_P_DrawVertices( pDisplay, pContext, mode, first, count, 0, NULL );
	}
}

/***************************************************************************/
GLAPI void APIENTRY glDrawElements(
	GLenum mode, 
	GLsizei count, 
	GLenum type, 
	const GLvoid *indices )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( (count < 0) || (indices == NULL) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( (type != GL_UNSIGNED_BYTE) && (type != GL_UNSIGNED_SHORT) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( count == 0 )
		return;

	if( (pContext->bVertexArray == GL_FALSE) || (pContext->VertexArray.pvMemory == NULL) )
		return;

	if( (pContext->Viewport.fWidth == 0.0) || (pContext->Viewport.fHeight == 0.0) )
		return;

	if( pContext->bScissorTest && ((pContext->Scissor.fWidth == 0.0) || (pContext->Scissor.fHeight == 0.0)) )
		return;

	if( (!pContext->bMaskRed) && (!pContext->bMaskBlue) && (!pContext->bMaskGreen) && (!pContext->bMaskAlpha) )
		return;

	if( (!pContext->bMaskRed) || (!pContext->bMaskBlue) || (!pContext->bMaskGreen) || (!pContext->bMaskAlpha) )
	{
		GL_P_SetColorMaskState_Pass0( pContext );
		GL_P_DrawVertices( pDisplay, pContext, mode, 0, count, type, indices );

		GL_P_SetColorMaskState_Pass1( pContext );
		GL_P_DrawVertices( pDisplay, pContext, mode, 0, count, type, indices );

		GL_P_SetColorMaskState_Done( pContext );
	}
	else
	{
		GL_P_DrawVertices( pDisplay, pContext, mode, 0, count, type, indices );
	}
}

/***************************************************************************/
GLAPI void APIENTRY glDrawAlignedRectangle(
	const GLfloat *v,
	const GLfloat *t,
	GLuint ulColor )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Texture *pTexture = pContext->pTexture;
	BP3D_Vertex vin0 = { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 };
	BP3D_Vertex vin1 = { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 };
	BP3D_Vertex vout0;
	BP3D_Vertex vout1;

	if( v == NULL )
		return;

	if( GL_P_LoadTexture( pDisplay, pContext, pTexture ) == GL_FALSE )
	{
		GL_P_SET_ERROR(GL_OUT_OF_MEMORY);
		return;
	}

	vin0.fX = v[0];
	vin0.fY = v[1];
	vin0.fS = t ? t[0] : 0;
	vin0.fT = t ? t[1] : 0;
	vin0.sX = (int16_t) vin0.fX;
	vin0.sY = (int16_t) vin0.fY;
	vin0.ulDiffuse = ulColor;

	vin1.fX = v[2];
	vin1.fY = v[3];
	vin1.fS = t ? t[2] : 0;
	vin1.fT = t ? t[3] : 0;
	vin1.sX = (int16_t) vin1.fX;
	vin1.sY = (int16_t) vin1.fY;
	vin1.ulDiffuse = ulColor;

	GL_P_TransformVertex( pContext, &vout0, &vin0 );
	GL_P_TransformVertex( pContext, &vout1, &vin1 );
	BP3D_DrawRectangle( pDisplay->hP3d, &vout0, &vout1 );
}

/* End of File */
