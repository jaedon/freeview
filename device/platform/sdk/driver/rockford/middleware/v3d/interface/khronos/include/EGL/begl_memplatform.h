/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  EGL driver
Module   :  Abstract Memory Interface

FILE DESCRIPTION
Defines an abstract interface that will be used to interact with platform memory.
=============================================================================*/

#ifndef _BEGL_MEMPLATFORM_H__
#define _BEGL_MEMPLATFORM_H__

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BEGL_MemInfoType
{
   BEGL_MemHeapStartPhys,
   BEGL_MemHeapEndPhys,
   BEGL_MemCacheLineSize,
   BEGL_MemLargestBlock,
   BEGL_MemFree
} BEGL_MemInfoType;

/* The platform MUST provide an implementation of this interface in order that the EGL driver
 * can interact with platform memory.
 */
typedef struct
{
   /* Context pointer - opaque to the 3d driver code, but passed out in all function pointer calls.
      Prevents the client code needing to perform context lookups. */
   void *context;

   /* Allocate aligned device memory, and return the cached address, or NULL on failure. 
    * Alignment is in units of bytes - not 2^alignment bytes */
   void *(*Alloc)(void *context, size_t numBytes, uint32_t alignment);

   /* Free a previously allocated block of device memory. Pass a cached address.*/
   void (*Free)(void *context, void *pCached);

   /* Return a physical device memory offset given a cached pointer. Returns 0 on failure.*/
   uint32_t (*ConvertCachedToPhysical)(void *context, void *pCached);

   /* Return a cached memory pointer given a physical device memory offset. Returns 0 on failure.*/
   void *(*ConvertPhysicalToCached)(void *context, uint32_t offset);

   /* Flush the cache for the given address range.*/
   void (*FlushCache)(void *context, void *pCached, size_t numBytes);

   /* Retrieve some information about the memory system */
   uint32_t (*GetInfo)(void *context, BEGL_MemInfoType type);

   /* Copy data as fast as possible. Might use DMA where available (if both pointers are in contiguous device memory) */
   void (*MemCopy)(void *context, void *pCachedDest, const void *pCachedSrc, uint32_t bytes);


} BEGL_MemoryInterface;

#ifdef __cplusplus
}
#endif

#endif /* _BEGL_MEMPLATFORM_H__ */
