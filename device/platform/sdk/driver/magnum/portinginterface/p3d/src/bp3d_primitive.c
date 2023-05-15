/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bp3d_primitive.c $
 * $brcm_Revision: Hydra_Software_Devel/24 $
 * $brcm_Date: 3/8/11 5:46p $
 *
 * Module Description: Private 3D Rendering Primitive Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/p3d/7405/bp3d_primitive.c $
 * 
 * Hydra_Software_Devel/24   3/8/11 5:46p nissen
 * SW7405-3704: Fixed vertex clamping.
 * 
 * Hydra_Software_Devel/23   7/23/10 5:27p nissen
 * SW7125-508: Fixed warnings when copying vertex data.
 * 
 * Hydra_Software_Devel/22   3/10/10 2:03p nissen
 * SW7405-3704: Added check for degenerate triangles.
 * 
 * Hydra_Software_Devel/21   7/4/09 5:04p nissen
 * PR 56587: Fixed problem destroying special primitive. Adjusting
 * horizontal and vertical lines, and using max edge mipmapping for
 * OpenGL conformance.
 * 
 * Hydra_Software_Devel/20   10/27/08 12:23p nissen
 * PR 41194: Adjusted mipmap lod calculation.
 * 
 * Hydra_Software_Devel/19   10/22/08 10:09p nissen
 * PR 41194: Fixed culling.
 * 
 * Hydra_Software_Devel/18   10/9/08 1:56p nissen
 * PR 46638: Changed clear to opaque.
 * 
 * Hydra_Software_Devel/17   9/8/08 9:22p nissen
 * PR 46638: Added support for clearing with an alpha blend.
 * 
 * Hydra_Software_Devel/16   9/4/08 10:59a nissen
 * PR 39323: Fixed rectangle primitive mipmaping.
 * 
 * Hydra_Software_Devel/15   8/1/08 5:16p nissen
 * PR 39323: Fixed build warnings.
 * 
 * Hydra_Software_Devel/14   6/16/08 8:30p nissen
 * PR 42507: Added support for rendering with multiple contexts.
 * 
 * Hydra_Software_Devel/13   5/5/08 11:35a nissen
 * PR 40555: Fixed problem with culling.
 * 
 * Hydra_Software_Devel/12   12/7/06 12:03p nissen
 * PR 26403: Added support for clearing the alpha channel of the render
 * target to a specified value.
 * 
 * Hydra_Software_Devel/11   9/20/06 2:07p nissen
 * PR 22858: Added support for polygon offset through a depth scaling
 * factor used to offset a primitive's depth values.
 * 
 * Hydra_Software_Devel/10   9/14/06 4:01p nissen
 * PR 22858: Fixed problem with calculating mipmap level of detail.
 * 
 * Hydra_Software_Devel/9   12/14/05 3:45p nissen
 * PR 17423: Fixed build problem with math.h include file.
 * 
 * Hydra_Software_Devel/8   10/5/05 9:46p nissen
 * PR 17273: Added support for cached memory.
 * 
 * Hydra_Software_Devel/7   8/10/04 4:54p nissen
 * PR 9132: Added support for clear rectangle.
 * 
 * Hydra_Software_Devel/6   7/27/04 11:03p nissen
 * PR 9132: Added fog support.
 * 
 * Hydra_Software_Devel/5   4/30/04 1:23p nissen
 * PR 10548: Added use of BCHP_MASK and BCHP_SHIFT macros.
 * 
 * Hydra_Software_Devel/4   4/30/04 11:19a nissen
 * PR 9132: Added error code return value to function that creates empty
 * point.
 * 
 * Hydra_Software_Devel/3   3/5/04 2:49p nissen
 * PR 9132: Fixed problem with getting primitive offset. Added support for
 * mipmapped rectangles.
 * 
 * Hydra_Software_Devel/2   2/20/04 4:14p nissen
 * PR 9132: More implementation.
 * 
 * Hydra_Software_Devel/1   1/22/04 2:53p nissen
 * PR 9132: Initial version.
 * 
 ***************************************************************************/

#include "bstd_defs.h"
#include "berr.h"
#include "bkni.h"
#include "bchp.h"
#include "bchp_px3d.h"
#include "bchp_common.h"
#include "bp3d.h"
#include "bp3d_private.h"
#include "math.h"

/***************************************************************************/
#define BP3D_P_CLAMP_TO_VIEWPORT( pViewport, pRect, sMinX, sMinY, sMaxX, sMaxY ) \
{ \
	(pRect)->usMinX = (uint16_t) BP3D_P_MIN(BP3D_P_MAX(sMinX, (int16_t) (pViewport)->usMinX), (int16_t) (pViewport)->usMaxX); \
	(pRect)->usMinY = (uint16_t) BP3D_P_MIN(BP3D_P_MAX(sMinY, (int16_t) (pViewport)->usMinY), (int16_t) (pViewport)->usMaxY); \
	(pRect)->usMaxX = (uint16_t) BP3D_P_MIN(BP3D_P_MAX(sMaxX, (int16_t) (pViewport)->usMinX), (int16_t) (pViewport)->usMaxX); \
	(pRect)->usMaxY = (uint16_t) BP3D_P_MIN(BP3D_P_MAX(sMaxY, (int16_t) (pViewport)->usMinY), (int16_t) (pViewport)->usMaxY); \
	(pRect)->usMinX = (pRect)->usMinX - (pViewport)->usMinX; \
	(pRect)->usMinY = (pRect)->usMinY - (pViewport)->usMinY; \
	(pRect)->usMaxX = (pRect)->usMaxX - (pViewport)->usMinX; \
	(pRect)->usMaxY = (pRect)->usMaxY - (pViewport)->usMinY; \
}

/***************************************************************************/
#define BP3D_P_BOUND_PRIMITIVE_1( pViewport, pRect, pV0 ) \
{ \
	int16_t sMinX = (int16_t) (pV0)->sX; \
	int16_t sMinY = (int16_t) (pV0)->sY; \
	int16_t sMaxX = (int16_t) (pV0)->sX + 1; \
	int16_t sMaxY = (int16_t) (pV0)->sY + 1; \
	BP3D_P_CLAMP_TO_VIEWPORT( pViewport, pRect, sMinX, sMinY, sMaxX, sMaxY ); \
}

/***************************************************************************/
#define BP3D_P_BOUND_PRIMITIVE_2( pViewport, pRect, pV0, pV1 ) \
{ \
	int16_t sMinX = (int16_t) BP3D_P_MIN((pV0)->sX, (pV1)->sX); \
	int16_t sMinY = (int16_t) BP3D_P_MIN((pV0)->sY, (pV1)->sY); \
	int16_t sMaxX = (int16_t) BP3D_P_MAX((pV0)->sX, (pV1)->sX) + 1; \
	int16_t sMaxY = (int16_t) BP3D_P_MAX((pV0)->sY, (pV1)->sY) + 1; \
	BP3D_P_CLAMP_TO_VIEWPORT( pViewport, pRect, sMinX, sMinY, sMaxX, sMaxY ); \
}

/***************************************************************************/
#define BP3D_P_BOUND_PRIMITIVE_3( pViewport, pRect, pV0, pV1, pV2 ) \
{ \
	int16_t sMinX = (int16_t) BP3D_P_MIN(BP3D_P_MIN((pV0)->sX, (pV1)->sX), (pV2)->sX); \
	int16_t sMinY = (int16_t) BP3D_P_MIN(BP3D_P_MIN((pV0)->sY, (pV1)->sY), (pV2)->sY); \
	int16_t sMaxX = (int16_t) BP3D_P_MAX(BP3D_P_MAX((pV0)->sX, (pV1)->sX), (pV2)->sX) + 1; \
	int16_t sMaxY = (int16_t) BP3D_P_MAX(BP3D_P_MAX((pV0)->sY, (pV1)->sY), (pV2)->sY) + 1; \
	BP3D_P_CLAMP_TO_VIEWPORT( pViewport, pRect, sMinX, sMinY, sMaxX, sMaxY ); \
}

/***************************************************************************/
#define BP3D_P_COPY_PRIMHEADER( pPrim, pHeader ) \
{ \
	(pPrim)->Header.ulPrimHeaderA = (pHeader)->ulPrimHeaderA; \
	(pPrim)->Header.ulPrimHeaderB = (pHeader)->ulPrimHeaderB; \
	(pPrim)->Header.ulTextureAddr = (pHeader)->ulTextureAddr; \
}

/***************************************************************************/
#define BP3D_P_COPY_VERTEX_ELEMENT( eo, ei ) \
	(eo) = (ei)

/***************************************************************************/
#define BP3D_P_COPY_VERTEX( pVo, pVi, pV0, eShade, fOffsetZ ) \
{ \
	BP3D_Vertex *pV = ((eShade) == BP3D_Shade_Gouraud) ? (pVi) : (pV0); \
	BP3D_P_COPY_VERTEX_ELEMENT((pVo)->fX, (pVi)->fX); \
	BP3D_P_COPY_VERTEX_ELEMENT((pVo)->fY, (pVi)->fY); \
	(pVo)->fZ = (pVi)->fZ + fOffsetZ; \
	BP3D_P_COPY_VERTEX_ELEMENT((pVo)->fW, (pVi)->fW); \
	BP3D_P_COPY_VERTEX_ELEMENT((pVo)->ulDiffuse, pV->ulDiffuse); \
	BP3D_P_COPY_VERTEX_ELEMENT((pVo)->ulSpecular, pV->ulSpecular); \
	BP3D_P_COPY_VERTEX_ELEMENT((pVo)->fS, (pVi)->fS); \
	BP3D_P_COPY_VERTEX_ELEMENT((pVo)->fT, (pVi)->fT); \
}

/***************************************************************************/
#define BP3D_P_GET_PRIM_OFFSET( pPrim ) \
	(((uint32_t) (pPrim) - (uint32_t) \
	hP3d->hContext->apvPrimBlocks[hP3d->hContext->ulPrimBlockCount - 1]) + \
	hP3d->hContext->aulPrimOffsets[hP3d->hContext->ulPrimBlockCount - 1])

/***************************************************************************/
bool BP3D_P_AllocPrimBlock(
	BP3D_Handle hP3d )
{
	uint32_t ulOffset;

	/* allocate local memory for primitive block */
	void *pvMemory = BP3D_P_AllocMemoryBlock( hP3d, &ulOffset );
	if( pvMemory == NULL )
		return false;

	hP3d->hContext->apvPrimBlocks[hP3d->hContext->ulPrimBlockCount] = pvMemory;
	hP3d->hContext->aulPrimOffsets[hP3d->hContext->ulPrimBlockCount] = ulOffset;
	hP3d->hContext->ulPrimBlockFree += BP3D_P_BLOCK_SIZE;
	hP3d->hContext->ulPrimBlockCount++;

	return true;
}

/***************************************************************************/
void BP3D_P_ResetPrimBlocks(
	BP3D_Handle hP3d )
{
	/* clear array of primitive block pointers */
	BKNI_Memset( (void *) hP3d->hContext->apvPrimBlocks, 0, BP3D_P_PRIM_BLOCK_COUNT * sizeof (uint32_t) );

	hP3d->hContext->ulPrimBlockPos = 0;
	hP3d->hContext->ulPrimBlockCount = 0;
	hP3d->hContext->ulPrimBlockFree = 0;
	hP3d->hContext->ulCurrPrimPos = 0;
}

/***************************************************************************/
BP3D_P_Primitive *BP3D_P_AllocPrimitives(
	BP3D_Handle hP3d,
	uint32_t ulPrimCount )
{
	/* check if primitive per scene max reached */
	if( (ulPrimCount + hP3d->hContext->ulPrimCount) > BP3D_P_PRIMITIVE_PER_SCENE )
		return NULL;

	/* add memory blocks for primitives as needed */
	while( hP3d->hContext->ulPrimBlockFree < (ulPrimCount * sizeof (BP3D_P_Primitive)) )
	{
		if( !BP3D_P_AllocPrimBlock( hP3d ) )
			return NULL;
	}

	return (BP3D_P_Primitive *) hP3d->hContext->apvPrimBlocks[hP3d->hContext->ulPrimBlockPos] + hP3d->hContext->ulCurrPrimPos;
}

/***************************************************************************/
BP3D_P_Primitive *BP3D_P_GetNextPrimitive(
	BP3D_Handle hP3d )
{
	/* decrement count of free memory and increment primitive position */
	hP3d->hContext->ulPrimBlockFree -= sizeof (BP3D_P_Primitive);
	hP3d->hContext->ulCurrPrimPos++;

	/* check if current primitive block is full */
	if( hP3d->hContext->ulCurrPrimPos == BP3D_P_PRIM_PER_BLOCK )
	{
		/* advance to next primitive block */
		hP3d->hContext->ulPrimBlockPos++;
		hP3d->hContext->ulCurrPrimPos = 0;
	}

	return (BP3D_P_Primitive *) hP3d->hContext->apvPrimBlocks[hP3d->hContext->ulPrimBlockPos] + hP3d->hContext->ulCurrPrimPos;
}

/***************************************************************************/
BP3D_P_Primitive *BP3D_P_CopyPoint(
	BP3D_Handle hP3d,
	BP3D_P_Primitive *pPrim, 
	BP3D_Vertex *pV0 )
{
	BP3D_P_Rect rect;
	uint32_t ulPrimOffset = BP3D_P_GET_PRIM_OFFSET( pPrim );

	/* bound primitive */
	BP3D_P_BOUND_PRIMITIVE_1( &hP3d->hContext->Viewport, &rect, pV0 );

	/* update primitive header */
	BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, PrimitiveType, Point );
	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderB, AppPriority, hP3d->hContext->ulPrimCount + 1 );

	/* copy primitive header and vertices */
	BP3D_P_COPY_PRIMHEADER( pPrim, &hP3d->hContext->PrimHeader );
	BP3D_P_COPY_VERTEX( &pPrim->v0, pV0, pV0, hP3d->hContext->ePrimitiveShade, 0.0 );

	/* add primitive to tiles */
	BP3D_P_AddPrimitiveToTiles( hP3d, &rect, ulPrimOffset );
	hP3d->hContext->ulPrimCount++;

	return BP3D_P_GetNextPrimitive( hP3d );
}

/***************************************************************************/
BP3D_P_Primitive *BP3D_P_CopyLine(
	BP3D_Handle hP3d,
	BP3D_P_Primitive *pPrim, 
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1 )
{
	BP3D_P_Rect rect;
	uint32_t ulPrimOffset = BP3D_P_GET_PRIM_OFFSET( pPrim );
	float fZOffset = 0.0f;

	/* bound primitive */
	BP3D_P_BOUND_PRIMITIVE_2( &hP3d->hContext->Viewport, &rect, pV0, pV1 );
	if( (rect.usMinX >= rect.usMaxX) || (rect.usMinY >= rect.usMaxY) )
		return pPrim;

	/* offset depth values */
	if( hP3d->hContext->fDepthFactorScale != 0.0 )
		fZOffset = (BP3D_P_MAX(pV0->fZ, pV1->fZ) - BP3D_P_MIN(pV0->fZ, pV1->fZ)) * hP3d->hContext->fDepthFactorScale;

	/* update primitive header */
	BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, PrimitiveType, Line );
	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderB, AppPriority, hP3d->hContext->ulPrimCount + 1 );

#ifdef EGL_P_CONFORMANT
	if( pV0->fX == pV1->fX )
	{
		if( pV0->fY > pV1->fY )
			pV1->fY += 1.0;
		else
			pV1->fY -= 1.0;
	}
	else if( pV0->fY == pV1->fY )
	{
		if( pV0->fX > pV1->fX )
		{
			pV0->fX += 1.0;
			pV1->fX += 1.0;
		}
	}
#endif

	/* copy primitive header and vertices */
	BP3D_P_COPY_PRIMHEADER( pPrim, &hP3d->hContext->PrimHeader );
	BP3D_P_COPY_VERTEX( &pPrim->v0, pV0, pV1, hP3d->hContext->ePrimitiveShade, fZOffset );
	BP3D_P_COPY_VERTEX( &pPrim->v1, pV1, pV1, hP3d->hContext->ePrimitiveShade, fZOffset );

	/* add primitive to tiles */
	BP3D_P_AddPrimitiveToTiles( hP3d, &rect, ulPrimOffset );
	hP3d->hContext->ulPrimCount++;

	return BP3D_P_GetNextPrimitive( hP3d );
}

/***************************************************************************/
BP3D_P_Primitive *BP3D_P_CopyRectangle(
	BP3D_Handle hP3d,
	BP3D_P_Primitive *pPrim, 
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1 )
{
	BP3D_P_Rect rect;
	uint32_t ulPrimOffset = BP3D_P_GET_PRIM_OFFSET( pPrim );
	float fZOffset = 0.0f;
	
	/* bound primitive */
	BP3D_P_BOUND_PRIMITIVE_2( &hP3d->hContext->Viewport, &rect, pV0, pV1 );
	if( (rect.usMinX >= rect.usMaxX) || (rect.usMinY >= rect.usMaxY) )
		return pPrim;

	/* offset depth values */
	if( hP3d->hContext->fDepthFactorScale != 0.0 )
		fZOffset = (BP3D_P_MAX(pV0->fZ, pV1->fZ) - BP3D_P_MIN(pV0->fZ, pV1->fZ)) * hP3d->hContext->fDepthFactorScale;

	/* update primitive header */
	BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, PrimitiveType, Rectangle );
	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderB, AppPriority, hP3d->hContext->ulPrimCount + 1 );

	/* copy primitive header and vertices */
	BP3D_P_COPY_PRIMHEADER( pPrim, &hP3d->hContext->PrimHeader );
	BP3D_P_COPY_VERTEX( &pPrim->v0, pV0, pV1, hP3d->hContext->ePrimitiveShade, fZOffset );
	BP3D_P_COPY_VERTEX( &pPrim->v1, pV1, pV1, hP3d->hContext->ePrimitiveShade, fZOffset );

	/* check if mipmapping */
	if( hP3d->hContext->bMipmap && hP3d->hContext->hTextureSurface ) 
	{
		float fPixelMinX = BP3D_P_MIN(pV0->fX, pV1->fX);
		float fPixelMinY = BP3D_P_MIN(pV0->fY, pV1->fY);
		float fPixelMaxX = BP3D_P_MAX(pV0->fX, pV1->fX);
		float fPixelMaxY = BP3D_P_MAX(pV0->fY, pV1->fY);
		float fPixelArea = (fPixelMaxX - fPixelMinX) * (fPixelMaxY - fPixelMinY);

		/* check if using non-zero mipmap level-of-detail */
		if( (hP3d->hContext->fMipmapLodMax > 1.0f) && (fPixelArea != 0.0f) )
		{
			/* calculate mipmap level-of-detail for all three vertices */
			float fTexelMinX = BP3D_P_MIN(pV0->fS, pV1->fS);
			float fTexelMinY = BP3D_P_MIN(pV0->fT, pV1->fT);
			float fTexelMaxX = BP3D_P_MAX(pV0->fS, pV1->fS);
			float fTexelMaxY = BP3D_P_MAX(pV0->fT, pV1->fT);
			float fTexelArea = (fTexelMaxX - fTexelMinX) * (fTexelMaxY - fTexelMinY) *
				hP3d->hContext->ulTextureWidth * hP3d->hContext->ulTextureHeight;
			float fAreaRatio = (float) fabs(fTexelArea / fPixelArea);
			float fLod = (float) (log(fAreaRatio) * BP3D_P_LOG4_CONSTANT) + hP3d->hContext->fMipmapLodBias;

			pPrim->v0.fM = BP3D_P_MAX(BP3D_P_MIN(fLod, hP3d->hContext->fMipmapLodMax + 0.5f), 0.0);
			pPrim->v1.fM = BP3D_P_MAX(BP3D_P_MIN(fLod, hP3d->hContext->fMipmapLodMax + 0.5f), 0.0);
		}
		else
		{
			/* set mipmap level-of-detail to zero */
			pPrim->v0.fM = 0.0f;
			pPrim->v1.fM = 0.0f;
		}
	}

	/* add primitive to tiles */
	BP3D_P_AddPrimitiveToTiles( hP3d, &rect, ulPrimOffset );
	hP3d->hContext->ulPrimCount++;

	return BP3D_P_GetNextPrimitive( hP3d );
}

/***************************************************************************/
BP3D_P_Primitive *BP3D_P_CopyTriangle(
	BP3D_Handle hP3d,
	BP3D_P_Primitive *pPrim, 
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1,
	BP3D_Vertex *pV2 )
{
	BP3D_P_Rect rect;
	uint32_t ulPrimOffset = BP3D_P_GET_PRIM_OFFSET( pPrim );
	float fZOffset = 0.0f;

	/* check for degenerate triangle */
	float fPixelArea = (pV1->fX - pV0->fX) * (pV2->fY - pV0->fY) - (pV2->fX - pV0->fX) * (pV1->fY - pV0->fY);
	if( fabs(fPixelArea) < 1.0f )
		return pPrim;

	/* bound primitive */
	BP3D_P_BOUND_PRIMITIVE_3( &hP3d->hContext->Viewport, &rect, pV0, pV1, pV2 );
	if( (rect.usMinX >= rect.usMaxX) || (rect.usMinY >= rect.usMaxY) )
		return pPrim;

	/* offset depth values */
	if( hP3d->hContext->fDepthFactorScale != 0.0 )
		fZOffset = (BP3D_P_MAX(BP3D_P_MAX(pV0->fZ, pV1->fZ), pV2->fZ) - 
			BP3D_P_MIN(BP3D_P_MIN(pV0->fZ, pV1->fZ), pV2->fZ)) * hP3d->hContext->fDepthFactorScale;

	/* check for mipmaping or culling */
	if( hP3d->hContext->bMipmap || (hP3d->hContext->ePrimitiveCull != BP3D_Cull_None) )
	{
		/* cull primitive */
		if( ((fPixelArea > 0.0f) && (hP3d->hContext->ePrimitiveCull == BP3D_Cull_Clockwise)) || 
			((fPixelArea < 0.0f) && (hP3d->hContext->ePrimitiveCull == BP3D_Cull_CounterClockwise)) )
			return pPrim;
	}

	/* update primitive header */
	BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, PrimitiveType, Triangle );
	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderB, AppPriority, hP3d->hContext->ulPrimCount + 1 );

	/* copy primitive header and vertices */
	BP3D_P_COPY_PRIMHEADER( pPrim, &hP3d->hContext->PrimHeader );
	BP3D_P_COPY_VERTEX( &pPrim->v0, pV0, pV2, hP3d->hContext->ePrimitiveShade, fZOffset );
	BP3D_P_COPY_VERTEX( &pPrim->v1, pV1, pV2, hP3d->hContext->ePrimitiveShade, fZOffset );
	BP3D_P_COPY_VERTEX( &pPrim->v2, pV2, pV2, hP3d->hContext->ePrimitiveShade, fZOffset );

	/* check if mipmapping */
	if( hP3d->hContext->bMipmap && hP3d->hContext->hTextureSurface ) 
	{
		/* check if using non-zero mipmap level-of-detail */
		if( (hP3d->hContext->fMipmapLodMax > 1.0f) && (fPixelArea != 0.0f) )
		{
			/* calculate mipmap level-of-detail for all three vertices */
#ifdef EGL_P_CONFORMANT
			float fDeltaS0 = fabs(pV1->fS - pV0->fS);
			float fDeltaS1 = fabs(pV2->fS - pV1->fS);
			float fDeltaS2 = fabs(pV0->fS - pV2->fS);
			float fDeltaS = BP3D_P_MAX(BP3D_P_MAX(fDeltaS0, fDeltaS1), fDeltaS2) * hP3d->hContext->ulTextureWidth;
			float fDeltaT0 = fabs(pV1->fT - pV0->fT);
			float fDeltaT1 = fabs(pV2->fT - pV1->fT);
			float fDeltaT2 = fabs(pV0->fT - pV2->fT);
			float fDeltaT = BP3D_P_MAX(BP3D_P_MAX(fDeltaT0, fDeltaT1), fDeltaT2)* hP3d->hContext->ulTextureHeight;
			float fDelta = BP3D_P_MAX(fDeltaS, fDeltaT);
			float fTexelArea = fDelta * fDelta;
#else
			float fTexelArea = ((pV1->fS - pV0->fS) * (pV2->fT - pV0->fT) - (pV2->fS - pV0->fS) * (pV1->fT - pV0->fT)) *
				hP3d->hContext->ulTextureWidth * hP3d->hContext->ulTextureHeight;
#endif
			float fAreaRatio = (float) fabs(fTexelArea / fPixelArea);

			float fCoverage0 = fAreaRatio * pV0->fW * pV0->fW * pV1->fInvW * pV2->fInvW;
			float fCoverage1 = fAreaRatio * pV1->fW * pV1->fW * pV2->fInvW * pV0->fInvW;
			float fCoverage2 = fAreaRatio * pV2->fW * pV2->fW * pV0->fInvW * pV1->fInvW;

			float fLod0 = (float) ((log(fCoverage0) * BP3D_P_LOG4_CONSTANT) + hP3d->hContext->fMipmapLodBias);
			float fLod1 = (float) ((log(fCoverage1) * BP3D_P_LOG4_CONSTANT) + hP3d->hContext->fMipmapLodBias);
			float fLod2 = (float) ((log(fCoverage2) * BP3D_P_LOG4_CONSTANT) + hP3d->hContext->fMipmapLodBias);

			pPrim->v0.fM = BP3D_P_MAX(BP3D_P_MIN(fLod0, hP3d->hContext->fMipmapLodMax + 0.5f), 0.0);
			pPrim->v1.fM = BP3D_P_MAX(BP3D_P_MIN(fLod1, hP3d->hContext->fMipmapLodMax + 0.5f), 0.0);
			pPrim->v2.fM = BP3D_P_MAX(BP3D_P_MIN(fLod2, hP3d->hContext->fMipmapLodMax + 0.5f), 0.0);
		}
		else
		{
			/* set mipmap level-of-detail to zero */
			pPrim->v0.fM = 0.0f;
			pPrim->v1.fM = 0.0f;
			pPrim->v2.fM = 0.0f;
		}
	}

	/* add primitive to tiles */
	BP3D_P_AddPrimitiveToTiles( hP3d, &rect, ulPrimOffset );
	hP3d->hContext->ulPrimCount++;

	return BP3D_P_GetNextPrimitive( hP3d );
}

/***************************************************************************/
BERR_Code BP3D_P_CreateSpecialPrimitives(
	BP3D_Handle hP3d )
{
	BERR_Code err;

	/* allocate device memory for primitives */
	hP3d->hContext->pEmptyPoint = (BP3D_P_Primitive *) BMEM_AllocAligned( hP3d->hMemory, 
		sizeof (BP3D_P_Primitive) * 3, BP3D_P_BLOCK_ALIGN, 0 );
	if( hP3d->hContext->pEmptyPoint == NULL )
		return BERR_OUT_OF_DEVICE_MEMORY;

	BKNI_Memset( (void *) hP3d->hContext->pEmptyPoint, 0, sizeof (BP3D_P_Primitive) * 3 );

	/* convert primitive address to offset */
	err = BMEM_ConvertAddressToOffset( hP3d->hMemory, (void *) hP3d->hContext->pEmptyPoint, &hP3d->hContext->ulEmptyPointOffset ) ;
	if( err != BERR_SUCCESS )
	{
		BMEM_Free( hP3d->hMemory, (void *) hP3d->hContext->pEmptyPoint );
		return BERR_TRACE(err);
	}

	/* set empty point primitive */
	hP3d->hContext->pEmptyPoint->Header.ulPrimHeaderA = 
		BCHP_FIELD_ENUM(PX3D_Type_PrimHeaderA, PrimitiveType, Point) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, SrcAlphaBlend, BP3D_Blend_Zero) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, DstAlphaBlend, BP3D_Blend_One);
	hP3d->hContext->pEmptyPoint->Header.ulPrimHeaderB = 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderB, AppPriority, 1) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderB, DepthMask, 1) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderB, ZCompare, BP3D_Compare_Always);

	/* set clear primitive */
	hP3d->hContext->pColorRectangle = hP3d->hContext->pEmptyPoint + 1;
	hP3d->hContext->ulColorRectangleOffset = hP3d->hContext->ulEmptyPointOffset + sizeof (BP3D_P_Primitive);

	hP3d->hContext->pColorRectangle->Header.ulPrimHeaderA = 
		BCHP_FIELD_ENUM(PX3D_Type_PrimHeaderA, PrimitiveType, Rectangle) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, AlphaEnable, 1 ) |
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, SrcAlphaBlend, BP3D_Blend_One) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, DstAlphaBlend, BP3D_Blend_Zero) |
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, AlphaCompare, BP3D_Compare_Always) |
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, Opaque, 1);

	/* set clear primitive */
	hP3d->hContext->pDepthRectangle = hP3d->hContext->pColorRectangle + 1;
	hP3d->hContext->ulDepthRectangleOffset = hP3d->hContext->ulColorRectangleOffset + sizeof (BP3D_P_Primitive);

	hP3d->hContext->pDepthRectangle->Header.ulPrimHeaderA = 
		BCHP_FIELD_ENUM(PX3D_Type_PrimHeaderA, PrimitiveType, Rectangle) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, SrcAlphaBlend, BP3D_Blend_Zero) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, DstAlphaBlend, BP3D_Blend_One) |
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, AlphaCompare, BP3D_Compare_Always) |
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderA, Opaque, 1);

	return BERR_SUCCESS;
}

/***************************************************************************/
void BP3D_P_DestroySpecialPrimitives(
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	BMEM_Free( hP3d->hMemory, (void *) hContext->pEmptyPoint );
}

/***************************************************************************/
void BP3D_P_AddEmptyPointToTiles(
	BP3D_Handle hP3d,
	uint16_t usX,
	uint16_t usY )
{
	BP3D_P_Rect PrimRect;

	hP3d->hContext->pEmptyPoint->v0.fX = (float) (hP3d->hContext->Viewport.usMinX + usX);
	hP3d->hContext->pEmptyPoint->v0.fY = (float) (hP3d->hContext->Viewport.usMinY + usY);
	hP3d->hContext->pEmptyPoint->v0.fW = 1.0f;

	PrimRect.usMinX = usX;
	PrimRect.usMinY = usY;
	PrimRect.usMaxX = usX + 1;
	PrimRect.usMaxY = usY + 1;

	BP3D_P_AddPrimitiveToTiles( hP3d, &PrimRect, hP3d->hContext->ulEmptyPointOffset );
}

/***************************************************************************/
void BP3D_P_AddColorRectangleToTiles(
	BP3D_Handle hP3d,
	uint32_t ulColor )
{
	BP3D_P_Rect PrimRect;

	hP3d->hContext->pColorRectangle->Header.ulPrimHeaderB = 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderB, AppPriority, hP3d->hContext->ulPrimCount + 1) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderB, DepthMask, 1) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderB, ZCompare, BP3D_Compare_Always);

	hP3d->hContext->pColorRectangle->v0.fX = (float) hP3d->hContext->Viewport.usMinX;
	hP3d->hContext->pColorRectangle->v0.fY = (float) hP3d->hContext->Viewport.usMinY;
	hP3d->hContext->pColorRectangle->v0.fZ = 0.0f;
	hP3d->hContext->pColorRectangle->v0.fW = 1.0f;
	hP3d->hContext->pColorRectangle->v0.ulDiffuse = ulColor;
	hP3d->hContext->pColorRectangle->v1.fX = (float) hP3d->hContext->Viewport.usMaxX;
	hP3d->hContext->pColorRectangle->v1.fY = (float) hP3d->hContext->Viewport.usMaxY;
	hP3d->hContext->pColorRectangle->v1.fZ = 0.0f;
	hP3d->hContext->pColorRectangle->v1.fW = 1.0f;
	hP3d->hContext->pColorRectangle->v1.ulDiffuse = ulColor;

	PrimRect.usMinX = 0;
	PrimRect.usMinY = 0;
	PrimRect.usMaxX = hP3d->hContext->Viewport.usMaxX - hP3d->hContext->Viewport.usMinX;
	PrimRect.usMaxY = hP3d->hContext->Viewport.usMaxY - hP3d->hContext->Viewport.usMinY;

	BP3D_P_AddPrimitiveToTiles( hP3d, &PrimRect, hP3d->hContext->ulColorRectangleOffset );
	hP3d->hContext->ulPrimCount++;
}

/***************************************************************************/
void BP3D_P_AddDepthRectangleToTiles(
	BP3D_Handle hP3d,
	float fDepth )
{
	BP3D_P_Rect PrimRect;

	hP3d->hContext->pDepthRectangle->Header.ulPrimHeaderB = 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderB, AppPriority, hP3d->hContext->ulPrimCount + 1) | 
		BCHP_FIELD_DATA(PX3D_Type_PrimHeaderB, ZCompare, BP3D_Compare_Always);

	hP3d->hContext->pDepthRectangle->v0.fX = (float) hP3d->hContext->Viewport.usMinX;
	hP3d->hContext->pDepthRectangle->v0.fY = (float) hP3d->hContext->Viewport.usMinY;
	hP3d->hContext->pDepthRectangle->v0.fZ = fDepth;
	hP3d->hContext->pDepthRectangle->v0.fW = 1.0f;
	hP3d->hContext->pDepthRectangle->v1.fX = (float) hP3d->hContext->Viewport.usMaxX;
	hP3d->hContext->pDepthRectangle->v1.fY = (float) hP3d->hContext->Viewport.usMaxY;
	hP3d->hContext->pDepthRectangle->v1.fZ = fDepth;
	hP3d->hContext->pDepthRectangle->v1.fW = 1.0f;

	PrimRect.usMinX = 0;
	PrimRect.usMinY = 0;
	PrimRect.usMaxX = hP3d->hContext->Viewport.usMaxX - hP3d->hContext->Viewport.usMinX;
	PrimRect.usMaxY = hP3d->hContext->Viewport.usMaxY - hP3d->hContext->Viewport.usMinY;

	BP3D_P_AddPrimitiveToTiles( hP3d, &PrimRect, hP3d->hContext->ulDepthRectangleOffset );
	hP3d->hContext->ulPrimCount++;
}

/* End of File */
