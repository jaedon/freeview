/***************************************************************************
 *     Copyright (c) 2006-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsymtable.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 5/30/08 7:34p $
 *
 * Module Description:
 *
 * Embeddeble symbol table
 * 		
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bprofile/bsymtable.h $
 * 
 * 3   5/30/08 7:34p vsilyaev
 * PR 42356: Updated code to make it safe for 64-bit environment
 * 
 * 2   11/20/06 4:54p vsilyaev
 * PR 25997: Decoupled profiling and symbol table
 * 
 * 1   11/16/06 5:25p vsilyaev
 * PR 25997: Embeddable profiler
 * 
 *******************************************************************************/
#ifndef _BSYMTABLE_H__
#define _BSYMTABLE_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bsymbol_entry {
	uint32_t addr;
	const char *name;
} bsymbol_entry;

typedef struct bsymbol_table {
	const bsymbol_entry *table;
	size_t nentries;
	unsigned offset;
} bsymbol_table;

/* this function returns pointer to the bsymbol_table with corrected addresses */
const bsymbol_table *bsymbol_fixup(void);
/* this function lookups table map and return nearest symbol and offset to this symbol */
const char *bsymbol_lookup(const bsymbol_table *map, unsigned long addr, unsigned *offset);

#ifdef __cplusplus
}
#endif


/*
 * this macro builds a function that does a binary seatch in a sorted array ([0] smallest value)
 * it returns either index of located entry or if result is negative
 * -(off+1), where off is offset to largest address in array that is smaller
 *  then address
 */
#define B_BIN_SEARCH(name, type, key)  \
int name(const type *a, size_t nentries, uint32_t v) { \
	int right = nentries-1; int left = 0; int mid; \
	while(left <= right) { \
		mid = (left+right) / 2; \
		if (v > a[mid].key) { left = mid+1; } \
		else if (v < a[mid].key) { right = mid-1; } \
		else { return mid; } \
	} return -(left+1);  \
}



#endif /* _BSYMTABLE_H__ */

