/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_memory.c $
* $brcm_Revision: 53 $
* $brcm_Date: 9/5/12 3:08p $
*
* API Description:
*   API name: Memory
*    Specific APIs related to device memory allocation
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/src/nexus_memory.c $
* 
* 53   9/5/12 3:08p erickson
* SW7552-338: fix bad assert
* 
* 52   7/13/12 11:58a erickson
* SW7552-303: use OS-reported max_dcache_line_size instead of nexus macro
* 
* 51   7/13/12 10:37a erickson
* SW7125-1305: Don't call NEXUS_OffsetToCachedAddr() in
*  NEXUS_Heap_GetStatus() for a reserved heap (no corresponding BMEM
*  heap).
* 
* 50   7/9/12 3:48p vsilyaev
* SW7435-256: Use BLST_AAT implementation of AA Tree
* 
* 49   7/9/12 1:58p erickson
* SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
* 
* 48   6/21/12 2:55p vsilyaev
* SW7420-2315: Use AA tree for address->handle lookup
* 
* 47   6/20/12 4:41p vsilyaev
* SW7420-2315: Insert entries into the tail of list (instead of head) so
*  to exploit weakness of the unit test
* 
* 46   6/20/12 9:45a erickson
* SW7420-2085: fix NEXUS_MemoryBlock_Allocate
* 
* 45   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 44   6/11/12 12:04p erickson
* SW7435-65: fix guardBanding status
* 
* 43   5/11/12 10:57a erickson
* SW7435-170: add NEXUS_HeapStatus.guardBanding
* 
* 42   4/23/12 5:08p erickson
* SWNOOS-532: merge
* 
* SWNOOS-532/1   4/19/12 1:52p agin
* SWNOOS-532:  Support ucos and no-os.
* 
* 40   12/19/11 10:04a erickson
* SW7425-1795: add function to validate cpu accessibility of heaps and
*  addresses
* 
* 39   12/9/11 1:37p erickson
* SW7125-1121: NEXUS_Heap_GetStatus() can return some status for a
*  reserved heap
* 
* SW7125-1121/1   12/9/11 11:58a mward
* SW7125-1121:  NEXUS_Heap_GetStatus() can return some status for a
*  reserved heap.
* 
* 38   11/29/11 11:39a erickson
* SW7420-2129: get current default heap using NEXUS_P_DefaultHeap
* 
* 37   11/16/11 1:21p erickson
* SW7125-1121: add NEXUS_MEMORY_TYPE_RESERVED for placeholder heaps that
*  prevent allocations from certain regions
* 
* 36   11/15/11 1:34p erickson
* SW7425-1293: remove workaround
* 
* 35   9/15/11 1:50p erickson
* SW7425-1293: only guard band if both cached and uncached mappings are
*  available in the driver
* 
* 34   3/4/11 11:26a erickson
* SW7400-3006: add locked setting. rework so that NEXUS_Heap_Create
*  populates g_NexusCore.publicHandles.
*
* 33   2/3/11 6:37p vsilyaev
* SW7420-1441: Use local (non proxied) implementation for function that
*  flush CPU cache
*
* 32   1/20/11 2:59p erickson
* SW7408-193: add NEXUS_MemoryStatus.highWatermark and
*  NEXUS_Heap_ResetHighWatermark
*
* 31   1/19/11 5:02p erickson
* SW7422-220: add heap index for better debug info
*
* 30   12/29/10 5:03p erickson
* SW7422-2: remove 7420 from NEXUS_MEMC1_NOT_ADDRESSABLE list
*
* 29   12/23/10 4:02p erickson
* SW7422-2: define and use NEXUS_MEMC1_NOT_ADDRESSABLE for default
*  NEXUS_MemoryType for MEMC1
*
* 28   12/20/10 6:37p erickson
* SW7422-2: rework NEXUS_Heap_GetDefaultMemcSettings
*
* 26   12/20/10 2:11p erickson
* SW7422-2: redefine NEXUS_MemoryType to be bitmask for different memory
*  mapping requirements per heap, remove deprecated api's
*
* 25   10/19/10 4:19p erickson
* SW7420-1155: use NEXUS_OffsetToCachedAddr instead of
*  BMEM_Heap_ConvertAddressToCached to get a cached address. avoids BMEM
*  limitation.
*
* 24   7/14/10 10:29a erickson
* SW7405-4621: rename to NEXUS_LARGEST_CPU_CACHE_LINE without MEMC param
*
* 23   6/23/10 12:10p erickson
* SW7550-453: add NEXUS_Heap_GetHeapHandle
*
* 22   3/29/10 4:19p erickson
* SW7405-3979: nexus now defaults to BMEM_SafetyConfig_eTrack. it is just
*  as fast as eNormal, and there's no filename memcpy any more.
*
* 21   2/26/10 2:15p erickson
* SW7325-655: use BMEM_Heap_FreeCached to remove side-effect based
*  NEXUS_Core_P_AddressToHeap
*
* 20   2/25/10 9:41a erickson
* SW7420-549: add NEXUS_MemoryStatus.alignment
*
* 19   2/24/10 4:15p erickson
* SW3548-2721: add NEXUS_Heap_Dump
*
* 18   10/6/09 6:49p vsilyaev
* SW7405-3143: Return to application virtual address of the heap
*
* 17   8/25/09 12:11p erickson
* SW7420-228: remove 7420 exception
*
* PR55501/1   8/11/09 8:39p bandrews
* PR55501: put this change on a branch until we get kernel define
*
* 16   8/11/09 8:35p bandrews
* PR55501: last change for kernel mode breaks user mode, undone
*
* 15   8/7/09 10:29p bandrews
* PR55501: kernel mode support for 7420
*
* 14   4/27/09 4:21p erickson
* PR53866: fix warning
*
* 13   4/8/09 5:20p erickson
* PR53866: add NEXUS_MemoryStatus.offset
*
* 12   3/24/09 11:37a erickson
* PR50748: fix default alignment setting
*
* 11   3/17/09 2:03p erickson
* PR53326: check for out of memory condition
*
* 10   3/12/09 6:39p katrep
* PR50748: Compiler warning
*
* 9   3/9/09 2:32p erickson
* PR50748: remove nexus multi-heap management
*
* 8   1/12/09 3:44p erickson
* PR48035: set NEXUS_MemoryStatus.free
*
* 7   10/17/08 3:21p erickson
* PR47030: added index to NEXUS_Memory_GetStatus
*
* 6   10/17/08 3:08p erickson
* PR47030: added NEXUS_Memory_GetStatus for debug
*
* 5   6/30/08 1:44p erickson
* PR44165: allow no NEXUS_CORE_NUM_SECURE_HEAPS
*
* 4   5/16/08 12:04p erickson
* PR41730: expose NEXUS_Core_P_AddressToHeap in private API
*
* 3   2/26/08 7:08p jgarrett
* PR 39388: Fixing default allocation
*
* 2   2/6/08 4:37p jgarrett
* PR 39388: Changing memory interface to conform to guidelines
*
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/1   12/7/07 8:21p jgarrett
* PR 38020: Adding memory interface
*
***************************************************************************/
#include "nexus_core_module.h"
#include "nexus_base.h"
#include "nexus_types.h"
#include "nexus_memory.h"
#include "priv/nexus_core.h"
#include "bmem_debug.h"

BDBG_MODULE(nexus_memory);

BDBG_OBJECT_ID(NEXUS_Heap);

struct NEXUS_HeapImpl {
    BDBG_OBJECT(NEXUS_Heap)
    unsigned index;
    BMEM_Heap_Handle memHeap;
    NEXUS_Core_MemoryRegion settings;
    NEXUS_DisplayHeapSettings displayHeapSettings;
    bool guardBanding;
};

void NEXUS_Memory_GetDefaultAllocationSettings( NEXUS_MemoryAllocationSettings *pSettings )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

void NEXUS_Heap_GetDefaultMemcSettings( unsigned memcIndex, NEXUS_Core_MemoryRegion *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->memcIndex = memcIndex;
    /* by default full access allowed */
    pSettings->memoryType = NEXUS_MemoryType_eFull;
    
    /* driver must set pSettings->alignment based on OS report of max dcache line size */

#if BCHP_CHIP == 7405 || BCHP_CHIP == 7400 || BCHP_CHIP == 7325 || BCHP_CHIP == 7335 || \
    BCHP_CHIP == 7340 || BCHP_CHIP == 7342
#define NEXUS_MEMC1_NOT_ADDRESSABLE 1
#endif

    if (memcIndex > 0) {
#if NEXUS_MEMC1_NOT_ADDRESSABLE
        /* MEMC1 is not cpu addressable */
        pSettings->memoryType = NEXUS_MemoryType_eDeviceOnly;
        /* device heaps default to 1K alignment */
        pSettings->alignment = 1024;
#else
        /* MEMC1 is cpu addressable, but only user mode access is allowed
        This applies to 7422, 7344, 7425 and beyond. */
        pSettings->memoryType = NEXUS_MemoryType_eApplication;
#endif
    }
}

NEXUS_HeapHandle NEXUS_Heap_Create( unsigned index, BMEM_ModuleHandle mem, const NEXUS_Core_MemoryRegion *pSettings )
{
    NEXUS_HeapHandle heap = NULL;
    BMEM_Heap_Settings mem_heap_settings;
    BERR_Code rc;

    BDBG_MSG(("NEXUS_Heap_Create %d: MEMC%d, offset %#x, length %d, addr %p, cached %p",
        index, pSettings->memcIndex, pSettings->offset, pSettings->length, pSettings->pvAddr, pSettings->pvAddrCached));

    heap = BKNI_Malloc(sizeof(*heap));
    if (!heap) {
        rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        return NULL;
    }
    BKNI_Memset(heap, 0, sizeof(*heap));
    BDBG_OBJECT_SET(heap, NEXUS_Heap);
    heap->settings = *pSettings;

    (void)BMEM_Heap_GetDefaultSettings(&mem_heap_settings);

    if (pSettings->memoryType == NEXUS_MEMORY_TYPE_RESERVED) {
        /* don't create a BMEM heap for a reserved heap */
        goto skip_bmem;
    }
    
    if (pSettings->memoryType & NEXUS_MemoryType_eDriver) {
        /* BMEM_SafetyConfig_eTrack will store file/line information and will guardband every allocation,
        but will not slow down malloc/free with BMEM_Heap_Validate calls. Instead, export debug_mem=y causes
        BMEM_Heap_Validate to be called from a timer.
        guard bands require driver-context memory mapping. */
        mem_heap_settings.eSafetyConfig = BMEM_SafetyConfig_eTrack;
        mem_heap_settings.eBookKeeping = BMEM_BookKeeping_eSystem;
        heap->guardBanding = true;
    }
    else {
        /* BMEM_SafetyConfig_eFastest means no file/line information is stored and no guardbands are provided.
        if there is no driver-context memory mapping, this is required. */
        mem_heap_settings.eSafetyConfig = BMEM_SafetyConfig_eFastest;
        mem_heap_settings.eBookKeeping = BMEM_BookKeeping_eSystem;
        heap->guardBanding = false;
    }

    rc = NEXUS_Memory_P_ConvertAlignment(pSettings->alignment, &mem_heap_settings.uiAlignment);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto error;}

    mem_heap_settings.pCachedAddress = pSettings->pvAddrCached;

    if (pSettings->pvAddrCached) {
        mem_heap_settings.flush = NEXUS_FlushCache;
        mem_heap_settings.flush_isr = NEXUS_FlushCache_isr;
    }

    rc = NEXUS_P_AddMap(pSettings->offset, pSettings->pvAddrCached, pSettings->pvAddr, pSettings->length);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto error;}

    rc = BMEM_Heap_Create(mem, pSettings->pvAddr, pSettings->offset, pSettings->length, &mem_heap_settings, &heap->memHeap);
    if (rc!=BERR_SUCCESS) {
        rc = BERR_TRACE(rc);
        goto error;
    }
    BDBG_ASSERT(heap->memHeap);

skip_bmem:
    heap->index = index;
    g_NexusCore.publicHandles.nexusHeap[index] = heap;
    g_NexusCore.publicHandles.heap[index] = NEXUS_Heap_GetMemHandle(heap);
    return heap;

error:
    if (heap) {
        BKNI_Free(heap);
    }
    return NULL;
}

void NEXUS_Heap_Destroy( NEXUS_HeapHandle heap )
{
    BDBG_OBJECT_ASSERT(heap, NEXUS_Heap);
    BDBG_ASSERT(heap == g_NexusCore.publicHandles.nexusHeap[heap->index]);
    if (heap->memHeap) {
        BMEM_Heap_Destroy(heap->memHeap);
    }
    g_NexusCore.publicHandles.nexusHeap[heap->index] = NULL;
    g_NexusCore.publicHandles.heap[heap->index] = NULL;
    BDBG_OBJECT_DESTROY(heap, NEXUS_Heap);
    BKNI_Free(heap);
}

void NEXUS_Heap_GetDefaultDisplayHeapSettings( NEXUS_DisplayHeapSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->fullHdBuffers.count = 4;
    pSettings->fullHdBuffers.format = NEXUS_VideoFormat_e1080p30hz;
    pSettings->fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    pSettings->hdBuffers.format = NEXUS_VideoFormat_e1080i;
    pSettings->hdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    pSettings->sdBuffers.format = NEXUS_VideoFormat_ePalG;
    pSettings->sdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
}

void NEXUS_Heap_GetDisplayHeapSettings( NEXUS_HeapHandle heap, NEXUS_DisplayHeapSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(heap, NEXUS_Heap);
    *pSettings = heap->displayHeapSettings;
}

NEXUS_Error NEXUS_Heap_SetDisplayHeapSettings( NEXUS_HeapHandle heap, const NEXUS_DisplayHeapSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(heap, NEXUS_Heap);
    heap->displayHeapSettings = *pSettings;
    return 0;
}

NEXUS_Error NEXUS_Heap_GetStatus( NEXUS_HeapHandle heap, NEXUS_MemoryStatus *pStatus )
{
    BMEM_Heap_Handle mheap;
    BMEM_HeapInfo info;

    BDBG_OBJECT_ASSERT(heap, NEXUS_Heap);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->alignment = heap->settings.alignment;
    pStatus->memcIndex = heap->settings.memcIndex;
    pStatus->memoryType = heap->settings.memoryType;
    pStatus->offset = heap->settings.offset;
    pStatus->size = heap->settings.length;

    mheap = NEXUS_Heap_GetMemHandle(heap);
    if (!mheap) {
        /* if there is no BMEM heap, return what we have */
        return NEXUS_SUCCESS;
    }   

    /* Cannot use BMEM_Heap_ConvertAddressToCached because it will not convert all possible addresses for the heap.
    Specifically, it will not convert addresses at the start of the heap that preceed the aligned start. */
    pStatus->addr = NEXUS_OffsetToCachedAddr(pStatus->offset);

    /* get status from BMEM */
    BMEM_Heap_GetInfo(mheap, &info);
    pStatus->free = info.ulTotalFree;
    pStatus->numAllocs = info.ulNumAllocated;
    pStatus->largestFreeBlock = info.ulLargestFree;
    pStatus->highWatermark = info.ulHighWatermark;
    pStatus->guardBanding = heap->guardBanding;

    return 0;
}

NEXUS_Error NEXUS_Memory_P_ConvertAlignment(unsigned alignment, unsigned *pAlignmentExponent)
{
    unsigned i;
    if ( alignment == 0 )
    {
        /* Default alignment */
        i = 0;
    }
    else
    {
        for ( i = 0; i < (sizeof(size_t)*8); i++ )
        {
            if ( alignment == 1UL<<i )
            {
                break;
            }
        }

        if ( i >= sizeof(size_t)*8 )
        {
            BDBG_ERR(("Invalid alignment (%d) specified", alignment));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
    }
    *pAlignmentExponent = i;
    return 0;
}

BMEM_Heap_Handle NEXUS_Heap_GetMemHandle( NEXUS_HeapHandle heap  )
{
    BDBG_OBJECT_ASSERT(heap, NEXUS_Heap);
    return heap->memHeap;
}

NEXUS_HeapHandle NEXUS_Heap_GetHeapHandle( BMEM_Heap_Handle mem )
{
    unsigned i;
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (g_pCoreHandles->heap[i] == mem) {
            return g_pCoreHandles->nexusHeap[i];
        }
    }
    BDBG_ERR(("Invalid heap specified"));
    return NULL;
}

void NEXUS_Heap_Dump( NEXUS_HeapHandle heap )
{
    BDBG_OBJECT_ASSERT(heap, NEXUS_Heap);
    if (heap->memHeap) {
        BMEM_Dbg_DumpHeap(heap->memHeap);
    }
}

void NEXUS_Heap_ResetHighWatermark( NEXUS_HeapHandle heap )
{
    BMEM_Heap_Handle mem;
    BDBG_OBJECT_ASSERT(heap, NEXUS_Heap);
    mem = NEXUS_Heap_GetMemHandle(heap);
    if (mem) {
        BMEM_Heap_ResetHighWatermark(mem);
    }
}

bool NEXUS_P_CpuAccessibleHeap( NEXUS_HeapHandle heap )
{
    BDBG_OBJECT_ASSERT(heap, NEXUS_Heap);
    /* magnum requires both cached and uncached accessibility throughout. */
    return NEXUS_P_CpuAccessibleAddress(NEXUS_OffsetToCachedAddr(heap->settings.offset)) &&
           NEXUS_P_CpuAccessibleAddress(NEXUS_OffsetToUncachedAddr(heap->settings.offset));
}
 
bool NEXUS_P_CpuAccessibleAddress( void *address )
{
/* TODO: for nfe image, kernel mode must be passed in as runtime param. see also nexus_platform_core.c. */
#if NEXUS_MODE_driver || NEXUS_BASE_OS_linuxkernel || defined(NO_OS_DIAGS) || defined(UCOS_DIAGS)
    /* kernel mode address is valid */
    return ((unsigned)address >= 0x80000000);
#else
    /* user mode address is valid */
    return ((unsigned)address < 0x80000000);
#endif
}

struct NEXUS_MemoryBlock {
    NEXUS_OBJECT(NEXUS_MemoryBlock);
    BLST_Q_ENTRY(NEXUS_MemoryBlock) link;
    BLST_AA_TREE_ENTRY(NEXUS_P_MemoryBlockTree) node;
    void *mem;
    BMEM_Heap_Handle heap;
};

void NEXUS_MemoryBlock_GetDefaultAllocationSettings(NEXUS_MemoryBlockAllocationSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    return;
}

static int NEXUS_P_MemoryBlock_Compare(const struct NEXUS_MemoryBlock* node, void *addr)
{
    if((char *)addr > (char *)node->mem) {
        return 1;
    } else if(addr==node->mem) {
        return 0;
    } else {
        return -1;
    }
}

BLST_AA_TREE_GENERATE_FIND(NEXUS_P_MemoryBlockTree , void *, NEXUS_MemoryBlock, node, NEXUS_P_MemoryBlock_Compare)
BLST_AA_TREE_GENERATE_INSERT(NEXUS_P_MemoryBlockTree, void *, NEXUS_MemoryBlock, node, NEXUS_P_MemoryBlock_Compare)
BLST_AA_TREE_GENERATE_REMOVE(NEXUS_P_MemoryBlockTree, NEXUS_MemoryBlock, node)

NEXUS_MemoryBlockHandle NEXUS_MemoryBlock_Allocate(NEXUS_HeapHandle allocHeap, size_t numBytes, size_t nexusAlignment, const NEXUS_MemoryBlockAllocationSettings *pSettings)
{
    BMEM_Heap_Handle heap;
    unsigned alignment=0;
    NEXUS_Error rc;
    NEXUS_HeapHandle nexusHeap;
    NEXUS_MemoryBlockHandle block;
    void *pMem;
    BSTD_UNUSED(pSettings);

    if (numBytes <= 0) { rc = BERR_TRACE(BERR_INVALID_PARAMETER); goto err_parameter; }


    rc = NEXUS_Memory_P_ConvertAlignment(nexusAlignment, &alignment);
    if (rc) {
        BDBG_ERR(("Invalid alignment (%u) specified", nexusAlignment));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);goto err_parameter;
    }

    nexusHeap = NEXUS_P_DefaultHeap(allocHeap, NEXUS_DefaultHeapType_eAny);
    if (nexusHeap) {
        if (nexusHeap->settings.locked) { rc = BERR_TRACE(NEXUS_NOT_AVAILABLE); goto err_parameter; }
        heap = NEXUS_Heap_GetMemHandle(nexusHeap);
    } else {
        heap = g_pCoreHandles->heap[0];
    }
    if ( !heap ) { 
        BDBG_ERR(("Invalid heap specified"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);goto err_parameter; 
    }

    block = BKNI_Malloc(sizeof(*block));
    if(block==NULL) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    NEXUS_OBJECT_INIT(NEXUS_MemoryBlock, block);
    pMem = BMEM_Heap_AllocAligned(heap, numBytes, alignment, 0); 
    if(!pMem) { rc=BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);goto err_mem_alloc;}
    (void)BMEM_Heap_ConvertAddressToCached(heap, pMem, &block->mem);
    block->heap = heap;
    BLST_Q_INSERT_TAIL(&g_NexusCore.allocatedBlocks, block, link);
    BLST_AA_TREE_INSERT(NEXUS_P_MemoryBlockTree, &g_NexusCore.allocatedBlocksTree, block->mem, block);
    return block;

err_mem_alloc:
    BKNI_Free(block);
err_alloc:
err_parameter:
    return NULL;
}

static void NEXUS_MemoryBlock_P_Finalizer(NEXUS_MemoryBlockHandle block)
{
    NEXUS_OBJECT_ASSERT(NEXUS_MemoryBlock, block);
    BMEM_Heap_FreeCached(block->heap, block->mem);
    BLST_Q_REMOVE(&g_NexusCore.allocatedBlocks, block, link);
    BLST_AA_TREE_REMOVE(NEXUS_P_MemoryBlockTree, &g_NexusCore.allocatedBlocksTree, block);
    NEXUS_OBJECT_DESTROY(NEXUS_MemoryBlock, block);
    BKNI_Free(block);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_MemoryBlock, NEXUS_MemoryBlock_Free);

NEXUS_Error NEXUS_MemoryBlock_Lock(NEXUS_MemoryBlockHandle block, void **ppMemory)
{
    NEXUS_OBJECT_ASSERT(NEXUS_MemoryBlock, block);
    *ppMemory = block->mem;
    return NEXUS_SUCCESS;
}

void NEXUS_MemoryBlock_Unlock(NEXUS_MemoryBlockHandle block)
{
    NEXUS_OBJECT_ASSERT(NEXUS_MemoryBlock, block);
    BSTD_UNUSED(block);
    return;
}

NEXUS_MemoryBlockHandle NEXUS_MemoryBlock_FromAddress(void *pMemory)
{
    NEXUS_MemoryBlockHandle block;

    block = BLST_AA_TREE_FIND(NEXUS_P_MemoryBlockTree, &g_NexusCore.allocatedBlocksTree, pMemory);
    if(block) {
        BDBG_ASSERT(pMemory==block->mem);
        return block; /* bingo */
    }
    for(block=BLST_Q_FIRST(&g_NexusCore.allocatedBlocks);block;block=BLST_Q_NEXT(block,link)) {
        if(block->mem == pMemory) {
            BDBG_ASSERT(0);
            return block; /* bingo */
        }
    }
    (void)BERR_TRACE(NEXUS_INVALID_PARAMETER);
    return NULL;
}

