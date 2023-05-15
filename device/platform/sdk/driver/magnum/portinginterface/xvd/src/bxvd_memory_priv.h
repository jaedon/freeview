/***************************************************************************
 *    Copyright (c) 2004-2008, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_memory_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 4/25/08 1:02p $
 *
 * Module Description:
 *	 This module contains the prototypes for the XVD memory sub-manager.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_memory_priv.h $
 * 
 * Hydra_Software_Devel/2   4/25/08 1:02p pblanco
 * PR42177: Code review changes
 * 
 * Hydra_Software_Devel/1   8/13/07 12:31p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/11   6/6/07 1:14p pblanco
 * PR29915: Increased maximum region count to 17 to account for possible
 * still channel allocation.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/10   5/21/07 3:37p pblanco
 * PR29915: Changed API signatures for BXVD_P_Memory_Dump and
 * BXVD_P_Memory_AllocateAtIndex.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/9   5/18/07 12:58p pblanco
 * PR29915: Added new protection parameter to BXVD_Open.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/8   5/2/07 2:29p pblanco
 * PR29915: Added new function prototype for allocate at specified region
 * index.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/7   5/1/07 4:04p pblanco
 * PR29915: Added prototypes for new APIs.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/6   4/27/07 1:44p pblanco
 * PR29915: Added prototypes for new APIs.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/5   4/26/07 3:17p pblanco
 * PR29915: Updated function prototype for BXVD_P_Memory_Dump and added
 * associated enum.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/4   4/24/07 12:21p pblanco
 * PR29915: Fix for the allocator routine and additional coding.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/3   4/19/07 3:14p pblanco
 * PR29915: Completed initial implementation.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/2   4/19/07 12:07p pblanco
 * PR29915: More code development.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/1   4/17/07 2:19p pblanco
 * PR29915: Adding to source control.
 * 
 ****************************************************************************/
#ifndef _BXVD_MEMORY_PRIV_H_
#define _BXVD_MEMORY_PRIV_H_

#include "bstd.h"
#include "bmem.h"
#include "bxvd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BXVD_P_MEMORY_DEBUG_FUNCS 0

#define BXVD_P_MEMORY_MAX_ALLOCATIONS 17
#define BXVD_P_MEMORY_FREE_REGION_INDEX (-1)

typedef int32_t BXVD_P_Memory_RegionIndex;

typedef enum BXVD_P_Memory_DumpFlag
{
   BXVD_P_Memory_DumpFlag_eAll,
   BXVD_P_Memory_DumpFlag_eAllocatedOnly
} BXVD_P_Memory_DumpFlag;

typedef enum BXVD_P_Memory_Protection
{
   BXVD_P_Memory_Protection_eProtect,
   BXVD_P_Memory_Protection_eDontProtect
} BXVD_P_Memory_Protection;

typedef struct BXVD_P_AllocatedRegion
{
      bool                       bRegionFree;
      BXVD_P_Memory_RegionIndex  iRegionIndex;
      uint32_t                   uiRegionAlignment;

      void                       *pvRegionVirtAddr;
      uint32_t                   uiRegionPhyAddr;
      uint32_t                   uiRegionSize;
      uint32_t                   uiRegionTag;
} BXVD_P_AllocatedRegion;

/*
 * XVD memory sub-manager context structure
 */
typedef struct BXVD_P_Memory
{
      BXVD_Handle            hXvd;
      BMEM_ModuleHandle      hMemModule;
      BMEM_Heap_Handle       hMem;
      uint32_t               uiRegionCount;
      uint32_t               uiHeapSize;
      void                   *uiHeapBaseAddr;
      BXVD_P_AllocatedRegion aAllocatedRegions[BXVD_P_MEMORY_MAX_ALLOCATIONS];
} BXVD_P_Memory;


/*
 * XVD memory sub-manager context handle
 */
typedef struct BXVD_P_Memory *BXVD_P_MemoryHandle;

/***************************************************************************
Summary:
  Open a BXVD_P_Memory instance.

Description:
  This API is used to open an instance of the BXVD_Memory sub-manager.

Returns:
  BERR_SUCCESS 

See Also:
  BXVD_P_Memory_Close
  BXVD_P_Memory_Allocate
  BXVD_P_Memory_Free
  BXVD_P_Memory_GetDecContextMem
  BXVD_P_Memory_GetCabacMem
  BXVD_P_Memory_GetPicBufMem
****************************************************************************/
BERR_Code BXVD_P_Memory_Open(BXVD_Handle hXvd,
                             BXVD_P_MemoryHandle *phXvdMem,
                             void *pvAddr,
                             uint32_t uiPhyAddr,
                             uint32_t uiSize,
                             BXVD_P_Memory_Protection eProtect);

/***************************************************************************
Summary:
  Close a BXVD_P_Memory instance.

Description:
  This API is used to close an instance of the BXD_Memory sub-manager opened
  via a BXVD_P_Memory_Open call.

Returns:
	BERR_SUCCESS 

See Also:
  BXVD_P_Memory_Open

****************************************************************************/
BERR_Code BXVD_P_Memory_Close(BXVD_P_MemoryHandle hXvdMem);

/***************************************************************************
Summary:
  Allocates a region of memory.

Description:
  Allocate a region of memory of size uiAllocationSize and alignment
  uiAlignment. An optional user defined identification tag can be supplied
  via the uiRegionTag parameter but this should not be confused with the
  allocator supplied region index.

Returns:
  Pointer to allocated memory or NULL on error. 

See Also:
  BXVD_P_Memory_Open
  BXVD_P_Memory_AllocateAtIndex
  BXVD_P_Memory_Free

****************************************************************************/
void *BXVD_P_Memory_Allocate(BXVD_P_MemoryHandle hXvdMem,
                             uint32_t uiAllocationSize,
                             uint32_t uiAlignment,
                             uint32_t uiRegionTag);

/***************************************************************************
Summary:
  Free a heap region based on its address.

Description:
  Free a region of memory allocated by a BXVD_P_Memory_Allocate call. This
  API frees the region and returns it to the free region pool.

Returns:
  BERR_SUCCESS 

See Also:
  BXVD_P_Memory_Open
  BXVD_P_Memory_Allocate
  BXVD_P_Memory_AllocateAtIndex

****************************************************************************/
BERR_Code BXVD_P_Memory_Free(BXVD_P_MemoryHandle hXvdMem,
                             void *pvAddr);

/***************************************************************************
Summary:
  Dump a memory region.

Description:
  This function dumps diagnostic information about the region or regions
  referred to by hXvdMem. 
  If the eDumpFlag is set to BXVD_P_Memory_DumpFlag_eAll, all regions will
  be dumped regardless of their allocation status. If the eDumpFlag is set
  to BXVD_P_Memory_DumpFlag_eAllocatedOnly, just the regions that have been
  allocated will be dumped.

Returns:
  Nothing

See Also:
  BXVD_P_Memory_Open
  BXVD_P_Memory_Allocate
  BXVD_P_Memory_AllocateAtIndex
****************************************************************************/
void BXVD_P_Memory_Dump(BXVD_P_MemoryHandle hXvdMem,
                             BXVD_P_Memory_DumpFlag eDumpFlag);

/***************************************************************************
Summary:
  Return a region's allocation status given its region index.

Description:
  This function will return a region's allocation status based on its index.

Returns:
  BERR_SUCCESS
  BERR_INVALID_PARAMETER

See Also:
  BXVD_P_Memory_Allocate
  BXVD_P_Memory_GetRegionIndex
****************************************************************************/
BERR_Code BXVD_P_Memory_IsRegionAllocated(BXVD_P_MemoryHandle hXvdMem,
                                          BXVD_P_Memory_RegionIndex regionIndex,
                                          bool *pbAllocated);

#if BXVD_P_MEMORY_DEBUG_FUNCS

/***************************************************************************
Summary:
  Allocates a region of memory at a specific region index.

Description:
  Allocate a region of memory of size uiAllocationSize and alignment
  uiAlignment at the region index specifiged by iRegionIndex. If the
  region is already allocated, a NULL address pointer is returned.
  An optional user defined identification tag can be supplied via the 
  uiRegionTag parameter but this should not be confused with the user
  supplied region index.

Returns:
  Pointer to allocated memory or NULL on error. 

See Also:
  BXVD_P_Memory_Open
  BXVD_P_Memory_Allocate
  BXVD_P_Memory_Free

****************************************************************************/
void *BXVD_P_Memory_AllocateAtIndex(BXVD_P_MemoryHandle hXvdMem,
                                    BXVD_P_Memory_RegionIndex iRegionIndex,
                                    uint32_t uiAllocationSize,
                                    uint32_t uiAlignment,
                                    uint32_t uiRegionTag);

/***************************************************************************
Summary:
  Free a heap region based on its region index.

Description:
  Free a region of memory allocated by a BXVD_P_Memory_Allocate call. This
  API frees the region and returns it to the free region pool.

Returns:
  BERR_SUCCESS 

See Also:
  BXVD_P_Memory_Open
  BXVD_P_Memory_Allocate
  BXVD_P_Memory_AllocateAtIndex

****************************************************************************/
BERR_Code BXVD_P_Memory_FreeByIndex(BXVD_P_MemoryHandle hXvdMem,
                                    BXVD_P_Memory_RegionIndex iRegionIndex);

/***************************************************************************
Summary:
  Return a region's virtual address given its region index.

Description:
  This function will return a region's virtual address based on its region
  index. If an out of range or invalid index is given, the function will
  return NULL.

Returns:
  Region's virtual address or NULL if invalid.

See Also:
  BXVD_P_Memory_Allocate
  BXVD_P_Memory_GetRegionIndex
****************************************************************************/
void *BXVD_P_Memory_GetVirtAddrByIndex(BXVD_P_MemoryHandle hXvdMem,
                                       BXVD_P_Memory_RegionIndex iRegionIndex);

/***************************************************************************
Summary:
  Return a region's index given its virtual address.

Description:
  This function will return a region's index based on its virtual address.
  If an invalid address is given, -1 is returned.

Returns:
  Region's index or -1 if invalid.

See Also:
  BXVD_P_Memory_Allocate
  BXVD_P_Memory_GetVirtAddrByIndex
****************************************************************************/
BXVD_P_Memory_RegionIndex BXVD_P_Memory_GetRegionIndex(
   BXVD_P_MemoryHandle hXvdMem,
   void *pvAddr);

/***************************************************************************
Summary:
  Return a region's size given its region index.

Description:
  This function will return a region's allocated size based on its index.
  If the region is not allocated zero is returned

Returns:
  BERR_SUCCESS
  BERR_INVALID_PARAMETER

See Also:
  BXVD_P_Memory_Allocate
  BXVD_P_Memory_GetRegionIndex
****************************************************************************/
BERR_Code BXVD_P_Memory_GetRegionSize(BXVD_P_MemoryHandle hXvdMem,
                                      BXVD_P_Memory_RegionIndex regionIndex,
                                      uint32_t *puiRegionSize);

#endif /* BXVD_P_MEMORY_DEBUG_FUNCS */

#ifdef __cplusplus
}
#endif

#endif /* _BXVD_MEMORY_PRIV_H_ */
