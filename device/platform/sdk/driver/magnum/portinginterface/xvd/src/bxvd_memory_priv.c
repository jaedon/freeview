/***************************************************************************
 *    Copyright (c) 2004-2011, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_memory_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 7/20/11 3:07p $
 *
 * Module Description:
 *	 This module contains the code for the XVD memory sub-manager.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_memory_priv.c $
 * 
 * Hydra_Software_Devel/8   7/20/11 3:07p davidp
 * SW7420-2001: Reorder header-file includes.
 * 
 * Hydra_Software_Devel/7   11/30/09 4:41p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_* at compile.
 * 
 * Hydra_Software_Devel/6   1/21/09 2:02p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 * 
 * Hydra_Software_Devel/5   10/20/08 1:46p pblanco
 * PR48055: Fix Coverity 'overrun static' error.
 * 
 * Hydra_Software_Devel/4   4/25/08 1:02p pblanco
 * PR42177: Code review changes
 * 
 * Hydra_Software_Devel/3   4/9/08 11:48a pblanco
 * PR41271: Fixed Coverity bug (CID 7880) RESOURCE_LEAK.
 * 
 * Hydra_Software_Devel/2   9/6/07 5:35p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 * 
 * Hydra_Software_Devel/1   8/13/07 12:31p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/16   6/13/07 8:11a pblanco
 * PR29915: Fixed incorrect comment regarding bookkeeping locality.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/15   6/12/07 8:33a pblanco
 * PR29915: Force all bookkeeping to be system instead of local which
 * won't work for 7400B0.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/14   6/11/07 10:43a pblanco
 * PR29915: Removed conditional define for guard band checking.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/13   6/6/07 1:15p pblanco
 * PR29915: Updated maximum region count in comment.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/12   6/4/07 3:55p pblanco
 * PR29915: Added conditional so that guard band testing can be enabled or
 * disabled on our locally created heap.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/11   5/21/07 3:37p pblanco
 * PR29915: Changed API signatures for BXVD_P_Memory_Dump and
 * BXVD_P_Memory_AllocateAtIndex.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/10   5/21/07 8:21a pblanco
 * PR29915: Checking in Dave's changes to new BXVD_P_Memory_Open.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/9   5/18/07 12:58p pblanco
 * PR29915: Added new protection parameter to BXVD_Open.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/8   5/2/07 2:30p pblanco
 * PR29915: Fixed a couple of bugs in the helper functions and added a new
 * API.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/7   5/1/07 4:04p pblanco
 * PR29915: Added new APIs for getting a regions size and it's allocation
 * status.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/6   4/27/07 1:43p pblanco
 * PR29915: Added a new API for freeing memory by index and two APIs for
 * getting a regions address and index.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/5   4/26/07 3:16p pblanco
 * PR29915: Cleaned up code and added new diagnostic functionality.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/4   4/24/07 12:21p pblanco
 * PR29915: Fix for the allocator routine and additional coding.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/3   4/19/07 3:13p pblanco
 * PR29915: Completed initial implementation.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/2   4/19/07 12:07p pblanco
 * PR29915: More code development.
 * 
 * xvd_PR29915_Rel_Mosaic_FW_API/1   4/17/07 2:19p pblanco
 * PR29915: Adding to source control.
 * 
 ****************************************************************************/
#include "bxvd_memory_priv.h"
#include "bstd.h"
#include "bdbg.h" 
#include "bkni.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h" 

BDBG_MODULE(BXVD_MEMORY);

/***************************************************************************
BXVD_P_Memory_Open: Opens and initializes an XVD memory instance
**************************************************************************/
BERR_Code BXVD_P_Memory_Open(BXVD_Handle hXvd, 
                             BXVD_P_MemoryHandle *phXvdMem,
                             void *pvAddr,
                             uint32_t uiPhyAddr,
                             uint32_t uiSize,
                             BXVD_P_Memory_Protection eProtect)
{
   BERR_Code rc = BERR_SUCCESS;
   BXVD_P_Memory *pMem = (BXVD_P_Memory *)NULL;
   BMEM_Settings memModuleSettings;
   BMEM_Heap_Settings memHeapSettings;
   BXVD_P_Memory_RegionIndex region;

   BDBG_ENTER(BXVD_P_Memory_Open);
   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(phXvdMem);
   BDBG_ASSERT(pvAddr);

   if (eProtect != BXVD_P_Memory_Protection_eProtect &&
       eProtect != BXVD_P_Memory_Protection_eDontProtect)
   {
      BXVD_DBG_ERR(hXvd, ("Invalid memory protection parameter"));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }
   
   /* Set handle to NULL in case the allocation fails */
   *phXvdMem = NULL;

   pMem = (BXVD_P_Memory*)(BKNI_Malloc(sizeof(BXVD_P_Memory)));
   if (!pMem)
   {
      BXVD_DBG_ERR(hXvd, ("Could not allocate context structure memory"));
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
   }

   /* Zero out the newly allocated context */
   BKNI_Memset((void*)pMem, BXVD_P_MEM_ZERO, sizeof(BXVD_P_Memory));

   pMem->hXvd = hXvd;
   pMem->uiHeapSize = uiSize;
   pMem->uiRegionCount = 0;
   pMem->uiHeapBaseAddr = pvAddr;

   /* Set the free flag to true for all the new, unallocated regions */
   for (region = 0; region < BXVD_P_MEMORY_MAX_ALLOCATIONS; region++)
   {
      (pMem->aAllocatedRegions[region]).bRegionFree = true;
      (pMem->aAllocatedRegions[region]).iRegionIndex = 
         BXVD_P_MEMORY_FREE_REGION_INDEX;
   }

   /* Set up underlying memory system, BMEM in our case */
   if ((rc = BMEM_GetDefaultSettings(&memModuleSettings)) != BERR_SUCCESS)
   {
      BXVD_DBG_ERR(hXvd, ("Could not get memory default settings"));
      BKNI_Free(pMem);
      return BERR_TRACE(rc);
   }

   if ((rc = BMEM_Open(&(pMem->hMemModule), 
                       &memModuleSettings)) != BERR_SUCCESS)
   {
      BXVD_DBG_ERR(hXvd, ("Could not open memory manager instance"));
      BKNI_Free(pMem);
      return BERR_TRACE(rc);
   }

   if ((rc = BMEM_Heap_GetDefaultSettings(&memHeapSettings)) != BERR_SUCCESS)
   {
      BXVD_DBG_ERR(hXvd, ("Could not get heap default settings"));
      BMEM_Close(pMem->hMemModule);
      BKNI_Free(pMem);
      return BERR_TRACE(rc);
   }

   if (eProtect == BXVD_P_Memory_Protection_eProtect)
      memHeapSettings.eSafetyConfig = BMEM_CONFIG_SAFE;
   else
      memHeapSettings.eSafetyConfig = BMEM_CONFIG_FASTEST;

   BXVD_DBG_MSG(hXvd, ("Memory %s be protected", eProtect ? "will not" : "will"));

   /* Bookkeeping is always SYSTEM for submanaged heaps */
   memHeapSettings.eBookKeeping  = BMEM_BOOKKEEPING_SYSTEM;
   BXVD_DBG_MSG(hXvd, ("Creating a 0x%x (%d) byte heap at virtual address 0x%x (0x%x)",
             uiSize, uiSize, pvAddr, uiPhyAddr));
   if ((rc = BMEM_Heap_Create(pMem->hMemModule, 
                              pvAddr, 
                              uiPhyAddr, 
                              uiSize, 
                              &memHeapSettings,
                              &(pMem->hMem))) != BERR_SUCCESS)
   {
      BMEM_Close(pMem->hMemModule);
      BKNI_Free(pMem);
      return BERR_TRACE(rc);
   }

   *phXvdMem = (BXVD_P_MemoryHandle)pMem;

   BDBG_LEAVE(BXVD_P_Memory_Open);
   return BERR_TRACE(rc);
}

/***************************************************************************
BXVD_P_Memory_Close: Closes an XVD memory instance
**************************************************************************/
BERR_Code BXVD_P_Memory_Close(BXVD_P_MemoryHandle hXvdMem)
{
   BDBG_ENTER(BXVD_P_Memory_Close);
   
   BDBG_ASSERT(hXvdMem);

   /* Destroy the heap */
   BMEM_Heap_Destroy(hXvdMem->hMem);

   /* Close the BMEM instance */
   BMEM_Close(hXvdMem->hMemModule);

   /* Release the memory context */
   BKNI_Free(hXvdMem);
   
   /* Null the handle */
   hXvdMem = NULL;   

   BDBG_LEAVE(BXVD_P_Memory_Close);
   return BERR_TRACE(BERR_SUCCESS); 
}

/***************************************************************************
BXVD_P_Memory_Allocate: Allocate a region of sub-managed memory
**************************************************************************/
void *BXVD_P_Memory_Allocate(BXVD_P_MemoryHandle hXvdMem,
                             uint32_t uiAllocationSize,
                             uint32_t uiAlignment,
                             uint32_t uiRegionTag)
{
   bool bAlloc, bRegionFound;
   void *ptr = NULL;
   int32_t iIndex;
   uint32_t uiPhyAddr;
   BXVD_P_AllocatedRegion stAllocReg;

   BDBG_ENTER(BXVD_P_Memory_Allocate);
   BDBG_ASSERT(hXvdMem);

   /*
    * Make sure we don't try to allocate > than BXVD_P_MEMORY_MAX_ALLOCATIONS
    * since that is the maximum number we can track. The default for maximum
    * is currently 17 regions but this can easily be changed.
    */
   if (((hXvdMem->uiRegionCount) + 1) > BXVD_P_MEMORY_MAX_ALLOCATIONS)
   {
      BDBG_ERR(("No more available regions. Maximum is %d",
                BXVD_P_MEMORY_MAX_ALLOCATIONS));
      return NULL;
   }
   
   /* Allocate the requested heap */
   ptr = BMEM_Heap_AllocAligned(hXvdMem->hMem,
                                uiAllocationSize, 
                                uiAlignment, 
                                0);
   if (ptr == NULL)
   {
      BDBG_ERR(("Could not allocate requested heap"));
      return NULL;
   }

   /* 
    * Convert the virtual address to physical offset so we can store it with
    * the other bookkeeping information.
    */
   if (BMEM_Heap_ConvertAddressToOffset(hXvdMem->hMem, ptr, &uiPhyAddr) !=
       BERR_SUCCESS)
   {
      BDBG_ERR(("Address out of range for conversion"));
      BMEM_Free(hXvdMem->hMem, ptr);
      return NULL;
   }

   /*
    * Store bookkeeping information in the allocated regions array.
    */

   bAlloc = false;
   bRegionFound = false;
   stAllocReg.bRegionFree = false;   

   /* Find the first unallocated region slot */
   for (iIndex = 0; iIndex < BXVD_P_MEMORY_MAX_ALLOCATIONS-1; iIndex++)
   {
      BXVD_P_Memory_IsRegionAllocated(hXvdMem, iIndex, &bAlloc);
      if (bAlloc == false)
      {
	 BDBG_MSG(("<<<< found a free allocation slot at %d >>>>", iIndex));
	 bRegionFound = true;
         break;
      }
   }

   /* Make sure we actually found a free allocation slot */
   if (bRegionFound == false)
   {
      BDBG_ERR(("No allocation region slots available"));
      return NULL;
   }

   /* Initialize allocation region members before copy */
   stAllocReg.iRegionIndex = (BXVD_P_Memory_RegionIndex)iIndex;
   stAllocReg.uiRegionAlignment = uiAlignment;
   stAllocReg.uiRegionPhyAddr = uiPhyAddr;
   stAllocReg.pvRegionVirtAddr = ptr;
   stAllocReg.uiRegionSize = uiAllocationSize;
   stAllocReg.uiRegionTag = uiRegionTag;

   /* Move it from found slot to allocated regions array */
   BKNI_Memcpy(&(hXvdMem->aAllocatedRegions[iIndex]),
               &stAllocReg,
               sizeof(BXVD_P_AllocatedRegion));

   /* Increment the region count */
   (hXvdMem->uiRegionCount)++;

   BDBG_LEAVE(BXVD_P_Memory_Allocate);
   return ptr;
}

/***************************************************************************
BXVD_P_Memory_AllocateAtIndex: Allocates a region of memory at a specific 
*                              region index.
****************************************************************************/
void *BXVD_P_Memory_AllocateAtIndex(BXVD_P_MemoryHandle hXvdMem,
                                    BXVD_P_Memory_RegionIndex iRegionIndex,
                                    uint32_t uiAllocationSize,
                                    uint32_t uiAlignment,
                                    uint32_t uiRegionTag)
{
   bool bAlloc;
   void *ptr = NULL;
   uint32_t uiPhyAddr;
   BXVD_P_AllocatedRegion stAllocReg;

   BDBG_ENTER(BXVD_P_Memory_AllocateAtIndex);
   BDBG_ASSERT(hXvdMem);

   /*
    * Make sure we don't try to allocate an already allocated region.
    */
   BXVD_P_Memory_IsRegionAllocated(hXvdMem, iRegionIndex, &bAlloc);
   if (bAlloc == true)
   {
      BDBG_ERR(("Region %d is already allocated", iRegionIndex));
      return NULL;
   }
   
   /* Allocate the requested heap */
   ptr = BMEM_Heap_AllocAligned(hXvdMem->hMem,
                                uiAllocationSize, 
                                uiAlignment, 
                                0);
   if (ptr == NULL)
   {
      BDBG_ERR(("Could not allocate requested heap"));
      return NULL;
   }

   /* 
    * Convert the virtual address to physical offset so we can store it with
    * the other bookkeeping information.
    */
   if (BMEM_Heap_ConvertAddressToOffset(hXvdMem->hMem, ptr, &uiPhyAddr) !=
       BERR_SUCCESS)
   {
      BDBG_ERR(("Address out of range for conversion"));
      return NULL;
   }

   /*
    * Store bookkeeping information in the allocated regions array.
    */
   stAllocReg.bRegionFree = false;
   stAllocReg.iRegionIndex = (BXVD_P_Memory_RegionIndex)iRegionIndex;
   stAllocReg.uiRegionAlignment = uiAlignment;
   stAllocReg.uiRegionPhyAddr = uiPhyAddr;
   stAllocReg.pvRegionVirtAddr = ptr;
   stAllocReg.uiRegionSize = uiAllocationSize;
   stAllocReg.uiRegionTag = uiRegionTag;

   BKNI_Memcpy(&(hXvdMem->aAllocatedRegions[iRegionIndex]),
               &stAllocReg,
               sizeof(BXVD_P_AllocatedRegion));

   /* Increment the region count */
   (hXvdMem->uiRegionCount)++;

   BDBG_LEAVE(BXVD_P_Memory_AllocateAtIndex);
   return ptr;
}

/***************************************************************************
BXVD_P_Memory_Free: Free a region of memory by address
**************************************************************************/
BERR_Code BXVD_P_Memory_Free(BXVD_P_MemoryHandle hXvdMem,
                             void *pvAddr)
{
   bool regionFound = false;
   BERR_Code rc = BERR_SUCCESS;
   BXVD_P_Memory_RegionIndex region;

   BDBG_ENTER(BXVD_P_Memory_Free);
   BDBG_ASSERT(hXvdMem);
   BDBG_ASSERT(pvAddr);

   /*
    * Look up the region about to be freed and do some house keeping.
    */
   for (region = 0; region < BXVD_P_MEMORY_MAX_ALLOCATIONS; region++)
   {
      if (hXvdMem->aAllocatedRegions[region].pvRegionVirtAddr == pvAddr)
      {
         hXvdMem->aAllocatedRegions[region].bRegionFree = true;
         hXvdMem->aAllocatedRegions[region].iRegionIndex = 
            BXVD_P_MEMORY_FREE_REGION_INDEX;
         hXvdMem->aAllocatedRegions[region].uiRegionPhyAddr = 0;
         hXvdMem->aAllocatedRegions[region].pvRegionVirtAddr = NULL;
         hXvdMem->aAllocatedRegions[region].uiRegionSize = 0;
         regionFound = true;
         break;
      }
   }

   /* 
    * The requested region was found. Free it and decrement the region count.
    */
   if (regionFound == true)
   {
      if ((rc = BMEM_Free(hXvdMem->hMem, pvAddr)) != BERR_SUCCESS)
      {
         BDBG_ERR(("BXVD_P_Memory_Free failed"));
         return BERR_TRACE(rc);
      }
      (hXvdMem->uiRegionCount)--;
   }

   BDBG_LEAVE(BXVD_P_Memory_Free);
   return BERR_TRACE(rc);
}

/***************************************************************************
BXVD_P_Memory_IsRegionAllocated: Return a region's allocation status given
its region index.
****************************************************************************/
BERR_Code BXVD_P_Memory_IsRegionAllocated(BXVD_P_MemoryHandle hXvdMem,
                                          BXVD_P_Memory_RegionIndex regionIndex,
                                          bool *pbAllocated)
{
   BDBG_ENTER(BXVD_P_Memory_IsRegionAllocated);

   BDBG_ASSERT(hXvdMem);
   BDBG_ASSERT(pbAllocated);

   if (regionIndex < 0 || regionIndex > BXVD_P_MEMORY_MAX_ALLOCATIONS-1)
   {
      *pbAllocated = false;
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   if ((hXvdMem->aAllocatedRegions[regionIndex].bRegionFree) == true)
      *pbAllocated = false;
   else
      *pbAllocated = true;

   BDBG_LEAVE(BXVD_P_Memory_IsRegionAllocated);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_P_Memory_DumpRegion: Diagnostic dump of memory regions
**************************************************************************/
void BXVD_P_Memory_Dump(BXVD_P_MemoryHandle hXvdMem,
                        BXVD_P_Memory_DumpFlag eDumpFlag)
{
   BXVD_P_Memory_RegionIndex region;

   BDBG_ENTER(BXVD_P_Memory_Dump);
   BDBG_ASSERT(hXvdMem);

   BKNI_Printf("--------------------------------------------------\n");
   if (hXvdMem->uiRegionCount > 1)
      BKNI_Printf("- 0x%2.2x (%2.2d) regions have been allocated          -\n",
                  hXvdMem->uiRegionCount, hXvdMem->uiRegionCount);
   else
      BKNI_Printf("- One region has been allocated                  -\n");
   BKNI_Printf("--------------------------------------------------\n");
   
   for (region = 0; region < BXVD_P_MEMORY_MAX_ALLOCATIONS; region++)
   {
      if (eDumpFlag == BXVD_P_Memory_DumpFlag_eAllocatedOnly &&
          hXvdMem->aAllocatedRegions[region].bRegionFree == true)
         continue;
      BKNI_Printf("Allocated/Free flag: %s\n",
                  (char *)(hXvdMem->aAllocatedRegions[region].bRegionFree == 
                           true ? "free" : "allocated"));
      BKNI_Printf("         Region tag: 0x%x (%d)\n",
                  hXvdMem->aAllocatedRegions[region].uiRegionTag,
                  hXvdMem->aAllocatedRegions[region].uiRegionTag);
      BKNI_Printf("       Region index: %d\n",
                  hXvdMem->aAllocatedRegions[region].iRegionIndex);
      BKNI_Printf("   Region alignment: %d\n",
                  hXvdMem->aAllocatedRegions[region].uiRegionAlignment);
      BKNI_Printf("   Physical address: 0x%x\n",
                  hXvdMem->aAllocatedRegions[region].uiRegionPhyAddr);
      BKNI_Printf("    Virtual address: 0x%x\n",
                  hXvdMem->aAllocatedRegions[region].pvRegionVirtAddr);
      BKNI_Printf("        Region size: 0x%x (%d)\n",
                  hXvdMem->aAllocatedRegions[region].uiRegionSize,
                  hXvdMem->aAllocatedRegions[region].uiRegionSize);
      BKNI_Printf("--------------------------------------------------\n");
   }

   BDBG_LEAVE(BXVD_P_Memory_Dump);
   return;
}

#if BXVD_P_MEMORY_DEBUG_FUNCS

/***************************************************************************
BXVD_P_Memory_Free: Free a region of memory by region index
**************************************************************************/
BERR_Code BXVD_P_Memory_FreeByIndex(BXVD_P_MemoryHandle hXvdMem,
                                    BXVD_P_Memory_RegionIndex iRegionIndex)
{
   void *addr;
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ASSERT(hXvdMem);

   if (iRegionIndex < 0 || iRegionIndex > BXVD_P_MEMORY_MAX_ALLOCATIONS-1)
      return BERR_TRACE(BERR_INVALID_PARAMETER);

   if (hXvdMem->aAllocatedRegions[iRegionIndex].bRegionFree == true)
   {
      BDBG_WRN(("Region %d is already free", iRegionIndex));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   
   addr = hXvdMem->aAllocatedRegions[iRegionIndex].pvRegionVirtAddr;
   if ((rc = BMEM_Free(hXvdMem->hMem, addr)) != BERR_SUCCESS)
   {
      BDBG_ERR(("BXVD_P_Memory_Free failed"));
      return BERR_TRACE(rc);
   }

   hXvdMem->aAllocatedRegions[iRegionIndex].bRegionFree = true;
   hXvdMem->aAllocatedRegions[iRegionIndex].iRegionIndex = 
      BXVD_P_MEMORY_FREE_REGION_INDEX;
   hXvdMem->aAllocatedRegions[iRegionIndex].uiRegionPhyAddr = 0;
   hXvdMem->aAllocatedRegions[iRegionIndex].pvRegionVirtAddr = NULL;
   hXvdMem->aAllocatedRegions[iRegionIndex].uiRegionSize = 0;
   (hXvdMem->uiRegionCount)--;

   return BERR_TRACE(rc);
}

/***************************************************************************
BXVD_P_Memory_GetVirtAddrByIndex: Get region's address based on its index
**************************************************************************/
void *BXVD_P_Memory_GetVirtAddrByIndex(BXVD_P_MemoryHandle hXvdMem,
                                       BXVD_P_Memory_RegionIndex iRegionIndex)
{
   BDBG_ENTER(BXVD_P_Memory_GetVirtAddrByIndex);

   BDBG_ASSERT(hXvdMem);

   if (iRegionIndex < 0 || iRegionIndex > BXVD_P_MEMORY_MAX_ALLOCATIONS-1)
   {
      BDBG_WRN(("Region index 0x%x (%d) out of range", 
                iRegionIndex, iRegionIndex));
      return NULL;
   }

   if (hXvdMem->aAllocatedRegions[iRegionIndex].bRegionFree == true)
   {
      BDBG_WRN(("Requested region 0x%x (%d) is not allocated",
                iRegionIndex, iRegionIndex));
      return NULL;
   }

   BDBG_LEAVE(BXVD_P_Memory_GetVirtAddrByIndex);
   return hXvdMem->aAllocatedRegions[iRegionIndex].pvRegionVirtAddr;
}

/***************************************************************************
BXVD_P_Memory_GetRegionIndex: Get region's index based on its address
**************************************************************************/
BXVD_P_Memory_RegionIndex BXVD_P_Memory_GetRegionIndex(
   BXVD_P_MemoryHandle hXvdMem,
   void *pvAddr)
{
   bool bRegionFound = false;
   BXVD_P_Memory_RegionIndex region;

   BDBG_ENTER(BXVD_P_Memory_GetRegionIndex);

   BDBG_ASSERT(hXvdMem);
   BDBG_ASSERT(pvAddr);

   for (region = 0; region < BXVD_P_MEMORY_MAX_ALLOCATIONS; region++)
   {
      if (hXvdMem->aAllocatedRegions[region].pvRegionVirtAddr == pvAddr)
      {
         bRegionFound = true;
         break;
      }
   }
   
   if (bRegionFound == false)
   {
      BDBG_WRN(("Specified address 0x%x not on allocation list", pvAddr));
      region = BXVD_P_MEMORY_FREE_REGION_INDEX;
   }

   BDBG_LEAVE(BXVD_P_Memory_GetRegionIndex);
   return region;
}


/***************************************************************************
BXVD_P_Memory_GetRegionSize: Return a region's size given its region index.
****************************************************************************/
BERR_Code BXVD_P_Memory_GetRegionSize(BXVD_P_MemoryHandle hXvdMem,
                                      BXVD_P_Memory_RegionIndex regionIndex,
                                      uint32_t *puiRegionSize)
{
   BDBG_ENTER(BXVD_P_Memory_GetRegionSize);

   BDBG_ASSERT(hXvdMem);
   BDBG_ASSERT(puiRegionSize);

   if (regionIndex < 0 || regionIndex > BXVD_P_MEMORY_MAX_ALLOCATIONS-1)
   {
      *puiRegionSize = 0;
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   *puiRegionSize = hXvdMem->aAllocatedRegions[regionIndex].uiRegionSize;

   BDBG_LEAVE(BXVD_P_Memory_GetRegionSize);
   return BERR_TRACE(BERR_SUCCESS);
}

#endif /* BXVD_P_MEMORY_DEBUG_FUNCS */
