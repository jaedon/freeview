/***************************************************************************
 *	   Copyright (c) 2007-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: botf_mem.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/24/12 2:52p $
 *
 * Module Description:
 *
 * Simple virtual memory module, it's used to convert between physical and virtual memory
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/otfpvr/botf_mem.h $
 * 
 * 2   2/24/12 2:52p vsilyaev
 * SW7425-2459: Added range test for address conversion
 * 
 * 1   3/31/09 7:28p vsilyaev
 * PR 50887: OTFPVR support for the Nexus video decoder
 * 
 *
 ***************************************************************************/

#ifndef BOTF_MEM_H_
#define BOTF_MEM_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct botf_mem {
    /* this is virtuall addrees corresponding to the physical address 0, it's used to convert from physicall to virtual (CPU) addresses */
    int32_t base; /* base0 is just a cheap way to convert from address to offset, offset = address - base0 and address = offset + base0 */
    uint32_t addr;/* used for debugging */
    const uint8_t *ptr;/* used for debugging */
    size_t range; /* used for debugging */
} botf_mem;

typedef const struct botf_mem *botf_mem_t;

void botf_mem_init(botf_mem *mem, uint32_t addr, void *ptr, size_t range);
uint32_t botf_mem_paddr(botf_mem_t mem, const void *ptr);
void *botf_mem_vaddr(botf_mem_t mem, uint32_t addr);

#ifdef __cplusplus
}
#endif

#endif /* BOTF_MEM_H_ */

