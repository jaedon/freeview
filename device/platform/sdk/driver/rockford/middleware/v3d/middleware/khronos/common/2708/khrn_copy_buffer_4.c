/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Copy buffer

FILE DESCRIPTION
Implementation.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "middleware/khronos/common/2708/khrn_copy_buffer_4.h"
#include "middleware/khronos/common/2708/khrn_interlock_priv_4.h"
#include "middleware/khronos/common/2708/khrn_render_state_4.h"
#include "middleware/khronos/common/2708/khrn_worker_4.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"
#include "middleware/khronos/common/khrn_hw.h"
#include "middleware/khronos/common/khrn_image.h"

void khrn_copy_buffer(MEM_HANDLE_T dst_handle, MEM_HANDLE_T src_handle)
{
   uint32_t i;
   KHRN_COPY_BUFFER_RENDER_STATE_T *render_state;

   i = khrn_render_state_start(KHRN_RENDER_STATE_TYPE_COPY_BUFFER);

   khrn_interlock_write(&((KHRN_IMAGE_T *)mem_lock(dst_handle))->interlock, khrn_interlock_user(i));
   mem_unlock(dst_handle);
   khrn_interlock_read(&((KHRN_IMAGE_T *)mem_lock(src_handle))->interlock, khrn_interlock_user(i));
   mem_unlock(src_handle);

   render_state = (KHRN_COPY_BUFFER_RENDER_STATE_T *)khrn_render_state_get_data(i);
   mem_acquire(dst_handle);
   render_state->dst = dst_handle;
   mem_acquire(src_handle);
   render_state->src = src_handle;
}

typedef struct {
   MEM_HANDLE_T dst, src;
} MSG_T;

static KHRN_WORKER_CALLBACK_RESULT_T callback(
   KHRN_WORKER_CALLBACK_REASON_T reason,
   void *message, uint32_t size)
{
   MSG_T *msg = (MSG_T *)message;
   UNUSED_NDEBUG(size);
   vcos_assert(size == sizeof(MSG_T));

   switch (reason) {
   case KHRN_WORKER_CALLBACK_REASON_DO_IT:
   {
      KHRN_IMAGE_T *dst = (KHRN_IMAGE_T *)mem_lock(msg->dst);
      KHRN_IMAGE_T *src = (KHRN_IMAGE_T *)mem_lock(msg->src);
      void *dstPtr;
      void *srcPtr;

      uint32_t storage_size = mem_get_size(dst->mh_storage);

      /* the buffers should be identically configured */
      vcos_assert(
         (dst->format == src->format) &&
         (dst->width == src->width) && (dst->height == src->height) &&
         (dst->stride == src->stride) &&
         (dst->offset == 0) && (src->offset == 0) &&
         (storage_size == mem_get_size(src->mh_storage)));

      dstPtr = mem_lock(dst->mh_storage);
      srcPtr = mem_lock(src->mh_storage);

      khrn_memcpy(dstPtr, srcPtr, storage_size);

      /* the dst buffer is likely to be used by a tile load next, so we'd better 
         ensure it hits device memory now. */
      khrn_hw_flush_dcache_range(dstPtr, storage_size);

      mem_unlock(dst->mh_storage);
      mem_unlock(src->mh_storage);

      mem_unlock(msg->src);
      mem_unlock(msg->dst);

      return KHRN_WORKER_CALLBACK_RESULT_DO_IT_OK_ADVANCE_EXIT_POS;
   }
   case KHRN_WORKER_CALLBACK_REASON_CLEANUP:
   {
      mem_release(msg->src);
      mem_release(msg->dst);
      return KHRN_WORKER_CALLBACK_RESULT_CLEANUP;
   }
   default:
   {
      UNREACHABLE();
      return (KHRN_WORKER_CALLBACK_RESULT_T)0;
   }
   }
}

void khrn_copy_buffer_render_state_flush(KHRN_COPY_BUFFER_RENDER_STATE_T *render_state)
{
   MSG_T *msg;

   khrn_worker_advance_enter_pos();
   khrn_interlock_transfer(&((KHRN_IMAGE_T *)mem_lock(render_state->dst))->interlock,
      khrn_interlock_user(khrn_render_state_get_index_from_data(render_state)),
      KHRN_INTERLOCK_FIFO_WORKER);
   mem_unlock(render_state->dst);
   khrn_interlock_transfer(&((KHRN_IMAGE_T *)mem_lock(render_state->src))->interlock,
      khrn_interlock_user(khrn_render_state_get_index_from_data(render_state)),
      KHRN_INTERLOCK_FIFO_WORKER);
   mem_unlock(render_state->src);

   msg = (MSG_T *)khrn_worker_post_begin(callback, sizeof(MSG_T));
   msg->dst = render_state->dst; /* transfer ownership */
   msg->src = render_state->src;
   khrn_worker_post_end();

   khrn_render_state_finish(khrn_render_state_get_index_from_data(render_state));
}
