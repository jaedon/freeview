/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Worker thread

FILE DESCRIPTION
For doing heavy lifting on the CPU in parallel to the rest of the system.
=============================================================================*/

#ifndef KHRN_WORKER_4_H
#define KHRN_WORKER_4_H

#include "interface/khronos/common/khrn_int_util.h"

extern bool khrn_worker_init(void);
extern void khrn_worker_term(void);

typedef enum {
   KHRN_WORKER_CALLBACK_RESULT_DO_IT_WAIT,
   KHRN_WORKER_CALLBACK_RESULT_DO_IT_OK,
   KHRN_WORKER_CALLBACK_RESULT_DO_IT_OK_ADVANCE_EXIT_POS,
   KHRN_WORKER_CALLBACK_RESULT_CLEANUP
} KHRN_WORKER_CALLBACK_RESULT_T;

typedef enum {
   KHRN_WORKER_CALLBACK_REASON_DO_IT,
   KHRN_WORKER_CALLBACK_REASON_CLEANUP
} KHRN_WORKER_CALLBACK_REASON_T;

typedef KHRN_WORKER_CALLBACK_RESULT_T (*KHRN_WORKER_CALLBACK_T)(
   KHRN_WORKER_CALLBACK_REASON_T reason,
   void *message, uint32_t size);

extern void *khrn_worker_post_begin(KHRN_WORKER_CALLBACK_T callback, uint32_t size); /* message will be 4-byte aligned */
extern void khrn_worker_post_end(void);

extern void khrn_worker_cleanup(void); /* called from khrn_sync_master_wait */

extern void khrn_worker_wait(void);

extern void khrn_worker_notify(void);

/*
   fifo pos stuff for interlocking with other fifos (eg hw)

   khrn_worker_get_enter_pos should only be called from the master task and
   should always return the right value

   khrn_worker_get_exit_pos can be called from any task, but might return the
   wrong value. the value will always be <= the right value however
*/

extern uint64_t khrn_worker_enter_pos;

extern VCOS_MUTEX_T worker_protector;

static INLINE void khrn_worker_advance_enter_pos(void)
{
   khrn_worker_enter_pos++;
}

static INLINE uint64_t khrn_worker_get_enter_pos(void)
{
   return khrn_worker_enter_pos;
}

extern uint64_t khrn_worker_exit_pos;

static INLINE void khrn_worker_advance_exit_pos(void)
{
   vcos_mutex_lock(&worker_protector);
   khrn_worker_exit_pos++;
   vcos_mutex_unlock(&worker_protector);
}

static INLINE uint64_t khrn_worker_get_exit_pos(void)
{
   uint64_t r;
   vcos_mutex_lock(&worker_protector);
   r = khrn_worker_exit_pos;
   vcos_mutex_unlock(&worker_protector);
   return r;
}

#endif
