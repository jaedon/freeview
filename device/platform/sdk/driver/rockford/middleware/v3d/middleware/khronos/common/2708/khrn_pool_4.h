/*=============================================================================
Copyright (c) 2009 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Pool

FILE DESCRIPTION
Pools of fixed-size buffers allocated from the relocatable heap. Currently
unused buffers are kept unlocked and unretained (so they can be discarded by the
memory manager when we get tight on memory) in a linked list.
=============================================================================*/

#ifndef KHRN_POOL_4_H
#define KHRN_POOL_4_H

#include "middleware/khronos/common/khrn_mem.h"

typedef struct {
   uint32_t size, align, n_max;
   MEM_FLAG_T flags;
   const char *desc;

   uint32_t n; /* total (in use + in free list) */
   MEM_HANDLE_T free_head;

   MEM_HANDLE_T preallocated_pool;
} KHRN_POOL_T;

extern void khrn_pool_init(KHRN_POOL_T *pool, bool preallocate,
   uint32_t size, uint32_t align, uint32_t n_max,
   MEM_FLAG_T extra_flags, const char *desc);
extern void khrn_pool_term(KHRN_POOL_T *pool);

extern MEM_HANDLE_T khrn_pool_alloc(KHRN_POOL_T *pool, bool fail_ok);
extern void khrn_pool_free(KHRN_POOL_T *pool, MEM_HANDLE_T handle);

extern void khrn_pool_cleanup(KHRN_POOL_T *pool); /* release discarded buffers */

#endif
