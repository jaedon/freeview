/***************************************************************************
 *     Copyright (c) 2001-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmem_debug.c $
 * $brcm_Revision: Hydra_Software_Devel/26 $
 * $brcm_Date: 9/21/12 6:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/mem/bmem_debug.c $
 * 
 * Hydra_Software_Devel/26   9/21/12 6:46p albertl
 * SW7435-255: Optimized BMEM with AA Tree implementation.
 * 
 * Hydra_Software_Devel/25   2/8/12 1:56p albertl
 * SW7425-2345: Changed total allocation and number of allocation tracking
 * to work in eFastest.
 * 
 * Hydra_Software_Devel/24   3/28/11 5:12p albertl
 * SW7425-247: Incorporated BDBG_OBJECT handle validation.
 *
 * Hydra_Software_Devel/23   11/4/10 7:56p albertl
 * SW7420-1155: Fixed pEnd to really be last available byte and addrEnd to
 * be last byte in chunk rather than byte after that.
 *
 * Hydra_Software_Devel/22   7/26/10 3:02p erickson
 * SW3548-3013: BMEM_Dbg_DumpHeap should report BMEM_Handle pointer value
 * and specified, not derived, overall size
 *
 * Hydra_Software_Devel/21   3/26/10 6:29p albertl
 * SW7405-3979: Changed filename field from string to pointer and removed
 * slow copy operation.
 *
 * Hydra_Software_Devel/20   2/24/10 4:14p erickson
 * SW3548-2721: don't print uninitialized file and line in
 * BMEM_Dbg_DumpHeap. make BMEM_Dbg_DumpBlock private because its params
 * are internal.
 *
 * Hydra_Software_Devel/19   2/18/10 6:31p albertl
 * SW3548-2721: Changed debug module name to BMEM_DBG.  Added BKNI_Printf
 * messages in csv format.
 *
 * Hydra_Software_Devel/18   5/26/09 4:18p albertl
 * PR55389: Replaced uintptr_t with uint32_t.
 *
 * Hydra_Software_Devel/17   4/14/08 4:52p albertl
 * PR36283:  Fixed debug message to properly calculate block start and end
 * addresses for both system and local bookkeeping.
 *
 * Hydra_Software_Devel/16   12/6/07 3:23p tdo
 * PR37550: Fix coverity issues
 *
 * Hydra_Software_Devel/15   5/14/07 1:19p albertl
 * PR30621:  Fixed heaps being added to allocation list but not being
 * removed when freed in some safety configurations.
 *
 * Hydra_Software_Devel/14   1/16/07 2:41p erickson
 * PR25037: improve BMEM validate and debug messages
 *
 * Hydra_Software_Devel/13   4/5/06 2:43p albertl
 * PR20659:  Changed BDBG_WRN to BDBG_MSG.
 *
 * Hydra_Software_Devel/12   12/20/04 9:21p marcusk
 * PR13562: Fixed typo in debug statement.
 *
 * Hydra_Software_Devel/11   12/20/04 8:08p marcusk
 * PR13562: Fixed bug in BMEM_Dbg_DumpBlock() that was not taking into
 * account the front scrap size when determining the addresses of the
 * buffer.
 *
 * Hydra_Software_Devel/10   12/14/04 4:31p marcusk
 * PR13562: Updated to output the heap size, allocated size, and free size
 * when a memory allocation fails.
 *
 * Hydra_Software_Devel/9   10/7/04 5:37p pntruong
 * PR12728: Fixed -pdantic build warnings and errors.
 *
 * Hydra_Software_Devel/8   4/1/04 12:56p vsilyaev
 * PR 10201: Added memory monitor
 *
 * Hydra_Software_Devel/7   2/25/04 2:52p hongtaoz
 * PR9855: removed redundent functions BMEM_Report and BMEM_ReportVerbose;
 * being aware of that BMEM_Dbg_DumpBlock and BMEM_Dbg_DumpHeap suffice
 * heap debug requirement; avoid using floating point in BMEM_Dbg_Map;
 *
 * Hydra_Software_Devel/6   12/31/03 12:02p jasonh
 * PR 8940: Changed return type of destroyheap to void. Removed use of
 * DEBUG macro and printf.
 *
 * Hydra_Software_Devel/5   10/2/03 1:02p pntruong
 * Removed -ansi warnings and build erros.
 *
 * Hydra_Software_Devel/4   9/5/03 1:57p jasonh
 * Changed macro to use standard version.
 *
 * Hydra_Software_Devel/3   9/2/03 1:34p vadim
 * Some magnum updates.
 *
 * Hydra_Software_Devel/2   3/20/03 3:51p erickson
 * renamed all MEM_ to BMEM_
 *
 * Hydra_Software_Devel/1   3/20/03 3:24p erickson
 * initial bmem work, taken from SetTop/memorylib
 *
 ***************************************************************************/
#include "bstd.h"

#include "bmem.h"
#include "bmem_config.h"
#include "bmem_priv.h"
#include "bmem_debug.h"
#include "bkni.h"

BDBG_MODULE(BMEM_DBG);

#define BMEM_MAP_SIZE       (80*40)
#define BMEM_MAP_ADDR(addr) ((addr) * BMEM_MAP_SIZE / (pheap->pEnd - pheap->pStart + 1))

BLST_AA_TREE_GENERATE_FIRST(BMEM_P_AddrTree, BMEM_P_BlockInfo, stAddrNode)
BLST_AA_TREE_GENERATE_NEXT(BMEM_P_AddrTree, BMEM_P_BlockInfo, stAddrNode)

BLST_AA_TREE_GENERATE_FIRST(BMEM_P_SizeTree, BMEM_P_BlockInfo, stSizeNode)
BLST_AA_TREE_GENERATE_NEXT(BMEM_P_SizeTree, BMEM_P_BlockInfo, stSizeNode)


/**********************************************************************func*
 * BMEM_Dbg_Map
 *
 */
void BMEM_Dbg_Map
(
	BMEM_Handle pheap
)
{
	int               i;
	BMEM_P_BlockInfo *pbi;
	char             *ach;

	BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);

	/* Allocate memory for ach */
	ach = (char *)(BKNI_Malloc((BMEM_MAP_SIZE+1)*sizeof(char)));
	if(!ach)
	{
		return;
	}

	for(i = 0; i < BMEM_MAP_SIZE; ++i)
	{
		ach[i] = '#';
	}

	/* now by address order instead of allocated order */
/*
	for(pbi = pheap->pAllocTop; pbi != NULL; pbi = pbi->pnext)
*/
	for(pbi=BLST_AA_TREE_FIRST(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree);
		pbi != NULL;
		pbi=BLST_AA_TREE_NEXT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, pbi))
	{
		uint32_t addrStart = BMEM_P_GetAddress( pheap, pbi ) - pbi->ulFrontScrap -
			(uint32_t) pheap->pStart;
		uint32_t addrFront = BMEM_P_GetAddress(pheap, pbi) - (uint32_t)pheap->pStart;
		uint32_t addrBack = addrStart+pbi->ulSize-pbi->ulBackScrap;
		uint32_t addrEnd = addrStart+pbi->ulSize;
		uint32_t ul;

		addrStart = (uint32_t)BMEM_MAP_ADDR(addrStart);
		addrFront = (uint32_t)BMEM_MAP_ADDR(addrFront);
		addrBack = (uint32_t)BMEM_MAP_ADDR(addrBack);
		addrEnd = (uint32_t)BMEM_MAP_ADDR(addrEnd);
		for(ul = addrStart; ul < addrFront; ++ul)
		{
			ach[ul] = ',';
		}
		for(ul = addrFront+1; ul < addrBack; ++ul)
		{
			ach[ul]='=';
		}
		for(ul = addrBack; ul<addrEnd; ++ul)
		{
			ach[ul]='.';
		}
		if(ach[addrFront] == '#')
		{
			ach[addrFront] = 'a';
		}
		else
		{
			ach[addrFront]++;
		}
	}


	for(pbi=BLST_AA_TREE_FIRST(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
		pbi != NULL;
		pbi=BLST_AA_TREE_NEXT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbi))
	{
		uint32_t addrStart = BMEM_P_GetAddress( pheap, pbi ) - pbi->ulFrontScrap -
			(uint32_t) pheap->pStart;
		uint32_t addrEnd = addrStart + pbi->ulSize;
		uint32_t ul;

		addrStart = (uint32_t)BMEM_MAP_ADDR(addrStart);
		addrEnd = (uint32_t)BMEM_MAP_ADDR(addrEnd);
		for(ul = addrStart+1; ul < addrEnd; ++ul)
		{
			ach[ul] = '-';
		}


		if(ach[addrStart] == '#')
		{
			ach[addrStart] = 'a';
		}
		else
		{
			ach[addrStart]++;
		}
	}

	ach[BMEM_MAP_SIZE] = '\0';
	BDBG_MSG(("%s", ach));
	BKNI_Free((char *)ach);
}

/**********************************************************************func*
 * BMEM_Dbg_DumpBlock
 *
 */
static void BMEM_Dbg_DumpBlock(BMEM_Handle pheap, BMEM_P_BlockInfo *pbi, bool freeBlock)
{
	const char *filename;
	unsigned linenum;

	BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);
	if (freeBlock) {
		/* free block has possibly uninitialized filename/linenum */
		filename = NULL;
		linenum = 0;
	}
	else {
		/* eliminate the path which is usually unnecessary and very long */
		filename = pbi->pchFilename;
	    while (*filename) filename++;
		while (*filename != '/' && filename > pbi->pchFilename) filename--;
		if (*filename == '/') filename++;

	    linenum = pbi->ulLine;
	}

	/* This format is directly importable to Excel where you can sort by address.
	It is not DBG output. It should be defaulted off because it will always print. */
	{
	uint32_t ulFrontBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                                  BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL : BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM;
	uint8_t *addr = (uint8_t *)(BMEM_P_GetAddress( pheap, pbi ) + (uint32_t)ulFrontBookKeepingSize);
	BKNI_Printf("%p,%#x,%#x,%#x,%lu,%s,%d\n",
		pheap,
		pheap->ulOffset,
		addr,
		addr + pbi->ulSize,
		(unsigned long int)pbi->ulSize,
		filename,
		linenum);
	}

#ifdef BMEM_TRACK_FILE_AND_LINE
	if(pheap->pSafetyConfigInfo->bTrackFileAndLine)
	{
		BDBG_MSG(("(%s:%d)", filename, pbi->ulLine));
	}
#endif
	/* The front scrap area comes before the pbi data structure */
	BDBG_MSG(("%p[%#x..%#x..%#x]: [%lu][%lu][%lu] [%p][%p]",
		pbi,    /* base address of the pbi */
		pheap->ulOffset + ((uint32_t)BMEM_P_GetAddress(pheap, pbi) - pbi->ulFrontScrap - (uint32_t)pheap->pvHeap), /* start of memory allocation block */
		pbi->ulSize, /* size */
		pheap->ulOffset + ((uint32_t)BMEM_P_GetAddress(pheap, pbi) - pbi->ulFrontScrap - (uint32_t)pheap->pvHeap) + pbi->ulSize, /* end of memory allocation block */
		(unsigned long int)pbi->ulFrontScrap,
		(unsigned long int)pbi->ulSize,
		(unsigned long int)pbi->ulBackScrap,
		&pbi->stAddrNode,
		&pbi->stSizeNode));
}

/**********************************************************************func*
 * BMEM_Dbg_DumpHeap
 *
 */
void BMEM_Dbg_DumpHeap(BMEM_Handle pheap)
{
	BMEM_P_BlockInfo *pbi;
	unsigned long ulFree = 0;

	BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);
	if (!BLST_AA_TREE_FIRST(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree)) {
		BKNI_Printf("You need to set BMEM_Heap_Settings.eSafetyConfig = BMEM_SafetyConfig_eSafe to track allocations.\n");
	}
	else {
		BKNI_Printf("Allocated:\n");
		BKNI_Printf("heap,offset,addr,endaddr,totalsize,filename,line\n");
		BDBG_MSG(("pbi[start..size..end]: [fscrap][totalsize][endscrap] [nextpbi]"));
		for(pbi = BLST_AA_TREE_FIRST(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree);
			pbi != NULL;
			pbi = BLST_AA_TREE_NEXT(BMEM_P_AddrTree, &pheap->stAllocTopAddrTree, pbi))
		{
			BMEM_Dbg_DumpBlock(pheap, pbi, false);
		}
	}

	BKNI_Printf(("Free:\n"));
	BKNI_Printf("heap,offset,addr,endaddr,totalsize,filename,line\n");
	BDBG_MSG(("pbi[start..size..end]: [fscrap][totalsize][endscrap] [nextpbi]"));
	for(pbi = BLST_AA_TREE_FIRST(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree);
		pbi != NULL;
		pbi = BLST_AA_TREE_NEXT(BMEM_P_SizeTree, &pheap->stFreeTopSizeTree, pbi))
	{
		ulFree += pbi->ulSize;
		BMEM_Dbg_DumpBlock(pheap, pbi, true);
	}

	BKNI_Printf("Heap %p Summary:\n", pheap);
	BKNI_Printf("Total Size: %ld, Allocated: %ld, Free: %ld\n", pheap->zSize, pheap->ulTotalAllocated, ulFree);
}

/**********************************************************************func*
 * BMEM_Dbg_GetErrorCount
 *
 */
uint32_t BMEM_Dbg_GetErrorCount(BMEM_Handle pheap)
{
    BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);
	return pheap->ulNumErrors;
}

/**********************************************************************func*
 * BMEM_Dbg_ClearErrorCount
 *
 */
void BMEM_Dbg_ClearErrorCount(BMEM_Handle pheap)
{
    BDBG_OBJECT_ASSERT(pheap, BMEM_Heap);
	pheap->ulNumErrors = 0;
}

/* End of File */
