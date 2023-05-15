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
 * $brcm_Workfile: gl_clip.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Clipping private functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_clip.c $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 4:44p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/1   8/6/10 8:07p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
 * 
 * Hydra_Software_Devel/5   9/23/08 6:29p nissen
 * PR 46638: Optimized rectangle blits.
 * 
 * Hydra_Software_Devel/4   9/8/08 12:41p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/3   10/17/05 9:05p nissen
 * PR 15708: Fixed clipping problem.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:55p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include <direct/messages.h>

#include "gl_private.h"

/***************************************************************************/
#define GL_P_CLIP_CURR_OUT   1
#define GL_P_CLIP_NEXT_OUT   2
#define GL_P_CLIP_IN_IN      0
#define GL_P_CLIP_OUT_IN     1
#define GL_P_CLIP_IN_OUT     2
#define GL_P_CLIP_OUT_OUT    3

/***************************************************************************/
#define GL_P_INTERP_COLOR( ci, co, sf ) ( \
	(((((ci) >> 24) + (int32_t) ((((GLfloat) (((co) >> 24) & 0xFF)) - ((GLfloat) (((ci) >> 24) & 0xFF))) * (sf))) & 0xFF) << 24) | \
	(((((ci) >> 16) + (int32_t) ((((GLfloat) (((co) >> 16) & 0xFF)) - ((GLfloat) (((ci) >> 16) & 0xFF))) * (sf))) & 0xFF) << 16) | \
	(((((ci) >>  8) + (int32_t) ((((GLfloat) (((co) >>  8) & 0xFF)) - ((GLfloat) (((ci) >>  8) & 0xFF))) * (sf))) & 0xFF) <<  8) | \
	(((((ci) >>  0) + (int32_t) ((((GLfloat) (((co) >>  0) & 0xFF)) - ((GLfloat) (((ci) >>  0) & 0xFF))) * (sf))) & 0xFF) <<  0))

/***************************************************************************/
#define GL_P_INTERP_VERTEX( xx, yy, zz, pvi, pvo, pvn, dir ) \
{ \
	GLfloat fDX = (pvo)->f##xx - (pvi)->f##xx; \
	GLfloat fDW = (pvo)->fW - (pvi)->fW; \
	GLfloat fDDiff = fDW - fDX * (dir); \
	GLfloat fScale = (fDDiff != 0.0 ) ? ((dir) * (pvi)->f##xx - (pvi)->fW) / fDDiff : 0.0f; \
	GLfloat fNewW = (pvi)->fW + (fScale * fDW); \
	(pvn)->f##xx = fNewW * (dir); \
	(pvn)->f##yy = (pvi)->f##yy + fScale * ((pvo)->f##yy - (pvi)->f##yy); \
	(pvn)->f##zz = (pvi)->f##zz + fScale * ((pvo)->f##zz - (pvi)->f##zz); \
	(pvn)->fW = fNewW; \
	(pvn)->ulDiffuse = GL_P_INTERP_COLOR((pvi)->ulDiffuse, (pvo)->ulDiffuse, fScale); \
	(pvn)->ulSpecular = GL_P_INTERP_COLOR((pvi)->ulSpecular, (pvo)->ulSpecular, fScale); \
	(pvn)->fS = (pvi)->fS + fScale * ((pvo)->fS - (pvi)->fS); \
	(pvn)->fT = (pvi)->fT + fScale * ((pvo)->fT - (pvi)->fT); \
	(pvn)->fInvW = 1.0f / fNewW; \
}

/***************************************************************************/
#define GL_P_CLIP_VERTEX_MIN( xx, yy, zz, count ) \
{ \
	if( iPingCount < (count) ) \
		return; \
	iPongCount = 0; \
	for( ii = 0; ii < iPingCount; ++ii ) \
	{ \
		GLsizei iClipCase = GL_P_CLIP_IN_IN; \
		pFirstVertex = apPingVertices[ii]; \
		pNextVertex = apPingVertices[((ii + 1) == iPingCount) ? 0 : (ii + 1)]; \
		if( pFirstVertex->f##xx < -pFirstVertex->fW ) \
			iClipCase += GL_P_CLIP_CURR_OUT; \
		if( pNextVertex->f##xx < -pNextVertex->fW ) \
			iClipCase += GL_P_CLIP_NEXT_OUT; \
		switch( iClipCase ) \
		{ \
		case GL_P_CLIP_IN_IN: \
			apPongVertices[iPongCount++] = pNextVertex; \
			break; \
		case GL_P_CLIP_IN_OUT: \
			GL_P_INTERP_VERTEX( xx, yy, zz, pFirstVertex, pNextVertex, aVertices + iVertexCount, -1 ); \
			apPongVertices[iPongCount++] = aVertices + iVertexCount++; \
			break; \
		case GL_P_CLIP_OUT_OUT: \
			break; \
		case GL_P_CLIP_OUT_IN: \
			GL_P_INTERP_VERTEX( xx, yy, zz, pNextVertex, pFirstVertex, aVertices + iVertexCount, -1 ); \
			apPongVertices[iPongCount++] = aVertices + iVertexCount++; \
			apPongVertices[iPongCount++] = pNextVertex; \
			break; \
		} \
	} \
}

/***************************************************************************/
#define GL_P_CLIP_VERTEX_MAX( xx, yy, zz, count ) \
{ \
	if( iPongCount < (count) ) \
		return; \
	iPingCount = 0; \
	for( ii = 0; ii < iPongCount; ++ii ) \
	{ \
		GLsizei iClipCase = GL_P_CLIP_IN_IN; \
		pFirstVertex = apPongVertices[ii]; \
		pNextVertex = apPongVertices[((ii + 1) == iPongCount) ? 0 : (ii + 1)]; \
		if( pFirstVertex->f##xx > pFirstVertex->fW  ) \
			iClipCase += GL_P_CLIP_CURR_OUT; \
		if( pNextVertex->f##xx > pNextVertex->fW ) \
			iClipCase += GL_P_CLIP_NEXT_OUT; \
		switch( iClipCase ) \
		{ \
		case GL_P_CLIP_IN_IN: \
			apPingVertices[iPingCount++] = pNextVertex; \
			break; \
		case GL_P_CLIP_IN_OUT: \
			GL_P_INTERP_VERTEX( xx, yy, zz, pFirstVertex, pNextVertex, aVertices + iVertexCount, 1 ); \
			apPingVertices[iPingCount++] = aVertices + iVertexCount++; \
			break; \
		case GL_P_CLIP_OUT_OUT: \
			break; \
		case GL_P_CLIP_OUT_IN: \
			GL_P_INTERP_VERTEX( xx, yy, zz, pNextVertex, pFirstVertex, aVertices + iVertexCount, 1 ); \
			apPingVertices[iPingCount++] = aVertices + iVertexCount++; \
			apPingVertices[iPingCount++] = pNextVertex; \
			break; \
		} \
	} \
}

/***************************************************************************/
#define GL_P_INTERP_VERTEX_RECTANGLE( xx, ss, pvi, pvo, dir ) \
{ \
	GLfloat fDX = (pvo)->f##xx - (pvi)->f##xx; \
	GLfloat fDW = (pvo)->fW - (pvi)->fW; \
	GLfloat fDDiff = fDW - fDX * (dir); \
	GLfloat fScale = (fDDiff != 0.0 ) ? ((dir) * (pvi)->f##xx - (pvi)->fW) / fDDiff : 0.0f; \
	GLfloat fNewW = (pvi)->fW + (fScale * fDW); \
	(pvo)->f##xx = fNewW * (dir); \
	(pvo)->fW = fNewW; \
	(pvo)->ulDiffuse = GL_P_INTERP_COLOR((pvi)->ulDiffuse, (pvo)->ulDiffuse, fScale); \
	(pvo)->ulSpecular = GL_P_INTERP_COLOR((pvi)->ulSpecular, (pvo)->ulSpecular, fScale); \
	(pvo)->f##ss = (pvi)->f##ss + fScale * ((pvo)->f##ss - (pvi)->f##ss); \
	(pvo)->fInvW = 1.0f / fNewW; \
}

/***************************************************************************/
#define GL_P_CLIP_RECTANGLE_3D( xx, ss, pv0, pv1, dir, clipcase ) \
{ \
	switch( clipcase ) \
	{ \
	case GL_P_CLIP_IN_OUT: GL_P_INTERP_VERTEX_RECTANGLE( xx, ss, pv0, pv1, dir ); break; \
	case GL_P_CLIP_OUT_IN: GL_P_INTERP_VERTEX_RECTANGLE( xx, ss, pv1, pv0, dir ); break; \
	case GL_P_CLIP_OUT_OUT: return; \
	} \
}

/***************************************************************************/
#define GL_P_CLIP_RECTANGLE_2D( xx, ss, phase, scale, lod, left, size ) \
{ \
	float fLeft = (float) (left); \
	float fRight = (float) (left) + (size); \
	if( (xx) < fLeft ) \
	{ \
		(phase) = (((scale) < 1) ? 1.0 : 0.0) / ((lod) + 1); \
		(ss) += (scale) * (fLeft - (xx)) - (phase); \
		(xx) = fLeft; \
	} \
	else if( (xx) > fRight ) \
 	{ \
		float fOffset = (((scale) < 1) ? 1.0 : 0.5) / ((lod) + 1); \
		(ss) -= (scale) * ((xx) - fRight) - fOffset; \
		(xx) = fRight; \
	} \
}

/***************************************************************************/
void GL_P_RenderPoint(
	p3dOpenGLContextData *pContext,
	BP3D_Vertex *pV0 )
{
	BP3D_Vertex v;

	GL_P_TransformVertex( pContext, &v, pV0 );
	BP3D_DrawPoint( pContext->hP3d, &v );
}

/***************************************************************************/
void GL_P_ClipAndRenderPoints(
	p3dOpenGLContextData *pContext,
	GLsizei iVertexCount )
{
	GLsizei ii;

	if( pContext->ulVertexOutCode )
	{
		for( ii = 0; ii < iVertexCount; ++ii )
		{
			BP3D_Vertex *pV0 = pContext->pVertices + ii;
			if( GL_P_GET_OUTCODE(pV0) == 0 )
				GL_P_RenderPoint( pContext, pV0 );
		}
	}
	else
	{
		for( ii = 0; ii < iVertexCount; ++ii )
			BP3D_DrawPoint( pContext->hP3d, pContext->pVertices + ii );
	}
}

/***************************************************************************/
void GL_P_RenderLine(
	p3dOpenGLContextData *pContext,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1 )
{
	BP3D_Vertex v[2];

	GL_P_TransformVertex( pContext, v + 0, pV0 );
	GL_P_TransformVertex( pContext, v + 1, pV1 );
	BP3D_DrawLine( pContext->hP3d, v, v + 1 );
}

/***************************************************************************/
void GL_P_ClipAndRenderLine(
	p3dOpenGLContextData *pContext,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1 )
{
	BP3D_Vertex aVertices[24];
	BP3D_Vertex aVerticesOut[10];
	BP3D_Vertex *apPingVertices[10];
	BP3D_Vertex *apPongVertices[10];
	BP3D_Vertex *pFirstVertex;
	BP3D_Vertex *pNextVertex;
	GLsizei iVertexCount = 2;
	GLsizei iPingCount = 2;
	GLsizei iPongCount = 0;
	GLsizei ii;

	aVertices[0] = *pV0;
	aVertices[1] = *pV1;

	apPingVertices[0] = aVertices;
	apPingVertices[1] = aVertices + 1;

	GL_P_CLIP_VERTEX_MIN( X, Y, Z, 2 );
	GL_P_CLIP_VERTEX_MAX( X, Y, Z, 2 );
	GL_P_CLIP_VERTEX_MIN( Y, Z, X, 2 );
	GL_P_CLIP_VERTEX_MAX( Y, Z, X, 2 );
	GL_P_CLIP_VERTEX_MIN( Z, X, Y, 2 );
	GL_P_CLIP_VERTEX_MAX( Z, X, Y, 2 );

	for( ii = 0; ii < iPingCount; ++ii )
		GL_P_TransformVertex( pContext, &aVerticesOut[ii], apPingVertices[ii] );

	for( ii = 0; ii < iPingCount - 1; ++ii )
		BP3D_DrawLine( pContext->hP3d, &aVerticesOut[0], &aVerticesOut[ii + 1] );
}

/***************************************************************************/
void GL_P_ClipAndRenderLineStrip(
	p3dOpenGLContextData *pContext,
	GLsizei iVertexCount )
{
	GLsizei ii;

	if( pContext->ulVertexOutCode )
	{
		for( ii = 0; ii < iVertexCount - 1; ++ii )
		{
			BP3D_Vertex *pV0 = pContext->pVertices + ii;
			BP3D_Vertex *pV1 = pContext->pVertices + ii + 1;
			GLuint ulOutCode0 = GL_P_GET_OUTCODE(pV0);
			GLuint ulOutCode1 = GL_P_GET_OUTCODE(pV1);

			if( (ulOutCode0 & ulOutCode1) == 0 )
			{
				if( ulOutCode0 | ulOutCode1 )
					GL_P_ClipAndRenderLine( pContext, pV0, pV1 );
				else
					GL_P_RenderLine( pContext, pV0, pV1 );
			}
		}
	}
	else
	{
		for( ii = 0; ii < iVertexCount - 1; ++ii )
			BP3D_DrawLine( pContext->hP3d, pContext->pVertices + ii, pContext->pVertices + ii + 1 );
	}
}

/***************************************************************************/
void GL_P_ClipAndRenderLineLoop(
	p3dOpenGLContextData *pContext,
	GLsizei iVertexCount )
{
	GLsizei ii;

	if( pContext->ulVertexOutCode )
	{
		for( ii = 0; ii < iVertexCount; ++ii )
		{
			BP3D_Vertex *pV0 = pContext->pVertices + ii;
			BP3D_Vertex *pV1 = pContext->pVertices + ((ii < iVertexCount - 1) ? ii + 1 : 0);
			GLuint ulOutCode0 = GL_P_GET_OUTCODE(pV0);
			GLuint ulOutCode1 = GL_P_GET_OUTCODE(pV1);

			if( (ulOutCode0 & ulOutCode1) == 0 )
			{
				if( ulOutCode0 | ulOutCode1 )
					GL_P_ClipAndRenderLine( pContext, pV0, pV1 );
				else
					GL_P_RenderLine( pContext, pV0, pV1 );
			}
		}
	}
	else
	{
		for( ii = 0; ii < iVertexCount; ++ii )
			BP3D_DrawLine( pContext->hP3d, pContext->pVertices + ii, 
				pContext->pVertices + ((ii < iVertexCount - 1) ? ii + 1 : 0) );
	}
}

/***************************************************************************/
void GL_P_ClipAndRenderLines(
	p3dOpenGLContextData *pContext,
	GLsizei iVertexCount )
{
	GLsizei ii;

	if( pContext->ulVertexOutCode )
	{
		for( ii = 0; ii < iVertexCount / 2; ++ii )
		{
			BP3D_Vertex *pV0 = pContext->pVertices + ii * 2;
			BP3D_Vertex *pV1 = pContext->pVertices + ii * 2 + 1;
			GLuint ulOutCode0 = GL_P_GET_OUTCODE(pV0);
			GLuint ulOutCode1 = GL_P_GET_OUTCODE(pV1);

			if( (ulOutCode0 & ulOutCode1) == 0 )
			{
				if( ulOutCode0 | ulOutCode1 )
					GL_P_ClipAndRenderLine( pContext, pV0, pV1 );
				else
					GL_P_RenderLine( pContext, pV0, pV1 );
			}
		}
	}
	else
	{
		for( ii = 0; ii < iVertexCount / 2; ++ii )
			BP3D_DrawLine( pContext->hP3d, pContext->pVertices + ii * 2, pContext->pVertices + ii * 2 + 1 );
	}
}

/***************************************************************************/
void GL_P_RenderTriangle(
	p3dOpenGLContextData *pContext,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1,
	BP3D_Vertex *pV2 )
{
	BP3D_Vertex v[3];

	GL_P_TransformVertex( pContext, v + 0, pV0 );
	GL_P_TransformVertex( pContext, v + 1, pV1 );
	GL_P_TransformVertex( pContext, v + 2, pV2 );
	BP3D_DrawTriangle( pContext->hP3d, v, v + 1, v + 2 );
}

/***************************************************************************/
void GL_P_ClipAndRenderTriangle(
	p3dOpenGLContextData *pContext,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1,
	BP3D_Vertex *pV2 )
{
	BP3D_Vertex aVertices[24];
	BP3D_Vertex aVerticesOut[10];
	BP3D_Vertex *apPingVertices[10];
	BP3D_Vertex *apPongVertices[10];
	BP3D_Vertex *pFirstVertex;
	BP3D_Vertex *pNextVertex;
	GLsizei iVertexCount = 3;
	GLsizei iPingCount = 3;
	GLsizei iPongCount = 0;
	GLsizei ii;

	aVertices[0] = *pV0; 
	aVertices[1] = *pV1;
	aVertices[2] = *pV2;

	apPingVertices[0] = aVertices;
	apPingVertices[1] = aVertices + 1;
	apPingVertices[2] = aVertices + 2;

	GL_P_CLIP_VERTEX_MIN( X, Y, Z, 3 );
	GL_P_CLIP_VERTEX_MAX( X, Y, Z, 3 );
	GL_P_CLIP_VERTEX_MIN( Y, Z, X, 3 );
	GL_P_CLIP_VERTEX_MAX( Y, Z, X, 3 );
	GL_P_CLIP_VERTEX_MIN( Z, X, Y, 3 );
	GL_P_CLIP_VERTEX_MAX( Z, X, Y, 3 );

	for( ii = 0; ii < iPingCount; ++ii )
		GL_P_TransformVertex( pContext, &aVerticesOut[ii], apPingVertices[ii] );

	BP3D_DrawTriangleFan( pContext->hP3d, aVerticesOut, iPingCount - 2 );
}

/***************************************************************************/
void GL_P_ClipAndRenderTriangleStrip(
	p3dOpenGLContextData *pContext,
	GLsizei iVertexCount )
{
	GLsizei ii;

	if( pContext->ulVertexOutCode )
	{
		for( ii = 0; ii < iVertexCount - 2; ++ii )
		{
			BP3D_Vertex *pV0 = pContext->pVertices + ii;
			BP3D_Vertex *pV1 = pContext->pVertices + ii + (ii & 1) + 1;
			BP3D_Vertex *pV2 = pContext->pVertices + ii + 2 - (ii & 1);
			GLuint ulOutCode0 = GL_P_GET_OUTCODE(pV0);
			GLuint ulOutCode1 = GL_P_GET_OUTCODE(pV1);
			GLuint ulOutCode2 = GL_P_GET_OUTCODE(pV2);

			if( (ulOutCode0 & ulOutCode1 & ulOutCode2) == 0 )
			{
				if( ulOutCode0 | ulOutCode1 | ulOutCode2 )
					GL_P_ClipAndRenderTriangle( pContext, pV0, pV1, pV2 );
				else
					GL_P_RenderTriangle( pContext, pV0, pV1, pV2 );
			}
		}
	}
	else
	{
		BP3D_DrawTriangleStrip( pContext->hP3d, pContext->pVertices, iVertexCount - 2 );
	}
}

/***************************************************************************/
void GL_P_ClipAndRenderTriangleFan(
	p3dOpenGLContextData *pContext,
	GLsizei iVertexCount )
{
	GLsizei ii;

	if( pContext->ulVertexOutCode )
	{
		BP3D_Vertex *pV0 = pContext->pVertices;
		BP3D_Vertex *pV1 = pContext->pVertices + 1;
		GLuint ulOutCode0 = GL_P_GET_OUTCODE(pV0);
		GLuint ulOutCode1 = GL_P_GET_OUTCODE(pV1);

		for( ii = 0; ii < iVertexCount - 2; ++ii )
		{
			BP3D_Vertex *pV2 = pContext->pVertices + ii + 2;
			GLuint ulOutCode2 = GL_P_GET_OUTCODE(pV2);

			if( (ulOutCode0 & ulOutCode1 & ulOutCode2) == 0 )
			{
				if( ulOutCode0 | ulOutCode1 | ulOutCode2 )
					GL_P_ClipAndRenderTriangle( pContext, pV0, pV1, pV2 );
				else
					GL_P_RenderTriangle( pContext, pV0, pV1, pV2 );
			}

			pV1 = pV2;
			ulOutCode1 = ulOutCode2;
		}
	}
	else
	{
		BP3D_DrawTriangleFan( pContext->hP3d, pContext->pVertices, iVertexCount - 2 );
	}
}

/***************************************************************************/
void GL_P_ClipAndRenderTriangles(
	p3dOpenGLContextData *pContext,
	GLsizei iVertexCount )
{
	GLsizei ii;

	if( pContext->ulVertexOutCode )
	{
		for( ii = 0; ii < iVertexCount / 3; ++ii )
		{
			BP3D_Vertex *pV0 = pContext->pVertices + ii * 3;
			BP3D_Vertex *pV1 = pContext->pVertices + ii * 3 + 1;
			BP3D_Vertex *pV2 = pContext->pVertices + ii * 3 + 2;
			GLuint ulOutCode0 = GL_P_GET_OUTCODE(pV0);
			GLuint ulOutCode1 = GL_P_GET_OUTCODE(pV1);
			GLuint ulOutCode2 = GL_P_GET_OUTCODE(pV2);

			if( (ulOutCode0 & ulOutCode1 & ulOutCode2) == 0 )
			{
//                             D_INFO("RenderTriangle\n");
				if( ulOutCode0 | ulOutCode1 | ulOutCode2 )
					GL_P_ClipAndRenderTriangle( pContext, pV0, pV1, pV2 );
				else
					GL_P_RenderTriangle( pContext, pV0, pV1, pV2 );
			}
		}
	}
	else
	{
        /*
             BP3D_Vertex *v = pContext->pVertices;

             D_INFO("DrawTriangleList %f, %f, %f   %f, %f, %f   %f, %f, %f   [%f, %f]   [%f, %f]   [%f, %f]\n",
                    v[0].fX, v[0].fY, v[0].fZ,
                    v[1].fX, v[1].fY, v[1].fZ,
                    v[2].fX, v[2].fY, v[2].fZ,
                    v[0].fS, v[0].fT,
                    v[1].fS, v[1].fT,
 		    v[2].fS, v[2].fT );
 */
		BP3D_DrawTriangleList( pContext->hP3d, pContext->pVertices, iVertexCount / 3 );
	}
}

/***************************************************************************/
void GL_P_RenderRectangle(
	p3dOpenGLContextData *pContext,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1 )
{
	BP3D_Vertex v[2];

	GL_P_TransformVertex( pContext, v + 0, pV0 );
	GL_P_TransformVertex( pContext, v + 1, pV1 );
	BP3D_DrawRectangle( pContext->hP3d, v, v + 1 );
}

/***************************************************************************/
#define GL_P_HALF    0.49999

/***************************************************************************/
#if 0
void GL_P_BlitRectangle(
	p3dOpenGLContextData *pContext,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1,
	int iCount )
{
	BSUR_Surface_Handle hTextureSurface = pContext->bTexture2D ? pContext->pTexture->hSurface : 0;
	int iTextureWidth = hTextureSurface ? pContext->pTexture->iWidth : 0;
	int iTextureHeight = hTextureSurface ? pContext->pTexture->iHeight : 0;
	int iLod = 0;

	float fDstX0 = pV0->fX - pContext->iViewportX + pContext->iViewportOffsetX;
	float fDstY0 = pV0->fY - pContext->iViewportY + pContext->iViewportOffsetY;
	float fDstX1 = pV1->fX - pContext->iViewportX + pContext->iViewportOffsetX;
	float fDstY1 = pV1->fY - pContext->iViewportY + pContext->iViewportOffsetY;

	float fSrcS0 = pV0->fS * iTextureWidth;
	float fSrcT0 = pV0->fT * iTextureHeight;
 	float fSrcS1 = pV1->fS * iTextureWidth;
	float fSrcT1 = pV1->fT * iTextureHeight;

	float fDeltaX = fDstX1 - fDstX0;
	float fDeltaY = fDstY1 - fDstY0;
  	float fDeltaS = fSrcS1 - fSrcS0;
	float fDeltaT = fSrcT1 - fSrcT0;

	float fScaleX = (fDeltaX != 0) ? fDeltaS / fDeltaX : 0;
	float fScaleY = (fDeltaY != 0) ? fDeltaT / fDeltaY : 0;
	float fPhaseX = 0;
	float fPhaseY = 0;

	if( (pV0->fZ < -pV0->fW) || (pV1->fZ < -pV1->fW) ||
		(pV0->fZ >  pV0->fW) || (pV1->fZ >  pV1->fW) )
	{
		return;
	}

	/* mipmap */
	if( hTextureSurface && pContext->pTexture->bMipmap && (fScaleX > 1) )
	{
		iLod = (int) (log(fabs(fScaleX)) * 1.4427 + 0.5);
		if( iLod >= pContext->pTexture->iLevelCount )
			iLod = pContext->pTexture->iLevelCount - 1;

		while( iLod > 0 )
		{
			if( iLod < pContext->pTexture->iLevelCount )
			{
				GL_P_MipmapLevel *pLevel = &pContext->pTexture->aLevels[iLod];
				if( pLevel->hSurface )
				{
					hTextureSurface = pLevel->hSurface;
					fSrcS0 = pV0->fS * pLevel->iWidth;
					fSrcT0 = pV0->fT * pLevel->iHeight;
 					fSrcS1 = pV1->fS * pLevel->iWidth;
					fSrcT1 = pV1->fT * pLevel->iHeight;
  					fDeltaS = fSrcS1 - fSrcS0;
					fDeltaT = fSrcT1 - fSrcT0;
					fScaleX = (fDeltaX != 0) ? fDeltaS / fDeltaX : 0;
					fScaleY = (fDeltaY != 0) ? fDeltaT / fDeltaY : 0;
					break;
				}
			}
			iLod--;
		}
	}

	/* clip */
	GL_P_CLIP_RECTANGLE_2D( fDstX0, fSrcS0, fPhaseX, fScaleX, iLod, pContext->iViewportOffsetX, pContext->iViewportWidth );
	GL_P_CLIP_RECTANGLE_2D( fDstX1, fSrcS1, fPhaseX, fScaleX, iLod, pContext->iViewportOffsetX, pContext->iViewportWidth );
	GL_P_CLIP_RECTANGLE_2D( fDstY0, fSrcT0, fPhaseY, fScaleY, iLod, pContext->iViewportOffsetY, pContext->iViewportHeight );
	GL_P_CLIP_RECTANGLE_2D( fDstY1, fSrcT1, fPhaseY, fScaleY, iLod, pContext->iViewportOffsetY, pContext->iViewportHeight );

	/* blit */
	native_P_BlitSurface( pContext, pContext->nativeDisplay, pContext->pDrawSurface->hRenderSurface, 
		fDstX0 + GL_P_HALF, fDstY0 + GL_P_HALF, fDstX1 - fDstX0 + GL_P_HALF, fDstY1 - fDstY0 + GL_P_HALF, hTextureSurface, 
		fSrcS0 + GL_P_HALF, fSrcT0 + GL_P_HALF, fSrcS1 - fSrcS0 + GL_P_HALF, fSrcT1 - fSrcT0 + GL_P_HALF, 
		fSrcS0 + fPhaseX, fSrcT0 + fPhaseY, fScaleX, fScaleY, pV0->ulDiffuse, iCount ? GL_TRUE : GL_FALSE );
}
#endif

/***************************************************************************/
void GL_P_ClipAndRenderRectangle(
	p3dOpenGLContextData *pContext,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1 )
{
	BP3D_Vertex v0 = *pV0;
	BP3D_Vertex v1 = *pV1;

	GLsizei iClipCaseMinX = ((pV0->fX < -pV0->fW) ? GL_P_CLIP_CURR_OUT : 0) + ((pV1->fX < -pV1->fW) ? GL_P_CLIP_NEXT_OUT : 0);
	GLsizei iClipCaseMinY = ((pV0->fY < -pV0->fW) ? GL_P_CLIP_CURR_OUT : 0) + ((pV1->fY < -pV1->fW) ? GL_P_CLIP_NEXT_OUT : 0);
	GLsizei iClipCaseMaxX = ((pV0->fX >  pV0->fW) ? GL_P_CLIP_CURR_OUT : 0) + ((pV1->fX >  pV1->fW) ? GL_P_CLIP_NEXT_OUT : 0);
	GLsizei iClipCaseMaxY = ((pV0->fY >  pV0->fW) ? GL_P_CLIP_CURR_OUT : 0) + ((pV1->fY >  pV1->fW) ? GL_P_CLIP_NEXT_OUT : 0);

	if( (pV0->fZ < -pV0->fW) || (pV1->fZ < -pV1->fW) ||
		(pV0->fZ >  pV0->fW) || (pV1->fZ >  pV1->fW) )
	{
		return;
	}

	GL_P_CLIP_RECTANGLE_3D( X, S, &v0, &v1, -1, iClipCaseMinX );
	GL_P_CLIP_RECTANGLE_3D( X, S, &v0, &v1,  1, iClipCaseMaxX );
	GL_P_CLIP_RECTANGLE_3D( Y, T, &v0, &v1, -1, iClipCaseMinY );
	GL_P_CLIP_RECTANGLE_3D( Y, T, &v0, &v1,  1, iClipCaseMaxY );

	GL_P_TransformVertex( pContext, &v0, &v0 );
	GL_P_TransformVertex( pContext, &v1, &v1 );

	BP3D_DrawRectangle( pContext->hP3d, &v0, &v1 );
}

/***************************************************************************/
void GL_P_ClipAndRenderRectangles(
	p3dOpenGLContextData *pContext,
	GLsizei iVertexCount )
{
	GLsizei ii;

	if( pContext->bRectangleBlit2D )
	{
	     D_UNIMPLEMENTED();
/*
		for( ii = 0; ii < iVertexCount / 2; ++ii )
		{
			BP3D_Vertex *pV0 = pContext->pVertices + ii * 2;
			BP3D_Vertex *pV1 = pContext->pVertices + ii * 2 + 1;

			if( pContext->ulVertexOutCode )
			{
				GL_P_TransformVertex( pContext, pV0, pV0 );
				GL_P_TransformVertex( pContext, pV1, pV1 );
			}

			GL_P_BlitRectangle( pContext, pV0, pV1, ii );
		}
*/
		return;
	}

	if( pContext->ulVertexOutCode )
	{
		for( ii = 0; ii < iVertexCount / 2; ++ii )
		{
			BP3D_Vertex *pV0 = pContext->pVertices + ii * 2;
			BP3D_Vertex *pV1 = pContext->pVertices + ii * 2 + 1;
			GLuint ulOutCode0 = GL_P_GET_OUTCODE(pV0);
			GLuint ulOutCode1 = GL_P_GET_OUTCODE(pV1);

			if( (ulOutCode0 & ulOutCode1) == 0 )
			{
				if( ulOutCode0 | ulOutCode1 )
					GL_P_ClipAndRenderRectangle( pContext, pV0, pV1 );
				else
					GL_P_RenderRectangle( pContext, pV0, pV1 );
			}
		}
	}
	else
	{
		for( ii = 0; ii < iVertexCount / 2; ++ii )
		{
			BP3D_Vertex *pV0 = pContext->pVertices + ii * 2;
			BP3D_Vertex *pV1 = pContext->pVertices + ii * 2 + 1;
			BP3D_DrawRectangle( pContext->hP3d, pV0, pV1 );
		}
	}
}

/* End of File */
