/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_transform.c $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 8/6/10 5:40p $
 *
 * Module Description: Transform private functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_transform.c $
 * 
 * Hydra_Software_Devel/16   8/6/10 5:40p nissen
 * SW7400-2378: Fixed fog.
 * 
 * Hydra_Software_Devel/15   8/6/10 1:43p nissen
 * SW7400-2378: Increased x and y coorindates by a small amount to pass
 * specular mustpass conformance.
 * 
 * Hydra_Software_Devel/14   7/29/10 8:53p nissen
 * SW7405-4657: Added support for color channel conversions when using
 * color arrays with lighting.
 * 
 * Hydra_Software_Devel/13   7/29/10 6:28p nissen
 * SW7405-4657: Added support for using color array with lighting enabled.
 * 
 * Hydra_Software_Devel/12   7/22/10 5:35p nissen
 * SW7125-507: Fixed type when copying fixed color array.
 * 
 * Hydra_Software_Devel/11   10/24/08 3:01p nissen
 * PR 41194: Adjusted depth range.
 * 
 * Hydra_Software_Devel/10   10/23/08 2:03p nissen
 * PR 41194: Fixed depth range again.
 * 
 * Hydra_Software_Devel/9   10/22/08 8:04p nissen
 * PR 41194: Fixed depth range.
 * 
 * Hydra_Software_Devel/8   8/19/08 10:49a nissen
 * PR 41194: Fixed warnings.
 * 
 * Hydra_Software_Devel/7   5/5/07 4:50p nissen
 * PR 30062: Switch from short to ushort for counter.
 * 
 * Hydra_Software_Devel/6   10/17/05 9:27p nissen
 * PR 17656: Added support for fixed point transform data.
 * 
 * Hydra_Software_Devel/5   3/9/05 3:41p nissen
 * PR 12812: Fixed problem with short coordinates.
 * 
 * Hydra_Software_Devel/4   9/7/04 4:25p nissen
 * PR 9132: Removed old code.
 * 
 * Hydra_Software_Devel/3   8/10/04 1:48p nissen
 * PR 9132: Fixed vertex strides. Fixed viewport transform for z. Added
 * fog support.
 * 
 * Hydra_Software_Devel/2   7/28/04 2:23p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
#define GL_P_TRANSFORM_VERTEX_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GL_TYPE, SRC ) \
{ \
	BP3D_Vertex *pVertex = pContext->pVertices; \
	GL_P_Matrix *pMat = &pContext->CombinedMatrix; \
	GL_TYPE tZ = (GL_TYPE) 0; \
	GL_TYPE tW = (GL_TYPE) GL_P_CONVERT_Ito##SRC( 1 ); \
	GLshort usIndex = 0; \
	GLushort ii; \
\
	GLsizei iByteStride = pContext->VertexArray.iStride ? pContext->VertexArray.iStride : (pContext->VertexArray.iSize * (GLsizei) sizeof (GL_TYPE)); \
	GLsizei iStride = iByteStride / sizeof (GL_TYPE); \
	GL_TYPE *ptMemoryV = (GL_TYPE *) pContext->VertexArray.pvMemory + iFirst * iStride; \
	GL_TYPE *ptMemoryZ = (pContext->VertexArray.iSize > 2) ? ptMemoryV + 2 : (&tZ); \
	GL_TYPE *ptMemoryW = (pContext->VertexArray.iSize > 3) ? ptMemoryV + 3 : (&tW); \
\
	GLshort *pusIndexV = (GLshort *) (pvIndices ? pvIndices : (&ii)); \
	GLshort *pusIndexZ = (pContext->VertexArray.iSize > 2) ? pusIndexV : (&usIndex); \
	GLshort *pusIndexW = (pContext->VertexArray.iSize > 3) ? pusIndexV : (&usIndex); \
	GLushort usIndexMask = (pvIndices && (eIndexType == GL_UNSIGNED_BYTE)) ? 0xFF : 0xFFFF; \
\
	GLsizei iIndexStrideV = pvIndices ? ((eIndexType == GL_UNSIGNED_BYTE) ? 1 : 2) : 0; \
	GLsizei iIndexStrideZ = (pContext->VertexArray.iSize > 2) ? iIndexStrideV : 0; \
	GLsizei iIndexStrideW = (pContext->VertexArray.iSize > 3) ? iIndexStrideV : 0; \
\
	pContext->ulVertexOutCode = 0; \
	pContext->ulVertexRejectCode = 0xFFFFFFFF; \
\
	for( ii = 0; ii < iCount; ++ii ) \
	{ \
		GL_TYPE *ptV = ptMemoryV + iStride * ((*((GLushort *) ((GLubyte *) pusIndexV + iIndexStrideV * ii))) & usIndexMask); \
		GL_TYPE *ptZ = ptMemoryZ + iStride * ((*((GLushort *) ((GLubyte *) pusIndexZ + iIndexStrideZ * ii))) & usIndexMask); \
		GL_TYPE *ptW = ptMemoryW + iStride * ((*((GLushort *) ((GLubyte *) pusIndexW + iIndexStrideW * ii))) & usIndexMask); \
\
		pVertex->fX = GL_P_CONVERT_##SRC##toF(pMat->a[0] * ptV[0] + pMat->a[4] * ptV[1] + pMat->a[8]  * ptZ[0] + pMat->a[12] * ptW[0]); \
		pVertex->fY = GL_P_CONVERT_##SRC##toF(pMat->a[1] * ptV[0] + pMat->a[5] * ptV[1] + pMat->a[9]  * ptZ[0] + pMat->a[13] * ptW[0]); \
		pVertex->fZ = GL_P_CONVERT_##SRC##toF(pMat->a[2] * ptV[0] + pMat->a[6] * ptV[1] + pMat->a[10] * ptZ[0] + pMat->a[14] * ptW[0]); \
		pVertex->fW = GL_P_CONVERT_##SRC##toF(pMat->a[3] * ptV[0] + pMat->a[7] * ptV[1] + pMat->a[11] * ptZ[0] + pMat->a[15] * ptW[0]); \
		pVertex->fInvW = (pVertex->fW != 0.0) ? 1.0 / pVertex->fW : 0.0; \
\
		GL_P_GENERATE_OUTCODE(pVertex, pContext->Viewport.fGBEpsilon); \
		pContext->ulVertexOutCode |= GL_P_GET_OUTCODE(pVertex); \
		pContext->ulVertexRejectCode &= GL_P_GET_OUTCODE(pVertex); \
		pVertex++; \
	} \
}

/***************************************************************************/
void GL_P_TransformVertexArray( 
	EGL_P_Context *pContext, 
	GLint iFirst, 
	GLsizei iCount,
	GLenum eIndexType, 
	const GLvoid *pvIndices )
{
	switch( pContext->VertexArray.eType )
	{
	case GL_FLOAT:
		GL_P_TRANSFORM_VERTEX_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfloat, F );
		break;
	case GL_FIXED:
		GL_P_TRANSFORM_VERTEX_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfixed, X );
		break;
	case GL_SHORT:
		GL_P_TRANSFORM_VERTEX_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLshort, I );
		break;
	case GL_UNSIGNED_SHORT:
		GL_P_TRANSFORM_VERTEX_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLushort, I );
		break;
	case GL_BYTE:
		GL_P_TRANSFORM_VERTEX_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLbyte, I );
		break;
	case GL_UNSIGNED_BYTE:
		GL_P_TRANSFORM_VERTEX_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLubyte, I );
		break;
	}
}

/***************************************************************************/
#define GL_P_TRANSFORM_EYECOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GL_TYPE, SRC ) \
{ \
	BP3D_Vertex *pVertex = pContext->pVertices; \
	GL_P_Matrix *pMat = &pContext->ModelViewMatrixStack.aMatrices[pContext->ModelViewMatrixStack.uiTop]; \
	GL_TYPE tZ = (GL_TYPE) 0; \
	GL_TYPE tW = (GL_TYPE) GL_P_CONVERT_Ito##SRC( 1 ); \
	GLshort usIndex = 0; \
	GLushort ii; \
\
	GLsizei iByteStride = pContext->VertexArray.iStride ? pContext->VertexArray.iStride : (pContext->VertexArray.iSize * (GLsizei) sizeof (GL_TYPE)); \
	GLsizei iStride = iByteStride / sizeof (GL_TYPE); \
	GL_TYPE *ptMemoryV = (GL_TYPE *) pContext->VertexArray.pvMemory + iFirst * iStride; \
	GL_TYPE *ptMemoryZ = (pContext->VertexArray.iSize > 2) ? ptMemoryV + 2 : (&tZ); \
	GL_TYPE *ptMemoryW = (pContext->VertexArray.iSize > 3) ? ptMemoryV + 3 : (&tW); \
\
	GLshort *pusIndexV = (GLshort *) (pvIndices ? pvIndices : (&ii)); \
	GLshort *pusIndexZ = (pContext->VertexArray.iSize > 2) ? pusIndexV : (&usIndex); \
	GLshort *pusIndexW = (pContext->VertexArray.iSize > 3) ? pusIndexV : (&usIndex); \
\
	GLushort usIndexMask = (pvIndices && (eIndexType == GL_UNSIGNED_BYTE)) ? 0xFF : 0xFFFF; \
	GLsizei iIndexStrideV = pvIndices ? ((eIndexType == GL_UNSIGNED_BYTE) ? 1 : 2) : 0; \
	GLsizei iIndexStrideZ = (pContext->VertexArray.iSize > 2) ? iIndexStrideV : 0; \
	GLsizei iIndexStrideW = (pContext->VertexArray.iSize > 3) ? iIndexStrideV : 0; \
\
	for( ii = 0; ii < iCount; ++ii ) \
	{ \
		GL_P_Coord3f *pEyeCoord = (GL_P_Coord3f *) (&pVertex->fX); \
		GL_TYPE *ptV = ptMemoryV + iStride * ((*((GLushort *) ((GLubyte *) pusIndexV + iIndexStrideV * ii))) & usIndexMask); \
		GL_TYPE *ptZ = ptMemoryZ + iStride * ((*((GLushort *) ((GLubyte *) pusIndexZ + iIndexStrideZ * ii))) & usIndexMask); \
		GL_TYPE *ptW = ptMemoryW + iStride * ((*((GLushort *) ((GLubyte *) pusIndexW + iIndexStrideW * ii))) & usIndexMask); \
\
		GLfloat fEyeW = GL_P_CONVERT_##SRC##toF(pMat->a[3] * ptV[0] + pMat->a[7] * ptV[1] + pMat->a[11] * ptZ[0] + pMat->a[15] * ptW[0]); \
		pEyeCoord->fX = GL_P_CONVERT_##SRC##toF(pMat->a[0] * ptV[0] + pMat->a[4] * ptV[1] + pMat->a[8]  * ptZ[0] + pMat->a[12] * ptW[0]); \
		pEyeCoord->fY = GL_P_CONVERT_##SRC##toF(pMat->a[1] * ptV[0] + pMat->a[5] * ptV[1] + pMat->a[9]  * ptZ[0] + pMat->a[13] * ptW[0]); \
		pEyeCoord->fZ = GL_P_CONVERT_##SRC##toF(pMat->a[2] * ptV[0] + pMat->a[6] * ptV[1] + pMat->a[10] * ptZ[0] + pMat->a[14] * ptW[0]); \
\
		if( fEyeW != 0.0f ) \
		{ \
			pEyeCoord->fX /= fEyeW; \
			pEyeCoord->fY /= fEyeW; \
			pEyeCoord->fZ /= fEyeW; \
		} \
		pVertex++; \
	} \
}

/***************************************************************************/
void GL_P_TransformEyeCoordArray( 
	EGL_P_Context *pContext, 
	GLint iFirst, 
	GLsizei iCount,
	GLenum eIndexType, 
	const GLvoid *pvIndices )
{
	switch( pContext->VertexArray.eType )
	{
	case GL_FLOAT:
		GL_P_TRANSFORM_EYECOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfloat, F );
		break;
	case GL_FIXED:
		GL_P_TRANSFORM_EYECOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfixed, X );
		break;
	case GL_SHORT:
		GL_P_TRANSFORM_EYECOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLshort, I );
		break;
	case GL_UNSIGNED_SHORT:
		GL_P_TRANSFORM_EYECOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLushort, I );
		break;
	case GL_BYTE:
		GL_P_TRANSFORM_EYECOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLbyte, I );
		break;
	case GL_UNSIGNED_BYTE:
		GL_P_TRANSFORM_EYECOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLubyte, I );
		break;
	}
}

/***************************************************************************/
#define GL_P_TRANSFORM_NORMAL_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GL_TYPE, SRC ) \
{ \
	BP3D_Vertex *pVertex = pContext->pVertices; \
	GL_P_Matrix *pMat = &pContext->NormalMatrix; \
	GLushort ii; \
\
	GLsizei iByteStride = pContext->NormalArray.iStride ? pContext->NormalArray.iStride : (pContext->NormalArray.iSize * (GLsizei) sizeof (GL_TYPE)); \
	GLsizei iStride = pContext->bNormalArray ? iByteStride / sizeof (GL_TYPE) : 0; \
	GLsizei iIndexStride = (pvIndices && pContext->bNormalArray) ? ((eIndexType == GL_UNSIGNED_BYTE) ? 1 : 2) : 0; \
	GLushort usIndexMask = (pvIndices && (eIndexType == GL_UNSIGNED_BYTE)) ? 0xFF : 0xFFFF; \
	GLushort *pusIndex = (GLushort *) (pvIndices ? pvIndices : (&ii)); \
	GL_TYPE *ptMemory = (GL_TYPE *) pContext->NormalArray.pvMemory + iFirst * iStride; \
	GL_TYPE *ptNormal = pContext->bNormalArray ? ptMemory : ((GL_TYPE *) (void *) (&pContext->Normal)); \
	GLfloat fRescaleFactor = 1.0; \
\
	if( pContext->bRescaleNormal ) \
		fRescaleFactor = (GLfloat) sqrt( pMat->a[2] * pMat->a[2] + pMat->a[6] * pMat->a[6] + pMat->a[10] * pMat->a[10] ); \
\
	for( ii = 0; ii < iCount; ++ii ) \
	{ \
		GL_P_Coord3f *pDstNormal = (GL_P_Coord3f *) (&pVertex->fW); \
		GL_TYPE *ptSrcNormal = ptNormal + iStride * \
			((*((GLushort *) ((GLubyte *) pusIndex + iIndexStride * ii))) & usIndexMask); \
\
		pDstNormal->fX = GL_P_CONVERT_##SRC##toF(pMat->a[0] * ptSrcNormal[0] + pMat->a[1] * ptSrcNormal[1] + pMat->a[2]  * ptSrcNormal[2]); \
		pDstNormal->fY = GL_P_CONVERT_##SRC##toF(pMat->a[4] * ptSrcNormal[0] + pMat->a[5] * ptSrcNormal[1] + pMat->a[6]  * ptSrcNormal[2]); \
		pDstNormal->fZ = GL_P_CONVERT_##SRC##toF(pMat->a[8] * ptSrcNormal[0] + pMat->a[9] * ptSrcNormal[1] + pMat->a[10] * ptSrcNormal[2]); \
\
		if( pContext->bRescaleNormal && (fRescaleFactor != 0.0) ) \
		{ \
			pDstNormal->fX /= fRescaleFactor; \
			pDstNormal->fY /= fRescaleFactor; \
			pDstNormal->fZ /= fRescaleFactor; \
		} \
\
		if( pContext->bNormalize ) \
		{ \
			GLfloat fSize = (GLfloat) sqrt( pDstNormal->fX * pDstNormal->fX + \
				pDstNormal->fY * pDstNormal->fY + pDstNormal->fZ * pDstNormal->fZ); \
\
			if( fSize != 0.0 ) \
			{ \
				pDstNormal->fX /= fSize; \
				pDstNormal->fY /= fSize; \
				pDstNormal->fZ /= fSize; \
			} \
		} \
		pVertex++; \
	} \
}

/***************************************************************************/
void GL_P_TransformNormalArray( 
	EGL_P_Context *pContext, 
	GLint iFirst, 
	GLsizei iCount,
	GLenum eIndexType, 
	const GLvoid *pvIndices )
{
	GLenum eType = pContext->bNormalArray ? pContext->NormalArray.eType : GL_FLOAT;

	switch( eType )
	{
	case GL_FLOAT:
		GL_P_TRANSFORM_NORMAL_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfloat, F );
		break;
	case GL_FIXED:
		GL_P_TRANSFORM_NORMAL_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfixed, X );
		break;
	case GL_SHORT:
		GL_P_TRANSFORM_NORMAL_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLshort, I );
		break;
	case GL_UNSIGNED_SHORT:
		GL_P_TRANSFORM_NORMAL_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLushort, I );
		break;
	case GL_BYTE:
		GL_P_TRANSFORM_NORMAL_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLbyte, I );
		break;
	case GL_UNSIGNED_BYTE:
		GL_P_TRANSFORM_NORMAL_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLubyte, I );
		break;
	}
}

/***************************************************************************/
#define GL_P_TRANSFORM_TEXCOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GL_TYPE, SRC ) \
{ \
	BP3D_Vertex *pVertex = pContext->pVertices; \
	GL_P_Matrix *pMat = &pContext->TextureMatrixStack.aMatrices[pContext->TextureMatrixStack.uiTop]; \
	GLushort ii; \
\
	GLsizei iByteStride = pContext->TexCoordArray.iStride ? pContext->TexCoordArray.iStride : (pContext->TexCoordArray.iSize * (GLsizei) sizeof (GL_TYPE)); \
	GLsizei iStride = pContext->bTexCoordArray ? iByteStride / sizeof (GL_TYPE) : 0; \
	GLsizei iIndexStride = (pvIndices && pContext->bTexCoordArray) ? ((eIndexType == GL_UNSIGNED_BYTE) ? 1 : 2) : 0; \
	GLushort usIndexMask = (pvIndices && (eIndexType == GL_UNSIGNED_BYTE)) ? 0xFF : 0xFFFF; \
	GLushort *pusIndex = (GLushort *) (pvIndices ? pvIndices : (&ii)); \
	GL_TYPE *ptMemory = (GL_TYPE *) pContext->TexCoordArray.pvMemory + iFirst * iStride; \
	GL_TYPE *ptCoord = pContext->bTexCoordArray ? ptMemory : ((GL_TYPE *) (void *) (&pContext->TexCoord)); \
\
	for( ii = 0; ii < iCount; ++ii ) \
	{ \
		GL_TYPE *ptTexCoord = ptCoord + iStride * \
			((*((GLushort *) ((GLubyte *) pusIndex + iIndexStride * ii))) & usIndexMask); \
\
		pVertex->fS = GL_P_CONVERT_##SRC##toF(pMat->a[0] * ptTexCoord[0] + pMat->a[4] * ptTexCoord[1] + pMat->a[12]); \
		pVertex->fT = GL_P_CONVERT_##SRC##toF(pMat->a[1] * ptTexCoord[0] + pMat->a[5] * ptTexCoord[1] + pMat->a[13]); \
		pVertex++; \
	} \
}

/***************************************************************************/
void GL_P_TransformTexCoordArray( 
	EGL_P_Context *pContext, 
	GLint iFirst, 
	GLsizei iCount,
	GLenum eIndexType, 
	const GLvoid *pvIndices )
{
	GLenum eType = pContext->bTexCoordArray ? pContext->TexCoordArray.eType : GL_FLOAT;

	switch( eType )
	{
	case GL_FLOAT:
		GL_P_TRANSFORM_TEXCOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfloat, F );
		break;
	case GL_FIXED:
		GL_P_TRANSFORM_TEXCOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfixed, X );
		break;
	case GL_SHORT:
		GL_P_TRANSFORM_TEXCOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLshort, I );
		break;
	case GL_UNSIGNED_SHORT:
		GL_P_TRANSFORM_TEXCOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLushort, I );
		break;
	case GL_BYTE:
		GL_P_TRANSFORM_TEXCOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLbyte, I );
		break;
	case GL_UNSIGNED_BYTE:
		GL_P_TRANSFORM_TEXCOORD_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLubyte, I );
		break;
	}
}

/***************************************************************************/
#define GL_P_COPY_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GL_TYPE, SRC ) \
{ \
	BP3D_Vertex *pVertex = pContext->pVertices; \
	GLushort ii; \
\
	GLsizei iByteStride = pContext->ColorArray.iStride ? pContext->ColorArray.iStride : (pContext->ColorArray.iSize * (GLsizei) sizeof (GL_TYPE)); \
	GLsizei iStride = pContext->bColorArray ? iByteStride / sizeof (GL_TYPE) : 0; \
	GLsizei iIndexStride = (pvIndices && pContext->bColorArray) ? ((eIndexType == GL_UNSIGNED_BYTE) ? 1 : 2) : 0; \
	GLushort usIndexMask = (pvIndices && (eIndexType == GL_UNSIGNED_BYTE)) ? 0xFF : 0xFFFF; \
	GLushort *pusIndex = (GLushort *) (pvIndices ? pvIndices : (&ii)); \
	GL_TYPE *ptMemory = (GL_TYPE *) pContext->ColorArray.pvMemory + iFirst * iStride; \
	GL_TYPE *ptColorBase = pContext->bColorArray ? ptMemory : ((GL_TYPE *) (void *) (&pContext->Color)); \
\
	for( ii = 0; ii < iCount; ++ii ) \
	{ \
		GL_TYPE *ptColor = ptColorBase + iStride * \
			((*((GLushort *) ((GLubyte *) pusIndex + iIndexStride * ii))) & usIndexMask); \
\
		pVertex->ulDiffuse = GL_P_CONVERT_COLOR_##SRC##toI( ptColor[3], ptColor[0], ptColor[1], ptColor[2] ); \
		pVertex++; \
	} \
}

/***************************************************************************/
void GL_P_CopyColorArray( 
	EGL_P_Context *pContext, 
	GLint iFirst, 
	GLsizei iCount,
	GLenum eIndexType, 
	const GLvoid *pvIndices )
{
	GLenum eType = pContext->bColorArray ? pContext->ColorArray.eType : GL_FLOAT;

	switch( eType )
	{
	case GL_FLOAT:
		GL_P_COPY_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfloat, F );
		break;
	case GL_FIXED:
		GL_P_COPY_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfixed, X );
		break;
	case GL_SHORT:
		GL_P_COPY_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLshort, I );
		break;
	case GL_UNSIGNED_SHORT:
		GL_P_COPY_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLushort, I );
		break;
	case GL_BYTE:
		GL_P_COPY_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLbyte, I );
		break;
	case GL_UNSIGNED_BYTE:
		GL_P_COPY_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLubyte, I );
		break;
	}
}

/***************************************************************************/
#define GL_P_TRANSFORM_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GL_TYPE, SRC ) \
{ \
	BP3D_Vertex *pVertex = pContext->pVertices; \
	GLushort ii; \
\
	GLsizei iByteStride = pContext->ColorArray.iStride ? pContext->ColorArray.iStride : (pContext->ColorArray.iSize * (GLsizei) sizeof (GL_TYPE)); \
	GLsizei iStride = pContext->bColorArray ? iByteStride / sizeof (GL_TYPE) : 0; \
	GLsizei iIndexStride = (pvIndices && pContext->bColorArray) ? ((eIndexType == GL_UNSIGNED_BYTE) ? 1 : 2) : 0; \
	GLushort usIndexMask = (pvIndices && (eIndexType == GL_UNSIGNED_BYTE)) ? 0xFF : 0xFFFF; \
	GLushort *pusIndex = (GLushort *) (pvIndices ? pvIndices : (&ii)); \
	GL_TYPE *ptMemory = (GL_TYPE *) pContext->ColorArray.pvMemory + iFirst * iStride; \
	GL_TYPE *ptColorBase = pContext->bColorArray ? ptMemory : ((GL_TYPE *) (void *) (&pContext->Color)); \
\
	for( ii = 0; ii < iCount; ++ii ) \
	{ \
		GL_TYPE *ptColor = ptColorBase + iStride * \
			((*((GLushort *) ((GLubyte *) pusIndex + iIndexStride * ii))) & usIndexMask); \
		GL_P_Color *pColor = (GL_P_Color *) (&pVertex->fS); \
\
		GL_P_SET_LIGHT_COLOR( pColor, \
			GL_P_CONVERT_CHANNEL_##SRC##toF(ptColor[0]), GL_P_CONVERT_CHANNEL_##SRC##toF(ptColor[1]), \
			GL_P_CONVERT_CHANNEL_##SRC##toF(ptColor[2]), GL_P_CONVERT_CHANNEL_##SRC##toF(ptColor[3]) ); \
		pVertex++; \
	} \
}

/***************************************************************************/
void GL_P_TransformColorArray( 
	EGL_P_Context *pContext, 
	GLint iFirst, 
	GLsizei iCount,
	GLenum eIndexType, 
	const GLvoid *pvIndices )
{
	GLenum eType = pContext->bColorArray ? pContext->ColorArray.eType : GL_FLOAT;

	switch( eType )
	{
	case GL_FLOAT:
		GL_P_TRANSFORM_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfloat, F );
		break;
	case GL_FIXED:
		GL_P_TRANSFORM_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLfixed, X );
		break;
	case GL_SHORT:
		GL_P_TRANSFORM_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLshort, I );
		break;
	case GL_UNSIGNED_SHORT:
		GL_P_TRANSFORM_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLushort, I );
		break;
	case GL_BYTE:
		GL_P_TRANSFORM_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLbyte, I );
		break;
	case GL_UNSIGNED_BYTE:
		GL_P_TRANSFORM_COLOR_ARRAY( pContext, iFirst, iCount, eIndexType, pvIndices, GLubyte, I );
		break;
	}
}

/***************************************************************************/
void GL_P_TransformVertex( 
	EGL_P_Context *pContext,
	BP3D_Vertex *pVOut,
	BP3D_Vertex *pVIn )
{
	GL_P_Viewport *pViewport = &pContext->Viewport;
	GLfloat fInvW = pVIn->fInvW * pViewport->fGBInvEpsilon;

	pVOut->fX = ( pVIn->fX * fInvW + 1.0) * pViewport->fGBWidth / 2 + 0.2;
	pVOut->fY = (-pVIn->fY * fInvW + 1.0) * pViewport->fGBHeight / 2 + 0.2;
	pVOut->fZ = ((pVIn->fZ * pVIn->fInvW + 1.0) / 2 - pContext->fDepthNear) * pContext->fInvDepthRange + pContext->fDepthOffset;
	pVOut->fZ = GL_P_MIN(GL_P_MAX(pVOut->fZ, 0.0), 1.0);
	pVOut->fW = pVIn->fW;
	pVOut->ulDiffuse = pVIn->ulDiffuse;
	pVOut->ulSpecular = pVIn->ulSpecular;
	pVOut->fS = pVIn->fS;
	pVOut->fT = pVIn->fT;
	pVOut->fInvW = pVIn->fInvW;
	pVOut->sX = (GLshort) pVOut->fX;
	pVOut->sY = (GLshort) pVOut->fY;

	if( pContext->bFog )
		GL_P_SetVertexFogIndex( pContext, pVIn, pVOut );
}

/***************************************************************************/
void GL_P_TransformVertices( 
	EGL_P_Context *pContext, 
	GLsizei iCount )
{
	BP3D_Vertex *pVertex = pContext->pVertices;
	GL_P_Viewport *pViewport = &pContext->Viewport;
	GLsizei ii;

	for( ii = 0; ii < iCount; ++ii )
	{
		GLfloat fInvW = pVertex->fInvW * pViewport->fGBInvEpsilon;

		if( pContext->bFog )
			GL_P_SetVertexFogIndex( pContext, pVertex, pVertex );

		pVertex->fX = ( pVertex->fX * fInvW + 1.0) * pViewport->fGBWidth / 2 + 0.2;
		pVertex->fY = (-pVertex->fY * fInvW + 1.0) * pViewport->fGBHeight / 2 + 0.2;
		pVertex->fZ = ((pVertex->fZ * pVertex->fInvW + 1.0) / 2 - pContext->fDepthNear) * pContext->fInvDepthRange + pContext->fDepthOffset;
		pVertex->fZ = GL_P_MIN(GL_P_MAX(pVertex->fZ, 0.0), 1.0);
		pVertex->sX = (GLshort) pVertex->fX;
		pVertex->sY = (GLshort) pVertex->fY;
		pVertex++;
	}
}

/* End of File */
