/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
DESC
=============================================================================*/

#include "nexus_memory.h"
#include "nexus_platform.h"

#include <EGL/egl.h>

#include <malloc.h>
#include <memory.h>

typedef struct  
{
   NEXUS_HeapHandle        heap;
   void                    *heapStartCached;
   uint32_t                heapStartPhys;
   uint32_t                heapSize;
} NXPL_MemoryData;

/*****************************************************************************
 * Memory interface
 *****************************************************************************/
/* Allocate aligned device memory, and return the cached address, or NULL on failure.*/
static void *MemAlloc(void *context, size_t numBytes, uint32_t alignment)
{
   NXPL_MemoryData               *data = (NXPL_MemoryData*)context;
   void                          *alloced = NULL;
   NEXUS_MemoryAllocationSettings settings;

   NEXUS_Memory_GetDefaultAllocationSettings(&settings);

   /* make alignment on 4k boundary */
   if (alignment < 4096) alignment = 4096;
   numBytes = (numBytes + (4096 - 1)) & ~(4096 -1);

   settings.alignment = alignment;
   settings.heap = data->heap;

   NEXUS_Memory_Allocate(numBytes, &settings, &alloced);

   return alloced;
}

/* Free a previously allocated block of device memory. Pass a cached address.*/
static void MemFree(void *context, void *pCached)
{
#ifndef NDEBUG
   if (pCached == 0)
      printf("NXPL : Trying to free NULL pointer\n");
#endif

   NEXUS_Memory_Free(pCached);
}

/* Return a physical device memory offset given a cached pointer. Returns 0 on failure.*/
static uint32_t MemConvertCachedToPhysical(void *context, void *pCached)
{
   NXPL_MemoryData   *data = (NXPL_MemoryData*)context;

#ifndef NDEBUG
   if (pCached == 0)
      printf("NXPL : Trying to convert NULL pointer\n");
#endif

   return data->heapStartPhys + ((uintptr_t)pCached - (uintptr_t)data->heapStartCached);
}

/* Return a cached memory pointer given a physical device memory offset. Returns 0 on failure.*/
static void *MemConvertPhysicalToCached(void *context, uint32_t offset)
{
   NXPL_MemoryData   *data = (NXPL_MemoryData*)context;

   return (void*)((uintptr_t)data->heapStartCached + (offset - data->heapStartPhys));
}

/* Flush the cache for the given address range.*/
static void MemFlushCache(void *context, void *pCached, size_t numBytes)
{
   NXPL_MemoryData   *data = (NXPL_MemoryData*)context;

   if (pCached < data->heapStartCached || numBytes > data->heapSize)
      NEXUS_Memory_FlushCache(data->heapStartCached, data->heapSize);
   else
      NEXUS_Memory_FlushCache(pCached, numBytes);
}

/* Retrieve some information about the memory system */
static uint32_t MemGetInfo(void *context, BEGL_MemInfoType type)
{
   NXPL_MemoryData   *data = (NXPL_MemoryData*)context;

   switch (type)
   {
   case BEGL_MemHeapStartPhys : return data->heapStartPhys; break;
   case BEGL_MemHeapEndPhys   : return data->heapStartPhys + data->heapSize - 1; break;
   }

   return 0;
}

BEGL_MemoryInterface *CARPL_CreateMemInterface(void)
{
   BEGL_MemoryInterface *mem = (BEGL_MemoryInterface*)malloc(sizeof(BEGL_MemoryInterface));
   if (mem != NULL)
   {
      NEXUS_PlatformConfiguration   platformConfig;
      NEXUS_MemoryStatus            memStatus;

      NXPL_MemoryData *data = (NXPL_MemoryData*)malloc(sizeof(NXPL_MemoryData));
      memset(mem, 0, sizeof(BEGL_MemoryInterface));

      if (data != NULL)
      {
         memset(data, 0, sizeof(NXPL_MemoryData));

         mem->context = (void*)data;
         mem->Alloc = MemAlloc;
         mem->Free = MemFree;
         mem->FlushCache = MemFlushCache;
         mem->ConvertCachedToPhysical = MemConvertCachedToPhysical;
         mem->ConvertPhysicalToCached = MemConvertPhysicalToCached;
         mem->GetInfo = MemGetInfo;

         /* Store the heap settings */
#if (BCHP_CHIP==7422) || (BCHP_CHIP==7231) || (BCHP_CHIP==7346) || (BCHP_CHIP==7344)
         data->heap = NEXUS_Platform_GetFramebufferHeap(0);
#else
         NEXUS_Platform_GetConfiguration(&platformConfig);
         data->heap = platformConfig.memc[0].heap[0];
#endif
         NEXUS_Heap_GetStatus(data->heap, &memStatus);
         data->heapStartCached = memStatus.addr;
         data->heapStartPhys = memStatus.offset;
         data->heapSize = memStatus.size;
      }
   }
   return mem;
}

void CARPL_DestroyMemInterface(BEGL_MemoryInterface *mem)
{
   if (mem != NULL)
   {
      if (mem->context != NULL)
         free(mem->context);

      memset(mem, 0, sizeof(BEGL_MemoryInterface));
      free(mem);
   }
}
