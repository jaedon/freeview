/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmem_system.c $
 * $brcm_Revision: Hydra_Software_Devel/37 $
 * $brcm_Date: 10/11/12 7:42p $
 *
 * Module Description:
 *
 * This module provides the basic functions used to implement a memory
 * manager which stores bookkeeping information in OS-allocated memory. This
 * is advantageous when the memory being managed has significant access time
 * penalties. If accessing the managed memory has no access time penalties,
 * do NOT use this module; it is algorithmically slower than direct access
 * because of the additional overhead needed.
 *
 * All of the bookeeping information close to the CPU, in "system memory".
 * To do this, there needs to be some way of associating managed memory
 * block with info about the managed memory block. In the local allocator,
 * this is done by physically locating the BlockInfo in front of the managed
 * block. This same approach can't be used if the BlockInfo lives in system
 * memory.
 *
 * Instead, a hash table is used to index the BlockInfo structures. The
 * has index is based upon the lowest significant bits of the managed memory
 * address of the block. A linked list is used in each hash bucket to handle
 * collisions. The nodes in thist list are not ordered in any way.
 *
 * When looking up a BlockInfo for a manged memory address, the address is
 * shifted and masked to give the hash key. The list of nodes at that hash
 * key is iterated over until a match is found. If we find that the lists
 * grow to an unreasonable length, then the number of hash buckets can be
 * increased. This is not done dynamically.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/mem/bmem_system.c $
 * 
 * Hydra_Software_Devel/37   10/11/12 7:42p albertl
 * SW7435-255: Fixed accounting errors in free and alloc tracking.
 * 
 * Hydra_Software_Devel/36   9/21/12 6:47p albertl
 * SW7435-255: Optimized BMEM with AA Tree implementation.
 * 
 * Hydra_Software_Devel/35   7/24/12 11:21a albertl
 * SW7552-303: Set default heap alignment to 9 for safety.  Removed
 * checking for 0 alignment, now handled by minimum checking code.
 * 
 * Hydra_Software_Devel/34   7/23/12 5:14p albertl
 * SW7552-303: Removed usage of BMEM_HEAP_ALIGNMENT for cache alignment
 * issues.  Default alignment is now 0 to force user to set a valid
 * alignment.
 * 
 * Hydra_Software_Devel/33   2/7/12 6:37p albertl
 * SW7425-2345: Changed total allocation and number of allocation tracking
 * to work in eFastest.
 * 
 * Hydra_Software_Devel/32   11/14/11 2:22p albertl
 * SW7425-1293: Fixed use of uncached address when cached addresses exist
 * during heap creation.  Fixed coverity issues.
 * 
 * Hydra_Software_Devel/31   9/9/11 7:10p albertl
 * SW7346-201: Moved watermark calculation to alloc and free for accurate
 * calculation.
 * 
 * Hydra_Software_Devel/30   9/7/11 2:05p albertl
 * SW7408-258: Fixed memory leak.
 * 
 * Hydra_Software_Devel/29   3/29/11 4:16p albertl
 * SW7425-247: Changed BDBG_OBJECT_DESTROY to BDBG_OBJECT_UNSET.
 * 
 * Hydra_Software_Devel/28   3/28/11 5:13p albertl
 * SW7425-247: Incorporated BDBG_OBJECT handle validation.
 * 
 * Hydra_Software_Devel/27   1/10/11 3:32p albertl
 * SW7408-193: BMEM_Heapinfo now includes high watermark.  Added
 * BMEM_Heap_ResetHighWatermark().
 * 
 * Hydra_Software_Devel/26   11/8/10 4:48p albertl
 * SW7420-1155: Fixed size calculation to match fixed implementation of
 * pEnd.
 * 
 * Hydra_Software_Devel/25   11/4/10 7:40p albertl
 * SW7420-1155: Fixed pEnd to really be last available byte and addrEnd to
 * be last byte in chunk rather than byte after that.
 * 
 * Hydra_Software_Devel/24   5/26/09 5:57p albertl
 * PR55389: Fixed mangled file.
 * 
 * Hydra_Software_Devel/23   5/26/09 5:52p albertl
 * PR55389: Replaced uintptr_t with uint32_t.
 * 
 * Hydra_Software_Devel/22   5/12/08 4:21p erickson
 * PR42628: free all mem in BMEM_P_SystemDestroyHeap
 *
 * Hydra_Software_Devel/21   7/12/07 6:26p albertl
 * PR31066:  Updated warning message to give platform and user specified
 * uiAlignment size.
 *
 * Hydra_Software_Devel/20   7/6/07 6:54p albertl
 * PR31066:  Changed BMEM_HEAP_ALIGNMENT to 6 for 7400 B0 and later and
 * 7405.  Added warnings when user specified alignment is less than and
 * forced to BMEM_HEAP_ALIGNMENT.
 *
 * Hydra_Software_Devel/19   3/15/07 7:24p albertl
 * PR28682:  Changed BMEM_GUARD_SIZE_BYTES to be dynamically calculated
 * based on runtime safety config.
 *
 * Hydra_Software_Devel/18   6/21/06 3:12p albertl
 * PR20247:  Moved safety config table to bmem.c  BMEM_P_Heap now uses a
 * BMEM_P_SafetyConfigInfo pointer to track safety configuration settings
 * instead of tracking each configuration separately.
 *
 * Hydra_Software_Devel/17   6/16/06 3:23p albertl
 * PR20247, PR20276, PR20354:  Added the ability to control debug
 * configuration at runtime.  Added address and offset checking to
 * conversion functions.  BMEM_SetCache can now only be called before
 * heaps are allocated from.  Added BMEM_Heap functions.
 *
 * Hydra_Software_Devel/16   10/7/05 3:53p jasonh
 * PR 17374: Allowed GetHeapInfo to return original creation parameters.
 *
 * Hydra_Software_Devel/15   3/17/05 6:12p pntruong
 * PR14494: Add preliminary software support to 3560 A0, remove linux
 * build warings.
 *
 * Hydra_Software_Devel/14   3/10/05 5:05p albertl
 * PR13677:  Both local and system bookkeeping made available at heap
 * creation though functions BMEM_CreateHeapSystem and
 * BMEM_CreateHeapLocal.
 *
 * Hydra_Software_Devel/13   3/2/05 3:50p jasonh
 * PR 14322: Fixing debug message argument.
 *
 * Hydra_Software_Devel/12   1/31/05 9:54a erickson
 * PR13097: if there's a leak, dump the heap (requires msg level)
 *
 * Hydra_Software_Devel/11   1/21/05 4:23p albertl
 * PR13717:  Replaced BMEM_DBG_ENTER with BDBG_ENTER.  Moved printouts to
 * DBG_MSG, fixed pvHeap.
 *
 * Hydra_Software_Devel/10   11/2/04 5:27p pntruong
 * PR13076: Initialized new members added.
 *
 * Hydra_Software_Devel/9   4/30/04 3:32p hongtaoz
 * PR8761: fixed C++ compile error.
 *
 * Hydra_Software_Devel/8   4/1/04 12:49p vsilyaev
 * PR 10201: Added memory monitor
 *
 * Hydra_Software_Devel/7   12/31/03 12:02p jasonh
 * PR 8940: Changed return type of destroyheap to void. Removed use of
 * DEBUG macro and printf.
 *
 * Hydra_Software_Devel/6   9/15/03 5:24p jasonh
 * Fixed create heap to store offset and address. Fixed return codes.
 * Added system specific destroy heap.
 *
 * Hydra_Software_Devel/5   9/5/03 5:48p hongtaoz
 * fixed compile errors.
 *
 * Hydra_Software_Devel/4   9/5/03 1:39p jasonh
 * Wrapped unused arguments with BSTD_UNUSED
 *
 * Hydra_Software_Devel/3   9/2/03 1:48p vadim
 * Some magnum updates.
 *
 * Hydra_Software_Devel/1   3/20/03 3:24p erickson
 * initial bmem work, taken from SetTop/memorylib
 *
 * SanJose_DVTSW_Devel/5   8/12/02 10:26a poz
 * Changed to kernel interface abstractions for malloc and free.
 *
 * SanJose_DVTSW_Devel\4   4/15/02 6:22p zinc
 * Fixed missed rename of HeapCheck.
 *
 * SanJose_DVTSW_Devel\3   4/15/02 3:34p poz
 * Updated function name to bcmHeapCreate.
 *
 * SanJose_DVTSW_Devel\2   4/15/02 2:30p poz
 * Modified to get semaphore when MEM_REENTRANT.
 *
 * SanJose_DVTSW_Devel\1   4/15/02 1:36p poz
 * Implementation of system-memory heap manager.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bmem.h"
#include "bmem_config.h"
#include "bmem_priv.h"
#include "bmem_debug.h"
#include "bkni.h"

BDBG_MODULE(BMEM);

/* The number of hash buckets to be used to store BlockInfo blocks */
#define BMEM_SYS_NUM_BUCKETS 256 /* must be a power of two */

/* Calculates the hash key given a managed memory address */
#define BUCKET_IDX(addr) (((addr) >> pshi->uiAlignment) & (BMEM_SYS_NUM_BUCKETS-1))

BLST_AA_TREE_GENERATE_INSERT(BMEM_P_AddrTree, BMEM_P_AddrKey*, BMEM_P_BlockInfo, stAddrNode, BMEM_P_Addr_Map_Cmp)
BLST_AA_TREE_GENERATE_FIRST(BMEM_P_AddrTree, BMEM_P_BlockInfo, stAddrNode)
BLST_AA_TREE_GENERATE_INSERT(BMEM_P_SizeTree, BMEM_P_SizeKey*, BMEM_P_BlockInfo, stSizeNode, BMEM_P_Size_Map_Cmp)


/***************************************************************************
 * BMEM_P_BINode - Stores managed block info for the system heap manger.
 *
 * Contains information about the managed block (the BlockInfo) as well as
 * members traversing the system-memory data structures.
 *
 */
typedef struct BMEM_P_BINode BMEM_P_BINode;
struct BMEM_P_BINode
{
	BMEM_P_BlockInfo pbi;   /* Managed block info. */
	                       /* Must be the first entry in this struct! */

	uint32_t addr;         /* Managed memory block base address */
	BMEM_P_BINode *pbiNext; /* Pointer to next node in the hash bucket */
	BMEM_P_BINode *pbiPrev; /* Pointer to previous node in the hash bucket */
};

/***************************************************************************
 * BMEM_P_SystemHeapInfo - Stores heap-wide info for the system heap manager.
 *
 * Heap-wide information specific to the System heap manager.
 *
 */
typedef struct BMEM_P_SystemHeapInfo
{
	unsigned int uiAlignment;
	BMEM_P_BINode *anodes[BMEM_SYS_NUM_BUCKETS];
} BMEM_P_SystemHeapInfo;

/**********************************************************************func*
 * BMEM_P_SystemGetAddress - Get the managed memory base address for a
 *                           block.
 *
 * Returns:
 *    0 if a NULL BlockInfo is given, otherwise the base address.
 */

uint32_t BMEM_P_SystemGetAddress
(
	BMEM_Handle pheap,
	BMEM_P_BlockInfo *pbi
)
{
	BSTD_UNUSED( pheap );

	if( pbi == NULL )
	{
		return 0;
	}

	/* We'll take advantage of BINode having the BlockInfo right at the
	 * front and just magically cast it.
	 */
	return ((BMEM_P_BINode *) pbi)->addr;
}

/**********************************************************************func*
 * BMEM_P_SystemGetBlockInfo - Get the bookkeeping info for a managed base
 *                             address.
 *
 * This function finds the bookeeping information for a given base address.
 * If the given base address has no bookkeeping information assigned to it,
 * a new BlockInfo is allocated from the system heap and added to the hash.
 * The new BlockInfo is then returned.
 *
 * Returns:
 *    On success, a pointer to the BlockInfo for the managed base address.
 *    On failure (which can occur if the system heap is full), NULL.
 *
 */

BMEM_P_BlockInfo *BMEM_P_SystemGetBlockInfo
(
	BMEM_Handle pheap,
	uint32_t addr
)
{
	BMEM_P_SystemHeapInfo *pshi = (BMEM_P_SystemHeapInfo *)(pheap->pvData);
	int idx = BUCKET_IDX(addr);

	BMEM_P_BINode *pbinode = pshi->anodes[idx];
	BMEM_P_BlockInfo *pbi;

	while(pbinode!=NULL && pbinode->addr!=addr)
	{
		pbinode = pbinode->pbiNext;
	}

	if(pbinode == NULL)
	{
		/* No entry found. Make a new one */
		pbinode = (BMEM_P_BINode *)BKNI_Malloc(sizeof(BMEM_P_BINode));
		if(pbinode == NULL)
		{
			BDBG_ERR(("Out of system memory. Can't allocate bookkeeping!"));
			return NULL;
		}
		pbinode->addr = addr;
		pbinode->pbiNext = pshi->anodes[idx];
		pbinode->pbiPrev = NULL;
		pshi->anodes[idx] = pbinode;
		pbinode->pbi.pHeap = pheap;

		if(pbinode->pbiNext!=NULL)
		{
			pbinode->pbiNext->pbiPrev = pbinode;
		}
	}

	/* We'll take advantage of BINode having the BlockInfo right at the
	 * front and just magically cast it.
	 */
	pbi = (BMEM_P_BlockInfo *)pbinode;

	return pbi;
}

/**********************************************************************func*
 * BMEM_P_SystemDropBlockInfo -
 *    Free up the bookkeeping info for a managed memory base address
 *
 * Returns:
 *    void
 */

void BMEM_P_SystemDropBlockInfo
(
	BMEM_Handle pheap,
	BMEM_P_BlockInfo *pbi
)
{
	BMEM_P_SystemHeapInfo *pshi = (BMEM_P_SystemHeapInfo *)pheap->pvData;
	/* We'll take advantage of BINode having the BlockInfo right at the
	 * front and just magically cast it.
	 */
	BMEM_P_BINode *pbinode = (BMEM_P_BINode *) pbi;
	int idx = BUCKET_IDX(pbinode->addr);

	/* Remove the BINode from the list in the hash table. */
	if(pbinode->pbiPrev != NULL)
	{
		pbinode->pbiPrev->pbiNext = pbinode->pbiNext;
	}
	if(pbinode->pbiNext != NULL)
	{
		pbinode->pbiNext->pbiPrev = pbinode->pbiPrev;
	}

	if(pshi->anodes[idx] == pbinode)
	{
		pshi->anodes[idx] = pbinode->pbiNext;
	}

	BKNI_Free(pbinode);
}

/**********************************************************************func*
 * bcmHeapCreate - Initializes the heap with local bookkeeping.
 *
 * This function inititalizes a heap at a given location and size.
 * Any previous allocations in the chunk of memory handed over to this
 * function are lost. Every heap has a base minimum alignment for all of
 * the allocations within that heap. (However, you can specify a greater
 * alignment when actually doing an allocation.)
 *
 * In this implementation, memory in the CPU's system heap is used to store
 * the bookkeeping information for the heap.
 *
 * Returns:
 *   Returns true if heap is initialized, or false if the size of the heap
 *   is too small to manage or there isn't enough system memory to allocate
 *   bookkeeping information.
 *
 */
BERR_Code BMEM_CreateHeapSystem
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
	pHeapSettings->eBookKeeping = BMEM_BOOKKEEPING_SYSTEM;

	return BMEM_Heap_Create(NULL, pvHeap, ulOffset, zSize, pHeapSettings, ppHeap);
}

BERR_Code BMEM_P_SystemCreateHeap
(
	BMEM_Handle        *ppHeap,      /* Address to store pointer of new heap info structure */
	void               *pvHeap,      /* Pointer to beginning of memory chunk to manage */
	uint32_t            ulOffset,    /* Device offset of initial location */
	size_t              zSize,       /* Size of chunk to manage in bytes */
	BMEM_Heap_Settings *pHeapSettings
)
{
	BERR_Code              err = BERR_SUCCESS;
	BMEM_P_SystemHeapInfo *pshi;
	BMEM_P_BlockInfo      *pbi;
	BMEM_P_Heap           *pheap;
	uint32_t               addrHeap = (uint32_t)pvHeap;
	uint32_t               addrEnd = (uint32_t)pvHeap + zSize - 1;
	int                    i;
	unsigned int           uiAlignment = pHeapSettings->uiAlignment;
	size_t                 ulAlignedSize;
	uint32_t               eSafetyConfig = pHeapSettings->eSafetyConfig;
	BMEM_P_AddrKey         stAddrKey;
	BMEM_P_SizeKey         stSizeKey;

	BDBG_ENTER(BMEM_P_SystemCreateHeap);
	BDBG_MSG(("BMEM_P_SystemCreateHeap(%p, %p, %lu, %d)",
		ppHeap, pvHeap, zSize, (int)uiAlignment));

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

	/* allocate heap structure */
	*ppHeap = (BMEM_P_Heap*) BKNI_Malloc( sizeof (BMEM_P_Heap) );
	if( *ppHeap == NULL )
	{
		err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(( "Out of system memory. Can't allocate bookkeeping!" ));
		goto done;
	}

	/* initialize heap structure */
	pheap = (BMEM_P_Heap *) *ppHeap;
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

	pheap->pGetAddressFunc = &BMEM_P_SystemGetAddress;
	pheap->pGetBlockInfoFunc = &BMEM_P_SystemGetBlockInfo;
	pheap->pDropBlockInfoFunc = &BMEM_P_SystemDropBlockInfo;
	pheap->pDestroyHeapFunc = &BMEM_P_SystemDestroyHeap;

	BLST_AA_TREE_INIT(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree);
	BLST_AA_TREE_INIT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
	BLST_AA_TREE_INIT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree);

#if (BMEM_REENTRANT_CONFIG==BMEM_REENTRANT)
	/* create mutex for re-entrant control */
	err = BERR_TRACE(BKNI_CreateMutex(&(pheap->pMutex)));
	if (err != BERR_SUCCESS)
	{
		goto done;
	}
#endif


	/* Force at least the minimum heap alignment */
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

	/* Allocate the hash buckets for storing the BlockInfos */
	pheap->pvData = BKNI_Malloc(sizeof(BMEM_P_SystemHeapInfo));
	if(pheap->pvData == NULL)
	{
		/* unable to allocate */
		err = BERR_OUT_OF_SYSTEM_MEMORY;
		BDBG_ERR(("Out of system memory. Can't allocate hash table!"));
		goto done;
	}
	pshi = (BMEM_P_SystemHeapInfo *)pheap->pvData;

	/* initialize table */
	for(i = 0; i<BMEM_SYS_NUM_BUCKETS; ++i)
	{
		pshi->anodes[i] = NULL;
	}
	pshi->uiAlignment = uiAlignment;

	/*
	 * Starting here, it's safe to make calls to the other heap management
	 * functions.
	 */

	/* Make the first block, which represents the remaining (free) heap */
	pbi = BMEM_P_SystemGetBlockInfo(pheap, (uint32_t)pheap->pStart);
	
	ulAlignedSize = pheap->pEnd-pheap->pStart+1;

	/* heap size greater than bookkeeping? */
	if(ulAlignedSize > BMEM_BOOKKEEPING_SIZE_SYSTEM)
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
	/* memory isn't even large enough for bookkeeping */
	{
		/* cannot create heap from this memory */
		err = BERR_INVALID_PARAMETER;
	}

#if BMEM_CHECK_ALL_GUARDS
	if (pheap->pSafetyConfigInfo->bCheckAllGuards)
	{
		/* heap has been created? */
		if (err == BERR_SUCCESS)
		{
			/* validate heap */
			err = BMEM_ValidateHeap(pheap);
		}
	}
#endif

done:
	/* free heap if error occured */
	if ((err != BERR_SUCCESS) && *ppHeap!=NULL)
	{
		/* destroy heap */
		BMEM_DestroyHeap(*ppHeap);
		*ppHeap = NULL;
	}

	/* return status */
	BDBG_MSG(("BMEM_P_SystemCreateHeap = %ul", err));
	BDBG_LEAVE(BMEM_P_SystemCreateHeap);
	return err;
}

/***************************************************************************/
void BMEM_P_SystemDestroyHeap
(
	BMEM_Handle pheap
)
{
#if BMEM_CHECK_ALL_GUARDS
	BERR_Code err;
#endif
	BMEM_P_SystemHeapInfo *pshi;
	unsigned i;

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
		BMEM_Dbg_DumpHeap(pheap);
	}

	/* free system hash table (should be empty since there
	   are no outstanding allocations) */
	pshi = (BMEM_P_SystemHeapInfo *)(pheap->pvData);
	for (i=0;i<BMEM_SYS_NUM_BUCKETS;i++) {
		BMEM_P_BINode *pbi = pshi->anodes[i];
		BMEM_P_BINode *pbiNext;
		while(pbi) {
			pbiNext = pbi->pbiNext;
			BKNI_Free(pbi);
			pbi = pbiNext;
		}
	}

	BKNI_Free(pheap->pvData);

#if (BMEM_REENTRANT_CONFIG==BMEM_REENTRANT)
	/* destroy mutex */
	BKNI_DestroyMutex(pheap->pMutex);
#endif

	/* free heap handle */
	BDBG_OBJECT_UNSET(pheap, BMEM_Heap);
	BKNI_Free(pheap);

done:
	/* return */
	return;
}

/* End of File */
