/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bp3d_tile.c $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 8/26/10 3:58p $
 *
 * Module Description: Private 3D Rendering Tile Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/p3d/7125/bp3d_tile.c $
 * 
 * Hydra_Software_Devel/27   8/26/10 3:58p mward
 * SW7125-605: Declare unused parameters to silence warnings.
 * 
 * Hydra_Software_Devel/26   8/12/10 6:24p nissen
 * SW7405-4720: Fixed destroying tiles.
 * 
 * Hydra_Software_Devel/25   7/23/10 5:34p nissen
 * SW7125-508: Fixed warnings dumping display list.
 * 
 * Hydra_Software_Devel/24   3/3/10 6:26p nissen
 * SW7405-3704: Added use of memory block out when using a source color
 * buffer.
 * 
 * Hydra_Software_Devel/23   1/4/10 5:32p nissen
 * SW7405-3704: Added use of memory block out when rendering with a depth
 * buffer to avoid possible hang on 7405.
 * 
 * Hydra_Software_Devel/22   8/26/09 3:31p nissen
 * SW7400-1101: Fixed problem when bin has no primitives.
 * 
 * Hydra_Software_Devel/21   8/12/09 1:28p nissen
 * PR 35853: Fixed binning.
 * 
 * Hydra_Software_Devel/20   8/4/09 8:08p nissen
 * PR 57173: Fixed coverity defect involving context handle.
 * 
 * Hydra_Software_Devel/19   8/4/09 3:52p nissen
 * PR 35853: Added initial support for binning instead of tiling.
 * 
 * Hydra_Software_Devel/18   6/8/09 2:10p nissen
 * PR 54288: Added condition to check if pTile0 is NULL.
 * 
 * Hydra_Software_Devel/17   6/8/09 1:54p nissen
 * PR 54272: Removed unused function.
 * 
 * Hydra_Software_Devel/16   8/1/08 5:10p nissen
 * PR 39323: Fixed build warnings.
 * 
 * Hydra_Software_Devel/15   7/15/08 1:58p nissen
 * PR 41229: Added support for configuring tiles and for timing renders.
 * 
 * Hydra_Software_Devel/14   6/16/08 8:42p nissen
 * PR 42507: Added support for rendering with multiple contexts.
 * 
 * Hydra_Software_Devel/13   9/10/07 11:36a nissen
 * PR 30062: Modified code to not render tiles when empty.
 * 
 * Hydra_Software_Devel/12   9/22/06 11:03a nissen
 * PR 22858: Fixed problem when rendering without primitives or clears but
 * with a source color buffer.
 * 
 * Hydra_Software_Devel/11   9/14/06 3:35p nissen
 * PR 22858: Fixed warnings.
 * 
 * Hydra_Software_Devel/10   4/6/06 11:06a nissen
 * PR 18903: Fixed warnings when using BDBG_MSG.
 * 
 * Hydra_Software_Devel/9   1/12/06 3:38p nissen
 * PR 18903: Fixed vxworks warnings and changed BKNI_Printf calls to
 * BDBG_MSG.
 * 
 * Hydra_Software_Devel/8   8/10/04 4:15p nissen
 * PR 9132: Added more information when dumping reg sets and display
 * lists.
 * 
 * Hydra_Software_Devel/7   7/27/04 11:11p nissen
 * PR 9132: Fixed problem with setting buffer clear bits in mode register.
 * 
 * Hydra_Software_Devel/6   4/30/04 11:27a nissen
 * PR 9132: Added error code return to function that creates tiles.
 * 
 * Hydra_Software_Devel/5   3/19/04 3:12p nissen
 * PR 9132: Fixed problem with using viewport to make tile dimensions.
 * 
 * Hydra_Software_Devel/3   3/5/04 2:54p nissen
 * PR 9132: Modified horz and vert tile merges to make better tile
 * configurations. Added functions to dump and verify display lists and
 * registers sets.
 * 
 * Hydra_Software_Devel/2   2/20/04 4:22p nissen
 * PR 9132: More implementation.
 * 
 * Hydra_Software_Devel/1   1/22/04 3:03p nissen
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

BDBG_MODULE(BP3D);

/***************************************************************************/
BERR_Code BP3D_P_CreateTiles(
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	uint16_t usTileCountX = (hContext->Viewport.usMaxX - hContext->Viewport.usMinX + hContext->ulTileWidth - 1) / hContext->ulTileWidth;
	uint16_t usTileCountY = (hContext->Viewport.usMaxY - hContext->Viewport.usMinY + hContext->ulTileHeight - 1) / hContext->ulTileHeight;
	uint16_t usTileWidth  = hContext->ulTileWidth;
	uint16_t usTileHeight = hContext->ulTileHeight;

	BSTD_UNUSED( hP3d );

	/* allocate memory for tile data structures */
	hContext->pTiles = (BP3D_P_Tile *) BKNI_Malloc( usTileCountX * usTileCountY * sizeof (BP3D_P_Tile) );
	if( hContext->pTiles == NULL )
		return BERR_OUT_OF_SYSTEM_MEMORY;

	BKNI_Memset( (void *) hContext->pTiles, 0, usTileCountX * usTileCountY * sizeof (BP3D_P_Tile) );

	/* set tile information */
	hContext->usTileCountX = usTileCountX;
	hContext->usTileCountY = usTileCountY;
	hContext->ucTileShiftX = 0;
	hContext->ucTileShiftY = 0;

	while( usTileWidth > 1 ) 
	{
		hContext->ucTileShiftX++;
		usTileWidth /= 2;
	}

	while( usTileHeight > 1 ) 
	{
		hContext->ucTileShiftY++;
		usTileHeight /= 2;
	}

	return BERR_SUCCESS;
}

/***************************************************************************/
void BP3D_P_DestroyTiles(
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	uint16_t ii, jj;

	BSTD_UNUSED(hP3d);

	/* loop through all tiles */
	for( jj = 0; jj < hContext->usTileCountY; ++jj )
	{
		for( ii = 0; ii < hContext->usTileCountX; ++ii )
		{
			/* destroy all layers in tile */
			BP3D_P_Tile *pTile = BP3D_P_GET_TILE(hContext, ii, jj);
			BP3D_P_DestroyAllLayers( pTile );
		}
	}

	/* free tile data structures */
	BKNI_Free( hContext->pTiles );
	hContext->pTiles = NULL;
}

/***************************************************************************/
void BP3D_P_ResetTiles(
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	uint16_t ii, jj;

	BSTD_UNUSED(hP3d);

	/* loop through all tiles */
	for( jj = 0; jj < hContext->usTileCountY; ++jj )
	{
		for( ii = 0; ii < hContext->usTileCountX; ++ii )
		{
			BP3D_P_Tile *pTile = BP3D_P_GET_TILE(hContext, ii, jj);
			BP3D_P_Layer *pLayer = pTile->pFirstLayer;

			/* reset layers */
			BP3D_P_ResetLayers( pTile );

			/* clear tile data structure */
			BKNI_Memset( pTile, 0, sizeof (BP3D_P_Tile) );
			pTile->pFirstLayer = pLayer;
			pTile->pCurrLayer = pLayer;
		}
	}
	hContext->bUsingDepthBuffer = false;
}

#ifdef BP3D_P_USE_BINS
/***************************************************************************/
void BP3D_P_AddPrimitiveToTiles(
	BP3D_Handle hP3d,
	BP3D_P_Rect *pRect,
	uint32_t ulPrimOffset )
{
	uint16_t usTileMinX = pRect->usMinX / hP3d->hContext->ulTileWidth;
	uint16_t usTileMaxX = (pRect->usMaxX + hP3d->hContext->ulTileWidth - 1) / hP3d->hContext->ulTileWidth;
	uint16_t ii;

	BP3D_P_Tile *pTile = BP3D_P_GET_TILE(hP3d->hContext, usTileMinX, 0);

	for( ii = usTileMinX; ii < usTileMaxX; ++ii )
	{
		BP3D_P_Layer *pLayer = BP3D_P_GetCurrentLayer( pTile );

		pLayer->ulNodeCount++;
		pLayer->aulNodes[pLayer->ulNodeCount] = pLayer->aucNodeIndicesRight[pRect->usMinY] | (ulPrimOffset << BP3D_P_PRIMITIVE_ADDR_SHIFT);
		pLayer->aucNodeIndicesRight[pRect->usMinY] = (uint8_t) pLayer->ulNodeCount;
		pTile++;
	}
}
#else
/***************************************************************************/
void BP3D_P_AddPrimitiveToTiles(
	BP3D_Handle hP3d,
	BP3D_P_Rect *pRect,
	uint32_t ulPrimOffset )
{
#if 0
	uint16_t usTileMinX = pRect->usMinX >> hP3d->hContext->ucTileShiftX;
	uint16_t usTileMinY = pRect->usMinY >> hP3d->hContext->ucTileShiftY;
	uint16_t usTileMaxX = (pRect->usMaxX + hP3d->hContext->ulTileWidth - 1) >> hP3d->hContext->ucTileShiftX;
	uint16_t usTileMaxY = (pRect->usMaxY + hP3d->hContext->ulTileHeight - 1) >> hP3d->hContext->ucTileShiftY;
#else
	uint16_t usTileMinX = pRect->usMinX / hP3d->hContext->ulTileWidth;
	uint16_t usTileMinY = pRect->usMinY / hP3d->hContext->ulTileHeight;
	uint16_t usTileMaxX = (pRect->usMaxX + hP3d->hContext->ulTileWidth - 1) / hP3d->hContext->ulTileWidth;
	uint16_t usTileMaxY = (pRect->usMaxY + hP3d->hContext->ulTileHeight - 1) / hP3d->hContext->ulTileHeight;
#endif
	uint16_t ii, jj;

	/* add primitive to tiles' layers */
	for( jj = usTileMinY; jj < usTileMaxY; ++jj )
	{
		BP3D_P_Layer *pLayer;
		BP3D_P_Tile *pTile = BP3D_P_GET_TILE(hP3d->hContext, usTileMinX, jj);
		uint16_t usTileMinScan = jj << hP3d->hContext->ucTileShiftY;
		uint16_t usTileMaxScan = usTileMinScan + hP3d->hContext->ulTileHeight;
		uint16_t usLayerMinY = (pRect->usMinY < usTileMinScan) ? 0 : (pRect->usMinY - usTileMinScan + 1);
		uint16_t usLayerMaxY = (pRect->usMaxY > usTileMaxScan) ? ((uint16_t) hP3d->hContext->ulTileHeight + 1) : (pRect->usMaxY - usTileMinScan + 1);

		/* add primitive to layer's right for all but right-most tile */
		for( ii = usTileMinX; ii < usTileMaxX - 1; ++ii )
		{
			pLayer = BP3D_P_GetCurrentLayer( pTile );
			if( pLayer )
				BP3D_P_AddPrimitiveToLayerRight( pLayer, ulPrimOffset, usLayerMinY, usLayerMaxY );
			pTile++;
		}

		/* add primitive to layer's left for right-most tile */
		pLayer = BP3D_P_GetCurrentLayer( pTile );
		if( pLayer )
			BP3D_P_AddPrimitiveToLayerLeft( pLayer, ulPrimOffset, usLayerMinY, usLayerMaxY );
	}
}
#endif

/***************************************************************************/
BP3D_P_Tile *BP3D_P_GetTile(
	BP3D_Handle hP3d,
	uint16_t usTileX,
	uint16_t usTileY )
{
	BP3D_P_Tile *pTile;

	/* check if tile coordinates are out of bounds */
	if( (usTileX >= hP3d->hContext->usTileCountX) || (usTileY >= hP3d->hContext->usTileCountY) )
		return NULL;

	/* get tile */
	pTile = BP3D_P_GET_TILE(hP3d->hContext, usTileX, usTileY);

	/* check if tile is mergeable */
	if( !BP3D_P_IS_TILE_MERGEABLE( pTile ) )
		return NULL;

	return pTile;
}

/***************************************************************************/
bool BP3D_P_MergeTiles(
	BP3D_Handle hP3d,
	BP3D_P_Tile *pTile0,
	BP3D_P_Tile *pTile1 )
{
	uint16_t jj;

	/* loop through tiles */
	BP3D_P_Tile *pTile = pTile0;
	while( pTile < pTile1 )
	{
		/* check if tile is mergeable */
		if( !BP3D_P_IS_TILE_MERGEABLE( pTile ) )
			return false;

		pTile++;
	}

	/* loop through scans in tile */
	for( jj = 0; jj <= hP3d->hContext->ulTileHeight; ++jj )
	{
		uint32_t ulPrimPerScan = 0;

		/* loop through all but last tile */
		pTile = pTile0;
		while( pTile < pTile1 - 1 )
		{
			/* count primitives in tile left */
			ulPrimPerScan += pTile->aucPrimPerScanLeft[jj];
			pTile++;
		}

		/* count all primitives in last tile and check if overflowed */
		ulPrimPerScan += pTile->aucPrimPerScanLeft[jj] + pTile->aucPrimPerScanRight[jj];
		if( ulPrimPerScan > BP3D_P_PRIMITIVE_PER_SCAN )
			return false;
	}

	return true;
}

/***************************************************************************/
void BP3D_P_MergeTiles_HorzAndVert(
	BP3D_Handle hP3d,
	uint16_t usTileX,
	uint16_t usTileY )
{
	uint16_t usMergeCountX = 1;
	uint16_t usMergeCountY = 1;
	bool bRightEdge = false;
	bool bBottomEdge = false;
	uint16_t ii, jj;

	/* merge tiles in increasing larger squares until there is an overflow */
	while( (!bRightEdge) && (!bBottomEdge) )
	{
		usMergeCountX++;
		usMergeCountY++;

		/* loop through tile rows */
		for( jj = 0; jj < usMergeCountY; ++jj )
		{
			/* get first and last tile in row */
			BP3D_P_Tile *pTile0 = BP3D_P_GetTile( hP3d, usTileX, usTileY + jj );
			BP3D_P_Tile *pTile1 = BP3D_P_GetTile( hP3d, usTileX + usMergeCountX - 1, usTileY + jj );

			/* check if tile row can be merged */
			if( (pTile0 == NULL) || (pTile1 == NULL) || (!BP3D_P_MergeTiles( hP3d, pTile0, pTile1 + 1 )) )
			{
				/* check if last row */
				if( jj == usMergeCountY - 1 )
					bBottomEdge = true;
				else
					bRightEdge = true;
				break;
			}
		}
	}

	usMergeCountY--;

	/* continue merging tiles in increasing larger horizontal rectangles until there is an overflow */
	while( !bRightEdge )
	{
		usMergeCountX++;

		/* loop through tile rows */
		for( jj = 0; jj < usMergeCountY; ++jj )
		{
			/* get first and last tile in row */
			BP3D_P_Tile *pTile0 = BP3D_P_GetTile( hP3d, usTileX, usTileY + jj );
			BP3D_P_Tile *pTile1 = BP3D_P_GetTile( hP3d, usTileX + usMergeCountX - 1, usTileY + jj );

			/* check if tile row can be merged */
			if( (pTile0 == NULL) || (pTile1 == NULL) || (!BP3D_P_MergeTiles( hP3d, pTile0, pTile1 + 1 )) )
			{
				bRightEdge = true;
				break;
			}
		}
	}

	usMergeCountX--;

	/* mark tiles as rendered */
	for( jj = usTileY; jj < usTileY + usMergeCountY; ++jj )
	{
		BP3D_P_Tile *pTile = BP3D_P_GET_TILE(hP3d->hContext, usTileX, jj);
		pTile->ulMergeCount = usMergeCountX;

		for( ii = usTileX; ii < usTileX + usMergeCountX; ++ii )
		{
			pTile->ulRenderCount = 1;
			pTile++;
		}
	}
}

/***************************************************************************/
void BP3D_P_ProcessTiles_Bins(
	BP3D_Handle hP3d )
{
	uint16_t ii;

	BP3D_P_Tile *pTile  = BP3D_P_GET_TILE(hP3d->hContext, 0, 0);
	for( ii = 0; ii < hP3d->hContext->usTileCountX; ++ii )
	{
		if( pTile->pFirstLayer )
		{
			uint32_t ulPrimCount = pTile->pFirstLayer->ulNodeCount;
			uint32_t ulRegsOffset;
			uint32_t ulListOffset;
			uint32_t *pulRegs;
			uint32_t *pulList;

			if( ulPrimCount == 0 )
			{
				BP3D_P_AddEmptyPointToTiles( hP3d, ii * hP3d->hContext->ulTileWidth, 0 );
				ulPrimCount++;
			}

			pulRegs = (uint32_t *) BP3D_P_AllocDisplayList( hP3d, ulPrimCount, &ulRegsOffset );
			if( pulRegs == NULL )
				return;

			pulList = pulRegs + BP3D_P_REG_COUNT;
			ulListOffset = ulRegsOffset + BP3D_P_REG_COUNT * sizeof (uint32_t);

			ulPrimCount = BP3D_P_WriteBinDisplayList( hP3d, pTile, pulList );

			BP3D_P_WriteTileRegSet( hP3d, pulRegs, ulRegsOffset, ulListOffset, ulPrimCount, ii, 0, 1, 1, true, true );
		}
		pTile++;
	}
}

/***************************************************************************/
void BP3D_P_ProcessTiles_NoMerge(
	BP3D_Handle hP3d )
{
	uint16_t ii, jj;

	/* loop through tiles vertically */
	for( jj = 0; jj < hP3d->hContext->usTileCountY; ++jj )
	{
		BP3D_P_Tile *pTile  = BP3D_P_GET_TILE(hP3d->hContext, 0, jj);

		/* loop through tiles horizontally */
		for( ii = 0; ii < hP3d->hContext->usTileCountX; ++ii )
		{
			/* loop through tile layers */
			BP3D_P_Layer *pLayer0 = pTile->pFirstLayer;
			do
			{
				/* merge layers within tile */
				BP3D_P_Layer *pLayer1 = pLayer0 ? BP3D_P_MergeLayers( pTile, pLayer0 ) : NULL;
				bool bTileOverflow = (bool) (pLayer1 || (pLayer0 != pTile->pFirstLayer));

				/* render tile */
				if( bTileOverflow )
				{
					BP3D_P_ProcessOverflowTile( hP3d, pTile, pLayer0, pLayer1, ii, jj );
				}
				else
				{
					BP3D_P_ProcessUnderflowTiles( hP3d, ii, jj, 1, 1 );
				}

				pLayer0 = pLayer1;
			}
			while( pLayer0 );

			pTile++;
		}
	}
}

/***************************************************************************/
void BP3D_P_ProcessTiles_HorzMerge(
	BP3D_Handle hP3d )
{
	uint16_t ii, jj;

	/* loop through tiles vertically */
	for( jj = 0; jj < hP3d->hContext->usTileCountY; ++jj )
	{
		BP3D_P_Tile *pTile  = BP3D_P_GET_TILE(hP3d->hContext, 0, jj);
		BP3D_P_Tile *pTile0 = pTile;
		BP3D_P_Tile *pTile1 = pTile;

		/* loop through tiles horizontally */
		for( ii = 0; ii < hP3d->hContext->usTileCountX; ++ii )
		{
			/* loop through tile layers */
			BP3D_P_Layer *pLayer0 = pTile1->pFirstLayer;
			do
			{
				/* merge layers within tile */
				BP3D_P_Layer *pLayer1 = pLayer0 ? BP3D_P_MergeLayers( pTile1, pLayer0 ) : NULL;

				bool bTileEmpty    = (bool) ((pLayer0 == NULL) && (!hP3d->hContext->bClearColor) && (!hP3d->hContext->bClearDepth));
				bool bTileOverflow = (bool) (pLayer1 || (pLayer0 != pTile1->pFirstLayer));
				bool bRowFinished  = (bool) (ii == (hP3d->hContext->usTileCountX - 1));

				/* check if muliple tiles pending */
				if( pTile0 < pTile1 )
				{
					/* check if current tile can be horizontally merged with previous tiles */
					if( bTileEmpty || bTileOverflow || (!BP3D_P_MergeTiles( hP3d, pTile0, pTile1 + 1 )) )
					{
						/* render previously merged tiles */
						BP3D_P_ProcessUnderflowTiles( hP3d, (uint16_t) (pTile0 - pTile), jj, (uint16_t) (pTile1 - pTile0), 1 );
						pTile0 = pTile1;
					}
				}

				/* render overflow tile */
				if( bTileOverflow )
				{
					BP3D_P_ProcessOverflowTile( hP3d, pTile1, pLayer0, pLayer1, ii, jj );
					pTile0 = pTile1 + 1;
				}
				/* render finished row of tiles */
				else if( bRowFinished )
				{
					BP3D_P_ProcessUnderflowTiles( hP3d, (uint16_t) (pTile0 - pTile), jj, (uint16_t) ((pTile1 - pTile0) + 1), 1 );
				}

				pLayer0 = pLayer1;
			}
			while( pLayer0 );

			pTile1++;
		}
	}
}

/***************************************************************************/
void BP3D_P_ProcessTiles_HorzThenVertMerge(
	BP3D_Handle hP3d )
{
	uint16_t ii, jj;

	/* FIRST PASS THROUGH TILES: Process tiles that overflow and track 
	   tiles that can be horizontally merged. */

	/* loop through tiles vertically */
	for( jj = 0; jj < hP3d->hContext->usTileCountY; ++jj )
	{
		BP3D_P_Tile *pTile0 = BP3D_P_GET_TILE(hP3d->hContext, 0, jj);
		BP3D_P_Tile *pTile1 = pTile0;

		/* loop through tiles horizontally */
		for( ii = 0; ii < hP3d->hContext->usTileCountX; ++ii )
		{
			/* check if the tile has any layers */
			BP3D_P_Layer *pLayer0 = pTile1->pFirstLayer;
			if( pLayer0 )
			{
				/* attempt to merge tile layers */
				BP3D_P_Layer *pLayer1 = BP3D_P_MergeLayers( pTile1, pLayer0 );
				if( pLayer1 )
				{
					/* loop through merged sets of tile layers */
					do
					{
						/* process overflow tile and merge next set of layers */
						BP3D_P_ProcessOverflowTile( hP3d, pTile1, pLayer0, pLayer1, ii, jj );
						pLayer0 = pLayer1;

						if( pLayer1 )
							pLayer1 = BP3D_P_MergeLayers( pTile1, pLayer1 );
					}
					while( pLayer0 );

					pTile0 = pTile1 + 1;
				}
				/* check if tile is not empty */
				else if( pLayer0->ulNodeCount || hP3d->hContext->bClearColor || hP3d->hContext->bClearDepth || hP3d->hContext->hSrcColorSurface )
				{
					/* check if tiles can be merged */
					if( (pTile0 == pTile1) || BP3D_P_MergeTiles( hP3d, pTile0, pTile1 + 1 ) )
					{
						pTile0->ulMergeCount++;
					}
					else
					{
						pTile0 = pTile1;
						if( pTile0->ulMergeCount == 0 )
							pTile0->ulMergeCount++;
					}
				}
			}
			/* check if tile has a clear */
			else if( hP3d->hContext->bClearColor || hP3d->hContext->bClearDepth || hP3d->hContext->hSrcColorSurface )
			{
				/* track tile merges */
				pTile0->ulMergeCount++;
			}
			/* tile is empty */
			else
			{
				pTile0 = pTile1 + 1;
			}

			pTile1++;
		}
	}

	/* SECOND PASS THROUGH TILES: Vertically merge tiles between consecutive 
	   rows that have the same amount of horizontally merged tiles. */

	/* loop through tiles horiztonally */
	for( ii = 0; ii < hP3d->hContext->usTileCountX; ++ii )
	{
		BP3D_P_Tile *pTile0 = BP3D_P_GET_TILE(hP3d->hContext, ii, 0);
		uint32_t ulVertMergeCount = 0;
		uint16_t usTileMinY = 0;

		/* loop through tiles vertically */
		for( jj = 0; jj < hP3d->hContext->usTileCountY; ++jj )
		{
			BP3D_P_Tile *pTile1 = (jj < hP3d->hContext->usTileCountY - 1) ? BP3D_P_GET_TILE(hP3d->hContext, ii, jj + 1) : NULL;

			if( pTile0 && pTile0->ulMergeCount )
			{
				ulVertMergeCount++;
				if( (pTile1 == NULL) || (pTile0->ulMergeCount != pTile1->ulMergeCount) )
				{
					/* process tile rows that could not be vertically merged */
					BP3D_P_ProcessUnderflowTiles( hP3d, ii, usTileMinY, (uint16_t) pTile0->ulMergeCount, (uint16_t) ulVertMergeCount );

					ulVertMergeCount = 0;
					usTileMinY = jj + 1;
					pTile0 = pTile1;
				}
				else
				{
					pTile1->ulMergeCount = 0;
				}
			}
			else
			{
				pTile0 = pTile1;
				usTileMinY = jj + 1;
			}
		}
	}
}

/***************************************************************************/
void BP3D_P_ProcessTiles_HorzAndVertMerge(
	BP3D_Handle hP3d )
{
	uint16_t ii, jj;

	/* FIRST PASS THROUGH TILES: Merge tile layers and process overflow tiles. */

	/* loop through tiles vertically */
	for( jj = 0; jj < hP3d->hContext->usTileCountY; ++jj )
	{
		BP3D_P_Tile *pTile = BP3D_P_GET_TILE(hP3d->hContext, 0, jj);

		/* loop through tiles horizontally */
		for( ii = 0; ii < hP3d->hContext->usTileCountX; ++ii )
		{
			/* check if the tile has any layers */
			BP3D_P_Layer *pLayer0 = pTile->pFirstLayer;
			if( pLayer0 && pLayer0->ulNodeCount )
			{
				/* attempt to merge tile layers */
				BP3D_P_Layer *pLayer1 = BP3D_P_MergeLayers( pTile, pLayer0 );
				if( pLayer1 )
				{
					/* loop through merged sets of tile layers */
					do
					{
						/* process overflow tile and merge next set of layers */
						BP3D_P_ProcessOverflowTile( hP3d, pTile, pLayer0, pLayer1, ii, jj );
						pLayer0 = pLayer1;

						if( pLayer1 )
							pLayer1 = BP3D_P_MergeLayers( pTile, pLayer1 );
						pTile->ulRenderCount++;
					}
					while( pLayer0 );
				}
			}
			else
			{
				if( (hP3d->hContext->hDstColorSurface == hP3d->hContext->hSrcColorSurface) && 
					(!hP3d->hContext->bClearColor) && (!hP3d->hContext->bClearDepth) )
				{
					pTile->ulRenderCount++;
				}
			}

			pTile++;
		}
	}

	/* SECOND PASS THROUGH TILES: Merge tiles horizontally and vertically. */

	/* loop through tiles vertically */
	for( jj = 0; jj < hP3d->hContext->usTileCountY; ++jj )
	{
		/* loop through tiles horizontally */
		for( ii = 0; ii < hP3d->hContext->usTileCountX; ++ii )
		{
			BP3D_P_Tile *pTile = BP3D_P_GetTile( hP3d, ii, jj );
			if( pTile )
				BP3D_P_MergeTiles_HorzAndVert( hP3d, ii, jj );
		}
	}

	/* THIRD PASS THROUGH TILES: Vertically merge tiles between consecutive 
	   rows that have the same amount of horizontally merged tiles. */

	/* loop through tiles horiztonally */
	for( ii = 0; ii < hP3d->hContext->usTileCountX; ++ii )
	{
		BP3D_P_Tile *pTile0 = BP3D_P_GET_TILE(hP3d->hContext, ii, 0);
		uint32_t ulVertMergeCount = 0;
		uint16_t usTileMinY = 0;

		/* loop through tiles vertically */
		for( jj = 0; jj < hP3d->hContext->usTileCountY; ++jj )
		{
			BP3D_P_Tile *pTile1 = (jj < hP3d->hContext->usTileCountY - 1) ? BP3D_P_GET_TILE(hP3d->hContext, ii, jj + 1) : NULL;

			if( pTile0 && pTile0->ulMergeCount )
			{
				ulVertMergeCount++;
				if( (pTile1 == NULL) || (pTile0->ulMergeCount != pTile1->ulMergeCount) )
				{
					/* process tile rows that could not be vertically merged */
					BP3D_P_ProcessUnderflowTiles( hP3d, ii, usTileMinY, (uint16_t) pTile0->ulMergeCount, (uint16_t) ulVertMergeCount );

					ulVertMergeCount = 0;
					usTileMinY = jj + 1;
					pTile0 = pTile1;
				}
				else
				{
					pTile1->ulMergeCount = 0;
				}
			}
			else
			{
				pTile0 = pTile1;
				usTileMinY = jj + 1;
			}
		}
	}
}

/***************************************************************************/
void BP3D_P_ProcessUnderflowTiles(
	BP3D_Handle hP3d,
	uint16_t usTileX,	
	uint16_t usTileY,
	uint16_t usTileCountX,
	uint16_t usTileCountY )
{
	uint32_t *pulRegs;
	uint32_t *pulList;
	uint32_t ulRegsOffset;
	uint32_t ulListOffset;
	uint32_t ulPrimCount = 0;
	uint32_t ulListCount;
	uint16_t ii, jj;

	/* count primitives */
	for( jj = usTileY; jj < usTileY + usTileCountY; ++jj )
	{
		BP3D_P_Tile *pTile = BP3D_P_GET_TILE(hP3d->hContext, usTileX, jj);

		for( ii = usTileX; ii < usTileX + usTileCountX - 1; ++ii )
		{
			ulPrimCount += (jj == usTileY) ? pTile->ulPrimCountLeft[0] : 0;
			ulPrimCount += pTile->ulPrimCountLeft[1];
			pTile++;
		}

		ulPrimCount += (jj == usTileY) ? pTile->ulPrimCountLeft[0] : 0;
		ulPrimCount += (jj == usTileY) ? pTile->ulPrimCountRight[0] : 0;
		ulPrimCount += pTile->ulPrimCountLeft[1];
		ulPrimCount += pTile->ulPrimCountRight[1];
	}

	/* PX3D fix: since the device cannot do a clear without 
	   rendering primitives add an empty point to the tile */
	if( ulPrimCount == 0 )
	{
		BP3D_P_AddEmptyPointToTiles( hP3d, usTileX * hP3d->hContext->ulTileWidth, usTileY * hP3d->hContext->ulTileHeight );
		ulPrimCount++;
	}

	/* allocate memory for registers and display list */
	pulRegs = (uint32_t *) BP3D_P_AllocDisplayList( hP3d, ulPrimCount, &ulRegsOffset );
	if( pulRegs == NULL )
	{
		BDBG_ERR(( "Could not allocate display list for underflow tile\n" ));
		return;
	}

	pulList = pulRegs + BP3D_P_REG_COUNT;
	ulListOffset = ulRegsOffset + BP3D_P_REG_COUNT * sizeof (uint32_t);

	/* write display list */
	ulListCount = BP3D_P_WriteUnderflowDisplayList( hP3d, pulList, usTileX, usTileY, usTileCountX, usTileCountY );
	if( ulListCount != ulPrimCount )
	{
		BDBG_MSG(( "Underflow Tile: Display List does not match primitive count (list=%d) (prim=%d) (x=%d) (y=%d) (w=%d) (h=%d)\n", 
			(int) ulListCount, (int) ulPrimCount, (int) usTileX, (int) usTileY, (int) usTileCountX, (int) usTileCountY ));
	}

	/* write registers */
	BP3D_P_WriteTileRegSet( hP3d, pulRegs, ulRegsOffset, ulListOffset, ulPrimCount, usTileX, usTileY, usTileCountX, usTileCountY, true, true );
}

/***************************************************************************/
void BP3D_P_ProcessOverflowTile(
	BP3D_Handle hP3d,
	BP3D_P_Tile *pTile,
	BP3D_P_Layer *pLayer0,
	BP3D_P_Layer *pLayer1,
	uint16_t usTileX,
	uint16_t usTileY )
{
	BP3D_P_Layer *pLayer = pLayer0;
	uint32_t *pulRegs;
	uint32_t *pulList;
	uint32_t ulRegsOffset;
	uint32_t ulListOffset;
	uint32_t ulListCount;
	uint32_t ulPrimCount = 0;
	bool bFirstLayer = (bool) (pTile->pFirstLayer == pLayer0);
	bool bLastLayer = (bool) (pLayer1 == NULL);

	/* count primitives */
	while( pLayer != pLayer1 )
	{
		ulPrimCount += pLayer->ulNodeCount;
		pLayer = pLayer->pNextLayer;
	}

	/* allocate memory for registers and display list */
	pulRegs = (uint32_t *) BP3D_P_AllocDisplayList( hP3d, ulPrimCount, &ulRegsOffset );
	if( pulRegs == NULL )
	{
		BDBG_ERR(( "Could not allocate display list for overflow tile\n" ));
		return;
	}

	pulList = pulRegs + BP3D_P_REG_COUNT;
	ulListOffset = ulRegsOffset + BP3D_P_REG_COUNT * sizeof (uint32_t);

	/* write display list */
	ulListCount = BP3D_P_WriteOverflowDisplayList( hP3d, pLayer0, pLayer1, pulList );
	if( ulListCount != ulPrimCount )
	{
		BDBG_MSG(( "Overflow Tile: Display List does not match primitive count (list=%d) (prim=%d)\n", 
			(int) ulListCount, (int) ulPrimCount ));
	}

	/* write registers */
	BP3D_P_WriteTileRegSet( hP3d, pulRegs, ulRegsOffset, ulListOffset, ulPrimCount, usTileX, usTileY, 1, 1, bFirstLayer, bLastLayer );
}

/***************************************************************************/
void BP3D_P_WriteTileRegSet(
	BP3D_Handle hP3d,
	uint32_t *pulRegs,
	uint32_t ulRegsOffset,
	uint32_t ulListOffset,
	uint32_t ulPrimCount,
	uint16_t usTileX,
	uint16_t usTileY,
	uint16_t usTileCountX,
	uint16_t usTileCountY,
	bool bFirstLayer,
	bool bLastLayer )
{
	BP3D_P_Buffer *pDstColorBuffer = bLastLayer ? &(hP3d->hContext->DstColorBuffer) : &(hP3d->TmpColorBuffer);
	BP3D_P_Buffer *pDstDepthBuffer = bLastLayer ? &(hP3d->hContext->DstDepthBuffer) : &(hP3d->TmpDepthBuffer);
	BP3D_P_Buffer *pSrcColorBuffer = bFirstLayer ? &(hP3d->hContext->SrcColorBuffer) : &(hP3d->TmpColorBuffer);
	BP3D_P_Buffer *pSrcDepthBuffer = bFirstLayer ? &(hP3d->hContext->SrcDepthBuffer) : &(hP3d->TmpDepthBuffer);

	bool bDstColor = (bool) ((!bLastLayer) || hP3d->hContext->hDstColorSurface);
	bool bDstDepth = (bool) ((!bLastLayer) || hP3d->hContext->hDstDepthSurface);
	bool bSrcColor = (bool) ((!bFirstLayer) || (hP3d->hContext->hSrcColorSurface && (!hP3d->hContext->bClearColor)));
	bool bSrcDepth = (bool) ((!bFirstLayer) || (hP3d->hContext->hSrcDepthSurface && (!hP3d->hContext->bClearDepth)));

	uint16_t usDstX = bLastLayer ? usTileX * hP3d->hContext->ulTileWidth + hP3d->hContext->ulBufferOffsetX : 0;
	uint16_t usDstY = bLastLayer ? usTileY * hP3d->hContext->ulTileHeight + hP3d->hContext->ulBufferOffsetY : 0;
	uint16_t usSrcX = bFirstLayer ? usTileX * hP3d->hContext->ulTileWidth + hP3d->hContext->ulBufferOffsetX : 0;
	uint16_t usSrcY = bFirstLayer ? usTileY * hP3d->hContext->ulTileHeight + hP3d->hContext->ulBufferOffsetY : 0;
	uint16_t usTileWidth = BP3D_P_MIN((usTileX + usTileCountX) * (uint16_t) hP3d->hContext->ulTileWidth, 
		hP3d->hContext->Viewport.usMaxX - hP3d->hContext->Viewport.usMinX) - usTileX * (uint16_t) hP3d->hContext->ulTileWidth;

	bool bBufferColor = hP3d->hContext->bClearColor || hP3d->hContext->hSrcColorSurface;
	bool bBufferDepth = hP3d->hContext->bClearDepth || hP3d->hContext->hSrcDepthSurface;

	/* check if using depth buffer */
	if( (bDstDepth && pDstDepthBuffer) || (bSrcDepth && pSrcDepthBuffer) || (bSrcColor && pSrcColorBuffer) )
		hP3d->hContext->bUsingDepthBuffer = true;

	/* set registers */
	BP3D_P_SET_SURFACE_REGISTERS( Dest, Dest, ColorDst, bDstColor, pDstColorBuffer, usDstX, usDstY, usTileWidth );
	BP3D_P_SET_SURFACE_REGISTERS( Dest, DestZ, ZDst, bDstDepth, pDstDepthBuffer, usDstX, usDstY, usTileWidth );
	BP3D_P_SET_SURFACE_REGISTERS( Source, Source, ColorSrc, bSrcColor, pSrcColorBuffer, usSrcX, usSrcY, usTileWidth );
	BP3D_P_SET_SURFACE_REGISTERS( Source, SourceZ, ZSrc, bSrcDepth, pSrcDepthBuffer, usSrcX, usSrcY, usTileWidth );
	BP3D_P_SET_BUFFERCLEAR_REGISTER( bBufferColor, bBufferDepth, bFirstLayer, bLastLayer );
	BP3D_P_SET_RECTANGLE_REGISTERS( &hP3d->hContext->Viewport, usTileX, usTileY, usTileCountX, usTileCountY );
	BP3D_P_SET_REGISTER_DATA( DisplayPageAddr, DisplayPageAddr, ulListOffset );
	BP3D_P_SET_REGISTER_DATA( PrimCount, PrimCount, ulPrimCount );

#ifdef BP3D_P_PX3D_TIMER
	BP3D_P_SET_REGISTER_DATA( IRQMask, EndOfFrame, 1 );
	BP3D_P_SET_REGISTER_DATA( ExceptionAction, EnableEndOfFrame, 1 );
#endif

	/* PX3D fix: when there is a src color buffer and there is no src depth buffer,
	   the depth format needs to be set to the same size as the color format */
	if( bSrcColor && (!bSrcDepth) )
		BP3D_P_SET_REGISTER_DATA( SourceConfig, ZSrcFormat, pSrcColorBuffer->ulBpp / 2 );

	/* copy registers to device memory list */
	BKNI_Memcpy( pulRegs, hP3d->hContext->aulRegs, BP3D_P_REG_COUNT * sizeof (uint32_t) );

	/* attach previous regset to current regset */
	if( hP3d->hContext->pulPrevRegSet )
		hP3d->hContext->pulPrevRegSet[BP3D_P_PX3D_NextRegTableAddr] = ulRegsOffset;
	else
		hP3d->hContext->ulRegSetOffset = ulRegsOffset;

	hP3d->hContext->ulRegSetCount++;
	hP3d->hContext->pulPrevRegSet = pulRegs;
}

/***************************************************************************/
void BP3D_P_DumpTiles(
	BP3D_Handle hP3d )
{
	uint16_t ii, jj;

	BDBG_MSG(( "TILES - %d x %d\n", (int) hP3d->hContext->usTileCountX, (int) hP3d->hContext->usTileCountY ));

	for( jj = 0; jj < hP3d->hContext->usTileCountY; ++jj )
	{
		for( ii = 0; ii < hP3d->hContext->usTileCountX; ++ii )
		{
			BP3D_P_Tile *pTile = BP3D_P_GET_TILE(hP3d->hContext, ii, jj);
			BP3D_P_Layer *pLayer = pTile->pFirstLayer;
			uint32_t ulLayerCount = 0;
			uint32_t ulPrimCount = 0;

			while( pLayer )
			{
				ulLayerCount++;
				ulPrimCount += pLayer->ulNodeCount;
				pLayer = pLayer->pNextLayer;
			}

			BDBG_MSG(( "%3x %4x - ", (unsigned int) ulLayerCount, (unsigned int) ulPrimCount ));
		}
	
		BDBG_MSG(( "\n" ));
	}

	BDBG_MSG(( "\n" ));
}

/***************************************************************************/
void BP3D_P_DumpDisplayList(
	BP3D_Handle hP3d,
	uint32_t *pulRegSet )
{
	void *pAddr;
	uint32_t ii;

	if( BMEM_ConvertOffsetToAddress( hP3d->hMemory, pulRegSet[BP3D_P_PX3D_DisplayPageAddr], &pAddr ) == BERR_SUCCESS )
	{
		uint32_t *pulList = (uint32_t *) pAddr;
		BDBG_ERR(( "PRIM COUNT: %d", (int) pulRegSet[BP3D_P_PX3D_PrimCount] ));
		for( ii = 0; ii < pulRegSet[BP3D_P_PX3D_PrimCount]; ++ii )
		{
			if( BMEM_ConvertOffsetToAddress( hP3d->hMemory, pulList[ii], &pAddr ) == BERR_SUCCESS )
			{
				BP3D_P_Primitive *pPrim = (BP3D_P_Primitive *) pAddr;
/*				BDBG_ERR(( "%08x - %08x %08x %08x", (unsigned int) pulList[ii], (unsigned int) pPrim->Header.ulPrimHeaderA, (unsigned int) pPrim->Header.ulPrimHeaderB, (unsigned int) pPrim->Header.ulTextureAddr ));
				BDBG_ERR(( "%8.3f %8.3f %5.3f %5.3f %08x %08x %6.4f %6.4f %5.3f", pPrim->v0.fX, pPrim->v0.fY, pPrim->v0.fZ, pPrim->v0.fW, (unsigned int) pPrim->v0.ulDiffuse, (unsigned int) pPrim->v0.ulSpecular, pPrim->v0.fS, pPrim->v0.fT, pPrim->v0.fM ));
				BDBG_ERR(( "%08x %08x %08x", (unsigned int) pPrim->Header.ulPrimHeaderA, (unsigned int) pPrim->Header.ulPrimHeaderB, (unsigned int) pPrim->Header.ulTextureAddr ));
				BDBG_ERR(( "%08x %08x %08x %08x %08x %08x %08x %08x %08x", *((unsigned int *) (void *) &pPrim->v0.fX), *((unsigned int *) (void *) &pPrim->v0.fY), *((unsigned int *) (void *) &pPrim->v0.fZ), *((unsigned int *) (void *) &pPrim->v0.fW), (unsigned int) pPrim->v0.ulDiffuse, (unsigned int) pPrim->v0.ulSpecular, *((unsigned int *) (void *) &pPrim->v0.fS), *((unsigned int *) (void *) &pPrim->v0.fT), *((unsigned int *) (void *) &pPrim->v0.fM) ));
*/				if( (pPrim->Header.ulPrimHeaderA >> 30) > 0 )
				{
/*					BDBG_ERR(( "%8.3f %8.3f %5.3f %5.3f %08x %08x %6.4f %6.4f %5.3f", pPrim->v1.fX, pPrim->v1.fY, pPrim->v1.fZ, pPrim->v1.fW, (unsigned int) pPrim->v1.ulDiffuse, (unsigned int) pPrim->v1.ulSpecular, pPrim->v1.fS, pPrim->v1.fT, pPrim->v1.fM ));
					BDBG_ERR(( "%08x %08x %08x %08x %08x %08x %08x %08x %08x", *((unsigned int *) (void *) &pPrim->v1.fX), *((unsigned int *) (void *) &pPrim->v1.fY), *((unsigned int *) (void *) &pPrim->v1.fZ), *((unsigned int *) (void *) &pPrim->v1.fW), (unsigned int) pPrim->v1.ulDiffuse, (unsigned int) pPrim->v1.ulSpecular, *((unsigned int *) (void *) &pPrim->v1.fS), *((unsigned int *) (void *) &pPrim->v1.fT), *((unsigned int *) (void *) &pPrim->v1.fM) ));
*/				}
				if( (pPrim->Header.ulPrimHeaderA >> 30) == 2 )
				{
/*					BDBG_ERR(( "%8.3f %8.3f %5.3f %5.3f %08x %08x %6.4f %6.4f %5.3f", pPrim->v2.fX, pPrim->v2.fY, pPrim->v2.fZ, pPrim->v2.fW, (unsigned int) pPrim->v2.ulDiffuse, (unsigned int) pPrim->v2.ulSpecular, pPrim->v2.fS, pPrim->v2.fT, pPrim->v2.fM ));
					BDBG_ERR(( "%08x %08x %08x %08x %08x %08x %08x %08x %08x", *((unsigned int *) (void *) &pPrim->v2.fX), *((unsigned int *) (void *) &pPrim->v2.fY), *((unsigned int *) (void *) &pPrim->v2.fZ), *((unsigned int *) (void *) &pPrim->v2.fW), (unsigned int) pPrim->v2.ulDiffuse, (unsigned int) pPrim->v2.ulSpecular, *((unsigned int *) (void *) &pPrim->v2.fS), *((unsigned int *) (void *) &pPrim->v2.fT), *((unsigned int *) (void *) &pPrim->v2.fM) ));
*/				}
			}
		}
		BDBG_ERR(( "%08x ", (unsigned int) pulList[ii] ));
	}
}

/***************************************************************************/
void BP3D_P_DumpRegSets(
	BP3D_Handle hP3d )
{
	void *pAddr;
	uint32_t ulRegSetOffset = hP3d->hContext->ulRegSetOffset;
	uint32_t jj;

	BDBG_ERR(( "REG SETS - %d", (int) hP3d->hContext->ulRegSetCount ));

	for( jj = 0; jj < hP3d->hContext->ulRegSetCount; ++jj )
	{
		if( BMEM_ConvertOffsetToAddress( hP3d->hMemory, ulRegSetOffset, &pAddr ) == BERR_SUCCESS )
		{
			uint32_t *pulRegSet = (uint32_t *) pAddr;
			BDBG_ERR(( "\n%08x\n", (unsigned int) ulRegSetOffset ));
			BDBG_ERR(( "Mode              %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_Mode] ));
			BDBG_ERR(( "TopLeft           %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_TopLeft] ));
			BDBG_ERR(( "BottomRight       %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_BottomRight] ));
			BDBG_ERR(( "DestConfig        %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_DestConfig] ));
			BDBG_ERR(( "SourceConfig      %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_SourceConfig] ));
			BDBG_ERR(( "DisplayListConfig %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_DisplayListConfig] ));
			BDBG_ERR(( "TextureConfig     %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_TextureConfig] ));
			BDBG_ERR(( "DisplayListAddr   %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_DisplayListAddr] ));
			BDBG_ERR(( "DisplayPageAddr   %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_DisplayPageAddr] ));
			BDBG_ERR(( "DestAddr          %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_DestAddr] ));
			BDBG_ERR(( "DestZAddr         %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_DestZAddr] ));
			BDBG_ERR(( "SourceAddr        %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_SourceAddr] ));
			BDBG_ERR(( "SourceZAddr       %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_SourceZAddr] ));
			BDBG_ERR(( "TranslateColorKey %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_TranslateColorKey] ));
			BDBG_ERR(( "ColorKey1         %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_ColorKey1] ));
			BDBG_ERR(( "ColorKey2         %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_ColorKey2] ));
			BDBG_ERR(( "ColorKey3         %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_ColorKey3] ));
			BDBG_ERR(( "FogColor          %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_FogColor] )); 
			BDBG_ERR(( "AlphaReference    %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_AlphaReference] ));
			BDBG_ERR(( "IRQMask           %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_IRQMask] ));
			BDBG_ERR(( "ExceptionAction   %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_ExceptionAction] ));
			BDBG_ERR(( "ExceptionStatus   %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_ExceptionStatus] ));
			BDBG_ERR(( "NextRegTableAddr  %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_NextRegTableAddr] ));
			BDBG_ERR(( "PrimCount         %08x", (unsigned int) pulRegSet[BP3D_P_PX3D_PrimCount] ));

			BP3D_P_DumpDisplayList( hP3d, pulRegSet );
			ulRegSetOffset = pulRegSet[BP3D_P_PX3D_NextRegTableAddr];
		}
	}
}

#if 0
/***************************************************************************/
void BP3D_P_VerifyRegSets(
	BP3D_Handle hP3d )
{
	void *pAddr;
	BP3D_P_Primitive *pPrim;
	uint8_t aucPrimPerScan[BP3D_P_SCENE_HEIGHT_MAX];
	uint32_t *pulRegSet;
	uint32_t *pulDisplayList;
	uint32_t ulRegSetOffset = hP3d->hContext->ulRegSetOffset;
	uint32_t ii, jj, ss;

	/* loop through reg sets */
	for( jj = 0; jj < hP3d->hContext->ulRegSetCount; ++jj )
	{
		uint32_t ulPrimPerScanMax = 0;
		uint32_t ulPrim = 0;
		uint32_t ulScan = 0;

		BKNI_Memset( aucPrimPerScan, 0, BP3D_P_SCENE_HEIGHT_MAX );

		/* get reg set address */
		if( BMEM_ConvertOffsetToAddress( hP3d->hMemory, ulRegSetOffset, &pAddr ) != BERR_SUCCESS )
		{
			BDBG_MSG(( "VERIFY ERROR: Count not get Reg Set address %08x\n", (unsigned int) ulRegSetOffset ));
			goto done;
		}
		pulRegSet = (uint32_t *) pAddr;

		/* get display list address */
		if( BMEM_ConvertOffsetToAddress( hP3d->hMemory, pulRegSet[BP3D_P_PX3D_DisplayPageAddr], &pAddr ) != BERR_SUCCESS )
		{
			BDBG_MSG(( "VERIFY ERROR: Count not get Display List address %08x\n", (unsigned int) pulRegSet[BP3D_P_PX3D_DisplayPageAddr] ));
			goto done;
		}
		pulDisplayList = (uint32_t *) pAddr;

		/* loop through primitives */
		for( ii = 0; ii < pulRegSet[BP3D_P_PX3D_PrimCount]; ++ii )
		{
			uint32_t ulTileMinY = pulRegSet[BP3D_P_PX3D_TopLeft] >> 16;
			uint32_t ulPrevMinY = 0;
			uint32_t ulPrimMinY = 0;
			uint32_t ulPrimMaxY = 0;

			/* get primitive address */
			if( BMEM_ConvertOffsetToAddress( hP3d->hMemory, pulDisplayList[ii], &pAddr ) != BERR_SUCCESS )
			{
				BDBG_MSG(( "VERIFY ERROR: Count not get Primitive address %08x\n", (unsigned int) pulDisplayList[ii] ));
				goto done;
			}
			pPrim = (BP3D_P_Primitive *) pAddr;
						
			ulPrevMinY = ulPrimMinY;
			if( BP3D_P_GET_PRIMHEADER_DATA( PrimHeaderA, PrimitiveType ) == BCHP_PX3D_Type_PrimHeaderA_PrimitiveType_Triangle )
			{
				ulPrimMinY = (uint32_t) BP3D_P_MIN(BP3D_P_MIN(pPrim->v0.fY, pPrim->v1.fY), pPrim->v2.fY);
				ulPrimMaxY = (uint32_t) BP3D_P_MAX(BP3D_P_MAX(pPrim->v0.fY, pPrim->v1.fY), pPrim->v2.fY) + 1;
			}
			else if( BP3D_P_GET_PRIMHEADER_DATA( PrimHeaderA, PrimitiveType ) == BCHP_PX3D_Type_PrimHeaderA_PrimitiveType_Point )
			{
				ulPrimMinY = (uint32_t) pPrim->v0.fY;
				ulPrimMaxY = (uint32_t) pPrim->v0.fY + 1;
			}
			else
			{
				ulPrimMinY = (uint32_t) BP3D_P_MIN(pPrim->v0.fY, pPrim->v1.fY);
				ulPrimMaxY = (uint32_t) BP3D_P_MAX(pPrim->v0.fY, pPrim->v1.fY) + 1;
			}

			/* count primitives per scan */
			for( ss = ulPrimMinY; ss < ulPrimMaxY; ++ss )
			{
				aucPrimPerScan[ss]++;
				ulPrimPerScanMax = BP3D_P_MAX(ulPrimPerScanMax, aucPrimPerScan[ss]);
				if( aucPrimPerScan[ss] > BP3D_P_PRIMITIVE_PER_SCAN )
				{
					ulPrim = ii;
					ulScan = ss;
				}
			}

			/* verify primitive scan order */
			if( ii && (ulPrimMinY >= ulTileMinY) && (ulPrimMinY < ulPrevMinY) )
			{
				BDBG_MSG(( "VERIFY ERROR: Primitives out of scan order\n" ));
				goto done;
			}
		}

		/* verify primitive per scan */
		if( ulPrimPerScanMax > BP3D_P_PRIMITIVE_PER_SCAN )
		{
			BDBG_MSG(( "VERIFY ERROR: Scanline Overflow (max=%d) (scan=%d) (prim=%d of %d) (l=%d) (t=%d) (r=%d) (b=%d)\n", 
				(int) ulPrimPerScanMax, (int) ulScan, (int) ulPrim, (int) pulRegSet[BP3D_P_PX3D_PrimCount], 
				(int) pulRegSet[BP3D_P_PX3D_TopLeft] & 0xFFFF, (int) pulRegSet[BP3D_P_PX3D_TopLeft] >> 16, 
				(int) pulRegSet[BP3D_P_PX3D_BottomRight] & 0xFFFF, (int) pulRegSet[BP3D_P_PX3D_BottomRight] >> 16 ));
			goto done;
		}

		/* verify null at end of display list */
		if( pulDisplayList[pulRegSet[BP3D_P_PX3D_PrimCount]] != 0 )
		{
			BDBG_MSG(( "VERIFY ERROR: Not NULL at end of Display List %08x\n", (unsigned int) pulDisplayList[pulRegSet[BP3D_P_PX3D_PrimCount]] ));
			goto done;
		}

		ulRegSetOffset = pulRegSet[BP3D_P_PX3D_NextRegTableAddr];
	}

	return;

done:
	BDBG_MSG(( "REG SET NUM: %d of %d\n", (int) jj, (int) hP3d->hContext->ulRegSetCount ));
}
#endif

/***************************************************************************/
#define BP3D_P_WRITE_PIXEL( dst_format, src_format, src_pixel, dst_pixel32, dst_pixel16 ) \
{ \
	BPXL_Format eFormat = dst_format; \
	if( eFormat == BPXL_eA8_R8_G8_B8 ) \
	{ \
		dst_pixel32 = (uint32_t) src_pixel; \
	} \
	else \
	{ \
		unsigned int uiPixel = 0; \
		BPXL_ConvertPixel( dst_format, src_format, (unsigned int) (src_pixel), &uiPixel ); \
		dst_pixel16 = (uint16_t) uiPixel; \
	} \
}

/***************************************************************************/
void BP3D_P_DrawTileBoundry(
	BP3D_Handle hP3d,
	BP3D_P_Buffer *pBuffer,
	uint32_t *pulRegSet,
	uint32_t ulColor )
{
	uint32_t *pulDst;
	uint16_t *pusDst;
	uint32_t ulMinX = (pulRegSet[BP3D_P_PX3D_TopLeft] & 0xFFFF) - hP3d->hContext->Viewport.usMinX + hP3d->hContext->ulBufferOffsetX;
	uint32_t ulMinY = (pulRegSet[BP3D_P_PX3D_TopLeft] >> 16) - hP3d->hContext->Viewport.usMinY + hP3d->hContext->ulBufferOffsetY;
	uint32_t ulMaxX = (pulRegSet[BP3D_P_PX3D_BottomRight] & 0xFFFF) - hP3d->hContext->Viewport.usMinX + hP3d->hContext->ulBufferOffsetX;
	uint32_t ulMaxY = (pulRegSet[BP3D_P_PX3D_BottomRight] >> 16) - hP3d->hContext->Viewport.usMinY + hP3d->hContext->ulBufferOffsetY;
	uint32_t ii;

	/* draw top line */
	pulDst = (uint32_t *) ((uint8_t *) pBuffer->pvData + ulMinY * pBuffer->ulPitch);
	pusDst = (uint16_t *) ((uint8_t *) pBuffer->pvData + ulMinY * pBuffer->ulPitch);
	for( ii = ulMinX; ii < ulMaxX; ++ii )
		BP3D_P_WRITE_PIXEL( pBuffer->eFormat, BPXL_eA8_R8_G8_B8, ulColor, pulDst[ii], pusDst[ii] );

	/* draw left line */
	pulDst = (uint32_t *) ((uint8_t *) pBuffer->pvData + ulMinX * pBuffer->ulBpp);
	pusDst = (uint16_t *) ((uint8_t *) pBuffer->pvData + ulMinX * pBuffer->ulBpp);
	for( ii = ulMinY; ii < ulMaxY; ++ii )
		BP3D_P_WRITE_PIXEL( pBuffer->eFormat, BPXL_eA8_R8_G8_B8, ulColor, 
			pulDst[ii * pBuffer->ulPitch / pBuffer->ulBpp], pusDst[ii * pBuffer->ulPitch / pBuffer->ulBpp] );

	/* draw bottom line */
	pulDst = (uint32_t *) ((uint8_t *) pBuffer->pvData + (ulMaxY - 1) * pBuffer->ulPitch);
	pusDst = (uint16_t *) ((uint8_t *) pBuffer->pvData + (ulMaxY - 1) * pBuffer->ulPitch);
	for( ii = ulMinX; ii < ulMaxX; ++ii )
		BP3D_P_WRITE_PIXEL( pBuffer->eFormat, BPXL_eA8_R8_G8_B8, ulColor, pulDst[ii], pusDst[ii] );

	/* draw right line */
	pulDst = (uint32_t *) ((uint8_t *) pBuffer->pvData + (ulMaxX - 1) * pBuffer->ulBpp);
	pusDst = (uint16_t *) ((uint8_t *) pBuffer->pvData + (ulMaxX - 1) * pBuffer->ulBpp);
	for( ii = ulMinY; ii < ulMaxY; ++ii )
		BP3D_P_WRITE_PIXEL( pBuffer->eFormat, BPXL_eA8_R8_G8_B8, ulColor, 
			pulDst[ii * pBuffer->ulPitch / pBuffer->ulBpp], pusDst[ii * pBuffer->ulPitch / pBuffer->ulBpp] );
}

/***************************************************************************/
void BP3D_P_DrawTileBoundries(
	BP3D_Handle hP3d )
{
	void *pAddr;
	uint32_t *pulRegSet;
	uint32_t *pulPrevRegSet = NULL;
	uint32_t ulRegSetOffset = hP3d->hContext->ulRenderRegSetOffset;
	uint32_t ulRenderCount = 0;
	uint32_t ii;

	for( ii = 0; ii < hP3d->hContext->ulRenderRegSetCount; ++ii )
	{
		if( BMEM_ConvertOffsetToAddress( hP3d->hMemory, ulRegSetOffset, &pAddr ) != BERR_SUCCESS )
			return;

		pulRegSet = (uint32_t *) pAddr;

		if( pulPrevRegSet && ((pulRegSet[BP3D_P_PX3D_TopLeft] != pulPrevRegSet[BP3D_P_PX3D_TopLeft]) ||
			(pulRegSet[BP3D_P_PX3D_BottomRight] != pulPrevRegSet[BP3D_P_PX3D_BottomRight])) )
		{
			BP3D_P_DrawTileBoundry( hP3d, &hP3d->hContext->RenderDstColorBuffer, pulPrevRegSet, 
				(ulRenderCount == 1) ? 0xFF00FF00 : 0xFFFF0000 );
			ulRenderCount = 0;
		}

		ulRenderCount++;
		pulPrevRegSet = pulRegSet;
		ulRegSetOffset = pulRegSet[BP3D_P_PX3D_NextRegTableAddr];
	}
		
	if( pulPrevRegSet )
	{
		BP3D_P_DrawTileBoundry( hP3d, &hP3d->hContext->RenderDstColorBuffer, pulPrevRegSet, 
			(ulRenderCount == 1) ? 0xFF00FF00 : 0xFFFF0000 );
	}
}

/* End of File */
