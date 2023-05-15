/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Interlock system

FILE DESCRIPTION
Interlock system to ensure correct ordering of reads/writes.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"
#include "middleware/khronos/common/2708/khrn_interlock_priv_4.h"
#include "middleware/khronos/common/2708/khrn_render_state_4.h"
#include "middleware/khronos/common/2708/khrn_worker_4.h"
#include "middleware/khronos/common/khrn_hw.h"
#include "middleware/khronos/egl/egl_disp.h"

#ifdef ANDROID
#include <cutils/log.h>
#endif

#define POS_WHO_MASK (0x3ull << 62)
#define POS_WHO_SHIFT 62
#define POS_WHO_HW_BIN    (0ull << 62)
#define POS_WHO_HW_RENDER (1ull << 62)
#define POS_WHO_WORKER    (2ull << 62)

static bool passed(uint64_t pos)
{
   switch ((uint32_t)(pos >> POS_WHO_SHIFT)) {
   case (uint32_t)(POS_WHO_HW_BIN >> POS_WHO_SHIFT):    return khrn_hw_get_bin_exit_pos() >= (pos & ~POS_WHO_MASK);
   case (uint32_t)(POS_WHO_HW_RENDER >> POS_WHO_SHIFT): return khrn_hw_get_render_exit_pos() >= (pos & ~POS_WHO_MASK);
   case (uint32_t)(POS_WHO_WORKER >> POS_WHO_SHIFT):    return khrn_worker_get_exit_pos() >= (pos & ~POS_WHO_MASK);
   default:                                             UNREACHABLE(); return false;
   }
}

static void wait(uint64_t pos)
{
   while (!passed(pos)) {
      if (khrn_sync_master_wait_timeout(2000) == false)
      {
         uint64_t current = 0;
         char type;
         if ((pos >> POS_WHO_SHIFT) == 0)
         {
            current = khrn_hw_get_bin_exit_pos();
            type = 'B';
         }
         else if ((pos >> POS_WHO_SHIFT) == 1)
         {
            current = khrn_hw_get_render_exit_pos();
            type = 'R';
         }
         else if ((pos >> POS_WHO_SHIFT) == 2)
         {
            current = khrn_worker_get_exit_pos();
            type = 'W';
         }
         else
         {
            current = ~0;
            type = 'I';
         }

#ifdef ANDROID
         LOGD("timed out in wait (%c), pos = %lld, current = %lld\n", type, (pos & ~POS_WHO_MASK), current);
#else
         printf("timed out in wait (%c), pos = %lld, current = %lld\n", type, (pos & ~POS_WHO_MASK), current);
#endif
      }
   }
}

void khrn_interlock_extra_init(KHRN_INTERLOCK_T *interlock)
{
   interlock->extra.hw_read_pos = 0;
   interlock->extra.worker_read_pos = 0;
   interlock->extra.write_pos = 0;
}

void khrn_interlock_extra_term(KHRN_INTERLOCK_T *interlock)
{
   UNUSED(interlock);
}

void khrn_interlock_read_immediate(KHRN_INTERLOCK_T *interlock)
{
   khrn_interlock_read(interlock, KHRN_INTERLOCK_USER_NONE);
   if (interlock->extra.write_pos) {
      wait(interlock->extra.write_pos);
      interlock->extra.write_pos = 0;
   }
}

void khrn_interlock_write_immediate(KHRN_INTERLOCK_T *interlock)
{
   khrn_interlock_write(interlock, KHRN_INTERLOCK_USER_NONE);
   if (interlock->extra.hw_read_pos) {
      wait(interlock->extra.hw_read_pos);
      interlock->extra.hw_read_pos = 0;
   }
   if (interlock->extra.worker_read_pos) {
      wait(interlock->extra.worker_read_pos);
      interlock->extra.worker_read_pos = 0;
   }
   if (interlock->extra.write_pos) {
      wait(interlock->extra.write_pos);
      interlock->extra.write_pos = 0;
   }
   if (interlock->disp_image_handle != EGL_DISP_IMAGE_HANDLE_INVALID) {
      while (egl_disp_on(interlock->disp_image_handle)) {
         khrn_sync_master_wait_timeout(-1);
      }
   }
}

void khrn_interlock_flush(KHRN_INTERLOCK_USER_T user)
{
   vcos_assert(_count(user) == 1);
   khrn_render_state_flush(khrn_interlock_render_state_i(user));
}

typedef struct {
   uint64_t pos;
} MSG_WAIT_T;

static KHRN_WORKER_CALLBACK_RESULT_T wait_callback(
   KHRN_WORKER_CALLBACK_REASON_T reason,
   void *message, uint32_t size)
{
   MSG_WAIT_T *msg = (MSG_WAIT_T *)message;
   UNUSED_NDEBUG(size);
   vcos_assert(size == sizeof(MSG_WAIT_T));

   switch (reason) {
   case KHRN_WORKER_CALLBACK_REASON_DO_IT:
   {
      return passed(msg->pos) ?
         KHRN_WORKER_CALLBACK_RESULT_DO_IT_OK :
         KHRN_WORKER_CALLBACK_RESULT_DO_IT_WAIT;
   }
   case KHRN_WORKER_CALLBACK_REASON_CLEANUP:
   {
      return KHRN_WORKER_CALLBACK_RESULT_CLEANUP;
   }
   default:
   {
      UNREACHABLE();
      return (KHRN_WORKER_CALLBACK_RESULT_T)0;
   }
   }
}

static void post_wait(uint64_t pos)
{
   MSG_WAIT_T *msg = (MSG_WAIT_T *)khrn_worker_post_begin(wait_callback, sizeof(MSG_WAIT_T));
   msg->pos = pos;
   khrn_worker_post_end();
}

void khrn_interlock_transfer(KHRN_INTERLOCK_T *interlock, KHRN_INTERLOCK_USER_T user, KHRN_INTERLOCK_FIFO_T fifo)
{
   vcos_assert(_count(user) == 1);
   vcos_assert(!(interlock->users & KHRN_INTERLOCK_USER_WRITING) || (interlock->users == (user | KHRN_INTERLOCK_USER_WRITING)));

   if (!(interlock->users & user)) {
      return; /* already transferred */
   }

   switch (fifo) {
   case KHRN_INTERLOCK_FIFO_HW_RENDER:
   {
      if ((interlock->users & KHRN_INTERLOCK_USER_WRITING) &&
         (interlock->extra.hw_read_pos || interlock->extra.worker_read_pos)) {
         /* just need to wait for the reads; they will be waiting for any
          * outstanding write themselves. we implicitly wait for the hw read,
          * so we just care about the worker read */
         if (interlock->extra.worker_read_pos &&
            !passed(interlock->extra.worker_read_pos)) {
            khrn_hw_queue_wait_for_worker(interlock->extra.worker_read_pos & ~POS_WHO_MASK);
         }
      } else if (interlock->extra.write_pos) {
         switch ((uint32_t)((interlock->extra.write_pos & POS_WHO_MASK) >> POS_WHO_SHIFT)) {
         case (uint32_t)(POS_WHO_HW_BIN >> POS_WHO_SHIFT):
         case (uint32_t)(POS_WHO_HW_RENDER >> POS_WHO_SHIFT): break; /* implicitly wait for */
         case (uint32_t)(POS_WHO_WORKER >> POS_WHO_SHIFT):
         {
            if (passed(interlock->extra.write_pos)) {
               interlock->extra.write_pos = 0;
            } else {
               khrn_hw_queue_wait_for_worker(interlock->extra.write_pos & ~POS_WHO_MASK);
            }
            break;
         }
         default: UNREACHABLE();
         }
      }

      if ((interlock->users & KHRN_INTERLOCK_USER_WRITING) &&
         (interlock->disp_image_handle != EGL_DISP_IMAGE_HANDLE_INVALID)) {
         egl_disp_post_wait(KHRN_INTERLOCK_FIFO_HW_RENDER, interlock->disp_image_handle);
      }

      if (interlock->users & KHRN_INTERLOCK_USER_WRITING) {
         /* everyone will have to wait for our write anyway, so forget any outstanding reads */
         interlock->extra.hw_read_pos = 0;
         interlock->extra.worker_read_pos = 0;
         interlock->extra.write_pos = khrn_hw_get_enter_pos() | POS_WHO_HW_RENDER;
      } else {
         interlock->extra.hw_read_pos = khrn_hw_get_enter_pos() | POS_WHO_HW_RENDER;
      }

      break;
   }
   case KHRN_INTERLOCK_FIFO_WORKER:
   {
      if ((interlock->users & KHRN_INTERLOCK_USER_WRITING) &&
         (interlock->extra.hw_read_pos || interlock->extra.worker_read_pos)) {
         /* just need to wait for the reads; they will be waiting for any
          * outstanding write themselves. we implicitly wait for the worker
          * read, so we just care about the hw read */
         if (interlock->extra.hw_read_pos &&
            !passed(interlock->extra.hw_read_pos)) {
            post_wait(interlock->extra.hw_read_pos);
         }
      } else if (interlock->extra.write_pos) {
         switch ((uint32_t)((interlock->extra.write_pos & POS_WHO_MASK) >> POS_WHO_SHIFT)) {
         case (uint32_t)(POS_WHO_HW_BIN >> POS_WHO_SHIFT):
         case (uint32_t)(POS_WHO_HW_RENDER >> POS_WHO_SHIFT):
         {
            if (passed(interlock->extra.write_pos)) {
               interlock->extra.write_pos = 0;
            } else {
               post_wait(interlock->extra.write_pos);
            }
            break;
         }
         case (uint32_t)(POS_WHO_WORKER >> POS_WHO_SHIFT): break; /* implicitly wait for */
         default: UNREACHABLE();
         }
      }

      if ((interlock->users & KHRN_INTERLOCK_USER_WRITING) &&
         (interlock->disp_image_handle != EGL_DISP_IMAGE_HANDLE_INVALID)) {
         egl_disp_post_wait(KHRN_INTERLOCK_FIFO_WORKER, interlock->disp_image_handle);
      }

      if (interlock->users & KHRN_INTERLOCK_USER_WRITING) {
         /* everyone will have to wait for our write anyway, so forget any outstanding reads */
         interlock->extra.hw_read_pos = 0;
         interlock->extra.worker_read_pos = 0;
         interlock->extra.write_pos = khrn_worker_get_enter_pos() | POS_WHO_WORKER;
      } else {
         interlock->extra.worker_read_pos = khrn_worker_get_enter_pos() | POS_WHO_WORKER;
      }

      break;
   }
   default:
   {
      UNREACHABLE();
   }
   }

   khrn_interlock_release(interlock, user);
}

KHRN_INTERLOCK_FIFO_T khrn_interlock_get_write_fifo(KHRN_INTERLOCK_T *interlock)
{
   if (!interlock->extra.write_pos) {
      return KHRN_INTERLOCK_FIFO_NONE;
   }

   switch ((uint32_t)((interlock->extra.write_pos & POS_WHO_MASK) >> POS_WHO_SHIFT)) {
   case (uint32_t)(POS_WHO_HW_RENDER >> POS_WHO_SHIFT): return KHRN_INTERLOCK_FIFO_HW_RENDER;
   case (uint32_t)(POS_WHO_WORKER >> POS_WHO_SHIFT):    return KHRN_INTERLOCK_FIFO_WORKER;
   default:                                             UNREACHABLE(); return (KHRN_INTERLOCK_FIFO_T)0;
   }
}
