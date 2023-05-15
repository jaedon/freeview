/***************************************************************************
 *     Copyright (c) 2001-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmem.c $
 * $brcm_Revision: Hydra_Software_Devel/SW7435-433/2 $
 * $brcm_Date: 10/29/12 3:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/mem/bmem.c $
 * 
 * Hydra_Software_Devel/SW7435-433/2   10/29/12 3:05p mward
 * SW7435-442: Back out changes from version 92.
 * 
 * Hydra_Software_Devel/SW7435-433/1   10/26/12 7:38p albertl
 * SW7435-442: Disable error on non-cached addresses for now.
 * 
 * Hydra_Software_Devel/91   10/23/12 6:36p albertl
 * SW7435-442: Fixed BMEM_P_Heap_FindCacheHeap and returns error when
 * BMEM_Heap_FlushCache doesn't actually flush cache.
 * 
 * Hydra_Software_Devel/90   10/12/12 10:17a albertl
 * SW7435-255: Fixed crash bug with pbiBack.
 * 
 * Hydra_Software_Devel/89   10/11/12 7:42p albertl
 * SW7435-255: Fixed accounting errors in free and alloc tracking.
 * 
 * Hydra_Software_Devel/88   9/25/12 5:15p albertl
 * SW7435-255: Turned off loop checking by default.
 * 
 * Hydra_Software_Devel/87   9/25/12 3:10p albertl
 * SW7435-255: Disabled local bookkeeping mode and set default to system
 * bookkeeping.
 * 
 * Hydra_Software_Devel/86   9/21/12 6:46p albertl
 * SW7435-255: Optimized BMEM with AA Tree implementation.
 * 
 * Hydra_Software_Devel/84   7/24/12 11:21a albertl
 * SW7552-303: Set default heap alignment to 9 for safety.  Removed
 * checking for 0 alignment, now handled by minimum checking code.
 * 
 * Hydra_Software_Devel/83   7/23/12 5:13p albertl
 * SW7552-303: Removed usage of BMEM_HEAP_ALIGNMENT for cache alignment
 * issues.  Default alignment is now 0 to force user to set a valid
 * alignment.
 * 
 * Hydra_Software_Devel/82   4/23/12 4:18p albertl
 * SW7231-660: Fixing coverity issue with semaphore locks.
 * 
 * Hydra_Software_Devel/81   2/7/12 6:36p albertl
 * SW7425-2345: Changed total allocation and number of allocation tracking
 * to work in eFastest.
 * 
 * Hydra_Software_Devel/80   1/18/12 6:00p albertl
 * SW7346-637: Fixed bug in free block size checking.  Changed to use base
 * address offset instead of front address offset.
 * 
 * Hydra_Software_Devel/79   1/17/12 7:15p albertl
 * SW7346-637: Fixed to properly check if aligned addresses run outside of
 * selected block.
 * 
 * Hydra_Software_Devel/78   1/12/12 4:13p albertl
 * SW7346-637: Fixed allocation to use larger of heap and requested
 * alignment when requesting free block.
 * 
 * Hydra_Software_Devel/77   11/30/11 4:25p albertl
 * SW7425-1857: Fixed unchecked BMEM_Heap_ConvertAddressToCached returns
 * and bIsCached checking.
 * 
 * Hydra_Software_Devel/76   11/14/11 3:16p albertl
 * SW7425-1627: Fixed addrBase calculation in
 * GetLargestAvailableBlockSize.
 * 
 * Hydra_Software_Devel/75   11/14/11 2:21p albertl
 * SW7425-1293: Fixed use of uncached address when cached addresses exist
 * during heap creation.  Fixed coverity issues.
 * 
 * Hydra_Software_Devel/74   11/11/11 4:00p albertl
 * SW7425-1627: Changed GetLargestAvailableBlockSize to take minimum
 * alignment into account.
 * 
 * Hydra_Software_Devel/73   10/25/11 4:00p albertl
 * SW7425-1293: Fixed type punned warnings.
 * 
 * Hydra_Software_Devel/72   10/19/11 7:03p albertl
 * SW7425-1293: Fixed cached guardband checking and filling with proper
 * flushing.
 * 
 * Hydra_Software_Devel/71   10/7/11 11:26p hongtaoz
 * SW7425-1293: rolled back the previous change;
 *
 * Hydra_Software_Devel/70   10/7/11 2:32p albertl
 * SW7425-1293: Changed guardband checking and fill to use cached memory
 * addresses when available.
 * 
 * Hydra_Software_Devel/69   9/9/11 7:10p albertl
 * SW7346-201: Moved watermark calculation to alloc and free for accurate
 * calculation.
 * 
 * Hydra_Software_Devel/68   9/8/11 4:22p albertl
 * SW7405-5350: Changed block info to be doublely linked.  Implemented
 * faster pbi removal and assigned slower removal and error checking to
 * BMEM_FREE_CAREFULLY.
 * 
 * Hydra_Software_Devel/67   9/7/11 2:39p albertl
 * SW7346-201: Changed watermark to not require allocation tracking.
 * 
 * Hydra_Software_Devel/66   8/24/11 5:35p albertl
 * SW7405-5349:  Added line parameter to memory monitor interface.
 * 
 * Hydra_Software_Devel/65   4/13/11 12:05p albertl
 * SW7425-333: Added smart detection of cached address to
 * BMEM_Heap_ConvertAddress_ToOffset.
 * 
 * Hydra_Software_Devel/64   4/5/11 1:29p erickson
 * SW7420-1734: remove incorrect enter/leave critical section in
 * BMEM_Close
 * 
 * Hydra_Software_Devel/63   3/28/11 5:12p albertl
 * SW7425-247: Incorporated BDBG_OBJECT handle validation.
 * 
 * Hydra_Software_Devel/62   3/7/11 5:05p albertl
 * SW7425-131: Allow BMEM_Heap_FlushCache to work for memory allocated on
 * any heap.
 * 
 * Hydra_Software_Devel/61   1/10/11 3:31p albertl
 * SW7408-193: BMEM_Heapinfo now includes high watermark.  Added
 * BMEM_Heap_ResetHighWatermark().
 * 
 * Hydra_Software_Devel/60   4/28/10 6:48p albertl
 * SW7405-4264: Fixed build warning.
 * 
 * Hydra_Software_Devel/59   4/15/10 3:26p albertl
 * SW7550-370: Fixed alignment to occur on physical addresses rather than
 * virtual addresses.
 * 
 * Hydra_Software_Devel/58   3/28/10 4:14p albertl
 * SW7405-3979: Changed to assign filename pointer correctly.
 * 
 * Hydra_Software_Devel/57   3/26/10 5:14p albertl
 * SW7405-3979: Changed filename field from string to pointer and removed
 * slow copy operation.
 * 
 * Hydra_Software_Devel/56   2/26/10 2:14p erickson
 * SW7325-655: BMEM_Heap_FreeCached should only flush user accessible
 * memory. the scrap and bookkeeping is never access via cached memory.
 *
 * Hydra_Software_Devel/55   2/26/10 1:44p erickson
 * SW7325-655: fix compilation error
 *
 * Hydra_Software_Devel/54   2/26/10 1:40p albertl
 * SW7325-655: Added BMEM_Heap_FreeCached.
 *
 * Hydra_Software_Devel/53   2/12/10 10:38a erickson
 * SW7405-3877: make BMEM_AllocAligned BDBG_MSG more useful to users
 *
 * Hydra_Software_Devel/52   5/26/09 4:17p albertl
 * PR55389: Replaced uintptr_t with uint32_t.
 *
 * Hydra_Software_Devel/51   5/11/09 2:18p pntruong
 * PR53840: [Core2.1/USB] USB Connect (Phase1) support in HPK driver for
 * OTV Core2 v2.1.
 *
 * Hydra_Software_Devel/50   3/23/09 7:38p albertl
 * PR41512: Fixed build warnings.
 *
 * Hydra_Software_Devel/49   3/23/09 6:37p albertl
 * PR41512: Optimized guard band checking.
 *
 * Hydra_Software_Devel/48   2/25/09 4:48p erickson
 * PR52471: added const keyword to global data
 *
 * Hydra_Software_Devel/47   12/3/08 5:16p albertl
 * PR48035: Added ulTotalFree and ulTotalAllocated to BMEM_HeapInfo.
 *
 * Hydra_Software_Devel/46   11/3/08 4:58p albertl
 * PR48488:  Changed default alignment to BMEM_HEAP_ALIGNMENT.
 *
 * Hydra_Software_Devel/45   10/24/08 5:45p albertl
 * PR48199, PR48046: Removed BMEM_SetCache entirely as it is obsolete.
 *
 * Hydra_Software_Devel/45   10/24/08 5:43p albertl
 * PR48199, PR48046: Removed BMEM_SetCache entirely as it is obsolete.
 *
 * Hydra_Software_Devel/44   10/24/08 4:40p albertl
 * PR48199: Fixed coverity error with unreachable BDBG_ASSERT.
 *
 * Hydra_Software_Devel/43   7/28/08 4:23p tdo
 * PR45187: Fix compiling error for BMEM_SAFETY_CONFIG=BMEM_CONFIG_SAFEST
 * flag
 *
 * Hydra_Software_Devel/42   4/8/08 7:19p albertl
 * PR40246:  Changed to assert if pheap parameters are null.
 *
 * Hydra_Software_Devel/41   6/8/07 5:19p darnstein
 * PR31999: Add a test for a certain failure in BMEM_Heap_Create().
 *
 * Hydra_Software_Devel/40   5/14/07 1:19p albertl
 * PR30621:  Fixed heaps being added to allocation list but not being
 * removed when freed in some safety configurations.
 *
 * Hydra_Software_Devel/39   3/19/07 2:26p erickson
 * PR28682: fix compilation error because of macro change, also removed
 * TRACE-like MSG's
 *
 * Hydra_Software_Devel/38   2/1/07 3:45p jgarrett
 * PR 20139: Removing release-mode warning
 *
 * Hydra_Software_Devel/37   1/24/07 8:05p albertl
 * PR27214:  Fixed compile worning with BMEM_P_PrintBlock.
 *
 * Hydra_Software_Devel/36   1/16/07 2:41p erickson
 * PR25037: improve BMEM validate and debug messages
 *
 * Hydra_Software_Devel/35   10/12/06 6:54p albertl
 * PR20247:  Fixed all heap safety config field checks to use new
 * pSafetyConfigInfo structure.
 *
 * Hydra_Software_Devel/34   6/21/06 3:10p albertl
 * PR20247:  Moved safety config table to bmem.c  BMEM_P_Heap now uses a
 * BMEM_P_SafetyConfigInfo pointer to track safety configuration settings
 * instead of tracking each configuration separately.
 *
 * Hydra_Software_Devel/33   6/20/06 6:05p albertl
 * PR20247:  hMem field in heap updated even when NULL.
 *
 * Hydra_Software_Devel/32   6/16/06 3:21p albertl
 * PR20247, PR20276, PR20354:  Added the ability to control debug
 * configuration at runtime.  Added address and offset checking to
 * conversion functions.  BMEM_SetCache can now only be called before
 * heaps are allocated from.  Added BMEM_Heap functions.
 *
 * Hydra_Software_Devel/31   2/8/06 7:29p hongtaoz
 * PR19082: BMEM_Alloc returns NULL pointer if size is zero;
 *
 * Hydra_Software_Devel/30   10/7/05 3:53p jasonh
 * PR 17374: Allowed GetHeapInfo to return original creation parameters.
 *
 * Hydra_Software_Devel/29   8/10/05 11:54a hongtaoz
 * PR13076: BMEM_SetCache returns error if NULL callbacks are provided;
 *
 * Hydra_Software_Devel/28   8/1/05 5:40p albertl
 * PR16375:  BMEM_Heap_Validate now returns the errorcode corresponding to
 * the first error encountered..
 *
 * Hydra_Software_Devel/27   4/28/05 5:07p jasonh
 * PR 15059: Fixed problems associated with freeing blocks (rare corner
 * case).
 *
 * Hydra_Software_Devel/26   3/10/05 5:04p albertl
 * PR13677:  Both local and system bookkeeping made available at heap
 * creation though functions BMEM_CreateHeapSystem and
 * BMEM_CreateHeapLocal.
 *
 * Hydra_Software_Devel/25   1/21/05 4:56p albertl
 * PR13717:  Replaced BMEM_DBG_ENTER with BDBG_ENTER.  Moved printouts to
 * BDBG_MSG, fixed pvHeap.
 *
 * Hydra_Software_Devel/24   12/14/04 4:31p marcusk
 * PR13562: Updated to output the heap size, allocated size, and free size
 * when a memory allocation fails.
 *
 * Hydra_Software_Devel/23   11/3/04 10:45a pntruong
 * PR13076: Call the correct flush for _isr function.
 *
 * Hydra_Software_Devel/22   11/2/04 5:27p pntruong
 * PR13076: Fixed re-entry problem.
 *
 * Hydra_Software_Devel/21   11/2/04 10:32a hongtaoz
 * PR13076: added cached memory support;
 *
 * Hydra_Software_Devel/20   5/27/04 7:40p hongtaoz
 * PR10059: fixed a minor merge sort bug;
 *
 * Hydra_Software_Devel/19   5/27/04 2:17p hongtaoz
 * PR10059: fixed free clear size error;
 *
 * Hydra_Software_Devel/18   5/26/04 7:01p hongtaoz
 * PR10059: sorted freed heap in size ascending order to avoid memory
 * fragamentation;
 *
 * Hydra_Software_Devel/17   4/1/04 12:48p vsilyaev
 * PR 10201: Added memory monitor
 *
 * Hydra_Software_Devel/16   2/25/04 2:52p hongtaoz
 * PR9855: removed redundent functions BMEM_Report and BMEM_ReportVerbose;
 * being aware of that BMEM_Dbg_DumpBlock and BMEM_Dbg_DumpHeap suffice
 * heap debug requirement; avoid using floating point in BMEM_Dbg_Map;
 *
 * Hydra_Software_Devel/15   12/31/03 12:02p jasonh
 * PR 8940: Changed return type of destroyheap to void. Removed use of
 * DEBUG macro and printf.
 *
 * Hydra_Software_Devel/14   11/11/03 11:17a jasonh
 * PR 8589: Fixed offset to address conversion.
 *
 * Hydra_Software_Devel/13   10/31/03 9:52a jasonh
 * Removed side-effects of assert statement.
 *
 * Hydra_Software_Devel/12   9/15/03 5:18p jasonh
 * Fixed semaphore assert check.
 *
 * Hydra_Software_Devel/11   9/15/03 5:02p jasonh
 * Moved destroy heap to local and system specific files. Added asserts
 * around semaphore access routines. Fixed get free block to allow block
 * of exact size to be allocated. Added implementation of conversion
 * functions.
 *
 * Hydra_Software_Devel/10   9/5/03 3:40p jasonh
 * Fixed guardband check.
 *
 * Hydra_Software_Devel/9   9/5/03 2:03p jasonh
 * Added documentation. Re-named Check to ValidateHeap. Removed
 * implementation of Alloc and AllocAligned (they are now macros). Added
 * stubs for convert and report functions.
 *
 * Hydra_Software_Devel/8   9/4/03 7:15p jasonh
 * Added basic implementations of alloc and allocaligned routines.
 *
 * Hydra_Software_Devel/7   9/4/03 6:44p jasonh
 * Added debug module tag.
 *
 * Hydra_Software_Devel/6   9/2/03 1:53p vadim
 * Some magnum updates.  DestroyHeap is currently a stub.
 *
 * Hydra_Software_Devel/2   3/20/03 3:51p erickson
 * renamed all MEM_ to BMEM_
 *
 * Hydra_Software_Devel/1   3/20/03 3:24p erickson
 * initial bmem work, taken from SetTop/memorylib
 *
 * SanJose_DVTSW_Devel\9   4/16/02 5:49p jasonh
 * Warning police.
 *
 * SanJose_DVTSW_Devel\8   4/15/02 3:34p poz
 * Updated public functions to new names.
 *
 * SanJose_DVTSW_Devel\7   4/15/02 3:13p ngibbs
 * Fixed semaphore-related oops introduced during merge.
 *
 * SanJose_DVTSW_Devel\6   4/15/02 2:37p jasonh
 * Whoops. A number of minor compile errors fixed.
 *
 * SanJose_DVTSW_Devel\5   4/15/02 2:28p poz
 * Modifications to allow bookkeeping to exist non-local to the data.
 * Tweaks to reentrancy stuff.
 * Updated comments, copyright, and keywords in header.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "berr_ids.h"
#include "bmem.h"
#include "bmem_config.h"
#include "bmem_priv.h"
#include "bmem_debug.h"

BDBG_MODULE(BMEM);

BDBG_OBJECT_ID(BMEM);
BDBG_OBJECT_ID(BMEM_Heap);

#define BMEM_P_GUARD_DWORD ((uint32_t)(BMEM_GUARD_BYTE << 24 | BMEM_GUARD_BYTE << 16 | BMEM_GUARD_BYTE << 8 | BMEM_GUARD_BYTE))

static void BMEM_P_AddUsed(BMEM_Heap_Handle pheap, BMEM_P_BlockInfo *pbi);
static void BMEM_P_AddFree(BMEM_Heap_Handle pheap, BMEM_P_BlockInfo *pbi);
static BMEM_P_BlockInfo *BMEM_P_GetFreeBlock(BMEM_Heap_Handle pheap,
	size_t ulSize, uint32_t ulAlignMask);

/* Safety Mode configuration table */
const BMEM_P_SafetyConfigInfo BMEM_P_SafetyConfigTbl[] =
{  /* eSafetyConfig,      bTrackAllocations,              bFreeClear,              bFreeCarefully,              iGuardSize,              bCheckGuardOnFree                 bCheckAllGuards,               bTrackFileAndLine  */
	{BMEM_CONFIG_FASTEST, BMEM_TRACK_ALLOCATIONS_FASTEST, BMEM_FREE_CLEAR_FASTEST, BMEM_FREE_CAREFULLY_FASTEST, BMEM_GUARD_SIZE_FASTEST, BMEM_CHECK_GUARD_ON_FREE_FASTEST, BMEM_CHECK_ALL_GUARDS_FASTEST, BMEM_TRACK_FILE_AND_LINE_FASTEST},
	{BMEM_CONFIG_NORMAL,  BMEM_TRACK_ALLOCATIONS_NORMAL,  BMEM_FREE_CLEAR_NORMAL,  BMEM_FREE_CAREFULLY_NORMAL,  BMEM_GUARD_SIZE_NORMAL,  BMEM_CHECK_GUARD_ON_FREE_NORMAL,  BMEM_CHECK_ALL_GUARDS_NORMAL,  BMEM_TRACK_FILE_AND_LINE_NORMAL},
	{BMEM_CONFIG_TRACK,   BMEM_TRACK_ALLOCATIONS_TRACK,   BMEM_FREE_CLEAR_TRACK,   BMEM_FREE_CAREFULLY_TRACK,   BMEM_GUARD_SIZE_TRACK,   BMEM_CHECK_GUARD_ON_FREE_TRACK,   BMEM_CHECK_ALL_GUARDS_TRACK,   BMEM_TRACK_FILE_AND_LINE_TRACK},
	{BMEM_CONFIG_SAFE,    BMEM_TRACK_ALLOCATIONS_SAFE,    BMEM_FREE_CLEAR_SAFE,    BMEM_FREE_CAREFULLY_SAFE,    BMEM_GUARD_SIZE_SAFE,    BMEM_CHECK_GUARD_ON_FREE_SAFE,    BMEM_CHECK_ALL_GUARDS_SAFE,    BMEM_TRACK_FILE_AND_LINE_SAFE},
	{BMEM_CONFIG_SAFEST,  BMEM_TRACK_ALLOCATIONS_SAFEST,  BMEM_FREE_CLEAR_SAFEST,  BMEM_FREE_CAREFULLY_SAFEST,  BMEM_GUARD_SIZE_SAFEST,  BMEM_CHECK_GUARD_ON_FREE_SAFEST,  BMEM_CHECK_ALL_GUARDS_SAFEST,  BMEM_TRACK_FILE_AND_LINE_SAFEST}
};

const uint32_t BMEM_P_SafetyTableSize = (sizeof(BMEM_P_SafetyConfigTbl) / sizeof(BMEM_P_SafetyConfigInfo));

static const BMEM_Heap_Settings s_stDefaultHeapSettings =
{
	9,                         /* uiAlignment */
	BMEM_SAFETY_CONFIG,        /* eSafetyConfig */
	BMEM_BOOKKEEPING_CONFIG,   /* eBookKeeping */
	0,                         /* pCachedAddress */
	NULL,                      /* flush */
	NULL                       /* flush_isr */
};

const char s_achFilenameFreeString[] = "(free)";

int BMEM_P_Addr_Map_Cmp(const BMEM_P_BlockInfo *pbi, const BMEM_P_AddrKey *pKey)
{
	uint32_t ulAddrPbi = BMEM_P_GetAddress(pbi->pHeap, (BMEM_P_BlockInfo *)pbi);
	BDBG_ASSERT(pbi->pHeap);

    if (pKey->ulAddr > ulAddrPbi)
	{
        return 1;
    }
	else if (pKey->ulAddr < ulAddrPbi)
	{
        return -1;
    }
	else
	{
        return 0;
	}
}

int BMEM_P_Size_Map_Cmp(const BMEM_P_BlockInfo *pbi, const BMEM_P_SizeKey *pKey)
{
	uint32_t ulAddrPbi = BMEM_P_GetAddress(pbi->pHeap, (BMEM_P_BlockInfo *)pbi);
	BDBG_ASSERT(pbi->pHeap);

	if (pKey->ulSize > pbi->ulSize)
	{
		return 1;
	}
	else if (pKey->ulSize < pbi->ulSize)
	{
		return -1;
	}
	else if (pKey->ulAddr > ulAddrPbi)
	{
		return 1;
	}
	else if (pKey->ulAddr < ulAddrPbi)
	{
		return -1;
	}
	/* ulSize and ullAddr both equal */
	else
	{
		return 0;
	}
}


BLST_AA_TREE_GENERATE_FIND(BMEM_P_AddrTree, BMEM_P_AddrKey*, BMEM_P_BlockInfo, stAddrNode, BMEM_P_Addr_Map_Cmp)
BLST_AA_TREE_GENERATE_FIND_SOME(BMEM_P_AddrTree, BMEM_P_AddrKey*, BMEM_P_BlockInfo, stAddrNode, BMEM_P_Addr_Map_Cmp)
BLST_AA_TREE_GENERATE_INSERT(BMEM_P_AddrTree, BMEM_P_AddrKey*, BMEM_P_BlockInfo, stAddrNode, BMEM_P_Addr_Map_Cmp)
BLST_AA_TREE_GENERATE_REMOVE(BMEM_P_AddrTree, BMEM_P_BlockInfo, stAddrNode)
BLST_AA_TREE_GENERATE_FIRST(BMEM_P_AddrTree, BMEM_P_BlockInfo, stAddrNode)
BLST_AA_TREE_GENERATE_NEXT(BMEM_P_AddrTree, BMEM_P_BlockInfo, stAddrNode)
BLST_AA_TREE_GENERATE_PREV(BMEM_P_AddrTree, BMEM_P_BlockInfo, stAddrNode)

BLST_AA_TREE_GENERATE_FIND_SOME(BMEM_P_SizeTree, BMEM_P_SizeKey*, BMEM_P_BlockInfo, stSizeNode, BMEM_P_Size_Map_Cmp)
BLST_AA_TREE_GENERATE_INSERT(BMEM_P_SizeTree, BMEM_P_SizeKey*, BMEM_P_BlockInfo, stSizeNode, BMEM_P_Size_Map_Cmp)
BLST_AA_TREE_GENERATE_REMOVE(BMEM_P_SizeTree, BMEM_P_BlockInfo, stSizeNode)
BLST_AA_TREE_GENERATE_FIRST(BMEM_P_SizeTree, BMEM_P_BlockInfo, stSizeNode)
BLST_AA_TREE_GENERATE_LAST(BMEM_P_SizeTree, BMEM_P_BlockInfo, stSizeNode)
BLST_AA_TREE_GENERATE_NEXT(BMEM_P_SizeTree, BMEM_P_BlockInfo, stSizeNode)


/***************************************************************************/
BERR_Code BMEM_GetDefaultSettings
(
	BMEM_Settings *pDefSettings
)
{
	*pDefSettings = NULL;
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BMEM_Open
(
    BMEM_ModuleHandle   *phMem,
    const BMEM_Settings *pDefSettings
)
{
	BMEM_ModuleHandle hMem;

	BSTD_UNUSED(pDefSettings);

	if (!phMem)
	{
		return BERR_INVALID_PARAMETER;
	}

	hMem = (BMEM_ModuleHandle) BKNI_Malloc( sizeof( BMEM_P_Mem ) );
	if (!hMem) {
    	return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BDBG_OBJECT_SET(hMem, BMEM);
	BLST_S_INIT(&(hMem->HeapList));

	*phMem = hMem;

	return BERR_SUCCESS;
}

/***************************************************************************/
void BMEM_Close
(
    BMEM_ModuleHandle hMem
)
{
	BMEM_P_Heap *pHeap = NULL;

	BDBG_OBJECT_ASSERT(hMem, BMEM);

	for(pHeap = BLST_S_FIRST(&(hMem->HeapList)); pHeap ; pHeap = BLST_S_FIRST(&(hMem->HeapList)))
	{
		BMEM_Heap_Destroy(pHeap);
	}
	BDBG_OBJECT_DESTROY(hMem, BMEM);
	BKNI_Free( hMem );
}

/***************************************************************************/
void *BMEM_P_Heap_TagAllocAligned(
	BMEM_Heap_Handle pheap,       /* Heap to allocate from */
	size_t           ulSize,      /* size in bytes of block to allocate */
	unsigned int     ucAlignBits, /* alignment for the block */
	unsigned int     Boundary,    /* boundry restricting allocated value */
	const char*      pchFilename, /* source filename where block is allocated from */
	int              iLine )      /* line number in file where allocation occurs */
{
	BERR_Code err;
	void *pvRet = NULL;
	BMEM_P_BlockInfo *pbiNew = NULL;
	BMEM_P_BlockInfo *pbi;

	uint32_t addrBase;
	uint32_t addrFinal;

	uint32_t addrFront;
	uint32_t addrFrontAligned;
	uint32_t addrFrontOffset = 0;
	uint32_t addrFrontOffsetAligned =0;
	size_t ulWasteFront;

	uint32_t addrMiddle;

	uint32_t addrBack;
	uint32_t addrBackAligned;
	uint32_t addrBackOffset = 0;
	uint32_t addrBackOffsetAligned = 0;
	size_t ulWasteBack;

	size_t ulOrigSize;

	uint32_t ulAlignMask;

	uint32_t ulBookKeepingSize;
	uint32_t ulFrontBookKeepingSize;
	uint32_t ulMinBlockSize;

	/* TODO: Implement Boundary */
	BSTD_UNUSED(Boundary);
#if !(BMEM_TRACK_FILE_AND_LINE)
	BSTD_UNUSED(pchFilename);
	BSTD_UNUSED(iLine);
#endif

    BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);
	BDBG_ENTER(BMEM_P_Heap_TagAllocAligned);

	ulBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                             BMEM_BOOKKEEPING_SIZE_LOCAL : BMEM_BOOKKEEPING_SIZE_SYSTEM;
	ulFrontBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                                  BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL : BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM;
	ulMinBlockSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                          BMEM_MIN_BLOCK_SIZE_LOCAL : BMEM_MIN_BLOCK_SIZE_SYSTEM;

#if 0
	/* this MSG before the allocation is useful for internal debug */
	BDBG_MSG(("BMEM_Heap_TagAlloc(%p (%p), %lu, %u, %s, %d)",
		(pheap ? pheap->pvHeap : NULL), pheap, ulSize, (int)ucAlignBits, pchFilename, iLine));
#endif

	if(ulSize == 0) return NULL;

	ulAlignMask = ((uint32_t) 1 << ucAlignBits) - 1;
	/* Force the heap's alignment at minimum */
	ulAlignMask |= pheap->ulAlignMask;

	/* The size of the block must be at the heap's alignment also */
	ulSize = (ulSize+pheap->ulAlignMask) & ~pheap->ulAlignMask;

#if BMEM_CHECK_ALL_GUARDS
	if(pheap->pSafetyConfigInfo->bCheckAllGuards)
	{
		BMEM_Heap_Validate(pheap);
	}
#endif

	/* get semaphore */
	err = BMEM_P_GET_SEMAPHORE(pheap);
	BDBG_ASSERT(err == BERR_SUCCESS);

	/* get a free block that can contain the requested size */
	if((pbi = BMEM_P_GetFreeBlock(pheap, ulSize, ulAlignMask)) == NULL)
	{
		/* unable to find a free block of the needed size. */
        BDBG_WRN(("Unable to allocate requested memory of size %ld by %s:%d", ulSize, pchFilename, iLine));
        BMEM_Dbg_DumpHeap(pheap);
		goto done;
	}

	/*
	 * TODO: I get an itching feeling that this code can be combined with
	 * the code later on which determines what memory can be given back
	 * to the free heap. It seems like I'm doing the same calculation
	 * twice: once to figure out the theoretical waste, and once for the
	 * actual waste.
	 *
	 * Anyway, they are still separate since it makes logical sense to do
	 * things in this order (determine options, decide which one, do the
	 * actual allocation). I haven't had the time to optimize (which I
	 * believe means simplify, in this case) the code.
	 *
	 * One obvious method for simplification would be to always allocate
	 * from the front (or back) of the free block. I decided that
	 * maximizing heap usage (i.e. minimizing fragmentation) was a better
	 * choice for now.
	 */

	/* Free blocks never have scrap, so we don't need to worry about that */

	/*
	 * There are two places to try to place the new allocation,
	 * at the front of the block and at the end.
	 *
	 * The wastage for each (due to alignment) will be calculated and
	 * the one with the minimum waste used.
	 */

	ulOrigSize = pbi->ulSize;
	addrBase = BMEM_P_GetAddress(pheap, pbi);

	/* First the front */
	addrFront = addrBase + ulFrontBookKeepingSize;

	/* Alignment should be calculated on physical address */
	/* Push it forward to the next alignment */
	if(BMEM_Heap_ConvertAddressToOffset(pheap, (void *)addrFront, &addrFrontOffset) != BERR_SUCCESS)
	{
		BDBG_ERR(("Invalid address being converted to offset."));
		goto done;
	}
	addrFrontOffsetAligned = (addrFrontOffset + ulAlignMask) & ~ulAlignMask;
	if(BMEM_Heap_ConvertOffsetToAddress(pheap, addrFrontOffsetAligned, (void *)&addrFrontAligned) != BERR_SUCCESS)
	{
		BDBG_ERR(("Invalid offset being converted to address."));
		goto done;
	}

	/* Calculate the waste */
	ulWasteFront = addrFrontOffsetAligned - addrFrontOffset;
	if((ulOrigSize-ulSize-ulWasteFront-BMEM_BACK_BOOKKEEPING_SIZE)
		< ulMinBlockSize)
	{
		ulWasteFront = ulOrigSize-ulSize-ulBookKeepingSize;
	}

	/* Now the back */
	addrBack = addrBase + ulOrigSize - ulSize - BMEM_BACK_BOOKKEEPING_SIZE;

	/* Calculate the waste */
	/* Alignment should be calculated on physical address */
	if(BMEM_Heap_ConvertAddressToOffset(pheap, (void *)addrBack, &addrBackOffset) != BERR_SUCCESS)
	{
		BDBG_ERR(("Invalid address being converted to offset."));
		goto done;
	}
	ulWasteBack = addrBackOffset & ulAlignMask;
	/* Pull it back to the next alignment */
	addrBackOffsetAligned = addrBackOffset & ~ulAlignMask;
	if(BMEM_Heap_ConvertOffsetToAddress(pheap, addrBackOffsetAligned, (void *)&addrBackAligned) != BERR_SUCCESS)
	{
		BDBG_ERR(("Invalid offset being converted to address."));
		goto done;
	}

	if((ulOrigSize-ulSize-ulWasteBack-ulBookKeepingSize)
		< ulMinBlockSize)
	{
		ulWasteBack = ulOrigSize-ulSize-ulBookKeepingSize;
	}

	/*
	 * Decide if we're taking the front or the back.
	 *
	 * This addrFinal is the address which will be returned by the
	 * function. The bookeeping info is a bit in front of it.
	 *
	 */
	addrFinal = (ulWasteFront <= ulWasteBack ? addrFrontAligned : addrBackAligned);

	/*
	 * OK! We now have the optimal place to put the allocation
	 * within the block. Split the block up and do the allocation.
	 */

	/*
	 * Of all those calculations above, the only things which are used
	 * below this point are addrFinal, addrBase, and ulOrigSize. (The
	 * other variables are wiped out and reused.)
	 *
	 * What this means is that if you come up with some fancy way to figure
	 * out addrFinal, you can do that instead of the above.
	 */

	/*
	 * There are theoretically three blocks which can be made here,
	 * a front free block, the allocated block, and a back free block.
	 * If any block is too small, then it is added to the allocation.
	 * This reduces fragmentation of tiny bits of memory.
	 *
	 *
	 * [addrBase.......addrFinal.......addrMiddle.....addrBack]
	 *
	 *                B^-------------^G
	 *                     ulSize
	 * ^-------------^                 ^----------------------^
	 *  ulWasteFront                         ulWasteBack
	 *
	 * B-------------GB---------------GB----------------------G
	 * pbiFreeFront   pbiNew           pbiFreeBack
	 *
	 *
	 * B = BMEM_P_BlockInfo (has guard bytes in it)
	 * G = guard bytes
	 *
	 */

	addrFinal -= ulFrontBookKeepingSize;
	addrMiddle = addrFinal + ulSize + ulBookKeepingSize;
	addrBack = addrBase + ulOrigSize;

	pbiNew = BMEM_P_GetBlockInfo(pheap, addrFinal);
	pbiNew->ulFrontScrap = 0;
	pbiNew->ulBackScrap = 0;
	pbiNew->ulSize = ulSize + ulBookKeepingSize;

#if BMEM_TRACK_FILE_AND_LINE
	if(pheap->pSafetyConfigInfo->bTrackFileAndLine)
	{
		pbiNew->ulLine = iLine;
		pbiNew->pchFilename = pchFilename;
	}
#endif

	/* Determine if slack at the front should be made into a free block. */
	ulWasteFront = addrFinal - addrBase;
	if(ulWasteFront < ulMinBlockSize)
	{
		/* Not enough to make a block, keep it as scrap */
		pbiNew->ulSize += ulWasteFront;
		pbiNew->ulFrontScrap = ulWasteFront;

		/* We can't reuse the old BlockInfo, so drop it. */
		if(addrBase!=addrFinal)
		{
			BMEM_P_DropBlockInfo(pheap, pbi);
		}
	}
	else
	{
		/* Make a free node for this left over area. */
		/* This is the same base address as the original free block */
		pbi->ulSize = ulWasteFront;
		pbi->ulFrontScrap = 0;
		pbi->ulBackScrap = 0;

		/* Connect up the free node and update the guard band */
		BMEM_P_AddFree(pheap, pbi);
	}

	/* Determine if slack at the back should be made into a free block. */
	ulWasteBack = addrBack - addrMiddle;
	if(ulWasteBack < ulMinBlockSize)
	{
		/* This leftover is too small to turn into a free block */
		pbiNew->ulSize += ulWasteBack;
		pbiNew->ulBackScrap = ulWasteBack;
	}
	else
	{
		/* Make a new free node for this leftover area. */
		BMEM_P_BlockInfo *pbiFreeBack = BMEM_P_GetBlockInfo(pheap, addrMiddle);

		pbiFreeBack->ulSize = ulWasteBack;
		pbiFreeBack->ulFrontScrap = 0;
		pbiFreeBack->ulBackScrap = 0;

		/* Connect up the free node and update the guard band */
		BMEM_P_AddFree(pheap, pbiFreeBack);
	}

	/*
	 * OK! We've freed what we could back to the free list and updated
	 * our size.
	 *
	 * Finalize the data structure and connect it up to the
	 * alloc'ed list.
	 */
	BMEM_P_AddUsed(pheap, pbiNew);

	pvRet = (void *)(addrFinal+ulFrontBookKeepingSize);

	if (pheap->monitor) {
		pheap->monitor->alloc(pheap->monitor->cnxt, pheap->ulOffset +
			((uint32_t)pvRet - (uint32_t)pheap->pvHeap), ulSize, pchFilename, iLine);
	}

	pheap->bCacheLocked = true;

	BDBG_MSG(("BMEM_AllocAligned(heap %p), offset=0x%x, ptr=%p, size=%lu, align=%u, code=%s:%d)",
		pheap,
		pheap->ulOffset + ((uint32_t)pvRet - (uint32_t)pheap->pvHeap),
		pvRet, ulSize, (int)ucAlignBits, pchFilename, iLine));

done:
	BMEM_P_RELEASE_SEMAPHORE(pheap);

	BDBG_LEAVE(BMEM_P_Heap_TagAllocAligned);
	return pvRet;
}

/*{private}*****************************************************************
Summary:
	Finds a free block of a certain size.

Description:
	This function searches through the free block list looking for a block
	which can contain an object of the given size and alignment. If one is
	found, it is removed from the free list and returned.

Returns:
	A pointer to the BlockInfo for a block which satisfies the size and
	alignment constraints, or NULL if no such block exists.
****************************************************************************/
BMEM_P_BlockInfo *BMEM_P_GetFreeBlock
(
	BMEM_Heap_Handle pheap,
	size_t ulSize,
	uint32_t ulAlignMask
)
{
	BMEM_P_BlockInfo *pbi;
	BMEM_P_BlockInfo *pbiLast = NULL;
	BMEM_P_SizeKey stKey;

	uint32_t ulBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                             BMEM_BOOKKEEPING_SIZE_LOCAL : BMEM_BOOKKEEPING_SIZE_SYSTEM;
	uint32_t ulFrontBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                                  BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL : BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM;

	stKey.ulAddr = 0; 	/* ulAddr is for comparing blocks.  set to 0 when finding a block of sufficient size. */
	stKey.ulSize = ulSize;

	for(pbi = BLST_AA_TREE_FIND_SOME(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, &stKey);
		pbi != NULL;
		pbi = BLST_AA_TREE_NEXT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbi))
	{
		if(pbi->ulSize >= (ulSize+ulBookKeepingSize))
		{
			uint32_t addrBaseOffset = 0;
			uint32_t addrFront = 0;
			uint32_t addrFrontOffset = 0;
			uint32_t addrFrontOffsetAligned = 0;

			/* Free blocks never have scrap */

			/* Determine where aligned allocation would start */
			addrFront = BMEM_P_GetAddress( pheap, pbi ) + ulFrontBookKeepingSize;

			/* Alignment should be calculated on physical offset of starting address */
			if(BMEM_Heap_ConvertAddressToOffset(pheap, (void *)addrFront, &addrFrontOffset) != BERR_SUCCESS)
			{
				BDBG_ERR(("Invalid address being converted to offset."));
				break;
			}
			addrBaseOffset = addrFrontOffset - ulFrontBookKeepingSize;
			addrFrontOffsetAligned = (addrFrontOffset + ulAlignMask) & ~ulAlignMask;

			/* Make sure allocation with requested size starting on aligned address still fits in block */
			if((addrFrontOffsetAligned + ulSize + BMEM_BACK_BOOKKEEPING_SIZE) <= (addrBaseOffset + pbi->ulSize))
			{
				/* This block can fit the request */
				/* Remove this free node from trees */
				BLST_AA_TREE_REMOVE(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree, pbi);
				BLST_AA_TREE_REMOVE(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbi);

				pheap->ulNumFree--;
				pheap->ulTotalFree -= pbi->ulSize;

				goto done;
			}
		}

		pbiLast = pbi;
	}

	pbi = NULL;
done:
	return pbi;
}

#if BMEM_TRACK_FILE_AND_LINE
/* This prints file/line information if available */
static void BMEM_P_PrintBlock(BMEM_Heap_Handle pheap, BMEM_P_BlockInfo *pbi)
{
	if (pheap->pSafetyConfigInfo->bTrackFileAndLine) {
		/* make sure the filename is printable. at this point, we're already corrupt, so
		why trust the filename? */
		char temp[BMEM_FILENAME_LENGTH];
		int i;

		BKNI_Memcpy(temp, pbi->pchFilename, BMEM_FILENAME_LENGTH);
		for (i=0;i<BMEM_FILENAME_LENGTH;i++) {
			if (!temp[i]) break;
			if (temp[i] < 32) temp[i]='.';
		}
		if (i == BMEM_FILENAME_LENGTH) {
			temp[BMEM_FILENAME_LENGTH-1] = 0;
		}

		/* NOTE: %.128s is not portable. we have to use %s. */
		BDBG_ERR(("    addr=%p(pbi=%p), size=%u, allocated at: %s:%d",
			BMEM_P_GetAddress( pheap, pbi ), pbi, pbi->ulSize, temp, pbi->ulLine));
	}
}
#endif

/***************************************************************************/
BERR_Code BMEM_Heap_Free(BMEM_Heap_Handle pheap, void *pv)
{
	BERR_Code err;
	BMEM_P_BlockInfo *pbi;

	uint32_t ulFrontBookKeepingSize;

	BDBG_ENTER(BMEM_Free);
    BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);
	ulFrontBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                                  BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL : BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM;
	BDBG_MSG(("BMEM_Free(%p (%p), %p)", (pheap ? pheap->pvHeap : NULL), pheap, pv));

	if(pv == NULL)
	{
		goto done;
	}

#if BMEM_CHECK_ALL_GUARDS
	if(pheap->pSafetyConfigInfo->bCheckAllGuards)
	{
		BMEM_Heap_Validate(pheap);
	}
#endif

	/* acquire semaphore */
	err = BMEM_P_GET_SEMAPHORE(pheap);
	BDBG_ASSERT(err == BERR_SUCCESS);

	/* get information about requested block */
	pbi = BMEM_P_GetBlockInfo( pheap,
		(uint32_t) pv - ulFrontBookKeepingSize );




#if BMEM_TRACK_ALLOCATIONS
	if(pheap->pSafetyConfigInfo->bTrackAllocations)
	{
		BMEM_P_BlockInfo *pbiTmp;
		BMEM_P_AddrKey stAddrKey;
		stAddrKey.ulAddr = BMEM_P_GetAddress(pheap, pbi);
	
#if BMEM_FREE_CAREFULLY
		if(pheap->pSafetyConfigInfo->bFreeCarefully)
		{
			pbiTmp = BLST_AA_TREE_FIND(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, &stAddrKey);
			if(pbiTmp == NULL)
			{
				BDBG_ERR(("Unable to find block in allocated list!"));
#if BMEM_TRACK_FILE_AND_LINE
				if(pheap->pSafetyConfigInfo->bTrackFileAndLine)
				{
					BMEM_P_PrintBlock(pheap, pbi);
				}
#endif
				pheap->ulNumErrors++;
				goto done_release_sem;
			}
		}
#endif /* BMEM_FREE_CAREFULLY */

		BLST_AA_TREE_REMOVE(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, pbi);
	}
#endif /* BMEM_TRACK_ALLOCATIONS */

#if BMEM_CHECK_GUARD_ON_FREE
	if(pheap->pSafetyConfigInfo->bCheckGuardOnFree)
	{
		BMEM_P_CheckGuard(pheap, pbi);
	}
#endif

	if(pbi->ulFrontScrap != 0)
	{
		/* Remove the scrap area */
		size_t ulSize = pbi->ulSize;
		uint32_t addr = BMEM_P_GetAddress( pheap, pbi ) - pbi->ulFrontScrap;

		BMEM_P_DropBlockInfo(pheap, pbi);

		pbi = BMEM_P_GetBlockInfo(pheap, addr);
		pbi->ulSize = ulSize;
		pbi->ulFrontScrap = 0;
	}
	pbi->ulBackScrap = 0;

#if BMEM_FREE_CLEAR
	if(pheap->pSafetyConfigInfo->bFreeClear)
	{
		uint32_t ulBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
		                             BMEM_BOOKKEEPING_SIZE_LOCAL : BMEM_BOOKKEEPING_SIZE_SYSTEM;
		uint8_t *pc = ((uint8_t *)BMEM_P_GetAddress(pheap, pbi))+
			ulFrontBookKeepingSize;
		BKNI_Memset((void*)pc, BMEM_CLEAR_BYTE,
			pbi->ulSize - ulBookKeepingSize - pbi->ulFrontScrap - pbi->ulBackScrap);
	}
#endif /* BMEM_FREE_CLEAR */

	pheap->ulNumAllocated--;
	pheap->ulTotalAllocated -= pbi->ulSize;

	BMEM_P_AddFree(pheap, pbi);

	if (pheap->monitor) {
		pheap->monitor->free(pheap->monitor->cnxt, pheap->ulOffset +
			((uint32_t)pv - (uint32_t)pheap->pvHeap));
	}

#if BMEM_FREE_CAREFULLY
done_release_sem:
#endif
	BMEM_P_RELEASE_SEMAPHORE(pheap);

done:
	BDBG_LEAVE(BMEM_Free);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BMEM_Heap_FreeCached(BMEM_Heap_Handle pheap, void *pvCachedAddress)
{
    BERR_Code err = BERR_SUCCESS;
    void *pv = NULL;

    BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);

    err = BMEM_P_GET_SEMAPHORE(pheap);
    BDBG_ASSERT(err == BERR_SUCCESS);

    /* verify it's a cached address we know about. */
    if (pvCachedAddress &&
        pheap->pvCache &&
        pheap->pfFlushCb_isr &&
        pvCachedAddress >= pheap->pvCache &&
        (uint8_t*)pvCachedAddress < (uint8_t*)pheap->pvCache+pheap->zSize)
    {
        BMEM_P_BlockInfo *pbi;
        uint32_t ulFrontBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
                                          BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL : BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM;
        unsigned userSize; /* the memory that the user sees. only flush this. */

        /* convert from cached to uncached before lookup & free */
        pv = (void*)((uint8_t*)pvCachedAddress - (uint8_t*)pheap->pvCache + (uint8_t*)pheap->pvHeap);

        /* get information about requested block */
        pbi = BMEM_P_GetBlockInfo( pheap, (uint32_t) pv - ulFrontBookKeepingSize );

        userSize = pbi->ulSize - pbi->ulFrontScrap - pbi->ulBackScrap - ulFrontBookKeepingSize - BMEM_BACK_BOOKKEEPING_SIZE;
        BDBG_ASSERT(userSize <= pbi->ulSize);

        BDBG_MSG(("BMEM_Heap_FreeCached(%p) flushing %p %d", pheap, pvCachedAddress, userSize));
        pheap->pfFlushCb_isr(pvCachedAddress, userSize);
    }
    else {
        /* this isn't a cached addr we know about. caller should fix the code. */
        err = BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BMEM_P_RELEASE_SEMAPHORE(pheap);

    if (pv) {
        /* here's the uncached free */
        BMEM_Heap_Free(pheap, pv);
    }

    return err;
}

/*{private}*****************************************************************
Summary:
	Adds a block to the free list. Note the free list is always kept as size
	ascending order.

Description:
	This function takes the given BlockInfo and adds it to the list of free
	blocks available for future allocations. When it does this, it performs a
	heap compaction in order to maximize the size of free blocks; it also sorts
	the free list according to the size ascending order to prevent memory
	fragmentation.

Returns:
	void
****************************************************************************/
static void BMEM_P_AddFree(BMEM_Heap_Handle pheap, BMEM_P_BlockInfo *pbi)
{
	BMEM_P_BlockInfo *pbiBack = NULL;
	BMEM_P_BlockInfo *pbiFront = NULL;
	BMEM_P_BlockInfo *pbiTmp, *pbiTmpPrev;
	BMEM_P_BlockInfo *pbiPrev = NULL;
	BMEM_P_BlockInfo *pbiNext = NULL;
	uint32_t          ulFrontAddress;
	uint32_t          ulBackAddress;
	BMEM_P_AddrKey    stAddrKey;
	BMEM_P_SizeKey    stSizeKey;

	/*
	 * The free list is kept sorted in size order (smallest to largest).
	 * This makes the compaction operation non-trivial. Neighbor compaction
	 * is done on every free.
	 *
	 * 1. Scan through the list to find the new free block's
	 *    neighbors (front and back).
	 * 2. If front neighbor found, combine with new node.
	 *    Resulting node is removed from list.
	 * 3. If back neighbor found, combine and remove back neighbor.
	 * 4. Insert resulting node into free list according to size.
	 */

#if BMEM_CHECK_DISORDER
	for(pbiTmpPrev=NULL, pbiTmp=BLST_AA_TREE_FIRST(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
		pbiTmp!=NULL;
		pbiTmpPrev=pbiTmp, pbiTmp=BLST_AA_TREE_NEXT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbiTmp))
	{
		if(pbiTmpPrev!=NULL)
		{
			if(pbiTmpPrev->ulSize > pbiTmp->ulSize)
			{
				BDBG_ERR(("Disordered blocks! Two blocks are:"));
#if BMEM_TRACK_FILE_AND_LINE
				if(pheap->pSafetyConfigInfo->bTrackFileAndLine)
				{
					BMEM_P_PrintBlock(pheap, pbiTmpPrev);
					BMEM_P_PrintBlock(pheap, pbiTmp);
				}
#endif
				pheap->ulNumErrors++;
			}
		}
	}
#else
	BSTD_UNUSED(pbiTmpPrev);
#endif /* BMEM_CHECK_DISORDER */

	/*
	 * Step #1:
	 * Scan through the list to find the new free block's
	 * neighbors (front and back).
	 */

	/* determine front and back address of new entry (re-used) */
	ulFrontAddress = BMEM_P_GetAddress(pheap, pbi) - pbi->ulFrontScrap;
	ulBackAddress = BMEM_P_GetAddress(pheap, pbi) + pbi->ulSize - pbi->ulFrontScrap;

	stAddrKey.ulAddr = BMEM_P_GetAddress(pheap, pbi);
	
	pbiTmp = BLST_AA_TREE_FIND_SOME(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree, &stAddrKey);

	if(pbiTmp)
	{
		if(BMEM_P_GetAddress(pheap, pbiTmp) > BMEM_P_GetAddress(pheap, pbi))
		{
			pbiPrev = BLST_AA_TREE_PREV(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree, pbiTmp);
			pbiNext = pbiTmp;
		}
		else
		{
			pbiPrev = pbiTmp;
			pbiNext = BLST_AA_TREE_NEXT(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree, pbiTmp);
		}
	}
	else
	{
		pbiPrev = NULL;
		pbiNext = NULL;
	}

	/* front neighbor?. */
	if(pbiPrev &&  (BMEM_P_GetAddress(pheap, pbiPrev) + pbiPrev->ulSize - pbiPrev->ulFrontScrap
		== ulFrontAddress))
	{
		/* found front neighbor */
		pbiFront = pbiPrev;
	}

	/* back neighbor? */
	if(pbiNext && (BMEM_P_GetAddress(pheap, pbiNext) - pbiNext->ulFrontScrap
		== ulBackAddress))
	{
		/* found back neighbor */
		pbiBack = pbiNext;
	}

	/*
	 * Step #2:
	 * If front neighbor found, combine with new node.
	 * Resulting combined node is removed from list so that in both cases
	 * the final node is not on list.
	 */
	if(pbiFront)
	{
		/* remove pbiFront from trees */
		BLST_AA_TREE_REMOVE(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree, pbiFront);
		BLST_AA_TREE_REMOVE(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbiFront);

		pheap->ulNumFree--;
		pheap->ulTotalFree -= pbiFront->ulSize;

		/* merge front neighbor with this block */
		pbiFront->ulSize += pbi->ulSize;

		/* no need to keep original new node */
		BMEM_P_DropBlockInfo(pheap, pbi);

		pbi = pbiFront;
	}

	/*
	 * Step #3:
	 * If back neighbor found, combine and remove back neighbor.
	 * At this point, pbi is the new node.
	 */

	/* do we have a back neighbor? */
	if(pbiBack)
	{
		/* remove pbiBack from trees */
		BLST_AA_TREE_REMOVE(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree, pbiBack);
		BLST_AA_TREE_REMOVE(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbiBack);

		pheap->ulNumFree--;
		pheap->ulTotalFree -= pbiBack->ulSize;

		/* update front size with back size */
		pbi->ulSize += pbiBack->ulSize;

		/* after merge, we need to remove back info */
		BMEM_P_DropBlockInfo(pheap, pbiBack);
	}

	/*
	 * Step #4:
	 * Insert resulting node into free trees.
	 */

	stAddrKey.ulAddr = BMEM_P_GetAddress(pheap, pbi);
	stSizeKey.ulSize = pbi->ulSize;
	stSizeKey.ulAddr = BMEM_P_GetAddress(pheap, pbi);
	BLST_AA_TREE_INSERT(BMEM_P_AddrTree, &pheap->stFreeTopAddrTree, &stAddrKey, pbi);
	BLST_AA_TREE_INSERT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, &stSizeKey, pbi);

	pheap->ulNumFree++;
	pheap->ulTotalFree += pbi->ulSize;

#if BMEM_TRACK_FILE_AND_LINE
	if(pheap->pSafetyConfigInfo->bTrackFileAndLine)
	{
		pbi->pchFilename = s_achFilenameFreeString;
		pbi->ulLine = 0;
	}
#endif

	BMEM_P_FillGuard(pheap, pbi);
}

/*{private}*****************************************************************
Summary:
	Adds a block to the allocated list.

Description:
	This function takes the given BlockInfo and adds it to the list of
	allocated blocks (assuming that allocated blocks are being tracked).

Returns:
	void
****************************************************************************/
static void BMEM_P_AddUsed(BMEM_Heap_Handle pheap, BMEM_P_BlockInfo *pbi)
{
#ifdef BMEM_TRACK_ALLOCATIONS
	if(pheap->pSafetyConfigInfo->bTrackAllocations)
	{
		BMEM_P_AddrKey stAddrKey;

		stAddrKey.ulAddr = BMEM_P_GetAddress(pheap, pbi);

		BLST_AA_TREE_INSERT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, &stAddrKey, pbi);
	}
#endif

	pheap->ulNumAllocated++;
	pheap->ulTotalAllocated += pbi->ulSize;

	if (pheap->ulTotalAllocated > pheap->ulHighWatermark)
	{
		pheap->ulHighWatermark = pheap->ulTotalAllocated;
	}

	BMEM_P_FillGuard(pheap, pbi);
}

/***************************************************************************/
size_t BMEM_Heap_GetLargestAvailableBlockSize(
	BMEM_Heap_Handle  pheap
	)
{
	BERR_Code err;
	BMEM_P_BlockInfo *pbi;
	size_t            zLargestSize = 0;

	uint32_t ulBookKeepingSize, ulFrontBookKeepingSize, ulFrontScrap;
	uint32_t addrBase = 0, addrFront, addrFrontOffset, addrFrontOffsetAligned;

    BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);
	BDBG_ENTER(BMEM_GetLargestAvailableBlockSize);
	ulBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                             BMEM_BOOKKEEPING_SIZE_LOCAL : BMEM_BOOKKEEPING_SIZE_SYSTEM;
	ulFrontBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                                  BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL : BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM;

	/* acquire semaphore */
	err = BMEM_P_GET_SEMAPHORE(pheap);
	BDBG_ASSERT(err == BERR_SUCCESS);

	pbi = BLST_AA_TREE_LAST(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
	if (pbi)
	{
		/* update current max */
		zLargestSize = pbi->ulSize;
		addrBase = BMEM_P_GetAddress(pheap, pbi);
	}

	if(zLargestSize)
	{
		/* the following calculates size of alignment scrap */
		addrFront = addrBase + ulFrontBookKeepingSize;

		/* Alignment should be calculated on physical address */
		/* Push it forward to the next alignment */
		if(BMEM_Heap_ConvertAddressToOffset(pheap, (void *)addrFront, &addrFrontOffset) != BERR_SUCCESS)
		{
			BDBG_ERR(("Invalid address being converted to offset."));
			zLargestSize = 0;
			goto done;
		}

		/* the following calculates size of alignment scrap */
		addrFrontOffsetAligned = (addrFrontOffset + pheap->ulAlignMask) & ~pheap->ulAlignMask;

		/* Calculate the waste */
		ulFrontScrap = addrFrontOffsetAligned - addrFrontOffset;

		/* can this block contain additional bookkeeping and alignment scrap bytes? */
		if (zLargestSize > ulBookKeepingSize + ulFrontScrap)
		{
			/* reduce the size of the block by the bookkeeping amount */
			zLargestSize -= (ulBookKeepingSize + ulFrontScrap);
			/* align available size */
			zLargestSize &= ~pheap->ulAlignMask;

			/* not large enough to contain bookkeeping bytes */
		} else
		{
			/* not big enough to allocate */
			zLargestSize = 0;
		}
	}

done:
	/* release semaphore */
	BMEM_P_RELEASE_SEMAPHORE(pheap);

	/* return largest size found */
	BDBG_MSG(("BMEM_LargestBlock = %lu", zLargestSize));
	BDBG_LEAVE(BMEM_GetLargestAvailableBlockSize);

	return zLargestSize;
}

/**********************************************************************func*
 * BMEM_GetInfo - Gets information about the heap.
 *
 * The function BMEM_GetInfo fills in a structure with a variety of
 * interesting information about the heap. This information is mainly useful
 * for debugging and statistic gathering. See the BMEM_HeapInfo structure
 * definition for the set of information returned.
 *
 * Returns:
 *    void
 *
 */
void BMEM_Heap_GetInfo
(
	BMEM_Heap_Handle pheap,
	BMEM_HeapInfo   *phi
)
{
	BERR_Code err;
	BMEM_P_BlockInfo *pbi;

    BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);
	BDBG_ENTER(BMEM_Heap_GetInfo);
	BDBG_MSG(("BMEM_Heap_GetInfo(%p (%p), %p)", (pheap ? pheap->pvHeap : NULL), pheap, phi));

	/* acquire semaphore */
	err = BMEM_P_GET_SEMAPHORE(pheap);
	BDBG_ASSERT(err == BERR_SUCCESS);

	/* get creation data */
	phi->pvAddress =   pheap->pvHeap;
	phi->ulOffset =    pheap->ulOffset;
	phi->zSize =       pheap->zSize;
	phi->uiAlignment = pheap->uiAlignment;

	/* get statistics */
	pbi = BLST_AA_TREE_FIRST(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
	phi->ulSmallestFree = pbi? pbi->ulSize : 0;
	pbi = BLST_AA_TREE_LAST(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
	phi->ulLargestFree = pbi? pbi->ulSize : 0;
	phi->ulTotalFree = pheap->ulTotalFree;
	phi->ulNumFree = pheap->ulNumFree;

	phi->ulLargestAllocated = 0;
	phi->ulSmallestAllocated = 0xffffffff;

#ifdef BMEM_TRACK_ALLOCATIONS
	if(pheap->pSafetyConfigInfo->bTrackAllocations)
	{
		for(pbi = BLST_AA_TREE_FIRST(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree);
			pbi != NULL;
			pbi = BLST_AA_TREE_NEXT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, pbi))
		{
			if(pbi->ulSize > phi->ulLargestAllocated)
			{
				phi->ulLargestAllocated = pbi->ulSize;
			}
			if(pbi->ulSize < phi->ulSmallestAllocated)
			{
				phi->ulSmallestAllocated = pbi->ulSize;
			}
		}
	}
#endif

	phi->ulNumAllocated = pheap->ulNumAllocated;
	phi->ulTotalAllocated = pheap->ulTotalAllocated;
	phi->ulHighWatermark = pheap->ulHighWatermark;

	/* set number of errors found */
	phi->ulNumErrors = pheap->ulNumErrors;

	/* release semaphore */
	BMEM_P_RELEASE_SEMAPHORE(pheap);

	BDBG_LEAVE(BMEM_Heap_GetInfo);
}

void BMEM_Heap_ResetHighWatermark
(
	BMEM_Heap_Handle pheap
)
{
	BDBG_ENTER(BMEM_Heap_ResetHighWatermark);
	pheap->ulHighWatermark = 0;
	BDBG_LEAVE(BMEM_Heap_ResetHighWatermark);
}

/***************************************************************************/
static BERR_Code BMEM_P_ValidatePointer(BMEM_Heap_Handle pheap, BMEM_P_BlockInfo *pbi)
{
	/* TODO: confirm this is necessary */
	if ((uint32_t)pbi % 4 != 0) {
		BDBG_ERR(("Unaligned pointer %p", pbi));
		return BERR_UNKNOWN;
	}

	if (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) {
		if ((void*)pbi < (void*)pheap->pStart || (void*)pbi > (void*)pheap->pEnd) {
			BDBG_ERR(("Pointer %p out of heap %p..%p", pbi, pheap->pStart, pheap->pEnd));
			return BERR_UNKNOWN;
		}
	}
	/* TODO: else for BMEM_BOOKKEEPING_SYSTEM */
	return BERR_SUCCESS;
}

/* Detect linked list loops */
#define BMEM_P_CHECK_LOOPS 0

static BERR_Code BMEM_P_CheckList(BMEM_Heap_Handle pheap, BMEM_P_BlockInfo *pbi, const char *name)
{
	BERR_Code err;

	BSTD_UNUSED(name);
	/* Check free list */
	/* 1. Validate pointers to avoid unaligned access and page faults based on corruptions. */
	err = BMEM_P_ValidatePointer(pheap, pbi);
	if (err) return BERR_TRACE(err);

	/* 2. Check guard bands for overruns. */
	err = BMEM_P_CheckGuard(pheap, pbi);
	if (err) return BERR_TRACE(err);

	return BERR_SUCCESS;
}

/* BMEM_Heap_Validate is the first line of defense against heap corruptions.
We should avoid any implicit failures (segfaults, unaligned accesses) and provide
explicit failures with hopefully helpful debug information. */
BERR_Code BMEM_Heap_Validate
(
	BMEM_Heap_Handle pheap
)
{
	BMEM_P_BlockInfo *pbiTmp;
#if BMEM_P_CHECK_LOOPS
	BMEM_P_BlockInfo *pbiLoop = NULL;
#endif
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BMEM_Heap_Validate);

    BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);

	/* acquire semaphore */
	err = BMEM_P_GET_SEMAPHORE(pheap);
	BDBG_ASSERT(err == BERR_SUCCESS);

	/* validate free list */
	for(pbiTmp=BLST_AA_TREE_FIRST(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
		pbiTmp!=NULL;
		pbiTmp=BLST_AA_TREE_NEXT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbiTmp))
	{
		err = BMEM_P_CheckList(pheap, pbiTmp, "free");
		if (err) goto done_release_sem;
	}

#ifdef BMEM_TRACK_ALLOCATIONS
	if(pheap->pSafetyConfigInfo->bTrackAllocations)
	{
		/* validate allocated list */
		for(pbiTmp=BLST_AA_TREE_FIRST(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree);
			pbiTmp!=NULL;
			pbiTmp=BLST_AA_TREE_NEXT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, pbiTmp))
		{
			err = BMEM_P_CheckList(pheap, pbiTmp, "allocated");
			if (err) goto done_release_sem;
		}
	}
#endif

#if BMEM_P_CHECK_LOOPS
	/* Check for a loop in the list. */
	for(pbiTmp=BLST_AA_TREE_FIRST(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree),
		pbiLoop=BLST_AA_TREE_FIRST(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
		pbiTmp!=NULL || pbiLoop!=NULL;
		pbiTmp=BLST_AA_TREE_NEXT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbiTmp))
	{
		if (pbiLoop)
		{
			pbiLoop = BLST_AA_TREE_NEXT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbiLoop);
		}

		if(pbiLoop)
		{
			pbiLoop = BLST_AA_TREE_NEXT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbiLoop);
		}

		if (pbiLoop == pbiTmp) {
			BDBG_ERR(("Loop in free list!"));
			BDBG_ASSERT(false);
			err = BERR_UNKNOWN;
			goto done_release_sem;
		}
	}

#ifdef BMEM_TRACK_ALLOCATIONS
	if(pheap->pSafetyConfigInfo->bTrackAllocations)
	{
		/* Check for a loop in the list. */
		for(pbiTmp=BLST_AA_TREE_FIRST(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree),
			pbiLoop=BLST_AA_TREE_FIRST(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree);
			pbiTmp!=NULL || pbiLoop!=NULL;
			pbiTmp=BLST_AA_TREE_NEXT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, pbiTmp))
		{
			if (pbiLoop)
			{
				pbiLoop = BLST_AA_TREE_NEXT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, pbiLoop);
			}

			if(pbiLoop)
			{
				pbiLoop = BLST_AA_TREE_NEXT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, pbiLoop);
			}

			if (pbiLoop == pbiTmp) {
				BDBG_ERR(("Loop in alloc list!"));
				BDBG_ASSERT(false);
				err = BERR_UNKNOWN;
				goto done_release_sem;
			}
		}
	}
#endif
#endif

done_release_sem:
	/* release semaphore */
	BMEM_P_RELEASE_SEMAPHORE(pheap);

	/* return status */
	BDBG_LEAVE(BMEM_Heap_Validate);
return err;
}

/**********************************************************************func*
 * BMEM_P_FillGuard - Fills the guards bytes of a block.
 *
 * This function fills both the front and back guard bytes of a given block.
 * If the block has any back scrap, this area is also filled with the guard
 * byte. (The front scrap is not since it actually sits in front of
 * bookkeeping information.)
 *
 * Returns:
 *    void
 */
void BMEM_P_FillGuard(BMEM_Heap_Handle pheap, BMEM_P_BlockInfo *pbi)
{
#if (BMEM_GUARD_SIZE != 0)
	if (pheap->ulGuardSizeBytes)
	{
		size_t i;
		uint32_t ulFrontBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
		                                  BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL : BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM;
		uint8_t *pcFront = (uint8_t *)BMEM_P_GetAddress(pheap, pbi) +
			ulFrontBookKeepingSize- pheap->ulGuardSizeBytes;
		uint8_t *pcBack = (uint8_t *)BMEM_P_GetAddress(pheap, pbi) - pbi->ulFrontScrap +
			pbi->ulSize - pbi->ulBackScrap - pheap->ulGuardSizeBytes;
		void *pcFrontCached = NULL;
		void *pcBackCached = NULL;
		bool bIsCached;
		BERR_Code err;

		err = BMEM_Heap_ConvertAddressToCached(pheap, pcFront, &pcFrontCached);
		if (err == BERR_SUCCESS) 
		{
			err = BMEM_Heap_ConvertAddressToCached(pheap, pcBack, &pcBackCached);
		}

		bIsCached = (err == BERR_SUCCESS) && ((pcFront != pcFrontCached) || (pcBack != pcBackCached));

		if (bIsCached)
		{
			BMEM_Heap_FlushCache_isr(pheap, pcFrontCached, pheap->ulGuardSizeBytes);
			BMEM_Heap_FlushCache_isr(pheap, pcBackCached, pbi->ulBackScrap + pheap->ulGuardSizeBytes);
			pcFront = pcFrontCached;
			pcBack = pcBackCached;
		}

		for(i=0; i<pheap->ulGuardSizeBytes; ++i)
		{
			pcFront[i] = BMEM_GUARD_BYTE;
		}

		for(i=0; i<pbi->ulBackScrap + pheap->ulGuardSizeBytes; ++i )
		{
			pcBack[i] = BMEM_GUARD_BYTE;
		}

		if (bIsCached)
		{
			BMEM_Heap_FlushCache_isr(pheap, pcFrontCached, pheap->ulGuardSizeBytes);
			BMEM_Heap_FlushCache_isr(pheap, pcBackCached, pbi->ulBackScrap + pheap->ulGuardSizeBytes);
		}
	}

#else
	BSTD_UNUSED(pbi);
#endif
	BSTD_UNUSED(pheap); /* GetAddress might be a macro which doesn't use pheap */
}

/**********************************************************************func*
 * BMEM_P_CheckGuard - Checks the guard bytes to see if they are valid.
 *
 * This function checks the front guard and back guard bytes (including the
 * back scrap, if any) for any out-of-bound writes. If the guard bytes are
 * not the specified value, then an error is noted and false returned.
 *
 * Returns:
 *    Returns true if the guard bytes are unmolested, and false if an overrun
 *    or underrun is detected.
 */
BERR_Code BMEM_P_CheckGuard
(
	BMEM_Heap_Handle pheap,
	BMEM_P_BlockInfo *pbi
)
{
	BERR_Code err = BERR_SUCCESS;

#if (BMEM_GUARD_SIZE != 0)
	if (pheap->ulGuardSizeBytes)
	{
		size_t ii;

		uint32_t ulFrontBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
		                                  BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL : BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM;

		/* pointers for checking in 32-bit increments */
		uint32_t *pulFront = (uint32_t *) (BMEM_P_GetAddress( pheap, pbi ) +
			ulFrontBookKeepingSize - pheap->ulGuardSizeBytes);

		uint32_t *pulBack  = (uint32_t *) (BMEM_P_GetAddress( pheap, pbi ) -
			pbi->ulFrontScrap +	pbi->ulSize - pbi->ulBackScrap - pheap->ulGuardSizeBytes);

		void *pulFrontCached = NULL;
		void *pulBackCached = NULL;
		bool bIsCached;

		uint32_t ulGuardSizeWords     = pheap->ulGuardSizeBytes / 4;
		uint32_t ulBackGuardSizeWords = (pheap->ulGuardSizeBytes + pbi->ulBackScrap) / 4;

		err = BMEM_Heap_ConvertAddressToCached(pheap, pulFront, &pulFrontCached);
		if (err == BERR_SUCCESS) 
		{
			err = BMEM_Heap_ConvertAddressToCached(pheap, pulBack, &pulBackCached);
		}

		bIsCached = (err == BERR_SUCCESS) && ((pulFront != pulFrontCached) || (pulBack != pulBackCached));

		if (bIsCached)
		{
			pulFront = pulFrontCached;
			pulBack = pulBackCached;
		}

		/* check in 32-bit chunks to take advantage of cache */
		for( ii = 0; ii < ulGuardSizeWords; ++ii )
		{
			if( pulFront[ ii ] != BMEM_P_GUARD_DWORD )
			{
				pheap->ulNumErrors++;
				err = BERR_UNKNOWN;
				BDBG_ERR(("Front guard violation in block! (%d)", pheap->ulGuardSizeBytes));
#if BMEM_TRACK_FILE_AND_LINE
				if(pheap->pSafetyConfigInfo->bTrackFileAndLine)
				{
					BMEM_P_PrintBlock(pheap, pbi);
				}
#endif
				for( ii = 0; ii < ulGuardSizeWords; ++ii )
				{
					BKNI_Printf("%02x", pulFront[ii]);
				}
				BKNI_Printf("\n");
				break;
			}
		}

		for( ii=0; ii < ulBackGuardSizeWords; ++ii )
		{
			if( pulBack[ ii ] != BMEM_P_GUARD_DWORD )
			{
				pheap->ulNumErrors++;
				err = BERR_UNKNOWN;
				BDBG_ERR(("Back guard violation in block! (%d)", pbi->ulBackScrap + pheap->ulGuardSizeBytes));
#if BMEM_TRACK_FILE_AND_LINE
				if(pheap->pSafetyConfigInfo->bTrackFileAndLine)
				{
					BMEM_P_PrintBlock(pheap, pbi);
				}
#endif
				for( ii=0; ii < ulBackGuardSizeWords; ++ii )
				{
					BKNI_Printf("%02x", pulBack[ii]);
				}
				BKNI_Printf("\n");
				break;
			}
		}
	}

#else
	BSTD_UNUSED(pheap);
	BSTD_UNUSED(pbi);
#endif

	return err;
}

/***************************************************************************/
BERR_Code BMEM_Heap_ConvertOffsetToAddress
(
	BMEM_Heap_Handle   Heap,
	uint32_t           Offset,
	void             **Address
)
{
	BMEM_Heap_Handle hCurHeap = Heap;

    BDBG_OBJECT_ASSERT(Heap, BMEM_Heap);

	/* calculate */
	*Address = (void *)((uint8_t *)hCurHeap->pvHeap + Offset - hCurHeap->ulOffset);
	if (BMEM_P_Heap_CheckAddress(hCurHeap, (uint8_t *)*Address))
	{
		goto done;
	}
	else if (Heap->hMem)
	{
		/* check other heaps for match */
		for(hCurHeap = BLST_S_FIRST(&(Heap->hMem->HeapList)); hCurHeap ; hCurHeap = BLST_S_NEXT(hCurHeap, link))
		{
			if (hCurHeap != Heap)
			{
				/* calculate */
				*Address = (void *)((uint8_t *)hCurHeap->pvHeap + Offset - hCurHeap->ulOffset);
				if (BMEM_P_Heap_CheckAddress(hCurHeap, (uint8_t *)*Address))
				{
					goto done;
				}
			}
		}
	}

	/* invalid offset, cannot find heap offset belongs to */
	return BERR_INVALID_PARAMETER;

done:
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BMEM_Heap_ConvertAddressToOffset
(
	BMEM_Heap_Handle  Heap,
	void             *Address,
	uint32_t         *Offset
)
{
	BMEM_Heap_Handle hCurHeap = Heap;
	void *unCachedAddress;

    BDBG_OBJECT_ASSERT(Heap, BMEM_Heap);

	if ((BMEM_P_Heap_CheckAddress(hCurHeap, (uint8_t *)Address)) ||
		(hCurHeap->pvCache && BMEM_P_Heap_CheckCachedAddress(hCurHeap, (uint8_t *)Address)))
	{
		goto done;
	}
	else if (Heap->hMem)
	{
		/* check other heaps for match */
		for(hCurHeap = BLST_S_FIRST(&(Heap->hMem->HeapList)); hCurHeap ; hCurHeap = BLST_S_NEXT(hCurHeap, link))
		{
			if ((hCurHeap != Heap) &&
				((BMEM_P_Heap_CheckAddress(hCurHeap, (uint8_t *)Address)) || 
				 (hCurHeap->pvCache && BMEM_P_Heap_CheckCachedAddress(hCurHeap, (uint8_t *)Address))))
			{
				goto done;
			}
		}
	}

	/* invalid address, cannot find heap address belongs to */
	return BERR_INVALID_PARAMETER;

done:
	/* calculate */
	
	if(BMEM_P_Heap_CheckCachedAddress(hCurHeap, (uint8_t *)Address))
	{
		unCachedAddress = (void*)((uint8_t*)Address - (uint8_t*)hCurHeap->pvCache + (uint8_t*)hCurHeap->pvHeap);
	}
	else
	{
		unCachedAddress = Address;
	}

	*Offset = hCurHeap->ulOffset + (uint8_t *)unCachedAddress - (uint8_t *)hCurHeap->pvHeap;

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BMEM_Heap_ConvertAddressToCached
(
   BMEM_Heap_Handle  Heap,             /* Heap that contains the memory block. */
   void             *pvAddress,        /* Address of the memory block */
   void            **ppvCachedAddress  /* [out] Returned cache address. */
)
{
	BMEM_Heap_Handle hCurHeap = Heap;

    BDBG_OBJECT_ASSERT(Heap, BMEM_Heap);

	if (BMEM_P_Heap_CheckAddress(hCurHeap, (uint8_t *)pvAddress))
	{
		goto done;
	}
	else if (Heap->hMem)
	{
		/* check other heaps for match */
		for(hCurHeap = BLST_S_FIRST(&(Heap->hMem->HeapList)); hCurHeap ; hCurHeap = BLST_S_NEXT(hCurHeap, link))
		{
			if ((hCurHeap != Heap) && (BMEM_P_Heap_CheckAddress(hCurHeap, (uint8_t *)pvAddress)))
			{
				goto done;
			}
		}
	}

	/* invalid address, cannot find heap address belongs to */
	return BERR_INVALID_PARAMETER;

done:
	/* If the user never provided the cached mapping nor cache flushing,
	 * then don't convert just return the non-cached address. */
	if((hCurHeap->pvCache) &&
	   (hCurHeap->pfFlushCb) &&
	   (hCurHeap->pfFlushCb_isr) &&
	   (ppvCachedAddress))
	{
		*ppvCachedAddress = (void*)((uint8_t*)hCurHeap->pvCache +
			((uint8_t*)pvAddress - (uint8_t*)hCurHeap->pvHeap));
	}
	else if(ppvCachedAddress)
	{
		/* Did not set cached so just return the non-cached address. */
		*ppvCachedAddress = pvAddress;
	}

	return BERR_SUCCESS;
}

/*{private}*****************************************************************
Summary:
	Finds the heap the cache address blongs to
	
Output:
	phCacheHeap
	
Returns:
	BERR_SUCCESS
	BERR_INVALID_PARAMETER
****************************************************************************/
BERR_Code BMEM_P_Heap_FindCacheHeap
(
   BMEM_Heap_Handle  Heap,             /* Heap */
   void             *pvCachedAddress,  /* Start address to flush */
   BMEM_Heap_Handle *phCacheHeap       /* Actual heap containing cache memory */
)
{
	BMEM_Heap_Handle hCurHeap = Heap;

    BDBG_OBJECT_ASSERT(Heap, BMEM_Heap);

	if ((pvCachedAddress >= (void*)Heap->pvCache) &&
		(pvCachedAddress <= (void*)(Heap->pEnd - (uint8_t *)Heap->pvHeap + (uint8_t *)Heap->pvCache)))
	{
		goto done;
	}
	else if (Heap->hMem)
	{
		/* check other heaps for match */
		for(hCurHeap = BLST_S_FIRST(&(Heap->hMem->HeapList)); hCurHeap ; hCurHeap = BLST_S_NEXT(hCurHeap, link))
		{
			if ((hCurHeap != Heap) &&
				(pvCachedAddress >= (void*)hCurHeap->pvCache) &&
				(pvCachedAddress <= (void*)(hCurHeap->pEnd - (uint8_t *)hCurHeap->pvHeap + (uint8_t *)hCurHeap->pvCache)))
			{
				goto done;
			}
		}
	}

	/* invalid address, cannot find heap address belongs to */
	*phCacheHeap = NULL;
	return BERR_INVALID_PARAMETER;

done:
	*phCacheHeap = hCurHeap;
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BMEM_Heap_FlushCache
(
   BMEM_Heap_Handle  Heap,             /* Heap containing the cached memory. */
   void             *pvCachedAddress,  /* Start address to flush */
   size_t            size              /* Size in bytes of the block to flush */
)
{
	BMEM_Heap_Handle hCacheHeap;
	BERR_Code err;

	BDBG_OBJECT_ASSERT(Heap, BMEM_Heap);

	err = BMEM_P_Heap_FindCacheHeap(Heap, pvCachedAddress, &hCacheHeap);

/* disable error on non cached addresses for now */
#if 0
	if(err != BERR_SUCCESS)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Call platform specific flush. This is required now in magnum. */
	if(!hCacheHeap->pfFlushCb) 
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* only return SUCCESS if actually flushed */
	hCacheHeap->pfFlushCb(pvCachedAddress, size);
#else
	if(err != BERR_SUCCESS)
	{
		return BERR_INVALID_PARAMETER;
	}

	/* Call platform specific flush. */
	if(hCacheHeap->pfFlushCb)
	{
		hCacheHeap->pfFlushCb(pvCachedAddress, size);
	}
#endif

	return BERR_SUCCESS;
}


/***************************************************************************/
BERR_Code BMEM_Heap_FlushCache_isr
(
   BMEM_Heap_Handle  Heap,             /* Heap containing the cached memory. */
   void             *pvCachedAddress,  /* Start address to flush */
   size_t            size              /* Size in bytes of the block to flush */
)
{
	BMEM_Heap_Handle hCacheHeap;
	BERR_Code err;

    BDBG_OBJECT_ASSERT(Heap, BMEM_Heap);

	err = BMEM_P_Heap_FindCacheHeap(Heap, pvCachedAddress, &hCacheHeap);

	if(err != BERR_SUCCESS)
	{
		return BERR_INVALID_PARAMETER;
	}

	/* Call platform specific flush. */
	if(hCacheHeap->pfFlushCb_isr)
	{
		hCacheHeap->pfFlushCb_isr(pvCachedAddress, size);
	}

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BMEM_Heap_InstallMonitor(BMEM_Heap_Handle heap, BMEM_MonitorInterface *monitor)
{
	BERR_Code err;

    BDBG_OBJECT_ASSERT(heap, BMEM_Heap);
	/* acquire semaphore */
	err = BMEM_P_GET_SEMAPHORE(heap);
	BDBG_ASSERT(err == BERR_SUCCESS);

	heap->monitor = monitor;
	/* release semaphore */
	BMEM_P_RELEASE_SEMAPHORE(heap);
	return BERR_SUCCESS;
}

/***************************************************************************/
void BMEM_Heap_RemoveMonitor(BMEM_Heap_Handle heap, BMEM_MonitorInterface *monitor)
{
	BERR_Code err;
	BSTD_UNUSED(monitor);

    BDBG_OBJECT_ASSERT(heap, BMEM_Heap);
	/* we have to acquire semaphore, to prevent removing monitor disapearing
	 * underneath of alloc or free */
	/* acquire semaphore */
	err = BMEM_P_GET_SEMAPHORE(heap);
	BDBG_ASSERT(err == BERR_SUCCESS);

	heap->monitor = NULL;

	/* release semaphore */
	BMEM_P_RELEASE_SEMAPHORE(heap);
return;
}

/***************************************************************************/
BERR_Code BMEM_Heap_GetDefaultSettings
(
	BMEM_Heap_Settings *pHeapSettings
)
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BMEM_Heap_GetDefaultSettings);

	if(pHeapSettings)
	{
		*pHeapSettings = s_stDefaultHeapSettings;
	}
	else
	{
		err = BERR_INVALID_PARAMETER;
	}

	BDBG_LEAVE(BMEM_Heap_GetDefaultSettings);
	return err;
}

/***************************************************************************/
BERR_Code BMEM_Heap_Create
(
    BMEM_ModuleHandle   hMem,          /* main handle from BMEM_Open() - NULL is possible for older chipsets (7038/3560 only) */
    void               *pvAddress,     /* Pointer to beginning of memory chunk to manage (uncached) */
    uint32_t            ulOffset,      /* Device offset of initial location */
    size_t              zSize,         /* Size of chunk to manage in bytes */
    BMEM_Heap_Settings *pHeapSettings, /* default settings */
    BMEM_Heap_Handle   *phHeap         /* returned heap */
)
{
	BERR_Code err = BERR_SUCCESS;

#if !((BCHP_CHIP==7038) || (BCHP_CHIP==3560))
	if(!hMem)
	{
		BDBG_ERR(("Must provide BMEM_ModuleHandle for this chipset."));
		return BERR_INVALID_PARAMETER;
	}
#endif

	if (pHeapSettings->eBookKeeping == BMEM_BOOKKEEPING_SYSTEM)
	{
		err = BMEM_P_SystemCreateHeap (phHeap, pvAddress, ulOffset, zSize, pHeapSettings);
	}
	else if (pHeapSettings->eBookKeeping == BMEM_BOOKKEEPING_LOCAL)
	{
		BDBG_ERR(("Local bookkeeping no longer supported.  Please use system bookkeeping."));
		return BERR_INVALID_PARAMETER;
/*
		err = BMEM_P_LocalCreateHeap (phHeap, pvAddress, ulOffset, zSize, pHeapSettings);
*/
	}
	else
	{
		BDBG_ERR(("Must be configured for either system or local bookkeeping."));
		BDBG_ASSERT(false);
		return BERR_INVALID_PARAMETER;
	}

	if (*phHeap)
	{
		(*phHeap)->hMem = hMem;

		if ((err == BERR_SUCCESS) && (hMem != NULL))
		{
			BLST_S_INSERT_HEAD(&(hMem->HeapList), (*phHeap), link);
		}
	}

    BDBG_OBJECT_ASSERT(*phHeap, BMEM_Heap);
	return err;
}

/***************************************************************************/
void BMEM_Heap_Destroy
(
	BMEM_Heap_Handle Heap
)
{
    BDBG_OBJECT_ASSERT(Heap, BMEM_Heap);
	if(Heap->hMem)
	{
		BLST_S_REMOVE(&(Heap->hMem->HeapList), Heap, BMEM_P_Heap, link);
	}
	(*Heap->pDestroyHeapFunc) (Heap);
}

/**********************************************************************func*
 * BMEM_CreateHeap - Initializes the heap with bookkeeping (whether system
 *                   or local) based on the BMEM_BOOKKEEPING_CONFIG flag
 *                   setting.
 *
 * This function inititalizes a heap at a given location and size.
 * Any previous allocations in the chunk of memory handed over to this
 * function are lost. Every heap has a base minimum alignment for all of
 * the allocations within that heap. (However, you can specify a greater
 * alignment when actually doing an allocation.)
 *
 *
 * Returns:
 *   Returns true if heap is initialized, or false if the size of the heap
 *   is too small to manage or there isn't enough system memory to allocate
 *   bookkeeping information (when configured for system memory
 *   bookkeeping).
 *
 */
/* This module is empty unless we're using local bookkeeping. */
BERR_Code BMEM_CreateHeap
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

#if (BMEM_BOOKKEEPING_CONFIG == BMEM_BOOKKEEPING_SYSTEM)
	pHeapSettings->eBookKeeping = BMEM_BOOKKEEPING_SYSTEM;
#elif (BMEM_BOOKKEEPING_CONFIG == BMEM_BOOKKEEPING_LOCAL)
	pHeapSettings->eBookKeeping = BMEM_BOOKKEEPING_LOCAL;
#else
	BDBG_ERR(("Must be configured for either system or local bookkeeping."));
	BDBG_ASSERT(false);
#endif /* (BMEM_BOOKKEEPING_CONFIG == BMEM_BOOKKEEPING_SYSTEM) */

	return (BMEM_Heap_Create(NULL, pvHeap, ulOffset, zSize, pHeapSettings, ppHeap));
}

/* End of File */
