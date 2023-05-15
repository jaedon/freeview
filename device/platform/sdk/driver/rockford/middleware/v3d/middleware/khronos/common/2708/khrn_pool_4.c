/*=============================================================================
Copyright (c) 2009 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Pool

FILE DESCRIPTION
Implementation.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "middleware/khronos/common/2708/khrn_pool_4.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"

static INLINE void set_next(MEM_HANDLE_T handle, MEM_HANDLE_T next_handle)
{
   mem_set_term(handle, (MEM_TERM_T)(uintptr_t)next_handle);
}

static INLINE MEM_HANDLE_T get_next(MEM_HANDLE_T handle)
{
   return (MEM_HANDLE_T)(uintptr_t)mem_get_term(handle);
}

/* the 3d core can only access 1GB of memory.  This means there are 3 crossing points (256MB, 512MB & 768MB).  This means that on the 4th try we MUST succeed */
#define BANKS_TO_TRY 4

void khrn_pool_init(KHRN_POOL_T *pool, bool preallocate,
   uint32_t size, uint32_t align, uint32_t n_max,
   MEM_FLAG_T extra_flags, const char *desc)
{
   int i;
   MEM_HANDLE_T tmp[BANKS_TO_TRY] = { MEM_INVALID_HANDLE };

   pool->size = size;
   pool->align = align;
   pool->n_max = n_max;
   pool->flags = (MEM_FLAG_T)(extra_flags | MEM_FLAG_DISCARDABLE | MEM_FLAG_RETAINED | MEM_FLAG_NO_INIT);
   pool->desc = desc;
   pool->preallocated_pool = MEM_INVALID_HANDLE;

   pool->n = 0;
   pool->free_head = MEM_INVALID_HANDLE;

   if (preallocate)
   {
      /* Since the binner can only work with memory blocks in a 256MB region, we must preallocate all our
         overspill blocks, and ensure they all live in a 256MB region */
      uint32_t mem;
      MEM_HANDLE_T created_handle;

      for (i = 0; i < BANKS_TO_TRY; i++)
      {
         uint32_t mem_end;
         if (size > align)
         {
            if ((size & (align - 1)) != 0)
            {
               size = ((size / align) + 1) * align;
            }
         }
         else
         {
            size = vcos_max(size, align);
         }

         /* grab the block in a single allocation (serialized access into the underlying OS) */
         /* mem_alloc_ex() rounds up to the nearest cache line size, adding (+1) byte to the allocation size gives us another line.
            the binner was found to be able to prefetch of the end of the block, causing ARC read violations */
         tmp[i] = mem_alloc_ex((size * n_max) + 1, pool->align, pool->flags, pool->desc, MEM_COMPACT_DISCARD);

         /* if no allocations can be made, then we are truely dead */
         if (tmp[i] == MEM_INVALID_HANDLE)
            goto out_of_mem;

         mem = khrn_hw_addr(mem_lock(tmp[i]));
         mem_end = mem + ((size * n_max) - 1);
         if ((mem & 0xF0000000) == (mem_end & 0xF0000000))
         {
            /* in the same bank, get out */
            mem_unlock(tmp[i]);
            /* reference count it.  the discard later will free if required */
            MEM_ASSIGN(pool->preallocated_pool, tmp[i]);
            break;
         }

         mem_unlock(tmp[i]);
      }

      /* free up the blocks, one of them suceeded by this point */
      for (i = 0; i < BANKS_TO_TRY; i++)
      {
         MEM_ASSIGN(tmp[i], MEM_INVALID_HANDLE);
      }

      /* chop up the blocks into wrapped memory, filling them in as before */
      mem = (uint32_t)mem_lock(pool->preallocated_pool);

      /* populate the head of the list */
      created_handle = mem_wrap((void *)mem, size, align, pool->flags, pool->desc);
      mem += size;
      pool->free_head = created_handle;

      /* create a linked list of the blocks */
      for (i = 1; i < (int)n_max; i++, mem += size)
      {
         MEM_HANDLE_T next_handle = mem_wrap((void *)mem, size, align, pool->flags, pool->desc);
         set_next(created_handle, next_handle);
         created_handle = next_handle;
      }

      mem_unlock(pool->preallocated_pool);
   }

   return;

out_of_mem:
   /* nothing is assigned yet, so just free up any blocks which may have been allocated, but failed */
   for (i = 0; i < BANKS_TO_TRY; i++)
   {
      MEM_ASSIGN(tmp[i], MEM_INVALID_HANDLE);
   }
   vcos_assert(pool->n == 0);
   pool->free_head = MEM_INVALID_HANDLE;
}

void khrn_pool_term(KHRN_POOL_T *pool)
{
   MEM_HANDLE_T handle, next_handle;

   for (handle = pool->free_head; handle != MEM_INVALID_HANDLE; handle = next_handle) {
      next_handle = get_next(handle);
      mem_set_term(handle, NULL);
      mem_release(handle);
      --pool->n;
   }
   vcos_assert(pool->n == 0);
}

MEM_HANDLE_T khrn_pool_alloc(KHRN_POOL_T *pool, bool fail_ok)
{
   MEM_HANDLE_T handle = MEM_INVALID_HANDLE, next_handle;

   if ((pool->preallocated_pool != MEM_INVALID_HANDLE) &&
       (pool->free_head == MEM_INVALID_HANDLE))
   {
      /* The preallocation failed */
      return MEM_INVALID_HANDLE;
   }

   for (handle = pool->free_head; handle != MEM_INVALID_HANDLE; handle = next_handle) 
   {
      next_handle = get_next(handle);
      if (mem_retain(handle))
      {
         pool->free_head = next_handle;
         return handle;
      }
      mem_unretain(handle);
      mem_set_term(handle, NULL);
      mem_release(handle);
      --pool->n;
   }
   pool->free_head = MEM_INVALID_HANDLE;

   if (fail_ok && (pool->n >= pool->n_max)) 
   {
      /* already have a lot of buffers -- caller will wait and try again later when some have been freed */
      return MEM_INVALID_HANDLE;
   }

   if (pool->preallocated_pool == MEM_INVALID_HANDLE)
   {
      /* mem_alloc_ex() rounds up to the nearest cache line size, adding (+1) byte to the allocation size gives us another line.
         the binner was found to be able to prefetch of the end of the block, causing ARC read violations */
      handle = mem_alloc_ex(pool->size + 1, pool->align, pool->flags, pool->desc, MEM_COMPACT_DISCARD); /* todo: do we always want MEM_COMPACT_DISCARD? */
      if (handle != MEM_INVALID_HANDLE)
         ++pool->n;
   }
   return handle;
}

void khrn_pool_free(KHRN_POOL_T *pool, MEM_HANDLE_T handle)
{
   mem_unretain(handle);
   mem_abandon(handle);
   set_next(handle, pool->free_head);
   pool->free_head = handle;
}

void khrn_pool_cleanup(KHRN_POOL_T *pool)
{
   MEM_HANDLE_T prev_handle, handle, next_handle;
   for (prev_handle = MEM_INVALID_HANDLE, handle = pool->free_head; handle != MEM_INVALID_HANDLE; prev_handle = handle, handle = next_handle) 
   {
      next_handle = get_next(handle);
      if (mem_get_size(handle) == 0) 
      {
         if (prev_handle == MEM_INVALID_HANDLE) 
         {
            pool->free_head = next_handle;
         } 
         else 
         {
            set_next(prev_handle, next_handle);
         }
         mem_set_term(handle, NULL);
         mem_release(handle);
         --pool->n;
         handle = prev_handle;
      }
   }

   /* if the pool was preallocated, remove the underlying memory */
   MEM_ASSIGN(pool->preallocated_pool, MEM_INVALID_HANDLE);
}
