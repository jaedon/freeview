#include "middleware/khronos/glsl/glsl_common.h"

#include "middleware/khronos/glsl/glsl_fastmem.h"
#include "middleware/khronos/common/khrn_hw.h"

#include <stdlib.h>

Hunk *fastmem_hunk;

void glsl_fastmem_init(void)
{
   fastmem_hunk = NULL;
}

void glsl_fastmem_term(void)
{
   Hunk *hunk, *next;

   for (hunk = fastmem_hunk; hunk; hunk = next) {
#ifndef FASTMEM_USE_MALLOC
      MEM_HANDLE_T handle = hunk->handle;
#endif

      next = hunk->next;
#ifdef FASTMEM_USE_MALLOC
      free(hunk);
#else
      mem_unlock(handle);
      mem_release(handle);
#endif
   }

   fastmem_hunk = NULL;
}

Hunk *glsl_fastmem_alloc_hunk(Hunk *next)
{
   MEM_HANDLE_T handle = MEM_INVALID_HANDLE;
   Hunk *hunk;

   /*
      check free list for available hunks; allocate one from the global heap if none found
   */

#ifdef FASTMEM_USE_MALLOC
   hunk = malloc(sizeof(Hunk));
   khrn_memset(hunk, 0, sizeof(Hunk));
#else
   handle = MEM_ALLOC_STRUCT_EX(Hunk, MEM_COMPACT_DISCARD);       // check, no term
   if (handle == MEM_INVALID_HANDLE)
      return NULL;

   hunk = (Hunk *)mem_lock(handle);
#endif

   /*
      initialize hunk
   */

   hunk->used = 0;
   hunk->next = next;
   hunk->handle = handle;

   return hunk;
}
