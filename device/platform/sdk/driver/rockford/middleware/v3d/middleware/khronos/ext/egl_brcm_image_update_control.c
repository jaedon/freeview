/*=============================================================================
Copyright (c) 2012 Broadcom Europe Limited.
All rights reserved.

Project  :  khronos
Module   :  EGL Broadcom image update control extension

FILE DESCRIPTION
Server-side implementation of EGL_BRCM_image_update_control.
=============================================================================*/

#include "interface/khronos/common/khrn_int_common.h"
#include "middleware/khronos/egl/egl_platform.h"
#include "middleware/khronos/egl/egl_server.h"
#include "middleware/khronos/ext/egl_khr_image.h"
#include "interface/khronos/common/khrn_api_interposer.h"
#include "middleware/khronos/common/2708/khrn_tfconvert_4.h"

#ifdef EGL_BRCM_image_update_control

#define PIXELS_PER_TILE 64

#ifdef TIMELINE_EVENT_LOGGING
static void EventLog(uint32_t t, uint32_t c, uint32_t r, uint32_t d, char *desc)
{
   EventData ev;
   ev.eventType = t;
   ev.eventCode = c;
   ev.eventRow  = r;
   ev.eventData = d;
   ev.desc = desc;
   khrn_remote_event_log(&ev);
}
#define LogLock()   EventLog(eEVENT_WAITING, eEVENT_EGLIMAGE_APP_LOCK, khrn_event_cpu_row(), (uint32_t)egl_image, 0);
#define LogLocked() EventLog(eEVENT_START,   eEVENT_EGLIMAGE_APP_LOCK, khrn_event_cpu_row(), (uint32_t)egl_image, 0);
#define LogUnlock() EventLog(eEVENT_END,     eEVENT_EGLIMAGE_APP_LOCK, khrn_event_cpu_row(), (uint32_t)egl_image, 0);
#else
#define LogLock()
#define LogLocked()
#define LogUnlock()
#endif

typedef struct
{
   MEM_HANDLE_T heglimage;
   EGL_IMAGE_T  *egl_image;
} Persist;

// This function is called with an API lock in place
void eglImageUpdateParameterBRCM_lockPhase1(EGLImageKHR image, void *serverData)
{
   EGL_SERVER_STATE_T *state = EGL_GET_SERVER_STATE();
   Persist            *persist = (Persist*)serverData;

   vcos_assert(serverData != NULL);

   persist->heglimage = khrn_map_lookup(&state->eglimages, (uint32_t)image);
   if (persist->heglimage)
   {
      EGL_IMAGE_T *egl_image = (EGL_IMAGE_T *)mem_lock(persist->heglimage);
      persist->egl_image = egl_image;
      if (persist->egl_image == NULL)
         return;

      LogLock();

      // Note: DON'T UNLOCK the eglimage here - we want it to remain locked until eglImageUpdateParameterBRCM_lockPhase4 is called.
      //mem_unlock(persist->heglimage);
   }
}

// NOTE : This function is called WITHOUT an API lock in place.
// Take extreme care!!
bool eglImageUpdateParameterBRCM_lockPhase2(EGLImageKHR image, void *serverData)
{
   EGL_SERVER_STATE_T *state = EGL_GET_SERVER_STATE();
   Persist            *persist = (Persist*)serverData;

   vcos_assert(serverData != NULL);

   if (persist->heglimage)
   {
      EGL_IMAGE_T *egl_image = persist->egl_image;

      if (egl_image == NULL)
         return true;

      // First wait for the 'missed' flag to be cleared following a successful conversion
      // It might already be clear of course.
      // (Timeout after 500ms to handle end of render conditions).
      if (vcos_semaphore_wait_timeout(&egl_image->missedConvFlag, 500) == VCOS_SUCCESS)
         vcos_semaphore_post(&egl_image->missedConvFlag);
      else
         return false;

      return true;
   }

   return true;
}

// This function is called with an API lock in place
void eglImageUpdateParameterBRCM_lockPhase3(EGLImageKHR image, bool phase2Ret, void *serverData)
{
   EGL_SERVER_STATE_T *state = EGL_GET_SERVER_STATE();
   Persist            *persist = (Persist*)serverData;

   vcos_assert(serverData != NULL);

   if (persist->heglimage)
   {
      EGL_IMAGE_T *egl_image = persist->egl_image;

      if (egl_image == NULL)
         return;

      if (!phase2Ret)
      {
         // Timed out waiting for the 'missed' flag to lock - it's highly likely that the app has stopped rendering 3d stuff.
         // So, we'll post the conversion again, and wait once more for completion. If we don't we could stall the 2d thread
         // indefinitely.
         khrn_rso_to_tf_convert(egl_image);
      }
   }
}

// NOTE : This function is called WITHOUT an API lock in place.
// Take extreme care!!
void eglImageUpdateParameterBRCM_lockPhase4(EGLImageKHR image, bool phase2Ret, void *serverData)
{
   EGL_SERVER_STATE_T *state = EGL_GET_SERVER_STATE();
   Persist            *persist = (Persist*)serverData;

   vcos_assert(serverData != NULL);

   if (persist->heglimage)
   {
      EGL_IMAGE_T *egl_image = persist->egl_image;

      if (egl_image == NULL)
         return;

      if (!phase2Ret)
      {
         if (vcos_semaphore_wait_timeout(&egl_image->missedConvFlag, 500) == VCOS_SUCCESS)
         {
            // We got the lock, so the conversion completed successfully. Clear the lock again.
            vcos_semaphore_post(&egl_image->missedConvFlag);
         }
         else
         {
            // We timed out again - so something weird must have happened. Let's pretend the 2d app got the image lock so
            // it doesn't stall. Nothing to do here.
         }
      }

      // Now lock the EGL image
      while (vcos_semaphore_wait(&egl_image->lockSemaphore) != VCOS_SUCCESS)
         continue;

      LogLocked();
      
      // Force the image into a conversion now to avoid 'every-other-frame-syndrome'
      vcos_mutex_lock(&egl_image->dirtyBitsMutex);
      egl_image->forceConversion = true;
      vcos_mutex_unlock(&egl_image->dirtyBitsMutex);

      mem_unlock(persist->heglimage);
   }
}

bool eglImageUpdateParameterBRCM_impl(EGLImageKHR image, EGLenum pname, const EGLint *params)
{
   EGL_SERVER_STATE_T *state = EGL_GET_SERVER_STATE();
   
   uint32_t id = (uint32_t)image;
   MEM_HANDLE_T heglimage;

   uint32_t maxTiles = sizeof(tile_bits_t) * 8; /* 32 for 2048 limit, 64 for 4096 limit */

#ifndef V3D_LEAN
   vcos_log("KHRN eglImageUpdateParameterBRCM_impl %d", image);
#endif

   heglimage = khrn_map_lookup(&state->eglimages, (uint32_t)id);
   if (heglimage)
   {
      EGL_IMAGE_T  *egl_image = (EGL_IMAGE_T *)mem_lock(heglimage);

      if (egl_image == NULL)
         return false;

      if (pname == EGL_IMAGE_UPDATE_CONTROL_SET_MODE_BRCM)
      {
         if (params[0] == EGL_IMAGE_UPDATE_CONTROL_ALWAYS_BRCM)
            egl_image->explicit_updates = false;
         else if (params[0] == EGL_IMAGE_UPDATE_CONTROL_EXPLICIT_BRCM)
            egl_image->explicit_updates = true;
      }
      else if (pname == EGL_IMAGE_UPDATE_CONTROL_SET_LOCK_STATE_BRCM)
      {
         if (params[0] == EGL_IMAGE_UPDATE_CONTROL_LOCK_BRCM)
         {
            // Should go through the separate lock interface
            UNREACHABLE();
         }
         else
         {
            // Unlock EGL image

            // See if we have a missed conversion to deal with
            bool missed = (vcos_semaphore_wait_timeout(&egl_image->missedConvFlag, 0) != VCOS_SUCCESS);
            if (!missed)
               vcos_semaphore_post(&egl_image->missedConvFlag); // Undo our tryLock that succeeded

            // If the conversion is flagged as 'missed' we need to repost the conversion to ensure that it happens at some point.
            if (missed)
               khrn_rso_to_tf_convert(egl_image);

            LogUnlock();
            vcos_semaphore_wait_timeout(&egl_image->lockSemaphore, 0);
            vcos_semaphore_post(&egl_image->lockSemaphore);
         }
      }
      else if (pname == EGL_IMAGE_UPDATE_CONTROL_CHANGED_REGION_BRCM)
      {
         uint32_t    tx[2], ty[2];
         uint32_t    y;
         tile_bits_t rowMask = 0, startMask = 0, endMask = 0;

         if (params[2] > 0 && params[3] > 0) // Don't process empty regions
         {
            /* Determine start tiles */
            tx[0] = params[0] / PIXELS_PER_TILE;
            ty[0] = params[1] / PIXELS_PER_TILE;

            /* Determine end tiles (using x + w - 1, y + h - 1) */
            tx[1] = (params[0] + params[2] - 1) / PIXELS_PER_TILE;
            ty[1] = (params[1] + params[3] - 1) / PIXELS_PER_TILE;

            startMask = ~((1 << tx[0]) - 1);
            if (tx[0] >= maxTiles)  // Can't shift 32 (or 64 for uint64), so check
               startMask = 0;

            endMask = (1 << (tx[1] + 1)) - 1;
            if (tx[1] + 1 >= maxTiles)  // Can't shift 32 (or 64 for uint64), so check
               endMask = ~0;

            rowMask = startMask & endMask;

            if (rowMask != 0 && ty[0] < maxTiles)
            {
               if (ty[1] >= maxTiles)
                  ty[1] = maxTiles - 1;

               vcos_mutex_lock(&egl_image->dirtyBitsMutex);
               for (y = ty[0]; y <= ty[1]; y++)
                  egl_image->dirtyBits.m_rowBits[y] |= rowMask;

               vcos_mutex_unlock(&egl_image->dirtyBitsMutex);
            }
         }
      }
      mem_unlock(heglimage);
   }
   else
      return false;

   return true;
}



#endif