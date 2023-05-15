/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  Default Nexus platform API for EGL driver
Module   :  Nexus platform 

FILE DESCRIPTION
DESC
=============================================================================*/

#include <EGL/egl.h>

#include <malloc.h>
#include <memory.h>

/*****************************************************************************
 * Memory interface
 *****************************************************************************/
/* Allocate aligned device memory, and return the cached address, or NULL on failure.*/
static void *MemAlloc(void *context, size_t numBytes, uint32_t alignment)
{
   void *ret = 0;

   /* make alignment on 4k boundary */
   if (alignment < 4096) alignment = 4096;
   numBytes = (numBytes + (4096 - 1)) & ~(4096 -1);

   ret = memalign(alignment, numBytes);

   return ret;
}

/* Free a previously allocated block of device memory. Pass a cached address.*/
static void MemFree(void *context, void *pCached)
{
#ifndef NDEBUG
   if (pCached == 0)
      printf("NXPL : Trying to free NULL pointer\n");
#endif

   if (pCached != NULL)
      free(pCached);
}

/* Return a physical device memory offset given a cached pointer. Returns 0 on failure.*/
static uint32_t MemConvertCachedToPhysical(void *context, void *pCached)
{
   return (uint32_t)pCached;
}

/* Return a cached memory pointer given a physical device memory offset. Returns 0 on failure.*/
static void *MemConvertPhysicalToCached(void *context, uint32_t offset)
{
   return (void*)offset;
}

/* Flush the cache for the given address range.*/
static void MemFlushCache(void *context, void *pCached, size_t numBytes)
{
}

/* Retrieve some information about the memory system */
static uint32_t MemGetInfo(void *context, BEGL_MemInfoType type)
{
   switch (type)
   {
   case BEGL_MemHeapStartPhys : return 0; break;
   case BEGL_MemHeapEndPhys   : return 0xFFFFFFFF; break;
   }

   return 0;
}

BEGL_MemoryInterface *CARPL_CreateMemInterface(void)
{
   BEGL_MemoryInterface *mem = (BEGL_MemoryInterface*)malloc(sizeof(BEGL_MemoryInterface));
   if (mem != NULL)
   {
      memset(mem, 0, sizeof(BEGL_MemoryInterface));

      mem->context = NULL;
      mem->Alloc = MemAlloc;
      mem->Free = MemFree;
      mem->FlushCache = MemFlushCache;
      mem->ConvertCachedToPhysical = MemConvertCachedToPhysical;
      mem->ConvertPhysicalToCached = MemConvertPhysicalToCached;
      mem->GetInfo = MemGetInfo;
   }
   return mem;
}

void CARPL_DestroyMemInterface(BEGL_MemoryInterface *mem)
{
   if (mem != NULL)
   {
      memset(mem, 0, sizeof(BEGL_MemoryInterface));
      free(mem);
   }
}
