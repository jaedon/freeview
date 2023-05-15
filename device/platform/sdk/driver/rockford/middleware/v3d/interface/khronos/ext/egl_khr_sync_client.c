/*=============================================================================
Copyright (c) 2008 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  Header file

FILE DESCRIPTION
Client-side implementation of the EGL_KHR_sync extension.
=============================================================================*/

#define EGL_EGLEXT_PROTOTYPES /* we want the prototypes so the compiler will check that the signatures match */

#include "interface/khronos/common/khrn_client_mangle.h"

#include "interface/khronos/ext/egl_khr_sync_client.h"
#include "interface/khronos/include/EGL/egl.h"
#include "interface/khronos/include/EGL/eglext.h"
#include "interface/khronos/common/khrn_int_misc_impl.h"

static EGL_SYNC_T *egl_sync_create(EGLSyncKHR name, EGLenum type, EGLint condition, EGLint threshold)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   EGL_SYNC_T *sync = (EGL_SYNC_T *)khrn_platform_malloc(sizeof(EGL_SYNC_T), "EGL_SYNC_T");
   uint64_t pid = khronos_platform_get_process_id();
   uint32_t token;

   if (!sync)
      return 0;

   sync->condition = condition;
   sync->threshold = threshold;
   sync->type = type;

   sync->sem[0] = (int)pid;
   sync->sem[1] = (int)(pid >> 32);
   sync->sem[2] = (int)name;

   if (khronos_platform_semaphore_create(&sync->master, sync->sem, 0) != KHR_SUCCESS) {
      khrn_platform_free(sync);
      return 0;
   }

   token = (uint32_t)name;
   {
      sync->serversync = RPC_UINT_RES(RPC_CALL4_RES(eglIntCreateSync_impl,
                                                 thread,
                                                 EGLINTCREATESYNC_ID,
                                                 RPC_UINT(type),
                                                 RPC_UINT(condition),
                                                 RPC_INT(threshold),
                                                 RPC_UINT(token)));
      if (!sync->serversync) {
         khronos_platform_semaphore_destroy(&sync->master);
         khrn_platform_free(sync);
         return 0;
      }
   }
   return sync;
}

/*
   void egl_sync_term(EGL_SYNC_T *sync)

   Implementation notes:

   -

   Preconditions:

   sync is a valid pointer

   Postconditions:

   -

   Invariants preserved:

   -

   Invariants used:

   -
 */

void egl_sync_term(EGL_SYNC_T *sync)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   {
      RPC_CALL1(eglIntDestroySync_impl,
                thread,
                EGLINTDESTROYSYNC_ID,
                RPC_UINT(sync->serversync));
   }
   khronos_platform_semaphore_destroy(&sync->master);
}

static EGLBoolean egl_sync_check_attribs(const EGLint *attrib_list, EGLenum type, EGLint *condition, EGLint *threshold)
{
   *condition = EGL_NONE;
   *threshold = 0;

   if (attrib_list) {
      while (1) {
         int name = *attrib_list++;
         if (name == EGL_NONE)
            break;
         else {
            /* int value = * */attrib_list++; /* at present no name/value pairs are handled */
            switch (name) {
            default:
               return EGL_FALSE;
            }
         }
      }
   }

   return ((type == EGL_SYNC_FENCE_KHR) || (type == 0));
}

EGLBoolean egl_sync_get_attrib(EGL_SYNC_T *sync, EGLint attrib, EGLint *value)
{
   switch (attrib) {
   case EGL_SYNC_TYPE_KHR:
      *value = sync->type;
      return EGL_TRUE;
   case EGL_SYNC_STATUS_KHR:
      *value = 0;
      return EGL_TRUE;
   case EGL_SYNC_CONDITION_KHR:
      *value = sync->condition;
      return EGL_TRUE;
   default:
      return EGL_FALSE;
   }
}

EGLAPI EGLSyncKHR EGLAPIENTRY eglCreateSyncKHR(EGLDisplay dpy, EGLenum type, const EGLint *attrib_list)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   EGLSyncKHR result = EGL_NO_SYNC_KHR;

   CLIENT_LOCK();

   {
      CLIENT_PROCESS_STATE_T *process = client_egl_get_process_state(thread, dpy, EGL_TRUE);

      EGLint condition;
      EGLint threshold;

      if (process)
      {
         if (egl_sync_check_attribs(attrib_list, type, &condition, &threshold)) {
            EGL_SYNC_T *sync = egl_sync_create((EGLSyncKHR)(size_t)process->next_sync, type, condition, threshold);

            if (sync) {
               if (khrn_pointer_map_insert(&process->syncs, process->next_sync, sync)) {
                  thread->error = EGL_SUCCESS;
                  result = (EGLSurface)(size_t)process->next_sync++;
               } else {
                  thread->error = EGL_BAD_ALLOC;
                  egl_sync_term(sync);
                  khrn_platform_free(sync);
               }
            } else
               thread->error = EGL_BAD_ALLOC;
         }
      }
   }

   CLIENT_UNLOCK();

   return result;
}

// TODO: should we make sure any syncs have come back before destroying the object?

EGLAPI EGLBoolean EGLAPIENTRY eglDestroySyncKHR(EGLDisplay dpy, EGLSyncKHR _sync)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   EGLBoolean result;

   CLIENT_LOCK();

   {
      CLIENT_PROCESS_STATE_T *process = client_egl_get_process_state(thread, dpy, EGL_TRUE);

      if (process) {
         EGL_SYNC_T *sync = (EGL_SYNC_T *)khrn_pointer_map_lookup(&process->syncs, (uint32_t)(size_t)_sync);

         if (sync) {
            thread->error = EGL_SUCCESS;

            khrn_pointer_map_delete(&process->syncs, (uint32_t)(uintptr_t)_sync);

            egl_sync_term(sync);
            khrn_platform_free(sync);
         } else
            thread->error = EGL_BAD_PARAMETER;

         result = (thread->error == EGL_SUCCESS ? EGL_TRUE : EGL_FALSE);
      } else {
         result = EGL_FALSE;
      }
   }

   CLIENT_UNLOCK();

   return result;
}

EGLAPI EGLint EGLAPIENTRY eglClientWaitSyncKHR(EGLDisplay dpy, EGLSyncKHR _sync, EGLint flags, EGLTimeKHR timeout)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();

   CLIENT_LOCK();

   {
      CLIENT_PROCESS_STATE_T *process = client_egl_get_process_state(thread, dpy, EGL_TRUE);

      if (process) {
         EGL_SYNC_T *sync = (EGL_SYNC_T *)khrn_pointer_map_lookup(&process->syncs, (uint32_t)(size_t)_sync);

         if (sync) {
            PLATFORM_SEMAPHORE_T semaphore;
            if( khronos_platform_semaphore_create(&semaphore, sync->sem, 1) == KHR_SUCCESS) {
               EGLint res;
               if (flags & EGL_SYNC_FLUSH_COMMANDS_BIT_KHR)
               {
                  /* needs to force a flush to the pipeline */
                  RPC_FLUSH(thread);

                  (void) RPC_INT_RES(RPC_CALL2_RES(eglIntFlushAndWait_impl,
                     thread,
                     EGLINTFLUSHANDWAIT_ID,
                     RPC_UINT(thread->bound_api == EGL_OPENGL_ES_API),
                     RPC_UINT(thread->bound_api == EGL_OPENVG_API)));
               }

               if (timeout != EGL_FOREVER_KHR) {
                  EGLTimeKHR ms_timeout = timeout / 1000000;
                  /* clamp the timeout to something reasonable - max 2147483 seconds */
                  if (ms_timeout >> 32)
                     ms_timeout = 0x7FFFFFFF;

                  if (khronos_platform_semaphore_acquire_timeout(&semaphore, (int)ms_timeout) == KHR_SUCCESS)
                     res = EGL_CONDITION_SATISFIED_KHR;
                  else
                     res = EGL_TIMEOUT_EXPIRED_KHR;

               } else  {
                  khronos_platform_semaphore_acquire(&semaphore);
                  res = EGL_CONDITION_SATISFIED_KHR;
               }

               if (res == EGL_CONDITION_SATISFIED_KHR) {
                  khronos_platform_semaphore_release(&semaphore);
               }
               khronos_platform_semaphore_destroy(&semaphore);

               CLIENT_UNLOCK();

               return res;
            } else
               thread->error = EGL_BAD_ALLOC;         // not strictly allowed by the spec, but indicates that we failed to create a reference to the named semaphore
         } else
            thread->error = EGL_BAD_PARAMETER;
      }
   }

   CLIENT_UNLOCK();

   return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglSignalSyncKHR(EGLDisplay dpy, EGLSyncKHR _sync, EGLenum mode)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();

   UNUSED(mode);

   CLIENT_LOCK();

   {
      CLIENT_PROCESS_STATE_T *process = client_egl_get_process_state(thread, dpy, EGL_TRUE);

      if (process) {
         EGL_SYNC_T *sync = (EGL_SYNC_T *)khrn_pointer_map_lookup(&process->syncs, (uint32_t)(size_t)_sync);

         if (sync)
            thread->error = EGL_BAD_MATCH;
         else
            thread->error = EGL_BAD_PARAMETER;
      }
   }

   CLIENT_UNLOCK();

   return EGL_FALSE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetSyncAttribKHR(EGLDisplay dpy, EGLSyncKHR _sync, EGLint attribute, EGLint *value)
{
   CLIENT_THREAD_STATE_T *thread = CLIENT_GET_THREAD_STATE();
   EGLBoolean result = EGL_FALSE;

   CLIENT_LOCK();

   {
      CLIENT_PROCESS_STATE_T *process = client_egl_get_process_state(thread, dpy, EGL_TRUE);

      if (process)
      {
         if (value)
         {
            EGL_SYNC_T *sync = (EGL_SYNC_T *)khrn_pointer_map_lookup(&process->syncs, (uint32_t)(size_t)_sync);

            if (sync) {
               if (egl_sync_get_attrib(sync, attribute, value)) {
                  thread->error = EGL_SUCCESS;
                  result = EGL_TRUE;
               } else
                  thread->error = EGL_BAD_ATTRIBUTE;
            } else
               thread->error = EGL_BAD_PARAMETER;
         }
         else
         {
            thread->error = EGL_BAD_PARAMETER;
         }
      }
   }

   CLIENT_UNLOCK();

   return result;
}
