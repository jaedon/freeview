/***************************************************************************
*     (c)2008-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_map.c $
* $brcm_Revision: 7 $
* $brcm_Date: 5/8/12 5:33p $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/linuxkernel/nexus_map.c $
* 
* 7   5/8/12 5:33p erickson
* SW7435-27: add webcpu support
* 
* 6   2/1/12 12:27p erickson
* SW7346-636: if BMEM overruns range, we must fail the system and fix the
*  bug.
* 
* 5   1/13/12 6:02p erickson
* SW7346-636: add BDBG_ERR for bad allocation
* 
* 4   7/1/11 11:20a erickson
* SW7346-187: consolidate code for MIPS kernel mode test. make
*  nexus_map.c generic.
* 
* 3   5/4/11 1:51p erickson
* SW7346-187: add NEXUS_BASE_OS_linuxkernel test
* 
* 2   3/3/11 10:54a erickson
* SW7422-310: use default BMEM alignment
*
* 1   3/2/11 10:46a erickson
* SW7422-310: dynamic fake address mapping scheme using nexus_map.c
*
***************************************************************************/
#include "nexus_map.h"
#include "blst_list.h"

BDBG_MODULE(nexus_map);

#define TEST_APP 0
#if TEST_APP
#undef BDBG_MSG
#define BDBG_MSG(X) do {printf X; printf("\n");} while (0)
#endif

/**
n_map_memory() is a wrapper around fake + real memory mapping.
fake mapping is handled with a simple, private allocator.
it is totally separate from the real BMEM heaps created by nexus_core.
**/

struct nexus_p_alloc
{
    BLST_D_ENTRY(nexus_p_alloc) link;
    unsigned offset, size;
};

static struct {
    struct nexus_map_settings settings;
    BLST_D_HEAD(nexus_p_free_list, nexus_p_alloc) free_list;
    BLST_D_HEAD(nexus_p_alloc_list, nexus_p_alloc) alloc_list;
} g_fake;

void nexus_p_get_default_map_settings(struct nexus_map_settings *p_settings)
{
    BKNI_Memset(p_settings, 0, sizeof(*p_settings));
}

int nexus_p_init_map(const struct nexus_map_settings *p_settings)
{
    struct nexus_p_alloc *node;
    if (!p_settings->size) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (BLST_D_FIRST(&g_fake.free_list)) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    if (BLST_D_FIRST(&g_fake.alloc_list)) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    g_fake.settings = *p_settings;

    node = (struct nexus_p_alloc *)BKNI_Malloc(sizeof(*node));
    if (!node) {
        return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
    }
    node->offset = p_settings->offset;
    node->size = p_settings->size;
    BLST_D_INSERT_HEAD(&g_fake.free_list, node, link);

    return 0;
}

void nexus_p_uninit_map(void)
{
    struct nexus_p_alloc *node;
    while ((node = BLST_D_FIRST(&g_fake.free_list))) {
        BLST_D_REMOVE(&g_fake.free_list, node, link);
        BKNI_Free(node);
    }
    while ((node = BLST_D_FIRST(&g_fake.alloc_list))) {
        BLST_D_REMOVE(&g_fake.alloc_list, node, link);
        BKNI_Free(node);
    }
    BKNI_Memset(&g_fake, 0, sizeof(g_fake));
}

static void *nexus_p_alloc_fake(unsigned size)
{
    struct nexus_p_alloc *node;
    BDBG_MSG(("alloc %d", size));
    for (node = BLST_D_FIRST(&g_fake.free_list); node; node = BLST_D_NEXT(node, link)) {
        if (node->size == size) {
            /* take the whole thing */
            BLST_D_REMOVE(&g_fake.free_list, node, link);
            BLST_D_INSERT_HEAD(&g_fake.alloc_list, node, link);
            BDBG_MSG(("  result: %p", (void*)node->offset));
            return (void*)node->offset;
        }
        else if (node->size > size) {
            /* split it */
            unsigned offset = node->offset;

            /* reduce the free entry */
            node->size -= size;
            node->offset += size;

            /* and add an alloc entry */
            node = (struct nexus_p_alloc *)BKNI_Malloc(sizeof(*node));
            if (!node) {
                BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
                return NULL;
            }
            node->offset = offset;
            node->size = size;
            BLST_D_INSERT_HEAD(&g_fake.alloc_list, node, link);

            BDBG_MSG(("  result: %p", (void*)node->offset));
            return (void*)node->offset;
        }
    }
    BDBG_ERR(("unable to allocate %d", size));
    return NULL;
}

static void nexus_p_free_fake(void *addr)
{
    struct nexus_p_alloc *node;
    BDBG_MSG(("free %p", addr));
    for (node = BLST_D_FIRST(&g_fake.alloc_list); node; node = BLST_D_NEXT(node, link)) {
        if (node->offset == (unsigned)addr) {
            struct nexus_p_alloc *temp;
            BLST_D_REMOVE(&g_fake.alloc_list, node, link);
            /* merge with an adjacent free entry */
            for (temp = BLST_D_FIRST(&g_fake.free_list); temp; temp = BLST_D_NEXT(temp, link)) {
                if (node->offset + node->size == temp->offset) {
                    temp->offset -= node->size;
                    temp->size += node->size;
                    BKNI_Free(node);
                    return;
                }
                else if (node->offset == temp->offset + temp->size) {
                    temp->size += node->size;
                    BKNI_Free(node);
                    return;
                }
            }
            /* if no merge possible, just move to free list */
            if (!temp) {
                BLST_D_INSERT_HEAD(&g_fake.free_list, node, link);
            }
            return;
        }
    }
    BDBG_ERR(("free of bad fake addr %p", addr));
}

void *nexus_p_map_memory(unsigned long offset, unsigned long length, NEXUS_MemoryMapType type)
{
    switch (type) {
    case NEXUS_MemoryMapType_eFake:
        {
            void *addr = nexus_p_alloc_fake(length);
            if ((unsigned)addr < g_fake.settings.offset || (unsigned)addr + length > g_fake.settings.offset + g_fake.settings.size) {
                BDBG_ERR(("fake address outside of bounds: bounds %08x %08x, alloc %08x %08x", 
                    g_fake.settings.offset, g_fake.settings.size, 
                    addr, length));
                BKNI_Fail(); /* we cannot go on. the code must be fixed. */
            }    
            return addr;
        }
    case NEXUS_MemoryMapType_eCached:
        return (g_fake.settings.mmap)(offset, length, true);
    case NEXUS_MemoryMapType_eUncached:
        return (g_fake.settings.mmap)(offset, length, false);
    default:
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
}

void nexus_p_unmap_memory( void *pMem, unsigned long length )
{
    if ((unsigned)pMem >= g_fake.settings.offset && (unsigned)pMem < g_fake.settings.offset + g_fake.settings.size) {
        nexus_p_free_fake(pMem);
    }
    else {
        (g_fake.settings.munmap)(pMem, length);
    }
}

#if TEST_APP
int test_nexus_map(void)
{
    struct nexus_map_settings settings;
    int rc;
    unsigned i, offset;
    struct {
        void *addr;
        unsigned offset;
        unsigned size;
#define TEST_ALLOCS 100
    } allocs[TEST_ALLOCS];
    
    nexus_p_get_default_map_settings(&settings);
    settings.offset = 0x00000000 + 4096;
    settings.size = 0x80000000 - 4096;
    rc = nexus_p_init_map(&settings);
    BDBG_ASSERT(!rc);
    
    offset = 0;
    for (i=0;i<TEST_ALLOCS;i++) {
        allocs[i].addr = NULL; /* no alloc */
        allocs[i].offset = offset;
        allocs[i].size = 1+rand()%100000;
        offset += allocs[i].size;
    }
    
    while (1) {
        i = rand() % TEST_ALLOCS;
        if (allocs[i].addr) {
            if (rand() % 2) {
                nexus_p_unmap_memory(allocs[i].addr, allocs[i].size);
                allocs[i].addr = NULL;
            }
        }
        else {
            allocs[i].addr = nexus_p_map_memory(allocs[i].offset, allocs[i].size, NEXUS_MemoryMapType_eFake);
        }
        
        if (rand()%100 == 0) {
            for (i=0;i<TEST_ALLOCS;i++) {
                printf("%d: %p %d %d\n",i, allocs[i].addr, allocs[i].offset, allocs[i].size);
            }
        }
    }
    
    nexus_p_uninit_map();
    
    return 0;
}

#endif
