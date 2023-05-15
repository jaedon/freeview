/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bp3d_memory.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 8/4/09 8:18p $
 *
 * Module Description: Private 3D Rendering Memory Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/p3d/7405/bp3d_memory.c $
 * 
 * Hydra_Software_Devel/10   8/4/09 8:18p nissen
 * PR 57173: Fixed coverity defect involving context handle.
 * 
 * Hydra_Software_Devel/9   7/4/09 5:02p nissen
 * PR 56587: Fixed problem freeing context memory blocks.
 * 
 * Hydra_Software_Devel/8   6/16/08 8:26p nissen
 * PR 42507: Added support for rendering with multiple contexts.
 * 
 * Hydra_Software_Devel/7   10/5/05 9:43p nissen
 * PR 17273: Added support for cached memory.
 * 
 * Hydra_Software_Devel/6   4/30/04 11:16a nissen
 * PR 9132: Fixed problem with free all memory blocks.
 * 
 * Hydra_Software_Devel/5   2/20/04 4:12p nissen
 * PR 9132: More implementation.
 * 
 * Hydra_Software_Devel/4   1/22/04 5:47p nissen
 * PR 9132: Fixed log comment.
 * 
 * Hydra_Software_Devel/3   1/22/04 5:43p nissen
 * PR 9132: Fixed log comment.
 * 
 ***************************************************************************/

#include "bstd_defs.h"
#include "berr.h"
#include "bkni.h"
#include "bp3d.h"
#include "bp3d_private.h"

/***************************************************************************/
void *BP3D_P_AllocMemoryBlock(
	BP3D_Handle hP3d,
	uint32_t *pulOffset )
{
	BP3D_P_Block *pBlock = NULL;
	while( pBlock == NULL )
	{
		/* find free block */
		pBlock = hP3d->hContext->pFirstBlock;
		while( pBlock )
		{
			if( pBlock->ulNum < hP3d->hContext->ulRenderNum )
				break;

			pBlock = pBlock->pNextBlock;
		}

		/* create new block if no free block was found */
		if( pBlock == NULL )
		{
			pBlock = BP3D_P_CreateMemoryBlock( hP3d );
			if( pBlock == NULL )
				return NULL;
		}
	}

	/* set status and frame number */
	pBlock->ulNum = hP3d->hContext->ulSceneNum;

	*pulOffset = pBlock->ulOffset;
	return pBlock->pvCached;
}

/***************************************************************************/
void BP3D_P_FreeUnusedMemoryBlocks(
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	/* loop through all blocks */
	BP3D_P_Block *pPrevBlock = NULL;
	BP3D_P_Block *pCurrBlock = hContext->pFirstBlock;
	while( pCurrBlock )
	{
		/* destroy block if unused for awhile */
		if( (pCurrBlock->ulNum < hContext->ulRenderNum) && 
			(hContext->ulSceneNum > pCurrBlock->ulNum + BP3D_P_BLOCK_LIFE) )
		{
			pCurrBlock = BP3D_P_DestroyMemoryBlock( hP3d, pCurrBlock, &pPrevBlock );
		}
		else
		{
			pPrevBlock = pCurrBlock;
			pCurrBlock = pCurrBlock->pNextBlock;
		}
	}
}

/***************************************************************************/
void BP3D_P_FreeAllMemoryBlocks(
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	/* destroy all blocks and wait if not done */
	BP3D_P_Block *pPrevBlock = NULL;
	BP3D_P_Block *pCurrBlock = hContext->pFirstBlock;
	while( pCurrBlock )
	{
		/* destroy block */
		pCurrBlock = BP3D_P_DestroyMemoryBlock( hP3d, pCurrBlock, &pPrevBlock );
	}
}

/***************************************************************************/
BP3D_P_Block *BP3D_P_CreateMemoryBlock(
	BP3D_Handle hP3d )
{
	BP3D_P_Block *pBlock;

	/* allocate memory for block structure */
	pBlock = (BP3D_P_Block *) BKNI_Malloc( sizeof (BP3D_P_Block) );
	if( pBlock == NULL )
		return NULL;

	/* allocate device memory for block */
	pBlock->pvMemory = (void *) BMEM_AllocAligned( hP3d->hMemory, BP3D_P_BLOCK_SIZE, BP3D_P_BLOCK_ALIGN, 0 );
	if( pBlock->pvMemory == NULL )
	{
		BKNI_Free( (void *) pBlock );
		return NULL;
	}

	/* get cached memory address */
	if( BMEM_ConvertAddressToCached( hP3d->hMemory, pBlock->pvMemory, &pBlock->pvCached ) != BERR_SUCCESS )
	{
		BMEM_Free( hP3d->hMemory, pBlock->pvMemory );
		BKNI_Free( (void *) pBlock );
		return NULL;
	}

	/* get primitive block offset */
	if( BMEM_ConvertAddressToOffset( hP3d->hMemory, pBlock->pvMemory, &pBlock->ulOffset ) != BERR_SUCCESS )
	{
		BMEM_Free( hP3d->hMemory, pBlock->pvMemory );
		BKNI_Free( (void *) pBlock );
		return NULL;
	}

	/* attach new block to list */
	if( hP3d->hContext->pFirstBlock == NULL )
		hP3d->hContext->pFirstBlock = pBlock;

	if( hP3d->hContext->pLastBlock )
		hP3d->hContext->pLastBlock->pNextBlock = pBlock;

	hP3d->ulBlockTotalSize += BP3D_P_BLOCK_SIZE;
	hP3d->hContext->pLastBlock = pBlock;
	pBlock->pNextBlock = NULL;

	return pBlock;
}

/***************************************************************************/
BP3D_P_Block *BP3D_P_DestroyMemoryBlock(
	BP3D_Handle hP3d,
	BP3D_P_Block *pBlock,
	BP3D_P_Block **ppPrevBlock )
{
	BP3D_P_Block *pPrevBlock = *ppPrevBlock;
	BP3D_P_Block *pNextBlock = pBlock->pNextBlock;

	/* detatch block */
	if( pPrevBlock )
		pPrevBlock->pNextBlock = pNextBlock;

	if( hP3d->hContext->pFirstBlock == pBlock )
	{
		hP3d->hContext->pFirstBlock = pBlock->pNextBlock;
		*ppPrevBlock = NULL;
	}

	if( hP3d->hContext->pLastBlock == pBlock )
		hP3d->hContext->pLastBlock = pPrevBlock;

	/* free device and structure memory */
	BMEM_Free( hP3d->hMemory, pBlock->pvMemory );
	BKNI_Free( (void *) pBlock );
	hP3d->ulBlockTotalSize -= BP3D_P_BLOCK_SIZE;

	return pNextBlock;
}

/* End of File */
