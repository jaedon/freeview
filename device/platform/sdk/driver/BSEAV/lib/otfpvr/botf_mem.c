/***************************************************************************
 *	   Copyright (c) 2007-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: botf_mem.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/24/12 2:52p $
 *
 * Module Description:
 *
 * Simple virtual memory module, it's used to convert between physical and virtual memory
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/otfpvr/botf_mem.c $
 * 
 * 2   2/24/12 2:52p vsilyaev
 * SW7425-2459: Added range test for address conversion
 * 
 * 1   3/31/09 7:28p vsilyaev
 * PR 50887: OTFPVR support for the Nexus video decoder
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "botf_mem.h"

void 
botf_mem_init(botf_mem *mem, uint32_t addr, void *ptr, size_t range)
{
    mem->base = (uint32_t)ptr - addr;
    mem->ptr = ptr;
    mem->addr = addr;
    mem->range = range;
    return;
}

uint32_t 
botf_mem_paddr(botf_mem_t mem, const void *ptr)
{
    BDBG_ASSERT((const uint8_t *)ptr >= mem->ptr && (const uint8_t *)ptr < (mem->ptr + mem->range));
    return (uint32_t)ptr - mem->base;
}

void *
botf_mem_vaddr(botf_mem_t mem, uint32_t addr)
{
    BDBG_ASSERT(addr >= mem->addr && addr < (mem->addr + mem->range));
    return (uint8_t *)mem->base + addr;
}


