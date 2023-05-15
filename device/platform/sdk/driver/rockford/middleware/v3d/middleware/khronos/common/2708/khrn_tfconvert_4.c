/*=============================================================================
Copyright (c) 2012 Broadcom Europe Limited.
All rights reserved.

Project  :  3D Tools
Module   :  Control list creation and submission for RSO to t-format using tile buffer

FILE DESCRIPTION
Builds a control list for RSO to t-format conversion using tile buffer
=============================================================================*/

#include "middleware/khronos/common/2708/khrn_tfconvert_4.h"
#include "interface/khronos/common/khrn_int_common.h"
#include "middleware/khronos/common/khrn_image.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"
#include "interface/khronos/common/khrn_api_interposer.h"

#define TLB_INVALID_FORMAT 3
#define NULL_LIST_SIZE     27

static uint32_t get_dest_format(KHRN_IMAGE_FORMAT_T format)
{
   switch (format & IMAGE_FORMAT_PIXEL_LAYOUT_MASK)
   {
   case IMAGE_FORMAT_8888 : return 1;
   case IMAGE_FORMAT_565  : return 2;
   default                : return TLB_INVALID_FORMAT;
   }
}

static uint32_t get_src_format(KHRN_IMAGE_FORMAT_T format)
{
   switch (format & IMAGE_FORMAT_PIXEL_LAYOUT_MASK)
   {
   case IMAGE_FORMAT_8888 : return 0;
   case IMAGE_FORMAT_565  : return 2;
   default                : return TLB_INVALID_FORMAT;
   }
}

static uint32_t get_type_flag(KHRN_IMAGE_FORMAT_T format)
{
   if (khrn_image_is_tformat(format))
      return 1;
   else if (khrn_image_is_lineartile(format))
      return 2;
   else // if (khrn_image_is_rso(format))
      return 0;
}

static bool kill_convert_with_null_render(CONVERT_CALLBACK_DATA_T *data)
{
   uint8_t *p = data->controlList;
   uint8_t *end = data->controlList + data->numCLBytes - 1;

   if (data->controlList == NULL || data->numCLBytes < NULL_LIST_SIZE)
      return false;

   ADD_BYTE(p, KHRN_HW_INSTR_STATE_TILE_RENDERING_MODE);
   ADD_WORD(p, 0);   /* frame */
   ADD_SHORT(p, KHRN_HW_TILE_WIDTH);
   ADD_SHORT(p, KHRN_HW_TILE_HEIGHT);
   ADD_BYTE(p, (uint8_t)(
      (0 << 0) |     /* disable ms mode */
      (0 << 1) |     /* disable 64-bit color mode */
      (1 << 2) |     /* 32-bit rso */
      (0 << 4)));    /* 1x decimate (ie none) */
   ADD_BYTE(p, 0);   /* unused */
   ADD_BYTE(p, KHRN_HW_INSTR_STATE_TILE_COORDS);
   ADD_BYTE(p, 0);
   ADD_BYTE(p, 0);
   ADD_BYTE(p, KHRN_HW_INSTR_STORE_GENERAL);
   ADD_BYTE(p, 0);      /* no buffer */
   ADD_BYTE(p, 0xe0);   /* don't clear any buffers */
   ADD_WORD(p, 1 << 3); /* eof */

   /* Jump to end of render list */
   ADD_BYTE(p, KHRN_HW_INSTR_BRANCH);
   ADD_WORD(p, khrn_hw_addr(end));

   /* Last instruction is a nop */
   ADD_BYTE(end, KHRN_HW_INSTR_NOP);

   return true;
}

// This function nulls out all tile conversions that are not needed because they aren't currently
// marked as dirty.
static bool skip_clean_tiles(CONVERT_CALLBACK_DATA_T *data, KHRN_IMAGE_T *dst)
{
   uint8_t *p = data->controlList;
   uint8_t *end = data->controlList + data->numCLBytes - 1;
   uint32_t cnt = 0;
   uint32_t xTiles, yTiles, i, j;
   uint32_t numDirty;

   if (data->controlList == NULL || data->numCLBytes < NULL_LIST_SIZE)
      return false;

   numDirty = egl_image_num_dirty_tiles(data->image);

   if (numDirty == 0)
      return kill_convert_with_null_render(data);

   xTiles = (dst->width  + 63) / 64;
   yTiles = (dst->height + 63) / 64;

   p = p + 11;
   for (j = 0; j != yTiles; ++j)
   {
      for (i = 0; i != xTiles; ++i)
      {
         /* We write the store for the previous tile here as we're not storing all tiles.
          * We can then add the final store with EOF after the loop */
         if (!egl_image_is_tile_dirty(data->image, i, j))
         {
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
            ADD_BYTE(p, KHRN_HW_INSTR_NOP);
         }
         else
         {
            cnt++;
            if (cnt == numDirty)
            {
               // Ensure the last valid tile uses an EOF store
               p = p + 10;
               ADD_BYTE(p, KHRN_HW_INSTR_STORE_SUBSAMPLE_EOF);
            }
            else
               p = p + 11;
         }
      }
   }

   return true;
}

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
#define LogLock()   EventLog(eEVENT_WAITING, eEVENT_TFCONVERT, eEVENT_RENDERER, (uint32_t)convData->image, 0);
#define LogLocked() EventLog(eEVENT_START,   eEVENT_TFCONVERT, eEVENT_RENDERER, (uint32_t)convData->image, 0);
#define LogUnlock() EventLog(eEVENT_END,     eEVENT_TFCONVERT, eEVENT_RENDERER, (uint32_t)convData->image, 0);
#else
#define LogLock()
#define LogLocked()
#define LogUnlock()
#endif

static void callback(KHRN_HW_CALLBACK_REASON_T reason, void *data, const uint32_t *specials)
{
   CONVERT_CALLBACK_DATA_T *convData = (CONVERT_CALLBACK_DATA_T *)data;

   if (reason == KHRN_HW_CALLBACK_REASON_RENDER_SUBMIT && convData && convData->image)
   {
      KHRN_IMAGE_T   *dst    = (KHRN_IMAGE_T *)mem_lock(convData->image->mh_image);
      uint8_t        *dstPtr = mem_lock(dst->mh_storage);

      convData->conversionKilled = false;
      convData->semaphoreTaken = false;

      vcos_mutex_lock(&convData->image->dirtyBitsMutex);
      
      if (!convData->image->forceConversion && !egl_image_any_tile_dirty(convData->image))
      {
         // No dirty tiles any more, so kill the conversion
         LogLocked();
         LogUnlock();
         convData->conversionKilled = true;
         kill_convert_with_null_render(convData);

         vcos_mutex_unlock(&convData->image->dirtyBitsMutex);
      }
      else
      {
         // Time to lock the EGL image
         LogLock();

         // Try to take the lock
         if (vcos_semaphore_wait_timeout(&convData->image->lockSemaphore, 0) == VCOS_SUCCESS)
         {
            // We got the lock
            convData->semaphoreTaken = true;
            LogLocked();

            // Adjust the control list for the current state of the dirty tiles
            skip_clean_tiles(convData, dst);
            egl_image_clr_all_tile_dirty_bits(convData->image);
            convData->image->forceConversion = false;
         }
         else
         {
            // The application has the image locked right now.
            // Record the fact that we missed a conversion that we actually wanted.
            // We use tryLock since we may already have the flag locked from a previous conversion attempt.
            // We don't care if it fails due to already being locked, we just want to ensure it is locked.
            vcos_semaphore_wait_timeout(&convData->image->missedConvFlag, 0);

            // Kill the conversion
            LogLocked();
            LogUnlock();
            convData->conversionKilled = true;
            kill_convert_with_null_render(convData);
         }
      }

      vcos_mutex_unlock(&convData->image->dirtyBitsMutex);

      mem_unlock(dst->mh_storage);
      mem_unlock(convData->image->mh_image);
   }
   else if (reason == KHRN_HW_CALLBACK_REASON_RENDER_FINISHED_ISR)
   {
      // We're done with the image, so unlock it
      if (!convData->conversionKilled)
      {
         LogUnlock();

         // Release our lock
         if (convData->semaphoreTaken)
         {
            vcos_semaphore_wait_timeout(&convData->image->lockSemaphore, 0);
            vcos_semaphore_post(&convData->image->lockSemaphore);

            // Clear the 'missed' flag mutex, since we've now done the conversion
            vcos_semaphore_wait_timeout(&convData->image->missedConvFlag, 0);
            vcos_semaphore_post(&convData->image->missedConvFlag);
         }

         INCR_DRIVER_COUNTER(hw_tf_conversions);
      }

      vcos_mutex_lock(&convData->image->dirtyBitsMutex);
      convData->image->completed++;
      vcos_mutex_unlock(&convData->image->dirtyBitsMutex);
   }
}

bool khrn_rso_to_tf_convert(EGL_IMAGE_T *eglImage)
{
   KHRN_FMEM_T             *fmem;
   void                    *data;
   uint8_t                 *p, *listPtr;
   uint32_t                i, j, b;
   bool                    store;
   KHRN_IMAGE_T            *src;
   KHRN_IMAGE_T            *dst;
   void                    *srcPtr;
   void                    *dstPtr;
   uint32_t                xTiles;
   uint32_t                yTiles;
   uint32_t                numDirtyTiles;
   uint32_t                numBytes;
   KHRN_HW_CALLBACK_T      data_callback;
   uint32_t                data_callback_size;
   CONVERT_CALLBACK_DATA_T *convData;

   if (eglImage->mh_shadow_image == MEM_INVALID_HANDLE || eglImage->mh_image == MEM_INVALID_HANDLE)
      return false;

   src = (KHRN_IMAGE_T *)mem_lock(eglImage->mh_shadow_image);
   dst = (KHRN_IMAGE_T *)mem_lock(eglImage->mh_image);

   srcPtr = mem_lock(src->mh_storage);
   dstPtr = mem_lock(dst->mh_storage);

   xTiles = (src->width  + 63) / 64;
   yTiles = (src->height + 63) / 64;

   vcos_assert(src->width == dst->width && src->height == dst->height);

   vcos_mutex_lock(&eglImage->dirtyBitsMutex);

   if (eglImage->queued > eglImage->completed)
   {
      vcos_mutex_unlock(&eglImage->dirtyBitsMutex);
      return true; // We have a pending conversion already
   }

   if (!eglImage->forceConversion && !egl_image_any_tile_dirty(eglImage))
   {
      vcos_mutex_unlock(&eglImage->dirtyBitsMutex);
      return true;
   }

   fmem = khrn_fmem_init(KHRN_INTERLOCK_USER_TEMP);
   if (!fmem)
   {
      vcos_mutex_unlock(&eglImage->dirtyBitsMutex);
      return false;
   }

   if (!khrn_fmem_start_render(fmem))
   {
      khrn_fmem_discard(fmem);
      vcos_mutex_unlock(&eglImage->dirtyBitsMutex);
      return false;
   }

   eglImage->queued++;

   vcos_mutex_unlock(&eglImage->dirtyBitsMutex);

   numDirtyTiles = xTiles * yTiles;

   numBytes = 11 + (numDirtyTiles * 11);
   if (numBytes < NULL_LIST_SIZE)
      numBytes = NULL_LIST_SIZE;    /* Must have at least this many bytes in case we need a NULL list */

   listPtr = p = khrn_fmem_cle(fmem, numBytes);
   if (p == NULL)
   {
      mem_unlock(src->mh_storage);
      mem_unlock(dst->mh_storage);
      mem_unlock(eglImage->mh_image);
      mem_unlock(eglImage->mh_shadow_image);
   }

   /* set rendering mode config */
   ADD_BYTE(p, KHRN_HW_INSTR_STATE_TILE_RENDERING_MODE);
   ADD_WORD(p, khrn_hw_addr(dstPtr));

   ADD_SHORT(p, src->width);
   ADD_SHORT(p, src->height);
   ADD_BYTE(p,
      (0 << 0) | /* disable ms mode */
      (0 << 1) | /* disable 64-bit color mode */
      (get_dest_format(dst->format) << 2) | 
      (0 << 4) | /* No decimation */
      (get_type_flag(dst->format) << 6));

   ADD_BYTE(p,
      (0 << 0) | /* no vg mask */
      (0 << 1) | /* no coverage mode */
      (0 << 2) | /* don't care z-update dir */
      (0 << 3) | /* disable early Z */
      (0 << 4)); /* no double buffer */

   store = false;
   for (j = 0; j != yTiles; ++j)
   {
      for (i = 0; i != xTiles; ++i) 
      {
         /* We write the store for the previous tile here as we're not storing all tiles.
          * We can then add the final store with EOF after the loop */
            if (store)
               ADD_BYTE(p, KHRN_HW_INSTR_STORE_SUBSAMPLE);

            ADD_BYTE(p, KHRN_HW_INSTR_LOAD_GENERAL);
            ADD_BYTE(p, (uint8_t)(
               (1 << 0) | /* load color */
               (get_type_flag(src->format) << 4)));

            ADD_BYTE(p, get_src_format(src->format));
            ADD_WORD(p, khrn_hw_addr(srcPtr));

            ADD_BYTE(p, KHRN_HW_INSTR_STATE_TILE_COORDS);
            ADD_BYTE(p, (uint8_t)i);
            ADD_BYTE(p, (uint8_t)j);

            store = true;
         }
      }

   /* Add the final store with EOF */
   ADD_BYTE(p, KHRN_HW_INSTR_STORE_SUBSAMPLE_EOF);

   /* Pad end with NOP's if necessary */
   for (b = 11 + (numDirtyTiles * 11); b < numBytes; b++)
      ADD_BYTE(p, KHRN_HW_INSTR_NOP);

   mem_unlock(src->mh_storage);
   mem_unlock(dst->mh_storage);
   mem_unlock(eglImage->mh_image);
   mem_unlock(eglImage->mh_shadow_image);

   khrn_hw_advance_enter_pos();

   /* We might need to lock/unlock or kill conversions so setup a callback */
   data_callback = callback;
   data_callback_size = sizeof(CONVERT_CALLBACK_DATA_T);

   data = khrn_fmem_queue(fmem,
      KHRN_HW_CC_FLAG_NONE,
      (KHRN_HW_CC_FLAG_T)(KHRN_HW_CC_FLAG_L2 | KHRN_HW_CC_FLAG_IC | KHRN_HW_CC_FLAG_UC | KHRN_HW_CC_FLAG_TU),
      1, /* frame count */
      0, /* no special stuff */
      0, /* bin mem size */
      0, /* actual VPM ? */
      KHRN_HW_VPM_BLOCKS_N, /* main hw doesn't need any vpm  */
      KHRN_HW_TYPE_VG,
      data_callback, data_callback_size);

   convData = (CONVERT_CALLBACK_DATA_T*)data;
   convData->image = eglImage;
   convData->controlList = listPtr;
   convData->conversionKilled = false;
   convData->semaphoreTaken = false;
   convData->numCLBytes = numBytes;

   khrn_hw_ready(true, data);

   return true;
}


