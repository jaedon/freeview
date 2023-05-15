/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmem_local.c $
 * $brcm_Revision: Hydra_Software_Devel/30 $
 * $brcm_Date: 10/11/12 7:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/mem/bmem_local.c $
 * 
 * Hydra_Software_Devel/30   10/11/12 7:42p albertl
 * SW7435-255: Fixed accounting errors in free and alloc tracking.
 * 
 * Hydra_Software_Devel/29   9/21/12 6:46p albertl
 * SW7435-255: Optimized BMEM with AA Tree implementation.
 * 
 * Hydra_Software_Devel/28   7/24/12 11:21a albertl
 * SW7552-303: Set default heap alignment to 9 for safety.  Removed
 * checking for 0 alignment, now handled by minimum checking code.
 * 
 * Hydra_Software_Devel/27   7/23/12 5:14p albertl
 * SW7552-303: Removed usage of BMEM_HEAP_ALIGNMENT for cache alignment
 * issues.  Default alignment is now 0 to force user to set a valid
 * alignment.
 * 
 * Hydra_Software_Devel/26   2/7/12 6:36p albertl
 * SW7425-2345: Changed total allocation and number of allocation tracking
 * to work in eFastest.
 * 
 * Hydra_Software_Devel/25   11/14/11 2:22p albertl
 * SW7425-1293: Fixed use of uncached address when cached addresses exist
 * during heap creation.  Fixed coverity issues.
 * 
 * Hydra_Software_Devel/24   9/9/11 7:10p albertl
 * SW7346-201: Moved watermark calculation to alloc and free for accurate
 * calculation.
 * 
 * Hydra_Software_Devel/23   3/28/11 5:12p albertl
 * SW7425-247: Incorporated BDBG_OBJECT handle validation.
 * 
 * Hydra_Software_Devel/22   1/10/11 3:32p albertl
 * SW7408-193: BMEM_Heapinfo now includes high watermark.  Added
 * BMEM_Heap_ResetHighWatermark().
 * 
 * Hydra_Software_Devel/21   11/8/10 4:48p albertl
 * SW7420-1155: Fixed size calculation to match fixed implementation of
 * pEnd.
 * 
 * Hydra_Software_Devel/20   11/4/10 7:39p albertl
 * SW7420-1155: Fixed pEnd to really be last available byte and addrEnd to
 * be last byte in chunk rather than byte after that.
 * 
 * Hydra_Software_Devel/19   5/26/09 4:18p albertl
 * PR55389: Replaced uintptr_t with uint32_t.
 * 
 * Hydra_Software_Devel/18   7/12/07 6:26p albertl
 * PR31066:  Updated warning message to give platform and user specified
 * uiAlignment size.
 * 
 * Hydra_Software_Devel/17   7/6/07 6:53p albertl
 * PR31066:  Changed BMEM_HEAP_ALIGNMENT to 6 for 7400 B0 and later and
 * 7405.  Added warnings when user specified alignment is less than and
 * forced to BMEM_HEAP_ALIGNMENT.
 * 
 * Hydra_Software_Devel/16   3/15/07 7:23p albertl
 * PR28682:  Changed BMEM_GUARD_SIZE_BYTES to be dynamically calculated
 * based on runtime safety config.
 * 
 * Hydra_Software_Devel/15   6/21/06 3:11p albertl
 * PR20247:  Moved safety config table to bmem.c  BMEM_P_Heap now uses a
 * BMEM_P_SafetyConfigInfo pointer to track safety configuration settings
 * instead of tracking each configuration separately.
 * 
 * Hydra_Software_Devel/14   6/16/06 3:23p albertl
 * PR20247, PR20276, PR20354:  Added the ability to control debug
 * configuration at runtime.  Added address and offset checking to
 * conversion functions.  BMEM_SetCache can now only be called before
 * heaps are allocated from.  Added BMEM_Heap functions.
 * 
 * Hydra_Software_Devel/13   10/7/05 3:53p jasonh
 * PR 17374: Allowed GetHeapInfo to return original creation parameters.
 * 
 * Hydra_Software_Devel/12   3/10/05 5:05p albertl
 * PR13677:  Both local and system bookkeeping made available at heap
 * creation though functions BMEM_CreateHeapSystem and
 * BMEM_CreateHeapLocal.
 * 
 * Hydra_Software_Devel/11   1/21/05 4:23p albertl
 * PR13717:  Replaced BMEM_DBG_ENTER with BDBG_ENTER.  Moved printouts to
 * DBG_MSG, fixed pvHeap.
 * 
 * Hydra_Software_Devel/10   11/2/04 5:27p pntruong
 * PR13076: Initialized new members added.
 *
 * Hydra_Software_Devel/9   4/1/04 12:49p vsilyaev
 * PR 10201: Added memory monitor
 *
 * Hydra_Software_Devel/8   12/31/03 12:02p jasonh
 * PR 8940: Changed return type of destroyheap to void. Removed use of
 * DEBUG macro and printf.
 *
 * Hydra_Software_Devel/7   9/15/03 5:21p jasonh
 * Fixed heap control to store offset and address. Added local specific
 * destroy heap function.
 *
 * Hydra_Software_Devel/6   9/5/03 2:04p jasonh
 * Changed Check to ValidateHeap.
 *
 * Hydra_Software_Devel/5   9/5/03 1:41p jasonh
 * Wrapped unused arguments with BSTD_UNUSED.
 *
 * Hydra_Software_Devel/4   9/2/03 1:48p vadim
 * Some magnum updates.
 *
 * Hydra_Software_Devel/2   3/20/03 3:51p erickson
 * renamed all MEM_ to BMEM_
 *
 * Hydra_Software_Devel/1   3/20/03 3:24p erickson
 * initial bmem work, taken from SetTop/memorylib
 *
 * SanJose_DVTSW_Devel\4   4/15/02 10:14p eric
 * BMEM_CheckHeap is dead.  Long live bcmHeapCheck.
 *
 * SanJose_DVTSW_Devel\3   4/15/02 3:34p poz
 * Updated function name to bcmHeapCreate.
 *
 * SanJose_DVTSW_Devel\2   4/15/02 2:29p poz
 * Modified to get semaphore when BMEM_REENTRANT.
 *
 * SanJose_DVTSW_Devel\1   4/15/02 1:35p poz
 * Implementation of local-memory heap manager.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bmem.h"
#include "bmem_config.h"
#include "bmem_priv.h"
#include "bmem_debug.h"
#include "berr.h"

BDBG_MODULE(BMEM);

BLST_AA_TREE_GENERATE_INSERT(BMEM_P_AddrTree, BMEM_P_AddrKey*, BMEM_P_BlockInfo, stAddrNode, BMEM_P_Addr_Map_Cmp)
BLST_AA_TREE_GENERATE_FIRST(BMEM_P_AddrTree, BMEM_P_BlockInfo, stAddrNode)
BLST_AA_TREE_GENERATE_INSERT(BMEM_P_SizeTree, BMEM_P_SizeKey*, BMEM_P_BlockInfo, stSizeNode, BMEM_P_Size_Map_Cmp)

/**********************************************************************func*
 * BMEM_P_LocalGetAddress - Get the managed memory base address for a
 *                           block.
 *
 * Returns:
 *    0 if a NULL BlockInfo is given, otherwise the base address.
 */

uint32_t BMEM_P_LocalGetAddress
(
	BMEM_Handle pheap,
	BMEM_P_BlockInfo *pbi
)
{
	BSTD_UNUSED( pheap );
	return (uint32_t) pbi;
}

/**********************************************************************func*
 * BMEM_P_LocalGetBlockInfo - Get the bookkeeping info for a managed base
 *                             address.
 *
 * Local implementation just returns the address, being also the location
 * of the bookkeeping info.
 *
 * Returns:
 *    On success, a pointer to the BlockInfo for the managed base address.
 *
 */

BMEM_P_BlockInfo *BMEM_P_LocalGetBlockInfo
(
	BMEM_Handle pheap,
	uint32_t addr
)
{
	BMEM_P_BlockInfo *pbi = (BMEM_P_BlockInfo *)addr;

	pbi->pHeap = pheap;

	return pbi;
}

/**********************************************************************func*
 * BMEM_P_LocalDropBlockInfo -
 *    Free up the bookkeeping info for a managed memory base address
 *
 * Returns:
 *    void
 */

void BMEM_P_LocalDropBlockInfo
(
	BMEM_Handle pheap,
	BMEM_P_BlockInfo *pbi
)
{
	BSTD_UNUSED( pheap );
	BSTD_UNUSED( pbi );
	return;
}


/**********************************************************************func*
 * BMEM_CreateHeapLocal - Initializes the heap with local bookkeeping.
 *
 * This function inititalizes a heap at a given location and size.
 * Any previous allocations in the chunk of memory handed over to this
 * function are lost. Every heap has a base minimum alignment for all of
 * the allocations within that heap. (However, you can specify a greater
 * alignment when actually doing an allocation.)
 *
 * In this implementation, the first bit of the heap is used to store heap
 * information such as pointers to the free and used list, as well as other
 * bookkeeping information.
 *
 * Returns:
 *   Returns true if heap is initialized, or false if the given memory
 *   chunk is too small to be a heap.
 *
 */
BERR_Code BMEM_CreateHeapLocal
(
	BMEM_Handle  *ppHeap,      /* Address to store pointer of new heap info structure */
	void         *pvHeap,      /* Pointer to beginning of memory chunk to manage */
	uint32_t      ulOffset,    /* Device offset of initial location */
	size_t        zSize,       /* Size of chunk to manage in bytes */
	unsigned int  uiAlignment  /* Minimum alignment for all allocations in the heap */
)
{
	BMEM_Heap_Settings stHeapSettings;
	BMEM_Heap_Settings *pHeapSettings = &stHeapSettings;

	BMEM_Heap_GetDefaultSettings(pHeapSettings);

	pHeapSettings->uiAlignment = uiAlignment;
	pHeapSettings->eBookKeeping = BMEM_BOOKKEEPING_LOCAL;

	return (BMEM_Heap_Create(NULL, pvHeap, ulOffset, zSize, pHeapSettings, ppHeap));
}

BERR_Code BMEM_P_LocalCreateHeap
(
	BMEM_Handle        *ppHeap,      /* Address to store pointer of new heap info structure */
	void               *pvHeap,      /* Pointer to beginning of memory chunk to manage */
	uint32_t            ulOffset,    /* Device offset of initial location */
	size_t              zSize,       /* Size of chunk to manage in bytes */
	BMEM_Heap_Settings *pHeapSettings
)
{
	BERR_Code         err = BERR_SUCCESS;
	BMEM_Handle       pheap;
	BMEM_P_BlockInfo *pbi;
	uint32_t          addrHeap;
	uint32_t          addrEnd = (uint32_t)pvHeap + zSize - 1;
	unsigned int      uiAlignment = pHeapSettings->uiAlignment;
	size_t            ulAlignedSize;
	uint32_t          eSafetyConfig = pHeapSettings->eSafetyConfig;
	BMEM_P_AddrKey    stAddrKey;
	BMEM_P_SizeKey    stSizeKey;

	BDBG_ENTER(BMEM_P_LocalCreateHeap);
	BDBG_MSG(("BMEM_P_LocalCreateHeap(%p, %lu, %d)",
		pvHeap, zSize, (int)pHeapSettings->uiAlignment));

	if (pHeapSettings->eSafetyConfig > BMEM_P_SafetyTableSize)
	{
		/* Invalid safety config. */
		err = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto done;
	}

	if (pHeapSettings->eSafetyConfig > BMEM_SAFETY_CONFIG)
	{
		/* Invalid run-time safety config. */
			BDBG_ERR(("Run-time safety config cannot be higher than compiled safety config!"));
			err = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto done;
	}


	/*
	 * The first bit of the heap is used to hold information about the heap
	 * itself. This is forced to a 4-byte alignment if for some reason we
	 * were given a non-aligned pointer.
	 */
	addrHeap = BMEM_HEAP_ALIGN( pvHeap );
	if( addrEnd < addrHeap + sizeof **ppHeap - 1)
	{
		/* There is no way to use this as a heap. It's too small. */
		err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
		goto done;
	}

	/* get location of heap */
	*ppHeap = pheap = (BMEM_Handle) addrHeap;
	addrHeap += sizeof (*pheap);

	/* initialize heap */
	BDBG_OBJECT_SET(pheap, BMEM_Heap);
	pheap->ulAlignMask = 0;
	pheap->pStart = NULL;
	pheap->pEnd = NULL;
	pheap->ulNumErrors = 0;
	pheap->pvData = NULL;
	pheap->pvHeap = pvHeap;
	pheap->ulOffset = ulOffset;
	pheap->monitor = NULL;
	pheap->pvCache = pHeapSettings->pCachedAddress;
	pheap->pfFlushCb = pHeapSettings->flush;
	pheap->pfFlushCb_isr = pHeapSettings->flush_isr;
	pheap->zSize = zSize;
	pheap->ulNumAllocated = 0;
	pheap->ulNumFree = 0;
	pheap->ulTotalAllocated = 0;
	pheap->ulTotalFree = 0;
	pheap->ulHighWatermark = 0;

	pheap->pSafetyConfigInfo = &(BMEM_P_SafetyConfigTbl[eSafetyConfig]);
	pheap->ulGuardSizeBytes  = pheap->pSafetyConfigInfo->iGuardSize * BMEM_GUARD_UNIT_BYTES;

	pheap->eBookKeeping = pHeapSettings->eBookKeeping;
	pheap->bCacheLocked = false;

	pheap->pGetAddressFunc = &BMEM_P_LocalGetAddress;
	pheap->pGetBlockInfoFunc = &BMEM_P_LocalGetBlockInfo;
	pheap->pDropBlockInfoFunc = &BMEM_P_LocalDropBlockInfo;
	pheap->pDestroyHeapFunc = &BMEM_P_LocalDestroyHeap;

	BLST_AA_TREE_INIT(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree);
	BLST_AA_TREE_INIT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
	BLST_AA_TREE_INIT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree);

#if (BMEM_REENTRANT_CONFIG==BMEM_REENTRANT)
	/* create mutex for re-entrant control */
	if(BKNI_CreateMutex(&(pheap->pMutex)) != BERR_SUCCESS)
	{
		err = BERR_OS_ERROR;
		goto done;
	}
#endif

	/*
	 * The minimum alignment allowed is 4 bytes since that is the required
	 * alignment for the bookkeeping information.
	 */
	if(uiAlignment < BMEM_HEAP_ALIGNMENT)
	{
		BDBG_WRN(("uiAlignment %d less than platform minimum %d.  Setting alignment to minimum.", uiAlignment, BMEM_HEAP_ALIGNMENT));
		uiAlignment = BMEM_HEAP_ALIGNMENT;
	}
	pheap->uiAlignment = uiAlignment;
	pheap->ulAlignMask = (1L << uiAlignment) - 1;

	/* Align the heap to be on the requested alignment. */
	pheap->pStart = (uint8_t *)((addrHeap+pheap->ulAlignMask) & ~pheap->ulAlignMask);
	/* Last usable byte after alignment. */
	pheap->pEnd = (uint8_t *)(((addrEnd + 1) & ~pheap->ulAlignMask) - 1);

	/* Make the first block, which represents the remaining (free) heap */
	pbi = BMEM_P_LocalGetBlockInfo(pheap, (uint32_t)pheap->pStart);

	ulAlignedSize = pheap->pEnd-pheap->pStart+1;

	if(ulAlignedSize > BMEM_BOOKKEEPING_SIZE_LOCAL)
	{
		/* Create a free block representing the entire heap. */
		pbi->ulSize = ulAlignedSize;
		pbi->ulFrontScrap = 0;
		pbi->ulBackScrap = 0;
		BMEM_P_FillGuard(pheap, pbi);

		stAddrKey.ulAddr = BMEM_P_GetAddress(pheap, pbi);
		stSizeKey.ulAddr = BMEM_P_GetAddress(pheap, pbi);
		stSizeKey.ulSize = pbi->ulSize;
		
		BLST_AA_TREE_INSERT(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree, &stAddrKey, pbi);
		BLST_AA_TREE_INSERT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, &stSizeKey, pbi);

		pheap->ulNumFree++;
		pheap->ulTotalFree = pbi->ulSize;
	}
	else
	{
		/* The chunk of memory we've been given is too small for a heap. */
		err = BERR_OS_ERROR;
	}

#if BMEM_CHECK_ALL_GUARDS
	if (pheap->pSafetyConfigInfo->bCheckAllGuards)
	{
		if (err)
		{
			BMEM_ValidateHeap(pheap);
		}
	}
#endif

done:
	BDBG_MSG(("BMEM_P_LocalCreateHeap = %d", err));
	BDBG_LEAVE(BMEM_P_LocalCreateHeap);
	return err;
}

/***************************************************************************/
void BMEM_P_LocalDestroyHeap
(
	BMEM_Handle pheap
)
{
#if BMEM_CHECK_ALL_GUARDS
	BERR_Code  err;
#endif

	BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);

#if BMEM_CHECK_ALL_GUARDS
	if (pheap->pSafetyConfigInfo->bCheckAllGuards)
	{
		/* validate heap */
		err = BERR_TRACE(BMEM_ValidateHeap(pheap));
		if (err != BERR_SUCCESS)
		{
			/* error validating heap */
			goto done;
		}
	}
#endif

	/* blocks still allocated? */
	if (BLST_AA_TREE_FIRST(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree))
	{
		/* leaked resource */
		BDBG_ERR(("Leaked resource -- unfreed blocks detected"));
	}

	BDBG_OBJECT_UNSET(pheap, BMEM_Heap);
#if (BMEM_REENTRANT_CONFIG==BMEM_REENTRANT)
	/* destroy mutex */
	BKNI_DestroyMutex(pheap->pMutex);
#endif

done:
	/* return */
	return;
}

/* End of File */
