/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Memory management
File     :  $RCSfile$
Revision :  $Revision$

FILE DESCRIPTION
Implementation of memory management API.
=============================================================================*/

#include <sys/types.h>
#include <stdarg.h>

#include "interface/vcos/vcos.h"

#include "rtos_abstract_mem.h"

#include "interface/khronos/include/EGL/begl_memplatform.h"
#include "interface/khronos/common/khrn_int_common.h"
#include "middleware/khronos/common/khrn_hw.h"           /* For stats recording */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/******************************************************************************
helpers
******************************************************************************/

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

/******************************************************************************
manager
******************************************************************************/

#if defined(BCG_VC4_DEFRAG) || !defined(NDEBUG)

/* Used to record allocations */
typedef struct MEM_HEADER_LIST_S
{
   MEM_HEADER_T               *header;
   struct MEM_HEADER_LIST_S   *next;
} MEM_HEADER_LIST_T;

#endif

/* The manager itself */
typedef struct {

   BEGL_MemoryInterface *memInterface;
   VCOS_MUTEX_T          mh;
   uintptr_t             minCachedAddress;
   uintptr_t             maxCachedAddress;

#if defined(BCG_VC4_DEFRAG) || !defined(NDEBUG)
   MEM_HEADER_LIST_T     *deviceAllocations;
#endif

} MEM_MANAGER_T;

/* C standard says this is auto initialized to zero */
static MEM_MANAGER_T g_mgr;

MEM_HANDLE_T MEM_ZERO_SIZE_HANDLE;
MEM_HANDLE_T MEM_EMPTY_STRING_HANDLE;

static void private_mem_hdr_list_dump(void);

/******************************************************************************
pool management api
******************************************************************************/

/*
   Initialize the memory subsystem, allocating a pool of a given size and
   with space for the given number of handles.
*/

int mem_init(void * memInterface)
{
   /* should this take a copy? */
   g_mgr.memInterface = memInterface;
   g_mgr.minCachedAddress = 0xFFFFFFFF;
   g_mgr.maxCachedAddress = 0x00000000;

   vcos_mutex_create((VCOS_MUTEX_T *)&g_mgr.mh, "mem_init");

   /* allocate special handles */
   MEM_ZERO_SIZE_HANDLE = mem_alloc(0, 4, MEM_FLAG_NONE, "zero size");
   MEM_EMPTY_STRING_HANDLE = mem_strdup("");

#if defined(BCG_VC4_DEFRAG) || !defined(NDEBUG)
   g_mgr.deviceAllocations = NULL;
#endif

   return 1;
}

uint32_t mem_map_cached_to_physical(void *virt)
{
   if (g_mgr.memInterface != NULL && g_mgr.memInterface->ConvertCachedToPhysical != NULL)
      return g_mgr.memInterface->ConvertCachedToPhysical(g_mgr.memInterface->context, virt);
   else
      return 0;
}

void *mem_map_physical_to_cached(uint32_t phys)
{
   if (g_mgr.memInterface != NULL && g_mgr.memInterface->ConvertPhysicalToCached)
      return g_mgr.memInterface->ConvertPhysicalToCached(g_mgr.memInterface->context, phys);
   else
      return NULL;
}

void mem_flush_cache(void)
{
   if (g_mgr.memInterface != NULL && g_mgr.memInterface->FlushCache != NULL)
      g_mgr.memInterface->FlushCache(g_mgr.memInterface->context, 0, 0xFFFFFFFF);
}

void mem_flush_cache_range(void * p, size_t numBytes)
{
   if (g_mgr.memInterface != NULL && g_mgr.memInterface->FlushCache != NULL)
      g_mgr.memInterface->FlushCache(g_mgr.memInterface->context, p, numBytes);
}

/*
   Terminate the memory subsystem, releasing the pool.
*/

void mem_term(void)
{
#ifndef NDEBUG
   if (g_mgr.deviceAllocations != NULL)
   {
      /* Looks like we might have some memory leaks */
      printf("MEMORY LEAKS DETECTED:\n");
      private_mem_hdr_list_dump();
   }
#endif

   vcos_mutex_delete((VCOS_MUTEX_T *)&g_mgr.mh);

   mem_release(MEM_ZERO_SIZE_HANDLE);
   mem_release(MEM_EMPTY_STRING_HANDLE);
}

/*
   The heap is compacted to the maximum possible extent. If (mode & MEM_COMPACT_DISCARD)
   is non-zero, all discardable, unlocked, and unretained MEM_HANDLE_Ts are resized to size 0.
   If (mode & MEM_COMPACT_AGGRESSIVE) is non-zero, all long-term block owners (which are
   obliged to have registered a callback) are asked to unlock their blocks for the duration
   of the compaction.
*/

void mem_compact(mem_compact_mode_t mode)
{
   UNUSED(mode);
   assert(0);
}

/******************************************************************************
core api
******************************************************************************/

/* Zepher has a 256kb L2 with 128byte line size - all allocations should therefore be a multiple of 128bytes and aligned to 128byte boundary (minimum) */
#define CACHELINE_SIZE 128

uint32_t mem_cacheline_size(void)
{
   if (g_mgr.memInterface != NULL && g_mgr.memInterface->GetInfo != NULL)
      return g_mgr.memInterface->GetInfo(g_mgr.memInterface->context, BEGL_MemCacheLineSize);
   else
      return CACHELINE_SIZE;
}

#if defined(BCG_VC4_DEFRAG) || !defined(NDEBUG)
/* MEM_HEADER_LIST_T functions
 *
 * The mem-header-list is used to record memory allocations so that they can be
 * defragmented if necessary when an allocation fails
 */

/* mem_hdr_list_length
 *
 * Calculate the length of a header list
 *
 * Use only within a mutex.
 */
static uint32_t mem_hdr_list_length(MEM_HEADER_LIST_T *list)
{
   uint32_t          l    = 0;
   MEM_HEADER_LIST_T *ptr = list;

   for (; ptr != NULL; ptr = ptr->next)
      l++;

   return l;
}

/* mem_hdr_list_free
 *
 * Frees all the elements in the list and NULLs the list
 *
 * Use only within a mutex.
 */
static void mem_hdr_list_free(MEM_HEADER_LIST_T **list)
{
   MEM_HEADER_LIST_T *ptr  = *list;
   MEM_HEADER_LIST_T *next = NULL;

   for (; ptr != NULL; ptr = next)
   {
      next = ptr->next;
      free(ptr);
   }

   *list = NULL;
}

/* mem_hdr_list_insert
 *
 * Inserts a header in front of the first list item with a greater pointer value
 * Can be used to implement an insert-sort.
 * If the malloc for the new item fails, no insertion takes place.
 *
 * Use only within a mutex.
 */
static void mem_hdr_list_insert(MEM_HEADER_LIST_T **list, MEM_HEADER_T *newHdr)
{
   MEM_HEADER_LIST_T *ptr   = *list;
   MEM_HEADER_LIST_T **prev = list;
   bool              found  = false;

   if (newHdr == NULL)
      return;

   for (; !found && ptr != NULL; prev = &ptr->next, ptr = ptr->next)
   {
      MEM_HEADER_T   *hdr = ptr->header;

      if (hdr != NULL && hdr->ptr > newHdr->ptr)
         found = true;
   }

   {
      MEM_HEADER_LIST_T *newNode = (MEM_HEADER_LIST_T *)malloc(sizeof(MEM_HEADER_LIST_T));

      if (newNode != NULL)
      {
         newNode->header = newHdr;
         newNode->next   = ptr;
         *prev = newNode;
      }
   }
}

static void mem_hdr_list_dump(MEM_HEADER_LIST_T *list)
{
   MEM_HEADER_LIST_T *ptr = list;

   for (; ptr != NULL; ptr = ptr->next)
   {
      MEM_HEADER_T   *h = ptr->header;

      printf("====================\n");

      if (h != NULL)
      {
         printf("alloc size: %d\n", h->allocedSize);
         printf("size      : %d\n", h->size);
         printf("align     : %d\n", h->align);
         printf("term      : %p\n", h->term);
         printf("desc      : %s\n", h->desc);
         printf("ref count : %d\n", h->ref_count);
#ifdef BCG_VC4_DEFRAG
         printf("lock count: %d\n", h->lock_count);
#endif
         printf("flags     : %x\n", h->flags);
      }
      else
      {
         printf("NULL\n");
      }
   }
}

static void private_mem_hdr_list_dump(void)
{
   mem_hdr_list_dump(g_mgr.deviceAllocations);
}

/* record_allocation
 *
 * Record an allocation by adding the header to the head of the g_mgr allocation list.
 */
static void record_allocation(MEM_HEADER_T *hdr)
{
   vcos_mutex_lock((VCOS_MUTEX_T *)&g_mgr.mh);

   {
      MEM_HEADER_LIST_T *alloc = (MEM_HEADER_LIST_T *)malloc(sizeof(MEM_HEADER_LIST_T));

      /* If we can't record it, forget it, it will just not be defragged - we're probably doomed anyway */
      if (alloc != NULL)
      {
         MEM_HEADER_LIST_T *head = g_mgr.deviceAllocations;

         g_mgr.deviceAllocations = alloc;

         alloc->next   = head;
         alloc->header = hdr;
      }
   }

   vcos_mutex_unlock((VCOS_MUTEX_T *)&g_mgr.mh);
}

/* remove_allocation
 *
 * Remove an allocation by removing it from the g_mgr allocation list.
 */
static void remove_allocation(MEM_HEADER_T *hdr)
{
   vcos_mutex_lock((VCOS_MUTEX_T *)&g_mgr.mh);

   {
      MEM_HEADER_LIST_T *ptr     =  g_mgr.deviceAllocations;
      MEM_HEADER_LIST_T **ptrPtr = &g_mgr.deviceAllocations;

      while (ptr != NULL)
      {
         MEM_HEADER_LIST_T *next = ptr->next;

         if (ptr->header == hdr)
         {
            free(ptr);

            *ptrPtr = next; 
            ptr     = NULL;
         }
         else
         {
            ptrPtr = &ptr->next;
            ptr    = next;
         }
      }

      vcos_mutex_unlock((VCOS_MUTEX_T *)&g_mgr.mh);
   }
}

#else
static void record_allocation(MEM_HEADER_T *hdr) {}
static void remove_allocation(MEM_HEADER_T *hdr) {}
#endif

#ifdef BCG_VC4_DEFRAG

/* mem_defrag
 *
 * Attempts to defragment device memory.
 *
 * For each group of movable memory allocations
 *    Copy to system memory
 *    Delete device memory
 * Then when there is no more system memory or when the end of a block is reached:
 *    Allocate device memory
 *    Copy back from system memory
 */
extern void glxx_hw_finish_context();

static uint32_t find_biggest_alloc(MEM_HEADER_T **allocs, uint32_t howMany)
{
   uint32_t i;
   uint32_t biggestSize  = 0;
   int32_t  biggestIndex = -1;

   for (i = 0; i < howMany; ++i)
   {
      if (allocs[i] != NULL && allocs[i]->allocedSize > biggestSize)
      {
         biggestSize  = allocs[i]->allocedSize;
         biggestIndex = i;
      }
   }

   return biggestIndex;
}

/* mem_defrag()
 *
 * Attempts to defragment the heap by copying data into system memory and back.  When copying back, biggest
 * items are copied first to maximise the chance that they will fit.
 *
 * Returns false if heap memory was not available during the copy back.  This is unlikely to happen, but could
 * occur if the heap is shared with other threads/processes.  This is a bad case because the memory will be in
 * an undefined state, so GL should report out-of-memory and applications should restart with a fresh context.
 */
static bool mem_defrag()
{
   static int  entries = 0;

   MEM_HEADER_LIST_T *sorted     = NULL;
   MEM_HEADER_LIST_T *ptr        = NULL;
   bool              ok          = false;
   MEM_HEADER_T      **sysAllocs = NULL;
   uint32_t          sysAllocTop = 0;

   /* Can't defrag if we don't have a working mem interface */
   if (g_mgr.memInterface == NULL || g_mgr.memInterface->Free == NULL || g_mgr.memInterface->Alloc == NULL)
      return false;

   vcos_atomic_increment(&entries);

   /* Can't defrag re-entrantly */
   if (entries > 1)
      goto exit_atomic;

   INCR_DRIVER_COUNTER(defrags);

   /* Flush the pipeline to unlock as much as possible */
   glxx_hw_finish_context(NULL, true);

   /* Don't want anyone changing stuff behind our backs */
   vcos_mutex_lock((VCOS_MUTEX_T *)&g_mgr.mh);

   /* Allocate enough room to record the allocations */
   sysAllocs   = (MEM_HEADER_T **)malloc(sizeof(MEM_HEADER_T *) * mem_hdr_list_length(g_mgr.deviceAllocations));
   sysAllocTop = 0;

   /* If we have no system memory then we can't defrag */
   if (sysAllocs == NULL)
      goto exit_mutex;

   /* Create a sorted version of the header list */
   for (ptr = g_mgr.deviceAllocations; ptr != NULL; ptr = ptr->next)
   {
      if (ptr->header != NULL)
         mem_hdr_list_insert(&sorted, ptr->header);
   }

   /* 
    * Note that the loop will keep running so long as there are still blocks to handle or there
    * are blocks to copy back from system memory.
    *
    * On each iteration, if ptr has not reached the end of the list then it will be advanced to the next
    * entry unless there is not enough system memory available.  In this case (and when the ptr has reached
    * the end of the list), sysPtr will be NULL and the second part of the loop will kick in.
    *
    * The second part of the loop will reset sysAllocTop to zero (regardless of whether all the memory
    * could be copied back -- it will copy as much as it can).
    *
    * There is a tricky case if the malloc fails, and we have no allocations in the system allocation list.  In
    * this case, we give up on defragging.  The defrag will return success because it may (unlikely) have done
    * some work.
    */
   for (ptr = sorted, ok = true; ptr != NULL || sysAllocTop != 0; )
   {
      void  *sysPtr = NULL;

      /* Deal with the next allocation, if there is one */
      if (ptr != NULL)
      {
         MEM_HEADER_T   *hdr = ptr->header;

         /* If we have an unlocked allocation, then copy to system memory and delete from device memory
          * and add it to the sysAlloc list.
          */
         if (hdr != NULL && hdr->lock_count == 0)
         {
            sysPtr = malloc(hdr->allocedSize);

            if (sysPtr != NULL)
            {
               memcpy(sysPtr, hdr->ptr, hdr->allocedSize);
               g_mgr.memInterface->Free(g_mgr.memInterface->context, hdr->ptr);
               hdr->ptr = sysPtr;

               /* Remember this one -- move on to next */
               sysAllocs[sysAllocTop++] = hdr;
               ptr = ptr->next;
            }
            else
            {
               /* sysAllocTop == 0: This would indicate that we have no system memory of our own to work with.  
                * We've freed up all our system allocations and the malloc has still failed.  Give up on defragging.
                */
               if (sysAllocTop == 0)
                  ptr = NULL;
            }
         }
         else
         {
            /* Skip this one */
            ptr = ptr->next;
         }
      }

      /* If the allocaction was locked/wrapped, or the system memory allocation failed, then sysPtr will
       * still be NULL, so copy back all the stacked allocations (if any) to device memory and patch up the memory headers.
       */
      if (sysPtr == NULL)
      {
         uint32_t i;

         for (i = 0; i < sysAllocTop; ++i) 
         {
            /* Work from the biggest down.  It is easier to fit small things round a big thing */
            uint32_t       biggest = find_biggest_alloc(sysAllocs, sysAllocTop);
            MEM_HEADER_T   *hdr = sysAllocs[biggest];
            void           *nptr;

            assert(hdr != NULL);

            /* We don't want to use this one again! */
            sysAllocs[biggest] = NULL;

            nptr = g_mgr.memInterface->Alloc(g_mgr.memInterface->context, hdr->allocedSize, hdr->align);
            
            /* Put the data back into device memory
             * This should not fail, but we need to do something sensible if it does
             */
            if (nptr != NULL)
               memcpy(nptr, hdr->ptr, hdr->allocedSize);
            else
               ok = false;

            free(hdr->ptr);
            hdr->ptr = nptr;
         }

         sysAllocTop = 0;
      }
   }

   mem_hdr_list_free(&sorted);
   free(sysAllocs);

   /* Make sure that the device memory is coherent */
   mem_flush_cache();

exit_mutex:
   vcos_mutex_unlock((VCOS_MUTEX_T *)&g_mgr.mh);

exit_atomic:
   vcos_atomic_decrement(&entries);

   return ok;
}

#else
static bool mem_defrag()                         { return false; }
#endif

/* allocate_direct
 *
 * Helper function for allocating device memory
 * Attempts to allocate memory, if this fails, then trigger a defragmentation and try again.
 * If second allocation fails, then give up.
 */
static void *allocate_direct(MEM_HEADER_T *h)
{
   void  *ptr = NULL;

   if (g_mgr.memInterface != NULL && g_mgr.memInterface->Alloc != NULL && g_mgr.memInterface->GetInfo != NULL)
   {
      uint32_t available = g_mgr.memInterface->GetInfo(g_mgr.memInterface->context, BEGL_MemLargestBlock);

      /* In case the platform layer doesn't support largest block report */
      if (available == 0)
         available = 0xffffffff;

      /* If there is enough room, try to allocate */
      if (available >= h->allocedSize)
         ptr = g_mgr.memInterface->Alloc(g_mgr.memInterface->context, h->allocedSize, h->align);

      /* If there wasn't enough room, or the allocation failed, then defrag and try again */
      if (ptr == NULL)
      {
         if (mem_defrag())
         {
            ptr = g_mgr.memInterface->Alloc(g_mgr.memInterface->context, h->allocedSize, h->align);
         }
      }
   }

   if (ptr != NULL)
   {
      g_mgr.minCachedAddress = min(g_mgr.minCachedAddress, (uintptr_t)ptr);
      g_mgr.maxCachedAddress = max(g_mgr.maxCachedAddress, (uintptr_t)ptr + h->allocedSize);
   }

   return ptr;
}

static void init_header(MEM_HEADER_T *h, uint32_t size, uint32_t align, MEM_FLAG_T flags, const char *desc)
{
   /* needs to be max of CPU cache line and the GCACHE on the v3d */
   uint32_t cacheline_size = max(mem_cacheline_size(), BCG_GCACHE_LINE_SIZE);
   h->allocedSize = (flags & MEM_FLAG_DIRECT) ? ((size + (cacheline_size - 1)) & ~(cacheline_size - 1)) : size;
   h->size = size;
   h->align = (flags & MEM_FLAG_DIRECT) ? ((align > cacheline_size) ? align : cacheline_size) : align;
   h->term = NULL;
   h->desc = desc;
   h->ref_count  = 1;
#ifdef BCG_VC4_DEFRAG
   h->lock_count = 0;
#endif
   h->magic = MAGIC;
   flags = (MEM_FLAG_T)((flags & ~MEM_FLAG_RETAINED) | ((flags & MEM_FLAG_NO_INIT) ? MEM_FLAG_ABANDONED : 0));
   h->flags = flags;

   if (h->size > 0)
   {
      /* in the case of non HW allocated blocks, just use a standard allocation */
      if (flags & MEM_FLAG_DIRECT)
      {
         h->ptr = allocate_direct(h);

         if (h->ptr)
         {
            /* initializ(s)e the memory */
            if ((flags & MEM_FLAG_NO_INIT) == 0)
               memset(h->ptr, (flags & MEM_FLAG_ZERO) ? 0 : MEM_HANDLE_INVALID, h->allocedSize);
            else
            {
               /*  in a debug build, set uninitialised memory to rubbish */
#if !defined(NDEBUG)
               /*  don't do this for large allocations (eg frame buffers) */
               if (h->size < (64 * 1024))
                  memset(h->ptr, 0xdddddddd, h->size);
#endif
            }

            /* Record this allocation for defrag purposes */
            record_allocation(h);
         }
#ifdef ASSERT_ON_ALLOC_FAIL
         else
         {
            printf("BEGL_memoryInterface->Alloc() FAILED, size = %d, align = %d, desc = %s\n", h->size, h->align, h->desc);
            assert(0);
         }
#endif
      }
      else
      {
         h->ptr = malloc(h->allocedSize);

         if (h->ptr)
         {
            /* initializ(s)e the memory */
            if ((flags & MEM_FLAG_NO_INIT) == 0)
               memset(h->ptr, (flags & MEM_FLAG_ZERO) ? 0 : MEM_HANDLE_INVALID, h->size);
            else
            {
               /*  in a debug build, set uninitialised memory to rubbish */
#if !defined(NDEBUG)
               /*  don't do this for large allocations (eg frame buffers) */
               if (h->size < (64 * 1024))
                  memset(h->ptr, 0xdddddddd, h->size);
#endif
            }
         }
#ifdef ASSERT_ON_ALLOC_FAIL
         else
            assert(0);
#endif
      }
   }
   else
      h->ptr = NULL;
}

/*
   Attempts to allocate a movable block of memory of the specified size and
   alignment. size may be 0. A MEM_HANDLE_T with size 0 is special: see
   mem_lock/mem_unlock. mode specifies the types of compaction permitted,
   including MEM_COMPACT_NONE.

   Preconditions:

   - align is a power of 2.
   - flags only has set bits within the range specified by MEM_FLAG_ALL.
   - desc is NULL or a pointer to a null-terminated string.

   Postconditions:

   If the attempt succeeds:
   - A fresh MEM_HANDLE_T referring to the allocated block of memory is
     returned.
   - The MEM_HANDLE_T is unlocked, without a terminator, and has a reference
     count of 1.
   - If MEM_FLAG_RETAINED was specified, the MEM_HANDLE_T has a retain count of
     1, otherwise it is unretained.
   - If the MEM_FLAG_ZERO flag was specified, the block of memory is set to 0.
     Otherwise, each aligned word is set to MEM_HANDLE_INVALID.

   If the attempt fails:
   - MEM_HANDLE_INVALID is returned.
*/

MEM_HANDLE_T mem_alloc_ex(
   uint32_t size,
   uint32_t align,
   MEM_FLAG_T flags,
   const char *desc,
   mem_compact_mode_t mode)
{
   MEM_HEADER_T * h;

   UNUSED(mode);

   assert(is_power_of_2(align));
   assert(!(flags & ~MEM_FLAG_ALL));
   assert((flags & MEM_FLAG_RESIZEABLE) || !(flags & MEM_FLAG_HINT_GROW));

#ifdef DETECT_LEAKS
   /* BMEM allocator has a leak detector.  Force this always to check for leaks (for free!) */
   flags |= MEM_FLAG_DIRECT;
#endif

   h = (MEM_HEADER_T *)malloc(sizeof(MEM_HEADER_T));
   if (h == NULL)
   {
#ifdef ASSERT_ON_ALLOC_FAIL
      assert(0);
#endif
      return MEM_HANDLE_INVALID;
   }

   init_header(h, size, align, flags, desc);
   if ((h->size > 0) && (h->ptr == NULL))
   {
      free(h);
      return MEM_HANDLE_INVALID;
   }
   else
      return (MEM_HANDLE_T)(uintptr_t)h;
}

MEM_HANDLE_T mem_wrap(void *p, uint32_t size, uint32_t align, MEM_FLAG_T flags, const char *desc)
{
   MEM_HEADER_T *h;

   assert(is_power_of_2(align));
   assert(!(flags & ~MEM_FLAG_ALL));
   assert(!(flags & MEM_FLAG_RESIZEABLE));

   h = (MEM_HEADER_T *)malloc(sizeof(MEM_HEADER_T));

   if (h == NULL)
   {
#ifdef ASSERT_ON_ALLOC_FAIL
      assert(0);
#endif
      return MEM_HANDLE_INVALID;
   }

   h->size = size;
   h->allocedSize = size;
   h->align = align;
   h->term = NULL;
   h->desc = desc;
   h->ref_count  = 1;
#ifdef BCG_VC4_DEFRAG
   h->lock_count = 0;
#endif
   h->magic = MAGIC;
   flags = (MEM_FLAG_T)((flags & ~MEM_FLAG_RETAINED) | ((flags & MEM_FLAG_NO_INIT) ? MEM_FLAG_ABANDONED : 0));
   flags |= MEM_FLAG_WRAPPED;
   h->flags = flags;
   h->ptr = p;

   if (flags & MEM_FLAG_DIRECT)
   {
      g_mgr.minCachedAddress = min(g_mgr.minCachedAddress, (uintptr_t)p);
      g_mgr.maxCachedAddress = max(g_mgr.maxCachedAddress, (uintptr_t)p + h->allocedSize);
   }

   return (MEM_HANDLE_T)(uintptr_t)h;
}

MEM_HANDLE_T mem_wrap_ex(void *p, void *physAddr, uint32_t size, uint32_t align, MEM_FLAG_T flags, const char *desc)
{
   MEM_HEADER_T *h = (MEM_HEADER_T*)mem_wrap(p, size, align, flags, desc);

   return (MEM_HANDLE_T)(uintptr_t)h;
}

/*
   If the reference count of the MEM_HANDLE_T is 1 and it has a terminator, the
   terminator is called with a pointer to and the size of the MEM_HANDLE_T's
   block of memory (or NULL/0 if the size of the MEM_HANDLE_T is 0). The
   MEM_HANDLE_T may not be used during the call.

   Preconditions:

   - handle is a valid MEM_HANDLE_T.
   - If its reference count is 1, it must not be locked or retained.

   Postconditions:

   If the reference count of the MEM_HANDLE_T was 1:
   - The MEM_HANDLE_T is now invalid. The associated block of memory has been
     freed.

   Otherwise:
   - The reference count of the MEM_HANDLE_T is decremented.
*/

void mem_release_inner(MEM_HEADER_T *h)
{
   /* put me back in if things start to go wrong */
   /*assert((handle != MEM_ZERO_SIZE_HANDLE) && (handle != MEM_EMPTY_STRING_HANDLE));*/

   if (h->term)
      h->term(h->ptr, h->size);

   if (h->allocedSize > 0 && h->ptr != NULL)
   {
      if (!(h->flags & MEM_FLAG_WRAPPED))
      {
         if (h->flags & MEM_FLAG_DIRECT)
         {
            if (g_mgr.memInterface != NULL && g_mgr.memInterface->Free != NULL)
               g_mgr.memInterface->Free(g_mgr.memInterface->context, h->ptr);

            remove_allocation(h);
         }
         else
         {
            free(h->ptr);
         }
      }
   }
   free(h);
}

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.

   Postconditions:

   If the reference count of the MEM_HANDLE_T was 1:
   - 0 is returned.
   - The reference count of the MEM_HANDLE_T is still 1.

   Otherwise:
   - 1 is returned.
   - The reference count of the MEM_HANDLE_T is decremented.
*/

int mem_try_release(
   MEM_HANDLE_T handle)
{
   MEM_HEADER_T *h = (MEM_HEADER_T *)handle;
   unsigned int ref_count;

   assert(h->magic == MAGIC);
   assert(h->ref_count != 0);

   /* <= in case another thread jumps in and multiple InterlockedDecrement occur */
   ref_count = vcos_atomic_decrement( &h->ref_count );

   if ( ref_count <= 0 )
   {
      vcos_atomic_increment( &h->ref_count );
      return 0;
   }
   else
      return 1;
}

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.

   Postconditions:

   - The total space used by the MEM_HANDLE_T is returned (this includes the
     header, the actual block, and padding).
   - sum_over_handles(mem_get_space(handle)) + mem_get_free_space() is constant
     over the lifetime of the pool.
*/

/*
uint32_t mem_get_space(
   MEM_HANDLE_T handle)
{
   UNUSED(handle);
   assert(0);
   return 0;
}
*/

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.

   Postconditions:

   - The MEM_HANDLE_T's reference count is returned.
*/

uint32_t mem_get_ref_count(
   MEM_HANDLE_T handle)
{
   MEM_HEADER_T *h = (MEM_HEADER_T *)handle;
   assert(h->magic == MAGIC);
   assert(h->ref_count != 0);
   return h->ref_count;
}

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.

   Postconditions:

   - The MEM_HANDLE_T's lock count is returned.
*/

uint32_t mem_get_lock_count(
   MEM_HANDLE_T handle)
{
   MEM_HEADER_T   *h = (MEM_HEADER_T *)handle;
   assert(h->magic == MAGIC);
#ifdef BCG_VC4_DEFRAG
   return h->lock_count;
#else
   return 0;
#endif
}

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.

   Postconditions:

   - The MEM_HANDLE_T's retain count is returned.
*/

/*
uint32_t mem_get_retain_count(
   MEM_HANDLE_T handle)
{
   assert(0);
   UNUSED(handle);
   return 0;
}
*/

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.

   Postconditions:

   - The MEM_HANDLE_T's description string is returned.
*/

const char *mem_get_desc(
   MEM_HANDLE_T handle)
{
   MEM_HEADER_T *h = (MEM_HEADER_T *)handle;
   assert(h->magic == MAGIC);
   return h->desc;
}

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.
   - desc is NULL or a pointer to a null-terminated string.

   Postconditions:

   - The MEM_HANDLE_T's description is set to desc.
*/

void mem_set_desc(
   MEM_HANDLE_T handle,
   const char *desc)
{
   MEM_HEADER_T *h = (MEM_HEADER_T *)handle;
   assert(h->magic == MAGIC);
   h->desc = desc;
}

/*
   The MEM_HANDLE_T's terminator is called just before the MEM_HANDLE_T becomes
   invalid: see mem_release.

   Preconditions:

   - handle is a valid MEM_HANDLE_T.
   - term must be NULL or a pointer to a function compatible with the MEM_TERM_T
     type.

   Postconditions:

   - The MEM_HANDLE_T's terminator is set to term (if term was NULL, the
     MEM_HANDLE_T no longer has a terminator).
*/

void mem_set_term(
   MEM_HANDLE_T handle,
   MEM_TERM_T term)
{
   MEM_HEADER_T *h = (MEM_HEADER_T *)handle;
   assert(h->magic == MAGIC);
   assert(h->ref_count != 0);
   h->term = term;
}

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.

   Postconditions:

   - The MEM_HANDLE_T's terminator is returned, or NULL if there is none.
*/

MEM_TERM_T mem_get_term(
   MEM_HANDLE_T handle)
{
   MEM_HEADER_T *h = (MEM_HEADER_T *)handle;
   assert(h->magic == MAGIC);
   assert(h->ref_count != 0);
   return h->term;
}

/*
   void mem_set_user_flag(MEM_HANDLE_T handle, int flag)

   Preconditions:

   - handle is a valid MEM_HANDLE_T.

   Postconditions:

   - The MEM_HANDLE_T's user flag is set to 0 if flag is 0, or to 1 otherwise.
*/

void mem_set_user_flag(
   MEM_HANDLE_T handle, int flag)
{
   MEM_HEADER_T *h = (MEM_HEADER_T *)handle;
   assert(h->magic == MAGIC);
   assert(h->ref_count != 0);

   vcos_mutex_lock((VCOS_MUTEX_T *)&g_mgr.mh);

   if (flag)
      h->flags |= MEM_FLAG_USER;
   else
      h->flags &= ~MEM_FLAG_USER;

   vcos_mutex_unlock((VCOS_MUTEX_T *)&g_mgr.mh);
}

/*
   Attempts to resize the MEM_HANDLE_T's block of memory. The attempt is
   guaranteed to succeed if the new size is less than or equal to the old size.
   size may be 0. A MEM_HANDLE_T with size 0 is special: see
   mem_lock/mem_unlock. mode specifies the types of compaction permitted,
   including MEM_COMPACT_NONE.

   Preconditions:

   - handle is a valid MEM_HANDLE_T.
   - It must not be locked.
   - It must have been created with the MEM_FLAG_RESIZEABLE flag.

   Postconditions:

   If the attempt succeeds:
   - 1 is returned.
   - The MEM_HANDLE_T's block of memory has been resized.
   - The contents in the region [0, min(old size, new size)) are unchanged. If
     the MEM_HANDLE_T is zero'd, the region [min(old size, new size), new size)
     is set to 0. Otherwise, each aligned word in the region
     [min(old size, new size), new size) is set to MEM_HANDLE_INVALID.

   If the attempt fails:
   - 0 is returned.
   - The MEM_HANDLE_T is still valid.
   - Its block of memory is unchanged.
*/

int mem_resize_ex(
   MEM_HANDLE_T handle,
   uint32_t size,
   mem_compact_mode_t mode)
{
   int res;
   MEM_HEADER_T *h = (MEM_HEADER_T *)handle;
   void * nptr;

   UNUSED(mode);

   assert(h->magic == MAGIC);
   assert(h->ref_count != 0);
#ifdef BCG_VC4_DEFRAG
   assert(h->lock_count == 0);
#endif
   assert(h->flags & MEM_FLAG_RESIZEABLE);

   if (h->flags & MEM_FLAG_DIRECT)
   {
      /* needs to be max of CPU cache line and the GCACHE on the v3d */
      uint32_t cacheline_size = max(mem_cacheline_size(), BCG_GCACHE_LINE_SIZE);
      h->allocedSize = (size + (cacheline_size - 1)) & ~(cacheline_size - 1);

      nptr = allocate_direct(h);

      vcos_mutex_lock((VCOS_MUTEX_T *)&g_mgr.mh);

      if (nptr)
      {
         if (h->ptr != NULL)
         {
            if (g_mgr.memInterface != NULL && g_mgr.memInterface->MemCopy != NULL)
               g_mgr.memInterface->MemCopy(g_mgr.memInterface->context, nptr, h->ptr, min(h->size, h->allocedSize));
            else
               memcpy(nptr, h->ptr, min(h->size, h->allocedSize));

            if (g_mgr.memInterface != NULL && g_mgr.memInterface->Free != NULL)
               g_mgr.memInterface->Free(g_mgr.memInterface->context, h->ptr);
         }

         h->ptr = nptr;

         if (h->ptr)
         {
            if ((size > h->size) && (h->flags & MEM_FLAG_ZERO))
               memset( (void *)((uintptr_t)h->ptr + h->size), 0, size - h->size );
            h->size = size;
            res = 1;
         }
         else
            res = 0;
      }
      else
         res = 0;
   }
   else
   {
      vcos_mutex_lock((VCOS_MUTEX_T *)&g_mgr.mh);

      h->ptr = realloc(h->ptr, size);

      if (h->ptr)
      {
         if ((size > h->size) && (h->flags & MEM_FLAG_ZERO))
            memset( (void *)((uintptr_t)h->ptr + h->size), 0, size - h->size );
         h->size = size;
         h->allocedSize = size;
         res = 1;
      }
      else
         res = 0;
   }

   vcos_mutex_unlock((VCOS_MUTEX_T *)&g_mgr.mh);

   return res;
}

void mem_lock_multiple(
   void **pointers,
   MEM_HANDLE_OFFSET_T *handles,
   uint32_t n)
{
   uint32_t i;

   for (i = 0; i < n; i++)
   {
      void * p = (handles[i].mh_handle != MEM_HANDLE_INVALID) ? mem_lock(handles[i].mh_handle) : NULL;
      pointers[i] = (void *)((uintptr_t)p + handles[i].offset);
   }
}

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.
   - It must be executable.
   - If it is locked, the previous lock must have been by mem_lock_exec and not
     mem_lock.

   Postconditions:

   If the MEM_HANDLE_T's size is 0:
   - NULL is returned.
   - The MEM_HANDLE_T is completely unchanged.

   Otherwise:
   - A pointer to the MEM_HANDLE_T's block of memory is returned. The pointer is
     valid until the MEM_HANDLE_T's lock count reaches 0. The pointer may only
     be used for code execution on the current core.
   - The MEM_HANDLE_T's lock count is incremented.
*/

void (*mem_lock_exec(
   MEM_HANDLE_T handle))(void)
{
   UNUSED(handle);
   assert(0);
   return NULL;
}

/*
   A version of mem_lock which indicates that an aggresive compaction
   should not wait for the block to be unlocked.
*/

void *mem_lock_perma(
   MEM_HANDLE_T handle)
{
   UNUSED(handle);
   assert(0);
   return NULL;
}

/*
void mem_unlock_multiple(
   MEM_HANDLE_OFFSET_T *handles,
   uint32_t n)
{
   UNUSED(handles);
   UNUSED(n);
   assert(0);
}

void mem_unlock_perma(
   MEM_HANDLE_T handle)
{
   UNUSED(handle);
   assert(0);
}
*/

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.

   Postconditions:

   If the MEM_HANDLE_T is not a small handle, and the lock count is zero:
   - Sets MEM_FLAG_ABANDONED, which causes the data content to become undefined
    when the lock count reaches zero.
   - Sets MEM_FLAG_NO_INIT.

   Otherwise:
   - Does nothing.
*/

void mem_abandon(
   MEM_HANDLE_T handle)
{
   UNUSED(handle);
}

/*
   A discardable MEM_HANDLE_T with a retain count greater than 0 is
   considered retained and may not be discarded by the memory manager.

   Preconditions:

   - handle is a valid MEM_HANDLE_T.
   - It must be discardable.

   Postconditions:

   - 0 is returned if the size of the MEM_HANDLE_T's block of memory is 0,
     otherwise 1 is returned.
   - The retain count of the MEM_HANDLE_T is incremented.
*/

int mem_retain(
   MEM_HANDLE_T handle)
{
   MEM_HEADER_T *h = (MEM_HEADER_T *)handle;

   assert(h->magic == MAGIC);
   assert(h->ref_count != 0);
   assert(h->flags & MEM_FLAG_DISCARDABLE);

   return !(h->size == 0);
}

/*
   Preconditions:

   - handle is a valid MEM_HANDLE_T.
   - It must be retained.

   Postconditions:

   - The retain count of the MEM_HANDLE_T is decremented.
*/

void mem_unretain(
   MEM_HANDLE_T handle)
{
   UNUSED(handle);
}

void mem_unlock_unretain_release_multiple(
   MEM_HANDLE_OFFSET_T *handles,
   unsigned int n)
{
   unsigned int i;

   for (i = 0; i < n; i++)
   {
      MEM_HEADER_T *h = (MEM_HEADER_T *)handles[i].mh_handle;

      if (handles[i].mh_handle == MEM_HANDLE_INVALID) { continue; }

      assert(h->magic == MAGIC);

      /* release */
      mem_unlock(handles[i].mh_handle);
      mem_release(handles[i].mh_handle);
   }
}

/******************************************************************************
Legacy memory blocks API
******************************************************************************/

#ifndef MEM_LEGACY_BLOCK_SIZE
#define MEM_LEGACY_BLOCK_SIZE    0x00200000
#endif

#ifndef MEM_LEGACY_BLOCK_ALIGN
#define MEM_LEGACY_BLOCK_ALIGN   4096
#endif

/*
   Allocate a fixed-size block. The size of legacy blocks is a constant for the
   life of the memory subsystem (and may be a build-time constant). If no existing
   blocks are free, the relocatable heap high-water mark is lowered to make room
   for the  block, which may necessitate a compaction.

   Preconditions:

   - flags must specify a memory alias
     o MEM_FLAG_NORMAL
     o MEM_FLAG_COHERENT
     o MEM_FLAG_DIRECT
     o MEM_FLAG_L1_NONALLOCATING (VC4 only)
     No other flags are permitted.

   Postconditions:

   If the attempt succeeds:
   - A pointer to the legacy block is returned.
   - The relocatable heap high-water mark may have been lowered to accommodate the
     new block.

   If the attempt fails:
   - NULL (0) is returned.
*/

void *mem_alloc_legacy_ex(MEM_FLAG_T flags)
{
   void * ptr = NULL;
   UNUSED(flags);

   if (g_mgr.memInterface != NULL && g_mgr.memInterface->Alloc != NULL)
      ptr = g_mgr.memInterface->Alloc(g_mgr.memInterface->context, MEM_LEGACY_BLOCK_SIZE, MEM_LEGACY_BLOCK_ALIGN);

   if (ptr)
   {
      g_mgr.minCachedAddress = min(g_mgr.minCachedAddress, (uintptr_t)ptr);
      g_mgr.maxCachedAddress = max(g_mgr.maxCachedAddress, (uintptr_t)ptr + MEM_LEGACY_BLOCK_SIZE);
   }

   return ptr;
}


/*
   Free a previously-allocated fixed-size block.

   Preconditions:

   - ptr must point to a block previously returned by mem_alloc_legacy.

   Postconditions:

   - The relocatable heap high-water mark may have been raised.
*/

void mem_free_legacy(void *ptr)
{
   if (g_mgr.memInterface != NULL && g_mgr.memInterface->Free != NULL)
      g_mgr.memInterface->Free(g_mgr.memInterface->context, ptr);
}


/*
   Preconditions:

   - None.

   Postconditions:

   - Returns the size of a legacy block.
*/

extern uint32_t mem_get_legacy_size(void)
{
   return MEM_LEGACY_BLOCK_SIZE;
}


/*
   Check the internal consistency of the heap. A corruption will result in
   a failed assert, which will cause a breakpoint in a debug build.

   If MEM_FILL_FREE_SPACE is defined for the build, then free space is also
   checked for corruption; this has a large performance penalty, but can help
   to track down random memory corruptions.

   Note that defining MEM_AUTO_VALIDATE will enable the automatic validation
   of the heap at key points - allocation, deallocation and compaction.

   Preconditions:

   - None.

   Postconditions:

   - None.
*/

/*
void mem_validate(void)
{
   assert(0);
}

*/

/******************************************************************************
Long-term lock owners' API
******************************************************************************/

/* Returns 1 on success, 0 on failure. */
/*
int mem_register_callback(mem_callback_func_t func, uintptr_t context)
{
   UNUSED(func);
   UNUSED(context);
   assert(0);
   return 1;
}

void mem_unregister_callback(mem_callback_func_t func, uintptr_t context)
{
   UNUSED(func);
   UNUSED(context);
   assert(0);
}
*/

/******************************************************************************
Movable memory helpers
******************************************************************************/

MEM_HANDLE_T mem_strdup_ex(
   const char *str,
   mem_compact_mode_t mode)
{
   MEM_HANDLE_T handle = mem_alloc_ex((uint32_t)strlen(str) + 1, 1, MEM_FLAG_NONE, "mem_strdup", mode);
   if (handle == MEM_HANDLE_INVALID) {
      return MEM_HANDLE_INVALID;
   }
   strcpy((char *)mem_lock(handle), str);
   mem_unlock(handle);
   return handle;
}

MEM_HANDLE_T mem_dup_ex(
   MEM_HANDLE_T handle,
   const char *desc,
   mem_compact_mode_t mode)
{
   MEM_HANDLE_T newhandle;
   assert(handle != MEM_HANDLE_INVALID);
   assert(mem_get_term(handle) == 0);
   newhandle = mem_alloc_ex(
      mem_get_size(handle),
      mem_get_align(handle),
      mem_get_flags(handle),
      desc,
      mode);

   if (handle == MEM_HANDLE_INVALID) {
      return MEM_HANDLE_INVALID;
   }

   memcpy(mem_lock(newhandle), mem_lock(handle), mem_get_size(handle));
   mem_unlock(newhandle);
   mem_unlock(handle);
   return newhandle;
}

void mem_print_state()
{
}

void mem_print_small_alloc_pool_state()
{
}

int mem_is_relocatable(const void *ptr)
{
   UNUSED(ptr);

   assert(0);
   return 0;
}

void mem_get_stats(uint32_t *blocks, uint32_t *bytes, uint32_t *locked)
{
   UNUSED(blocks);
   UNUSED(bytes);
   UNUSED(locked);

   assert(0);
}

uint32_t mem_get_total_space(void)
{
   assert(0);
   return 0;
}

uint32_t mem_get_free_space(void)
{
   return 0x01000000;
}

void mem_assign_null_multiple(
   MEM_HANDLE_OFFSET_T *handles,
   uint32_t n)
{
   UNUSED(handles);
   UNUSED(n);

   assert(0);
}

/******************************************************************************
Global initialisation helpers
******************************************************************************/

/***********************************************************
 * Name: rtos_common_mem_init
 *
 * Arguments:
 *       void
 *
 * Description: Initialise the relocatable heap.
 *
 * Returns: int ( < 0 is fail )
 *
 ***********************************************************/
int32_t rtos_common_mem_init( void )
{
   assert(0);
   return 0;
}

uintptr_t mem_min_cached_addr()
{
   return g_mgr.minCachedAddress;
}

uintptr_t mem_max_cached_addr()
{
   return g_mgr.maxCachedAddress;
}
