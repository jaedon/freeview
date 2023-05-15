/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bp3d_layer.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/20/04 4:09p $
 *
 * Module Description: Private 3D Rendering Layer Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/p3d/7038/bp3d_layer.c $
 * 
 * Hydra_Software_Devel/2   2/20/04 4:09p nissen
 * PR 9132: More implementation.
 * 
 * Hydra_Software_Devel/1   1/22/04 2:37p nissen
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
BP3D_P_Layer *BP3D_P_CreateLayer(
	BP3D_P_Tile *pTile )
{
	/* allocate memory for layer data structure */
	BP3D_P_Layer *pLayer = (BP3D_P_Layer *) BKNI_Malloc( sizeof (BP3D_P_Layer) );
	if( pLayer == NULL )
		return NULL;

	BKNI_Memset( (void *) pLayer, 0, sizeof (BP3D_P_Layer) );

	/* attach layer to tile */
	if( pTile->pFirstLayer == NULL )
		pTile->pFirstLayer = pLayer;

	if( pTile->pCurrLayer )
		pTile->pCurrLayer->pNextLayer = pLayer;

	return pLayer;
}

/***************************************************************************/
void BP3D_P_DestroyAllLayers(
	BP3D_P_Tile *pTile )
{
	/* free all layer data structures */
	BP3D_P_Layer *pLayer = pTile->pFirstLayer;
	while( pLayer )
	{
		BP3D_P_Layer *pNextLayer = pLayer->pNextLayer;
		BKNI_Free( pLayer );
		pLayer = pNextLayer;
	}
}

/***************************************************************************/
void BP3D_P_ResetLayers(
	BP3D_P_Tile *pTile )
{
	BP3D_P_Layer *pLayer = pTile->pFirstLayer;
	if( pLayer )
	{
		/* free all but first layer data structures */
		pLayer = pLayer->pNextLayer;
		while( pLayer )
		{
			BP3D_P_Layer *pNextLayer = pLayer->pNextLayer;
			BKNI_Free( pLayer );
			pLayer = pNextLayer;
		}

		/* clear first layer data structure */
		BKNI_Memset( (void *) pTile->pFirstLayer, 0, sizeof (BP3D_P_Layer) );
	}
}

/***************************************************************************/
BP3D_P_Layer *BP3D_P_GetCurrentLayer(
	BP3D_P_Tile *pTile )
{
	/* check if layer is missing or full */
	if( (pTile->pCurrLayer == NULL) || (pTile->pCurrLayer->ulNodeCount == BP3D_P_PRIMITIVE_PER_LAYER) )
	{
		/* create new layer */
		pTile->pCurrLayer = BP3D_P_CreateLayer( pTile );
		if( pTile->pCurrLayer == NULL )
			return NULL;
	}

	return pTile->pCurrLayer;
}

/***************************************************************************/
void BP3D_P_AddPrimitiveToLayerLeft(
	BP3D_P_Layer *pLayer,
	uint32_t ulPrimOffset,
	uint16_t usLayerMinY,
	uint16_t usLayerMaxY
)
{
	/* add primitive to layer on left */
	pLayer->ulNodeCount++;
	pLayer->aScanData[usLayerMinY].ucStartCountLeft++;
	pLayer->aScanData[usLayerMaxY].ucStopCountLeft++;
	pLayer->aulNodes[pLayer->ulNodeCount] = pLayer->aucNodeIndicesLeft[usLayerMinY] | (ulPrimOffset << BP3D_P_PRIMITIVE_ADDR_SHIFT);
	pLayer->aucNodeIndicesLeft[usLayerMinY] = (uint8_t) pLayer->ulNodeCount;
}

/***************************************************************************/
void BP3D_P_AddPrimitiveToLayerRight(
	BP3D_P_Layer *pLayer,
	uint32_t ulPrimOffset,
	uint16_t usLayerMinY,
	uint16_t usLayerMaxY
)
{
	/* add primitive to layer on right */
	pLayer->ulNodeCount++;
	pLayer->aScanData[usLayerMinY].ucStartCountRight++;
	pLayer->aScanData[usLayerMaxY].ucStopCountRight++;
	pLayer->aulNodes[pLayer->ulNodeCount] = pLayer->aucNodeIndicesRight[usLayerMinY] | (ulPrimOffset << BP3D_P_PRIMITIVE_ADDR_SHIFT);
	pLayer->aucNodeIndicesRight[usLayerMinY] = (uint8_t) pLayer->ulNodeCount;
}

/***************************************************************************/
BP3D_P_Layer *BP3D_P_MergeLayers(
	BP3D_P_Tile *pTile,
	BP3D_P_Layer *pLayer )
{
	uint32_t ulPrimCountLeft = 0;
	uint32_t ulPrimCountRight = 0;
	uint32_t ulPrimPerScanLeft = 0;
	uint32_t ulPrimPerScanRight = 0;
	uint16_t ii;

	/* loop through scans first layer */
	for( ii = 0; ii <= BP3D_P_TILE_HEIGHT; ++ii )
	{
		/* count primitives on left and right */
		ulPrimCountLeft += pLayer->aScanData[ii].ucStartCountLeft;
		ulPrimCountRight += pLayer->aScanData[ii].ucStartCountRight;
		ulPrimPerScanLeft += pLayer->aScanData[ii].ucStartCountLeft - pLayer->aScanData[ii].ucStopCountLeft;
		ulPrimPerScanRight += pLayer->aScanData[ii].ucStartCountRight - pLayer->aScanData[ii].ucStopCountRight;

		/* store primitive scan totals in tile */
		pTile->aucPrimPerScanLeft[ii] = (uint8_t) ulPrimPerScanLeft;
		pTile->aucPrimPerScanRight[ii] = (uint8_t) ulPrimPerScanRight;
	}

	/* store primitive totals in tile */
	pTile->ulPrimCountLeft[0] = pLayer->aScanData[0].ucStartCountLeft;
	pTile->ulPrimCountLeft[1] = ulPrimCountLeft - pLayer->aScanData[0].ucStartCountLeft;
	pTile->ulPrimCountRight[0] = pLayer->aScanData[0].ucStartCountRight;
	pTile->ulPrimCountRight[1] = ulPrimCountRight - pLayer->aScanData[0].ucStartCountRight;

	/* loop through the rest of the layers */
	pLayer = pLayer->pNextLayer;
	while( pLayer )
	{
		ulPrimCountLeft = 0;
		ulPrimCountRight = 0;
		ulPrimPerScanLeft = 0;
		ulPrimPerScanRight = 0;

		/* loop through scans in tile */
		for( ii = 0; ii <= BP3D_P_TILE_HEIGHT; ++ii )
		{
			/* count primitives on left and right */
			ulPrimCountLeft += pLayer->aScanData[ii].ucStartCountLeft;
			ulPrimCountRight += pLayer->aScanData[ii].ucStartCountRight;
			ulPrimPerScanLeft += pLayer->aScanData[ii].ucStartCountLeft - pLayer->aScanData[ii].ucStopCountLeft;
			ulPrimPerScanRight += pLayer->aScanData[ii].ucStartCountRight - pLayer->aScanData[ii].ucStopCountRight;

			/* store primitive scan totals in tile */
			pTile->aucPrimPerScanLeft[ii] = pTile->aucPrimPerScanLeft[ii] + (uint8_t) ulPrimPerScanLeft;
			pTile->aucPrimPerScanRight[ii] = pTile->aucPrimPerScanRight[ii] + (uint8_t) ulPrimPerScanRight;

			/* check if layers are overflowed */
			if( (pTile->aucPrimPerScanLeft[ii] + pTile->aucPrimPerScanRight[ii]) > BP3D_P_PRIMITIVE_PER_SCAN )
				return pLayer;
		}

		/* store primitive totals in tile */
		pTile->ulPrimCountLeft[0] += pLayer->aScanData[0].ucStartCountLeft;
		pTile->ulPrimCountLeft[1] += ulPrimCountLeft - pLayer->aScanData[0].ucStartCountLeft;
		pTile->ulPrimCountRight[0] += pLayer->aScanData[0].ucStartCountRight;
		pTile->ulPrimCountRight[1] += ulPrimCountRight - pLayer->aScanData[0].ucStartCountRight;

		pLayer = pLayer->pNextLayer;
	}

	return NULL;
}

/* End of File */
