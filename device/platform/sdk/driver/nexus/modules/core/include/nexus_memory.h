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
* $brcm_Workfile: nexus_memory.h $
* $brcm_Revision: 29 $
* $brcm_Date: 6/20/12 10:22a $
*
* API Description:
*   API name: Memory
*    Specific APIs related to device memory allocation
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/7400/include/nexus_memory.h $
* 
* 29   6/20/12 10:22a erickson
* SW7420-2085: add attr{null_allowed=y} to NEXUS_MemoryBlock_Allocate
*  heap param
* 
* 28   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 27   5/11/12 10:57a erickson
* SW7435-170: add NEXUS_HeapStatus.guardBanding
* 
* 26   3/14/12 3:12p erickson
* SW7425-2607: use NEXUS_MAX_MEMC
* 
* 25   3/14/12 11:04a erickson
* SW7425-2607: use NEXUS_MAX_MEMC and allow NEXUS_NUM_MEMC to be
*  customized
* 
* 24   3/13/12 4:41p erickson
* SW7425-2607: remove #ifndef for macros used in API
* 
* 23   11/29/11 11:39a erickson
* SW7420-2129: get current default heap using NEXUS_P_DefaultHeap
* 
* 22   11/9/11 9:59a erickson
* SWDEPRECATED-2425: add comment
* 
* 21   4/14/11 11:59a erickson
* SW7420-1679: clean up NEXUS_Memory_Allocate using
*  attr{dealloc=NEXUS_Memory_Free}
*
* 20   2/3/11 6:37p vsilyaev
* SW7420-1441: Use local (non proxied) implementation for function that
*  flush CPU cache
*
* 19   1/20/11 2:59p erickson
* SW7408-193: add NEXUS_MemoryStatus.highWatermark and
*  NEXUS_Heap_ResetHighWatermark
*
* 18   12/27/10 11:57a erickson
* SW7422-2: increase default NEXUS_MAX_HEAPS
*
* 17   12/22/10 2:28p erickson
* SW7422-2: add support for multiple physical address regions within a
*  single MEMC
*
* 16   12/20/10 2:11p erickson
* SW7422-2: redefine NEXUS_MemoryType to be bitmask for different memory
*  mapping requirements per heap, remove deprecated api's
*
* 15   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
*
* 14   8/10/10 2:48p bandrews
* SW7335-801: allow platform features to override NEXUS_MAX_HEAPS
*
* 13   2/25/10 9:41a erickson
* SW7420-549: add NEXUS_MemoryStatus.alignment
*
* 12   2/24/10 4:15p erickson
* SW3548-2721: add NEXUS_Heap_Dump
*
* 11   10/6/09 6:49p vsilyaev
* SW7405-3143: Return to application virtual address of the heap
*
* 10   4/8/09 5:20p erickson
* PR53866: add NEXUS_MemoryStatus.offset
*
* 9   3/9/09 2:32p erickson
* PR50748: remove nexus multi-heap management
*
* 8   10/30/08 8:49p erickson
* PR47030: added NEXUS_HeapHandle and related functions
*
* 7   10/20/08 1:54p erickson
* PR47030: update comments
*
* 6   10/17/08 3:21p erickson
* PR47030: added index to NEXUS_Memory_GetStatus
*
* 5   10/17/08 3:08p erickson
* PR47030: added NEXUS_Memory_GetStatus for debug
*
* 4   10/6/08 7:08p vsilyaev
* PR 39388: Fixed attributes for NEXUS_Memory_Allocate
*
* 3   2/6/08 4:37p jgarrett
* PR 39388: Changing memory interface to conform to guidelines
*
* 2   1/31/08 5:04p vsilyaev
* PR 38682: Added attributed to functions that need special handling in
*  kernel/user proxy configuration
*
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/1   12/7/07 8:21p jgarrett
* PR 38020: Adding memory routines
*
***************************************************************************/

#ifndef NEXUS_MEMORY_H__
#define NEXUS_MEMORY_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* NEXUS_NUM_MEMC is only for internal use. */

/* NEXUS_NUM_MEMC_REGIONS is the maximum number of distinct physical addressing regions on one MEMC */
#define NEXUS_NUM_MEMC_REGIONS 2

/* NEXUS_MAX_HEAPS is the maximum number of heaps in the system, pointed to be NEXUS_HeapHandle.
A heap is required for any memory access, whether by HW or SW.
This depends on both HW capabilities & SW configuration. */
#define NEXUS_MAX_HEAPS 8

/***************************************************************************
Summary:
Memory allocation settings used in NEXUS_Memory_Allocate
***************************************************************************/
typedef struct NEXUS_MemoryAllocationSettings
{
    NEXUS_HeapHandle heap;      /* Heap from which to allocate. If set, type and bankIndex are ignored. */
    size_t alignment;           /* Alignment of this allocation in units of bytes (not in units of 2^alignment bytes).
                                   0 will select default alignment (which may not be byte-aligned). */
} NEXUS_MemoryAllocationSettings;

/**
Summary:
**/
typedef struct NEXUS_MemoryBlock *NEXUS_MemoryBlockHandle;

typedef struct NEXUS_MemoryBlockAllocationSettings {
    unsigned unused;
} NEXUS_MemoryBlockAllocationSettings;

/***************************************************************************
Summary:
Get default memory allocation settings

Description:
This function will initialize a NEXUS_MemoryBlockAllocationSettings structure to
defaults.

See Also:
NEXUS_MemoryBlock_Allocate
***************************************************************************/
void NEXUS_MemoryBlock_GetDefaultAllocationSettings(
    NEXUS_MemoryBlockAllocationSettings *pSettings   /* [out] Default Settings */
    );

/***************************************************************************
Summary:
Allocate memory from a Nexus heap

Description:
Heap memory is distinct from system or OS-controlled memory.
Nexus may implement multiple heaps. For instance, a non-UMA system may have a general heap and special picture heaps.
Some secure systems may have specialized secure heaps.

Call NEXUS_Memory_Free to free whatever memory was allocated.
***************************************************************************/
NEXUS_MemoryBlockHandle NEXUS_MemoryBlock_Allocate( /* attr{destructor=NEXUS_MemoryBlock_Free} */
    NEXUS_HeapHandle heap,      /* attr{null_allowed=y} Heap from which to allocate. If NULL, default heap is selected. */
    size_t numBytes,            /* Number of bytes to allocate. */
    size_t alignment,           /* Alignment of this allocation in units of bytes (not in units of 2^alignment bytes).
                                   0 will select default alignment (which may not be byte-aligned). */
    const NEXUS_MemoryBlockAllocationSettings *pSettings    /* attr{null_allowed=y} may be NULL for default settings */
    );

/***************************************************************************
Summary:
Free device memory allocated with NEXUS_MemoryBlock_Allocate
***************************************************************************/
void NEXUS_MemoryBlock_Free(
    NEXUS_MemoryBlockHandle memoryBlock
    );

/***************************************************************************
Summary:
Returns address of allocated block
***************************************************************************/
NEXUS_Error NEXUS_MemoryBlock_Lock( 
    NEXUS_MemoryBlockHandle memoryBlock,
    void **ppMemory                                     /* [out] attr{memory=cached} pointer to memory */
    );

/***************************************************************************
Summary:
Releases address of allocated block
***************************************************************************/
void NEXUS_MemoryBlock_Unlock( 
    NEXUS_MemoryBlockHandle memoryBlock
    );

/***************************************************************************
Summary:
Returns existing NEXUS_MemoryBlockHandle based on address
***************************************************************************/
NEXUS_MemoryBlockHandle NEXUS_MemoryBlock_FromAddress( 
    void *pMemory                                     /* [attr{memory=cached} pointer to memory */
    );


/***************************************************************************
Summary:
Get default memory allocation settings

Description:
This function will initialize a NEXUS_MemoryAllocationSettings structure to
defaults.

See Also:
NEXUS_Memory_Allocate
***************************************************************************/
void NEXUS_Memory_GetDefaultAllocationSettings(
    NEXUS_MemoryAllocationSettings *pSettings   /* [out] Default Settings */
    );

/***************************************************************************
Summary:
Allocate memory from a Nexus heap

Description:
Heap memory is distinct from system or OS-controlled memory.
Nexus may implement multiple heaps. For instance, a non-UMA system may have a general heap and special picture heaps.
Some secure systems may have specialized secure heaps.

Call NEXUS_Memory_Free to free whatever memory was allocated.
***************************************************************************/
NEXUS_Error NEXUS_Memory_Allocate( /* attr{local=true}  */
    size_t numBytes,                                    /* Number of bytes to allocate */
    const NEXUS_MemoryAllocationSettings *pSettings,    /* attr{null_allowed=y} may be NULL for default settings */
    void **ppMemory                                     /* [out] attr{memory=cached} pointer to memory that has been allocated */
    );

/***************************************************************************
Summary:
Free device memory allocated with NEXUS_Memory_Allocate
***************************************************************************/
void NEXUS_Memory_Free( /* attr{local=true}  */
    void *pMemory  /* attr{memory=cached} */
    );

/***************************************************************************
Summary:
Flush the data cache for the specified memory.

Description:
NEXUS_Memory_FlushCache is equivalent to NEXUS_FlushCache, defined in nexus_base_os.h. 
Both can be used by the application.
***************************************************************************/
void NEXUS_Memory_FlushCache( /* attr{local=true}  */
    void *pMemory,      /* attr{memory=cached} */
    size_t numBytes     /* Number of bytes to flush */
    );

/***************************************************************************
Summary:
Status of memory heap returned by NEXUS_Memory_GetStatus
***************************************************************************/
typedef struct NEXUS_MemoryStatus
{
    unsigned memoryType; /* see NEXUS_MEMORY_TYPE bitmasks and NEXUS_MemoryType macros in nexus_types.h */
    unsigned memcIndex;
    uint32_t offset; /* physical offset of the heap */
    void    *addr; /*  attr{memory=cached} virtual address of the heap */
    unsigned size; /* total size of the heap in bytes */
    unsigned free; /* total free space in the heap in bytes. be aware that you cannot
                      necessarily allocate this in one block. see largestFreeBlock. */
    unsigned alignment; /* Minimum alignment for this heap in units of bytes (not in units of 2^alignment bytes). */
    unsigned numAllocs; /* total number of allocations in the heap */
    unsigned largestFreeBlock; /* the largest free block that can be allocated. this
                      number assumes byte alignment (NEXUS_MemoryAllocationSettings.alignment == 0).
                      any larger alignment will reduce the actual largest allocation possible.  */
    unsigned highWatermark; /* most bytes allocated since system-init or since last NEXUS_Heap_ResetHighWatermark. */
    bool guardBanding; /* returns true if this heap has guardbands. guardbands can help find memory overruns, but at the cost of lower performance. */
} NEXUS_MemoryStatus;

/***************************************************************************
Summary:
Get the current status of a memory heap
***************************************************************************/
NEXUS_Error NEXUS_Heap_GetStatus(
    NEXUS_HeapHandle heap,
    NEXUS_MemoryStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
Get default Display module heap settings
***************************************************************************/
void NEXUS_Heap_GetDefaultDisplayHeapSettings(
    NEXUS_DisplayHeapSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
Get current Display module heap settings
***************************************************************************/
void NEXUS_Heap_GetDisplayHeapSettings(
    NEXUS_HeapHandle heap,
    NEXUS_DisplayHeapSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
Set new Display module heap settings
***************************************************************************/
NEXUS_Error NEXUS_Heap_SetDisplayHeapSettings(
    NEXUS_HeapHandle heap,
    const NEXUS_DisplayHeapSettings *pSettings
    );

/***************************************************************************
Summary:
Print heap allocations and free blocks to the console
***************************************************************************/
void NEXUS_Heap_Dump(
    NEXUS_HeapHandle heap
    );

/***************************************************************************
Summary:
Reset NEXUS_MemoryStatus.highWatermark
***************************************************************************/
void NEXUS_Heap_ResetHighWatermark(
    NEXUS_HeapHandle heap
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_MEMORY_H__ */

