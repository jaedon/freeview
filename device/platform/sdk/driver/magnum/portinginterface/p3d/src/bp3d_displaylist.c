/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bp3d_displaylist.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 8/12/10 6:58p $
 *
 * Module Description: Private 3D Rendering Display List Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/p3d/7405/bp3d_displaylist.c $
 * 
 * Hydra_Software_Devel/8   8/12/10 6:58p nissen
 * SW7405-4720: Fixed macro to get tile.
 * 
 * Hydra_Software_Devel/7   8/4/09 4:02p nissen
 * PR 35853: Added initial support for binning instead of tiling.
 * 
 * Hydra_Software_Devel/6   6/16/08 8:22p nissen
 * PR 42507: Added support for rendering with multiple contexts.
 * 
 * Hydra_Software_Devel/5   10/5/05 9:40p nissen
 * PR 17273: Added support for cached memory.
 * 
 * Hydra_Software_Devel/4   7/27/04 10:52p nissen
 * PR 9132: Fixed problem with allocating display lists.
 * 
 * Hydra_Software_Devel/3   3/5/04 2:46p nissen
 * PR 9132: Added module handle parameter to functions that write display
 * list.
 * 
 * Hydra_Software_Devel/2   2/20/04 4:07p nissen
 * PR 9132: More implementation.
 * 
 * Hydra_Software_Devel/1   1/22/04 2:43p nissen
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

/***************************************************************************/
#define BP3D_P_GET_LIST_OFFSET( pMem ) \
	(((uint32_t) (pMem) - (uint32_t) \
	hP3d->hContext->apvPrimBlocks[hP3d->hContext->ulPrimBlockCount - 1]) + \
	hP3d->hContext->aulPrimOffsets[hP3d->hContext->ulPrimBlockCount - 1])

/***************************************************************************/
uint32_t *BP3D_P_AllocDisplayList(
	BP3D_Handle hP3d,
	uint32_t ulPrimCount,
	uint32_t *pulOffset )
{
	uint32_t ulAlignMask = sizeof (BP3D_P_Primitive) - 1;
	uint32_t ulAllocSize = ((ulPrimCount + BP3D_P_REG_COUNT + 1) * sizeof (uint32_t) + ulAlignMask) & (~ulAlignMask); 
	uint32_t ulBlockFree = hP3d->hContext->ulPrimBlockFree ? ((hP3d->hContext->ulPrimBlockFree - 1) & (BP3D_P_BLOCK_SIZE - 1)) + 1 : 0;
	uint32_t ulBlockUsed;
	void * pvMemory;

	/* check if allocation is too big */
	if( ulAllocSize > BP3D_P_BLOCK_SIZE )
		return NULL;

	/* check if there is enough free space in current block */
	if( ulAllocSize > ulBlockFree )
	{
		/* go to next primitive block */
		if( hP3d->hContext->ulPrimBlockFree )
		{
			hP3d->hContext->ulPrimBlockFree -= ulBlockFree;
			hP3d->hContext->ulPrimBlockPos++;
		}
		else
		{
			hP3d->hContext->ulPrimBlockPos = hP3d->hContext->ulPrimBlockCount;
		}

		/* allocate another block if necessary */
		if( ulAllocSize > hP3d->hContext->ulPrimBlockFree )
		{
			if( !BP3D_P_AllocPrimBlock( hP3d ) )
				return NULL;
		}
	}

	/* get address within block */
	ulBlockFree = hP3d->hContext->ulPrimBlockFree ? ((hP3d->hContext->ulPrimBlockFree - 1) & (BP3D_P_BLOCK_SIZE - 1)) + 1 : 0;
	ulBlockUsed = BP3D_P_BLOCK_SIZE - ulBlockFree;
	pvMemory = (void *) ((uint8_t *) hP3d->hContext->apvPrimBlocks[hP3d->hContext->ulPrimBlockPos] + ulBlockUsed);

	/* convert address to offset */
    *pulOffset = BP3D_P_GET_LIST_OFFSET( pvMemory );

	hP3d->hContext->ulPrimBlockFree -= ulAllocSize;
	if( hP3d->hContext->ulPrimBlockFree && ((hP3d->hContext->ulPrimBlockFree & (BP3D_P_BLOCK_SIZE - 1)) == 0) )
		hP3d->hContext->ulPrimBlockPos++;

	return (uint32_t *) pvMemory;
}

/***************************************************************************/
uint32_t *BP3D_P_WriteDisplayListScan(
	BP3D_Handle hP3d,
	BP3D_P_Layer *pLayer, 
	uint32_t *pulList, 
	uint8_t ucIndex )
{
	BSTD_UNUSED( hP3d );

	/* loop through nodes on scan */
	while( ucIndex )
	{
		/* write display list entry and get next node index */
		*pulList++ = (pLayer->aulNodes[ucIndex] >> 8) << (8 - BP3D_P_PRIMITIVE_ADDR_SHIFT);
		ucIndex = (uint8_t) pLayer->aulNodes[ucIndex];
	}

	return pulList;
}

/***************************************************************************/
uint32_t BP3D_P_WriteUnderflowDisplayList(
	BP3D_Handle hP3d,
	uint32_t *pulList,
	uint16_t usTileX,
	uint16_t usTileY,
	uint16_t usTileCountX,
	uint16_t usTileCountY )
{
	uint32_t *pulListBase = pulList;
	uint16_t ulStartScan = 0;
	uint16_t ii, jj, ss;

	/* loop through tile rows */
	for( jj = usTileY; jj < usTileY + usTileCountY; ++jj )
	{
		/* loop through scans */
 		for( ss = ulStartScan; ss <= BP3D_P_TILE_HEIGHT; ++ss )
		{
			BP3D_P_Tile *pTile = BP3D_P_GET_TILE(hP3d->hContext, usTileX, jj);
			BP3D_P_Layer *pLayer;

			/* loop through all but last tile */
			for( ii = usTileX; ii < usTileX + usTileCountX - 1; ++ii )
			{
				/* loop through all layers within tile */
				pLayer = pTile->pFirstLayer;
				while( pLayer )
				{
					/* write display list for tile left */
					pulList = BP3D_P_WriteDisplayListScan( hP3d, pLayer, pulList, pLayer->aucNodeIndicesLeft[ss] );
					pLayer = pLayer->pNextLayer;
				}

				pTile++;
			}

			/* loop through all layers within last tile */
			pLayer = pTile->pFirstLayer;
			while( pLayer )
			{
				/* write display list for tile left and right */
				pulList = BP3D_P_WriteDisplayListScan( hP3d, pLayer, pulList, pLayer->aucNodeIndicesLeft[ss] );
				pulList = BP3D_P_WriteDisplayListScan( hP3d, pLayer, pulList, pLayer->aucNodeIndicesRight[ss] );
				pLayer = pLayer->pNextLayer;
			}
		}

		ulStartScan = 1;
	}

	*pulList = 0;
	return (uint32_t) (pulList - pulListBase);
}

/***************************************************************************/
uint32_t BP3D_P_WriteOverflowDisplayList(
	BP3D_Handle hP3d,
	BP3D_P_Layer *pLayer0,
	BP3D_P_Layer *pLayer1,
	uint32_t *pulList )
{
	uint32_t *pulListBase = pulList;
	uint32_t ss;

	/* loop through scans */
	for( ss = 0; ss <= BP3D_P_TILE_HEIGHT; ++ss )
	{
		/* loop through specified layers within tile */
		BP3D_P_Layer *pLayer = pLayer0;
		while( pLayer != pLayer1 )
		{
			/* write display list for tile left and right */
			pulList = BP3D_P_WriteDisplayListScan( hP3d, pLayer, pulList, pLayer->aucNodeIndicesLeft[ss] );
			pulList = BP3D_P_WriteDisplayListScan( hP3d, pLayer, pulList, pLayer->aucNodeIndicesRight[ss] );
			pLayer = pLayer->pNextLayer;
		}
	}

	*pulList = 0;
	return (uint32_t) (pulList - pulListBase);
}

/***************************************************************************/
uint32_t BP3D_P_WriteBinDisplayList(
	BP3D_Handle hP3d,
	BP3D_P_Tile *pTile,
	uint32_t *pulList )
{
	BP3D_P_Layer *pLayer = pTile->pFirstLayer;
	uint32_t *pulListBase = pulList;
	uint16_t ss;

	/* loop through scans */
	for( ss = 0; ss <= hP3d->hContext->Viewport.usMaxX - hP3d->hContext->Viewport.usMinX; ++ss )
	{
		pulList = BP3D_P_WriteDisplayListScan( hP3d, pLayer, pulList, pLayer->aucNodeIndicesRight[ss] );
	}

	*pulList = 0;
	return (uint32_t) (pulList - pulListBase);
}

/* End of File */
