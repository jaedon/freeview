/***************************************************************************
 *     Copyright (c) 2001-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmem_debug_wince.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 6/15/12 10:03a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /brcm_wince/97400/Private/BMSIPTV/magnum/basemodules/mem/wince/bmem_debug_wince.c $
 * 
 * Hydra_Software_Devel/4   6/15/12 10:03a rrlee
 * SW7405-5628: Update to CL2.2
 * 
 * Hydra_Software_Devel/3   12/2/11 5:17p ptimariu
 * SW7405-3705: perform file and line traversal/printout only if flag
 * enabled
 * 
 * Hydra_Software_Devel/2   11/11/10 11:01a katrep
 * SW7405-4959:update for Phase 75
 * 
 * Hydra_Software_Devel/1   11/10/10 6:17p katrep
 * SW7405-3705:add wince specific bmem_debug_wince.c
 * 
 *
 ***************************************************************************/
#include "bstd.h"
#include "bmem.h"
#include "bmem_config.h"
#include "bmem_priv.h"
#include "bmem_debug.h"
#include "bkni.h"

#if defined(_WIN32_WCE)
#include <windows.h>
#endif

BDBG_MODULE(BMEM_DEBUG_WINCE);

#define BMEM_MAP_SIZE       (80*40)
#define BMEM_MAP_ADDR(addr) ((addr) * BMEM_MAP_SIZE / (pheap->pEnd - pheap->pStart))
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

	for(pbi = pheap->pAllocTop; pbi != NULL; pbi = pbi->pnext)
	{
		uintptr_t addrStart = BMEM_P_GetAddress( pheap, pbi ) - pbi->ulFrontScrap -
			(uintptr_t) pheap->pStart;
		uintptr_t addrFront = BMEM_P_GetAddress(pheap, pbi) - (uintptr_t)pheap->pStart;
		uintptr_t addrBack = addrStart+pbi->ulSize-pbi->ulBackScrap;
		uintptr_t addrEnd = addrStart+pbi->ulSize;
		uintptr_t ul;

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

	for(pbi = pheap->pFreeTop; pbi != NULL; pbi = pbi->pnext)
	{
		uintptr_t addrStart = BMEM_P_GetAddress( pheap, pbi ) - pbi->ulFrontScrap -
			(uintptr_t) pheap->pStart;
		uintptr_t addrEnd = addrStart + pbi->ulSize;
		uintptr_t ul;

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
	RETAILMSG(1, (L"%S\n", ach));
	BKNI_Free((char *)ach);
}

/**********************************************************************func*
 * BMEM_Dbg_DumpBlock_Verbose
 *
 */
void BMEM_Dbg_DumpBlock_Verbose(BMEM_Handle pheap, BMEM_P_BlockInfo *pbi)
{
#if defined(_WIN32_WCE)
    __try {
#endif

#ifdef BMEM_TRACK_FILE_AND_LINE
	const char *filename = pbi->pchFilename;
	if(pheap->pSafetyConfigInfo->bTrackFileAndLine) {
		/* eliminate the path which is usually unnecessary and very long */
		while (*filename) filename++;
		while (*filename != '/' && filename > pbi->pchFilename) filename--;
		if (*filename == '/') filename++;
	}
#endif

#ifdef BMEM_TRACK_EXCEL_STYLE

	/* This format is directly importable to Excel where you can sort by address. 
	It is not DBG output. It should be defaulted off because it will always print. */
	{
	uint32_t ulFrontBookKeepingSize = (pheap->eBookKeeping == BMEM_BOOKKEEPING_LOCAL) ?
	                                  BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL : BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM;
	uint8_t *addr = BMEM_P_GetAddress( pheap, pbi ) + (uintptr_t)ulFrontBookKeepingSize;
#ifdef BMEM_TRACK_FILE_AND_LINE
		if(pheap->pSafetyConfigInfo->bTrackFileAndLine) {
			BKNI_Printf("%#x,%#x,%lu,%s,%d\n", 
				addr, addr + pbi->ulSize, (unsigned long int)pbi->ulSize, filename, pbi->ulLine);
		}
#endif
	}

#else /* BMEM_TRACK_EXCEL_STYLE */

#ifdef BMEM_TRACK_FILE_AND_LINE
	if(pheap->pSafetyConfigInfo->bTrackFileAndLine)
	{
		RETAILMSG(1, (L"(%S:%d) ", filename, pbi->ulLine));
	}
#endif
    /* The front scrap area comes before the pbi data structure */
	RETAILMSG(1, (L"%p[%#x..%#x..%#x]: [%lu][%lu][%lu]  [%p]\n",
		pbi,    /* base address of the pbi */
		pheap->ulOffset + ((uint32_t)BMEM_P_GetAddress(pheap, pbi) - pbi->ulFrontScrap - (uint32_t)pheap->pvHeap), /* start of memory allocation block */
		pbi->ulSize, /* size */
		pheap->ulOffset + ((uint32_t)BMEM_P_GetAddress(pheap, pbi) - pbi->ulFrontScrap - (uint32_t)pheap->pvHeap) + pbi->ulSize, /* end of memory allocation block */
		(unsigned long int)pbi->ulFrontScrap,
		(unsigned long int)pbi->ulSize,
		(unsigned long int)pbi->ulBackScrap,
		pbi->pnext));
#endif
#if defined(_WIN32_WCE)
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    	RETAILMSG(1, (L"Invalid PBI=%p\n", pbi));
    }
#endif

    BSTD_UNUSED(pheap);
    BSTD_UNUSED(pbi);
}

/**********************************************************************func*
 * BMEM_Dbg_DumpHeap
 *
 */
void BMEM_Dbg_DumpHeap(BMEM_Handle pheap)
{
	BMEM_P_BlockInfo *pbi;
    unsigned long ulHeapSize = 0;
    unsigned long ulAllocated = 0;
    unsigned long ulFree = 0;

	RETAILMSG(1,(L"pbi[start..size..end]: [fscrap][totalsize][endscrap] [nextpbi]\n"));

	RETAILMSG(1, (L"Allocated:\n"));
	for(pbi = pheap->pAllocTop; pbi != NULL; pbi = pbi->pnext)
	{
        ulAllocated += pbi->ulSize;
		BMEM_Dbg_DumpBlock_Verbose(pheap, pbi);
	}

	RETAILMSG(1,(L"Free:\n"));
	for(pbi = pheap->pFreeTop; pbi != NULL; pbi = pbi->pnext)
	{
        ulFree += pbi->ulSize;
		BMEM_Dbg_DumpBlock_Verbose(pheap, pbi);
	}

    ulHeapSize = ulAllocated + ulFree;

	RETAILMSG(1, (L"Heap Summary:"));
    RETAILMSG(1, (L"Total Size: %ld, Allocated: %ld, Free: %ld\n", ulHeapSize, ulAllocated, ulFree));    
}

/**********************************************************************func*
 * BMEM_Dbg_GetErrorCount
 *
 */
uint32_t BMEM_Dbg_GetErrorCount(BMEM_Handle pheap)
{
	return pheap->ulNumErrors;
}

/**********************************************************************func*
 * BMEM_Dbg_ClearErrorCount
 *
 */
void BMEM_Dbg_ClearErrorCount(BMEM_Handle pheap)
{
	pheap->ulNumErrors = 0;
}

/* End of File */
