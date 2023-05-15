/*=============================================================================
Copyright (c) 2009 Broadcom Europe Limited.
All rights reserved.

Project  :  vcfw
Module   :  chip driver
File     :  $RCSfile: $
Revision :  $Revision: #1 $

FILE DESCRIPTION
VideoCore OS Abstraction Layer - event flags implemented via a semaphore
=============================================================================*/

#ifndef VCOS_GENERIC_EVENT_FLAGS_H
#define VCOS_GENERIC_EVENT_FLAGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "interface/vcos/vcos_types.h"
#include "vcos_platform.h"

/**
  * \file
  *
  * This provides event flags (as per Nucleus Event Groups) based on a
  * mutex, a semaphore (per waiting thread) and a timer (per waiting
  * thread).
  * 
  * The data structure is a 32 bit unsigned int (the current set of
  * flags) and a linked list of clients waiting to be 'satisfied'.
  *
  * The mutex merely locks access to the data structure. If a client
  * calls vcos_event_flags_get() and the requested bits are not already
  * present, it then sleeps on its per-thread semaphore after adding
  * this semaphore to the queue waiting. It also sets up a timer.
  *
  * The per-thread semaphore and timer are actually stored in the
  * thread context (joinable thread). In future it may become necessary
  * to support non-VCOS threads by using thread local storage to
  * create these objects and associate them with the thread.
  */

struct VCOS_EVENT_WAITER_T;

typedef struct VCOS_EVENT_FLAGS_T
{
   VCOS_UNSIGNED events;      /**< Events currently set */
   VCOS_MUTEX_T lock;         /**< Serialize access */
   struct
   {
      struct VCOS_EVENT_WAITER_T *head;   /**< List of threads waiting */
      struct VCOS_EVENT_WAITER_T *tail;   /**< List of threads waiting */
   } waiters;
} VCOS_EVENT_FLAGS_T;

#define VCOS_OR      1
#define VCOS_AND     2
#define VCOS_CONSUME 4
#define VCOS_OR_CONSUME (VCOS_OR | VCOS_CONSUME)
#define VCOS_AND_CONSUME (VCOS_AND | VCOS_CONSUME)
#define VCOS_EVENT_FLAG_OP_MASK (VCOS_OR|VCOS_AND)

VCOSPRE_  VCOS_STATUS_T VCOSPOST_ vcos_generic_event_flags_create(VCOS_EVENT_FLAGS_T *flags, const char *name);
VCOSPRE_  void VCOSPOST_ vcos_generic_event_flags_set(VCOS_EVENT_FLAGS_T *flags,
                                                      VCOS_UNSIGNED events,
                                                      VCOS_OPTION op);
VCOSPRE_  void VCOSPOST_ vcos_generic_event_flags_delete(VCOS_EVENT_FLAGS_T *);
VCOSPRE_  VCOS_STATUS_T VCOSPOST_ vcos_generic_event_flags_get(VCOS_EVENT_FLAGS_T *flags,
                                                               VCOS_UNSIGNED requested_events,
                                                               VCOS_OPTION op,
                                                               VCOS_UNSIGNED suspend,
                                                               VCOS_UNSIGNED *retrieved_events);

#ifdef VCOS_INLINE_BODIES

VCOS_INLINE_IMPL
VCOS_STATUS_T vcos_event_flags_create(VCOS_EVENT_FLAGS_T *flags, const char *name) {
   return vcos_generic_event_flags_create(flags, name);
}

VCOS_INLINE_IMPL
void vcos_event_flags_set(VCOS_EVENT_FLAGS_T *flags,
                          VCOS_UNSIGNED events,
                          VCOS_OPTION op) {
   vcos_generic_event_flags_set(flags, events, op);
}

VCOS_INLINE_IMPL
void vcos_event_flags_delete(VCOS_EVENT_FLAGS_T *f) {
   vcos_generic_event_flags_delete(f);
}

VCOS_INLINE_IMPL
VCOS_STATUS_T vcos_event_flags_get(VCOS_EVENT_FLAGS_T *flags,
                                   VCOS_UNSIGNED requested_events,
                                   VCOS_OPTION op,
                                   VCOS_UNSIGNED suspend,
                                   VCOS_UNSIGNED *retrieved_events) {
   return vcos_generic_event_flags_get(flags, requested_events, op, suspend, retrieved_events);
}

#endif /* VCOS_INLINE_BODIES */

#ifdef __cplusplus
}
#endif
#endif
