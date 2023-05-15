/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Header file

FILE DESCRIPTION
Various cross-API hardware-specific functions
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "interface/khronos/common/khrn_int_parallel.h"
#include "middleware/khronos/common/2708/khrn_copy_buffer_4.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"
#include "middleware/khronos/common/2708/khrn_interlock_priv_4.h"
#include "middleware/khronos/common/2708/khrn_render_state_4.h"
#include "middleware/khronos/common/2708/khrn_worker_4.h"
#include "middleware/khronos/common/khrn_hw.h"
#include "middleware/khronos/common/khrn_llat.h"
#include "middleware/khronos/egl/egl_platform.h"
#include "middleware/khronos/egl/egl_server.h"
#include "middleware/khronos/egl/egl_disp.h"
#include "middleware/khronos/glxx/2708/glxx_inner_4.h" /* for glxx_hw_init, glxx_hw_term */
#include "middleware/khronos/vg/vg_server.h" /* for vg_maybe_term */

#ifndef WIN32
#include <sys/times.h>
#endif

static VCOS_EVENT_T *master_event;

static KHRN_DRIVER_COUNTERS_T driver_counters;

static void nmem_callback(void * a)
{
   khrn_sync_notify_master();
   khrn_hw_notify_llat();
}

/* order of init/term calls is important */

bool khrn_hw_common_init(void)
{
   if (!khrn_hw_init()) {
      return false;
   }
   if (!khrn_worker_init()) {
      khrn_hw_term();
      return false;
   }
   if (!khrn_nmem_init()) {
      khrn_worker_term();
      khrn_hw_term();
      return false;
   }
   khrn_nmem_register(nmem_callback, NULL);
   khrn_render_state_init();

   return true;
}

void khrn_hw_common_term(void)
{
   khrn_render_state_term();
   khrn_nmem_unregister(nmem_callback, NULL);
   khrn_nmem_term();
   khrn_worker_term();
   khrn_hw_term();
}

void khrn_hw_common_flush(void)
{
   khrn_render_state_flush_all(KHRN_RENDER_STATE_TYPE_NONE);
}

void khrn_hw_common_wait(void)
{
   khrn_hw_wait();
   khrn_worker_wait();
}

typedef struct {
   EGL_DISP_SLOT_HANDLE_T slot_handle;
} MSG_DISPLAY_T;

static KHRN_WORKER_CALLBACK_RESULT_T display_callback(
   KHRN_WORKER_CALLBACK_REASON_T reason,
   void *message, uint32_t size)
{
   MSG_DISPLAY_T *msg = (MSG_DISPLAY_T *)message;
   UNUSED_NDEBUG(size);
   vcos_assert(size == sizeof(MSG_DISPLAY_T));

   switch (reason) {
   case KHRN_WORKER_CALLBACK_REASON_DO_IT:
   {
      egl_disp_ready(msg->slot_handle, false);
      return KHRN_WORKER_CALLBACK_RESULT_DO_IT_OK;
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

void khrn_delayed_display(KHRN_IMAGE_T *image, EGL_DISP_SLOT_HANDLE_T slot_handle)
{
   switch (image ? khrn_interlock_get_write_fifo(&image->interlock) : KHRN_INTERLOCK_FIFO_NONE) {
   case KHRN_INTERLOCK_FIFO_NONE:
   {
      /* no outstanding writes. display now! */
      egl_disp_ready(slot_handle, false);
      break;
   }
   case KHRN_INTERLOCK_FIFO_HW_RENDER:
   {
      /* latest outstanding write is in the hw fifo. post the display message there too */
      khrn_hw_queue_display(slot_handle);
      break;
   }
   case KHRN_INTERLOCK_FIFO_WORKER:
   {
      /* latest outstanding write is in the worker fifo. post the display message there too */
      MSG_DISPLAY_T *msg = (MSG_DISPLAY_T *)khrn_worker_post_begin(display_callback, sizeof(MSG_DISPLAY_T));
      msg->slot_handle = slot_handle;
      khrn_worker_post_end();
      break;
   }
   default: UNREACHABLE();
   }
}

typedef struct {
   EGL_DISP_HANDLE_T disp_handle;
   uint32_t pos;
} MSG_WAIT_FOR_DISPLAY_T;

static KHRN_WORKER_CALLBACK_RESULT_T wait_for_display_callback(
   KHRN_WORKER_CALLBACK_REASON_T reason,
   void *message, uint32_t size)
{
   MSG_WAIT_FOR_DISPLAY_T *msg = (MSG_WAIT_FOR_DISPLAY_T *)message;
   UNUSED_NDEBUG(size);
   vcos_assert(size == sizeof(MSG_WAIT_FOR_DISPLAY_T));

   switch (reason) {
   case KHRN_WORKER_CALLBACK_REASON_DO_IT:
   {
      return egl_disp_still_on(msg->disp_handle, msg->pos) ?
         KHRN_WORKER_CALLBACK_RESULT_DO_IT_WAIT :
         KHRN_WORKER_CALLBACK_RESULT_DO_IT_OK;
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

void khrn_delayed_wait_for_display(uint32_t fifo, EGL_DISP_HANDLE_T disp_handle, uint32_t pos)
{
   switch (fifo) {
   case KHRN_INTERLOCK_FIFO_HW_RENDER:
   {
      khrn_hw_queue_wait_for_display(disp_handle, pos);
      break;
   }
   case KHRN_INTERLOCK_FIFO_WORKER:
   {
      MSG_WAIT_FOR_DISPLAY_T *msg = (MSG_WAIT_FOR_DISPLAY_T *)khrn_worker_post_begin(wait_for_display_callback, sizeof(MSG_WAIT_FOR_DISPLAY_T));
      msg->disp_handle = disp_handle;
      msg->pos = pos;
      khrn_worker_post_end();
      break;
   }
   default: UNREACHABLE();
   }
}

void khrn_delayed_copy_buffer(MEM_HANDLE_T dst_handle, MEM_HANDLE_T src_handle)
{
   khrn_copy_buffer(dst_handle, src_handle);
}

bool khrn_hw_supports_early_z(void)
{
   return false;
}

void khrn_sync_notify_master(void)
{
   vcos_event_signal(master_event);
}

bool khrn_sync_master_wait_timeout(int ms)
{
   bool res = true;
#if defined(__linux__) && defined(__GNUC__)
   res = khrn_event_wait_timeout(master_event, ms);
#else
   vcos_event_wait(master_event);
#endif

   /* if it obtains the semaphore then cleanup, timeout infinity will always do this */
   if (res)
   {
      khrn_hw_cleanup();
      khrn_worker_cleanup();
#ifndef NO_OPENVG
      vg_maybe_term();
#endif
   }

   return res;
}

void khrn_sync_display_returned_notify(void)
{
   khrn_sync_notify_master();
   khrn_hw_notify_llat();
   khrn_worker_notify();
}

void khrn_specify_event(VCOS_EVENT_T *ev)
{
   master_event = ev;
}

int32_t khrn_do_suspend_resume(uint32_t up)
{
   UNUSED(up);

   /* This function probably isn't needed */
   return 0;
}

void khrn_reset_driver_counters(int32_t hw_bank, int32_t l3c_bank)
{
#ifdef __linux__
   struct timeval curTime;
   unsigned int nowMs;
   struct tms cput;
   uint32_t cputime;
   clock_t ticks;
#endif
   int32_t hw = driver_counters.hw_group_active;
   int32_t l3c = driver_counters.l3c_group_active;

   if (hw_bank >= 0 && hw_bank <= 1)
      hw = hw_bank;
   else if (hw < 0 || hw > 1)
      hw = 0;

   if (l3c_bank >= 0 && l3c_bank <= 1)
      l3c = l3c_bank;
   else if (l3c < 0 || l3c > 1)
      l3c = 0;

   memset(&driver_counters, 0, sizeof(KHRN_DRIVER_COUNTERS_T));

#ifdef __linux__
   /* reset the time */
   gettimeofday(&curTime, NULL);
   nowMs = curTime.tv_usec / 1000;
   nowMs += curTime.tv_sec * 1000;

   khrn_driver_counters()->reset_time = nowMs;

   ticks = times(&cput);
   cputime = cput.tms_utime + cput.tms_stime + cput.tms_cutime + cput.tms_cstime;

   khrn_driver_counters()->last_cpu_time = cputime;
   khrn_driver_counters()->last_cpu_ticks = (uint32_t)ticks;
#endif

   driver_counters.hw_group_active = hw;
   driver_counters.l3c_group_active = l3c;

   khrn_hw_register_perf_counters(&driver_counters);
}

KHRN_DRIVER_COUNTERS_T *khrn_driver_counters(void)
{
   return &driver_counters;
}
